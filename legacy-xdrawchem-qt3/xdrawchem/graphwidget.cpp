// graphwidget.cpp - definitions for functions of class GraphWidget

#include <iostream>
#include <qwidget.h>
#include <qpixmap.h>
#include <qpaintdevicemetrics.h>
#include <qclipboard.h>
#include <qapplication.h>

using std::cout;
using std::endl;

using std::cout;

#include "defs.h"
#include "render2d.h"
#include "graphwidget.h"
#include "graphdata.h"

GraphWidget::GraphWidget(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  datatype = 0;
  output = 0;
  PrintSetup();
  m_toclip="";
}

void GraphWidget::paintEvent(QPaintEvent *pe) {
  QPainter p;
  QString s, ft;
  bool cmp2 = false;
  int cx = 10, cy = 10;

  if (output == 0)
    p.begin(this);
  else
    p.begin(printer);

  if (p1.width() > 0) {
    p.drawPixmap(QPoint(10,10), p1);
    cy = cy + p1.height() + 5;
  }
  if (datatype == 1) { // 13C-NMR
    cy = cy + 90;
    // draw baseline
    p.drawLine( QPoint(50,cy), QPoint(550,cy) );
    p.drawLine( QPoint(500,cy), QPoint(500,cy+3) );
    p.drawText( QPoint(500,cy+15), "0 ppm");
    p.drawLine( QPoint(440,cy), QPoint(440,cy+3) );
    p.drawText( QPoint(440,cy+15), "20");
    p.drawLine( QPoint(380,cy), QPoint(380,cy+3) );
    p.drawText( QPoint(380,cy+15), "40");
    p.drawLine( QPoint(320,cy), QPoint(320,cy+3) );
    p.drawText( QPoint(320,cy+15), "60");
    p.drawLine( QPoint(260,cy), QPoint(260,cy+3) );
    p.drawText( QPoint(260,cy+15), "80");
    p.drawLine( QPoint(200,cy), QPoint(200,cy+3) );
    p.drawText( QPoint(200,cy+15), "100");
    p.drawLine( QPoint(140,cy), QPoint(140,cy+3) );
    p.drawText( QPoint(140,cy+15), "120");
    p.drawLine( QPoint(80,cy), QPoint(80,cy+3) );
    p.drawText( QPoint(80,cy+15), "140");
    for (GraphData *tg = peaks.first(); tg != 0; tg = peaks.next()) {
      int cx = 500 - (int)(tg->value * 3.0);
      int intense = tg->intensity * 30;
      if (intense > 90) intense = 90;
      p.setPen(tg->pcolor);
      p.drawLine( QPoint(cx, cy), QPoint(cx, cy - intense) );
    }
    cy = cy + 45;
  }
  if (datatype == 2) // IR
    cy = cy + 20;
  if (datatype == 3) { // 1H-NMR
    cy = cy + 90;
    // draw baseline
    p.drawLine( QPoint(50,cy), QPoint(550,cy) );
    p.drawLine( QPoint(500,cy), QPoint(500,cy+3) );
    p.drawText( QPoint(500,cy+15), "-2 ppm");
    p.drawLine( QPoint(440,cy), QPoint(440,cy+3) );
    p.drawText( QPoint(440,cy+15), "0");
    p.drawLine( QPoint(380,cy), QPoint(380,cy+3) );
    p.drawText( QPoint(380,cy+15), "2");
    p.drawLine( QPoint(320,cy), QPoint(320,cy+3) );
    p.drawText( QPoint(320,cy+15), "4");
    p.drawLine( QPoint(260,cy), QPoint(260,cy+3) );
    p.drawText( QPoint(260,cy+15), "6");
    p.drawLine( QPoint(200,cy), QPoint(200,cy+3) );
    p.drawText( QPoint(200,cy+15), "8");
    p.drawLine( QPoint(140,cy), QPoint(140,cy+3) );
    p.drawText( QPoint(140,cy+15), "10");
    p.drawLine( QPoint(80,cy), QPoint(80,cy+3) );
    p.drawText( QPoint(80,cy+15), "12");
    for (GraphData *tg = peaks.first(); tg != 0; tg = peaks.next()) {
      int cx = 440 - (int)(tg->value * 30.0);
      int intense = tg->intensity * 10;
      if (intense > 90) intense = 90;
      p.setPen(tg->pcolor);
      p.drawLine( QPoint(cx, cy), QPoint(cx, cy - intense) );
    }
    cy = cy + 45;
  }
 
  for (GraphData *tg = peaks.first(); tg != 0; tg = peaks.next()) {
    s.setNum(tg->intensity);
    ft = tg->fulltext;
    if (datatype == 1) { // 13C-NMR
      ft.append(", intensity ");
      ft.append(s);
    }
    p.setPen(tg->pcolor);
    if (tg->pcolor == QColor(0,0,255)) cmp2 = true;
    p.drawText(QPoint(cx, cy), ft);
	//And now to clipboard
	m_toclip+=ft+"\n";
    cy = cy + 15;
  }
  if (cmp2) {
    p.setPen(QColor(255,0,0));
    p.drawText(QPoint(cx, cy), tr("Red is reactant"));
    cy = cy + 15;
    p.setPen(QColor(0,0,255));
    p.drawText(QPoint(cx, cy), tr("Blue is product"));
  }

  p.end();
}

void GraphWidget::PrintSetup() {
  printer = new QPrinter;
  printer->setFullPage(true);
  printer->setPageSize(QPrinter::Letter);
  printer->setOrientation(QPrinter::Portrait);
  printer->setColorMode(QPrinter::GrayScale);
}

void GraphWidget::Print() {
  cout << "Graph::Print" << endl;
  if (!printer->setup(this)) {
    return;
  }

  output = 1;
  repaint();
  output = 0;
  repaint();
}

void GraphWidget::AddPeak(double v1, QColor c1, QString l1, QString t1) {
  GraphData *g1 = new GraphData;
  g1->value = v1;
  g1->fulltext = t1;
  g1->pcolor = c1;
  if (l1.isEmpty() == false) { g1->label = l1; g1->drawlabel = true; }
  // check for duplicate peaks
  for (GraphData *tg = peaks.first(); tg != 0; tg = peaks.next()) {
    if (tg->value == v1) {
      tg->intensity++;
      return;
    }
  }
  peaks.append(g1);
}

void GraphWidget::AddPeak(double v1, int m1, QColor c1, 
			  QString l1, QString t1) {
  GraphData *g1 = new GraphData;
  g1->value = v1;
  g1->fulltext = t1;
  g1->pcolor = c1;
  g1->intensity = m1;
  if (l1.isEmpty() == false) { g1->label = l1; g1->drawlabel = true; }
  peaks.append(g1);
}

void GraphWidget::Export()
{
  QClipboard* clip=QApplication::clipboard();
  if(m_toclip!="")
    clip->setText(m_toclip);
}

