#include <qwidget.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qdatetime.h>

#include "dyk.h"

DYKDialog::DYKDialog(QWidget *parent, QString name):
	QDialog(parent, name, TRUE)
{
  populateTipList();
  setCaption( tr("Did You Know?") );
  QLabel *l1 = new QLabel(this, "DYK_top");
  l1->setGeometry(10,10,210,30);
  l1->setText( tr("Did You Know?") );
  tipcount = preferences.getDYKCount();
  if (tipcount >= tips.count() ) tipcount = 0;
  curtip = new QLabel(this, "DYK_curtip");
  curtip->setGeometry(10,50,320,180);
  curtip->setAlignment(Qt::AlignTop | Qt::AlignAuto);
  curtip->setTextFormat(Qt::RichText);
  curtip->setText( tips[tipcount] );
  QPushButton *pt = new QPushButton(tr("Previous tip"), this);
  pt->setGeometry(350,10,120,30);
  pt->setPalette(QPalette(lightGray));
  connect(pt, SIGNAL(clicked()), SLOT(prevTip()) );
  QPushButton *nt = new QPushButton(tr("Next tip"), this);
  nt->setGeometry(350,50,120,30);
  nt->setPalette(QPalette(lightGray));
  connect(nt, SIGNAL(clicked()), SLOT(nextTip()) );
  futureshow = new QCheckBox(tr("Don't show this dialog at startup"), this);
  futureshow->setChecked(!preferences.getDYK());
  //futureshow->setPalette(QPalette(lightGray));
  futureshow->setGeometry(10,230,320,30);
  QPushButton *ok = new QPushButton(tr("OK"), this);
  ok->setGeometry(120,270,80,30);
  ok->setPalette(QPalette(lightGray));
  ok->setDefault(true);
  connect(ok, SIGNAL(clicked()), SLOT(finished()) );
}

void DYKDialog::finished(void) {
  preferences.setDYK( !futureshow->isChecked() );
  tipcount++;
  if (tipcount >= tips.count() ) tipcount = 0;
  preferences.setDYKCount(tipcount);
  accept();
}

void DYKDialog::populateTipList(void) {
  tips.append( tr("XDrawChem can be run from the command line to produce images.  Type \"xdrawchem --help\" for details.") );
  tips.append( tr("When in select mode, you can edit most objects by right-clicking on them.") );
  tips.append( tr("When drawing bonds, you can edit the bond style by right-clicking on the bond.") );
  tips.append( tr("When in select mode, press Shift+LeftButton and drag over part of a molecule to select the entire molecule.") );
  tips.append( tr("When in select mode, press Ctrl+LeftButton on a molecule to get molecule information.") );
  tips.append( tr("Bond length, bond angle, and units are set in the Drawing Settings option under the Format menu.") );
  tips.append( tr("You can attach a subscript to a bracket by selecting the text tool, then clicking on the bracket.") );
  tips.append( tr("You can toggle the grid display by pressing Ctrl+G.  Turning the grid on or off does not affect whether objects snap to the grid.") );
  tips.append( tr("A number of ring and molecule templates are available from the ring tool, including single and fused rings, amino acids, and nucleosides.") );
  tips.append( tr("Remember to select a file type when opening or saving documents.") );
  tips.append( tr("The option \"Clean up molecule\" under the Tools menu can be used to refine structures such as rings and macromolecules.") );
  tips.append( tr("It's possible to attach rings to structures by selecting a ring and clicking on an atom.") );
  tips.append( tr("XDrawChem can automatically attach amino acids from N-terminal to C-terminal (but the result isn't always pretty :)") );
  tips.append( tr("Be sure to check for updates at\nhttp://xdrawchem.sourceforge.net/") );
  tips.append( tr("XDrawChem can draw over 100,000 substances using its Internet database.  Select Find on internet from the File menu to search for compounds.") );
  tips.append( tr("XDrawChem can identify compounds by CAS number and occasionally by name.  Select Molecule information from the Tools menu and click on a molecule.  However, this only works if the compound is listed in the database.") );
}

void DYKDialog::nextTip(void) {
  tipcount++;
  if (tipcount >= tips.count() ) tipcount = 0;
  curtip->setText( tips[tipcount] );
}

void DYKDialog::prevTip(void) {
  tipcount--;
  if (tipcount < 0 ) tipcount = tips.count() - 1;
  curtip->setText( tips[tipcount] );
}

