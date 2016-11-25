#include <iostream>
#include <qstring.h>
#include <qstringlist.h>
#include <qptrlist.h>
#include <qtextstream.h>
#include <qfile.h>

using std::cout;
using std::endl;

#include "chemdata.h"
#include "cdxml_reader.h"
#include "dpoint.h"
#include "text.h"
#include "defs.h"

using std::cout;

bool ChemData::load_xml(QString fn) {
  QFile f(fn);
  if (!f.open(IO_ReadOnly)) {
    return false;
  }  

  QTextStream t(&f);

  // Read entire file to string
  QString wholefile = t.read();
  // We're done with the file.  Close it
  f.close();

  // try and guess whether (new) CML file
  bool cmlfile = false;
  if (wholefile.upper().contains("<ATOM") > 0) cmlfile = true;
  if (wholefile.upper().contains("<XDRAWCHEM") > 0) cmlfile = false;
  if (wholefile.upper().contains("<CDXML") > 0) cmlfile = false;
  if (wholefile.upper().contains("ATOMREFS=") > 0) cmlfile = false;
  if (wholefile.upper().contains("<STRINGARRAY") > 0) cmlfile = false;

  if (cmlfile == true) {
    bool r1 = NewLoadCMLFile(fn);
    return r1;
  } 

  // trasfer control to process_xml()
  return ProcessXML(wholefile);
}

// if you have already read in a file (e.g., from network), call this
bool ChemData::ProcessXML(QString wholefile) {
  QString line;
  QString doctype, thistag, nexttag, cml_doctype, cdxml_doctype, xdc_doctype;
  QStringList tokens, refs;
  int d1, d2, d3, ptr = 0, cml_type = 0;
  //double l1, l2, l3, l4;

  // clear all lists
  CML_Atoms.clear();
  CML_Atoms_Sub.clear();
  CML_Bonds.clear();
  CML_Bonds_Sub.clear();
  CDXML_Objects.clear();

  // kludge!
  cml_doctype = QString("idgniosdnvi");
  cdxml_doctype = QString("idgniosdnvi");
  xdc_doctype = QString("idgniosdnvi");
  
  do {
    thistag = ReadTag(wholefile, ptr);
    // ReadTag returns a null at EOF (ideally).
    if (thistag.isNull()) break;
    // check for 'special' tags...
    // continue past <?xml...> tag
    if (thistag.find(QString("?xml")) >= 0) continue;
    // but we really want the <!DOCTYPE...> tag
    if (thistag.find(QString("!DOCTYPE")) >= 0) {
      // read the actual document type
      d1 = thistag.find(QString("!DOCTYPE"));
      d2 = thistag.find(QString(" "), d1);
      d3 = thistag.find(QString(" "), d2 + 1);
      doctype = thistag.mid(d2 + 1, d3 - d2 - 1);
      if (thistag.upper().find("CML.DTD") >= 0) cml_type = 1;
      if (thistag.upper().find("CML-1999-05-15") >= 0) cml_type = 1;
      if (thistag.upper().find("CDXML.DTD") >= 0) cml_type = 2;
      if (thistag.upper().find("XDRAWCHEM-1") >= 0) cml_type = 3;
      if (cml_type == 1) cml_doctype = doctype;
      if (cml_type == 2) cdxml_doctype = doctype;
      if (cml_type == 3) xdc_doctype = doctype;
      if (cml_type == 0) {
	return false;
      }
      continue;
    }
    // Extract CML documents, pass to LoadCMLFile
    if (thistag.find(QString("<") + cml_doctype) >= 0) {
      d1 = ptr - thistag.length();
      d2 = wholefile.find(QString("</") + cml_doctype + QString(">"), ptr) + 
	3 + cml_doctype.length();
      LoadCMLFile(wholefile.mid(d1, d2 - d1), cml_doctype);
      wholefile.remove(d1, d2 - d1);
    }
    // Extract CDXML documents, pass to LoadCDXMLFile
    if (thistag.find(QString("<") + cdxml_doctype) >= 0) {
      // test functionality
      CDXML_Reader reader(this);
      reader.ReadFile(wholefile);
      // end test
      d1 = ptr - thistag.length();
      d2 = wholefile.find(QString("</") + cdxml_doctype + QString(">"), ptr) + 
	3 + cml_doctype.length();
      //LoadCDXMLFile(wholefile.mid(d1, d2 - d1), cdxml_doctype);
      wholefile.remove(d1, d2 - d1);
    }
    // Extract native (XDC) documents, pass to load_native()
    // note that native documents expect <xdrawchem>...</xdrawchem>
    // and that this pair should contain all elements in the document
    if (thistag.find(QString("<") + xdc_doctype) >= 0) {
      d1 = ptr - thistag.length();
      d2 = wholefile.find(QString("</") + xdc_doctype + QString(">"), ptr) + 
	3 + cml_doctype.length();
      load_native( wholefile.mid(d1, d2 - d1) );
      wholefile.remove(d1, d2 - d1);
    }
  } while (ptr < wholefile.length());
  r->repaint(false);
  
  return true;
}

// *****************************
// General XML parsing functions
// *****************************

// function to see if tag is 'self-contained', i.e.:
// <bond>data</bond> returns false
// <bond data="x"/> returns true
// (returns true if tag ends in "/>"
bool ChemData::SelfContainedTag(QString tag) {
  if (tag.right(2) == "/>")
    return true;
  else
    return false;
}

// function to extract tags from stream
QString ChemData::ReadTag(QString f, int &ptr) {
  int t1 = f.find("<", ptr);
  ptr = t1;
  int t2 = f.find(">", ptr);
  ptr = t2 + 1;
  return f.mid(t1, ptr - t1).ascii();
}

// function to extract data between tags
QString ChemData::ReadData(QString f, int &ptr) {
  int t1 = f.find("<", ptr);
  return f.mid(ptr, t1 - ptr);
}

// function to extract attributes from tag
// Return format:  list item 0=first attribute, 1=value of first attribute,
//   2=second attribute, 3=value of second attribute, you get it, right?
QStringList ChemData::ReadAttr(QString tag) {
  QStringList l1;
  QString attr;
  QString a1, v1;
  int t1, eq1, te;
  // where to start tokenizing
  int ptr = tag.find(QString(" "));
  // where to end tokenizing
  int eol = tag.find(QString(">"));
  
  do {
    while (tag[ptr].isSpace()) { ptr++; }
    t1 = tag.find(QString("\""), ptr);
    te = tag.find(QString("\""), t1 + 1);
    attr = tag.mid(ptr, te - ptr + 1);
    ptr = te + 1;
    eq1 = attr.find(QString("="));
    a1 = attr.left(eq1);
    v1 = attr.right(attr.length() - eq1 - 1);
    v1 = v1.mid(1, v1.length() - 2);
    if (a1.find(QString(">")) >= 0) break; // end of tags
    //cout << "[" << a1 << "=" << v1 << "]" << endl;
    l1.append(a1); l1.append(v1);
  } while (ptr < eol);

  return l1;
}

// function to tokenize
QStringList ChemData::Tokenize(QString tag) {
  QStringList l1;
  QString a1, tag1;
  int t1;
  // strip leading and trailing white space
  tag1 = tag.simplifyWhiteSpace();
  // where to start tokenizing
  int ptr = 0;
  // where to end tokenizing
  int eol = tag1.findRev(QString(" "));
  
  do {
    t1 = tag1.find(QString(" "), ptr);
    a1 = tag1.mid(ptr, t1 - ptr);
    ptr = t1 + 1;
    l1.append(a1);
    cout << "[" << a1 << "]" << endl;
  } while (ptr < eol);
  
  a1 = tag1.mid(ptr);
  l1.append(a1);

  return l1;
}

// ParseBoundingBox(): parse CDXML BoundingBox element.
void ChemData::ParseBoundingBox(QString BBox, double &x1, double &y1, 
				double &x2, double &y2) {
  int s1 = BBox.find(QString(" "), 0);
  int s2 = BBox.find(QString(" "), s1 + 1);
  int s3 = BBox.find(QString(" "), s2 + 1);
  //cout << BBox.mid(0, s1) << "|";
  //cout << BBox.mid(s1 + 1, s2 - s1) << "|";
  //cout << BBox.mid(s2 + 1, s3 - s2) << "|";
  //cout << BBox.mid(s3 + 1) << "|" << endl;
  x1 = BBox.mid(0, s1).toDouble();
  y1 = BBox.mid(s1 + 1, s2 - s1).toDouble();
  x2 = BBox.mid(s2 + 1, s3 - s2).toDouble();
  y2 = BBox.mid(s3 + 1).toDouble();
}

// *********************
// end parsing functions
// *********************
