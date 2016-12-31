// ngw.h -- class definition for 'new' XDrawChem NMR/MS/IR graph widget.

#ifndef NEWGRAPHWIDGET_H
#define NEWGRAPHWIDGET_H

#include <QWidget>
#include <QPixmap>

#include "graphdata.h"
#include "peak.h"

#define DATA_1HNMR 1
#define DATA_13CNMR 2
#define DATA_IR 3

class QPaintEvent;
class QPrinter;

class NewGraphWidget : public QWidget
{
    Q_OBJECT

public:
    NewGraphWidget( QWidget *parent = 0 );
    void setDataType(int x) { datatype = x; }
    void AddPeak( double, QColor, QString l1 = QString(), QString t1 = QString() );
    void AddPeak( double, int, QColor, QString l1 = QString(), QString t1 = QString() );
    void AddPeak( Peak *, QColor );
    void AddPixmap( QPixmap p ) { p1 = p; }
    void PrintSetup();

public slots:
    void Print();

protected:
    void paintEvent( QPaintEvent * );

private:
    int datatype;
    int output;
    QList<GraphData *> peaks;
    GraphData *tmp_data;
    QPixmap p1;
    QPrinter *printer;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
