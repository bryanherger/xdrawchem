#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "curvearrow.h"
#include "bracket.h"
#include "text.h"
#include "symbol.h"
#include "biotools.h"
#include "chemdata.h"
#include "defs.h"

ChemData::ChemData( QObject *parent )
    : QObject( parent )
{
    thick_kludge = -1;
    notSaved = false;
}

void ChemData::drawAll()
{
    // draw all objects in ChemData
    foreach ( tmp_draw, drawlist ) {
        qDebug() << "drawAll: " << tmp_draw;
        tmp_draw->Render();
    }
}

// update Molecules after move
void ChemData::FinishMove()
{
    Molecule *tmp_mol;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            tmp_mol->Changed();
        }
    }

    notSaved = true;
}

Molecule *ChemData::firstMolecule()
{
    Molecule *tmp_mol;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            return tmp_mol;
        }
    }
    return 0;
}

void ChemData::addMolecule( Molecule * m1 )
{
    drawlist.append( m1 );
    notSaved = true;
}

void ChemData::addArrow( DPoint * s, DPoint * e, QColor c, int t, int p2, bool hl )
{
    Arrow *a1 = new Arrow( r );

    a1->setPoints( s, e );
    a1->SetColor( c );
    a1->SetStyle( t );
    a1->setThick( p2 );
    if ( hl )
        a1->Highlight( true );
    drawlist.append( a1 );
    notSaved = true;
}

void ChemData::addCurveArrow( DPoint * s, DPoint * e, QColor c, QString s1, bool hl )
{
    CurveArrow *a1 = new CurveArrow( r );

    a1->setPoints( s, e );
    a1->SetColor( c );
    a1->SetCurve( s1 );
    if ( hl )
        a1->Highlight( true );
    drawlist.append( a1 );
    notSaved = true;
}

void ChemData::addBracket( DPoint * s, DPoint * e, QColor c, int type, bool hl )
{
    Bracket *a1 = new Bracket( r );

    a1->setPoints( s, e );
    a1->SetColor( c );
    a1->SetStyle( type );
    if ( hl )
        a1->Highlight( true );
    drawlist.append( a1 );
    notSaved = true;
}

void ChemData::addText( Text * t )
{
    qDebug() << "addText";
    if ( t->Justify() == JUSTIFY_TOPLEFT ) {  // add to drawing
        drawlist.append( t );
    } else {                    // add label to specific Molecule
        foreach ( tmp_draw, drawlist ) {
            if ( tmp_draw->Find( t->Start() ) == true ) {
                Molecule *tm = ( Molecule * ) tmp_draw; // this is cheating, I know!

                tm->addText( t );
                return;
            }
        }
        qDebug() << "FYI, add text failed";
    }
    notSaved = true;
}

void ChemData::addGraphicObject( GraphicObject * t )
{
    drawlist.append( t );
    notSaved = true;
}

void ChemData::addBond( DPoint * s, DPoint * e, int thick, int order, QColor c, bool hl )
{
    //qDebug() << "Request to add bond:" ;
    //qDebug() << "(" << s->x << "," << s->y << ")-(" << e->x << "," << e->y << ")";
    //qDebug() ;
    Drawable *m1 = 0, *m2 = 0;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Find( s ) == true )
            m1 = tmp_draw;
        if ( tmp_draw->Find( e ) == true )
            m2 = tmp_draw;
    }
    // neither point exists -- create new Molecule
    if ( ( m1 == 0 ) && ( m2 == 0 ) ) {
        Molecule *m = new Molecule( r );

        m->SetChemdata( this );
        m->addBond( s, e, thick, order, c, hl );
        drawlist.append( m );
        notSaved = true;
        return;
    }
    // one point exists, or both in same molecule
    if ( ( m1 == 0 ) && ( m2 != 0 ) ) {
        m1 = m2;
        m2 = 0;
    }
    if ( ( ( m1 != 0 ) && ( m2 == 0 ) ) || ( m1 == m2 ) ) {
        m1->addBond( s, e, thick, order, c, hl );
        notSaved = true;
        return;
    }
    // both points exist in different molecules
    if ( m1 != m2 ) {
        m1->addBond( s, e, thick, order, c, hl );
        m1->addMolecule( m2 );
        drawlist.removeAll( m2 );
        delete m2;
    }
    notSaved = true;
}

void ChemData::addSymbol( DPoint * a, QString symbolfile, bool hl )
{
    Molecule *m1;
    Symbol *s1 = new Symbol( r );

    s1->setPoint( a );
    s1->SetSymbol( symbolfile );
    if ( hl )
        s1->Highlight( true );
    // determine whether point exists or not; if exists, add to Molecule
    foreach ( tmp_draw, drawlist ) {
        if ( ( tmp_draw->Find( a ) == true ) && ( tmp_draw->Type() == TYPE_MOLECULE ) ) {
            m1 = ( Molecule * ) tmp_draw;
            m1->addSymbol( s1 );
            return;
        }
    }
    drawlist.append( s1 );
    notSaved = true;
}

Molecule *ChemData::insideMolecule( DPoint * t1 )
{
    Molecule *m1;

    //qDebug() << t1->x << "," << t1->y;
    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            m1 = ( Molecule * ) tmp_draw;
            //QRect tr1 = m1->BoundingBoxAll();
            //qDebug() << tr1.left() << "," << tr1.top() << ";";
            //qDebug() << tr1.right() << "," << tr1.bottom();
            if ( m1->BoundingBoxAll().contains( t1->toQPoint(), false ) )
                return m1;
        }
    }
    return 0;
}

DPoint *ChemData::FindNearestPoint( DPoint * target, double &dist )
{
    DPoint *nearest = 0, *d1;
    double mindist = 9999.0, d1dist = 999999.0;

    foreach ( tmp_draw, drawlist ) {
        d1 = tmp_draw->FindNearestPoint( target, d1dist );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = d1;
        }
    }
    dist = mindist;
    return nearest;
}

Drawable *ChemData::FindNearestObject( DPoint * target, double &dist )
{
    Drawable *nearest = 0, *d1;
    double mindist = 2000.0, d1dist = 999999.0;

    foreach ( tmp_draw, drawlist ) {
        d1 = tmp_draw->FindNearestObject( target, d1dist );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = d1;
        }
    }
    dist = mindist;
    return nearest;
}

void ChemData::Erase( Drawable * d )
{
    QList<Drawable *> removelist;
    bool erased = false;

    if ( drawlist.removeAll( d ) == false ) {
        foreach ( tmp_draw, drawlist ) {
            erased = tmp_draw->Erase( d );
            // collect empty Molecules for removal
            if ( tmp_draw->Members() == 0 )
                removelist.append( tmp_draw );
            qDebug() << "erased:" << erased;
            if ( erased == true )
                break;          //should only be one instance of d to remove!
        }
    } else {                    // drawlist.remove(d) == true
        delete d;
    }
    // remove empty Molecules
    foreach ( tmp_draw, removelist ) {
        drawlist.removeAll( tmp_draw );
        delete tmp_draw;
    }
    // Split Molecules as needed
    DetectSplit();
    notSaved = true;
}

void ChemData::EraseSelected()
{
    Molecule *m;

    QList < Drawable * >removelist;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            m = ( Molecule * ) tmp_draw;
            m->EraseSelected();
            // collect empty Molecules for removal
            if ( tmp_draw->Members() == 0 )
                removelist.append( tmp_draw );
        } else {
            if ( tmp_draw->Highlighted() == true ) {
                removelist.append( tmp_draw );
            }
        }
    }
    foreach ( tmp_draw, removelist ) {
        /*
           if (tmp_draw->Type() == TYPE_TEXT) {
           Text *tmp_text = (Text *)tmp_draw;
           if (tmp_text->getDataType() == TEXT_DATA_MW) {
           tmp_text->getMolecule()->MWLabelDeleted();
           }
           if (tmp_text->getDataType() == TEXT_DATA_FORMULA) {
           tmp_text->getMolecule()->FormulaLabelDeleted();
           }
           }
         */
        drawlist.removeAll( tmp_draw );
        delete tmp_draw;
    }
    // Split Molecules as needed
    DetectSplit();
    notSaved = true;
}

// Split Molecule's which hold multiple structures (e.g. after delete)
void ChemData::DetectSplit()
{
    QList< Drawable *> removelist;
    QList< Molecule *> split_list;
    Molecule *tmp_mol;
    Drawable *td2;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            split_list = tmp_mol->MakeSplit();
            if ( split_list.count() > 1 ) {
                qDebug() << "Split needed";
                removelist.append( tmp_draw );
                foreach ( td2, split_list ) {
                    drawlist.append( td2 );
                }
                split_list.clear();
            }
        }
    }
    // remove old Molecules
    foreach ( tmp_draw, removelist ) {
        drawlist.removeAll( tmp_draw );
        delete tmp_draw;
    }
}

void ChemData::SelectAll()
{
    QList < DPoint * >allpts = UniquePoints();

    foreach ( tmp_draw, drawlist ) {
        tmp_draw->SelectAll();
    }
    foreach ( tmp_pt, allpts ) {
        tmp_pt->setHighlighted( true );
    }
}

void ChemData::DeselectAll()
{
    QList < DPoint * >allpts = UniquePoints();

    foreach ( tmp_draw, drawlist ) {
        tmp_draw->DeselectAll();
    }
    foreach ( tmp_pt, allpts ) {
        tmp_pt->setHighlighted( false );
    }
}

void ChemData::SetColorIfHighlighted( QColor c )
{
    foreach ( tmp_draw, drawlist )
        tmp_draw->SetColorIfHighlighted( c );
}

void ChemData::Move( double dx, double dy )
{
    foreach ( tmp_draw, drawlist )
        tmp_draw->Move( dx, dy );
    notSaved = true;
}

void ChemData::Resize( DPoint * d1, double dy )
{
    foreach ( tmp_draw, drawlist )
        tmp_draw->Resize( d1, dy );
    notSaved = true;
}

void ChemData::Rotate( DPoint * d1, double dy )
{
    foreach ( tmp_draw, drawlist )
        tmp_draw->Rotate( d1, dy );
    notSaved = true;
}

void ChemData::Flip( DPoint * d1, int dy )
{
    foreach ( tmp_draw, drawlist )
        tmp_draw->Flip( d1, dy );
    notSaved = true;
}

// Find minimum rectangle needed to enclose selection
QRect ChemData::selectionBox()
{
    int top = 99999, bottom = 0, left = 99999, right = 0;
    QRect tmprect;

    foreach ( tmp_draw, drawlist ) {
        tmprect = tmp_draw->BoundingBox();
        qDebug() << tmprect.width() << "X" << tmprect.height();
        if ( tmprect.isValid() ) {
            if ( tmprect.left() < left )
                left = tmprect.left();
            if ( tmprect.right() > right )
                right = tmprect.right();
            if ( tmprect.top() < top )
                top = tmprect.top();
            if ( tmprect.bottom() > bottom )
                bottom = tmprect.bottom();
        }
    }

    left -= 3;
    right += 5;
    top -= 3;
    bottom += 3;
    if ( left < 0 )
        left = 0;
    if ( top < 0 )
        top = 0;
    return QRect( QPoint( left, top ), QPoint( right, bottom ) );
}

// when doing multiple selection via MODE_SELECT_MULTIPLE, we will
// have to highlight/unhighlight regions of the drawing as the selection
// box changes.  This function is called to start checking whether objects
// fall within the select box.
void ChemData::NewSelectRect( QRect n, bool shiftdown )
{
    QList < DPoint * >allpts = UniquePoints();

    foreach ( tmp_pt, allpts ) {
        if ( n.contains( tmp_pt->toQPoint() ) == true ) {
            tmp_pt->setHighlighted( true );
        } else {
            tmp_pt->setHighlighted( false );
        }
    }

    foreach ( tmp_draw, drawlist ) {
        tmp_draw->isWithinRect( n, shiftdown );
    }
}

// Get list of unique points contained in all Molecules.
QList < DPoint * >ChemData::UniquePoints()
{
    QList < DPoint * >up, tp;

    foreach ( tmp_draw, drawlist ) {
        tp = tmp_draw->AllPoints();
        foreach ( tmp_pt, tp )
            up.append( tmp_pt );
    }

    qDebug() << up.count();
    return up;
}

QList < Drawable * >ChemData::UniqueObjects()
{
    QList < Drawable * >uo, to;
    Drawable *td2;

    foreach ( tmp_draw, drawlist ) {
        to = tmp_draw->AllObjects();
        foreach ( td2, to )
            uo.append( td2 );
    }

    qDebug() << uo.count();
    return uo;
}

//cmake#include "chemdata.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
