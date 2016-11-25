#include <iostream>
#include <math.h>
#include <qwidget.h>
#include <qpainter.h>

using std::cout;
using std::endl;

#include "render2d.h"
#include "chemdata.h"
#include "text.h"
#include "defs.h"

void Render2D::Select_mouseMoveEvent(QMouseEvent *e1) {
  // MODE_SELECT_LASSO handled slightly differently
  if ( (mode == MODE_SELECT_LASSO) && (mouse1down == false) )
    return;
  if ( (mode == MODE_SELECT_LASSO) && (mouse1down == true) ) {
    int lasso_insert = tmp_lasso.size();
    tmp_lasso.resize(lasso_insert + 1);
    tmp_lasso.setPoint(lasso_insert, e1->pos());
    repaint(false);
    directdraw = true;
    painter->begin(this);
    drawPolyline(tmp_lasso, QColor(0,0,255));
    painter->end();
    directdraw = false;    
    return;
  }

  //bool update;
  DPoint *prevhighlight = highlightpoint;
  Drawable *prevhighlightobject = highlightobject;
  // Create DPoint of current pointer position
  QPoint curqpt = zoomCorrectReverse( e1->pos() );
  DPoint e(curqpt.x(),curqpt.y());
  DPoint *np = 0;
 
  double dist, distobj, dx, dy;
  // Get DPoint of nearest point
  np = c->FindNearestPoint(&e, dist);
  // get Drawable of nearest object
  Drawable *no = c->FindNearestObject(&e, distobj);

  //cout << "np = " << dist << " no = " << distobj;
  //if (np == 0) cout << " (np = 0)";
  //cout << endl;
  // no buttons down
  if ( (mouse1down == false) && (mouse3down == false) ) {
    if (mode == MODE_SELECT_MULTIPLE_SELECTED) {
      // check if near rotate or resize handle
      if (DistanceBetween(resizeHandle, curqpt) < 4.0) {
	setCursor(sizeFDiagCursor);
	mode = MODE_SELECT_MULTIPLE_RESIZE;
	return;
      }
      if (DistanceBetween(rotateHandle, curqpt) < 4.0) {
	setCursor(rotatecursor);
	mode = MODE_SELECT_MULTIPLE_ROTATE;
	return;
      }
      // else, check if in selection box (for move)
      if (selectionBox.contains( curqpt ))
	setCursor(sizeAllCursor);
      else
	setCursor(arrowCursor);
      return;
    }
    if (mode == MODE_SELECT_MULTIPLE_RESIZE) {
      if (DistanceBetween(resizeHandle, e1->pos()) > 4.0) {
	mode = MODE_SELECT_MULTIPLE_SELECTED;
	if (selectionBox.contains( e1->pos() ))
	  setCursor(sizeAllCursor);
	else
	  setCursor(arrowCursor);
	return;
      }
      return; // do nothing if still on resize handle
    }
    if (mode == MODE_SELECT_MULTIPLE_ROTATE) {
      if (DistanceBetween(rotateHandle, e1->pos()) > 4.0) {
	mode = MODE_SELECT_MULTIPLE_SELECTED;
	if (selectionBox.contains( e1->pos() ))
	  setCursor(sizeAllCursor);
	else
	  setCursor(arrowCursor);
	return;
      }
      return; // do nothing if still on resize handle
    }
    if (no != 0) {
      // highlight text and symbol objects preferentially when MODE_SELECT...
      if ( (no->Type() == TYPE_TEXT) || (no->Type() == TYPE_SYMBOL) ){
	highlightpoint = 0;
	highlightobject = no;
	if ( (prevhighlightobject != highlightobject) &&
	     (prevhighlightobject != 0) )
	  prevhighlightobject->Highlight(false);
	highlightobject->Highlight(true);
	if (prevhighlightobject != highlightobject) repaint(false);
	setCursor(sizeAllCursor);
	// return since no need to check points
	return;
      }
      // highlight object if object close enough and no point close
      if ( (distobj < 6.0) && (dist >= 8.0) ) {
	highlightpoint = 0;
	highlightobject = no;
	if (prevhighlightobject != 0) prevhighlightobject->Highlight(false);
	highlightobject->Highlight(true);
	if (prevhighlightobject != highlightobject) repaint(false);
	setCursor(sizeAllCursor);
	// return since no need to check points
	return;
      }
      // unhighlight object if no object close
      if (distobj >= 6.0) {
	// Clear highlighted object
	highlightobject = 0;
	if (prevhighlightobject != 0) prevhighlightobject->Highlight(false);
	if (prevhighlightobject != highlightobject) repaint(false);
	// don't return; go on to check for points
      }
      // unhighlight object if point close
      if (dist < 8.0) {
	// Clear highlighted object
	highlightobject = 0;
	if (prevhighlightobject != 0) prevhighlightobject->Highlight(false);
	if (prevhighlightobject != highlightobject) repaint(false);
	// don't return; go on to check for points
      }
    }
    if (np == 0) {
      highlightpoint = 0;
      highlightobject = 0;
      if (prevhighlightobject != 0) {
	prevhighlightobject->Highlight(false);
	repaint(false);
      }
      setCursor(arrowCursor);
      return;
    }
    if (np != 0) {
      //cout << dist << " to (" << np->x << "," << np->y << ")" << endl;
      if (dist < 8.0) {
	highlightpoint = np;
	if (prevhighlight != highlightpoint) repaint(false);
	setCursor(sizeAllCursor);
	return;
      } else {
	// Clear object, if any
	if (prevhighlightobject != 0) {
	  prevhighlightobject->Highlight(false);
	  highlightobject = 0;
	  repaint(false);
	}
	// Clear highlighted point
	highlightpoint = 0;
	if (prevhighlight != highlightpoint) repaint(false);
	setCursor(arrowCursor);
	return;
      }
    }
    return; // rest of function only if mouse button down
  }

  if ( ( mode == MODE_SELECT_MULTIPLE) && (mouse1down) ) {
    drawBox( zoomCorrectReverse(selectOrigin), 
	     zoomCorrectReverse(selectPrev), 
	     QColor(255,255,255));
    selectPrev = e1->pos();
    c->NewSelectRect( QRect( zoomCorrectReverse(selectOrigin), 
			     zoomCorrectReverse(selectPrev) ), 
		      false);
    repaint(false);
    directdraw = true;
    painter->begin(this);
    drawBox( zoomCorrectReverse(selectOrigin), 
	     zoomCorrectReverse(selectPrev), 
	     QColor(0,0,0));
    painter->end();
    directdraw = false;
  }

  if ( (mode == MODE_SELECT_RESIZE_MULTIPLE) && (mouse1down) ) {
    //double dh, dw;
    DPoint dp1( selectionBox.topLeft() );
    // disallow moves to the left or above selection box
    if ( (e.x < (dp1.x + 2.0) ) || (e.y < (dp1.y + 2.0) ) ) {
      cout << "Warning!" << endl;
      QPoint gin((int)prevpos->x, (int)prevpos->y), gout;
      gout = mapToGlobal(gin);
      cursor().setPos(gout);
      return;
    }
    dx = dp1.distanceTo(&e);
    dy = dp1.distanceTo(prevpos);
    //cout << "resize factor " << dx / dy << endl;
    c->Resize(&dp1, dx / dy);
    selectionBox = c->selectionBox();
    repaint(false);
  }

  if ( (mode == MODE_SELECT_ROTATE_MULTIPLE) && (mouse1down) ) {
    //DPoint dp1( rotateOrigin );
    DPoint *dp1 = new DPoint;
    dp1->x = rotateOrigin.x(); dp1->y = rotateOrigin.y();
    double da = getAngle( rotateOrigin, e1->pos() );
    double db = prevangle - da;
    if (db < -300.0) db += 360.0;
    if (db > 300.0) db -= 360.0;
    db = db * M_PI / 180.0;
    //cout << db << endl;
    prevangle = da;
    c->Rotate(dp1, db);
    selectionBox = c->selectionBox();
    repaint(false);
    delete dp1;
  }

  if ( (mode == MODE_SELECT_MOVE_MULTIPLE) && (mouse1down) ) {
    dx = e.x - prevpos->x; dy = e.y - prevpos->y;
    //cout << "Move by " << dx << "," << dy << endl;
    c->Move(dx, dy);
    selectionBox.moveBy((int)dx, (int)dy);
    repaint(false);
  }

  if ( (mode == MODE_SELECT_MOVE_OBJECT) && (mouse1down) ) {
    dx = e.x - prevpos->x; dy = e.y - prevpos->y;
    //cout << "Move by " << dx << "," << dy << endl;
    c->Move(dx, dy);
    selectionBox.moveBy((int)dx, (int)dy);
    repaint(false);
  }

  if ( (mode == MODE_SELECT_MOVE_POINT) && (mouse1down) ) {
    dx = e.x - prevpos->x; dy = e.y - prevpos->y;
    //cout << "Move by " << dx << "," << dy << endl;
    highlightpoint->x += dx;
    highlightpoint->y += dy;
    repaint(false);
  }
}

// Get angle between -BA> and -BD> (Law of Cosines)
double Render2D::GetAngle(DPoint *a, DPoint *b, DPoint *d) {
  double a1 = b->distanceTo(a);
  double a2 = b->distanceTo(d);
  double a3 = a->distanceTo(d);
  double cos1 = (a2*a2 + a1*a1 - a3*a3) / (2*a1*a2);
  return acos(cos1);
}
