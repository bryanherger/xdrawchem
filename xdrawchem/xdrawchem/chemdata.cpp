#include <iostream>
#include <qobject.h>
#include <qptrlist.h>

using std::cout;
using std::endl;

#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "curvearrow.h"
#include "bracket.h"
#include "text.h"
#include "symbol.h"
#include "biotools.h"
#include "chemdata.h"
#include "defs.h"

using std::cout;

ChemData::ChemData(QObject *parent, const char *name)
  : QObject(parent, name)
{
  thick_kludge = -1;
}

void ChemData::drawAll() {
  // draw all objects in ChemData
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    tmp_draw->Render();
  }
}

// update Molecules after move
void ChemData::FinishMove() {
  Molecule *tmp_mol;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Type() == TYPE_MOLECULE) {
      tmp_mol = (Molecule *)tmp_draw;
      tmp_mol->Changed();
    }
  }
}

Molecule * ChemData::firstMolecule() {
  Molecule *tmp_mol;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Type() == TYPE_MOLECULE) {
      tmp_mol = (Molecule *)tmp_draw;
      return tmp_mol;
    }
  }
  return 0;
}

void ChemData::addMolecule(Molecule *m1) {
  drawlist.append(m1);
}

void ChemData::addArrow(DPoint *s, DPoint *e, QColor c, int t, int p2,
			bool hl) {
  Arrow *a1 = new Arrow(r);
  a1->setPoints(s, e);
  a1->SetColor(c);
  a1->SetStyle(t);
  a1->setThick(p2);
  if (hl) a1->Highlight(true);
  drawlist.append(a1);
}

void ChemData::addCurveArrow(DPoint *s, DPoint *e, QColor c, QString s1,
			     bool hl) {
  CurveArrow *a1 = new CurveArrow(r);
  a1->setPoints(s, e);
  a1->SetColor(c);
  a1->SetCurve(s1);
  if (hl) a1->Highlight(true);
  drawlist.append(a1);
}

void ChemData::addBracket(DPoint *s, DPoint *e, QColor c, int type, bool hl) {
  Bracket *a1 = new Bracket(r);
  a1->setPoints(s, e);
  a1->SetColor(c);
  a1->SetStyle(type);
  if (hl) a1->Highlight(true);
  drawlist.append(a1);
}

void ChemData::addText(Text *t) {
  if (t->Justify() == JUSTIFY_TOPLEFT) { // add to drawing
    drawlist.append(t);
  } else { // add label to specific Molecule
    for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
	 tmp_draw = drawlist.next()) {
      if (tmp_draw->Find(t->Start()) == true) {
	Molecule *tm = (Molecule *)tmp_draw;  // this is cheating, I know!
	tm->addText(t);
	return;
      }
    }
    std::cerr << "FYI, add text failed" << std::endl;
  }
}

void ChemData::addGraphicObject(GraphicObject *t) {
  drawlist.append(t);
}

void ChemData::addBond(DPoint *s, DPoint *e, int thick, int order, 
		       QColor c, bool hl) {
  //cout << "Request to add bond:" << endl;
  //cout << "(" << s->x << "," << s->y << ")-(" << e->x << "," << e->y << ")";
  //cout << endl;
  Drawable *m1 = 0, *m2 = 0;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Find(s) == true) m1 = tmp_draw;
    if (tmp_draw->Find(e) == true) m2 = tmp_draw;
  }
  // neither point exists -- create new Molecule
  if ( (m1 == 0) && (m2 == 0) ) {
    Molecule *m = new Molecule(r);
    m->SetChemdata(this);
    m->addBond(s, e, thick, order, c, hl);
    drawlist.append(m);
    return;
  }
  // one point exists, or both in same molecule
  if ( (m1 == 0) && (m2 != 0) ) {
    m1 = m2; m2 = 0;
  }
  if ( ( (m1 != 0) && (m2 == 0) ) || (m1 == m2) ) {
    m1->addBond(s, e, thick, order, c, hl);
    return;
  }
  // both points exist in different molecules
  if (m1 != m2) {
    m1->addBond(s, e, thick, order, c, hl);
    m1->addMolecule(m2);
    drawlist.remove(m2);
    delete m2;
  }
}

void ChemData::addSymbol(DPoint *a, QString symbolfile, bool hl) {
  Molecule *m1;
  Symbol *s1 = new Symbol(r);
  s1->setPoint(a);
  s1->SetSymbol(symbolfile);
  if (hl) s1->Highlight(true);
  // determine whether point exists or not; if exists, add to Molecule
  for (tmp_draw = drawlist.first(); tmp_draw != NULL;
       tmp_draw = drawlist.next()) {
    if ( (tmp_draw->Find(a) == true) && (tmp_draw->Type() == TYPE_MOLECULE) ) {
      m1 = (Molecule *)tmp_draw;
      m1->addSymbol(s1);
      return;
    }
  }
  drawlist.append(s1);
}

Molecule * ChemData::insideMolecule(DPoint *t1) {
  Molecule *m1;
  //std::cout << t1->x << "," << t1->y << std::endl;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL;
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Type() == TYPE_MOLECULE) {
      m1 = (Molecule *)tmp_draw;
      //QRect tr1 = m1->BoundingBoxAll();
      //std::cout << tr1.left() << "," << tr1.top() << ";";
      //std::cout << tr1.right() << "," << tr1.bottom() << std::endl;
      if (m1->BoundingBoxAll().contains(t1->toQPoint(), false)) return m1;
    }
  }
  return 0;
}

DPoint * ChemData::FindNearestPoint(DPoint *target, double &dist) {
  DPoint *nearest = 0, *d1;
  double mindist = 9999.0, d1dist = 999999.0;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    d1 = tmp_draw->FindNearestPoint(target, d1dist);
    if (d1dist < mindist) { mindist = d1dist; nearest = d1; }
  }
  dist = mindist;
  return nearest;
}

Drawable * ChemData::FindNearestObject(DPoint *target, double &dist) {
  Drawable *nearest = 0, *d1;
  double mindist = 2000.0, d1dist = 999999.0;
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    d1 = tmp_draw->FindNearestObject(target, d1dist);
    if (d1dist < mindist) { mindist = d1dist; nearest = d1; }
  }
  dist = mindist;
  return nearest;
}

void ChemData::Erase(Drawable *d) {
  QPtrList<Drawable> removelist;
  QPtrList<Molecule> split_list;
  bool erased = false;

  if (drawlist.remove(d) == false) {
    for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
	 tmp_draw = drawlist.next()) {
      erased = tmp_draw->Erase(d);
      // collect empty Molecules for removal
      if (tmp_draw->Members() == 0) removelist.append(tmp_draw);
      cout << "erased:" << erased << endl;
      if (erased == true) break;  //should only be one instance of d to remove!
    }
  } else { // drawlist.remove(d) == true
    delete d;
  }
  // remove empty Molecules
  for (tmp_draw = removelist.first(); tmp_draw != NULL; 
       tmp_draw = removelist.next() ) {
    drawlist.remove(tmp_draw);
    delete tmp_draw;
  }
  // Split Molecules as needed
  DetectSplit();
}

void ChemData::EraseSelected() {
  Molecule *m;
  QPtrList<Drawable> removelist;

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Type() == TYPE_MOLECULE) {
      m = (Molecule *)tmp_draw;
      m->EraseSelected();
      // collect empty Molecules for removal
      if (tmp_draw->Members() == 0) removelist.append(tmp_draw);
    } else {
      if (tmp_draw->Highlighted() == true) {
	removelist.append( tmp_draw );
      }
    }
  }
  for (tmp_draw = removelist.first(); tmp_draw != NULL; 
       tmp_draw = removelist.next() ) {
    /*
    if (tmp_draw->Type() == TYPE_TEXT) {
      Text *tmp_text = (Text *)tmp_draw;
      if (tmp_text->getDataType() == TEXT_DATA_MW) {
	tmp_text->getMolecule()->MWLabelDeleted();
      }
      if (tmp_text->getDataType() == TEXT_DATA_FORMULA) {
	tmp_text->getMolecule()->FormulaLabelDeleted();
      }
    }
    */
    drawlist.remove(tmp_draw);
    delete tmp_draw;
  }
  // Split Molecules as needed
  DetectSplit();
}

// Split Molecule's which hold multiple structures (e.g. after delete)
void ChemData::DetectSplit() {
  QPtrList<Drawable> removelist;
  QPtrList<Molecule> split_list;
  Molecule *tmp_mol;
  Drawable *td2;

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    if (tmp_draw->Type() == TYPE_MOLECULE) {
      tmp_mol = (Molecule *)tmp_draw;
      split_list = tmp_mol->MakeSplit();
      if (split_list.count() > 1) {
	cout << "Split needed" << endl;
	removelist.append(tmp_draw);
	for (td2 = split_list.first(); td2 != NULL; td2 = split_list.next()) {
	  drawlist.append(td2);
	}
	split_list.clear();
      }
    }
  }
  // remove old Molecules
  for (tmp_draw = removelist.first(); tmp_draw != NULL; 
       tmp_draw = removelist.next() ) {
    drawlist.remove(tmp_draw);
    delete tmp_draw;
  }
}

void ChemData::SelectAll(void) {
  QPtrList<DPoint> allpts = UniquePoints();

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    tmp_draw->SelectAll();
  }
  for (tmp_pt = allpts.first(); tmp_pt != NULL; tmp_pt = allpts.next()) {
    tmp_pt->setHighlighted(true);
  }
}

void ChemData::DeselectAll(void) {
  QPtrList<DPoint> allpts = UniquePoints();

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) 
    tmp_draw->DeselectAll();
  for (tmp_pt = allpts.first(); tmp_pt != NULL; tmp_pt = allpts.next()) {
    tmp_pt->setHighlighted(false);
  }
}

void ChemData::SetColorIfHighlighted(QColor c) {
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) 
    tmp_draw->SetColorIfHighlighted(c);
}

void ChemData::Move(double dx, double dy) {
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) 
    tmp_draw->Move(dx, dy);
}

void ChemData::Resize(DPoint *d1, double dy) {
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) 
    tmp_draw->Resize(d1, dy);
}

void ChemData::Rotate(DPoint *d1, double dy) {
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) 
    tmp_draw->Rotate(d1, dy);
}

void ChemData::Flip(DPoint *d1, int dy) {
  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) 
    tmp_draw->Flip(d1, dy);
}

// Find minimum rectangle needed to enclose selection
QRect ChemData::selectionBox() {
  int top = 99999, bottom = 0, left = 99999, right = 0;
  QRect tmprect;

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    tmprect = tmp_draw->BoundingBox();
    cout << tmprect.width() << "X" << tmprect.height() << endl;
    if (tmprect.isValid()) {
      if (tmprect.left() < left) left = tmprect.left();
      if (tmprect.right() > right) right = tmprect.right();
      if (tmprect.top() < top) top = tmprect.top();
      if (tmprect.bottom() > bottom) bottom = tmprect.bottom();
    }
  }

  left -= 3; right += 5; top -= 3; bottom += 3;
  if (left < 0) left = 0;
  if (top < 0) top = 0;
  return QRect( QPoint(left,top), QPoint(right,bottom) );
}

// when doing multiple selection via MODE_SELECT_MULTIPLE, we will
// have to highlight/unhighlight regions of the drawing as the selection
// box changes.  This function is called to start checking whether objects
// fall within the select box.
void ChemData::NewSelectRect(QRect n, bool shiftdown) {
  QPtrList<DPoint> allpts = UniquePoints();

  for (tmp_pt = allpts.first(); tmp_pt != NULL; tmp_pt = allpts.next()) {
    if (n.contains(tmp_pt->toQPoint()) == true) {
      tmp_pt->setHighlighted(true);
    } else {
      tmp_pt->setHighlighted(false);
    }
  }

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    tmp_draw->WithinRect(n, shiftdown);
  }
}

// Get list of unique points contained in all Molecules.
QPtrList<DPoint> ChemData::UniquePoints() {
  QPtrList<DPoint> up, tp;

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    tp = tmp_draw->AllPoints();
    for (tmp_pt = tp.first(); tmp_pt != NULL; tmp_pt = tp.next() )
      up.append(tmp_pt);
  }

  cout << up.count() << endl;
  return up;
}

QPtrList<Drawable> ChemData::UniqueObjects() {
  QPtrList<Drawable> uo, to;
  Drawable *td2;

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    to = tmp_draw->AllObjects();
    for (td2 = to.first(); td2 != NULL; td2 = to.next() )
      uo.append(td2);
  }

  cout << uo.count() << endl;
  return uo;
}
