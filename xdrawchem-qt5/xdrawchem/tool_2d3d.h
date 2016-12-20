#ifndef TOOL_2D3D_H
#define TOOL_2D3D_H

#include "molecule.h"
#include "peak.h"
#include "tooldialog.h"

using namespace OpenBabel;

class Tool_2D3D : public ToolDialog
{
public:
    Tool_2D3D( QWidget *parent = 0 );
    void process();
    void save3DFile();

private:
    Molecule *new3dmol;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
