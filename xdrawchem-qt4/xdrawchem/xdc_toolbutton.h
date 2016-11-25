#ifndef XDC_TB_H
#define XDC_TB_H

#include <qtoolbutton.h>
#include <qstring.h>

class XDC_ToolButton : public QToolButton {
  Q_OBJECT

    public:
  XDC_ToolButton(QWidget *parent, QString name);

  public slots:
    void trigger();

 signals:
  void IncludeFile(QString);

 private:
  QString fn;
};

#endif
