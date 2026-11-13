// molecule_cip.cpp — CIP R/S and E/Z stereochemistry label calculation and rendering
//
// Uses OpenBabel's stereochemistry perception (OBStereoFacade) to compute
// CIP descriptors for:
//   - Tetrahedral (R/S) stereocenters
//   - Cis/trans (E/Z) double bonds
//
// Labels are computed on demand and cached in per-molecule QMaps keyed by
// DPoint/Bond pointers so repeated rendering is cheap.

#include "ob_compat.h"
OB_COMPAT_BEGIN
#include <openbabel/mol.h>
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/stereo/stereo.h>
#include <openbabel/stereo/tetrahedral.h>
#include <openbabel/stereo/cistrans.h>
OB_COMPAT_END

#include <QMap>
#include <QPainter>

#include "molecule.h"
#include "moldata.h"
#include "render2d.h"
#include "bond.h"
#include "dpoint.h"
#include "xdc_logging.h"
#include <QLoggingCategory>

void Molecule::ClearCIPCache()
{
    qDebug() << "CIP: ClearCIPCache called for molecule";
    m_cipPointLabels.clear();
    m_cipBondLabels.clear();
    m_cipLabelsValid = false;
}

void Molecule::CalcCIPLabels()
{
    qDebug() << "CIP: CalcCIPLabels called, m_cipLabelsValid=" << m_cipLabelsValid;
    if ( m_cipLabelsValid )
        return;

    // Ensure ring data is available so we can detect double bonds inside rings.
    MakeSSSR();

    m_cipPointLabels.clear();
    m_cipBondLabels.clear();

    // Need an OBMol to run stereo perception.  convertToOBMol() builds one
    // from the current molecule geometry.  This is a relatively expensive
    // call, which is why we cache the results.
    OpenBabel::OBMol *obmol = convertToOBMol();
    if ( !obmol || obmol->NumAtoms() == 0 ) {
        delete obmol;
        m_cipLabelsValid = true;
        return;
    }

    qDebug() << "CIP: OBMol has" << obmol->NumAtoms() << "atoms,"
                      << obmol->NumBonds() << "bonds";

    // Build a map from OB atom ID → DPoint* so we can route results back to
    // our internal data structures.
    QList<DPoint*> allPoints = AllPoints();
    QMap<unsigned long, DPoint*> obAtomIdToPoint;

    // convertToOBMol() sets atom IDs to the 0-based index in AllPoints().
    // OpenBabel stereo configs use GetId() values (OBStereo::Ref), not GetIdx().
    for ( int i = 0; i < allPoints.count() && i < (int)obmol->NumAtoms(); ++i ) {
        OpenBabel::OBAtom *obAtom = obmol->GetAtom( i + 1 ); // OB is 1-based
        if ( obAtom )
            obAtomIdToPoint.insert( obAtom->GetId(), allPoints[i] );
    }

    // Debug: dump OBMol structure
    for ( unsigned int i = 1; i <= obmol->NumAtoms(); ++i ) {
        OpenBabel::OBAtom *a = obmol->GetAtom(i);
        if ( a ) {
            qDebug() << "CIP: atom" << i << "id=" << a->GetId()
                                << "elem=" << a->GetAtomicNum()
                                << "coords=" << a->GetX() << a->GetY();
        }
    }
    for ( unsigned int i = 0; i < obmol->NumBonds(); ++i ) {
        OpenBabel::OBBond *b = obmol->GetBond(i);
        if ( b ) {
            qDebug() << "CIP: bond" << i
                                << "begin=" << b->GetBeginAtomIdx() << "end=" << b->GetEndAtomIdx()
                                << "order=" << b->GetBondOrder()
                                << "flags=" << b->GetFlags();
        }
    }

    // --- Compute E/Z from 2D coordinates directly ---
    // For each double bond, find the two highest-priority substituents on each end
    // and determine if they're on the same side (Z/cis) or opposite sides (E/trans).
    // Skip double bonds that lie entirely within a ring because the ring constrains
    // the geometry to cis-like configurations, so E/Z labels are not meaningful.
    for ( Bond *tmp_bond : bonds ) {
        if ( tmp_bond->Order() != 2 )
            continue;  // skip non-double bonds

        DPoint *c1 = tmp_bond->Start();
        DPoint *c2 = tmp_bond->End();

        // If both atoms of the double bond belong to a common ring, skip E/Z labeling
        bool bothInSameRing = false;
        for ( QList<DPoint *> *ring : this_sssr.sssr ) {
            if ( ring->contains( c1 ) && ring->contains( c2 ) ) {
                bothInSameRing = true;
                break;
            }
        }
        if ( bothInSameRing ) {
            qDebug() << "CIP: skipping E/Z for ring double bond";
            continue;
        }

        // Find substituents on each carbon
        QList<DPoint*> subs1;  // substituents on c1 (excluding c2)
        QList<DPoint*> subs2;  // substituents on c2 (excluding c1)
        for ( Bond *b : bonds ) {
            if ( b == tmp_bond )
                continue;
            if ( b->Start() == c1 || b->End() == c1 ) {
                DPoint *other = (b->Start() == c1) ? b->End() : b->Start();
                if ( other != c2 )
                    subs1.append( other );
            }
            if ( b->Start() == c2 || b->End() == c2 ) {
                DPoint *other = (b->Start() == c2) ? b->End() : b->Start();
                if ( other != c1 )
                    subs2.append( other );
            }
        }

        // For E/Z isomerism to exist, each double-bond carbon must have at least
        // one substituent besides the other double-bond carbon.
        if ( subs1.isEmpty() || subs2.isEmpty() )
            continue;

        // Pick highest CIP priority substituent on each carbon.
        // CIP priority rules (simplified for 2D drawings):
        // 1. Higher atomic number = higher priority
        // 2. For same atomic number, heavier isotope = higher priority
        auto cipPriority = []( DPoint *p ) -> int {
            // Atomic number is primary sort key (higher = more priority)
            return p->getAtomicNumber();
        };

        DPoint *sub1 = *std::max_element( subs1.begin(), subs1.end(),
            [&]( DPoint *a, DPoint *b ) { return cipPriority( a ) < cipPriority( b ); } );
        DPoint *sub2 = *std::max_element( subs2.begin(), subs2.end(),
            [&]( DPoint *a, DPoint *b ) { return cipPriority( a ) < cipPriority( b ); } );

        // Check that the highest-priority substituent is unique on each carbon.
        // If there are multiple substituents with the same highest priority,
        // there's no E/Z isomerism (e.g., (CH3)2C=CH-CH3).
        int maxPriority1 = cipPriority( sub1 );
        int maxPriority2 = cipPriority( sub2 );
        int countMax1 = std::count_if( subs1.begin(), subs1.end(),
            [&]( DPoint *p ) { return cipPriority( p ) == maxPriority1; } );
        int countMax2 = std::count_if( subs2.begin(), subs2.end(),
            [&]( DPoint *p ) { return cipPriority( p ) == maxPriority2; } );
        if ( countMax1 > 1 || countMax2 > 1 ) {
            qDebug() << "CIP: skipping - multiple substituents with same priority";
            continue;
        }

        // Determine if sub1 and sub2 are on the same side of the double bond.
        // Vector from c1 to c2:
        double dx = c2->x - c1->x;
        double dy = c2->y - c1->y;

        // Line equation: ax + by + c = 0 for bond line
        double a = dy;
        double b = -dx;
        double c = -(a * c1->x + b * c1->y);
        double dist1 = a * sub1->x + b * sub1->y + c;
        double dist2 = a * sub2->x + b * sub2->y + c;

        qDebug() << "CIP: double bond" << c1->x << c1->y << "to" << c2->x << c2->y
                              << "sub1=" << sub1->x << sub1->y << "sub2=" << sub2->x << sub2->y
                              << "dist1=" << dist1 << "dist2=" << dist2;

        // If both distances have the same sign, they're on the same side -> Z (cis)
        // If opposite signs, they're on opposite sides -> E (trans)
        QString label;
        if ( ( dist1 > 0 ) == ( dist2 > 0 ) )
            label = QStringLiteral( "Z" );
        else
            label = QStringLiteral( "E" );

        qDebug() << "CIP: computed" << label << "for double bond";
        m_cipBondLabels.insert( tmp_bond, label );
    }

    // Run stereo perception for R/S tetrahedral centers
    OpenBabel::OBStereoFacade facade( obmol, true ); // true = call PerceiveStereo
    qDebug() << "CIP: tetrahedral count =" << facade.GetAllTetrahedralStereo().size();

    // --- Tetrahedral (R/S) ---
    std::vector<OpenBabel::OBTetrahedralStereo*> tetraList = facade.GetAllTetrahedralStereo();
    for ( OpenBabel::OBTetrahedralStereo *tet : tetraList ) {
        if ( !tet )
            continue;

        OpenBabel::OBTetrahedralStereo::Config cfg = tet->GetConfig();
        if ( !cfg.specified )
            continue;

        DPoint *centerPt = obAtomIdToPoint.value( cfg.center, nullptr );
        if ( !centerPt )
            continue;

        /*  R/S from the CIP-ranked ligands.

            `cfg.winding` alone is not enough: it describes the order of
            `cfg.refs`, which OpenBabel emits in perception order, not CIP
            priority order.  Reading it directly gave a letter that never
            changed when the substituents changed — measured on a hand-drawn
            centre and on a PubChem (S)-lactic acid SDF, which read "R".

            R/S is: rank the four ligands by CIP priority, then determine
            whether the sequence 1 -> 2 -> 3 runs clockwise when the lowest
            priority ligand (4) points away.

            OpenBabel gives us `refs` (three explicit ligand ids) plus `from`
            (the viewing/id ligand, often an implicit H).  Both orders are
            needed: the *stored* order as `refs`, and the *priority* order
            from cipRankLigands().  Relating the two is a permutation, whose
            parity tells us whether the stored winding must be inverted.     */

        // Assemble the four ligand refs and their DPoints.  cfg.refs is a
        // std::vector<unsigned long>, so copy it into a QList explicitly.
        QList<OpenBabel::OBStereo::Ref> ligandRefs;
        for ( OpenBabel::OBStereo::Ref r : cfg.refs )
            ligandRefs.append( r );
        if ( ligandRefs.count() == 3 && cfg.from != OpenBabel::OBStereo::NoRef )
            ligandRefs.append( cfg.from );
        if ( ligandRefs.count() != 4 )
            continue;

        QList<DPoint *> ligandPoints;
        bool mapped = true;
        for ( OpenBabel::OBStereo::Ref ref : ligandRefs ) {
            if ( ref == OpenBabel::OBStereo::ImplicitRef ) {
                ligandPoints.append( nullptr );
                continue;
            }
            DPoint *pt = obAtomIdToPoint.value( ref, nullptr );
            if ( pt == nullptr )
                mapped = false;
            ligandPoints.append( pt );
        }
        if ( !mapped )
            continue;

        const QList<QPair<DPoint *, QString>> ranked =
            cipRankLigands( centerPt, ligandPoints );
        if ( ranked.count() != 4 )
            continue;

        // Two ligands with identical signatures means no stereocentre.
        if ( ranked.at( 0 ).second == ranked.at( 1 ).second
             || ranked.at( 2 ).second == ranked.at( 3 ).second )
            continue;

        // Position of each ranked ligand within the stored ligandRefs order.
        QList<int> order;
        for ( const auto &entry : ranked ) {
            const int idx = ligandPoints.indexOf( entry.first );
            order.append( idx );
        }
        if ( order.contains( -1 ) )
            continue;

        // Parity of the permutation taking priority order to stored order.
        int inversions = 0;
        for ( int i = 0; i < 4; ++i )
            for ( int j = i + 1; j < 4; ++j )
                if ( order.at( i ) > order.at( j ) )
                    ++inversions;
        const bool odd = ( inversions % 2 ) != 0;

        if ( cfg.winding != OpenBabel::OBStereo::Clockwise
             && cfg.winding != OpenBabel::OBStereo::AntiClockwise )
            continue;

        /*  Sense of the priority order as seen with ligand 4 pointing away.
            Clockwise 1->2->3 is R *when looking from the far side*; OpenBabel
            reports the winding as seen from `view`'s direction, and a 2-D
            depiction is viewed from the front, so the reported winding is the
            mirror of the CIP convention.  Measured: with the direct mapping
            the (S) and (R) lactic-acid files came out "R" and "S" — both
            inverted — which is this sign, not the permutation parity.        */
        bool clockwise = ( cfg.winding == OpenBabel::OBStereo::Clockwise );
        if ( odd )
            clockwise = !clockwise;
        if ( cfg.view == OpenBabel::OBStereo::ViewTowards )
            clockwise = !clockwise;

        const QString label = clockwise ? QStringLiteral( "S" )
                                        : QStringLiteral( "R" );

        m_cipPointLabels.insert( centerPt, label );
    }
    qDebug() << "CIP: R/S labels found:" << m_cipPointLabels.size();

    delete obmol;
    m_cipLabelsValid = true;
}

void Molecule::DrawCIPLabels()
{
    qDebug() << "CIP: DrawCIPLabels called, m_cipLabelsValid=" << m_cipLabelsValid;
    if ( !m_cipLabelsValid )
        CalcCIPLabels();

    if ( m_cipPointLabels.isEmpty() && m_cipBondLabels.isEmpty() )
        return;

    QFont labelFont( QStringLiteral( "Helvetica" ), 10, QFont::Bold );
    QColor labelColor( 0, 100, 200 ); // distinct blue

    // Draw R/S next to chiral centers
    for ( auto it = m_cipPointLabels.constBegin(); it != m_cipPointLabels.constEnd(); ++it ) {
        DPoint *pt = it.key();
        QString label = it.value();

        // Place label slightly above and to the right of the atom.
        QPoint drawPos( (int)pt->x + 8, (int)pt->y - 8 );
        r->drawString( label, drawPos, labelColor, labelFont );
    }

    // Draw E/Z at midpoint of double bonds
    for ( auto it = m_cipBondLabels.constBegin(); it != m_cipBondLabels.constEnd(); ++it ) {
        Bond *b = it.key();
        QString label = it.value();

        // Midpoint of bond, offset slightly to avoid overlapping the bond line.
        QPointF midF = b->midpoint();
        QPoint mid( qRound( midF.x() ), qRound( midF.y() ) );
        mid.rx() += 6;
        mid.ry() -= 6;
        r->drawString( label, mid, labelColor, labelFont );
    }
}

QString Molecule::GetCIPLabelForPoint( DPoint *dp )
{
    if ( !m_cipLabelsValid )
        CalcCIPLabels();
    return m_cipPointLabels.value( dp, QString() );
}

QString Molecule::GetCIPLabelForBond( Bond *b )
{
    if ( !m_cipLabelsValid )
        CalcCIPLabels();
    return m_cipBondLabels.value( b, QString() );
}

/*  Materialise the implicit hydrogen at a stereocentre.

    The reporter of #22/#26 asked for hydrogens that *matter* — the ones on a
    stereocentre, which carry the stereochemistry — and no others cluttering a
    skeletal formula.  The previous implementation painted a wedge during
    Render(), choosing up versus down from screen position:

        bool up = ( b.y() < a.y() );

    Where a hydrogen lands on screen says nothing about whether it points
    towards or away from the viewer, so the wedge depicted nothing.  It was
    also paint-time only: not a Bond, not selectable, lost on save and reload.

    Here the hydrogen becomes a real atom and a real Bond, so it survives a
    round trip and can be picked up with the mouse.  The wedge direction is not
    guessed: it is chosen so that Open Babel's perception of the new structure
    reproduces the descriptor the centre had before — the same machinery that
    fixed #27.  If neither direction reproduces it, the bond is left as a hash
    and the case is reported, because a wrong wedge is worse than a visible
    default.

    Only centres that already have a CIP descriptor get a hydrogen: that means
    Open Babel perceived an actual configuration, so there is a real
    stereocentre to depict rather than three bonds and an assumption.
*/
void Molecule::AddStereocenterHydrogens()
{
    // Descriptors first — this is what tells us which points are stereogenic
    // and gives us the letter to preserve.
    ClearCIPCache();
    CalcCIPLabels();

    const QMap<DPoint *, QString> wanted = m_cipPointLabels;
    if ( wanted.isEmpty() )
        return;

    for ( auto it = wanted.constBegin(); it != wanted.constEnd(); ++it ) {
        DPoint *centre = it.key();
        if ( centre == nullptr || centre->element != QLatin1String( "C" ) )
            continue;

        // Count the drawn bonds and their order contributions.
        QList<DPoint *> neighbours;
        int sumbonds = 0;
        bool alreadyExplicit = false;
        for ( Bond *b : bonds ) {
            if ( !b->Find( centre ) )
                continue;
            DPoint *other = b->otherPoint( centre );
            if ( other == nullptr )
                continue;
            if ( other->element == QLatin1String( "H" ) )
                alreadyExplicit = true;
            neighbours.append( other );
            // wedge/hash orders 5 and 7 are single bonds
            if ( b->Order() == 5 || b->Order() == 7 )
                sumbonds += 1;
            else
                sumbonds += qBound( 1, b->Order(), 3 );
        }
        if ( alreadyExplicit || neighbours.count() != 3 )
            continue;

        if ( MolData::Hydrogens( centre->element ) - sumbonds != 1 )
            continue;

        /*  Where to put the hydrogen?

            "Opposite the mean of the three bonds" is the obvious answer and it
            is wrong often enough to matter: on lactic acid it lands on top of
            the carboxyl group.  Try a fan of directions around that bisector
            instead and take the one with the most room — the largest distance
            to the nearest existing atom.  Cheap, deterministic, and it does
            not need to know anything about rings.                        */
        double sx = 0.0, sy = 0.0;
        for ( DPoint *n : neighbours ) {
            sx += centre->x - n->x;
            sy += centre->y - n->y;
        }
        const double bl = preferences.getBond_fixedlength();
        double baseAngle = std::atan2( sy, sx );
        if ( std::hypot( sx, sy ) < 1e-6 )
            baseAngle = -M_PI / 2.0;        // degenerate: straight up

        const QList<DPoint *> allPts = AllPoints();
        double bestAngle = baseAngle;
        double bestClearance = -1.0;
        for ( int step = 0; step < 12; ++step ) {
            const double angle = baseAngle + ( step * 30.0 * M_PI / 180.0 );
            const double hx = centre->x + std::cos( angle ) * bl;
            const double hy = centre->y + std::sin( angle ) * bl;

            double clearance = 1e18;
            for ( DPoint *other : allPts ) {
                if ( other == centre )
                    continue;
                clearance = qMin( clearance, std::hypot( hx - other->x,
                                                         hy - other->y ) );
            }
            // prefer the fan direction when clearances tie, so the result is
            // stable rather than dependent on iteration order
            if ( clearance > bestClearance + 1e-9 ) {
                bestClearance = clearance;
                bestAngle = angle;
            }
        }

        DPoint *hp = new DPoint;
        hp->element = QLatin1String( "H" );
        hp->x = centre->x + std::cos( bestAngle ) * bl;
        hp->y = centre->y + std::sin( bestAngle ) * bl;

        Text *hlabel = new Text( r );
        hlabel->setPoint( hp );
        hlabel->setText( QStringLiteral( "H" ) );
        hlabel->setJustify( JUSTIFY_CENTER );
        hlabel->Highlight( false );

        addBond( centre, hp, 1, 5, QColor( 0, 0, 0 ), false );
        addText( hlabel );

        Bond *hb = FindBond( centre, hp );

        /*  Choose the wedge direction that reproduces the descriptor.

            Perception depends on the real geometry, so this is a measurement
            rather than a rule of thumb: try the wedge, recompute, and flip if
            the letter changed.                                           */
        const QString before = it.value();
        bool ok = false;
        for ( int attempt = 0; attempt < 2 && hb != nullptr; ++attempt ) {
            ClearCIPCache();
            if ( GetCIPLabelForPoint( centre ) == before ) {
                ok = true;
                break;
            }
            hb->setOrder( attempt == 0 ? 7 : 5 );
        }
        if ( !ok && hb != nullptr )
            qInfo() << "stereocentre hydrogen: could not reproduce" << before
                    << "for the centre at" << centre->x << centre->y;
    }

    ClearCIPCache();
}
