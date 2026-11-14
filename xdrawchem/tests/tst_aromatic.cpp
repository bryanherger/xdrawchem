// tst_aromatic.cpp — regression tests for aromatic ring depiction (issue #33)
//
// A benzene ring is drawn as a Kekulé structure: three of the six ring bonds
// carry a second line, alternating around the ring, and each second line goes
// on the *interior* side.
//
// Aspirin used to break the second half of that.  The ring itself was correct —
// the bond orders alternated properly — but the second line of the bond at the
// carboxyl-bearing carbon was drawn on the outside, where it collided with the
// carboxyl bond and read as a spike across the ring.
//
// Cause: Molecule::Render() chose the side by counting other bonds at each
// endpoint and comparing how many made an angle below 180 degrees.  That count
// is blind to which side of the bond a neighbour is on, and it treats a
// substituent as if it were a ring continuation.  At the substituted carbon the
// counts tied, the tie fell through to "side 1", and for that particular bond
// side 1 is outwards.  Ring bonds are now decided geometrically.
//
// QTEST_MAIN: the molecule constructor builds QTextDocument.

#include <QtTest>
#include <QSet>
#include <cmath>

#include "../xdrawchem/molecule.h"
#include "../xdrawchem/dpoint.h"
#include "../xdrawchem/bond.h"
#include "../xdrawchem/render2d.h"

namespace {

// Walk from `current` back to `goal` in exactly `remaining` steps without
// reusing the bond under test.  Returning to the goal one step early is not a
// cycle, so the goal is excluded from the path until the last step.
bool findCycle( const QList<Bond *> &bonds, Bond *from, DPoint *current,
                DPoint *goal, QSet<DPoint *> &path, int remaining,
                QList<DPoint *> &out )
{
    if ( remaining == 0 )
        return current == goal;

    for ( Bond *b : bonds ) {
        if ( b == from || !b->Find( current ) )
            continue;
        DPoint *next = b->otherPoint( current );
        if ( next == nullptr )
            continue;
        if ( next == goal )
            return remaining == 1;          // closes the ring exactly here
        if ( path.contains( next ) )
            continue;
        path.insert( next );
        out.append( next );
        if ( findCycle( bonds, from, next, goal, path, remaining - 1, out ) )
            return true;
        out.removeLast();
        path.remove( next );
    }
    return false;
}

// As findCycle(), but records every cycle of this size instead of stopping at
// the first.  A bond shared by two fused rings closes two of them, and the
// caller must see both before deciding that a second line is outside *every*
// ring.
void collectCycles( const QList<Bond *> &bonds, Bond *from, DPoint *current,
                    DPoint *goal, QSet<DPoint *> &path, int remaining,
                    QList<DPoint *> &out,
                    QList<QList<DPoint *>> &rings )
{
    if ( remaining == 0 ) {
        if ( current == goal ) {
            QList<DPoint *> ring = out;
            ring.append( goal );
            rings.append( ring );
        }
        return;
    }

    for ( Bond *b : bonds ) {
        if ( b == from || !b->Find( current ) )
            continue;
        DPoint *next = b->otherPoint( current );
        if ( next == nullptr )
            continue;
        if ( next == goal ) {
            if ( remaining == 1 ) {
                QList<DPoint *> ring = out;
                ring.append( goal );
                rings.append( ring );
            }
            continue;
        }
        if ( path.contains( next ) )
            continue;
        path.insert( next );
        out.append( next );
        collectCycles( bonds, from, next, goal, path, remaining - 1, out, rings );
        out.removeLast();
        path.remove( next );
    }
}

// Every ring (size 3..8) that contains `bond`.  A fused bond belongs to two.
QList<QList<DPoint *>> allRingsFor( const QList<Bond *> &bonds, Bond *bond )
{
    QList<QList<DPoint *>> rings;
    QList<QSet<DPoint *>> seen;
    for ( int size = 3; size <= 8; ++size ) {
        QList<DPoint *> atoms;
        QSet<DPoint *> path;
        atoms.append( bond->Start() );
        path.insert( bond->Start() );
        path.insert( bond->End() );
        collectCycles( bonds, bond, bond->End(), bond->Start(), path, size - 1,
                       atoms, rings );
    }
    // Drop duplicates: the same ring can be walked from either end.
    QList<QList<DPoint *>> unique;
    for ( const QList<DPoint *> &ring : rings ) {
        QSet<DPoint *> key;
        for ( DPoint *pt : ring )
            key.insert( pt );
        bool duplicate = false;
        for ( const QSet<DPoint *> &s : seen ) {
            if ( s == key ) {
                duplicate = true;
                break;
            }
        }
        if ( duplicate )
            continue;
        seen.append( key );
        unique.append( ring );
    }
    return unique;
}

// The ring that contains `bond`, as a list of its atoms, or empty.
QList<DPoint *> ringFor( const QList<Bond *> &bonds, Bond *bond )
{
    for ( int size = 3; size <= 8; ++size ) {
        QList<DPoint *> atoms;
        QSet<DPoint *> path;
        atoms.append( bond->Start() );
        path.insert( bond->Start() );
        path.insert( bond->End() );
        // remaining edges after this bond: size - 1
        if ( findCycle( bonds, bond, bond->End(), bond->Start(), path,
                        size - 1, atoms ) ) {
            atoms.append( bond->End() );
            return atoms;
        }
    }
    return {};
}

// Which perpendicular side of `bond` is the ring interior?
// Returns true when the interior is the +perpendicular side, matching the
// convention Bond::Render() uses: perpendicular of (End - Start) rotated +90.
bool interiorIsPlusSide( Bond *bond, const QList<DPoint *> &ring )
{
    double cx = 0.0, cy = 0.0;
    for ( DPoint *pt : ring ) {
        cx += pt->x;
        cy += pt->y;
    }
    cx /= ring.size();
    cy /= ring.size();

    const double angle =
        ( Drawable::getAngle( bond->Start(), bond->End() ) + 90.0 ) * M_PI / 180.0;
    const double px = cos( angle );
    const double py = sin( angle );

    const double mx = ( bond->Start()->x + bond->End()->x ) / 2.0;
    const double my = ( bond->Start()->y + bond->End()->y ) / 2.0;
    return ( ( cx - mx ) * px + ( cy - my ) * py ) > 0.0;
}

} // namespace

class TestAromatic : public QObject
{
    Q_OBJECT

private slots:

    // ── Kekulé structure of a substituted ring ───────────────────────────────
    // Three double bonds, and no two of them may share an atom: a six-membered
    // ring alternates by necessity, so a shared atom means a pentavalent carbon
    // in the drawing.
    void aspirin_ring_has_an_alternating_kekule_structure()
    {
        Molecule m( nullptr );
        m.FromSMILES( "CC(=O)Oc1ccccc1C(=O)O" );

        Bond *probe = nullptr;
        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() == 2 && !ringFor( m.getBonds(), b ).isEmpty() ) {
                probe = b;
                break;
            }
        }
        QVERIFY2( probe != nullptr, "expected a double bond inside a ring" );

        const QList<DPoint *> ring = ringFor( m.getBonds(), probe );
        QCOMPARE( ring.count(), 6 );

        int doubles = 0;
        QSet<DPoint *> shared;
        bool clash = false;
        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() != 2 )
                continue;
            if ( !ring.contains( b->Start() ) || !ring.contains( b->End() ) )
                continue;
            ++doubles;
            if ( shared.contains( b->Start() ) || shared.contains( b->End() ) )
                clash = true;
            shared.insert( b->Start() );
            shared.insert( b->End() );
        }
        QCOMPARE( doubles, 3 );
        QVERIFY2( !clash,
                  "two ring double bonds share an atom — that is not a valid "
                  "Kekulé structure for a six-membered ring" );
    }

    // ── The side each second line is drawn on ────────────────────────────────
    // This is the part that was wrong.  Cross-checked against the ring centroid
    // rather than against the renderer's own vote, so the test is not simply
    // restating the implementation.
    void ring_double_bond_inner_lines_are_inside_the_ring()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "CC(=O)Oc1ccccc1C(=O)O" );

        // Let the renderer decide, then read back what it chose.
        m.Render();

        Bond *probe = nullptr;
        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() == 2 && !ringFor( m.getBonds(), b ).isEmpty() ) {
                probe = b;
                break;
            }
        }
        QVERIFY( probe != nullptr );
        const QList<DPoint *> ring = ringFor( m.getBonds(), probe );
        QCOMPARE( ring.count(), 6 );

        int checked = 0;
        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() != 2 )
                continue;
            if ( !ring.contains( b->Start() ) || !ring.contains( b->End() ) )
                continue;
            if ( b->WSide() == 2 )
                continue;       // centred by the C(=O) special case; no side
            ++checked;

            const bool renderedPlus = ( b->WSide() == 1 );
            const bool interiorPlus = interiorIsPlusSide( b, ring );
            QVERIFY2( renderedPlus == interiorPlus,
                      qPrintable( QStringLiteral(
                          "ring double bond (%1,%2)-(%3,%4) draws its second line "
                          "outside the ring: renderer chose %5, interior is %6" )
                          .arg( b->Start()->x ).arg( b->Start()->y )
                          .arg( b->End()->x ).arg( b->End()->y )
                          .arg( b->WSide() )
                          .arg( interiorPlus ? "the +perpendicular side"
                                             : "the -perpendicular side" ) ) );
        }
        QCOMPARE( checked, 3 );
    }

    // ── Benzene must stay correct ────────────────────────────────────────────
    void benzene_inner_lines_are_inside_the_ring()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "c1ccccc1" );
        m.Render();

        Bond *probe = nullptr;
        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() == 2 && !ringFor( m.getBonds(), b ).isEmpty() ) {
                probe = b;
                break;
            }
        }
        QVERIFY( probe != nullptr );
        const QList<DPoint *> ring = ringFor( m.getBonds(), probe );
        QCOMPARE( ring.count(), 6 );

        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() != 2 )
                continue;
            if ( !ring.contains( b->Start() ) || !ring.contains( b->End() ) )
                continue;
            if ( b->WSide() == 2 )
                continue;
            QVERIFY2( ( b->WSide() == 1 ) == interiorIsPlusSide( b, ring ),
                      "benzene: a second line was placed outside the ring" );
        }
    }

    // ── Fused rings ──────────────────────────────────────────────────────────
    // A bond shared by two rings has ring bonds on both sides, so "toward the
    // interior" is ambiguous: either side is inside one of the rings.  What
    // must hold is that the chosen side is inside *a* ring, not floating
    // outside both.  Naphthalene is the control for the fused case.
    void naphthalene_inner_lines_are_inside_a_ring()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "c1ccc2ccccc2c1" );
        m.Render();

        int checked = 0;
        for ( Bond *b : m.getBonds() ) {
            if ( b->Order() != 2 || b->WSide() == 2 )
                continue;

            // Every ring that contains this bond.  A fused bond is in two,
            // and the second line only has to be inside one of them.
            const QList<QList<DPoint *>> rings = allRingsFor( m.getBonds(), b );
            if ( rings.isEmpty() )
                continue;
            ++checked;

            bool insideSomeRing = false;
            for ( const QList<DPoint *> &ring : rings )
                if ( ( b->WSide() == 1 ) == interiorIsPlusSide( b, ring ) )
                    insideSomeRing = true;

            QVERIFY2( insideSomeRing,
                      qPrintable( QStringLiteral(
                          "double bond (%1,%2)-(%3,%4) draws its second line "
                          "outside every ring that contains it" )
                          .arg( b->Start()->x ).arg( b->Start()->y )
                          .arg( b->End()->x ).arg( b->End()->y ) ) );
        }
        QVERIFY2( checked >= 5,
                  qPrintable( QStringLiteral( "expected several ring double "
                                              "bonds, checked %1" ).arg( checked ) ) );
    }
};

QTEST_MAIN( TestAromatic )
#include "tst_aromatic.moc"
