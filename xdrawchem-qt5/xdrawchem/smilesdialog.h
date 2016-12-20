#ifndef SMILESDIALOG_H
#define SMILESDIALOG_H

#include <QDialog>
#include <QLineEdit>

class SmilesDialog : public QDialog
{
public:
    SmilesDialog( QWidget *parent );
    QString getSMILES();

private:
    QLineEdit *smilesInput;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
