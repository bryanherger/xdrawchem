#ifndef DYKDIALOG_H
#define DYKDIALOG_H

#include <QDialog>
#include <QCheckBox>
#include <QLabel>

#include "prefs.h"

extern Preferences preferences;

class DYKDialog : public QDialog
{
    Q_OBJECT

public:
    DYKDialog( QWidget *parent = 0 );

public slots:
    void nextTip();
    void prevTip();
    void finished();

private:
    void populateTipList();

    QCheckBox *futureshow;
    QLabel *curtip;
    QStringList tips;
    int tipcount;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
