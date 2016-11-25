// bracket.h -- subclass of Drawable for brackets

#ifndef BRACKET_H
#define BRACKET_H

#include <qobject.h>
#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class Bracket : public Drawable {
  Q_OBJECT
    public:
  Bracket(Render2D *, QObject *parent = 0, const char *name = 0);
  void Render();  // draw this object
  void Edit();
  int Type();  // return type of object
  bool Find(DPoint *); // does this Bracket contain this DPoint?
  DPoint * FindNearestPoint(DPoint *, double &);
  Drawable * FindNearestObject(DPoint *, double &);
  Drawable * DeepCopy();
  void setPoints(DPoint *, DPoint *);
  bool WithinRect(QRect, bool);
  QRect BoundingBox();
  QString ToXML(QString);
  QString ToCDXML(QString);
  void FromXML(QString);
  int Style() { return style; }
  void SetStyle(int s1) { style = s1; }
  void setText(QString s1) { subtext = s1; }
  QString getText() { return subtext; }
  void setFill(bool b1) { fill = b1; }
  bool getFill() { return fill; }
  void setFillColor(QColor fc1) { fillColor = fc1; }
  QColor getFillColor() { return fillColor; }
  void SetFillColorFromXML(QString);
  bool fillable() {
    if (style > 3) return true;  // see defs.h for BRACKET_<types>
    return false;
  }
 private:
  // Renderer
  Render2D *r;
  // subscript text, if applicable
  QString subtext;
  // fill options, where applicable
  bool fill;
  QColor fillColor;
};

#endif
