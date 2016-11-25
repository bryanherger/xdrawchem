#include <iostream>
#include <qstring.h>
#include <qptrlist.h>
#include <qtextstream.h>
#include <qmessagebox.h>
#include <qfile.h>

using std::cout;
using std::endl;

#include "chemdata.h"
#include "cdxml_reader.h"
#include "dpoint.h"
#include "text.h"
#include "defs.h"


using std::cout;

bool ChemData::load(QString fn) {
  // identify file type by first few lines
  QFile f(fn);
  if (!f.open(IO_ReadOnly)) {
    QMessageBox::warning(0, "Couldn't open file", "Could not open the file: " + fn);
    return false;
  }

  // first, read first 8 bytes for CDX (binary ChemDraw) magic cookie
  f.flush();
  char cookie[10];
  f.readBlock(cookie, 8);
  cookie[8] = '\0';
  QString qcookie(cookie);
  cout << "cookie:" << qcookie << endl;
  //if (qcookie == "VjCD0100") {
  if (qcookie.left(2).upper() == "VJ") {
    QString wf = CDXToCDXML(fn);
    CDXML_Reader reader(this);
    reader.ReadFile(wf);
    return true;
  }
  f.reset();

  // now check for text file types
  QTextStream t(&f);
  QString ln;
  ln = t.read();

  // magic cookie check
  // pre-0.90 native format contains XDRAWCHEM-1.0 in first line
  if (ln.find("XDRAWCHEM-1.0") >= 0) {
    f.close();
    return load_legacy(fn);
  }
  // Check for XML: could be CML, CDXML, or native XDC...
  // look for many "<"   :)
  if (ln.contains("<") > 8) {
    f.close();
    return load_xml(fn);
  }
  // if not XML, the last type supported is MDL Molfile - try that
  f.close();
  return load_mdl(fn);

  // if all else fails...
  return false;
}

bool ChemData::save(QString fn) {
  // determine save file type by extension
  if (fn.right(3).lower() == QString("xdc"))
    return save_native(fn);
  if (fn.right(3).lower() == QString("cml"))
    return save_cml(fn);
  if (fn.right(3).lower() == QString("mol"))
    return save_mdl(fn);
  if (fn.right(3).lower() == QString("cdx"))
    return save_cdx(fn);
  if (fn.right(5).lower() == QString("cdxml"))
    return save_cdxml(fn);

  return false;
}

// Load legacy XDrawChem (0.85 and prior)
bool ChemData::load_legacy(QString fn) {
  QFile f(fn);
  if (!f.open(IO_ReadOnly)) return false;
  QTextStream t(&f);
  QString ln, dummy, tmpline;
  int i1, i2, i3, i4, i5;
  QPtrList<DPoint> points;
  DPoint *tp;
  Text *mt;

  do {
    ln = t.readLine();
    // read POINTs into QPtrList points (hopefully all at beginning of file :)
    if (ln.left(5) == QString("POINT")) {
      tp = new DPoint;
      QTextIStream(&ln) >> dummy >> i1 >> tp->x >> tp->y >> i2 >> i3;
      points.append(tp);
    }
    // add LINEs
    if (ln.left(4) == QString("LINE")) {
      QTextIStream(&ln) >> dummy >> i1 >> i2 >> i3 >> i4 >> i5;
      addBond(points.at(i1), points.at(i2), 1, i3, QColor(0,0,0));
    }
    // add UPLINEs
    if (ln.left(6) == QString("UPLINE")) {
      QTextIStream(&ln) >> dummy >> i1 >> i2 >> i3 >> i4 >> i5;
      addBond(points.at(i1), points.at(i2), 1, 5, QColor(0,0,0));
    }
    // add DOWNLINEs
    if (ln.left(8) == QString("DOWNLINE")) {
      QTextIStream(&ln) >> dummy >> i1 >> i2 >> i3 >> i4 >> i5;
      addBond(points.at(i1), points.at(i2), 1, 7, QColor(0,0,0));
    }
    // add ARROWs
    if (ln.left(5) == QString("ARROW")) {
      QTextIStream(&ln) >> dummy >> i1 >> i2;
      addArrow(points.at(i1), points.at(i2), QColor(0,0,0), ARROW_REGULAR, 1);
    }
    // add BRACKETs
    if (ln.left(7) == QString("BRACKET")) {
      QTextIStream(&ln) >> dummy >> i1 >> i2;
      addBracket(points.at(i1), points.at(i2), QColor(0,0,0), BRACKET_SQUARE);
    }
    // add TEXT
    if (ln.left(4) == QString("TEXT")) {
      QTextIStream(&ln) >> tmpline >> i1;
      tmpline = ln.mid(ln.find('`') + 1);
      mt = new Text(r);
      mt->setPoint(points.at(i1));
      mt->setJustify(JUSTIFY_TOPLEFT);
      mt->SetColor(QColor(0,0,0));
      mt->setText(tmpline);
      tmpline.fill(' ');
      mt->setTextMask(tmpline);
      addText(mt);
    }
    // add LABEL
    if (ln.left(5) == QString("LABEL")) {
      QTextIStream(&ln) >> tmpline >> i1;
      tmpline = ln.mid(ln.find('`') + 1);
      mt = new Text(r);
      mt->setPoint(points.at(i1));
      mt->setJustify(JUSTIFY_CENTER);
      mt->SetColor(QColor(0,0,0));
      mt->setText(tmpline);
      tmpline.fill(' ');
      mt->setTextMask(tmpline);
      addText(mt);
    }
  } while (!t.atEnd());

  cout << "Legacy: found " << points.count() << " points." << endl;
  return true;
}
