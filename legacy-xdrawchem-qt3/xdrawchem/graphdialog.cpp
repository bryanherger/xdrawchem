#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>

#include "render2d.h"
#include "graphdialog.h"
#include "graphdata.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

GraphDialog::GraphDialog(QWidget *parent, QString name) : 
	QDialog(parent, name, FALSE)
{
  setBackgroundColor(lightGray);
  setCaption(name);
  g = new GraphWidget(this, "NMR goes here");
  g->setBackgroundColor(QColor(255,255,255));
  g->setGeometry(0,0,600,500);
  //QPushButton *showhide = new QPushButton("Show/hide molecule", this);
  //showhide->setGeometry(20,320,150,40);
  //showhide->setPalette(QPalette(lightGray));
  QPushButton *qprint = new QPushButton( tr("Print"), this);
  qprint->setGeometry(20,520,120,40);
  qprint->setPalette(QPalette(lightGray));
  connect(qprint, SIGNAL(clicked()), g, SLOT(Print()) );
  QPushButton *qexport = new QPushButton( tr("Export Peak List"), this);
  qexport->setGeometry(150,520,120,40);
  qexport->setPalette(QPalette(lightGray));
  connect(qexport, SIGNAL(clicked()), g, SLOT(Export()) );
  QPushButton *qhelp = new QPushButton( tr("Help"), this);
  qhelp->setGeometry(280,520,120,40);
  qhelp->setPalette(QPalette(lightGray));
  connect(qhelp, SIGNAL(clicked()), SLOT(SendHelp()));
  QPushButton *qclose = new QPushButton( tr("Close"), this);
  qclose->setGeometry(410,520,120,40);
  qclose->setPalette(QPalette(lightGray));
  connect(qclose, SIGNAL(clicked()), SLOT(accept()));

  if (name.contains("13C-NMR") > 0) g->setDataType(1);
  if (name.contains("IR") > 0) g->setDataType(2);
  if (name.contains("1H-NMR") > 0) g->setDataType(3);
}

void GraphDialog::AddPeak(double v1, QString l1, QString t1) {
  AddPeak(v1, QColor(0,0,0), l1, t1);
}

void GraphDialog::AddPeak(double v1, QColor c1, QString l1, QString t1) {
  if (v1 < 0) return;  // negative indicates invalid peak.
  g->AddPeak(v1, c1, l1, t1);
}

void GraphDialog::AddPeak(double v1, int m1, QColor c1, 
			  QString l1, QString t1) {
  g->AddPeak(v1, m1, c1, l1, t1);
}

void GraphDialog::AddPixmap(QPixmap p1) {
  g->AddPixmap(p1);
}

void GraphDialog::SendHelp() {
  QString home, topic;
  topic = "spectra.html";
#ifdef UNIX
  home = RingDir + "doc/" + topic;
#else
  home = RingDir + "doc\\" + topic;
#endif
  HelpWindow *help = new HelpWindow(home, ".", 0, "help viewer");
  help->setCaption(QString(XDC_VERSION) + " - Help viewer");
  help->show();
}
