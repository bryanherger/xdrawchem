// tst_hydrogens.cpp — stereocentre hydrogens (issue #26, Part B)
//
// The reporter of #22/#26 asked for the hydrogens that carry information — the
// ones on a stereocentre — and none of the clutter on a skeletal formula.
//
// The previous implementation painted a wedge during Render() and decided up
// versus down from screen position:
//
//     bool up = ( b.y() < a.y() );
//
// which depicts nothing, was not a Bond, could not be selected, and was lost on
// save and reload.  The replacement creates a real hydrogen atom and a real
// wedge/hash Bond, and chooses the direction by measurement: whichever one
// makes Open Babel's perception reproduce the descriptor the centre already
// had.
//
// QTEST_MAIN: the molecule constructor builds QTextDocument.

#include <QtTest>
#include <QSet>

#include "../xdrawchem/molecule.h"
#include "../xdrawchem/dpoint.h"
#include "../xdrawchem/bond.h"
#include "../xdrawchem/text.h"

namespace {

// Hydrogens bonded to `centre`.
QList<DPoint *> explicitHydrogens( Molecule &mol, DPoint *centre )
{
    QList<DPoint *> out;
    for ( Bond *b : mol.getBonds() ) {
        if ( !b->Find( centre ) )
            continue;
        DPoint *other = b->otherPoint( centre );
        if ( other != nullptr && other->element == QLatin1String( "H" ) )
            out.append( other );
    }
    return out;
}

// The stereocentre: the carbon carrying a CIP descriptor.
DPoint *stereocentre( Molecule &mol )
{
    for ( DPoint *pt : mol.AllPoints() ) {
        if ( !mol.GetCIPLabelForPoint( pt ).isEmpty() )
            return pt;
    }
    return nullptr;
}

} // namespace

class TestHydrogens : public QObject
{
    Q_OBJECT

private slots:

    // ── A stereocentre gets exactly one explicit hydrogen ────────────────────
    void stereocentre_gets_one_hydrogen()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@H](O)C(=O)O" );      // (S)-lactic acid

        DPoint *centre = stereocentre( m );
        QVERIFY2( centre != nullptr, "expected a stereocentre" );
        QCOMPARE( explicitHydrogens( m, centre ).count(), 0 );

        m.AddStereocenterHydrogens();

        QCOMPARE( explicitHydrogens( m, centre ).count(), 1 );
    }

    // ── The wedge must reproduce the descriptor ─────────────────────────────
    // This is the whole point: the drawn wedge has to mean something, so the
    // descriptor computed from the structure *with* the hydrogen must be the
    // one the centre had without it.
    void descriptor_survives_materialising_the_hydrogen()
    {
        for ( const QString &smiles : { QStringLiteral( "C[C@H](O)C(=O)O" ),
                                        QStringLiteral( "C[C@@H](O)C(=O)O" ),
                                        QStringLiteral( "N[C@@H](C)C(=O)O" ) } ) {
            Molecule m( nullptr );
            m.FromSMILES( smiles );

            DPoint *centre = stereocentre( m );
            QVERIFY( centre != nullptr );
            const QString before = m.GetCIPLabelForPoint( centre );

            m.AddStereocenterHydrogens();

            m.ClearCIPCache();
            const QString after = m.GetCIPLabelForPoint( centre );

            QVERIFY2( after == before,
                      qPrintable( QStringLiteral( "%1: descriptor changed from %2 to %3 "
                                                  "when the stereocentre hydrogen was drawn" )
                                  .arg( smiles, before, after ) ) );
        }
    }

    // ── The hydrogen is drawn as a stereo bond, not a plain line ────────────
    void hydrogen_bond_is_a_wedge_or_hash()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@H](O)C(=O)O" );

        DPoint *centre = stereocentre( m );
        QVERIFY( centre != nullptr );
        m.AddStereocenterHydrogens();

        const QList<DPoint *> hs = explicitHydrogens( m, centre );
        QCOMPARE( hs.count(), 1 );

        Bond *hb = m.FindBond( centre, hs.first() );
        QVERIFY( hb != nullptr );
        QVERIFY2( hb->Order() == 5 || hb->Order() == 7,
                  qPrintable( QStringLiteral( "hydrogen bond order is %1, expected a "
                                              "wedge (5) or hash (7)" )
                              .arg( hb->Order() ) ) );
    }

    // ── The hydrogen carries an "H" label ───────────────────────────────────
    void hydrogen_has_a_label()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@H](O)C(=O)O" );

        DPoint *centre = stereocentre( m );
        QVERIFY( centre != nullptr );
        m.AddStereocenterHydrogens();

        const QList<DPoint *> hs = explicitHydrogens( m, centre );
        QCOMPARE( hs.count(), 1 );

        bool found = false;
        for ( Text *t : m.AllLabels() )
            if ( t->Start() == hs.first() && t->getText() == QStringLiteral( "H" ) )
                found = true;
        QVERIFY2( found, "the hydrogen atom has no label, so it will not be visible" );
    }

    // ── Idempotency ─────────────────────────────────────────────────────────
    // The method runs after every layout, so a second call must not add a
    // second hydrogen to the same centre.
    void calling_twice_adds_nothing()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@H](O)C(=O)O" );

        DPoint *centre = stereocentre( m );
        QVERIFY( centre != nullptr );

        m.AddStereocenterHydrogens();
        const int afterFirst = explicitHydrogens( m, centre ).count();
        m.AddStereocenterHydrogens();

        QCOMPARE( explicitHydrogens( m, centre ).count(), afterFirst );
        QCOMPARE( afterFirst, 1 );
    }

    // ── Achiral structures are untouched ────────────────────────────────────
    void achiral_structures_get_no_hydrogens()
    {
        for ( const QString &smiles : { QStringLiteral( "CCO" ),        // ethanol
                                        QStringLiteral( "c1ccccc1" ),   // benzene
                                        QStringLiteral( "CC(=O)O" ) } ) {  // acetic acid
            Molecule m( nullptr );
            m.FromSMILES( smiles );

            const int pointsBefore = m.AllPoints().count();
            m.AddStereocenterHydrogens();
            QCOMPARE( m.AllPoints().count(), pointsBefore );
        }
    }

    // ── Two stereocentres, two hydrogens ────────────────────────────────────
    void threonine_gets_one_hydrogen_per_centre()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@@H](O)[C@H](N)C(=O)O" );    // (2S,3R)-threonine

        QList<DPoint *> centres;
        for ( DPoint *pt : m.AllPoints() )
            if ( !m.GetCIPLabelForPoint( pt ).isEmpty() )
                centres.append( pt );
        QCOMPARE( centres.count(), 2 );

        m.AddStereocenterHydrogens();

        int withHydrogen = 0;
        for ( DPoint *c : centres )
            if ( explicitHydrogens( m, c ).count() == 1 )
                ++withHydrogen;
        QCOMPARE( withHydrogen, 2 );
    }
};

QTEST_MAIN( TestHydrogens )
#include "tst_hydrogens.moc"
