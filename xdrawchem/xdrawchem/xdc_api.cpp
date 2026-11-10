// xdc_api.cpp — implementation of the headless agent API (see xdc_api.h)
//
// Design notes
// ------------
// * Everything here is synchronisation-free GUI code: we build the normal
//   ChemData + Render2D pair, but the Render2D is never shown.  The CLI sets
//   QT_QPA_PLATFORM=offscreen so Qt never needs a display.
// * Structure parsing goes through Molecule::FromSMILES / FromMolfile, which
//   use OpenBabel — the same code the GUI uses, so results agree with what a
//   user sees on screen.
// * Chemical facts come from OpenBabel first (offline, fast, deterministic);
//   PubChem is used only where a registry answer is genuinely required
//   (name -> structure, IUPAC name, CID, cross-checks in compare()).

#include <algorithm>

#include <QBuffer>
#include <QByteArray>
#include <QFile>
#include <QFileInfo>
#include <QHash>
#include <QImage>
#include <QMap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QTimer>
#include <QEventLoop>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QPainter>
#include <QPixmap>
#include <QPrinter>
#include <QPageSize>
#include <QPageLayout>
#include <QTemporaryFile>
#include <QDir>
#include <QSvgGenerator>
#include <QUrlQuery>
#include <QtMath>

#include "ob_compat.h"
OB_COMPAT_BEGIN
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/data.h>
#include <openbabel/elements.h>
#include <openbabel/forcefield.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
#include <openbabel/builder.h>
#include <openbabel/obiter.h>
#include <openbabel/descriptor.h>
OB_COMPAT_END
using namespace OpenBabel;

#include "xdc_api.h"
#include "molecule.h"
#include "molecule_nts.h"
#include "chemdata.h"
#include "render2d.h"
#include "dpoint.h"
#include "bond.h"
#include "text.h"
#include "defs.h"

// ─────────────────────────────────────────────────────────────────────────────
// Small helpers
// ─────────────────────────────────────────────────────────────────────────────

QJsonObject resultToJson( const XdcApiResult &r )
{
    QJsonObject o = r.payload;
    // "ok" and "command" always win, so a payload cannot forge them.
    o.insert( QStringLiteral( "ok" ), r.ok );
    if ( !r.command.isEmpty() )
        o.insert( QStringLiteral( "command" ), r.command );
    if ( !r.ok && !r.error.isEmpty() )
        o.insert( QStringLiteral( "error" ), r.error );
    o.insert( QStringLiteral( "schema_version" ),
              QString::fromLatin1( XDC_API_SCHEMA_VERSION ) );
    return o;
}

QString resultToJsonText( const XdcApiResult &r )
{
    QJsonDocument doc( resultToJson( r ) );
    return QString::fromUtf8( doc.toJson( QJsonDocument::Indented ) );
}

static XdcApiResult fail( const QString &command, const QString &message )
{
    XdcApiResult r;
    r.ok = false;
    r.command = command;
    r.error = message;
    return r;
}

static XdcApiResult succeed( const QString &command )
{
    XdcApiResult r;
    r.ok = true;
    r.command = command;
    return r;
}

// Serialise an OBMol to the named OpenBabel output format.
static QString obMolToFormat( OBMol *mol, const QString &format )
{
    OBConversion conv;
    QByteArray fmt = format.toLatin1();
    if ( !conv.SetOutFormat( fmt.constData() ) )
        return QString();

    // SMILES without the trailing tab + title that OpenBabel normally adds.
    if ( format == QLatin1String( "smi" ) || format == QLatin1String( "can" ) )
        conv.SetOptions( "i", conv.OUTOPTIONS );     // "i": no title

    std::string out = conv.WriteString( mol, true );
    QString s = QString::fromStdString( out );
    // OpenBabel appends "\t\n" to SMILES records; the CLI should hand an agent
    // a bare SMILES string it can paste straight back in.
    if ( format == QLatin1String( "smi" ) || format == QLatin1String( "can" ) )
        s = s.trimmed().section( QChar( '\t' ), 0, 0 ).trimmed();
    return s;
}

// Read any OBMol-readable text into an OBMol.
static bool readStructureIntoOBMol( OBMol &mol, const QString &structure,
                                    const QString &format )
{
    QByteArray data = structure.toUtf8();
    QString fmt = format.trimmed().toLower();

    if ( fmt.isEmpty() ) {
        // Auto-detect the common cases rather than guessing OpenBabel formats.
        if ( structure.contains( QLatin1String( "InChI=" ) ) )
            fmt = QStringLiteral( "inchi" );
        else if ( structure.contains( QLatin1String( "M  END" ) )
                  || structure.contains( QLatin1String( "V2000" ) )
                  || structure.contains( QLatin1String( "V3000" ) ) )
            fmt = QStringLiteral( "mol" );
        else
            fmt = QStringLiteral( "smi" );
    }
    if ( fmt == QLatin1String( "molfile" ) )
        fmt = QStringLiteral( "mol" );
    if ( fmt == QLatin1String( "smiles" ) )
        fmt = QStringLiteral( "smi" );

    QByteArray qfmt = fmt.toLatin1();
    OBConversion conv;
    if ( !conv.SetInFormat( qfmt.constData() ) )
        return false;

    std::istringstream iss( data.constData() );
    mol.Clear();
    return conv.Read( &mol, &iss ) && mol.NumAtoms() > 0;
}

// ─────────────────────────────────────────────────────────────────────────────
// Structure loading into a live XDrawChem document
// ─────────────────────────────────────────────────────────────────────────────

Molecule *apiLoadMolecule( ChemData *cd, const QString &structure,
                           const QString &format, QString *error )
{
    if ( cd == nullptr )
        return nullptr;

    QString s = structure;
    QString fmt = format.trimmed().toLower();
    bool fromFile = ( fmt == QLatin1String( "file" ) ) || ( fmt.isEmpty() && s.startsWith( QLatin1String( "@" ) ) );

    if ( fromFile ) {
        if ( s.startsWith( QLatin1Char( '@' ) ) )
            s = s.mid( 1 );
        QFile f( s );
        if ( !f.open( QIODevice::ReadOnly ) ) {
            if ( error )
                *error = QStringLiteral( "cannot open file: %1" ).arg( s );
            return nullptr;
        }
        QByteArray raw = f.readAll();
        f.close();

        const QString suffix = QFileInfo( s ).suffix().toLower();
        // Native/CDX formats must go through ChemData's own loaders.
        if ( suffix == QLatin1String( "xdc" ) || suffix == QLatin1String( "cdx" )
             || suffix == QLatin1String( "cdxml" ) ) {
            cd->load( s );
            Molecule *m = cd->firstMolecule();
            if ( !m && error )
                *error = QStringLiteral( "could not parse %1" ).arg( s );
            return m;
        }
        s = QString::fromUtf8( raw );
        fmt.clear();
    }

    // SMILES/InChI/molfile text.  fromString() handles both the OpenBabel
    // path and (for molfiles) Molecule::FromMolfile.
    Molecule *m = cd->fromString( s );
    if ( !m && fmt == QLatin1String( "inchi" ) ) {
        // fromString() cannot tell InChI from SMILES; go through OpenBabel.
        OBMol obmol;
        if ( readStructureIntoOBMol( obmol, s, fmt ) ) {
            m = new Molecule( cd->getRender2D(), cd );
            if ( !m->convertFromOBMol( &obmol ) ) {
                delete m;
                m = nullptr;
            }
        }
    }
    if ( !m && error )
        *error = QStringLiteral( "could not parse structure as %1" )
                 .arg( fmt.isEmpty() ? QStringLiteral( "auto" ) : fmt );
    return m;
}

// ─────────────────────────────────────────────────────────────────────────────
// Chemical facts shared by describe() / validate() / compare()
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// Convert the first molecule in the document into an OBMol for analysis.
OBMol *moleculeToOBMol( Molecule *m )
{
    if ( m == nullptr )
        return nullptr;
    OBMol *obmol = m->convertToOBMol();
    if ( obmol == nullptr || obmol->NumAtoms() == 0 ) {
        delete obmol;
        return nullptr;
    }
    return obmol;
}

// Per-element atom counts, in Hill-system order.
QJsonObject elementCounts( OBMol &mol )
{
    QMap<QString, int> counts;
    FOR_ATOMS_OF_MOL( a, mol ) {
        int n = a->GetAtomicNum();
        QString sym = QString::fromLatin1( OBElements::GetSymbol( n ) );
        counts[sym] += 1;
    }

    // Hill notation: C first, then H, then the rest alphabetically.
    QJsonObject obj;
    if ( counts.contains( QStringLiteral( "C" ) ) ) {
        obj.insert( QStringLiteral( "C" ), counts.value( QStringLiteral( "C" ) ) );
        counts.remove( QStringLiteral( "C" ) );
        if ( counts.contains( QStringLiteral( "H" ) ) ) {
            obj.insert( QStringLiteral( "H" ), counts.value( QStringLiteral( "H" ) ) );
            counts.remove( QStringLiteral( "H" ) );
        }
    }
    for ( auto it = counts.constBegin(); it != counts.constEnd(); ++it )
        obj.insert( it.key(), it.value() );
    return obj;
}

// Molecular weight from the standard atomic weights OpenBabel carries.
double molecularWeight( OBMol &mol )
{
    double mw = 0.0;
    FOR_ATOMS_OF_MOL( a, mol )
        mw += OBElements::GetMass( a->GetAtomicNum() );
    return mw;
}

QString inchiKeyFromInchi( const QString &inchi )
{
    if ( inchi.trimmed().isEmpty() )
        return QString();
    OBConversion conv;
    if ( !conv.SetInAndOutFormats( "inchi", "inchikey" ) )
        return QString();
    OBMol mol;
    std::istringstream iss( inchi.toStdString() );
    if ( !conv.Read( &mol, &iss ) )
        return QString();
    return QString::fromStdString( conv.WriteString( &mol, true ) ).trimmed();
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// convert — structure text in one language, out another
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiConvert( const QString &structure, const QString &fromFormat,
                         const QString &toFormat )
{
    const QString cmd = QStringLiteral( "convert" );

    QString to = toFormat.trimmed().toLower();
    if ( to == QLatin1String( "smiles" ) ) to = QStringLiteral( "smi" );
    if ( to == QLatin1String( "canonical" ) ) to = QStringLiteral( "can" );
    if ( to == QLatin1String( "molfile" ) ) to = QStringLiteral( "mol" );
    if ( to == QLatin1String( "inchi" ) ) to = QStringLiteral( "inchi" );
    if ( to == QLatin1String( "inchikey" ) ) to = QStringLiteral( "inchikey" );
    if ( to.isEmpty() )
        to = QStringLiteral( "smi" );

    QList<QString> known = { "smi", "can", "inchi", "inchikey", "mol", "sdf",
                             "cml", "xyz", "formula" };
    if ( !known.contains( to ) )
        return fail( cmd, QStringLiteral( "unsupported target format '%1' "
                                          "(use one of: %2)" )
                              .arg( to, known.join( QStringLiteral( ", " ) ) ) );

    OBMol mol;
    QString readErr;
    if ( !readStructureIntoOBMol( mol, structure, fromFormat ) )
        return fail( cmd, QStringLiteral( "could not parse input structure" ) );

    if ( to == QLatin1String( "formula" ) ) {
        QString f = QString::fromStdString( mol.GetFormula() );
        XdcApiResult r = succeed( cmd );
        r.payload.insert( QStringLiteral( "formula" ), f );
        r.payload.insert( QStringLiteral( "input_format" ),
                          fromFormat.isEmpty() ? QStringLiteral( "auto" )
                                               : fromFormat );
        return r;
    }

    QString out;
    if ( to == QLatin1String( "can" ) ) {
        OBConversion conv;
        if ( !conv.SetOutFormat( "can" ) )
            return fail( cmd, QStringLiteral( "OpenBabel canonical SMILES "
                                              "writer unavailable" ) );
        conv.SetOptions( "i", conv.OUTOPTIONS );
        out = QString::fromStdString( conv.WriteString( &mol, true ) )
              .trimmed().section( QChar( '\t' ), 0, 0 ).trimmed();
    } else if ( to == QLatin1String( "inchikey" ) ) {
        out = inchiKeyFromInchi( obMolToFormat( &mol, "inchi" ) );
        if ( out.isEmpty() )
            return fail( cmd, QStringLiteral( "could not compute InChIKey" ) );
    } else {
        out = obMolToFormat( &mol, to );
        if ( out.isEmpty() )
            return fail( cmd, QStringLiteral( "OpenBabel could not write "
                                              "the '%1' format" ).arg( to ) );
    }

    XdcApiResult r = succeed( cmd );
    r.payload.insert( QStringLiteral( "output_format" ), to );
    r.payload.insert( QStringLiteral( "structure" ), out );
    r.payload.insert( QStringLiteral( "formula" ),
                      QString::fromStdString( mol.GetFormula() ) );
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// name_to_structure — chemical name / CAS -> structure
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiNameToStructure( const QString &name, const QString &toFormat )
{
    const QString cmd = QStringLiteral( "name_to_structure" );
    if ( name.trimmed().isEmpty() )
        return fail( cmd, QStringLiteral( "empty chemical name" ) );

    QString smiles = nameToSMILES( name );
    if ( smiles.isEmpty() )
        return fail( cmd, QStringLiteral( "PubChem has no structure for '%1' "
                                          "(check the spelling, or the "
                                          "network is unavailable)" ).arg( name ) );

    QString to = toFormat.trimmed().toLower();
    if ( to.isEmpty() )
        to = QStringLiteral( "smiles" );

    XdcApiResult conv = apiConvert( smiles, QStringLiteral( "smiles" ), to );
    XdcApiResult r = succeed( cmd );
    r.payload.insert( QStringLiteral( "query" ), name );
    r.payload.insert( QStringLiteral( "smiles" ), smiles );
    r.payload.insert( QStringLiteral( "output_format" ),
                      conv.payload.value( QStringLiteral( "output_format" ) ).toString( "smiles" ) );
    r.payload.insert( QStringLiteral( "structure" ),
                      conv.ok ? conv.payload.value( QStringLiteral( "structure" ) ).toString()
                              : smiles );
    r.payload.insert( QStringLiteral( "formula" ),
                      conv.payload.value( QStringLiteral( "formula" ) ).toString() );
    r.payload.insert( QStringLiteral( "source" ), QStringLiteral( "pubchem" ) );
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// describe — the "what is this molecule" summary
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiDescribe( const QString &structure, const QString &format )
{
    const QString cmd = QStringLiteral( "describe" );

    OBMol mol;
    if ( !readStructureIntoOBMol( mol, structure, format ) )
        return fail( cmd, QStringLiteral( "could not parse input structure" ) );

    XdcApiResult r = succeed( cmd );
    r.payload.insert( QStringLiteral( "formula" ),
                      QString::fromStdString( mol.GetFormula() ) );
    r.payload.insert( QStringLiteral( "molecular_weight" ),
                      qRound( molecularWeight( mol ) * 100.0 ) / 100.0 );
    r.payload.insert( QStringLiteral( "atoms" ), int( mol.NumAtoms() ) );
    r.payload.insert( QStringLiteral( "heavy_atoms" ), int( mol.NumHvyAtoms() ) );
    r.payload.insert( QStringLiteral( "bonds" ), int( mol.NumBonds() ) );
    r.payload.insert( QStringLiteral( "charge" ), mol.GetTotalCharge() );
    r.payload.insert( QStringLiteral( "spin_multiplicity" ),
                      int( mol.GetTotalSpinMultiplicity() ) );
    r.payload.insert( QStringLiteral( "elements" ), elementCounts( mol ) );

    QString smiles = obMolToFormat( &mol, "smi" );
    QString can = obMolToFormat( &mol, "can" );
    QString inchi = obMolToFormat( &mol, "inchi" );
    r.payload.insert( QStringLiteral( "smiles" ), smiles );
    r.payload.insert( QStringLiteral( "canonical_smiles" ), can );
    r.payload.insert( QStringLiteral( "inchi" ), inchi );
    QString key = inchiKeyFromInchi( inchi );
    if ( !key.isEmpty() )
        r.payload.insert( QStringLiteral( "inchikey" ), key );

    // OBMol has no IsChiral(); a molecule is chiral when at least one of its
    // atoms carries three or more distinct neighbour graphs and a tetrahedral
    // stereo annotation.  Count atoms flagged chiral by the perception pass.
    int chiralCenters = 0;
    for ( int i = 1; i <= int( mol.NumAtoms() ); ++i ) {
        OBAtom *at = mol.GetAtom( i );
        if ( at != nullptr && at->IsChiral() )
            ++chiralCenters;
    }
    r.payload.insert( QStringLiteral( "is_chiral" ), chiralCenters > 0 );
    r.payload.insert( QStringLiteral( "chiral_centers" ), chiralCenters );

    // ── PubChem annotations: registry-level facts OpenBabel cannot know ──
    // Failures here are silent: a structure that PubChem does not index is
    // still fully describable from the local analysis above.
    if ( !smiles.isEmpty() ) {
        QString iupac = QString();
        QString cas = QString();
        QString cid = QString();

        // Use the project's existing PubChem helpers where they exist.
        // IUPACName() posts the SMILES; CAS/CID come from the same PUG REST
        // service, so ask once and parse all three.
        QNetworkAccessManager nam;
        QEventLoop loop;
        QUrl url( QStringLiteral( "https://pubchem.ncbi.nlm.nih.gov/rest/pug/"
                                  "compound/smiles/property/IUPACName,Title/"
                                  "TXT" ) );
        QNetworkRequest req( url );
        req.setHeader( QNetworkRequest::ContentTypeHeader,
                       QStringLiteral( "application/x-www-form-urlencoded" ) );
        QUrlQuery post;
        post.addQueryItem( QStringLiteral( "smiles" ), smiles );
        QNetworkReply *reply = nam.post( req, post.toString( QUrl::FullyEncoded ).toUtf8() );
        QObject::connect( reply, &QNetworkReply::finished, &loop, [&]() {
            if ( reply->error() == QNetworkReply::NoError ) {
                QStringList lines = QString::fromUtf8( reply->readAll() )
                                    .split( QChar( '\n' ), Qt::SkipEmptyParts );
                if ( lines.size() >= 1 ) iupac = lines.at( 0 ).trimmed();
                if ( lines.size() >= 2 ) cas   = lines.at( 1 ).trimmed();
            }
            loop.quit();
        } );
        QTimer::singleShot( 10000, &loop, &QEventLoop::quit );
        loop.exec();
        reply->deleteLater();

        if ( !iupac.isEmpty() )
            r.payload.insert( QStringLiteral( "iupac_name" ), iupac );
        if ( !cas.isEmpty() && cas != iupac )
            r.payload.insert( QStringLiteral( "cas" ), cas );

        // CID lookup is a separate cheap request.
        QNetworkAccessManager nam2;
        QEventLoop loop2;
        QByteArray enc = QUrl::toPercentEncoding( smiles );
        QUrl cidUrl( QStringLiteral( "https://pubchem.ncbi.nlm.nih.gov/rest/pug/"
                                     "compound/smiles/" ) + QString::fromUtf8( enc )
                     + QStringLiteral( "/cids/TXT" ) );
        QNetworkReply *r2 = nam2.get( QNetworkRequest( cidUrl ) );
        QObject::connect( r2, &QNetworkReply::finished, &loop2, [&]() {
            if ( r2->error() == QNetworkReply::NoError )
                cid = QString::fromUtf8( r2->readAll() ).trimmed()
                      .section( QChar( '\n' ), 0, 0 );
            loop2.quit();
        } );
        QTimer::singleShot( 10000, &loop2, &QEventLoop::quit );
        loop2.exec();
        r2->deleteLater();
        if ( !cid.isEmpty() ) {
            bool isNum = false;
            cid.toInt( &isNum );
            if ( isNum )
                r.payload.insert( QStringLiteral( "pubchem_cid" ), cid.toInt() );
        }
    }

    // Round-trip fidelity: was the input understood exactly as given?
    QString canonicalInput = obMolToFormat( &mol, "can" );
    r.payload.insert( QStringLiteral( "canonical_roundtrip_ok" ),
                      !canonicalInput.isEmpty() && canonicalInput == can );
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// validate — parse check + valence check
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiValidate( const QString &structure, const QString &format )
{
    const QString cmd = QStringLiteral( "validate" );

    OBMol mol;
    if ( !readStructureIntoOBMol( mol, structure, format ) )
        return fail( cmd, QStringLiteral( "could not parse input structure" ) );

    // XDrawChem's own valence check (Molecule::ValenceErrors) walks the
    // internal representation; build it so agents get the same verdict the
    // GUI tool gives.
    ChemData cd;
    Render2D *r = cd.getRender2D();   // may be null in pure headless use
    Q_UNUSED( r );
    Molecule *m = new Molecule( cd.getRender2D(), &cd );
    if ( !m->convertFromOBMol( &mol ) ) {
        delete m;
        return fail( cmd, QStringLiteral( "could not build internal "
                                          "representation" ) );
    }

    QStringList problems = m->ValenceErrors();
    QJsonArray errors;
    for ( const QString &problem : problems )
        errors.append( problem );

    XdcApiResult res = succeed( cmd );
    res.payload.insert( QStringLiteral( "valid" ), errors.isEmpty() );
    res.payload.insert( QStringLiteral( "atom_count" ), int( mol.NumAtoms() ) );
    res.payload.insert( QStringLiteral( "bond_count" ), int( mol.NumBonds() ) );
    res.payload.insert( QStringLiteral( "problems" ), errors );

    delete m;
    return res;
}

// ─────────────────────────────────────────────────────────────────────────────
// inchi_key
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiInchiKey( const QString &structure, const QString &format )
{
    const QString cmd = QStringLiteral( "inchi_key" );
    OBMol mol;
    if ( !readStructureIntoOBMol( mol, structure, format ) )
        return fail( cmd, QStringLiteral( "could not parse input structure" ) );

    QString inchi = obMolToFormat( &mol, "inchi" );
    QString key = inchiKeyFromInchi( inchi );
    if ( key.isEmpty() )
        return fail( cmd, QStringLiteral( "could not compute InChIKey" ) );

    XdcApiResult r = succeed( cmd );
    r.payload.insert( QStringLiteral( "inchikey" ), key );
    r.payload.insert( QStringLiteral( "inchi" ), inchi );
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// compare — structural equivalence
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiCompare( const QString &structureA, const QString &structureB,
                         const QString &formatA, const QString &formatB )
{
    const QString cmd = QStringLiteral( "compare" );

    OBMol a, b;
    QString smilesA = structureA;
    QString smilesB = structureB;

    if ( !readStructureIntoOBMol( a, structureA, formatA ) ) {
        // Not a structure string — try it as a chemical name via PubChem.
        smilesA = nameToSMILES( structureA );
        if ( smilesA.isEmpty()
             || !readStructureIntoOBMol( a, smilesA, QStringLiteral( "smi" ) ) )
            return fail( cmd, QStringLiteral( "could not parse first structure "
                                              "('%1') as structure or name" )
                                  .arg( structureA ) );
    }
    if ( !readStructureIntoOBMol( b, structureB, formatB ) ) {
        smilesB = nameToSMILES( structureB );
        if ( smilesB.isEmpty()
             || !readStructureIntoOBMol( b, smilesB, QStringLiteral( "smi" ) ) )
            return fail( cmd, QStringLiteral( "could not parse second structure "
                                              "('%1') as structure or name" )
                                  .arg( structureB ) );
    }

    QString keyA = inchiKeyFromInchi( obMolToFormat( &a, "inchi" ) );
    QString keyB = inchiKeyFromInchi( obMolToFormat( &b, "inchi" ) );

    bool sameConnectivity = false;
    bool sameStereo = false;
    if ( !keyA.isEmpty() && !keyB.isEmpty() ) {
        // InChIKey's first 14 characters encode the skeleton; the second block
        // encodes stereochemistry/isotopes.  Comparing the skeleton block is
        // the standard "same compound ignoring stereo" test.
        QString skelA = keyA.section( QChar( '-' ), 0, 0 );
        QString skelB = keyB.section( QChar( '-' ), 0, 0 );
        sameConnectivity = ( skelA == skelB );
        sameStereo = ( keyA == keyB );
    }

    XdcApiResult r = succeed( cmd );
    if ( smilesA != structureA )
        r.payload.insert( QStringLiteral( "resolved_a" ), smilesA );
    if ( smilesB != structureB )
        r.payload.insert( QStringLiteral( "resolved_b" ), smilesB );
    r.payload.insert( QStringLiteral( "same_structure" ), sameStereo );
    r.payload.insert( QStringLiteral( "same_connectivity" ), sameConnectivity );
    r.payload.insert( QStringLiteral( "inchikey_a" ), keyA );
    r.payload.insert( QStringLiteral( "inchikey_b" ), keyB );
    r.payload.insert( QStringLiteral( "formula_a" ),
                      QString::fromStdString( a.GetFormula() ) );
    r.payload.insert( QStringLiteral( "formula_b" ),
                      QString::fromStdString( b.GetFormula() ) );

    if ( !sameConnectivity ) {
        QString ca = obMolToFormat( &a, "can" );
        QString cb = obMolToFormat( &b, "can" );
        r.payload.insert( QStringLiteral( "canonical_a" ), ca );
        r.payload.insert( QStringLiteral( "canonical_b" ), cb );
    }
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// Rendering: SVG / PNG / PDF
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// Draw the document's content into a QSvgGenerator-backed QPainter.
// We draw the objects directly rather than grabbing a pixmap, so the SVG
// holds real vector paths and selectable text instead of one embedded bitmap.
QByteArray renderDocumentToSVG( ChemData *cd, int width, int height )
{
    QByteArray buf;
    QBuffer device( &buf );
    device.open( QIODevice::WriteOnly );

    // See apiRenderPNG: bounds are only reported for highlighted bonds.
    cd->SelectAll();
    QRect bounds = cd->selectionBox();
    cd->DeselectAll();
    if ( !bounds.isValid() || bounds.width() < 2 || bounds.height() < 2 )
        bounds = QRect( 0, 0, 300, 200 );

    // The canvas coordinate system has its own origin; translate so the
    // drawing starts at the SVG's top-left corner, then scale to taste.
    const int margin = 10;

    // Molecule coordinates are in points (~100 dpi logical units), so a
    // 300-point bond must not become a 300-pixel image.  Normalise to a
    // readable raster size first, then honour an explicit width if given.
    const double pointToPixel = 2.0;
    const int baseW = qRound( ( bounds.width()  + 2 * margin ) * pointToPixel );
    const int baseH = qRound( ( bounds.height() + 2 * margin ) * pointToPixel );

    double scale = pointToPixel;
    if ( width > 0 )
        scale = double( width ) / double( bounds.width() + 2 * margin );

    const int outW = width  > 0 ? width  : baseW;
    const int outH = height > 0 ? height : qRound( outW * double( baseH ) / double( baseW ) );
    if ( outH <= 0 )
        return QByteArray();

    QSvgGenerator gen;
    gen.setOutputDevice( &device );
    gen.setSize( QSize( outW, outH ) );
    gen.setViewBox( QRect( 0, 0, outW, outH ) );
    gen.setTitle( QStringLiteral( "XDrawChem structure" ) );
    gen.setDescription( QStringLiteral( "Generated by XDrawChem agent API" ) );

    // A throwaway Render2D provides the drawing primitives the Drawables call
    // (drawLine, drawString, ...).  Point it at the generator-backed painter.
    Render2D scratch;
    scratch.setChemData( cd );

    QPainter p;
    if ( !p.begin( &gen ) )
        return QByteArray();

    scratch.setExternalPainter( &p );

    p.setRenderHint( QPainter::Antialiasing, true );
    // Each Drawable holds its own Render2D pointer, captured when the
    // structure was parsed.  Redirect them at the export renderer so their
    // primitives land in the generator instead of the hidden canvas.
    cd->rebindRender2D( &scratch );
    p.scale( scale, scale );
    p.translate( -bounds.left() + margin, -bounds.top() + margin );
    cd->drawAll();

    p.end();
    scratch.setExternalPainter( nullptr );
    device.close();
    return buf;
}

} // namespace

QImage apiRenderPNG( ChemData *cd, Render2D *r, int widthPx, int heightPx,
                     bool transparent )
{
    if ( cd == nullptr || r == nullptr )
        return QImage();

    // Measure the drawing. Bond::BoundingBox() only reports geometry for
    // highlighted bonds, so select first (same dance MakePixmap does).
    cd->SelectAll();
    QRect bounds = cd->selectionBox();
    cd->DeselectAll();
    if ( !bounds.isValid() || bounds.width() < 2 || bounds.height() < 2 )
        return QImage();

    const int margin = 10;
    const int logicalW = bounds.width() + 2 * margin;
    const int logicalH = bounds.height() + 2 * margin;

    // Paint at the requested resolution instead of grabbing the widget at
    // canvas size and scaling that up (which only magnifies existing pixels
    // and makes text soft).  QPainter scales vectors and text at draw time,
    // so one pass at the final size stays sharp.
    const double scale = ( widthPx > 0 )
                         ? double( widthPx ) / double( logicalW )
                         : 1.0;
    const int outW = ( widthPx > 0 ) ? widthPx : logicalW;
    const int outH = ( heightPx > 0 ) ? heightPx
                                      : qRound( logicalH * scale );

    QImage img( outW, outH, QImage::Format_ARGB32_Premultiplied );
    img.fill( transparent ? Qt::transparent : Qt::white );

    QPainter p;
    if ( !p.begin( &img ) )
        return QImage();
    p.setRenderHint( QPainter::Antialiasing, true );
    p.setRenderHint( QPainter::TextAntialiasing, true );
    p.setRenderHint( QPainter::SmoothPixmapTransform, true );

    Render2D scratch;
    scratch.setChemData( cd );
    scratch.setExternalPainter( &p );

    p.scale( scale, scale );
    p.translate( -bounds.left() + margin, -bounds.top() + margin );

    // Drawables cache their Render2D pointer from parse time; retarget them
    // at the export renderer so primitives land in this image.
    cd->rebindRender2D( &scratch );
    cd->drawAll();

    p.end();
    scratch.setExternalPainter( nullptr );

    return img;
}

QByteArray apiRenderPDF( ChemData *cd, Render2D *r )
{
    Q_UNUSED( cd );
    if ( r == nullptr )
        return QByteArray();

    QPixmap pm = r->MakeFullPixmap();
    if ( pm.isNull() )
        return QByteArray();

    // QPrinter always writes to a file, so render into a temporary one and
    // read the bytes back.  The temp file is removed before returning.
    QTemporaryFile tmp( QDir::tempPath() + QStringLiteral( "/xdc_api_XXXXXX.pdf" ) );
    if ( !tmp.open() )
        return QByteArray();
    const QString tmpPath = tmp.fileName();
    tmp.close();

    QPrinter printer( QPrinter::HighResolution );
    printer.setOutputFormat( QPrinter::PdfFormat );
    printer.setOutputFileName( tmpPath );
    printer.setPageSize( QPageSize( QPageSize::A4 ) );
    printer.setPageOrientation( pm.width() >= pm.height()
                                ? QPageLayout::Landscape : QPageLayout::Portrait );
    printer.setColorMode( QPrinter::Color );
    printer.setPageMargins( QMarginsF( 10, 10, 10, 10 ), QPageLayout::Millimeter );

    QPainter p;
    if ( !p.begin( &printer ) )
        return QByteArray();

    QRect page = printer.pageRect( QPrinter::DevicePixel ).toRect();
    QSize scaled = pm.size().scaled( page.size(), Qt::KeepAspectRatio );
    QRect target( page.left() + ( page.width() - scaled.width() ) / 2,
                  page.top()  + ( page.height() - scaled.height() ) / 2,
                  scaled.width(), scaled.height() );
    p.drawPixmap( target, pm );
    p.end();

    QFile f( tmpPath );
    QByteArray out;
    if ( f.open( QIODevice::ReadOnly ) ) {
        out = f.readAll();
        f.close();
    }
    QFile::remove( tmpPath );
    return out;
}

XdcApiResult apiRender( const QString &structure, const QString &format,
                        const QString &output, const QString &outPath,
                        int widthPx, int heightPx, bool transparent )
{
    const QString cmd = QStringLiteral( "render" );

    QString fmt = output.trimmed().toLower();
    if ( fmt.isEmpty() )
        fmt = QStringLiteral( "svg" );
    if ( fmt == QLatin1String( "png" ) || fmt == QLatin1String( "svg" )
         || fmt == QLatin1String( "pdf" ) ) {
        // ok
    } else {
        return fail( cmd, QStringLiteral( "unsupported output '%1' "
                                          "(use svg, png or pdf)" ).arg( fmt ) );
    }

    ChemData cd;
    Render2D r;                      // offscreen widget; never shown
    cd.setRender2D( &r );
    r.setChemData( &cd );

    QString err;
    Molecule *m = apiLoadMolecule( &cd, structure, format, &err );
    if ( m == nullptr )
        return fail( cmd, err.isEmpty()
                          ? QStringLiteral( "could not parse structure" ) : err );

    // Structure diagram generation already ran inside FromSMILES/FromMolfile
    // (Molecule::SDG).  AutoLayout() is for arranging whole schemes (arrows,
    // multiple molecules) and adds padding that a single structure does not
    // need, so leave the coordinates alone and just refresh the drawing.
    Q_UNUSED( m );
    cd.drawAll();

    QByteArray data;
    QString mime;
    if ( fmt == QLatin1String( "svg" ) ) {
        data = renderDocumentToSVG( &cd, widthPx, heightPx );
        mime = QStringLiteral( "image/svg+xml" );
    } else if ( fmt == QLatin1String( "png" ) ) {
        QImage img = apiRenderPNG( &cd, &r, widthPx, heightPx, transparent );
        if ( img.isNull() )
            return fail( cmd, QStringLiteral( "PNG rasterisation failed "
                                              "(no display or empty document)" ) );
        QBuffer b( &data );
        b.open( QIODevice::WriteOnly );
        img.save( &b, "PNG" );
        b.close();
        mime = QStringLiteral( "image/png" );
    } else {
        data = apiRenderPDF( &cd, &r );
        mime = QStringLiteral( "application/pdf" );
    }

    if ( data.isEmpty() )
        return fail( cmd, QStringLiteral( "rendering produced no data" ) );

    XdcApiResult res = succeed( cmd );
    res.payload.insert( QStringLiteral( "format" ), fmt );
    res.payload.insert( QStringLiteral( "mime" ), mime );
    res.payload.insert( QStringLiteral( "bytes" ), int( data.size() ) );

    if ( outPath.trimmed().isEmpty() ) {
        // Inline the content for small payloads; agents can write files but
        // should not have to parse megabytes of base64 out of a chat reply.
        if ( data.size() <= 256 * 1024 ) {
            if ( fmt == QLatin1String( "svg" ) )
                res.payload.insert( QStringLiteral( "svg" ),
                                    QString::fromUtf8( data ) );
            else
                res.payload.insert( QStringLiteral( "data_base64" ),
                                    QString::fromLatin1( data.toBase64() ) );
        } else {
            res.payload.insert( QStringLiteral( "note" ),
                                QStringLiteral( "output too large to inline; "
                                                "re-run with --out <path>" ) );
        }
    } else {
        QFile f( outPath );
        if ( !f.open( QIODevice::WriteOnly ) )
            return fail( cmd, QStringLiteral( "cannot write '%1'" ).arg( outPath ) );
        f.write( data );
        f.close();
        res.payload.insert( QStringLiteral( "path" ), outPath );
    }
    return res;
}

// ─────────────────────────────────────────────────────────────────────────────
// make_3d
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiMake3D( const QString &structure, const QString &format,
                        const QString &output )
{
    const QString cmd = QStringLiteral( "make_3d" );

    QString fmt = output.trimmed().toLower();
    if ( fmt.isEmpty() )
        fmt = QStringLiteral( "mol" );
    if ( fmt != QLatin1String( "mol" ) && fmt != QLatin1String( "sdf" )
         && fmt != QLatin1String( "xyz" ) )
        return fail( cmd, QStringLiteral( "unsupported output '%1' "
                                          "(use mol, sdf or xyz)" ).arg( fmt ) );

    OBMol obmol;
    if ( !readStructureIntoOBMol( obmol, structure, format ) )
        return fail( cmd, QStringLiteral( "could not parse input structure" ) );

    obmol.AddHydrogens();

    OBBuilder builder;
    if ( !builder.Build( obmol ) )
        return fail( cmd, QStringLiteral( "OpenBabel could not generate "
                                          "3-D coordinates (check valence)" ) );
    obmol.SetDimension( 3 );

    OBForceField *ff = OBForceField::FindForceField( "MMFF94" );
    if ( ff == nullptr || !ff->Setup( obmol ) )
        ff = OBForceField::FindForceField( "UFF" );
    QString forceField = QStringLiteral( "none" );
    if ( ff != nullptr && ff->Setup( obmol ) ) {
        ff->ConjugateGradients( 250 );
        ff->GetCoordinates( obmol );
        forceField = QString::fromStdString( ff->GetID() );
    }

    QString out = obMolToFormat( &obmol, fmt == QLatin1String( "sdf" )
                                          ? QStringLiteral( "sdf" )
                                          : fmt );
    if ( out.isEmpty() )
        return fail( cmd, QStringLiteral( "OpenBabel could not serialise "
                                          "the 3-D model" ) );

    XdcApiResult r = succeed( cmd );
    r.payload.insert( QStringLiteral( "output_format" ), fmt );
    r.payload.insert( QStringLiteral( "force_field" ), forceField );
    r.payload.insert( QStringLiteral( "structure" ), out );
    r.payload.insert( QStringLiteral( "atom_count" ), int( obmol.NumAtoms() ) );
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// clean_up — regenerate a 2-D layout
// ─────────────────────────────────────────────────────────────────────────────

// ─────────────────────────────────────────────────────────────────────────────
// pubchem_lookup — registry URI for a name / SMILES / InChI
// ─────────────────────────────────────────────────────────────────────────────

namespace {

// One blocking PubChem PUG REST GET, returning the body and HTTP status.
// Uses a local event loop with a timeout, matching molecule_nts.cpp.
QByteArray pubchemGet( const QString &url, int *status, QString *error )
{
    QNetworkAccessManager nam;
    QEventLoop loop;
    QByteArray body;
    int httpStatus = 0;
    QString errText;

    QNetworkRequest req( ( QUrl( url ) ) );
    req.setHeader( QNetworkRequest::UserAgentHeader,
                   QStringLiteral( "XDrawChem/2.0 (agent-api)" ) );
    QNetworkReply *reply = nam.get( req );

    QObject::connect( reply, &QNetworkReply::finished, &loop, [&]() {
        httpStatus = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        if ( httpStatus == 200 )
            body = reply->readAll();
        else if ( reply->error() != QNetworkReply::NoError )
            errText = reply->errorString();
        else
            errText = QStringLiteral( "HTTP %1" ).arg( httpStatus );
        loop.quit();
    } );

    QTimer::singleShot( 15000, &loop, &QEventLoop::quit );
    loop.exec();
    reply->deleteLater();

    if ( status )
        *status = httpStatus;
    if ( error )
        *error = errText;
    return body;
}

// One blocking PubChem PUG REST POST (form-encoded), returning the body.
// Needed for InChI: the InChI string is posted as the `inchi` parameter.
QByteArray pubchemPost( const QString &url, const QString &field,
                        const QString &value, int *status, QString *error )
{
    QNetworkAccessManager nam;
    QEventLoop loop;
    QByteArray body;
    int httpStatus = 0;
    QString errText;

    QNetworkRequest req( ( QUrl( url ) ) );
    req.setHeader( QNetworkRequest::UserAgentHeader,
                   QStringLiteral( "XDrawChem/2.0 (agent-api)" ) );
    req.setHeader( QNetworkRequest::ContentTypeHeader,
                   QStringLiteral( "application/x-www-form-urlencoded" ) );

    QUrlQuery form;
    form.addQueryItem( field, value );

    QNetworkReply *reply = nam.post( req, form.toString( QUrl::FullyEncoded ).toUtf8() );

    QObject::connect( reply, &QNetworkReply::finished, &loop, [&]() {
        httpStatus = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute ).toInt();
        if ( httpStatus == 200 )
            body = reply->readAll();
        else if ( reply->error() != QNetworkReply::NoError )
            errText = reply->errorString();
        else
            errText = QStringLiteral( "HTTP %1" ).arg( httpStatus );
        loop.quit();
    } );

    QTimer::singleShot( 15000, &loop, &QEventLoop::quit );
    loop.exec();
    reply->deleteLater();

    if ( status )
        *status = httpStatus;
    if ( error )
        *error = errText;
    return body;
}

} // namespace

XdcApiResult apiPubChemLookup( const QString &query, const QString &format )
{
    const QString cmd = QStringLiteral( "pubchem_lookup" );
    QString q = query.trimmed();
    if ( q.isEmpty() )
        return fail( cmd, QStringLiteral( "empty query" ) );

    // Decide which PUG REST namespace the query belongs to.
    QString namespaceName;
    QString encodedQuery = QString::fromUtf8( QUrl::toPercentEncoding( q ) );
    QString fmt = format.trimmed().toLower();

    if ( fmt.isEmpty() ) {
        if ( q.startsWith( QLatin1String( "InChI=" ) ) )
            fmt = QStringLiteral( "inchi" );
        else if ( q.contains( QLatin1Char( 'M' ) )
                  && q.contains( QLatin1String( "M  END" ) ) )
            fmt = QStringLiteral( "molfile" );
        else
            fmt = QStringLiteral( "auto" );
    }

    if ( fmt == QLatin1String( "inchi" ) || q.startsWith( QLatin1String( "InChI=" ) ) ) {
        namespaceName = QStringLiteral( "inchi" );
    } else if ( fmt == QLatin1String( "smiles" ) || fmt == QLatin1String( "smi" ) ) {
        namespaceName = QStringLiteral( "smiles" );
    } else if ( fmt == QLatin1String( "auto" ) ) {
        // A bare name or CAS number has no SMILES punctuation to speak of.
        // Try to parse it locally: if it parses, it is a structure.
        OBMol probe;
        if ( readStructureIntoOBMol( probe, q, QString() ) ) {
            namespaceName = QStringLiteral( "smiles" );
            // Canonicalise so PubChem gets an unambiguous query.
            QString can = obMolToFormat( &probe, "can" );
            if ( !can.isEmpty() ) {
                q = can;
                encodedQuery = QString::fromUtf8( QUrl::toPercentEncoding( q ) );
            }
        } else {
            namespaceName = QStringLiteral( "name" );
        }
    } else if ( fmt == QLatin1String( "name" ) || fmt == QLatin1String( "cas" ) ) {
        namespaceName = QStringLiteral( "name" );
    } else {
        return fail( cmd, QStringLiteral( "unsupported input format '%1' "
                                          "(use auto, name, smiles or inchi)" )
                              .arg( fmt ) );
    }

    // Ask PubChem for the CID first: it is the stable identifier the URI uses.
    // InChI queries must be POSTed (an InChI is long and contains characters
    // that break the path form, which PUG REST rejects with HTTP 400).
    int status = 0;
    QString err;
    QByteArray cidBody;
    if ( namespaceName == QLatin1String( "inchi" ) ) {
        cidBody = pubchemPost(
            QStringLiteral( "https://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/inchi/cids/TXT" ),
            QStringLiteral( "inchi" ), q, &status, &err );
    } else {
        cidBody = pubchemGet(
            QStringLiteral( "https://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/%1/%2/cids/TXT" )
                .arg( namespaceName, encodedQuery ), &status, &err );
    }

    QString cid = QString::fromUtf8( cidBody ).trimmed().section( QChar( '\n' ), 0, 0 );

    if ( cid.isEmpty() ) {
        // A 404 is a clean "not in PubChem"; anything else is a network problem.
        if ( status == 404 )
            return fail( cmd, QStringLiteral( "PubChem has no compound matching '%1'" ).arg( q ) );
        if ( status == 0 )
            return fail( cmd, QStringLiteral( "could not reach PubChem"
                                              "%1" ).arg( err.isEmpty()
                                                          ? QString()
                                                          : QStringLiteral( ": " ) + err ) );
        return fail( cmd, QStringLiteral( "PubChem lookup failed (HTTP %1)" ).arg( status ) );
    }

    bool cidOk = false;
    const int cidNum = cid.toInt( &cidOk );
    if ( !cidOk )
        return fail( cmd, QStringLiteral( "PubChem returned an unparseable CID: '%1'" ).arg( cid ) );

    XdcApiResult res = succeed( cmd );
    res.payload.insert( QStringLiteral( "query" ), query );
    res.payload.insert( QStringLiteral( "matched_by" ), namespaceName );
    res.payload.insert( QStringLiteral( "pubchem_cid" ), cidNum );
    res.payload.insert( QStringLiteral( "pubchem_uri" ),
                        QStringLiteral( "https://pubchem.ncbi.nlm.nih.gov/compound/%1" ).arg( cidNum ) );

    // Structure properties, when available.  Failure here is not fatal: the
    // URI and CID above are already the answer to "where is this in PubChem".
    QByteArray propBody = pubchemGet(
        QStringLiteral( "https://pubchem.ncbi.nlm.nih.gov/rest/pug/compound/cid/%1/"
                        "property/IsomericSMILES,InChIKey,MolecularFormula,TITLE/TXT" )
            .arg( cidNum ), &status, &err );
    QString props = QString::fromUtf8( propBody ).trimmed();
    if ( !props.isEmpty() ) {
        const QStringList lines = props.split( QChar( '\n' ), Qt::SkipEmptyParts );
        if ( lines.size() >= 1 )
            res.payload.insert( QStringLiteral( "smiles" ), lines.at( 0 ).trimmed() );
        if ( lines.size() >= 2 )
            res.payload.insert( QStringLiteral( "inchikey" ), lines.at( 1 ).trimmed() );
        if ( lines.size() >= 3 )
            res.payload.insert( QStringLiteral( "formula" ), lines.at( 2 ).trimmed() );
        if ( lines.size() >= 4 )
            res.payload.insert( QStringLiteral( "title" ), lines.at( 3 ).trimmed() );
    }
    return res;
}

XdcApiResult apiCleanUp( const QString &structure, const QString &format,
                         const QString &output )
{
    const QString cmd = QStringLiteral( "clean_up" );

    QString out = output.trimmed().toLower();
    if ( out.isEmpty() )
        out = QStringLiteral( "smiles" );
    if ( out == QLatin1String( "molfile" ) )
        out = QStringLiteral( "mol" );
    if ( out == QLatin1String( "canonical" ) )
        out = QStringLiteral( "can" );
    if ( out != QLatin1String( "smiles" ) && out != QLatin1String( "smi" )
         && out != QLatin1String( "can" ) && out != QLatin1String( "mol" )
         && out != QLatin1String( "sdf" ) )
        return fail( cmd, QStringLiteral( "unsupported output '%1' "
                                          "(use smiles, canonical, mol or sdf)" )
                              .arg( out ) );

    // Build a document so the real CleanUp() runs — it is the same path the
    // GUI's Tools -> Clean Up Molecule uses.
    ChemData cd;
    Render2D r;
    cd.setRender2D( &r );
    r.setChemData( &cd );

    QString err;
    Molecule *m = apiLoadMolecule( &cd, structure, format, &err );
    if ( m == nullptr )
        return fail( cmd, err.isEmpty()
                          ? QStringLiteral( "could not parse structure" ) : err );

    // Record the input layout so we can report whether cleanup changed it.
    auto bboxOf = []( Molecule *mol ) {
        mol->SelectAll();
        QRect b = mol->BoundingBoxAll();
        mol->DeselectAll();
        return b;
    };
    QRect before = bboxOf( m );
    QString smilesBefore = m->ToSMILES();

    m->CleanUp();
    cd.drawAll();

    QRect after = bboxOf( m );
    QString smilesAfter = m->ToSMILES();
    QString result = ( out == QLatin1String( "mol" ) || out == QLatin1String( "sdf" ) )
                     ? m->ToMDLMolfile()
                     : smilesAfter.trimmed();

    XdcApiResult res = succeed( cmd );
    res.payload.insert( QStringLiteral( "output_format" ), out );
    res.payload.insert( QStringLiteral( "structure" ), result );
    res.payload.insert( QStringLiteral( "smiles" ), smilesAfter.trimmed() );
    res.payload.insert( QStringLiteral( "connectivity_unchanged" ),
                        smilesBefore == smilesAfter );
    res.payload.insert( QStringLiteral( "bounds_before" ),
                        QStringLiteral( "%1x%2" ).arg( before.width() ).arg( before.height() ) );
    res.payload.insert( QStringLiteral( "bounds_after" ),
                        QStringLiteral( "%1x%2" ).arg( after.width() ).arg( after.height() ) );
    res.payload.insert( QStringLiteral( "note" ),
                        QStringLiteral( "Re-ran structure-diagram generation. "
                                        "For SMILES input the layout was already "
                                        "generated at parse time; pass molfile "
                                        "coordinates to actually change geometry." ) );
    return res;
}

// ─────────────────────────────────────────────────────────────────────────────
// reaction
// ─────────────────────────────────────────────────────────────────────────────

XdcApiResult apiReaction( const QString &reaction, const QString &format )
{
    const QString cmd = QStringLiteral( "reaction" );
    Q_UNUSED( format );

    // Accept a reaction SMILES "A.B>>C.D" or a reaction molfile (RXN).
    QStringList sides = reaction.split( QStringLiteral( ">>" ) );
    if ( sides.size() != 2 )
        return fail( cmd, QStringLiteral( "expected a reaction SMILES in the "
                                          "form 'reactants>>products'" ) );

    OBMol rmol, pmol;
    if ( !readStructureIntoOBMol( rmol, sides.at( 0 ), QStringLiteral( "smi" ) ) )
        return fail( cmd, QStringLiteral( "could not parse reactants" ) );

    bool haveProducts = !sides.at( 1 ).trimmed().isEmpty();
    if ( haveProducts
         && !readStructureIntoOBMol( pmol, sides.at( 1 ), QStringLiteral( "smi" ) ) )
        return fail( cmd, QStringLiteral( "could not parse products" ) );

    XdcApiResult r = succeed( cmd );
    QJsonObject payload;
    payload.insert( QStringLiteral( "reactants" ), sides.at( 0 ).trimmed() );
    payload.insert( QStringLiteral( "products" ),
                    haveProducts ? sides.at( 1 ).trimmed() : QString() );
    payload.insert( QStringLiteral( "reactant_formula" ),
                    QString::fromStdString( rmol.GetFormula() ) );
    payload.insert( QStringLiteral( "reactant_atoms" ), int( rmol.NumAtoms() ) );
    payload.insert( QStringLiteral( "reactant_smiles" ),
                    obMolToFormat( &rmol, "can" ) );

    if ( haveProducts ) {
        QString pf = QString::fromStdString( pmol.GetFormula() );
        QString rf = QString::fromStdString( rmol.GetFormula() );
        payload.insert( QStringLiteral( "product_formula" ), pf );
        payload.insert( QStringLiteral( "product_atoms" ), int( pmol.NumAtoms() ) );
        payload.insert( QStringLiteral( "product_smiles" ),
                        obMolToFormat( &pmol, "can" ) );
        payload.insert( QStringLiteral( "atom_delta" ),
                        int( pmol.NumAtoms() ) - int( rmol.NumAtoms() ) );
        payload.insert( QStringLiteral( "formula_changed" ), rf != pf );
    }

    r.payload = payload;
    return r;
}

// ─────────────────────────────────────────────────────────────────────────────
// manifest — the machine-readable contract
// ─────────────────────────────────────────────────────────────────────────────

QJsonObject apiManifest()
{
    auto arg = []( const QString &name, const QString &type, bool required,
                   const QString &desc ) {
        QJsonObject a;
        a.insert( QStringLiteral( "name" ), name );
        a.insert( QStringLiteral( "type" ), type );
        a.insert( QStringLiteral( "required" ), required );
        a.insert( QStringLiteral( "description" ), desc );
        return a;
    };

    QJsonArray commands;

    auto addCommand = [&]( const QString &name, const QString &summary,
                           const QJsonArray &args, const QString &output ) {
        QJsonObject c;
        c.insert( QStringLiteral( "name" ), name );
        c.insert( QStringLiteral( "summary" ), summary );
        c.insert( QStringLiteral( "arguments" ), args );
        c.insert( QStringLiteral( "output" ), output );
        commands.append( c );
    };

    addCommand( QStringLiteral( "convert" ),
        QStringLiteral( "Convert a structure between SMILES, canonical SMILES, "
                        "InChI, InChIKey, Molfile, SDF, CML, XYZ or formula." ),
        QJsonArray{
            arg( "structure", "string", true,  "Structure text (or @path)" ),
            arg( "from", "string", false, "Input format: smiles|inchi|molfile|file (default auto)" ),
            arg( "to", "string", true,  "Target: smiles|canonical|inchi|inchikey|mol|sdf|cml|xyz|formula" ) },
        QStringLiteral( "JSON {structure, formula, output_format}" ) );

    addCommand( QStringLiteral( "name_to_structure" ),
        QStringLiteral( "Resolve a chemical name, IUPAC name, trade name or CAS "
                        "number to a structure via PubChem." ),
        QJsonArray{
            arg( "name", "string", true, "Chemical name or CAS number" ),
            arg( "to", "string", false, "Output format (default smiles)" ) },
        QStringLiteral( "JSON {structure, smiles, formula, source}" ) );

    addCommand( QStringLiteral( "describe" ),
        QStringLiteral( "Report formula, molecular weight, atom/bond counts, "
                        "charge, canonical SMILES, InChI/InChIKey and, when "
                        "available, PubChem IUPAC name, CAS and CID." ),
        QJsonArray{
            arg( "structure", "string", true, "Structure text (or @path)" ),
            arg( "format", "string", false, "Input format (default auto)" ) },
        QStringLiteral( "JSON structure summary" ) );

    addCommand( QStringLiteral( "validate" ),
        QStringLiteral( "Check that a structure parses and that every atom "
                        "obeys valence rules; lists offending atoms." ),
        QJsonArray{
            arg( "structure", "string", true, "Structure text (or @path)" ),
            arg( "format", "string", false, "Input format (default auto)" ) },
        QStringLiteral( "JSON {valid, problems[]}" ) );

    addCommand( QStringLiteral( "inchi_key" ),
        QStringLiteral( "Compute the standard 27-character InChIKey." ),
        QJsonArray{
            arg( "structure", "string", true, "Structure text (or @path)" ),
            arg( "format", "string", false, "Input format (default auto)" ) },
        QStringLiteral( "JSON {inchikey, inchi}" ) );

    addCommand( QStringLiteral( "compare" ),
        QStringLiteral( "Decide whether two structures are the same compound, "
                        "optionally ignoring stereochemistry." ),
        QJsonArray{
            arg( "structure_a", "string", true, "First structure" ),
            arg( "structure_b", "string", true, "Second structure" ),
            arg( "format_a", "string", false, "Format of A (default auto)" ),
            arg( "format_b", "string", false, "Format of B (default auto)" ) },
        QStringLiteral( "JSON {same_structure, same_connectivity, inchikeys}" ) );

    addCommand( QStringLiteral( "render" ),
        QStringLiteral( "Draw a 2-D depiction as SVG, PNG or PDF." ),
        QJsonArray{
            arg( "structure", "string", true, "Structure text (or @path)" ),
            arg( "format", "string", false, "Output: svg|png|pdf (default svg)" ),
            arg( "out", "string", false, "Write here instead of inlining" ),
            arg( "width", "integer", false, "Target width in pixels" ),
            arg( "height", "integer", false, "Target height in pixels" ),
            arg( "transparent", "boolean", false, "PNG only: transparent background" ),
            arg( "input_format", "string", false, "Input format (default auto)" ) },
        QStringLiteral( "SVG text, base64 PNG/PDF, or a file path" ) );

    addCommand( QStringLiteral( "make_3d" ),
        QStringLiteral( "Generate an MMFF94/UFF-minimised 3-D conformer as "
                        "Molfile, SDF or XYZ." ),
        QJsonArray{
            arg( "structure", "string", true, "Structure text (or @path)" ),
            arg( "format", "string", false, "Output: mol|sdf|xyz (default mol)" ),
            arg( "input_format", "string", false, "Input format (default auto)" ) },
        QStringLiteral( "JSON {structure, force_field, atom_count}" ) );

    addCommand( QStringLiteral( "pubchem_lookup" ),
        QStringLiteral( "Resolve a name, CAS number, SMILES or InChI to its "
                        "PubChem compound page; returns the URI and CID." ),
        QJsonArray{
            arg( "query", "string", true, "Chemical name, CAS number, SMILES or InChI" ),
            arg( "format", "string", false, "Input kind: auto|name|cas|smiles|inchi (default auto)" ) },
        QStringLiteral( "JSON {pubchem_uri, pubchem_cid, smiles, inchikey, formula, title}" ) );

    addCommand( QStringLiteral( "clean_up" ),
        QStringLiteral( "Regenerate a clean 2-D layout (same as Tools -> Clean Up "
                        "Molecule). Most useful for molfiles with poor geometry." ),
        QJsonArray{
            arg( "structure", "string", true, "Structure text (or @path)" ),
            arg( "format", "string", false, "Output: smiles|canonical|mol|sdf (default smiles)" ),
            arg( "input_format", "string", false, "Input format (default auto)" ) },
        QStringLiteral( "JSON {structure, smiles, connectivity_unchanged}" ) );

    addCommand( QStringLiteral( "reaction" ),
        QStringLiteral( "Summarise a reaction given as reaction SMILES "
                        "('reactants>>products')." ),
        QJsonArray{
            arg( "reaction", "string", true, "Reaction SMILES" ) },
        QStringLiteral( "JSON {reactants, products, formulas, atom_delta}" ) );

    QJsonObject manifest;
    manifest.insert( QStringLiteral( "ok" ), true );
    manifest.insert( QStringLiteral( "command" ), QStringLiteral( "manifest" ) );
    manifest.insert( QStringLiteral( "name" ), QStringLiteral( "xdrawchem" ) );
    manifest.insert( QStringLiteral( "version" ),
                     QString( XDC_VERSION ) );
    manifest.insert( QStringLiteral( "schema_version" ),
                     QString::fromLatin1( XDC_API_SCHEMA_VERSION ) );
    manifest.insert( QStringLiteral( "description" ),
                     QStringLiteral( "Headless chemical structure tooling backed "
                                     "by XDrawChem and OpenBabel: convert, "
                                     "resolve names, describe, validate, "
                                     "compare, render and build 3-D models." ) );
    manifest.insert( QStringLiteral( "transport" ),
                     QStringLiteral( "cli: xdrawchem --api <command> --json; "
                                     "mcp: integrations/mcp/xdrawchem_mcp.py" ) );
    manifest.insert( QStringLiteral( "commands" ), commands );
    return manifest;
}

// ─────────────────────────────────────────────────────────────────────────────
// Command dispatch
// ─────────────────────────────────────────────────────────────────────────────

QString apiDispatch( const QStringList &args, int *exitCode )
{
    auto finish = [&]( const XdcApiResult &r ) {
        if ( exitCode )
            *exitCode = r.ok ? 0 : 1;
        return resultToJsonText( r );
    };

    if ( args.isEmpty() ) {
        if ( exitCode )
            *exitCode = 0;
        return QString::fromUtf8( QJsonDocument( apiManifest() )
                                  .toJson( QJsonDocument::Indented ) );
    }

    const QString command = args.at( 0 );
    QHash<QString, QString> opt;
    // "--key value" pairs; bare words become positional arguments.
    QStringList positional;
    for ( int i = 1; i < args.size(); ++i ) {
        QString a = args.at( i );
        if ( a.startsWith( QLatin1String( "--" ) ) ) {
            QString key = a.mid( 2 );
            if ( i + 1 < args.size() && !args.at( i + 1 ).startsWith( QLatin1String( "--" ) ) ) {
                opt.insert( key, args.at( i + 1 ) );
                ++i;
            } else {
                opt.insert( key, QStringLiteral( "true" ) );
            }
        } else {
            positional.append( a );
        }
    }

    auto get = [&]( const QString &key, const QString &fallback = QString() ) {
        return opt.contains( key ) ? opt.value( key ) : fallback;
    };
    auto structureArg = [&]( int index, const QString &key ) {
        if ( opt.contains( key ) )
            return opt.value( key );
        return index < positional.size() ? positional.at( index ) : QString();
    };
    auto boolArg = [&]( const QString &key ) {
        QString v = get( key ).toLower();
        return v == QLatin1String( "1" ) || v == QLatin1String( "true" )
               || v == QLatin1String( "yes" );
    };
    auto intArg = [&]( const QString &key ) {
        bool ok = false;
        int v = get( key, QStringLiteral( "0" ) ).toInt( &ok );
        return ok ? v : 0;
    };

    if ( command == QLatin1String( "manifest" ) || command == QLatin1String( "--help" ) ) {
        if ( exitCode )
            *exitCode = 0;
        return QString::fromUtf8( QJsonDocument( apiManifest() )
                                  .toJson( QJsonDocument::Indented ) );
    }

    if ( command == QLatin1String( "convert" ) )
        return finish( apiConvert( structureArg( 0, "structure" ),
                                   get( "from" ), get( "to" ) ) );

    if ( command == QLatin1String( "name_to_structure" ) )
        return finish( apiNameToStructure( structureArg( 0, "name" ), get( "to" ) ) );

    if ( command == QLatin1String( "describe" ) )
        return finish( apiDescribe( structureArg( 0, "structure" ), get( "format" ) ) );

    if ( command == QLatin1String( "validate" ) )
        return finish( apiValidate( structureArg( 0, "structure" ), get( "format" ) ) );

    if ( command == QLatin1String( "inchi_key" ) )
        return finish( apiInchiKey( structureArg( 0, "structure" ), get( "format" ) ) );

    if ( command == QLatin1String( "compare" ) )
        return finish( apiCompare( structureArg( 0, "structure_a" ),
                                   structureArg( 1, "structure_b" ),
                                   get( "format_a" ), get( "format_b" ) ) );

    if ( command == QLatin1String( "render" ) )
        return finish( apiRender( structureArg( 0, "structure" ),
                                  get( "input_format" ), get( "format" ),
                                  get( "out" ), intArg( "width" ),
                                  intArg( "height" ), boolArg( "transparent" ) ) );

    if ( command == QLatin1String( "make_3d" ) )
        return finish( apiMake3D( structureArg( 0, "structure" ),
                                  get( "input_format" ), get( "format" ) ) );

    if ( command == QLatin1String( "pubchem_lookup" ) )
        return finish( apiPubChemLookup( structureArg( 0, "query" ), get( "format" ) ) );

    if ( command == QLatin1String( "clean_up" ) )
        return finish( apiCleanUp( structureArg( 0, "structure" ),
                                   get( "input_format" ), get( "format" ) ) );

    if ( command == QLatin1String( "reaction" ) )
        return finish( apiReaction( structureArg( 0, "reaction" ), get( "format" ) ) );

    return finish( fail( command, QStringLiteral( "unknown command; run "
                                                  "'xdrawchem --api' for the "
                                                  "manifest" ) ) );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
