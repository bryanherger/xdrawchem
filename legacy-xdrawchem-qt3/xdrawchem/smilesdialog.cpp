#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>

#include "application.h"
#include "chemdata.h"
#include "render2d.h"
#include "smilesdialog.h"

void ApplicationWindow::FromSMILES(void) {
  SmilesDialog i(this, QString("smiles_dialog"));
  if ( !i.exec() ) return;
  QString sm = i.getSMILES();
  c->fromSMILES(sm);
  r->Inserted();
  r->repaint(false);
}

SmilesDialog::SmilesDialog(QWidget *parent, QString name) : 
	QDialog(parent, name, TRUE)
{
  QPushButton *ok, *cancel;
  QLabel *t1;
  setBackgroundColor(lightGray);
  setCaption( tr("Enter SMILES string") );
  t1 = new QLabel( tr("Enter SMILES string:"), this);
  t1->setGeometry(10,10,180,30);
  t1->setBackgroundColor(lightGray);
  tf_smiles = new QLineEdit(this);
  tf_smiles->setGeometry(10,50,200,30);
  connect(tf_smiles, SIGNAL(returnPressed()), this, SLOT(accept()) );
  ok = new QPushButton( tr("OK"), this);
  ok->setGeometry(10,90,100,30);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  cancel = new QPushButton( tr("Cancel"), this);
  cancel->setGeometry(120,90,100,30);
  cancel->setPalette(QPalette(lightGray));
  connect(cancel, SIGNAL(clicked()), SLOT(reject()) );
}

QString SmilesDialog::getSMILES(void)
{
  return tf_smiles->text();
}
