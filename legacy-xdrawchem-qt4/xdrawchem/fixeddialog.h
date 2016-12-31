#ifndef FIXEDDIALOG_H
#define FIXEDDIALOG_H

#include <QDialog>
#include <QCheckBox>

class QComboBox;
class QDoubleSpinBox;

class FixedDialog : public QDialog
{
    Q_OBJECT

public:
    FixedDialog( QWidget *parent );
    double getLength_bond();
    double getAngle_bond();
    void setLength_bond( double );
    void setAngle_bond( double );
    double getLength_arrow();
    double getAngle_arrow();
    void setLength_arrow( double );
    void setAngle_arrow( double );
    double getDoubleBondOffset();
    void setDoubleBondOffset( double );
    void setAntiChecked( bool checked ) { anti_toggle->setChecked( checked ); }
    bool getAntiChecked() { return anti_toggle->isChecked(); }

public slots:
    void onSuccess();
    void setDefaults();
    void setUnits( int );

private:
    QComboBox *unitCombo;
    QDoubleSpinBox *bondLengthBox;
    QDoubleSpinBox *bondAngleBox;
    QDoubleSpinBox *arrowLengthBox;
    QDoubleSpinBox *arrowAngleBox;
    QDoubleSpinBox *doubleBondOffset;
    QDoubleSpinBox *gridSpacingBox;
    QCheckBox *anti_toggle, *snapGridBox;
    QComboBox *showgrid;
    int bl, al, dsp, gsp, prevunit;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
