// bond.cpp - Bond's implementation of functions

#include "render2d.h"
#include "drawable.h"
#include "bond.h"
#include "defs.h"
#include "bondedit.h"

Bond::Bond( Render2D * r1, QObject * parent )
    : Drawable( parent )
{
    r = r1;
    order = 1;
    stereo = STEREO_UNDEFINED;
    dashed = 0;
    highlighted = false;
    thick = 1;
    wside = 0;
    auto_wside = 1;
    IR_shift = -100.0;
    IR_intensity = "NA";
    cname = "";
    rxnlist = "Run 'Tools/Reaction/Reverse reactions' first.";
}

void Bond::setOrder( int a )
{
    order = a;
    if ( order == 5 )
        stereo = STEREO_UP;
    if ( order == 7 )
        stereo = STEREO_DOWN;
    if ( order == 8 ) {
        order = 7;
        //tmp_pt = start;
        //start = end;
        //end = tmp_pt;
        stereo = STEREO_DOWN;
    }
}

bool Bond::Equals( Bond * a )
{
    if ( ( start == a->Start() ) && ( end == a->End() ) )
        return true;
    if ( ( start == a->End() ) && ( end == a->Start() ) )
        return true;
    return false;
}

bool Bond::isWithinRect( QRect n, bool /*shiftdown*/ )
{
    if ( DPointInRect( start, n ) && DPointInRect( end, n ) )
        highlighted = true;
    else
        highlighted = false;
    return highlighted;
}

void Bond::Edit()
{
    int db1 = 0;

    qDebug() << "edit bond";
    BondEditDialog be( r, start, end, TYPE_BOND, order, dashed, thick, 0, color );

    if ( auto_wside == 0 )
        db1 = wside;
    be.setDoubleBond( db1 );
    if ( !be.exec() )
        return;
    qDebug() << "change";
    color = be.Color();
    order = be.Order();
    dashed = be.Dash();
    thick = be.Thick();
    db1 = be.DoubleBond();
    if ( db1 == 0 ) {
        auto_wside = 1;
    } else {
        auto_wside = 0;
        wside = db1;
    }
}

void Bond::Render()
{
    double myangle, x1, y1, x2, y2, rise, run, lx, ly, cf = 0.02;
    QColor c1;

    if ( highlighted )
        c1 = QColor( 255, 0, 0 );
    else
        c1 = color;
    // for single bonds
    if ( order == 1 ) {
        if ( dashed > 0 )
            r->drawLine( start->toQPoint(), end->toQPoint(), thick, c1, 1 );
        else
            r->drawLine( start->toQPoint(), end->toQPoint(), thick, c1 );
        return;
    }
    // for stereo_up bonds
    if ( order == 5 ) {
        r->drawUpLine( start->toQPoint(), end->toQPoint(), c1 );
        return;
    }
    // for wavy bonds (order = 6)
    if ( order == 6 ) {
        r->drawWavyLine( start->toQPoint(), end->toQPoint(), c1 );
        return;
    }
    // for stereo_down bonds
    if ( order == 7 ) {
        r->drawDownLine( start->toQPoint(), end->toQPoint(), c1 );
        return;
    }
    // for double and triple bonds
    myangle = getAngle( start, end );
    myangle += 90.0;
    myangle = myangle * ( M_PI / 180.0 );
    //double offs = start->distanceTo(end) / 25.0;
    /*
       offs = preferences.getDoubleBondOffset();
       if (order == 3) {
       if (thick > 2) offs += 0.5;
       if (thick > 3) offs += 0.4;
       if (thick > 4) offs += 0.3;
       } else {
       if (thick > 2) offs += 0.3;
       if (thick > 4) offs += 0.3;
       }
       x2 = cos(myangle) * 3.0 * offs;
       y2 = sin(myangle) * 3.0 * offs;
       // x1, y1, 3 and 4 are for use with special case 1 below
       x1 = cos(myangle) * 2.5 * offs;
       y1 = sin(myangle) * 2.5 * offs;
     */

    /* original (does not use preferences!)
       double abl = 1.0;
       abl = (double)thick / 2.0 + 1.0;
     */

    double abl = 1.0;

    // TODO: scale according molecule size
    abl = ( double ) thick / 2.0 + preferences.getDoubleBondOffset();

    // check for order 2 special case (center double bond)
    bool special_case_1 = false;

    if ( order == 2 ) {
        //qDebug() << "start->element: " << start->element ;
        //qDebug() << "start->subst: " << start->substituents ;
        //qDebug() << "end->element: " << end->element ;
        //qDebug() << "end->subst: " << end->substituents ;
        if ( ( start->element == "O" ) && ( start->substituents == 2 ) )
            special_case_1 = true;
        if ( ( end->element == "O" ) && ( end->substituents == 2 ) )
            special_case_1 = true;
        if ( ( start->element == "C" ) && ( start->substituents == 2 ) )
            special_case_1 = true;
        if ( ( end->element == "C" ) && ( end->substituents == 2 ) )
            special_case_1 = true;
        if ( ( start->element == "CH2" ) && ( start->substituents == 2 ) )
            special_case_1 = true;
        if ( ( end->element == "CH2" ) && ( end->substituents == 2 ) )
            special_case_1 = true;
        if ( ( start->element == "N" ) && ( end->element == "N" ) )
            special_case_1 = true;
        if ( wside == 2 )       // force centered double bond
            special_case_1 = true;
    }
    if ( special_case_1 )
        abl = ( double ) thick / 2.0 + preferences.getDoubleBondOffset() / 4.0;
    else
        abl = ( double ) thick / 2.0 + preferences.getDoubleBondOffset() / 2.0;

    //qDebug() << "abl = " << abl;

    x2 = cos( myangle ) * abl;
    y2 = sin( myangle ) * abl;
    // x1, y1, 3 and 4 are for use with special case 1 below
    x1 = cos( myangle ) * abl;
    y1 = sin( myangle ) * abl;

    QPoint sp3( qRound( start->x + x1 ), qRound( start->y + y1 ) );
    QPoint ep3( qRound( end->x + x1 ), qRound( end->y + y1 ) );
    QPoint sp4( qRound( start->x - x1 ), qRound( start->y - y1 ) );
    QPoint ep4( qRound( end->x - x1 ), qRound( end->y - y1 ) );

    // shorten lines by removing from each end
    double bl = start->distanceTo( end );

    if ( bl < 100.0 )
        cf = 0.03;
    if ( bl < 51.0 )
        cf = 0.05;
    if ( bl < 27.0 )
        cf = 0.07;
    // find sp1 and ep1
    rise = ( end->y + y2 ) - ( start->y + y2 );
    run = ( end->x + x2 ) - ( start->x + x2 );
    lx = start->x + x2 + run * cf;
    ly = start->y + y2 + rise * cf;
    QPoint sp1( ( int ) lx, ( int ) ly );

    lx = start->x + x2 + run * ( 1.0 - cf );
    ly = start->y + y2 + rise * ( 1.0 - cf );
    QPoint ep1( ( int ) lx, ( int ) ly );

    // find sp2 and ep2
    rise = ( end->y - y2 ) - ( start->y - y2 );
    run = ( end->x - x2 ) - ( start->x - x2 );
    lx = start->x - x2 + run * cf;
    ly = start->y - y2 + rise * cf;
    QPoint sp2( ( int ) lx, ( int ) ly );

    lx = start->x - x2 + run * ( 1.0 - cf );
    ly = start->y - y2 + rise * ( 1.0 - cf );
    QPoint ep2( ( int ) lx, ( int ) ly );

    if ( special_case_1 == true ) {
        if ( dashed > 1 )
            r->drawLine( sp3, ep3, thick, c1, 1 );
        else
            r->drawLine( sp3, ep3, thick, c1 );
        if ( dashed > 0 )
            r->drawLine( sp4, ep4, thick, c1, 1 );
        else
            r->drawLine( sp4, ep4, thick, c1 );
        return;
    }

    QPoint sp, ep;

    if ( order == 2 ) {
        if ( wside == 1 ) {
            sp = sp1;
            ep = ep1;
        } else {
            sp = sp2;
            ep = ep2;
        }
        //qDebug() << "wside:" << wside;
        if ( dashed > 1 )
            r->drawLine( start->toQPoint(), end->toQPoint(), thick, c1, 1 );
        else
            r->drawLine( start->toQPoint(), end->toQPoint(), thick, c1 );
        if ( dashed > 0 )
            r->drawLine( sp, ep, thick, c1, 1 );
        else
            r->drawLine( sp, ep, thick, c1 );
        return;
    }
    if ( order == 3 ) {
        if ( dashed > 2 )
            r->drawLine( start->toQPoint(), end->toQPoint(), thick, c1, 1 );
        else
            r->drawLine( start->toQPoint(), end->toQPoint(), thick, c1 );
        if ( dashed > 1 )
            r->drawLine( sp1, ep1, thick, c1, 1 );
        else
            r->drawLine( sp1, ep1, thick, c1 );
        if ( dashed > 0 )
            r->drawLine( sp2, ep2, thick, c1, 1 );
        else
            r->drawLine( sp2, ep2, thick, c1 );
        return;
    }
}

int Bond::Type()
{
    return TYPE_BOND;
}

bool Bond::Find( DPoint * target )
{
    if ( start == target )
        return true;
    if ( end == target )
        return true;
    return false;
}

// You *did* call Find() first to make sure target is in this Bond...?
DPoint *Bond::otherPoint( DPoint * target )
{
    if ( target == start )
        return end;
    if ( target == end )
        return start;
    return 0;
}

DPoint *Bond::FindNearestPoint( DPoint * target, double &dist )
{
    if ( target->distanceTo( start ) < target->distanceTo( end ) ) {
        dist = target->distanceTo( start );
        return start;
    } else {
        dist = target->distanceTo( end );
        return end;
    }
}

Drawable *Bond::FindNearestObject( DPoint * target, double &dist )
{
    dist = DistanceToLine( start, end, target );
    return this;
}

void Bond::setPoints( DPoint * s, DPoint * e )
{
    start = s;
    end = e;
}

QRect Bond::BoundingBox()
{
    if ( highlighted == false )
        return QRect( QPoint( 999, 999 ), QPoint( 0, 0 ) );
    int top, bottom, left, right, swp;

    top = ( int ) start->y;
    left = ( int ) start->x;
    bottom = ( int ) end->y;
    right = ( int ) end->x;
    if ( bottom < top ) {
        swp = top;
        top = bottom;
        bottom = swp;
    }
    if ( right < left ) {
        swp = left;
        left = right;
        right = swp;
    }
    return QRect( QPoint( left, top ), QPoint( right, bottom ) );
}

double Bond::Enthalpy()
{
    double dh = 0.0;

    QString atom1, atom2, swp;

    atom1 = start->element;
    atom2 = end->element;
    if ( QString::compare( atom1, atom2 ) > 0 ) {
        swp = atom1;
        atom1 = atom2;
        atom2 = swp;
    }
    // special cases (functional groups, etc.)
    if ( atom1 == "OH" ) {
        dh += 463.0;
        atom1 = "O";
    }
    if ( atom2 == "OH" ) {
        dh += 463.0;
        atom2 = "O";
    }

    if ( ( order == 1 ) || ( order == 5 ) || ( order == 7 ) ) {
        if ( ( atom1 == "Br" ) && ( atom2 == "H" ) )
            dh = 366.0;
        if ( ( atom1 == "C" ) && ( atom2 == "Br" ) )
            dh = 276.0;
        if ( ( atom1 == "C" ) && ( atom2 == "C" ) )
            dh = 348.0;
        if ( ( atom1 == "C" ) && ( atom2 == "Cl" ) )
            dh = 328.0;
        if ( ( atom1 == "C" ) && ( atom2 == "F" ) )
            dh = 441.0;
        if ( ( atom1 == "C" ) && ( atom2 == "H" ) )
            dh = 413.0;
        if ( ( atom1 == "C" ) && ( atom2 == "I" ) )
            dh = 240.0;
        if ( ( atom1 == "C" ) && ( atom2 == "N" ) )
            dh = 292.0;
        if ( ( atom1 == "C" ) && ( atom2 == "O" ) )
            dh = 351.0;
        if ( ( atom1 == "C" ) && ( atom2 == "S" ) )
            dh = 259.0;
        if ( ( atom1 == "Cl" ) && ( atom2 == "H" ) )
            dh = 432.0;
        if ( ( atom1 == "Cl" ) && ( atom2 == "N" ) )
            dh = 200.0;
        if ( ( atom1 == "Cl" ) && ( atom2 == "O" ) )
            dh = 203.0;
        if ( ( atom1 == "F" ) && ( atom2 == "H" ) )
            dh = 563.0;
        if ( ( atom1 == "F" ) && ( atom2 == "N" ) )
            dh = 270.0;
        if ( ( atom1 == "F" ) && ( atom2 == "O" ) )
            dh = 185.0;
        if ( ( atom1 == "H" ) && ( atom2 == "H" ) )
            dh = 436.0;
        if ( ( atom1 == "H" ) && ( atom2 == "I" ) )
            dh = 299.0;
        if ( ( atom1 == "H" ) && ( atom2 == "N" ) )
            dh = 391.0;
        if ( ( atom1 == "H" ) && ( atom2 == "O" ) )
            dh = 463.0;
        if ( ( atom1 == "H" ) && ( atom2 == "S" ) )
            dh = 339.0;
        if ( ( atom1 == "O" ) && ( atom2 == "O" ) )
            dh = 139.0;
    }
    if ( order == 2 ) {
        if ( ( atom1 == "C" ) && ( atom2 == "C" ) )
            dh = 615.0;
        if ( ( atom1 == "C" ) && ( atom2 == "N" ) )
            dh = 615.0;
        if ( ( atom1 == "C" ) && ( atom2 == "O" ) )
            dh = 728.0;
        if ( ( atom1 == "C" ) && ( atom2 == "S" ) )
            dh = 477.0;
        if ( ( atom1 == "N" ) && ( atom2 == "N" ) )
            dh = 418.0;
        if ( ( atom1 == "O" ) && ( atom2 == "O" ) )
            dh = 498.0;
    }
    if ( order == 3 ) {
        if ( ( atom1 == "C" ) && ( atom2 == "C" ) )
            dh = 812.0;
        if ( ( atom1 == "C" ) && ( atom2 == "N" ) )
            dh = 891.0;
        if ( ( atom1 == "N" ) && ( atom2 == "N" ) )
            dh = 945.0;
    }

    return dh;
}

double Bond::Length()
{
    return start->distanceTo( end );
}

void Bond::reverse() {
    DPoint* copy = start;
    start = end;
    end = copy;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
