#include <iostream>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qdatetime.h>
#include <qxml.h>

using std::cout;
using std::endl;

#include "defs.h"
#include "chemdata.h"
#include "dpoint.h"
#include "text.h"
#include "cml.h"
#include "xml_cml.h"

bool ChemData::NewLoadCMLFile(QString fn) {
  CMLParser handler(r);
  QFile xmlFile( fn );
  if ( !xmlFile.open(IO_ReadOnly) ) return false;
  QTextStream t1(&xmlFile);
  QXmlInputSource source( t1 );
  QXmlSimpleReader reader;
  reader.setFeature("http://trolltech.com/xml/features/report-whitespace-only-CharData", false);
  reader.setContentHandler( &handler );
  reader.parse( source );

  cout << "done parsing" << endl;

  // add bonds and labels to main drawing
  double totlen = 0.0, dx = 99999.0, dy = 99999.0;
  int n = 0;
  int th1;
  std::cout << "thick_kludge: " << thick_kludge << std::endl;
  if (thick_kludge > 0) {
    th1 = thick_kludge;
  } else {
    th1 = 1;
  }
  QPtrList<Bond> parsedBonds;
  Bond *tbond;
  parsedBonds = handler.getBonds();
  for (tbond = parsedBonds.first(); tbond != 0; tbond = parsedBonds.next()) {
    addBond(tbond->Start(), tbond->End(), th1, tbond->Order(), QColor(0,0,0),
	    true);
    if (tbond->Start()->x < dx) dx = tbond->Start()->x;
    if (tbond->End()->x < dx) dx = tbond->End()->x;
    if (tbond->Start()->y < dy) dy = tbond->Start()->y;
    if (tbond->End()->y < dy) dy = tbond->End()->y;
    totlen += tbond->Start()->distanceTo(tbond->End());
    n++;
  }
  QPtrList<DPoint> parsedPoints;
  parsedPoints = handler.getPoints();
  QString makesub, ms2;
  for (tmp_pt = parsedPoints.first(); tmp_pt != 0; 
       tmp_pt = parsedPoints.next()) {
    if (tmp_pt->element != "C") {
      Text *nt = new Text(r);
      nt->setPoint(tmp_pt);
      nt->setJustify(JUSTIFY_CENTER);
      nt->Highlight(true);
      nt->setText(tmp_pt->element);
      makesub = tmp_pt->element;
      ms2 = "";
      for (int cc = 0; cc < makesub.length(); cc++) {
	if (makesub[cc].isLetter()) ms2.append(" ");
	if (makesub[cc].isNumber()) ms2.append("-");
	if (makesub[cc] == '+') ms2.append("+");
	if (makesub[cc] == '-') ms2.append("+");
      }
      nt->setTextMask(ms2);
      addText(nt);    
    }
  }

  // scale drawing
  // scale points to match current fixed length
  double curfixed = preferences.getBond_fixedlength();
  double avglen = totlen / (double)n;
  double sf = curfixed / avglen;
  cout << sf << endl;
  for (tmp_pt = parsedPoints.first(); tmp_pt != NULL; 
       tmp_pt = parsedPoints.next() ) {
    tmp_pt->x *= sf;
    tmp_pt->y *= sf;
  }
  
  Move(-dx * sf, -dy * sf);
  Move(40,40);
  return true;
}

// load CML format (save is waaaay down at the end of this file :)
// 'legacy' CML support.  Current CML format handled by LoadNew...() above
bool ChemData::LoadCMLFile(QString wholefile, QString doctype) {
  QString line;
  QString tmpline, tfont, tdata, a1, v1;
  QString thistag, nexttag;
  QStringList tokens, refs;
  QStringList elementlist, fclist, x2list, y2list, idlist;
  QStringList a1list, a2list, olist;
  QString str_id, str_title, str_builtin, atomrefs1, atomrefs2, str_order,
    str_element;
  QString arr_fc, arr_x2, arr_y2;
  int d1, d5, ptr = 0;
  int *st1;

  do {
    thistag = ReadTag(wholefile, ptr);
    cout << thistag.ascii() << endl;
    // ReadTag returns a null at EOF (ideally).
    if (thistag.isNull()) break;
    // Look for beginning of document
    if (thistag.find(QString("<") + doctype) >= 0) {
      // push this tag on stack
      st1 = new int;
      *st1 = CML_MOLECULE;
      TagStack.append(st1);
      // parse attributes in tag
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) MoleculeId = v1;
	if (a1.upper() == QString("TITLE")) MoleculeTitle = v1;
      }
    }
    // handle <atom> records
    if (thistag.find(QString("<atom ")) >= 0) {
      st1 = new int;
      *st1 = CML_ATOM;
      TagStack.append(st1);
      tmp_atom = new CML_Atom;
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) tmp_atom->id = v1;
      }
    }
    // handle </atom>
    if (thistag == QString("</atom>")) {
      st1 = TagStack.last();
      if (*st1 == CML_ATOM) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <atom> tags!" << endl;
      }
      CML_Atoms.append(tmp_atom);
    }
    // handle <bond> records
    if (thistag.find(QString("<bond ")) >= 0) {
      st1 = new int;
      *st1 = CML_BOND;
      TagStack.append(st1);
      tmp_bond = new CML_Bond;
      tmp_bond->a1 = "";
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("ID")) tmp_bond->id = v1;
	if (a1.upper() == QString("ATOMREFS")) {
	  d5 = v1.find(QString(" "));
	  tmp_bond->a1 = v1.left(d5);
	  tmp_bond->a2 = v1.mid(d5 + 1);
	  cout << "bond[" << tmp_bond->a1.ascii() << "|";
	  cout << tmp_bond->a2.ascii() << "]" << endl;
	}
      }
    }
    // handle </bond>
    if (thistag == QString("</bond>")) {
      st1 = TagStack.last();
      if (*st1 == CML_BOND) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <bond> tags!" << endl;
      }
      CML_Bonds.append(tmp_bond);
    }
    // handle <atomArray> records
    if (thistag == QString("<atomArray>")) {
      st1 = new int;
      *st1 = CML_ATOMARRAY;
      TagStack.append(st1);
      str_id.remove(0,9999);
      str_element.remove(0,9999);
    }
    // handle </atomArray>
    if (thistag == QString("</atomArray>")) {
      st1 = TagStack.last();
      if (*st1 == CML_ATOMARRAY) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <atomArray> tags!" << endl;
      }
      // create lists of tokens in arr_x2, arr_y2, str_id, str_element, arr_fc
      idlist = Tokenize(str_id);
      elementlist = Tokenize(str_element);
      x2list = Tokenize(arr_x2);
      y2list = Tokenize(arr_y2);
      fclist = Tokenize(arr_fc);
      for (d1 = 0; d1 < idlist.count(); d1++) {
	tmp_atom = new CML_Atom;
	tmp_atom->id = idlist[d1];
	tmp_atom->element = elementlist[d1];
	tmp_atom->formalCharge = fclist[d1].toInt();
	tmp_atom->x = x2list[d1].toDouble();
	tmp_atom->y = y2list[d1].toDouble();
	CML_Atoms.append(tmp_atom);
      }
    }
    // handle <bondArray> records
    if (thistag == QString("<bondArray>")) {
      st1 = new int;
      *st1 = CML_BONDARRAY;
      TagStack.append(st1);
      atomrefs1.remove(0,9999);
      atomrefs2.remove(0,9999);
      str_order.remove(0,9999);
    }
    // handle </bondArray>
    if (thistag == QString("</bondArray>")) {
      cout << atomrefs1 << endl;
      cout << atomrefs2 << endl;
      cout << str_order << endl;
      st1 = TagStack.last();
      if (*st1 == CML_BONDARRAY) {
	TagStack.removeLast();
      } else {
	cout << "Imbalanced <bondArray> tags!" << endl;
      }
      // tokenize data
      a1list = Tokenize(atomrefs1);
      a2list = Tokenize(atomrefs2);
      olist = Tokenize(str_order);
      for (d1 = 0; d1 < a1list.count(); d1++) {
	tmp_bond = new CML_Bond;
	tmp_bond->a1 = a1list[d1];
	tmp_bond->a2 = a2list[d1];
	tmp_bond->order = olist[d1].toInt();
	CML_Bonds.append(tmp_bond);
      }
    }
    // handle <coordinate2>-</coordinate2>
    if (thistag.find(QString("<coordinate2 ")) >= 0) {
      st1 = TagStack.last();
      if (*st1 != CML_ATOM)
	cout << "Improperly placed <coordinate2> tag!" << endl;
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "COORD2-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("TITLE")) str_title = v1;
	if (a1.upper() == QString("BUILTIN")) str_builtin = v1;
      }
      tdata = ReadData(wholefile, ptr);
      nexttag = ReadTag(wholefile, ptr);
      if (nexttag != QString("</coordinate2>"))
	cout << "Imbalanced <coordinate2> tags!" << endl;
      if (str_builtin != QString("xy2"))
	cout << "Unrecognized builtin type in <coordinate2>!" << endl;
      d5 = tdata.find(QString(" "));
      tmp_atom->x = tdata.left(d5).toDouble();
      tmp_atom->y = tdata.mid(d5).toDouble();
    }
    // handle <coordinate3>-</coordinate3>
    if (thistag.find(QString("<coordinate3 ")) >= 0) {
      st1 = TagStack.last();
      if (*st1 != CML_ATOM)
	cout << "Improperly placed <coordinate3> tag!" << endl;
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "COORD3-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("TITLE")) str_title = v1;
	if (a1.upper() == QString("BUILTIN")) str_builtin = v1;
      }
      tdata = ReadData(wholefile, ptr);
      nexttag = ReadTag(wholefile, ptr);
      if (nexttag != QString("</coordinate3>"))
	cout << "Imbalanced <coordinate3> tags!" << endl;
      if ( (str_builtin != QString("xyz3")) &&
	   (str_builtin != QString("xyzFract")) )
	cout << "Unrecognized builtin type in <coordinate3>!" << endl;
      tokens = Tokenize(tdata);
      tmp_atom->x = tokens[0].toDouble();
      tmp_atom->y = tokens[1].toDouble();
      tmp_atom->z = tokens[2].toDouble();
    }
    // handle <string>-</string> AND <stringArray>-</stringArray>
    if ( (thistag.find(QString("<string ")) >= 0) ||
	 (thistag.find(QString("<stringArray ")) >= 0) ) {
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	cout << "STRING-" << a1.ascii() << "|" << v1.ascii() << "-" << endl;
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("TITLE")) str_title = v1;
	if (a1.upper() == QString("BUILTIN")) str_builtin = v1;
      }
      tdata = ReadData(wholefile, ptr);
      nexttag = ReadTag(wholefile, ptr);
      cout << "DATA<string>[" << tdata << "]" << endl;
      if (nexttag.left(8) != QString("</string"))
	cout << "Imbalanced <string> tags!" << endl;
      // known cases of <string>
      st1 = TagStack.last();
      if ( (*st1 == CML_BOND) && (str_builtin == QString("atomRef")) ) {
	if (tmp_bond->a1 == "") 
	  tmp_bond->a1 = tdata;
	else
	  tmp_bond->a2 = tdata;
      }
      if ( (*st1 == CML_BOND) && (str_builtin == QString("order")) )
	tmp_bond->order = tdata.toInt();
      if ( (*st1 == CML_ATOM) && (str_builtin == QString("elementType")) )
	tmp_atom->element = tdata;
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin == QString("elementType")) )
	str_element = tdata;
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin == QString("id")) )
	str_id = tdata;
      if ( (*st1 == CML_BONDARRAY) && (str_builtin == QString("atomRefs")) ) {
	if (atomrefs1.length() == 0)
	  atomrefs1 = tdata;
	else
	  atomrefs2 = tdata;
      }
      if ( (*st1 == CML_BONDARRAY) && (str_builtin == QString("order")) )
	str_order = tdata;
    }
    // handle <integerArray>-</integerArray> AND 
    // <floatArray>-</floatArray>
    if ( (thistag.find(QString("<integerArray ")) >= 0) ||
	 (thistag.find(QString("<floatArray ")) >= 0) ) {
      tokens = ReadAttr(thistag);
      for (int c1 = 0; c1 < tokens.count(); c1 += 2) {
	a1 = *(tokens.at(c1));
	v1 = *(tokens.at(c1 + 1));
	// compare attribute a1 with list of relevant attr's
	if (a1.upper() == QString("TITLE")) str_title = v1;
	if (a1.upper() == QString("BUILTIN")) str_builtin = v1;
      }
      tdata = ReadData(wholefile, ptr);
      nexttag = ReadTag(wholefile, ptr);
      cout << "DATA<numArray>[" << tdata << "]" << endl;
      if ( (nexttag != QString("</integerArray>")) &&
	   (nexttag != QString("</floatArray>")) )
	cout << "Imbalanced <integerArray> or <floatArray> tags!" << endl;
      // known cases of <integerArray> and <floatArray>
      st1 = TagStack.last();
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin==QString("formalCharge")) )
	arr_fc = tdata;
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin == QString("x2")) )
	arr_x2 = tdata;
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin == QString("y2")) )
	arr_y2 = tdata;
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin == QString("x3")) )
	arr_x2 = tdata;
      if ( (*st1 == CML_ATOMARRAY) && (str_builtin == QString("y3")) )
	arr_y2 = tdata;
      if ( (*st1 == CML_BONDARRAY) && (str_builtin == QString("order")) )
	str_order = tdata;
    }
    // Look for end of document
    if (thistag.find(QString("</") + doctype) >= 0) {
      st1 = TagStack.last();
      if (*st1 == CML_MOLECULE) {
	TagStack.removeLast();
	// should be last tag...
	if (!TagStack.isEmpty()) cout << "Imbalanced tags!" << endl;
	continue;
      }
    }
  } while (ptr < wholefile.length());

  // convert CML_Atom and CML_Bond to DPoint and customObject, respectively
  Convert_CML_Lists_To_Native();

  return true;
}

void ChemData::Convert_CML_Lists_To_Native() {
  double totlen = 0.0;
  int n = 0;
  QString makesub, ms2;
  int th1;
  std::cout << "thick_kludge: " << thick_kludge << std::endl;
  if (thick_kludge > 0) {
    th1 = thick_kludge;
  } else {
    th1 = 1;
  }

  // convert CML_Atoms to DPoints
  QPtrList<DPoint> points;
  for (tmp_atom = CML_Atoms.first(); tmp_atom != NULL; 
       tmp_atom = CML_Atoms.next()) {
    points.append(tmp_atom->toDPoint());
  }
  // add Bonds
  DPoint *end1, *end2;  // Bond endpoints
  for (tmp_bond = CML_Bonds.first(); tmp_bond != NULL;
       tmp_bond = CML_Bonds.next() ) {
    // find first atom
    for (tmp_pt = points.first(); tmp_pt != NULL;
	 tmp_pt = points.next() ) {
      if (tmp_bond->a1 == tmp_pt->id) {
	end1 = tmp_pt;
	break;
      }
    }
    // find second atom
    for (tmp_pt = points.first(); tmp_pt != NULL;
	 tmp_pt = points.next() ) {
      if (tmp_bond->a2 == tmp_pt->id) {
	end2 = tmp_pt;
	break;
      }
    }
    addBond(end1, end2, th1, tmp_bond->order, QColor(0,0,0), true);
    totlen += end1->distanceTo(end2);
    n++;
  }
  // add Text labels
  for (tmp_pt = points.first(); tmp_pt != NULL; tmp_pt = points.next() ) {
    if (tmp_pt->element != QString("C")) {
      cout << tmp_pt->element << endl;
      Text *nt = new Text(r);
      nt->setPoint(tmp_pt);
      nt->setJustify(JUSTIFY_CENTER);
      nt->Highlight(true);
      nt->setText(tmp_pt->element);
      makesub = tmp_pt->element;
      ms2 = "";
      for (int cc = 0; cc < makesub.length(); cc++) {
	if (makesub[cc].isLetter()) ms2.append(" ");
	if (makesub[cc].isNumber()) ms2.append("-");
	if (makesub[cc] == '+') ms2.append("+");
	if (makesub[cc] == '-') ms2.append("+");
      }
      nt->setTextMask(ms2);
      addText(nt);
    }
  }

  // scale points to match current fixed length
  double curfixed = preferences.getBond_fixedlength();
  double avglen = totlen / (double)n;
  double sf = curfixed / avglen;
  cout << sf << endl;
  for (tmp_draw = CDXML_Objects.first(); tmp_draw != NULL;
       tmp_draw = CDXML_Objects.next() ) {
    points.append(tmp_draw->Start());
    points.append(tmp_draw->End());
  }
  for (tmp_pt = points.first(); tmp_pt != NULL; tmp_pt = points.next() ) {
    tmp_pt->x *= sf;
    tmp_pt->y *= sf;
  }
  for (tmp_draw = CDXML_Objects.first(); tmp_draw != NULL;
       tmp_draw = CDXML_Objects.next() ) {
    drawlist.append(tmp_draw);
  }
  
  QRect sb = selectionBox();
  cout << sb.left() << endl;
  cout << sb.right() << endl;
  cout << sb.top() << endl;
  cout << sb.bottom() << endl;
  double tx = RenderTopLeft.x() + 50 - sb.left();
  double ty = RenderTopLeft.y() + 50 - sb.top();
  Move(tx, ty);
}

// Save CML format
bool ChemData::save_cml(QString fn) {
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
  t << "<molecule ";
  QString MoleculeId = fn.right(fn.length() - fn.findRev("/") - 1);
  t << "id=\"" << MoleculeId.ascii() << "\">" << endl;
  // add miscellaneous info, a la JChemPaint
  t << "<string title=\"GenerationDate\">" 
    << QDateTime::currentDateTime().toString() << "</string>" << endl;
  t << "<string title=\"GenerationSoftware\">" << XDC_VERSION 
    << "</string>" <<endl;
  
  // Copy text from Text objects to element field in DPoint
  for (tmp_draw = uo.first(); tmp_draw != NULL; tmp_draw = uo.next() ) {
    if (tmp_draw->Type() == TYPE_TEXT) {
      tmp_text = (Text *)tmp_draw;  // is this cheating?
      tmp_text->Start()->element = tmp_text->getText();
    }
  }
  // Add XML ID's to DPoint's, write as we go
  t << "<atomArray>" << endl;
  for (tmp_pt = up.first(); tmp_pt != NULL; tmp_pt = up.next() ) {
    n1.setNum(n);
    nfull = QString("a") + n1;
    tmp_pt->id = nfull;
    n++;
    t << "<atom id=\"" << tmp_pt->id << "\">" << endl;
    t << "<string builtin=\"elementType\">";
    t << tmp_pt->element << "</string>" << endl;
    t << "<float builtin=\"x2\">";
    t << tmp_pt->x;
    t << "</float>" << endl << "<float builtin=\"y2\">";
    t << tmp_pt->y;
    t << "</float>" << endl;
    //t << "<integer builtin=\"formalCharge\">0</integer>" << endl; 
    t << "</atom>" << endl;    
  }
  t << "</atomArray>" << endl;

  // add XML ID's to Bond's, write as we go
  n = 0;
  t << "<bondArray>" << endl;
  for (tmp_draw = uo.first(); tmp_draw != NULL; tmp_draw = uo.next() ) {
    if (tmp_draw->Type() == TYPE_BOND) {
      tmp_bond = (Bond *)tmp_draw;  // I ask again, is this cheating?
      n1.setNum(n);
      nfull = QString("b") + n1;
      tmp_draw->setID(nfull);
      n++;
      t << "<bond id=\"" << tmp_bond->getID() << "\">" << endl;
      t << "<string builtin=\"atomRef\">" << tmp_bond->Start()->id 
	<< "</string>" << endl;
      t << "<string builtin=\"atomRef\">" << tmp_bond->End()->id 
	<< "</string>" << endl;
      t << "<string builtin=\"order\">" << tmp_bond->Order() 
	<< "</string>" << endl;
      if (tmp_bond->Order() == 5) {  // stereo-up bonds
	t << "<string builtin=\"stereo\" convention=\"MDLMol\">W</string>" 
	  << endl;
      }
      if (tmp_bond->Order() == 7) {  // stereo-down bonds
	t << "<string builtin=\"stereo\" convention=\"MDLMol\">H</string>" 
	  << endl;
      }
      t << "</bond>" << endl;      
    }
  }
  t << "</bondArray>" << endl;
  // End file...
  t << "</molecule>" << endl;

  f.close();
  return true;
}
