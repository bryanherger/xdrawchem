/****************************************************************************
** $Id: xml_cml.cpp,v 1.2 2005/05/18 01:06:37 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <QList>

#include "xml_cml.h"
#include "defs.h"

bool CMLParser::startDocument()
{
    qDebug() << "New CML parser started.";
    return TRUE;
}

bool CMLParser::startElement( const QString &, const QString &, const QString & qName, const QXmlAttributes & attr )
{
    qDebug() << "Start:" << qName;
    if ( qName.toUpper() == "ATOM" ) {
        states = CML_ATOM;
        tmp_pt = new DPoint;
        tmp_pt->id = attr.value( "id" );
        qDebug() << "Atom id=" << attr.value( "id" );
    }
    if ( qName.toUpper() == "BOND" ) {
        states = CML_BOND;
        tmp_bond = new Bond( r );
        tmp_bond->setID( attr.value( "id" ) );
        ep1 = 0;
        ep2 = 0;
    }
    if ( qName.toUpper() == "FLOAT" ) {
        last_builtin = attr.value( "builtin" ).toUpper();
        if ( last_builtin == "X3" )
            last_builtin = "X2";
        if ( last_builtin == "X3" )
            last_builtin = "Y2";
    }
    if ( qName.toUpper() == "STRING" ) {
        last_builtin = attr.value( "builtin" ).toUpper();
    }
    return TRUE;
}

bool CMLParser::endElement( const QString &, const QString &, const QString & qName )
{
    qDebug() << "End:" << qName;
    if ( qName.toUpper() == "ATOM" ) {
        localPoints.append( tmp_pt );
        tmp_pt = 0;
        states = CML_NONE;
        qDebug() << "finished atom";
    }
    if ( qName.toUpper() == "BOND" ) {
        tmp_bond->setPoints( ep1, ep2 );
        localBonds.append( tmp_bond );
        tmp_bond = 0;
        states = CML_NONE;
        qDebug() << "finished bond";
    }
    return TRUE;
}

bool CMLParser::characters( const QString & ch )
{
    qDebug() << "char:" << ch << ":";
    if ( states == CML_ATOM ) {
        if ( last_builtin == "ELEMENTTYPE" )
            tmp_pt->element = ch;
        if ( last_builtin == "X2" )
            tmp_pt->x = ch.toDouble();
        if ( last_builtin == "Y2" )
            tmp_pt->y = ch.toDouble();
    }
    if ( states == CML_BOND ) {
        if ( last_builtin == "ATOMREF" ) {
            foreach ( tmp_pt, localPoints ) {
                if ( tmp_pt->id == ch )
                    break;
            }
            if ( ep1 == 0 )
                ep1 = tmp_pt;
            else
                ep2 = tmp_pt;
        }
        if ( last_builtin == "ORDER" )
            tmp_bond->setOrder( ch.toInt() );
        if ( last_builtin == "STEREO" ) {
            if ( ch == "H" )
                tmp_bond->setOrder( 7 );
            if ( ch == "W" )
                tmp_bond->setOrder( 5 );
        }
    }
    return TRUE;
}

bool CMLParser::ignorableWhitespace( const QString & ch )
{
    qDebug() << "ignored:" << ch << ":";
    return TRUE;
}

QList < DPoint * >CMLParser::getPoints()
{
    return localPoints;
}

QList < Bond * >CMLParser::getBonds()
{
    return localBonds;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
