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

        // Determine R/S from winding and view direction.
        // Clockwise + ViewFrom  → R
        // AntiClockwise + ViewFrom → S
        // The inverse is true for ViewTowards, but OB normalizes these
        // for us in GetConfig().  We just use the returned winding.
        QString label;
        if ( cfg.winding == OpenBabel::OBStereo::Clockwise )
            label = QStringLiteral( "R" );
        else if ( cfg.winding == OpenBabel::OBStereo::AntiClockwise )
            label = QStringLiteral( "S" );
        else
            label = QString(); // UnknownWinding

        if ( !label.isEmpty() )
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

// Render chirality-relevant implicit hydrogens.
//
// Only a hydrogen that is the *sole* implicit H on a tetrahedral stereocenter
// is drawn, and it is drawn as a wedge (or hash) bond rather than a floating
// "H" label.  This is the behavior the issue reporter asked for: hydrogens on
// carbon are shown only when they matter for chirality, and never clutter the
// skeletal formula otherwise.
void Molecule::RenderStereocenterHydrogens()
{
    // Identify tetrahedral stereocenters.  We reuse OpenBabel's perception via
    // CalcCIPLabels() when available, but that needs a full OBMol and is only
    // populated when a stereo configuration is actually specified.  For the
    // simpler "is this a potential stereocenter with exactly one implicit H"
    // test we work directly from the bond graph, which is cheaper and does not
    // require OpenBabel to have already assigned a configuration.
    QList<DPoint *> pts = AllPoints();

    for ( DPoint *center : pts ) {
        QString el = center->element;
        if ( el.isEmpty() )
            el = QStringLiteral( "C" );

        // Only carbon can be a stereocenter here (per issue scope).  Also skip
        // atoms that already spell out hydrogens or carry charges.
        if ( el != QStringLiteral( "C" ) )
            continue;
        if ( el.contains( QLatin1Char( '+' ) ) || el.contains( QLatin1Char( '-' ) ) )
            continue;

        // Gather bonded neighbours and count their bond-order contributions.
        QList<DPoint *> neighbours;
        int sumbonds = 0;
        bool hasStereoBond = false;
        for ( Bond *b : bonds ) {
            if ( !b->Find( center ) )
                continue;
            DPoint *other = b->otherPoint( center );
            neighbours.append( other );
            int o = b->Order();
            // Stereo bond orders (5=up/wedge, 7=down/hash) are single bonds.
            if ( o == 5 || o == 7 ) {
                hasStereoBond = true;
                sumbonds += 1;
            } else if ( o < 4 ) {
                sumbonds += o;
            } else {
                sumbonds += 1;
            }
        }

        // A tetrahedral carbon stereocenter needs exactly four distinct
        // substituents.  With implicit hydrogens hidden, that shows up here as
        // three visible bonds plus one implicit hydrogen.
        if ( neighbours.size() != 3 )
            continue;

        int h = MolData::Hydrogens( el ) - sumbonds;
        // Exactly one implicit hydrogen and three distinct neighbours is what
        // makes the implicit H stereochemically meaningful.
        if ( h != 1 )
            continue;

        // Distinctness test: reject if two neighbours are the same atom (can't
        // happen with a simple bond graph, but keep it safe).
        bool distinct = true;
        for ( int i = 0; i < neighbours.size() && distinct; ++i ) {
            for ( int j = i + 1; j < neighbours.size(); ++j ) {
                if ( neighbours[i] == neighbours[j] ) {
                    distinct = false;
                    break;
                }
            }
        }
        if ( !distinct )
            continue;

        // Choose the direction (wedge up vs hash down) for the implicit H so
        // it sits in the least-crowded octant, away from the three visible
        // substituents.  We place the H opposite the average direction of the
        // existing bonds.
        double sx = 0.0, sy = 0.0;
        for ( DPoint *n : neighbours ) {
            sx += ( center->x - n->x );
            sy += ( center->y - n->y );
        }
        double len = sqrt( sx * sx + sy * sy );
        DPoint hp;
        hp.element = QStringLiteral( "H" );
        if ( len < 1e-6 ) {
            // Degenerate geometry; place H straight up.
            hp.x = center->x;
            hp.y = center->y - preferences.getBond_fixedlength();
        } else {
            double bl = preferences.getBond_fixedlength();
            hp.x = center->x + ( sx / len ) * bl;
            hp.y = center->y + ( sy / len ) * bl;
        }

        // Draw the H as a wedge if it points mostly up/right ("toward viewer")
        // or as a hash if it points mostly down/left ("away from viewer").
        QPoint a = center->toQPoint();
        QPoint b = hp.toQPoint();
        bool up = ( b.y() < a.y() );
        if ( up )
            r->drawUpLine( a, b, QColor( 0, 0, 0 ) );
        else
            r->drawDownLine( a, b, QColor( 0, 0, 0 ) );

        // Draw a small "H" label at the far end of the wedge/hash bond so the
        // hydrogen is identifiable.
        r->drawString( QStringLiteral( "H" ), b, QColor( 0, 0, 0 ), QFont( QStringLiteral( "Helvetica" ), 10 ) );
    }
}
