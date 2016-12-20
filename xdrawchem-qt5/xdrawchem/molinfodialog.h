#ifndef MIDIALOG_H
#define MIDIALOG_H

#include <QDebug>
#include <QDialog>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>

class MolInfoDialog : public QDialog
{
    Q_OBJECT

public:
    MolInfoDialog( QWidget *parent );
    bool isMWChecked() { return molWeightCheckBox->isChecked(); }
    bool isEFChecked() { return ef_check->isChecked(); }
    bool isEAChecked() { return ea_check->isChecked(); }
    void setMW(QString s1) { molWeightCheckBox->setText(s1); }

    void setEF(QString s1)
    {
        ef = tr("Formula = ");
        ef += s1;
        ef_check->setText(ef);
    }

    void setEA(QString s1)
    {
        ea = s1;
        eaLabel->setText(ea);
    }

    void setCAS(QString s1)
    {
        if (s1 != "unknown") {
        casLabel->setText(tr("CAS:") + s1);
        }
    }

    void setPCC(QString s1)
    {
        if (s1 != "unknown") {
        pcc = s1;
        getpcc->setEnabled(true);
        pcLabel->setText(tr("PubChem compound:") + s1);
        }
    }

    void setName(QString s1)
    {
        qDebug() << s1;
        if (s1 != "unknown") {
        nameLabel->setText(tr("Name:") + s1);
        }
    }

    void setSynonyms(QString s1)
    {
        qDebug() << s1;
        if (s1 != "unknown") {
        synLabel->setText(tr("Other names:") + s1);
        }
    }

public slots:
    void SendHelp();
    void doAccept();
    void doPubChem();

signals:
    void MIDClose();

private:
    QCheckBox *molWeightCheckBox, *ef_check, *dg_check, *ea_check;
    QLabel *nameLabel, *casLabel, *synLabel, *pcLabel, *eaLabel;
    QString molWeight, ea, ef, dg, casno, pcc;
    QPushButton *getpcc;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
