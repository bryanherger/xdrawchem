#ifndef XRULER_H
#define XRULER_H

#include "prefs.h"

class QPaintEvent;

extern Preferences preferences;

class XRuler : public QWidget
{
  Q_OBJECT

public:
    XRuler( QWidget *parent = 0 );

public slots:
    void setHV( int );
    void setRange( int, int );
    void zoomEvent();

protected:
    void paintEvent( QPaintEvent * );

private:
    int ix, iy, iu, ihv, incr;
    double dx, dy, zf;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
