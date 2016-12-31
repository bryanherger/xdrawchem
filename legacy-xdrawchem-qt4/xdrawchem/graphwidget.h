// graphwidget.h -- class definition for XDrawChem NMR/MS/IR graph widget.

#ifndef GRAPHWIDGET_H
#define GRAPHWIDGET_H

#include <QWidget>
#include <QPixmap>

class QPaintEvent;

#include "graphdata.h"

class GraphWidget : public QWidget
{
    Q_OBJECT
public:
    GraphWidget( QWidget *parent = 0 );
    void setDataType( int x ) { datatype = x; }
    void AddPeak( double, QColor, QString l1 = QString(), QString t1 = QString() );
    void AddPeak( double, int, QColor, QString l1 = QString(), QString t1 = QString() );
    void AddPixmap( QPixmap p ) { p1 = p; }
    void PrintSetup();

public slots:
    void Print();
    void Export();

protected:
    void paintEvent( QPaintEvent * );
    QString m_toclip;

private:
    int datatype;
    int output;
    QList<GraphData *> peaks;
    QPixmap p1;
    QPrinter *printer;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
