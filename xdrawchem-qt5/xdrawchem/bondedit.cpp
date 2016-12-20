#include <QGridLayout>
#include <QPixmap>
#include <QRadioButton>
#include <QButtonGroup>
#include <QGroupBox>
#include <QLabel>
#include <QColorDialog>
#include <QBitmap>

// XPM's
#include "lines.h"

#include "bondedit.h"
#include "previewwidget.h"
#include "defs.h"

BondEditDialog::BondEditDialog( QWidget *parent, DPoint * s, DPoint * e, int ty, int o, int d, int th, int st, QColor c1 )
    : QDialog( parent )
{
    s1 = s;
    e1 = e;
    type = ty;
    order = o;
    dash = d;
    thick = th;
    style = st;
    color = c1;

    QGridLayout *mygrid = new QGridLayout();
    setLayout( mygrid );

    QGroupBox *pwFrame = new QGroupBox( tr( "Preview:" ) );
    QGridLayout *pwFrameLayout = new QGridLayout();
    pwFrame->setLayout( pwFrameLayout );

    QSpacerItem *spacerTop = new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pwFrameLayout->addItem( spacerTop, 0, 1 );
    QSpacerItem *spacerBottom = new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::Expanding );
    pwFrameLayout->addItem( spacerBottom, 2, 0 );
    QSpacerItem *spacerLeft = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
    pwFrameLayout->addItem( spacerLeft, 1, 0 );
    QSpacerItem *spacerRight = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
    pwFrameLayout->addItem( spacerRight, 1, 2 );

    pw = new PreviewWidget();
    pw->setMinimumSize( QSize( 50, 50 ) );
    pw->updateWidget( type, thick, dash, order, style, color );
    pwFrameLayout->addWidget( pw, 1, 1 );
    mygrid->addWidget( pwFrame, 0, 0 );

    QLabel *colorLabel = new QLabel( tr( "Color:" ) );
    mygrid->addWidget( colorLabel, 1, 0 );

/*    QPixmap *tmp_pm = new QPixmap( 30, 18 );
    QBitmap mask0( 30, 18 );
    mask0.fill( Qt::color1 );
    tmp_pm->setMask( mask0 );
    tmp_pm->fill( color );*/
    colorBtn = new ColorButton( color );
//    colorBtn->setIcon( QIcon( *tmp_pm ) );
    connect( colorBtn, SIGNAL( pressed() ), SLOT( GetNewColor() ) );
    mygrid->addWidget( colorBtn, 1, 1 );

    if ( type == TYPE_ARROW ) {
        setWindowTitle( tr( "Arrow editor" ) );
        QGroupBox *arrowBtnBox = new QGroupBox( tr( "Style:" ) );
        QVBoxLayout *arrowBtnBoxLayout = new QVBoxLayout();
        arrowBtnBox->setLayout( arrowBtnBoxLayout );
        QButtonGroup *arrowBtnGroup = new QButtonGroup( this );
        arrowBtnGroup->setExclusive( true );
        connect( arrowBtnGroup, SIGNAL( buttonClicked( int ) ), SLOT( styleChanged( int ) ) );

        QRadioButton *plainArrowBtn = new QRadioButton( tr( "Plain arrow" ) );
        if ( style == ARROW_REGULAR )
            plainArrowBtn->setChecked( true );
        arrowBtnBoxLayout->addWidget( plainArrowBtn );
        arrowBtnGroup->addButton( plainArrowBtn, ARROW_REGULAR );

        QRadioButton *nrArrowBtn = new QRadioButton( tr( "NR arrow" ) );
        if ( style == ARROW_DIDNT_WORK )
            nrArrowBtn->setChecked( true );
        arrowBtnBoxLayout->addWidget( nrArrowBtn );
        arrowBtnGroup->addButton( nrArrowBtn, ARROW_DIDNT_WORK );

        QRadioButton *dashedArrowBtn = new QRadioButton( tr( "Dashed arrow" ) );
        if ( style == ARROW_DASH )
            dashedArrowBtn->setChecked( true );
        arrowBtnBoxLayout->addWidget( dashedArrowBtn );
        arrowBtnGroup->addButton( dashedArrowBtn, ARROW_DASH );

        QRadioButton *twArrowBtn = new QRadioButton( tr( "Two-way arrow" ) );
        if ( style == ARROW_BI1 )
            twArrowBtn->setChecked( true );
        arrowBtnBoxLayout->addWidget( twArrowBtn );
        arrowBtnGroup->addButton( twArrowBtn, ARROW_BI1 );

        QRadioButton *splitTwArrowBtn = new QRadioButton( tr( "Split two-way arrow" ) );
        if ( style == ARROW_BI2 )
            splitTwArrowBtn->setChecked( true );
        arrowBtnBoxLayout->addWidget( splitTwArrowBtn );
        arrowBtnGroup->addButton( splitTwArrowBtn, ARROW_BI2 );

        QRadioButton *retroArrowBtn = new QRadioButton( tr( "Retrosynthetic arrow" ) );
        if ( style == ARROW_RETRO )
            retroArrowBtn->setChecked( true );
        arrowBtnBoxLayout->addWidget( retroArrowBtn );
        arrowBtnGroup->addButton( retroArrowBtn, ARROW_RETRO );

        mygrid->addWidget( arrowBtnBox, 0, 1 );

        QLabel *arrowThickLbl = new QLabel( tr( "Arrow thickness:" ) );
        mygrid->addWidget( arrowThickLbl, 2, 0 );
        // arrow thickness list
        ltList = new QComboBox();
        QPixmap px1;
        QBitmap mask0( 12, 12 );

        mask0.fill( Qt::color0 );
        px1 = QPixmap( line1 );
        px1.setMask( mask0 );
        ltList->addItem( QIcon( QPixmap( line1 ) ), "1" );
        ltList->addItem( QIcon( QPixmap( line2 ) ), "2" );
        ltList->addItem( QIcon( QPixmap( line3 ) ), "3" );
        ltList->addItem( QIcon( QPixmap( line4 ) ), "4" );
        ltList->addItem( QIcon( QPixmap( line5 ) ), "5" );
        connect( ltList, SIGNAL( activated( int ) ), SLOT( setThick( int ) ) );
        ltList->setCurrentIndex( th - 1 );
        mygrid->addWidget( ltList, 2, 1 );

    } else if ( type == TYPE_BOND ) {
        setWindowTitle( tr( "Bond editor" ) );
        QGroupBox *bondBtnBox = new QGroupBox( tr( "Bond order:" ) );
        QVBoxLayout *bondBtnBoxLayout = new QVBoxLayout();
        bondBtnBox->setLayout( bondBtnBoxLayout );
        QButtonGroup *bondBtnGroup = new QButtonGroup( this );
        bondBtnGroup->setExclusive( true );
        connect( bondBtnGroup, SIGNAL( buttonClicked( int ) ), SLOT( orderChanged( int ) ) );

        QRadioButton *dashedBondBtn = new QRadioButton( tr( "Dashed line" ) );
        if ( ( order == 1 ) && ( dash == 1 ) )
            dashedBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( dashedBondBtn );
        bondBtnGroup->addButton( dashedBondBtn, 0 );

        QRadioButton *singleBondBtn = new QRadioButton( tr( "Single" ) );
        if ( ( order == 1 ) && ( dash == 0 ) )
            singleBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( singleBondBtn );
        bondBtnGroup->addButton( singleBondBtn, 1 );

        QRadioButton *stereoUpBondBtn = new QRadioButton( tr( "Stereo-up" ) );
        if ( ( order == 5 ) && ( dash == 0 ) )
            stereoUpBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( stereoUpBondBtn );
        bondBtnGroup->addButton( stereoUpBondBtn, 5 );

        QRadioButton *stereoDownBondBtn = new QRadioButton( tr( "Stereo-down" ) );
        if ( ( order == 7 ) && ( dash == 0 ) )
            stereoDownBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( stereoDownBondBtn );
        bondBtnGroup->addButton( stereoDownBondBtn, 7 );

        QRadioButton *wavyBondBtn = new QRadioButton( tr( "Wavy" ) );
        if ( ( order == 6 ) && ( dash == 0 ) )
            wavyBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( wavyBondBtn );
        bondBtnGroup->addButton( wavyBondBtn, 6 );

        QRadioButton *aromaticBondBtn = new QRadioButton( tr( "Single/double (aromatic)" ) );
        if ( ( order == 2 ) && ( dash == 1 ) )
            aromaticBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( aromaticBondBtn );
        bondBtnGroup->addButton( aromaticBondBtn, 11 );

        QRadioButton *doubleBondBtn = new QRadioButton( tr( "Double" ) );
        if ( ( order == 2 ) && ( dash == 0 ) )
            doubleBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( doubleBondBtn );
        bondBtnGroup->addButton( doubleBondBtn, 2 );

        QRadioButton *doubleTripleBondBtn = new QRadioButton( tr( "Double/triple" ) );
        if ( ( order == 3 ) && ( dash == 1 ) )
            doubleTripleBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( doubleTripleBondBtn );
        bondBtnGroup->addButton( doubleTripleBondBtn, 12 );

        QRadioButton *tripleBondBtn = new QRadioButton( tr( "Triple" ) );
        if ( ( order == 3 ) && ( dash == 0 ) )
            tripleBondBtn->setChecked( true );
        bondBtnBoxLayout->addWidget( tripleBondBtn );
        bondBtnGroup->addButton( tripleBondBtn, 3 );

        mygrid->addWidget( bondBtnBox, 0, 1 );
        //QToolTip::add(ltList, TOOLTIP_SET_THICKNESS);
        //QWhatsThis::add(ltList, "Set Line Thickness");
        QLabel *bondThickLbl = new QLabel( tr( "Bond thickness:" ) );
        mygrid->addWidget( bondThickLbl, 2, 0 );
        // bond thickness list
        ltList = new QComboBox();
        QPixmap px1;
        QBitmap mask0( 12, 12 );

        mask0.fill( Qt::color0 );
        px1 = QPixmap( line1 );
        px1.setMask( mask0 );
        ltList->addItem( QIcon ( QPixmap( line1 ) ), "1" );
        ltList->addItem( QIcon ( QPixmap( line2 ) ), "2" );
        ltList->addItem( QIcon ( QPixmap( line3 ) ), "3" );
        ltList->addItem( QIcon ( QPixmap( line4 ) ), "4" );
        ltList->addItem( QIcon ( QPixmap( line5 ) ), "5" );
        connect( ltList, SIGNAL( activated( int ) ), this, SLOT( setThick( int ) ) );

        mygrid->addWidget( ltList, 2, 1 );
        ltList->setCurrentIndex( th - 1 );

        QLabel *bondAlignLbl = new QLabel( tr( "Double bond alignment:" ) );
        mygrid->addWidget( bondAlignLbl, 3, 0 );

        dbList = new QComboBox();
        dbList->addItem( tr( "Auto" ) );
        dbList->addItem( tr( "Left" ) );
        dbList->addItem( tr( "Center" ) );
        dbList->addItem( tr( "Right" ) );
        connect( dbList, SIGNAL( activated( int ) ), this, SLOT( setDoubleBond( int ) ) );

        mygrid->addWidget( dbList, 3, 1 );

    } else if ( type == TYPE_BRACKET ) {
        setWindowTitle( tr( "Bracket editor" ) );
        QGroupBox *bracketBtnBox = new QGroupBox( tr( "Style:" ) );
        QVBoxLayout *bracketBtnBoxLayout = new QVBoxLayout();
        bracketBtnBox->setLayout( bracketBtnBoxLayout );
        QButtonGroup *bracketBtnGroup = new QButtonGroup( this );
        bracketBtnGroup->setExclusive( true );
        connect( bracketBtnGroup, SIGNAL( buttonClicked( int ) ), SLOT( styleChanged( int ) ) );

        QRadioButton *squareBracketBtn =  new QRadioButton( tr( "Square bracket" ) );
        if ( style == BRACKET_SQUARE )
            squareBracketBtn->setChecked( true );
        bracketBtnBoxLayout->addWidget( squareBracketBtn );
        bracketBtnGroup->addButton( squareBracketBtn, BRACKET_SQUARE );

        QRadioButton *parenBracketBtn = new QRadioButton( tr( "Parentheses" ) );
        if ( style == BRACKET_CURVE )
            parenBracketBtn -> setChecked( true );
        bracketBtnBoxLayout->addWidget( parenBracketBtn );
        bracketBtnGroup->addButton( parenBracketBtn, BRACKET_CURVE );

        QRadioButton *curlyBracketBtn =  new QRadioButton( tr( "Curly bracket" ) );
        if ( style == BRACKET_BRACE )
            curlyBracketBtn -> setChecked( true );
        bracketBtnBoxLayout->addWidget( curlyBracketBtn );
        bracketBtnGroup->addButton( curlyBracketBtn, BRACKET_BRACE );

        QRadioButton *boxBracketBtn =  new QRadioButton( tr( "Box" ) );
        if ( style == BRACKET_BOX )
            boxBracketBtn -> setChecked( true );
        bracketBtnBoxLayout->addWidget( boxBracketBtn );
        bracketBtnGroup->addButton( boxBracketBtn, BRACKET_BOX );

        QRadioButton *ellipseBracketBtn =  new QRadioButton( tr( "Ellipse" ) );
        if ( style == BRACKET_ELLIPSE )
            ellipseBracketBtn -> setChecked( true );
        bracketBtnBoxLayout->addWidget( ellipseBracketBtn );
        bracketBtnGroup->addButton( ellipseBracketBtn, BRACKET_ELLIPSE );

        mygrid->addWidget( bracketBtnBox, 0, 1 );

    } else if ( type == TYPE_CURVEARROW ) {
        setWindowTitle( tr( "Curved Arrow editor" ) );
        QGroupBox *curveArrowBtnBox = new QGroupBox( tr( "Style:" ) );
        QVBoxLayout *curveArrowBtnBoxLayout = new QVBoxLayout();
        curveArrowBtnBox->setLayout( curveArrowBtnBoxLayout );
        QButtonGroup *curveArrowBtnGroup = new QButtonGroup( this );
        curveArrowBtnGroup->setExclusive( true );
        connect( curveArrowBtnGroup, SIGNAL( buttonClicked( int ) ), SLOT( styleChanged( int ) ) );

        QRadioButton *cw90ArrowBtn = new QRadioButton( tr( "90 degree clockwise" ) );
        if ( style == CA_CW90 )
            cw90ArrowBtn->setChecked( true );
        curveArrowBtnBoxLayout->addWidget( cw90ArrowBtn );
        curveArrowBtnGroup->addButton( cw90ArrowBtn, CA_CW90 );

        QRadioButton *cw180ArrowBtn = new QRadioButton( tr( "180 degree clockwise" ) );
        if ( style == CA_CW180 )
            cw180ArrowBtn->setChecked( true );
        curveArrowBtnBoxLayout->addWidget( cw180ArrowBtn );
        curveArrowBtnGroup->addButton( cw180ArrowBtn, CA_CW180 );

        QRadioButton *cw270ArrowBtn = new QRadioButton( tr( "270 degree clockwise" ) );
        if ( style == CA_CW270 )
            cw270ArrowBtn->setChecked( true );
        curveArrowBtnBoxLayout->addWidget( cw270ArrowBtn );
        curveArrowBtnGroup->addButton( cw270ArrowBtn, CA_CW270 );

        QRadioButton *ccw90ArrowBtn = new QRadioButton( tr( "90 degree counterclockwise" ) );
        if ( style == CA_CCW90 )
            ccw90ArrowBtn->setChecked( true );
        curveArrowBtnBoxLayout->addWidget( ccw90ArrowBtn );
        curveArrowBtnGroup->addButton( ccw90ArrowBtn, CA_CCW90 );

        QRadioButton *ccw180ArrowBtn = new QRadioButton( tr( "180 degree counterclockwise" ) );
        if ( style == CA_CCW180 )
            ccw180ArrowBtn->setChecked( true );
        curveArrowBtnBoxLayout->addWidget( ccw180ArrowBtn );
        curveArrowBtnGroup->addButton( ccw180ArrowBtn, CA_CCW180 );

        QRadioButton *ccw270ArrowBtn = new QRadioButton( tr( "270 degree counterclockwise" ) );
        if ( style == CA_CCW270 )
            ccw270ArrowBtn->setChecked( true );
        curveArrowBtnBoxLayout->addWidget( ccw270ArrowBtn );
        curveArrowBtnGroup->addButton( ccw270ArrowBtn, CA_CCW270 );

        mygrid->addWidget( curveArrowBtnBox, 0, 1 );

    } else if ( type == TYPE_SYMBOL ) {
        setWindowTitle( tr( "Symbol editor" ) );
        QGroupBox *symbolBtnBox = new QGroupBox( tr( "Style:" ) );
        QVBoxLayout *symbolBtnBoxLayout = new QVBoxLayout();
        symbolBtnBox->setLayout( symbolBtnBoxLayout );
        QButtonGroup *symbolBtnGroup = new QButtonGroup( this );
        symbolBtnGroup->setExclusive( true );
        connect( symbolBtnGroup, SIGNAL( buttonClicked( int ) ), SLOT( styleChanged( int ) ) );

        QRadioButton *plusSymbolBtn = new QRadioButton( tr( "Positive charge" ) );
        if ( style == SYM_PLUS )
            plusSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( plusSymbolBtn );
        symbolBtnGroup->addButton( plusSymbolBtn, SYM_PLUS );

        QRadioButton *minusSymbolBtn = new QRadioButton( tr( "Negative charge" ) );
        if ( style == SYM_MINUS )
            minusSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( minusSymbolBtn );
        symbolBtnGroup->addButton( minusSymbolBtn, SYM_MINUS );

        QRadioButton *partPlusSymbolBtn = new QRadioButton( tr( "Partial positive charge" ) );
        if ( style == SYM_DELTA_PLUS )
            partPlusSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( partPlusSymbolBtn );
        symbolBtnGroup->addButton( partPlusSymbolBtn, SYM_DELTA_PLUS );

        QRadioButton *partMinusSymbolBtn = new QRadioButton( tr( "Partial negative charge" ) );
        if ( style == SYM_DELTA_MINUS )
            partMinusSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( partMinusSymbolBtn );
        symbolBtnGroup->addButton( partMinusSymbolBtn, SYM_DELTA_MINUS );

        QRadioButton *singleSymbolBtn = new QRadioButton( tr( "Single electron" ) );
        if ( style == SYM_1E )
            singleSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( singleSymbolBtn );
        symbolBtnGroup->addButton( singleSymbolBtn, SYM_1E );

        QRadioButton *pairSymbolBtn = new QRadioButton( tr( "Electron pair" ) );
        if ( style == SYM_2E )
            pairSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( pairSymbolBtn );
        symbolBtnGroup->addButton( pairSymbolBtn, SYM_2E );

        QRadioButton *ringUpSymbolBtn = new QRadioButton( tr( "Stereochemical ring hydrogen" ) );
        if ( style == SYM_RING_UP )
            ringUpSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( ringUpSymbolBtn );
        symbolBtnGroup->addButton( ringUpSymbolBtn, SYM_RING_UP );

        QRadioButton *pOrbitalSymbolBtn = new QRadioButton( tr( "p orbital" ) );
        if ( style == SYM_P_ORBITAL )
            pOrbitalSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( pOrbitalSymbolBtn );
        symbolBtnGroup->addButton( pOrbitalSymbolBtn, SYM_P_ORBITAL );

        QRadioButton *pDoubleSymbolBtn = new QRadioButton( tr( "p double" ) );
        if ( style == SYM_P_DOUBLE )
            pDoubleSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( pDoubleSymbolBtn );
        symbolBtnGroup->addButton( pDoubleSymbolBtn, SYM_P_DOUBLE );

        QRadioButton *beadSymbolBtn = new QRadioButton( tr( "bead" ) );
        if ( style == SYM_BEAD )
            beadSymbolBtn->setChecked( true );
        symbolBtnBoxLayout->addWidget( beadSymbolBtn );
        symbolBtnGroup->addButton( beadSymbolBtn, SYM_BEAD );

        mygrid->addWidget( symbolBtnBox, 0, 1 );
    }

    QHBoxLayout *btnLayout = new QHBoxLayout();

    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
    btnLayout->addItem( spacer );

    QPushButton *okBtn, *dismissBtn;

    okBtn = new QPushButton( tr( "OK" ), this );
    connect( okBtn, SIGNAL( clicked() ), SLOT( accept() ) );
    btnLayout->addWidget( okBtn );
    dismissBtn = new QPushButton( tr( "Cancel" ), this );
    connect( dismissBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    btnLayout->addWidget( dismissBtn );

    mygrid->addLayout(btnLayout, 4, 0, 1, 2);
}

void BondEditDialog::GetNewColor()
{
    QColor nc1 = QColorDialog::getColor( color );

    if ( nc1.isValid() ) {
        color = nc1;
        colorBtn->setColor( color );
    }
}

//cmake#include "bondedit.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
