#ifndef NETDIALOG_H
#define NETDIALOG_H

#include <qdialog.h>
#include <qcombobox.h>
#include <qcheckbox.h>
#include <qlineedit.h>

class NetDialog:public QDialog {
  Q_OBJECT
    public:
  NetDialog(QWidget *parent, QString name);
  QString getKey() { 
    QString ret;
    if (keylist->currentItem() == 0) ret = "name";
    if (keylist->currentItem() == 1) ret = "cas";
    if (keylist->currentItem() == 2) ret = "formula";
    return ret;
  }
  QString getValue() { return searchkey->text(); }
  QString getServer() { return serverkey->text(); }
  bool getExact() { return emcheck->isChecked(); } 
  public slots:
      void OK();
      
    private:
      QString fn;
      QComboBox *dblist, *keylist;
      QCheckBox *emcheck;
      QLineEdit *searchkey, *serverkey;
};

#endif
