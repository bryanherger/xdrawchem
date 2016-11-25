#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

using std::cerr;
using std::endl;

#include "dpoint.h"
#include "chemdata.h"
#include "application.h"
#include "bond.h"
#include "molecule.h"

struct Point {

  float x,y;
  bool operator==(const Point& point) const { return (x==point.x&&y==point.y); }
  bool operator<(const Point& point) const { return (x*x+y*y)<(point.x*point.x+point.y*point.y); }

};

struct pt_cmp {

  bool operator() (const Point& one, const Point& two) { return one<two; }

};

using namespace OpenBabel;

const char Molecule::symbol[110][4] =
{
  "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
  "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",		// 20
  "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
  "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr",		// 40
  "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
  "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",		// 60
  "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
  "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg",		// 80
  "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
  "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm",		// 100
  "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Uun"
};

bool Molecule::convertFromOBMol(OBMol *obmol) {

  OBBond* bond;
  OBAtom *atom1,*atom2;
  DPoint *s,*e;
  Point point;
  Text* text;

  std::vector<OBEdgeBase*>::iterator bonditr;
  std::map<Point,DPoint*,pt_cmp> points;
  std::map<Point,DPoint*,pt_cmp>::iterator itr;

  //int i=0;
  float x,y;

  /* look through bonds and store points found since ChemData::addBond() requires
     DPoint*, and not DPoint. */

  for(bond=obmol->BeginBond(bonditr);bond;bond=obmol->NextBond(bonditr)) {

    atom1=bond->GetBeginAtom();
    atom2=bond->GetEndAtom();

    x=atom1->GetX();x=(x+5)*15+100;
    y=atom1->GetY();y=(y+5)*15+100;
      
    point.x=x;
    point.y=y;

    itr=points.find(point);
    if(itr==points.end()) {
      std::cerr << "true" << endl;
      points[point]=new DPoint(x,y);
    }

    x=atom2->GetX();x=(x+5)*15+100;
    y=atom2->GetY();y=(y+5)*15+100;

    point.x=x;
    point.y=y;

    itr=points.find(point);
    if(itr==points.end())
      points[point]=new DPoint(x,y);

  }

  /* use information from previous for-loop and add bonds and labels*/

  for(bond=obmol->BeginBond(bonditr);bond;bond=obmol->NextBond(bonditr)) {

    atom1=bond->GetBeginAtom();
    atom2=bond->GetEndAtom();

    x=atom1->GetX();x=(x+5)*15+100;
    y=atom1->GetY();y=(y+5)*15+100;
      
    point.x=x;
    point.y=y;

    itr=points.find(point);
    s=(*itr).second;

    x=atom2->GetX();x=(x+5)*15+100;
    y=atom2->GetY();y=(y+5)*15+100;

    point.x=x;
    point.y=y;

    itr=points.find(point);
    e=(*itr).second;

    int bondorder=bond->GetBondOrder();

    //set elements
    if(!atom1->IsCarbon()) {
      QString str("<element>");
      str+=symbol[atom1->GetAtomicNum()-1];
      str+="</element>";
      s->SetElementFromXML(str);
				
    }

    if(!atom2->IsCarbon()) {
      QString str("<element>");
      str+=symbol[atom2->GetAtomicNum()-1];
      str+="</element>";
      e->SetElementFromXML(str);

    }

    addBond(s,e,1,bondorder,QColor("black"),true);

    //label atoms if not Carbon

    if(!atom1->IsCarbon()) {

      text=new Text(r);
      QString str=symbol[atom1->GetAtomicNum()-1];
      text->setText(str);
      text->setPoint(s);
      str.fill(' ');
      text->setTextMask(str);
      addText(text);
    }

    if(!atom2->IsCarbon()) {

      text=new Text(r);
      QString str=symbol[atom2->GetAtomicNum()-1];
      text->setText(str);
      text->setPoint(e);
      str.fill(' ');
      text->setTextMask(str);
      text->setJustify(JUSTIFY_CENTER);
      addText(text);

    }
  }

  return true;
}

//
// TODO :)
//
OBMol * Molecule::convertToOBMol() {
  QPtrList<DPoint> allpoints;
  QPtrList<Bond> allbonds;
  DPoint *tmp_atom;
  Bond *tmp_bond;
  Molecule *this_mol;
  OBMol *obmol = new OBMol;

  allpoints = AllPoints();
  allbonds = AllBonds();

  // adapted from openbabel/mdl.cpp, most similar format to this...
  // (i.e., MDL is atom list followed by bond list)

  obmol->BeginModify();
  obmol->ReserveAtoms( allpoints.count() );
  char type[5];
  vector3 v;
  OBAtom atom;

  for (tmp_atom = allpoints.first(); tmp_atom != 0; 
       tmp_atom = allpoints.next()) {
    v.SetX(tmp_atom->x);v.SetY(tmp_atom->y);v.SetZ(tmp_atom->z);
    atom.SetVector(v);
    atom.SetAtomicNum( tmp_atom->getAtomicNumber() );
    strcpy(type, tmp_atom->baseElement() );
    atom.SetType( type );

    if (!obmol->AddAtom(atom))
      return(false);
    atom.Clear();
  }

  int start,end,order,flag,bst;
  for (tmp_bond = allbonds.first(); tmp_bond != 0; 
       tmp_bond = allbonds.next()) {
    flag = 0;
    start = allpoints.find(tmp_bond->Start()) + 1;
    end = allpoints.find(tmp_bond->End()) + 1;
    order = tmp_bond->Order();
    bst = tmp_bond->Stereo();
    if (bst == STEREO_UP) flag |= OB_WEDGE_BOND;
    if (bst == STEREO_DOWN) flag |= OB_HASH_BOND;
    if (!obmol->AddBond(start,end,order,flag)) return(false);
  }

  obmol->EndModify();

  return obmol;
}

//eof
