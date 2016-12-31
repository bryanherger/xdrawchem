#include <qwidget.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qcolordialog.h>
#include <qcolor.h>
#include <qfont.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qprinter.h>
#include <qlayout.h>

#include "textshapedialog.h"
#include "defs.h"

TextShapeDialog::TextShapeDialog(QWidget *parent, QString name):
	QDialog(parent, name, TRUE)
{
  QGridLayout *tsgl = new QGridLayout(this, 6, 2);
  setCaption( tr("Edit text shape") );

  stype = new QComboBox(false, this, "Shape type");
  stype->insertItem( tr("No shape") );
  stype->insertItem( tr("Ellipse") );
  stype->insertItem( tr("Rectangle") );
  stype->insertItem( tr("Circle") );
  stype->insertItem( tr("Square") );
  tsgl->addWidget(stype, 0, 0);

  borderCheck = new QCheckBox(this, "border check");
  borderCheck->setText( tr("Border") );
  tsgl->addWidget(borderCheck, 1, 0);
  bbutton = new QPushButton(this);
  bbutton->setText( tr("Border color") );
  bbutton->setPaletteBackgroundColor( QColor(0,0,0) );
  bbutton->setPaletteForegroundColor( QColor(255,255,255) );
  tsgl->addWidget(bbutton, 1, 1);
  connect(bbutton, SIGNAL(clicked()), SLOT(editBorderColor()) );

  fillCheck = new QCheckBox(this, "fill check");
  fillCheck->setText( tr("Fill") );
  tsgl->addWidget(fillCheck, 2, 0);
  fbutton = new QPushButton(this);
  fbutton->setText( tr("Fill color") );
  tsgl->addWidget(fbutton, 2, 1);
  connect(fbutton, SIGNAL(clicked()), SLOT(editFillColor()) );

  QLabel *lw1, *lh1;
  lw1 = new QLabel(this);
  lw1->setText( tr("Width:") );
  tsgl->addWidget(lw1, 3, 0);
  lh1 = new QLabel(this);
  lh1->setText( tr("Height:") );
  tsgl->addWidget(lh1, 3, 1);

  swidth = new QLineEdit(this);
  tsgl->addWidget(swidth, 4, 0);

  sheight = new QLineEdit(this);
  tsgl->addWidget(sheight, 4, 1);

  QPushButton *ok, *dismiss;
  ok = new QPushButton(tr("OK"), this);
  ok->setPalette(QPalette(lightGray));
  tsgl->addWidget(ok, 5, 0);
  connect(ok, SIGNAL(clicked()), SLOT(accept()) );
  dismiss = new QPushButton(tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  tsgl->addWidget(dismiss, 5, 1);
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
}

void TextShapeDialog::editBorderColor() {
  QColor c1;

  c1 = QColorDialog::getColor(bc);

  bbutton->setPaletteBackgroundColor(c1);
  bc = c1;
}

void TextShapeDialog::editFillColor() {
  QColor c1;

  c1 = QColorDialog::getColor(fc);

  fbutton->setPaletteBackgroundColor(c1);
  fc = c1;
}

void TextShapeDialog::setBorderColor(QColor c1) { 
  bc = c1;
  bbutton->setPaletteBackgroundColor(c1);
}

void TextShapeDialog::setFillColor(QColor c1) { 
  fc = c1;
  fbutton->setPaletteBackgroundColor(c1);
}
