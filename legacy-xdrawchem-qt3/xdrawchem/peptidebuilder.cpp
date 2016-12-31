#include <qwidget.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qbuttongroup.h>
#include <qdialog.h>

#include "peptidebuilder.h"

PeptDialog::PeptDialog(QWidget *parent, QString name) {
  setCaption( tr("Peptide Builder") );

  QGridLayout *mygrid;
  mygrid = new QGridLayout(this,6,8,0);

  QLabel *ltop = new QLabel(this);
  ltop->setText( tr("Enter peptide sequence -\nthree-letter abbreviations:") );
  mygrid->addMultiCellWidget(ltop,0,0,0,7);
  peptide = new QLineEdit(this);
  mygrid->addMultiCellWidget(peptide, 1,1,0,7);
  QLabel *lne = new QLabel(this);
  lne->setText( tr("Shortcut: Hold 'Alt' and type one-letter codes!") );
  mygrid->addMultiCellWidget(lne,2,2,0,7);
  QButtonGroup *bgroup = new QButtonGroup(3, Qt::Horizontal, this);
  QPushButton *b1;
  b1 = new QPushButton("Ala (&A)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Arg (&R)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Asn (&N)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Asp (&D)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Cys (&C)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Gln (&Q)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Glu (&E)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Gly (&G)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("His (&H)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Ile (&I)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Leu (&L)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Lys (&K)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Met (&M)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Nph", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Phe (&F)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Pro (&P)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Ser (&S)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Sta", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Thr (&T)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Trp (&W)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Tyr (&Y)", bgroup);
  bgroup->insert(b1);
  b1 = new QPushButton("Val (&V)", bgroup);
  bgroup->insert(b1);
  connect(bgroup, SIGNAL(clicked(int)), this, SLOT(buttonPress(int)) );
  mygrid->addMultiCellWidget(bgroup, 3,4,0,7);
  QPushButton *ok, *dismiss;
  ok = new QPushButton( tr("OK"), this);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  mygrid->addMultiCellWidget(ok, 5, 5, 0, 1);
  dismiss = new QPushButton( tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  mygrid->addMultiCellWidget(dismiss, 5, 5, 6, 7);
}

QString PeptDialog::getPeptide() {
  return peptide->text();
}

void PeptDialog::buttonPress(int b1) {
  switch (b1) {
  case 0:
    peptide->insert("Ala");
    break;
  case 1:
    peptide->insert("Arg");
    break;
  case 2:
    peptide->insert("Asn");
    break;
  case 3:
    peptide->insert("Asp");
    break;
  case 4:
    peptide->insert("Cys");
    break;
  case 5:
    peptide->insert("Gln");
    break;
  case 6:
    peptide->insert("Glu");
    break;
  case 7:
    peptide->insert("Gly");
    break;
  case 8:
    peptide->insert("His");
    break;
  case 9:
    peptide->insert("Ile");
    break;
  case 10:
    peptide->insert("Leu");
    break;
  case 11:
    peptide->insert("Lys");
    break;
  case 12:
    peptide->insert("Met");
    break;
  case 13:
    peptide->insert("Nph");
    break;
  case 14:
    peptide->insert("Phe");
    break;
  case 15:
    peptide->insert("Pro");
    break;
  case 16:
    peptide->insert("Ser");
    break;
  case 17:
    peptide->insert("Sta");
    break;
  case 18:
    peptide->insert("Thr");
    break;
  case 19:
    peptide->insert("Trp");
    break;
  case 20:
    peptide->insert("Tyr");
    break;
  case 21:
    peptide->insert("Val");
    break;
  default:
    break;
  }
}

