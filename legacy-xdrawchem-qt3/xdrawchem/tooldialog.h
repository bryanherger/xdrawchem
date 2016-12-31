#ifndef TOOLDIALOG_H
#define TOOLDIALOG_H

#include <qdialog.h>
#include <qptrlist.h>
#include <qpixmap.h>

#include <openbabel/mol.h>
#include "molecule.h"
#include "peak.h"

using namespace OpenBabel;

class ToolDialog:public QDialog {
  Q_OBJECT
    public:
  ToolDialog(QWidget *parent = 0, QString name = "");
  void setMolecule(Molecule *);
  void setMolecule(OBMol *);
  void setProductMolecule(Molecule *);
  void setProductMolecule(OBMol *);
  virtual void process();
  public slots:
    void SendHelp();
 protected:
  QString helpfile;
  Molecule *this_mol, *product_mol;
  QPtrList<Peak> peaklist;
  Peak *tmp_peak;
};

#endif
