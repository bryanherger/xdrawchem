// molecule.cpp - Molecule's implementation of functions

#include <qobject.h>
#include <qptrlist.h>
#include <qfont.h>
#include <qpoint.h>
#include <qtextstream.h>
#include <iostream>

using std::cout;
using std::endl;

#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "text.h"
#include "dpoint.h"
#include "defs.h"

Molecule::Molecule(Render2D *r1, QObject *parent, const char *name)
  : Drawable(parent, name)
{
  r = r1;
  text_mw = 0;
  text_formula = 0;
  group_type = GROUP_NONE;
  showPC = false;
  peaklist.setAutoDelete(true);
  //bonds.setAutoDelete(true);
  //labels.setAutoDelete(true);
  //symbols.setAutoDelete(true);
}

// implement more cleanup here
Molecule::~Molecule() {
  peaklist.clear();
}

void Molecule::Render(void)
{
  // collect all points
  AllPoints();
  // update double bond states
  Bond *tb2;
  int nleft = 0, nright = 0;
  double ang;
  QPtrListIterator<Bond> it(bonds);
  for ( ; it.current(); ++it ) {
    tb2 = it.current();
    if (tb2->Order() == 2) {
      nleft = 0; nright = 0;
      for (tmp_bond = bonds.first(); tmp_bond != NULL; 
	   tmp_bond = bonds.next()) {
	if (tmp_bond == tb2) continue;
	if (tmp_bond->Find(tb2->Start())) {
	  ang = Angle(tb2, tmp_bond);
	  if (ang < 180.0) 
	    nleft++;
	  else
	    nright++;
	}
	if (tmp_bond->Find(tb2->End())) {
	  ang = Angle(tb2, tmp_bond);
	  if (ang < 180.0) 
	    nleft++;
	  else
	    nright++;
	}
      }
      if (nleft > nright)
	tb2->RenderSide(0);
      else
	tb2->RenderSide(1);
    }
  }
  // draw group box if needed
  if (group_type != GROUP_NONE) {
    QRect gb1 = BoundingBoxAll();
    gb1.setLeft(gb1.left() - 6);
    gb1.setRight(gb1.right() + 6);
    gb1.setBottom(gb1.bottom() + 6);
    r->drawLine(gb1.topLeft(), gb1.bottomLeft(), 1, QColor(0,0,0), 0);
    r->drawLine(gb1.bottomLeft(), gb1.bottomRight(), 1, QColor(0,0,0), 0);
    r->drawLine(gb1.bottomRight(), gb1.topRight(), 1, QColor(0,0,0), 0);
    QPoint tl1(gb1.topLeft().x() + 2, gb1.topLeft().y() - 4);
    QString label1;
    if (group_type == GROUP_REACTANT)
      label1 = tr("Reactant");
    if (group_type == GROUP_PRODUCT)
      label1 = tr("Product");
    r->drawString(label1, tl1, QColor(0,0,0), QFont("times", 8));
  }

  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next())
    tmp_bond->Render();
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    // ignore if just "C"
    //if (tmp_text->getText() == "C") continue;
    tmp_text->Render();
  }
  CalcOffsets();
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next())
    tmp_sym->Render();
}

// Calculate offsets, i.e., where to put a Symbol.
void Molecule::CalcOffsets() {
  bool top = true, bottom = true, left = true, right = true;
  DPoint *op;
  double ang;

  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next()) {
    top = true; bottom = true; left = true; right = true;
    tmp_pt = tmp_sym->Start();
    for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
      if (tmp_bond->Find(tmp_pt)) {
	op = tmp_bond->otherPoint(tmp_pt);
	ang = tmp_bond->getAngle(tmp_pt, op);
	if ( (ang > 300.0) || (ang < 60.0) )
	  right = false;
	if ( (ang > 30.0) && (ang < 150.0) )
	  bottom = false;
	if ( (ang > 120.0) && (ang < 240.0) )
	  left = false;
	if ( (ang > 210.0) && (ang < 330.0) )
	  top = false;
      }
    }
    if (bottom == true) {
      tmp_sym->SetOffset(QPoint(0,12));
      tmp_sym->SetRotate(0.0);
      continue;
    }
    if (top == true) {
      tmp_sym->SetOffset(QPoint(0,-12));
      tmp_sym->SetRotate(180.0);
      continue;
    }
    if (right == true) {
      tmp_sym->SetOffset(QPoint(12,0));
      tmp_sym->SetRotate(-90.0);
      continue;
    }
    if (left == true) {
      tmp_sym->SetOffset(QPoint(-12,0));
      tmp_sym->SetRotate(90.0);
      continue;
    }
    tmp_sym->SetOffset(QPoint(0,12));
  }
}

int Molecule::Type(void)
{
  return TYPE_MOLECULE;
}

bool Molecule::Find(DPoint *target) {
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    if (tmp_bond->Find(target) == true) return true;
  }
  return false;
}

bool Molecule::WithinRect(QRect qr, bool shiftdown) {
  if (shiftdown) cout << "shiftdown" << endl;
  bool ret = false;
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    if (tmp_bond->WithinRect(qr, false)) ret = true;
  }
  if ( (ret == true) && (shiftdown == true) ) {
    cout << "selectall" << endl;
    SelectAll();
    return false;
  }
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next())
    tmp_text->WithinRect(qr, false);
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next())
    tmp_sym->WithinRect(qr, false);
  return false;
}

void Molecule::SelectAll() {
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next())
    tmp_bond->SelectAll();
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next())
    tmp_text->SelectAll();
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next())
    tmp_sym->SelectAll();
}

void Molecule::DeselectAll() {
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next())
    tmp_bond->DeselectAll();
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next())
    tmp_text->DeselectAll();
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next())
    tmp_sym->DeselectAll();
}

void Molecule::SetColorIfHighlighted(QColor c) {
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next())
    tmp_bond->SetColorIfHighlighted(c);
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next())
    tmp_text->SetColorIfHighlighted(c);
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next())
    tmp_sym->SetColorIfHighlighted(c);
}

// Copy Text labels into DPoint::element (needed esp. by save, CalcMW,
// NMR prediction...)
void Molecule::CopyTextToDPoint() {
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    tmp_pt = tmp_text->Start();
    tmp_pt->element = tmp_text->getText();
    // clear aromaticity here
    tmp_pt->aromatic = false;
  }
}

QRect Molecule::BoundingBoxAll() {
  QRect fr;
  SelectAll();
  fr = BoundingBox();
  DeselectAll();
  return fr;
}

bool Molecule::WithinBounds(DPoint *target) {
  QRect fr = BoundingBoxAll();
  return fr.contains(target->toQPoint());
}

// Determine if split is necessary (e.g., if this structure contains two
// or more fragments).  Return empty list if not, return new Molecule's 
// if needed
QPtrList<Molecule> Molecule::MakeSplit() {
  QPtrList<DPoint> up;
  QPtrList<DPoint> pointqueue;
  QPtrList<Bond> uo;
  QPtrList<Bond> removequeue;
  QPtrList<Molecule> molecules;
  Molecule *nm;
  DPoint *current;

  uo = bonds;
  // Start with first Bond...
  nm = new Molecule(r);
  nm->SetChemdata(cd);
  tmp_bond = uo.first();
  nm->addBond(tmp_bond);
  uo.removeFirst();
  pointqueue.append(tmp_bond->Start());
  pointqueue.append(tmp_bond->End());
  do { 
    do {
      current = pointqueue.first();
      pointqueue.removeFirst();
      for (tmp_bond = uo.first(); tmp_bond != NULL; 
	   tmp_bond = uo.next()) {
	if (tmp_bond->Find(current)) {
	  pointqueue.append(tmp_bond->otherPoint(current));
	  nm->addBond(tmp_bond);
	  removequeue.append(tmp_bond);
	}
      }
      for (tmp_bond = removequeue.first(); tmp_bond != NULL; 
	   tmp_bond = removequeue.next()) {
	uo.remove(tmp_bond);
      }
      removequeue.clear();
    } while (pointqueue.count() > 0);
    if (uo.count() > 0) {
      // Still bonds left, make new Molecule
      molecules.append(nm);
      nm = new Molecule(r);
      nm->SetChemdata(cd);
      tmp_bond = uo.first();
      nm->addBond(tmp_bond);
      uo.removeFirst();
      pointqueue.append(tmp_bond->Start());
      pointqueue.append(tmp_bond->End());
    }
  } while (uo.count() > 0);
  molecules.append(nm);
  cout << "Found " << molecules.count() << " distinct structures." << endl;
  cout << nm->Members() << endl;
  // Now split Text and Symbol lists
  if (molecules.count() > 1) {
    for (tmp_text = labels.first(); tmp_text != 0; tmp_text = labels.next()) {
      for (nm = molecules.first(); nm != 0; nm = molecules.next()) {
	if (nm->Find(tmp_text->Start())) nm->addText(tmp_text);
      }
    }
    for (tmp_sym = symbols.first(); tmp_sym != 0; tmp_sym = symbols.next()) {
      for (nm = molecules.first(); nm != 0; nm = molecules.next()) {
	if (nm->Find(tmp_sym->Start())) nm->addSymbol(tmp_sym);
      }
    }
  }
  return molecules;
}

// Create and return a list of all unique DPoint's in this Molecule
QPtrList<DPoint> Molecule::AllPoints() {
  QPtrList<DPoint> pl;

  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    tmp_pt = tmp_bond->Start();
    if (pl.contains(tmp_pt) == 0) pl.append(tmp_pt);
    tmp_pt = tmp_bond->End();
    if (pl.contains(tmp_pt) == 0) pl.append(tmp_pt);
  }
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    tmp_pt = tmp_text->Start();
    if (pl.contains(tmp_pt) == 0) pl.append(tmp_pt);
  }
  // Copy Text labels to DPoint's as needed
  CopyTextToDPoint();
  // Determine number of substituents at each point
  int c1 = 0;
  for (tmp_pt = pl.first(); tmp_pt != NULL; tmp_pt = pl.next() ) {
    tmp_pt->serial = c1;  // serialize atoms
    c1++;
    tmp_pt->hit = false;  // also reset "hit"
    tmp_pt->substituents = 0;
    for (tmp_bond = bonds.first(); tmp_bond != 0; tmp_bond = bonds.next()) {
      if (tmp_bond->Find(tmp_pt))
	tmp_pt->substituents = tmp_pt->substituents + tmp_bond->Order();
    }
  }
  // serialize atoms -- copy list place to serial field of DPoint
  //for (int c1 = 0; c1 < pl.count(); c1++) {
  //  tmp_pt = pl.at(c1);
  //  tmp_pt->serial = c1;
  //  tmp_pt->hit = false;  // also reset "hit"
  //}
  //cout << "AllPoints():" << pl.count() << endl;
  return pl;
}

// Create and return a list of all unique Drawable's in this Molecule
QPtrList<Drawable> Molecule::AllObjects() {
  QPtrList<Drawable> dl;

  up = AllPoints();

  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    dl.append((Drawable *)tmp_bond);
  }
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    dl.append((Drawable *)tmp_text);
  }
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next()) {
    dl.append((Drawable *)tmp_sym);
  }
  return dl;
}

QPtrList<Bond> Molecule::AllBonds() {
  // this should be safe, right?  ioiface.cpp should use this read-only...  :)
  return bonds;
}

// Create a list of unique DPoints to move
void Molecule::MakeTomoveList() {
  tomove.clear();

  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    if (tmp_bond->Highlighted()) {
      tmp_pt = tmp_bond->Start();
      if (tomove.contains(tmp_pt) == 0) tomove.append(tmp_pt);
      tmp_pt = tmp_bond->End();
      if (tomove.contains(tmp_pt) == 0) tomove.append(tmp_pt);
    }
  }
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    if (tmp_text->Highlighted()) {
      tmp_pt = tmp_text->Start();
      if (tomove.contains(tmp_pt) == 0) tomove.append(tmp_pt);
    }
  }
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next()) {
    if (tmp_sym->Highlighted()) {
      tmp_pt = tmp_sym->Start();
      if (tomove.contains(tmp_pt) == 0) tomove.append(tmp_pt);
    }
  }
}

void Molecule::Move(double dx, double dy) {
  MakeTomoveList();

  for (tmp_pt = tomove.first(); tmp_pt != NULL; tmp_pt = tomove.next() ) {
    tmp_pt->x += dx;
    tmp_pt->y += dy;
  }
}

void Molecule::Rotate(DPoint *origin, double angle) {
  MakeTomoveList();

  for (tmp_pt = tomove.first(); tmp_pt != NULL; tmp_pt = tomove.next() ) {
    double thisx = tmp_pt->x - origin->x;
    double thisy = tmp_pt->y - origin->y;
    double newx = thisx * cos(angle) + thisy * sin(angle);
    double newy = -thisx * sin(angle) + thisy * cos(angle);
    tmp_pt->x = newx + origin->x;
    tmp_pt->y = newy + origin->y;  
  }
  Changed();
}

// rotate about center of molecule
void Molecule::Rotate(double angle) {
  MakeTomoveList();

  double centerx = 0.0, centery = 0.0;
  int n = 0;

  for (tmp_pt = tomove.first(); tmp_pt != NULL; tmp_pt = tomove.next() ) {
    centerx += tmp_pt->x;
    centery += tmp_pt->y;
    n++;
  }
  centerx /= (double)n;
  centery /= (double)n;

  for (tmp_pt = tomove.first(); tmp_pt != NULL; tmp_pt = tomove.next() ) {
    double thisx = tmp_pt->x - centerx;
    double thisy = tmp_pt->y - centery;
    double newx = thisx * cos(angle) + thisy * sin(angle);
    double newy = -thisx * sin(angle) + thisy * cos(angle);
    tmp_pt->x = newx + centerx;
    tmp_pt->y = newy + centery;  
  }
  Changed();
}

void Molecule::Flip(DPoint *origin, int direction) {
  MakeTomoveList();
  double delta;

  for (tmp_pt = tomove.first(); tmp_pt != NULL; tmp_pt = tomove.next() ) {
    if (direction == FLIP_H) {
      delta = tmp_pt->x - origin->x;
      tmp_pt->x = tmp_pt->x - 2*delta;
    } else { // direction == FLIP_V
      delta = tmp_pt->y - origin->y;
      tmp_pt->y = tmp_pt->y - 2*delta;
    }
  }
}

void Molecule::Resize(DPoint *origin, double scale) {
  double dx, dy;

  MakeTomoveList();

  for (tmp_pt = tomove.first(); tmp_pt != NULL; tmp_pt = tomove.next() ) {
    dx = tmp_pt->x - origin->x;
    dy = tmp_pt->y - origin->y;
    dx *= scale;
    dy *= scale;
    tmp_pt->x = origin->x + dx;
    tmp_pt->y = origin->y + dy;
  }
}

QRect Molecule::BoundingBox() {
  int top = 99999, bottom = 0, left = 99999, right = 0;
  QRect tmprect;

  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    tmprect = tmp_bond->BoundingBox();
    if (tmprect.isValid()) {
      if (tmprect.left() < left) left = tmprect.left();
      if (tmprect.right() > right) right = tmprect.right();
      if (tmprect.top() < top) top = tmprect.top();
      if (tmprect.bottom() > bottom) bottom = tmprect.bottom();
    }
  }
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    tmprect = tmp_text->BoundingBox();
    if (tmprect.isValid()) {
      if (tmprect.left() < left) left = tmprect.left();
      if (tmprect.right() > right) right = tmprect.right();
      if (tmprect.top() < top) top = tmprect.top();
      if (tmprect.bottom() > bottom) bottom = tmprect.bottom();
    }
   }

  return QRect( QPoint(left,top), QPoint(right,bottom) );
}

DPoint * Molecule::FindNearestPoint(DPoint *target, double &dist) {
  DPoint *nearest = 0, *d1;
  double mindist = 99999.0, d1dist = 999999.0;
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    d1 = tmp_bond->FindNearestPoint(target, d1dist);
    if (d1dist < mindist) { mindist = d1dist; nearest = d1; }
  }
  dist = mindist;
  if (!nearest) return 0;
  nearest->other = 0;
  bool already_found = false;
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    if (tmp_bond->Find(nearest) == true) {
      if (already_found == true) {
	nearest->other = 0;
	return nearest;
      } else {
	already_found = true;
	nearest->other = tmp_bond->otherPoint(nearest);
	nearest->otherBond = tmp_bond;
      }
    }
  }
  // check text objects to see if nearest point has a label
  // if it does, then don't select this point.
  //for (tmp_text=labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    //if (tmp_text->Start() == nearest) dist = 9999.0;
  //}
  return nearest;
}

Drawable * Molecule::FindNearestObject(DPoint *target, double &dist) {
  Drawable *nearest = 0, *d1;
  double mindist = 999999.0, d1dist = 999999.0;
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    d1 = tmp_bond->FindNearestObject(target, d1dist);
    if (d1dist < mindist) { mindist = d1dist; nearest = d1; }
  }
  // if on Text label, highlight it...
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    if (tmp_text->WithinBounds(target)) {
      nearest = tmp_text;
      dist = 0.01;
      return nearest;
    }
  }
  // if on Symbol, highlight it...
  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next()) {
    if (tmp_sym->WithinBounds(target)) {
      nearest = tmp_sym;
      dist = 0.01;
      return nearest;
    }
  }
  dist = mindist;
  return nearest;
}

void Molecule::addBond(DPoint *s, DPoint *e, int thick, int order, 
		       QColor c, bool hl) {
  int o, p;
  Bond *b = new Bond(r);
  b->setPoints(s, e);
  b->setThick(thick);
  if (preferences.getBondCenter() == true) {
    b->setDoubleBondAuto(0);
    b->setDoubleBondAlign(2);
  }
  if (order != 99) {
    b->setOrder(order);
  } else {
    b->setDash(1);
    b->setOrder(1);
  }
  if (order == 4) {  // MDL molfiles store aromaticity this way
    b->setDash(1);
    b->setOrder(2);
  }
  b->SetColor(c);
  if (hl) b->Highlight(true);
  // check if bond exists before adding
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    if (tmp_bond->Equals(b)) {
      o = tmp_bond->Order();
      p = tmp_bond->Dash();
      if (o < 3) tmp_bond->setOrder(o + 1);
      if (order == 99) tmp_bond->setDash(p + 1);
      Changed();
      return;
    }
  }  
  bonds.append(b);
  Changed();
}

// shortcut addBond
void Molecule::addBond(Bond *b) {
  // but only add if it's not already there.
  if (bonds.containsRef(b) == 0) bonds.append(b);
  Changed();
}

// add a text label
void Molecule::addText(Text *t) {
  // remove Text associated with this point, if any
  DPoint *findpt = t->Start();
  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    if (tmp_text->Start() == findpt) {
      labels.remove(tmp_text);
      break;
    }
  }

  t->setMolecule(this);
  labels.append(t);
  Changed();
}

// add a Symbol
void Molecule::addSymbol(Symbol *s) {
  symbols.append(s);
  Changed();
}

// add a molecule to this one.  The Drawable passed had better be a Molecule!
void Molecule::addMolecule(Drawable *m1) {
  cout << "Uh-oh!  Need to merge" << endl;
  for (tmp_bond = m1->bondsFirst(); tmp_bond != NULL; 
       tmp_bond = m1->bondsNext()) {
    bonds.append(tmp_bond);
  }
  for (tmp_text = m1->labelsFirst(); tmp_text != NULL; 
       tmp_text = m1->labelsNext()) {
    tmp_text->setMolecule(this);
    labels.append(tmp_text);
  }
  Changed();
}

// Erase
bool Molecule::Erase(Drawable *d) {
  bool removed;
  bool retval = false;

  if (d->Type() == TYPE_BOND) {
    tmp_bond = (Bond *)d;
    if (bonds.containsRef(tmp_bond) > 0) {
      if ( (tmp_bond->Order() == 2) || (tmp_bond->Order() == 3) ) {
	tmp_bond->setOrder(tmp_bond->Order() - 1);
	if (tmp_bond->Dash() > 0)
	  tmp_bond->setDash(tmp_bond->Dash() - 1);
	Changed();
	return true;
      } else {
	removed = bonds.remove(tmp_bond);
	if (removed == true) delete d;
	retval = removed;
      }
    }
  } else { // deleting TEXT or SYMBOL
    if (d->Type() == TYPE_TEXT) {
      removed = labels.remove((Text *)d);
      if (removed == true) {
	d->Start()->element = "C";
	delete d;
      }
      return removed;
    }
    if (d->Type() == TYPE_SYMBOL) {
      removed = symbols.remove((Symbol *)d);
      if (removed == true) delete d;
      return removed;
    }
  }
  if (retval == false) return false;
  Changed();
  return true;
}

// Erase selected items
void Molecule::EraseSelected() {
  QPtrList<Bond> removebonds;
  QPtrList<Text> removetext;
  QPtrList<Symbol> removesym;

  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next()) {
    if (tmp_bond->Highlighted() == true) {
      removebonds.append(tmp_bond);
    }
  }
  for (tmp_bond = removebonds.first(); tmp_bond != NULL;
       tmp_bond = removebonds.next() ) {
    bonds.remove(tmp_bond);
    delete tmp_bond;
  }

  for (tmp_text = labels.first(); tmp_text != NULL; tmp_text = labels.next()) {
    if (tmp_text->Highlighted() == true) {
      removetext.append(tmp_text);
    }
  }
  for (tmp_text = removetext.first(); tmp_text != NULL;
       tmp_text = removetext.next() ) {
    labels.remove(tmp_text);
    delete tmp_text;
  }

  for (tmp_sym = symbols.first(); tmp_sym != NULL; tmp_sym = symbols.next()) {
    if (tmp_sym->Highlighted() == true) {
      removesym.append(tmp_sym);
    }
  }
  for (tmp_sym = removesym.first(); tmp_sym != NULL;
       tmp_sym = removesym.next() ) {
    symbols.remove(tmp_sym);
    delete tmp_sym;
  }

  Changed();
}

// XML (XDrawChem format) representation of this Molecule
QString Molecule::ToXML(QString xml_id) {
  QString s("");

  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  Drawable *tmp_draw;
  QString n1, nfull;
  int n = 0;

  up = AllPoints();
  uo = AllObjects();

  if (up.count() == 0) return s;  // don't write an empty file

  // Copy text from Text objects to element field in DPoint
  // also copy Symbol descriptions
  for (tmp_draw = uo.first(); tmp_draw != NULL; tmp_draw = uo.next() ) {
    tmp_draw->Start()->hit = false;
    if (tmp_draw->Type() == TYPE_TEXT) {
      tmp_text = (Text *)tmp_draw;  // is this cheating?
      tmp_text->Start()->element = tmp_text->getText();
      tmp_text->Start()->elementmask = tmp_text->getRichText();
      tmp_text->Start()->color = tmp_text->GetColor();
      tmp_text->Start()->font = tmp_text->getFont();
      tmp_text->Start()->hit = true;
    }
    if (tmp_draw->Type() == TYPE_SYMBOL) {
      tmp_sym = (Symbol *)tmp_draw;
      tmp_sym->Start()->symbol = tmp_sym->GetSymbol();
    }
  }

  // begin Molecule
  s.append("<molecule id=\"");
  s.append(xml_id);
  s.append("\">\n");

  if (group_type != GROUP_NONE) {
    s.append("<grouptype>");
    if (group_type == GROUP_REACTANT) s.append("reactant");
    if (group_type == GROUP_PRODUCT) s.append("product");
    s.append("</grouptype>\n");
  }

  // Add XML ID's to DPoint's, write as we go
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next() ) {
    n1.setNum(n);
    nfull = QString("a") + n1;
    tmp_pt->id = nfull;
    n++;
    s.append("<atom id=\"");
    s.append(tmp_pt->id);
    s.append("\">\n");
    s.append("<element>");
    s.append(tmp_pt->element);
    s.append("</element>\n");
    s.append("<elemask>");
    s.append(tmp_pt->elementmask);
    s.append("</elemask>\n");
    if (tmp_pt->hit) s.append("<textobject>yes</textobject>\n");
    if (tmp_pt->symbol.length() > 1) {
      s.append("<symtype>");
      s.append(tmp_pt->symbol);
      s.append("</symtype>\n");
    }
    s.append("<coordinate2>");
    n1.setNum(tmp_pt->x);
    s.append(n1);
    s.append(" ");
    n1.setNum(tmp_pt->y);
    s.append(n1);
    s.append("</coordinate2>\n");

    // write color
    s.append("<color>");
    n1.setNum(tmp_pt->color.red());
    s.append(n1);
    s.append(" ");
    n1.setNum(tmp_pt->color.green());
    s.append(n1);
    s.append(" ");
    n1.setNum(tmp_pt->color.blue());
    s.append(n1);
    s.append("</color>\n");

    // write font
    s.append("<font>");
    s.append(tmp_pt->font.family());
    s.append(QString("#"));
    n1.setNum(tmp_pt->font.pointSize());
    s.append(n1);
    s.append("</font>\n");

    s.append("</atom>\n");    
  }

  // add XML ID's to Bond's, write as we go
  n = 0;
  for (tmp_draw = uo.first(); tmp_draw != NULL; tmp_draw = uo.next() ) {
    if (tmp_draw->Type() == TYPE_BOND) {
      tmp_bond = (Bond *)tmp_draw;  // I ask again, is this cheating?
      n1.setNum(n);
      nfull = QString("b") + n1;
      tmp_draw->setID(nfull);
      n++;
      s.append("<bond id=\""); 
      s.append(tmp_bond->getID()); 
      s.append("\">\n<Start>");
      s.append(tmp_bond->Start()->id);
      s.append("</Start>\n<End>");
      s.append(tmp_bond->End()->id);
      s.append("</End>\n");
      s.append("<order>");
      n1.setNum(tmp_bond->Order());
      s.append(n1);
      s.append("</order>\n");
      s.append("<dash>");
      n1.setNum(tmp_bond->Dash());
      s.append(n1);
      s.append("</dash>\n");
      // write thickness
      s.append("<thick>");
      n1.setNum(tmp_bond->Thick());
      s.append(n1);
      s.append("</thick>\n");
      // write color
      s.append("<color>");
      n1.setNum(tmp_bond->GetColor().red());
      s.append(n1);
      s.append(" ");
      n1.setNum(tmp_bond->GetColor().green());
      s.append(n1);
      s.append(" ");
      n1.setNum(tmp_bond->GetColor().blue());
      s.append(n1);
      s.append("</color>\n");
      s.append("</bond>\n");      
    }
  }

  s.append("</molecule>\n");
  return s;
}

// XML (ChemDraw(TM) format) representation of this Molecule
QString Molecule::ToCDXML(QString xml_id) {
  QString s("");
  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  Drawable *tmp_draw;
  QString n1, nfull;
  int n = xml_id.toInt() + 1;

  up = AllPoints();
  uo = AllObjects();

  if (up.count() == 0) return s;  // don't write an empty file

  // also copy Symbol descriptions
  for (tmp_draw = uo.first(); tmp_draw != NULL; tmp_draw = uo.next() ) {
    if (tmp_draw->Type() == TYPE_SYMBOL) {
      tmp_sym = (Symbol *)tmp_draw;
      tmp_sym->Start()->symbol = tmp_sym->GetSymbol();
    }
  }

  // begin Molecule
  s.append("<fragment id=\"");
  s.append(xml_id);
  s.append("\">\n");

  // Add XML ID's to DPoint's, write as we go
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next() ) {
    n1.setNum(n);
    tmp_pt->id = n1;
    n++;
    s.append("<n id=\"");
    s.append(tmp_pt->id);
    s.append("\" p=\"");
    n1.setNum(tmp_pt->x);
    s.append(n1);
    s.append(" ");
    n1.setNum(tmp_pt->y);
    s.append(n1);
    s.append("\"");
    if (tmp_pt->element == "C") {
      s.append("/>\n");
    } else {
      s.append("><t><s font=\"21\" size=\"10\" face=\"96\">");
      s.append(tmp_pt->element);
      s.append("</s></t></n>\n");
    }
  }

  // add XML ID's to Bond's, write as we go
  for (tmp_draw = uo.first(); tmp_draw != NULL; tmp_draw = uo.next() ) {
    if (tmp_draw->Type() == TYPE_BOND) {
      tmp_bond = (Bond *)tmp_draw;  // I ask again, is this cheating?
      n1.setNum(n);
      tmp_draw->setID(n1);
      n++;
      s.append("<b id=\""); 
      s.append(tmp_bond->getID()); 
      s.append("\" B=\"");
      s.append(tmp_bond->Start()->id);
      s.append("\" E=\"");
      s.append(tmp_bond->End()->id);
      if (tmp_bond->Order() < 4) {
	s.append("\" Order=\"");
	n1.setNum(tmp_bond->Order());
	s.append(n1);
      }
      if (tmp_bond->Order() == 5) {
	s.append("\" Display=\"WedgeBegin");
      }
      if (tmp_bond->Order() == 7) {
	s.append("\" Display=\"WedgedHashBegin");
      }
      s.append("\"/>\n");
    }
  }

  s.append("</fragment>\n");
  return s;
}

// Generate MDL Molfile to export to generate SMILES, export for 3-D 
QString Molecule::ToMDLMolfile(int coords) {
  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  Bond *tmp_bond;
  QString tmpline;
  int acount = 0, bcount = 0;

  // get all unique points and objects
  up = AllPoints();

  QString molfile;

  QTextStream t(&molfile, IO_WriteOnly);

  // write first three lines -- blank, unless we want to add comments etc.
  t << endl;
  t << endl;
  t << endl;
  // find counts...
  for (tmp_pt = up.first(); tmp_pt != 0; tmp_pt = up.next() ) {
    acount++;
  }
  bcount = bonds.count();
  // write counts line
  t.width(3);
  t << acount;
  t.width(3);
  t << bcount << "  0  0  0  0  0  0  0  0  1" << endl;

  // Write atom list
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next() ) {
    tmpline = tmp_pt->element;
    if (tmpline.length() < 3) tmpline.append(' ');
    if (tmpline.length() < 3) tmpline.append(' ');
    if (coords == 0) {  // 2D
      t.width(10);
      t << tmp_pt->x;
      t.width(10);
      t << -tmp_pt->y << "    0.0000 " << tmpline;
    } else {  // 3D
      t.width(10);
      t << tmp_pt->x;
      t.width(10);
      t << tmp_pt->y;
      t.width(10);
      t << tmp_pt->z;
      t << tmpline;
    }
    t << " 0  0  0  0  0  0  0  0  0  0  0  0" << endl;
  }
  // write bond list
  for (tmp_bond = bonds.first(); tmp_bond != NULL; tmp_bond = bonds.next() ) {
    t.width(3);
    t << up.find( tmp_bond->Start() ) + 1;
    t.width(3);
    t << up.find( tmp_bond->End() ) + 1;
    t.width(3);
    t << tmp_bond->Order();
    if (tmp_bond->Order() < 4)
      t << "  0  0  0  0" << endl;
    if (tmp_bond->Order() == 5)
      t << "  1  0  0  0" << endl;
    if (tmp_bond->Order() == 7)
      t << "  6  0  0  0" << endl;
  }

  t << "M  END" << endl;
  
  return molfile;
}

void Molecule::FromXML(QString xml_tag) {
  QString bondtag, subtag, startid, endid;
  QPtrList<DPoint> points;
  DPoint *s1, *e1;
  int i1, i2;

  i1 = xml_tag.find("<grouptype>");
  if (i1 > 0) {
    subtag = xml_tag.mid(i1, 20);
    if (subtag.lower().contains("reactant") > 0) group_type = GROUP_REACTANT;
    if (subtag.lower().contains("product") > 0) group_type = GROUP_PRODUCT;
  }

  // Extract atom's
  do {
    i1 = xml_tag.find("<atom ");
    i2 = xml_tag.find("</atom>") + 7;
    if (i1 >= 0) {
      tmp_pt = new DPoint;
      tmp_pt->FromXML(xml_tag.mid(i1, i2 - i1));
      points.append(tmp_pt);
      xml_tag.remove(i1, i2 - i1);
    } else {
      break;
    }
  } while (1);
  // Add bond's
  do {
    i1 = xml_tag.find("<bond ");
    i2 = xml_tag.find("</bond>") + 7;
    if (i1 >= 0) {
      tmp_bond = new Bond(r);
      bondtag = xml_tag.mid(i1, i2 - i1);
      xml_tag.remove(i1, i2 - i1);
      cout << bondtag << endl;
      // parse Start
      i1 = bondtag.find("<Start>");
      i2 = bondtag.find("</Start>") + 8;
      subtag = bondtag.mid(i1, i2 - i1);
      i1 = subtag.find("<Start>");
      i2 = subtag.find("</Start>");
      subtag.remove(i2, 999);
      subtag.remove(i1, 7);
      for (tmp_pt = points.first(); tmp_pt != NULL; tmp_pt = points.next() ) {
	if (tmp_pt->id == subtag) {
	  s1 = tmp_pt;
	  break;
	}
      }
      // parse End
      i1 = bondtag.find("<End>");
      i2 = bondtag.find("</End>") + 6;
      subtag = bondtag.mid(i1, i2 - i1);
      i1 = subtag.find("<End>");
      i2 = subtag.find("</End>");
      subtag.remove(i2, 999);
      subtag.remove(i1, 5);
      for (tmp_pt = points.first(); tmp_pt != NULL; tmp_pt = points.next() ) {
	if (tmp_pt->id == subtag) {
	  e1 = tmp_pt;
	  break;
	}
      }
      // add points
      tmp_bond->setPoints(s1, e1);
      // parse Order
      i1 = bondtag.find("<order>");
      i2 = bondtag.find("</order>") + 8;
      subtag = bondtag.mid(i1, i2 - i1);
      i1 = subtag.find("<order>");
      i2 = subtag.find("</order>");
      subtag.remove(i2, 999);
      subtag.remove(i1, 7);
      tmp_bond->setOrder(subtag.toInt());
      // parse Dash
      i1 = bondtag.find("<dash>");
      i2 = bondtag.find("</dash>") + 7;
      subtag = bondtag.mid(i1, i2 - i1);
      i1 = subtag.find("<dash>");
      i2 = subtag.find("</dash>");
      subtag.remove(i2, 999);
      subtag.remove(i1, 6);
      tmp_bond->setDash(subtag.toInt());
      // parse thickness
      i1 = bondtag.find("<thick>");
      i2 = bondtag.find("</thick>") + 8;
      subtag = bondtag.mid(i1, i2 - i1);
      i1 = subtag.find("<thick>");
      i2 = subtag.find("</thick>");
      subtag.remove(i2, 999);
      subtag.remove(i1, 7);
      tmp_bond->setThick(subtag.toInt());
      // parse Dash
      i1 = bondtag.find("<color>");
      i2 = bondtag.find("</color>") + 8;
      subtag = bondtag.mid(i1, i2 - i1);
      tmp_bond->SetColor(GetColorFromXML(subtag));
      bonds.append(tmp_bond);
    } else {
      break;
    }
  } while (1);
  // add Text and Symbol as needed
  for (tmp_pt = points.first(); tmp_pt != NULL; tmp_pt = points.next()) {
    if ( (tmp_pt->element != QString("C")) || (tmp_pt->hit == true) ) {
      tmp_text = new Text(r);
      tmp_text->setPoint(tmp_pt);
      tmp_text->SetColor(tmp_pt->color);
      tmp_text->setFont(tmp_pt->font);
      tmp_text->setText(tmp_pt->elementmask);
      if (tmp_pt->elementmask.length() > 0) {
	tmp_text->setTextMask(tmp_pt->elementmask);
      } else {
	tmp_pt->element.fill(' ');
	tmp_text->setTextMask(tmp_pt->element);
      }
      labels.append(tmp_text);
    }
    if (tmp_pt->symbol.length() > 0) {
      tmp_sym = new Symbol(r);
      tmp_sym->setPoint(tmp_pt);
      tmp_sym->SetSymbol(tmp_pt->symbol);
      symbols.append(tmp_sym);
    }
  }
}

// update MW and formula
void Molecule::Changed() {
  // add hydrogens and correct labels
  //cout << "changed" << endl;
  AddHydrogens();
  return;
}
