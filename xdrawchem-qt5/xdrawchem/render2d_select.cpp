#include <QMouseEvent>

#include "render2d.h"
#include "chemdata.h"
#include "text.h"
#include "defs.h"

void Render2D::Select_mouseMoveEvent( QMouseEvent * e1 )
{
    // MODE_SELECT_LASSO handled slightly differently
    if ( ( mode == MODE_SELECT_LASSO ) && ( mouse1down == false ) )
        return;
    if ( ( mode == MODE_SELECT_LASSO ) && ( mouse1down == true ) ) {
        tmp_lasso.append( e1->pos() );
        update();
        directdraw = true;
//    painter->begin(this);
        drawPolyline( tmp_lasso, QColor( 0, 0, 255 ) );
//    painter->end();
        directdraw = false;
        return;
    }
    //bool update;
    DPoint *prevhighlight = highlightpoint;
    Drawable *prevhighlightobject = highlightobject;

    // Create DPoint of current pointer position
    QPoint curqpt = zoomCorrectReverse( e1->pos() );
    DPoint e( curqpt.x(), curqpt.y() );
    DPoint *np = 0;

    double dist, distobj, dx, dy;

    // Get DPoint of nearest point
    np = c->FindNearestPoint( &e, dist );
    // get Drawable of nearest object
    Drawable *no = c->FindNearestObject( &e, distobj );

    //qDebug() << "np = " << dist << " no = " << distobj;
    //if (np == 0) qDebug() << " (np = 0)";
    //qDebug() ;
    // no buttons down
    if ( ( mouse1down == false ) && ( mouse3down == false ) ) {
        if ( mode == MODE_SELECT_MULTIPLE_SELECTED ) {
            // check if near rotate or resize handle
            if ( DistanceBetween( resizeHandle, curqpt ) < 4.0 ) {
                setCursor( Qt::SizeFDiagCursor );
                mode = MODE_SELECT_MULTIPLE_RESIZE;
                return;
            }
            if ( DistanceBetween( rotateHandle, curqpt ) < 4.0 ) {
                setCursor( rotatecursor );
                mode = MODE_SELECT_MULTIPLE_ROTATE;
                return;
            }
            // else, check if in selection box (for move)
            if ( selectionBox.contains( curqpt ) )
                setCursor( Qt::SizeAllCursor );
            else
                setCursor( Qt::ArrowCursor );
            return;
        }
        if ( mode == MODE_SELECT_MULTIPLE_RESIZE ) {
            if ( DistanceBetween( resizeHandle, e1->pos() ) > 4.0 ) {
                mode = MODE_SELECT_MULTIPLE_SELECTED;
                if ( selectionBox.contains( e1->pos() ) )
                    setCursor( Qt::SizeAllCursor );
                else
                    setCursor( Qt::ArrowCursor );
                return;
            }
            return;             // do nothing if still on resize handle
        }
        if ( mode == MODE_SELECT_MULTIPLE_ROTATE ) {
            if ( DistanceBetween( rotateHandle, e1->pos() ) > 4.0 ) {
                mode = MODE_SELECT_MULTIPLE_SELECTED;
                if ( selectionBox.contains( e1->pos() ) )
                    setCursor( Qt::SizeAllCursor );
                else
                    setCursor( Qt::ArrowCursor );
                return;
            }
            return;             // do nothing if still on resize handle
        }
        if ( no != 0 ) {
            // highlight text and symbol objects preferentially when MODE_SELECT...
            if ( ( no->Type() == TYPE_TEXT ) || ( no->Type() == TYPE_SYMBOL ) ) {
                highlightpoint = 0;
                highlightobject = no;
                if ( ( prevhighlightobject != highlightobject ) && ( prevhighlightobject != 0 ) )
                    prevhighlightobject->Highlight( false );
                highlightobject->Highlight( true );
                if ( prevhighlightobject != highlightobject )
                    update();
                setCursor( Qt::SizeAllCursor );
                // return since no need to check points
                return;
            }
            // highlight object if object close enough and no point close
            if ( ( distobj < 6.0 ) && ( dist >= 8.0 ) ) {
                highlightpoint = 0;
                highlightobject = no;
                if ( prevhighlightobject != 0 )
                    prevhighlightobject->Highlight( false );
                highlightobject->Highlight( true );
                if ( prevhighlightobject != highlightobject )
                    update();
                setCursor( Qt::SizeAllCursor );
                // return since no need to check points
                return;
            }
            // unhighlight object if no object close
            if ( distobj >= 6.0 ) {
                // Clear highlighted object
                highlightobject = 0;
                if ( prevhighlightobject != 0 )
                    prevhighlightobject->Highlight( false );
                if ( prevhighlightobject != highlightobject )
                    update();
                // don't return; go on to check for points
            }
            // unhighlight object if point close
            if ( dist < 8.0 ) {
                // Clear highlighted object
                highlightobject = 0;
                if ( prevhighlightobject != 0 )
                    prevhighlightobject->Highlight( false );
                if ( prevhighlightobject != highlightobject )
                    update();
                // don't return; go on to check for points
            }
        }
        if ( np == 0 ) {
            highlightpoint = 0;
            highlightobject = 0;
            if ( prevhighlightobject != 0 ) {
                prevhighlightobject->Highlight( false );
                update();
            }
            setCursor( Qt::ArrowCursor );
            return;
        }
        if ( np != 0 ) {
            //qDebug() << dist << " to (" << np->x << "," << np->y << ")" ;
            if ( dist < 8.0 ) {
                highlightpoint = np;
                if ( prevhighlight != highlightpoint )
                    update();
                setCursor( Qt::SizeAllCursor );
                return;
            } else {
                // Clear object, if any
                if ( prevhighlightobject != 0 ) {
                    prevhighlightobject->Highlight( false );
                    highlightobject = 0;
                    update();
                }
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                setCursor( Qt::ArrowCursor );
                return;
            }
        }
        return;                 // rest of function only if mouse button down
    }

    if ( ( mode == MODE_SELECT_MULTIPLE ) && ( mouse1down ) ) {
        drawBox( zoomCorrectReverse( selectOrigin ), zoomCorrectReverse( selectPrev ), QColor( 255, 255, 255 ) );
        selectPrev = e1->pos();
        c->NewSelectRect( QRect( zoomCorrectReverse( selectOrigin ), zoomCorrectReverse( selectPrev ) ), false );
        update();
        directdraw = true;
//    painter->begin(this);
        drawBox( zoomCorrectReverse( selectOrigin ), zoomCorrectReverse( selectPrev ), QColor( 0, 0, 0 ) );
//    painter->end();
        directdraw = false;
    }

    if ( ( mode == MODE_SELECT_RESIZE_MULTIPLE ) && ( mouse1down ) ) {
        //double dh, dw;
        DPoint dp1( selectionBox.topLeft() );

        // disallow moves to the left or above selection box
        if ( ( e.x < ( dp1.x + 2.0 ) ) || ( e.y < ( dp1.y + 2.0 ) ) ) {
            qDebug() << "Warning!";
            QPoint gin( ( int ) prevpos->x, ( int ) prevpos->y ), gout;

            gout = mapToGlobal( gin );
            cursor().setPos( gout );
            return;
        }
        dx = dp1.distanceTo( &e );
        dy = dp1.distanceTo( prevpos );
        //qDebug() << "resize factor " << dx / dy ;
        c->Resize( &dp1, dx / dy );
        selectionBox = c->selectionBox();
        update();
    }

    if ( ( mode == MODE_SELECT_ROTATE_MULTIPLE ) && ( mouse1down ) ) {
        //DPoint dp1( rotateOrigin );
        DPoint *dp1 = new DPoint;

        dp1->x = rotateOrigin.x();
        dp1->y = rotateOrigin.y();
        double da = getAngle( rotateOrigin, e1->pos() );
        double db = prevangle - da;

        if ( db < -300.0 )
            db += 360.0;
        if ( db > 300.0 )
            db -= 360.0;
        db = db * M_PI / 180.0;
        //qDebug() << db ;
        prevangle = da;
        c->Rotate( dp1, db );
        selectionBox = c->selectionBox();
        update();
        delete dp1;
    }

    if ( ( mode == MODE_SELECT_MOVE_MULTIPLE ) && ( mouse1down ) ) {
        dx = e.x - prevpos->x;
        dy = e.y - prevpos->y;
        //qDebug() << "Move by " << dx << "," << dy ;
        c->Move( dx, dy );
        selectionBox.translate( ( int ) dx, ( int ) dy );
        update();
    }

    if ( ( mode == MODE_SELECT_MOVE_OBJECT ) && ( mouse1down ) ) {
        dx = e.x - prevpos->x;
        dy = e.y - prevpos->y;
        //qDebug() << "Move by " << dx << "," << dy ;
        c->Move( dx, dy );
        selectionBox.translate( ( int ) dx, ( int ) dy );
        update();
    }

    if ( ( mode == MODE_SELECT_MOVE_POINT ) && ( mouse1down ) ) {
        dx = e.x - prevpos->x;
        dy = e.y - prevpos->y;
        //qDebug() << "Move by " << dx << "," << dy ;
        highlightpoint->x += dx;
        highlightpoint->y += dy;
        update();
    }
}

// Get angle between -BA> and -BD> (Law of Cosines)
double Render2D::GetAngle( DPoint * a, DPoint * b, DPoint * d )
{
    double a1 = b->distanceTo( a );
    double a2 = b->distanceTo( d );
    double a3 = a->distanceTo( d );
    double cos1 = ( a2 * a2 + a1 * a1 - a3 * a3 ) / ( 2 * a1 * a2 );

    return acos( cos1 );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
