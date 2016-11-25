#include <iostream>
#include <qobject.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qmessagebox.h>
#include <qregexp.h>

using std::cout;
using std::endl;

#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "bracket.h"
#include "text.h"
#include "chemdata.h"
#include "defs.h"

// local class for parsing
class RetroTreeNode {
public:
  QString thisnode;
  int depth;
  RetroTreeNode *child1, *child2, *child3;
};

void ChemData::Retro(Molecule *m1) {
  // does anything actually call this function?
}

QString Molecule::BuildReactionList(QString inbond) {
  QString rxns;
  rxns = tr("No reactions found");

  return rxns;
}

int Molecule::Retro() {
  // note that we appear to be doing this in Reactivity in molecule_tools_2.cpp
  QStringList rxnfile, rxnlist, patlist, resultlist;
  QString resultString;

  // read pattern and reaction lists from retro.txt
  QString fileName = RingDir + QString("retro.txt");
  QFile fin(fileName);
  
  if (fin.open(IO_ReadOnly) == false)
    return -1;
  
  QTextStream tin(&fin);
  QString line, thisbond, norbond, pat, rxn;
  int i1, i2;
  do {
    line = tin.readLine();
    rxnfile.append(line);
  } while (!tin.atEnd());
  fin.close();

  int cnt = 0;
  QStringList::Iterator it1, it2;
  for ( it1 = bondRxns.begin(); it1 != bondRxns.end(); ++it1 ) {
    thisbond = *it1;
    norbond = thisbond;
    norbond.remove("r");
    resultString = "";
    for ( it2 = rxnfile.begin(); it2 != rxnfile.end(); ++it2 ) {
      line = *it2;
      i1 = line.find(";;");
      pat = line.left(i1);
      rxn = line.mid(i1 + 2);
      if (RetroMatch(thisbond, pat)) {
	resultlist.append(rxn);
	resultString.append(rxn);
	resultString.append("\n");
      }
      if (thisbond.contains("r") > 0) {
	if (RetroMatch(norbond, pat)) {
	  resultlist.append(rxn);
	  resultString.append(rxn);
	  resultString.append("\n");
	}
      }
    }
    //std::cout << cnt << " " << resultString << std::endl;
    bonds.at(cnt)->setReactions(resultString);
    cnt++;
  }
    
  //std::cout << "Retro(): " << resultlist.count() << " reactions" << std::endl;
  resultString = "";

  for ( it1 = resultlist.begin(); it1 != resultlist.end(); ++it1 ) {
    resultString.append(*it1);
    resultString.append("\n");
    //std::cout << *it1 << std::endl;
  }

  resultString.prepend( tr("The following reactions could have resulted in this product:\n\n") );
  resultString.append( tr("\nTarget bonds will be highlighted in green.\nTo see reactions affecting a highlighted bond, right-click on the bond and select Info.\nTo clear highlights, select Undo from the Edit menu, or press Ctrl+Z.") );
  if (resultlist.count() > 0) {
    QMessageBox::information( 0, tr("Reverse reactions list"), resultString);
  } else {
    QMessageBox::information( 0, tr("Reverse reactions list"), 
			      tr("No reactions in the database could yield the selected molecule.") );
  }

  return resultlist.count();
}

QString Molecule::RetroAtomName(DPoint *latom) {
  // thought of the day:  would the HOSE code suffice here?  Probably yes...
  // this function is needed in case someday I decide not to use HOSE codes
  // for this purpose, though they work well for pKa :)

  QString rval;

  rval = latom->hosecode;

  return rval;
}

QString Molecule::RetroBondName(Bond *lbond, bool runsssr) {
  // hopefully only need to do this once per retro/reactivity operation!
  if (runsssr)
    MakeSSSR();

  // basic idea: iterate thru bonds, create something like a canonical
  // SMILES describing the bond and nearest neighbors, and compare to
  // database.  For certain reactions, do a second iteration over atoms
  // and bonds to rule out side products (e.g. Grignards won't work if
  // more than one ketone or -COOH)
  // DB would conatin fields like [SMILES],[name or reaction],[conditions],
  // [stoppers (like (C=O) in Grignard as noted)]
  // canonical strings will likely take the form:
  // <1st(2nd[3rd])(2nd)>0-0<1st><1st(2nd)(2nd[3rd][3rd])>
  // ...where canonicalization will be accomplished by alphabetizing
  // within each level.

  QPtrListIterator<Bond> innerIter1(bonds), innerIter2(bonds), innerIter3(bonds), 
    outerIter(bonds);
  QStringList innerstrlist1, innerstrlist2, innerstrlist3;
  Bond *local_tmp_bond, *inner_tmp_bond;
  DPoint *outer1, *outer2, *inner1, *inner2, *inner3;
  QString canonicalBond, leveltmpstr, outer1el, outer2el;

  canonicalBond.setLength(0);

  // in the long run, it might be wise to break out this code for
  // making canonical SMILES-type names into a separate function to
  // facilitate making the database

  local_tmp_bond = lbond;
  outer1 = local_tmp_bond->Start();
  outer2 = local_tmp_bond->End();
  if (QString::compare(outer1->element, outer2->element) > 0) {
    inner1 = outer1; outer1 = outer2; outer2 = inner1;
  }
  outer1el = outer1->baseElement();
  canonicalBond = outer1el.remove( QRegExp("\\-+H+\\d+") );
  // for some reason, this isn't reliable
  if (outer1->inring) canonicalBond.append("r");
    if (local_tmp_bond->aromatic()) {
      canonicalBond.append("*");
    } else {
  if (local_tmp_bond->Order() == 1) {
    canonicalBond.append("-");
  }
  if (local_tmp_bond->Order() == 5) {
    canonicalBond.append("-");
  }
  if (local_tmp_bond->Order() == 7) {
    canonicalBond.append("-");
  }
  if (local_tmp_bond->Order() == 2) {
    canonicalBond.append("=");
  }
  if (local_tmp_bond->Order() == 3) {
    canonicalBond.append("#");
  }
    }
  outer2el = outer2->baseElement();
  canonicalBond.append(outer2el.remove( QRegExp("\\-+H+\\d+") ));
  // for some reason, this isn't reliable
  if (outer2->inring) canonicalBond.append("r");

  canonicalBond.prepend(RetroTraverseBonds(outer2, outer1, 
					   local_tmp_bond, 3));
  canonicalBond.append(RetroTraverseBonds(outer1, outer2, 
					  local_tmp_bond, 3));

  //std::cout << "canonical bond name:" << canonicalBond << std::endl;

  // also test R-group subs
  if (runsssr) {
    bool rm1 = RetroMatch( canonicalBond, QString("<-R><-R><-R>C-O") );
    if (rm1)
      std::cout << "tertiary alcohol" << std::endl;
    else
      std::cout << "*NOT* a tertiary alcohol" << std::endl;
  }

  return canonicalBond;
}

QString Molecule::RetroTraverseBonds(DPoint *parentNode, DPoint *thisNode,
				     Bond *this_bond, int tlevel) {
  QPtrListIterator<Bond> innerIter1(bonds), innerIter2(bonds), innerIter(bonds), 
    outerIter(bonds);
  QStringList innerstrlist1, innerstrlist2, innerstrlist3;
  Bond *local_tmp_bond, *inner_tmp_bond;
  DPoint *outer1, *outer2, *inner1, *inner2, *inner3;
  QString canonicalBond, leveltmpstr, leftMarker, rightMarker, thislevelstr,
    outer1el, outer2el;
  if (tlevel == 3) { leftMarker = ""; rightMarker = ""; }
  if (tlevel == 2) { leftMarker = "<"; rightMarker = ">"; }
  if (tlevel == 1) { leftMarker = "("; rightMarker = ")"; }

  outer1 = thisNode;
  
  canonicalBond.setLength(0);
  if (tlevel == 0) { 
    leftMarker = "["; rightMarker = "]"; 
    if (thisNode->element == "H") {
      return canonicalBond;
    } else {
      if (this_bond->aromatic()) {
	canonicalBond.append("*");
      } else {
	if (this_bond->Order() == 1) {
	  canonicalBond.append("-");
	}
	if (this_bond->Order() == 5) {
	  canonicalBond.append("-");
	}
	if (this_bond->Order() == 7) {
	  canonicalBond.append("-");
	}
	if (this_bond->Order() == 2) {
	  canonicalBond.append("=");
	}
	if (this_bond->Order() == 3) {
	  canonicalBond.append("#");
	}
      }
      outer1el = thisNode->baseElement();
      canonicalBond.append(outer1el.remove( QRegExp("\\-+H+\\d+") ));
      // for some reason, this isn't reliable
      // don't care whether side groups are in ring, yet
      //if (thisNode->inring) canonicalBond.append("r");
      canonicalBond.prepend(leftMarker);
      canonicalBond.append(rightMarker);
      return canonicalBond;
    }
  }

  if (this_bond->aromatic()) {
    canonicalBond.append("*");
  } else {
    if (this_bond->Order() == 1) {
      canonicalBond.append("-");
    }
    if (this_bond->Order() == 5) {
      canonicalBond.append("-");
    }
    if (this_bond->Order() == 7) {
      canonicalBond.append("-");
    }
    if (this_bond->Order() == 2) {
      canonicalBond.append("=");
    }
    if (this_bond->Order() == 3) {
      canonicalBond.append("#");
    }
  }
  outer1el = thisNode->baseElement();
  canonicalBond.append(outer1el.remove( QRegExp("\\-+H+\\d+") ));
  // for some reason, this isn't reliable
  //if (thisNode->inring) canonicalBond.append("r");
  
  innerIter.toFirst();
  innerstrlist1.clear();

  while ( (inner_tmp_bond = innerIter.current()) != 0 ) {
    ++innerIter;
    if (inner_tmp_bond->Find(outer1) == true) {
      inner1 = inner_tmp_bond->otherPoint(outer1);
      if (inner1 != parentNode) {
	//std::cout << inner1->element << std::endl;
	if (inner1->element != "H") {
	  // traverse next level (level 0 handled elsewhere)
	  innerstrlist1.append(RetroTraverseBonds(outer1, inner1, 
						  inner_tmp_bond, tlevel - 1));
	}
      }
    }
  }
  innerstrlist1.sort();

  if (tlevel == 3) canonicalBond.setLength(0);
  for ( QStringList::Iterator it = innerstrlist1.begin(); 
	it != innerstrlist1.end(); ++it ) {
    canonicalBond.append(*it);
  }

  canonicalBond.prepend(leftMarker);
  canonicalBond.append(rightMarker);
  return canonicalBond;
}

// does thisbond match this R-substitution pattern?
bool Molecule::RetroMatch(QString inbond, QString inrpat) {
  //std::cout << "TEST:" << inbond << ":AGAINST:" << inrpat << std::endl;

  int i1, i2;
  QString top_bond, top_rpat;
  top_bond = inbond; top_rpat = inrpat;
  do {
    i1 = top_bond.find("<");
    i2 = top_bond.find(">") + 1;
    top_bond.remove(i1, i2 - i1);
  } while (top_bond.contains("<") > 0);
  do {
    i1 = top_rpat.find("<");
    i2 = top_rpat.find(">") + 1;
    top_rpat.remove(i1, i2 - i1);
  } while (top_rpat.contains("<") > 0);

  // bail if inbond isn't the same top-level type of bond as inrpat
  //std::cout << "tops:" << top_bond << " " << top_rpat << std::endl;
  if (top_bond != top_rpat) return false;

  // create left and right halves of input and pattern, pass to RetroTreeMatch
  i1 = inbond.find(top_bond);
  i2 = inrpat.find(top_rpat);
  QString inleft, inright, patleft, patright;
  inleft = inbond.left(i1);
  inright = inbond.mid(i1 + top_bond.length());
  patleft = inrpat.left(i2);
  patright = inrpat.mid(i2 + top_rpat.length());

  bool rt1;
  rt1 = RetroTreeMatch(inleft, inright, patleft, patright);
  if (rt1) return true;
  rt1 = RetroTreeMatch(inleft, inright, patleft, patright);
  if (rt1) return true;

  // build trees
  QValueList<QString> vbond, vrpat;
  int i3, l1, l2, l3, lmid;
  for (l1 = 0; l1 < 78; l1++) {
    vbond.append("-R"); vrpat.append("-X");
  }

  // parse inbond
  lmid = inbond.find(top_bond);
  i2 = 0; l1 = 0;
  bool midflag = true;
  while (1) {
    i1 = inbond.find("<", i2);
    if (i1 < 0) break;
    if ( (i1 > lmid) && midflag ) { l1 = 39; midflag = false; }
    i2 = inbond.find(">", i1);
    QString &vlstr = vbond[l1];
    vlstr = inbond.mid(i1 + 1, i2 - 1 - i1);
    l1 = l1 + 13;
  }

  QString thisstr, this3str;
  for (l1 = 0; l1 < 78; l1 = l1 + 13) {
    thisstr = vbond[l1];
    //std::cout << l1 << ":" << thisstr << std::endl;
    if (thisstr == "-R") continue;
    l2 = l1 + 1;
    i2 = 0;
    while (1) {
      i1 = thisstr.find("(", i2);
      if (i1 < 0) break;
      i2 = thisstr.find(")", i1);
      QString &vlstr = vbond[l2];
      vlstr = thisstr.mid(i1 + 1, i2 - 1 - i1);
      l3 = l2 + 1; i3 = 0;
      this3str = vlstr;
      //std::cout << "this3str:" << l3 << ":" << this3str << std::endl;
      while (1) {
	i1 = this3str.find("[", i3);
	if (i1 < 0) break;
	i3 = this3str.find("]", i1);
	QString &vlstr = vbond[l3];
	vlstr = this3str.mid(i1 + 1, i3 - i1 - 1);
	l3++;
      }
      l2 = l2 + 4;      
    }
  }

  // parse inrpat
  lmid = inrpat.find(top_rpat);
  i2 = 0; l1 = 0;
  midflag = true;
  while (1) {
    i1 = inrpat.find("<", i2);
    if (i1 < 0) break;
    if ( (i1 > lmid) && midflag ) { l1 = 39; midflag = false; }
    i2 = inrpat.find(">", i1);
    QString &vlstr = vrpat[l1];
    vlstr = inrpat.mid(i1 + 1, i2 - 1 - i1);
    l1 = l1 + 13;
  }

  for (l1 = 0; l1 < 78; l1 = l1 + 13) {
    thisstr = vrpat[l1];
    //std::cout << l1 << ":" << thisstr << std::endl;
    if (thisstr == "-R") continue;
    l2 = l1 + 1;
    i2 = 0;
    while (1) {
      i1 = thisstr.find("(", i2);
      if (i1 < 0) break;
      i2 = thisstr.find(")", i1);
      QString &vlstr = vrpat[l2];
      vlstr = thisstr.mid(i1 + 1, i2 - 1 - i1);
      l3 = l2 + 1; i3 = 0;
      this3str = vlstr;
      //std::cout << "this3str:" << l3 << ":" << this3str << std::endl;
      while (1) {
	i1 = this3str.find("[", i3);
	if (i1 < 0) break;
	i3 = this3str.find("]", i1);
	QString &vlstr = vrpat[l3];
	vlstr = this3str.mid(i1 + 1, i3 - i1 - 1);
	l3++;
      }
      l2 = l2 + 4;      
    }
  }

  // print and clean up
  for (l1 = 0; l1 < 78; l1++) {
    QString &vlstr = vbond[l1];
    i1 = vlstr.find("[");
    i2 = vlstr.findRev("]");
    if (i1 >= 0) vlstr.remove(i1, i2 - i1 + 1);
    i1 = vlstr.find("(");
    i2 = vlstr.findRev(")");
    if (i1 >= 0) vlstr.remove(i1, i2 - i1 + 1);
    //std::cout << l1 << ":" << vbond[l1] << "   ";
  }
  //std::cout << std::endl;
  for (l1 = 0; l1 < 78; l1++) {
    QString &vlstr = vrpat[l1];
    i1 = vlstr.find("[");
    i2 = vlstr.findRev("]");
    if (i1 >= 0) vlstr.remove(i1, i2 - i1 + 1);
    i1 = vlstr.find("(");
    i2 = vlstr.findRev(")");
    if (i1 >= 0) vlstr.remove(i1, i2 - i1 + 1);
    //std::cout << l1 << ":" << vrpat[l1] << "   ";
  }
  //std::cout << std::endl;

  // compare (old)
  for (l1 = 0; l1 < 78; l1++) {
    //std::cout << l1 << std::endl;
    if (vrpat[l1] == "-X") {
      // continue; doesn't matter what vbond is
      continue;
    }
    if (vrpat[l1] == "-R") {
      // vbond[l1] must be something other than -R
      if (vbond[l1] == "-R") 
	return false;
      else
	continue;
    }
    // vbond must equal vrpat
    if (vbond[l1] != vrpat[l1]) return false;
  }

  return true;
}

bool Molecule::RetroTreeMatch(QString inleft, QString inright, QString patleft,
			      QString patright) {
  std::cout << "l/r:" << inleft << ":" << inright << ":" << patleft << ":"
	    << patright << std::endl;
  return false;
}
