// arrow.h -- subclass of Drawable for arrows

#ifndef ARROW_H
#define ARROW_H

#include <qobject.h>
#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class Arrow : public Drawable {
  Q_OBJECT
    public:
  Arrow(Render2D *, QObject *parent = 0, const char *name = 0);
  void Render();  // draw this object
  void Edit();  // edit this object
  int Type();  // return type of object
  bool Find(DPoint *); // does this Arrow contain this DPoint?
  DPoint * FindNearestPoint(DPoint *, double &);
  Drawable * FindNearestObject(DPoint *, double &);
  Drawable * DeepCopy();
  void setPoints(DPoint *, DPoint *);
  QRect BoundingBox();
  bool WithinRect(QRect, bool);
  QString ToXML(QString);
  QString ToCDXML(QString);
  void FromXML(QString);
  int Orientation();
  QPoint Midpoint();
  int Style() { return style; }
  void SetStyle(int s1) { style = s1; }
 private:
  // Renderer
  Render2D *r;
};

#endif
