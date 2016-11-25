#include <qwidget.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qbitmap.h>
#include <qprinter.h>
#include <qcombobox.h>
#include <qvbox.h>
#include <qvbuttongroup.h>
#include <qlayout.h>
#include <qpushbutton.h>
#include <qradiobutton.h>
#include <qcolordialog.h>

// language file
//#include "language.h"
// end language include(s)

// XPM's
#include "lines.h"

#include "bondedit.h"
#include "previewwidget.h"
#include "defs.h"

BondEditDialog::BondEditDialog(QWidget *parent, QString name, DPoint *s, DPoint *e, int ty, int o, int d, int th, int st, QColor c1):
	QDialog(parent, name, TRUE)
{
  s1 = s; e1 = e;
  type = ty;
  order = o; dash = d; thick = th;
  style = st;
  color = c1;

  QGridLayout *mygrid;
  mygrid = new QGridLayout(this,5,2,5);

  pw = new PreviewWidget(this);
  pw->Update(type, thick, dash, order, style);
  mygrid->addWidget(pw, 0, 0);

  QPixmap *tmp_pm = new QPixmap(30, 18);
  QBitmap mask0(30, 18);
  mask0.fill(Qt::color1);
  tmp_pm->setMask(mask0);
  tmp_pm->fill(color);
  pb1 = new QPushButton(*tmp_pm, tr("Color"), this);
  connect(pb1, SIGNAL(pressed()), this, SLOT(GetNewColor()) );
  mygrid->addWidget(pb1, 1, 0);

  if (type == TYPE_ARROW) {
    setCaption( tr("Arrow editor") );
    QVButtonGroup *stbg = new QVButtonGroup( tr("Style:"), this);
    connect(stbg, SIGNAL(pressed(int)), this, SLOT(StyleChanged(int)) );
    QRadioButton *rb1;
    rb1 = new QRadioButton( tr("Plain arrow"), stbg);
    if (style == ARROW_REGULAR) rb1->setChecked(true);
    stbg->insert(rb1, ARROW_REGULAR);
    rb1 = new QRadioButton( tr("NR arrow"), stbg);
    if (style == ARROW_DIDNT_WORK) rb1->setChecked(true);
    stbg->insert(rb1, ARROW_DIDNT_WORK);
    rb1 = new QRadioButton( tr("Dashed arrow"), stbg);
    if (style == ARROW_DASH) rb1->setChecked(true);
    stbg->insert(rb1, ARROW_DASH);
    rb1 = new QRadioButton( tr("Two-way arrow"), stbg);
    if (style == ARROW_BI1) rb1->setChecked(true);
    stbg->insert(rb1, ARROW_BI1);
    rb1 = new QRadioButton( tr("Split two-way arrow"), stbg);
    if (style == ARROW_BI2) rb1->setChecked(true);
    stbg->insert(rb1, ARROW_BI2);
    rb1 = new QRadioButton( tr("Retrosynthetic arrow"), stbg);
    if (style == ARROW_RETRO) rb1->setChecked(true);
    stbg->insert(rb1, ARROW_RETRO);
    mygrid->addMultiCellWidget(stbg, 0, 1, 1, 1);

    // bond thickness list
    ltList = new QComboBox(false, this);
    QPixmap px1;
    QBitmap mask0(12, 12);
    mask0.fill(Qt::color0);
    px1 = QPixmap(line1);
    px1.setMask(mask0);
    ltList->insertItem(line1, "1");
    ltList->insertItem(line2, "2");
    ltList->insertItem(line3, "3");
    ltList->insertItem(line4, "4");
    ltList->insertItem(line5, "5");
    connect(ltList, SIGNAL(activated(int)), this, SLOT(SetThick(int)) );
    mygrid->addWidget(ltList, 2, 1);
    ltList->setCurrentItem(th-1);
  }
  if (type == TYPE_BOND) {
    setCaption( tr("Bond editor") );
    // bond order list
    QVButtonGroup *stbg = new QVButtonGroup( tr("Bond order:"), this);
    connect(stbg, SIGNAL(pressed(int)), this, SLOT(OrderChanged(int)) );
    QRadioButton *rb1;
    rb1 = new QRadioButton( tr("Dashed line"), stbg);
    if ( (order == 1) && (dash == 1) ) rb1->setChecked(true);
    stbg->insert(rb1, 0);
    rb1 = new QRadioButton( tr("Single"), stbg);
    if ( (order == 1) && (dash == 0) ) rb1->setChecked(true);
    stbg->insert(rb1, 1);
    rb1 = new QRadioButton( tr("Stereo-up"), stbg);
    if ( (order == 5) && (dash == 0) ) rb1->setChecked(true);
    stbg->insert(rb1, 5);
    rb1 = new QRadioButton( tr("Stereo-down"), stbg);
    if ( (order == 7) && (dash == 0) ) rb1->setChecked(true);
    stbg->insert(rb1, 7);
    rb1 = new QRadioButton( tr("Wavy"), stbg);
    if ( (order == 6) && (dash == 0) ) rb1->setChecked(true);
    stbg->insert(rb1, 6);
    rb1 = new QRadioButton( tr("Single/double (aromatic)"), stbg);
    if ( (order == 2) && (dash == 1) ) rb1->setChecked(true);
    stbg->insert(rb1, 11);
    rb1 = new QRadioButton( tr("Double"), stbg);
    if ( (order == 2) && (dash == 0) ) rb1->setChecked(true);
    stbg->insert(rb1, 2);
    rb1 = new QRadioButton( tr("Double/triple"), stbg);
    if ( (order == 3) && (dash == 1) ) rb1->setChecked(true);
    stbg->insert(rb1, 12);
    rb1 = new QRadioButton( tr("Triple"), stbg);
    if ( (order == 3) && (dash == 0) ) rb1->setChecked(true);
    stbg->insert(rb1, 3);
    mygrid->addMultiCellWidget(stbg, 0, 1, 1, 1);
    //QToolTip::add(ltList, TOOLTIP_SET_THICKNESS);
    //QWhatsThis::add(ltList, "Set Line Thickness");
    QLabel *lb1 = new QLabel(this, "thickness label");
    lb1->setText( tr("Bond thickness:") );
    mygrid->addWidget(lb1, 2, 0);
    // bond thickness list
    ltList = new QComboBox(false, this);
    QPixmap px1;
    QBitmap mask0(12, 12);
    mask0.fill(Qt::color0);
    px1 = QPixmap(line1);
    px1.setMask(mask0);
    ltList->insertItem(line1, "1");
    ltList->insertItem(line2, "2");
    ltList->insertItem(line3, "3");
    ltList->insertItem(line4, "4");
    ltList->insertItem(line5, "5");
    connect(ltList, SIGNAL(activated(int)), this, SLOT(SetThick(int)) );
    mygrid->addWidget(ltList, 2, 1);
    ltList->setCurrentItem(th-1);

    QLabel *lb2 = new QLabel(this, "double bond label");
    lb2->setText( tr("Double bond alignment:") );
    mygrid->addWidget(lb2, 3, 0);
    dbList = new QComboBox(false, this);
    dbList->insertItem( tr("Auto") );
    dbList->insertItem( tr("Left") );
    dbList->insertItem( tr("Center") );
    dbList->insertItem( tr("Right") );
    connect(dbList, SIGNAL(activated(int)), this, SLOT(SetDoubleBond(int)) );
    mygrid->addWidget(dbList, 3, 1);
  }
  if (type == TYPE_BRACKET) {
    setCaption( tr("Bracket editor") );
    QVButtonGroup *stbg = new QVButtonGroup( tr("Style:"), this);
    connect(stbg, SIGNAL(pressed(int)), this, SLOT(StyleChanged(int)) );
    QRadioButton *rb1;
    rb1 = new QRadioButton( tr("Square bracket"), stbg);
    if (style == BRACKET_SQUARE) rb1->setChecked(true);
    stbg->insert(rb1, BRACKET_SQUARE);
    rb1 = new QRadioButton( tr("Parentheses"), stbg);
    if (style == BRACKET_CURVE) rb1->setChecked(true);
    stbg->insert(rb1, BRACKET_CURVE);
    rb1 = new QRadioButton( tr("Curly bracket"), stbg);
    if (style == BRACKET_BRACE) rb1->setChecked(true);
    stbg->insert(rb1, BRACKET_BRACE);
    rb1 = new QRadioButton( tr("Box"), stbg);
    if (style == BRACKET_BOX) rb1->setChecked(true);
    stbg->insert(rb1, BRACKET_BOX);
    rb1 = new QRadioButton( tr("Ellipse"), stbg);
    if (style == BRACKET_ELLIPSE) rb1->setChecked(true);
    stbg->insert(rb1, BRACKET_ELLIPSE);
    mygrid->addMultiCellWidget(stbg, 0, 1, 1, 1);
  }
  if (type == TYPE_CURVEARROW) {
    setCaption( tr("Curved Arrow editor") );
    QVButtonGroup *stbg = new QVButtonGroup( tr("Style:"), this);
    connect(stbg, SIGNAL(pressed(int)), this, SLOT(StyleChanged(int)) );
    QRadioButton *rb1;
    rb1 = new QRadioButton( tr("90 degree clockwise"), stbg);
    if (style == CA_CW90) rb1->setChecked(true);
    stbg->insert(rb1, CA_CW90);
    rb1 = new QRadioButton( tr("180 degree clockwise"), stbg);
    if (style == CA_CW180) rb1->setChecked(true);
    stbg->insert(rb1, CA_CW180);
    rb1 = new QRadioButton( tr("270 degree clockwise"), stbg);
    if (style == CA_CW270) rb1->setChecked(true);
    stbg->insert(rb1, CA_CW270);
    rb1 = new QRadioButton( tr("90 degree counterclockwise"), stbg);
    if (style == CA_CCW90) rb1->setChecked(true);
    stbg->insert(rb1, CA_CCW90);
    rb1 = new QRadioButton( tr("180 degree counterclockwise"), stbg);
    if (style == CA_CCW180) rb1->setChecked(true);
    stbg->insert(rb1, CA_CCW180);
    rb1 = new QRadioButton( tr("270 degree counterclockwise"), stbg);
    if (style == CA_CCW270) rb1->setChecked(true);
    stbg->insert(rb1, CA_CCW270);
    mygrid->addMultiCellWidget(stbg, 0, 1, 1, 1);
  }
  if (type == TYPE_SYMBOL) {
    setCaption( tr("Symbol editor") );
    QVButtonGroup *stbg = new QVButtonGroup( tr("Style:"), this);
    connect(stbg, SIGNAL(pressed(int)), this, SLOT(StyleChanged(int)) );
    QRadioButton *rb1;
    rb1 = new QRadioButton( tr("Positive charge"), stbg);
    if (style == SYM_PLUS) rb1->setChecked(true);
    stbg->insert(rb1, SYM_PLUS);
    rb1 = new QRadioButton( tr("Negative charge"), stbg);
    if (style == SYM_MINUS) rb1->setChecked(true);
    stbg->insert(rb1, SYM_MINUS);
    rb1 = new QRadioButton( tr("Partial positive charge"), stbg);
    if (style == SYM_DELTA_PLUS) rb1->setChecked(true);
    stbg->insert(rb1, SYM_DELTA_PLUS);
    rb1 = new QRadioButton( tr("Partial negative charge"), stbg);
    if (style == SYM_DELTA_MINUS) rb1->setChecked(true);
    stbg->insert(rb1, SYM_DELTA_MINUS);
    rb1 = new QRadioButton( tr("Single electron"), stbg);
    if (style == SYM_1E) rb1->setChecked(true);
    stbg->insert(rb1, SYM_1E);
    rb1 = new QRadioButton( tr("Electron pair"), stbg);
    if (style == SYM_2E) rb1->setChecked(true);
    stbg->insert(rb1, SYM_2E);
    rb1 = new QRadioButton( tr("Stereochemical ring hydrogen"), stbg);
    if (style == SYM_RING_UP) rb1->setChecked(true);
    stbg->insert(rb1, SYM_RING_UP);
    rb1 = new QRadioButton( tr("p orbital"), stbg);
    if (style == SYM_P_ORBITAL) rb1->setChecked(true);
    stbg->insert(rb1, SYM_P_ORBITAL);
    rb1 = new QRadioButton( tr("p double"), stbg);
    if (style == SYM_P_DOUBLE) rb1->setChecked(true);
    stbg->insert(rb1, SYM_P_DOUBLE);
    rb1 = new QRadioButton( tr("bead"), stbg);
    if (style == SYM_BEAD) rb1->setChecked(true);
    stbg->insert(rb1, SYM_BEAD);
    mygrid->addMultiCellWidget(stbg, 0, 1, 1, 1);
  }
  QPushButton *ok, *dismiss;
  ok = new QPushButton( tr("OK"), this);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  mygrid->addWidget(ok, 4, 0);
  dismiss = new QPushButton( tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  mygrid->addWidget(dismiss, 4, 1);
}

void BondEditDialog::GetNewColor() {
  QColor nc1 = QColorDialog::getColor(color);
  if (nc1.isValid()) {
    color = nc1;
    QPixmap tmp_pm(30, 18);
    QBitmap mask0(30, 18);
    mask0.fill(Qt::color1);
    tmp_pm.setMask(mask0);
    tmp_pm.fill(color);
    pb1->setIconSet(tmp_pm);    
  }
}
