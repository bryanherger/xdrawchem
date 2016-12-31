// graphwidget.h -- class definition for XDrawChem NMR/MS/IR graph widget.

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <qwidget.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qprinter.h>

#include "graphdata.h"

class GraphWidget : public QWidget {
  Q_OBJECT
    public:
  GraphWidget(QWidget *parent = 0, const char *name = 0);
  void setDataType(int x) { datatype = x; }
  void AddPeak(double, QColor, QString l1 = "", QString t1 = "");
  void AddPeak(double, int, QColor, QString l1 = "", QString t1 = "");
  void AddPixmap(QPixmap p) { p1 = p; }
  void PrintSetup();

  public slots:
    void Print();
	void Export();

 protected:
  void paintEvent(QPaintEvent *);
  QString m_toclip;

 private:
  int datatype;
  int output;
  QPtrList<GraphData> peaks;
  QPixmap p1;
  QPrinter *printer;
};

#endif
