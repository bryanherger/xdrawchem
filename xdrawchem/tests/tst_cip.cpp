// tst_cip.cpp — regression tests for CIP stereochemistry descriptors
//
// Molecule::CalcCIPLabels() computes R/S for tetrahedral centres (via
// OpenBabel's OBStereoFacade) and E/Z for non-ring double bonds (from the 2D
// coordinates).  GetCIPLabelForPoint()/GetCIPLabelForBond() expose the result.
//
// These tests pin the *chemistry*, not the rendering: a wrong R/S descriptor is
// worse than a missing one, because it actively misleads.  Fixtures are chosen
// so each assertion has one unambiguous correct answer.
//
// QTEST_MAIN: FromSMILES() reaches code that constructs QTextDocument.
//
// Known-weak area, deliberately covered below: CalcCIPLabels() derives E/Z by
// comparing 2D coordinates against the double-bond line, and picks the
// "highest priority" substituent with a simplified atomic-number comparison
// (dpoint.h getAtomicNumber()).  That is not full CIP sequence-rule ordering,
// so the tests assert only cases where the simplified rule agrees with CIP.

#include <QtTest>
#include <QColor>
#include <QSet>
#include <cmath>

#include "../xdrawchem/molecule.h"
#include "../xdrawchem/dpoint.h"
#include "../xdrawchem/bond.h"

namespace {

// All R/S labels present, in atom order, for diagnostics.
QStringList allPointLabels( Molecule *mol )
{
    QStringList out;
    for ( DPoint *pt : mol->AllPoints() ) {
        const QString label = mol->GetCIPLabelForPoint( pt );
        if ( !label.isEmpty() )
            out.append( label );
    }
    return out;
}

// All E/Z labels present, in bond order, for diagnostics.
QStringList allBondLabels( Molecule *mol )
{
    QStringList out;
    for ( Bond *b : mol->getBonds() ) {
        const QString label = mol->GetCIPLabelForBond( b );
        if ( !label.isEmpty() )
            out.append( label );
    }
    return out;
}

// The first double bond in the molecule that carries no E/Z label, or nullptr.
Bond *firstUnlabelledDoubleBond( Molecule *mol )
{
    for ( Bond *b : mol->getBonds() ) {
        if ( b->Order() == 2 && mol->GetCIPLabelForBond( b ).isEmpty() )
            return b;
    }
    return nullptr;
}

} // namespace


class TestCip : public QObject
{
    Q_OBJECT

private slots:

    // ── Achiral control: nothing should be labelled ─────────────────────────
    void ethanol_has_no_stereo_labels()
    {
        Molecule m( nullptr );
        m.FromSMILES( "CCO" );

        const QStringList pts = allPointLabels( &m );
        const QStringList bds = allBondLabels( &m );
        QVERIFY2( pts.isEmpty(),
                  qPrintable( QStringLiteral( "ethanol should have no R/S, got: "
                                              ) + pts.join( QLatin1Char( ',' ) ) ) );
        QVERIFY2( bds.isEmpty(),
                  qPrintable( QStringLiteral( "ethanol should have no E/Z, got: "
                                              ) + bds.join( QLatin1Char( ',' ) ) ) );
    }

    void benzene_has_no_stereo_labels()
    {
        Molecule m( nullptr );
        m.FromSMILES( "c1ccccc1" );

        QVERIFY2( allPointLabels( &m ).isEmpty(), "benzene has no stereocentres" );
        // Ring double bonds must never get E/Z: the ring forces a cis geometry,
        // so a descriptor there would be meaningless.
        QVERIFY2( allBondLabels( &m ).isEmpty(),
                  "ring double bonds must not be labelled E/Z" );
    }

    // ── Tetrahedral R/S ──────────────────────────────────────────────────────
    // Lactic acid is the fixture that caught a wrong assumption: the `@`/`@@`
    // prefix does not map to a fixed CIP letter, because the letters depend on
    // the order the substituents appear in the SMILES string.  These two cases
    // are pinned to an external standard rather than to the intent of the
    // notation:
    //
    //   obabel -:"C[C@H](O)C(=O)O" -oinchi
    //   InChI=1S/C3H6O3/.../t2-/m0/s1      (identical to PubChem CID 107689,
    //                                      L-(+)-lactic acid, which is (S))
    //   obabel -:"C[C@@H](O)C(=O)O" -oinchi
    //   InChI=1S/C3H6O3/.../t2-/m1/s1      (D-lactic acid, which is (R))
    //
    // So C[C@H](O)C(=O)O is (S), not (R).  The original expectation here said
    // the opposite; it was wrong and the code was right.
    void lactic_acid_cc_at_h_is_labelled_S()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@H](O)C(=O)O" );      // (S)-lactic acid

        const QStringList labels = allPointLabels( &m );
        QVERIFY2( labels.count() == 1,
                  qPrintable( QStringLiteral( "expected exactly one stereocentre, "
                                              "got: " ) + labels.join( QLatin1Char( ',' ) ) ) );
        QCOMPARE( labels.first(), QStringLiteral( "S" ) );
    }

    // The mirror image must invert: D-lactic acid is (R).
    void lactic_acid_cc_at_at_h_is_labelled_R()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@@H](O)C(=O)O" );     // (R)-lactic acid

        const QStringList labels = allPointLabels( &m );
        QVERIFY2( labels.count() == 1,
                  qPrintable( QStringLiteral( "expected exactly one stereocentre, "
                                              "got: " ) + labels.join( QLatin1Char( ',' ) ) ) );
        QCOMPARE( labels.first(), QStringLiteral( "R" ) );
    }

    // Alanine: nitrogen instead of oxygen at the stereocentre.  Verified the
    // same way: N[C@@H](C)C(=O)O is InChI .../t2-/m0/s1, which is L-alanine,
    // and L-alanine is (S).
    void s_alanine_is_labelled_S()
    {
        Molecule m( nullptr );
        m.FromSMILES( "N[C@@H](C)C(=O)O" );

        const QStringList labels = allPointLabels( &m );
        QVERIFY2( labels.count() == 1,
                  qPrintable( QStringLiteral( "expected one stereocentre, got: "
                                              ) + labels.join( QLatin1Char( ',' ) ) ) );
        QCOMPARE( labels.first(), QStringLiteral( "S" ) );
    }

    // Two stereocentres: both must be labelled, one R and one S.
    void threonine_has_two_labelled_centres()
    {
        // (2S,3R)-threonine
        Molecule m( nullptr );
        m.FromSMILES( "C[C@@H](O)[C@H](N)C(=O)O" );

        const QStringList labels = allPointLabels( &m );
        QVERIFY2( labels.count() == 2,
                  qPrintable( QStringLiteral( "expected two stereocentres, got: "
                                              ) + labels.join( QLatin1Char( ',' ) ) ) );
        QStringList sorted = labels;
        sorted.sort();
        QCOMPARE( sorted, QStringList( { QStringLiteral( "R" ), QStringLiteral( "S" ) } ) );
    }

    // ── E/Z on non-ring double bonds ─────────────────────────────────────────
    void trans_2_butene_is_labelled_E()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C/C=C/C" );

        QCOMPARE( allBondLabels( &m ), QStringList( { QStringLiteral( "E" ) } ) );
    }

    void cis_2_butene_is_labelled_Z()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C/C=C\\C" );

        QCOMPARE( allBondLabels( &m ), QStringList( { QStringLiteral( "Z" ) } ) );
    }

    // A double bond whose ends carry identical substituents is not stereogenic.
    void terminal_alkene_has_no_ez_label()
    {
        Molecule m( nullptr );
        m.FromSMILES( "CC=C" );     // propene: CH3-CH=CH2

        QVERIFY2( allBondLabels( &m ).isEmpty(),
                  "propene has no E/Z isomerism" );
    }

    // 2-methyl-2-butene: three identical methyls on one end -> not stereogenic.
    void trisubstituted_identical_end_has_no_ez_label()
    {
        Molecule m( nullptr );
        m.FromSMILES( "CC=C(C)C" );

        QVERIFY2( allBondLabels( &m ).isEmpty(),
                  "2-methyl-2-butene has a symmetric end and no E/Z" );
    }

    // ── Cache invalidation ───────────────────────────────────────────────────
    // Changing the structure must not leave stale descriptors behind.  The
    // cache is keyed on DPoint*/Bond* pointers, so a stale entry would survive
    // a coordinate change and silently mislabel the drawing.
    void clearing_the_cache_recomputes_labels()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C[C@H](O)C(=O)O" );      // (S)-lactic acid, see above

        QCOMPARE( allPointLabels( &m ), QStringList( { QStringLiteral( "S" ) } ) );

        m.ClearCIPCache();
        QCOMPARE( allPointLabels( &m ), QStringList( { QStringLiteral( "S" ) } ) );
    }

    // ── Consistency: labels must survive repeated queries ───────────────────
    // CalcCIPLabels() caches behind m_cipLabelsValid; a second call must return
    // the same answer rather than an empty map.
    void repeated_queries_are_stable()
    {
        Molecule m( nullptr );
        m.FromSMILES( "C/C=C/C" );

        const QStringList first = allBondLabels( &m );
        const QStringList second = allBondLabels( &m );
        QCOMPARE( first, second );
        QCOMPARE( first, QStringList( { QStringLiteral( "E" ) } ) );
    }

    // ── Depiction: the wedge must actually be drawn ──────────────────────────
    // A descriptor that disagrees with the picture is worse than no descriptor,
    // so the stereo configuration has to reach the renderer, not just the label
    // map.  XDrawChem draws a wedge from Bond::order 5 and a hash from 7.
    void chirality_from_smiles_is_drawn_as_a_stereo_bond()
    {
        Molecule up( nullptr );
        up.FromSMILES( "C[C@H](O)C(=O)O" );
        int wedges = 0, hashes = 0;
        for ( Bond *b : up.getBonds() ) {
            if ( b->Order() == 5 ) ++wedges;
            if ( b->Order() == 7 ) ++hashes;
        }
        QCOMPARE( wedges + hashes, 1 );

        // The enantiomer must be drawn the other way round.
        Molecule down( nullptr );
        down.FromSMILES( "C[C@@H](O)C(=O)O" );
        int wedges2 = 0, hashes2 = 0;
        for ( Bond *b : down.getBonds() ) {
            if ( b->Order() == 5 ) ++wedges2;
            if ( b->Order() == 7 ) ++hashes2;
        }
        QCOMPARE( wedges2 + hashes2, 1 );
        QVERIFY2( wedges != wedges2,
                  "the two enantiomers must not be drawn identically" );
    }

    // ── Molfile path ────────────────────────────────────────────────────────
    // The wedge-to-Bond::stereo bridge in convertFromOBMol() and the bond-order
    // normalisation in convertToOBMol() have to keep working together.  This is
    // the (S)-lactic acid depiction Open Babel writes for C[C@H](O)C(=O)O,
    // with the wedge on bond 2-1.
    void molfile_wedge_is_labelled_S()
    {
        const QString molblock = QStringLiteral(
            "\n"                                     // molecule name (empty)
            " OpenBabel11112622262D\n"                 // program line
            "\n"                                     // comment line
            "  6  5  0  0  1  0  0  0  0  0999 V2000\n"
            "    1.7321    1.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
            "    0.8660    1.5000    0.0000 C   0  0  2  0  0  0  0  0  0  0  0  0\n"
            "    0.8660    2.5000    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
            "    0.0000    1.0000    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
            "   -0.8660    1.5000    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
            "   -0.0000    0.0000    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
            "  2  1  1  1  0  0  0\n"
            "  2  3  1  0  0  0  0\n"
            "  2  4  1  0  0  0  0\n"
            "  4  5  2  0  0  0  0\n"
            "  4  6  1  0  0  0  0\n"
            "M  END\n" );

        Molecule m( nullptr );
        QVERIFY2( m.FromMolfile( molblock ), "molfile should parse" );

        const QStringList labels = allPointLabels( &m );
        QVERIFY2( labels.count() == 1,
                  qPrintable( QStringLiteral( "expected one stereocentre, got: " )
                              + labels.join( QLatin1Char( ',' ) ) ) );
        QCOMPARE( labels.first(), QStringLiteral( "S" ) );
    }
};

QTEST_MAIN( TestCip )
#include "tst_cip.moc"
