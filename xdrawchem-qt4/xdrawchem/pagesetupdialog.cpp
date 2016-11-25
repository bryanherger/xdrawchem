#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QComboBox>
#include <QDoubleSpinBox>

#include "pagesetupdialog.h"
#include "defs.h"
#include "prefs.h"

extern Preferences preferences;

PageSetupDialog::PageSetupDialog( QWidget * parent )
:  QDialog( parent )
{
    setWindowTitle( tr( "Page setup" ) );

    QGridLayout *mygrid = new QGridLayout();

    QLabel *pageSizeLabel = new QLabel();

    pageSizeLabel->setText( tr( "Paper size:" ) );
    mygrid->addWidget( pageSizeLabel, 0, 0, 1, 2 );

    paperSize = new QComboBox();
    paperSize->addItem( tr( "Letter (8.5\" x 11\")" ) );
    paperSize->addItem( tr( "Legal (8.5\" x 14\")" ) );
    paperSize->addItem( tr( "A4 (210 mm x 297 mm)" ) );
    paperSize->addItem( tr( "640x480 pixels" ) );
    paperSize->addItem( tr( "800x600 pixels" ) );
    paperSize->addItem( tr( "1024x768 pixels" ) );
    paperSize->addItem( tr( "Custom paper size" ) );
    if ( preferences.getPageSize() == PAGE_LETTER )
        paperSize->setCurrentIndex( 0 );
    if ( preferences.getPageSize() == PAGE_LEGAL )
        paperSize->setCurrentIndex( 1 );
    if ( preferences.getPageSize() == PAGE_A4 )
        paperSize->setCurrentIndex( 2 );
    if ( preferences.getPageSize() == PAGE_640 )
        paperSize->setCurrentIndex( 3 );
    if ( preferences.getPageSize() == PAGE_800 )
        paperSize->setCurrentIndex( 4 );
    if ( preferences.getPageSize() == PAGE_1024 )
        paperSize->setCurrentIndex( 5 );
    if ( preferences.getPageSize() == PAGE_CUSTOM )
        paperSize->setCurrentIndex( 6 );
    ps_set = paperSize->currentIndex();
    mygrid->addWidget( paperSize, 0, 2, 1, 2 );

    QLabel *paperOrientLabel = new QLabel( tr( "Orientation:" ) );
    mygrid->addWidget( paperOrientLabel, 1, 0, 1, 2 );

    paperOrient = new QComboBox();
    paperOrient->addItem( tr( "Portrait" ) );
    paperOrient->addItem( tr( "Landscape" ) );
    if ( preferences.getPageOrientation() == PAGE_PORTRAIT )
        paperOrient->setCurrentIndex( 0 );
    if ( preferences.getPageOrientation() == PAGE_LANDSCAPE )
        paperOrient->setCurrentIndex( 1 );
    po_set = paperOrient->currentIndex();
    connect( paperOrient, SIGNAL( activated( int ) ), SLOT( SwapWH( int ) ) );

    mygrid->addWidget( paperOrient, 1, 2, 1, 2 );

    int pw = preferences.getPageWidth();
    int ph = preferences.getPageHeight();
    int pu = preferences.getUnits();
    int decimals = 2;
    double pwf, phf;
    QString unitstr, n1;

    unitstr = "undef";
    if ( pu == PIXELS ) {
        unitstr = tr( "pixels" );
        decimals = 0;
        pwf = ( double ) pw;
        phf = ( double ) ph;
    }
    if ( pu == ENGLISH ) {
        unitstr = tr( "inches" );
        pwf = ( double ) pw / 100.0;
        phf = ( double ) ph / 100.0;
    }
    if ( pu == METRIC ) {
        unitstr = tr( "cm" );
        pwf = ( double ) pw / 40.0;
        phf = ( double ) ph / 40.0;
    }

    QLabel *paperWidthLabel = new QLabel( tr( "Page width:" ) );
    mygrid->addWidget( paperWidthLabel, 2, 0, 1, 2 );

    paperWidth = new QDoubleSpinBox();
    paperWidth->setMaximum( 5000 );
    paperWidth->setSuffix( " " + unitstr );
    paperWidth->setDecimals( decimals );
    paperWidth->setValue( pwf );
    mygrid->addWidget( paperWidth, 2, 2, 1, 2 );


    QLabel *paperHeightLabel = new QLabel( tr( "Page height:" ) );
    mygrid->addWidget( paperHeightLabel, 3, 0, 1, 2 );

    paperHeight = new QDoubleSpinBox();
    paperHeight->setMaximum( 5000 );
    paperHeight->setSuffix( " " + unitstr );
    paperHeight->setDecimals( decimals );
    paperHeight->setValue( phf );
    mygrid->addWidget( paperHeight, 3, 2, 1, 2 );

    QHBoxLayout *buttonHBox = new QHBoxLayout();

    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
    buttonHBox->addItem( spacer );

    QPushButton *ok, *dismiss;

    ok = new QPushButton( tr( "OK" ) );
    connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
    buttonHBox->addWidget( ok );

    dismiss = new QPushButton( tr( "Cancel" ) );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
    buttonHBox->addWidget( dismiss );

    mygrid->addLayout( buttonHBox, 4, 0, 1, 4 );
    setLayout( mygrid );
}

void PageSetupDialog::onSuccessful()
{
    // save settings to Preferences class
    double pw = paperWidth->value();
    double ph = paperHeight->value();
    int pu = preferences.getUnits();
    int pwf, phf;

    if ( pu == PIXELS ) {
        pwf = ( int ) pw;
        phf = ( int ) ph;
    }
    if ( pu == ENGLISH ) {
        pwf = ( int ) ( pw * 100.0 );
        phf = ( int ) ( ph * 100.0 );
    }
    if ( pu == METRIC ) {
        pwf = ( int ) ( pw * 40.0 );
        phf = ( int ) ( ph * 40.0 );
    }
    preferences.setPageWidth( pwf );
    preferences.setPageHeight( phf );

    if ( paperSize->currentIndex() == 0 )
        preferences.setPageSize( PAGE_LETTER );
    if ( paperSize->currentIndex() == 1 )
        preferences.setPageSize( PAGE_LEGAL );
    if ( paperSize->currentIndex() == 2 )
        preferences.setPageSize( PAGE_A4 );
    if ( paperSize->currentIndex() == 3 )
        preferences.setPageSize( PAGE_640 );
    if ( paperSize->currentIndex() == 4 )
        preferences.setPageSize( PAGE_800 );
    if ( paperSize->currentIndex() == 5 )
        preferences.setPageSize( PAGE_1024 );
    if ( paperSize->currentIndex() == 6 )
        preferences.setPageSize( PAGE_CUSTOM );

    if ( paperOrient->currentIndex() == 0 )
        preferences.setPageOrientation( PAGE_PORTRAIT );
    if ( paperOrient->currentIndex() == 1 )
        preferences.setPageOrientation( PAGE_LANDSCAPE );

    accept();                 // exit gracefully
}

void PageSetupDialog::SwapWH( int nx )
{
    if ( po_set == paperOrient->currentIndex() )
        return;
    po_set = paperOrient->currentIndex();
    double n1;

    n1 = paperWidth->value();
    paperWidth->setValue( paperHeight->value() );
    paperHeight->setValue( n1 );
}

void PageSetupDialog::setPageSize( int n )
{
    if ( n == PAGE_LETTER )
        paperSize->setCurrentIndex( 0 );
    if ( n == PAGE_LEGAL )
        paperSize->setCurrentIndex( 1 );
    if ( n == PAGE_A4 )
        paperSize->setCurrentIndex( 2 );
    if ( n == PAGE_640 )
        paperSize->setCurrentIndex( 3 );
    if ( n == PAGE_800 )
        paperSize->setCurrentIndex( 4 );
    if ( n == PAGE_1024 )
        paperSize->setCurrentIndex( 5 );
}

int PageSetupDialog::getPageSize()
{
    if ( paperSize->currentIndex() == 0 )
        return PAGE_LETTER;
    if ( paperSize->currentIndex() == 1 )
        return PAGE_LEGAL;
    if ( paperSize->currentIndex() == 2 )
        return PAGE_A4;
    if ( paperSize->currentIndex() == 3 )
        return PAGE_640;
    if ( paperSize->currentIndex() == 4 )
        return PAGE_800;
    if ( paperSize->currentIndex() == 5 )
        return PAGE_1024;
    return PAGE_LETTER;
}

void PageSetupDialog::setOrientation( int n )
{
    if ( n == PAGE_PORTRAIT )
        paperOrient->setCurrentIndex( 0 );
    if ( n == PAGE_LANDSCAPE )
        paperOrient->setCurrentIndex( 1 );
}

int PageSetupDialog::getOrientation()
{
    if ( paperOrient->currentIndex() == 0 )
        return PAGE_PORTRAIT;
    if ( paperOrient->currentIndex() == 1 )
        return PAGE_LANDSCAPE;
    return PAGE_PORTRAIT;
}

//cmake#include "pagesetupdialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
