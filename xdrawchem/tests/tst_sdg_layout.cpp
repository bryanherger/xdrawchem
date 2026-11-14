// tst_sdg_layout.cpp — regression tests for structure-diagram generation
//
// SDG (sdg.h) lays molecules out by positioning rings first, then placing
// substituents.  A defect there produces collinear (180 deg) and
// near-overlapping (60 deg) bond angles around a substituted ring nitrogen in
// caffeine, which renders as a substituent drawn on top of an existing bond.
//
// The tests below FAIL against the current implementation and pass once the
// defect is fixed.  They are committed deliberately: they pin the defect and
// are the definition of done for the SDG work.
//
// Findings so far (see tools/sdg_angles.py for the measurement harness):
//   - All-carbon fused aromatics (naphthalene, anthracene) are exact at 120.0.
//   - A methyl on the ring nitrogen BETWEEN the two carbonyls is the trigger;
//     the other ring nitrogen's methyl is fine.
//   - The collision is not an angle choice: two atoms are written to the SAME
//     coordinates, so 174/180 deg angles are a consequence, not the cause.
//   - Disproven hypotheses (do not re-try): starting angleOpt's rot[] loop at
//     index 0; suppressing rotAtom()'s arerings 60 deg rewrite (this one also
//     regressed the N1-methyl case); the id and ihil "unset neighbour" guards
//     in positionAtom()'s branches.
//   - Last trace: the colliding placements all enter positionAtomSubstituents()
//     with modus == ring size (6 for a five-membered ring) and a helpPoint of
//     denormal garbage, i.e. from a ring centre that was never assigned.
//
// These tests measure real interior bond angles from a laid-out molecule and
// assert they stay in a chemically sensible range.  They are deliberately
// tolerant (the drawing need not be perfect, only not degenerate) so that a
// legitimate layout change does not fail them, while a collision does.
//
// QTEST_MAIN: FromSMILES() reaches code that constructs QTextDocument.

#include <QtTest>
#include <QColor>
#include <cmath>

#include "../xdrawchem/molecule.h"
#include "../xdrawchem/dpoint.h"
#include "../xdrawchem/bond.h"

namespace {

// Collect every bond angle at `center`, in degrees.
QList<double> anglesAt( Molecule *mol, DPoint *center )
{
    QList<double> angles;
    QList<Bond *> incident;
    for ( Bond *b : mol->getBonds() ) {
        if ( b->Find( center ) )
            incident.append( b );
    }
    for ( int i = 0; i < incident.count(); ++i ) {
        for ( int j = i + 1; j < incident.count(); ++j ) {
            DPoint *a = incident.at( i )->otherPoint( center );
            DPoint *b = incident.at( j )->otherPoint( center );
            if ( a == nullptr || b == nullptr )
                continue;
            const double v1x = a->x - center->x, v1y = a->y - center->y;
            const double v2x = b->x - center->x, v2y = b->y - center->y;
            const double n1 = std::hypot( v1x, v1y );
            const double n2 = std::hypot( v2x, v2y );
            if ( n1 < 1e-6 || n2 < 1e-6 )
                continue;   // coincident atoms: caught by the overlap test
            double cosang = ( v1x * v2x + v1y * v2y ) / ( n1 * n2 );
            cosang = qBound( -1.0, cosang, 1.0 );
            angles.append( std::acos( cosang ) * 180.0 / M_PI );
        }
    }
    return angles;
}

// Minimum distance between any two distinct atoms, in bond-length units.
// Atom spacing in a 2-D depiction is ~bond length (25 units by default);
// a value far below that means atoms have been drawn on top of each other.
double closestAtomPair( Molecule *mol )
{
    const QList<DPoint *> pts = mol->AllPoints();
    double best = 1e9;
    for ( int i = 0; i < pts.count(); ++i )
        for ( int j = i + 1; j < pts.count(); ++j )
            best = qMin( best, std::hypot( pts[i]->x - pts[j]->x,
                                           pts[i]->y - pts[j]->y ) );
    return best;
}

QList<double> allAngles( Molecule *mol )
{
    QList<double> out;
    for ( DPoint *pt : mol->AllPoints() )
        out.append( anglesAt( mol, pt ) );
    return out;
}

} // namespace


class TestSdgLayout : public QObject
{
    Q_OBJECT

private slots:

    // ── Baseline: all-carbon fused aromatics already lay out correctly ──────
    void naphthalene_regular_angles()
    {
        Molecule m( nullptr );
        m.FromSMILES( "c1ccc2ccccc2c1" );

        const QList<double> angles = allAngles( &m );
        QVERIFY2( !angles.isEmpty(), "naphthalene produced no bond angles" );
        for ( double a : angles ) {
            QVERIFY2( a > 100.0 && a < 140.0,
                      qPrintable( QStringLiteral( "naphthalene angle %1 out of "
                                                  "range" ).arg( a ) ) );
        }
    }

    // ── The regression: a methyl on the ring N between two carbonyls ────────
    // This is the minimal structure that reproduced the collision.  A methyl
    // on the *other* ring nitrogen is fine, so the two are tested together to
    // keep the distinction visible if the fix is ever refactored away.
    void purine_n3_methyl_has_no_collapsed_angles()
    {
        Molecule m( nullptr );
        m.FromSMILES( "c1cnc2c1c(=O)n(C)c(=O)n2" );

        const QList<double> angles = allAngles( &m );
        QVERIFY2( !angles.isEmpty(), "no bond angles produced" );

        for ( double a : angles ) {
            // 180 deg means a substituent drawn exactly opposite a ring bond;
            // 60 deg means two bonds drawn almost on top of each other.
            QVERIFY2( a < 150.0,
                      qPrintable( QStringLiteral( "collinear bond angle %1 deg "
                                                  "(substituent overlaps an "
                                                  "existing bond)" ).arg( a ) ) );
            QVERIFY2( a > 75.0,
                      qPrintable( QStringLiteral( "near-overlapping bond angle "
                                                  "%1 deg" ).arg( a ) ) );
        }
    }

    void purine_n1_methyl_has_no_collapsed_angles()
    {
        Molecule m( nullptr );
        m.FromSMILES( "Cn1cnc2c1c(=O)nc(=O)n2" );

        const QList<double> angles = allAngles( &m );
        QVERIFY2( !angles.isEmpty(), "no bond angles produced" );
        for ( double a : angles )
            QVERIFY2( a < 150.0 && a > 75.0,
                      qPrintable( QStringLiteral( "collapsed angle %1 deg" ).arg( a ) ) );
    }

    // ── Full caffeine: the reported case ────────────────────────────────────
    void caffeine_has_no_collapsed_angles()
    {
        Molecule m( nullptr );
        m.FromSMILES( "Cn1cnc2c1c(=O)n(c(=O)n2C)C" );

        const QList<double> angles = allAngles( &m );
        QVERIFY2( !angles.isEmpty(), "caffeine produced no bond angles" );
        for ( double a : angles )
            QVERIFY2( a < 150.0 && a > 75.0,
                      qPrintable( QStringLiteral( "caffeine angle %1 deg is "
                                                  "degenerate" ).arg( a ) ) );
    }

    // ── Aliphatic chains must keep their zig-zag ─────────────────────────────
    // The "make aliphatics look good" kink in rotAtom() rewrites near-zero
    // rotations to +-60 deg.  It exists for chains and must keep working after
    // any change aimed at ring substituents, so these guard it explicitly.
    void hexane_chain_is_regular()
    {
        Molecule m( nullptr );
        m.FromSMILES( "CCCCCC" );

        const QList<double> angles = allAngles( &m );
        QVERIFY2( !angles.isEmpty(), "hexane produced no bond angles" );
        for ( double a : angles )
            QVERIFY2( a > 90.0 && a < 150.0,
                      qPrintable( QStringLiteral( "hexane angle %1 deg out of "
                                                  "range" ).arg( a ) ) );
        QVERIFY2( closestAtomPair( &m ) > 5.0,
                  "hexane atoms overlap" );
    }

    void cyclohexane_is_regular()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C1CCCCC1" );

        const QList<double> angles = allAngles( &m );
        QVERIFY2( !angles.isEmpty(), "cyclohexane produced no bond angles" );
        for ( double a : angles )
            QVERIFY2( a > 100.0 && a < 140.0,
                      qPrintable( QStringLiteral( "cyclohexane angle %1 deg out "
                                                  "of range" ).arg( a ) ) );
    }

    // ── Atoms must not be drawn on top of each other ────────────────────────
    // Independent of angle arithmetic: a collision shows up here even if the
    // angle at the colliding atom happens not to be sampled.
    // ── Diagnostic: report the first coincident-atom write ─────────────────
    // Not an assertion — prints where SDG puts two atoms on the same point.
    // Enabled permanently while the defect is open; harmless when the layout
    // is correct.
    void caffeine_coordinate_trace()
    {
        Molecule m( nullptr );
        m.FromSMILES( "c1cnc2c1c(=O)n(C)c(=O)n2" );

        const double closest = closestAtomPair( &m );
        qInfo() << "closest atom pair:" << closest;
        QVERIFY( !m.AllPoints().isEmpty() );
    }

    void caffeine_atoms_do_not_overlap()
    {
        Molecule m( nullptr );
        m.FromSMILES( "Cn1cnc2c1c(=O)n(c(=O)n2C)C" );

        const double closest = closestAtomPair( &m );
        QVERIFY2( closest > 5.0,
                  qPrintable( QStringLiteral( "closest atom pair %1 units apart "
                                              "(atoms drawn on top of each "
                                              "other)" ).arg( closest ) ) );
    }
};

QTEST_MAIN( TestSdgLayout )
#include "tst_sdg_layout.moc"
