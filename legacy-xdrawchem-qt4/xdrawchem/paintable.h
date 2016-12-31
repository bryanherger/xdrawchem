#ifndef PAINTABLE_H
#define PAINTABLE_H

#include <QPixmap>

class Paintable
{
public:
    int op;
    QPoint a;
    QPoint b;
    QPoint b1;
    QColor c;
    QFont f;
    QChar ch;
    QString st;
    QPixmap p;
    int s;
    int t;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
