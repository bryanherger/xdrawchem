#include <iostream>
#include <math.h>
#include <qwidget.h>
#include <qpainter.h>
#include <qdragobject.h>
#include <qglobal.h>
#include <qcursor.h>
#include <qmessagebox.h>
#include <qcolordialog.h>

using std::cout;
using std::endl;

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
QPoint Render2D::GridLock(QPoint qp1) {
  if (mode < 200)
    return qp1;
  if (mode > 299)
    return qp1;

  int d1 = my_round( (double)preferences.getGridSpacing() * zoomFactor );

  if (preferences.getDrawGrid() == GRID_SQUARE) {
    double gs = (double)d1, gx = (double)qp1.x(), gy = (double)qp1.y();
    int rx, ry;
    rx = my_round(gx / gs);
    ry = my_round(gy / gs);
    qp1.setX(rx * d1);
    qp1.setY(ry * d1);
    
    repaint(false);
    directdraw = true;
    painter->begin(this);
    drawBox(QPoint( qp1.x() - 2, 
		    qp1.y() - 2 ),
	    QPoint( qp1.x() + 2, 
		    qp1.y() + 2 ),
	    QColor(255,0,0));
    painter->end();
    directdraw = false;
    
    return qp1;
  }
  if (preferences.getDrawGrid() == GRID_HEX) {
    bool hexoffset = true;
    double hexhoriz = (double)d1 / 2.0;
    double hexgap = 2.0 * hexhoriz + (double)d1;
    double hexelevation = hexhoriz * 1.73205;
    double hex_initx, hex_rightx, dx, dy;
    double maxdist = 9999.0, max_x = 0.0, max_y = 0.0;
    for (double hex_y = 0.0; hex_y < renderHeight; hex_y += hexelevation) {
      if (hexoffset)
	hex_initx = hexhoriz;
      else
	hex_initx = hexgap;
      for (double hex_x = hex_initx; hex_x < renderWidth; hex_x += hexgap) {
	hex_rightx = hex_x + (double)d1;
	dx = hex_x - qp1.x();
	dy = hex_y - qp1.y();
	if (sqrt(dx * dx + dy * dy) < maxdist) {
	  max_x = hex_x; max_y = hex_y;
	  maxdist = sqrt(dx * dx + dy * dy);
	}
	hex_x = hex_rightx;
	dx = hex_x - qp1.x();
	dy = hex_y - qp1.y();
	if (sqrt(dx * dx + dy * dy) < maxdist) {
	  max_x = hex_x; max_y = hex_y;
	  maxdist = sqrt(dx * dx + dy * dy);
	}
      }
      hexoffset = !hexoffset;
    }    
    qp1.setX((int)max_x); qp1.setY((int)max_y);
    return qp1;
  }

  return qp1;
}

// right-click popup choices from mousePressEvent popup (rtclickpopup)
void Render2D::molInfo() {
  std::cerr << "Popup-info" << std::endl;
  DPoint *tmp_pt2 = new DPoint;
  tmp_pt2->set( targetmol->BoundingBoxAll().center() );
  c->Tool(tmp_pt2, MODE_TOOL_MOLECULE_INFO);
}
void Render2D::molSelect() {
  DeselectAll();
  targetmol->SelectAll();
  selectionBox = c->selectionBox();
  mode = MODE_SELECT_MULTIPLE_SELECTED;  
  repaint(false);
}
void Render2D::molCut() {
  DeselectAll();
  targetmol->SelectAll();
  Cut();
  std::cerr << "popup-Cut" << std::endl;
}
void Render2D::molCopy() {
  DeselectAll();
  targetmol->SelectAll();
  Copy();
  DeselectAll();
  std::cerr << "popup-Copy" << std::endl;
}
void Render2D::bondEdit() {
  if (highlightobject->Type() == TYPE_TEXT) {
    doubleClickFlag = true;
    prev_mode = mode;
    mode = MODE_TEXT;
    DrawText_mousePressEvent( 0, QPoint(0,0) );
    return;
  }
  highlightobject->Edit();
  highlightobject->Highlight(false);
  highlightobject = 0;
  if (mode == MODE_SELECT)
    setCursor(arrowCursor);
  else
    setCursor(crossCursor);
  mouse3down = false;
  repaint(false);
}
void Render2D::bondInfo() {
  QString objinf;
  objinf = tr("No information.");

  if (highlightobject->Type() == TYPE_BOND) {
    objinf = "ID: ";
    objinf.append( ((Bond*)highlightobject)->CName() );
    objinf.append( "\nReactions: " );
    objinf.append( ((Bond*)highlightobject)->getReactions() );
    // make it easier to add to retro.txt
    std::cout << "ID:" << ((Bond*)highlightobject)->CName() << std::endl;
  }

  QMessageBox::information( 0, tr("Object information"), objinf);
}
void Render2D::textShape() {
  localtext = (Text *)highlightobject;
  TextShapeDialog *tsd = 
    new TextShapeDialog(this, "text shape dialog");

  tsd->set_stype( localtext->getShape() );
  tsd->setBorderCheck( localtext->getBorder() );
  tsd->setFillCheck( localtext->getFill() );
  tsd->setBorderColor( localtext->getBorderColor() );
  tsd->setFillColor( localtext->getFillColor() );
  tsd->setWidth( localtext->getShapeWidth() );
  tsd->setHeight( localtext->getShapeHeight() );

  if ( tsd->exec() ) {
    std::cout << "textShape OK" << std::endl;
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
void Render2D::bracketFill() {
  Bracket *lbracket = (Bracket *)highlightobject;
  QColor b1color = lbracket->getFillColor();

  b1color = QColorDialog::getColor(b1color);

  if (b1color.isValid() == true) {
    lbracket->setFill(true);
    lbracket->setFillColor(b1color);
  }
}
void Render2D::bracketFillOff() {
  Bracket *lbracket = (Bracket *)highlightobject;
  lbracket->setFill(false);
}

void Render2D::mousePressEvent(QMouseEvent *e1) {
  if (localstring == "XDC_DELETE_ME") {
    delete localtexteditor;
    localtexteditor = 0;
    localstring = " ";
  }

  QPoint curqpt = zoomCorrectReverse( e1->pos() );

  if (preferences.getSnapGrid() != GRID_NONE) {
    curqpt = GridLock(curqpt);
  }

  bool allowedit = false;
  moved = false;

  DPoint tmp_pt(curqpt.x(), curqpt.y());
  DPoint *tmp_pt2 = new DPoint;
  tmp_pt2->x = curqpt.x(); tmp_pt2->y = curqpt.y();

  if (mode > 100)
    c->StartUndo(mode, 0);   // BMS: StartUndo does nothing with the point - don't copy!
  //cout << "Click" << endl;
  // update button state
  if (e1->button() == LeftButton)
    mouse1down = true;
  if (e1->button() == RightButton) {
    mouse3down = true;
  }
  //cout << "start at " << curqpt.x() << "," << curqpt.y() << endl;

  // can the current mode use right button popup menu?
  bool rbm = false;
  if (mode == MODE_SELECT) rbm = true;
  if (mode == MODE_SELECT_MOVE_OBJECT) rbm = true;
  if (mode == MODE_DRAWBRACKET) rbm = true;

  if ( rbm && mouse3down ) {
    //if (rtclickpopup != 0) delete rtclickpopup;
    rtclickpopup = new QPopupMenu(this);
    int id1;
    bool menu_ok = false;
    if ( (mode == MODE_DRAWBRACKET) && (highlightobject == 0) ) {
      // put nearest bracket into highlightobject
      DPoint *e = new DPoint;
      e->x = curqpt.x(); e->y = curqpt.y();
      double distobj;
      // get Drawable of nearest object
      Drawable *no = c->FindNearestObject(e, distobj);
      if ( (distobj < 5.0) && (no->Type() == TYPE_BRACKET) )
	highlightobject = no;
    }
    if (highlightobject != 0) {
      QString rtmt;
      rtmt = tr("Object");
      if (highlightobject->Type() == TYPE_ARROW)
	rtmt = tr("Arrow");
      if (highlightobject->Type() == TYPE_BOND)
	rtmt = tr("Bond");
      if (highlightobject->Type() == TYPE_BRACKET)
	rtmt = tr("Bracket");
      if (highlightobject->Type() == TYPE_CURVEARROW)
	rtmt = tr("Curved Arrow");
      if (highlightobject->Type() == TYPE_SYMBOL)
	rtmt = tr("Symbol");
      if (highlightobject->Type() == TYPE_TEXT)
	rtmt = tr("Text");
      id1 = rtclickpopup->insertItem( rtmt ); 
      rtclickpopup->setItemEnabled(id1, false);
      rtclickpopup->insertItem( tr("Edit"), this, SLOT(bondEdit()) ); 
      if (highlightobject->Type() == TYPE_BOND)
	rtclickpopup->insertItem( tr("Info"), this, SLOT(bondInfo()) ); 
      if (highlightobject->Type() == TYPE_BRACKET) {
	rtclickpopup->insertItem( tr("Fill color..."), this, SLOT(bracketFill()) ); 
	rtclickpopup->insertItem( tr("No fill"), this, SLOT(bracketFillOff()) ); 
      }
      if (highlightobject->Type() == TYPE_TEXT)
	rtclickpopup->insertItem( tr("Shape..."), this, SLOT(textShape()) ); 
      menu_ok = true;
    }
    targetmol = c->insideMolecule(tmp_pt2);
    if (targetmol != 0) {
      id1 = rtclickpopup->insertItem( tr("Molecule") ); 
      rtclickpopup->setItemEnabled(id1, false);
      rtclickpopup->insertItem( tr("Info"), this, SLOT(molInfo()) ); 
      rtclickpopup->insertItem( tr("Select"), this, SLOT(molSelect()) ); 
      rtclickpopup->insertItem( tr("Cut"), this, SLOT(molCut()) ); 
      rtclickpopup->insertItem( tr("Copy"), this, SLOT(molCopy()) ); 
      menu_ok = true;
    }
    mouse3down = false;
    if (menu_ok)
      rtclickpopup->popup( mapToGlobal(e1->pos()) );
  }

  // MODE_TEXT with right button
  if ( (mode == MODE_TEXT) && mouse3down ) {
    rtclickpopup = new QPopupMenu(this);
    int id1;
    bool menu_ok = false;
    if (highlightobject != 0) {
      QString rtmt;
      rtmt = tr("Object");
      if (highlightobject->Type() == TYPE_TEXT)
	rtmt = tr("Text");
      id1 = rtclickpopup->insertItem( rtmt ); 
      rtclickpopup->setItemEnabled(id1, false);
      rtclickpopup->insertItem( tr("Edit"), this, SLOT(bondEdit()) ); 
      if (highlightobject->Type() == TYPE_TEXT)
	rtclickpopup->insertItem( tr("Shape..."), this, SLOT(textShape()) ); 
      menu_ok = true;
    }
    mouse3down = false;
    if (menu_ok) {
      rtclickpopup->popup( mapToGlobal(e1->pos()) );
      return;
    }
  }

  // MODE_TEXT: call special function to handle text events
  if (mode == MODE_TEXT) {
    DrawText_mousePressEvent(e1, curqpt);
    return;
  }

    // MODE_SELECT_LASSO: start lasso
    if ( mode == MODE_SELECT_LASSO ) {
      c->DeselectAll();
      tmp_lasso.resize(1);
      tmp_lasso.setPoint(0, curqpt);
      return;
    }

    // MODE_SELECT: start multiple selection
    if ( (mode == MODE_SELECT) && (mouse1down) ) {
      if ( ( highlightpoint == 0) && (highlightobject == 0) ) {
	selectOrigin = e1->pos();
	mode = MODE_SELECT_MULTIPLE;
	return;
      } else {
	cout << "Should move." << endl;
	if (highlightpoint == 0) {
	  mode = MODE_SELECT_MOVE_OBJECT;
	} else {
	  if (highlightpoint->other != 0) {
	    mode = MODE_DRAWLINE_DRAWING;
	    savedBondOrder = highlightpoint->otherBond->Order();
	    c->Erase(highlightpoint->otherBond);
	    if (savedBondOrder == 2)
	      c->Erase(highlightpoint->otherBond);
	    if (savedBondOrder == 3)
	      c->Erase(highlightpoint->otherBond);
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
    if (mode == MODE_SELECT)
      allowedit = true;
    // get Drawable of nearest object
    DPoint *e = new DPoint;
    e->x = curqpt.x();
    e->y = curqpt.y();
    double distobj;
    Drawable *no = c->FindNearestObject(e, distobj);
    if (distobj > 5.0)
        no = 0;
    if (no != 0) {
        if ( (mode == MODE_DRAWLINE) &&
                (no->Type() == TYPE_BOND) )
            allowedit = true;
        if ( (mode == MODE_DRAWLINE_DASH) &&
                (no->Type() == TYPE_BOND) )
            allowedit = true;
        if ( (mode == MODE_DRAWLINE_UP) &&
                (no->Type() == TYPE_BOND) )
            allowedit = true;
        if ( (mode == MODE_DRAWLINE_DOWN) &&
                (no->Type() == TYPE_BOND) )
            allowedit = true;
    }
    // allowedit and BUTTON3: edit item under mouse
    if ( (allowedit) && (mouse3down) ) {
        if (highlightobject == 0)
            highlightobject = no;
        if (highlightobject != 0) {
            if (highlightobject->Type() == TYPE_TEXT) {
                setCursor(ibeamCursor);
                mode = MODE_TEXT;
                text_exists = true;
                localtext = (Text *)highlightobject;
                emit TextOn(localtext->getFont());
                repaint(false);
                return;
            }
            cout << "Will edit" << endl;
            highlightobject->Edit();
            highlightobject->Highlight(false);
            highlightobject = 0;
            if (mode == MODE_SELECT)
                setCursor(arrowCursor);
            else
                setCursor(crossCursor);
            mouse3down = false;
            repaint(false);
        }
        return;
    }

    // MODE_SELECT_MULTIPLE_SELECTED: either move, or deselect all
    if ( (mode == MODE_SELECT_MULTIPLE_SELECTED) && (mouse1down) ) {
        // check for move
        if (selectionBox.contains( curqpt )) {
            c->StartUndo(0,0);
            mode = MODE_SELECT_MOVE_MULTIPLE;
        } else {
            c->DeselectAll();
            mode = MODE_SELECT;
            repaint(false);
        }
    }

    // MODE_SELECT_MULTIPLE_SELECTED and Button3: Drag and Drop!
    if ( (mode == MODE_SELECT_MULTIPLE_SELECTED) && (mouse3down) ) {
        // check for move
        if (selectionBox.contains( curqpt )) {
            dragging = true;
        } else {
            c->DeselectAll();
            mode = MODE_SELECT;
            repaint(false);
        }
    }

    // MODE_SELECT_MULTIPLE_RESIZE: resize drawing
    if ( (mode == MODE_SELECT_MULTIPLE_RESIZE) && (mouse1down) ) {
        mode = MODE_SELECT_RESIZE_MULTIPLE;
    }

    // MODE_SELECT_MULTIPLE_ROTATE: rotate drawing
    if ( (mode == MODE_SELECT_MULTIPLE_ROTATE) && (mouse1down) ) {
        mode = MODE_SELECT_ROTATE_MULTIPLE;
        rotateOrigin.setX( (selectionBox.right() + selectionBox.left() - 2) / 2);
        rotateOrigin.setY( (selectionBox.bottom() + selectionBox.top()) / 2);
        prevangle = getAngle( rotateOrigin, curqpt );
    }

    // MODE_DRAWLINE (draw line tool selected), and left button.
    // This means we start drawing a line
    if ( (mode == MODE_DRAWLINE) && (mouse1down) ) {
        startpoint = highlightpoint; // both NULL if nothing selected
        highlightpoint = 0; // stop drawing highlight box
        if (startpoint == NULL) {
            startpoint = new DPoint;
            startpoint->set(curqpt);
            endpoint = NULL;
        }
        mode = MODE_DRAWLINE_DRAWING;
    }

    // MODE_DRAWWAVYLINE (draw line tool selected), and left button.
    // This means we start drawing a line
    if ( (mode == MODE_DRAWWAVYLINE) && (mouse1down) ) {
      startpoint = highlightpoint; // both NULL if nothing selected
      highlightpoint = 0; // stop drawing highlight box
      if (startpoint == NULL) {
	startpoint = new DPoint;
	startpoint->set(curqpt);
	endpoint = NULL;
      }
      mode = MODE_DRAWWAVYLINE_DRAWING;
    }

    // MODE_DRAWCHAIN (draw chain tool selected), and left button.
    // This means we start drawing an aliphatic chain
    if ( (mode == MODE_DRAWCHAIN) && (mouse1down) ) {
        startpoint = highlightpoint; // both NULL if nothing selected
        highlightpoint = 0; // stop drawing highlight box
        if (startpoint == NULL) {
            startpoint = new DPoint;
            startpoint->set(curqpt);
            endpoint = NULL;
        }
        mode = MODE_DRAWCHAIN_DRAWING;
    }

    // MODE_DRAWLINE_DASH (draw dashed line tool selected), and left button.
    // This means we start drawing a dashed line
    if ( (mode == MODE_DRAWLINE_DASH) && (mouse1down) ) {
        startpoint = highlightpoint; // both NULL if nothing selected
        highlightpoint = 0; // stop drawing highlight box
        if (startpoint == NULL) {
            startpoint = new DPoint;
            startpoint->set(curqpt);
            endpoint = NULL;
        }
        mode = MODE_DRAWLINE_DASH_DRAWING;
    }

    // MODE_DRAWLINE_UP (draw stereo-up line tool selected), and left button.
    // This means we start drawing a line
    if ( (mode == MODE_DRAWLINE_UP) && (mouse1down) ) {
        startpoint = highlightpoint; // both NULL if nothing selected
        highlightpoint = 0; // stop drawing highlight box
        if (startpoint == NULL) {
            startpoint = new DPoint;
            startpoint->set(curqpt);
            endpoint = NULL;
        }
        mode = MODE_DRAWLINE_UP_DRAWING;
    }

    // MODE_DRAWLINE_DOWN (draw stereo-down line tool selected), and left button.
    // This means we start drawing a line
    if ( (mode == MODE_DRAWLINE_DOWN) && (mouse1down) ) {
        startpoint = highlightpoint; // both NULL if nothing selected
        highlightpoint = 0; // stop drawing highlight box
        if (startpoint == NULL) {
            startpoint = new DPoint;
            startpoint->set(curqpt);
            endpoint = NULL;
        }
        mode = MODE_DRAWLINE_DOWN_DRAWING;
    }

    // MODE_DRAWARROW (draw arrow tool selected), and left button.
    // This means we start drawing an arrow
    if ( (mode == MODE_DRAWARROW) && (mouse1down) ) {
        startpoint = new DPoint;
        startpoint->set(curqpt);
        endpoint = NULL;
        mode = MODE_DRAWARROW_DRAWING;
    }

    // MODE_DRAWCURVEARROW (draw curve arrow tool selected), and left button.
    // This means we start drawing an arrow
    if ( (mode == MODE_DRAWCURVEARROW) && (mouse1down) ) {
        startpoint = new DPoint;
        startpoint->set(curqpt);
        endpoint = NULL;
        mode = MODE_DRAWCURVEARROW_DRAWING;
    }

    // MODE_DRAWBRACKET (draw bracket tool selected), and left button.
    // This means we start drawing an bracket
    if ( (mode == MODE_DRAWBRACKET) && (mouse1down) ) {
        startpoint = new DPoint;
        startpoint->set(curqpt);
        endpoint = NULL;
        mode = MODE_DRAWBRACKET_DRAWING;
    }

    // MODE_ERASE, and left button.  Erase highlighted object.
    if ( (mode == MODE_ERASE) && (mouse1down) ) {
        if (highlightobject == 0)
            return;
        c->Erase(highlightobject);
        highlightobject = 0;
        repaint(false);
    }
}

void Render2D::mouseDoubleClickEvent(QMouseEvent *e1) {
  if (localstring == "XDC_DELETE_ME") {
    delete localtexteditor;
    localtexteditor = 0;
    localstring = " ";
  }

  QPoint curqpt = zoomCorrectReverse( e1->pos() );

  if (preferences.getSnapGrid() != GRID_NONE) {
    curqpt = GridLock(curqpt);
  }

  bool allowedit = false;
  moved = false;

  DPoint tmp_pt(curqpt.x(), curqpt.y());
  DPoint *tmp_pt2 = new DPoint;
  tmp_pt2->x = curqpt.x(); tmp_pt2->y = curqpt.y();

  std::cout << "mouseDoubleClickEvent received" << std::endl;

  if (highlightobject != 0) {
    if (highlightobject->Type() == TYPE_TEXT) {
      std::cout << "Activate text object?" << std::endl;
      doubleClickFlag = true;
      prev_mode = mode;
      mode = MODE_TEXT;
      DrawText_mousePressEvent(e1, curqpt);
      return; 
    }
  }
}

void Render2D::mouseReleaseEvent(QMouseEvent *e1) {
  std::cout << "release MODE: " << mode << std::endl;

  DPoint *tmp_pt;
  XDC_Event *evt;
  QPoint curqpt = zoomCorrectReverse( e1->pos() );

  if (preferences.getSnapGrid() != GRID_NONE) {
    curqpt = GridLock(curqpt);
  }

    //cout << "Unclick" << endl;
    // update button state
    if (e1->button() == LeftButton)
        mouse1down = false;
    if (e1->button() == RightButton)
        mouse3down = false;
    //cout << "end at " << curqpt.x() << "," << curqpt.y() << endl;

    // MODE_TEXT: call special function to handle text events
    if (mode == MODE_TEXT) {
        DrawText_mouseReleaseEvent(e1, curqpt);
        return;
    }

    // MODE_SELECT_LASSO: finish lasso
    if ( mode == MODE_SELECT_LASSO ) {
      // TODO: validate loop
      // TODO: if loop is OK ("closed") then select points and draw box
      int lasso_insert = tmp_lasso.size();
      tmp_lasso.resize(lasso_insert + 1);
      tmp_lasso.setPoint(lasso_insert, tmp_lasso.point(0));
      if (c->SelectWithinLoop(tmp_lasso)) {
	std::cout << "points within loop" << std::endl;
	bool shiftdown = false;
	if ((e1->state() & ShiftButton) == ShiftButton)
	  shiftdown = true;
	//c->NewSelectRect(QRect(selectOrigin, selectPrev), shiftdown);
	selectionBox = c->selectionBox();
	if (selectionBox.left() < 999)
	  mode = MODE_SELECT_MULTIPLE_SELECTED;
	else
	  mode = MODE_SELECT_LASSO;
	repaint(false);
      }

      repaint(false);
      return;
    }

    // Control-LeftClick invokes Molecule Information window
    if ( (e1->button() == LeftButton) &&
	 ((e1->state() & ControlButton) == ControlButton) ) {
      tmp_pt = new DPoint(curqpt);
      c->Tool(tmp_pt, MODE_TOOL_MOLECULE_INFO);
      repaint(false);
      return;
    }

    // MODE_DRAWBEZIER
    if (mode == MODE_DRAWBEZIER) {
      repaint(false);
      directdraw = true;
      painter->begin(this);
      if (bezier_count < 3) {
	tmp_bezier.setPoint(bezier_count, curqpt.x(), curqpt.y());
	drawBezier(tmp_bezier, currentColor, true);
	bezier_count++;
      } else {
	tmp_bezier.setPoint(bezier_count, curqpt.x(), curqpt.y());
	// add new GraphicObject
	evt = new XDC_Event(EVT_ADD_GRAPHIC);
	evt->setPointArray(tmp_bezier);
	evt->setParam1(bracket_type);
	evt->setColor(currentColor);
	emit XDCEventSignal(evt);
	drawBezier(tmp_bezier, currentColor, false, bracket_type);
	tmp_bezier.resize(0); tmp_bezier.resize(4);
	bezier_count = 0;
      }
      directdraw = false;
      painter->end();
      return;
    }

    // MODE_TOOL_*: ChemData handles this, mostly
    if (mode == MODE_TOOL_RETRO_BONDNAME) {
      std::cout << "mode invoke" << std::endl;
      // retro_bondname handled by Molecule containing this bond
      if (highlightobject != 0) {
	std::cout << "invoke" << std::endl;
	std::cout << c->insideMolecule(highlightobject->Start())->RetroBondName((Bond*)highlightobject, true) << std::endl;
	highlightobject->Highlight(false);
	highlightobject = 0;
	repaint(false);
	setMode_Select();
      }
      return;
    }
    if ( (mode > 600) && (mode < 699) ) {
      tmp_pt = new DPoint(curqpt);
      c->Tool(tmp_pt, mode);
      repaint(false);
      //if (mode == MODE_TOOL_CLEANUPMOL) {
      setMode_Select();
      //}
      return;
    }

    // MODE_SELECT_MULTIPLE: finish multiple selection
    if ( (mode == MODE_SELECT_MULTIPLE) && (!mouse1down) ) {
      int tmp1;
      if (selectOrigin.x() > selectPrev.x()) {
	tmp1 = selectOrigin.x();
	selectOrigin.setX(selectPrev.x());
	selectPrev.setX(tmp1);
      }
      if (selectOrigin.y() > selectPrev.y()) {
	tmp1 = selectOrigin.y();
	selectOrigin.setY(selectPrev.y());
	selectPrev.setY(tmp1);
      }
      repaint(false);
      directdraw = true;
      painter->begin(this);
      drawBox(selectOrigin, selectPrev, QColor(255,255,255));
      bool shiftdown = false;
      if ((e1->state() & ShiftButton) == ShiftButton)
	shiftdown = true;
      c->NewSelectRect( QRect( zoomCorrectReverse(selectOrigin), 
			       zoomCorrectReverse(selectPrev) ), 
			shiftdown);
      selectionBox = c->selectionBox();
      if (selectionBox.left() < 999)
	mode = MODE_SELECT_MULTIPLE_SELECTED;
      else
	mode = MODE_SELECT;
      directdraw = false;
      painter->end();
      return;
    }

    // MODE_SELECT_MOVE_MULTIPLE: finish moving
    if ( (mode == MODE_SELECT_MOVE_MULTIPLE) && (!mouse1down) ) {
        mode = MODE_SELECT_MULTIPLE_SELECTED;
        c->FinishMove();
        repaint(false);
    }

    // MODE_SELECT_MOVE_OBJECT: finish moving
    if ( (mode == MODE_SELECT_MOVE_OBJECT) && (!mouse1down) ) {
        mode = MODE_SELECT;
        c->FinishMove();
        repaint(false);
    }

    // MODE_SELECT_MOVE_POINT: finish moving
    if ( (mode == MODE_SELECT_MOVE_POINT) && (!mouse1down) ) {
        mode = MODE_SELECT;
        c->FinishMove();
        repaint(false);
    }

    // MODE_SELECT_RESIZE_MULTIPLE: finish resizing
    if ( (mode == MODE_SELECT_RESIZE_MULTIPLE) && (!mouse1down) ) {
        mode = MODE_SELECT_MULTIPLE_SELECTED;
    }

    // MODE_SELECT_ROTATE_MULTIPLE: finish rotating
    if ( (mode == MODE_SELECT_ROTATE_MULTIPLE) && (!mouse1down) ) {
        mode = MODE_SELECT_MULTIPLE_SELECTED;
    }

    // MODE_DRAWLINE_DRAWING (drawing line), and left button.
    // Finish line
    if ( (mode == MODE_DRAWLINE_DRAWING) && (!mouse1down) ) {
      if (moved) {
	if (endpoint == NULL) {
	  endpoint = new DPoint;
	  endpoint->set
	    (curqpt);
	}
	// submit this line/bond to ChemData
	if (startpoint != endpoint) {
	  evt = new XDC_Event(EVT_ADD_BOND);
	  evt->setStart(startpoint);
	  evt->setEnd(endpoint);
	  evt->setParam1(thick);
	  evt->setParam2(savedBondOrder);
	  evt->setColor(currentColor);
	  emit XDCEventSignal(evt);
	  savedBondOrder = -1;
	  //c->addBond(startpoint, endpoint, thick, 1, currentColor);
	}
      }
      startpoint = NULL;
      endpoint = NULL;
      if (prev_mode == MODE_SELECT) {
	mode = MODE_SELECT;
	setCursor(arrowCursor);
	emit SignalSetStatusBar( tr("Select mode: left click on object to move, right click on object to edit") );
	prev_mode = 0;
      } else {
	mode = MODE_DRAWLINE;
      }
      repaint(false);
      return;
    }

    // MODE_DRAWCHAIN_DRAWING (drawing chain), and left button.
    // Finish chain
    if ( (mode == MODE_DRAWCHAIN_DRAWING) && (!mouse1down) ) {
      if (moved) {
	if (endpoint == NULL) {
	  endpoint = new DPoint;
	  endpoint->set(curqpt);
	}
	// submit this set of lines/bonds to ChemData
	double ang, len;
	QString sang, slen;
	ang = Drawable::getAngle(startpoint, endpoint);
	double alt_ang1 = (ang - 30.0) * M_PI / 180.0;
	double alt_ang2 = (ang + 30.0) * M_PI / 180.0;
	double alt_swap;
	if ( (e1->state() & ShiftButton) == ShiftButton) {
	  alt_swap = alt_ang1; alt_ang1 = alt_ang2; alt_ang2 = alt_swap;
	}
	len = startpoint->distanceTo(endpoint);
	double blen = preferences.getBond_fixedlength();
	int n1, nseg = my_round(len / blen);
	QPoint spoint, epoint, ipoint1, ipoint2;
	spoint = startpoint->toQPoint();
	ipoint1 = spoint;
	for (n1 = 0; n1 < nseg; n1++) {
	  ipoint2.setX( my_round(ipoint1.x() + cos(alt_ang1) * blen) );
	  ipoint2.setY( my_round(ipoint1.y() + sin(alt_ang1) * blen) );
	  //drawLine( ipoint1, ipoint2, 1, currentColor );
	  evt = new XDC_Event(EVT_ADD_BOND);
	  if (n1 == 0)
	    evt->setStart(startpoint);
	  else
	    evt->setStart(endpoint);
	  endpoint = new DPoint(ipoint2);
	  evt->setEnd(endpoint);
	  evt->setParam1(thick);
	  evt->setParam2(1);
	  evt->setColor(currentColor);
	  emit XDCEventSignal(evt);
	  alt_swap = alt_ang1; alt_ang1 = alt_ang2; alt_ang2 = alt_swap;
	  ipoint1 = ipoint2;
	}
      }
      startpoint = NULL;
      endpoint = NULL;
      mode = MODE_DRAWCHAIN;
      repaint(false);
      return;
    }

    // MODE_DRAWLINE_DASH_DRAWING (drawing line), and left button.
    // Finish line
    if ( (mode == MODE_DRAWLINE_DASH_DRAWING) && (!mouse1down) ) {
        if (moved) {
            if (endpoint == NULL) {
                endpoint = new DPoint;
                endpoint->set
                (curqpt);
            }
            // submit this line/bond to ChemData
	    evt = new XDC_Event(EVT_ADD_BOND_DASH);
	    evt->setStart(startpoint);
	    evt->setEnd(endpoint);
	    evt->setParam1(thick);
	    evt->setColor(currentColor);
	    emit XDCEventSignal(evt);
            //c->addBond(startpoint, endpoint, thick, 99, currentColor);
        }
        startpoint = NULL;
        endpoint = NULL;
        mode = MODE_DRAWLINE_DASH;
        repaint(false);
        return;
    }

    // MODE_DRAWLINE_UP_DRAWING (drawing line), and left button.
    // Finish line
    if ( (mode == MODE_DRAWLINE_UP_DRAWING) && (!mouse1down) ) {
        if (moved) {
            if (endpoint == NULL) {
                endpoint = new DPoint;
                endpoint->set
                (curqpt);
            }
            // submit this line/bond to ChemData
	    evt = new XDC_Event(EVT_ADD_BOND_UP);
	    evt->setStart(startpoint);
	    evt->setEnd(endpoint);
	    evt->setColor(currentColor);
	    emit XDCEventSignal(evt);
            //c->addBond(startpoint, endpoint, 1, 5, currentColor);
        }
        startpoint = NULL;
        endpoint = NULL;
        mode = MODE_DRAWLINE_UP;
        repaint(false);
        return;
    }

    // MODE_DRAWLINE_DOWN_DRAWING (drawing line), and left button.
    // Finish line
    if ( (mode == MODE_DRAWLINE_DOWN_DRAWING) && (!mouse1down) ) {
        if (moved) {
            if (endpoint == NULL) {
                endpoint = new DPoint;
                endpoint->set
                (curqpt);
            }
            // submit this line/bond to ChemData
	    evt = new XDC_Event(EVT_ADD_BOND_DOWN);
	    evt->setStart(startpoint);
	    evt->setEnd(endpoint);
	    evt->setColor(currentColor);
	    emit XDCEventSignal(evt);
            //c->addBond(startpoint, endpoint, 1, 7, currentColor);
        }
        startpoint = NULL;
        endpoint = NULL;
        mode = MODE_DRAWLINE_DOWN;
        repaint(false);
        return;
    }

    // MODE_DRAWWAVYLINE_DRAWING (drawing line), and left button.
    // Finish line
    if ( (mode == MODE_DRAWWAVYLINE_DRAWING) && (!mouse1down) ) {
      if (moved) {
	if (endpoint == NULL) {
	  endpoint = new DPoint;
	  endpoint->set
	    (curqpt);
	}
	// submit this line/bond to ChemData
	evt = new XDC_Event(EVT_ADD_BOND_WAVY);
	evt->setStart(startpoint);
	evt->setEnd(endpoint);
	evt->setColor(currentColor);
	emit XDCEventSignal(evt);
      }
      startpoint = NULL;
      endpoint = NULL;
      mode = MODE_DRAWWAVYLINE;
      repaint(false);
      return;
    }

    // MODE_DRAWARROW_DRAWING (drawing arrow), and left button.
    // Finish arrow
    if ( (mode == MODE_DRAWARROW_DRAWING) && (!mouse1down) ) {
        if (moved) {
            if (endpoint == NULL) {
                endpoint = new DPoint;
                endpoint->set
                (curqpt);
            }
            // submit this arrow to ChemData
	    evt = new XDC_Event(EVT_ADD_ARROW);
	    evt->setStart(startpoint);
	    evt->setEnd(endpoint);
	    evt->setColor(currentColor);
	    evt->setParam1(bracket_type);
	    evt->setParam2(thick);
	    emit XDCEventSignal(evt);
            //c->addArrow(startpoint, endpoint, currentColor, bracket_type);
        }
        startpoint = NULL;
        endpoint = NULL;
        mode = MODE_DRAWARROW;
        repaint(false);
    }

    // MODE_DRAWCURVEARROW_DRAWING (drawing curved arrow), and left button.
    // Finish arrow
    if ( (mode == MODE_DRAWCURVEARROW_DRAWING) && (!mouse1down) ) {
        endpoint = new DPoint;
        endpoint->set
        (curqpt);
	// submit this arrow to ChemData
	evt = new XDC_Event(EVT_ADD_CURVEARROW);
	evt->setStart(startpoint);
	evt->setEnd(endpoint);
	evt->setColor(currentColor);
	evt->setText(symbolfile);
	emit XDCEventSignal(evt);
        //c->addCurveArrow(startpoint, endpoint, currentColor, symbolfile);
        startpoint = NULL;
        endpoint = NULL;
        mode = MODE_DRAWCURVEARROW;
        repaint(false);
    }

    // MODE_DRAWBRACKET_DRAWING (drawing bracket), and left button.
    // Finish bracket
    if ( (mode == MODE_DRAWBRACKET_DRAWING) && (!mouse1down) ) {
        if (endpoint == NULL) {
            endpoint = new DPoint;
            endpoint->set(curqpt);
        }
        // submit this bracket to ChemData
	evt = new XDC_Event(EVT_ADD_BRACKET);
	evt->setStart(startpoint);
	evt->setEnd(endpoint);
	evt->setColor(currentColor);
	evt->setParam1(bracket_type);
	emit XDCEventSignal(evt);
        //c->addBracket(startpoint, endpoint, currentColor, bracket_type);
        startpoint = NULL;
        endpoint = NULL;
        mode = MODE_DRAWBRACKET;
        repaint(false);
    }

    // MODE_SYMBOL: create Symbol
    if ( (mode == MODE_SYMBOL) && (!mouse1down) ) {
      if (highlightpoint == 0) {
	endpoint = new DPoint(curqpt);
      } else {
	endpoint = highlightpoint;
      }
      if (symbolfile == "antibody") {
	c->DeselectAll();
	DPoint *ab0, *ab1, *ab2;
	double dab, dabscale;
	dabscale = preferences.getBond_fixedlength();
	dab = dabscale / 1.41421;
	ab0 = new DPoint(endpoint);
	ab0->y = ab0->y - dabscale;
	ab1 = new DPoint(ab0);
	ab2 = new DPoint(ab0);
	ab1->x = ab1->x - dab;
	ab1->y = ab1->y - dab;
	ab2->x = ab2->x + dab;
	ab2->y = ab2->y - dab;
	preferences.setBondCenter(true);
	evt = new XDC_Event(EVT_ADD_BOND);
	evt->setStart(endpoint);
	evt->setEnd(ab0);
	evt->setParam1(2);
	evt->setParam2(2);
	evt->setColor(currentColor);
	evt->setBool1(true);
	emit XDCEventSignal(evt);
	preferences.setBondCenter(false);
	evt = new XDC_Event(EVT_ADD_BOND);
	evt->setStart(ab0);
	evt->setEnd(ab1);
	evt->setParam1(1);
	evt->setParam2(2);
	evt->setColor(currentColor);
	evt->setBool1(true);
	emit XDCEventSignal(evt);
	evt = new XDC_Event(EVT_ADD_BOND);
	evt->setStart(ab0);
	evt->setEnd(ab2);
	evt->setParam1(1);
	evt->setParam2(2);
	evt->setBool1(true);
	evt->setColor(currentColor);
	emit XDCEventSignal(evt);
	mode = MODE_SELECT_MULTIPLE_SELECTED;
	selectionBox = c->selectionBox();
        repaint(false);
	endpoint = 0;
	return;
      }
      if (highlightpoint != 0) {
	c->addSymbol(highlightpoint, symbolfile);  // add to existing point
      } else {
	c->addSymbol(endpoint, symbolfile);  // add at current mouse position
      }
      endpoint = NULL;
      repaint(false);
    }

    // MODE_DRAWNEWMAN_*: create Newman projection
    if ( (mode == MODE_DRAWNEWMAN_ANTI) && (!mouse1down) ) {
      endpoint = new DPoint;
      endpoint->set(curqpt);
      double blen = preferences.getBond_fixedlength();
      int n1;
      QPoint ipoint1 = endpoint->toQPoint();
      QPoint ipoint2;
      for (n1 = 30; n1 < 331; n1 = n1 + 60) {
	ipoint2.setX( my_round(ipoint1.x() + cos( (double)n1 * M_PI / 180.0) * blen) );
	ipoint2.setY( my_round(ipoint1.y() + sin( (double)n1 * M_PI / 180.0) * blen) );
	tmp_pt = new DPoint(ipoint2);
	evt = new XDC_Event(EVT_ADD_BOND);
	evt->setStart(endpoint);
	evt->setEnd(tmp_pt);
	evt->setParam1(thick);
	evt->setColor(currentColor);
	emit XDCEventSignal(evt);
      }
      c->addSymbol(endpoint, "newman");
      endpoint = NULL;
      repaint(false);
    }

    // MODE_DRAWNEWMAN_*: create Newman projection
    if ( (mode == MODE_DRAWNEWMAN_GAUCHE) && (!mouse1down) ) {
      endpoint = new DPoint;
      endpoint->set(curqpt);
      double blen = preferences.getBond_fixedlength();
      int n1;
      QPoint ipoint1 = endpoint->toQPoint();
      QPoint ipoint2;
      for (n1 = 30; n1 < 331; n1 = n1 + 120) {
	ipoint2.setX( my_round(ipoint1.x() + cos( (double)n1 * M_PI / 180.0) * blen) );
	ipoint2.setY( my_round(ipoint1.y() + sin( (double)n1 * M_PI / 180.0) * blen) );
	tmp_pt = new DPoint(ipoint2);
	evt = new XDC_Event(EVT_ADD_BOND);
	evt->setStart(endpoint);
	evt->setEnd(tmp_pt);
	evt->setParam1(thick);
	evt->setColor(currentColor);
	emit XDCEventSignal(evt);
      }
      for (n1 = 50; n1 < 351; n1 = n1 + 120) {
	ipoint2.setX( my_round(ipoint1.x() + cos( (double)n1 * M_PI / 180.0) * blen) );
	ipoint2.setY( my_round(ipoint1.y() + sin( (double)n1 * M_PI / 180.0) * blen) );
	tmp_pt = new DPoint(ipoint2);
	evt = new XDC_Event(EVT_ADD_BOND);
	evt->setStart(endpoint);
	evt->setEnd(tmp_pt);
	evt->setParam1(thick);
	evt->setColor(currentColor);
	emit XDCEventSignal(evt);
      }
      c->addSymbol(endpoint, "newman");
      endpoint = NULL;
      repaint(false);
    }

    // MODE_RING: place ring
    if ( (mode == MODE_RING) && (!mouse1down) ) {
        endpoint = new DPoint;
        endpoint->set
        (curqpt);
	c->setThickKludge(thick);
        // are we smart-placing this ring?
        if ( (smartplace != 0) && (highlightpoint != 0) ) {
	  cout << "smart placement! Mode " << smartplace << endl;
	  if (smartplace == 1)
	    c->SmartPlace(symbolfile, highlightpoint);
	  if (smartplace == 2)
	    c->SmartPlaceToo(symbolfile, highlightpoint);
	  if (smartplace == 3)
	    c->SmartPlaceThree(symbolfile, highlightpoint);
	  // Clear highlighted point
	  highlightpoint = 0;
	  endpoint = NULL;
	  mode = MODE_SELECT_MULTIPLE_SELECTED;
	  emit SignalSetStatusBar( tr("Select mode: left click on object to move") );
	  setCursor(sizeAllCursor);
	  selectionBox = c->selectionBox();
	  repaint(false);
	  smartplace = 0;
	  c->setThickKludge(-1);
	  return;
        }
        // translate ring to click point
        c->load(symbolfile);
	c->setThickKludge(-1);
        mode = MODE_SELECT_MULTIPLE_SELECTED;
        emit SignalSetStatusBar( tr("Select mode: left click on object to move, right click on object to edit") );
        setCursor(sizeAllCursor);
        selectionBox = c->selectionBox();
        double dx = endpoint->x - selectionBox.center().x();
        double dy = endpoint->y - selectionBox.center().y();
        //cout << "Move by " << dx << "," << dy << endl;
        c->Move(dx, dy);
        selectionBox.moveBy((int)dx, (int)dy);

        endpoint = NULL;
        repaint(false);
        smartplace = 0;
        return;
    }
}

void Render2D::mouseMoveEvent(QMouseEvent *e1) {
  if (localstring == "XDC_DELETE_ME") {
    delete localtexteditor;
    localtexteditor = 0;
    localstring = " ";
  }

  if (mode == MODE_DRAWBRACKET) highlightobject = 0;

  if ( (mode == MODE_RING) && mouse1down )
    return;

  if ( (mode == MODE_DRAWBEZIER) && mouse1down )
    return;

  QPoint curqpt = zoomCorrectReverse( e1->pos() );
  if (preferences.getSnapGrid() != GRID_NONE) {
    curqpt = GridLock(curqpt);
  }

  lastmouse = curqpt;

  moved = true;

  DPoint tmp_pt(curqpt.x(), curqpt.y()); 
  Molecule *tmp_mol;
  tmp_molecule = 0;

  // Tools
  if ((mode >= 600) && (mode < 698)) {
    tmp_molecule = c->insideMolecule(&tmp_pt);
    if (tmp_molecule != 0) {
      repaint(false);
      directdraw = true;
      painter->begin(this);
      QRect mr = tmp_molecule->BoundingBoxAll();
      mr.setLeft(mr.left() - 2);
      mr.setRight(mr.right() + 2);
      mr.setTop(mr.top() - 2);
      mr.setBottom(mr.bottom() + 2);
      drawBox( mr.topLeft(), mr.bottomRight(), QColor(0,0,255) );
      painter->end();
      directdraw = false;
    } else {
      repaint(false);
    }
  }

  if (mode == MODE_TOOL_CALCMW) {
    // check if we're over a molecule
    tmp_mol = c->insideMolecule(&tmp_pt);
    if (tmp_mol != 0) {
      Text *tmp_txt = tmp_mol->CalcMW();
      emit SignalSetStatusBar( tmp_txt->getText() +
			       tr(", click to paste into drawing") );
      delete tmp_txt;
    } else {
      emit SignalSetStatusBar( tr("Click on a molecule to calculate its molecular weight") );
    }
    return;
  }
  if (mode == MODE_TOOL_CALCEF) {
    // check if we're over a molecule
    tmp_mol = c->insideMolecule(&tmp_pt);  // doesn't copy pointer
    if (tmp_mol != 0) {
      Text *tmp_txt = tmp_mol->CalcEmpiricalFormula();
      emit SignalSetStatusBar( tmp_txt->getText() +
			       tr(", click to paste into drawing") );
      delete tmp_txt;
    } else {
      emit SignalSetStatusBar( tr("Click on a molecule to calculate its empirical formula") );
    }
    return;
  }

  // MODE_TOOL_*: return
  if ((mode >= 600) && (mode < 698)) 
    return;

  // Dragging
  if (dragging && mouse3down) {
    cout << "Start drag" << endl;
    // make pixmap of selection
    QRect savebox = selectionBox;
    int prevmode = mode;
    mode = MODE_SELECT;
    c->DeselectAll();
    //repaint(false);
    QPixmap pm( savebox.size() );
    bitBlt( &pm, 0, 0, this, savebox.left(), savebox.top(),
	    savebox.width(), savebox.height() );
    c->NewSelectRect(savebox, false);
    selectionBox = c->selectionBox();
    mode = prevmode;
    drag1 = new QImageDrag( pm.convertToImage(), this );
    drag1->dragCopy();
    dragging = false;
    repaint(false);
    setCursor(arrowCursor);
    mouse3down = false;
    return;
  }

  // MODE_TEXT: call special function to handle text events
  if (mode == MODE_TEXT) {
    DrawText_mouseMoveEvent(e1);
    if (prevpos == 0) { 
      prevpos = new DPoint;
    }
    prevpos->x = curqpt.x();
    prevpos->y = curqpt.y();
    return;
  }
  // MODE_SELECT: call special function to handle select events
  if (mode < 199) {
    Select_mouseMoveEvent(e1);
    if (prevpos == 0) { 
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
  DPoint *np = 0;
  e->x = curqpt.x();
  e->y = curqpt.y();
  double dist, distobj;
  // Get DPoint of nearest point
  np = c->FindNearestPoint(e, dist);
  // get Drawable of nearest object
  Drawable *no = c->FindNearestObject(e, distobj);

  // no buttons down
  if ( (mouse1down == false) && (mouse3down == false) ) {
    if (mode == MODE_DRAWARROW)
      return;

    // if (no != 0) {
    // highlight object if object close enough
    if ( (no != 0) && (mode == MODE_ERASE) && (distobj < 6.0) ) {
      highlightobject = no;
      if (prevhighlightobject != 0)
	prevhighlightobject->Highlight(false);
      highlightobject->Highlight(true);
      if (prevhighlightobject != highlightobject)
	repaint(false);
    }
    // unhighlight object if no object close
    if ( (mode == MODE_ERASE) && (no == 0 || distobj >= 6.0) ) {
      // Clear highlighted object
      highlightobject = 0;
      if (prevhighlightobject != 0)
	prevhighlightobject->Highlight(false);
      if (prevhighlightobject != highlightobject)
	repaint(false);
      return;
    }
    if ( (no != 0) && (mode == MODE_TOOL_RETRO_BONDNAME) && (distobj < 6.0) ) {
      if (no->Type() == TYPE_BOND) {
	highlightobject = no;
	if (prevhighlightobject != 0) {
	  prevhighlightobject->Highlight(false);
	}
	highlightobject->Highlight(true);
	if (prevhighlightobject != highlightobject) {
	  repaint(false);
	}
      }
    }
    // unhighlight object if no object close
    if ( (mode == MODE_TOOL_RETRO_BONDNAME) && (no == 0 || distobj >= 6.0) ) {
      // Clear highlighted object
      highlightobject = 0;
      if (prevhighlightobject != 0)
	prevhighlightobject->Highlight(false);
      if (prevhighlightobject != highlightobject)
	repaint(false);
      return;
    }
    // highlight text object preferentially when MODE_SELECT...
    if ((no != 0) && (mode == MODE_SELECT) && (no->Type() == TYPE_TEXT) ) {
      highlightpoint = 0;
      highlightobject = no;
      if ( (prevhighlightobject != highlightobject) &&
	   (prevhighlightobject != 0) )
	prevhighlightobject->Highlight(false);
      highlightobject->Highlight(true);
      if (prevhighlightobject != highlightobject)
	repaint(false);
      // return since no need to check points
      return;
    }
    // highlight object if object close enough and no point close
    if ((no != 0) && (mode == MODE_SELECT) && (distobj < 6.0) && (dist >= 8.0) ) {
      highlightobject = no;
      if (prevhighlightobject != 0)
	prevhighlightobject->Highlight(false);
      highlightobject->Highlight(true);
      if (prevhighlightobject != highlightobject)
	repaint(false);
      // return since no need to check points
      return;
    }
    // unhighlight object if no object close
    if ( (mode == MODE_SELECT) && (no == 0 || distobj >= 6.0) ) {
      // Clear highlighted object
      highlightobject = 0;
      if (prevhighlightobject != 0)
	prevhighlightobject->Highlight(false);
      if (prevhighlightobject != highlightobject)
	repaint(false);
      // don't return; go on to check for points
    }
    // unhighlight object if point close
    if ( (mode == MODE_SELECT) && (dist < 8.0) ) {
      // Clear highlighted object
      highlightobject = 0;
      if (prevhighlightobject != 0)
	prevhighlightobject->Highlight(false);
      if (prevhighlightobject != highlightobject)
	repaint(false);
      // don't return; go on to check for points
    }
    // }
    if (np != 0) {
      //cout << dist << " to (" << np->x << "," << np->y << ")" << endl;
      if ( (mode == MODE_TEXT) && (dist < 8.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_TEXT) && (dist >= 8.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_SYMBOL) && (dist < 8.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_SYMBOL) && (dist >= 8.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_SELECT) && (dist < 8.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_SELECT) && (dist >= 8.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE) && (dist < 6.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE) && (dist >= 6.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_RING) && smartplace && (dist < 6.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_RING) && smartplace && (dist >= 6.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE_DASH) && (dist < 6.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE_DASH) && (dist >= 6.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE_UP) && (dist < 6.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE_UP) && (dist >= 6.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE_DOWN) && (dist < 6.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWLINE_DOWN) && (dist >= 6.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWCHAIN) && (dist < 6.0) ) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
      if ( (mode == MODE_DRAWCHAIN) && (dist >= 6.0) ) {
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint)
	  repaint(false);
	return;
      }
    }
    return; // rest of function only if mouse button down
  }

  // we're probably going to draw something...
  repaint(false);
  directdraw = true;

  // MODE_DRAWLINE_DRAWING: draw temporary line
  if ( (mode == MODE_DRAWLINE_DRAWING) && mouse1down ) {
    // if within range of existing point AND the end point is not the same
    // as the start point, snap to that point
    if ( (dist < 6.0) && (startpoint != np) ) {
      highlightpoint = np;
      endpoint = np;
    } else {
      endpoint = new DPoint;
      endpoint->x = curqpt.x();
      endpoint->y = curqpt.y();
      highlightpoint = 0;  // stop highlighting
      if (preferences.getBond_fixed()) { // fix endpoint if fixed set
	CorrectEndpoint_bond();
	np = c->FindNearestPoint(endpoint, dist);
	if (dist < 2.0) {
	  highlightpoint = np;
	  endpoint = np;
	}
      }
    }
    ang = Drawable::getAngle(startpoint, endpoint);
    len = startpoint->distanceTo(endpoint);
    sang.setNum(ang);
    slen.setNum(len);
    emit SignalSetStatusBar( tr("Draw Line - Length = ") + slen + tr(", Angle = ") + sang);
    painter->begin(this);
    drawLine( startpoint->toQPoint(), endpoint->toQPoint(), thick,
	      currentColor );
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWCHAIN_DRAWING: draw temporary aliphatic chain
  if ( (mode == MODE_DRAWCHAIN_DRAWING) && mouse1down ) {
    // if within range of existing point AND the end point is not the same
    // as the start point, snap to that point
    if ( (dist < 6.0) && (startpoint != np) ) {
      highlightpoint = np;
      endpoint = np;
    } else {
      endpoint = new DPoint;
      endpoint->x = curqpt.x();
      endpoint->y = curqpt.y();
      highlightpoint = 0;  // stop highlighting
    }
    ang = Drawable::getAngle(startpoint, endpoint);
    double alt_ang1 = (ang - 30.0) * M_PI / 180.0;
    double alt_ang2 = (ang + 30.0) * M_PI / 180.0;
    double alt_swap;
    if ( (e1->state() & ShiftButton) == ShiftButton) {
      alt_swap = alt_ang1; alt_ang1 = alt_ang2; alt_ang2 = alt_swap;
    }
    len = startpoint->distanceTo(endpoint);
    double blen = preferences.getBond_fixedlength();
    int n1, nseg = my_round(len / blen);
    slen.setNum(nseg);
    emit SignalSetStatusBar( tr("Draw aliphatic chain - Length = ") + slen + tr(" bonds") );
    QPoint spoint, epoint, ipoint1, ipoint2;
    spoint = startpoint->toQPoint();
    ipoint1 = spoint;
    painter->begin(this);
    for (n1 = 0; n1 < nseg; n1++) {
      ipoint2.setX( my_round(ipoint1.x() + cos(alt_ang1) * blen) );
      ipoint2.setY( my_round(ipoint1.y() + sin(alt_ang1) * blen) );
      drawLine( ipoint1, ipoint2, 1, currentColor );
      alt_swap = alt_ang1; alt_ang1 = alt_ang2; alt_ang2 = alt_swap;
      ipoint1 = ipoint2;
    }
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWLINE_DASH_DRAWING: draw temporary line
  if ( (mode == MODE_DRAWLINE_DASH_DRAWING) && mouse1down ) {
    // if within range of existing point AND the end point is not the same
    // as the start point, snap to that point
    if ( (dist < 6.0) && (startpoint != np) ) {
      highlightpoint = np;
      endpoint = np;
    } else {
      endpoint = new DPoint;
      endpoint->x = curqpt.x();
      endpoint->y = curqpt.y();
      highlightpoint = 0;  // stop highlighting
      if (preferences.getBond_fixed()) { // fix endpoint if fixed set
	CorrectEndpoint_bond();
	np = c->FindNearestPoint(endpoint, dist);
	if (dist < 2.0) {
	  highlightpoint = np;
	  endpoint = np;
	}
      }
    }
    painter->begin(this);
    drawLine( startpoint->toQPoint(), endpoint->toQPoint(), 1,
	      currentColor, 1 );
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWLINE_UP_DRAWING: draw temporary line
  if ( (mode == MODE_DRAWLINE_UP_DRAWING) && mouse1down ) {
    // if within range of existing point AND the end point is not the same
    // as the start point, snap to that point
    if ( (dist < 6.0) && (startpoint != np) ) {
      highlightpoint = np;
      endpoint = np;
    } else {
      endpoint = new DPoint;
      endpoint->x = curqpt.x();
      endpoint->y = curqpt.y();
      highlightpoint = 0;  // stop highlighting
      if (preferences.getBond_fixed()) { // fix endpoint if fixed set
	CorrectEndpoint_bond();
	np = c->FindNearestPoint(endpoint, dist);
	if (dist < 2.0) {
	  highlightpoint = np;
	  endpoint = np;
	}
      }
    }
    painter->begin(this);
    drawUpLine( startpoint->toQPoint(), endpoint->toQPoint(), currentColor );
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWLINE_DOWN_DRAWING: draw temporary line
  if ( (mode == MODE_DRAWLINE_DOWN_DRAWING) && mouse1down ) {
    // if within range of existing point AND the end point is not the same
    // as the start point, snap to that point
    if ( (dist < 6.0) && (startpoint != np) ) {
      highlightpoint = np;
      endpoint = np;
    } else {
      endpoint = new DPoint;
      endpoint->x = curqpt.x();
      endpoint->y = curqpt.y();
      highlightpoint = 0;  // stop highlighting
      if (preferences.getBond_fixed()) { // fix endpoint if fixed set
	CorrectEndpoint_bond();
	np = c->FindNearestPoint(endpoint, dist);
	if (dist < 2.0) {
	  highlightpoint = np;
	  endpoint = np;
	}
      }
    }
    painter->begin(this);
    drawDownLine(startpoint->toQPoint(), endpoint->toQPoint(), currentColor);
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWWAVYLINE_DRAWING: draw temporary line
  if ( (mode == MODE_DRAWWAVYLINE_DRAWING) && mouse1down ) {
    // if within range of existing point AND the end point is not the same
    // as the start point, snap to that point
    if ( (dist < 6.0) && (startpoint != np) ) {
      highlightpoint = np;
      endpoint = np;
    } else {
      endpoint = new DPoint;
      endpoint->x = curqpt.x();
      endpoint->y = curqpt.y();
      highlightpoint = 0;  // stop highlighting
      if (preferences.getBond_fixed()) { // fix endpoint if fixed set
	CorrectEndpoint_bond();
	np = c->FindNearestPoint(endpoint, dist);
	if (dist < 2.0) {
	  highlightpoint = np;
	  endpoint = np;
	}
      }
    }
    painter->begin(this);
    drawWavyLine(startpoint->toQPoint(), endpoint->toQPoint(), currentColor);
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWARROW_DRAWING: draw temporary arrow
  if ( (mode == MODE_DRAWARROW_DRAWING) && mouse1down ) {
    endpoint = new DPoint;
    endpoint->x = curqpt.x();
    endpoint->y = curqpt.y();
    if (preferences.getArrow_fixed()) { // fix endpoint if fixed set
      CorrectEndpoint_arrow();
    }
    painter->begin(this);
    drawArrow( startpoint->toQPoint(), endpoint->toQPoint(), currentColor,
	       bracket_type, thick );
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWCURVEARROW_DRAWING: draw temporary curve arrow
  if ( (mode == MODE_DRAWCURVEARROW_DRAWING) && mouse1down ) {
    endpoint = new DPoint;
    endpoint->x = curqpt.x();
    endpoint->y = curqpt.y();
    // should not draw curve arrows fixed, even if selected
    //if (fixed) { // fix endpoint if fixed set
    //  CorrectEndpoint();
    //}
    painter->begin(this);
    drawCurveArrow( startpoint->toQPoint(), endpoint->toQPoint(),
		    currentColor, symbolfile );
    directdraw = false;
    painter->end();
    return;
  }

  // MODE_DRAWBRACKET_DRAWING: draw temporary bracket
  if ( (mode == MODE_DRAWBRACKET_DRAWING) && mouse1down ) {
    endpoint = new DPoint;
    endpoint->x = curqpt.x();
    endpoint->y = curqpt.y();
    // adjust position for square and circle "brackets"
    if ( (bracket_type == BRACKET_CLOSEDSQUARE) || 
	 (bracket_type == BRACKET_CIRCLE) ) {
      double bdx = endpoint->x - startpoint->x;
      double bdy = endpoint->y - startpoint->y;
      double mgn = fabs(bdx / bdy);
      if (bdx > bdy) {
	bdy *= mgn;
	endpoint->y = startpoint->y + bdy;
      }
      if (bdx < bdy) {
	bdx /= mgn;
	endpoint->x = startpoint->x + bdx;
      }
      bdx = endpoint->x - startpoint->x;
      bdy = endpoint->y - startpoint->y;
      std::cout << "after:" << bdx << "," << bdy << std::endl;
    }
    QColor icv;
    painter->begin(this);
    drawBracket( startpoint->toQPoint(), endpoint->toQPoint(), currentColor,
		 bracket_type, icv );
    directdraw = false;
    painter->end();
    return;
  }

  // just in case
  directdraw = false;
}

// handle resize event
void Render2D::resizeEvent(QResizeEvent *rz) {
  dbuffer.resize(renderWidth, renderHeight);
}

void Render2D::paintEvent(QPaintEvent *) {
  if (localstring == "XDC_DELETE_ME") {
    delete localtexteditor;
    localtexteditor = 0;
    localstring = " ";
  }
  // draw background
  //drawFillBox(QPoint(0,0), QPoint(renderWidth, renderHeight), bgcolor);

  // initialize global painter object
  if (outputDevice == OUTPUT_PRINTER) {
    std::cout << "printer" << std::endl;
    painter->begin(printer);
  }
  if (outputDevice == OUTPUT_SCREEN) {
    std::cout << "screen" << std::endl;
    if (directdraw) 
      painter->begin(this);
    else
      painter->begin(&dbuffer);
    painter->scale(zoomFactor, zoomFactor);
  }

  // clear buffer pixmap
  dbuffer.fill(bgcolor);
  // draw grid as needed
  int i1, cl1;
  if (preferences.getDrawGrid() == GRID_SQUARE) {
    i1 = my_round( (double)preferences.getGridSpacing() * zoomFactor );
    painter->setPen( QPen(QColor(128,128,128), 1, Qt::DotLine) );
    // draw horizontal
    for (cl1 = i1; cl1 < renderHeight; cl1 += i1) {
      painter->drawLine(0,cl1,renderWidth,cl1);
    }
    // draw vertical
    for (cl1 = i1; cl1 < renderWidth; cl1 += i1) {
      painter->drawLine(cl1,0,cl1,renderHeight);
    }
  }
  if (preferences.getDrawGrid() == GRID_HEX) {
    bool hexoffset = true;
    i1 = my_round( (double)preferences.getGridSpacing() * zoomFactor );
    double hexhoriz = (double)i1 / 2.0;
    double hexgap = 2.0 * hexhoriz + (double)i1;
    double hexelevation = hexhoriz * 1.73205;
    double hex_initx, hex_rightx;
    painter->setPen( QPen(QColor(128,128,128), 1, Qt::DotLine) );
    for (double hex_y = 0.0; hex_y < renderHeight; hex_y += hexelevation) {
      if (hexoffset)
	hex_initx = hexhoriz;
      else
	hex_initx = hexgap;
      for (double hex_x = hex_initx; hex_x < renderWidth; hex_x += hexgap) {
	hex_rightx = hex_x + (double)i1;
	painter->drawLine(hex_x,hex_y,hex_rightx,hex_y);
	painter->drawLine(hex_x, hex_y, hex_x - hexhoriz, 
			  hex_y + hexelevation);
	painter->drawLine(hex_rightx, hex_y, 
			  hex_rightx + hexhoriz, hex_y + hexelevation);
	hex_x = hex_rightx;
      }
      hexoffset = !hexoffset;
    }
  }
  // draw all data
  c->drawAll();
  // local/temporary draw operations
  if (highlightpoint != 0) {
    drawBox(QPoint( my_round(highlightpoint->x - 2.0), 
		    my_round(highlightpoint->y - 2.0) ),
	    QPoint( my_round(highlightpoint->x + 2.0), 
		    my_round(highlightpoint->y + 2.0) ),
	    QColor(0,0,0));
  }

  // if multiple selection, draw rotate and resize handles
  if ( (mode > 120) && (mode < 129) ) {
    // draw selection box
    drawBox( (selectionBox.topLeft() ), 
	     (selectionBox.bottomRight() ),
	     QColor(0,0,0));
    // draw rotate handle
    rotateHandle = selectionBox.topRight();
    drawFillBox( ( QPoint(rotateHandle.x() - 5, rotateHandle.y()) ),
		 ( QPoint(rotateHandle.x(), rotateHandle.y() + 5) ),
		 QColor(0,0,0) );
    // draw resize handle
    resizeHandle = selectionBox.bottomRight();
    drawFillBox( ( QPoint(resizeHandle.x() - 5, resizeHandle.y() - 5) ),
		 ( resizeHandle ),
		 QColor(0,0,0) );
  }
  if (mode == 91) {
    // draw selection box
    drawBox( ( selectionBox.topLeft() ), 
	     ( selectionBox.bottomRight() ),
	     QColor(0,0,0));
    // draw rotate handle
    rotateHandle = selectionBox.topRight();
    drawFillBox( ( QPoint(rotateHandle.x() - 5, rotateHandle.y()) ),
		 ( QPoint(rotateHandle.x(), rotateHandle.y() + 5) ),
		 QColor(0,0,0) );
    // draw resize handle
    resizeHandle = selectionBox.bottomRight();
    drawFillBox( ( QPoint(resizeHandle.x() - 5, resizeHandle.y() - 5) ),
		 ( resizeHandle ),
		 QColor(0,0,0) );
  }
  // now bitBlt buffer onto widget
  bitBlt(this, 0, 0, &dbuffer);
  finishedPainting = true;
  painter->end();
}

void Render2D::zoomEvent() {
  // read zoom factor from preferences and act accordingly
  double ozf = zoomFactor;
  zoomFactor = (double)preferences.getZoom() / 100.0;

  // scale canvas
  double sf = zoomFactor / ozf;
  renderWidth = my_round( (double)renderWidth * sf );
  renderHeight = my_round( (double)renderHeight * sf );
  dbuffer.resize(renderWidth, renderHeight);
  resize(renderWidth, renderHeight);

  // redraw
  repaint(false);
}
