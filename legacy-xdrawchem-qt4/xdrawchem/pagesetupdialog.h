#ifndef PSDIALOG_H
#define PSDIALOG_H

#include <QDialog>

class QComboBox;
class QDoubleSpinBox;
class QLabel;

class PageSetupDialog : public QDialog
{
    Q_OBJECT
public:
    PageSetupDialog(QWidget *parent);
    void setPageSize(int);
    int getPageSize();
    void setOrientation(int);
    int getOrientation();

public slots:
    void onSuccessful();
    void SwapWH(int);

private:
    QComboBox *paperSize, *paperOrient;
    QDoubleSpinBox *paperWidth, *paperHeight;
    QLabel *unitWidthLabel, *unitHeightLabel;
    int papersize, orient, ps_set, po_set;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
