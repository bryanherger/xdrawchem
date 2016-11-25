/***************************************************************************
                          crings_dialog.cpp  -  Dialog for Custom Rigns
                             -------------------
    begin                : Sat Aug 31 2002
    copyright            : (C) 2002 by Adam Tenderholt
    email                : atenderholt@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "crings_dialog.h"
#include "defs.h"
#include "prefs.h"

#include <qpixmap.h>
#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

extern Preferences preferences;

CustomRingWidget::CustomRingWidget(QWidget* parent, const char* name)
  : QWidget(parent,name)  {

  setFixedSize(320,310);

  render=new Render2D(0);
  data=new ChemData(this);

  widget=new QWidget(this);
  widget->setFixedSize(300,240);
  widget->setGeometry(10,10,300,240);
  widget->show();

  render->setChemData(data);
  data->setRender2D(render);

  title=new QLineEdit(this);
  title->setGeometry(100,270,160,20);

  connect(title,SIGNAL(textChanged(const QString&)),this,SIGNAL(signalTextChanged(const QString&)));

  label=new QLabel("Title: ",this);
  label->setGeometry(60,270,25,20);

  //render->setMode_SelectNone();

}

CustomRingWidget::~CustomRingWidget() {

  if(render)
    delete render;
  if(data)
    delete data;
  if(title)
    delete title;
  if(label)
    delete label;

}

void CustomRingWidget::slotTitle(QString& string) {

  data->save(string);
  cout << "saved: " << string << endl << "done with crwidget" << endl;
  string.truncate(string.length() - 4);
  string.append(".png");
  cout << "png: " << string << endl;
  pixmap.save(string, "PNG");
}

void CustomRingWidget::setMolecule(Molecule* m) {

  data->addMolecule(m);
  Render2D* real_render=m->getRender2D();

  //QPixmap pixmap=real_render->MakePixmap();

  QPixmap mol = real_render->MakeFullPixmap();
  QRect rr1 = m->BoundingBoxAll();
  rr1.setLeft(rr1.left() - 4);
  rr1.setTop(rr1.top() - 4);
  rr1.setBottom(rr1.bottom() + 4);
  rr1.setRight(rr1.right() + 4);
  QPixmap mol1(rr1.size());
  bitBlt(&mol1, QPoint(0,0), &mol, rr1);

  pixmap=mol1;
  //pixmap.fill(Qt::white);
  //if(pixmap.isNull())
    //cerr << "bad pixmpa!!!" << endl;
  //m->SelectAll();
  //QRect rect=m->BoundingBox();

  QPainter painter(widget);
  //painter.drawPixmap(QPoint(0,0),pixmap,QRect(0,0,rect.width(),rect.height()));
  //cerr << rect.x() << " " << rect.y() << " " << rect.width() << " " << endl;
  //bitBlt(widget,QPoint(0,0),&pixmap,rect);
  painter.drawPixmap(QPoint(0,0),mol1);
  widget->show();
}

void CustomRingWidget::paintEvent(QPaintEvent* pe) {

  QPainter p(widget);
  p.drawPixmap(QPoint(0,0),pixmap);
}

CustomRingDialog::CustomRingDialog(QWidget* parent,const char* name)
  : QDialog(parent,"customring",true) {
  QGridLayout *mygrid;
  mygrid = new QGridLayout(this,2,2,0);

  setCaption( tr("Add custom ring to menu list") );
  m_widget=new CustomRingWidget(this);
  mygrid->addMultiCellWidget(m_widget,1,1,1,2);

  connect(m_widget,SIGNAL(signalTextChanged(const QString&)),this,SLOT(slotTextChanged(const QString&)));
  connect(this,SIGNAL(signalTitle(QString&)),m_widget,SLOT(slotTitle(QString&)));

  //disableResize();
  //enableButtonOK(false);
  QPushButton *ok, *dismiss;
  ok = new QPushButton(tr("OK"), this);
  ok->setPalette(QPalette(lightGray));
  connect(ok, SIGNAL(clicked()), SLOT(slotOk()) );
  mygrid->addWidget(ok,2,1);
  dismiss = new QPushButton(tr("Cancel"), this);
  dismiss->setPalette(QPalette(lightGray));
  connect(dismiss, SIGNAL(clicked()), SLOT(reject()) );
  mygrid->addWidget(dismiss,2,2);
}

CustomRingDialog::~CustomRingDialog() {

  if(m_widget)
    delete m_widget;
}

void CustomRingDialog::slotTextChanged(const QString& string) {

  //if(string.isEmpty())
  //  enableButtonOK(false);
  //else //if string!=QString::null
  //  enableButtonOK(true);
}

void CustomRingDialog::slotOk() {

  QString title=m_widget->getTitle();
  std::cerr << title << endl;
  QString filename=title.lower();
  filename.append(".cml");

  QString file = preferences.getCustomRingDir();

  file+=filename;

  emit signalTitle(file);

  accept();
}

//eof


