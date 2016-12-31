#ifndef MIDIALOG_H
#define MIDIALOG_H

#include <iostream>
#include <qdialog.h>
#include <qcombobox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpushbutton.h>

class MolInfoDialog:public QDialog {
  Q_OBJECT
public:
  MolInfoDialog(QWidget *parent, QString name);
  bool isMWChecked() { return mw_check->isChecked(); }
  bool isEFChecked() { return ef_check->isChecked(); }
  bool isEAChecked() { return ea_check->isChecked(); }
  void setMW(QString s1) { 
    mw_check->setText(s1); 
  }
  void setEF(QString s1) { 
    ef = tr("Formula = ");
    ef += s1;
    ef_check->setText(ef); 
  }
  void setEA(QString s1) { 
    ea = s1;
    ea_check->setText(ea); 
  }
  void setCAS(QString s1) { 
    if (s1 != "unknown") {
      cas_label->setText(tr("CAS:") + s1);
    } 
  }
  void setPCC(QString s1) { 
    if (s1 != "unknown") {
      pcc = s1;
      getpcc->setEnabled(true);
      pc_label->setText(tr("PubChem compound:") + s1);
    } 
  }
  void setName(QString s1) { 
    std::cout << s1 << std:: endl;
    if (s1 != "unknown") {
      name_label->setText(tr("Name:") + s1);
    } 
  }
  void setSynonyms(QString s1) { 
    std::cout << s1 << std:: endl;
    if (s1 != "unknown") {
      syn_label->setText(tr("Other names:") + s1);
    } 
  }

  public slots:
    void SendHelp();
  void doAccept();
  void doPubChem();

 signals:
  void MIDClose();

 private:
  QCheckBox *mw_check, *ef_check, *dg_check, *ea_check;
  QLabel *name_label, *cas_label, *syn_label, *pc_label;
  QString mw, ea, ef, dg, casno, pcc;
  QPushButton *getpcc;
};

#endif
