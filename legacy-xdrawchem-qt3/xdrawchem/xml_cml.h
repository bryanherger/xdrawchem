/****************************************************************************
** $Id: xml_cml.h 148 2005-05-18 01:06:37Z bherger $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef XML_CML_H
#define XML_CML_H

#include <qxml.h>
#include <qptrlist.h>

#include "drawable.h"
#include "dpoint.h"
#include "bond.h"
#include "render2d.h"

class QString;

// possible states (CMLParser::states)
// set in defs.h
//#define CML_NONE 0
//#define CML_ATOM 1
//#define CML_BOND 2

class CMLParser : public QXmlDefaultHandler
{
public:
  CMLParser(Render2D *r1) { r = r1; }
    bool startDocument();
    bool startElement( const QString&, const QString&, const QString& ,
                       const QXmlAttributes& );
    bool endElement( const QString&, const QString&, const QString& );
    bool characters( const QString& );
    bool ignorableWhitespace( const QString& );
    QPtrList<DPoint> getPoints();
    QPtrList<Bond> getBonds();

private:
    QPtrList<DPoint> localPoints;
    QPtrList<Bond> localBonds;
    DPoint *tmp_pt, *ep1, *ep2;
    Bond *tmp_bond;
    Render2D *r;
    QString indent, last_builtin;
    int states;
};

#endif

