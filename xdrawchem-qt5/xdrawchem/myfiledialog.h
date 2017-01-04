#ifndef MYFILEDIALOG_H
#define MYFILEDIALOG_H

#include <QFileDialog>
#include <QCheckBox>

class MyFileDialog : public QFileDialog
{
    Q_OBJECT
public:
    MyFileDialog( QWidget *parent );
    bool isTransparent();

private:
    QCheckBox *trans_toggle;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
