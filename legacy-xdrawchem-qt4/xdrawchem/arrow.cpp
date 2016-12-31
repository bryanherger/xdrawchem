// arrow.cpp - Arrow's implementation of functions

#include "render2d.h"
#include "drawable.h"
#include "arrow.h"
#include "bondedit.h"
#include "defs.h"

Arrow::Arrow( Render2D * r1, QObject * parent )
    : Drawable( parent )
{
    m_renderer = r1;
    highlighted = false;
    style = ARROW_REGULAR;
    thick = 1;
}

QString Arrow::ToXML( QString xml_id )
{
    QString s, n1;

    // begin arrow
    s.append( "<arrow id=\"" );
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

    // write style
    s.append( "<style>" );
    n1.setNum( style );
    s.append( n1 );
    s.append( "</style>\n" );

    // write thickness
    s.append( "<thick>" );
    n1.setNum( thick );
    s.append( n1 );
    s.append( "</thick>\n" );

    // end arrow
    s.append( "</arrow>\n" );

    return s;
}

QString Arrow::ToCDXML( QString xml_id )
{
    QString s, n1;

    // begin arrow
    s.append( "<graphic id=\"" );
    s.append( xml_id );
    s.append( "\" BoundingBox=\"" );
    n1.setNum( end->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( end->y );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->y );
    s.append( n1 );
    s.append( "\" " );
    if ( style == ARROW_REGULAR )
        s.append( "GraphicType=\"Line\" ArrowType=\"FullHead\" HeadSize=\"1000\"" );
    if ( style == ARROW_DASH )
        s.append( "GraphicType=\"Line\" LineType=\"Dashed\" ArrowType=\"FullHead\" HeadSize=\"1000\"" );
    if ( style == ARROW_BI1 )
        s.append( "GraphicType=\"Line\" ArrowType=\"Resonance\" HeadSize=\"1000\"" );
    if ( style == ARROW_BI2 )
        s.append( "GraphicType=\"Line\" ArrowType=\"Equilibrium\" HeadSize=\"1000\"" );
    if ( style == ARROW_RETRO )
        s.append( "GraphicType=\"Line\" ArrowType=\"RetroSynthetic\" HeadSize=\"1000\"" );
    s.append( "/>\n" );

    return s;
}

// set Arrow from XDrawChem-format XML
void Arrow::FromXML( QString xml_tag )
{
    qDebug() << "read arrow";
    int i1, i2;

    i1 = xml_tag.indexOf( "<Start>" );
    i2 = xml_tag.indexOf( "</Start>" ) + 8;
    SetStartFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<End>" );
    i2 = xml_tag.indexOf( "</End>" ) + 6;
    SetEndFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<style>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</style>" ) + 8;
        qDebug() << "arrow::style = |" << xml_tag.mid( i1 + 7, i2 - i1 - 15 )
            << "|";
        style = xml_tag.mid( i1 + 7, i2 - i1 - 15 ).toInt();
    }
    i1 = xml_tag.indexOf( "<thick>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</thick>" ) + 8;
        thick = xml_tag.mid( i1 + 7, 1 ).toInt();
    }
    i1 = xml_tag.indexOf( "<color>" );
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</color>" ) + 8;
        SetColorFromXML( xml_tag.mid( i1, i2 - i1 ) );
    }
}

void Arrow::Render()
{
    QColor c1;

    if ( highlighted )
        c1 = QColor( 255, 0, 0 );
    else
        c1 = color;
    m_renderer->drawArrow( start->toQPoint(), end->toQPoint(), c1, style, thick );
}

int Arrow::Type()
{
    return TYPE_ARROW;
}

void Arrow::Edit()
{
    qDebug() << "edit arrow";
    BondEditDialog be( m_renderer, start, end, TYPE_ARROW, 0, 0, thick, style, color );

    if ( !be.exec() )
        return;
    qDebug() << "change";
    style = be.Style();
    color = be.Color();
    thick = be.Thick();
}

int Arrow::Orientation()
{
    double a1 = getAngle( start, end );

    if ( a1 < 45.0 )
        return ARROW_HORIZONTAL;
    if ( a1 < 135.0 )
        return ARROW_VERTICAL;
    if ( a1 < 225.0 )
        return ARROW_HORIZONTAL;
    if ( a1 < 315.0 )
        return ARROW_VERTICAL;
    return ARROW_HORIZONTAL;
}

QPoint Arrow::Midpoint()
{
    QPoint a;

    a.setX( qRound( ( start->x + end->x ) / 2.0 ) );
    a.setY( qRound( ( start->y + end->y ) / 2.0 ) );
    return a;
}

bool Arrow::Find( DPoint * target )
{
    if ( start == target )
        return true;
    if ( end == target )
        return true;
    return false;
}

// Do not allow connections to this object.
// Simplest way to do this, I think, is to disallow this function
DPoint *Arrow::FindNearestPoint( DPoint */*target*/, double &dist )
{
    dist = 99999.0;
    return 0;
}

Drawable *Arrow::FindNearestObject( DPoint * target, double &dist )
{
    dist = DistanceToLine( start, end, target );
    return this;
}

void Arrow::setPoints( DPoint * s, DPoint * e )
{
    start = s;
    end = e;
}

QRect Arrow::BoundingBox()
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

bool Arrow::isWithinRect( QRect n, bool /*shiftdown*/ )
{
    if ( DPointInRect( start, n ) && DPointInRect( end, n ) )
        highlighted = true;
    else
        highlighted = false;
    return highlighted;
}

//cmake#include "arrow.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
