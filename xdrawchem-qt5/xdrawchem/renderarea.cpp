#include <QScrollBar>

#include "renderarea.h"

RenderArea::RenderArea(QWidget *parent)
    : QScrollArea( parent )
{
}

void RenderArea::scrollContentsBy( int dx, int dy )
{
    QScrollArea::scrollContentsBy( dx, dy );
    emit scrolled( horizontalScrollBar()->value(), verticalScrollBar()->value() );
}

//cmake#include "renderarea.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
