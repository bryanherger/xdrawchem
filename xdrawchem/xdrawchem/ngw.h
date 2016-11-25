// ngw.h -- class definition for 'new' XDrawChem NMR/MS/IR graph widget.

#ifndef NEWGRAPHWIDGET_H
#define NEWGRAPHWIDGET_H

#include <qwidget.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include <qpainter.h>
#include <qprinter.h>

#include "graphdata.h"
#include "peak.h"

#define DATA_1HNMR 1
#define DATA_13CNMR 2
#define DATA_IR 3

class NewGraphWidget : public QWidget {
  Q_OBJECT
    public:
  NewGraphWidget(QWidget *parent = 0, const char *name = 0);
  void setDataType(int x) { datatype = x; }
  void AddPeak(double, QColor, QString l1 = "", QString t1 = "");
  void AddPeak(double, int, QColor, QString l1 = "", QString t1 = "");
  void AddPeak(Peak *, QColor);
  void AddPixmap(QPixmap p) { p1 = p; }
  void PrintSetup();

  public slots:
    void Print();

 protected:
  void paintEvent(QPaintEvent *);

 private:
  int datatype;
  int output;
  QPtrList<GraphData> peaks;
  GraphData *tmp_data;
  QPixmap p1;
  QPrinter *printer;
};

#endif
