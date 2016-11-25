#ifndef GRAPHDIALOG_H
#define GRAPHDIALOG_H

#include <QDialog>
#include <QString>
#include <QColor>

#include "graphdata.h"
#include "graphwidget.h"

class GraphDialog : public QDialog
{
    Q_OBJECT

public:
    GraphDialog( QWidget *parent, const QString &name );
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

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
