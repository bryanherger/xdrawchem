#ifndef PAINTABLE_H
#define PAINTABLE_H

#include <qpoint.h>
#include <qcolor.h>
#include <qpixmap.h>

class Paintable {
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
