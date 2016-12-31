// molecule.cpp - Molecule's implementation of functions

#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "text.h"
#include "dpoint.h"
#include "defs.h"

Molecule::Molecule( Render2D * r1, QObject * parent )
    : Drawable( parent )
{
    r = r1;
    text_mw = 0;
    text_formula = 0;
    group_type = GROUP_NONE;
    showPC = false;
    //peaklist.setAutoDelete(true);
    //bonds.setAutoDelete(true);
    //labels.setAutoDelete(true);
    //symbols.setAutoDelete(true);
}

// implement more cleanup here
Molecule::~Molecule()
{
    qDeleteAll( peaklist );
    peaklist.clear();
}

void Molecule::Render()
{
    /// collect all points
    AllPoints();

    /// update double bond states
    Bond *tb2;
    int nleft = 0, nright = 0;
    double ang;

    foreach ( tb2, bonds ) {
        if ( tb2->Order() == 2 ) {
            nleft = 0;
            nright = 0;
            foreach ( tmp_bond, bonds ) {
                if ( tmp_bond == tb2 )
                    continue;
                if ( tmp_bond->Find( tb2->Start() ) ) {
                    ang = Angle( tb2, tmp_bond );
                    if ( ang < 180.0 )
                        nleft++;
                    else
                        nright++;
                }
                if ( tmp_bond->Find( tb2->End() ) ) {
                    ang = Angle( tb2, tmp_bond );
                    if ( ang < 180.0 )
                        nleft++;
                    else
                        nright++;
                }
            }
            if ( nleft > nright )
                tb2->RenderSide( 0 );
            else
                tb2->RenderSide( 1 );
        }
    }
    // draw group box if needed
    if ( group_type != GROUP_NONE ) {
        QRect gb1 = BoundingBoxAll();

        gb1.setLeft( gb1.left() - 6 );
        gb1.setRight( gb1.right() + 6 );
        gb1.setBottom( gb1.bottom() + 6 );
        r->drawLine( gb1.topLeft(), gb1.bottomLeft(), 1, QColor( 0, 0, 0 ), 0 );
        r->drawLine( gb1.bottomLeft(), gb1.bottomRight(), 1, QColor( 0, 0, 0 ), 0 );
        r->drawLine( gb1.bottomRight(), gb1.topRight(), 1, QColor( 0, 0, 0 ), 0 );
        QPoint tl1( gb1.topLeft().x() + 2, gb1.topLeft().y() - 4 );
        QString label1;

        if ( group_type == GROUP_REACTANT )
            label1 = tr( "Reactant" );
        if ( group_type == GROUP_PRODUCT )
            label1 = tr( "Product" );
        r->drawString( label1, tl1, QColor( 0, 0, 0 ), QFont( "times", 8 ) );
    }

    foreach ( tmp_bond, bonds )
        tmp_bond->Render();

    foreach ( tmp_text, labels ) {
        // ignore if just "C"
        //if (tmp_text->getText() == "C") continue;
        tmp_text->Render();
    }
    CalcOffsets();
    foreach ( tmp_sym, symbols )
        tmp_sym->Render();
}

// Calculate offsets, i.e., where to put a Symbol.
void Molecule::CalcOffsets()
{
    bool top = true, bottom = true, left = true, right = true;
    DPoint *op;
    double ang;

    foreach ( tmp_sym, symbols ) {
        top = true;
        bottom = true;
        left = true;
        right = true;
        tmp_pt = tmp_sym->Start();
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) ) {
                op = tmp_bond->otherPoint( tmp_pt );
                ang = tmp_bond->getAngle( tmp_pt, op );
                if ( ( ang > 300.0 ) || ( ang < 60.0 ) )
                    right = false;
                if ( ( ang > 30.0 ) && ( ang < 150.0 ) )
                    bottom = false;
                if ( ( ang > 120.0 ) && ( ang < 240.0 ) )
                    left = false;
                if ( ( ang > 210.0 ) && ( ang < 330.0 ) )
                    top = false;
            }
        }
        if ( bottom == true ) {
            tmp_sym->SetOffset( QPoint( 0, 12 ) );
            tmp_sym->SetRotate( 0.0 );
            continue;
        }
        if ( top == true ) {
            tmp_sym->SetOffset( QPoint( 0, -12 ) );
            tmp_sym->SetRotate( 180.0 );
            continue;
        }
        if ( right == true ) {
            tmp_sym->SetOffset( QPoint( 12, 0 ) );
            tmp_sym->SetRotate( -90.0 );
            continue;
        }
        if ( left == true ) {
            tmp_sym->SetOffset( QPoint( -12, 0 ) );
            tmp_sym->SetRotate( 90.0 );
            continue;
        }
        tmp_sym->SetOffset( QPoint( 0, 12 ) );
    }
}

int Molecule::Type()
{
    return TYPE_MOLECULE;
}

bool Molecule::Find( DPoint * target )
{
    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Find( target ) == true )
            return true;
    }
    return false;
}

bool Molecule::isWithinRect( QRect qr, bool shiftdown )
{
    if ( shiftdown )
        qDebug() << "shiftdown";
    bool ret = false;

    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->isWithinRect( qr, false ) )
            ret = true;
    }
    if ( ( ret == true ) && ( shiftdown == true ) ) {
        qDebug() << "selectall";
        SelectAll();
        return false;
    }
    foreach ( tmp_text, labels )
        tmp_text->isWithinRect( qr, false );

    foreach ( tmp_sym, symbols )
        tmp_sym->isWithinRect( qr, false );
    return false;
}

void Molecule::SelectAll()
{
    foreach ( tmp_bond, bonds )
        tmp_bond->SelectAll();

    foreach ( tmp_text, labels )
        tmp_text->SelectAll();

    foreach ( tmp_sym, symbols )
        tmp_sym->SelectAll();
}

void Molecule::DeselectAll()
{
    foreach ( tmp_bond, bonds )
        tmp_bond->DeselectAll();

    foreach ( tmp_text, labels )
        tmp_text->DeselectAll();

    foreach ( tmp_sym, symbols )
        tmp_sym->DeselectAll();
}

void Molecule::SetColorIfHighlighted( QColor c )
{
    foreach ( tmp_bond, bonds )
        tmp_bond->SetColorIfHighlighted( c );

    foreach ( tmp_text, labels )
        tmp_text->SetColorIfHighlighted( c );

    foreach ( tmp_sym, symbols )
        tmp_sym->SetColorIfHighlighted( c );
}

// Copy Text labels into DPoint::element (needed esp. by save, CalcMW,
// NMR prediction...)
void Molecule::CopyTextToDPoint()
{
    foreach ( tmp_text, labels ) {
        tmp_pt = tmp_text->Start();
        tmp_pt->element = tmp_text->getText();
        // clear aromaticity here
        tmp_pt->aromatic = false;
    }
}

QRect Molecule::BoundingBoxAll()
{
    QRect fr;

    SelectAll();
    fr = BoundingBox();
    DeselectAll();
    return fr;
}

bool Molecule::WithinBounds( DPoint * target )
{
    QRect fr = BoundingBoxAll();

    return fr.contains( target->toQPoint() );
}

// Determine if split is necessary (e.g., if this structure contains two
// or more fragments).  Return empty list if not, return new Molecule's
// if needed
QList < Molecule * >Molecule::MakeSplit()
{
//  QList<DPoint *> up;
    QList < DPoint * >pointqueue;
    QList < Bond * >uo;
    QList < Bond * >removequeue;
    QList < Molecule * >molecules;
    Molecule *nm;
    DPoint *current;

    uo = bonds;
    // Start with first Bond...
    nm = new Molecule( r );
    nm->SetChemdata( cd );
    tmp_bond = uo.first();
    nm->addBond( tmp_bond );
    uo.removeFirst();
    pointqueue.append( tmp_bond->Start() );
    pointqueue.append( tmp_bond->End() );
    do {
        do {
            current = pointqueue.first();
            pointqueue.removeFirst();
            foreach ( tmp_bond, uo ) {
                if ( tmp_bond->Find( current ) ) {
                    pointqueue.append( tmp_bond->otherPoint( current ) );
                    nm->addBond( tmp_bond );
                    removequeue.append( tmp_bond );
                }
            }
            foreach ( tmp_bond, removequeue ) {
                uo.removeAll( tmp_bond );
            }
            removequeue.clear();
        } while ( pointqueue.count() > 0 );
        if ( uo.count() > 0 ) {
            // Still bonds left, make new Molecule
            molecules.append( nm );
            nm = new Molecule( r );
            nm->SetChemdata( cd );
            tmp_bond = uo.first();
            nm->addBond( tmp_bond );
            uo.removeFirst();
            pointqueue.append( tmp_bond->Start() );
            pointqueue.append( tmp_bond->End() );
        }
    } while ( uo.count() > 0 );
    molecules.append( nm );
    qDebug() << "Found " << molecules.count() << " distinct structures.";
    qDebug() << nm->Members();
    // Now split Text and Symbol lists
    if ( molecules.count() > 1 ) {
        foreach ( tmp_text, labels ) {
            foreach ( nm, molecules ) {
                if ( nm->Find( tmp_text->Start() ) )
                    nm->addText( tmp_text );
            }
        }
        foreach ( tmp_sym, symbols ) {
            foreach ( nm, molecules ) {
                if ( nm->Find( tmp_sym->Start() ) )
                    nm->addSymbol( tmp_sym );
            }
        }
    }
    return molecules;
}

// Create and return a list of all unique DPoint's in this Molecule
QList < DPoint * >Molecule::AllPoints()
{
    QList < DPoint * >pl;

    foreach ( tmp_bond, bonds ) {
        tmp_pt = tmp_bond->Start();
        if ( pl.contains( tmp_pt ) == 0 )
            pl.append( tmp_pt );
        tmp_pt = tmp_bond->End();
        if ( pl.contains( tmp_pt ) == 0 )
            pl.append( tmp_pt );
    }
    foreach ( tmp_text, labels ) {
        tmp_pt = tmp_text->Start();
        if ( pl.contains( tmp_pt ) == 0 )
            pl.append( tmp_pt );
    }
    // Copy Text labels to DPoint's as needed
    CopyTextToDPoint();
    // Determine number of substituents at each point
    int c1 = 0;

    foreach ( tmp_pt, pl ) {
        tmp_pt->serial = c1;    // serialize atoms
        c1++;
        tmp_pt->hit = false;    // also reset "hit"
        tmp_pt->substituents = 0;
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) )
                tmp_pt->substituents = tmp_pt->substituents + tmp_bond->Order();
        }
    }
    // serialize atoms -- copy list place to serial field of DPoint
    //for (int c1 = 0; c1 < pl.count(); c1++) {
    //  tmp_pt = pl.at(c1);
    //  tmp_pt->serial = c1;
    //  tmp_pt->hit = false;  // also reset "hit"
    //}
    //qDebug() << "AllPoints():" << pl.count() ;
    return pl;
}

// Create and return a list of all unique Drawable's in this Molecule
QList < Drawable * >Molecule::AllObjects()
{
    QList < Drawable * >dl;

    up = AllPoints();

    foreach ( tmp_bond, bonds ) {
        dl.append( ( Drawable * ) tmp_bond );
    }
    foreach ( tmp_text, labels ) {
        dl.append( ( Drawable * ) tmp_text );
    }
    foreach ( tmp_sym, symbols ) {
        dl.append( ( Drawable * ) tmp_sym );
    }
    return dl;
}

QList < Bond * >Molecule::AllBonds()
{
    // this should be safe, right?  ioiface.cpp should use this read-only...  :)
    return bonds;
}

// Create a list of unique DPoints to move
void Molecule::MakeTomoveList()
{
    tomove.clear();

    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Highlighted() ) {
            tmp_pt = tmp_bond->Start();
            if ( tomove.contains( tmp_pt ) == 0 )
                tomove.append( tmp_pt );
            tmp_pt = tmp_bond->End();
            if ( tomove.contains( tmp_pt ) == 0 )
                tomove.append( tmp_pt );
        }
    }
    foreach ( tmp_text, labels ) {
        if ( tmp_text->Highlighted() ) {
            tmp_pt = tmp_text->Start();
            if ( tomove.contains( tmp_pt ) == 0 )
                tomove.append( tmp_pt );
        }
    }
    foreach ( tmp_sym, symbols ) {
        if ( tmp_sym->Highlighted() ) {
            tmp_pt = tmp_sym->Start();
            if ( tomove.contains( tmp_pt ) == 0 )
                tomove.append( tmp_pt );
        }
    }
}

void Molecule::Move( double dx, double dy )
{
    MakeTomoveList();

    foreach ( tmp_pt, tomove ) {
        tmp_pt->x += dx;
        tmp_pt->y += dy;
    }
}

void Molecule::Rotate( DPoint * origin, double angle )
{
    MakeTomoveList();

    foreach ( tmp_pt, tomove ) {
        double thisx = tmp_pt->x - origin->x;
        double thisy = tmp_pt->y - origin->y;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        tmp_pt->x = newx + origin->x;
        tmp_pt->y = newy + origin->y;
    }
    Changed();
}

// rotate about center of molecule
void Molecule::Rotate( double angle )
{
    MakeTomoveList();

    double centerx = 0.0, centery = 0.0;
    int n = 0;

    foreach ( tmp_pt, tomove ) {
        centerx += tmp_pt->x;
        centery += tmp_pt->y;
        n++;
    }
    centerx /= ( double ) n;
    centery /= ( double ) n;

    foreach ( tmp_pt, tomove ) {
        double thisx = tmp_pt->x - centerx;
        double thisy = tmp_pt->y - centery;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        tmp_pt->x = newx + centerx;
        tmp_pt->y = newy + centery;
    }
    Changed();
}

void Molecule::Flip( DPoint * origin, int direction )
{
    MakeTomoveList();
    double delta;

    foreach ( tmp_pt, tomove ) {
        if ( direction == FLIP_H ) {
            delta = tmp_pt->x - origin->x;
            tmp_pt->x = tmp_pt->x - 2 * delta;
        } else {                // direction == FLIP_V
            delta = tmp_pt->y - origin->y;
            tmp_pt->y = tmp_pt->y - 2 * delta;
        }
    }
}

void Molecule::Resize( DPoint * origin, double scale )
{
    double dx, dy;

    MakeTomoveList();

    foreach ( tmp_pt, tomove ) {
        dx = tmp_pt->x - origin->x;
        dy = tmp_pt->y - origin->y;
        dx *= scale;
        dy *= scale;
        tmp_pt->x = origin->x + dx;
        tmp_pt->y = origin->y + dy;
    }
}

QRect Molecule::BoundingBox()
{
    int top = 99999, bottom = 0, left = 99999, right = 0;
    QRect tmprect;

    foreach ( tmp_bond, bonds ) {
        tmprect = tmp_bond->BoundingBox();
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
    foreach ( tmp_text, labels ) {
        tmprect = tmp_text->BoundingBox();
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

    return QRect( QPoint( left, top ), QPoint( right, bottom ) );
}

DPoint *Molecule::FindNearestPoint( DPoint * target, double &dist )
{
    DPoint *nearest = 0, *d1;
    double mindist = 99999.0, d1dist = 999999.0;

    foreach ( tmp_bond, bonds ) {
        d1 = tmp_bond->FindNearestPoint( target, d1dist );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = d1;
        }
    }
    dist = mindist;
    if ( !nearest )
        return 0;
    nearest->other = 0;
    bool already_found = false;

    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Find( nearest ) == true ) {
            if ( already_found == true ) {
                nearest->other = 0;
                return nearest;
            } else {
                already_found = true;
                nearest->other = tmp_bond->otherPoint( nearest );
                nearest->otherBond = tmp_bond;
            }
        }
    }
    // check text objects to see if nearest point has a label
    // if it does, then don't select this point.
    //for (tmp_text=labels.first(); tmp_text != 0; tmp_text = labels.next()) {
    //if (tmp_text->Start() == nearest) dist = 9999.0;
    //}
    return nearest;
}

Drawable *Molecule::FindNearestObject( DPoint * target, double &dist )
{
    Drawable *nearest = 0, *d1;
    double mindist = 999999.0, d1dist = 999999.0;

    foreach ( tmp_bond, bonds ) {
        d1 = tmp_bond->FindNearestObject( target, d1dist );
        if ( d1dist < mindist ) {
            mindist = d1dist;
            nearest = d1;
        }
    }
    // if on Text label, highlight it...
    foreach ( tmp_text, labels ) {
        if ( tmp_text->WithinBounds( target ) ) {
            nearest = tmp_text;
            dist = 0.01;
            return nearest;
        }
    }
    // if on Symbol, highlight it...
    foreach ( tmp_sym, symbols ) {
        if ( tmp_sym->WithinBounds( target ) ) {
            nearest = tmp_sym;
            dist = 0.01;
            return nearest;
        }
    }
    dist = mindist;
    return nearest;
}

void Molecule::addBond( DPoint * s, DPoint * e, int thick, int order, QColor c, bool hl )
{
    int o, p;
    Bond *b = new Bond( r );

    b->setPoints( s, e );
    b->setThick( thick );
    if ( preferences.getBondCenter() == true ) {
        b->setDoubleBondAuto( 0 );
        b->setDoubleBondAlign( 2 );
    }
    if ( order != 99 ) {
        b->setOrder( order );
    } else {
        b->setDash( 1 );
        b->setOrder( 1 );
    }
    if ( order == 4 ) {         /// MDL molfiles store aromaticity this way
        b->setDash( 1 );
        b->setOrder( 2 );
    }
    b->SetColor( c );
    if ( hl )
        b->Highlight( true );
    /// check if bond exists before adding
    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Equals( b ) ) {
            o = tmp_bond->Order();
            p = tmp_bond->Dash();
            if ( o < 3 )
                tmp_bond->setOrder( o + 1 );
            if ( order == 99 )
                tmp_bond->setDash( p + 1 );
            Changed();
            return;
        }
    }
    bonds.append( b );
    Changed();
}

// shortcut addBond
void Molecule::addBond( Bond * b )
{
    // but only add if it's not already there.
    if ( bonds.count( b ) == 0 )
        bonds.append( b );
    Changed();
}

// add a text label
void Molecule::addText( Text * t )
{
    // remove Text associated with this point, if any
    DPoint *findpt = t->Start();

    foreach ( tmp_text, labels ) {
        if ( tmp_text->Start() == findpt ) {
            labels.removeAll( tmp_text );
            break;
        }
    }

    t->setMolecule( this );
    labels.append( t );
    Changed();
}

// add a Symbol
void Molecule::addSymbol( Symbol * s )
{
    symbols.append( s );
    Changed();
}

// add a molecule to this one.  The Drawable passed had better be a Molecule!
void Molecule::addMolecule( Drawable *m1 )
{
    qDebug() << "Uh-oh!  Need to merge";
    for ( tmp_bond = m1->bondsFirst(); tmp_bond != 0; tmp_bond = m1->bondsNext() ) {
        bonds.append( tmp_bond );
    }
    for ( tmp_text = m1->labelsFirst(); tmp_text != 0; tmp_text = m1->labelsNext() ) {
        tmp_text->setMolecule( this );
        labels.append( tmp_text );
    }
    Changed();
}

// Erase
bool Molecule::Erase( Drawable * d )
{
    int removed;
    bool retval = false;

    if ( d->Type() == TYPE_BOND ) {
        tmp_bond = ( Bond * ) d;
        if ( bonds.contains( tmp_bond ) ) {
            if ( ( tmp_bond->Order() == 2 ) || ( tmp_bond->Order() == 3 ) ) {
                tmp_bond->setOrder( tmp_bond->Order() - 1 );
                if ( tmp_bond->Dash() > 0 )
                    tmp_bond->setDash( tmp_bond->Dash() - 1 );
                Changed();
                return true;
            } else {
                removed = bonds.removeAll( tmp_bond );
                if ( removed > 0 )
                    delete d;

                retval = removed;
            }
        }
    } else {                    // deleting TEXT or SYMBOL
        if ( d->Type() == TYPE_TEXT ) {
            removed = labels.removeAll( ( Text * ) d );
            if ( removed > 0 ) {
                d->Start()->element = "C";
                delete d;
            }
            return removed;
        }
        if ( d->Type() == TYPE_SYMBOL ) {
            removed = symbols.removeAll( ( Symbol * ) d );
            if ( removed > 0 )
                delete d;

            return removed;
        }
    }
    if ( retval == false )
        return false;
    Changed();
    return true;
}

// Erase selected items
void Molecule::EraseSelected()
{
    QList < Bond * >removebonds;
    QList < Text * >removetext;
    QList < Symbol * >removesym;

    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Highlighted() == true ) {
            removebonds.append( tmp_bond );
        }
    }
    foreach ( tmp_bond, removebonds ) {
        bonds.removeAll( tmp_bond );
        delete tmp_bond;
    }
    foreach ( tmp_text, labels ) {
        if ( tmp_text->Highlighted() == true ) {
            removetext.append( tmp_text );
        }
    }
    foreach ( tmp_text, removetext ) {
        labels.removeAll( tmp_text );
        delete tmp_text;
    }
    foreach ( tmp_sym, symbols ) {
        if ( tmp_sym->Highlighted() == true ) {
            removesym.append( tmp_sym );
        }
    }
    foreach ( tmp_sym, removesym ) {
        symbols.removeAll( tmp_sym );
        delete tmp_sym;
    }

    Changed();
}

// XML (XDrawChem format) representation of this Molecule
QString Molecule::ToXML( QString xml_id )
{
    QString s( "" );

    QList < DPoint * >up;
    QList < Drawable * >uo;
    Drawable *tmp_draw;
    QString n1, nfull;
    int n = 0;

    up = AllPoints();
    uo = AllObjects();

    if ( up.count() == 0 )
        return s;               // don't write an empty file

    // Copy text from Text objects to element field in DPoint
    // also copy Symbol descriptions
    foreach ( tmp_draw, uo ) {
        tmp_draw->Start()->hit = false;
        if ( tmp_draw->Type() == TYPE_TEXT ) {
            tmp_text = ( Text * ) tmp_draw;     // is this cheating?
            tmp_text->Start()->element = tmp_text->getText();
            tmp_text->Start()->elementmask = tmp_text->getRichText();
            tmp_text->Start()->color = tmp_text->GetColor();
            tmp_text->Start()->font = tmp_text->getFont();
            tmp_text->Start()->hit = true;
        }
        if ( tmp_draw->Type() == TYPE_SYMBOL ) {
            tmp_sym = ( Symbol * ) tmp_draw;
            tmp_sym->Start()->symbol = tmp_sym->GetSymbol();
        }
    }

    // begin Molecule
    s.append( "<molecule id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    if ( group_type != GROUP_NONE ) {
        s.append( "<grouptype>" );
        if ( group_type == GROUP_REACTANT )
            s.append( "reactant" );
        if ( group_type == GROUP_PRODUCT )
            s.append( "product" );
        s.append( "</grouptype>\n" );
    }
    // Add XML ID's to DPoint's, write as we go
    foreach ( tmp_pt, up ) {
        n1.setNum( n );
        nfull = QString( "a" ) + n1;
        tmp_pt->id = nfull;
        n++;
        s.append( "<atom id=\"" );
        s.append( tmp_pt->id );
        s.append( "\">\n" );
        s.append( "<element>" );
        s.append( tmp_pt->element );
        s.append( "</element>\n" );
        s.append( "<elemask>" );
        s.append( tmp_pt->elementmask );
        s.append( "</elemask>\n" );
        if ( tmp_pt->hit )
            s.append( "<textobject>yes</textobject>\n" );
        if ( tmp_pt->symbol.length() > 1 ) {
            s.append( "<symtype>" );
            s.append( tmp_pt->symbol );
            s.append( "</symtype>\n" );
        }
        s.append( "<coordinate2>" );
        n1.setNum( tmp_pt->x );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tmp_pt->y );
        s.append( n1 );
        s.append( "</coordinate2>\n" );

        // write color
        s.append( "<color>" );
        n1.setNum( tmp_pt->color.red() );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tmp_pt->color.green() );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tmp_pt->color.blue() );
        s.append( n1 );
        s.append( "</color>\n" );

        // write font
        s.append( "<font>" );
        s.append( tmp_pt->font.family() );
        s.append( QString( "#" ) );
        n1.setNum( tmp_pt->font.pointSize() );
        s.append( n1 );
        s.append( "</font>\n" );

        s.append( "</atom>\n" );
    }

    // add XML ID's to Bond's, write as we go
    n = 0;
    foreach ( tmp_draw, uo ) {
        if ( tmp_draw->Type() == TYPE_BOND ) {
            tmp_bond = ( Bond * ) tmp_draw;     // I ask again, is this cheating?
            n1.setNum( n );
            nfull = QString( "b" ) + n1;
            tmp_draw->setID( nfull );
            n++;
            s.append( "<bond id=\"" );
            s.append( tmp_bond->getID() );
            s.append( "\">\n<Start>" );
            s.append( tmp_bond->Start()->id );
            s.append( "</Start>\n<End>" );
            s.append( tmp_bond->End()->id );
            s.append( "</End>\n" );
            s.append( "<order>" );
            n1.setNum( tmp_bond->Order() );
            s.append( n1 );
            s.append( "</order>\n" );
            s.append( "<dash>" );
            n1.setNum( tmp_bond->Dash() );
            s.append( n1 );
            s.append( "</dash>\n" );
            // write thickness
            s.append( "<thick>" );
            n1.setNum( tmp_bond->Thick() );
            s.append( n1 );
            s.append( "</thick>\n" );
            // write color
            s.append( "<color>" );
            n1.setNum( tmp_bond->GetColor().red() );
            s.append( n1 );
            s.append( " " );
            n1.setNum( tmp_bond->GetColor().green() );
            s.append( n1 );
            s.append( " " );
            n1.setNum( tmp_bond->GetColor().blue() );
            s.append( n1 );
            s.append( "</color>\n" );
            s.append( "</bond>\n" );
        }
    }

    s.append( "</molecule>\n" );
    return s;
}

// XML (ChemDraw(TM) format) representation of this Molecule
QString Molecule::ToCDXML( QString xml_id )
{
    QString s( "" );

    QList < DPoint * >up;
    QList < Drawable * >uo;
    Drawable *tmp_draw;
    QString n1, nfull;
    int n = xml_id.toInt() + 1;

    up = AllPoints();
    uo = AllObjects();

    if ( up.count() == 0 )
        return s;               // don't write an empty file

    // also copy Symbol descriptions
    foreach ( tmp_draw, uo ) {
        if ( tmp_draw->Type() == TYPE_SYMBOL ) {
            tmp_sym = ( Symbol * ) tmp_draw;
            tmp_sym->Start()->symbol = tmp_sym->GetSymbol();
        }
    }

    // begin Molecule
    s.append( "<fragment id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    // Add XML ID's to DPoint's, write as we go
    foreach ( tmp_pt, up ) {
        n1.setNum( n );
        tmp_pt->id = n1;
        n++;
        s.append( "<n id=\"" );
        s.append( tmp_pt->id );
        s.append( "\" p=\"" );
        n1.setNum( tmp_pt->x );
        s.append( n1 );
        s.append( " " );
        n1.setNum( tmp_pt->y );
        s.append( n1 );
        s.append( "\"" );
        if ( tmp_pt->element == "C" ) {
            s.append( "/>\n" );
        } else {
            s.append( "><t><s font=\"21\" size=\"10\" face=\"96\">" );
            s.append( tmp_pt->element );
            s.append( "</s></t></n>\n" );
        }
    }

    // add XML ID's to Bond's, write as we go
    foreach ( tmp_draw, uo ) {
        if ( tmp_draw->Type() == TYPE_BOND ) {
            tmp_bond = ( Bond * ) tmp_draw;     // I ask again, is this cheating?
            n1.setNum( n );
            tmp_draw->setID( n1 );
            n++;
            s.append( "<b id=\"" );
            s.append( tmp_bond->getID() );
            s.append( "\" B=\"" );
            s.append( tmp_bond->Start()->id );
            s.append( "\" E=\"" );
            s.append( tmp_bond->End()->id );
            if ( tmp_bond->Order() < 4 ) {
                s.append( "\" Order=\"" );
                n1.setNum( tmp_bond->Order() );
                s.append( n1 );
            }
            if ( tmp_bond->Order() == 5 ) {
                s.append( "\" Display=\"WedgeBegin" );
            }
            if ( tmp_bond->Order() == 7 ) {
                s.append( "\" Display=\"WedgedHashBegin" );
            }
            s.append( "\"/>\n" );
        }
    }

    s.append( "</fragment>\n" );
    return s;
}

// Generate MDL Molfile to export to generate SMILES, export for 3-D
QString Molecule::ToMDLMolfile( int coords )
{
  QTextDocument localtextdocument;
    QList < DPoint * >up;
//   Q3PtrList<Drawable> uo;
    Bond *tmp_bond;
    QString tmpline;
    int acount = 0, bcount = 0;

    /// get all unique points and objects
    up = AllPoints();

    QString molfile;

    QTextStream t( &molfile, QIODevice::WriteOnly );

    /// write first three lines -- blank, unless we want to add comments etc.
    t << endl << endl << endl;

    /// find counts...
    foreach ( tmp_pt, up ) {
        acount++;
    }

    bcount = bonds.count();
    // write counts line
    t.setFieldWidth( 3 );
    t << acount;
    t.setFieldWidth( 3 );
    t << bcount << "  0  0  0  0  0  0  0  0  1" << endl;

    // Write atom list
    foreach ( tmp_pt, up ) {
      localtextdocument.setHtml(tmp_pt->element);
      tmpline = localtextdocument.toPlainText();
        qInfo() << "ToMDLMolfile::tmpline = " << tmpline;
      //tmpline = tmp_pt->element;
        if ( tmpline.length() < 3 )
            tmpline.append( ' ' );
        if ( tmpline.length() < 3 )
            tmpline.append( ' ' );
        if ( coords == 0 ) {    // 2D
            t.setFieldWidth( 10 );
            t << tmp_pt->x;
            t.setFieldWidth( 10 );
            t << -tmp_pt->y << "    0.0000 " << tmpline;
        } else {                // 3D
            t.setFieldWidth( 10 );
            t << tmp_pt->x;
            t.setFieldWidth( 10 );
            t << tmp_pt->y;
            t.setFieldWidth( 10 );
            t << tmp_pt->z;
            t << tmpline;
        }
        t << " 0  0  0  0  0  0  0  0  0  0  0  0" << endl;
    }
    // write bond list
    foreach ( tmp_bond, bonds ) {
        t.setFieldWidth( 3 );
        t << up.indexOf( tmp_bond->Start() ) + 1;
        t.setFieldWidth( 3 );
        t << up.indexOf( tmp_bond->End() ) + 1;
        t.setFieldWidth( 3 );
        t << tmp_bond->Order();
        if ( tmp_bond->Order() < 4 )
            t << "  0  0  0  0";
        if ( tmp_bond->Order() == 5 )
            t << "  1  0  0  0";
        if ( tmp_bond->Order() == 7 )
            t << "  6  0  0  0";
	t << endl;
    }

    t << "M  END";

    qInfo() << "molfile: " << molfile;

    return molfile;
}

void Molecule::FromXML( QString xml_tag )
{
    QString bondtag, subtag, startid, endid;

    QList < DPoint * >points;
    DPoint *s1, *e1;
    int i1, i2;

    i1 = xml_tag.indexOf( "<grouptype>" );
    if ( i1 > 0 ) {
        subtag = xml_tag.mid( i1, 20 );
        if ( subtag.toLower().contains( "reactant" ) > 0 )
            group_type = GROUP_REACTANT;
        if ( subtag.toLower().contains( "product" ) > 0 )
            group_type = GROUP_PRODUCT;
    }
    // Extract atom's
    do {
        i1 = xml_tag.indexOf( "<atom " );
        i2 = xml_tag.indexOf( "</atom>" ) + 7;
        if ( i1 >= 0 ) {
            tmp_pt = new DPoint;
            tmp_pt->FromXML( xml_tag.mid( i1, i2 - i1 ) );
            points.append( tmp_pt );
            xml_tag.remove( i1, i2 - i1 );
        } else {
            break;
        }
    } while ( 1 );
    // Add bond's
    do {
        i1 = xml_tag.indexOf( "<bond " );
        i2 = xml_tag.indexOf( "</bond>" ) + 7;
        if ( i1 >= 0 ) {
            tmp_bond = new Bond( r );
            bondtag = xml_tag.mid( i1, i2 - i1 );
            xml_tag.remove( i1, i2 - i1 );
            qDebug() << bondtag;
            // parse Start
            i1 = bondtag.indexOf( "<Start>" );
            i2 = bondtag.indexOf( "</Start>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<Start>" );
            i2 = subtag.indexOf( "</Start>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 7 );
            foreach ( tmp_pt, points ) {
                if ( tmp_pt->id == subtag ) {
                    s1 = tmp_pt;
                    break;
                }
            }
            // parse End
            i1 = bondtag.indexOf( "<End>" );
            i2 = bondtag.indexOf( "</End>" ) + 6;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<End>" );
            i2 = subtag.indexOf( "</End>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 5 );
            foreach ( tmp_pt, points ) {
                if ( tmp_pt->id == subtag ) {
                    e1 = tmp_pt;
                    break;
                }
            }
            // add points
            tmp_bond->setPoints( s1, e1 );
            // parse Order
            i1 = bondtag.indexOf( "<order>" );
            i2 = bondtag.indexOf( "</order>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<order>" );
            i2 = subtag.indexOf( "</order>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 7 );
            tmp_bond->setOrder( subtag.toInt() );
            // parse Dash
            i1 = bondtag.indexOf( "<dash>" );
            i2 = bondtag.indexOf( "</dash>" ) + 7;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<dash>" );
            i2 = subtag.indexOf( "</dash>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 6 );
            tmp_bond->setDash( subtag.toInt() );
            // parse thickness
            i1 = bondtag.indexOf( "<thick>" );
            i2 = bondtag.indexOf( "</thick>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            i1 = subtag.indexOf( "<thick>" );
            i2 = subtag.indexOf( "</thick>" );
            subtag.remove( i2, 999 );
            subtag.remove( i1, 7 );
            tmp_bond->setThick( subtag.toInt() );
            // parse Dash
            i1 = bondtag.indexOf( "<color>" );
            i2 = bondtag.indexOf( "</color>" ) + 8;
            subtag = bondtag.mid( i1, i2 - i1 );
            tmp_bond->SetColor( GetColorFromXML( subtag ) );
            bonds.append( tmp_bond );
        } else {
            break;
        }
    } while ( 1 );
    // add Text and Symbol as needed
    foreach ( tmp_pt, points ) {
        if ( ( tmp_pt->element != QString( "C" ) ) || ( tmp_pt->hit == true ) ) {
            tmp_text = new Text( r );
            tmp_text->setPoint( tmp_pt );
            tmp_text->SetColor( tmp_pt->color );
            tmp_text->setFont( tmp_pt->font );
            tmp_text->setText( tmp_pt->elementmask );
            if ( tmp_pt->elementmask.length() > 0 ) {
//                tmp_text->setTextMask( tmp_pt->elementmask );
            } else {
                tmp_pt->element.fill( ' ' );
//                tmp_text->setTextMask( tmp_pt->element );
            }
            labels.append( tmp_text );
        }
        if ( tmp_pt->symbol.length() > 0 ) {
            tmp_sym = new Symbol( r );
            tmp_sym->setPoint( tmp_pt );
            tmp_sym->SetSymbol( tmp_pt->symbol );
            symbols.append( tmp_sym );
        }
    }
}

// update MW and formula
void Molecule::Changed()
{
    // add hydrogens and correct labels
    //qDebug() << "changed" ;
    AddHydrogens();
    return;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
