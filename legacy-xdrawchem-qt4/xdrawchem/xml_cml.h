/****************************************************************************
** $Id: xml_cml.h,v 1.2 2005/05/18 01:06:37 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef XML_CML_H
#define XML_CML_H

/*
#include <qxml.h>
#include <q3ptrlist.h>
*/

#include <QXmlDefaultHandler>

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
    CMLParser( Render2D *r1 ) { r = r1; }
    bool startDocument();
    bool startElement( const QString &, const QString &, const QString &,
                       const QXmlAttributes & );
    bool endElement( const QString &, const QString &, const QString & );
    bool characters( const QString & );
    bool ignorableWhitespace( const QString & );
    QList<DPoint *> getPoints();
    QList<Bond *> getBonds();

private:
    QList<DPoint *> localPoints;
    QList<Bond *> localBonds;
    DPoint *tmp_pt, *ep1, *ep2;
    Bond *tmp_bond;
    Render2D *r;
    QString indent, last_builtin;
    int states;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
