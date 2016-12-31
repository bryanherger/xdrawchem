// drawable.cpp - Drawable's implementation of functions

#include "drawable.h"
#include "dpoint.h"
#include "defs.h"

Drawable::Drawable( QObject * parent )
    : QObject( parent )
{
    start = 0;
    end = 0;
    color = QColor( 0, 0, 0 );
}

// ***************************************************
// functions shared by all Drawables (except Molecule)
//
// AllPoints() and AllObjects(): return all unique DPoint's and all
// Drawables, respectively, contained in Molecule.  Only used by save_cml,
// I think, and only applies to Molecule.
int Drawable::Members()
{
    return 1;
}

QList < DPoint * >Drawable::AllPoints()
{
    QList < DPoint * >l;
    return l;
}

QList < Drawable * >Drawable::AllObjects()
{
    QList < Drawable * >l;
    return l;
}

double Drawable::DistanceBetween( QPoint a, QPoint b )
{
    double dx = a.x() - b.x();
    double dy = a.y() - b.y();

    return sqrt( dx * dx + dy * dy );
}

// convert Drawable to XML tag.  Reimplement for all subclasses!
QString Drawable::ToXML( QString )
{
    return QString( "<drawable/>\n" );
}

// convert Drawable to CDXML tag.  Reimplement for all subclasses!
QString Drawable::ToCDXML( QString )
{
    return QString();
}

// convert XML tag to Drawable.  Reimplement for all subclasses!
void Drawable::FromXML( QString )
{
    return;
}

// convert XML <color> tag to QColor and set current
void Drawable::SetColorFromXML( QString xml_tag )
{
    qDebug() << "SetColorFromXML:" << xml_tag;
    int i1, i2;
    int d1, d2, d3;

    i1 = xml_tag.indexOf( "<color>" );
    i2 = xml_tag.indexOf( "</color>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 7 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> d1 >> d2 >> d3;

    color = QColor( d1, d2, d3 );
}

// convert XML <color> tag to QColor and return QColor
QColor Drawable::GetColorFromXML( QString xml_tag )
{
    qDebug() << "GetColorFromXML:" << xml_tag;
    QColor ncolor;
    int i1, i2;
    int d1, d2, d3;

    i1 = xml_tag.indexOf( "<color>" );
    i2 = xml_tag.indexOf( "</color>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 7 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> d1 >> d2 >> d3;

    ncolor = QColor( d1, d2, d3 );
    return ncolor;
}

// set DPoint *start from XML
void Drawable::SetStartFromXML( QString xml_tag )
{
    qDebug() << "SetStartFromXML:" << xml_tag;
    int i1, i2;
    double x1, y1;

    i1 = xml_tag.indexOf( "<Start>" );
    i2 = xml_tag.indexOf( "</Start>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 7 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> x1 >> y1;

    tmp_pt = new DPoint( x1, y1 );      // necessary

    start = tmp_pt;
}

// set DPoint *end from XML
void Drawable::SetEndFromXML( QString xml_tag )
{
    qDebug() << "SetEndFromXML:" << xml_tag;
    int i1, i2;
    double x1, y1;

    i1 = xml_tag.indexOf( "<End>" );
    i2 = xml_tag.indexOf( "</End>" );
    xml_tag.remove( i2, 999 );
    xml_tag.remove( i1, 5 );

    QTextStream ts( &xml_tag, QIODevice::ReadOnly );

    ts >> x1 >> y1;

    tmp_pt = new DPoint( x1, y1 );      // necessary

    end = tmp_pt;
}

void Drawable::Highlight()
{
    if ( end == 0 ) {
        if ( start->isHighlighted() == true ) {
            highlighted = true;
        } else {
            highlighted = false;
        }
    } else {
        if ( ( start->isHighlighted() == true ) && ( end->isHighlighted() == true ) ) {
            highlighted = true;
        } else {
            highlighted = false;
        }
    }
}

void Drawable::Highlight( bool hl )
{
    highlighted = hl;
    if ( hl == true ) {
        start->setHighlighted( true );
    } else {
    }
}

bool Drawable::Highlighted()
{
    return highlighted;
}

void Drawable::SelectAll()
{
    highlighted = true;
}

void Drawable::DeselectAll()
{
    highlighted = false;
}

void Drawable::SetColorIfHighlighted( QColor c )
{
    if ( highlighted )
        color = c;
}

void Drawable::Move( double dx, double dy )
{
    if ( ( highlighted ) && ( start != 0 ) ) {
        start->x += dx;
        start->y += dy;
    }
    if ( ( highlighted ) && ( end != 0 ) ) {
        end->x += dx;
        end->y += dy;
    }
}

void Drawable::ForceMove( double dx, double dy )
{
    if ( start != 0 ) {
        start->x += dx;
        start->y += dy;
    }
    if ( end != 0 ) {
        end->x += dx;
        end->y += dy;
    }
}

void Drawable::Flip( DPoint *origin, int direction )
{
    double delta;

    if ( highlighted == false )
        return;
    if ( start != 0 ) {
        if ( direction == FLIP_H ) {
            delta = start->x - origin->x;
            start->x = start->x - 2 * delta;
        } else {                // direction == FLIP_V
            delta = start->y - origin->y;
            start->y = start->y - 2 * delta;
        }
    }
    if ( end != 0 ) {
        if ( direction == FLIP_H ) {
            delta = end->x - origin->x;
            end->x = end->x - 2 * delta;
        } else {                // direction == FLIP_V
            delta = end->y - origin->y;
            end->y = end->y - 2 * delta;
        }
    }
}

void Drawable::Rotate( DPoint *origin, double angle )
{
    //double dx, dy;

    if ( highlighted == false )
        return;
    if ( start != 0 ) {
        double thisx = start->x - origin->x;
        double thisy = start->y - origin->y;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        start->x = ( newx + origin->x );
        start->y = ( newy + origin->y );
    }
    if ( end != 0 ) {
        double thisx = end->x - origin->x;
        double thisy = end->y - origin->y;
        double newx = thisx * cos( angle ) + thisy * sin( angle );
        double newy = -thisx * sin( angle ) + thisy * cos( angle );

        end->x = ( newx + origin->x );
        end->y = ( newy + origin->y );
    }
}

void Drawable::Resize( DPoint *origin, double scale )
{
    double dx, dy;

    if ( highlighted == false )
        return;
    if ( start != 0 ) {
        dx = start->x - origin->x;
        dy = start->y - origin->y;
        dx *= scale;
        dy *= scale;
        start->x = origin->x + dx;
        start->y = origin->y + dy;
    }
    if ( end != 0 ) {
        dx = end->x - origin->x;
        dy = end->y - origin->y;
        dx *= scale;
        dy *= scale;
        end->x = origin->x + dx;
        end->y = origin->y + dy;
    }
}

//
// end common functions
// ********************

void Drawable::Render()
{
    qDebug() << "Not possible to render Drawable, only its subclasses";
}

void Drawable::Edit()
{
    qDebug() << "Not possible to edit Drawable, only its subclasses";
}

int Drawable::Type()
{
    return TYPE_DRAWABLE;
}

DPoint *Drawable::FindNearestPoint( DPoint *, double &d )
{
    d = 99999.0;
    return 0;
}

Drawable *Drawable::FindNearestObject( DPoint *, double &d )
{
    d = 99999.0;
    return 0;
}

bool Drawable::Find( DPoint * )
{
    return false;
}

void Drawable::addBond( DPoint *, DPoint *, int, int, QColor, bool )
{
    return;
}

void Drawable::addMolecule( Drawable * )
{
    return;
}

bool Drawable::Erase( Drawable * )
{
    return false;
}

bool Drawable::isWithinRect( QRect, bool )
{
    return false;
}

QRect Drawable::BoundingBox()
{
    return QRect( QPoint( 999, 999 ), QPoint( 0, 0 ) );
}

// math functions all Drawables might need
double Drawable::getAngle( DPoint * a, DPoint * b )
{
    double dx = b->x - a->x;
    double dy = b->y - a->y;
    double ang = atan( dy / dx ) * ( 180.0 / M_PI );

    if ( ang < 0.0 )
        ang *= -1.0;

    if ( b->x > a->x && b->y > a->y )
        ang = 0.0 + ang;
    if ( b->x < a->x && b->y > a->y )
        ang = 180.0 - ang;
    if ( b->x < a->x && b->y < a->y )
        ang = 180.0 + ang;
    if ( b->x > a->x && b->y < a->y )
        ang = 360.0 - ang;
    if ( dx == 0.0 ) {
        if ( dy < 0.0 )
            ang = 270.0;
        else
            ang = 90.0;
    }
    if ( dy == 0.0 ) {
        if ( dx < 0.0 )
            ang = 180.0;
        else
            ang = 0.0;
    }

    return ang;
}

// start->end is the line; target is the point we're measuring to
double Drawable::DistanceToLine( DPoint * start1, DPoint * end1, DPoint * target )
{
    // check if close enough to even worry
#if 0
    double xmin, xmax, ymin, ymax;

    if ( start1->x > end1->x ) {
        xmax = start1->x;
        xmin = end1->x;
    } else {
        xmin = start1->x;
        xmax = end1->x;
    }
    if ( start1->y > end1->y ) {
        ymax = start1->y;
        ymin = end1->y;
    } else {
        ymin = start1->y;
        ymax = end1->y;
    }
    xmin -= 6.0;
    ymin -= 6.0;
    xmax += 6.0;
    ymax += 6.0;
    if ( ( target->x < xmin ) || ( target->x > xmax ) || ( target->y < ymin ) || ( target->y > ymax ) )
        return 999999.0;
    // find distance if target is inside "bounding box"
    double x1 = start1->x;
    double y1 = start1->y;
    double x2 = end1->x;
    double y2 = end1->y;
    double x3 = target->x;
    double y3 = target->y;
    double unum = ( x3 - x1 ) * ( x2 - x1 ) + ( y3 - y1 ) * ( y2 - y1 );
    double dx = x2 - x1;
    double dy = y2 - y1;
    double rt = ( double ) sqrt( dx * dx + dy * dy );
    double uden = rt * rt;
    double u = unum / uden;
    double xp = x1 + u * ( x2 - x1 );
    double yp = y1 + u * ( y2 - y1 );

    dx = xp - x3;
    dy = yp - y3;
    return ( double ) sqrt( dx * dx + dy * dy );
#else
    double x1 = start1->x;
    double y1 = start1->y;
    double x2 = end1->x;
    double y2 = end1->y;
    double x3 = target->x;
    double y3 = target->y;
    double dx = x2 - x1;
    double dy = y2 - y1;
    double rt = ( double ) sqrt( dx * dx + dy * dy );

    if ( rt <= 0.0 ) {
        dx = x3 - x1;
        dy = y3 - y1;
        return sqrt( dx * dx + dy * dy );
    }
    if ( dy * ( y3 - y1 ) + dx * ( x3 - x1 ) < 0.0 ) {
        dx = x3 - x1;
        dy = y3 - y1;
        return sqrt( dx * dx + dy * dy );
    }
    if ( dy * ( y3 - y2 ) + dx * ( x3 - x2 ) > 0.0 ) {
        dx = x3 - x2;
        dy = y3 - y2;
        return sqrt( dx * dx + dy * dy );
    }
    double unum = dx * ( y3 - y1 ) - dy * ( x3 - x1 );

    if ( unum < 0.0 )
        unum = -unum;
    return unum / rt;
#endif
}

// DPointInRect: companion to WithinRect() in subclasses.
bool Drawable::DPointInRect( DPoint * d, QRect r )
{
    return r.contains( QPoint( ( int ) d->x, ( int ) d->y ), true );
}

//cmake#include "drawable.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
