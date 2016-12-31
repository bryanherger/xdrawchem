#ifndef RINGDIALOG_H
#define RINGDIALOG_H

#include <QDialog>

class QComboBox;

class RingDialog : public QDialog
{
    Q_OBJECT

public:
    RingDialog( QWidget *parent );
    QString getFile() { return fn; }

public slots:
    void fromComboBox( int );

private:
    QString fn;
    QComboBox *filelist;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
