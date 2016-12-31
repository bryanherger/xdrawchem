// XDrawChem Event structure.

#ifndef XDC_EVENT_H
#define XDC_EVENT_H

#include <qcolor.h>
#include <qstring.h>
#include <qpointarray.h>

#include "dpoint.h"

#define EVT_ADD_BOND 1
#define EVT_ADD_BOND_UP 2
#define EVT_ADD_BOND_DOWN 3
#define EVT_ADD_BOND_DASH 4
#define EVT_ADD_BOND_WAVY 5
#define EVT_ADD_BRACKET 10
#define EVT_ADD_ARROW 20
#define EVT_ADD_CURVEARROW 30
#define EVT_ADD_GRAPHIC 40
#define EVT_NONE 999 // send this to trigger undo.

class XDC_Event {
 public:
  XDC_Event() { undo_able = true; tf1 = false; tf2 = false; }
  XDC_Event(int t1) { 
    evt_type = t1; 
    undo_able = true; 
    tf1 = false; 
    tf2 = false;
  }
  int type() { return evt_type; }
  void setType(int p1) { evt_type = p1; }
  DPoint *Start() { return start; }
  void setStart(DPoint *p1) { start = p1; }
  DPoint *End() { return end; }
  void setEnd(DPoint *p1) { end = p1; }
  void setDeltaXY(double nx, double ny) { dx = nx; dy = ny; }
  double deltaX() { return dx; }
  double deltaY() { return dy; }
  int Param1() { return param1; }
  void setParam1(int p1) { param1 = p1; }
  int Param2() { return param2; }
  void setParam2(int p1) { param2 = p1; }
  int Param3() { return param3; }
  void setParam3(int p1) { param3 = p1; }
  int Param4() { return param4; }
  void setParam4(int p1) { param4 = p1; }
  QString text() { return evt_text; }
  void setText(QString p1) { evt_text = p1; }
  QColor color() { return evt_color; }
  void setColor(QColor p1) { evt_color = p1; }
  void setBool1(bool p1) { tf1 = p1; }
  bool bool1() { return tf1; }
  void setBool2(bool p1) { tf2 = p1; }
  bool bool2() { return tf2; }
  void setUndoable(bool p1) { undo_able = p1; }
  bool undoable() { return undo_able; }
  void setPointArray(QPointArray inp1) { pa1 = inp1; }
  QPointArray pointArray() { return pa1; }
 private:
  int evt_type;
  DPoint *start, *end;
  double dx, dy;
  int param1, param2, param3, param4;
  bool tf1, tf2, undo_able;
  QString evt_text;
  QColor evt_color;
  QPointArray pa1;
};

#endif
