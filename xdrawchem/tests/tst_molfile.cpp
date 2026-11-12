// tst_molfile.cpp — MDL import: labels, wedges, and hydrogen handling
//
// Issue #32: a molfile rendered through Molecule::FromMolfile() came out with
// no heteroatom labels at all (a carboxylic acid looked like a hydrocarbon),
// the wedge was invisible, and files that carry explicit hydrogens rendered
// every one of them as a vertex.
//
// Three independent causes, each pinned below:
//   1. Text::setText() writes through `start`, so it must follow setPoint().
//      Called in the other order it silently does nothing.
//   2. Bond::stereo is only a CIP annotation; the renderer draws from
//      Bond::order 5 (wedge) and 7 (hash).
//   3. FromMolfile() called OpenBabel's AddHydrogens(), materialising implicit
//      hydrogens — and files from PubChem and friends already contain explicit
//      ones, which arrived as atoms too.
//
// QTEST_MAIN: the molecule constructor builds QTextDocument.

#include <QtTest>
#include <QSet>

#include "../xdrawchem/molecule.h"
#include "../xdrawchem/dpoint.h"
#include "../xdrawchem/bond.h"
#include "../xdrawchem/text.h"

namespace {

// Lactic acid, MDL V2000, implicit hydrogens, one wedge on bond 2-1.
// Same file as tests/tst_cip.cpp uses for its molfile case.
QString lacticAcidMolfile()
{
    return QStringLiteral(
        "\n"
        " OpenBabel11112622262D\n"
        "\n"
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
}

// The same compound with all six hydrogens written out explicitly, the way
// PubChem ships an SDF.  Twelve atoms, eleven bonds, one hash on 4-1.
QString lacticAcidWithExplicitH()
{
    return QStringLiteral(
        "107689\n"
        "  -OEChem-11102622062D\n"
        "\n"
        " 12 11  0     1  0  0  0  0  0999 V2000\n"
        "    2.5369    0.2500    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    5.1350   -0.2500    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    4.2690    1.2500    0.0000 O   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    3.4030   -0.2500    0.0000 C   0  0  1  0  0  0  0  0  0  0  0  0\n"
        "    3.4030   -1.2500    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    4.2690    0.2500    0.0000 C   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    3.4030    0.3700    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    2.7830   -1.2500    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    3.4030   -1.8700    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    4.0230   -1.2500    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    2.0000   -0.0600    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "    5.6720    0.0600    0.0000 H   0  0  0  0  0  0  0  0  0  0  0  0\n"
        "  4  1  1  6  0  0  0\n"
        "  1 11  1  0  0  0  0\n"
        "  2  6  1  0  0  0  0\n"
        "  2 12  1  0  0  0  0\n"
        "  3  6  2  0  0  0  0\n"
        "  4  5  1  0  0  0  0\n"
        "  4  6  1  0  0  0  0\n"
        "  4  7  1  0  0  0  0\n"
        "  5  8  1  0  0  0  0\n"
        "  5  9  1  0  0  0  0\n"
        "  5 10  1  0  0  0  0\n"
        "M  END\n" );
}

} // namespace

class TestMolfile : public QObject
{
    Q_OBJECT

private slots:

    // ── The headline defect: heteroatom labels must survive the import ──────
    void heteroatom_labels_are_created()
    {
        Molecule m( nullptr );
        QVERIFY2( m.FromMolfile( lacticAcidMolfile() ), "molfile should parse" );

        QStringList texts;
        for ( Text *t : m.AllLabels() )
            texts.append( t->getText() );

        int oxygens = 0;
        for ( const QString &s : texts )
            if ( s == QStringLiteral( "O" ) )
                ++oxygens;

        QVERIFY2( oxygens == 3,
                  qPrintable( QStringLiteral( "expected three oxygen labels, got: %1 "
                                              "(labels: %2)" )
                              .arg( oxygens )
                              .arg( texts.join( QLatin1Char( ',' ) ) ) ) );
    }

    // Text::setText() writes through `start`; with the order reversed the text
    // is empty and the label renders as nothing at all.
    void label_text_is_not_empty()
    {
        Molecule m( nullptr );
        QVERIFY( m.FromMolfile( lacticAcidMolfile() ) );

        for ( Text *t : m.AllLabels() ) {
            QVERIFY2( !t->getText().isEmpty(),
                      "a label exists but carries no text — setText() before "
                      "setPoint() silently does nothing" );
        }
    }

    // ── The wedge has to be drawn, not just stored ──────────────────────────
    void wedge_from_molfile_is_drawn()
    {
        Molecule m( nullptr );
        QVERIFY( m.FromMolfile( lacticAcidMolfile() ) );

        int stereoBonds = 0;
        for ( Bond *b : m.getBonds() )
            if ( b->Order() == 5 || b->Order() == 7 )
                ++stereoBonds;

        QCOMPARE( stereoBonds, 1 );
    }

    // ── Explicit hydrogens are dropped on import ────────────────────────────
    // A skeletal formula is the point of the notation; drawing every hydrogen
    // makes a twelve-atom file unreadable, and the SMILES path never did it.
    void explicit_hydrogens_are_dropped()
    {
        Molecule m( nullptr );
        QVERIFY( m.FromMolfile( lacticAcidWithExplicitH() ) );

        QCOMPARE( m.AllPoints().count(), 6 );

        int hydrogenLabels = 0;
        for ( Text *t : m.AllLabels() )
            if ( t->getText() == QStringLiteral( "H" ) )
                ++hydrogenLabels;
        QCOMPARE( hydrogenLabels, 0 );
    }

    // Dropping them must not lose the stereochemistry: this is (S)-lactic acid
    // and the wedge pointed at a hydrogen that has just become implicit.
    void explicit_hydrogen_file_keeps_its_descriptor()
    {
        Molecule m( nullptr );
        QVERIFY( m.FromMolfile( lacticAcidWithExplicitH() ) );

        QStringList labels;
        for ( DPoint *pt : m.AllPoints() ) {
            const QString label = m.GetCIPLabelForPoint( pt );
            if ( !label.isEmpty() )
                labels.append( label );
        }
        QCOMPARE( labels, QStringList( { QStringLiteral( "S" ) } ) );
    }

    // ── Both import routes must agree ───────────────────────────────────────
    // Same compound, one with implicit hydrogens and one with explicit; the
    // descriptors must match, since they describe the same molecule.
    void implicit_and_explicit_files_agree()
    {
        Molecule a( nullptr );
        Molecule b( nullptr );
        QVERIFY( a.FromMolfile( lacticAcidMolfile() ) );
        QVERIFY( b.FromMolfile( lacticAcidWithExplicitH() ) );

        auto firstLabel = []( Molecule &mol ) -> QString {
            for ( DPoint *pt : mol.AllPoints() ) {
                const QString label = mol.GetCIPLabelForPoint( pt );
                if ( !label.isEmpty() )
                    return label;
            }
            return QString();
        };

        QCOMPARE( firstLabel( a ), firstLabel( b ) );
        QCOMPARE( firstLabel( a ), QStringLiteral( "S" ) );
    }
};

QTEST_MAIN( TestMolfile )
#include "tst_molfile.moc"
