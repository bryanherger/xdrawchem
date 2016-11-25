#include <iostream>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qtextstream.h>
#include <qfile.h>

using std::cout;
using std::endl;

#include "chemdata.h"
#include "dpoint.h"
#include "arrow.h"
#include "bracket.h"
#include "text.h"
#include "defs.h"
#include "cml.h"

using std::cout;

// handle CDXML format (used by ChemDraw, TM Cambridge Software)
// assume Cambridge Software CDXML.DTD
bool ChemData::LoadCDXMLFile(QString wholefile, QString doctype) {
  cout << "CDXML" << endl;
  int num_pages = 0;
  int currentpage;
  int currentfragment;
  int nodedepth = 0;
  QString line;
  QString tmpline, tfont, tdata, a1, v1, textpos, textstr, textid;
  QString graphic_id, graphictype, arrowtype, boundingbox;
  QString nodetype;
  DPoint *s1, *e1;
  QString thistag, nexttag;
  QStringList tokens, refs;
  int d1, ptr = 0;
  int *st1;
  double l1, l2, l3, l4;

  do {
    thistag = ReadTag(wholefile, ptr);
    // ReadTag returns a null at EOF (ideally).
    if (thistag.isNull()) break;
    // Look for beginning of document
    if (thistag.find(QString("<") + doctype) >= 0) {
      cout << "<CDXML>" << endl;
      // push this tag on stack
      st1 = new int;
      *st1 = CML_MOLECULE;
      TagStack.append(st1);
      // parse attributes in tag
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("NAME")) MoleculeId = v1;
	//	if (a1.upper() == QString("TITLE")) MoleculeTitle = v1;
      }
    }
    // <colortable>, <fonttable> and contained objects get ignored
    // (for now - may implement them later)
    //
    // handle <page ...>
    if (thistag.find(QString("<page")) >= 0) {
      num_pages++;
      st1 = new int;
      *st1 = CDXML_PAGE;
      TagStack.append(st1);
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) currentpage = v1.toUInt();
      }
    }
    // handle </page>
    if (thistag == QString("</page>")) {
      st1 = TagStack.last();
      if (*st1 == CDXML_PAGE) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <page> tags!" << endl;
      }
    }    
    // handle <fragment>
    if (thistag.find(QString("<fragment")) >= 0) {
      st1 = new int;
      *st1 = CDXML_FRAGMENT;
      TagStack.append(st1);
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) currentfragment = v1.toUInt();
      }
    }
    // handle </fragment>
    if (thistag == QString("</fragment>")) {
      st1 = TagStack.last();
      if (*st1 == CDXML_FRAGMENT) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <fragment> tags!" << endl;
      }
    }
    // handle <graphic> and <graphic/>
    if (thistag.find(QString("<graphic")) >= 0) {
      cout << thistag << endl;
      st1 = new int;
      *st1 = CDXML_GRAPHIC;
      TagStack.append(st1);
      tokens = ReadAttr(thistag);
      tmp_bond = new CML_Bond;
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) graphic_id = v1;
	if (a1.upper() == QString("BOUNDINGBOX")) boundingbox = v1;
	if (a1.upper() == QString("GRAPHICTYPE")) graphictype = v1;
	if (a1.upper() == QString("ARROWTYPE")) arrowtype = v1;
	if (a1.upper() == QString("BRACKETTYPE")) arrowtype = v1;
      }
      if (SelfContainedTag(thistag)) {
	TagStack.removeLast();
	cout << "ID = " << graphic_id << endl;
	cout << "BBox = " << boundingbox << endl;
	cout << "GType = " << graphictype << endl;
	cout << "AType = " << arrowtype << endl;
	ParseBoundingBox(boundingbox, l1, l2, l3, l4);
	cout << l1 << " " << l2 << " ";
	cout << l3 << " " << l4 << endl;
	if ( (graphictype.upper() == QString("LINE")) &&
	     (arrowtype.upper() == QString("FULLHEAD")) ) {
	  // create end points of this arrow
	  s1 = new DPoint(l1, l2);
	  e1 = new DPoint(l3, l4);
	  Arrow *dr1 = new Arrow(r);
	  dr1->setPoints(s1, e1);
	  dr1->Highlight(true);
	  CDXML_Objects.append(dr1);
	}
	if ( (graphictype.upper() == QString("BRACKET")) &&
	     (arrowtype.upper() == QString("SQUAREPAIR")) ) {
	  // create corner points of this bracket
	  cout << "bracket" << endl;
	  s1 = new DPoint(l1, l2);
	  e1 = new DPoint(l3, l4);
	  Bracket *dr1 = new Bracket(r);
	  dr1->setPoints(s1, e1);
	  dr1->Highlight(true);
	  CDXML_Objects.append(dr1);
	}
      }
    }
    // handle </graphic>
    if (thistag == QString("</graphic>")) {
      cout << thistag << endl;
      st1 = TagStack.last();
      if (*st1 == CDXML_GRAPHIC) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <graphic> tags!" << endl;
      }
    }
    // handle <s> (string)
    if (thistag.find(QString("<s")) >= 0) {
      textstr.append(ReadData(wholefile, ptr));
      nexttag = ReadTag(wholefile, ptr);      
      if (nexttag != QString("</s>"))
	cout << "Imbalanced <s> tags!" << endl;
    }
    // handle <t> (text box)
    if (thistag.find(QString("<t")) >= 0) {
      st1 = new int;
      *st1 = CDXML_TEXT;
      TagStack.append(st1);
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) textid = v1;
	if (a1.upper() == QString("P")) textpos = v1;
      }
    }
    // handle </t>
    if (thistag == QString("</t>")) {
      st1 = TagStack.last();
      if (*st1 == CDXML_TEXT) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <t> tags!" << endl;
      }
      if ( (tmp_atom != NULL) && (nodedepth > 0) )
	tmp_atom->element = textstr;
      if (nodedepth == 0) {
	cout << "TEXT:" << textid << "|" << textpos << "|" << textstr << endl;
	tmp_atom = new CML_Atom;
	tmp_atom->atom = false;
	tmp_atom->element = textstr;
	d1 = textpos.find(QString(" "));
	tmp_atom->x = textpos.mid(0, d1).toDouble();
	tmp_atom->y = textpos.mid(d1).toDouble();
	CML_Atoms.append(tmp_atom);
      }
      textstr = QString("");
    }
    // handle node <n>
    if (thistag.find(QString("<n")) >= 0) {
      st1 = new int;
      *st1 = CDXML_NODE;
      TagStack.append(st1);
      nodedepth++;
      tmp_atom = new CML_Atom;
      tokens = ReadAttr(thistag);
      nodetype = QString("");
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) tmp_atom->id = v1;
	if (a1.upper() == QString("NODETYPE")) nodetype = v1;
	if (a1.upper() == QString("P")) {
	  d1 = v1.find(QString(" "));
	  tmp_atom->x = v1.mid(0, d1).toDouble();
	  tmp_atom->y = v1.mid(d1 + 1).toDouble();
	}
	if (a1.upper() == QString("ELEMENT")) {
	  if (v1 == QString("1")) tmp_atom->element = QString("H");
	  if (v1 == QString("5")) tmp_atom->element = QString("B");
	  if (v1 == QString("7")) tmp_atom->element = QString("N");
	  if (v1 == QString("8")) tmp_atom->element = QString("O");
	  if (v1 == QString("9")) tmp_atom->element = QString("F");
	  if (v1 == QString("11")) tmp_atom->element = QString("Na");
	  if (v1 == QString("15")) tmp_atom->element = QString("P");
	  if (v1 == QString("16")) tmp_atom->element = QString("S");
	  if (v1 == QString("17")) tmp_atom->element = QString("Cl");
	  if (v1 == QString("19")) tmp_atom->element = QString("K");
	  if (v1 == QString("35")) tmp_atom->element = QString("Br");
	  if (v1 == QString("53")) tmp_atom->element = QString("I");
	}
      }
      if (SelfContainedTag(thistag)) {
	TagStack.removeLast();
	nodedepth--;
	if (nodedepth == 0)  // onl add if not nested
	  CML_Atoms.append(tmp_atom);
      }
    }
    // handle </node> which is really </n>
    if (thistag == QString("</n>")) {
      st1 = TagStack.last();
      if (*st1 == CDXML_NODE) {
	TagStack.removeLast();
	nodedepth--;
      } else {
	cout << "Imbalanced </n> (node) tags!" << endl;
      }
      if (nodedepth == 0)  // only add if not nested
	CML_Atoms.append(tmp_atom);
    }
    // handle bonds <b>...</b> or <b/>
    if (thistag.find(QString("<b")) >= 0) {
      cout << thistag << endl;
      st1 = new int;
      *st1 = CDXML_BOND;
      TagStack.append(st1);
      tmp_bond = new CML_Bond;
      tmp_bond->a1 = "";
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) tmp_bond->id = v1;
	if (a1.upper() == QString("B")) tmp_bond->a1 = v1;
	if (a1.upper() == QString("E")) tmp_bond->a2 = v1;
	if (a1.upper() == QString("ORDER")) tmp_bond->order = v1.toUInt();
	if (a1.upper() == QString("DISPLAY")) {
	  if (v1 == QString("WedgeBegin")) tmp_bond->order = 5;
	  if (v1 == QString("WedgedHashBegin")) tmp_bond->order = 7;
	}
      }
      if (SelfContainedTag(thistag)) {
	TagStack.removeLast();
	if (nodedepth == 0)  // only add if not in a node
	  CML_Bonds.append(tmp_bond);
      }      
    }
    // handle </bond>
    if (thistag == QString("</b>")) {
      st1 = TagStack.last();
      if (*st1 == CDXML_BOND) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <bond> tags!" << endl;
      }
      if (nodedepth == 0)  // only add if not in a node
	CML_Bonds.append(tmp_bond);
    }
    // Look for end of document
    if (thistag.find(QString("</") + doctype) >= 0) {
      cout << "</CDXML>" << endl;
      st1 = TagStack.last();
      if (*st1 == CML_MOLECULE) {
	TagStack.removeLast();
	// should be last tag...
	if (!TagStack.isEmpty()) cout << "Imbalanced tags!" << endl;
	continue;
      }
    }
  } while (ptr < wholefile.length());

  cout << CML_Atoms.count() << " atoms" << endl;
  cout << CML_Bonds.count() << " bonds" << endl;

  // convert CML_Atom and CML_Bond to DPoint and customObject, respectively
  Convert_CML_Lists_To_Native();

  return false;
}

/*
/ Save CDXML file (fn = filename)
bool ChemData::SaveCDXML(QString fn) {
  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  Text *tmp_text;
  Bond *tmp_bond;
  QString n1, nfull;
  int n = 0;

  up = UniquePoints();
  uo = UniqueObjects();

  if (up.count() == 0) return false;  // don't write an empty file

  QFile f(fn);
  if (!f.open(IO_WriteOnly)) {
    return false;
  }  

  QTextStream t(&f);
  // introductory XML stuff -- declare DTD, start molecule
  t << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl;
  t << "<!DOCTYPE molecule SYSTEM \"cml.dtd\" []>" << endl;
  t << "</molecule>" << endl;

  f.close();
  return true;
}
*/

