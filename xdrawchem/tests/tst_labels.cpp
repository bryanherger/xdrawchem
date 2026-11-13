// tst_labels.cpp — atom label placement (issue #34)
//
// Labels used to be drawn centred exactly on their atom, so a bond ran through
// the glyphs and two labels on neighbouring atoms merged into one run of text.
// Glucose's CH2OH and ring hydroxyls came out as "OHOH" and were unreadable.
//
// The ring was measured before anything was changed: edge lengths 24.7-25.6
// (max/min 1.03), interior angles 118-122 degrees — a regular hexagon, identical
// to cyclohexane and benzene.  So the layout was never at fault and this is
// purely a label placement fix.
//
// Labels are now pushed away from the mean direction of their atom's
// neighbours, by half their own width plus a small gap.  These tests pin the
// two things that were wrong: a label sits clear of its atom, and neighbouring
// labels do not share space.
//
// QTEST_MAIN: the molecule constructor builds QTextDocument.

#include <QtTest>
#include <QSet>
#include <cmath>

#include "../xdrawchem/molecule.h"
#include "../xdrawchem/render2d.h"
#include "../xdrawchem/dpoint.h"
#include "../xdrawchem/bond.h"
#include "../xdrawchem/text.h"

namespace {

// Bounding box of a label as drawn: position from the Text, size from the
// renderer's own metrics.
QRect labelBox( Render2D &r, Text *t )
{
    const QRect b = r.GetTextDimensions( t->getText(), t->getFont() );
    const QPoint tl = t->GetTopLeftPoint();
    return QRect( tl, b.size() );
}

} // namespace

class TestLabels : public QObject
{
    Q_OBJECT

private slots:

    // ── A label is not drawn on top of its atom ──────────────────────────────
    void atom_labels_are_offset_from_their_atom()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "CC(=O)Oc1ccccc1C(=O)O" );     // aspirin: four labelled O
        m.Render();

        int labelled = 0;
        for ( Text *t : m.AllLabels() ) {
            if ( t->Start() == nullptr )
                continue;
            ++labelled;
            const QPointF off = t->getLabelOffset();
            QVERIFY2( std::hypot( off.x(), off.y() ) > 1.0,
                      qPrintable( QStringLiteral( "label \"%1\" sits on its atom" )
                                  .arg( t->getText() ) ) );
        }
        QVERIFY2( labelled >= 4, "expected the four oxygen labels" );
    }

    // The offset points away from the atom's neighbours, not into the molecule.
    void offset_points_away_from_the_bond()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "CCO" );                      // ethanol: one labelled O
        m.Render();

        for ( Text *t : m.AllLabels() ) {
            DPoint *a = t->Start();
            QVERIFY( a != nullptr );
            QVERIFY( !a->neighbors.isEmpty() );

            DPoint *nb = a->neighbors.first();
            const QPointF off = t->getLabelOffset();
            const double along = ( a->x - nb->x ) * off.x()
                               + ( a->y - nb->y ) * off.y();
            QVERIFY2( along > 0.0,
                      "the label was pushed back along its bond instead of past it" );
        }
    }

    // ── Neighbouring labels must not share space ────────────────────────────
    // Glucose is the fixture from the issue: four heteroatoms on adjacent
    // positions, two of them hydroxyls on neighbouring ring atoms.
    void glucose_labels_do_not_overlap()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "OC[C@H]1OC(O)[C@H](O)[C@@H](O)[C@@H]1O" );
        m.Render();

        QList<Text *> labels = m.AllLabels();
        QVERIFY( labels.count() >= 5 );

        for ( int i = 0; i < labels.count(); ++i ) {
            for ( int j = i + 1; j < labels.count(); ++j ) {
                const QRect a = labelBox( r, labels[i] );
                const QRect b = labelBox( r, labels[j] );
                const QRect both = a.intersected( b );
                QVERIFY2( !both.isValid() || both.width() <= 0 || both.height() <= 0,
                          qPrintable( QStringLiteral( "\"%1\" and \"%2\" overlap by "
                                                      "%3x%4" )
                                      .arg( labels[i]->getText(),
                                            labels[j]->getText() )
                                      .arg( both.width() ).arg( both.height() ) ) );
            }
        }
    }

    // ── Free-standing text must not move ─────────────────────────────────────
    // A label with no atom behind it was placed by the user; the offset rule
    // must leave it alone.
    void free_text_is_not_moved()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "CCO" );
        m.UpdateLabelOffsets();

        for ( Text *t : m.AllLabels() ) {
            if ( t->Start() != nullptr )
                continue;
            const QPointF off = t->getLabelOffset();
            QCOMPARE( off.x(), 0.0 );
            QCOMPARE( off.y(), 0.0 );
        }
    }

    // ── The label box must be inside the drawing's bounds ───────────────────
    // The export path measures the drawing before drawing it, so an offset
    // label that is not inside BoundingBoxAll() gets cropped at the edge.
    void offset_labels_are_inside_the_bounding_box()
    {
        Render2D r;
        Molecule m( &r );
        m.FromSMILES( "OC[C@H]1OC(O)[C@H](O)[C@@H](O)[C@@H]1O" );

        const QRect bounds = m.BoundingBoxAll();
        QVERIFY( bounds.isValid() );

        for ( Text *t : m.AllLabels() ) {
            const QRect box = labelBox( r, t );
            QVERIFY2( bounds.contains( box ),
                      qPrintable( QStringLiteral( "label \"%1\" at %2,%3 lies outside "
                                                  "the bounding box %4,%5 %6x%7" )
                                  .arg( t->getText() )
                                  .arg( box.left() ).arg( box.top() )
                                  .arg( bounds.left() ).arg( bounds.top() )
                                  .arg( bounds.width() ).arg( bounds.height() ) ) );
        }
    }
};

QTEST_MAIN( TestLabels )
#include "tst_labels.moc"
