// symbol.cpp - Symbol's implementation of functions

#include <QPainterPath>

#include "render2d.h"
#include "drawable.h"
#include "symbol.h"
#include "bondedit.h"
#include "defs.h"

// XPM files defining symbols
#include "symbol_xpm.h"
// end symbol defs

Symbol::Symbol( Render2D *r1, QObject *parent )
    : Drawable( parent )
{
    m_renderer = r1;
    highlighted = false;
    offset = QPoint( 0, 0 );
    need_offset = false;
    rotation = 0.0;
    scale = 1.0;
}

void Symbol::SetSymbol( QString n )
{
    which = n;
    // load symbol.  Note that "sym_ring_up" is not represented by a pixmap.
    // "sym_2e_line" is also generated as needed.
    // most p orbitals are also generated on the fly.
    if ( which == "sym_plus" ) {
        originalRegular = QPixmap( sym_plus_xpm );
        rotateRegular = QPixmap( sym_plus_xpm );
        originalHighlighted = QPixmap( hl_sym_plus_xpm );
        rotateHighlighted = QPixmap( hl_sym_plus_xpm );
        return;
    }
    if ( which == "sym_minus" ) {
        originalRegular = QPixmap( sym_minus_xpm );
        rotateRegular = QPixmap( sym_minus_xpm );
        originalHighlighted = QPixmap( hl_sym_minus_xpm );
        rotateHighlighted = QPixmap( hl_sym_minus_xpm );
        return;
    }
    if ( which == "sym_delta_plus" ) {
        originalRegular = QPixmap( sym_delta_plus_xpm );
        rotateRegular = QPixmap( sym_delta_plus_xpm );
        originalHighlighted = QPixmap( hl_sym_delta_plus_xpm );
        rotateHighlighted = QPixmap( hl_sym_delta_plus_xpm );
        return;
    }
    if ( which == "sym_delta_minus" ) {
        originalRegular = QPixmap( sym_delta_minus_xpm );
        rotateRegular = QPixmap( sym_delta_minus_xpm );
        originalHighlighted = QPixmap( hl_sym_delta_minus_xpm );
        rotateHighlighted = QPixmap( hl_sym_delta_minus_xpm );
        return;
    }
    if ( which == "sym_2e" ) {
        originalRegular = QPixmap( sym_2e_xpm );
        rotateRegular = QPixmap( sym_2e_xpm );
        originalHighlighted = QPixmap( hl_sym_2e_xpm );
        rotateHighlighted = QPixmap( hl_sym_2e_xpm );
        return;
    }
    if ( which == "sym_1e" ) {
        originalRegular = QPixmap( sym_1e_xpm );
        rotateRegular = QPixmap( sym_1e_xpm );
        originalHighlighted = QPixmap( hl_sym_1e_xpm );
        rotateHighlighted = QPixmap( hl_sym_1e_xpm );
        return;
    }
    if ( which == "p_orbital" ) {
        originalRegular = QPixmap( p_orbital_xpm );
        rotateRegular = QPixmap( p_orbital_xpm );
        originalHighlighted = QPixmap( hl_p_orbital_xpm );
        rotateHighlighted = QPixmap( hl_p_orbital_xpm );
        return;
    }
    if ( which == "bead" ) {
        originalRegular = QPixmap( bead_xpm );
        rotateRegular = QPixmap( bead_xpm );
        originalHighlighted = QPixmap( hl_bead_xpm );
        rotateHighlighted = QPixmap( hl_bead_xpm );
        return;
    }
}

void Symbol::SetRotate( double d )
{
    rotation = d;
    // if text symbol, don't rotate
    if ( which == QString( "sym_delta_plus" ) )
        return;
    if ( which == QString( "sym_delta_minus" ) )
        return;
    if ( which == QString( "sym_minus" ) )
        return;
    if ( which == QString( "p_orbital" ) )
        return;
    if ( which == QString( "p_double_orbital" ) )
        return;
    if ( which == QString( "bead" ) )
        return;
    // rotate symbol
    QMatrix rm;

    rm.rotate( d );
    rotateRegular = originalRegular.transformed( rm );
    rotateHighlighted = originalHighlighted.transformed( rm );
}

QString Symbol::ToXML( QString xml_id )
{
    QString s, n1;

    // begin symbol
    s.append( "<symbol id=" );
    s.append( xml_id );
    s.append( ">\n" );

    // write Start
    s.append( "<Start>" );
    n1.setNum( start->x );
    s.append( n1 );
    s.append( " " );
    n1.setNum( start->y );
    s.append( n1 );
    s.append( "</Start>\n" );

    // write symbol code
    s.append( "<symtype>" );
    s.append( which );
    s.append( "</symtype>\n" );

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

    // rotate and scale
    s.append( "<rotate>" );
    n1.setNum( rotation );
    s.append( n1 );
    s.append( "</rotate>\n" );
    s.append( "<scale>" );
    n1.setNum( scale );
    s.append( n1 );
    s.append( "</scale>\n" );

    // end symbol
    s.append( "</symbol>\n" );

    return s;
}

// set Symbol from XDrawChem-format XML
void Symbol::FromXML( QString xml_tag )
{
    int i1, i2;

    i1 = xml_tag.indexOf( "<Start>" );
    i2 = xml_tag.indexOf( "</Start>" ) + 8;
    SetStartFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<symtype>" ) + 9;
    i2 = xml_tag.indexOf( "</symtype>" );
    SetSymbol( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<color>" );
    i2 = xml_tag.indexOf( "</color>" ) + 8;
    SetColorFromXML( xml_tag.mid( i1, i2 - i1 ) );
    i1 = xml_tag.indexOf( "<rotate>" ) + 8;
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</rotate>" );
        rotation = xml_tag.mid( i1, i2 - i1 ).toDouble();
    }
    i1 = xml_tag.indexOf( "<scale>" ) + 7;
    if ( i1 >= 0 ) {
        i2 = xml_tag.indexOf( "</scale>" );
        scale = xml_tag.mid( i1, i2 - i1 ).toDouble();
    }
}

void Symbol::Render()
{
    QPoint p1 = start->toQPoint();
    QPoint p11, p22;

    if ( which == "newman" ) {  // generate on the fly
        if ( highlighted ) {
            m_renderer->drawCircle( QPoint( p1.x() - 12, p1.y() - 12 ), 12, QColor( 0, 0, 0 ) );
            m_renderer->drawLine( p1, QPoint( p1.x(), p1.y() - 12 ), 1, QColor( 255, 255, 255 ) );
            m_renderer->drawLine( p1, QPoint( p1.x() - 10, p1.y() + 6 ), 1, QColor( 255, 255, 255 ) );
            m_renderer->drawLine( p1, QPoint( p1.x() + 10, p1.y() + 6 ), 1, QColor( 255, 255, 255 ) );
        } else {
            m_renderer->drawCircle( QPoint( p1.x() - 12, p1.y() - 12 ), 12, QColor( 255, 255, 255 ) );
            m_renderer->drawLine( p1, QPoint( p1.x(), p1.y() - 12 ), 1, QColor( 0, 0, 0 ) );
            m_renderer->drawLine( p1, QPoint( p1.x() - 10, p1.y() + 6 ), 1, QColor( 0, 0, 0 ) );
            m_renderer->drawLine( p1, QPoint( p1.x() + 10, p1.y() + 6 ), 1, QColor( 0, 0, 0 ) );
        }
        return;
    }
    if ( which == "sym_ring_up" ) {     // generate on the fly
        if ( highlighted ) {
            m_renderer->drawFillBox( QPoint( p1.x() - 5, p1.y() - 5 ), QPoint( p1.x() + 5, p1.y() + 5 ), QColor( 0, 0, 0 ) );
            m_renderer->drawCircle( QPoint( p1.x() - 4, p1.y() - 4 ), 4, QColor( 255, 255, 255 ) );
        } else {
            m_renderer->drawCircle( QPoint( p1.x() - 4, p1.y() - 4 ), 4, QColor( 0, 0, 0 ) );
        }
        return;
    }
    if ( which == "p_orbital" ) {       // generate on the fly
        qDebug() << "p_orbital " << rotation;
        if ( !highlighted ) {
            if ( rotation == 180.0 ) {
                QPoint ce = QPoint( p1.x(), p1.y() - 10 );
                QPainterPath pp;
                double d = 5.0;

                pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, 2880 );
                m_renderer->drawPolyline( pp, color );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );
            }
            if ( rotation == 0.0 ) {
/*                QPoint ce = QPoint( p1.x(), p1.y() + 10 );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, -2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );*/
            }
            if ( rotation == 90.0 ) {
/*                QPoint ce = QPoint( p1.x() - 10, p1.y() );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, 2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );*/
            }
            if ( rotation == -90.0 ) {
/*                QPoint ce = QPoint( p1.x() + 10, p1.y() );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, -2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );*/
            }
        } else {
            if ( rotation == 180.0 ) {
/*                QPoint ce = QPoint( p1.x(), p1.y() - 10 );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, 2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );*/
            }
            if ( rotation == 0.0 ) {
/*                QPoint ce = QPoint( p1.x(), p1.y() + 10 );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, -2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );*/
            }
            if ( rotation == 90.0 ) {
/*                QPoint ce = QPoint( p1.x() - 10, p1.y() );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, 2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );*/
            }
            if ( rotation == -90.0 ) {
/*                QPoint ce = QPoint( p1.x() + 10, p1.y() );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, -2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );*/
            }
        }
        return;
    }
    if ( which == "p_double_orbital" ) {        // generate on the fly
        if ( highlighted ) {
            if ( fabs( rotation ) == 90.0 ) {
 /*               QPoint ce = QPoint( p1.x() - 10, p1.y() );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, 2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce = QPoint( p1.x() + 10, p1.y() );
                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, -2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );*/
            } else {
/*                QPoint ce = QPoint( p1.x(), p1.y() - 10 );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, 2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce = QPoint( p1.x(), p1.y() + 10 );
                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, -2880 );
                m_renderer->drawPolyline( pa, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, QColor( 255, 0, 0 ) );*/
            }
        } else {
            if ( fabs( rotation ) == 90.0 ) {
/*                QPoint ce = QPoint( p1.x() - 10, p1.y() );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, 2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce = QPoint( p1.x() + 10, p1.y() );
                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 1440, -2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setY( p1.y() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setY( p1.y() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );*/
            } else {
/*                QPoint ce = QPoint( p1.x(), p1.y() - 10 );
                Q3PointArray pa;
                double d = 5.0;

                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, 2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce = QPoint( p1.x(), p1.y() + 10 );
                pa.makeArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, 0, -2880 );
                m_renderer->drawPolyline( pa, color );
                ce.setX( p1.x() - 5 );
                m_renderer->drawLine( p1, ce, 1, color );
                ce.setX( p1.x() + 5 );
                m_renderer->drawLine( p1, ce, 1, color );*/
            }
        }
        return;
    }
    if ( which == "sym_2e_line" ) {
        if ( highlighted ) {
            p11 = p1;
            p22 = p1;
            p11 += QPoint( -5, -5 );
            p22 += QPoint( 5, 5 );
            m_renderer->drawFillBox( p11, p22, QColor( 255, 0, 0 ) );
        }
        p11 = p1;
        p22 = p1;
        if ( rotation == 0.0 ) {
            p11 += QPoint( -5, 10 );
            p22 += QPoint( 5, 10 );
        }
        if ( rotation == 90.0 ) {
            p11 += QPoint( -10, 5 );
            p22 += QPoint( -10, -5 );
        }
        if ( rotation == 180.0 ) {
            p11 += QPoint( -5, -10 );
            p22 += QPoint( 5, -10 );
        }
        if ( rotation == -90.0 ) {
            p11 += QPoint( 10, 5 );
            p22 += QPoint( 10, -5 );
        }
        m_renderer->drawLine( p11, p22, 1, QColor( 0, 0, 0 ) );
        return;
    }
    if ( which != "p_orbital" ) {
        p1.setX( p1.x() + offset.x() - 8 );
        p1.setY( p1.y() + offset.y() - 8 );
    } else {
        p1.setX( p1.x() - 8 );
        p1.setY( p1.y() - 20 );
    }
    if ( which == "bead" ) {
        p1.setY( p1.y() - 15 );
    }
    if ( highlighted ) {
        m_renderer->drawPixmap( p1, rotateHighlighted );
    } else {
        m_renderer->drawPixmap( p1, rotateRegular );
    }
}

void Symbol::Edit()
{
    int lsty;

    if ( which == "sym_plus" )
        lsty = SYM_PLUS;
    if ( which == "sym_minus" )
        lsty = SYM_MINUS;
    if ( which == "sym_delta_plus" )
        lsty = SYM_DELTA_PLUS;
    if ( which == "sym_minus" )
        lsty = SYM_DELTA_MINUS;
    if ( which == "sym_2e" )
        lsty = SYM_2E;
    if ( which == "sym_1e" )
        lsty = SYM_1E;
    if ( which == "sym_ring_up" )
        lsty = SYM_RING_UP;
    if ( which == "p_orbital" )
        lsty = SYM_P_ORBITAL;
    if ( which == "p_double_orbital" )
        lsty = SYM_P_DOUBLE;
    if ( which == "bead" )
        lsty = SYM_BEAD;
    BondEditDialog be( m_renderer, start, end, TYPE_SYMBOL, 0, 0, 0, lsty, color );

    if ( !be.exec() )
        return;
    qDebug() << "change";
    lsty = be.Style();
    if ( lsty == SYM_PLUS )
        SetSymbol( "sym_plus" );
    if ( lsty == SYM_MINUS )
        SetSymbol( "sym_minus" );
    if ( lsty == SYM_DELTA_PLUS )
        SetSymbol( "sym_delta_plus" );
    if ( lsty == SYM_DELTA_MINUS )
        SetSymbol( "sym_delta_minus" );
    if ( lsty == SYM_2E )
        SetSymbol( "sym_2e" );
    if ( lsty == SYM_1E )
        SetSymbol( "sym_1e" );
    if ( lsty == SYM_RING_UP )
        SetSymbol( "sym_ring_up" );
    if ( lsty == SYM_P_ORBITAL )
        SetSymbol( "p_orbital" );
    if ( lsty == SYM_P_DOUBLE )
        SetSymbol( "p_double_orbital" );
    if ( lsty == SYM_BEAD )
        SetSymbol( "bead" );
}

int Symbol::Type()
{
    return TYPE_SYMBOL;
}

bool Symbol::Find( DPoint * target )
{
    if ( start == target )
        return true;
    return false;
}

// Do not allow connections to this object.
// Simplest way to do this, I think, is to disallow this function
DPoint *Symbol::FindNearestPoint( DPoint * target, double &dist )
{
    dist = 99999.0;
    return 0;
}

Drawable *Symbol::FindNearestObject( DPoint * target, double &dist )
{
    if ( WithinBounds( target ) )
        dist = 0.01;
    else
        dist = 99999.0;
    return this;
}

bool Symbol::WithinBounds( DPoint * target )
{
    tmp_pt = new DPoint( start->x, start->y );
    tmp_pt->x = tmp_pt->x + offset.x();
    tmp_pt->y = tmp_pt->y + offset.y();
    if ( tmp_pt->distanceTo( target ) < 8.0 )
        return true;
    else
        return false;
}

void Symbol::setPoint( DPoint * s )
{
    start = s;
}

QRect Symbol::BoundingBox()
{
    if ( highlighted == false )
        return QRect( QPoint( 999, 999 ), QPoint( 0, 0 ) );
    int top, bottom, left, right, swp;

    top = ( int ) start->y + offset.y() - 8;
    left = ( int ) start->x + offset.x() - 8;
    bottom = ( int ) start->y + offset.y() + 8;
    right = ( int ) start->x + offset.x() + 8;
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

bool Symbol::isWithinRect( QRect n, bool shiftdown )
{
    tmp_pt = new DPoint( start->x, start->y );
    tmp_pt->x = tmp_pt->x + offset.x();
    tmp_pt->y = tmp_pt->y + offset.y();
    if ( n.contains( tmp_pt->toQPoint(), true ) )
        highlighted = true;
    else
        highlighted = false;
    return highlighted;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
