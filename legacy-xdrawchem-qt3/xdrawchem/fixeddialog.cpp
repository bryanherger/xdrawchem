#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qlayout.h>

// language file
//#include "language.h"
// end language include(s)

#include "render2d.h"
#include "fixeddialog.h"
#include "prefs.h"

extern Preferences preferences;

FixedDialog::FixedDialog(QWidget *parent, QString name) : 
	QDialog(parent, name, TRUE)
{
  int i1; double d1; QString n1;
  QGridLayout *fixedlayout = new QGridLayout(this, 10, 3, 0, 5);
  QPushButton *ok, *cancel, *def1;
  QLabel *t1;
  setBackgroundColor(lightGray);
  setCaption( tr("Set fixed angle and length") );
  t1 = new QLabel(this, "unit");
  t1->setBackgroundColor(lightGray);
  t1->setText( tr("Units:") );
  fixedlayout->addWidget( t1, 0, 0 );
  unitCombo = new QComboBox(false, this, "units");
  unitCombo->insertItem( tr("pixels") );
  // not ready for these...yet
  unitCombo->insertItem( tr("inches") );
  unitCombo->insertItem( tr("cm") );
  unitCombo->setCurrentItem( preferences.getUnits() );
  prevunit = preferences.getUnits();
  QString ustr = unitCombo->currentText();
  connect( unitCombo, SIGNAL(activated(int)), this, SLOT(setUnits(int)) );
  fixedlayout->addWidget( unitCombo, 0, 1 );
  t1 = new QLabel( tr("Bond fixed length:"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 1, 0 );
  ul1 = new QLabel( ustr, this );
  ul1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( ul1, 1, 2 );
  t1 = new QLabel( tr("Bond fixed angle:"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 2, 0 );
  bond_length = new QLineEdit(this);
  i1 = (int)preferences.getBond_fixedlength();
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n1.setNum(d1);
  bond_length->setText(n1);
  fixedlayout->addWidget( bond_length, 1, 1 );
  bond_angle = new QLineEdit(this);
  d1 = preferences.getBond_fixedangle();
  n1.setNum(d1);
  bond_angle->setText(n1);
  fixedlayout->addWidget( bond_angle, 2, 1 );
  t1 = new QLabel( tr("degrees"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 2, 2 );
  t1 = new QLabel( tr("Arrow fixed length:"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 3, 0 );
  t1 = new QLabel( tr("Arrow fixed angle:"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 4, 0 );
  arrow_length = new QLineEdit(this);
  i1 = (int)preferences.getArrow_fixedlength();
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n1.setNum(d1);
  arrow_length->setText(n1);
  fixedlayout->addWidget( arrow_length, 3, 1 );
  ul2 = new QLabel( ustr, this );
  ul2->setBackgroundColor(lightGray);
  fixedlayout->addWidget( ul2, 3, 2 );
  arrow_angle = new QLineEdit(this);
  d1 = preferences.getArrow_fixedangle();
  n1.setNum(d1);
  arrow_angle->setText(n1);
  fixedlayout->addWidget( arrow_angle, 4, 1 );
  t1 = new QLabel( tr("degrees"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 4, 2 );
  t1 = new QLabel( tr("Double bond spacing:"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 5, 0 );
  dbond_offset = new QLineEdit(this);
  i1 = (int)preferences.getDoubleBondOffset();
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n1.setNum(d1);
  dbond_offset->setText(n1);
  fixedlayout->addWidget( dbond_offset, 5, 1 );
  ul3 = new QLabel( ustr, this );
  ul3->setBackgroundColor(lightGray);
  fixedlayout->addWidget( ul3, 5, 2 );
  //anti_toggle = new QCheckBox(tr("Anti-aliasing"), this);
  //anti_toggle->setGeometry(20,170,100,30);
  showgrid = new QComboBox( false, this, tr("Show grid") );
  showgrid->insertItem( tr("Do not show grid") );
  showgrid->insertItem( tr("Show square grid") );
  showgrid->insertItem( tr("Show hex grid") );
  showgrid->setCurrentItem(preferences.getDrawGrid());
  fixedlayout->addMultiCellWidget(showgrid, 6, 6, 0, 2);
  snapgrid = new QCheckBox( tr("Snap to grid"), this );
  snapgrid->setBackgroundColor(lightGray);
  bool sg = false;
  if (preferences.getSnapGrid() == GRID_SQUARE) sg = true;
  snapgrid->setChecked(sg);
  fixedlayout->addMultiCellWidget(snapgrid, 7, 7, 0, 2);
  t1 = new QLabel( tr("Grid spacing"), this);
  t1->setBackgroundColor(lightGray);
  fixedlayout->addWidget( t1, 8, 0 );
  gridspacing = new QLineEdit(this);
  i1 = (int)preferences.getGridSpacing();
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n1.setNum(d1);
  gridspacing->setText(n1);
  fixedlayout->addWidget( gridspacing, 8, 1 );
  ul4 = new QLabel( ustr, this );
  ul4->setBackgroundColor(lightGray);
  fixedlayout->addWidget( ul4, 8, 2 );
  ok = new QPushButton( tr("OK"), this);
  ok->setPalette(QPalette(lightGray));
  fixedlayout->addWidget( ok, 9, 0 );
  connect(ok, SIGNAL(clicked()), SLOT(onSuccess()) );
  def1 = new QPushButton( tr("Default"), this);
  def1->setPalette(QPalette(lightGray));
  fixedlayout->addWidget( def1, 9, 1 );
  connect(def1, SIGNAL(clicked()), SLOT(setDefaults()) );
  cancel = new QPushButton( tr("Cancel"), this);
  cancel->setPalette(QPalette(lightGray));
  fixedlayout->addWidget( cancel, 9, 2 );
  connect(cancel, SIGNAL(clicked()), SLOT(reject()) );
}

void FixedDialog::onSuccess() {
  double d1;

  preferences.setDrawGrid(showgrid->currentItem());

  if (snapgrid->isChecked()) {
    preferences.setSnapGrid(GRID_SQUARE);
  } else {
    preferences.setSnapGrid(GRID_NONE);
  }
  preferences.setUnits(unitCombo->currentItem());

  d1 = bond_length->text().toDouble();
  if (prevunit == PIXELS) bl = (int)d1;
  if (prevunit == ENGLISH) bl = my_round(d1 * 100.0);
  if (prevunit == METRIC) bl = my_round(d1 * 40.0);
  d1 = arrow_length->text().toDouble();
  if (prevunit == PIXELS) al = (int)d1;
  if (prevunit == ENGLISH) al = my_round(d1 * 100.0);
  if (prevunit == METRIC) al = my_round(d1 * 40.0);
  d1 = dbond_offset->text().toDouble();
  if (prevunit == PIXELS) dsp = (int)d1;
  if (prevunit == ENGLISH) dsp = my_round(d1 * 100.0);
  if (prevunit == METRIC) dsp = my_round(d1 * 40.0);
  d1 = gridspacing->text().toDouble();
  if (prevunit == PIXELS) gsp = (int)d1;
  if (prevunit == ENGLISH) gsp = my_round(d1 * 100.0);
  if (prevunit == METRIC) gsp = my_round(d1 * 40.0);

  d1 = arrow_angle->text().toDouble();
  if (d1 > 0.0)
    preferences.setArrow_fixedangle(d1);
  if (al > 0.0)
    preferences.setArrow_fixedlength(al);
  d1 = bond_angle->text().toDouble();
  if (d1 > 0.0)
    preferences.setBond_fixedangle(d1);
  if (bl > 0.0)
    preferences.setBond_fixedlength(bl);
  if (dsp > 0.0)
    preferences.setDoubleBondOffset(dsp);
  if (gsp > 0.0)
    preferences.setGridSpacing(my_round(gsp));

  accept();
}

void FixedDialog::setDefaults() {
  QString n;
  int i1; double d1;
  i1 = 25;
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n.setNum(d1);
  bond_length->setText(n);
  n.setNum(15);
  bond_angle->setText(n);
  i1 = 50;
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n.setNum(d1);
  arrow_length->setText(n);
  n.setNum(15);
  arrow_angle->setText(n);
  i1 = 1;
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n.setNum(d1);
  dbond_offset->setText(n);
  showgrid->setCurrentItem(GRID_NONE);
  snapgrid->setChecked(false);
  i1 = 50;
  if (prevunit == PIXELS) d1 = (double)i1;
  if (prevunit == ENGLISH) d1 = (double)i1 / 100.0;
  if (prevunit == METRIC) d1 = (double)i1 / 40.0;
  n.setNum(d1);
  gridspacing->setText(n);
  //anti_toggle->setChecked(false);
}

void FixedDialog::setUnits(int n1) {
  double d1; QString n;
  d1 = bond_length->text().toDouble();
  if (prevunit == PIXELS) bl = (int)d1;
  if (prevunit == ENGLISH) bl = my_round(d1 * 100.0);
  if (prevunit == METRIC) bl = my_round(d1 * 40.0);
  d1 = arrow_length->text().toDouble();
  if (prevunit == PIXELS) al = (int)d1;
  if (prevunit == ENGLISH) al = my_round(d1 * 100.0);
  if (prevunit == METRIC) al = my_round(d1 * 40.0);
  d1 = dbond_offset->text().toDouble();
  if (prevunit == PIXELS) dsp = (int)d1;
  if (prevunit == ENGLISH) dsp = my_round(d1 * 100.0);
  if (prevunit == METRIC) dsp = my_round(d1 * 40.0);
  d1 = gridspacing->text().toDouble();
  if (prevunit == PIXELS) gsp = (int)d1;
  if (prevunit == ENGLISH) gsp = my_round(d1 * 100.0);
  if (prevunit == METRIC) gsp = my_round(d1 * 40.0);
  QString ustr;
  if (n1 == 0) {
    ustr = tr("pixels");
  }
  if (n1 == 1) {
    ustr = tr("inches");
  }
  if (n1 == 2) {
    ustr = tr("cm");
  }
  ul1->setText(ustr);
  ul2->setText(ustr);
  ul3->setText(ustr);
  ul4->setText(ustr);
  prevunit = n1;
  if (prevunit == PIXELS) d1 = (double)bl;
  if (prevunit == ENGLISH) d1 = (double)bl / 100.0;
  if (prevunit == METRIC) d1 = (double)bl / 40.0;
  n.setNum(d1);
  bond_length->setText(n);
  if (prevunit == PIXELS) d1 = (double)al;
  if (prevunit == ENGLISH) d1 = (double)al / 100.0;
  if (prevunit == METRIC) d1 = (double)al / 40.0;
  n.setNum(d1);
  arrow_length->setText(n);
  if (prevunit == PIXELS) d1 = (double)dsp;
  if (prevunit == ENGLISH) d1 = (double)dsp / 100.0;
  if (prevunit == METRIC) d1 = (double)dsp / 40.0;
  n.setNum(d1);
  dbond_offset->setText(n);
  if (prevunit == PIXELS) d1 = (double)gsp;
  if (prevunit == ENGLISH) d1 = (double)gsp / 100.0;
  if (prevunit == METRIC) d1 = (double)gsp / 40.0;
  n.setNum(d1);
  gridspacing->setText(n);
}

void FixedDialog::setLength_bond(double t)
{
  QString n;
  n.setNum(t);

  bond_length->setText(n);
}

void FixedDialog::setAngle_bond(double t)
{
  QString n;
  n.setNum(t);

  bond_angle->setText(n);
}

double FixedDialog::getLength_bond(void)
{
  double t;
  bool b;

  t = bond_length->text().toDouble(&b);
  if (b == true)
    return t;
  return -1.0;
}

double FixedDialog::getAngle_bond(void)
{
  double t;
  bool b;

  t = bond_angle->text().toDouble(&b);
  if (b == true)
    return t;
  return -1.0;
}

void FixedDialog::setLength_arrow(double t)
{
  QString n;
  n.setNum(t);

  arrow_length->setText(n);
}

void FixedDialog::setAngle_arrow(double t)
{
  QString n;
  n.setNum(t);

  arrow_angle->setText(n);
}

double FixedDialog::getLength_arrow(void)
{
  double t;
  bool b;

  t = arrow_length->text().toDouble(&b);
  if (b == true)
    return t;
  return -1.0;
}

double FixedDialog::getAngle_arrow(void)
{
  double t;
  bool b;

  t = arrow_angle->text().toDouble(&b);
  if (b == true)
    return t;
  return -1.0;
}

double FixedDialog::getDoubleBondOffset(void) {
  double t;
  bool b;

  t = dbond_offset->text().toDouble(&b);
  if (b == true)
    return t;
  return -1.0;}

void FixedDialog::setDoubleBondOffset(double t) {
  QString n;
  n.setNum(t);

  dbond_offset->setText(n);
}
