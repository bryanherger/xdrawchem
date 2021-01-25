/***************************************************************************
                          ioiface.h  -  interface between ChemData and OELib
                             -------------------
    begin                : Sun Aug 11 2002
    copyright            : (C) 2002 by Adam Tenderholt
    email                : atenderholt@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _IOIFACE_H
#define _IOIFACE_H

#include "chemdata.h"

#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/math/vector3.h>
#include <openbabel/mol.h>

using namespace OpenBabel;

class IOIface {

  public:
    IOIface(ChemData* cd=0, OBMol* mol=0);
    ~IOIface();

    static const char symbol[110][4];

    void setChemData(ChemData* cd);
    void setOBMol(OBMol* mol);
    void convertToChemData();
    bool convertToOBMol();

  private:

    ChemData* chemdata;
    OBMol* obmol;

};

#endif

//eof

