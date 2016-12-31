// molecule_tools_2.cpp - Molecule's implementation of (more) functions

#include "graphdialog.h"
#include "molinfodialog.h"
#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "dpoint.h"
#include "defs.h"
//#include "namer.h"

// Preferences
extern Preferences preferences;

// Scale the molecule.  Should be used when loading the file.
// bond_length sets the AVERAGE bond length.  bond_length < 0 means
// set according to current preferences.
void Molecule::Scale( double bond_length )
{
    double bond_sum = 0.0, bond_avg = 0.0, sf = 1.0;
    int nbonds = 0;

    // calculate current average bond length
    foreach ( tmp_bond, bonds ) {
        bond_sum += tmp_bond->Length();
        nbonds++;
    }
    bond_avg = bond_sum / ( double ) nbonds;
    if ( bond_length < 0.0 ) {
        bond_length = preferences.getBond_fixedlength();
    }
    sf = bond_length / bond_avg;

    double topedge = 9999.0, leftedge = 9999.0;

    up = AllPoints();
    foreach ( tmp_pt, up ) {
        if ( tmp_pt->x < leftedge )
            leftedge = tmp_pt->x;
        if ( tmp_pt->y < topedge )
            topedge = tmp_pt->y;
    }
    foreach ( tmp_pt, up ) {
        tmp_pt->x = ( ( tmp_pt->x - leftedge ) * sf ) + ( leftedge * sf );
        tmp_pt->y = ( ( tmp_pt->y - topedge ) * sf ) + ( topedge * sf );
    }
}

QList < DPoint * >Molecule::BreakRingBonds( DPoint * target1 )
{
    QList < DPoint * >bb;

    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Find( target1 ) == true ) {
            tmp_pt = tmp_bond->otherPoint( target1 );
            tmp_pt->new_order = tmp_bond->Order();
            bb.append( tmp_pt );
            bonds.removeAll( tmp_bond );
            tmp_bond = bonds.first();
        }
    }

    return bb;
}

DPoint *Molecule::GetAttachPoint( QString sf )
{
    if ( ( sf.contains( "fmoc" ) > 0 ) || ( sf.contains( "boc" ) > 0 ) || ( sf.contains( "dabcyl" ) > 0 ) || ( sf.contains( "dabsyl" ) > 0 ) || ( sf.contains( "dansyl" ) > 0 ) ) {
        up = AllPoints();
        foreach ( tmp_pt, up ) {
            if ( tmp_pt->element == "Cl" ) {
                qDebug() << "Point:Cl";
                tmp_pt->element = "C";
                break;
            }
        }
        foreach ( tmp_text, labels ) {
            if ( tmp_text->Start() == tmp_pt ) {
                qDebug() << "removed";
                labels.removeAll( tmp_text );
                break;
            }
        }
        qDebug() << tmp_pt->element;
        return tmp_pt;
    }
    if ( ( sf.contains( "edans" ) > 0 ) ) {
        up = AllPoints();
        foreach ( tmp_pt, up ) {
            if ( tmp_pt->element == "NH2" ) {
                qDebug() << "Point:NH2";
                tmp_pt->element = "C";
                break;
            }
        }
        foreach ( tmp_text, labels ) {
            if ( tmp_text->Start() == tmp_pt ) {
                qDebug() << "removed";
                labels.removeAll( tmp_text );
                break;
            }
        }
        qDebug() << tmp_pt->element;
        return tmp_pt;
    }
    if ( ( sf.contains( "biotin" ) > 0 ) ) {
        up = AllPoints();
        foreach ( tmp_pt, up ) {
            if ( tmp_pt->element == "OH" ) {
                qDebug() << "Biotin-Point:OH";
                tmp_pt->element = "C";
                break;
            }
        }
        foreach ( tmp_text, labels ) {
            if ( tmp_text->Start() == tmp_pt ) {
                qDebug() << "removed";
                labels.removeAll( tmp_text );
                break;
            }
        }
        qDebug() << tmp_pt->element;
        return tmp_pt;
    }
    // end biotin

    // for amino acids
    double ymax = 0.0;
    DPoint *ymaxpt = 0;

    up = AllPoints();
    foreach ( tmp_pt, up ) {
        if ( tmp_pt->element.contains( "N" ) > 0 ) {
            qDebug() << "Point:NH2";
            if ( tmp_pt->y > ymax ) {
                ymaxpt = tmp_pt;
                ymax = tmp_pt->y;
            }
        }
    }
    ymaxpt->element = "C";
    foreach ( tmp_text, labels ) {
        if ( tmp_text->Start() == ymaxpt ) {
            qDebug() << "removed";
            labels.removeAll( tmp_text );
            break;
        }
    }
    qDebug() << ymaxpt->element;
    return ymaxpt;
}

DPoint *Molecule::GetRingAttachPoint()
{
    double ymin = 99999.0;
    DPoint *yminpt = 0;

    up = AllPoints();
    foreach ( tmp_pt, up ) {
        if ( tmp_pt->y < ymin ) {
            yminpt = tmp_pt;
            ymin = tmp_pt->y;
        }
    }

    return yminpt;
}

double Molecule::CalculateRingAttachAngle( DPoint * t1 )
{
    double dx = 0.0, dy = 0.0;

    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->Find( t1 ) == true ) {
            tmp_pt = tmp_bond->otherPoint( t1 );
            dx = dx + ( tmp_pt->x - t1->x );
            dy = dy + ( tmp_pt->y - t1->y );
            break;
        }
    }

    double ang = atan( dy / dx );

    if ( dx > 0.0 )
        ang = ang + 3.14159;

    return ang;
}

// return sum of gas-phase bond enthalpy
double Molecule::SumBondEnthalpy()
{
    double dh = 0.0;

    foreach ( tmp_bond, bonds ) {
        dh += tmp_bond->Enthalpy();
    }

    return dh;
}

// set group information
void Molecule::setGroupType( int g1 )
{
    group_type = g1;
}

// build DPoint::neighbors list(s)
void Molecule::AllNeighbors()
{
    int n1, n2, n3;

    QList < DPoint * >groupAtoms = AllPoints();

    foreach ( tmp_pt, groupAtoms ) {
        n1 = 0;
        n2 = 0;
        n3 = 0;
        tmp_pt->neighbors.clear();
        tmp_pt->aromatic = false;
        tmp_pt->inring = false;
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) == true ) {
                if ( tmp_bond->Order() == 1 )
                    n1++;
                if ( tmp_bond->Order() == 5 )
                    n1++;
                if ( tmp_bond->Order() == 7 )
                    n1++;
                if ( tmp_bond->Order() == 2 )
                    n2++;
                if ( tmp_bond->Order() == 3 )
                    n3++;
                tmp_pt->neighbors.append( tmp_bond->otherPoint( tmp_pt ) );
                tmp_pt->bondorder[( tmp_pt->neighbors.count() - 1 )] = tmp_bond->Order();
            }
        }
        tmp_pt->hybrid = "unknown";
        if ( ( n1 == 1 ) && ( n2 == 0 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp3";
        if ( ( n1 == 2 ) && ( n2 == 0 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp3";
        if ( ( n1 == 3 ) && ( n2 == 0 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp3";
        if ( ( n1 == 4 ) && ( n2 == 0 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp3";
        if ( ( n1 == 5 ) && ( n2 == 0 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp3d2";
        if ( ( n1 == 6 ) && ( n2 == 0 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp3d2";
        if ( ( n1 == 0 ) && ( n2 == 1 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp2";
        if ( ( n1 == 1 ) && ( n2 == 1 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp2";
        if ( ( n1 == 2 ) && ( n2 == 1 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp2";
        if ( ( n1 == 0 ) && ( n2 == 2 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "sp";
        if ( ( n1 == 0 ) && ( n2 == 0 ) && ( n3 == 1 ) )
            tmp_pt->hybrid = "sp";
        if ( ( n1 == 1 ) && ( n2 == 0 ) && ( n3 == 1 ) )
            tmp_pt->hybrid = "sp";
        if ( ( n1 == 2 ) && ( n2 == 2 ) && ( n3 == 0 ) )
            tmp_pt->hybrid = "spd2";
    }
}

// invoke from chemdata_tools.cpp
// revised: now invoked from reactivity functions in Molecule
// revised again:  moved to static function in MolData class
double Molecule::CalcPartialCharge( QString atomtype )
{
    return 0.0;
}

Text *Molecule::CalcEmpiricalFormula( bool from_mw )
{
    QList < DPoint * >up;
    QString ef;
    QStringList allatoms, oddatoms;

    up = AllPoints();
    // Split all labels into allatoms (one atom per entry)
    foreach ( tmp_pt, up ) {
        // parse this string
        QString x = tmp_pt->plainElement();
        QString iso;            // isotope MW
        QString thiselement;    // current element
        QString repeatnum;      // number of repeats
        int ptr = 0;            // cursor position (pointer) in x
        int isoflag = false;    // isotope-override normal MW lookup if MW specified
        int repeat = 1;         // number of this atom

        do {
            iso.remove( 0, 999 );
            thiselement.remove( 0, 999 );
            repeatnum.remove( 0, 999 );
            // Check if token starts with a number
            if ( x.at( ptr ).isNumber() == true ) {   // read isotope value
                isoflag = true;
                iso.append( x.at( ptr ) );
                ptr++;
                if ( x.at( ptr ).isNumber() == true ) {
                    iso.append( x.at( ptr ) );
                    ptr++;
                }
                if ( x.at( ptr ).isNumber() == true ) {
                    iso.append( x.at( ptr ) );
                    ptr++;
                }
            }
            // ptr now points to first letter of element
            thiselement.append( x.at( ptr ) );
            ptr++;
            qDebug() << x << " " << ptr;
            // if next letter is lowercase, add it too
            if ( ptr < x.length() && x.at( ptr ).category() == QChar::Letter_Lowercase ) {
                thiselement.append( x.at( ptr ) );
                ptr++;
            }
            // if next character is number, it's the subscript
            if ( ptr < x.length() && x.at( ptr ).isNumber() ) {
                repeatnum.append( x.at( ptr ) );
                ptr++;
                if ( ptr < x.length() && x.at( ptr ).isNumber() ) {
                    repeatnum.append( x.at( ptr ) );
                    ptr++;
                }
                if ( ptr < x.length() && x.at( ptr ).isNumber() ) {
                    repeatnum.append( x.at( ptr ) );
                    ptr++;
                }
                repeat = repeatnum.toInt();
            }
            // Move to next letter/number
            if ( ptr < x.length() && x.at( ptr ).isSpace() ) {
                    ptr++;
            }
            // add atoms to list
            for ( int cc = 0; cc < repeat; cc++ )
                allatoms.append( thiselement );
            isoflag = false;
            repeat = 1;
        } while ( ptr < x.length() );
    }
    // need to find implicit hydrogens here!
    int num_c = 0, num_h = 0, num_n = 0, num_o = 0, num_p = 0, num_s = 0;

    foreach ( tmp_pt, up ) {
        int possible_h = 0;

        //qDebug() << "CalcEF:" << tmp_pt->element;
        possible_h = MolData::Hydrogens( tmp_pt->plainElement() );
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) )
                possible_h -= tmp_bond->Order();
        }
        if ( possible_h < 0 ) {
            qDebug() << tmp_pt->element << " resulted in negative hydrogens ";
            qDebug() << "in Molecule::CalcEmpiricalFormula()";
            possible_h = 0;
        }
        num_h += possible_h;
    }

    // convert allatoms to formula: first extract C,H,O,N,P,S
    // calculate molecular weight too
    nmw = ( double ) num_h *1.00794;

    for ( QStringList::Iterator it = allatoms.begin(); it != allatoms.end(); ++it ) {
        nmw += MolData::NameToMW( *it );
        if ( *it == "C" ) {
            num_c++;
            continue;
        }
        if ( *it == "H" ) {
            num_h++;
            continue;
        }
        if ( *it == "O" ) {
            num_o++;
            continue;
        }
        if ( *it == "N" ) {
            num_n++;
            continue;
        }
        if ( *it == "P" ) {
            num_p++;
            continue;
        }
        if ( *it == "S" ) {
            num_s++;
            continue;
        }
        oddatoms.append( *it );
    }

    //qDebug() << "Found " << oddatoms.count() << " odd atoms." ;
    // sort odd atom list alphabetically (if necessary)
    if ( oddatoms.count() > 0 )
        oddatoms.sort();

    QString finalef, n1;

    if ( num_c > 0 ) {
        n1.setNum( num_c );
        finalef = finalef + "C";
        if ( num_c > 1 )
            finalef = finalef + n1;
    }
    if ( num_h > 0 ) {
        n1.setNum( num_h );
        finalef = finalef + "H";
        if ( num_h > 1 )
            finalef = finalef + n1;
    }
    if ( num_o > 0 ) {
        n1.setNum( num_o );
        finalef = finalef + "O";
        if ( num_o > 1 )
            finalef = finalef + n1;
    }
    if ( num_n > 0 ) {
        n1.setNum( num_n );
        finalef = finalef + "N";
        if ( num_n > 1 )
            finalef = finalef + n1;
    }
    if ( num_p > 0 ) {
        n1.setNum( num_p );
        finalef = finalef + "P";
        if ( num_p > 1 )
            finalef = finalef + n1;
    }
    if ( num_s > 0 ) {
        n1.setNum( num_s );
        finalef = finalef + "S";
        if ( num_s > 1 )
            finalef = finalef + n1;
    }
    // add odd atoms
    if ( oddatoms.count() > 0 ) {
        do {
            QString te = oddatoms.first();
            int tc = 0;

            for ( QStringList::Iterator ir = oddatoms.begin(); ir != oddatoms.end(); ++ir ) {
                if ( *ir == te )
                    tc++;
            }
            finalef = finalef + te;
            n1.setNum( tc );
            if ( tc > 1 )
                finalef = finalef + n1;
            oddatoms.removeAll( te );
        } while ( oddatoms.count() > 0 );
    }
    // these are used for elemental analysis, don't delete
    nc = num_c;
    nh = num_h;
    nn = num_n;
    no = num_o;

    double nx, ny;

    QRect nr = BoundingBoxAll();

    ny = nr.top() - 16.0;
    nx = ( nr.left() + nr.right() ) / 2.0;

    tmp_text = new Text( r );
    tmp_pt = new DPoint( nx, ny );
    tmp_text->setPoint( tmp_pt );
    tmp_text->setJustify( JUSTIFY_TOPLEFT );
    tmp_text->setText( finalef );
    for ( int c = 0; c < finalef.length(); c++ ) {
        if ( finalef[c].isLetter() )
            finalef.replace( c, 1, " " );
        if ( finalef[c].isNumber() )
            finalef.replace( c, 1, "-" );
    }
//    tmp_text->setTextMask( finalef );

    //if (from_mw == false) {
    //  text_formula = tmp_text;
    //  tmp_text->setMolecule(this);
    //  tmp_text->setDataType(TEXT_DATA_FORMULA);
    //}

    return tmp_text;
}

Text *Molecule::CalcMW( bool from_change )
{
    Text *tmp_txt1;

    tmp_txt1 = CalcEmpiricalFormula( true );

    double nx, ny;

    QRect nr = BoundingBoxAll();

    ny = nr.bottom() + 5.0;
    nx = ( nr.left() + nr.right() ) / 2.0;

    tmp_text = new Text( r );
    tmp_pt = new DPoint( nx, ny );
    tmp_text->setPoint( tmp_pt );
    tmp_text->setJustify( JUSTIFY_TOPLEFT );
    QString finalmw;

    finalmw.setNum( nmw );
    finalmw = QString( "MW = " ) + finalmw;
    tmp_text->setText( finalmw );
    finalmw.fill( ' ' );
//    tmp_text->setTextMask( finalmw );

    //if (from_change == false) {
    //  text_mw = tmp_text;
    //  tmp_text->setMolecule(this);
    //  tmp_text->setDataType(TEXT_DATA_MW);
    //}

    return tmp_text;
}

Text *Molecule::CalcElementalAnalysis( bool show_dialog )
{
    QString ea, n1;

    // calc empirical formula and total molecular weight
    tmp_text = CalcMW();
    nc *= 12.011;
    nh *= 1.00794;
    nn *= 14.0067;
    no *= 15.994;
    nc = nc * 100 / nmw;
    nh = nh * 100 / nmw;
    no = no * 100 / nmw;
    nn = nn * 100 / nmw;

    ea.append( tr( "Elemental analysis:\n" ) );
    ea.append( "C = " );
    n1.setNum( nc );
    ea.append( n1 );
    ea.append( "%\n" );
    ea.append( "H = " );
    n1.setNum( nh );
    ea.append( n1 );
    ea.append( "%\n" );
    ea.append( "O = " );
    n1.setNum( no );
    ea.append( n1 );
    ea.append( "%\n" );
    ea.append( "N = " );
    n1.setNum( nn );
    ea.append( n1 );
    ea.append( "%" );

    /*
       int m1;
       if (show_dialog == true) {
       m1 = QMessageBox::information(r, tr("Elemental analysis"), ea, tr("Paste"), tr("OK"),
       QString::null, 1, 1);
       } else {
       m1 = 0;
       }
     */

    double nx, ny;

    QRect nr = BoundingBoxAll();

    ny = nr.top() + 8.0;
    nx = nr.right() + 16.0;

    tmp_text = new Text( r );
    //CHECK_PTR(tmp_text);
    tmp_pt = new DPoint( nx, ny );
    tmp_text->setPoint( tmp_pt );
    //qDebug() << ea;
    tmp_text->setJustify( JUSTIFY_TOPLEFT );
    tmp_text->setText( ea );
    ea.fill( ' ' );
//    tmp_text->setTextMask( ea );
    return tmp_text;
}

void Molecule::AddPeak( double d1, QString s1, QString s2 )
{
    tmp_peak = new Peak;
    tmp_peak->value = d1;
    tmp_peak->multiplicity = 1;
    tmp_peak->intensity = 80;
    if ( s2.contains( "broad" ) > 0 )
        tmp_peak->intensity = 40;
    if ( s2.contains( "narrow" ) > 0 )
        tmp_peak->intensity = 120;
    tmp_peak->comment = s2;
    foreach ( Peak * tpeak, peaklist ) {
        if ( ( tpeak->value == tmp_peak->value ) && ( tpeak->multiplicity == tmp_peak->multiplicity ) ) {
            tpeak->intensity += 1;
            delete tmp_peak;

            tmp_peak = 0;
            break;
        }
    }
    if ( tmp_peak != 0 )
        peaklist.append( tmp_peak );
}

// add hydrogens to atoms.  Only add to carbon if to_carbons == true
void Molecule::AddHydrogens( bool to_carbon )
{
    qInfo() << "AddHydrogens(" << to_carbon << "), prefs = " << preferences.getFixHydrogens();
    up = AllPoints();
    int h = 0, sumbonds = 0;
    double dx;
    QString hnum, orig_element;
    int least_hindered_side;

    // calculate hindrance first
    foreach ( tmp_pt, up ) {
        // find order of bonds
        least_hindered_side = 0; sumbonds = 0;
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) ) {
                dx = tmp_pt->x - tmp_bond->otherPoint( tmp_pt )->x;
                if ( dx > 0.5 )
                    least_hindered_side++;
                if ( dx < -0.5 )
                    least_hindered_side--;
            }
        }
        // save # of bonds found
        qInfo() << tmp_pt->toQPoint() << "(" << tmp_pt->element << "): found substituents = " << sumbonds;
        tmp_pt->substituents = sumbonds;
        tmp_pt->C13_shift = least_hindered_side;
        // update Text, if it exists, with least hindered side
        foreach ( tmp_text, labels ) {
            if ( tmp_text->Start() == tmp_pt ) {
                if ( least_hindered_side < 0 )
                    tmp_text->CheckAlignment( 2 );      // left hindered
                else
                    tmp_text->CheckAlignment( 1 );      // right hindered
            }
        }
    }
    // add hydrogens if user requested
    if ( preferences.getFixHydrogens() == false )
        return;
    foreach ( tmp_pt, up ) {
        orig_element = tmp_pt->element;
        if ( tmp_pt->element == "" )
            tmp_pt->element = "C";
        sumbonds = 0;
        least_hindered_side = 0;
        h = 0;
        // don't add to carbons unless specifically instructed
        if ( ( tmp_pt->element == "C" ) && ( to_carbon == false ) )
            continue;
        // skip special cases
        if ( tmp_pt->element == "CO" )
            continue;
        if ( tmp_pt->element == "SO" )
            continue;
        // don't do fragments with charges
        if ( tmp_pt->element.contains( "+" ) > 0 )
            continue;
        if ( tmp_pt->element.contains( "-" ) > 0 )
            continue;
        // N typically needs correcting...
        if ( tmp_pt->element == "HN" )
            tmp_pt->element = "N";
        if ( tmp_pt->element == "NH" )
            tmp_pt->element = "N";
        if ( tmp_pt->element == "H2N" )
            tmp_pt->element = "N";
        if ( tmp_pt->element == "NH2" )
            tmp_pt->element = "N";
        if ( tmp_pt->element == "H3N" )
            tmp_pt->element = "N";
        if ( tmp_pt->element == "NH3" )
            tmp_pt->element = "N";
        // so does O
        if ( tmp_pt->element == "HO" )
            tmp_pt->element = "O";
        if ( tmp_pt->element == "OH" )
            tmp_pt->element = "O";
        if ( tmp_pt->element == "H2O" )
            tmp_pt->element = "O";
        if ( tmp_pt->element == "OH2" )
            tmp_pt->element = "O";
        // let's do thiols too, so H ends up on proper side
        if ( tmp_pt->element == "HS" )
            tmp_pt->element = "S";
        if ( tmp_pt->element == "SH" )
            tmp_pt->element = "S";
        // retrieve # of bonds found
        sumbonds = 0;//tmp_pt->substituents;
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) ) {
                if (tmp_bond->Order() < 4) sumbonds += tmp_bond->Order();
                if (tmp_bond->Order() >= 4) sumbonds += 1;
            }
        }
        least_hindered_side = ( int ) tmp_pt->C13_shift;
        // don't add if hydrogen already present
        if ( tmp_pt->element.contains( "H" ) == 0 ) {
            h = MolData::Hydrogens( tmp_pt->element ) - sumbonds;
            qInfo() << "Found hydrogens: " << h;
            if ( h > 1 )
                hnum.setNum( h );
            else
                hnum = "";
            if ( h > 0 ) {
                if ( least_hindered_side < 0 )
                    tmp_pt->element.prepend( "H" + hnum );
                else
                    tmp_pt->element.append( "H" + hnum );
            }
        }
        // now find Text which reference this DPoint and copy back
        QString elbackup;

        if ( orig_element != tmp_pt->element ) {
            foreach ( tmp_text, labels ) {
                if ( tmp_text->Start() == tmp_pt ) {
                    elbackup = tmp_pt->element;
                    qDebug() << tmp_pt->element;
                    tmp_text->setText( tmp_pt->element );
                    // *sigh* I suppose the least I can do is compute a proper textmask
                    elbackup.fill( ' ' );
                    if ( h > 1 ) {
                        if ( least_hindered_side < 0 ) {
                            elbackup.replace( 1, 1, "-" );
                        } else {
                            elbackup.replace( tmp_pt->element.length() - 1, 1, "-" );
                        }
                    }
//                    tmp_text->setTextMask( elbackup );
                    //tmp_pt->element = elbackup;
                }               // if (tmp_text->...)
            }                   // for(tmp_text...)
        }                       // if (orig...)
    }
}

// reactivity:  use this for doing partial charges/VdW for now
// int argument specifies analysis type
void Molecule::Reactivity( int react_type )
{
    qDebug() << "Reactivity(" << react_type << ")";
    // get list of unique points
    up = AllPoints();
    // find aromatic rings
    MakeSSSR();
    // solve HOSE codes
    Calc13CNMR( false );
    // add hydrogens
    qDebug() << bonds.count();
    AddNMRprotons();
    up = AllPoints();         // need to re-run after adding protons
    qDebug() << bonds.count();

    // reactivity... build list of all atom and bond retro-types
    QString localatomtype;
    double sumcharge = 0.0;

    atomRxns.clear();
    bondRxns.clear();
    foreach ( DPoint * tmp_atomrxniter, up ) {
        localatomtype = RetroAtomName( tmp_atomrxniter );
        atomRxns.append( localatomtype );
        // set partial charges here
        tmp_atomrxniter->react_atom_type = localatomtype;
        tmp_atomrxniter->partial_charge = CalcPartialCharge( localatomtype );
        sumcharge += MolData::partialCharge( localatomtype, 0 );
    }
    qDebug() << "Calc total charge: " << sumcharge;
    qDebug() << "Atom count: " << atomRxns.count();
    foreach ( Bond * tmp_bondRxns, bonds ) {
        if ( tmp_bondRxns->isCHBond() == false ) {
            bondRxns.append( RetroBondName( tmp_bondRxns ) );
            tmp_bondRxns->setCName( RetroBondName( tmp_bondRxns ) );
            //bondrxniter.current()->setReactions(BuildReactionList(bondrxniter.current()->CName()));
        }
    }
    qDebug() << "Bond count: " << bondRxns.count();

    // do stuff, based on value passed in arg
    if ( react_type == MODE_TOOL_REACTIVITY_FORWARD ) {
        // for reactivity, we should look at atom types first, since atoms will
        // be the likely target of forward reactions.
        qDebug() << "Atom type list for FORWARD:";
        for ( QStringList::Iterator it = atomRxns.begin(); it != atomRxns.end(); ++it ) {
            qDebug() << *it;
        }
        qDebug() << "--- list ends ---";
        // we will also have to consider other atom types and bonds that may
        // interfere with a given reaction
    }
    if ( react_type == MODE_TOOL_REACTIVITY_RETRO ) {
        // for retrosynthesis, we are more interested in bonds, to se how they
        // might have been formed
        qDebug() << "Bond type list for RETRO:";
        for ( QStringList::Iterator it = bondRxns.begin(); it != bondRxns.end(); ++it ) {
            qDebug() << *it;
        }
        qDebug() << "--- list ends ---";
        // we will also have to consider R group substitution here.
        // call Molecule::Retro() (in retro.cpp) to do matching
        int nr = Retro();
    }
    if ( react_type == MODE_TOOL_CHARGES ) {
        // just look at atom types and assign AMBER-like partial charges
        qDebug() << "Sum of charges = " << sumcharge;
    }
    // remove added hydrogens
    RemoveNMRprotons();
    qDebug() << bonds.count();
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
