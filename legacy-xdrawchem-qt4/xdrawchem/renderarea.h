#ifndef RENDERAREA_H
#define RENDERAREA_H

#include <QScrollArea>


class RenderArea : public QScrollArea
{
    Q_OBJECT

public:
    RenderArea( QWidget *parent = 0 );
    void scrollContentsBy ( int dx, int dy );

signals:
    void scrolled( int x, int y );
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
