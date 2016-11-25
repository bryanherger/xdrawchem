/****************************************************************************
** $Id: application.cpp,v 1.46 2005/11/16 03:01:55 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <QDateTime>
#include <QGridLayout>
#include <QApplication>
#include <QCloseEvent>
#include <QWhatsThis>
#include <QStatusBar>
#include <QMenuBar>
#include <QMessageBox>
#include <QColorDialog>
#include <QFontDialog>
#include <QDesktopWidget>
#include <QLineEdit>
#include <QBitmap>
#include <unistd.h>

#include "defs.h"
#include "drawable.h"
#include "renderarea.h"
#include "render2d.h"
#include "chemdata.h"
#include "ringdialog.h"
#include "fixeddialog.h"
#include "netdialog.h"
#include "netchoosedialog.h"
#include "netaccess.h"
#include "pagesetupdialog.h"
#include "peptidebuilder.h"
#include "crings_dialog.h"
#include "helpwindow.h"
#include "xdc_event.h"
#include "xdc_toolbutton.h"
#include "myfiledialog.h"
#include "xruler.h"
#include "dyk.h"
#include "charsel.h"
#include "application.h"
#include "colorbutton.h"

// main window icon
//#include "xdc.xpm"


// top toolbar
#include "filesave.xpm"
#include "fileopen.xpm"
#include "fileprint.xpm"
#include "cuttool.xpm"
#include "copytool.xpm"
#include "pastetool.xpm"

// line thicknesses
#include "lines.h"

// text operation buttons
#include "boldtool.xpm"
#include "italictool.xpm"
#include "underlinetool.xpm"
#include "superscript.xpm"
#include "subscript.xpm"
#include "justifytools.h"
#include "magnifytools.h"

// left side toolbar
#include "selecttool.xpm"
#include "lassotool.xpm"
#include "erasetool.xpm"
#include "linetool.xpm"
#include "dashtool.xpm"
#include "chaintool.xpm"
#include "uptool.xpm"
#include "downtool.xpm"
#include "wavytool.xpm"
#include "texttool.xpm"
#include "arrowtool.xpm"
#include "curvearrowtool.xpm"
#include "brackettool.xpm"
#include "ringtool.xpm"
#include "symboltool.xpm"

// predefined toolbar
#include "rings.h"

// Brackets
#include "brackets.h"
// end brackets

// Symbols
#include "symbol_xpm.h"
#include "newman.xpm"
#include "newman-anti.xpm"
#include "newman-eclipse.xpm"
// end symbols

// Arrows
#include "arrows.h"
#include "cw90.xpm"
#include "ccw90.xpm"
#include "cw180.xpm"
#include "ccw180.xpm"
#include "cw270.xpm"
#include "ccw270.xpm"
#include "cubicbezier.h"
// end Arrows

// defined in main.cpp
//extern QString RingDir, HomeDir;

ApplicationWindow::ApplicationWindow()
    :  QMainWindow()
{
    setFont( preferences.getMainFont() );

    // new, fancy PNG icon
    setWindowIcon( QPixmap( RingDir + "xdrawchem-icon.png" ) );

    OBGetFilters();

    // What's This? text for top toolbar
    QString fileOpenText = tr( "Click this button to open a file.<br><br>You can also select the Open command from the File menu." );
    QString fileSaveText = tr( "Click this button to save the file you are editing.<br><br>You can also select the Save command from the File menu.<br><br>" );
    QString filePrintText = tr( "Click this button to print the file you are editing.<br><br>You can also select the Print command from the File menu." );
    QString editCutText = tr( "Click this button to cut a selection.<br><br>You can also select the Cut command from the Edit menu, or press Ctrl+X." );
    QString editCopyText = tr( "Click this button to copy a selection.<br><br>You can also select the Copy command from the Edit menu, or press Ctrl+C." );
    QString editPasteText = tr( "Click this button to paste a selection.<br><br>You can also select the Paste command from the Edit menu, or press Ctrl+V." );
    QString editMPText = tr( "Click this button to zoom in." );
    QString editMMText = tr( "Click this button to zoom out." );

    // What's This? text for left side toolbar
    QString selectToolText = tr( "Select tool<br><br>Use the Select tool to select and move items inside a box.<br><br>You can select multiple items and cut, copy, move and rotate them." );
    QString lassoToolText = tr( "Lasso tool<br><br>Use the Lasso tool to select and move items by drawing a loop around them.<br><br>You can select multiple items and cut, copy, move and rotate them." );
    QString eraseToolText = tr( "Erase tool<br><br>Use the Erase tool to erase individual items." );
    QString lineToolText = tr( "Line tool<br><br>Use the Line tool to draw bonds.  Draw over existing bonds to create double and triple bonds." );
    QString dashLineToolText = tr( "Dashed Line tool<br><br>Use the Dashed Line tool to draw dashed lines.  Draw over existing bonds to add dashed lines (to indicate resonance, etc.)." );
    QString chainToolText = tr( "Chain tool<br><br>Use the Chain tool to draw aliphatic chains.  The length of each segment is the current bond length." );
    QString upLineToolText = tr( "Stereo Up Line tool<br><br>Use the Line tool to draw stereo-up lines, as shown on the button." );
    QString downLineToolText = tr( "Stereo Down Line tool<br><br>Use the Line tool to draw stereo-down lines, as shown on the button." );
    QString wavyLineToolText = tr( "Wavy Line tool<br><br>Use the Wavy Line tool to draw wavy lines, as shown on the button." );
    QString arrowToolText = tr( "Arrow tool<br><br>Use the Arrow tool to draw straight arrows.<br><br>Click to draw a straight arrow.<br><br>Click and hold to pick from a list of available arrows." );
    QString cArrowToolText = tr( "Curved Arrow tool<br><br>Use the Curved Arrow tool to insert curved arrows.<br><br>Click and hold to select from a picture menu of arrows." );
    QString bracketToolText = tr( "Bracket tool<br><br>Use the Bracket tool to draw brackets and parentheses.<br><br>Click to draw square brackets.<br><br>Click and hold to select from a picture menu of brackets." );
    QString textToolText = tr( "Text tool<br><br>Use the Text tool to add text and label atoms and points.<br>(See manual for info on formatting text)" );
    QString ringToolText = tr( "Ring tool<br><br>Use the Ring tool to insert ready-made rings and structures.<br><br>Click to open the ring dialog, which allows selection from a list of all built-in rings and structures.<br><br>Click and hold to select from a picture menu of select rings.<br>(See manual for more info on modifying this menu)" );
    QString symbolToolText = tr( "Symbol tool<br><br>Use the Symbol tool to insert symbols.<br><br>Click and hold to select from a picture menu of symbols.<br>" );

    printer = new QPrinter;
    QPixmap openIcon, saveIcon, printIcon, superIcon, subIcon;
    QPixmap swatchblack, swatchred, swatchgreen, swatchblue;
    QPixmap selectIcon, drawBoxIcon, drawLineIcon, downLineIcon, upLineIcon, textIcon, arrowIcon, bracketIcon, cutIcon, copyIcon, pasteIcon, ringIcon, dashIcon, symbolIcon, curveArrowIcon, chainIcon, lassoIcon, mmIcon, mpIcon;

    m_centralWidget = new QWidget( this );
    setCentralWidget( m_centralWidget );

    /// create scrolling viewport and render widget
    m_renderArea = new RenderArea( m_centralWidget );
    m_renderer = new Render2D( m_centralWidget );
    m_renderArea->setWidget( m_renderer );
    connect( m_renderArea, SIGNAL( scrolled( int, int ) ), SLOT( svXY( int, int ) ) );

    /// create grid layout
    QGridLayout *glo = new QGridLayout( m_centralWidget );

    m_rulerUnitLabel = new QLabel( m_centralWidget );
    m_rulerUnitLabel->setText( QString::fromLatin1( "px" ) );
    glo->addWidget( m_rulerUnitLabel, 0, 0 );
    vruler = new XRuler( m_centralWidget );
    vruler->setHV( 2 );
    vruler->setFixedWidth( 20 );
    glo->addWidget( vruler, 1, 0 );
    hruler = new XRuler( m_centralWidget );
    hruler->setHV( 1 );
    hruler->setFixedHeight( 20 );
    glo->addWidget( hruler, 0, 1 );
    glo->addWidget( m_renderArea, 1, 1 );

    /**
     * top toolbar
     */
    fileTools = new QToolBar( tr( "File Operations" ), this );
    addToolBar( fileTools );

    QAction *fileOpenAction = fileTools->addAction( QIcon( RingDir + "fileopen.png" ), tr( "Open file" ),
                                                    this, SLOT( load() ) );

    fileOpenAction->setWhatsThis( fileOpenText );

    QAction *fileSaveAction = fileTools->addAction( QIcon( RingDir + "filesave.png" ), tr( "Save file" ),
                                                    this, SLOT( save() ) );

    fileSaveAction->setWhatsThis( fileSaveText );

    QAction *filePrintAction = fileTools->addAction( QIcon( RingDir + "fileprint.png" ), tr( "Print file" ),
                                                     this, SLOT( print() ) );

    filePrintAction->setWhatsThis( filePrintText );

    QAction *cutAction = fileTools->addAction( QIcon( RingDir + "cuttool.png" ), tr( "Cut" ),
                                               this, SLOT( Cut() ) );

    cutAction->setWhatsThis( editCutText );

    QAction *copyAction = fileTools->addAction( QIcon( RingDir + "copytool.png" ), tr( "Copy" ),
                                                this, SLOT( Copy() ) );

    copyAction->setWhatsThis( editCopyText );

    QAction *pasteAction = fileTools->addAction( QIcon( RingDir + "pastetool.png" ), tr( "Paste" ),
                                                 this, SLOT( Paste() ) );

    pasteAction->setWhatsThis( editPasteText );

    QAction *magPlusAction = fileTools->addAction( QIcon( RingDir + "mag_plus.png" ), tr( "Zoom In" ),
                                                   this, SLOT( MagnifyPlus() ) );

    magPlusAction->setWhatsThis( editMPText );

    QAction *magMinusAction = fileTools->addAction( QIcon( RingDir + "mag_minus.png" ), tr( "Zoom Out" ),
                                                    this, SLOT( MagnifyMinus() ) );

    magMinusAction->setWhatsThis( editMMText );

//     QPixmap tmp_pm = QPixmap( 32, 20 );
//
//     tmp_pm.fill( QColor( 210, 210, 210 ) );
//     QPainter pixpaint( &tmp_pm );
//
//     pixpaint.fillRect( 2, 3, 14, 14, QColor( 0, 0, 0 ) );
//     pixpaint.setPen( QColor( 0, 0, 0 ) );
//     pixpaint.drawLine( 24, 2, 24, 16 );
//     pixpaint.drawLine( 24, 16, 20, 12 );
//     pixpaint.drawLine( 24, 16, 28, 12 );
//     setColorAction = fileTools->addAction( QIcon( tmp_pm ), tr( "Color" ), this, SLOT( NewColor() ) );

    colorBtn = new ColorButton( m_renderer->GetColor() );
    connect( colorBtn, SIGNAL( pressed() ), SLOT( NewColor() ) );
    fileTools->addWidget( colorBtn );

    ltList = new QComboBox( fileTools );
    ltList->setToolTip( tr( "Set line thickness" ) );
    ltList->setWhatsThis( tr( "Set Line Thickness" ) );
    QPixmap px1;
    QBitmap mask0( 12, 12 );

    mask0.fill( Qt::color0 );
    px1 = QPixmap( line1 );
    px1.setMask( mask0 );
    ltList->addItem( QIcon( RingDir + "line1.png" ), "1" );
    ltList->addItem( QIcon( RingDir + "line2.png" ), "2" );
    ltList->addItem( QIcon( RingDir + "line3.png" ), "3" );
    ltList->addItem( QIcon( RingDir + "line4.png" ), "4" );
    ltList->addItem( QIcon( RingDir + "line5.png" ), "5" );
    connect( ltList, SIGNAL( activated( int ) ), SLOT( SetThick( int ) ) );

    fileTools->addWidget( ltList );

    fontList = new QComboBox( fileTools );
    fontList->setToolTip( tr( "Set font" ) );
    fontList->setWhatsThis( tr( "Set Font" ) );
    fontList->addItem( "Courier" );
    fontList->addItem( "Helvetica" );
    fontList->addItem( "Symbol" );
    fontList->addItem( "Times" );
    fontList->setCurrentIndex( 1 );
    connect( fontList, SIGNAL( activated( int ) ), SLOT( setFontFace( int ) ) );

    fileTools->addWidget( fontList );

    fontSizeList = new QComboBox( fileTools );
    fontSizeList->setToolTip( tr( "Set font size" ) );
    fontSizeList->setWhatsThis( tr( "Set Font Size" ) );
    fontSizeList->addItem( "8" );
    fontSizeList->addItem( "10" );
    fontSizeList->addItem( "12" );
    fontSizeList->addItem( "14" );
    fontSizeList->addItem( "18" );
    fontSizeList->addItem( "24" );
    fontSizeList->addItem( "32" );
    fontSizeList->setCurrentIndex( 2 );
    connect( fontSizeList, SIGNAL( activated( int ) ), this, SLOT( setFontPoints( int ) ) );

    fileTools->addWidget( fontSizeList );

    justifyLeftAction = fileTools->addAction( QIcon( RingDir + "justifylefttool.png" ), tr( "Left-justify selected text" ), m_renderer, SLOT( JustifyLeft() ) );
    justifyLeftAction->setVisible( false );

    justifyCenterAction = fileTools->addAction( QIcon( RingDir + "justifycentertool.png" ), tr( "Center selected text" ), m_renderer, SLOT( JustifyCenter() ) );
    justifyCenterAction->setVisible( false );

    justifyRightAction = fileTools->addAction( QIcon( RingDir + "justifyrighttool.png" ), tr( "Right-justify selected text" ), m_renderer, SLOT( JustifyRight() ) );
    justifyRightAction->setVisible( false );

    boldAction = fileTools->addAction( QIcon( RingDir + "boldtool.png" ), tr( "Make selected text <b>bold</b>" ), m_renderer, SLOT( Bold() ) );
    boldAction->setVisible( false );

    italicAction = fileTools->addAction( QIcon( RingDir + "italictool.png" ), tr( "<i>Italicize</i> selected text" ), m_renderer, SLOT( Italic() ) );
    italicAction->setVisible( false );

    underlineAction = fileTools->addAction( QIcon( RingDir + "underlinetool.png" ), tr( "Underline selected text" ), m_renderer, SLOT( Underline() ) );
    underlineAction->setVisible( false );

    superscriptAction = fileTools->addAction( QIcon( RingDir + "superscript.png" ), tr( "Superscript selected text" ), m_renderer, SLOT( Superscript() ) );
    superscriptAction->setVisible( false );

    subscriptAction = fileTools->addAction( QIcon( RingDir + "subscript.png" ), tr( "Subscript selected text" ), m_renderer, SLOT( Subscript() ) );
    subscriptAction->setVisible( false );

    QAction *whatsThisAction = QWhatsThis::createAction( fileTools );

    fileTools->addAction( whatsThisAction );

    /**
     * left side toolbar
     */
    drawTools = new QToolBar( this );
    addToolBar( Qt::LeftToolBarArea, drawTools );

    QAction *selectAction = drawTools->addAction( QIcon( RingDir + "selecttool.png" ), tr( "Select" ),
                                                  m_renderer, SLOT( setMode_Select() ) );

    selectAction->setWhatsThis( selectToolText );

    QAction *lassoAction = drawTools->addAction( QIcon( RingDir + "lassotool.png" ), tr( "Lasso" ),
                                                 m_renderer, SLOT( setMode_Lasso() ) );

    lassoAction->setWhatsThis( lassoToolText );

    QAction *eraseAction = drawTools->addAction( QIcon( RingDir + "erasetool.png" ), tr( "Erase" ),
                                                 m_renderer, SLOT( setMode_Erase() ) );

    eraseAction->setWhatsThis( eraseToolText );

    QAction *drawLineAction = drawTools->addAction( QIcon( RingDir + "linetool.png" ), tr( "Draw Line" ),
                                                    m_renderer, SLOT( setMode_DrawLine() ) );

    drawLineAction->setWhatsThis( lineToolText );

    QAction *drawDashLineAction = drawTools->addAction( QIcon( RingDir + "dashtool.png" ), tr( "Draw dashed line" ),
                                                        m_renderer, SLOT( setMode_DrawDashLine() ) );

    drawDashLineAction->setWhatsThis( dashLineToolText );

    QAction *drawChainAction = drawTools->addAction( QIcon( RingDir + "chaintool.png" ), tr( "Draw aliphatic chain" ),
                                                     m_renderer, SLOT( setMode_DrawChain() ) );

    drawChainAction->setWhatsThis( chainToolText );

    QAction *drawUpLineAction = drawTools->addAction( QIcon( RingDir + "uptool.png" ), tr( "Draw stereo-up line" ),
                                                      m_renderer, SLOT( setMode_DrawUpLine() ) );

    drawUpLineAction->setWhatsThis( upLineToolText );

    QAction *drawDownLineAction = drawTools->addAction( QIcon( RingDir + "downtool.png" ), tr( "Draw stereo-down line" ),
                                                        m_renderer, SLOT( setMode_DrawDownLine() ) );

    drawDownLineAction->setWhatsThis( downLineToolText );

    QAction *drawWavyLineAction = drawTools->addAction( QIcon( RingDir + "wavytool.png" ), tr( "Draw wavy bond" ),
                                                        m_renderer, SLOT( setMode_DrawWavyLine() ) );

    drawWavyLineAction->setWhatsThis( wavyLineToolText );

    /// arrowbutton
    drawArrowButton = new QToolButton();
    regularArrowMenu = BuildArrowMenu();
    drawArrowButton->setMenu( regularArrowMenu );
    drawArrowButton->setPopupMode( QToolButton::MenuButtonPopup );
    drawArrowButton->setDefaultAction( regularArrowAction );
    drawArrowButton->setWhatsThis( arrowToolText );
    drawArrowButton->setIcon( QIcon( arrow_regular_xpm ) );
    connect( drawArrowButton, SIGNAL( triggered( QAction * ) ), SLOT( FromArrowMenu( QAction * ) ) );
    connect( regularArrowMenu, SIGNAL( triggered( QAction * ) ), SLOT( setRegularArrowAction( QAction * ) ) );
    drawTools->addWidget( drawArrowButton );

    /// curvearrowbutton
    drawCurveArrowButton = new QToolButton();
    curveArrowMenu = BuildCurveArrowMenu();
    drawCurveArrowButton->setMenu( curveArrowMenu );
    drawCurveArrowButton->setPopupMode( QToolButton::MenuButtonPopup );
    drawCurveArrowButton->setDefaultAction( carrowCW90Action );
    drawCurveArrowButton->setWhatsThis( cArrowToolText );
    drawCurveArrowButton->setIcon( QIcon( cw90_xpm ) );
    connect( drawCurveArrowButton, SIGNAL( triggered( QAction * ) ), SLOT( FromCurveArrowMenu( QAction * ) ) );
    connect( curveArrowMenu, SIGNAL( triggered( QAction * ) ), SLOT( setCurveArrowAction( QAction * ) ) );
    drawTools->addWidget( drawCurveArrowButton );

    /// bracketbutton
    drawBracketButton = new QToolButton();
    bracketMenu = BuildBracketMenu();
    drawBracketButton->setMenu( bracketMenu );
    drawBracketButton->setPopupMode( QToolButton::MenuButtonPopup );
    drawBracketButton->setDefaultAction( squareBracketAction );
    drawBracketButton->setWhatsThis( bracketToolText );
    drawBracketButton->setIcon( QIcon( squarebracket_xpm ) );
    connect( drawBracketButton, SIGNAL( triggered( QAction * ) ), SLOT( FromBracketMenu( QAction * ) ) );
    connect( bracketMenu, SIGNAL( triggered( QAction * ) ), SLOT( setBracketAction( QAction * ) ) );
    drawTools->addWidget( drawBracketButton );

    /// textbutton
    QAction *drawTextAction = drawTools->addAction( QIcon( RingDir + "texttool.png" ), tr( "Draw or edit text" ),
                                                    m_renderer, SLOT( setMode_DrawText() ) );

    drawTextAction->setWhatsThis( textToolText );

    //QAction *drawRingAction = new QAction( QIcon( RingDir + "ringtool.png" ), tr( "Draw ring" ), this );

    drawRingButton = new QToolButton();
    ringMenu = BuildNewRingMenu();
    drawRingButton->setMenu( ringMenu );
    drawRingButton->setPopupMode( QToolButton::MenuButtonPopup );
    drawRingButton->setDefaultAction( ring3Action );
    drawRingButton->setWhatsThis( ringToolText );
    drawRingButton->setIcon( QIcon( RingDir + "ringtool.png" ) );
    connect( drawRingButton, SIGNAL( triggered( QAction * ) ), SLOT( FromNewerRingMenu( QAction * ) ) );
    connect( ringMenu, SIGNAL( triggered( QAction * ) ), SLOT( setRingAction( QAction * ) ) );
    drawTools->addWidget( drawRingButton );

    /// symbolbutton
    drawSymbolButton = new QToolButton();
    symbolMenu = BuildSymbolMenu();
    drawSymbolButton->setMenu( symbolMenu );
    drawSymbolButton->setPopupMode( QToolButton::MenuButtonPopup );
    drawSymbolButton->setDefaultAction( symbolPlusAction );
    drawSymbolButton->setWhatsThis( symbolToolText );
    drawSymbolButton->setIcon( QIcon( RingDir + "sym_plus.png" ) );
    connect( drawSymbolButton, SIGNAL( triggered( QAction * ) ), SLOT( FromSymbolMenu( QAction * ) ) );
    connect( symbolMenu, SIGNAL( triggered( QAction * ) ), SLOT( setSymbolAction( QAction * ) ) );
    drawTools->addWidget( drawSymbolButton );

    addToolBarBreak( Qt::LeftToolBarArea );

    /**
     * second (left? right?) toolbar - predefined rings
     */
    ringTools = new QToolBar( this );
    addToolBar( Qt::LeftToolBarArea, ringTools );

    XDC_ToolButton *b1;
    b1 = new XDC_ToolButton( ringTools, "cyclopropane.cml");
    b1->setIcon( QIcon(RingDir + "cyclopropane.png") );
    connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
    connect( b1, SIGNAL(IncludeFile(QString)), this,
	     SLOT(FromRingToolbar(QString)) );
    ringTools->addWidget( b1 );

    b1 = new XDC_ToolButton( ringTools, "cyclobutane.cml");
    b1->setIcon( QIcon(RingDir + "cyclobutane.png") );
    connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
    connect( b1, SIGNAL(IncludeFile(QString)), this,
	     SLOT(FromRingToolbar(QString)) );
    ringTools->addWidget( b1 );

    b1 = new XDC_ToolButton( ringTools, "cyclopentane.cml");
    b1->setIcon( QIcon(RingDir + "cyclopentane.png") );
    connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
    connect( b1, SIGNAL(IncludeFile(QString)), this,
	     SLOT(FromRingToolbar(QString)) );
    ringTools->addWidget( b1 );

    b1 = new XDC_ToolButton( ringTools, "cyclopentadiene.cml");
    b1->setIcon( QIcon(RingDir + "cyclopentadiene.png") );
    connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
    connect( b1, SIGNAL(IncludeFile(QString)), this,
	     SLOT(FromRingToolbar(QString)) );
    ringTools->addWidget( b1 );

    b1 = new XDC_ToolButton( ringTools, "cyclohexane.cml");
    b1->setIcon( QIcon(RingDir + "cyclohexane.png") );
    connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
    connect( b1, SIGNAL(IncludeFile(QString)), this,
	     SLOT(FromRingToolbar(QString)) );
    ringTools->addWidget( b1 );

    b1 = new XDC_ToolButton( ringTools, "benzene.cml");
    b1->setIcon( QIcon(RingDir + "benzene.png") );
    connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
    connect( b1, SIGNAL(IncludeFile(QString)), this,
	     SLOT(FromRingToolbar(QString)) );
    ringTools->addWidget( b1 );

    /* may have to use these for Qt 4...
    QAction *cyclopropaneAction = ringTools->addAction( QIcon( RingDir + "cyclopropane.png" ), tr( "Cyclopropane" ), this, SLOT( FromRingToolbar( QString("cyclopropane.cml") ) ) );

    QAction *cyclobutaneAction = ringTools->addAction( QIcon( RingDir + "cyclobutane.png" ), tr( "Cyclobutane" ), this, SLOT( FromRingToolbar( QString("cyclobutane.cml") ) ) );

    QAction *cyclopentaneAction = ringTools->addAction( QIcon( RingDir + "cyclopentane.png" ), tr( "Cyclopentane" ), this, SLOT( FromRingToolbar( QString("cyclopentane.cml") ) ) );

    QAction *cyclopentadieneAction = ringTools->addAction( QIcon( RingDir + "cyclopentadiene.png" ), tr( "Cyclopentadiene" ), this, SLOT( FromRingToolbar( QString("cyclopentadiene.cml") ) ) );

    QAction *cyclohexaneAction = ringTools->addAction( QIcon( RingDir + "cyclohexane.png" ), tr( "Cyclohexane" ), this, SLOT( FromRingToolbar( QString("cyclohexane.cml") ) ) );

    QAction *ring_chairAction = ringTools->addAction( QIcon( RingDir + "6ring_chair.png" ), tr( "Cyclohexane - chair conformation" ), this, SLOT( FromRingToolbar( QString("6ring_chair.cml") ) ) );

    QAction *ring_boatAction = ringTools->addAction( QIcon( RingDir + "6ring_boat.png" ), tr( "Cyclohexane - boat conformation" ), this, SLOT( FromRingToolbar( QString("6ring_boat.cml") ) ) );

    QAction *benzeneAction = ringTools->addAction( QIcon( RingDir + "benzene.png" ), tr( "Benzene" ), this, SLOT( FromRingToolbar( QString("benzene.cml") ) ) );
    */

    /**
     * BioTools toolbar
     */
    bioTools = BuildBioTools();
    addToolBar( Qt::RightToolBarArea, bioTools );

    /**
     * menus
     *
     * file menu
     */
    QMenu *file = menuBar()->addMenu( tr( "&File" ) );

    file->addAction( tr( "&New" ), this, SLOT( newDoc() ), Qt::CTRL + Qt::Key_N );
    file->addAction( QIcon( RingDir + "fileopen.png" ), tr( "&Open" ), this, SLOT( load() ), Qt::CTRL + Qt::Key_O );
    file->addAction( tr( "&Find on Internet" ), this, SLOT( MakeNetDialog() ), Qt::CTRL + Qt::Key_F );
    file->addAction( saveIcon, tr( "&Save" ), this, SLOT( save() ), Qt::CTRL + Qt::Key_S );
    file->addAction( tr( "Save &as..." ), this, SLOT( saveAs() ) );
    file->addAction( tr( "Save picture..." ), this, SLOT( savePicture() ) );

    file->addSeparator();

    file->addAction( tr( "Send to &external program..." ), this, SLOT( externalProgram() ) );

    file->addSeparator();

    file->addAction( tr( "Pa&ge setup" ), this, SLOT( pageSetup() ) );
    file->addAction( printIcon, tr( "&Print" ), this, SLOT( print() ), Qt::CTRL + Qt::Key_P );

    file->addSeparator();

    file->addAction( tr( "Close" ), this, SLOT( close() ), Qt::CTRL + Qt::Key_W );
    file->addAction( tr( "Quit" ), qApp, SLOT( closeAllWindows() ), Qt::CTRL + Qt::Key_Q );

    /**
     * edit menu
     */
    edit = menuBar()->addMenu( tr( "&Edit" ) );

    edit->addAction( tr( "&Undo" ), this, SLOT( Undo() ), Qt::CTRL + Qt::Key_Z );
    insertSymbolAction = edit->addAction( tr( "Insert s&ymbol" ), this, SLOT( InsertSymbol() ) );
    insertSymbolAction->setVisible( false );

    edit->addSeparator();

    edit->addAction( tr( "Cu&t" ), this, SLOT( Cut() ), Qt::CTRL + Qt::Key_X );
    edit->addAction( tr( "&Copy" ), this, SLOT( Copy() ), Qt::CTRL + Qt::Key_C );
    edit->addAction( tr( "&Paste" ), this, SLOT( Paste() ), Qt::CTRL + Qt::Key_V );
    edit->addAction( tr( "Clear" ), this, SLOT( Clear() ), Qt::Key_Delete );

    edit->addSeparator();

    edit->addAction( tr( "Select &All" ), this, SLOT( SelectAll() ), Qt::CTRL + Qt::Key_A );
    edit->addAction( tr( "&Deselect All" ), this, SLOT( DeselectAll() ), Qt::CTRL + Qt::SHIFT + Qt::Key_A );

    QMenu *rotateSub = new QMenu( tr( "&Rotate" ), this );

    rotateSub->addAction( tr( "Rotate 90 degrees clockwise" ), this, SLOT( Rotate90() ) );
    rotateSub->addAction( tr( "Rotate 180 degrees" ), this, SLOT( Rotate180() ) );
    rotateSub->addAction( tr( "Rotate 90 degrees counterclockwise" ), this, SLOT( Rotate270() ) );

    QMenu *flipSub = new QMenu( tr( "&Flip" ), this );

    flipSub->addAction( tr( "Flip &horizontal" ), this, SLOT( FlipH() ) );
    flipSub->addAction( tr( "Flip &vertical" ), this, SLOT( FlipV() ) );

    QMenu *zoomSub = new QMenu( tr( "&Zoom" ), this );

    zoomSub->addAction( tr( "Normal (100%)" ), this, SLOT( Magnify100() ), Qt::CTRL + Qt::Key_5 );
    zoomSub->addAction( tr( "Zoom out" ), this, SLOT( MagnifyMinus() ), Qt::CTRL + Qt::Key_1 );
    zoomSub->addAction( tr( "Zoom in" ), this, SLOT( MagnifyPlus() ), Qt::CTRL + Qt::Key_0 );

    edit->addMenu( rotateSub );
    edit->addMenu( flipSub );

    edit->addSeparator();

    edit->addMenu( zoomSub );

    /**
     * group menu
     */
    QMenu *groupmenu = menuBar()->addMenu( tr( "&Group" ) );

    groupmenu->addAction( tr( "Select &Reactant" ), this, SLOT( setGroup_Reactant() ) );
    groupmenu->addAction( tr( "Select &Product" ), this, SLOT( setGroup_Product() ) );
    groupmenu->addAction( tr( "Clear &group" ), this, SLOT( clearGroup() ) );
    groupmenu->addAction( tr( "Clear all &groups" ), this, SLOT( clearAllGroups() ) );

    /**
     * format menu
     */
    format = menuBar()->addMenu( tr( "Forma&t" ) );

    fixedBondAction = format->addAction( tr( "&Bond - Fixed length and angle" ), this, SLOT( setFixed_bond() ) );
    fixedBondAction->setCheckable( true );
    fixedBondAction->setChecked( preferences.getBond_fixed() );

    fixedArrowAction = format->addAction( tr( "&Arrow - Fixed length and angle" ), this, SLOT( setFixed_arrow() ) );
    fixedArrowAction->setCheckable( true );
    fixedArrowAction->setChecked( preferences.getArrow_fixed() );

    format->addSeparator();

    format->addAction( tr( "Set background &color" ), this, SLOT( BackgroundColor() ) );
    format->addAction( tr( "Toggle &grid" ), this, SLOT( toggleGrid() ), Qt::CTRL + Qt::Key_G );

    format->addSeparator();

    format->addAction( tr( "&Drawing settings..." ), this, SLOT( ShowFixedDialog() ) );
    format->addAction( tr( "&XDC settings..." ), this, SLOT( XDCSettings() ) );

    /**
     * tools menu
     */
    QMenu *tools = menuBar()->addMenu( tr( "T&ools" ) );

    tools->addAction( tr( "Clean up molecule" ), this, SLOT( CleanUpMolecule() ) );
    tools->addAction( tr( "Auto &layout" ), this, SLOT( AutoLayout() ), Qt::CTRL + Qt::Key_L );
    tools->addAction( tr( "Create custom ring" ), this, SLOT( saveCustomRing() ) );

    tools->addSeparator();

    tools->addAction( tr( "Molecule information..." ), this, SLOT( MoleculeInfo() ) );
    tools->addAction( tr( "Predict 1H NMR" ), this, SLOT( Calc1HNMR() ) );
    tools->addAction( tr( "Predict 13C NMR" ), this, SLOT( Calc13CNMR() ) );
    tools->addAction( tr( "Predict IR" ), this, SLOT( CalcIR() ) );
    tools->addAction( tr( "Predict pKa" ), this, SLOT( CalcpKa() ) );
    tools->addAction( tr( "Predict octanol-water partition (Kow)" ), this, SLOT( CalcKOW() ) );

    QMenu *reactionSub = new QMenu( tr( "Reaction" ), this );

    reactionSub->addAction( tr( "Estimate gas-phase enthalphy change" ), this, SLOT( reactionAnalysisEnthalpy() ) );
    reactionSub->addAction( tr( "Compare 1H NMR" ), this, SLOT( reactionAnalysis1HNMR() ) );
    reactionSub->addAction( tr( "Compare 13C NMR" ), this, SLOT( reactionAnalysis13CNMR() ) );

    reactionSub->addSeparator();

    reactionSub->addAction( tr( "Reverse reactions" ), this, SLOT( reactivityRetro() ) );
    reactionSub->addAction( tr( "Get bond identifier" ), this, SLOT( RetroBondName() ) );

    tools->addMenu( reactionSub );

    tools->addSeparator();

    tools->addAction( tr( "Input SMILES" ), this, SLOT( FromSMILES() ) );
    tools->addAction( tr( "Output SMILES" ), this, SLOT( ToSMILES() ) );
    tools->addAction( tr( "Output InChI" ), this, SLOT( ToInChI() ) );

    tools->addSeparator();

    tools->addAction( tr( "Build 3D model of molecule" ), this, SLOT( To3D() ) );

    /**
     * help menu
     */
    QMenu *help = menuBar()->addMenu( tr( "&Help" ) );

    help->addAction( tr( "&Manual" ), this, SLOT( NewManual() ), Qt::Key_F1 );
    help->addAction( tr( "&Did You Know?" ), this, SLOT( showDYK() ) );
    help->addAction( tr( "&About" ), this, SLOT( about() ) );
    help->addAction( tr( "&Support" ), this, SLOT( support() ) );
    help->addAction( tr( "&References" ), this, SLOT( Refs() ) );

    help->addSeparator();

    help->addAction( tr( "What's &This" ), this, SLOT( whatsThis() ), Qt::SHIFT + Qt::Key_F1 );

    /**
     * create data system
     */
    m_chemData = new ChemData;
    m_chemData->setClipboard( 0 );

    /// connect (non-Qt) data center and render widget
    m_renderer->setChemData( m_chemData );
    m_chemData->setRender2D( m_renderer );
    connect( m_renderer, SIGNAL( XDCEventSignal( XDC_Event * ) ), m_chemData, SLOT( XDCEventHandler( XDC_Event * ) ) );
    // connect m_renderer to application window
    connect( m_renderer, SIGNAL( textOn( QFont ) ), SLOT( showTextButtons( QFont ) ) );
    connect( m_renderer, SIGNAL( TextOff() ), SLOT( HideTextButtons() ) );
    connect( m_renderer, SIGNAL( SignalSetStatusBar( QString ) ), SLOT( SetStatusBar( QString ) ) );
    connect( m_chemData, SIGNAL( SignalSetStatusBar( QString ) ), SLOT( SetStatusBar( QString ) ) );
    connect( m_renderer, SIGNAL( SignalHelpTopic( QString ) ), SLOT( HelpTopic( QString ) ) );
    connect( m_chemData, SIGNAL( SignalHelpTopic( QString ) ), SLOT( HelpTopic( QString ) ) );
    connect( m_chemData, SIGNAL( SignalUpdateCustomRingMenu() ), SLOT( updateCustomRingMenu() ) );

    statusBar()->showMessage( "Ready" );
    resize( 640, 640 );
}

// externalProgram() sends to an external program
void ApplicationWindow::externalProgram()
{
    // supported programs
#define PROGRAM_KRYOMOL 1
#define PROGRAM_GHEMICAL 2

    int sendto = 0, qbox, passfail = 0;
    QString exportFileName;

    qbox = QMessageBox::question( this, tr( "Send to external program" ), tr( "This function will open a 3-D version of one molecule\nin an external modelling program.\n\nSend to program:" ), tr( "Ghemical" ), tr( "KryoMol" ), tr( "Cancel" ), 2 );
    if ( qbox == 0 )
        sendto = 1;
    if ( qbox == 1 )
        sendto = 2;
    if ( qbox == 2 )
        sendto = 0;

    if ( sendto > 0 ) {
        exportFileName = QDateTime::currentDateTime().toString( "MMddhhmmsszzz" );
        exportFileName.append( ".mol" );
        exportFileName.prepend( "/tmp/" );
        qDebug() << "export " << exportFileName;
        m_chemData->firstMolecule()->Make3DVersion( exportFileName );
        if ( QFile::exists( exportFileName ) == false ) {
            QMessageBox::critical( this, tr( "Send to external program failed" ), tr( "Could not save file for external program." )
                 );
            return;
        }
        if ( sendto == 1 ) {
            qDebug() << "Ghemical: ";
            exportFileName.prepend( "ghemical " );
            passfail = system( exportFileName.toAscii() );
            qDebug() << passfail;
        }
        if ( sendto == 2 ) {
            qDebug() << "KryoMol: ";
            exportFileName.prepend( "kryomol " );
            passfail = system( exportFileName.toAscii() );
            qDebug() << passfail;
        }
        if ( passfail != 0 ) {
            QMessageBox::critical( this, tr( "Send to external program failed" ), tr( "Could not execute the external program.\nPlease verify that it is installed correctly." )
                 );
            return;
        }
    }
}

QMenu *ApplicationWindow::BuildSymbolMenu()
{
    QMenu *symbolSub = new QMenu( this );

    symbolPlusAction = symbolSub->addAction( QIcon( RingDir + "sym_plus.png" ), tr( "Plus sign" ) );
    symbolMinusAction = symbolSub->addAction( QIcon( RingDir + "sym_minus.png" ), tr( "Minus sign" ) );
    symbolDeltaPlusAction = symbolSub->addAction( QIcon( RingDir + "sym_delta_plus.png" ), tr( "Delta plus" ) );
    symbolDeltaMinusAction = symbolSub->addAction( QIcon( RingDir + "sym_minus.png" ), tr( "Delta minus" ) );
    symbol1EAction = symbolSub->addAction( QIcon( RingDir + "sym_1e.png" ), tr( "1e symbol" ) );
    symbol2EAction = symbolSub->addAction( QIcon( RingDir + "sym_2e.png" ), tr( "2e symbol" ) );
    symbol2ELineAction = symbolSub->addAction( QIcon( RingDir + "sym_2e_line.png" ), tr( "2e line" ) );
    symbolRingUpAction = symbolSub->addAction( QIcon( RingDir + "sym_ring_up.png" ), tr( "Ring up" ) );
    symbolPOrbitalAction = symbolSub->addAction( QIcon( RingDir + "sym_p_orbital.png" ), tr( "Orbital" ) );
    symbolPDoubleAction = symbolSub->addAction( QIcon( RingDir + "sym_p_double.png" ), tr( "Double orbital" ) );
    symbolBeadAction = symbolSub->addAction( QIcon( RingDir + "sym_bead.png" ), tr( "Bead" ) );
    symbolAntibodyAction = symbolSub->addAction( QIcon( RingDir + "sym_antibody.png" ), tr( "Antibody" ) );
    symbolNewmanAction = symbolSub->addAction( QIcon( RingDir + "sym_newman.png" ), tr( "Newman projection symbol" ) );
    symbolNewmanAntiAction = symbolSub->addAction( QIcon( RingDir + "sym_newman_anti.png" ), tr( "Newman projection - staggered (anti)" ) );
    symbolNewmanEclipseAction = symbolSub->addAction( QIcon( RingDir + "sym_newman_eclipse.png" ), tr( "Newman projection - eclipsed" ) );

    return symbolSub;
}

QMenu *ApplicationWindow::BuildArrowMenu()
{
    QMenu *arrowSub = new QMenu( this );

    regularArrowAction = arrowSub->addAction( QIcon( arrow_regular_xpm ), tr( "Regular Arrow" ) );
    topharpoonArrowAction = arrowSub->addAction( QIcon( arrow_topharpoon_xpm ), tr( "Top harpoon" ) );
    bottomharpoonAction = arrowSub->addAction( QIcon( arrow_bottomharpoon_xpm ), tr( "Bottom harpoon" ) );
    middleArrowAction = arrowSub->addAction( QIcon( arrow_middle_xpm ), tr( "Middle" ) );
    didntworkArrowAction = arrowSub->addAction( QIcon( arrow_didnt_work_xpm ), tr( "Did not work" ) );
    dashedArrowAction = arrowSub->addAction( QIcon( arrow_dash_xpm ), tr( "Dashed arrow" ) );
    bi1ArrowAction = arrowSub->addAction( QIcon( arrow_bi1_xpm ), tr( "BI1 arrow" ) );
    bi2ArrowAction = arrowSub->addAction( QIcon( arrow_bi2_xpm ), tr( "BI2 arrow" ) );
    retroArrowAction = arrowSub->addAction( QIcon( arrow_retro_xpm ), tr( "Retro arrow" ) );

    return arrowSub;
}

QMenu *ApplicationWindow::BuildCurveArrowMenu()
{
    QMenu *arrowSub = new QMenu( this );

    carrowCW90Action = arrowSub->addAction( QIcon( cw90_xpm ), tr( "90° cw" ) );
    carrowCCW90Action = arrowSub->addAction( QIcon( ccw90_xpm ), tr( "90° ccw" ) );
    carrowCW180Action = arrowSub->addAction( QIcon( cw180_xpm ), tr( "180° cw" ) );
    carrowCCW180Action = arrowSub->addAction( QIcon( ccw180_xpm ), tr( "180° ccw" ) );
    carrowCW270Action = arrowSub->addAction( QIcon( cw270_xpm ), tr( "270° cw" ) );
    carrowCCW270Action = arrowSub->addAction( QIcon( ccw270_xpm ), tr( "270° ccw" ) );
    carrowBezierAction = arrowSub->addAction( QIcon( cbezier_xpm ), tr( "Cubic bezier" ) );
    carrowBezierhalfAction = arrowSub->addAction( QIcon( cbezierhalf_xpm ), tr( "Cubic bezier - half arrow" ) );
    carrowBezierfullAction = arrowSub->addAction( QIcon( cbezierfull_xpm ), tr( "Cubic bezier - full arrow" ) );

    return arrowSub;
}

QMenu *ApplicationWindow::BuildBracketMenu()
{
    QMenu *bracketSub = new QMenu( this );

    squareBracketAction = bracketSub->addAction( QIcon( squarebracket_xpm ), tr( "Square bracket" ) );
    curveBracketAction = bracketSub->addAction( QIcon( curvebracket_xpm ), tr( "Curve bracket" ) );
    braceBracketAction = bracketSub->addAction( QIcon( bracebracket_xpm ), tr( "Brace bracket" ) );
    boxBracketAction = bracketSub->addAction( QIcon( boxbracket_xpm ), tr( "Box bracket" ) );
    ellipseBracketAction = bracketSub->addAction( QIcon( ellipsebracket_xpm ), tr( "Ellipse bracket" ) );
    closedsquareBracketAction = bracketSub->addAction( QIcon( closedsquarebracket_xpm ), tr( "Closed square bracket" ) );
    circleBracketAction = bracketSub->addAction( QIcon( circlebracket_xpm ), tr( "Circle bracket" ) );

    return bracketSub;
}

// loads custom rings.
void ApplicationWindow::FromRingMenu( int x )
{
    m_chemData->StartUndo( 0, 0 );
    m_chemData->DeselectAll();
///TODO  m_chemData->SetTopLeft(sa->viewportToContents(QPoint(0,0)));
    QString fname( ringlist[x] );

    fname.replace( fname.length() - 3, 3, QString( "cml" ) );
    m_chemData->load( preferences.getCustomRingDir() + fname );
    m_renderer->Inserted();
}

void ApplicationWindow::updateCustomRingMenu()
{
    ringmenu->removeAction( customRingMenuAction );
    customRingMenuAction = ringmenu->addMenu( BuildCustomRingMenu() );
}

void ApplicationWindow::FromRingToolbar( QString fi )
{
  qDebug() << QString("FromRingToolbar:") << fi;
    m_chemData->DeselectAll();
    if ( fi.contains( "6ring" ) > 0 ) {
        m_renderer->setMode_DrawRing( RingDir + fi, fi.left( fi.length() - 4 ) );
        return;
    }
    m_renderer->setMode_DrawRing( RingDir + fi, fi.left( fi.length() - 4 ), 1 );
}

void ApplicationWindow::FromSymbolMenu( QAction *action )
{
    if ( action == symbolPlusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_plus" );
    } else if ( action == symbolMinusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_minus" );
    } else if ( action == symbolDeltaPlusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_delta_plus" );
    } else if ( action == symbolDeltaMinusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_delta_minus" );
    } else if ( action == symbol1EAction ) {
        m_renderer->setMode_DrawSymbol( "sym_1e" );
    } else if ( action == symbol2EAction ) {
        m_renderer->setMode_DrawSymbol( "sym_2e" );
    } else if ( action == symbol2ELineAction ) {
        m_renderer->setMode_DrawSymbol( "sym_2e_line" );
    } else if ( action == symbolRingUpAction ) {
        m_renderer->setMode_DrawSymbol( "sym_ring_up" );
    } else if ( action == symbolPOrbitalAction ) {
        m_renderer->setMode_DrawSymbol( "p_orbital" );
    } else if ( action == symbolPDoubleAction ) {
        m_renderer->setMode_DrawSymbol( "p_double_orbital" );
    } else if ( action == symbolBeadAction ) {
        m_renderer->setMode_DrawSymbol( "bead" );
    } else if ( action == symbolAntibodyAction ) {
        m_renderer->setMode_DrawSymbol( "antibody" );
    } else if ( action == symbolNewmanAction ) {
        m_renderer->setMode_DrawSymbol( "newman" );
    } else if ( action == symbolNewmanAntiAction ) {
        m_renderer->setMode_DrawNewmanAnti();
    } else if ( action == symbolNewmanEclipseAction ) {
        m_renderer->setMode_DrawNewmanGauche();
    } else {
        qDebug() << "unknown action";
    }
}

void ApplicationWindow::setSymbolAction( QAction *action )
{
    if ( action == symbolPlusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_plus" );
        drawSymbolButton->setDefaultAction( symbolPlusAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_plus.png" ) );
    } else if ( action == symbolMinusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_minus" );
        drawSymbolButton->setDefaultAction( symbolMinusAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_minus.png" ) );
    } else if ( action == symbolDeltaPlusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_delta_plus" );
        drawSymbolButton->setDefaultAction( symbolDeltaPlusAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_delta_plus.png" ) );
    } else if ( action == symbolDeltaMinusAction ) {
        m_renderer->setMode_DrawSymbol( "sym_delta_minus" );
        drawSymbolButton->setDefaultAction( symbolDeltaMinusAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_delta_minus.png" ) );
    } else if ( action == symbol1EAction ) {
        m_renderer->setMode_DrawSymbol( "sym_1e" );
        drawSymbolButton->setDefaultAction( symbol1EAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_1e.png" ) );
    } else if ( action == symbol2EAction ) {
        m_renderer->setMode_DrawSymbol( "sym_2e" );
        drawSymbolButton->setDefaultAction( symbol2EAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_2e.png" ) );
    } else if ( action == symbol2ELineAction ) {
        m_renderer->setMode_DrawSymbol( "sym_2e_line" );
        drawSymbolButton->setDefaultAction( symbol2ELineAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_2e_line.png" ) );
    } else if ( action == symbolRingUpAction ) {
        m_renderer->setMode_DrawSymbol( "sym_ring_up" );
        drawSymbolButton->setDefaultAction( symbolRingUpAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_ring_up.png" ) );
    } else if ( action == symbolPOrbitalAction ) {
        m_renderer->setMode_DrawSymbol( "p_orbital" );
        drawSymbolButton->setDefaultAction( symbolPOrbitalAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_p_orbital.png" ) );
    } else if ( action == symbolPDoubleAction ) {
        m_renderer->setMode_DrawSymbol( "p_double_orbital" );
        drawSymbolButton->setDefaultAction( symbolPDoubleAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_p_double.png" ) );
    } else if ( action == symbolBeadAction ) {
        m_renderer->setMode_DrawSymbol( "bead" );
        drawSymbolButton->setDefaultAction( symbolBeadAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_bead.png" ) );
    } else if ( action == symbolAntibodyAction ) {
        m_renderer->setMode_DrawSymbol( "antibody" );
        drawSymbolButton->setDefaultAction( symbolAntibodyAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_antibody.png" ) );
    } else if ( action == symbolNewmanAction ) {
        m_renderer->setMode_DrawSymbol( "newman" );
        drawSymbolButton->setDefaultAction( symbolNewmanAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_newman.png" ) );
    } else if ( action == symbolNewmanAntiAction ) {
        m_renderer->setMode_DrawNewmanAnti();
        drawSymbolButton->setDefaultAction( symbolNewmanAntiAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_newman_anti.png" ) );
    } else if ( action == symbolNewmanEclipseAction ) {
        m_renderer->setMode_DrawNewmanGauche();
        drawSymbolButton->setDefaultAction( symbolNewmanEclipseAction );
        drawSymbolButton->setIcon( QIcon( RingDir + "sym_newman_eclipse.png" ) );
    } else {
        qDebug() << "unknown action";
    }
}

void ApplicationWindow::FromArrowMenu( QAction *action )
{
    if ( action == regularArrowAction ) {
        m_renderer->setMode_DrawArrow( regularArrow );
    } else if ( action == topharpoonArrowAction ) {
        m_renderer->setMode_DrawArrow( topharpoonArrow );
    } else if ( action == bottomharpoonAction ) {
        m_renderer->setMode_DrawArrow( bottomharpoonArrow );
    } else if ( action == middleArrowAction ) {
        m_renderer->setMode_DrawArrow( middleArrow );
    } else if ( action == didntworkArrowAction ) {
        m_renderer->setMode_DrawArrow( didntworkArrow );
    } else if ( action == dashedArrowAction ) {
        m_renderer->setMode_DrawArrow( dashedArrow );
    } else if ( action == bi1ArrowAction ) {
        m_renderer->setMode_DrawArrow( bi1Arrow );
    } else if ( action == bi2ArrowAction ) {
        m_renderer->setMode_DrawArrow( bi2Arrow );
    } else if ( action == retroArrowAction ) {
        m_renderer->setMode_DrawArrow( retroArrow );
    } else {
        qDebug() << "unknown action";
    }
}

void ApplicationWindow::setRegularArrowAction( QAction * action )
{
    if ( action == regularArrowAction ) {
        m_renderer->setMode_DrawArrow( regularArrow );
        drawArrowButton->setDefaultAction( regularArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_regular_xpm ) );
    } else if ( action == topharpoonArrowAction ) {
        m_renderer->setMode_DrawArrow( topharpoonArrow );
        drawArrowButton->setDefaultAction( topharpoonArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_topharpoon_xpm ) );
    } else if ( action == bottomharpoonAction ) {
        m_renderer->setMode_DrawArrow( bottomharpoonArrow );
        drawArrowButton->setDefaultAction( bottomharpoonAction );
        drawArrowButton->setIcon( QIcon( arrow_bottomharpoon_xpm ) );
    } else if ( action == middleArrowAction ) {
        m_renderer->setMode_DrawArrow( middleArrow );
        drawArrowButton->setDefaultAction( middleArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_middle_xpm ) );
    } else if ( action == didntworkArrowAction ) {
        m_renderer->setMode_DrawArrow( didntworkArrow );
        drawArrowButton->setDefaultAction( didntworkArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_didnt_work_xpm ) );
    } else if ( action == dashedArrowAction ) {
        m_renderer->setMode_DrawArrow( dashedArrow );
        drawArrowButton->setDefaultAction( dashedArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_dash_xpm ) );
    } else if ( action == bi1ArrowAction ) {
        m_renderer->setMode_DrawArrow( bi1Arrow );
        drawArrowButton->setDefaultAction( bi1ArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_bi1_xpm ) );
    } else if ( action == bi2ArrowAction ) {
        m_renderer->setMode_DrawArrow( bi2Arrow );
        drawArrowButton->setDefaultAction( bi2ArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_bi2_xpm ) );
    } else if ( action == retroArrowAction ) {
        m_renderer->setMode_DrawArrow( retroArrow );
        drawArrowButton->setDefaultAction( retroArrowAction );
        drawArrowButton->setIcon( QIcon( arrow_retro_xpm ) );
    } else {
        qDebug() << "unknown action";
    }
}

void ApplicationWindow::FromCurveArrowMenu( QAction * action )
{
    if ( action == carrowCW90Action ) {
        m_renderer->setMode_DrawCurveArrow( cw90Arrow );
    } else if ( action == carrowCCW90Action ) {
        m_renderer->setMode_DrawCurveArrow( ccw90Arrow );
    } else if ( action == carrowCW180Action ) {
        m_renderer->setMode_DrawCurveArrow( cw180Arrow );
    } else if ( action == carrowCCW180Action ) {
        m_renderer->setMode_DrawCurveArrow( ccw180Arrow );
    } else if ( action == carrowCW270Action ) {
        m_renderer->setMode_DrawCurveArrow( cw270Arrow );
    } else if ( action == carrowCCW270Action ) {
        m_renderer->setMode_DrawCurveArrow( ccw270Arrow );
    } else if ( action == carrowBezierAction ) {
        m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, 0 );
    } else if ( action == carrowBezierhalfAction ) {
        m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, 1 );
    } else if ( action == carrowBezierfullAction ) {
        m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, 2 );
    } else {
        qDebug() << "unknown action";
    }
}

void ApplicationWindow::setCurveArrowAction( QAction * action )
{
    if ( action == carrowCW90Action ) {
        m_renderer->setMode_DrawCurveArrow( cw90Arrow );
        drawCurveArrowButton->setDefaultAction( carrowCW90Action );
        drawCurveArrowButton->setIcon( QIcon( cw90_xpm ) );
    } else if ( action == carrowCCW90Action ) {
        m_renderer->setMode_DrawCurveArrow( ccw90Arrow );
        drawCurveArrowButton->setDefaultAction( carrowCCW90Action );
        drawCurveArrowButton->setIcon( QIcon( ccw90_xpm ) );
    } else if ( action == carrowCW180Action ) {
        m_renderer->setMode_DrawCurveArrow( cw180Arrow );
        drawCurveArrowButton->setDefaultAction( carrowCW180Action );
        drawCurveArrowButton->setIcon( QIcon( cw180_xpm ) );
    } else if ( action == carrowCCW180Action ) {
        m_renderer->setMode_DrawCurveArrow( ccw180Arrow );
        drawCurveArrowButton->setDefaultAction( carrowCCW180Action );
        drawCurveArrowButton->setIcon( QIcon( ccw180_xpm ) );
    } else if ( action == carrowCW270Action ) {
        m_renderer->setMode_DrawCurveArrow( cw270Arrow );
        drawCurveArrowButton->setDefaultAction( carrowCW270Action );
        drawCurveArrowButton->setIcon( QIcon( cw270_xpm ) );
    } else if ( action == carrowCCW270Action ) {
        m_renderer->setMode_DrawCurveArrow( ccw270Arrow );
        drawCurveArrowButton->setDefaultAction( carrowCCW270Action );
        drawCurveArrowButton->setIcon( QIcon( ccw270_xpm ) );
    } else if ( action == carrowBezierAction ) {
        m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, 0 );
        drawCurveArrowButton->setDefaultAction( carrowBezierAction );
        drawCurveArrowButton->setIcon( QIcon( cbezier_xpm ) );
    } else if ( action == carrowBezierhalfAction ) {
        m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, 1 );
        drawCurveArrowButton->setDefaultAction( carrowBezierhalfAction );
        drawCurveArrowButton->setIcon( QIcon( cbezierhalf_xpm ) );
    } else if ( action == carrowBezierfullAction ) {
        m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, 2 );
        drawCurveArrowButton->setDefaultAction( carrowBezierfullAction );
        drawCurveArrowButton->setIcon( QIcon( cbezierfull_xpm ) );
    } else {
        qDebug() << "unknown action";
    }
}

void ApplicationWindow::FromBracketMenu( QAction * action )
{
    if ( action == squareBracketAction ) {
        m_renderer->setMode_DrawBracket( squareBracket );
    } else if ( action == curveBracketAction ) {
        m_renderer->setMode_DrawBracket( curveBracket );
    } else if ( action == braceBracketAction ) {
        m_renderer->setMode_DrawBracket( braceBracket );
    } else if ( action == boxBracketAction ) {
        m_renderer->setMode_DrawBracket( boxBracket );
    } else if ( action == ellipseBracketAction ) {
        m_renderer->setMode_DrawBracket( ellipseBracket );
    } else if ( action == closedsquareBracketAction ) {
        m_renderer->setMode_DrawBracket( closedsquareBracket );
    } else if ( action == circleBracketAction ) {
        m_renderer->setMode_DrawBracket( circleBracket );
    } else {
        qDebug() << "unknown Action";
    }
}

void ApplicationWindow::setBracketAction( QAction * action )
{
    if ( action == squareBracketAction ) {
        m_renderer->setMode_DrawBracket( squareBracket );
        drawBracketButton->setDefaultAction( squareBracketAction );
        drawBracketButton->setIcon( QIcon( squarebracket_xpm ) );
    } else if ( action == curveBracketAction ) {
        m_renderer->setMode_DrawBracket( curveBracket );
        drawBracketButton->setDefaultAction( curveBracketAction );
        drawBracketButton->setIcon( QIcon( curvebracket_xpm ) );
    } else if ( action == braceBracketAction ) {
        m_renderer->setMode_DrawBracket( braceBracket );
        drawBracketButton->setDefaultAction( braceBracketAction );
        drawBracketButton->setIcon( QIcon( bracebracket_xpm ) );
    } else if ( action == boxBracketAction ) {
        m_renderer->setMode_DrawBracket( boxBracket );
        drawBracketButton->setDefaultAction( boxBracketAction );
        drawBracketButton->setIcon( QIcon( boxbracket_xpm ) );
    } else if ( action == ellipseBracketAction ) {
        m_renderer->setMode_DrawBracket( ellipseBracket );
        drawBracketButton->setDefaultAction( ellipseBracketAction );
        drawBracketButton->setIcon( QIcon( ellipsebracket_xpm ) );
    } else if ( action == closedsquareBracketAction ) {
        m_renderer->setMode_DrawBracket( closedsquareBracket );
        drawBracketButton->setDefaultAction( closedsquareBracketAction );
        drawBracketButton->setIcon( QIcon( closedsquarebracket_xpm ) );
    } else if ( action == circleBracketAction ) {
        m_renderer->setMode_DrawBracket( circleBracket );
        drawBracketButton->setDefaultAction( circleBracketAction );
        drawBracketButton->setIcon( QIcon( circlebracket_xpm ) );
    } else {
        qDebug() << "unknown Action";
    }
}

void ApplicationWindow::setFixed_arrow()
{
    if ( fixedArrowAction->isChecked() ) {
        fixedArrowAction->setChecked( false );
        preferences.setArrow_fixed( false );
    } else {
        fixedArrowAction->setChecked( true );
        preferences.setArrow_fixed( true );
    }
}

void ApplicationWindow::setFixed_bond()
{
    if ( fixedBondAction->isChecked() ) {
        fixedBondAction->setChecked( false );
        preferences.setBond_fixed( false );
    } else {
        fixedBondAction->setChecked( true );
        preferences.setBond_fixed( true );
    }
}

/*
void ApplicationWindow::setFix_Hydrogens()
{
    if ( format->isItemChecked( fix_hydrogens ) ) {
        format->setItemChecked( fix_hydrogens, false );
        preferences.setFixHydrogens( false );
    } else {
        format->setItemChecked( fix_hydrogens, true );
        preferences.setFixHydrogens( true );
    }
}
*/

ApplicationWindow::~ApplicationWindow()
{
    delete printer;
}

/*
void ApplicationWindow::setRenderFont()
{
    // read new font setting from combo boxes and pass to r
    QFont f( fontList->currentText(), fontSizeList->currentText().toInt() );

    if ( fontList->currentText() == "Courier" )
        f.setStyleHint( QFont::Courier );
    if ( fontList->currentText() == "Helvetica" )
        f.setStyleHint( QFont::Helvetica );
    if ( fontList->currentText() == "Times" )
        f.setStyleHint( QFont::Times );
    qDebug() << f.family() << f.pointSize();
    m_renderer->SetFont( f );
}
*/

void ApplicationWindow::SetColor( int m )
{
    if ( m == 0 )
        m_renderer->SetColor( QColor( 0, 0, 0 ) );
    if ( m == 1 )
        m_renderer->SetColor( QColor( 127, 0, 0 ) );
    if ( m == 2 )
        m_renderer->SetColor( QColor( 0, 127, 0 ) );
    if ( m == 3 )
        m_renderer->SetColor( QColor( 0, 0, 127 ) );
}

void ApplicationWindow::SetThick( int t )
{
    m_renderer->SetThick( t + 1 );
}

void ApplicationWindow::showTextButtons( QFont infont )
{
    insertSymbolAction->setVisible( true );
    justifyLeftAction->setVisible( true );
    justifyCenterAction->setVisible( true );
    justifyRightAction->setVisible( true );
    boldAction->setVisible( true );
    italicAction->setVisible( true );
    underlineAction->setVisible( true );
    superscriptAction->setVisible( true );
    subscriptAction->setVisible( true );
    // set font combo boxes to object font (passed as infont)
    if ( infont.family().toLower() == QString( "courier" ) )
        fontList->setCurrentIndex( 0 );
    if ( infont.family().toLower() == QString( "helvetica" ) )
        fontList->setCurrentIndex( 1 );
    if ( infont.family().toLower() == QString( "times" ) )
        fontList->setCurrentIndex( 2 );
    if ( infont.pointSize() == 8 )
        fontSizeList->setCurrentIndex( 0 );
    if ( infont.pointSize() == 10 )
        fontSizeList->setCurrentIndex( 1 );   if ( infont.family().toLower() == QString( "courier" ) )
        fontList->setCurrentIndex( 0 );
    if ( infont.family().toLower() == QString( "helvetica" ) )
        fontList->setCurrentIndex( 1 );
    if ( infont.family().toLower() == QString( "times" ) )
        fontList->setCurrentIndex( 2 );
    if ( infont.pointSize() == 8 )
        fontSizeList->setCurrentIndex( 0 );
    if ( infont.pointSize() == 10 )
        fontSizeList->setCurrentIndex( 1 );
    if ( infont.pointSize() == 12 )
        fontSizeList->setCurrentIndex( 2 );
    if ( infont.pointSize() == 14 )
        fontSizeList->setCurrentIndex( 3 );
    if ( infont.pointSize() == 18 )
        fontSizeList->setCurrentIndex( 4 );
    if ( infont.pointSize() == 24 )
        fontSizeList->setCurrentIndex( 5 );
    if ( infont.pointSize() == 32 )
        fontSizeList->setCurrentIndex( 6 );
    if ( infont.pointSize() == 12 )
        fontSizeList->setCurrentIndex( 2 );
    if ( infont.pointSize() == 14 )
        fontSizeList->setCurrentIndex( 3 );
    if ( infont.pointSize() == 18 )
        fontSizeList->setCurrentIndex( 4 );
    if ( infont.pointSize() == 24 )
        fontSizeList->setCurrentIndex( 5 );
    if ( infont.pointSize() == 32 )
        fontSizeList->setCurrentIndex( 6 );
}

void ApplicationWindow::HideTextButtons()
{
    insertSymbolAction->setVisible( false );
    justifyLeftAction->setVisible( false );
    justifyCenterAction->setVisible( false );
    justifyRightAction->setVisible( false );
    boldAction->setVisible( false );
    italicAction->setVisible( false );
    underlineAction->setVisible( false );
    superscriptAction->setVisible( false );
    subscriptAction->setVisible( false );
    // set font combo boxes to object font (passed as infont)
    // set font combo boxes to current font
    QFont infont = m_renderer->GetFont();

    if ( infont.family().toLower() == QString( "courier" ) )
        fontList->setCurrentIndex( 0 );
    if ( infont.family().toLower() == QString( "helvetica" ) )
        fontList->setCurrentIndex( 1 );
    if ( infont.family().toLower() == QString( "times" ) )
        fontList->setCurrentIndex( 2 );
    if ( infont.pointSize() == 8 )
        fontSizeList->setCurrentIndex( 0 );
    if ( infont.pointSize() == 10 )
        fontSizeList->setCurrentIndex( 1 );
    if ( infont.pointSize() == 12 )
        fontSizeList->setCurrentIndex( 2 );
    if ( infont.pointSize() == 14 )
        fontSizeList->setCurrentIndex( 3 );
    if ( infont.pointSize() == 18 )
        fontSizeList->setCurrentIndex( 4 );
    if ( infont.pointSize() == 24 )
        fontSizeList->setCurrentIndex( 5 );
    if ( infont.pointSize() == 32 )
        fontSizeList->setCurrentIndex( 6 );
}

void ApplicationWindow::newDoc()
{
    ApplicationWindow *ed = new ApplicationWindow;

    ed->setClipboard( m_renderer->getClipboard() );
    ed->show();
    ed->HideTextButtons();
}

void ApplicationWindow::load()
{
    OBImport();
    return;

    /* DELETE ME
       QFileDialog fd(QString(), QString(), 0, 0, TRUE);
       fd.setWindowTitle("Load file");
       fd.setMode(QFileDialog::ExistingFile);
       QStringList filters;
       filters.append("All files (*)");
       filters.append("XDrawChem native (*.xdc)");
       filters.append("MDL Molfile (*.mol)");
       filters.append("CML - Chemical Markup Language (*.cml)");
       fd.setFilters(filters);
       if ( fd.exec() == QDialog::Accepted )
       load( fd.selectedFile() );
     */
}

void ApplicationWindow::load( QString fileName )
{
    OBNewLoad( fileName, "--Select a filter-- (*)" );
    return;

    //
    // fix me
    // to use OpenBabel
    //
    /*
       QString realFileName = fileName;

       QFile f(fileName);
       if ( !f.open(IO_ReadOnly) ) {
       QMessageBox::warning(0, tr("Couldn't open file"), tr("Could not open the file: ") + fileName);
       statusBar()->showMessage(tr("Unable to load ") + fileName);
       return;
       }
       f.close();
       SelectAll();
       Clear();

       if ( !m_chemData->load(fileName) ) {
       statusBar()->showMessage(tr("Unable to load ") + fileName);
       return;
       }

       fileName = realFileName;
       setWindowTitle( QString(XDC_VERSION) + QString(" - ") + fileName );
       statusBar()->showMessage( tr("Loaded document ") + fileName );
       filename = fileName;
       m_chemData->DeselectAll();
       m_renderer->update();
     */
}


void ApplicationWindow::save()
{
    if ( filename.isEmpty() ) {
        OBExport();
        return;
    }

    OBNewSave();

    return;

    if ( filename.isEmpty() ) {
        saveAs();
        return;
    }

    if ( !m_chemData->save( filename ) ) {
        statusBar()->showMessage( tr( "Could not write to " ) + filename );
        return;
    }

    setWindowTitle( QString( XDC_VERSION ) + QString( " - " ) + filename );

    statusBar()->showMessage( tr( "Saved file " ) + filename );
}


void ApplicationWindow::saveAs()
{
    OBExport();
    return;
}

void ApplicationWindow::savePicture()
{
    int pm = 0;
    bool was_saved = false;
    QStringList picFiles;
    QString selectedFile;

    MyFileDialog fd( 0 );

    fd.setWindowTitle( tr( "Save as picture..." ) );
    fd.setFileMode( QFileDialog::AnyFile );
    QStringList picfilters;

    picfilters.append( "Portable Network Graphic (*.png)" );
    picfilters.append( "Windows Bitmap (*.bmp)" );
    picfilters.append( "Encapsulated PostScript (*.eps)" );
    picfilters.append( "Scalable Vector Graphics (*.svg)" );
    fd.setFilters( picfilters );
    if ( fd.exec() == QDialog::Accepted ) {
        picFiles = fd.selectedFiles();
        if ( !picFiles.isEmpty() ) selectedFile = picFiles.at( 0 );
        QString tmpx = selectedFile.right( 4 ).toLower();

        if ( tmpx == QString( ".png" ) )
            pm = 1;
        if ( tmpx == QString( ".bmp" ) )
            pm = 2;
        if ( tmpx == QString( ".svg" ) )
            pm = 3;
        if ( ( tmpx != QString( ".png" ) ) && ( tmpx != QString( ".bmp" ) ) && ( tmpx != QString( ".svg" ) ) && ( tmpx != QString( ".eps" ) ) ) {
            if ( fd.selectedFilter().left( 3 ) == QString( "Por" ) ) {
                selectedFile.append( ".png" );
                pm = 1;
            }
            if ( fd.selectedFilter().left( 3 ) == QString( "Win" ) ) {
                selectedFile.append( ".bmp" );
                pm = 2;
            }
            if ( fd.selectedFilter().left( 3 ) == QString( "Enc" ) ) {
                selectedFile.append( ".eps" );
            }
            if ( fd.selectedFilter().left( 3 ) == QString( "Sca" ) ) {
                selectedFile.append( ".svg" );
                pm = 3;
            }
        }
        if ( pm == 3 ) {
            // save as Scalable Vector Graphics
            was_saved = m_renderer->SaveSVG( selectedFile );
            if ( was_saved )
                statusBar()->showMessage( tr( "Saved picture file " ) + selectedFile );
            else
                statusBar()->showMessage( tr( "Unable to save picture!" ) );
            return;
        }
        if ( pm != 0 ) {
            // check for "convert" (ImageMagick)
            bool imagemagick = false;
            QString n1, cmd1;

            n1.setNum( getpid() );
            n1.prepend( "/tmp/" );
            cmd1 = "convert > " + n1;
            system( cmd1.toAscii() );

//      QFile f1( n1 );
//      f1.open(QIODevice::ReadOnly);
//      f1.readLine(cmd1, 256);

            QFile f1( n1 );

            if ( !f1.open( QIODevice::ReadOnly | QIODevice::Text ) )
                return;

            QTextStream in( &f1 );

            cmd1 = in.readLine();

            if ( cmd1.contains( "ImageMagick" ) > 0 ) {
                qDebug() << "ImageMagick is present";
                imagemagick = true;
            }

            cmd1 = "rm " + n1;
            system( cmd1.toAscii() );

            QPixmap tosave;

            // use "tosave" with ImageMagick to specify size
            tosave = m_renderer->MakePixmap( fd.isTransparent() );
            QString sizehint;

            n1.setNum( tosave.width() );
            sizehint = n1;
            sizehint.append( "x" );
            n1.setNum( tosave.height() );
            sizehint.append( n1 );
            if ( imagemagick ) {
                n1 = selectedFile;
                n1.append( ".eps" );
                m_renderer->SaveEPS( n1 );
                //cmd1 = "convert -antialias " + n1 + " " + selectedFile;
                cmd1 = "convert -antialias -size " + sizehint + " " + n1 + " -resize " + sizehint + " " + selectedFile;
                system( cmd1.toAscii() );
                cmd1 = "rm " + n1;
                system( cmd1.toAscii() );
                was_saved = true;
            } else {
                tosave = m_renderer->MakePixmap( fd.isTransparent() );
                if ( pm == 1 )  // PNG
                    was_saved = tosave.save( selectedFile, "PNG" );
                if ( pm == 2 )  // BMP
                    was_saved = tosave.save( selectedFile, "BMP" );
            }
            if ( was_saved )
                statusBar()->showMessage( tr( "Saved picture file " ) + selectedFile );
            else
                statusBar()->showMessage( tr( "Unable to save picture!" ) );
        } else {                // save as EPS
            was_saved = m_renderer->SaveEPS( selectedFile );
            if ( was_saved )
                statusBar()->showMessage( tr( "Saved picture file " ) + selectedFile );
            else
                statusBar()->showMessage( tr( "Unable to save picture!" ) );
        }
    }
}

void ApplicationWindow::savePNG()
{
    bool was_saved;

    // check for "convert" (ImageMagick)
    bool imagemagick = false;
    QString n1, cmd1;

    n1.setNum( getpid() );
    n1.prepend( "/tmp/" );
    cmd1 = "convert > " + n1;
    system( cmd1.toAscii() );

//  QFile f1( n1 );
//  f1.open(QIODevice::ReadOnly);
//  f1.readLine(cmd1, 256);

    QFile f1( n1 );

    if ( !f1.open( QIODevice::ReadOnly | QIODevice::Text ) )
        return;

    QTextStream in( &f1 );

    cmd1 = in.readLine();

    if ( cmd1.contains( "ImageMagick" ) > 0 ) {
        qDebug() << "ImageMagick is present";
        imagemagick = true;
    }

    cmd1 = "rm " + n1;
    system( cmd1.toAscii() );

    QPixmap tosave;

    tosave = m_renderer->MakePixmap( ni_tflag );
    QString sizehint;

    n1.setNum( tosave.width() );
    sizehint = n1;
    sizehint.append( "x" );
    n1.setNum( tosave.height() );
    sizehint.append( n1 );
    if ( imagemagick ) {
        n1 = ni_savefile;
        n1.append( ".eps" );
        m_renderer->SaveEPS( n1 );
        cmd1 = "convert -antialias -size " + sizehint + " " + n1 + " -resize " + sizehint + " " + ni_savefile;
        system( cmd1.toAscii() );
        cmd1 = "rm " + n1;
        system( cmd1.toAscii() );
        was_saved = true;
    } else {
        was_saved = tosave.save( ni_savefile, "PNG" );
    }

    //QPixmap tosave = m_renderer->MakePixmap( ni_tflag );
    //was_saved = tosave.save(ni_savefile, "PNG");
    if ( was_saved == false )
        qDebug() << "save PNG failed";
    close();                  // this function is only used in non-interactive mode
}

void ApplicationWindow::save3D()
{
//    bool was_saved;

    m_chemData->Save3D( ni_savefile );
    close();                  // this function is only used in non-interactive mode
}

void ApplicationWindow::print()
{
    m_renderer->Print();
}

void ApplicationWindow::closeEvent( QCloseEvent * ce )
{
    if ( !m_chemData->edited() ) {
        ce->accept();
        return;
    }

    switch ( QMessageBox::information( this, "XDrawChem question", "The document has been changed since " "the last save.", "Save Now", "Cancel", "Leave Anyway", 0, 1 ) ) {
    case 0:
        save();
        ce->accept();
        break;
    case 1:
    default:                   // just for sanity
        ce->ignore();
        break;
    case 2:
        ce->accept();
        break;
    }
}


void ApplicationWindow::about()
{
    QMessageBox::about( this, XDC_VERSION,
                        QString( XDC_VERSION ) +
                        tr
                        ( "\nBryan Herger\nbherger@users.sourceforge.net\n\nPlease subscribe to the mailing list for information about future releases.\nSend a message to xdrawchem-announce-request@lists.sourceforge.net with 'subscribe' as the subject.\n\nXDrawChem is copyright (C) 2004 Bryan Herger.\nPortions copyright (C) 1997-2000 Dr. Christoph Steinbeck and the JChemPaint project\nOpenBabel code copyright (C) 2003 by the OpenBabel project team.\nSee file COPYRIGHT.txt for more details" ) );
}

void ApplicationWindow::support()
{
    QMessageBox::information( 0, tr( "How to get help" ),
                              tr
                              ( "Current information on XDrawChem can always be found at\nhttp://xdrawchem.sourceforge.net/\nThe latest release will be posted here, as well as links to mailing lists and the bug tracker.\n\nPlease submit bugs using the SourceForge tracker: http://www.sourceforge.net/tracker/?group_id=34518\n\nThere are two mailing lists:  xdrawchem-announce, where new releases will be announced,\nand xdrawchem-user, for open discussion among XDrawChem users.\nSubscribe by sending a blank e-mail with subject \"subscribe\" to \n\"xdrawchem-announce-request@lists.sourceforge.net\" or\n\"xdrawchem-user-request@lists.sourceforge.net\"\n\nYou can contact the author directly at\nbherger@users.sourceforge.net" ) );
}

void ApplicationWindow::whatsThis()
{
    QWhatsThis::enterWhatsThisMode();
}

const char *reflist =
    "Krause S, Willighagen E, Steinbeck C, "
    "\"JChemPaint - Using the Collaborative Forces of the Internet\nto Develop "
    "a Free Editor for 2D Chemical Structures\", Molecules 5:93-98\n\n"
    "Bremser W, \"HOSE - A Novel Substructure Code\", Anal. Chim. Acta 103:"
    "355-365\n\n"
    "Bremser W, \"Expectation Ranges of 13C NMR Chemical Shifts\", "
    "Mag. Res. Chem. 23(4):271-275\n\n"
    "Weininger D, \"SMILES, a Chemical Language and Information System.  1. Introduction to Methodology and Encoding Rules\",\nJ. Chem. Inf. Comput. Sci. 28:31-36\n\n"
    "Figueras J, \"Ring Perception Using Breadth-First Search\", J. Chem. Inf. Comput Sci. 36:986-991\n\n"
    "Ugi I et al., Journal of Chemical research (M), 1991, 2601-2689\n\n"
    "Pauling, L. \"The Nature of the Chemical Bond\", 3ed., 1960, Cornell University Press\n\n"
    "Pretsch, Clerc, Seibl, Simon. \"Tables of Spectral Data for Structure Determination of Organic Compounds\", 2ed., 1989, Springer-Verlag\n\n"
    "Lin S and Sandler SI, J. Phys. Chem. A, 2000, vol. 104, 7099-7105\n\n" "Steinbeck C, JMDraw (software), http://jmdraw.sourceforge.net/\n\n" "Molecules in the XDrawChem database are from the NCI Open Database, October 1999 release:  http://cactvs.cit.nih.gov/";

void ApplicationWindow::Refs()
{
    QMessageBox::about( this, "XDrawChem References", reflist );
}

void ApplicationWindow::SetStatusBar( const QString &s )
{
    statusBar()->showMessage( s );
}

/* obsolete.
void ApplicationWindow::MakeRingDialog() {
  RingDialog t(this, "ring dialog");
  if ( !t.exec() ) return;

  m_chemData->StartUndo(0,0);
  m_chemData->DeselectAll();
  m_chemData->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
  m_chemData->load( RingDir + t.getFile() );
  m_renderer->Inserted();
}
*/

void ApplicationWindow::ShowFixedDialog()
{
    FixedDialog fixedDialog( this );

    if ( !fixedDialog.exec() )
        return;
    hruler->update();
    vruler->update();
    /*
       na = i.getAngle_arrow();
       nl = i.getLength_arrow();
       if (na < 0.0) return;
       if (nl < 0.0) return;
       preferences.setArrow_fixedangle(na);
       preferences.setArrow_fixedlength(nl);
       na = i.getAngle_bond();
       nl = i.getLength_bond();
       if (na < 0.0) return;
       if (nl < 0.0) return;
       preferences.setBond_fixedangle(na);
       preferences.setBond_fixedlength(nl);
       na = i.getDoubleBondOffset();
       if (na < 0.0) return;
       preferences.setDoubleBondOffset(na);
     */
}

/*
void ApplicationWindow::MakeNetDialog()
{
    netDlg = new NetDialog( this );
    if ( !netDlg->exec() )
        return;
    qDebug() << "Server:" << netDlg->getServer();
    qDebug() << "Key   :" << netDlg->getKey();
    qDebug() << "Value :" << netDlg->getValue();
    NetAccess *na = new NetAccess();
    connect( na, SIGNAL( choicesFinished( const QStringList & ) ), this, SLOT( slotChoicesFinished( const QStringList & ) ) );

    setCursor( Qt::WaitCursor );
    m_renderer->setWaitCursor();
    na->getChoices( netDlg->getServer(), netDlg->getKey(), netDlg->getValue(), netDlg->getExact() );
}
*/


void ApplicationWindow::MakeNetDialog() {
  NetDialog n(this);
  if ( !n.exec() ) return;
  //cout << "Server:" << n.getServer() << endl;
  //cout << "Key   :" << n.getKey() << endl;
  //cout << "Value :" << n.getValue() << endl;
  NetAccess na;
  setCursor(Qt::WaitCursor);
  m_renderer->setWaitCursor();
  QStringList choices = na.getChoices(n.getServer(), n.getKey(), n.getValue(),
                                      n.getExact() );
  setCursor(Qt::ArrowCursor);
  m_renderer->setArrowCursor();
  if (choices.count() == 0) {
    QMessageBox::warning( this, tr("Database query failed"), tr("No molecules in the database match the query.") );
    return;
  }
  /*
  if (choices.count() == 1) {
    int i1 = choices[0].find("|");
    QString subfile = choices[0].left(i1) + ".mol";
    QString wf = na.getFile(n.getServer(), subfile);
    c->DeselectAll();
    c->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
    c->ProcessMDL(wf);
    r->Inserted();
    return;
  }
  */
  NetChooseDialog nc(this, choices);
  if ( !nc.exec() ) return;
  //cout << nc.getFile() << endl;
  // TODO: insert SMILES string returned by database
  m_chemData->fromSMILES(nc.getFile());
  m_renderer->Inserted();
  m_renderer->update();

  /*
  if (nc.getFile().contains(".mol")) {
    c->DeselectAll();
    c->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
    c->ProcessMDL(wf);
    r->Inserted();
   return;
  }
  */
}


void ApplicationWindow::slotChoicesFinished( const QStringList & choices )
{
    qDebug() << "slotChoicesFinished";
    setCursor( Qt::ArrowCursor );
    m_renderer->setArrowCursor();
    if ( choices.count() == 0 ) {
        QMessageBox::warning( this, tr( "Database query failed" ), tr( "No molecules in the database match the query." ) );
        return;
    }
    /*
       if (choices.count() == 1) {
       int i1 = choices[0].find("|");
       QString subfile = choices[0].left(i1) + ".mol";
       QString wf = na.getFile(n.getServer(), subfile);
       m_chemData->DeselectAll();
       m_chemData->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
       m_chemData->ProcessMDL(wf);
       m_renderer->Inserted();
       return;
       }
     */
    NetChooseDialog nc( this, choices );

    if ( !nc.exec() )
        return;
    qDebug() << nc.getFile();
    // TODO: insert SMILES string returned by database
    m_chemData->fromSMILES( nc.getFile() );
    m_renderer->Inserted();
    m_renderer->update();

    /*
       if (nc.getFile().contains(".mol")) {
       m_chemData->DeselectAll();
       m_chemData->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
       m_chemData->ProcessMDL(wf);
       m_renderer->Inserted();
       return;
       }
     */
}

void ApplicationWindow::pageSetup()
{
    PageSetupDialog p( this );

    //p.setPageSize(preferences.getPageSize());
    //p.setOrientation(preferences.getPageOrientation());
    if ( !p.exec() )
        return;
    //preferences.setPageSize(p.getPageSize());
    //preferences.setPageOrientation(p.getOrientation());
    m_renderer->UpdatePageGeometry();
}

void ApplicationWindow::NewColor()
{
    QColor newColor = QColorDialog::getColor( m_renderer->GetColor() );

    if ( newColor.isValid() ) {
        m_renderer->SetColor( newColor );
        colorBtn->setColor( newColor );
    }
}

void ApplicationWindow::BackgroundColor()
{
    QColor nc1 = QColorDialog::getColor( m_renderer->getBGColor() );

    if ( nc1.isValid() ) {
        m_renderer->setBGColor( nc1 );
        m_renderer->update();
    }
}

// peptide builder
void ApplicationWindow::PeptideBuilder()
{
    PeptDialog p1( this );

    if ( !p1.exec() )
        return;
    QString seq1 = p1.getPeptide();

    qDebug() << "peptide: " << seq1;
}

// help from help menu
void ApplicationWindow::NewManual()
{
    QString home;

#ifdef UNIX
    home = RingDir + "doc/index.html";
#else
    home = RingDir + "doc\\index.html";
#endif
    HelpBrowser( home );
}

// invoke context-sensitive help
void ApplicationWindow::HelpTopic( QString topic )
{
    QString home;

#ifdef UNIX
    home = RingDir + "doc/" + topic;
#else
    home = RingDir + "doc\\" + topic;
#endif
    HelpBrowser( home );
}

// new manual (based on QTextBrowser) -- derived from Qt example "helpviewer"
/****************************************************************************
** $Id: application.cpp,v 1.46 2005/11/16 03:01:55 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
void ApplicationWindow::HelpBrowser( QString home )
{
    HelpWindow *help = new HelpWindow( home, ".", 0 );

    help->setWindowTitle( QString( XDC_VERSION ) + " - Help viewer" );
    if ( QApplication::desktop()->width() > 400 && QApplication::desktop()->height() > 500 )
        help->show();
    else
        help->showMaximized();
}

//
// end code covered by "helpviewer" example copyright
//

// pass thru to Render2D
void ApplicationWindow::Cut()
{
    m_renderer->Cut();
}

void ApplicationWindow::Copy()
{
    m_renderer->Copy();
}

void ApplicationWindow::Paste()
{
    m_renderer->Paste();
}

void ApplicationWindow::Undo()
{
    m_renderer->Undo();
}

void ApplicationWindow::FlipH()
{
    m_renderer->Flip( FLIP_H );
}

void ApplicationWindow::FlipV()
{
    m_renderer->Flip( FLIP_V );
}

void ApplicationWindow::Rotate90()
{
    m_renderer->Rotate90();
}

void ApplicationWindow::Rotate180()
{
    m_renderer->Rotate180();
}

void ApplicationWindow::Rotate270()
{
    m_renderer->Rotate270();
}

void ApplicationWindow::Clear()
{
    m_renderer->EraseSelected();
}

void ApplicationWindow::MoleculeInfo()
{
    m_renderer->Tool( MODE_TOOL_MOLECULE_INFO );
}

void ApplicationWindow::CalcEA()
{
    m_renderer->Tool( MODE_TOOL_CALCEA );
}

void ApplicationWindow::CalcEF()
{
    m_renderer->Tool( MODE_TOOL_CALCEF );
}

void ApplicationWindow::CalcMW()
{
    m_renderer->Tool( MODE_TOOL_CALCMW );
}

void ApplicationWindow::Calc13CNMR()
{
    m_renderer->Tool( MODE_TOOL_13CNMR );
}

void ApplicationWindow::Calc1HNMR()
{
    m_renderer->Tool( MODE_TOOL_1HNMR );
}

void ApplicationWindow::CalcIR()
{
    m_renderer->Tool( MODE_TOOL_IR );
}

void ApplicationWindow::CalcpKa()
{
    m_renderer->Tool( MODE_TOOL_PKA );
}

void ApplicationWindow::CalcKOW()
{
    m_renderer->Tool( MODE_TOOL_KOW );
}

void ApplicationWindow::CalcName()
{
    m_renderer->Tool( MODE_TOOL_NAME );
}

void ApplicationWindow::ToSMILES()
{
    m_renderer->Tool( MODE_TOOL_TOSMILES );
}

void ApplicationWindow::ToInChI()
{
    m_renderer->Tool( MODE_TOOL_TOINCHI );
}

void ApplicationWindow::To3D()
{
    m_renderer->Tool( MODE_TOOL_2D3D );
}

void ApplicationWindow::CleanUpMolecule()
{
    m_renderer->Tool( MODE_TOOL_CLEANUPMOL );
}

void ApplicationWindow::saveCustomRing()
{
    m_renderer->Tool( MODE_TOOL_CUSTOMRING );
}

void ApplicationWindow::SelectAll()
{
    m_renderer->SelectAll();
}

void ApplicationWindow::DeselectAll()
{
    m_renderer->DeselectAll();
}

void ApplicationWindow::AutoLayout()
{
    m_renderer->AutoLayout();
}

void ApplicationWindow::DrawRegularArrow()
{
    m_renderer->setMode_DrawArrow( regularArrow );
}

void ApplicationWindow::DrawSquareBracket()
{
    m_renderer->setMode_DrawBracket( squareBracket );
}
void ApplicationWindow::CubicBezierTool( int x )
{
    qDebug() << "Bezier:" << x - 6;
    m_renderer->setMode_DrawGraphicObject( TYPE_BEZIER, x - 6 );
}

void ApplicationWindow::setClipboard( Clipboard * clip1 )
{
    m_renderer->setClipboard( clip1 );
}

void ApplicationWindow::setGroup_Reactant()
{
    m_renderer->Tool( MODE_TOOL_GROUP_REACTANT );
}

void ApplicationWindow::setGroup_Product()
{
    m_renderer->Tool( MODE_TOOL_GROUP_PRODUCT );
}

void ApplicationWindow::clearGroup()
{
    m_renderer->Tool( MODE_TOOL_GROUP_CLEAR );
}

void ApplicationWindow::Retro()
{
    m_renderer->Tool( MODE_TOOL_RETRO );
}

void ApplicationWindow::RetroAtomName()
{
    m_renderer->Tool( MODE_TOOL_RETRO_ATOMNAME );
}

void ApplicationWindow::RetroBondName()
{
    m_renderer->Tool( MODE_TOOL_RETRO_BONDNAME );
}

void ApplicationWindow::InsertSymbol()
{
    m_renderer->InsertSymbol();
}

void ApplicationWindow::Test()
{
    CharSelDialog cd1( this );

    cd1.exec();
    //m_renderer->Tool(MODE_TOOL_TEST);
}

void ApplicationWindow::clearAllGroups()
{
    m_renderer->clearAllGroups();
}

void ApplicationWindow::reactionAnalysisTest()
{
    m_renderer->ReactionAnalysis( RXN_TEST );
}

void ApplicationWindow::reactionAnalysisEnthalpy()
{
    m_renderer->ReactionAnalysis( RXN_ENTHALPY_ESTIMATE );
}

void ApplicationWindow::reactionAnalysis1HNMR()
{
    m_renderer->ReactionAnalysis( RXN_1HNMR );
}

void ApplicationWindow::reactionAnalysis13CNMR()
{
    m_renderer->ReactionAnalysis( RXN_13CNMR );
}

void ApplicationWindow::reactivityForward()
{
    m_renderer->Tool( MODE_TOOL_REACTIVITY_FORWARD );
}

void ApplicationWindow::reactivityRetro()
{
    m_renderer->Tool( MODE_TOOL_REACTIVITY_RETRO );
}

void ApplicationWindow::reactivityPC()
{
    m_renderer->Tool( MODE_TOOL_CHARGES );
}

void ApplicationWindow::toggleGrid()
{
    int i1;

    i1 = preferences.getDrawGrid();
    i1++;
    if ( i1 > 2 )
        i1 = 0;
    preferences.setDrawGrid( i1 );
    m_renderer->createGrid();
    m_renderer->update();
}

void ApplicationWindow::svXY( int x1, int y1 )
{
    hruler->setRange( x1, x1 + m_centralWidget->width() );
    hruler->update();
    vruler->setRange( y1, y1 + m_centralWidget->height() );
    vruler->update();
}

void ApplicationWindow::showDYK()
{
    DYKDialog dyk1;

    dyk1.exec();
}

void ApplicationWindow::MagnifyPlus()
{
    preferences.setZoom( preferences.getZoom() + 25 );
    if ( preferences.getZoom() > 200 )
        preferences.setZoom( 200 );
    m_renderer->zoomEvent();
    hruler->zoomEvent();
    vruler->zoomEvent();
    SetStatusBar( tr( "Zoom = %1 %" ).arg( preferences.getZoom() ) );
}

void ApplicationWindow::MagnifyMinus()
{
    preferences.setZoom( preferences.getZoom() - 25 );
    if ( preferences.getZoom() < 25 )
        preferences.setZoom( 25 );
    m_renderer->zoomEvent();
    hruler->zoomEvent();
    vruler->zoomEvent();
    SetStatusBar( tr( "Zoom = %1 %" ).arg( preferences.getZoom() ) );
}

void ApplicationWindow::Magnify100()
{
    preferences.setZoom( 100 );
    m_renderer->zoomEvent();
    hruler->zoomEvent();
    vruler->zoomEvent();
    SetStatusBar( tr( "Zoom = %1 %" ).arg( preferences.getZoom() ) );
}
void ApplicationWindow::Zoom( int /*z1*/ )
{
    m_renderer->zoomEvent();
    hruler->zoomEvent();
    vruler->zoomEvent();
    SetStatusBar( tr( "Zoom = %1 %" ).arg( preferences.getZoom() ) );
}

void ApplicationWindow::XDCSettings()
{
    bool ok;
    QFont font;

    switch ( QMessageBox::information( this, tr( "XDC Settings" ), tr( "Change XDrawChem settings:" ), tr( "&Main font" ), tr( "&Ruler font" ), tr( "Cancel" ), 2,      // Enter == button 2
                                       2 ) ) {  // Escape == button 2
    case 0:
        font = QFontDialog::getFont( &ok, preferences.getMainFont(), this );
        if ( ok ) {
            preferences.setMainFont( font );
            setFont( font );
            update();
        } else {
            return;
        }
        break;
    case 1:
        font = QFontDialog::getFont( &ok, preferences.getRulerFont(), this );
        if ( ok ) {
            preferences.setRulerFont( font );
            //setFont(font);
            hruler->update();
            vruler->update();
            update();
        } else {
            return;
        }
        break;
    case 2:
        return;
        break;
    }
}

void ApplicationWindow::setFontFace( int fn1 )
{
  qDebug() << "FontFace: " << fn1;
}

void ApplicationWindow::setFontPoints( int fn1 )
{
  qDebug() << "FontPoints: " << fn1;
}

//cmake#include "application.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
