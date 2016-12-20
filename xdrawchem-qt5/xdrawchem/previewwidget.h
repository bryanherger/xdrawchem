#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <QWidget>

class QPaintEvent;

class PreviewWidget : public QWidget
{
public:
    PreviewWidget( QWidget *parent = 0 );
    void updateWidget( int a1, int a2, int a3, int a4, int a5, QColor c = Qt::black )
    {
        type = a1;
        th = a2;
        da = a3;
        _or = a4;
        style = a5;
        color = c;
        update();
    }
    int type, th, da, _or, style;
    QColor color;

protected:
    void paintEvent(QPaintEvent *);
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
