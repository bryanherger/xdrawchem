#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QCheckBox>

#include "render2d.h"
#include "fixeddialog.h"
#include "prefs.h"

extern Preferences preferences;

FixedDialog::FixedDialog( QWidget *parent )
    : QDialog( parent )
{
    setWindowTitle( tr( "Set fixed angle and length" ) );

    int decimals = 2;
    double prefValue, bondLength, arrowLength, offset, gridSpacing;
    QPushButton *ok, *cancel, *def1;

    QGridLayout *fixedlayout = new QGridLayout();
    QLabel *unitsLabel = new QLabel();

    unitsLabel->setText( tr( "Units:" ) );
    fixedlayout->addWidget( unitsLabel, 0, 0, 1, 1 );

    unitCombo = new QComboBox();
    unitCombo->addItem( tr( "pixels" ) );
    // not ready for these...yet
//    unitCombo->addItem( tr("inches") );
//    unitCombo->addItem( tr("cm") );
    prevunit = preferences.getUnits();
    if ( prevunit == PIXELS )
        decimals = 0;
    unitCombo->setCurrentIndex( prevunit );
    QString ustr = unitCombo->currentText();
    connect( unitCombo, SIGNAL( activated( int ) ), this, SLOT( setUnits( int ) ) );

    fixedlayout->addWidget( unitCombo, 0, 1, 1, 2 );

    QLabel *bondLengthLabel = new QLabel( tr( "Bond fixed length:" ) );
    fixedlayout->addWidget( bondLengthLabel, 1, 0 );

    bondLengthBox = new QDoubleSpinBox();
    bondLengthBox->setSuffix( " " + ustr );
    bondLengthBox->setMinimum( 5 );
    bondLengthBox->setMaximum( 1000 );
    bondLengthBox->setDecimals( decimals );
    prefValue = preferences.getBond_fixedlength();
    if ( prevunit == PIXELS )
        bondLength = prefValue;
    else if ( prevunit == ENGLISH )
        bondLength = prefValue / 100.0;
    else
        bondLength = prefValue / 40.0;
    bondLengthBox->setValue( bondLength );
    fixedlayout->addWidget( bondLengthBox, 1, 1, 1, 2 );

    QLabel *bondAngleLabel = new QLabel( tr( "Bond fixed angle:" ) );
    fixedlayout->addWidget( bondAngleLabel, 2, 0 );

    bondAngleBox = new QDoubleSpinBox();
    bondAngleBox->setSuffix( " " + tr( "degrees" ) );
    bondLengthBox->setMaximum( 359.99 );
    bondAngleBox->setValue( preferences.getBond_fixedangle() );
    fixedlayout->addWidget( bondAngleBox, 2, 1, 1, 2 );

    QLabel *arrowLengthLabel = new QLabel( tr( "Arrow fixed length:" ) );
    fixedlayout->addWidget( arrowLengthLabel, 3, 0 );

    arrowLengthBox = new QDoubleSpinBox();
    arrowLengthBox->setSuffix( " " + ustr );
    arrowLengthBox->setMinimum( prevunit == PIXELS ? 1 : 0.01 );
    bondLengthBox->setMaximum( 1000 );
    arrowLengthBox->setDecimals( decimals );
    prefValue = preferences.getArrow_fixedlength();
    if ( prevunit == PIXELS )
        arrowLength = prefValue;
    else if ( prevunit == ENGLISH )
        arrowLength = prefValue / 100.0;
    else
        arrowLength = prefValue / 40.0;
    arrowLengthBox->setValue( arrowLength );
    fixedlayout->addWidget( arrowLengthBox, 3, 1, 1, 2 );

    QLabel *arrowAngleLabel = new QLabel( tr( "Arrow fixed angle:" ) );
    fixedlayout->addWidget( arrowAngleLabel, 4, 0 );

    arrowAngleBox = new QDoubleSpinBox();
    arrowAngleBox->setSuffix( " " + tr( "degrees" ) );
    bondLengthBox->setMaximum( 359.99 );
    arrowAngleBox->setValue( preferences.getArrow_fixedangle() );
    fixedlayout->addWidget( arrowAngleBox, 4, 1, 1, 2 );

    QLabel *bondSpacingLabel = new QLabel( tr( "Double bond spacing:" ) );
    fixedlayout->addWidget( bondSpacingLabel, 5, 0 );

    doubleBondOffset = new QDoubleSpinBox();
    doubleBondOffset->setSuffix( " " + ustr );
    doubleBondOffset->setMinimum( prevunit == PIXELS ? 1 : 0.01 );
    doubleBondOffset->setDecimals( decimals );
    prefValue = preferences.getDoubleBondOffset();
    if ( prevunit == PIXELS )
        offset = prefValue;
    else if ( prevunit == ENGLISH )
        offset = prefValue / 100.0;
    else
        offset = prefValue / 40.0;
    doubleBondOffset->setValue( offset );
    fixedlayout->addWidget( doubleBondOffset, 5, 1, 1, 2 );

    //anti_toggle = new QCheckBox(tr("Anti-aliasing"), this);
    //anti_toggle->setGeometry(20,170,100,30);

    showgrid = new QComboBox();
    showgrid->addItem( tr( "Do not show grid" ) );
    showgrid->addItem( tr( "Show square grid" ) );
    showgrid->addItem( tr( "Show hex grid" ) );
    showgrid->setCurrentIndex( preferences.getDrawGrid() );
    fixedlayout->addWidget( showgrid, 6, 1, 1, 2 );

    snapGridBox = new QCheckBox( tr( "Snap to grid" ) );
    bool snapgrid = false;

    if ( preferences.getSnapGrid() == GRID_SQUARE )
        snapgrid = true;
    snapGridBox->setChecked( snapgrid );
    fixedlayout->addWidget( snapGridBox, 7, 1, 1, 2 );

    QLabel *gridSpacingLabel = new QLabel( tr( "Grid spacing:" ) );
    fixedlayout->addWidget( gridSpacingLabel, 8, 0 );

    gridSpacingBox = new QDoubleSpinBox();
    gridSpacingBox->setSuffix( " " + ustr );
    gridSpacingBox->setMinimum( prevunit == PIXELS ? 1 : 0.01 );
    gridSpacingBox->setDecimals( decimals );
    prefValue = preferences.getGridSpacing();
    if ( prevunit == PIXELS )
        gridSpacing = prefValue;
    else if ( prevunit == ENGLISH )
        gridSpacing = prefValue / 100.0;
    else
        gridSpacing = prefValue / 40.0;
    gridSpacingBox->setValue( gridSpacing );
    fixedlayout->addWidget( gridSpacingBox, 8, 1, 1, 2 );

    QHBoxLayout *buttonHBox = new QHBoxLayout();
    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );

    buttonHBox->addItem( spacer );

    def1 = new QPushButton( tr( "Default" ) );
    connect( def1, SIGNAL( clicked() ), SLOT( setDefaults() ) );
    buttonHBox->addWidget( def1 );

    ok = new QPushButton( tr( "OK" ) );
    connect( ok, SIGNAL( clicked() ), SLOT( onSuccess() ) );
    buttonHBox->addWidget( ok );

    cancel = new QPushButton( tr( "Cancel" ) );
    connect( cancel, SIGNAL( clicked() ), SLOT( reject() ) );
    buttonHBox->addWidget( cancel );

    fixedlayout->addLayout( buttonHBox, 9, 0, 1, 3 );
    setLayout( fixedlayout );
}

void FixedDialog::onSuccess()
{
    double d1;

    preferences.setDrawGrid( showgrid->currentIndex() );

    if ( snapGridBox->isChecked() ) {
        preferences.setSnapGrid( GRID_SQUARE );
    } else {
        preferences.setSnapGrid( GRID_NONE );
    }
    preferences.setUnits( unitCombo->currentIndex() );

    d1 = bondLengthBox->value();
    if ( prevunit == PIXELS )
        bl = ( int ) d1;
    if ( prevunit == ENGLISH )
        bl = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        bl = qRound( d1 * 40.0 );
    d1 = arrowLengthBox->value();
    if ( prevunit == PIXELS )
        al = ( int ) d1;
    if ( prevunit == ENGLISH )
        al = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        al = qRound( d1 * 40.0 );
    d1 = doubleBondOffset->value();
    if ( prevunit == PIXELS )
        dsp = ( int ) d1;
    if ( prevunit == ENGLISH )
        dsp = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        dsp = qRound( d1 * 40.0 );
    d1 = gridSpacingBox->value();
    if ( prevunit == PIXELS )
        gsp = ( int ) d1;
    if ( prevunit == ENGLISH )
        gsp = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        gsp = qRound( d1 * 40.0 );

    d1 = arrowAngleBox->value();
    if ( d1 > 0.0 )
        preferences.setArrow_fixedangle( d1 );
    if ( al > 0.0 )
        preferences.setArrow_fixedlength( al );
    d1 = bondAngleBox->value();
    if ( d1 > 0.0 )
        preferences.setBond_fixedangle( d1 );
    if ( bl > 0.0 )
        preferences.setBond_fixedlength( bl );
    if ( dsp > 0.0 )
        preferences.setDoubleBondOffset( dsp );
    if ( gsp > 0.0 )
        preferences.setGridSpacing( qRound( gsp ) );

    accept();
}

void FixedDialog::setDefaults()
{
    QString n;
    int i1;
    double d1;

    i1 = 25;
    if ( prevunit == PIXELS )
        d1 = ( double ) i1;
    if ( prevunit == ENGLISH )
        d1 = ( double ) i1 / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) i1 / 40.0;
    bondLengthBox->setValue( d1 );
    bondAngleBox->setValue( 15 );
    i1 = 50;
    if ( prevunit == PIXELS )
        d1 = ( double ) i1;
    if ( prevunit == ENGLISH )
        d1 = ( double ) i1 / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) i1 / 40.0;
    arrowLengthBox->setValue( d1 );
    arrowAngleBox->setValue( 15 );
    i1 = 1;
    if ( prevunit == PIXELS )
        d1 = ( double ) i1;
    if ( prevunit == ENGLISH )
        d1 = ( double ) i1 / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) i1 / 40.0;
    doubleBondOffset->setValue( d1 );
    showgrid->setCurrentIndex( GRID_NONE );
    snapGridBox->setChecked( false );
    i1 = 50;
    if ( prevunit == PIXELS )
        d1 = ( double ) i1;
    if ( prevunit == ENGLISH )
        d1 = ( double ) i1 / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) i1 / 40.0;
    gridSpacingBox->setValue( d1 );
    //anti_toggle->setChecked(false);
}

void FixedDialog::setUnits( int n1 )
{
    double d1;
    QString n;

    d1 = bondLengthBox->value();
    if ( prevunit == PIXELS )
        bl = ( int ) d1;
    if ( prevunit == ENGLISH )
        bl = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        bl = qRound( d1 * 40.0 );
    d1 = arrowLengthBox->value();
    if ( prevunit == PIXELS )
        al = ( int ) d1;
    if ( prevunit == ENGLISH )
        al = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        al = qRound( d1 * 40.0 );
    d1 = doubleBondOffset->value();
    if ( prevunit == PIXELS )
        dsp = ( int ) d1;
    if ( prevunit == ENGLISH )
        dsp = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        dsp = qRound( d1 * 40.0 );
    d1 = gridSpacingBox->value();
    if ( prevunit == PIXELS )
        gsp = ( int ) d1;
    if ( prevunit == ENGLISH )
        gsp = qRound( d1 * 100.0 );
    if ( prevunit == METRIC )
        gsp = qRound( d1 * 40.0 );
    QString ustr;

    if ( n1 == 0 ) {
        ustr = tr( "pixels" );
    }
    if ( n1 == 1 ) {
        ustr = tr( "inches" );
    }
    if ( n1 == 2 ) {
        ustr = tr( "cm" );
    }
    bondLengthBox->setSuffix( " " + ustr );
    arrowLengthBox->setSuffix( " " + ustr );
    doubleBondOffset->setSuffix( " " + ustr );
    gridSpacingBox->setSuffix( " " + ustr );
    prevunit = n1;
    if ( prevunit == PIXELS )
        d1 = ( double ) bl;
    if ( prevunit == ENGLISH )
        d1 = ( double ) bl / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) bl / 40.0;
    bondLengthBox->setValue( d1 );
    if ( prevunit == PIXELS )
        d1 = ( double ) al;
    if ( prevunit == ENGLISH )
        d1 = ( double ) al / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) al / 40.0;
    arrowLengthBox->setValue( d1 );
    if ( prevunit == PIXELS )
        d1 = ( double ) dsp;
    if ( prevunit == ENGLISH )
        d1 = ( double ) dsp / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) dsp / 40.0;
    doubleBondOffset->setValue( d1 );
    if ( prevunit == PIXELS )
        d1 = ( double ) gsp;
    if ( prevunit == ENGLISH )
        d1 = ( double ) gsp / 100.0;
    if ( prevunit == METRIC )
        d1 = ( double ) gsp / 40.0;
    gridSpacingBox->setValue( d1 );
}

void FixedDialog::setLength_bond( double t )
{
    bondLengthBox->setValue( t );
}

void FixedDialog::setAngle_bond( double t )
{
    bondAngleBox->setValue( t );
}

double FixedDialog::getLength_bond()
{
    double t = bondLengthBox->value();

    return t;
}

double FixedDialog::getAngle_bond()
{
    double t = bondAngleBox->value();

    return t;
}

void FixedDialog::setLength_arrow( double t )
{
    arrowLengthBox->setValue( t );
}

void FixedDialog::setAngle_arrow( double t )
{
    arrowAngleBox->setValue( t );
}

double FixedDialog::getLength_arrow()
{
    double t = arrowLengthBox->value();

    return t;
}

double FixedDialog::getAngle_arrow()
{
    double t = arrowAngleBox->value();

    return t;
}

double FixedDialog::getDoubleBondOffset()
{
    double t = doubleBondOffset->value();

    return t;
}

void FixedDialog::setDoubleBondOffset( double t )
{
    doubleBondOffset->setValue( t );
}

//cmake#include "fixeddialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
