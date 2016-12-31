#include <qapplication.h>
#include <qclipboard.h>
#include <stdlib.h>

#include "molinfodialog.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

MolInfoDialog::MolInfoDialog(QWidget *parent, QString name):
        QDialog(parent, name, false)
{
  setMinimumWidth(400);
  QGridLayout *grid = new QGridLayout(this, 10, 2);
  setCaption( tr("Molecule Info") );
  QPushButton *ok, *dismiss, *help;
  name_label = new QLabel(tr("Name: "), this);
  name_label->setText(tr("Name: "));
  grid->addMultiCellWidget(name_label, 1, 1, 1, 2);
  syn_label = new QLabel(tr("Other names: "), this);
  syn_label->setText(tr("Other names: "));
  syn_label->setAlignment(syn_label->alignment() | Qt::WordBreak);
  grid->addMultiCellWidget(syn_label, 2, 2, 1, 2);
  cas_label = new QLabel(tr("CAS: "), this);
  cas_label->setText(tr("CAS: "));
  grid->addMultiCellWidget(cas_label, 3, 3, 1, 2);
  pc_label = new QLabel(tr("PubChem compound: "), this);
  pc_label->setText(tr("PubChem compound: "));
  grid->addMultiCellWidget(pc_label, 4, 4, 1, 2);
  mw = tr("Molecular weight = ");
  mw_check = new QCheckBox(mw, this);
  grid->addMultiCellWidget(mw_check, 5, 5, 1, 2);
  ef = tr("Formula = ");
  ef_check = new QCheckBox(ef, this);
  grid->addMultiCellWidget(ef_check, 6, 6, 1, 2);
  ea = tr("Analysis = ");
  ea_check = new QCheckBox(ea, this);
  grid->addMultiCellWidget(ea_check, 7, 7, 1, 2);
  ok = new QPushButton(tr("OK"), this);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(doAccept()) );
  grid->addWidget(ok, 8, 1);
  help = new QPushButton(tr("Help"), this);
  help->setPalette(QPalette(lightGray));
  connect(help, SIGNAL(clicked()), SLOT(SendHelp()) );
  grid->addWidget(help, 8, 2);
  getpcc = new QPushButton(tr("Search PubChem"), this);
  getpcc->setPalette(QPalette(lightGray));
  connect(getpcc, SIGNAL(clicked()), SLOT(doPubChem()) );
  grid->addMultiCellWidget(getpcc, 9, 9, 1, 2);
  getpcc->setEnabled(false);
  dismiss = new QPushButton(tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  grid->addMultiCellWidget(dismiss, 10, 10, 1, 2);
}

void MolInfoDialog::doAccept() {
  emit MIDClose();
}

void MolInfoDialog::doPubChem() {
  QString ffurl, ffcmd;

  ffurl = "http://pubchem.ncbi.nlm.nih.gov/summary/summary.cgi?cid=" + pcc;
  ffcmd = "firefox " + ffurl + " &";
  std::cout << ffcmd << std::endl;
  system(ffcmd.ascii());
  QClipboard *cb = QApplication::clipboard();

  // Copy text into the clipboard
  cb->setText( ffurl, QClipboard::Clipboard );
}

void MolInfoDialog::SendHelp() {
  QString home, topic;
  topic = "molinfo.html";
#ifdef UNIX
  home = RingDir + "doc/" + topic;
#else
  home = RingDir + "doc\\" + topic;
#endif
  HelpWindow *help = new HelpWindow(home, ".", 0, "help viewer");
  help->setCaption(QString(XDC_VERSION) + " - Help viewer");
  help->show();
}
