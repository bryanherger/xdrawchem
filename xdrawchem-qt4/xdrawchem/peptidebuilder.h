#ifndef PEPTDIALOG_H
#define PEPTDIALOG_H

#include <QDialog>
#include <QLineEdit>

class PeptDialog : public QDialog
{
    Q_OBJECT

public:
    PeptDialog( QWidget *parent );
    QString getPeptide();

public slots:
    void buttonPress( int );

private:
    QLineEdit *peptide;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
