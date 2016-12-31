// gobject.h -- subclass of Drawable for graphic objects

#ifndef GOBJECT_H
#define GOBJECT_H

#include <qobject.h>
#include <qpointarray.h>
#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class GraphicObject : public Drawable {
  Q_OBJECT
    public:
  GraphicObject(Render2D *, QObject *parent = 0, const char *name = 0);
  void Render();  // draw this object
  void Edit();
  int Type();  // return type of object
  bool Find(DPoint *); // does this GraphicObject contain this DPoint?
  DPoint * FindNearestPoint(DPoint *, double &);
  Drawable * FindNearestObject(DPoint *, double &);
  Drawable * DeepCopy();
  void setPoints(DPoint *, DPoint *);
  void setPointArray(QPointArray);
  bool WithinRect(QRect, bool);
  QRect BoundingBox();
  QString ToXML(QString);
  QString ToCDXML(QString);
  void FromXML(QString);
  int Style() { return style; }
  void SetStyle(int s1) { style = s1; }
  int objectType() { return ot; }
  void setObjectType(int t1) { ot = t1; }
 private:
  // Renderer
  Render2D *r;
  // Point array (for cubic bezier and possibly others)
  QPointArray pa1;
  // graphic object type
  int ot;
};

#endif
