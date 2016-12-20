#include <QPainter>
#include <QMouseEvent>
#include <QMenu>
#include <QAction>
#include <QMessageBox>
#include <QColorDialog>
#include <QPrinter>
#include <QDrag>
#include <QMimeData>

#include "render2d.h"
#include "chemdata.h"
#include "drawable.h"
#include "molecule.h"
#include "text.h"
#include "bracket.h"
#include "defs.h"
#include "xdc_event.h"
#include "molinfodialog.h"
#include "textshapedialog.h"

// move point to grid if needed
QPoint Render2D::GridLock( QPoint qp1 )
{
    if ( mode < 200 )
        return qp1;
    if ( mode > 299 )
        return qp1;

    int d1 = qRound( ( double ) preferences.getGridSpacing() * zoomFactor );

    if ( preferences.getDrawGrid() == GRID_SQUARE ) {
        double gs = ( double ) d1, gx = ( double ) qp1.x(), gy = ( double ) qp1.y();
        int rx, ry;

        rx = qRound( gx / gs );
        ry = qRound( gy / gs );
        qp1.setX( rx * d1 );
        qp1.setY( ry * d1 );

        update();
        directdraw = true;
//      painter->begin(this);
        drawBox( QPoint( qp1.x() - 2, qp1.y() - 2 ), QPoint( qp1.x() + 2, qp1.y() + 2 ), QColor( 255, 0, 0 ) );
//      painter->end();
        directdraw = false;

        return qp1;
    }

    if ( preferences.getDrawGrid() == GRID_HEX ) {
        bool hexoffset = true;
        double hexhoriz = ( double ) d1 / 2.0;
        double hexgap = 2.0 * hexhoriz + ( double ) d1;
        double hexelevation = hexhoriz * 1.73205;
        double hex_initx, hex_rightx, dx, dy;
        double maxdist = 9999.0, max_x = 0.0, max_y = 0.0;

        for ( double hex_y = 0.0; hex_y < renderHeight; hex_y += hexelevation ) {
            if ( hexoffset )
                hex_initx = hexhoriz;
            else
                hex_initx = hexgap;
            for ( double hex_x = hex_initx; hex_x < renderWidth; hex_x += hexgap ) {
                hex_rightx = hex_x + ( double ) d1;
                dx = hex_x - qp1.x();
                dy = hex_y - qp1.y();
                if ( sqrt( dx * dx + dy * dy ) < maxdist ) {
                    max_x = hex_x;
                    max_y = hex_y;
                    maxdist = sqrt( dx * dx + dy * dy );
                }
                hex_x = hex_rightx;
                dx = hex_x - qp1.x();
                dy = hex_y - qp1.y();
                if ( sqrt( dx * dx + dy * dy ) < maxdist ) {
                    max_x = hex_x;
                    max_y = hex_y;
                    maxdist = sqrt( dx * dx + dy * dy );
                }
            }
            hexoffset = !hexoffset;
        }
        qp1.setX( ( int ) max_x );
        qp1.setY( ( int ) max_y );
        return qp1;
    }

    return qp1;
}

// right-click popup choices from mousePressEvent popup (rtclickpopup)
void Render2D::molInfo()
{
    qDebug() << "Popup-info";
    DPoint *tmp_pt2 = new DPoint;

    tmp_pt2->set( targetmol->BoundingBoxAll().center() );
    c->Tool( tmp_pt2, MODE_TOOL_MOLECULE_INFO );
}

void Render2D::molSelect()
{
    DeselectAll();
    targetmol->SelectAll();
    selectionBox = c->selectionBox();
    mode = MODE_SELECT_MULTIPLE_SELECTED;
    update();
}

void Render2D::molCut()
{
    DeselectAll();
    targetmol->SelectAll();
    Cut();
    qDebug() << "popup-Cut";
}

void Render2D::molCopy()
{
    DeselectAll();
    targetmol->SelectAll();
    Copy();
    DeselectAll();
    qDebug() << "popup-Copy";
}

void Render2D::bondEdit()
{
    if ( highlightobject->Type() == TYPE_TEXT ) {
        doubleClickFlag = true;
        prev_mode = mode;
        mode = MODE_TEXT;
        DrawText_mousePressEvent( 0, QPoint( 0, 0 ) );
        return;
    }
    highlightobject->Edit();
    highlightobject->Highlight( false );
    highlightobject = 0;
    if ( mode == MODE_SELECT )
        setCursor( Qt::ArrowCursor );
    else
        setCursor( Qt::CrossCursor );
    mouse3down = false;
    update();
}

void Render2D::bondInfo()
{
    QString objinf;

    objinf = tr( "No information." );

    if ( highlightobject->Type() == TYPE_BOND ) {
        objinf = "ID: ";
        objinf.append( ( ( Bond * ) highlightobject )->CName() );
        objinf.append( "\nReactions: " );
        objinf.append( ( ( Bond * ) highlightobject )->getReactions() );
        // make it easier to add to retro.txt
        qDebug() << "ID:" << ( ( Bond * ) highlightobject )->CName();
    }

    QMessageBox::information( 0, tr( "Object information" ), objinf );
}

void Render2D::textShape()
{
    localtext = ( Text * ) highlightobject;
    TextShapeDialog *tsd = new TextShapeDialog( this );

    tsd->set_stype( localtext->getShape() );
    tsd->setBorderCheck( localtext->getBorder() );
    tsd->setFillCheck( localtext->getFill() );
    tsd->setBorderColor( localtext->getBorderColor() );
    tsd->setFillColor( localtext->getFillColor() );
    tsd->setWidth( localtext->getShapeWidth() );
    tsd->setHeight( localtext->getShapeHeight() );

    if ( tsd->exec() ) {
        qDebug() << "textShape OK";
    }

    localtext->setShape( tsd->get_stype() );
    localtext->setBorder( tsd->getBorderCheck() );
    localtext->setFill( tsd->getFillCheck() );
    localtext->setBorderColor( tsd->getBorderColor() );
    localtext->setFillColor( tsd->getFillColor() );
    localtext->setShapeWidth( tsd->getWidth() );
    localtext->setShapeHeight( tsd->getHeight() );

    delete tsd;

    localtext = 0;
}

void Render2D::bracketFill()
{
    Bracket *lbracket = ( Bracket * ) highlightobject;
    QColor b1color = lbracket->getFillColor();

    b1color = QColorDialog::getColor( b1color );

    if ( b1color.isValid() == true ) {
        lbracket->setFill( true );
        lbracket->setFillColor( b1color );
    }
}

void Render2D::bracketFillOff()
{
    Bracket *lbracket = ( Bracket * ) highlightobject;

    lbracket->setFill( false );
}

void Render2D::mousePressEvent( QMouseEvent * e1 )
{
    qDebug() << "mousePressEvent begin; mode: " << mode;
    if ( localstring == "XDC_DELETE_ME" ) {
        delete localtexteditor;

        localtexteditor = 0;
        localstring = " ";
    }

    QPoint curqpt = zoomCorrectReverse( e1->pos() );

    if ( preferences.getSnapGrid() != GRID_NONE ) {
        curqpt = GridLock( curqpt );
    }

    bool allowedit = false;

    moved = false;

    DPoint tmp_pt( curqpt.x(), curqpt.y() );
    DPoint *tmp_pt2 = new DPoint;

    tmp_pt2->x = curqpt.x();
    tmp_pt2->y = curqpt.y();

    if ( mode > 100 )
        c->StartUndo( mode, 0 );        // BMS: StartUndo does nothing with the point - don't copy!
    //qDebug() << "Click" ;
    // update button state
    if ( e1->button() == Qt::LeftButton )
        mouse1down = true;
    if ( e1->button() == Qt::RightButton ) {
        mouse3down = true;
    }
    //qDebug() << "start at " << curqpt.x() << "," << curqpt.y() ;

    // can the current mode use right button popup menu?
    bool rbm = false;

    if ( mode == MODE_SELECT || mode == MODE_SELECT_MOVE_OBJECT || mode == MODE_DRAWBRACKET )
        rbm = true;

    if ( rbm && mouse3down ) {
        //if (rtclickpopup != 0) delete rtclickpopup;
        rtclickpopup = new QMenu( this );
        bool menu_ok = false;

        if ( ( mode == MODE_DRAWBRACKET ) && ( highlightobject == 0 ) ) {
            // put nearest bracket into highlightobject
            DPoint *e = new DPoint;

            e->x = curqpt.x();
            e->y = curqpt.y();
            double distobj;

            // get Drawable of nearest object
            Drawable *no = c->FindNearestObject( e, distobj );

            if ( ( distobj < 5.0 ) && ( no->Type() == TYPE_BRACKET ) )
                highlightobject = no;
        }

        if ( highlightobject != 0 ) {
            QString rtmt;

            rtmt = tr( "Object" );
            if ( highlightobject->Type() == TYPE_ARROW )
                rtmt = tr( "Arrow" );
            else if ( highlightobject->Type() == TYPE_BOND )
                rtmt = tr( "Bond" );
            else if ( highlightobject->Type() == TYPE_BRACKET )
                rtmt = tr( "Bracket" );
            else if ( highlightobject->Type() == TYPE_CURVEARROW )
                rtmt = tr( "Curved Arrow" );
            else if ( highlightobject->Type() == TYPE_SYMBOL )
                rtmt = tr( "Symbol" );
            else if ( highlightobject->Type() == TYPE_TEXT )
                rtmt = tr( "Text" );
            QAction *title = new QAction( "test", this );
            title->setEnabled(false);
            QFont f = title->font();
//            rtclickpopup->setFont( QFont("Helvetica", 12, QFont::Bold) );
            rtclickpopup->addAction( title );
            rtclickpopup->addSeparator();
            rtclickpopup->addAction( tr( "Edit" ), this, SLOT( bondEdit() ) );
            if ( highlightobject->Type() == TYPE_BOND ) {
                rtclickpopup->addAction( tr( "Info" ), this, SLOT( bondInfo() ) );
            } else if ( highlightobject->Type() == TYPE_BRACKET ) {
                rtclickpopup->addAction( tr( "Fill color..." ), this, SLOT( bracketFill() ) );
                rtclickpopup->addAction( tr( "No fill" ), this, SLOT( bracketFillOff() ) );
            } else if ( highlightobject->Type() == TYPE_TEXT ) {
                rtclickpopup->addAction( tr( "Shape..." ), this, SLOT( textShape() ) );
            }
            menu_ok = true;
        }

        targetmol = c->insideMolecule( tmp_pt2 );
        if ( targetmol != 0 ) {
            rtclickpopup->setTitle( tr( "Molecule" ) );
            rtclickpopup->addAction( tr( "Info" ), this, SLOT( molInfo() ) );
            rtclickpopup->addAction( tr( "Select" ), this, SLOT( molSelect() ) );
            rtclickpopup->addAction( tr( "Cut" ), this, SLOT( molCut() ) );
            rtclickpopup->addAction( tr( "Copy" ), this, SLOT( molCopy() ) );
            menu_ok = true;
        }
        mouse3down = false;
        if ( menu_ok )
            rtclickpopup->popup( mapToGlobal( e1->pos() ) );
    }
    // MODE_TEXT with right button
    if ( ( mode == MODE_TEXT ) && mouse3down ) {
        rtclickpopup = new QMenu( this );
        int id1;
        bool menu_ok = false;

        if ( highlightobject != 0 ) {
            QString rtmt;

            rtmt = tr( "Object" );
            if ( highlightobject->Type() == TYPE_TEXT )
                rtmt = tr( "Text" );
            rtclickpopup->addAction( rtmt );
            rtclickpopup->addAction( tr( "Edit" ), this, SLOT( bondEdit() ) );
            if ( highlightobject->Type() == TYPE_TEXT )
                rtclickpopup->addAction( tr( "Shape..." ), this, SLOT( textShape() ) );
            menu_ok = true;
        }
        mouse3down = false;
        if ( menu_ok ) {
            rtclickpopup->popup( mapToGlobal( e1->pos() ) );
            return;
        }
    }
    // MODE_TEXT: call special function to handle text events
    if ( mode == MODE_TEXT ) {

        qDebug() << "mode_text 1";
        DrawText_mousePressEvent( e1, curqpt );
        qDebug() << "mode_text 2";
        return;
    }
    // MODE_SELECT_LASSO: start lasso
    if ( mode == MODE_SELECT_LASSO ) {
        c->DeselectAll();
        tmp_lasso.clear();
        tmp_lasso.append( curqpt );
        return;
    }
    // MODE_SELECT: start multiple selection
    if ( ( mode == MODE_SELECT ) && ( mouse1down ) ) {
        if ( ( highlightpoint == 0 ) && ( highlightobject == 0 ) ) {
            selectOrigin = e1->pos();
            mode = MODE_SELECT_MULTIPLE;
            return;
        } else {
            qDebug() << "Should move.";
            if ( highlightpoint == 0 ) {
                mode = MODE_SELECT_MOVE_OBJECT;
            } else {
                if ( highlightpoint->other != 0 ) {
                    mode = MODE_DRAWLINE_DRAWING;
                    savedBondOrder = highlightpoint->otherBond->Order();
                    c->Erase( highlightpoint->otherBond );
                    if ( savedBondOrder == 2 )
                        c->Erase( highlightpoint->otherBond );
                    if ( savedBondOrder == 3 )
                        c->Erase( highlightpoint->otherBond );
                    startpoint = highlightpoint->other;
                    endpoint = 0;
                    highlightpoint = 0;
                    prev_mode = MODE_SELECT;
                } else {
                    mode = MODE_SELECT_MOVE_POINT;
                }
            }
        }
        return;
    }
    // set allowedit = true for appropriate modes
    if ( mode == MODE_SELECT )
        allowedit = true;
    // get Drawable of nearest object
    DPoint *e = new DPoint;

    e->x = curqpt.x();
    e->y = curqpt.y();
    double distobj;
    Drawable *no = c->FindNearestObject( e, distobj );

    if ( distobj > 5.0 )
        no = 0;
    if ( no != 0 ) {
        if ( ( mode == MODE_DRAWLINE ) && ( no->Type() == TYPE_BOND ) )
            allowedit = true;
        if ( ( mode == MODE_DRAWLINE_DASH ) && ( no->Type() == TYPE_BOND ) )
            allowedit = true;
        if ( ( mode == MODE_DRAWLINE_UP ) && ( no->Type() == TYPE_BOND ) )
            allowedit = true;
        if ( ( mode == MODE_DRAWLINE_DOWN ) && ( no->Type() == TYPE_BOND ) )
            allowedit = true;
    }
    // allowedit and BUTTON3: edit item under mouse
    if ( ( allowedit ) && ( mouse3down ) ) {
        if ( highlightobject == 0 )
            highlightobject = no;
        if ( highlightobject != 0 ) {
            if ( highlightobject->Type() == TYPE_TEXT ) {
                setCursor( Qt::IBeamCursor );
                mode = MODE_TEXT;
                text_exists = true;
                localtext = ( Text * ) highlightobject;
                emit textOn( localtext->getFont() );

                update();
                return;
            }
            qDebug() << "Will edit";
            highlightobject->Edit();
            highlightobject->Highlight( false );
            highlightobject = 0;
            if ( mode == MODE_SELECT )
                setCursor( Qt::ArrowCursor );
            else
                setCursor( Qt::CrossCursor );
            mouse3down = false;
            update();
        }
        return;
    }
    // MODE_SELECT_MULTIPLE_SELECTED: either move, or deselect all
    if ( ( mode == MODE_SELECT_MULTIPLE_SELECTED ) && ( mouse1down ) ) {
        // check for move
        if ( selectionBox.contains( curqpt ) ) {
            c->StartUndo( 0, 0 );
            mode = MODE_SELECT_MOVE_MULTIPLE;
        } else {
            c->DeselectAll();
            mode = MODE_SELECT;
            update();
        }
    }
    // MODE_SELECT_MULTIPLE_SELECTED and Button3: Drag and Drop!
    if ( ( mode == MODE_SELECT_MULTIPLE_SELECTED ) && ( mouse3down ) ) {
        // check for move
        if ( selectionBox.contains( curqpt ) ) {
            dragging = true;
        } else {
            c->DeselectAll();
            mode = MODE_SELECT;
            update();
        }
    }
    // MODE_SELECT_MULTIPLE_RESIZE: resize drawing
    if ( ( mode == MODE_SELECT_MULTIPLE_RESIZE ) && ( mouse1down ) ) {
        mode = MODE_SELECT_RESIZE_MULTIPLE;
    }
    // MODE_SELECT_MULTIPLE_ROTATE: rotate drawing
    if ( ( mode == MODE_SELECT_MULTIPLE_ROTATE ) && ( mouse1down ) ) {
        mode = MODE_SELECT_ROTATE_MULTIPLE;
        rotateOrigin.setX( ( selectionBox.right() + selectionBox.left() - 2 ) / 2 );
        rotateOrigin.setY( ( selectionBox.bottom() + selectionBox.top() ) / 2 );
        prevangle = getAngle( rotateOrigin, curqpt );
    }
    // MODE_DRAWLINE (draw line tool selected), and left button.
    // This means we start drawing a line
    if ( ( mode == MODE_DRAWLINE ) && ( mouse1down ) ) {
        startpoint = highlightpoint;    // both 0 if nothing selected
        highlightpoint = 0;     // stop drawing highlight box
        if ( startpoint == 0 ) {
            startpoint = new DPoint;
            startpoint->set( curqpt );
            endpoint = 0;
        }
        mode = MODE_DRAWLINE_DRAWING;
    }
    // MODE_DRAWWAVYLINE (draw line tool selected), and left button.
    // This means we start drawing a line
    if ( ( mode == MODE_DRAWWAVYLINE ) && ( mouse1down ) ) {
        startpoint = highlightpoint;    // both 0 if nothing selected
        highlightpoint = 0;     // stop drawing highlight box
        if ( startpoint == 0 ) {
            startpoint = new DPoint;
            startpoint->set( curqpt );
            endpoint = 0;
        }
        mode = MODE_DRAWWAVYLINE_DRAWING;
    }
    // MODE_DRAWCHAIN (draw chain tool selected), and left button.
    // This means we start drawing an aliphatic chain
    if ( ( mode == MODE_DRAWCHAIN ) && ( mouse1down ) ) {
        startpoint = highlightpoint;    // both 0 if nothing selected
        highlightpoint = 0;     // stop drawing highlight box
        if ( startpoint == 0 ) {
            startpoint = new DPoint;
            startpoint->set( curqpt );
            endpoint = 0;
        }
        mode = MODE_DRAWCHAIN_DRAWING;
    }
    // MODE_DRAWLINE_DASH (draw dashed line tool selected), and left button.
    // This means we start drawing a dashed line
    if ( ( mode == MODE_DRAWLINE_DASH ) && ( mouse1down ) ) {
        startpoint = highlightpoint;    // both 0 if nothing selected
        highlightpoint = 0;     // stop drawing highlight box
        if ( startpoint == 0 ) {
            startpoint = new DPoint;
            startpoint->set( curqpt );
            endpoint = 0;
        }
        mode = MODE_DRAWLINE_DASH_DRAWING;
    }
    // MODE_DRAWLINE_UP (draw stereo-up line tool selected), and left button.
    // This means we start drawing a line
    if ( ( mode == MODE_DRAWLINE_UP ) && ( mouse1down ) ) {
        startpoint = highlightpoint;    // both 0 if nothing selected
        highlightpoint = 0;     // stop drawing highlight box
        if ( startpoint == 0 ) {
            startpoint = new DPoint;
            startpoint->set( curqpt );
            endpoint = 0;
        }
        mode = MODE_DRAWLINE_UP_DRAWING;
    }
    // MODE_DRAWLINE_DOWN (draw stereo-down line tool selected), and left button.
    // This means we start drawing a line
    if ( ( mode == MODE_DRAWLINE_DOWN ) && ( mouse1down ) ) {
        startpoint = highlightpoint;    // both 0 if nothing selected
        highlightpoint = 0;     // stop drawing highlight box
        if ( startpoint == 0 ) {
            startpoint = new DPoint;
            startpoint->set( curqpt );
            endpoint = 0;
        }
        mode = MODE_DRAWLINE_DOWN_DRAWING;
    }
    // MODE_DRAWARROW (draw arrow tool selected), and left button.
    // This means we start drawing an arrow
    if ( ( mode == MODE_DRAWARROW ) && ( mouse1down ) ) {
        startpoint = new DPoint;
        startpoint->set( curqpt );
        endpoint = 0;
        mode = MODE_DRAWARROW_DRAWING;
    }
    // MODE_DRAWCURVEARROW (draw curve arrow tool selected), and left button.
    // This means we start drawing an arrow
    if ( ( mode == MODE_DRAWCURVEARROW ) && ( mouse1down ) ) {
        startpoint = new DPoint;
        startpoint->set( curqpt );
        endpoint = 0;
        mode = MODE_DRAWCURVEARROW_DRAWING;
    }
    // MODE_DRAWBRACKET (draw bracket tool selected), and left button.
    // This means we start drawing an bracket
    if ( ( mode == MODE_DRAWBRACKET ) && ( mouse1down ) ) {
        startpoint = new DPoint;
        startpoint->set( curqpt );
        endpoint = 0;
        mode = MODE_DRAWBRACKET_DRAWING;
    }
    // MODE_ERASE, and left button.  Erase highlighted object.
    if ( ( mode == MODE_ERASE ) && ( mouse1down ) ) {
        if ( highlightobject == 0 )
            return;
        c->Erase( highlightobject );
        highlightobject = 0;
        update();
    }
}

void Render2D::mouseDoubleClickEvent( QMouseEvent * e1 )
{
    if ( localstring == "XDC_DELETE_ME" ) {
        delete localtexteditor;

        localtexteditor = 0;
        localstring = " ";
    }

    QPoint curqpt = zoomCorrectReverse( e1->pos() );

    if ( preferences.getSnapGrid() != GRID_NONE ) {
        curqpt = GridLock( curqpt );
    }

    bool allowedit = false;

    moved = false;

    DPoint tmp_pt( curqpt.x(), curqpt.y() );
    DPoint *tmp_pt2 = new DPoint;

    tmp_pt2->x = curqpt.x();
    tmp_pt2->y = curqpt.y();

    qDebug() << "mouseDoubleClickEvent received";

    if ( highlightobject != 0 ) {
        if ( highlightobject->Type() == TYPE_TEXT ) {
            qDebug() << "Activate text object?";
            doubleClickFlag = true;
            prev_mode = mode;
            mode = MODE_TEXT;
            DrawText_mousePressEvent( e1, curqpt );
            return;
        }
    }
}

void Render2D::mouseReleaseEvent( QMouseEvent *e1 )
{
    qDebug() << "release MODE: " << mode;

    DPoint *tmp_pt;
    XDC_Event *evt;
    QPoint curqpt = zoomCorrectReverse( e1->pos() );

    if ( preferences.getSnapGrid() != GRID_NONE ) {
        curqpt = GridLock( curqpt );
    }
    //qDebug() << "Unclick" ;
    // update button state
    if ( e1->button() == Qt::LeftButton )
        mouse1down = false;
    if ( e1->button() == Qt::RightButton )
        mouse3down = false;
    //qDebug() << "end at " << curqpt.x() << "," << curqpt.y() ;

    // Control-LeftClick invokes Molecule Information window
    if ( e1->button() == Qt::LeftButton && e1->modifiers() == Qt::ControlModifier ) {
        tmp_pt = new DPoint( curqpt );
        c->Tool( tmp_pt, MODE_TOOL_MOLECULE_INFO );
        update();
        return;
    }
    // MODE_TEXT: call special function to handle text events
    if ( mode == MODE_TEXT ) {
        DrawText_mouseReleaseEvent( e1, curqpt );
        return;
    }
    // MODE_SELECT_LASSO: finish lasso
    if ( mode == MODE_SELECT_LASSO ) {
        // TODO: validate loop
        // TODO: if loop is OK ("closed") then select points and draw box
        tmp_lasso.append( tmp_lasso.at( 0 ) );
        if ( c->SelectWithinLoop( tmp_lasso ) ) {
            qDebug() << "points within loop";
            bool shiftdown = false;

            if ( e1->button() == Qt::LeftButton && e1->modifiers() == Qt::ShiftModifier )
                shiftdown = true;
            //c->NewSelectRect(QRect(selectOrigin, selectPrev), shiftdown);
            selectionBox = c->selectionBox();
            if ( selectionBox.left() < 999 )
                mode = MODE_SELECT_MULTIPLE_SELECTED;
            else
                mode = MODE_SELECT_LASSO;
            update();
        }

        update();
        tmp_lasso.clear();
        emit SignalSetStatusBar( tr( "Select mode: left click on objects to move" ) );
        return;
    }
    // MODE_DRAWBEZIER
    if ( mode == MODE_DRAWBEZIER ) {
        update();
        directdraw = true;
//        painter->begin(this);
        if ( bezier_count < 3 ) {
//            tmp_bezier.setPoint( bezier_count, curqpt.x(), curqpt.y() );
            drawBezier( tmp_bezier, currentColor, true );
            bezier_count++;
        } else {
//            tmp_bezier.setPoint( bezier_count, curqpt.x(), curqpt.y() );
            // add new GraphicObject
            evt = new XDC_Event( EVT_ADD_GRAPHIC );
            evt->setPoints( tmp_bezier );
            evt->setParam1( bracket_type );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );

//            drawBezier( tmp_bezier, currentColor, false, bracket_type );
            tmp_bezier.resize( 0 );
            tmp_bezier.resize( 4 );
            bezier_count = 0;
        }
        directdraw = false;
        //      painter->end();
        return;
    }
    // MODE_TOOL_*: ChemData handles this, mostly
    if ( mode == MODE_TOOL_RETRO_BONDNAME ) {
        qDebug() << "mode invoke";
        // retro_bondname handled by Molecule containing this bond
        if ( highlightobject != 0 ) {
            qDebug() << "invoke";
            qDebug() << c->insideMolecule( highlightobject->Start() )->RetroBondName( ( Bond * ) highlightobject, true );
            highlightobject->Highlight( false );
            highlightobject = 0;
            update();
            setMode_Select();
        }
        return;
    }

    if ( ( mode > MODE_TOOL ) && ( mode < MODE_TOOL_RETRO_BONDNAME ) ) {
        tmp_pt = new DPoint( curqpt );
        c->Tool( tmp_pt, mode );
        //if (mode == MODE_TOOL_CLEANUPMOL) {
        setMode_Select();
        update();
        //}
        return;
    }
    // MODE_SELECT_MULTIPLE: finish multiple selection
    if ( ( mode == MODE_SELECT_MULTIPLE ) && ( !mouse1down ) ) {
        int tmp1;

        if ( selectOrigin.x() > selectPrev.x() ) {
            tmp1 = selectOrigin.x();
            selectOrigin.setX( selectPrev.x() );
            selectPrev.setX( tmp1 );
        }
        if ( selectOrigin.y() > selectPrev.y() ) {
            tmp1 = selectOrigin.y();
            selectOrigin.setY( selectPrev.y() );
            selectPrev.setY( tmp1 );
        }
        update();
        directdraw = true;
        //      painter->begin(this);
        drawBox( selectOrigin, selectPrev, QColor( 255, 255, 255 ) );
        bool shiftdown = false;

        if ( e1->modifiers() == Qt::ShiftModifier )
            shiftdown = true;
        c->NewSelectRect( QRect( zoomCorrectReverse( selectOrigin ), zoomCorrectReverse( selectPrev ) ), shiftdown );
        selectionBox = c->selectionBox();
        if ( selectionBox.left() < 999 )
            mode = MODE_SELECT_MULTIPLE_SELECTED;
        else
            mode = MODE_SELECT;
        directdraw = false;
        //      painter->end();
        return;
    }
    // MODE_SELECT_MOVE_MULTIPLE: finish moving
    if ( ( mode == MODE_SELECT_MOVE_MULTIPLE ) && ( !mouse1down ) ) {
        mode = MODE_SELECT_MULTIPLE_SELECTED;
        c->FinishMove();
        update();
    }
    // MODE_SELECT_MOVE_OBJECT: finish moving
    if ( ( mode == MODE_SELECT_MOVE_OBJECT ) && ( !mouse1down ) ) {
        mode = MODE_SELECT;
        c->FinishMove();
        update();
    }
    // MODE_SELECT_MOVE_POINT: finish moving
    if ( ( mode == MODE_SELECT_MOVE_POINT ) && ( !mouse1down ) ) {
        mode = MODE_SELECT;
        c->FinishMove();
        update();
    }
    // MODE_SELECT_RESIZE_MULTIPLE: finish resizing
    if ( ( mode == MODE_SELECT_RESIZE_MULTIPLE ) && ( !mouse1down ) ) {
        mode = MODE_SELECT_MULTIPLE_SELECTED;
    }
    // MODE_SELECT_ROTATE_MULTIPLE: finish rotating
    if ( ( mode == MODE_SELECT_ROTATE_MULTIPLE ) && ( !mouse1down ) ) {
        mode = MODE_SELECT_MULTIPLE_SELECTED;
    }
    // MODE_DRAWLINE_DRAWING (drawing line), and left button.
    // Finish line
    if ( ( mode == MODE_DRAWLINE_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this line/bond to ChemData
            if ( startpoint != endpoint ) {
                evt = new XDC_Event( EVT_ADD_BOND );
                evt->setStart( startpoint );
                evt->setEnd( endpoint );
                evt->setParam1( thick );
                evt->setParam2( savedBondOrder );
                evt->setColor( currentColor );
                emit XDCEventSignal( evt );

                savedBondOrder = -1;
                //c->addBond(startpoint, endpoint, thick, 1, currentColor);
            }
        }
        startpoint = 0;
        endpoint = 0;
        if ( prev_mode == MODE_SELECT ) {
            mode = MODE_SELECT;
            setCursor( Qt::ArrowCursor );
            emit SignalSetStatusBar( tr( "Select mode: left click on object to move, right click on object to edit" ) );

            prev_mode = 0;
        } else {
            mode = MODE_DRAWLINE;
        }
        update();
        return;
    }
    // MODE_DRAWCHAIN_DRAWING (drawing chain), and left button.
    // Finish chain
    if ( ( mode == MODE_DRAWCHAIN_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this set of lines/bonds to ChemData
            double ang, len;
            QString sang, slen;

            ang = Drawable::getAngle( startpoint, endpoint );
            double alt_ang1 = ( ang - 30.0 ) * M_PI / 180.0;
            double alt_ang2 = ( ang + 30.0 ) * M_PI / 180.0;
            double alt_swap;

            if ( e1->modifiers() == Qt::ShiftModifier ) {
                alt_swap = alt_ang1;
                alt_ang1 = alt_ang2;
                alt_ang2 = alt_swap;
            }
            len = startpoint->distanceTo( endpoint );
            double blen = preferences.getBond_fixedlength();
            int n1, nseg = qRound( len / blen );
            QPoint spoint, epoint, ipoint1, ipoint2;

            spoint = startpoint->toQPoint();
            ipoint1 = spoint;
            for ( n1 = 0; n1 < nseg; n1++ ) {
                ipoint2.setX( qRound( ipoint1.x() + cos( alt_ang1 ) * blen ) );
                ipoint2.setY( qRound( ipoint1.y() + sin( alt_ang1 ) * blen ) );
                //drawLine( ipoint1, ipoint2, 1, currentColor );
                evt = new XDC_Event( EVT_ADD_BOND );
                if ( n1 == 0 )
                    evt->setStart( startpoint );
                else
                    evt->setStart( endpoint );
                endpoint = new DPoint( ipoint2 );
                evt->setEnd( endpoint );
                evt->setParam1( thick );
                evt->setParam2( 1 );
                evt->setColor( currentColor );
                emit XDCEventSignal( evt );

                alt_swap = alt_ang1;
                alt_ang1 = alt_ang2;
                alt_ang2 = alt_swap;
                ipoint1 = ipoint2;
            }
        }
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWCHAIN;
        update();
        return;
    }
    // MODE_DRAWLINE_DASH_DRAWING (drawing line), and left button.
    // Finish line
    if ( ( mode == MODE_DRAWLINE_DASH_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this line/bond to ChemData
            evt = new XDC_Event( EVT_ADD_BOND_DASH );
            evt->setStart( startpoint );
            evt->setEnd( endpoint );
            evt->setParam1( thick );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );

            //c->addBond(startpoint, endpoint, thick, 99, currentColor);
        }
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWLINE_DASH;
        update();
        return;
    }
    // MODE_DRAWLINE_UP_DRAWING (drawing line), and left button.
    // Finish line
    if ( ( mode == MODE_DRAWLINE_UP_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this line/bond to ChemData
            evt = new XDC_Event( EVT_ADD_BOND_UP );
            evt->setStart( startpoint );
            evt->setEnd( endpoint );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );

            //c->addBond(startpoint, endpoint, 1, 5, currentColor);
        }
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWLINE_UP;
        update();
        return;
    }
    // MODE_DRAWLINE_DOWN_DRAWING (drawing line), and left button.
    // Finish line
    if ( ( mode == MODE_DRAWLINE_DOWN_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this line/bond to ChemData
            evt = new XDC_Event( EVT_ADD_BOND_DOWN );
            evt->setStart( startpoint );
            evt->setEnd( endpoint );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );

            //c->addBond(startpoint, endpoint, 1, 7, currentColor);
        }
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWLINE_DOWN;
        update();
        return;
    }
    // MODE_DRAWWAVYLINE_DRAWING (drawing line), and left button.
    // Finish line
    if ( ( mode == MODE_DRAWWAVYLINE_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this line/bond to ChemData
            evt = new XDC_Event( EVT_ADD_BOND_WAVY );
            evt->setStart( startpoint );
            evt->setEnd( endpoint );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );
        }
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWWAVYLINE;
        update();
        return;
    }
    // MODE_DRAWARROW_DRAWING (drawing arrow), and left button.
    // Finish arrow
    if ( ( mode == MODE_DRAWARROW_DRAWING ) && ( !mouse1down ) ) {
        if ( moved ) {
            if ( endpoint == 0 ) {
                endpoint = new DPoint;
                endpoint->set( curqpt );
            }
            // submit this arrow to ChemData
            evt = new XDC_Event( EVT_ADD_ARROW );
            evt->setStart( startpoint );
            evt->setEnd( endpoint );
            evt->setColor( currentColor );
            evt->setParam1( bracket_type );
            evt->setParam2( thick );
            emit XDCEventSignal( evt );

            //c->addArrow(startpoint, endpoint, currentColor, bracket_type);
        }
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWARROW;
        update();
    }
    // MODE_DRAWCURVEARROW_DRAWING (drawing curved arrow), and left button.
    // Finish arrow
    if ( ( mode == MODE_DRAWCURVEARROW_DRAWING ) && ( !mouse1down ) ) {
        endpoint = new DPoint;
        endpoint->set( curqpt );
        // submit this arrow to ChemData
        evt = new XDC_Event( EVT_ADD_CURVEARROW );
        evt->setStart( startpoint );
        evt->setEnd( endpoint );
        evt->setColor( currentColor );
        evt->setText( symbolfile );
        emit XDCEventSignal( evt );

        //c->addCurveArrow(startpoint, endpoint, currentColor, symbolfile);
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWCURVEARROW;
        update();
    }
    // MODE_DRAWBRACKET_DRAWING (drawing bracket), and left button.
    // Finish bracket
    if ( ( mode == MODE_DRAWBRACKET_DRAWING ) && ( !mouse1down ) ) {
        if ( endpoint == 0 ) {
            endpoint = new DPoint;
            endpoint->set( curqpt );
        }
        // submit this bracket to ChemData
        evt = new XDC_Event( EVT_ADD_BRACKET );
        evt->setStart( startpoint );
        evt->setEnd( endpoint );
        evt->setColor( currentColor );
        evt->setParam1( bracket_type );
        emit XDCEventSignal( evt );

        //c->addBracket(startpoint, endpoint, currentColor, bracket_type);
        startpoint = 0;
        endpoint = 0;
        mode = MODE_DRAWBRACKET;
        update();
    }
    // MODE_SYMBOL: create Symbol
    if ( ( mode == MODE_SYMBOL ) && ( !mouse1down ) ) {
        if ( highlightpoint == 0 ) {
            endpoint = new DPoint( curqpt );
        } else {
            endpoint = highlightpoint;
        }
        if ( symbolfile == "antibody" ) {
            c->DeselectAll();
            DPoint *ab0, *ab1, *ab2;
            double dab, dabscale;

            dabscale = preferences.getBond_fixedlength();
            dab = dabscale / 1.41421;
            ab0 = new DPoint( endpoint );
            ab0->y = ab0->y - dabscale;
            ab1 = new DPoint( ab0 );
            ab2 = new DPoint( ab0 );
            ab1->x = ab1->x - dab;
            ab1->y = ab1->y - dab;
            ab2->x = ab2->x + dab;
            ab2->y = ab2->y - dab;
            preferences.setBondCenter( true );
            evt = new XDC_Event( EVT_ADD_BOND );
            evt->setStart( endpoint );
            evt->setEnd( ab0 );
            evt->setParam1( 2 );
            evt->setParam2( 2 );
            evt->setColor( currentColor );
            evt->setBool1( true );
            emit XDCEventSignal( evt );

            preferences.setBondCenter( false );
            evt = new XDC_Event( EVT_ADD_BOND );
            evt->setStart( ab0 );
            evt->setEnd( ab1 );
            evt->setParam1( 1 );
            evt->setParam2( 2 );
            evt->setColor( currentColor );
            evt->setBool1( true );
            emit XDCEventSignal( evt );

            evt = new XDC_Event( EVT_ADD_BOND );
            evt->setStart( ab0 );
            evt->setEnd( ab2 );
            evt->setParam1( 1 );
            evt->setParam2( 2 );
            evt->setBool1( true );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );

            mode = MODE_SELECT_MULTIPLE_SELECTED;
            selectionBox = c->selectionBox();
            update();
            endpoint = 0;
            return;
        }
        if ( highlightpoint != 0 ) {
            c->addSymbol( highlightpoint, symbolfile ); // add to existing point
        } else {
            c->addSymbol( endpoint, symbolfile );       // add at current mouse position
        }
        endpoint = 0;
        update();
    }
    // MODE_DRAWNEWMAN_*: create Newman projection
    if ( ( mode == MODE_DRAWNEWMAN_ANTI ) && ( !mouse1down ) ) {
        endpoint = new DPoint;
        endpoint->set( curqpt );
        double blen = preferences.getBond_fixedlength();
        int n1;
        QPoint ipoint1 = endpoint->toQPoint();
        QPoint ipoint2;

        for ( n1 = 30; n1 < 331; n1 = n1 + 60 ) {
            ipoint2.setX( qRound( ipoint1.x() + cos( ( double ) n1 * M_PI / 180.0 ) * blen ) );
            ipoint2.setY( qRound( ipoint1.y() + sin( ( double ) n1 * M_PI / 180.0 ) * blen ) );
            tmp_pt = new DPoint( ipoint2 );
            evt = new XDC_Event( EVT_ADD_BOND );
            evt->setStart( endpoint );
            evt->setEnd( tmp_pt );
            evt->setParam1( thick );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );
        }
        c->addSymbol( endpoint, "newman" );
        endpoint = 0;
        update();
    }
    // MODE_DRAWNEWMAN_*: create Newman projection
    if ( ( mode == MODE_DRAWNEWMAN_GAUCHE ) && ( !mouse1down ) ) {
        endpoint = new DPoint;
        endpoint->set( curqpt );
        double blen = preferences.getBond_fixedlength();
        int n1;
        QPoint ipoint1 = endpoint->toQPoint();
        QPoint ipoint2;

        for ( n1 = 30; n1 < 331; n1 = n1 + 120 ) {
            ipoint2.setX( qRound( ipoint1.x() + cos( ( double ) n1 * M_PI / 180.0 ) * blen ) );
            ipoint2.setY( qRound( ipoint1.y() + sin( ( double ) n1 * M_PI / 180.0 ) * blen ) );
            tmp_pt = new DPoint( ipoint2 );
            evt = new XDC_Event( EVT_ADD_BOND );
            evt->setStart( endpoint );
            evt->setEnd( tmp_pt );
            evt->setParam1( thick );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );
        }
        for ( n1 = 50; n1 < 351; n1 = n1 + 120 ) {
            ipoint2.setX( qRound( ipoint1.x() + cos( ( double ) n1 * M_PI / 180.0 ) * blen ) );
            ipoint2.setY( qRound( ipoint1.y() + sin( ( double ) n1 * M_PI / 180.0 ) * blen ) );
            tmp_pt = new DPoint( ipoint2 );
            evt = new XDC_Event( EVT_ADD_BOND );
            evt->setStart( endpoint );
            evt->setEnd( tmp_pt );
            evt->setParam1( thick );
            evt->setColor( currentColor );
            emit XDCEventSignal( evt );
        }
        c->addSymbol( endpoint, "newman" );
        endpoint = 0;
        update();
    }
    // MODE_RING: place ring
    if ( ( mode == MODE_RING ) && ( !mouse1down ) ) {
        endpoint = new DPoint;
        endpoint->set( curqpt );
        c->setThickKludge( thick );
        // are we smart-placing this ring?
        if ( ( smartplace != 0 ) && ( highlightpoint != 0 ) ) {
            qDebug() << "smart placement! Mode " << smartplace;
            if ( smartplace == 1 )
                c->SmartPlace( symbolfile, highlightpoint );
            if ( smartplace == 2 )
                c->SmartPlaceToo( symbolfile, highlightpoint );
            if ( smartplace == 3 )
                c->SmartPlaceThree( symbolfile, highlightpoint );
            // Clear highlighted point
            highlightpoint = 0;
            endpoint = 0;
            mode = MODE_SELECT_MULTIPLE_SELECTED;
            emit SignalSetStatusBar( tr( "Select mode: left click on object to move" ) );

            setCursor( Qt::SizeAllCursor );
            selectionBox = c->selectionBox();
            update();
            smartplace = 0;
            c->setThickKludge( -1 );
            return;
        }
        // translate ring to click point
        c->load( symbolfile );
        c->setThickKludge( -1 );
        mode = MODE_SELECT_MULTIPLE_SELECTED;
        emit SignalSetStatusBar( tr( "Select mode: left click on object to move, right click on object to edit" ) );

        setCursor( Qt::SizeAllCursor );
        selectionBox = c->selectionBox();
        double dx = endpoint->x - selectionBox.center().x();
        double dy = endpoint->y - selectionBox.center().y();

        //qDebug() << "Move by " << dx << "," << dy ;
        c->Move( dx, dy );
        selectionBox.translate( ( int ) dx, ( int ) dy );

        endpoint = 0;
        update();
        smartplace = 0;
        return;
    }
}

void Render2D::mouseMoveEvent( QMouseEvent * e1 )
{
  //qDebug() << "mouseMoveEvent begin; mode: " << mode;
    if ( localstring == "XDC_DELETE_ME" ) {
        delete localtexteditor;

        localtexteditor = 0;
        localstring = " ";
    }

    if ( mode == MODE_DRAWBRACKET )
        highlightobject = 0;

    if ( ( mode == MODE_RING ) && mouse1down )
        return;

    if ( ( mode == MODE_DRAWBEZIER ) && mouse1down )
        return;

    QPoint curqpt = zoomCorrectReverse( e1->pos() );

    if ( preferences.getSnapGrid() != GRID_NONE ) {
        curqpt = GridLock( curqpt );
    }

    lastmouse = curqpt;

    moved = true;

    DPoint tmp_pt( curqpt.x(), curqpt.y() );
    Molecule *tmp_mol;

    tmp_molecule = 0;

    // Tools
    if ( ( mode >= 600 ) && ( mode < 698 ) ) {
        tmp_molecule = c->insideMolecule( &tmp_pt );
        if ( tmp_molecule != 0 ) {
            QRect mr = tmp_molecule->BoundingBoxAll();
            mr.setLeft( mr.left() - 2 );
            mr.setRight( mr.right() + 2 );
            mr.setTop( mr.top() - 2 );
            mr.setBottom( mr.bottom() + 2 );
	    selectionBox = mr;
            update();
        } else {
            update();
        }
    }

    if ( mode == MODE_TOOL_CALCMW ) {
        // check if we're over a molecule
        tmp_mol = c->insideMolecule( &tmp_pt );
        if ( tmp_mol != 0 ) {
            Text *tmp_txt = tmp_mol->CalcMW();
            emit SignalSetStatusBar( tmp_txt->getText() + tr( ", click to paste into drawing" ) );
            delete tmp_txt;
        } else {
            emit SignalSetStatusBar( tr( "Click on a molecule to calculate its molecular weight" ) );
        }
        return;
    }

    if ( mode == MODE_TOOL_CALCEF ) {
        // check if we're over a molecule
        tmp_mol = c->insideMolecule( &tmp_pt ); // doesn't copy pointer
        if ( tmp_mol != 0 ) {
            Text *tmp_txt = tmp_mol->CalcEmpiricalFormula();
            emit SignalSetStatusBar( tmp_txt->getText() + tr( ", click to paste into drawing" ) );
            delete tmp_txt;
        } else {
            emit SignalSetStatusBar( tr( "Click on a molecule to calculate its empirical formula" ) );
        }
        return;
    }
    // MODE_TOOL_*: return
    if ( ( mode >= 600 ) && ( mode < 698 ) )
        return;

    // Dragging
    if ( dragging && mouse3down ) {
        qDebug() << "Start drag";
        // make pixmap of selection
        QRect savebox = selectionBox;
        int prevmode = mode;

        mode = MODE_SELECT;
        c->DeselectAll();
        //update();
        QPixmap pm;
        pm = QPixmap::grabWidget( this, savebox );

        c->NewSelectRect( savebox, false );
        selectionBox = c->selectionBox();
        mode = prevmode;
        QDrag *drag = new QDrag( this );
        QMimeData *dragObject = new QMimeData();
        dragObject->setImageData( pm.toImage() );
        drag->setMimeData( dragObject );
        QSize maxSize = pm.size().boundedTo( QSize( 100, 100 ) );
        drag->setPixmap( pm.scaled( maxSize, Qt::KeepAspectRatio, Qt::SmoothTransformation ) );
        drag->start();
        dragging = false;
        update();
        setCursor( Qt::ArrowCursor );
        mouse3down = false;
        return;
    }
    // MODE_TEXT: call special function to handle text events
    if ( mode == MODE_TEXT ) {
        DrawText_mouseMoveEvent( e1 );
        if ( prevpos == 0 ) {
            prevpos = new DPoint;
        }
        prevpos->x = curqpt.x();
        prevpos->y = curqpt.y();
        return;
    }
    // MODE_SELECT: call special function to handle select events
    if ( mode < 199 ) {
        Select_mouseMoveEvent( e1 );
        if ( prevpos == 0 ) {
            prevpos = new DPoint;
        }
        prevpos->x = curqpt.x();
        prevpos->y = curqpt.y();
        return;
    }
    //bool update;
    double ang, len;
    QString sang, slen;
    DPoint *prevhighlight = highlightpoint;
    Drawable *prevhighlightobject = highlightobject;

    // Create DPoint of current pointer position
    DPoint *e = new DPoint;
    DPoint *nearestPoint = 0;

    e->x = curqpt.x();
    e->y = curqpt.y();
    double dist, distobj;

    // Get DPoint of nearest point
    nearestPoint = c->FindNearestPoint( e, dist );
    // get Drawable of nearest object
    Drawable *no = c->FindNearestObject( e, distobj );

    // no buttons down
    if ( ( mouse1down == false ) && ( mouse3down == false ) ) {
        if ( mode == MODE_DRAWARROW )
            return;

        // if (no != 0) {
        // highlight object if object close enough
        if ( ( no != 0 ) && ( mode == MODE_ERASE ) && ( distobj < 6.0 ) ) {
            highlightobject = no;
            if ( prevhighlightobject != 0 )
                prevhighlightobject->Highlight( false );
            highlightobject->Highlight( true );
            if ( prevhighlightobject != highlightobject )
                update();
        }
        // unhighlight object if no object close
        if ( ( mode == MODE_ERASE ) && ( no == 0 || distobj >= 6.0 ) ) {
            // Clear highlighted object
            highlightobject = 0;
            if ( prevhighlightobject != 0 )
                prevhighlightobject->Highlight( false );
            if ( prevhighlightobject != highlightobject )
                update();
            return;
        }

        if ( ( no != 0 ) && ( mode == MODE_TOOL_RETRO_BONDNAME ) && ( distobj < 6.0 ) ) {
            if ( no->Type() == TYPE_BOND ) {
                highlightobject = no;
                if ( prevhighlightobject != 0 ) {
                    prevhighlightobject->Highlight( false );
                }
                highlightobject->Highlight( true );
                if ( prevhighlightobject != highlightobject ) {
                    update();
                }
            }
        }
        // unhighlight object if no object close
        if ( ( mode == MODE_TOOL_RETRO_BONDNAME ) && ( no == 0 || distobj >= 6.0 ) ) {
            // Clear highlighted object
            highlightobject = 0;
            if ( prevhighlightobject != 0 )
                prevhighlightobject->Highlight( false );
            if ( prevhighlightobject != highlightobject )
                update();
            return;
        }
        // highlight text object preferentially when MODE_SELECT...
        if ( ( no != 0 ) && ( mode == MODE_SELECT ) && ( no->Type() == TYPE_TEXT ) ) {
            highlightpoint = 0;
            highlightobject = no;
            if ( ( prevhighlightobject != highlightobject ) && ( prevhighlightobject != 0 ) )
                prevhighlightobject->Highlight( false );
            highlightobject->Highlight( true );
            if ( prevhighlightobject != highlightobject )
                update();
            // return since no need to check points
            return;
        }
        // highlight object if object close enough and no point close
        if ( ( no != 0 ) && ( mode == MODE_SELECT ) && ( distobj < 6.0 ) && ( dist >= 8.0 ) ) {
            highlightobject = no;
            if ( prevhighlightobject != 0 )
                prevhighlightobject->Highlight( false );
            highlightobject->Highlight( true );
            if ( prevhighlightobject != highlightobject )
                update();
            // return since no need to check points
            return;
        }
        // unhighlight object if no object close
        if ( ( mode == MODE_SELECT ) && ( no == 0 || distobj >= 6.0 ) ) {
            // Clear highlighted object
            highlightobject = 0;
            if ( prevhighlightobject != 0 )
                prevhighlightobject->Highlight( false );
            if ( prevhighlightobject != highlightobject )
                update();
            // don't return; go on to check for points
        }
        // unhighlight object if point close
        if ( ( mode == MODE_SELECT ) && ( dist < 8.0 ) ) {
            // Clear highlighted object
            highlightobject = 0;
            if ( prevhighlightobject != 0 )
                prevhighlightobject->Highlight( false );
            if ( prevhighlightobject != highlightobject )
                update();
            // don't return; go on to check for points
        }

        if ( nearestPoint != 0 ) {
            //qDebug() << dist << " to (" << nearestPoint->x << "," << nearestPoint->y << ")" ;
            if ( ( mode == MODE_TEXT ) && ( dist < 8.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_TEXT ) && ( dist >= 8.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_SYMBOL ) && ( dist < 8.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_SYMBOL ) && ( dist >= 8.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_SELECT ) && ( dist < 8.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_SELECT ) && ( dist >= 8.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE ) && ( dist < 6.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE ) && ( dist >= 6.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_RING ) && smartplace && ( dist < 6.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_RING ) && smartplace && ( dist >= 6.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE_DASH ) && ( dist < 6.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE_DASH ) && ( dist >= 6.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE_UP ) && ( dist < 6.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE_UP ) && ( dist >= 6.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE_DOWN ) && ( dist < 6.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWLINE_DOWN ) && ( dist >= 6.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWCHAIN ) && ( dist < 6.0 ) ) {
                highlightpoint = nearestPoint;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

            if ( ( mode == MODE_DRAWCHAIN ) && ( dist >= 6.0 ) ) {
                // Clear highlighted point
                highlightpoint = 0;
                if ( prevhighlight != highlightpoint )
                    update();
                return;
            }

        }                       // nearestPoint != 0

        return;                 // rest of function only if mouse button down
    }                           // no button down

    // we're probably going to draw something...
    update();
    directdraw = true;

    // MODE_DRAWLINE_DRAWING: draw temporary line
    if ( ( mode == MODE_DRAWLINE_DRAWING ) && mouse1down ) {
        qDebug() << "mouseMoveEvent: drawing line";
        // if within range of existing point AND the end point is not the same
        // as the start point, snap to that point
        if ( ( dist < 6.0 ) && ( startpoint != nearestPoint ) ) {
            highlightpoint = nearestPoint;
            endpoint = nearestPoint;
        } else {
            endpoint = new DPoint;
            endpoint->x = curqpt.x();
            endpoint->y = curqpt.y();
            highlightpoint = 0; // stop highlighting
            if ( preferences.getBond_fixed() ) {      // fix endpoint if fixed set
                CorrectEndpoint_bond();
                nearestPoint = c->FindNearestPoint( endpoint, dist );
                if ( dist < 2.0 ) {
                    highlightpoint = nearestPoint;
                    endpoint = nearestPoint;
                }
            }
        }
        ang = Drawable::getAngle( startpoint, endpoint );
        len = startpoint->distanceTo( endpoint );
        sang.setNum( ang );
        slen.setNum( len );
        emit SignalSetStatusBar( tr( "Draw Line - Length = " ) + slen + tr( ", Angle = " ) + sang );

        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
//        drawLine( startpoint->toQPoint(), endpoint->toQPoint(), thick, currentColor );
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWLINE_DASH_DRAWING: draw temporary line
    if ( ( mode == MODE_DRAWLINE_DASH_DRAWING ) && mouse1down ) {
        qDebug() << "mouseMoveEvent: drawing dashed line";
        // if within range of existing point AND the end point is not the same
        // as the start point, snap to that point
        if ( ( dist < 6.0 ) && ( startpoint != nearestPoint ) ) {
            highlightpoint = nearestPoint;
            endpoint = nearestPoint;
        } else {
            endpoint = new DPoint;
            endpoint->x = curqpt.x();
            endpoint->y = curqpt.y();
            highlightpoint = 0; // stop highlighting
            if ( preferences.getBond_fixed() ) {      // fix endpoint if fixed set
                CorrectEndpoint_bond();
                nearestPoint = c->FindNearestPoint( endpoint, dist );
                if ( dist < 2.0 ) {
                    highlightpoint = nearestPoint;
                    endpoint = nearestPoint;
                }
            }
        }
        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
        drawLine( startpoint->toQPoint(), endpoint->toQPoint(), 1, currentColor, 1 );
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWCHAIN_DRAWING: draw temporary aliphatic chain
    if ( ( mode == MODE_DRAWCHAIN_DRAWING ) && mouse1down ) {
        // if within range of existing point AND the end point is not the same
        // as the start point, snap to that point
        if ( ( dist < 6.0 ) && ( startpoint != nearestPoint ) ) {
            highlightpoint = nearestPoint;
            endpoint = nearestPoint;
        } else {
            endpoint = new DPoint;
            endpoint->x = curqpt.x();
            endpoint->y = curqpt.y();
            highlightpoint = 0; // stop highlighting
        }
        ang = Drawable::getAngle( startpoint, endpoint );
        double alt_ang1 = ( ang - 30.0 ) * M_PI / 180.0;
        double alt_ang2 = ( ang + 30.0 ) * M_PI / 180.0;
        double alt_swap;

        if ( e1->modifiers() == Qt::ShiftModifier ) {
            alt_swap = alt_ang1;
            alt_ang1 = alt_ang2;
            alt_ang2 = alt_swap;
        }
        len = startpoint->distanceTo( endpoint );
        double blen = preferences.getBond_fixedlength();
        int n1, nseg = qRound( len / blen );

        slen.setNum( nseg );
        emit SignalSetStatusBar( tr( "Draw aliphatic chain - Length = " ) + slen + tr( " bonds" ) );
        QPoint spoint, epoint, ipoint1, ipoint2;

        spoint = startpoint->toQPoint();
        ipoint1 = spoint;
        chainPoints.clear();
        //    painter->begin(this);
        for ( n1 = 0; n1 < nseg; n1++ ) {
            ipoint2.setX( qRound( ipoint1.x() + cos( alt_ang1 ) * blen ) );
            ipoint2.setY( qRound( ipoint1.y() + sin( alt_ang1 ) * blen ) );
            chainPoints << ipoint1 << ipoint2;
            drawLine( ipoint1, ipoint2, 1, currentColor );
            alt_swap = alt_ang1;
            alt_ang1 = alt_ang2;
            alt_ang2 = alt_swap;
            ipoint1 = ipoint2;
        }
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWLINE_UP_DRAWING: draw temporary line
    if ( ( mode == MODE_DRAWLINE_UP_DRAWING ) && mouse1down ) {
        // if within range of existing point AND the end point is not the same
        // as the start point, snap to that point
        if ( ( dist < 6.0 ) && ( startpoint != nearestPoint ) ) {
            highlightpoint = nearestPoint;
            endpoint = nearestPoint;
        } else {
            endpoint = new DPoint;
            endpoint->x = curqpt.x();
            endpoint->y = curqpt.y();
            highlightpoint = 0; // stop highlighting
            if ( preferences.getBond_fixed() ) {      // fix endpoint if fixed set
                CorrectEndpoint_bond();
                nearestPoint = c->FindNearestPoint( endpoint, dist );
                if ( dist < 2.0 ) {
                    highlightpoint = nearestPoint;
                    endpoint = nearestPoint;
                }
            }
        }
        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
//        drawUpLine( startpoint->toQPoint(), endpoint->toQPoint(), currentColor );
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWLINE_DOWN_DRAWING: draw temporary line
    if ( ( mode == MODE_DRAWLINE_DOWN_DRAWING ) && mouse1down ) {
        // if within range of existing point AND the end point is not the same
        // as the start point, snap to that point
        if ( ( dist < 6.0 ) && ( startpoint != nearestPoint ) ) {
            highlightpoint = nearestPoint;
            endpoint = nearestPoint;
        } else {
            endpoint = new DPoint;
            endpoint->x = curqpt.x();
            endpoint->y = curqpt.y();
            highlightpoint = 0; // stop highlighting
            if ( preferences.getBond_fixed() ) {      // fix endpoint if fixed set
                CorrectEndpoint_bond();
                nearestPoint = c->FindNearestPoint( endpoint, dist );
                if ( dist < 2.0 ) {
                    highlightpoint = nearestPoint;
                    endpoint = nearestPoint;
                }
            }
        }
        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
//        drawDownLine( startpoint->toQPoint(), endpoint->toQPoint(), currentColor );
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWWAVYLINE_DRAWING: draw temporary line
    if ( ( mode == MODE_DRAWWAVYLINE_DRAWING ) && mouse1down ) {
        // if within range of existing point AND the end point is not the same
        // as the start point, snap to that point
        if ( ( dist < 6.0 ) && ( startpoint != nearestPoint ) ) {
            highlightpoint = nearestPoint;
            endpoint = nearestPoint;
        } else {
            endpoint = new DPoint;
            endpoint->x = curqpt.x();
            endpoint->y = curqpt.y();
            highlightpoint = 0; // stop highlighting
            if ( preferences.getBond_fixed() ) {      // fix endpoint if fixed set
                CorrectEndpoint_bond();
                nearestPoint = c->FindNearestPoint( endpoint, dist );
                if ( dist < 2.0 ) {
                    highlightpoint = nearestPoint;
                    endpoint = nearestPoint;
                }
            }
        }
        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
//        drawWavyLine( startpoint->toQPoint(), endpoint->toQPoint(), currentColor );
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWARROW_DRAWING: draw temporary arrow
    if ( ( mode == MODE_DRAWARROW_DRAWING ) && mouse1down ) {
        endpoint = new DPoint;
        endpoint->x = curqpt.x();
        endpoint->y = curqpt.y();
        if ( preferences.getArrow_fixed() ) { // fix endpoint if fixed set
            CorrectEndpoint_arrow();
        }
        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
//        drawArrow( startpoint->toQPoint(), endpoint->toQPoint(), currentColor, bracket_type, thick );
        directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWCURVEARROW_DRAWING: draw temporary curve arrow
    if ( ( mode == MODE_DRAWCURVEARROW_DRAWING ) && mouse1down ) {
        endpoint = new DPoint;
        endpoint->x = curqpt.x();
        endpoint->y = curqpt.y();
        // should not draw curve arrows fixed, even if selected
        //if (fixed) { // fix endpoint if fixed set
        //  CorrectEndpoint();
        //}
        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
        //drawCurveArrow( startpoint->toQPoint(), endpoint->toQPoint(), currentColor, symbolfile );
        //directdraw = false;
        //    painter->end();
        return;
    }
    // MODE_DRAWBRACKET_DRAWING: draw temporary bracket
    if ( ( mode == MODE_DRAWBRACKET_DRAWING ) && mouse1down ) {
        endpoint = new DPoint;
        endpoint->x = curqpt.x();
        endpoint->y = curqpt.y();
        // adjust position for square and circle "brackets"
        if ( ( bracket_type == BRACKET_CLOSEDSQUARE ) || ( bracket_type == BRACKET_CIRCLE ) ) {
            double bdx = endpoint->x - startpoint->x;
            double bdy = endpoint->y - startpoint->y;
            double mgn = fabs( bdx / bdy );

            if ( bdx > bdy ) {
                bdy *= mgn;
                endpoint->y = startpoint->y + bdy;
            } else if ( bdx < bdy ) {
                bdx /= mgn;
                endpoint->x = startpoint->x + bdx;
            }
            bdx = endpoint->x - startpoint->x;
            bdy = endpoint->y - startpoint->y;
            qDebug() << "after:" << bdx << "," << bdy;
        }
        QColor icv;

        //    painter->begin(this);
        startPoint = startpoint->toQPoint();
        endPoint = endpoint->toQPoint();
//        drawBracket( startpoint->toQPoint(), endpoint->toQPoint(), currentColor, bracket_type, icv );
        directdraw = false;
        //    painter->end();
        return;
    }
    // just in case
    directdraw = false;
    qDebug() << "mouseMoveEvent end";
}

// handle resize event
void Render2D::resizeEvent( QResizeEvent * rz )
{
    dbuffer = QPixmap( renderWidth, renderHeight );
}

void Render2D::paintEvent( QPaintEvent * )
{
    if ( localstring == "XDC_DELETE_ME" ) {
        delete localtexteditor;

        localtexteditor = 0;
        localstring = " ";
    }
    // draw background
    //drawFillBox(QPoint(0,0), QPoint(renderWidth, renderHeight), bgcolor);

    // initialize global painter object
    if ( outputDevice == OUTPUT_PRINTER ) {
        qDebug() << "printer";
        painter->begin( printer );
    } else if ( outputDevice == OUTPUT_SCREEN ) {
        qDebug() << "screen";
        if ( directdraw ) {
            painter->begin( this );
        } else {
            qDebug() << "paintEvent painter begin";
            painter->begin( this );
        }
        ///TODO: make that an option
        painter->setRenderHint( QPainter::Antialiasing, true );
    }
    // clear buffer pixmap
    dbuffer.fill( bgcolor );
    // draw grid as needed
    int i1, cl1;

    if ( preferences.getDrawGrid() == GRID_SQUARE || preferences.getDrawGrid() == GRID_HEX )
        painter->drawPixmap( 0, 0, grid );

    painter->scale( zoomFactor, zoomFactor );

    // draw all data
    c->drawAll();

    qDebug() << "paintEvent mode: " << mode;
    // local/temporary draw operations
    if ( highlightpoint != 0 ) {
        qDebug() << "tempBox";
        drawBox( QPoint( qRound( highlightpoint->x - 2.0 ), qRound( highlightpoint->y - 2.0 ) ), QPoint( qRound( highlightpoint->x + 2.0 ), qRound( highlightpoint->y + 2.0 ) ), QColor( 0, 0, 0 ) );
    }
    // if multiple selection, draw rotate and resize handles
    if ( mode == MODE_SELECT_MULTIPLE ) {
      drawBox( zoomCorrectReverse( selectOrigin ), zoomCorrectReverse( selectPrev ), QColor( 0, 0, 0 ) );
    }

    // draw box if over molecule in Tool modes
    if ( ( mode >= 600 ) && ( mode < 698 ) ) {
      if (tmp_molecule != 0) {
	qDebug() << "tool mode";
	drawBox( ( selectionBox.topLeft() ), ( selectionBox.bottomRight() ), QColor( 0, 0, 255 ) );
      }
    }

    if ( ( mode > 120 ) && ( mode < 129 ) ) {
        // draw selection box
        qDebug() << "multiple selectionBox";
        drawBox( ( selectionBox.topLeft() ), ( selectionBox.bottomRight() ), QColor( 0, 0, 0 ) );
        // draw rotate handle
        rotateHandle = selectionBox.topRight();
        drawFillBox( ( QPoint( rotateHandle.x() - 5, rotateHandle.y() ) ), ( QPoint( rotateHandle.x(), rotateHandle.y() + 5 ) ), QColor( 0, 0, 0 ) );
        // draw resize handle
        resizeHandle = selectionBox.bottomRight();
        drawFillBox( ( QPoint( resizeHandle.x() - 5, resizeHandle.y() - 5 ) ), ( resizeHandle ), QColor( 0, 0, 0 ) );
    }

    if ( mode == 91 ) {
        // draw selection box
        qDebug() << "simple selectionBox";
        drawBox( ( selectionBox.topLeft() ), ( selectionBox.bottomRight() ), QColor( 0, 0, 0 ) );
        // draw rotate handle
        rotateHandle = selectionBox.topRight();
        drawFillBox( ( QPoint( rotateHandle.x() - 5, rotateHandle.y() ) ), ( QPoint( rotateHandle.x(), rotateHandle.y() + 5 ) ), QColor( 0, 0, 0 ) );
        // draw resize handle
        resizeHandle = selectionBox.bottomRight();
        drawFillBox( ( QPoint( resizeHandle.x() - 5, resizeHandle.y() - 5 ) ), ( resizeHandle ), QColor( 0, 0, 0 ) );
    }

    /// previews
    if ( mode == MODE_SELECT_LASSO ) {
      drawPolyline( tmp_lasso, Qt::blue );
    } else if ( mode == MODE_DRAWLINE_DRAWING ) {
      drawLine( startPoint, endPoint, thick, currentColor );
    } else if ( mode == MODE_DRAWLINE_DASH_DRAWING ) {
      drawLine( startPoint, endPoint, thick, currentColor, 1 );
    } else if ( mode == MODE_DRAWCHAIN_DRAWING ) {
      drawPolyline( chainPoints, currentColor );
    } else if ( mode == MODE_DRAWLINE_UP_DRAWING ) {
      drawUpLine( startPoint, endPoint, currentColor );
    } else if ( mode == MODE_DRAWLINE_DOWN_DRAWING ) {
      drawDownLine( startPoint, endPoint, currentColor );
    } else if ( mode == MODE_DRAWWAVYLINE_DRAWING ) {
      drawWavyLine( startPoint, endPoint, currentColor );
    } else if ( mode == MODE_DRAWARROW_DRAWING ) {
      drawArrow( startPoint, endPoint, currentColor, bracket_type, thick );
    } else if ( mode == MODE_DRAWCURVEARROW_DRAWING ) {
      drawCurveArrow( startPoint, endPoint, currentColor, symbolfile );
    } else if ( mode == MODE_DRAWBRACKET_DRAWING ) {
      drawBracket( startPoint, endPoint, currentColor, bracket_type, currentColor );
    }
    // now bitBlt buffer onto widget
//    bitBlt(this, 0, 0, &dbuffer);
    finishedPainting = true;
    qDebug() << "paintEvent painter end";
    painter->end();
}

void Render2D::zoomEvent()
{
    // read zoom factor from preferences and act accordingly
    double ozf = zoomFactor;

    zoomFactor = ( double ) preferences.getZoom() / 100.0;

    // scale canvas
    double sf = zoomFactor / ozf;

    renderWidth = qRound( ( double ) renderWidth * sf );
    renderHeight = qRound( ( double ) renderHeight * sf );
    dbuffer = QPixmap( renderWidth, renderHeight );
    resize( renderWidth, renderHeight );
    createGrid();
    // redraw
    update();
}

void Render2D::createGrid()
{
    grid = QPixmap( renderWidth, renderHeight );
    grid.fill( bgcolor );
    QPainter *gridPainter = new QPainter( &grid );

    ///TODO: make that an option
    gridPainter->setRenderHint( QPainter::Antialiasing, true );

    int i1, cl1;

    if ( preferences.getDrawGrid() == GRID_SQUARE ) {
        i1 = qRound( preferences.getGridSpacing() * zoomFactor );
        int width = parentWidget()->size().width();
        int height = parentWidget()->size().height();

        gridPainter->setPen( QPen( QColor( 128, 128, 128 ), 1, Qt::DotLine ) );
        // draw horizontal
        for ( cl1 = i1; cl1 < renderHeight; cl1 += i1 ) {
            gridPainter->drawLine( 0, cl1, renderWidth, cl1 );
        }

        // draw vertical
        for ( cl1 = i1; cl1 < renderWidth; cl1 += i1 ) {
            gridPainter->drawLine( cl1, 0, cl1, renderHeight );
        }
    } else if ( preferences.getDrawGrid() == GRID_HEX ) {
        bool hexoffset = true;

        i1 = qRound( preferences.getGridSpacing() * zoomFactor );
        float hexhoriz = i1 / 2.0;
        float hexgap = 2.0 * hexhoriz + i1;
        float hexelevation = hexhoriz * 1.73205;
        float hex_initx, hex_rightx;

        gridPainter->setPen( QPen( QColor( 128, 128, 128 ), 1, Qt::DotLine ) );
        for ( float hex_y = 0.0; hex_y < renderHeight; hex_y += hexelevation ) {
            if ( hexoffset )
                hex_initx = hexhoriz;
            else
                hex_initx = hexgap;
            for ( float hex_x = hex_initx; hex_x < renderWidth; hex_x += hexgap ) {
                hex_rightx = hex_x + i1;
                gridPainter->drawLine( hex_x, hex_y, hex_rightx, hex_y );
                gridPainter->drawLine( hex_x, hex_y, hex_x - hexhoriz, hex_y + hexelevation );
                gridPainter->drawLine( hex_rightx, hex_y, hex_rightx + hexhoriz, hex_y + hexelevation );
                hex_x = hex_rightx;
            }
            hexoffset = !hexoffset;
        }
    }
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
