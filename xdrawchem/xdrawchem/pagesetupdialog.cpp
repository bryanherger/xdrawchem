#include <qwidget.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qprinter.h>

#include "pagesetupdialog.h"
#include "defs.h"
#include "prefs.h"

extern Preferences preferences;

PageSetupDialog::PageSetupDialog(QWidget *parent, QString name):
	QDialog(parent, name, TRUE)
{
  setCaption( tr("Page setup") );
  QLabel *l1 = new QLabel(this, "Paper size label");
  l1->setGeometry(10,10,80,30);
  l1->setText( tr("Paper size:") );
  psize = new QComboBox(false, this, "Paper size");
  psize->setGeometry(100,10,180,30);
  psize->insertItem(tr("Letter (8.5\" x 11\")"));
  psize->insertItem(tr("Legal (8.5\" x 14\")"));
  psize->insertItem(tr("A4 (210 mm x 297 mm)"));
  psize->insertItem(tr("640x480 pixels"));
  psize->insertItem(tr("800x600 pixels"));
  psize->insertItem(tr("1024x768 pixels"));
  psize->insertItem(tr("Custom paper size"));
  if (preferences.getPageSize() == PAGE_LETTER)
    psize->setCurrentItem(0);
  if (preferences.getPageSize() == PAGE_LEGAL)
    psize->setCurrentItem(1);
  if (preferences.getPageSize() == PAGE_A4)
    psize->setCurrentItem(2);
  if (preferences.getPageSize() == PAGE_640)
    psize->setCurrentItem(3);
  if (preferences.getPageSize() == PAGE_800)
    psize->setCurrentItem(4);
  if (preferences.getPageSize() == PAGE_1024)
    psize->setCurrentItem(5);
  if (preferences.getPageSize() == PAGE_CUSTOM)
    psize->setCurrentItem(6);
  ps_set = psize->currentItem();

  QLabel *l2 = new QLabel(this, "Paper size label");
  l2->setGeometry(10,50,80,30);
  l2->setText( tr("Orientation:") );
  porient = new QComboBox(false, this, "Paper orientation");
  porient->setGeometry(100,50,180,30);
  porient->insertItem( tr("Portrait") );
  porient->insertItem( tr("Landscape") );
  if (preferences.getPageOrientation() == PAGE_PORTRAIT)
    porient->setCurrentItem(0);
  if (preferences.getPageOrientation() == PAGE_LANDSCAPE)
    porient->setCurrentItem(1);
  po_set = porient->currentItem();
  connect( porient, SIGNAL(activated(int)), this, SLOT(SwapWH(int)) );

  int pw = preferences.getPageWidth();
  int ph = preferences.getPageHeight();
  int pu = preferences.getUnits();
  double pwf, phf;
  QString unitstr, n1;
  unitstr = "undef";
  if (pu == PIXELS) {
    unitstr = tr("pixels");
    pwf = (double)pw;
    phf = (double)ph;
  }
  if (pu == ENGLISH) {
    unitstr = tr("inches");
    pwf = (double)pw / 100.0;
    phf = (double)ph / 100.0;
  }
  if (pu == METRIC) {
    unitstr = tr("cm");
    pwf = (double)pw / 40.0;
    phf = (double)ph / 40.0;
  }

  QLabel *l3 = new QLabel( this, "Paper width");
  l3->setGeometry(10,90,80,30);
  l3->setText( tr("Page width:") );
  pwidth = new QLineEdit(this);
  pwidth->setGeometry(100,90,120,30);
  n1.setNum(pwf);
  pwidth->setText(n1);
  ul1 = new QLabel( this, "units" );
  ul1->setText(unitstr);
  ul1->setGeometry(230,90,50,30);
  QLabel *l4 = new QLabel( this, "Paper height");
  l4->setGeometry(10,130,80,30);
  l4->setText( tr("Page height:") );
  pheight = new QLineEdit(this);
  pheight->setGeometry(100,130,120,30);
  n1.setNum(phf);
  pheight->setText(n1);
  ul2 = new QLabel( this, "units" );
  ul2->setText(unitstr);
  ul2->setGeometry(230,130,50,30);
  QPushButton *ok, *dismiss;
  ok = new QPushButton(tr("OK"), this);
  ok->setGeometry(50,170,80,30);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  dismiss = new QPushButton(tr("Cancel"), this);
  dismiss->setGeometry(140,170,80,30);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
}

void PageSetupDialog::onSuccessful() {
  // save settings to Preferences class
  double pw = pwidth->text().toDouble();
  double ph = pheight->text().toDouble();
  int pu = preferences.getUnits();
  int pwf, phf;
  if (pu == PIXELS) {
    pwf = (int)pw;
    phf = (int)ph;
  }
  if (pu == ENGLISH) {
    pwf = (int)(pw * 100.0);
    phf = (int)(ph * 100.0);
  }
  if (pu == METRIC) {
    pwf = (int)(pw * 40.0);
    phf = (int)(ph * 40.0);
  }
  preferences.setPageWidth(pwf);
  preferences.setPageHeight(phf);

  if (psize->currentItem() == 0)
    preferences.setPageSize(PAGE_LETTER);
  if (psize->currentItem() == 1)
    preferences.setPageSize(PAGE_LEGAL);
  if (psize->currentItem() == 2)
    preferences.setPageSize(PAGE_A4);
  if (psize->currentItem() == 3)
    preferences.setPageSize(PAGE_640);
  if (psize->currentItem() == 4)
    preferences.setPageSize(PAGE_800);
  if (psize->currentItem() == 5)
    preferences.setPageSize(PAGE_1024);
  if (psize->currentItem() == 6)
    preferences.setPageSize(PAGE_CUSTOM);

  if (porient->currentItem() == 0)
    preferences.setPageOrientation(PAGE_PORTRAIT);
  if (porient->currentItem() == 1)
    preferences.setPageOrientation(PAGE_LANDSCAPE);

  accept();  // exit gracefully
}

void PageSetupDialog::SwapWH(int nx) {
  if (po_set == porient->currentItem())
    return;
  po_set = porient->currentItem();
  QString n1;
  n1 = pwidth->text();
  pwidth->setText(pheight->text());
  pheight->setText(n1);
}

void PageSetupDialog::setPageSize(int n)
{
  if (n == PAGE_LETTER)
    psize->setCurrentItem(0);
  if (n == PAGE_LEGAL)
    psize->setCurrentItem(1);
  if (n == PAGE_A4)
    psize->setCurrentItem(2);
  if (n == PAGE_640)
    psize->setCurrentItem(3);
  if (n == PAGE_800)
    psize->setCurrentItem(4);
  if (n == PAGE_1024)
    psize->setCurrentItem(5);
}

int PageSetupDialog::getPageSize(void)
{
  if (psize->currentItem() == 0)
    return PAGE_LETTER;
  if (psize->currentItem() == 1)
    return PAGE_LEGAL;
  if (psize->currentItem() == 2)
    return PAGE_A4;
  if (psize->currentItem() == 3)
    return PAGE_640;
  if (psize->currentItem() == 4)
    return PAGE_800;
  if (psize->currentItem() == 5)
    return PAGE_1024;
  return PAGE_LETTER;
}

void PageSetupDialog::setOrientation(int n)
{
  if (n == PAGE_PORTRAIT)
    porient->setCurrentItem(0);
  if (n == PAGE_LANDSCAPE)
    porient->setCurrentItem(1);
}

int PageSetupDialog::getOrientation(void)
{
  if (porient->currentItem() == 0)
    return PAGE_PORTRAIT;
  if (porient->currentItem() == 1)
    return PAGE_LANDSCAPE;
  return PAGE_PORTRAIT;
}
