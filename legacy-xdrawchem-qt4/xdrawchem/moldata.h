// moldata.h -- class MolData knows how to calculate molecular weights
// and partial charges

#ifndef MOLDATA_H
#define MOLDATA_H

#include <QString>
#include <QChar>
#include <QStringList>
#include <QDebug>

class MolData
{
public:
    static double NameToMW(QString e)
    {
        if (e.toUpper() == QString("H")) return 1.00794;
        if (e.toUpper() == QString("HE")) return 4.0026;
        if (e.toUpper() == QString("LI")) return 6.941;
        if (e.toUpper() == QString("B")) return 10.811;
        if (e.toUpper() == QString("C")) return 12.011;
        if (e.toUpper() == QString("N")) return 14.0067;
        if (e.toUpper() == QString("O")) return 15.9994;
        if (e.toUpper() == QString("F")) return 18.9984;
        if (e.toUpper() == QString("NA")) return 22.98977;
        if (e.toUpper() == QString("MG")) return 24.305;
        if (e.toUpper() == QString("SI")) return 28.0855;
        if (e.toUpper() == QString("P")) return 30.97376;
        if (e.toUpper() == QString("S")) return 32.066;
        if (e.toUpper() == QString("CL")) return 35.4527;
        if (e.toUpper() == QString("K")) return 39.0983;
        if (e.toUpper() == QString("SE")) return 78.96;
        if (e.toUpper() == QString("BR")) return 79.904;
        if (e.toUpper() == QString("I")) return 126.9045;
        return 6.0;
    }

    static double MW(QString x)
    {
        qDebug() << "Request for MW of " << x;
        // parse this string
        QString iso;  // isotope MW
        QString thiselement;  // current element
        QString repeatnum;  // number of repeats
        int ptr = 0;  // cursor position (pointer) in x
        int isoflag = false;  // isotope-override normal MW lookup if MW specified
        double this_mw; // MW of this element
        int repeat = 1; // number of this atom
        double mw_out = 0.0;  // MW returned
        QStringList tokens;
        do {
            iso.remove(0,999);
            thiselement.remove(0,999);
            repeatnum.remove(0,999);
            // Check if token starts with a number
            if (x.at(ptr).isNumber() == true) { // read isotope value
                isoflag = true;
                iso.append(x.at(ptr));
                ptr++;
                if (x.at(ptr).isNumber() == true) {
                iso.append(x.at(ptr));
                ptr++;
                }
                if (x.at(ptr).isNumber() == true) {
                    iso.append(x.at(ptr));
                    ptr++;
                }
            }
            // ptr now points to first letter of element
            thiselement.append(x.at(ptr));
            ptr++;
            // if next letter is lowercase, add it too
            if (x.at(ptr).category() == QChar::Letter_Lowercase) {
                thiselement.append(x.at(ptr));
                ptr++;
            }
            // if next character is number, it's the subscript
            if (x.at(ptr).isNumber() == true) {
                repeatnum.append(x.at(ptr));
                ptr++;
                if (x.at(ptr).isNumber() == true) {
                    repeatnum.append(x.at(ptr));
                    ptr++;
                }
                if (x.at(ptr).isNumber() == true) {
                    repeatnum.append(x.at(ptr));
                    ptr++;
                }
            repeat = repeatnum.toInt();
            }
            // Move to next letter/number
            if (ptr < x.length()) {
                if (x.at(ptr).isSpace() == true) ptr++;
            }
            if (isoflag)
                this_mw = iso.toDouble();
            else
                this_mw = NameToMW(thiselement);
            // add to molecular weight
            mw_out = mw_out + (repeat * this_mw);
            isoflag = false;
            repeat = 1;
        } while (ptr < x.length());
        qDebug() << "MW = " << mw_out;
        return mw_out;
    }
    // How many bonds (hydrogens) should this element or group have?
    static int Hydrogens(QString x)
    {
        //qDebug() << "Request for bonds/hydrogens to |" << x << "|";
        // first check 'special' patterns
        if (x.toUpper().contains("TIPS") == 1) return 1;
        if (x.toUpper().contains("TMS") == 1) return 1;
        if (x.toUpper().contains("MS") == 1) return 1;
        if (x == "CO") return 2;
        if (x == "SO") return 2;
        if (x == "AcO") return 1;
        if (x == "MeO") return 1;
        if (x == "EtO") return 1;
        if (x == "TsO") return 1;
        if (x == "BnO") return 1;
        if (x == "BzO") return 1;
        if (x == "TBSO") return 1;
        if (x == "OAc") return 1;
        if (x == "OMe") return 1;
        if (x == "OEt") return 1;
        if (x == "OTs") return 1;
        if (x == "OBn") return 1;
        if (x == "OBz") return 1;
        if (x == "OTBS") return 1;
        if (x == "NO2") return 1;
        if (x == "O2N") return 1;
        if (x == "CH3") return 1;
        if (x == "OH") return 1;
        if (x == "HO") return 1;
        if (x == "CHO") return 1;
        if (x == "COOH") return 1;
        if (x == "HOOC") return 1;
        if (x == "CN") return 1;
        if (x == "NH2") return 1;
        if (x == "H2N") return 1;
        if (x == "NH") return 2;
        if (x == "HN") return 2;
        if (x == "O-") return 1;
        if (x == "-O") return 1;
        int flag = false;
        int ptr = 0;
        QString firstelement;  // element to consider
        // Find first letter
        do {
            if (x.at(ptr).isLetter() == false)
                ptr++;
            else
                flag = true;
        } while (flag == false);
        firstelement.append(x.at(ptr));
        ptr++;
        if ( ptr < x.length() && x.at( ptr ).category() == QChar::Letter_Lowercase) {
            firstelement.append(x.at(ptr));
            ptr++;
        }
        if (ptr < x.length() && x.at(ptr).category() == QChar::Letter_Lowercase) {
            firstelement.append(x.at(ptr));
            ptr++;
        }
        if (firstelement.toUpper() == QString("H")) return 1;
        if (firstelement.toUpper() == QString("HE")) return 0;
        if (firstelement.toUpper() == QString("LI")) return 1;
        if (firstelement.toUpper() == QString("B")) return 3;
        if (firstelement.toUpper() == QString("C")) return 4;
        if (firstelement.toUpper() == QString("N")) return 3;
        if (firstelement.toUpper() == QString("O")) return 2;
        if (firstelement.toUpper() == QString("F")) return 1;
        if (firstelement.toUpper() == QString("NA")) return 1;
        if (firstelement.toUpper() == QString("MG")) return 2;
        if (firstelement.toUpper() == QString("SI")) return 4;
        if (firstelement.toUpper() == QString("CL")) return 1;
        if (firstelement.toUpper() == QString("K")) return 1;
        if (firstelement.toUpper() == QString("SE")) return 2;
        if (firstelement.toUpper() == QString("BR")) return 1;
        if (firstelement.toUpper() == QString("I")) return 1;
        // these could cause problems
        if (firstelement.toUpper() == QString("P")) return 3;
        if (firstelement.toUpper() == QString("S")) return 2;
        // if not known...
        return 1;
    }

    static double partialCharge(QString /*ident*/, int /*method*/)
    {
        double ret_pc;

        ret_pc = 0.0;

        return ret_pc;
    }

    static double bondLength(int el1, int el2, int order)
    {
        // make el1 the smaller element (by atomic number)
        int swp;
        if (el1 > el2) { swp = el1; el1 = el2; el2 = swp; }

        if (order == 1) {
            if (el1 == 1) {
                if (el2 == 6) {
                    return 1.09;
                }
                if (el2 == 7) {
                    return 1.01;
                }
                if (el2 == 8) {
                    return 0.96;
                }
            }
            if (el1 == 6) {
                if (el2 == 6) {
                return 1.54;
                }
                if (el2 == 7) {
                return 1.47;
                }
                if (el2 == 8) {
                return 1.43;
                }
                if (el2 == 9) {
                return 1.35;
                }
                if (el2 == 16) {
                return 1.82;
                }
                if (el2 == 17) {
                return 1.77;
                }
                if (el2 == 35) {
                return 1.94;
                }
                if (el2 == 53) {
                return 2.14;
                }
            }
            if (el1 == 7) {
                if (el2 == 8) {
                }
            }
        }

        if (order == 2) {
            if (el1 == 6) {
                if (el2 == 6) {
                    return 1.34;
                }
                if (el2 == 7) {
                }
                if (el2 == 8) {
                }
            }
        }

        if (order == 3) {
            if (el1 == 6) {
                if (el2 == 6) {
                    return 1.20;
                }
                if (el2 == 7) {
                }
            }
        }

        return 1.54;
    }

    static double bondAngleDegrees(int el1, int neighbors)
    {
        if (el1 == 6) {
            if (neighbors == 1) return 0.0;
            if (neighbors == 2) return 180.0;
            if (neighbors == 3) return 120.0;
            if (neighbors == 4) return 109.47;
        }
        if (el1 == 7) {
            if (neighbors == 1) return 180.0;
            if (neighbors == 2) return 120.0;
            if (neighbors == 3) return 109.47;
        }
        if (el1 == 8) {
            if (neighbors == 1) return 180.0;
            if (neighbors == 2) return 120.0;
        }
        if (el1 == 14) {
            if (neighbors == 1) return 0.0;
            if (neighbors == 2) return 180.0;
            if (neighbors == 3) return 120.0;
            if (neighbors == 4) return 109.47;
        }
        if (el1 == 15) {
            if (neighbors == 1) return 180.0;
            if (neighbors == 2) return 120.0;
            if (neighbors == 3) return 109.47;
        }
        if (el1 == 16) {
            if (neighbors == 1) return 180.0;
            if (neighbors == 2) return 120.0;
        }

        return 109.47;
    }

    // important!  maht.h expects angles in radians!
    static double bondAngle(int el1, int neighbors)
    {
        double retvald, retval;
        retvald = bondAngleDegrees(el1, neighbors);
        retval = retvald * 3.14159 / 180.0;
        return retval;
    }
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
