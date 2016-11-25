// molecule_1h_nmr.cpp - Molecule's implementation of (more) functions

#include "graphdialog.h"
#include "molinfodialog.h"
#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "dpoint.h"
#include "defs.h"

// SSSR (see comments in this file)
#include "molecule_sssr.h"

QStringList Molecule::Calc1HNMR( bool show_dialog )
{
    QStringList peak_list;
    int c1, c2, c3, mul, inten;
    double shift1;
    QString n1, nfull, ntemp;

    // get list of unique points
    up = AllPoints();
    // find aromatic rings
    MakeSSSR();
    // add hydrogens
    qDebug() << bonds.count();
    AddNMRprotons();
    up = AllPoints();         // need to re-run after adding protons
    qDebug() << bonds.count();
    // assign magnetic environment
    ProtonEnvironment();
    // determine multiplicity and chirality
    Multiplicity_1HNMR();
    // sort lists and calculate spectrum
    Calc1HMultiplicityAndIntensity();
    // remove added hydrogens
    RemoveNMRprotons();
    qDebug() << bonds.count();

    GraphDialog *g = new GraphDialog( r, tr( "Predicted 1H-NMR" ) );

    peaklist.clear();

    // print (for now) list of NMR values
    for ( QStringList::Iterator it = protonFinalList.begin(); it != protonFinalList.end(); ++it ) {
        ntemp = *it;
        c1 = ntemp.indexOf( ',' );
        c2 = ntemp.indexOf( ',', c1 + 1 );
        c3 = ntemp.indexOf( ',', c2 + 1 );
        shift1 = ntemp.mid( 0, c1 ).toDouble();
        nfull = "";
        n1.setNum( shift1 );
        nfull = n1 + " ppm";
        nfull = nfull + tr( " Intensity: " ) + ntemp.mid( c1 + 1, c2 - c1 - 1 );
        nfull = nfull + tr( " Multiplicity: " ) + ntemp.mid( c2 + 1, c3 - c2 - 1 );
        inten = ntemp.mid( c1 + 1, c2 - c1 - 1 ).toInt();
        mul = ntemp.mid( c2 + 1, c3 - c2 - 1 ).toInt();
        tmp_peak = new Peak;
        tmp_peak->value = shift1;
        tmp_peak->intensity = inten;
        tmp_peak->multiplicity = mul;
        tmp_peak->comment = nfull;
        peaklist.append( tmp_peak );
        g->AddPeak( shift1, mul, QColor( 0, 0, 0 ), QString( "peak" ), nfull );
    }
    if ( show_dialog == true )
        g->show();

    return protonFinalList;
}

void Molecule::AddNMRprotons()
{
    DPoint *new_proton;
    int h, ct, sumbonds;
    QString orig_element;
    double ox, oy;

    foreach ( tmp_pt, up ) {
        orig_element = tmp_pt->element;
        ox = tmp_pt->x;
        oy = tmp_pt->y;
        if ( orig_element == "H" )
            continue;
        if ( orig_element == "" )
            orig_element = "C";
        sumbonds = 0;
        h = 0;
        // remove protons and charges
        for ( ct = 0; ct < orig_element.length(); ct++ ) {
            if ( orig_element[ct] == 'H' ) {
                orig_element.remove( ct, 1 );
                ct = 0;
            }
            if ( orig_element[ct] == '+' ) {
                orig_element.remove( ct, 1 );
                ct = 0;
            }
            if ( orig_element[ct] == '-' ) {
                orig_element.remove( ct, 1 );
                ct = 0;
            }
            if ( orig_element[ct].isNumber() == true ) {
                orig_element.remove( ct, 1 );
                ct = 0;
            }
        }
        // find order of bonds
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) ) {
                if ( tmp_bond->Order() < 4 )
                    sumbonds += tmp_bond->Order();
                else
                    sumbonds += 1;      // order 5 and 7 (stereo) and order 1 bonds
            }
        }
        h = MolData::Hydrogens( orig_element ) - sumbonds;
        if ( h < 0 )
            h = 0;
        for ( ct = 0; ct < h; ct++ ) {
            tmp_bond = new Bond( r );
            new_proton = new DPoint;
            new_proton->element = "H";
            new_proton->nmr_proton = true;
            new_proton->x = ox;
            new_proton->y = oy;
            tmp_bond->setPoints( tmp_pt, new_proton );
            bonds.append( tmp_bond );
        }
    }
}

void Molecule::RemoveNMRprotons()
{
    foreach ( tmp_bond, bonds ) {
        if ( tmp_bond->End()->nmr_proton == true ) {
            bonds.removeAll( tmp_bond );
            delete tmp_bond;

            tmp_bond = bonds.first();
        }
    }
}

/// Do depth-first search to identify magnetic environment
/// Essentially, assign a HOSE-style code to each proton.
void Molecule::ProtonEnvironment()
{
    int adj_protons;
    DPoint *depth1_pt, *depth2_pt, *depth3_pt;

    QList < DPoint * >search_stack;     // depth 2 atoms
    QString nearest, r1, r2, r3, r_current, depth3_str;
    QStringList rgroups, depth3list;
    int rnext = 0;

    protonMagEnvList.clear();

    foreach ( tmp_pt, up ) {
        tmp_pt->protonHOSECode = "";
        rgroups.clear();
        r1 = "";
        r2 = "";
        r3 = "";
        r_current = "";
        rnext = 0;
        adj_protons = 1;
        tmp_pt->H1_multiplicity = 0;
        search_stack.clear();
        if ( tmp_pt->element != "H" )
            continue;
        /// first, find atom this proton is connected to (depth 1)
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) ) {
                depth1_pt = tmp_bond->otherPoint( tmp_pt );
                nearest = depth1_pt->baseElement();
                if ( ( depth1_pt->inring == true ) && ( depth1_pt->aromatic == true ) )
                    nearest.append( "A" );
                if ( ( depth1_pt->inring == true ) && ( depth1_pt->aromatic == false ) )
                    nearest.append( "R" );
                break;
            }
        }
        /// now, find depth 2 atoms and put in list
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( depth1_pt ) ) {
                depth2_pt = tmp_bond->otherPoint( depth1_pt );
                //qDebug() << "DEPTH2: "<< depth2_pt->element ;
                if ( depth2_pt == tmp_pt )
                    continue;
                search_stack.append( depth2_pt );
                depth2_pt->serial = tmp_bond->Order();
            }
        }
        /// determine R groups
        foreach ( depth2_pt, search_stack ) {
            r_current = "";
            if ( ( depth2_pt->serial == 2 ) && ( depth2_pt->aromatic == false ) )
                r_current = "=";
            if ( depth2_pt->serial == 3 )
                r_current = "#";
            r_current.append( depth2_pt->baseElement() );
            if ( depth2_pt->aromatic == true )
                r_current.append( "A" );
            depth3list.clear();
            foreach ( tmp_bond, bonds ) {
                if ( tmp_bond->Find( depth2_pt ) ) {
                    depth3_pt = tmp_bond->otherPoint( depth2_pt );
                    if ( depth3_pt == depth1_pt )
                        continue;
                    //qDebug() << "DEPTH3: "<< depth3_pt->element ;
                    depth3_str = "";
                    if ( ( tmp_bond->Order() == 2 ) && ( depth3_pt->aromatic == false ) )
                        depth3_str = "=";
                    if ( tmp_bond->Order() == 3 )
                        depth3_str = "#";
                    depth3_str.append( depth3_pt->baseElement() );
                    depth3list.append( depth3_str );
                }
            }
            depth3list.sort();
            for ( QStringList::Iterator it1 = depth3list.begin(); it1 != depth3list.end(); ++it1 ) {
                r_current.append( *it1 );
            }
            rgroups.append( r_current );
        }
        rgroups.sort();
        for ( QStringList::Iterator it2 = rgroups.begin(); it2 != rgroups.end(); ++it2 ) {
            nearest.append( "(" );
            nearest.append( *it2 );
            nearest.append( ")" );
            if ( r1 == "" ) {
                r1 = *it2;
                continue;
            }
            if ( r2 == "" ) {
                r2 = *it2;
                continue;
            }
            if ( r3 == "" ) {
                r3 = *it2;
                continue;
            }
        }
        // check for chirality
        if ( ( r1 != r2 ) && ( r2 != r3 ) && ( r1 != r3 ) && ( r1 != "H" ) && ( r1 != "" ) && ( r2 != "" ) && ( r3 != "" ) && ( r2 != "H" ) && ( r3 != "H" ) && ( nearest.left( 1 ) == "C" ) && ( nearest.left( 2 ) != "CA" ) ) {
            nearest.prepend( "*" );
        }
        tmp_pt->protonHOSECode = nearest;
        qDebug() << "env:" << nearest;
    }
}

/// Do depth-first search to find adjacent protons (H at depth 2 and 3)
void Molecule::Multiplicity_1HNMR()
{
    int adj_protons;
    bool split, chiral;
    DPoint *depth1_pt, *depth2_pt, *depth3_pt;
    QString new_magenv;

    QList < DPoint * >search_stack;     // depth 2 atoms

    foreach ( tmp_pt, up ) {
        /// chiral atoms should implicitly be split...
        if ( tmp_pt->protonHOSECode.count( "*" ) > 0 ) {
            protonMagEnvList.append( tmp_pt->protonHOSECode );
            continue;
        }
        adj_protons = 1;
        split = false;
        chiral = false;
        tmp_pt->H1_multiplicity = 0;
        search_stack.clear();
        if ( tmp_pt->element != "H" )
            continue;
        /// first, find atom this proton is connected to (depth 1)
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) ) {
                depth1_pt = tmp_bond->otherPoint( tmp_pt );
                break;
            }
        }
        /// now, find depth 2 atoms and put in list
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( depth1_pt ) ) {
                depth2_pt = tmp_bond->otherPoint( depth1_pt );
                //qDebug() << "DEPTH2: "<< depth2_pt->element ;
                if ( depth2_pt->element == "H" )
                    adj_protons += 0;
                else
                    search_stack.append( depth2_pt );
            }
        }
        //qDebug() << "Stack: " << search_stack.count() ;
        /// now find depth 3 protons
        foreach ( depth2_pt, search_stack ) {
            foreach ( tmp_bond, bonds ) {
                if ( tmp_bond->Find( depth2_pt ) ) {
                    depth3_pt = tmp_bond->otherPoint( depth2_pt );
                    //qDebug() << "DEPTH3: "<< depth3_pt->element ;
                    if ( depth3_pt->element == "H" ) {
                        adj_protons++;
                        if ( depth3_pt->protonHOSECode != tmp_pt->protonHOSECode )
                            split = true;
                    }
                }
            }
        }
        tmp_pt->H1_multiplicity = adj_protons;
        qDebug() << "1H_mult = " << adj_protons;
        new_magenv = tmp_pt->protonHOSECode;
        if ( split ) {
            new_magenv.prepend( "+" );
        } else {
            new_magenv.prepend( "-" );
        }
        protonMagEnvList.append( new_magenv );

        qDebug() << "Magnetic environment: " << new_magenv;
    }                           // for (tmp_pt...)
}

void Molecule::Calc1HMultiplicityAndIntensity()
{
    QString current_nmr, tmp_nmr, rev_nmr, n1, chiralnote;
    int int_count = 1, multi = 0;
    bool first_time = true;

    protonFinalList.clear();
    protonMagEnvList.sort();
    for ( QStringList::Iterator it2 = protonMagEnvList.begin(); it2 != protonMagEnvList.end(); ++it2 ) {
        tmp_nmr = *it2;
        if ( first_time == true ) {
            first_time = false;
            current_nmr = tmp_nmr;
            continue;
        }
        if ( tmp_nmr != current_nmr ) {
            rev_nmr = current_nmr;
            rev_nmr.replace( QRegExp( "\\(H\\)" ), "" );
            qDebug() << "rev_nmr = " << rev_nmr;
            multi = rev_nmr.count( 'H' ) + 1;
            if ( rev_nmr.count( '-' ) > 0 )
                multi = 1;
            // chirality?
            chiralnote = "";
            if ( rev_nmr.count( '*' ) > 0 ) {
                qDebug() << "Oh no! Chiral center!";
                chiralnote = " (chiral)";
            }
            rev_nmr = current_nmr;
            rev_nmr.prepend( "," );
            rev_nmr.prepend( chiralnote );
            n1.setNum( multi );
            rev_nmr.prepend( n1 );
            n1.setNum( int_count );
            rev_nmr.prepend( "," );
            rev_nmr.prepend( n1 );
            rev_nmr.prepend( "," );
            n1.setNum( Calc1HShift( current_nmr ) );
            rev_nmr.prepend( n1 );
            protonFinalList.append( rev_nmr );
            current_nmr = tmp_nmr;
            int_count = 1;
        } else {
            int_count++;
        }
    }
    rev_nmr = current_nmr;
    rev_nmr.replace( QRegExp( "\\(H\\)" ), "" );
    multi = rev_nmr.count( 'H' ) + 1;
    if ( rev_nmr.count( '-' ) > 0 )
        multi = 1;
    // chirality?
    chiralnote = "";
    if ( rev_nmr.count( '*' ) > 0 ) {
        qDebug() << "Oh no! Chiral center!";
        chiralnote = " (chiral)";
    }
    rev_nmr = current_nmr;
    rev_nmr.prepend( "," );
    rev_nmr.prepend( chiralnote );
    n1.setNum( multi );
    rev_nmr.prepend( n1 );
    n1.setNum( int_count );
    rev_nmr.prepend( "," );
    rev_nmr.prepend( n1 );
    rev_nmr.prepend( "," );
    n1.setNum( Calc1HShift( current_nmr ) );
    rev_nmr.prepend( n1 );
    protonFinalList.append( rev_nmr );
    current_nmr = tmp_nmr;
    int_count = 0;

    qDebug() << endl << "Proton final list:";
    for ( QStringList::Iterator it = protonFinalList.begin(); it != protonFinalList.end(); ++it ) {
        qDebug() << *it;
    }
}

//
// everything below this point probably belongs in a separate file.
//
// proton types used in identification
#define PROTON_CH 10
#define PROTON_CH_AROMATIC 20
#define PROTON_CH2 30
#define PROTON_CH2_RING 40
#define PROTON_CH3 50
#define PROTON_NH 60
#define PROTON_NH2 70
#define PROTON_OH 80
#define PROTON_SH 90

double Molecule::Calc1HShift( QString phc )
{
    QString substr, nearest, r1, r2, r3;
    int l1, l2, rcount, nearest_code, protons;
    double base_shift;

    l1 = phc.indexOf( '+' );
    if ( l1 < 0 )
        l1 = phc.indexOf( '-' );
    if ( l1 < 0 )
        l1 = phc.indexOf( '*' );
    substr = phc.mid( l1 + 1 );

    qDebug() << "1H shift of " << substr;

    // identify base type:  extract primary atom and R groups
    rcount = 0;
    nearest_code = 0;
    protons = 0;
    l1 = substr.indexOf( "(" );

    nearest = substr.left( l1 );
    l2 = substr.indexOf( ")", l1 );
    r1 = substr.mid( l1 + 1, l2 - l1 - 1 );
    rcount++;
    l1 = substr.indexOf( "(", l2 );
    if ( l1 > 0 ) {
        l2 = substr.indexOf( ")", l1 );
        r2 = substr.mid( l1 + 1, l2 - l1 - 1 );
        rcount++;
        l1 = substr.indexOf( "(", l2 );
        if ( l1 > 0 ) {
            l2 = substr.indexOf( ")", l1 );
            r3 = substr.mid( l1 + 1, l2 - l1 - 1 );
            rcount++;
        }
    }

    if ( r1 == "H" )
        protons++;
    if ( r2 == "H" )
        protons++;
    if ( r3 == "H" )
        protons++;

    if ( nearest == "C" ) {
        if ( protons == 0 )
            nearest_code = PROTON_CH;
        if ( protons == 1 )
            nearest_code = PROTON_CH2;
        if ( protons == 2 )
            nearest_code = PROTON_CH3;
    }
    if ( nearest == "CA" ) {
        nearest_code = PROTON_CH_AROMATIC;
    }
    if ( nearest == "CR" ) {
        if ( protons == 0 )
            nearest_code = PROTON_CH;
        if ( protons == 1 )
            nearest_code = PROTON_CH2_RING;
    }
    if ( nearest == "N" ) {
        if ( protons == 0 )
            nearest_code = PROTON_NH;
        if ( protons == 1 )
            nearest_code = PROTON_NH2;
    }
    if ( nearest == "O" ) {
        nearest_code = PROTON_OH;
    }
    if ( nearest == "S" ) {
        nearest_code = PROTON_SH;
    }
    // start with a base shift based on proton type
    switch ( nearest_code ) {
    case PROTON_CH:
        base_shift = 2.1;
        if ( ( r1 == "=O" ) && ( r2 == "OC" ) )
            return 8.1;         // -O-CHO
        if ( r1 == "=O" )
            return 9.7;         // -CHO
        break;
    case PROTON_CH_AROMATIC:
        base_shift = 7.5;
        break;
    case PROTON_CH2:
        base_shift = 1.8;
        if ( ( r1 == "C=OC" ) && ( r2 == "C=OC" ) )
            return 3.7;         // -CO-CH2-CO-
        if ( ( r1 == "C=OC" ) && ( r3 == "OC" ) )
            return 4.75;        // -CO-CH2-O-
        if ( ( r2 == "OC" ) && ( r3 == "OC" ) )
            return 5.5;         // -S-CH2-S-
        if ( ( r2 == "SC" ) && ( r3 == "SC" ) )
            return 3.75;        // -S-CH2-S-
        if ( ( r2 == "OC" ) && ( r3 == "SC" ) )
            return 4.9;         // -O-CH2-S-
        if ( ( r2.left( 1 ) == "N" ) && ( r3.left( 1 ) == "N" ) )
            return 4.3;         // -N-CH2-N-
        break;
    case PROTON_CH2_RING:
        base_shift = 1.8;
        break;
    case PROTON_CH3:
        base_shift = 0.8;
        if ( r3 == "OC" )
            return 3.6;         // CH3-O-
        if ( r3.left( 1 ) == "N" )
            return 2.6;         // CH3-N-
        if ( r3.left( 1 ) == "S" )
            return 2.2;         // CH3-S-
        if ( r1.left( 2 ) == "CA" )
            return 2.5;         // CH3-Ar
        if ( r1.left( 3 ) == "C=O" )
            return 2.1;         // CH3-CO-
        break;
    case PROTON_NH:
        base_shift = 1.8;
        if ( r1.left( 2 ) == "CA" )
            return 5.0;         // Ar-NH-
        break;
    case PROTON_NH2:
        base_shift = ( double ) ( phc.length() % 10 );
        break;
    case PROTON_OH:
        base_shift = 3.0;       // R-OH
        if ( r1.left( 2 ) == "CA" )
            return 5.8;         // Ar-OH
        if ( r1 == "C=OC" )
            return 11.5;        // carboxylic acid characteristic
        break;
    case PROTON_SH:
        base_shift = 2.0;
        if ( r1.left( 2 ) == "CA" )
            return 4.0;         // Ar-SH
        break;
    }

    return base_shift;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
