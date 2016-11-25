// drawable.h -- the class that is the parent of all drawable objects

#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <qobject.h>
#include <qrect.h>
#include <qcolor.h>
#include <qptrlist.h>
#include "dpoint.h"
class Bond;
class Text;

class Drawable : public QObject {
  Q_OBJECT
    public:
  Drawable(QObject *parent = 0, const char *name = 0);
  virtual void Render();  // draw this object
  virtual void Edit();  // edit this object
  virtual int Type();  // return type of object
  virtual DPoint * FindNearestPoint(DPoint *, double &);
  virtual Drawable * FindNearestObject(DPoint *, double &);
  virtual Drawable * DeepCopy();
  virtual bool Find(DPoint *);
  virtual void addBond(DPoint *, DPoint *, int, int, QColor, bool hl = false);
  virtual void addMolecule(Drawable *);
  virtual void Highlight();
  virtual void Highlight(bool);
  virtual bool Highlighted();
  virtual bool Erase(Drawable *);
  virtual bool WithinRect(QRect, bool);
  virtual void SelectAll();
  virtual void DeselectAll();
  virtual void Move(double, double);
  virtual void ForceMove(double, double);
  virtual void Rotate(DPoint *, double);
  virtual void Flip(DPoint *, int);
  virtual void Resize(DPoint *, double);
  virtual QRect BoundingBox();
  virtual QPtrList<DPoint> AllPoints();
  virtual QPtrList<Drawable> AllObjects();
  virtual QString ToXML(QString);
  virtual QString ToCDXML(QString);
  virtual void FromXML(QString);
  virtual int Members();
  // needed only to merge Molecules; see molecule.cpp, addMolecule()
  virtual Bond *bondsFirst() { return 0; }
  virtual Bond *bondsNext() { return 0; }
  virtual Text *labelsFirst() { return 0; }
  virtual Text *labelsNext() { return 0; }
  // stuff that all Drawables should know about
  static double getAngle(DPoint *, DPoint *);
  double DistanceToLine(DPoint *, DPoint *, DPoint *);
  double DistanceBetween(QPoint, QPoint);
  bool DPointInRect(DPoint *, QRect);
  DPoint *Start() { return start; }
  DPoint *End() { return end; }
  void setStart(DPoint *ns) { start = ns; }
  void setEnd(DPoint *ne) { end = ne; }
  virtual void SetColorIfHighlighted(QColor);
  void SetColor(QColor c) { color = c; }
  QColor GetColor() { return color; }
  QString getID() { return id; }
  void setID(QString x) { id = x; }
  QColor GetColorFromXML(QString);
  void SetColorFromXML(QString);
  void SetStartFromXML(QString);
  void SetEndFromXML(QString);
  bool isInGroup() { return ingroup; }
  void setInGroup(bool x) { ingroup = x; }
  int Thick() { return thick; }
  void setThick(int t) { thick = t; }
  // more or less everything must be protected so derived classes can use them
    protected:
  // highlighted?
  bool highlighted;
  // member of a group?
  bool ingroup;
  // points which define this Drawable (only start needed for TEXT)
  DPoint *start, *end;
  // color
  QColor color;
  // XML ID
  QString id;
  // temporary DPoint
  DPoint *tmp_pt;
  // arrow or bracket style
  int style;
  // which symbol or curvearrow to draw
  QString which;
  // thickness of bond/bracket/line
  int thick;
};

#endif
