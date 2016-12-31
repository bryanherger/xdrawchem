// curvearrow.h -- subclass of Drawable for curvearrows

#ifndef CURVEARROW_H
#define CURVEARROW_H

#include <qobject.h>
#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class CurveArrow : public Drawable {
  Q_OBJECT
    public:
  CurveArrow(Render2D *, QObject *parent = 0, const char *name = 0);
  void Render();  // draw this object
  void Edit();
  int Type();  // return type of object
  bool Find(DPoint *); // does this CurveArrow contain this DPoint?
  DPoint * FindNearestPoint(DPoint *, double &);
  Drawable * FindNearestObject(DPoint *, double &);
  Drawable * DeepCopy();
  void setPoints(DPoint *, DPoint *);
  QRect BoundingBox();
  bool WithinRect(QRect, bool);
  QString ToXML(QString);
  void FromXML(QString);
  void SetCurve(QString t) { which = t; }
  QString GetCurve() { return which; }
 private:
  // Renderer
  Render2D *r;
};

#endif
