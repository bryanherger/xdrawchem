#ifndef TOOL_IR_H
#define TOOL_IR_H

#include <qdialog.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include <qtextedit.h>

#include "molecule.h"
#include "tooldialog.h"
#include "ngw.h"

using namespace OpenBabel;

class Tool_IR_Dialog:public ToolDialog {
  Q_OBJECT
    public:
  Tool_IR_Dialog(QWidget *parent = 0, QString name = "");
  void process();
 private:
  NewGraphWidget *ngw;
  QTextEdit *peaktext;
};

#endif
