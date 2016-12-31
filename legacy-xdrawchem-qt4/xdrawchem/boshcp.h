// boshcp.h -- A C++/Qt version of the Bremser C13 prediction table.
// See Bremser W, "Expectation Ranges of 13C NMR ChemicalShifts", Mag. Res.
// Chem. 23(4):271-275
// Derived from Java implementation by Christoph Steinbeck, see copyright
// notice below

/* BremserOneSphereHOSECodePredictor.java
 *
 * Copyright (C) 1997 - 2000  Dr. Christoph Steinbeck
 *
 * Contact: steinbeck@ice.mpg.de
 *
 * This software is published and distributed under artistic license.
 * The intent of this license is to state the conditions under which this Package
 * may be copied, such that the Copyright Holder maintains some semblance
 * of artistic control over the development of the package, while giving the
 * users of the package the right to use and distribute the Package in a
 * more-or-less customary fashion, plus the right to make reasonable modifications.
 *
 * THIS PACKAGE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES,
 * INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * The complete text of the license can be found in a file called LICENSE
 * accompanying this package.
 */

#ifndef BOSHCP_H
#define BOSHCP_H

#include <QString>
#include <QList>

class HOSECodeShiftRange
{
public:
    QString code;
    double shift;
    double confidenceLimit;
    QString multiplicity;
    double maxShift;
    double minShift;
    int fourSphereCount;
    int lineCount;
    bool valid;

    HOSECodeShiftRange() { valid = true; }

    HOSECodeShiftRange( QString c, double s, double conf, QString m,
                        double ma, double mi, int f, int l )
    {
        code = c;
        shift = s;
        confidenceLimit = conf;
        multiplicity = m;
        maxShift = ma;
        minShift = mi;
        fourSphereCount = f;
        lineCount = l;
        valid = true;
    }

    QString toString()
    {
        QString tmp;
        QString s( "delta_C: " );
        tmp.setNum( shift );
        s.append( tmp );
        s.append( "(" );
        s.append( multiplicity );
        s.append( ")" );
        //s.append(" for code ");
        //s.append(code);
        //s.append(" with confidence limit ");
        s.append( " +/- " );
        tmp.setNum( confidenceLimit );
        s.append( tmp );
        s.append( " in shift range " );
        tmp.setNum( maxShift );
        s.append( tmp );
        s.append( "-" );
        tmp.setNum( minShift );
        s.append( tmp );
        return s;
    }
};

class HashKey
{
public:
    HashKey() { }
    QString key;
    HOSECodeShiftRange data;
};

class PseudoHashTable
{
public:
    QList<HashKey *> table;
    HashKey *tmp_key;

    HOSECodeShiftRange get( QString n )
    {
        foreach ( tmp_key, table ) {
            if ( tmp_key->key == n )
                return tmp_key->data;
        }
        HOSECodeShiftRange ret;
        ret.valid = false;
        return ret;
    }

    void put( QString n, HOSECodeShiftRange r )
    {
        tmp_key = new HashKey;
        tmp_key->key = n;
        tmp_key->data = r;
        table.append(tmp_key);
    }

    bool containsKey(QString n)
    {
        foreach (tmp_key, table) {
            if (tmp_key->key == n)
                return true;
        }
        return false;
    }
};

class BremserOneSphereHOSECodePredictor
{
public:
    BremserOneSphereHOSECodePredictor() { prepareHashTable(); }

    double predict(QString hoseCode)
    {
        if (ht.containsKey(hoseCode))
            return ht.get(hoseCode).shift;
        return -1.0;
    }

    QString predictFull(QString hoseCode)
    {
        if (ht.containsKey(hoseCode))
            return ht.get(hoseCode).toString();
        return QString("");
    }

    QString getMult(QString hoseCode)
    {
        if (ht.containsKey(hoseCode))
            return ht.get(hoseCode).multiplicity;
        return QString("");
    }

    double getConfidenceLimit(QString hoseCode)
    {
        if (ht.containsKey(hoseCode))
            return ht.get(hoseCode).confidenceLimit;
        return -1.0;
    }

    QString getBibData()
    {
        QString s = "The carbon NMR chemical shift prediction of this module \n";
        s += "is based on the 651 1-sphere HOSE-Code table published by W. Bremser in:\n";
        s += "W. Bremser, \"Expectation Ranges of 13C NMR Chemical Shifts\", \n";
        s += "Mag. Res. Chem., Vol. 23, No. 4, 1985, 271-275.\n";
        s += "It is important to understand, that these values are indeed not more than expectation ranges.\n";
        return s;
    }

    void prepareHashTable();
    void prepTable2();

private:
    PseudoHashTable ht;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
