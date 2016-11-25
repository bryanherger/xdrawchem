#include <iostream>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qtextstream.h>
#include <qfile.h>
#include <qcolor.h>

using std::cout;
using std::endl;

#include "defs.h"
#include "chemdata.h"
#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "curvearrow.h"
#include "bracket.h"
#include "gobject.h"
#include "text.h"
#include "dpoint.h"
#include "text.h"
#include "cml.h"

using std::cout;

// handle new native format

bool ChemData::load_native(QString wholefile) {
  QString line;
  QString tmpline, tfont, tdata, a1, v1;
  QString thistag, nexttag;
  int d1, d2, d3, ptr = 0;
  int psize = PAGE_LETTER, porient = PAGE_PORTRAIT;

  // strip <xdrawchem> and </xdrawchem> tags.
  d1 = wholefile.find("<xdrawchem");
  d2 = wholefile.find(">", d1);
  wholefile.remove(d1, d2 - d1 + 1);
  d3 = wholefile.find("</xdrawchem>");
  wholefile.remove(d3, 999);

  // find page size/orientation tags (these are optional)
  d1 = wholefile.find("<pagesize>");
  if (d1 >= 0) {
    d2 = wholefile.find("</pagesize>");
    line = wholefile.mid(d1 + 10, d2 - d1 - 10);
    cout << "PAGESIZE:" << line << endl;
    if (line == "PAGE_LETTER") psize = PAGE_LETTER;
    if (line == "PAGE_LEGAL") psize = PAGE_LEGAL;
    if (line == "PAGE_A4") psize = PAGE_A4;
    if (line == "PAGE_640") psize = PAGE_640;
    if (line == "PAGE_800") psize = PAGE_800;
    if (line == "PAGE_1024") psize = PAGE_1024;
  }
  d1 = wholefile.find("<pageorient>");
  if (d1 >= 0) {
    d2 = wholefile.find("</pageorient>");
    line = wholefile.mid(d1 + 12, d2 - d1 - 12);
    cout << "PAGEORIENT:" << line << endl;
    // porient defaults to PAGE_PORTRAIT
    if (line == "PAGE_LANDSCAPE") porient = PAGE_LANDSCAPE;
  }
  r->setPageSizeAndOrientation();
  // read and set color
  d1 = wholefile.find("<bgcolor>");
  if (d1 >= 0) {
    d2 = wholefile.find("</bgcolor>");
    line = wholefile.mid(d1 + 9, d2 - d1 - 9);
    QTextStream is(&line, IO_ReadOnly);
    int rn, gn, bn;
    is >> rn >> gn >> bn;
    QColor bc1(rn, gn, bn);
    r->setBGColor(bc1);
  }

  do {
    thistag = ReadTag(wholefile, ptr);
    // ReadTag returns a null at EOF (ideally).
    if (thistag.isNull()) break;
    // check what object type this is
    if (thistag.contains("<arrow") > 0) {
      d1 = wholefile.find("<arrow");
      d2 = wholefile.find("</arrow>");
      thistag = wholefile.mid(d1, d2 - d1 + 8);
      Arrow *a1 = new Arrow(r);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 8);
      ptr = 0;
      continue;
    }
    if (thistag.contains("<graphicobject") > 0) {
      d1 = wholefile.find("<graphicobject");
      d2 = wholefile.find("</graphicobject>");
      thistag = wholefile.mid(d1, d2 - d1 + 16);
      GraphicObject *a1 = new GraphicObject(r);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 13);
      ptr = 0;
      continue;
    }
    if (thistag.contains("<curvearrow") > 0) {
      d1 = wholefile.find("<curvearrow");
      d2 = wholefile.find("</curvearrow>");
      thistag = wholefile.mid(d1, d2 - d1 + 13);
      CurveArrow *a1 = new CurveArrow(r);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 13);
      ptr = 0;
      continue;
    }
    if (thistag.contains("<text") > 0) {
      d1 = wholefile.find("<text");
      d2 = wholefile.find("</text>");
      thistag = wholefile.mid(d1, d2 - d1 + 7);
      Text *a1 = new Text(r);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 7);
      ptr = 0;
      continue;
    }
    if (thistag.contains("<bracket") > 0) {
      d1 = wholefile.find("<bracket");
      d2 = wholefile.find("</bracket>");
      thistag = wholefile.mid(d1, d2 - d1 + 10);
      Bracket *a1 = new Bracket(r);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 10);
      ptr = 0;
      continue;
    }
    if (thistag.contains("<symbol") > 0) {
      d1 = wholefile.find("<symbol");
      d2 = wholefile.find("</symbol>");
      thistag = wholefile.mid(d1, d2 - d1 + 9);
      Symbol *a1 = new Symbol(r);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 9);
      ptr = 0;
      continue;
    }
    if (thistag.contains("<molecule") > 0) {
      d1 = wholefile.find("<molecule");
      d2 = wholefile.find("</molecule>");
      thistag = wholefile.mid(d1, d2 - d1 + 11);
      Molecule *a1 = new Molecule(r);
      a1->SetChemdata(this);
      a1->FromXML(thistag);
      drawlist.append(a1);
      wholefile.remove(d1, d2 - d1 + 11);
      ptr = 0;
      continue;
    }
  } while (ptr < wholefile.length());

  return false;
}

bool ChemData::save_native(QString fn) {
  int n = 0;
  QString n1, nfull, fulltext;
  int ps = preferences.getPageSize(), 
    po = preferences.getPageOrientation();

  QTextStream t(&fulltext, IO_WriteOnly);

  if (fn.length() > 0) {
    t << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" << endl;
    t << "<!DOCTYPE xdrawchem SYSTEM \"xdrawchem-1.dtd\" []>" << endl;
  } else {
    current_undo_file = "";
  }

  t << "<xdrawchem ";
  QString MoleculeId = fn.right(fn.length() - fn.findRev("/") - 1);
  t << "id=\"" << MoleculeId.ascii();
  t << "\">" << endl;

  // write page size and orientation
  t << "<pagesize>";
  switch (ps) {
  case PAGE_LETTER:
    t << "PAGE_LETTER";
    break;
  case PAGE_LEGAL:
    t << "PAGE_LEGAL";
    break;
  case PAGE_A4:
    t << "PAGE_A4";
    break;
  case PAGE_640:
    t << "PAGE_640";
    break;
  case PAGE_800:
    t << "PAGE_800";
    break;
  case PAGE_1024:
    t << "PAGE_1024";
    break;
  default:
    t << "PAGE_LETTER";
    break;    
  }
  t << "</pagesize>" << endl;
  if (po == PAGE_PORTRAIT)
    t << "<pageorient>PAGE_PORTRAIT</pageorient>" << endl;
  else
    t << "<pageorient>PAGE_LANDSCAPE</pageorient>" << endl;
  // write background color
  t << "<bgcolor>";
  QColor bc1 = r->getBGColor();
  t << bc1.red() << " ";
  t << bc1.green() << " ";
  t << bc1.blue() << "</bgcolor>" << endl;

  for (tmp_draw = drawlist.first(); tmp_draw != NULL; 
       tmp_draw = drawlist.next()) {
    n1.setNum(n);
    nfull = QString("obj") + n1;
    t << tmp_draw->ToXML(nfull);
    n++;
  }

  t << "</xdrawchem>" << endl;

  if (fn.length() > 0) {
    QFile f(fn);
    if (!f.open(IO_WriteOnly)) {
      return false;
    }  
    
    QTextStream t1(&f);
    t1 << fulltext << endl;
    f.close();
  } else {
    current_undo_file = fulltext;
  }

  return true;
}
