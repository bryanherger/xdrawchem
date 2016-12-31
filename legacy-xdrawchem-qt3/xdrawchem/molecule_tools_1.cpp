// molecule_tools_1.cpp - Molecule's implementation of (more) functions

#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <iostream>
#include <math.h>

using std::cout;
using std::endl;

#include "graphdialog.h"
#include "molinfodialog.h"
#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "netaccess.h"
#include "dpoint.h"
#include "defs.h"
//#include "namer.h"

// Preferences
extern Preferences preferences;
// Prediction stuff
#include "boshcp.h"
// SSSR (see comments in this file)
#include "molecule_sssr.h"

// note that this function builds the DPoint::neighbors list as well
void Molecule::MakeSSSR() {
  AllNeighbors();
  QPtrList<DPoint> groupAtoms = AllPoints();
  this_sssr.BuildSSSR(groupAtoms);
  this_sssr.PrintSSSR();
  this_sssr.FindAromatic(bonds);
}

int Molecule::OrderOfBond(DPoint *e1, DPoint *e2) {
  for (tmp_bond = bonds.first(); tmp_bond != 0; tmp_bond = bonds.next()) {
    if (tmp_bond->Find(e1) == true) {
      tmp_pt = tmp_bond->otherPoint(e1);
      if (tmp_pt == e2) return tmp_bond->Order();
    }  
  }

  return 0;
}

Bond * Molecule::FindBond(DPoint *e1, DPoint *e2) {
  for (tmp_bond = bonds.first(); tmp_bond != 0; tmp_bond = bonds.next()) {
    if (tmp_bond->Find(e1) == true) {
      tmp_pt = tmp_bond->otherPoint(e1);
      if (tmp_pt == e2) return tmp_bond;
    }  
  }

  return 0;
}

QString Molecule::CalcName() {
  QString nom = ToSMILES();
  nom = nom.stripWhiteSpace();
  /* canonical SMILES no longer available  :(
  // compose Python command (hopefully this function only gets called
  // when the Python test passes in application.cpp :)
  NetAccess na1;
  QString serverName = getenv("XDC_SERVER");
  if (serverName.length() < 2)
    serverName = XDC_SERVER;
  nom = na1.getCanonicalSmiles(serverName, nom);

  nom = nom.stripWhiteSpace();
  std::cout << nom << std::endl;
  */
  return nom;
}

QStringList Molecule::Calc13CNMR(bool show_dialog) {
  BremserOneSphereHOSECodePredictor boshcp;
  QStringList hosecodes;
  QPtrList<DPoint> sphere1;
  QString tmp_str, ts;
  DPoint *tmp_pt2, *tmp_pt3;
  int hs, hsmax;

  // get list of unique points
  up = AllPoints();
  // find rings (esp. find aromaticity) - do after CopyTextToDPoint()
  MakeSSSR();

  // clear peak list
  peaklist.clear();

  // Scan for keto groups
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next()) {
    tmp_pt->ketos = 0;
    for (tmp_bond = bonds.first(); tmp_bond != 0; tmp_bond = bonds.next()) {
      if (tmp_bond->Find(tmp_pt) == true) {
	tmp_pt2 = tmp_bond->otherPoint(tmp_pt);
	if ( (tmp_pt2->element == "O") && (tmp_bond->Order() == 2) )
	  tmp_pt->ketos += 1;
      }
    }
  }
  // Determine atoms surrounding each atom and build HOSE code list
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next()) {
    if (tmp_pt->element == "C") {  // only look at carbons
      for (tmp_bond = bonds.first(); tmp_bond != 0; tmp_bond = bonds.next()) {
	if (tmp_bond->Find(tmp_pt) == true) {
	  tmp_str = "";
	  hs = 0;
	  tmp_pt2 = tmp_bond->otherPoint(tmp_pt);
	  if (tmp_bond->Order() == 3) { tmp_str.append("%"); hs += 300; }
	  if ( (tmp_pt->aromatic == true) && (tmp_pt2->aromatic == true) ) {
	    tmp_str.append("*"); hs += 100;
	  } else {
	    if (tmp_bond->Order() == 2) {
	      if (tmp_bond->Dash() == 0) 
		{ tmp_str.append("="); hs += 200; }
	      if (tmp_bond->Dash() == 1)
		{ tmp_str.append("*"); hs += 100; }
	    }
	  }
	  if (tmp_pt2->element == "C") { tmp_str.append("C"); hs += 24; }
	  if (tmp_pt2->element == "O") { tmp_str.append("O"); hs += 22; }
	  if (tmp_pt2->element == "OH") { tmp_str.append("O"); hs += 22; }
	  if (tmp_pt2->element == "HO") { tmp_str.append("O"); hs += 22; }
	  if (tmp_pt2->element == "N") { tmp_str.append("N"); hs += 20; }
	  if (tmp_pt2->element == "NH2") { tmp_str.append("N"); hs += 20; }
	  if (tmp_pt2->element == "H2N") { tmp_str.append("N"); hs += 20; }
	  if (tmp_pt2->element == "NH") { tmp_str.append("N"); hs += 20; }
	  if (tmp_pt2->element == "HN") { tmp_str.append("N"); hs += 20; }
	  if (tmp_pt2->element == "P") { tmp_str.append("P"); hs += 18; }
	  if (tmp_pt2->element == "S") { tmp_str.append("S"); hs += 16; }
	  if (tmp_pt2->element == "SH") { tmp_str.append("S"); hs += 16; }
	  if (tmp_pt2->element == "HS") { tmp_str.append("S"); hs += 16; }
	  if (tmp_pt2->element == "Si") { tmp_str.append("Q"); hs += 14; }
	  if (tmp_pt2->element == "B") { tmp_str.append("B"); hs += 12; }
	  if (tmp_pt2->element == "F") { tmp_str.append("F"); hs += 10; }
	  if (tmp_pt2->element == "Cl") { tmp_str.append("X"); hs += 8; }
	  if (tmp_pt2->element == "Br") { tmp_str.append("Y"); hs += 6; }
	  if (tmp_pt2->element == "I") { tmp_str.append("I"); hs += 4; }
	  if (tmp_pt2->ketos == 2) { tmp_str.append("$$"); }
	  if (tmp_pt2->ketos == 1) { tmp_str.append("$"); }
	  tmp_pt2->tmphose = tmp_str;
	  tmp_pt2->hosescore = hs;
	  sphere1.append(tmp_pt2);
	}
      }
      tmp_str = "";
      do {
	hsmax = -1;
	for (tmp_pt3 = sphere1.first(); tmp_pt3 != 0; 
	     tmp_pt3 = sphere1.next()) {
	  if (tmp_pt3->hosescore > hsmax) { 
	    hsmax = tmp_pt3->hosescore; 
	    ts = tmp_pt3->tmphose;
	    tmp_pt2 = tmp_pt3;
	  }
	}
	sphere1.remove(tmp_pt2);
	tmp_str.append(ts);
      } while (sphere1.count() > 0);
      tmp_str.append("(//)");
      hosecodes.append(tmp_str);
      tmp_pt->hosecode = tmp_str;
      cout << tmp_str << endl;
    }
  }

  GraphDialog *g = new GraphDialog(r, tr("Predicted 13C-NMR"));
  // print (for now) list of NMR values
  for ( QStringList::Iterator it = hosecodes.begin(); 
	it != hosecodes.end(); ++it ) {
    //cout << boshcp.predictFull(*it) << endl;
    g->AddPeak(boshcp.predict(*it), boshcp.getMult(*it), 
	       boshcp.predictFull(*it));
    tmp_peak = new Peak;
    tmp_peak->value = boshcp.predict(*it);
    tmp_peak->intensity = 1;
    tmp_peak->multiplicity = 1;
    if (boshcp.getMult(*it) == "D")
      tmp_peak->multiplicity = 2;
    if (boshcp.getMult(*it) == "T")
      tmp_peak->multiplicity = 3;
    if (boshcp.getMult(*it) == "Q")
      tmp_peak->multiplicity = 4;
    tmp_peak->comment = boshcp.predictFull(*it);
    // check for existing peak
    for (Peak *tpeak = peaklist.first(); tpeak != 0; tpeak = peaklist.next()) {
      if ( (tpeak->value == tmp_peak->value) &&
	   (tpeak->multiplicity == tmp_peak->multiplicity) ) {
	tpeak->intensity += 1;
	delete tmp_peak;
	tmp_peak = 0;
	break;
      }
    }
    if (tmp_peak != 0) peaklist.append(tmp_peak);
  }

  if (show_dialog == false) {
    delete g;
    return hosecodes;
  }

  QPixmap mol = r->MakeFullPixmap();
  QRect rr1 = BoundingBoxAll();
  rr1.setLeft(rr1.left() - 4);
  rr1.setTop(rr1.top() - 4);
  rr1.setBottom(rr1.bottom() + 4);
  rr1.setRight(rr1.right() + 4);
  QPixmap mol1(rr1.size());
  bitBlt(&mol1, QPoint(0,0), &mol, rr1);

  g->AddPixmap(mol1);
  g->show();

  delete g;
  return hosecodes;
}

void Molecule::CalcIR() {
  QPtrList<DPoint> up;
  int atom1, atom2, lorder, swp;

  // get list of unique points
  up = AllPoints();
  // find rings (esp. find aromaticity)
  MakeSSSR();

  peaklist.clear();

  GraphDialog *g = new GraphDialog(r, tr("Predicted IR"));
  // iterate thru all Bonds in Molecule
  QPtrListIterator<Bond> outer(bonds), inner(bonds);
  for ( ; outer.current(); ++outer) {
    // check for 'obvious' cases
    tmp_bond = outer.current();
    // rules for atoms and groups
    lorder = tmp_bond->Order();
    if (lorder > 3) lorder = 1;  // stereo bonds (5,7) are single bonds (1)
    atom1 = tmp_bond->Start()->getAtomicNumber();
    atom2 = tmp_bond->End()->getAtomicNumber();
    if (atom1 > atom2) { swp = atom1; atom1 = atom2; atom2 = swp; }
    if (lorder == 1) {
      if ( (atom1 == 6) && (atom2 == 7) )
	AddPeak(1300.0, QString("C-N"), QString(tr("~1350-1000, C-N")));
      if ( (atom1 == 6) && (atom2 == 8) )
	AddPeak(1300.0, QString("C-O"), QString(tr("~1300-1000, C-O")));
    }
    if (lorder == 2) {
      if ( (atom1 == 6) && (atom2 == 6) )
	AddPeak(1660.0, QString("C=C"), QString(tr("~1660-1600, C=C (cis/vinyl strong; trans weak)")));
      if ( (atom1 == 6) && (atom2 == 7) )
	AddPeak(1670.0, QString("C=N"), QString(tr("~1690-1640, C=N")));
      if ( (atom1 == 6) && (atom2 == 8) )
	AddPeak(1700.0, QString("C=O"), QString(tr("~1700 (narrow), C=O")));
      if ( (atom1 == 8) && (atom2 == 16) )
	AddPeak(1350.0, QString("S=O"), QString(tr("~1350-1300, S=O (~1050 if R-(S=O)-R')")));
    }
    if (lorder == 3) {
      if ( (atom1 == 6) && (atom2 == 7) )
	AddPeak(2250.0, QString("CN"), QString(tr("~2250 (narrow), nitrile")));
    }

    /* ####################################################################
    if (tmp_bond->Order() == 1) {
      if ( (tmp_bond->Start()->element == "C") &&
	   (tmp_bond->End()->element.contains("O") > 0) ) {
	AddPeak(1300.0, QString("C-O"), QString(tr("~1300-1000, C-O")));
      }
      if ( (tmp_bond->Start()->element.contains("O") > 0) &&
	   (tmp_bond->End()->element == "C") ) {
	AddPeak(1300.0, QString("C-O"), QString(tr("~1300-1000, C-O")));
      }
      if ( (tmp_bond->Start()->element == "C") &&
	   (tmp_bond->End()->element == "N") ) {
	AddPeak(1300.0, QString("C-N"), QString(tr("~1350-1000, C-N")));
      }
      if ( (tmp_bond->Start()->element == "N") &&
	   (tmp_bond->End()->element == "C") ) {
	AddPeak(1300.0, QString("C-N"), QString(tr("~1350-1000, C-N")));
      }      
    }
    if (tmp_bond->Order() == 2) {
      if ( (tmp_bond->Start()->element == "C") &&
	   (tmp_bond->End()->element == "O") ) {  // ketone
	AddPeak(1700.0, QString("C=O"), QString(tr("~1700 (narrow), ketone")));
      }
      if ( (tmp_bond->Start()->element == "O") &&
	   (tmp_bond->End()->element == "C") ) {  // ketone
	AddPeak(1700.0, QString("C=O"), QString(tr("~1700 (narrow), ketone")));
      }
      if ( (tmp_bond->Start()->element == "C") &&
	   (tmp_bond->End()->element == "C") ) {  // carbon double bond
	AddPeak(1700.0, QString("C=C"), QString(tr("~1660-1600, C=C (cis/vinyl strong; trans weak)")));
      }
    }
    if (tmp_bond->Order() == 3) {
      if ( (tmp_bond->Start()->element == "C") &&
	   (tmp_bond->End()->element == "N") ) {  // nitrile
	AddPeak(2250.0, QString("CN"), QString(tr("~2250 (narrow), nitrile")));
      }
      if ( (tmp_bond->Start()->element == "N") &&
	   (tmp_bond->End()->element == "C") ) {  // nitrile
	AddPeak(2250.0, QString("CN"), QString(tr("~2250 (narrow), nitrile")));
      }
    }
    #########################################################################
    */
  }
  // iterate thru unique atoms, look for functional groups
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next()) {
    cout << "|" << tmp_pt->element << "|" << endl;
    if ( (tmp_pt->element == "C") && (tmp_pt->substituents < 4) )
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if (tmp_pt->element == "CH")
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if (tmp_pt->element == "HC")
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if (tmp_pt->element == "CH2")
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if (tmp_pt->element == "H2C")
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if (tmp_pt->element == "CH3")
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if (tmp_pt->element == "H3C")
      AddPeak(3000.0, QString("CH"), QString(tr("~3000 (broad), C-H")));
    if ( (tmp_pt->element == "N") && (tmp_pt->substituents == 1) )
      AddPeak(3350.0, QString("NH"), QString(tr("two peaks: ~3400, ~3300, primary N-H")));
    if ( (tmp_pt->element == "N") && (tmp_pt->substituents == 2) )
      AddPeak(3300.0, QString("NH"), QString(tr("~3300 (broad), secondary N-H")));
    if (tmp_pt->element == "NH")
      AddPeak(3300.0, QString("NH"), QString(tr("~3300 (broad), secondary N-H")));
    if (tmp_pt->element == "HN")
      AddPeak(3300.0, QString("NH"), QString(tr("~3300 (broad), secondary N-H")));
    if (tmp_pt->element == "NH2")
      AddPeak(3350.0, QString("NH"), QString(tr("two peaks: ~3400, ~3300, primary N-H")));
    if (tmp_pt->element == "H2N")
      AddPeak(3350.0, QString("NH"), QString(tr("two peaks: ~3400, ~3300, primary N-H")));
    if ( (tmp_pt->element == "S") && (tmp_pt->substituents < 2) )
      AddPeak(2550.0, QString("SH"), QString(tr("~2550 (broad), S-H")));
    if ( (tmp_pt->element == "SH") && (tmp_pt->substituents < 2) )
      AddPeak(2550.0, QString("SH"), QString(tr("~2550 (broad), S-H")));
    if ( (tmp_pt->element == "HS") && (tmp_pt->substituents < 2) )
      AddPeak(2550.0, QString("SH"), QString(tr("~2550 (broad), S-H")));
    if ( (tmp_pt->element == "O") && (tmp_pt->substituents < 2) )
      AddPeak(2550.0, QString("OH"), QString(tr("~3400 (broad), O-H")));
    if (tmp_pt->element == "CN")
      AddPeak(2250.0, QString("CN"), QString(tr("~2250 (narrow), nitrile")));
    if (tmp_pt->element == "NC")
      AddPeak(2250.0, QString("CN"), QString(tr("~2250 (narrow), nitrile")));
    if (tmp_pt->element == "NCO")
      AddPeak(2270.0, QString("NCO"), QString(tr("~2270 (narrow), -N=C=O")));
    if (tmp_pt->element == "NCS")
      AddPeak(2125.0, QString("NCS"), QString(tr("~2125 (narrow), -N=C=S")));
    if (tmp_pt->element == "OH")
      AddPeak(3400.0, QString("OH"), QString(tr("~3400 (broad), O-H")));
    if (tmp_pt->element == "HO")
      AddPeak(3400.0, QString("OH"), QString(tr("~3400 (broad), O-H")));
    if (tmp_pt->element == "NO2")
      AddPeak(1525.0, QString("NO2"), QString(tr("~1525 (narrow), -NO2")));
    if (tmp_pt->element == "O2N")
      AddPeak(1525.0, QString("NO2"), QString(tr("~1525 (narrow), -NO2")));
    if (tmp_pt->aromatic == true) {
      AddPeak(1600.0, QString("aromatic"), QString(tr("~1600 (narrow), aromatic ring C=C")));
      AddPeak(1475.0, QString("aromatic"), QString(tr("~1475 (narrow), aromatic ring C=C")));
    }
  }

  QPixmap mol = r->MakeFullPixmap();
  QRect rr1 = BoundingBoxAll();
  rr1.setLeft(rr1.left() - 4);
  rr1.setTop(rr1.top() - 4);
  rr1.setBottom(rr1.bottom() + 4);
  rr1.setRight(rr1.right() + 4);
  QPixmap mol1(rr1.size());
  bitBlt(&mol1, QPoint(0,0), &mol, rr1);

  g->AddPixmap(mol1);
  //g->show();
}

// invoked from chemdata_tools.cpp
// derived (well, butchered) from Lin S-T and Sandler SI, J. Phys. Chem. A ,
// 2000, 104(30), 7099-7105
double Molecule::CalcKOW() {
  // get list of unique points
  up = AllPoints();
  MakeSSSR();
  AllNeighbors();
  double ri = 0.0, qi = 0.0, dg = 0.0, fkow;

  DPoint *alt_pt1, *alt_pt2, *alt_pt3;
  int testcount;

  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next()) {
    if (tmp_pt->hit) continue;  // skip atoms already considered
    if (tmp_pt->baseElement() == "H") continue;
    
    /*
    std::cout << "KOW parameters:" << std::endl;
    std::cout << tmp_pt->baseElement() << std::endl;
    alt_pt1 = tmp_pt->neighbors.at(0);
    std::cout << alt_pt1->baseElement() << std::endl;
    std::cout << tmp_pt->bondorder[0] << std::endl;
    if (tmp_pt->neighbors.count() > 1) {
      alt_pt2 = tmp_pt->neighbors.at(1);
      std::cout << alt_pt2->baseElement() << std::endl;
      std::cout << tmp_pt->bondorder[1] << std::endl;
    }
    if (tmp_pt->neighbors.count() > 2) {
      alt_pt3 = tmp_pt->neighbors.at(2);
      std::cout << alt_pt3->baseElement() << std::endl;
      std::cout << tmp_pt->bondorder[2] << std::endl;
    }
    */

    if (tmp_pt->baseElement() == "Br") {
      ri += 1.114; qi += 0.935; dg += 1.268;
    }
    if (tmp_pt->baseElement() == "Cl") {
      ri += 0.861; qi += 0.771; dg += 1.129;
    }
    if (tmp_pt->baseElement() == "F") {
      ri += 0.376; qi += 0.458; dg += 0.946;
    }
    if ( (tmp_pt->baseElement() == "C") && 
	 (tmp_pt->neighbors.count() == 1) ) {
      ri += 0.887; qi += 0.840; dg += 2.201;
    }
    if ( (tmp_pt->baseElement() == "C") && 
	 (tmp_pt->neighbors.count() == 2) ) {
      // check first for aldehyde/nitrile
      alt_pt1 = tmp_pt->neighbors.at(0);
      alt_pt2 = tmp_pt->neighbors.at(1);
      if ( (alt_pt1->baseElement() == "N") &&
	   (tmp_pt->bondorder[0] == 3) ) {
	std::cout << "Nitrile" << std::endl;
	continue;
      }
      if ( (alt_pt1->baseElement() == "O") &&
	   (tmp_pt->bondorder[0] == 2) ) {
	std::cout << "Aldehyde" << std::endl;
	continue;
      }
      if ( (alt_pt2->baseElement() == "N") &&
	   (tmp_pt->bondorder[1] == 3) ) {
	std::cout << "Nitrile" << std::endl;
	continue;
      }
      if ( (alt_pt2->baseElement() == "O") &&
	   (tmp_pt->bondorder[1] == 2) ) {
	std::cout << "Aldehyde" << std::endl;
	continue;
      }
      if (tmp_pt->aromatic) {
	ri += 0.537; qi += 0.431; dg += 0.468;
      } else {
	if (tmp_pt->inring) {
	  ri += 0.665; qi += 0.523; dg += 1.071;
	} else {
	  ri += 0.665; qi += 0.523; dg += 1.077;
	}
      }
    }
    if ( (tmp_pt->baseElement() == "C") && 
	 (tmp_pt->neighbors.count() == 3) ) {
      // check first for ester/ketone
      alt_pt1 = tmp_pt->neighbors.at(0);
      alt_pt2 = tmp_pt->neighbors.at(1);
      alt_pt3 = tmp_pt->neighbors.at(2);
      int ocount = 0, odb = 0;
      if (alt_pt1->baseElement() == "O") { 
	ocount++;
	if (tmp_pt->bondorder[0] == 2) odb = 1;
      }
      if (alt_pt2->baseElement() == "O") { 
	ocount++;
	if (tmp_pt->bondorder[1] == 2) odb = 1;
      }
      if (alt_pt3->baseElement() == "O") { 
	ocount++;
	if (tmp_pt->bondorder[2] == 2) odb = 1;
      }
      if ( (ocount == 1) && (odb == 1) ) {
	// ketone
	ri += 0.716; qi += 0.527; dg += -3.315;
	continue;
      }
      if ( (ocount == 2) && (odb == 1) ) {
	// ester
	ri += 1.062; qi += 0.821; dg += -0.723;
	continue;
      }
      if (tmp_pt->aromatic) {
	ri += 0.316; qi += 0.114; dg += 0.031;
      } else {
	if (tmp_pt->inring) {
	  ri += 0.497; qi += 0.235; dg += -0.051;
	} else {
	  ri += 0.497; qi += 0.235; dg += -0.091;
	}
      }
    }
    if ( (tmp_pt->baseElement() == "C") && 
	 (tmp_pt->neighbors.count() == 4) ) {
      ri += 0.213; qi += 0.0; dg += -0.729;
    }
    if ( (tmp_pt->baseElement() == "O") && 
	 (tmp_pt->neighbors.count() == 1) ) {
      ri += 0.532; qi += 0.572; dg += -10.405;
    }
    if ( (tmp_pt->baseElement() == "O") && 
	 (tmp_pt->neighbors.count() == 2) ) {
      alt_pt1 = tmp_pt->neighbors.at(0);
      alt_pt2 = tmp_pt->neighbors.at(1);
      if ( (alt_pt1->baseElement() == "C") &&
	   (alt_pt2->baseElement() == "C") )
	ri += 0.742; qi += 0.748; dg += -9.255;
    }
    if ( (tmp_pt->baseElement() == "N") && 
	 (tmp_pt->neighbors.count() == 1) ) {
      ri += 0.742; qi += 0.748; dg += -9.255;
    }
    if ( (tmp_pt->baseElement() == "N") && 
	 (tmp_pt->neighbors.count() == 2) ) {
      if ( (tmp_pt->bondorder[0] + tmp_pt->bondorder[1]) == 2 )
	ri += 0.553; qi += 0.401; dg += -4.959;
      if ( (tmp_pt->bondorder[0] + tmp_pt->bondorder[1]) == 3 )
	ri += 0.436; qi += 0.339; dg += -1.301;
    }
    if ( (tmp_pt->baseElement() == "N") && 
	 (tmp_pt->neighbors.count() == 3) ) {
      ri += 0.341; qi += 0.0; dg += -5.525;
    }
  }

  fkow = -0.126 + 1.031 * ri - 1.208 * qi + dg / 1.364;

  return fkow;
}

// invoked from chemdata_tools.cpp
void Molecule::CalcpKa() {
  // pKa's from -1.2 (CH3SO3H) to 18 (t-butyl alcohol) are considered
  // beyond that, (e.g, HCl, r-CH3), the user will probably not be interested

  QStringList pKa_table;
  // why reinvent the wheel?  Use HOSE codes found by 13C-NMR to predict
  // environment
  Calc13CNMR(false);
  // restore neighbors list(s)
  AllNeighbors();
  // get list of unique points
  up = AllPoints();

  DPoint *alt_pt1, *alt_pt2, *alt_pt3;
  int testcount;

  // first thing to do is scan for large patterns (e.g. guanidino, rings)
  for (tmp_pt = up.first(); tmp_pt != 0; tmp_pt = up.next() ) {
    if (tmp_pt->hit) continue;  // skip atoms already considered
    if (tmp_pt->baseElement() == "C") {
      if (tmp_pt->neighbors.count() == 3) {
	// check for guanidino
	alt_pt1 = tmp_pt->neighbors.at(0);
	alt_pt2 = tmp_pt->neighbors.at(1);
	alt_pt3 = tmp_pt->neighbors.at(2);
	testcount = alt_pt1->neighbors.count() + alt_pt2->neighbors.count() + 
	  alt_pt3->neighbors.count();
	if ( (alt_pt1->baseElement() == "N") &&
	     (alt_pt2->baseElement() == "N") &&
	     (alt_pt3->baseElement() == "N") &&
	     (testcount == 4) ) {
	  alt_pt1->hit = true;
	  alt_pt2->hit = true;
	  alt_pt3->hit = true;
	  pKa_table.append( tr("10.5 (Guanidine)") );
	}
      }
    }
  }

  for (tmp_pt = up.first(); tmp_pt != 0; tmp_pt = up.next() ) {
    //std::cout << tmp_pt->serial << "," << tmp_pt->baseElement() << std::endl;
    if (tmp_pt->hit) continue;  // skip atoms already considered
    if (tmp_pt->baseElement() == "C") {
      if (tmp_pt->neighbors.count() == 3) {

      }
      // few cases where C-H proton is going to split!
      if ( (tmp_pt->hosecode == "C$C$(//)") && 
	   (tmp_pt->substituents == 2) ) {
	alt_pt1 = tmp_pt->neighbors.at(0);
	alt_pt2 = tmp_pt->neighbors.at(1);
	if ( (alt_pt1->hosecode == "=OCC") &&
	     (alt_pt1->hosecode == "=OCC") ) {
	  pKa_table.append( tr("9 (a-carbon between two ketones)") );	  
	}
	if ( (alt_pt1->hosecode == "=OCO") &&
	     (alt_pt1->hosecode == "=OCC") ) {
	  pKa_table.append( tr("11 (a-carbon between two ketones)") );	  
	}
	if ( (alt_pt1->hosecode == "=OCC") &&
	     (alt_pt1->hosecode == "=OCO") ) {
	  pKa_table.append( tr("11 (a-carbon between ketone and ester)") );	  
	}
	if ( (alt_pt1->hosecode == "=OCO") &&
	     (alt_pt1->hosecode == "=OCO") ) {
	  pKa_table.append( tr("13 (a-carbon between two esters)") );	  
	}
      }
    }
    if (tmp_pt->baseElement() == "N") {
      std::cout << tmp_pt->substituents << std::endl;
      if (tmp_pt->substituents == 1) {
	alt_pt1 = tmp_pt->neighbors.at(0);  // MakeSSSR() should set this
	if (alt_pt1->hosecode.contains("*") > 0) {
	  pKa_table.append( tr("5 (Aromatic primary amine)") );
	} else {
	  if (alt_pt1->hosecode.contains("=") > 0) {
	    if (alt_pt1->hosecode.contains("=O") > 0) {
	      // amide!
	    } else {
	      pKa_table.append( tr("9-10 (Alkene-primary amine)") );
	    }
	  } else {
	    pKa_table.append( tr("11 (Aliphatic primary amine)") );
	  }
	}
      }
      if (tmp_pt->substituents == 2) {  // check secondary amine
	alt_pt1 = tmp_pt->neighbors.at(0);
	alt_pt2 = tmp_pt->neighbors.at(1);
	//std::cout << "alt_pt1 = " << alt_pt1->hosecode << std::endl;
	//std::cout << "alt_pt2 = " << alt_pt2->hosecode << std::endl;
	if ( (alt_pt1->hosecode == "=CN(//)") &&
	     (alt_pt2->hosecode == "=CN(//)") ) {
	  // pyrrole
	  pKa_table.append( tr("-4 (pyrrole)") );
	  continue;
	}
	if ( (alt_pt1->hosecode == "=CN(//)") &&
	     (alt_pt2->hosecode == "=NN(//)") ) {
	  pKa_table.append( tr("14.4 (secondary amine)") );
	  continue;
	}
	if ( (alt_pt1->hosecode.contains("=O") == 0 ) &&
	     (alt_pt2->hosecode.contains("=O") == 0 ) ) {
	  pKa_table.append( tr("11 (aliphatic secondary amine)") );
	}
      }
      if (tmp_pt->substituents == 3) {  // check conjugated and tertiary amines
	if (tmp_pt->neighbors.count() == 2) {
	  // conjugated/aromatic amine
	  alt_pt1 = tmp_pt->neighbors.at(0);
	  alt_pt2 = tmp_pt->neighbors.at(1);
	  std::cout << "alt_pt1 = " << alt_pt1->hosecode << std::endl;
	  std::cout << "alt_pt2 = " << alt_pt2->hosecode << std::endl;
	  if ( (alt_pt1->hosecode == "=CN(//)") &&
	       (alt_pt2->hosecode == "=NN(//)") ) {
	    pKa_table.append( tr("6.8 (conjugated secondary amine)") );
	  }
	  if ( (alt_pt1->hosecode.contains("*C*N(//)") == 1 ) &&
	       (alt_pt2->hosecode.contains("*C*N(//)") == 1 ) ) {
	    pKa_table.append( tr("5 (aromatic secondary amine)") );
	  }
	  if ( (alt_pt1->hosecode.contains("*N*N(//)") == 1 ) &&
	       (alt_pt2->hosecode.contains("*C*N(//)") == 1 ) ) {
	    pKa_table.append( tr("1-3 (purine/pyrimidine)") );
	  }
	  if ( (alt_pt1->hosecode.contains("=NC(//)") == 1 ) &&
	       (alt_pt2->hosecode.contains("CN(//)") == 1 ) ) {
	    pKa_table.append( tr("1-3 (purine/pyrimidine)") );
	  }
	}
	if (tmp_pt->neighbors.count() == 3) {
	  // tertiary amine
	  alt_pt1 = tmp_pt->neighbors.at(0);
	  alt_pt2 = tmp_pt->neighbors.at(1);
	  alt_pt3 = tmp_pt->neighbors.at(2);
	  if ( (alt_pt1->hosecode.contains("*") > 0 ) ||
	       (alt_pt2->hosecode.contains("*") > 0 ) ||
	       (alt_pt3->hosecode.contains("*") > 0 ) ) {
	    pKa_table.append( tr("4 (tertiary amine)") );
	    continue;
	  }
	  if ( (alt_pt1->hosecode.contains("=O") == 0 ) &&
	       (alt_pt2->hosecode.contains("=O") == 0 ) &&
	       (alt_pt3->hosecode.contains("=O") == 0 ) ) {
	    pKa_table.append( tr("10 (aliphatic tertiary amine)") );
	  }
	}
      }
    }
    if (tmp_pt->baseElement() == "O") {
      //std::cout << "base element: O" << std::endl;
      if (tmp_pt->substituents == 1) {
	alt_pt1 = tmp_pt->neighbors.at(0);  // MakeSSSR() should set this
	if (alt_pt1->baseElement() == "O") {
	  pKa_table.append( tr("8 (Peroxy acid)") );
	}
	if (alt_pt1->baseElement() == "S") {
	  // sulfonic acid?
	  if (alt_pt1->substituents >= 4) {
	    int c_count = 0, o_count = 0;
	    for (alt_pt2 = alt_pt1->neighbors.first(); alt_pt2 != 0;
		 alt_pt2 = alt_pt1->neighbors.next() ) {
	      //std::cout << "-" << alt_pt2->baseElement() << std::endl;
	      if (alt_pt2->baseElement() == "O") {
		alt_pt2->hit = true; o_count++; 
	      }
	      if (alt_pt2->baseElement() == "O") {
		c_count++; 
	      }
	    }
	    if ( (c_count == 1) && (o_count == 3) )
	      pKa_table.append( tr("-1 (sulfonic acid)") );
	  }
	}
	if (alt_pt1->baseElement() == "C") {
	  if (alt_pt1->hosecode == "=OCO(//)") { // R-COOH
	    //std::cout << "carboxylic acid " << alt_pt1->neighbors.count() << std::endl;
	    for (alt_pt2 = alt_pt1->neighbors.first(); alt_pt2 != 0;
		 alt_pt2 = alt_pt1->neighbors.next() ) {
	      //std::cout << "-" << alt_pt2->baseElement() << std::endl;
	      if (alt_pt2->baseElement() == "O") {
		alt_pt2->hit = true; 
	      } else {
		//std::cout << "--" << alt_pt2->hosecode << std::endl;
		if (alt_pt2->hosecode.contains("*") > 0) {
		  pKa_table.append( tr("4 (Aromatic carboxylic acid)") );
		} else {
		  int halide_count = alt_pt2->hosecode.contains("F") + alt_pt2->hosecode.contains("X") + alt_pt2->hosecode.contains("Y") + alt_pt2->hosecode.contains("I");
		  if (halide_count == 0)
		    pKa_table.append( tr("4.5 (Aliphatic carboxylic acid)") );
		  if (halide_count == 1)
		    pKa_table.append( tr("3 (Aliphatic carboxylic acid, a-halide)") );
		  if (halide_count == 2)
		    pKa_table.append( tr("1.3 (Aliphatic carboxylic acid, 2 a-halide)") );
		}
	      }
	    }
	    continue;
	  }
	  if (alt_pt1->hosecode.contains("*") > 0) {
	    pKa_table.append( tr("10 (Aromatic -OH)") );
	  } else {
	    pKa_table.append( tr("16 (Aliphatic -OH)") );
	  }
	}
      }
    }
    if (tmp_pt->baseElement() == "S") {
      // R-SH, Ar-SH (R-SO3H = consider under "O")
      if (tmp_pt->substituents == 1) {
	alt_pt1 = tmp_pt->neighbors.at(0);  // MakeSSSR() should set this
	if (alt_pt1->hosecode.contains("*") > 0) {
	  pKa_table.append( tr("7.8 (Aromatic -SH)") );
	} else {
	  pKa_table.append( tr("10.6 (Aliphatic -SH)") );
	}
      }
    }
  }

  QString finalList;
  finalList.append( tr("Estimated pKa's:") );
  for ( QStringList::Iterator it = pKa_table.begin(); 
	it != pKa_table.end(); ++it ) {
    std::cout << *it << std::endl;
    finalList.append("\n");
    finalList.append(*it);
  }

  if (pKa_table.count() < 1) {
    finalList.append( "\n" );
    finalList.append( tr("There seem to be no sites with pKa between -1 and 15.") );
  }

  QMessageBox::information( r, tr("Estimated pKa's"),
			    finalList );
}

