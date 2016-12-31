#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <qdialog.h>
#include <qptrlist.h>
#include <qpixmap.h>
#include "graphdata.h"
#include "graphwidget.h"

class GraphDialog:public QDialog {
  Q_OBJECT
    public:
  GraphDialog(QWidget *parent, QString name);
  void AddPeak(double, QString l1 = "", QString t1 = "");
  void AddPeak(double, QColor, QString l1 = "", QString t1 = "");
  void AddPeak(double, int, QColor, QString l1 = "", QString t1 = "");
  void AddPixmap(QPixmap);
  public slots:
    void SendHelp();
 private:
  GraphWidget *g;
};

#endif
