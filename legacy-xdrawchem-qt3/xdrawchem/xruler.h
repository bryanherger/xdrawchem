#ifndef XRULER_H
#define XRULER_H

#include <qwidget.h>
#include <qrect.h>
#include <qfont.h>
#include <qpainter.h>
#include "prefs.h"

extern Preferences preferences;

class XRuler : public QWidget {
  Q_OBJECT
    public:
  XRuler(QWidget *parent = 0, const char *name = 0);

  public slots:
    void setHV(int);
  void setRange(int, int);
  void zoomEvent();

 protected:
  void paintEvent(QPaintEvent *);

 private:
  int ix, iy, iu, ihv, incr;
    double dx, dy, zf;
};

#endif
