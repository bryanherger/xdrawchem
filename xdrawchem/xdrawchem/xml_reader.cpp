// XML_Reader.cpp - base class for XML type readers

#include <iostream>
#include <qstring.h>
#include <qstringlist.h>

using std::cout;
using std::endl;

#include "xml_reader.h"

// *****************************
// General XML parsing functions
// *****************************

// function to see if tag is 'self-contained', i.e.:
// <bond>data</bond> returns false
// <bond data="x"/> returns true
// (returns true if tag ends in "/>"
bool XML_Reader::SelfContainedTag(QString tag) {
  if (tag.right(2) == "/>")
    return true;
  else
    return false;
}

// function to extract tags from stream
QString XML_Reader::ReadTag(QString f, int &ptr) {
  int t1 = f.find("<", ptr);
  ptr = t1;
  int t2 = f.find(">", ptr);
  ptr = t2 + 1;
  return f.mid(t1, ptr - t1).ascii();
}

// function to extract data between tags
QString XML_Reader::ReadData(QString f, int &ptr) {
  int t1 = f.find("<", ptr);
  return f.mid(ptr, t1 - ptr);
}

// function to extract attributes from tag
// Return format:  list item 0=first attribute, 1=value of first attribute,
//   2=second attribute, 3=value of second attribute, you get it, right?
QStringList XML_Reader::ReadAttr(QString tag) {
  QStringList l1;
  QString attr;
  QString a1, v1;
  int t1, eq1, te;
  // where to start tokenizing
  int ptr = tag.find(QString(" "));
  // where to end tokenizing
  int eol = tag.find(QString(">"));

  // exit gracefully if no attributes
  if (tag.find("=") < 0) return l1;
  
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
QStringList XML_Reader::Tokenize(QString tag) {
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
void XML_Reader::ParseBoundingBox(QString BBox, double &x1, double &y1, 
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

// make sure we get the correct close tag
// e.g., if passed <n><b>xyz</b><n><b>zyx</b></n></n>, return the correct
// outer tag (the last </n>)
// note this returns a relative reference, from beginning of wtag.
int XML_Reader::PositionOfEndTag(QString wtag, QString tname) {
  int depth = 0, curpos = 0;
  int i1, i2;
  //cout << "l:" << tname.length() << endl;
  do {
    i1 = wtag.find(tname, curpos);
    //cout << i1 << "," << depth << endl;
    i2 = wtag.find(">", i1);
    if (wtag[i2 - 1] != '/') {  // skip self-contained tags
      if (wtag[i1 - 1] == '<') depth++;
      if (wtag[i1 - 1] == '/') depth--;
      if (depth == 0) break;
    }
    curpos = i1 + 1;
  } while (1);
  i2 = i1 + tname.length() + 1;
  //cout << "r:" << i2 << endl;
  return i2;
}

// *********************
// end parsing functions
// *********************
