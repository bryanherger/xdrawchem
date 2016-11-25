// curvearrow.cpp - Curvearrow's implementation of functions

#include "render2d.h"
#include "drawable.h"
#include "curvearrow.h"
#include "bondedit.h"
#include "defs.h"

CurveArrow::CurveArrow( Render2D *r1, QObject *parent )
    : Drawable( parent )
{
    r = r1;
    highlighted = false;
}

QString CurveArrow::ToXML( QString xml_id )
{
    QString s, n1;

    // begin curvearrow
    s.append( "<curvearrow id=\"" );
    s.append( xml_id );
    s.append( "\">\n" );

    // write Start
    s.append( "<Start>" );
    n1.setNum( start->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->y );
    s.append( n1 );
    s.append( "</Start>\n" );

    // write End
    s.append( "<End>" );
    n1.setNum( end->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( end->y );
    s.append( n1 );
    s.append( "</End>\n" );

    // write type
    s.append( "<curvetype>" );
    s.append( which );
    s.append( "</curvetype>\n" );

    // write color
    s.append( "<color>" );
    n1.setNum( color.red() );
    s.append( n1 );
    s.append( " " );
    n1.setNum( color.green() );
    s.append( n1 );
    s.append( " " );
    n1.setNum( color.blue() );
    s.append( n1 );
    s.append( "</color>\n" );

    // end curvearrow
    s.append( "</curvearrow>\n" );

    return s;
}

// set CurveArrow from XDrawChem-format XML
void CurveArrow::FromXML( QString xml_tag )
{
    int i1, i2;

    i1 = xml_tag.indexOf( "<Start>" );
    i2 = xml_tag.indexOf( "</Start>" ) + 8;
    SetStartFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<End>" );
    i2 = xml_tag.indexOf( "</End>" ) + 6;
    SetEndFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<curvetype>" ) + 11;
    i2 = xml_tag.indexOf( "</curvetype>" );
    which = xml_tag.mid( i1, i2 - i1 );
    qDebug() << which;
    i1 = xml_tag.indexOf( "<color>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</color>" ) + 8;
        SetColorFromXML( xml_tag.mid( i1, i2 - i1 ) );
    }
}

void CurveArrow::Render()
{
    QColor c1;

    if ( highlighted )
        c1 = QColor( 255, 0, 0 );
    else
        c1 = color;
    r->drawCurveArrow( start->toQPoint(), end->toQPoint(), c1, which );
}

void CurveArrow::Edit()
{
    int lsty;

    if ( which == "CW90" )
        lsty = CA_CW90;
    if ( which == "CW180" )
        lsty = CA_CW180;
    if ( which == "CW270" )
        lsty = CA_CW270;
    if ( which == "CCW90" )
        lsty = CA_CCW90;
    if ( which == "CCW180" )
        lsty = CA_CCW180;
    if ( which == "CCW270" )
        lsty = CA_CCW270;
    BondEditDialog be( r, start, end, TYPE_CURVEARROW, 0, 0, 0, lsty, color );

    if ( !be.exec() )
        return;
    qDebug() << "change";
    lsty = be.Style();
    color = be.Color();
    switch ( lsty ) {
    case CA_CW90:
        which = "CW90";
        break;
    case CA_CW180:
        which = "CW180";
        break;
    case CA_CW270:
        which = "CW270";
        break;
    case CA_CCW90:
        which = "CCW90";
        break;
    case CA_CCW180:
        which = "CCW180";
        break;
    case CA_CCW270:
        which = "CCW270";
        break;
    }
}

int CurveArrow::Type()
{
    return TYPE_CURVEARROW;
}

bool CurveArrow::Find( DPoint *target )
{
    if ( start == target )
        return true;
    if ( end == target )
        return true;
    return false;
}

// Do not allow connections to this object.
// Simplest way to do this, I think, is to disallow this function
DPoint *CurveArrow::FindNearestPoint( DPoint * target, double &dist )
{
    dist = 99999.0;
    return 0;
}

Drawable *CurveArrow::FindNearestObject( DPoint *target, double &dist )
{
    dist = DistanceToLine( start, end, target );
    return this;
}

void CurveArrow::setPoints( DPoint *s, DPoint *e )
{
    start = s;
    end = e;
}

/*
QRect CurveArrow::BoundingBox() {
  if (highlighted == false)
    return QRect( QPoint(999,999), QPoint(0,0) );
  int top, bottom, left, right, swp;
  left = (int)start->x;
  top = (int)start->y;
  right = (int)end->x;
  bottom = (int)end->y;
  if ( (which == QString("CW180")) || (which == QString("CCW180")) ) {
    QPoint midp((start->x + end->x) / 2,
		(start->y + end->y) / 2);
    double dx = start->x - (double)midp.x();
    double dy = start->y - (double)midp.y();
    double dist = sqrt( dx*dx + dy*dy );
    int dist1 = (int)dist + 1;
    top = midp.y() - dist1;
    left = midp.x() - dist1;
    bottom = midp.y() + dist1;
    right = midp.x() + dist1;
  }
  if (bottom < top) { swp = top; top = bottom; bottom = swp; }
  if (right < left) { swp = left; left = right; right = swp; }
  return QRect( QPoint(left,top), QPoint(right,bottom) );
}
*/

QRect CurveArrow::BoundingBox()
{
    if ( highlighted == false )
        return QRect( QPoint( 999, 999 ), QPoint( 0, 0 ) );
    int top, bottom, left, right, swp;
    QPoint midp( qRound( ( start->x + end->x ) / 2.0 ), qRound( ( start->y + end->y ) / 2.0 ) );
    double dx = start->x - ( double ) midp.x();
    double dy = start->y - ( double ) midp.y();
    double dist = sqrt( dx * dx + dy * dy );
    int dist1 = ( int ) dist + 1;

    top = midp.y() - dist1;
    left = midp.x() - dist1;
    bottom = midp.y() + dist1;
    right = midp.x() + dist1;
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

bool CurveArrow::isWithinRect( QRect n, bool /*shiftdown*/ )
{
    if ( DPointInRect( start, n ) && DPointInRect( end, n ) )
        highlighted = true;
    else
        highlighted = false;
    return highlighted;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
