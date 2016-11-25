#ifndef TOOL_2D3D_H
#define TOOL_2D3D_H

#include <qdialog.h>
#include <qptrlist.h>
#include <qpixmap.h>

#include "molecule.h"
#include "peak.h"
#include "tooldialog.h"

using namespace OpenBabel;

class Tool_2D3D:public ToolDialog {
  Q_OBJECT
    public:
  Tool_2D3D(QWidget *parent = 0, QString name = "");
  void process();
  void save3DFile();
 private:
  Molecule *new3dmol;
};

#endif
