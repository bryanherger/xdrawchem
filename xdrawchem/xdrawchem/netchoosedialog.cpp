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
#include <qlistview.h>

using std::cout;
using std::endl;

#include "netchoosedialog.h"
#include "defs.h"

NetChooseDialog::NetChooseDialog(QWidget *parent, QString name,QStringList r1):
  QDialog(parent, name, TRUE)
{
  results = r1;

  QPushButton *ok, *dismiss;
  QLabel *cap1;
  setBackgroundColor(lightGray);
  setCaption("Choose Structure");

  QGridLayout *mygrid = new QGridLayout(this,3,5,5);

  cap1 = new QLabel("Select a molecule:", this);
  cap1->setBackgroundColor(lightGray);
  mygrid->addMultiCellWidget(cap1, 0, 0, 0, 4);

  lv = new QListView(this);
  lv->addColumn("CAS");
  lv->addColumn("IUPAC Name");
  lv->addColumn("Formula");
  lv->addColumn("Other names");
  lv->addColumn("SMILES");
  lv->setSorting(0);
  mygrid->addMultiCellWidget(lv, 1, 1, 0, 4);

  int i1;
  QString tmp_str, tcas, tformula, tformat, tname, taltname;
  QListViewItem *lvi;

  for (QStringList::Iterator ir = r1.begin(); 
       ir != r1.end(); ++ir) {
    tmp_str = *ir;
    tmp_str.replace("\",\"", "~");
    i1 = tmp_str.find("~");
    tcas = tmp_str.mid(1, i1-1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tcas = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tname = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tformula = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    taltname = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tformat = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    //std::cout << tcas << "," << tname <<","<< tformula << "," << std::endl;
    tname.replace("\"", "");
    tcas.replace("\"", "");
    tformat.replace("\"", "");
    taltname.replace("\"", "");
    tformula.replace("\"", "");
    lvi = new QListViewItem(lv);
    lvi->setText(0, tcas);
    lvi->setText(1, tname);
    lvi->setText(2, tformula);
    lvi->setText(3, taltname);
    lvi->setText(4, tformat);
    //lv->insertItem(lvi);
  }

  ok = new QPushButton( tr("Select"), this);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(OK()) );
  mygrid->addMultiCellWidget(ok, 2, 2, 1, 1);

  dismiss = new QPushButton( tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  mygrid->addMultiCellWidget(dismiss, 2, 2, 3, 3);
}

void NetChooseDialog::OK() {
  // save SMILES string of selected item
  fn = lv->selectedItem()->text(4);
  fn.replace("\"", "");  // strip quotes
  accept();
}
