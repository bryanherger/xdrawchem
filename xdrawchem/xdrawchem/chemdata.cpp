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

#include "xdc_logging.h"

ChemData::ChemData( QObject *parent )
    : QObject( parent )
{
    thick_kludge = -1;
    notSaved = false;
}

void ChemData::drawAll()
{
    // draw all objects in ChemData
    for (Drawable *tmp_draw : drawlist) {
        tmp_draw->Render();
    }
}

// update Molecules after move
void ChemData::FinishMove()
{
    Molecule *tmp_mol;

    for (Drawable *tmp_draw : drawlist) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            tmp_mol->Changed();
        }
    }

    notSaved = true;
    emit SignalMoleculeChanged();
}

Molecule *ChemData::firstMolecule()
{
    Molecule *tmp_mol;

    for (Drawable *tmp_draw : drawlist) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            return tmp_mol;
        }
    }
    return 0;
}

void ChemData::refreshMolecules()
{
    for ( Drawable *tmp_draw : drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            ( (Molecule *) tmp_draw )->Changed();
        }
    }
}

// Returns the first molecule containing a highlighted (selected) atom.
// Falls back to firstMolecule() if nothing is highlighted.
Molecule *ChemData::highlightedMolecule()
{
    for (Drawable *tmp_draw : drawlist) {
        if ( tmp_draw->Type() != TYPE_MOLECULE )
            continue;
        if ( tmp_draw->Highlighted() ) {
            return static_cast<Molecule *>( tmp_draw );
        }
    }
    return firstMolecule();
}


void ChemData::addMolecule( Molecule * m1 )
{
    drawlist.append( m1 );
    notSaved = true;
    emit SignalMoleculeChanged();
}

Molecule* ChemData::findMolecule(DPoint* p) {
    for(int i=0; i < drawlist.size(); i++) {
        if(drawlist[i]->Find(p) && drawlist[i]->Type() == TYPE_MOLECULE) {
            return (Molecule*) drawlist[i];
        }
    }

    return NULL;
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
    emit SignalMoleculeChanged();
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
    qCDebug(lcChemData) << "addText";
    if ( t->Justify() == JUSTIFY_TOPLEFT ) {  // add to drawing
        drawlist.append( t );
    } else {                    // add label to specific Molecule
        for (Drawable *tmp_draw : drawlist) {
            if ( tmp_draw->Find( t->Start() ) == true ) {
                Molecule *tm = ( Molecule * ) tmp_draw; // this is cheating, I know!

                tm->addText( t );
                emit SignalMoleculeChanged();
                return;
            }
        }
        qCWarning(lcChemData) << "FYI, add text failed";
    }
    notSaved = true;
    emit SignalMoleculeChanged();
}

void ChemData::addGraphicObject( GraphicObject * t )
{
    drawlist.append( t );
    notSaved = true;
    emit SignalMoleculeChanged();
}

void ChemData::addBond( DPoint * s, DPoint * e, int thick, int order, QColor c, bool hl )
{
    //qInfo() << "Request to add bond:" << s->element << "(" << s->x << "," << s->y << ")-" << e->element << "(" << e->x << "," << e->y << "), order " << order;
    Drawable *m1 = nullptr, *m2 = 0;

    for (Drawable *tmp_draw : drawlist) {
        if ( tmp_draw->Find( s ) == true )
            m1 = tmp_draw;
        if ( tmp_draw->Find( e ) == true )
            m2 = tmp_draw;
    }
    //qInfo() << "m1 = " << (m1 == 0 ? -1 : m1->Type()) << ", m2 = " << (m2 == 0 ? -1 : m2->Type());
    // neither point exists -- create new Molecule
    if ( ( m1 == 0 ) && ( m2 == 0 ) ) {
        //qInfo() << "neither point exists, create new Molecule";
        Molecule *m = new Molecule( r );
        m->SetChemdata( this );
        m->addBond( s, e, thick, order, c, hl );
        drawlist.append( m );
        notSaved = true;
        emit SignalMoleculeChanged();
        return;
    }
    // one point exists, or both in same molecule
    if ( ( m1 == 0 ) && ( m2 != 0 ) ) {
        m1 = m2;
        m2 = 0;
    }
    if ( ( ( m1 != 0 ) && ( m2 == 0 ) ) || ( m1 == m2 ) ) {
        //qInfo() << "one point exists, or both in same molecule";
        m1->addBond( s, e, thick, order, c, hl );
        notSaved = true;
        emit SignalMoleculeChanged();
        return;
    }
    // both points exist in different molecules
    if ( m1 != m2 ) {
        //qInfo() << "both points exist in different molecules";
        m1->addBond( s, e, thick, order, c, hl );
        m1->addMolecule( m2 ); // before or after addBond?
        drawlist.removeAll( m2 );
        delete m2;
    }
    notSaved = true;
    emit SignalMoleculeChanged();
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
    for (Drawable *tmp_draw : drawlist) {
        if ( ( tmp_draw->Find( a ) == true ) && ( tmp_draw->Type() == TYPE_MOLECULE ) ) {
            m1 = ( Molecule * ) tmp_draw;
            m1->addSymbol( s1 );
            emit SignalMoleculeChanged();
            return;
        }
    }
    drawlist.append( s1 );
    notSaved = true;
    emit SignalMoleculeChanged();
}

Molecule *ChemData::insideMolecule( DPoint * t1 )
{
    Molecule *m1;

    //qDebug() << t1->x << "," << t1->y;
    for (Drawable *tmp_draw : drawlist) {
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
    DPoint *nearest = nullptr, *d1;
    double mindist = 9999.0, d1dist = 999999.0;

    for (Drawable *tmp_draw : drawlist) {
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
    Drawable *nearest = nullptr, *d1;
    double mindist = 2000.0, d1dist = 999999.0;

    for (Drawable *tmp_draw : drawlist) {
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
        for (Drawable *tmp_draw : drawlist) {
            erased = tmp_draw->Erase( d );
            // collect empty Molecules for removal
            if ( tmp_draw->Members() == 0 )
                removelist.append( tmp_draw );
            qCDebug(lcChemData) << "erased:" << erased;
            if ( erased == true )
                break;          //should only be one instance of d to remove!
        }
    } else {                    // drawlist.remove(d) == true
        delete d;
    }
    // remove empty Molecules
    for (Drawable *tmp_draw : removelist) {
        drawlist.removeAll( tmp_draw );
        delete tmp_draw;
    }
    // Split Molecules as needed
    DetectSplit();
    notSaved = true;
    emit SignalMoleculeChanged();
}

void ChemData::EraseSelected()
{
    qDebug() << "CIP: ChemData::EraseSelected() called";
    Molecule *m;

    QList < Drawable * >removelist;

    for (Drawable *tmp_draw : drawlist) {
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
    for (Drawable *tmp_draw : removelist) {
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
    emit SignalMoleculeChanged();
}

// Split Molecule's which hold multiple structures (e.g. after delete)
void ChemData::DetectSplit()
{
    qDebug() << "CIP: DetectSplit called";
    QList< Drawable *> removelist;
    QList< Molecule *> split_list;
    Molecule *tmp_mol;
    Drawable *td2;

    for (Drawable *tmp_draw : drawlist) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            split_list = tmp_mol->MakeSplit();
            if ( split_list.count() > 1 ) {
                qDebug() << "CIP: Split needed";
                removelist.append( tmp_draw );
                for (Drawable *td2 : split_list) {
                    drawlist.append( td2 );
                }
                split_list.clear();
            }
        }
    }
    // remove old Molecules
    for (Drawable *tmp_draw : removelist) {
        drawlist.removeAll( tmp_draw );
        delete tmp_draw;
    }
}

void ChemData::SelectAll()
{
    QList < DPoint * >allpts = UniquePoints();

    for (Drawable *tmp_draw : drawlist) {
        tmp_draw->SelectAll();
    }
    for (DPoint *tmp_pt : allpts) {
        tmp_pt->setHighlighted( true );
    }
    emit SignalSelectionChanged();
}

void ChemData::DeselectAll()
{
    QList < DPoint * >allpts = UniquePoints();

    for (Drawable *tmp_draw : drawlist) {
        tmp_draw->DeselectAll();
    }
    for (DPoint *tmp_pt : allpts) {
        tmp_pt->setHighlighted( false );
    }
    emit SignalSelectionChanged();
}

void ChemData::SetColorIfHighlighted( QColor c )
{
    for (Drawable *tmp_draw : drawlist)
        tmp_draw->SetColorIfHighlighted( c );
}

void ChemData::Move( double dx, double dy )
{
    for (Drawable *tmp_draw : drawlist)
        tmp_draw->Move( dx, dy );
    notSaved = true;
}

void ChemData::Resize( DPoint * d1, double dy )
{
    for (Drawable *tmp_draw : drawlist)
        tmp_draw->Resize( d1, dy );
    notSaved = true;
}

void ChemData::Rotate( DPoint * d1, double dy )
{
    for (Drawable *tmp_draw : drawlist)
        tmp_draw->Rotate( d1, dy );
    notSaved = true;
}

void ChemData::Flip( DPoint * d1, int dy )
{
    for (Drawable *tmp_draw : drawlist)
        tmp_draw->Flip( d1, dy );
    notSaved = true;
}

// Find minimum rectangle needed to enclose selection
QRect ChemData::selectionBox()
{
    int top = 99999, bottom = 0, left = 99999, right = 0;
    QRect tmprect;

    for (Drawable *tmp_draw : drawlist) {
        tmprect = tmp_draw->BoundingBox();
        qCDebug(lcChemData) << tmprect.width() << "X" << tmprect.height();
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

    for (DPoint *tmp_pt : allpts) {
        if ( n.contains( tmp_pt->toQPoint() ) == true ) {
            tmp_pt->setHighlighted( true );
        } else {
            tmp_pt->setHighlighted( false );
        }
    }

    for (Drawable *tmp_draw : drawlist) {
        tmp_draw->isWithinRect( n, shiftdown );
    }

    // Fallback: if any atom in a molecule is highlighted but no bond is,
    // select all bonds in that molecule (user wants whole-molecule select).
    // Bond::isWithinRect only highlights when BOTH endpoints are strictly
    // inside the box, which misses atoms whose bonds cross the boundary.
    for (Drawable *tmp_draw : drawlist) {
        if (tmp_draw->Type() != TYPE_MOLECULE)
            continue;
        Molecule *m = (Molecule *) tmp_draw;
        bool anyAtomHighlighted = false;
        for (DPoint *tmp_pt : m->AllPoints()) {
            if (tmp_pt->isHighlighted()) {
                anyAtomHighlighted = true;
                break;
            }
        }
        if (anyAtomHighlighted && !m->Highlighted()) {
            for (Bond *tmp_bond : m->getBonds())
                tmp_bond->Highlight(true);
        }
    }

    emit SignalSelectionChanged();
}

// Get list of unique points contained in all Molecules.
QList < DPoint * >ChemData::UniquePoints()
{
    QList < DPoint * >up, tp;

    for (Drawable *tmp_draw : drawlist) {
        tp = tmp_draw->AllPoints();
        for (DPoint *tmp_pt : tp)
            up.append( tmp_pt );
    }

    qCDebug(lcChemData) << up.count();
    return up;
}

QList < Drawable * >ChemData::UniqueObjects()
{
    QList < Drawable * >uo, to;
    Drawable *td2;

    for (Drawable *tmp_draw : drawlist) {
        to = tmp_draw->AllObjects();
        for (Drawable *td2 : to)
            uo.append( td2 );
    }

    qCDebug(lcChemData) << uo.count();
    return uo;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
