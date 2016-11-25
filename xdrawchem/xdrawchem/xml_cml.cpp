/****************************************************************************
** $Id: xml_cml.cpp 148 2005-05-18 01:06:37Z bherger $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "xml_cml.h"
#include "defs.h"

#include <iostream>
using std::cout;
using std::endl;

#include <qfile.h>
#include <qstring.h>
 
bool CMLParser::startDocument()
{
  cout << "New CML parser started." << endl;
  return TRUE;
}

bool CMLParser::startElement( const QString&, const QString&, 
                                    const QString& qName, 
                                    const QXmlAttributes& attr )
{
  cout << "Start:" << qName << endl;
  if (qName.upper() == "ATOM") {
    states = CML_ATOM;
    tmp_pt = new DPoint;
    tmp_pt->id = attr.value("id");
    cout << "Atom id=" << attr.value("id") << endl;
  }
  if (qName.upper() == "BOND") {
    states = CML_BOND;
    tmp_bond = new Bond(r);
    tmp_bond->setID(attr.value("id"));
    ep1 = 0; ep2 = 0;
  }
  if (qName.upper() == "FLOAT") {
    last_builtin = attr.value("builtin").upper();
    if (last_builtin == "X3") last_builtin = "X2";
    if (last_builtin == "X3") last_builtin = "Y2";
  }
  if (qName.upper() == "STRING") {
    last_builtin = attr.value("builtin").upper();
  }
  return TRUE;
}

bool CMLParser::endElement( const QString&, const QString&, 
			    const QString& qName )
{
  cout << "End:" << qName << endl;
  if (qName.upper() == "ATOM") {
    localPoints.append(tmp_pt);
    tmp_pt = 0;
    states = CML_NONE;
    cout << "finished atom" << endl;
  }
  if (qName.upper() == "BOND") {
    tmp_bond->setPoints(ep1, ep2);
    localBonds.append(tmp_bond);
    tmp_bond = 0;
    states = CML_NONE;
    cout << "finished bond" << endl;
  }
  return TRUE;
}

bool CMLParser::characters( const QString &ch ) {
  cout << "char:" << ch << ":" << endl;
  if (states == CML_ATOM) {
    if (last_builtin == "ELEMENTTYPE") tmp_pt->element = ch;
    if (last_builtin == "X2") tmp_pt->x = ch.toDouble();
    if (last_builtin == "Y2") tmp_pt->y = ch.toDouble();
  }
  if (states == CML_BOND) {
    if (last_builtin == "ATOMREF") {
      for (tmp_pt = localPoints.first(); tmp_pt != 0;
	   tmp_pt = localPoints.next() ) {
	if (tmp_pt->id == ch) break;
      }
      if (ep1 == 0)
	ep1 = tmp_pt;
      else
	ep2 = tmp_pt;
    }
    if (last_builtin == "ORDER") tmp_bond->setOrder(ch.toInt());
    if (last_builtin == "STEREO") {
      if (ch == "H") tmp_bond->setOrder(7);
      if (ch == "W") tmp_bond->setOrder(5);
    }
  }
  return TRUE;
}

bool CMLParser::ignorableWhitespace( const QString &ch ) {
  cout << "ignored:" << ch << ":" << endl;
  return TRUE;
}

QPtrList<DPoint> CMLParser::getPoints() { return localPoints; }
QPtrList<Bond> CMLParser::getBonds() { return localBonds; }
