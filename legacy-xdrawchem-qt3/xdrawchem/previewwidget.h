#ifndef PREVIEWWIDGET_H
#define PREVIEWWIDGET_H

#include <qwidget.h>
#include <qpixmap.h>
#include <qpainter.h>

class PreviewWidget : public QWidget {
  Q_OBJECT
    public:
  PreviewWidget(QWidget *parent = 0, const char *name = 0);
  void Update(int a1, int a2, int a3, int a4, int a5) {
    type = a1;
    th = a2;
    da = a3;
    _or = a4;
    style = a5;
    repaint();
  }
  int type, th, da, _or, style;

  public slots:

 protected:
  void paintEvent(QPaintEvent *);
};

#endif
