#include "xruler.h"
#include "defs.h"
#include "prefs.h"

#include <qpixmap.h>

extern Preferences preferences;

XRuler::XRuler(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  setPaletteBackgroundColor( QColor(255,255,255) );
  setPaletteForegroundColor( QColor(0,0,0) );
  ix = 0; iy = 1024; zf = 1.0;
}

void XRuler::setHV(int n) {
  ihv = n;
}

void XRuler::setRange(int nl, int nh) {
  ix = nl; iy = nh;
}

void XRuler::paintEvent(QPaintEvent *) {
  // get current units and spacing from Preferences
  iu = preferences.getUnits();
  incr = preferences.getGridSpacing();
  int c1, c2, wf;

  QPainter p;
  p.begin(this);
  p.setPen( QColor(0,0,0) );

  QPixmap numbers;
  numbers.resize(30,10);
  //QFont fnum("Courier", 10);
  QFont fnum = preferences.getRulerFont();
  QFontMetrics fmet(fnum);
  QSize numsize;
  QPainter p1;
  p1.begin(&numbers);
  p1.setPen( QColor(0,0,0) );
  p1.setFont( fnum );
  QString n1;
  QWMatrix wm;
  wm = wm.rotate(90.0);

  if (ihv == 1) { // horizontal ruler
    for (c1 = ix; c1 < iy; c1++) {
      if ( (c1 % incr) == 0 ) {
	c2 = my_round( (double)c1 * zf );
	p.drawLine(c2 - ix + 2, 10, c2 - ix + 2, 20);
	if (iu == PIXELS) n1.setNum(c1);
	if (iu == ENGLISH) n1.setNum( (double)c1 / 100.0 );
	if (iu == METRIC) n1.setNum( (double)c1 / 40.0 );
	if (c1 == ix) {
	  if (iu == PIXELS) n1.append( tr("px") );
	  if (iu == ENGLISH) n1.append( tr("in") );
	  if (iu == METRIC) n1.append( tr("cm") );
	}
	numbers.fill();
	numsize = fmet.size(0, n1);
	wf = (30 - numsize.width()) / 2;
	p1.drawText(wf, 10, n1);
	if (c1 > ix)
	  p.drawPixmap(c2 - ix - 15, 0, numbers);
	else
	  p.drawPixmap(c2 - ix, 0, numbers);
      }
    }
  }
  if (ihv == 2) { // vertical ruler
    for (c1 = ix; c1 < iy; c1++) {
      if ( (c1 % incr) == 0 ) {
	c2 = my_round( (double)c1 * zf );
	p.drawLine(10, c2 - ix + 2, 20, c2 - ix + 2);
	if (iu == PIXELS) n1.setNum(c1);
	if (iu == ENGLISH) n1.setNum( (double)c1 / 100.0 );
	if (iu == METRIC) n1.setNum( (double)c1 / 40.0 );
	if (c1 == ix) {
	  if (iu == PIXELS) n1.append( tr("px") );
	  if (iu == ENGLISH) n1.append( tr("in") );
	  if (iu == METRIC) n1.append( tr("cm") );
	}
	numbers.fill();
	numsize = fmet.size(0, n1);
	wf = (30 - numsize.width()) / 2;
	p1.drawText(wf, 10, n1);
	if (c1 > ix)
	  p.drawPixmap(0, c2 - ix - 15, numbers.xForm(wm));
	else
	  p.drawPixmap(0, c2 - ix, numbers.xForm(wm));
      }
    }
  }
}

void XRuler::zoomEvent() {
  zf = (double)preferences.getZoom() / 100.0;
  repaint();
}
