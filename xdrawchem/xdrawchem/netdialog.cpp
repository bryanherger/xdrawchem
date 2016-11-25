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
#include <qcheckbox.h>
#include <iostream>
#include <stdlib.h>

#include <qstring.h>
#include <qurloperator.h>
#include <qcstring.h>
#include <qnetworkprotocol.h>

#include "netdialog.h"
#include "defs.h"
#include "http.h"

NetDialog::NetDialog(QWidget *parent, QString name):
  QDialog(parent, name, TRUE)
{
  QString serverName;

  serverName = getenv("XDC_SERVER");

  if (serverName.length() < 2)
    serverName = XDC_SERVER;

  QPushButton *ok, *dismiss;
  QLabel *cap1, *caption, *l1;
  setBackgroundColor(lightGray);
  setCaption( tr("Find structure via Internet") );

  QGridLayout *mygrid = new QGridLayout(this,8,5,5);

  cap1 = new QLabel( tr("XDC database server:"), this);
  cap1->setMinimumWidth(375);
  cap1->setBackgroundColor(lightGray);
  mygrid->addMultiCellWidget(cap1, 0, 0, 0, 4);

  serverkey = new QLineEdit(this);
  serverkey->setText(serverName);
  mygrid->addMultiCellWidget(serverkey, 1, 1, 0, 4);

  caption = new QLabel( tr("Search type:"), this);
  caption->setBackgroundColor(lightGray);
  mygrid->addMultiCellWidget(caption, 2, 2, 0, 4);

  keylist = new QComboBox(FALSE, this);
  keylist->insertItem(tr("Chemical name"));
  keylist->insertItem(tr("CAS Number"));
  keylist->insertItem(tr("Formula"));
  mygrid->addMultiCellWidget(keylist, 3, 3, 0, 4);

  l1 = new QLabel(tr("Look for:"), this);
  l1->setBackgroundColor(lightGray);
  mygrid->addMultiCellWidget(l1, 4, 4, 0, 4);
  
  searchkey = new QLineEdit(this);
  mygrid->addMultiCellWidget(searchkey, 5, 5, 0, 4);
  connect(searchkey, SIGNAL(returnPressed()), this, SLOT(accept()) );
  searchkey->setFocus();

  emcheck = new QCheckBox(tr("Exact matches only"), this);
  mygrid->addMultiCellWidget(emcheck, 6, 6, 0, 4);

  ok = new QPushButton( tr("Search"), this);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(OK()) );
  mygrid->addMultiCellWidget(ok, 7, 7, 1, 1);

  dismiss = new QPushButton( tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  mygrid->addMultiCellWidget(dismiss, 7, 7, 3, 3);
}

void NetDialog::OK() {
  accept();
}
