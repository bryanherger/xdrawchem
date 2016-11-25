// text.h -- subclass of Drawable for text/labels

#ifndef TEXT_H
#define TEXT_H

#include <qobject.h>
#include <qrect.h>
#include <qfont.h>
#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

#define TEXT_AUTO 0
#define TEXT_LALIGN 1
#define TEXT_CALIGN 2
#define TEXT_RALIGN 3

class Molecule;

class Text : public Drawable {
  Q_OBJECT
    public:
  Text(Render2D *, QObject *parent = 0, const char *name = 0);
  void Render();  // draw this object
  int Type();  // return type of object
  bool Find(DPoint *); // does this Text contain this DPoint?
  DPoint * FindNearestPoint(DPoint *, double &);
  Drawable * FindNearestObject(DPoint *, double &);
  Drawable * DeepCopy();
  void setPoint(DPoint *);
  int Justify() { return justify; }
  void setJustify(int a) { justify = a; }
  bool WithinBounds(DPoint *);
  bool WithinRect(QRect, bool);
  void InsertCharacter(QKeyEvent *);
  void InsertString(QString);
  void DeleteKeyPressed();
  QPoint GetTopLeftPoint(void);  // get real top left point, based on justify
  QPoint NearestCenter(QPoint, int, int &);
  void setText(QString nt1, QString nt2) { 
    if (start != 0) {
      start->element = nt1; // element info (plain text)
      start->elementmask = nt2;  // actual display text in underlying point!
      displayText = nt2;
    }
  }
  void setText(QString nt) {
    setText(nt, nt);
  }
  QString getText() { 
    if (start != 0)
      return start->element; 
    else
      return "";
  } // not just Text()!
  QString getRichText() {
    if (start != 0)
      return displayText; 
    else
      return "";
  }
  void setTextMask(QString nm) { 
    // this function is (probably) obsolete.  Use setText() instead.
    //if (start != 0) start->elementmask = nm; 
  }
  QString getTextMask() {
    if (start != 0)
      return start->elementmask; 
    else
      return "";
  }
  void setFont(QFont f) { 
    font = f; 
    bfont = f;
    bfont.setWeight(QFont::Bold);
    ifont = f;
    ifont.setItalic(true);
    ufont = f;
    ufont.setUnderline(true);
  }
  QFont subscriptFont(QFont);
  void setMolecule(Molecule *m1) { molecule = m1; }
  Molecule *getMolecule() { return molecule; }
  void setDataType(int d1) { DataType = d1; }
  int getDataType() { return DataType; }
  QFont getFont() { return font; }
  void MoveCursor(DPoint *);
  void Select(DPoint *, DPoint *);
  QRect BoundingBox();
  void DoSuperscript();
  void DoSubscript();
  void DoBold();
  void DoItalic();
  void DoUnderline();
  void DeselectAllText() { selectMin = -1; selectMax = -1; }
  // set text parsed from XML file
  void SetTextFromXML(QString) { }
  QString ToXML(QString);
  QString ToCDXML(QString);
  void FromXML(QString);
  void SetFontFromXML(QString);
  void SetTextstringFromXML(QString);
  void SetRichstringFromXML(QString);
  void SetTextmaskFromXML(QString);
  void SetBorderColorFromXML(QString);
  void SetFillColorFromXML(QString);
  void SetShapeGeometryFromXML(QString);
  void CheckAlignment(int);
  void ForceAlignment(int);
  void UpdateDisplayText();
  // set shape/border/fill
  void setShape(int s1) {
    shape = s1;
  }
  int getShape() { return shape; }
  void setBorder(bool b1) { drawBorder = b1; }
  bool getBorder() { return drawBorder; }
  void setBorderColor(QColor c1) { borderColor = c1; }
  QColor getBorderColor() { return borderColor; }
  void setFill(bool b1) { drawFill = b1; }
  bool getFill() { return drawFill; }
  void setFillColor(QColor c1) { fillColor = c1; }
  QColor getFillColor() { return fillColor; }
  void setShapeWidth(int i1) {
    shapewidth = i1;
  }
  void setShapeHeight(int i1) {
    shapeheight = i1;
  }
  int getShapeWidth() {
    if (shapewidth < 0) return oshapewidth;
    return shapewidth;
  }
  int getShapeHeight() {
    if (shapeheight < 0) return oshapeheight;
    return shapeheight;
  }

 private:
  // Renderer
  Render2D *r;
  // Text this object holds
  //QString text;  *** NOW DEFINED IN dpoint.h
  // Text modifier -- super/subscript flags
  //QString textmask;  *** NOW DEFINED IN dpoint.h
  // Text and flags which are actually displayed
  QString displayText, displayTextMask;
  // for text which labels a point -- which side is less hindered?
  int whichside;  // 1 = left, 2 = right
  // Text font(s) - regular, bold, italic, underline
  QFont font, bfont, ifont, ufont;
  // justify -- how/where to draw this text
  int justify, tjustify;
  // dimensions of text box
  QRect BBox;
  // cursor position (character # within text)
  int cursor;
  // selection range
  int selectMin, selectMax;
  // is the Shift key down?
  bool shiftdown;
  // Molecule this text gets data from
  Molecule *molecule;
  int DataType;
  // shape/border/fill variables
  int shape;  // 0 = no shape
  bool drawBorder, drawFill;
  QColor borderColor, fillColor;
  int shapewidth, shapeheight;
  int oshapewidth, oshapeheight;
};

#endif
