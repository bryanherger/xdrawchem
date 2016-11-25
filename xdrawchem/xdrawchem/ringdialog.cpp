#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qdir.h>
#include <qstringlist.h>
#include <iostream>

using std::cout;
using std::endl;

#include "ringdialog.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

RingDialog::RingDialog(QWidget *parent, QString name):
  QDialog(parent, name, TRUE)
{
  // Init variables which tell which ring was picked
  fn = QString("noring");

  QPixmap pix;
  QLabel *caption;
  QPushButton *dismiss;
  setBackgroundColor(lightGray);
  setCaption( tr("Ring tool") );

  QGridLayout *mygrid = new QGridLayout(this,4,5,5);

  caption = new QLabel(tr("Choose a structure from list:"), this);
  caption->setBackgroundColor(lightGray);
  mygrid->addMultiCellWidget(caption, 0, 0, 0, 4);

  QDir dr(RingDir, "*.cml");
  QStringList dl = dr.entryList();
  filelist = new QComboBox(FALSE, this);
  filelist->insertItem("(pick)");
  for (int cc = 0; cc < dl.count(); cc++)
    filelist->insertItem(dl[cc].left(dl[cc].length() - 4));
  connect(filelist, SIGNAL(activated(int)),
	  this, SLOT(fromComboBox(int)) );
  mygrid->addMultiCellWidget(filelist, 1, 1, 0, 4);
  
  dismiss = new QPushButton(tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  mygrid->addMultiCellWidget(dismiss, 2, 2, 1, 3);
}

void RingDialog::fromComboBox(int i)
{
  if (filelist->currentText() == "(pick)") return;
  fn = filelist->currentText() + QString(".cml");
  //cout << "File: " << fn << endl;
  accept();
}
