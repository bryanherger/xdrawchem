// bracket.cpp - Bracket's implementation of functions

#include <qobject.h>
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

#include "render2d.h"
#include "drawable.h"
#include "bracket.h"
#include "bondedit.h"
#include "defs.h"

Bracket::Bracket(Render2D *r1, QObject *parent, const char *name)
  : Drawable(parent, name)
{
  r = r1;
  highlighted = false;
  style = BRACKET_SQUARE;
  subtext = "";
  fill = false;
  fillColor = QColor(0,255,255);
}

QString Bracket::ToXML(QString xml_id) {
  QString s, n1;

  // begin bracket
  s.append("<bracket id=\"");
  s.append(xml_id);
  s.append("\">\n");

  // write Start
  s.append("<Start>");
  n1.setNum(start->x);
  s.append(n1);
  s.append(" ");
  n1.setNum(start->y);
  s.append(n1);
  s.append("</Start>\n");

  // write End
  s.append("<End>");
  n1.setNum(end->x);
  s.append(n1);
  s.append(" ");
  n1.setNum(end->y);
  s.append(n1);
  s.append("</End>\n");

  // write Style
  s.append("<style>");
  n1.setNum(style);
  s.append(n1);
  s.append("</style>\n");

  // write subscript text, if present
  if (subtext.length() > 0) {
    s.append("<subscript>");
    s.append(subtext);
    s.append("</subscript>\n");
  }

  // write color
  s.append("<color>");
  n1.setNum(color.red());
  s.append(n1);
  s.append(" ");
  n1.setNum(color.green());
  s.append(n1);
  s.append(" ");
  n1.setNum(color.blue());
  s.append(n1);
  s.append("</color>\n");

  // fill info
  s.append("<fill>");
  if (fill == true)
    s.append("true");
  else
    s.append("false");
  s.append("</fill>");
  s.append("<fillcolor>");
  n1.setNum(fillColor.red());
  s.append(n1);
  s.append(" ");
  n1.setNum(fillColor.green());
  s.append(n1);
  s.append(" ");
  n1.setNum(fillColor.blue());
  s.append(n1);
  s.append("</fillcolor>\n");

  // end bracket
  s.append("</bracket>\n");

  return s;
}

QString Bracket::ToCDXML(QString xml_id) {
  QString s, n1;
  // begin arrow
  s.append("<graphic id=\"");
  s.append(xml_id);
  s.append("\" BoundingBox=\"");
  n1.setNum(end->x);
  s.append(n1);
  s.append(" ");
  n1.setNum(end->y);
  s.append(n1);
  s.append(" ");
  n1.setNum(start->x);
  s.append(n1);
  s.append(" ");
  n1.setNum(start->y);
  s.append(n1);
  s.append("\" ");
  if (style == BRACKET_SQUARE)
    s.append("GraphicType=\"Bracket\" BracketType=\"SquarePair\"");
  if (style == BRACKET_CURVE)
    s.append("GraphicType=\"Bracket\" BracketType=\"RoundPair\"");
  if (style == BRACKET_BRACE)
    s.append("GraphicType=\"Bracket\" BracketType=\"CurlyPair\"");
  s.append("/>\n");

  return s;
}

// set Bracket from XDrawChem-format XML
void Bracket::FromXML(QString xml_tag) {
  int i1, i2;

  i1 = xml_tag.find("<Start>");
  i2 = xml_tag.find("</Start>") + 8;
  SetStartFromXML(xml_tag.mid(i1, i2 - i1));
  i1 = xml_tag.find("<End>");
  i2 = xml_tag.find("</End>") + 6;
  SetEndFromXML(xml_tag.mid(i1, i2 - i1));
  i1 = xml_tag.find("<style>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</style>") + 8;
    style = xml_tag.mid(i1 + 7, 1).toInt();
  }
  i1 = xml_tag.find("<subscript>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</subscript>") + 12;
    subtext = xml_tag.mid(i1, i2 - i1);
    subtext.remove(0,11);
    i2 = subtext.find("</subscript>");
    subtext.remove(i2,999);
  }
  i1 = xml_tag.find("<color>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</color>") + 8;
    SetColorFromXML(xml_tag.mid(i1, i2 - i1));
  }
  i1 = xml_tag.find("<fill>");
  if (i1 >= 0) {
    if ( xml_tag.mid(i1 + 6, 4) == "true" )
      fill = true;
    else
      fill = false;
  }
  i1 = xml_tag.find("<fillcolor>");
  if (i1 >= 0) {
    i2 = xml_tag.find("</fillcolor>") + 12;
    SetFillColorFromXML(xml_tag.mid(i1, i2 - i1));
  }
}

void Bracket::SetFillColorFromXML(QString xml_tag) {
  cout << "SetFColorFromXML:" << xml_tag << endl;
  int i1, i2;
  int d1, d2, d3;

  i1 = xml_tag.find("<fillcolor>");
  i2 = xml_tag.find("</fillcolor>");
  xml_tag.remove(i2, 999);
  xml_tag.remove(i1, 11);

  QTextStream ts(&xml_tag, IO_ReadOnly);

  ts >> d1 >> d2 >> d3;

  fillColor = QColor(d1, d2, d3);
}

void Bracket::Render(void)
{
  QColor c1, ivc;
  if (highlighted)
    c1 = QColor(255,0,0);
  else
    c1 = color;
  // safety check
  if ( (fill == true) && (fillable() == true) ) {
    ivc = fillColor;
    // otherwise, it's invalid
  }
  r->drawBracket( start->toQPoint(), end->toQPoint(), c1, style, ivc );
  int tx = end->toQPoint().x(), ty = end->toQPoint().y();
  tx += 4;
  if (subtext.length() > 0)
    r->drawString(subtext, QPoint(tx,ty), c1, QFont("helvetica", 10));
}

void Bracket::Edit() {
  BondEditDialog be(r, "bracket editor", start, end, TYPE_BRACKET, 0, 0, 0, 
		    style, color);
  if ( !be.exec() ) return;
  cout << "change" << endl;
  style = be.Style();
  color = be.Color();
}

int Bracket::Type(void)
{
  return TYPE_BRACKET;
}

bool Bracket::Find(DPoint *target) {
  if (start == target) return true;
  if (end == target) return true;
  return false;
}

// Do not allow connections to this object.
// Simplest way to do this, I think, is to disallow this function
DPoint * Bracket::FindNearestPoint(DPoint *target, double &dist) {
  dist = 99999.0;
  return 0;
}

Drawable * Bracket::FindNearestObject(DPoint *target, double &dist) {
  DPoint *tl = new DPoint;
  DPoint *tr = new DPoint;
  DPoint *bl = new DPoint;
  DPoint *br = new DPoint;
  double tl_x, tl_y, br_x, br_y, swp, dist1, dist2;
  tl_x = start->x; tl_y = start->y;
  br_x = end->x; br_y = end->y;
  if (tl_x < br_x) { swp = tl_x; tl_x = br_x; br_x = swp; }
  if (tl_y < br_y) { swp = tl_y; tl_y = br_y; br_y = swp; }
  tl->x = tl_x; tl->y = tl_y;
  bl->x = tl_x; bl->y = br_y;
  tr->x = br_x; tr->y = tl_y;
  br->x = br_x; br->y = br_y;
  dist1 = DistanceToLine(tl, bl, target);
  dist2 = DistanceToLine(tr, br, target);
  if (dist1 < dist2)
    dist = dist1;
  else
    dist = dist2;
  delete tl, tr, bl, br;
  return this;
}

void Bracket::setPoints(DPoint *s, DPoint *e) {
  start = s;
  end = e;
}

bool Bracket::WithinRect(QRect n, bool shiftdown) {
  if ( DPointInRect(start, n) && DPointInRect(end, n) )
    highlighted = true;
  else
    highlighted = false;
  return highlighted;
}

QRect Bracket::BoundingBox() {
  if (highlighted == false)
    return QRect( QPoint(999,999), QPoint(0,0) ); 
  int top, bottom, left, right, swp;
  top = (int)start->y;
  left = (int)start->x;
  bottom = (int)end->y;
  right = (int)end->x;
  if (bottom < top) { swp = top; top = bottom; bottom = swp; }
  if (right < left) { swp = left; left = right; right = swp; }
  return QRect( QPoint(left,top), QPoint(right,bottom) );
}

Drawable * Bracket::DeepCopy() {
  Bracket *np = new Bracket(r);
  DPoint *s1 = new DPoint(start);
  DPoint *e1 = new DPoint(end);
  np->setPoints(s1, e1);
  np->SetColor(color);
  np->SetStyle(style);
  np->setText( getText() );
  if (highlighted) np->Highlight(true);
  return np;
}
