/***************************************************************************
                          ioiface.cpp  -  interface between ChemData and OBMol
                             -------------------
    begin                : Sun Aug 11 2002
    copyright            : (C) 2002 by adam
    email                : adam@adamlt
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <vector>
#include <map>
#include <algorithm>
#include <iostream>

/*
//Added by qt3to4:
#include <Q3PtrList>
*/

#include "ioiface.h"
#include "dpoint.h"
#include "chemdata.h"
#include "application.h"
#include "bond.h"
#include "molecule.h"

struct Point
{

    float x, y;
    bool operator==( const Point & point ) const
    {
        return ( x == point.x && y == point.y );
    }
    bool operator<( const Point & point ) const
    {
        return ( x * x + y * y ) < ( point.x * point.x + point.y * point.y );
    }

};

struct pt_cmp
{

    bool operator() ( const Point & one, const Point & two )
    {
        return one < two;
    }

};

using namespace OpenBabel;

const char IOIface::symbol[110][4] = {
    "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
    "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",    // 20
    "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
    "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr",  // 40
    "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
    "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",  // 60
    "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
    "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg",  // 80
    "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
    "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm",  // 100
    "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Uun"
};


IOIface::IOIface( ChemData * cd, OBMol * mol )
{

    chemdata = cd;
    obmol = mol;

}

IOIface::~IOIface()
{

    //chemdata and obmol shouldn't be deleted in case
    //they are to be used later

}

void IOIface::setChemData( ChemData * cd )
{

    chemdata = cd;
}

void IOIface::setOBMol( OBMol * mol )
{

    obmol = mol;
}

void IOIface::convertToChemData()
{

    OBBond *bond;
    OBAtom *atom1, *atom2;
    DPoint *s, *e;
    Point point;
    Text *text;

    std::vector < OBBond * >::iterator bonditr;
    std::map < Point, DPoint *, pt_cmp > points;
    std::map < Point, DPoint *, pt_cmp >::iterator itr;

    //int i=0;
    float x, y;

    /* look through bonds and store points found since ChemData::addBond() requires
       DPoint*, and not DPoint. */

    for ( bond = obmol->BeginBond( bonditr ); bond; bond = obmol->NextBond( bonditr ) ) {

        atom1 = bond->GetBeginAtom();
        atom2 = bond->GetEndAtom();

        x = atom1->GetX();
        x = ( x + 5 ) * 15 + 100;
        y = atom1->GetY();
        y = ( y + 5 ) * 15 + 100;

        point.x = x;
        point.y = y;

        itr = points.find( point );
        if ( itr == points.end() ) {
            std::cerr << "true";
            points[point] = new DPoint( x, y );
        }

        x = atom2->GetX();
        x = ( x + 5 ) * 15 + 100;
        y = atom2->GetY();
        y = ( y + 5 ) * 15 + 100;

        point.x = x;
        point.y = y;

        itr = points.find( point );
        if ( itr == points.end() )
            points[point] = new DPoint( x, y );

    }

    /* use information from previous for-loop and add bonds and labels */

    for ( bond = obmol->BeginBond( bonditr ); bond; bond = obmol->NextBond( bonditr ) ) {

        atom1 = bond->GetBeginAtom();
        atom2 = bond->GetEndAtom();

        x = atom1->GetX();
        x = ( x + 5 ) * 15 + 100;
        y = atom1->GetY();
        y = ( y + 5 ) * 15 + 100;

        point.x = x;
        point.y = y;

        itr = points.find( point );
        s = ( *itr ).second;

        x = atom2->GetX();
        x = ( x + 5 ) * 15 + 100;
        y = atom2->GetY();
        y = ( y + 5 ) * 15 + 100;

        point.x = x;
        point.y = y;

        itr = points.find( point );
        e = ( *itr ).second;

        int bondorder = bond->GetBondOrder();

        //set elements
        if ( !atom1->IsCarbon() ) {
            QString str( "<element>" );

            str += IOIface::symbol[atom1->GetAtomicNum() - 1];
            str += "</element>";
            s->SetElementFromXML( str );

        }

        if ( !atom2->IsCarbon() ) {
            QString str( "<element>" );

            str += IOIface::symbol[atom2->GetAtomicNum() - 1];
            str += "</element>";
            e->SetElementFromXML( str );

        }

        chemdata->addBond( s, e, 1, bondorder, QColor( "black" ), true );

        //label atoms if not Carbon

        if ( !atom1->IsCarbon() ) {

            text = new Text( chemdata->getRender2D() );
            QString str = IOIface::symbol[atom1->GetAtomicNum() - 1];

            text->setPoint( s );
            text->setText( str );
            //str.fill(' ');
            //text->setTextMask(str);
            chemdata->addText( text );
            //qDebug() << "ioiface(1) text:" << str;
        }

        if ( !atom2->IsCarbon() ) {

            text = new Text( chemdata->getRender2D() );
            QString str = IOIface::symbol[atom2->GetAtomicNum() - 1];

            text->setPoint( e );
            text->setText( str );
            //str.fill(' ');
            //text->setTextMask(str);
            text->setJustify( JUSTIFY_CENTER );
            chemdata->addText( text );
            //qDebug() << "ioiface(2) text:" << str;

        }
    }
}

//
// TODO :)
//
bool IOIface::convertToOBMol()
{
    QList < DPoint * >allpoints;
    QList < Bond * >allbonds;
    DPoint *tmp_atom;
    Bond *tmp_bond;
    Molecule *this_mol;

    // now, this might not always do what the user wants but most file
    // formats can only save one molecule.  So, let's get the first molecule
    // from the ChemData and do that.

    this_mol = chemdata->firstMolecule();
    allpoints = this_mol->AllPoints();
    allbonds = this_mol->AllBonds();

    // adapted from openbabel/mdl.cpp, most similar format to this...
    // (i.e., MDL is atom list followed by bond list)

    obmol->BeginModify();
    obmol->ReserveAtoms( allpoints.count() );
    char type[5];
    vector3 v;
    OBAtom atom;

    foreach ( tmp_atom, allpoints ) {
        v.SetX( tmp_atom->x );
        v.SetY( tmp_atom->y );
        v.SetZ( tmp_atom->z );
        atom.SetVector( v );
        atom.SetAtomicNum( tmp_atom->getAtomicNumber() );
        strcpy( type, tmp_atom->baseElement().toAscii() );
        atom.SetType( type );
        //atom.SetType( tmp_atom->baseElement().toAscii() );

        if ( !obmol->AddAtom( atom ) )
            return ( false );
        atom.Clear();
    }

    int start, end, order, flag, bst;

    foreach ( tmp_bond, allbonds ) {
        flag = 0;
        start = allpoints.indexOf( tmp_bond->Start() ) + 1;
        end = allpoints.indexOf( tmp_bond->End() ) + 1;
        order = tmp_bond->Order();
        bst = tmp_bond->Stereo();
        if ( bst == STEREO_UP )
            flag |= OB_WEDGE_BOND;
        if ( bst == STEREO_DOWN )
            flag |= OB_HASH_BOND;
        if ( !obmol->AddBond( start, end, order, flag ) )
            return ( false );
    }

    obmol->EndModify();

    return true;
}

//eof


// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
