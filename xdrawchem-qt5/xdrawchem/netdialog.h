#ifndef NETDIALOG_H
#define NETDIALOG_H

#include <QDialog>
#include <QComboBox>

class QLineEdit;

class NetDialog : public QDialog
{
public:
    NetDialog( QWidget *parent );

    QString getKey()
    {
        QString ret;
        if ( keylist->currentIndex() == 0 ) ret = "name";
        else if ( keylist->currentIndex() == 1 ) ret = "cas";
        else if ( keylist->currentIndex() == 2 ) ret = "formula";
        return ret;
    }

    QString getValue() { return searchkey->text(); }
    QString getServer() { return serverkey->text(); }
    bool getExact() { return emcheck->isChecked(); }

private:
    QString fn;
    QComboBox *dblist, *keylist;
    QCheckBox *emcheck;
    QLineEdit *searchkey, *serverkey;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
