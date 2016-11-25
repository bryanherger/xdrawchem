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
#include "text.h"
#include "defs.h"

// save as ChemDraw binary format (IBM PC byte order)
bool ChemData::save_cdx(QString fn) {
  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  QString tmpline;

  // get all unique points and objects
  up = UniquePoints();
  uo = UniqueObjects();

  if (up.count() == 0) return false;  // don't write an empty file

  // open file and text stream
  QFile f(fn);
  if (!f.open(IO_WriteOnly)) {
    return false;
  }  

  f.close();
  return false;
}

// save as ChemDraw XML text format
bool ChemData::save_cdxml(QString fn) {
  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  QString tmpline, cur_id;
  int idcount = 1;

  // open file and text stream
  QFile f(fn);
  if (!f.open(IO_WriteOnly)) {
    return false;
  }  
  
  QTextStream t(&f);

  t << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << endl;
  t << "<!DOCTYPE CDXML SYSTEM \"http://www.camsoft.com/xml/cdxml.dtd\">" << endl;
  t << "<CDXML CreationProgram=\"" << XDC_VERSION << "\">" << endl;
  t << "<fonttable>" << endl;
  t << "<font id=\"21\" charset=\"x-mac-roman\" name=\"Helvetica\"/>" << endl;
  t << "</fonttable>" << endl;
  t << "<page>" << endl;

  // save document here
  for (tmp_draw = drawlist.first(); tmp_draw != 0; 
       tmp_draw = drawlist.next() ) {
    cur_id.setNum(idcount);
    tmpline = tmp_draw->ToCDXML(cur_id);
    if (tmp_draw->Type() == TYPE_MOLECULE)
      idcount += 500;
    else
      idcount += 2;
    t << tmpline;
  }

  t << "</page>" << endl;
  t << "</CDXML>" << endl;

  f.close();
  return true;
}
