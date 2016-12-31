#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qlayout.h>
#include <iostream>

using std::cerr;
using std::endl;

#include "tool_ir.h"
#include "render2d.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
//extern QString RingDir;

Tool_IR_Dialog::Tool_IR_Dialog(QWidget *parent, QString name) : 
	ToolDialog(parent, name)
{
  setCaption( tr("IR spectrum") );
  helpfile = "spectra.html";
  setBackgroundColor(lightGray);
  QGridLayout *mygrid;
  mygrid = new QGridLayout(this,3,2,5);
  ngw = new NewGraphWidget( this );
  ngw->setDataType(DATA_IR);
  mygrid->addMultiCellWidget(ngw,0,0,0,1);
  peaktext = new QTextEdit( this, "peak list" );
  peaktext->setReadOnly(true);
  mygrid->addMultiCellWidget(peaktext,1,1,0,1);
  QPushButton *qhelp = new QPushButton( tr("Help"), this);
  qhelp->setPalette(QPalette(lightGray));
  mygrid->addWidget(qhelp,2,0);
  connect(qhelp, SIGNAL(clicked()), SLOT(SendHelp()));
  QPushButton *qclose = new QPushButton( tr("Close"), this);
  qclose->setPalette(QPalette(lightGray));
  mygrid->addWidget(qclose,2,1);
  connect(qclose, SIGNAL(clicked()), SLOT(accept()));
}

void Tool_IR_Dialog::process() {
  this_mol->CalcIR();
  peaklist = this_mol->peaklist;
  std::cerr << "got peak list" << endl;
  if (product_mol == 0) {
    for (tmp_peak = peaklist.first(); tmp_peak != 0; 
	 tmp_peak = peaklist.next() ) {
      ngw->AddPeak(tmp_peak, QColor(0,0,0) );
      peaktext->append(tmp_peak->comment);
      //peaktext->append("\n");
    }
    return;
  }
  peaktext->setColor( QColor(255,0,0) );
  peaktext->insert( tr("Peaks of reactant:\n") );
  for (tmp_peak = peaklist.first(); tmp_peak != 0; 
       tmp_peak = peaklist.next() ) {
    ngw->AddPeak(tmp_peak, QColor(255,0,0) );
    peaktext->insert(tmp_peak->comment);
    peaktext->insert("\n");
  }
  product_mol->CalcIR();
  peaklist.clear();
  peaklist = product_mol->peaklist;
  peaktext->setColor( QColor(0,0,255) );
  peaktext->insert( tr("Peaks of product:\n") );
  for (tmp_peak = peaklist.first(); tmp_peak != 0; 
       tmp_peak = peaklist.next() ) {
    ngw->AddPeak(tmp_peak, QColor(0,0,255) );
    peaktext->insert(tmp_peak->comment);
    peaktext->insert("\n");
  }
}


