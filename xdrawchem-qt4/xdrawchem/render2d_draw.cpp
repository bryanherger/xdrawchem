#include <QPainter>
#include <QBitmap>

#include "render2d.h"
#include "paintable.h"
#include "chemdata.h"
#include "defs.h"

double Render2D::getAngle( QPoint a, QPoint b )
{
    double dx = b.x() - a.x();
    double dy = b.y() - a.y();
    double ang = atan( ( double ) dy / ( double ) dx ) * ( 180.0 / M_PI );

    if ( ang < 0.0 )
        ang *= -1.0;

    if ( ( b.x() > a.x() ) && ( b.y() > a.y() ) )
        ang = 0.0 + ang;
    if ( ( b.x() < a.x() ) && ( b.y() > a.y() ) )
        ang = 180.0 - ang;
    if ( ( b.x() < a.x() ) && ( b.y() < a.y() ) )
        ang = 180.0 + ang;
    if ( ( b.x() > a.x() ) && ( b.y() < a.y() ) )
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

void Render2D::drawLine( QPoint a, QPoint b, int t, QColor c1, int s )
{
    qDebug() << "Render2D::drawLine()";
    if ( outputDevice == OUTPUT_EPS ) {
        a.setY( a.y() + ( 2 * ( selectionBox.center().y() - a.y() ) ) );
        b.setY( b.y() + ( 2 * ( selectionBox.center().y() - b.y() ) ) );
        if ( s == 0 ) {         // solid line
            output_ts << "[] 0 setdash";
            //output_ts<<"newpath" ;
            output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " moveto";
            output_ts << b.x() - eps_dx << " " << b.y() - eps_dy << " lineto";
            //output_ts<<"closepath stroke" ;
            output_ts << "stroke";
        } else {                // s != 0 is a dashed line
            output_ts << "[5 5] 0 setdash";
            output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " moveto";
            output_ts << b.x() - eps_dx << " " << b.y() - eps_dy << " lineto";
            output_ts << "stroke";
        }
        return;
    }
    if ( outputDevice == OUTPUT_SVG ) {
        //a.setY(a.y() + (2 * (selectionBox.center().y() - a.y() ) ) );
        //b.setY(b.y() + (2 * (selectionBox.center().y() - b.y() ) ) );
        a.setX( a.x() - svg_dx );
        a.setY( a.y() - svg_dy );
        b.setX( b.x() - svg_dx );
        b.setY( b.y() - svg_dy );
        output_ts << "<line x1=\"";
        output_ts << a.x() << "\" y1=\"" << a.y() << "\" x2=\"";
        output_ts << b.x() << "\" y2=\"" << b.y() << "\" stroke=\"";
        output_ts << c1.name() << "\" stroke-width=\"" << t << "\"/>";
        return;
    }
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put line on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       if (s == 0)
       pa->op = OP_LINE;
       else
       pa->op = OP_DASH_LINE;
       pa->a = a;
       pa->b = b;
       pa->c = c1;
       pa->s = s;
       pa->t = t;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
     */
    //p.scale(zoomFactor, zoomFactor);
    if ( s == 0 )               // solid
        //p.setPen(QPen(c1, t));
        painter->setPen( QPen( c1, t ) );
    else                        // dashed -- always apply thickness
        //p.setPen(QPen(c1, t, DotLine));
        painter->setPen( QPen( c1, t, Qt::DotLine ) );
    //p.drawLine(a, b);
    painter->drawLine( a, b );
}

// This function exists to turn Polyline into bits for drawLine(...)
// (to avoid reimplementing EPS/Printer stuff for now :)
void Render2D::drawPolyline( QPainterPath a1, QColor c1 )
{
    qDebug() << "Render2D::drawPolyline()";
/*  int lastpoint = a1.elementCount() - 1;
  for (int cc = 0; cc < lastpoint; cc++)
    drawLine(a1[cc], a1[cc+1], 1, c1);*/
    ///TODO
    painter->drawPath( a1 );
}

void Render2D::drawPolyline( QVector<QPoint> p, QColor c1 )
{
    qDebug() << "Render2D::drawPolyline() 2";
    if (!p.isEmpty()) {
        painter->setPen( c1 );
        painter->drawLines( p );
    }
//    for ( int i = 0; i < p.size(); ++i )
//        drawLine( p.at( i ), p.at( i + 1 ), 1, c1 );
}

// note that this only works on screen and bitmap (for now)
void Render2D::drawBezier( QVector<QPoint> a1, QColor c1, bool drawPoints, int arrowHead )
{
    qDebug() << "Render2D::drawBezier()";
    /*
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
/*    painter->setPen( c1 );
    int xp1, yp1;

    if ( drawPoints ) {
        for ( int l1 = 0; l1 <= bezier_count; l1++ ) {
            a1.point( l1, &xp1, &yp1 );
            painter->drawRect( xp1 - 1, yp1 - 1, 3, 3 );
        }
    }
    if ( drawPoints == false ) {
        painter->drawCubicBezier( a1 );
        QPoint ar1, ar2;        // apparent arrowhead shaft:  (ar1)-->(ar2)

        ar1 = a1.point( a1.count() - 2 );
        ar2 = a1.point( a1.count() - 1 );

        double ang;

        ang = getAngle( ar2, ar1 );

        double newang1 = ang + 30.0;
        double newang2 = ang - 30.0;

        QPoint p1( qRound( ar2.x() + ( cos( newang1 / MOL_ARAD ) * 10.0 ) ), qRound( ar2.y() + ( sin( newang1 / MOL_ARAD ) * 10.0 ) ) );
        QPoint p2( qRound( ar2.x() + ( cos( newang2 / MOL_ARAD ) * 10.0 ) ), qRound( ar2.y() + ( sin( newang2 / MOL_ARAD ) * 10.0 ) ) );

        if ( arrowHead > 0 ) {
            // draw half arrowhead
            drawLine( ar2, p1, 1, c1 );
        }
        if ( arrowHead > 1 ) {
            // draw other half
            drawLine( ar2, p2, 1, c1 );
        }
        return;
    }
    if ( ( mode == MODE_DRAWBEZIER ) && ( bezier_count < 3 ) )
        return;
    painter->drawCubicBezier( a1 );*/
}

void Render2D::drawBracket( QPoint a, QPoint b, QColor c1, int type, QColor fillColor )
{
    int tl_x, tl_y, br_x, br_y, swp;

    tl_x = a.x();
    tl_y = a.y();
    br_x = b.x();
    br_y = b.y();
    if ( tl_x > br_x ) {
        swp = tl_x;
        tl_x = br_x;
        br_x = swp;
    }
    if ( tl_y > br_y ) {
        swp = tl_y;
        tl_y = br_y;
        br_y = swp;
    }
    double sf = 0.08 * ( double ) ( br_y - tl_y );
    double sf2 = sf / 2.0;
    QPoint tl( tl_x, tl_y );
    QPoint bl( tl_x, br_y );
    QPoint tr( br_x, tl_y );
    QPoint br( br_x, br_y );

    if ( ( type == BRACKET_BOX ) || ( type == BRACKET_CLOSEDSQUARE ) ) {
        /*
           drawLine(tl, bl, 1, c1);
           drawLine(bl, br, 1, c1);
           drawLine(tr, br, 1, c1);
           drawLine(tl, tr, 1, c1);
         */
        if ( fillColor.isValid() == true ) {
            drawFillBox( tl, br, fillColor, true, c1, 0 );
        } else {
            drawBox( tl, br, c1 );
        }
    }
    if ( ( type == BRACKET_ELLIPSE ) || ( type == BRACKET_CIRCLE ) ) {
        /*
           QPointArray el;
           el.makeEllipse(tl.x(), tl.y(), br.x() - tl.x(), br.y() - tl.y() );
           drawPolyline(el, c1);
         */
        if ( fillColor.isValid() == true ) {
            drawEllipse( tl, br, true, c1, true, fillColor );
        } else {
            drawEllipse( tl, br, true, c1, false, fillColor );
        }
    }
    if ( type == BRACKET_SQUARE ) {
        drawLine( tl, bl, 1, c1 );
        drawLine( tr, br, 1, c1 );
        drawLine( tl, QPoint( tl_x + qRound( sf ), tl_y ), 1, c1 );
        drawLine( bl, QPoint( tl_x + qRound( sf ), br_y ), 1, c1 );
        drawLine( tr, QPoint( br_x - qRound( sf ), tl_y ), 1, c1 );
        drawLine( br, QPoint( br_x - qRound( sf ), br_y ), 1, c1 );
    }
    if ( type == BRACKET_CURVE ) {
        // calculate center of circle
        double cx = ( tl_x + br_x ) / 2.0;
        double cy = ( tl_y + br_y ) / 2.0;

        // calculate box enclosing circle
        double wx = br_x - tl_x;
        double wy2 = ( br_y - tl_y ) / 2.0;
        double box_x = cx - wx;
        double box_y = cy - wx;
        double ang1 = acos( wy2 / wx ) * 180.0 / M_PI;

        ang1 = 90.0 - ang1;
        //ang1 *= 16.0;
        QPainterPath pp;

	pp.moveTo( br );
        pp.arcTo( qRound( box_x ), qRound( box_y ), qRound( wx * 2.0 ), qRound( wx * 2.0 ), qRound( -ang1 ), qRound( 2.0 * ang1 ) );
        drawPolyline( pp, c1 );
	pp.moveTo( tl );
        pp.arcTo( qRound( box_x ), qRound( box_y ), qRound( wx * 2.0 ), qRound( wx * 2.0 ), qRound( -ang1 + 180.0 ), qRound( 2.0 * ang1 ) );
        drawPolyline( pp, c1 );
    }
    if ( type == BRACKET_BRACE ) {
        drawLine( QPoint( tl_x + qRound( sf2 ), tl_y ), QPoint( tl_x + qRound( sf2 ), br_y ), 1, c1 );
        drawLine( QPoint( br_x - qRound( sf2 ), tl_y ), QPoint( br_x - qRound( sf2 ), br_y ), 1, c1 );
        drawLine( QPoint( tl_x + qRound( sf2 ), tl_y ), QPoint( tl_x + qRound( sf ), tl_y ), 1, c1 );
        drawLine( QPoint( tl_x + qRound( sf2 ), br_y ), QPoint( tl_x + qRound( sf ), br_y ), 1, c1 );
        drawLine( QPoint( br_x - qRound( sf2 ), tl_y ), QPoint( br_x - qRound( sf ), tl_y ), 1, c1 );
        drawLine( QPoint( br_x - qRound( sf2 ), br_y ), QPoint( br_x - qRound( sf ), br_y ), 1, c1 );
        double midy = ( tl_y + br_y ) / 2.0;

        drawLine( QPoint( tl_x, qRound( midy ) ), QPoint( tl_x + qRound( sf2 ), qRound( midy ) ), 1, c1 );
        drawLine( QPoint( br_x, qRound( midy ) ), QPoint( br_x - qRound( sf2 ), qRound( midy ) ), 1, c1 );
    }
}

void Render2D::drawWavyLine( QPoint a, QPoint b, QColor c1 )
{
    /*
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.setPen(c1);
     */

    double dampl = 3.0;
    QPoint last, swpc;

    last.setX( -10 );
    if ( a.x() > b.x() ) {
        swpc = a;
        a = b;
        b = swpc;
    }
    double ia = getAngle( a, b );
    double dx = b.x() - a.x();
    double dy = b.y() - a.y();
    double dmag = sqrt( dx * dx + dy * dy );
    int nwaves = qRound( dmag / 6.0 );
    double wwidth = dmag / ( double ) nwaves;
    int cn;
    double dx1, dy1, ia_rad, dx2, dy2;

    for ( cn = 0; cn <= ( int ) dmag; cn++ ) {
        dx1 = ( double ) cn;
        dy1 = dampl * sin( cn * M_PI / wwidth );
        ia_rad = ia * M_PI / 180.0;
        dx2 = dx1 * cos( ia_rad ) - dy1 * sin( ia_rad );
        dy2 = dx1 * sin( ia_rad ) + dy1 * cos( ia_rad );
        dx2 += ( double ) a.x();
        dy2 += ( double ) a.y();
        swpc.setX( ( int ) dx2 );
        swpc.setY( ( int ) dy2 );
        //drawLine(swpc, swpc, 1, c1, 0);
        if ( last.x() > 0 ) {
            //p.drawPoint(swpc);
            drawLine( last, swpc, 1, c1, 0 );
        }
        last = swpc;
    }
}

void Render2D::drawUpLine( QPoint a, QPoint b, QColor c1 )
{
    double ang = getAngle( a, b );
    double dx = b.x() - a.x();
    double dy = b.y() - a.y();
    double angvar = asin( 3.0 / sqrt( dx * dx + dy * dy ) ) * 180 / M_PI;
    double newang1 = ang + angvar;
    double newang2 = ang - angvar;
    double newlen = sqrt( dx * dx + dy * dy );
    double mysign = 1.0;

    dx = mysign * newlen * cos( newang1 / MOL_ARAD );
    dy = mysign * newlen * sin( newang1 / MOL_ARAD );
    QPoint t1( ( int ) ( a.x() + dx ), ( int ) ( a.y() + dy ) );

    dx = mysign * newlen * cos( newang2 / MOL_ARAD );
    dy = mysign * newlen * sin( newang2 / MOL_ARAD );
    QPoint t2( ( int ) ( a.x() + dx ), ( int ) ( a.y() + dy ) );

    if ( outputDevice == OUTPUT_EPS ) {
        a.setY( a.y() + ( 2 * ( selectionBox.center().y() - a.y() ) ) );
        t1.setY( t1.y() + ( 2 * ( selectionBox.center().y() - t1.y() ) ) );
        t2.setY( t2.y() + ( 2 * ( selectionBox.center().y() - t2.y() ) ) );
        output_ts << "newpath";
        output_ts << t1.x() - eps_dx << " " << t1.y() - eps_dy << " moveto";
        output_ts << t2.x() - eps_dx << " " << t2.y() - eps_dy << " lineto";
        output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " lineto";
        output_ts << t1.x() - eps_dx << " " << t1.y() - eps_dy << " lineto";
        output_ts << "closepath fill";
        return;
    }
    if ( outputDevice == OUTPUT_SVG ) {
        a.setX( a.x() - svg_dx );
        a.setY( a.y() - svg_dy );
        t1.setX( t1.x() - svg_dx );
        t1.setY( t1.y() - svg_dy );
        t2.setX( t2.x() - svg_dx );
        t2.setY( t2.y() - svg_dy );
        output_ts << "<polygon fill=\"" << c1.name() << "\" points=\"";
        output_ts << t1.x() << "," << t1.y() << " ";
        output_ts << t2.x() << "," << t2.y() << " ";
        output_ts << a.x() << "," << a.y() << "\"/>";
        return;
    }
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put line on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       pa->op = OP_UP_LINE;
       pa->a = t1;
       pa->b = t2;
       pa->b1 = a;
       pa->c = c1;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setPen( c1 );
    painter->setBrush( QBrush( c1 ) );
    QPolygon triangle( 3 );

    triangle.setPoint( 0, t1 );
    triangle.setPoint( 1, t2 );
    triangle.setPoint( 2, a );
    painter->drawPolygon( triangle );
    painter->setBrush( QBrush( Qt::NoBrush ) );
}

void Render2D::drawDownLine( QPoint a, QPoint b, QColor c1 )
{
    double ang = getAngle( a, b );
    double dx = b.x() - a.x();
    double dy = b.y() - a.y();
    double angvar = asin( 4.0 / sqrt( dx * dx + dy * dy ) ) * 180 / M_PI;
    double newang1 = ang + angvar;
    double newang2 = ang - angvar;
    double newlen = sqrt( dx * dx + dy * dy );
    QPoint t1, t2;

    for ( double scalefactor = 1.0; scalefactor < newlen; scalefactor = scalefactor + 3.0 ) {
        dx = scalefactor * cos( newang1 / MOL_ARAD );
        dy = scalefactor * sin( newang1 / MOL_ARAD );
        t1.setX( ( a.x() + ( int ) dx ) );
        t1.setY( ( a.y() + ( int ) dy ) );
        dx = scalefactor * cos( newang2 / MOL_ARAD );
        dy = scalefactor * sin( newang2 / MOL_ARAD );
        t2.setX( ( a.x() + ( int ) dx ) );
        t2.setY( ( a.y() + ( int ) dy ) );
        drawLine( t1, t2, 1, c1 );
    }
    dx = cos( newang1 / MOL_ARAD );
    dy = sin( newang1 / MOL_ARAD );
    t1.setX( ( a.x() + ( int ) dx ) );
    t1.setY( ( a.y() + ( int ) dy ) );
    dx = cos( newang2 / MOL_ARAD );
    dy = sin( newang2 / MOL_ARAD );
    t2.setX( ( a.x() + ( int ) dx ) );
    t2.setY( ( a.y() + ( int ) dy ) );
    drawLine( t1, t2, 1, c1 );
}

void Render2D::drawArrow( QPoint a, QPoint b, QColor c1, int s1, int th1 )
{
    // Build arrowhead(s)
    double ang, oppang, myangle, x2, y2;

    oppang = getAngle( a, b );
    ang = getAngle( b, a );

    double newang1 = ang + 30.0;
    double newang2 = ang - 30.0;

    double newang3 = oppang + 30.0;
    double newang4 = oppang - 30.0;

    QPoint a1( qRound( b.x() + ( cos( newang1 / MOL_ARAD ) * 10.0 ) ), qRound( b.y() + ( sin( newang1 / MOL_ARAD ) * 10.0 ) ) );
    QPoint a2( qRound( b.x() + ( cos( newang2 / MOL_ARAD ) * 10.0 ) ), qRound( b.y() + ( sin( newang2 / MOL_ARAD ) * 10.0 ) ) );

    QPoint a3( qRound( a.x() + ( cos( newang3 / MOL_ARAD ) * 10.0 ) ), qRound( a.y() + ( sin( newang3 / MOL_ARAD ) * 10.0 ) ) );
    QPoint a4( qRound( a.x() + ( cos( newang4 / MOL_ARAD ) * 10.0 ) ), qRound( a.y() + ( sin( newang4 / MOL_ARAD ) * 10.0 ) ) );

    if ( s1 == ARROW_REGULAR ) {
        drawLine( a, b, th1, c1 );
        drawLine( b, a1, th1, c1 );
        drawLine( b, a2, th1, c1 );
    }
    if ( s1 == ARROW_TOPHARPOON ) {
        drawLine( a, b, th1, c1 );
        if ( a1.y() < a2.y() )
            drawLine( b, a1, th1, c1 );
        else
            drawLine( b, a2, th1, c1 );
    }
    if ( s1 == ARROW_BOTTOMHARPOON ) {
        drawLine( a, b, th1, c1 );
        if ( a1.y() > a2.y() )
            drawLine( b, a1, th1, c1 );
        else
            drawLine( b, a2, th1, c1 );
    }
    if ( s1 == ARROW_MIDDLE ) {
        QPoint m1 = Midpoint( a, b );
        double newang5 = ang + 45.0;
        double newang6 = ang - 45.0;
        double newang7 = oppang + 45.0;
        double newang8 = oppang - 45.0;
        QPoint a5( qRound( m1.x() + ( cos( newang5 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang5 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a6( qRound( m1.x() + ( cos( newang6 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang6 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a7( qRound( m1.x() + ( cos( newang7 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang7 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a8( qRound( m1.x() + ( cos( newang8 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang8 / MOL_ARAD ) * 10.0 ) ) );

        drawLine( a, b, th1, c1 );
        //drawLine(b, a1, th1, c1);
        //drawLine(b, a2, th1, c1);
        drawLine( m1, a5, th1, c1 );
        drawLine( m1, a6, th1, c1 );
        //drawLine(m1, a7, th1, c1);
        //drawLine(m1, a8, th1, c1);
    }
    if ( s1 == ARROW_DIDNT_WORK ) {
        QPoint m1 = Midpoint( a, b );
        double newang5 = ang + 45.0;
        double newang6 = ang - 45.0;
        double newang7 = oppang + 45.0;
        double newang8 = oppang - 45.0;
        QPoint a5( qRound( m1.x() + ( cos( newang5 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang5 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a6( qRound( m1.x() + ( cos( newang6 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang6 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a7( qRound( m1.x() + ( cos( newang7 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang7 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a8( qRound( m1.x() + ( cos( newang8 / MOL_ARAD ) * 10.0 ) ), qRound( m1.y() + ( sin( newang8 / MOL_ARAD ) * 10.0 ) ) );

        drawLine( a, b, th1, c1 );
        drawLine( b, a1, th1, c1 );
        drawLine( b, a2, th1, c1 );
        drawLine( m1, a5, th1, c1 );
        drawLine( m1, a6, th1, c1 );
        drawLine( m1, a7, th1, c1 );
        drawLine( m1, a8, th1, c1 );
    }
    if ( s1 == ARROW_DASH ) {
        drawLine( a, b, th1, c1, 1 );
        drawLine( b, a1, th1, c1 );
        drawLine( b, a2, th1, c1 );
    }
    if ( s1 == ARROW_BI1 ) {
        drawLine( a, b, 1, c1 );
        drawLine( b, a1, 1, c1 );
        drawLine( b, a2, 1, c1 );
        drawLine( a, a3, 1, c1 );
        drawLine( a, a4, 1, c1 );
    }
    if ( s1 == ARROW_BI2 ) {
        myangle = getAngle( a, b );
        myangle += 90.0;
        myangle = myangle * ( M_PI / 180.0 );
        double offs = DistanceBetween( a, b ) / 25.0;

        if ( offs > 2.0 )
            offs = 2.0;
        x2 = cos( myangle ) * 2.5 * offs;
        y2 = sin( myangle ) * 2.5 * offs;
        QPoint sp1( qRound( a.x() + x2 ), qRound( a.y() + y2 ) );
        QPoint ep1( qRound( b.x() + x2 ), qRound( b.y() + y2 ) );
        QPoint sp2( qRound( a.x() - x2 ), qRound( a.y() - y2 ) );
        QPoint ep2( qRound( b.x() - x2 ), qRound( b.y() - y2 ) );
        QPoint a8( qRound( ep2.x() + ( cos( newang1 / MOL_ARAD ) * 10.0 ) ), qRound( ep2.y() + ( sin( newang1 / MOL_ARAD ) * 10.0 ) ) );
        QPoint a9( qRound( sp1.x() + ( cos( newang3 / MOL_ARAD ) * 10.0 ) ), qRound( sp1.y() + ( sin( newang3 / MOL_ARAD ) * 10.0 ) ) );

        drawLine( sp1, ep1, 1, c1 );
        drawLine( ep2, a8, 1, c1 );
        drawLine( sp2, ep2, 1, c1 );
        drawLine( sp1, a9, 1, c1 );
    }
    if ( s1 == ARROW_RETRO ) {
        myangle = getAngle( a, b );
        myangle += 90.0;
        myangle = myangle * ( M_PI / 180.0 );
        double offs = DistanceBetween( a, b ) / 25.0;

        if ( offs > 2.0 )
            offs = 2.0;
        x2 = cos( myangle ) * 4.0 * offs;
        y2 = sin( myangle ) * 4.0 * offs;
        QPoint sp1( qRound( a.x() + x2 ), qRound( a.y() + y2 ) );
        QPoint ep1( qRound( b.x() + x2 ), qRound( b.y() + y2 ) );
        QPoint sp2( qRound( a.x() - x2 ), qRound( a.y() - y2 ) );
        QPoint ep2( qRound( b.x() - x2 ), qRound( b.y() - y2 ) );
        double dx = ep1.x() - sp1.x();
        double dy = ep1.y() - sp1.y();

        dx = dx / DistanceBetween( sp1, ep1 );
        dy = dy / DistanceBetween( sp1, ep1 );
        dx = dx * ( DistanceBetween( sp1, ep1 ) - ( 4.0 * offs ) );
        dy = dy * ( DistanceBetween( sp1, ep1 ) - ( 4.0 * offs ) );
        ep1.setX( qRound( sp1.x() + dx ) );
        ep1.setY( qRound( sp1.y() + dy ) );
        ep2.setX( qRound( sp2.x() + dx ) );
        ep2.setY( qRound( sp2.y() + dy ) );
        double newang5 = ang + 45.0;
        double newang6 = ang - 45.0;
        QPoint a8( qRound( b.x() + ( cos( newang5 / MOL_ARAD ) * 10.0 * offs ) ), qRound( b.y() + ( sin( newang5 / MOL_ARAD ) * 10.0 * offs ) ) );
        QPoint a9( qRound( b.x() + ( cos( newang6 / MOL_ARAD ) * 10.0 * offs ) ), qRound( b.y() + ( sin( newang6 / MOL_ARAD ) * 10.0 * offs ) ) );

        drawLine( sp1, ep1, 1, c1 );
        drawLine( sp2, ep2, 1, c1 );
        drawLine( sp1, sp2, 1, c1 );
        drawLine( b, a8, 1, c1 );
        drawLine( b, a9, 1, c1 );
    }
}

void Render2D::drawBox( QPoint a, QPoint b, QColor c1 )
{
    /*
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setPen( c1 );
    painter->setBrush( QBrush( Qt::NoBrush ) );
    painter->drawRect( QRect( a, b ) );
}

void Render2D::drawFillBox( QPoint a, QPoint b, QColor c1 )
{
    drawFillBox( a, b, c1, false, QColor( 255, 255, 255 ), 0 );
}
void Render2D::drawFillBox( QPoint a, QPoint b, QColor c1, bool border, QColor bordercolor, int style )
{
    if ( outputDevice == OUTPUT_EPS ) {
        a.setY( a.y() + ( 2 * ( selectionBox.center().y() - a.y() ) ) );
        b.setY( b.y() + ( 2 * ( selectionBox.center().y() - b.y() ) ) );
        output_ts << "1.0 1.0 1.0 setrgbcolor";
        output_ts << "newpath";
        output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " moveto";
        output_ts << a.x() - eps_dx << " " << b.y() - eps_dy << " lineto";
        output_ts << b.x() - eps_dx << " " << b.y() - eps_dy << " lineto";
        output_ts << b.x() - eps_dx << " " << a.y() - eps_dy << " lineto";
        output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " lineto";
        output_ts << "closepath fill";
        output_ts << "0.0 0.0 0.0 setrgbcolor";
        return;
    }
    if ( outputDevice == OUTPUT_SVG ) {
        a.setX( a.x() - svg_dx );
        a.setY( a.y() - svg_dy );
        b.setX( b.x() - svg_dx );
        b.setY( b.y() - svg_dy );
        //a.setY(a.y() + (2 * (selectionBox.center().y() - a.y() ) ) );
        //b.setY(b.y() + (2 * (selectionBox.center().y() - b.y() ) ) );
        output_ts << "<rect x=\"" << a.x() << "\" y=\"" << a.y() << "\" width=\"" << b.x() - a.x() << "\" height=\"" << b.y() - a.y()
            << "\" fill=\"" << c1.name() << "\"/>";
        return;
    }
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put line on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       pa->op = OP_FILLBOX;
       pa->a = a;
       pa->b = b;
       pa->c = c1;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setPen( c1 );
    painter->fillRect( QRect( a, b ), c1 );
    if ( border ) {
        if ( style == 0 )
            painter->setPen( bordercolor );
        if ( style == 1 )
            painter->setPen( QPen( bordercolor, 0, Qt::DotLine ) );
        painter->drawRect( QRect( a, b ) );
    }
}

void Render2D::drawCircle( QPoint a, int rad, QColor cfill )
{
    /*
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setBrush( cfill );
    painter->setPen( QColor( 0, 0, 0 ) );
    painter->drawEllipse( a.x(), a.y(), rad * 2, rad * 2 );
    painter->setBrush( QBrush( Qt::NoBrush ) );
}

void Render2D::drawEllipse( QPoint tl, QPoint br, bool drawLine, QColor lineColor, bool drawFill, QColor fillColor )
{
    /*
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    if ( drawFill )
        painter->setBrush( fillColor );
    if ( drawLine )
        painter->setPen( lineColor );
    painter->drawEllipse( QRect( tl, br ) );
    painter->setBrush( QBrush( Qt::NoBrush ) );
}

void Render2D::drawText( QChar ch, QPoint a, QColor c1, QFont f )
{
    qDebug() << "drawText: " << ch;
    int fsize, dy;

    fsize = f.pointSize();
    if ( fsize < 1 )
        fsize = f.pixelSize();
    if ( outputDevice == OUTPUT_EPS ) {
        // calc flip
        a.setY( a.y() + ( 2 * ( selectionBox.center().y() - a.y() ) ) );
        output_ts << "/" << f.family() << " findfont " << fsize;
        output_ts << " scalefont setfont";
        if ( ( rotateOrigin.x() == 0 ) && ( rotateOrigin.y() == 0 ) ) {
            output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " moveto";
            rotateOrigin.setX( a.x() );
            rotateOrigin.setY( a.y() );
        } else {
            dy = a.y() - rotateOrigin.y();
            if ( dy != 0 )
                output_ts << "0 " << dy << " rmoveto";
            if ( ( ch == '(' ) || ( ch == ')' ) )
                output_ts << "(\\" << ch << ") show";
            else
                output_ts << "(" << ch << ") show";
            if ( dy != 0 )
                output_ts << "0 " << -dy << " rmoveto";
            return;
        }
        if ( ( ch == '(' ) || ( ch == ')' ) )
            output_ts << "(\\" << ch << ") show";
        else
            output_ts << "(" << ch << ") show";
        return;
    }
    if ( outputDevice == OUTPUT_SVG ) {
        // calc flip
        //a.setY(a.y() + (2 * (selectionBox.center().y() - a.y() ) ) );
        a.setX( a.x() - svg_dx );
        a.setY( a.y() - svg_dy );
        output_ts << "<text x=\"" << a.x() << "\" y=\"" << a.y() << "\" font-family=\"" << f.family() << "\" font-size=\"" << fsize << "\" fill=\"" << c1.name() << "\">";
        output_ts << ch << "</text>";
        return;
    }
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put line on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       pa->op = OP_TEXT;
       pa->a = a;
       pa->c = c1;
       pa->f = f;
       pa->ch = ch;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setPen( c1 );
    painter->setFont( f );
    painter->drawText( a, ch );
}

void Render2D::drawString( QString s1, QPoint a, QColor c1, QFont f )
{
    if ( outputDevice == OUTPUT_EPS ) {
        // calc flip
        a.setY( a.y() + ( 2 * ( selectionBox.center().y() - a.y() ) ) );
        output_ts << "/" << f.family() << " findfont " << f.pointSize();
        output_ts << " scalefont setfont";
        output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " moveto";
        output_ts << "(" << s1 << ") show";
        return;
    }
    if ( outputDevice == OUTPUT_SVG ) {
        // calc flip
        //a.setY(a.y() + (2 * (selectionBox.center().y() - a.y() ) ) );
        a.setX( a.x() - svg_dx );
        a.setY( a.y() - svg_dy );
        output_ts << "<text x=\"" << a.x() << "\" y=\"" << a.y() << "\" font-family=\"" << f.family() << "\" font-size=\"" << f.pointSize() << "\" fill=\"" << c1.name() << "\">";
        output_ts << s1 << "</text>";
        return;
    }
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put line on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       pa->op = OP_STRING;
       pa->a = a;
       pa->st = s1;
       pa->f = f;
       pa->c = c1;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setPen( c1 );
    painter->setFont( f );
    painter->drawText( a, s1 );
}

void Render2D::drawTextReverse( QChar ch, QPoint a, QColor c1, QFont f )
{
    /*
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->setPen( c1 );
    painter->setFont( f );
    QFontMetrics fm = painter->fontMetrics();
    QPoint topleft( a.x(), a.y() - fm.ascent() );
    QPoint bottomright( a.x() + fm.width( ch ), a.y() + fm.descent() );
    QRect fr( topleft, bottomright );

    painter->fillRect( fr, c1 );
    painter->setPen( bgcolor );
    painter->drawText( a, ch );
}

void Render2D::drawPixmap( QPoint a, QPixmap pix )
{
    if ( outputDevice == OUTPUT_EPS ) {
        // calc flip
        a.setY( a.y() + ( 2 * ( selectionBox.center().y() - a.y() ) ) - 16 );
        output_ts << "20 dict begin";
        output_ts << "/picstr 16 string def";
        output_ts << "gsave";
        output_ts << a.x() - eps_dx << " " << a.y() - eps_dy << " translate";
        output_ts << "16 16 scale";
        output_ts << "16 16 1";
        output_ts << "[16 0 0 -16 0 16]";
        output_ts << "{currentfile picstr readhexstring pop}";
        output_ts << "image";
        QImage im1 = pix.toImage();

        qDebug() << im1.width() << ":" << im1.height();
        long sumtotal;
        int row, col;

        for ( row = 0; row < im1.height(); row++ ) {
            sumtotal = 0;
            for ( col = 0; col < im1.width(); col++ ) {
                if ( qGray( im1.pixel( col, row ) ) > 127 ) {
                    sumtotal += ( long ) pow( 2.0, ( double ) ( 15 - col ) );
                }
            }
            output_ts << hex << sumtotal << dec;
        }
        output_ts << "end" << endl << "grestore" << endl;
        return;
    }
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put line on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       pa->op = OP_PIXMAP;
       pa->a = a;
       pa->p = pix;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
    painter->drawPixmap( a, pix );
}

void Render2D::drawCurveArrow( QPoint a, QPoint b, QColor c1, QString wh )
{
    /*
       if (outputDevice == OUTPUT_PRINTER) {
       // put curve on paintqueue to render in Print()
       Paintable *pa = new Paintable;
       if (wh == "CW90")
       pa->op = OP_CURVE_CW90;
       if (wh == "CCW90")
       pa->op = OP_CURVE_CCW90;
       if (wh == "CW180")
       pa->op = OP_CURVE_CW180;
       if (wh == "CCW180")
       pa->op = OP_CURVE_CCW180;
       if (wh == "CW270")
       pa->op = OP_CURVE_CW270;
       if (wh == "CCW270")
       pa->op = OP_CURVE_CCW270;
       pa->a = a;
       pa->b = b;
       pa->c = c1;
       paintqueue.append(pa);
       return;
       }
       if (directdraw)
       p.begin(this);
       else
       p.begin(&dbuffer);
       p.scale(zoomFactor, zoomFactor);
     */
  QPainterPath pp;
  painter->setPen( QPen(c1, 1) );
  if ( wh == "CW180" ) {
    // calculate curve
    QPoint ce = Midpoint( a, b );
    int d = RoundOff( DistanceBetween( a, ce ) );
    double sa = getAngle( ce, a );

    pp.moveTo( a );
    pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound(-sa), -180 );
    painter->drawPath( pp );
    //painter->drawArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound(-sa * 16), -180 * 16 );
    //painter->drawArc( 0, 0, 100, 100, 0, 180 );
    //drawPolyline( pp, c1 );
    // calculate arrowhead
    // if curve too small, don't draw arrowhead
    QPoint realb( pp.currentPosition().toPoint() );
    sa = getAngle(b, a);
    double newang1 = sa + 60.0;
    double newang2 = sa + 120.0;
    QPoint a1( qRound(realb.x() + (cos(newang1/MOL_ARAD) * 10.0)),
	       qRound(realb.y() + (sin(newang1/MOL_ARAD) * 10.0)) );
    QPoint a2( qRound(realb.x() + (cos(newang2/MOL_ARAD) * 10.0)),
	       qRound(realb.y() + (sin(newang2/MOL_ARAD) * 10.0)) );
    drawLine(realb, a1, 1, c1);
    drawLine(realb, a2, 1, c1);
    return;
  }
    if ( wh == "CCW180" ) {
      // calculate curve
      QPoint ce = Midpoint( a, b );
      int d = RoundOff( DistanceBetween( a, ce ) );
      double sa = getAngle( ce, a );
      
      pp.moveTo( a );
      pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound(-sa), 180 );
      painter->drawPath( pp );
      //painter->drawArc( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound(-sa * 16), 180 * 16 );
        // calculate arrowhead
        // if curve too small, don't draw arrowhead
      QPoint realb( pp.currentPosition().toPoint() );
      sa = getAngle(b, a);
      double newang1 = sa + 240.0;
      double newang2 = sa + 300.0;
      QPoint a1( qRound(realb.x() + (cos(newang1/MOL_ARAD) * 10.0)),
		 qRound(realb.y() + (sin(newang1/MOL_ARAD) * 10.0)) );
      QPoint a2( qRound(realb.x() + (cos(newang2/MOL_ARAD) * 10.0)),
		 qRound(realb.y() + (sin(newang2/MOL_ARAD) * 10.0)) );
      drawLine(realb, a1, 1, c1);
      drawLine(realb, a2, 1, c1);
      return;
    }
    if ( wh == "CW90" ) {
        // first, figure out where middle of circle is
        double d1 = DistanceBetween( a, b );
        double ia = getAngle( a, b );

        d1 = d1 / 1.4142136;
        ia = 45.0;
        double dx1 = ( ( double ) b.x() - ( double ) a.x() ) / 1.4142136;
        double dy1 = ( ( double ) b.y() - ( double ) a.y() ) / 1.4142136;

        // rotate vector (dx1, dy1) 45 degrees clockwise
        double ia_rad = ia * M_PI / 180.0;
        double dx2 = dx1 * cos( ia_rad ) - dy1 * sin( ia_rad );
        double dy2 = dx1 * sin( ia_rad ) + dy1 * cos( ia_rad );
        QPoint ce( a.x() + RoundOff( dx2 ), a.y() + RoundOff( dy2 ) );
        double sa = getAngle( ce, a );
        int d = RoundOff( d1 );

	pp.moveTo( a );
	pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound( -sa ), -90 );
        drawPolyline( pp, c1 );
        //painter->drawChord( QRect( a, b ), 180 * 16, -90 * 16 );
        // calculate arrowhead
        // if curve too small, don't draw arrowhead
	//     if (pa.count() == 0) return;
	//QPoint realb(pa.at(pa.count() - 1));
	sa = getAngle(b, a);
	double newang1 = sa + 15.0;
	double newang2 = sa + 75.0;
	QPoint a1( qRound(b.x() + (cos(newang1/MOL_ARAD) * 10.0)),
		   qRound(b.y() + (sin(newang1/MOL_ARAD) * 10.0)) );
	QPoint a2( qRound(b.x() + (cos(newang2/MOL_ARAD) * 10.0)),
		   qRound(b.y() + (sin(newang2/MOL_ARAD) * 10.0)) );
	drawLine(b, a1, 1, c1);
	drawLine(b, a2, 1, c1);
    }
    if ( wh == "CCW90" ) {
        // first, figure out where middle of circle is
        double d1 = DistanceBetween( a, b );
        double ia = getAngle( a, b );

        d1 = d1 / 1.4142136;
        ia = -45.0;
        double dx1 = ( ( double ) b.x() - ( double ) a.x() ) / 1.4142136;
        double dy1 = ( ( double ) b.y() - ( double ) a.y() ) / 1.4142136;

        // rotate vector (dx1, dy1) 45 degrees clockwise
        double ia_rad = ia * M_PI / 180.0;
        double dx2 = dx1 * cos( ia_rad ) - dy1 * sin( ia_rad );
        double dy2 = dx1 * sin( ia_rad ) + dy1 * cos( ia_rad );
        QPoint ce( a.x() + RoundOff( dx2 ), a.y() + RoundOff( dy2 ) );
        double sa = getAngle( ce, a );
        int d = RoundOff( d1 );

	pp.moveTo( a );
        pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound( -sa ), 90 );
        drawPolyline( pp, c1 );
        // calculate arrowhead
        // if curve too small, don't draw arrowhead
	//    if (pa.count() == 0) return;
	//QPoint realb(pa.at(pa.count() - 1));
	sa = getAngle(b, a);
	double newang1 = sa - 15.0;
	double newang2 = sa - 75.0;
	QPoint a1( qRound(b.x() + (cos(newang1/MOL_ARAD) * 10.0)),
		   qRound(b.y() + (sin(newang1/MOL_ARAD) * 10.0)) );
	QPoint a2( qRound(b.x() + (cos(newang2/MOL_ARAD) * 10.0)),
		   qRound(b.y() + (sin(newang2/MOL_ARAD) * 10.0)) );
	drawLine(b, a1, 1, c1);
	drawLine(b, a2, 1, c1);
    }
    if ( wh == "CW270" ) {
        // first, figure out where middle of circle is
        double d1 = DistanceBetween( a, b );
        double ia = getAngle( a, b );

        d1 = d1 / 1.4142136;
        ia = -45.0;
        double dx1 = ( ( double ) b.x() - ( double ) a.x() ) / 1.4142136;
        double dy1 = ( ( double ) b.y() - ( double ) a.y() ) / 1.4142136;

        // rotate vector (dx1, dy1) 45 degrees clockwise
        double ia_rad = ia * M_PI / 180.0;
        double dx2 = dx1 * cos( ia_rad ) - dy1 * sin( ia_rad );
        double dy2 = dx1 * sin( ia_rad ) + dy1 * cos( ia_rad );
        QPoint ce( a.x() + RoundOff( dx2 ), a.y() + RoundOff( dy2 ) );
        double sa = getAngle( ce, a );
        int d = RoundOff( d1 );

	pp.moveTo( a );
        pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound( -sa ), -270 );
        drawPolyline( pp, c1 );
        // calculate arrowhead
        // if curve too small, don't draw arrowhead
	//    if (pa.count() == 0) return;
	//QPoint realb(pa.at(pa.count() - 1));
	QPoint realb( pp.currentPosition().toPoint() );
	sa = getAngle(b, a);
	double newang1 = sa + 165.0;
	double newang2 = sa + 105.0;
	QPoint a1( qRound(realb.x() + (cos(newang1/MOL_ARAD) * 10.0)),
		   qRound(realb.y() + (sin(newang1/MOL_ARAD) * 10.0)) );
	QPoint a2( qRound(realb.x() + (cos(newang2/MOL_ARAD) * 10.0)),
		   qRound(realb.y() + (sin(newang2/MOL_ARAD) * 10.0)) );
	drawLine(realb, a1, 1, c1);
	drawLine(realb, a2, 1, c1);
    }
    if ( wh == "CCW270" ) {
        // first, figure out where middle of circle is
        double d1 = DistanceBetween( a, b );
        double ia = getAngle( a, b );

        d1 = d1 / 1.4142136;
        ia = 45.0;
        double dx1 = ( ( double ) b.x() - ( double ) a.x() ) / 1.4142136;
        double dy1 = ( ( double ) b.y() - ( double ) a.y() ) / 1.4142136;

        // rotate vector (dx1, dy1) 45 degrees clockwise
        double ia_rad = ia * M_PI / 180.0;
        double dx2 = dx1 * cos( ia_rad ) - dy1 * sin( ia_rad );
        double dy2 = dx1 * sin( ia_rad ) + dy1 * cos( ia_rad );
        QPoint ce( a.x() + RoundOff( dx2 ), a.y() + RoundOff( dy2 ) );
        double sa = getAngle( ce, a );
        int d = RoundOff( d1 );

	pp.moveTo( a );
        pp.arcTo( ce.x() - d, ce.y() - d, 2 * d, 2 * d, qRound( -sa ), 270 );
        drawPolyline( pp, c1 );
        // calculate arrowhead
        // if curve too small, don't draw arrowhead
	//    if (pa.count() == 0) return;
	//QPoint realb(pa.at(pa.count() - 1));
	QPoint realb( pp.currentPosition().toPoint() );
	sa = getAngle(b, a);
	double newang1 = sa - 165.0;
	double newang2 = sa - 105.0;
	QPoint a1( qRound(realb.x() + (cos(newang1/MOL_ARAD) * 10.0)),
		   qRound(realb.y() + (sin(newang1/MOL_ARAD) * 10.0)) );
	QPoint a2( qRound(realb.x() + (cos(newang2/MOL_ARAD) * 10.0)),
		   qRound(realb.y() + (sin(newang2/MOL_ARAD) * 10.0)) );
	drawLine(realb, a1, 1, c1);
	drawLine(realb, a2, 1, c1);
    }
}

// Make QPixmap of this drawing.
// just the drawing if page_size set to paper (LETTER, LEGAL, A4, ...)
// whole area if set to a screen size (640, 800, 1024)
QPixmap Render2D::MakePixmap( bool transp )
{
    double oldspace = preferences.getDoubleBondOffset();

    preferences.setDoubleBondOffset( oldspace * 2.0 );
    font_size_kludge = true;
    prev_mode = mode;
    QRect r;

    c->SelectAll();

    DPoint dp1;

    selectionBox = c->selectionBox();
    QRect finalsize = selectionBox;

    dp1.set( selectionBox.topLeft() );
    c->Resize( &dp1, 2.0 );

    if ( preferences.getPageSize() < PAGE_640 )
        r = c->selectionBox();
    else
        r = QRect( 0, 0, renderWidth, renderHeight );
    c->DeselectAll();
    mode = MODE_SELECT;         // so selection box will not appear
    finishedPainting = false;
    update();
    r.setTopLeft( zoomCorrect( r.topLeft() ) );
    r.setBottomRight( zoomCorrect( r.bottomRight() ) );
    QPixmap pm( r.size() );

    qDebug() << "X1:" << r.left() << " Y1:" << r.top();
    qDebug() << "W:" << r.width() << " H:" << r.height();

    while ( !finishedPainting ) {
        qDebug() << "Waiting...";
    }
    pm = QPixmap::grabWidget( this, r.left(), r.top(), r.width(), r.height() );
    //bitBlt( &pm, 0, 0, this, r.left(), r.top(), r.width(), r.height() );

    pm = pm.scaled( finalsize.width(), finalsize.height(), Qt::KeepAspectRatio, Qt::SmoothTransformation );

    if ( transp ) {
        QBitmap bm( r.size() );

        bm = pm;
        QImage bi;

        bi = bm.toImage();
        //bi.invertPixels();
        bm = QBitmap::fromImage( bi );
        pm.setMask( bm );
    }

    mode = prev_mode;

    c->SelectAll();
    selectionBox = c->selectionBox();
    dp1.set( selectionBox.topLeft() );
    c->Resize( &dp1, 0.5 );
    c->DeselectAll();
    font_size_kludge = false;
    preferences.setDoubleBondOffset( oldspace );

    return pm;
}

// Make QPixmap of this drawing.
QPixmap Render2D::MakeFullPixmap()
{
    c->DeselectAll();
    mode = MODE_SELECT;         // so selection box will not appear
    update();
    QPixmap pm( size() );

    pm = QPixmap::grabWidget( this, rect() );

    return pm;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
