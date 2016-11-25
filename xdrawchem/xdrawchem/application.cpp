/****************************************************************************
** $Id: application.cpp 187 2005-11-16 03:01:55Z bherger $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include "application.h"

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

using std::cout;
using std::endl;

#include <qglobal.h>
#include <qdatetime.h>
#include <qimage.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qclipboard.h>
#include <qdragobject.h>
#include <qtooltip.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qmultilineedit.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qfontdialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qpaintdevicemetrics.h>
#include <qwhatsthis.h>
#include <qscrollview.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qmime.h>
#include <qaccel.h>
#include <qlayout.h>

#ifdef UNIX
#include <stdlib.h>
#include <time.h>
#endif

#include "defs.h"
#include "drawable.h"
#include "render2d.h"
#include "chemdata.h"
#include "ringdialog.h"
#include "fixeddialog.h"
#include "netdialog.h"
#include "netchoosedialog.h"
#include "netaccess.h"
#include "pagesetupdialog.h"
#include "peptidebuilder.h"
#include "xdc_toolbutton.h"
#include "crings_dialog.h"
#include "helpwindow.h"
#include "xdc_event.h"
#include "myfiledialog.h"
#include "xruler.h"
#include "dyk.h"
#include "charsel.h"

// main window icon
#include "xdc.xpm"

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
    : QMainWindow( 0, "XDrawChem main window", WDestructiveClose )
{
  setFont(preferences.getMainFont());

  //old, boring icon (xdc.xpm)
  //setIcon(QPixmap(XDC_icon));

  // new, fancy PNG icon
  setIcon( QPixmap(RingDir + "xdrawchem-icon.png") );

  OBGetFilters();

  // What's This? text for top toolbar
  QString fileOpenText=tr("Click this button to open a file.<br><br>You can also select the Open command from the File menu.");
  QString fileSaveText=tr("Click this button to save the file you are editing.<br><br>You can also select the Save command from the File menu.<br><br>");
  QString filePrintText=tr("Click this button to print the file you are editing.<br><br>You can also select the Print command from the File menu.");
  QString editCutText=tr("Click this button to cut a selection.<br><br>You can also select the Cut command from the Edit menu, or press Ctrl+X.");
  QString editCopyText=tr("Click this button to copy a selection.<br><br>You can also select the Copy command from the Edit menu, or press Ctrl+C.");
  QString editPasteText=tr("Click this button to paste a selection.<br><br>You can also select the Paste command from the Edit menu, or press Ctrl+V.");
  QString editMPText=tr("Click this button to zoom in.");
  QString editMMText=tr("Click this button to zoom out.");

  // What's This? text for left side toolbar
  QString selectToolText = tr("Select tool<br><br>Use the Select tool to select and move items inside a box.<br><br>You can select multiple items and cut, copy, move and rotate them.");
  QString lassoToolText = tr("Lasso tool<br><br>Use the Lasso tool to select and move items by drawing a loop around them.<br><br>You can select multiple items and cut, copy, move and rotate them.");
  QString eraseToolText = tr("Erase tool<br><br>Use the Erase tool to erase individual items.");
  QString lineToolText = tr("Line tool<br><br>Use the Line tool to draw bonds.  Draw over existing bonds to create double and triple bonds.");
  QString dashLineToolText = tr("Dashed Line tool<br><br>Use the Dashed Line tool to draw dashed lines.  Draw over existing bonds to add dashed lines (to indicate resonance, etc.).");
  QString chainToolText = tr("Chain tool<br><br>Use the Chain tool to draw aliphatic chains.  The length of each segment is the current bond length.");
  QString upLineToolText = tr("Stereo Up Line tool<br><br>Use the Line tool to draw stereo-up lines, as shown on the button.");
  QString downLineToolText = tr("Stereo Down Line tool<br><br>Use the Line tool to draw stereo-down lines, as shown on the button.");
  QString wavyLineToolText = tr("Wavy Line tool<br><br>Use the Wavy Line tool to draw wavy lines, as shown on the button.");
  QString arrowToolText = tr("Arrow tool<br><br>Use the Arrow tool to draw straight arrows.<br><br>Click to draw a straight arrow.<br><br>Click and hold to pick from a list of available arrows.");
  QString cArrowToolText = tr("Curved Arrow tool<br><br>Use the Curved Arrow tool to insert curved arrows.<br><br>Click and hold to select from a picture menu of arrows.");
  QString bracketToolText = tr("Bracket tool<br><br>Use the Bracket tool to draw brackets and parentheses.<br><br>Click to draw square brackets.<br><br>Click and hold to select from a picture menu of brackets.");
  QString textToolText = tr("Text tool<br><br>Use the Text tool to add text and label atoms and points.<br>(See manual for info on formatting text)");
  QString ringToolText = tr("Ring tool<br><br>Use the Ring tool to insert ready-made rings and structures.<br><br>Click to open the ring dialog, which allows selection from a list of all built-in rings and structures.<br><br>Click and hold to select from a picture menu of select rings.<br>(See manual for more info on modifying this menu)");
  QString symbolToolText = tr("Symbol tool<br><br>Use the Symbol tool to insert symbols.<br><br>Click and hold to select from a picture menu of symbols.<br>");

  int id;
  QMimeSourceFactory *myfactory = QMimeSourceFactory::defaultFactory();

  printer = new QPrinter;
  QPixmap openIcon, saveIcon, printIcon, superIcon, subIcon;
  QPixmap swatchblack, swatchred, swatchgreen, swatchblue;
  QPixmap selectIcon, drawBoxIcon, drawLineIcon, downLineIcon, upLineIcon,
    textIcon, arrowIcon, bracketIcon, cutIcon, copyIcon, pasteIcon, ringIcon,
    dashIcon, symbolIcon, curveArrowIcon, chainIcon, lassoIcon, mmIcon, mpIcon;

  cw = new QWidget(this);

  // create scrolling viewport and render widget
  sv = new QScrollView(cw);
  r = new Render2D( sv->viewport(), "render2d" );
  sv->addChild(r);
  connect(sv, SIGNAL(contentsMoving(int,int)),
	  this, SLOT(svXY(int,int)) );

  // create grid layout
  QGridLayout *glo = new QGridLayout(cw, 2, 2);
  vruler = new XRuler(cw);
  vruler->setHV(2);
  vruler->setFixedWidth(20);
  glo->addWidget(vruler, 1, 0);
  hruler = new XRuler(cw);
  hruler->setHV(1);
  hruler->setFixedHeight(20);
  glo->addWidget(hruler, 0, 1);
  glo->addWidget(sv, 1, 1);

  // top toolbar
  fileTools = new QToolBar( this, "file operations" );
  fileTools->setLabel( tr( "File Operations" ) );

  openIcon = QPixmap( fileopen );
  myfactory->setPixmap("fileopenicon", openIcon );
  QToolButton * fileOpen
    = new QToolButton( openIcon, tr("Open file"), QString::null,
		       this, SLOT(load()), fileTools, "open file" );
  QWhatsThis::add(fileOpen, fileOpenText);

  saveIcon = QPixmap( filesave );
  myfactory->setPixmap("filesaveicon", saveIcon );
  QToolButton * fileSave
    = new QToolButton( saveIcon, tr("Save file"), QString::null,
		       this, SLOT(save()), fileTools, "save file" );
  QWhatsThis::add(fileSave, fileSaveText);

  printIcon = QPixmap( fileprint );
  myfactory->setPixmap("fileprinticon", printIcon );
  QToolButton * filePrint
    = new QToolButton( printIcon, tr("Print file"), QString::null,
		       this, SLOT(print()), fileTools, "print file" );
  QWhatsThis::add(filePrint, filePrintText);

  cutIcon = QPixmap(cuttool);
  myfactory->setPixmap("cuticon", cutIcon );
  QToolButton * editCut
    = new QToolButton( cutIcon, tr("Cut"), QString::null,
		       this, SLOT(Cut()), fileTools, "cut" );
  QWhatsThis::add(editCut, editCutText);

  copyIcon = QPixmap(copytool);
  myfactory->setPixmap("copyicon", copyIcon );
  QToolButton * editCopy
    = new QToolButton( copyIcon, tr("Copy"), QString::null,
		       this, SLOT(Copy()), fileTools, "copy" );
  QWhatsThis::add(editCopy, editCopyText);

  pasteIcon = QPixmap(pastetool);
  myfactory->setPixmap("pasteicon", pasteIcon );
  QToolButton * editPaste
    = new QToolButton( pasteIcon, tr("Paste"), QString::null,
		       this, SLOT(Paste()), fileTools, "paste" );
  QWhatsThis::add(editPaste, editPasteText);

  mpIcon = QPixmap(magnify_plus_xpm);
  myfactory->setPixmap("mpicon", mpIcon );
  QToolButton * editMP
    = new QToolButton( mpIcon, tr("Zoom In"), QString::null,
		       this, SLOT(MagnifyPlus()), fileTools, "plus" );
  QWhatsThis::add(editMP, editMPText);

  mmIcon = QPixmap(magnify_minus_xpm);
  myfactory->setPixmap("mmicon", mmIcon );
  QToolButton * editMM
    = new QToolButton( mmIcon, tr("Zoom Out"), QString::null,
		       this, SLOT(MagnifyMinus()), fileTools, "minus" );
  QWhatsThis::add(editMM, editMMText);

  QPixmap *tmp_pm = new QPixmap(32, 20);
  tmp_pm->fill(QColor(210,210,210));
  QPainter pixpaint(tmp_pm);
  pixpaint.fillRect(2, 3, 14, 14, QColor(0,0,0));
  pixpaint.setPen(QColor(0,0,0));
  pixpaint.drawLine( 24, 2, 24, 16 );
  pixpaint.drawLine( 24, 16, 20, 12 );
  pixpaint.drawLine( 24, 16, 28, 12 );
  pb1 = new QPushButton(fileTools);
  pb1->setPixmap(*tmp_pm);
  QToolTip::add(pb1, tr("Set drawing color"));
  connect(pb1, SIGNAL(pressed()), this, SLOT(NewColor()) );

  ltList = new QComboBox(false, fileTools);
  QToolTip::add(ltList, tr("Set line thickness"));
  QWhatsThis::add(ltList, tr("Set Line Thickness") );
  QPixmap px1;
  QBitmap mask0(12, 12);
  mask0.fill(Qt::color0);
  px1 = QPixmap(line1);
  px1.setMask(mask0);
  ltList->insertItem(line1, "1");
  ltList->insertItem(line2, "2");
  ltList->insertItem(line3, "3");
  ltList->insertItem(line4, "4");
  ltList->insertItem(line5, "5");
  connect(ltList, SIGNAL(activated(int)), this, SLOT(SetThick(int)) );

  fontList = new QComboBox(false, fileTools);
  QToolTip::add(fontList, tr("Set font"));
  QWhatsThis::add(fontList, tr("Set Font") );
  fontList->insertItem( "Courier" );
  fontList->insertItem( "Helvetica" );
  fontList->insertItem( "Symbol" );
  fontList->insertItem( "Times" );
  fontList->setCurrentItem(1);
  connect(fontList, SIGNAL(activated(int)), this, SLOT(SetFont(int)) );

  fontSizeList = new QComboBox(false, fileTools);
  QToolTip::add(fontSizeList, tr("Set font size"));
  QWhatsThis::add(fontSizeList, tr("Set Font Size") );
  fontSizeList->insertItem(QString("8"));
  fontSizeList->insertItem(QString("10"));
  fontSizeList->insertItem(QString("12"));
  fontSizeList->insertItem(QString("14"));
  fontSizeList->insertItem(QString("18"));
  fontSizeList->insertItem(QString("24"));
  fontSizeList->insertItem(QString("32"));
  fontSizeList->setCurrentItem(2);
  connect(fontSizeList, SIGNAL(activated(int)), this, SLOT(SetFont(int)) );

  superIcon = QPixmap( justifylefttool_xpm );
  myfactory->setPixmap("lefttool", superIcon );
  leftJustify
    = new QToolButton( superIcon, tr("Left-justify selected text"), 
		       QString::null, r, SLOT(JustifyLeft()), fileTools, 
		       "left" );
  leftJustify->hide();

  superIcon = QPixmap( justifycentertool_xpm );
  myfactory->setPixmap("centertool", superIcon );
  centerJustify
    = new QToolButton( superIcon, tr("Center selected text"), 
		       QString::null, r, SLOT(JustifyCenter()), fileTools, 
		       "center" );
  centerJustify->hide();

  superIcon = QPixmap( justifyrighttool_xpm );
  myfactory->setPixmap("righttool", superIcon );
  rightJustify
    = new QToolButton( superIcon, tr("Right-justify selected text"), 
		       QString::null, r, SLOT(JustifyRight()), fileTools, 
		       "right" );
  rightJustify->hide();

  superIcon = QPixmap( boldtool_xpm );
  myfactory->setPixmap("boldtool", superIcon );
  Bold
    = new QToolButton( superIcon, tr("Make selected text <b>bold</b> (Ctrl+B)"), 
		       QString::null, r, SLOT(Bold()), fileTools, "bold" );
  QAccel *accel1 = new QAccel( Bold );        
  accel1->connectItem( accel1->insertItem(CTRL+Key_B), 
		       r, SLOT(Bold()) );        
  Bold->hide();

  superIcon = QPixmap( italictool_xpm );
  myfactory->setPixmap("italictool", superIcon );
  Italic
    = new QToolButton( superIcon, tr("<i>Italicize</i> selected text (Ctrl+I)"), 
		       QString::null, r, SLOT(Italic()), fileTools, "italic" );
  QAccel *accel2 = new QAccel( Italic );        
  accel2->connectItem( accel2->insertItem(CTRL+Key_I), 
		       r, SLOT(Italic()) );        
  Italic->hide();

  superIcon = QPixmap( underlinetool_xpm );
  myfactory->setPixmap("underlinetool", superIcon );
  Underline
    = new QToolButton( superIcon, tr("Underline selected text (Ctrl+U)"), QString::null,
		       r, SLOT(Underline()), fileTools, "underline" );
  QAccel *accel3 = new QAccel( Underline );        
  accel3->connectItem( accel3->insertItem(CTRL+Key_U), 
		       r, SLOT(Underline()) );        
  Underline->hide();

  superIcon = QPixmap( superscript_xpm );
  myfactory->setPixmap("supericon", superIcon );
  superScript
    = new QToolButton( superIcon, tr("Superscript selected text (Ctrl-Plus)"), 
		       QString::null, r, SLOT(Superscript()), fileTools, 
		       "superscript" );
  QAccel *accel4 = new QAccel( superScript );        
  accel4->connectItem( accel4->insertItem(CTRL+SHIFT+Key_Plus), 
		       r, SLOT(Superscript()) );        
  superScript->hide();

  subIcon = QPixmap( subscript_xpm );
  myfactory->setPixmap("subicon", subIcon );
  subScript
    = new QToolButton( subIcon, tr("Subscript selected text (Ctrl-Minus)"), QString::null,
		       r, SLOT(Subscript()), fileTools, "subscript" );
  QAccel *accel5 = new QAccel( subScript );        
  accel5->connectItem( accel5->insertItem(CTRL+Key_Minus), 
		       r, SLOT(Subscript()) );        
  subScript->hide();

  QToolButton *wt = QWhatsThis::whatsThisButton( fileTools );
  myfactory->setPixmap( "whatsthisicon", *(wt->pixmap()) );

  // left side toolbar
  drawTools = new QToolBar(this, "draw operations");
  moveDockWindow(drawTools, Qt::DockLeft);
  selectIcon = QPixmap( selecttool );
  myfactory->setPixmap("selecttoolicon", selectIcon );
  QToolButton * selectObjects
    = new QToolButton( selectIcon, tr("Select"), QString::null,
		       r, SLOT(setMode_Select()), drawTools, "select" );
  QWhatsThis::add(selectObjects, selectToolText);

  lassoIcon = QPixmap( lassotool );
  myfactory->setPixmap("lassotoolicon", lassoIcon );
  QToolButton * lassoObjects
    = new QToolButton( lassoIcon, tr("Lasso"), QString::null,
		       r, SLOT(setMode_Lasso()), drawTools, "lasso" );
  QWhatsThis::add(lassoObjects, lassoToolText);

  drawBoxIcon = QPixmap( eraseIcon );
  myfactory->setPixmap("erasetoolicon", drawBoxIcon );
  QToolButton * drawBox
    = new QToolButton( drawBoxIcon, tr("Erase"), QString::null,
		       r, SLOT(setMode_Erase()), drawTools, "erase" );
  QWhatsThis::add(drawBox, eraseToolText);

  drawLineIcon = QPixmap( linetool );
  myfactory->setPixmap("linetoolicon", drawLineIcon );
  QToolButton * drawLine
    = new QToolButton( drawLineIcon, tr("Draw line"), QString::null,
		       r, SLOT(setMode_DrawLine()), drawTools, 
		       "draw line" );
  QWhatsThis::add(drawLine, lineToolText);

  dashIcon = QPixmap( dashtool );
  myfactory->setPixmap("dashtoolicon", dashIcon );
  QToolButton * drawDashLine
    = new QToolButton( dashIcon, tr("Draw dashed line"), QString::null,
		       r, SLOT(setMode_DrawDashLine()), drawTools, 
		       "draw dashed line" );
  QWhatsThis::add(drawDashLine, dashLineToolText);

  chainIcon = QPixmap( chaintool );
  myfactory->setPixmap("chaintoolicon", chainIcon );
  QToolButton * drawChainLine
    = new QToolButton( chainIcon, tr("Draw aliphatic chain"), QString::null,
		       r, SLOT(setMode_DrawChain()), drawTools, 
		       "draw chain" );
  QWhatsThis::add(drawChainLine, chainToolText);

  upLineIcon = QPixmap( uptool );
  myfactory->setPixmap("uplinetoolicon", upLineIcon );
  QToolButton * drawUpLine
    = new QToolButton( upLineIcon, tr("Draw stereo-up line"), QString::null,
		       r, SLOT(setMode_DrawUpLine()), drawTools, 
		       "draw stereo-up line" );
  QWhatsThis::add(drawUpLine, upLineToolText);

  downLineIcon = QPixmap( downtool );
  myfactory->setPixmap("downlinetoolicon", downLineIcon );
  QToolButton * drawDownLine
    = new QToolButton( downLineIcon, tr("Draw stereo-down line"), 
		       QString::null, r, SLOT(setMode_DrawDownLine()), 
		       drawTools, "draw stereo-down line" );
  QWhatsThis::add(drawDownLine, downLineToolText);

  QPixmap wavyLineIcon = QPixmap( wavytool );
  myfactory->setPixmap("wavylinetoolicon", wavyLineIcon );
  QToolButton * drawWavyLine
    = new QToolButton( wavyLineIcon, tr("Draw wavy bond"), 
		       QString::null, r, SLOT(setMode_DrawWavyLine()), 
		       drawTools, "draw wavy bond" );
  QWhatsThis::add(drawWavyLine, wavyLineToolText);

  arrowIcon = QPixmap( arrowtool );
  myfactory->setPixmap("arrowtoolicon", arrowIcon );
  QToolButton * drawArrowLine
    = new QToolButton( arrowIcon, tr("Draw arrow"), QString::null,
		       this, SLOT(DrawRegularArrow()), drawTools, 
		       "draw arrow" );
  drawArrowLine->setPopup(BuildArrowMenu());
  drawArrowLine->setPopupDelay(200);
  QWhatsThis::add(drawArrowLine, arrowToolText);

  curveArrowIcon = QPixmap( curvearrowtool_xpm );
  myfactory->setPixmap("curvearrowtoolicon", curveArrowIcon );
  QToolButton * drawCArrow
    = new QToolButton( curveArrowIcon, tr("Draw curved arrow"), QString::null,
		       this, SLOT(Dummy()), drawTools, 
		       "draw curved arrow" );
  drawCArrow->setPopup(BuildCurveArrowMenu());
  drawCArrow->setPopupDelay(1);
  QWhatsThis::add(drawCArrow, cArrowToolText);

  bracketIcon = QPixmap( brackettool );
  myfactory->setPixmap("brackettoolicon", bracketIcon );
  QToolButton * drawBracket
    = new QToolButton( bracketIcon, tr("Draw bracket"), QString::null,
		       this, SLOT(DrawSquareBracket()), drawTools, 
		       "draw bracket" );
  drawBracket->setPopup(BuildBracketMenu());
  drawBracket->setPopupDelay(200);
  QWhatsThis::add(drawBracket, bracketToolText);

  textIcon = QPixmap( texttool );
  myfactory->setPixmap("texttoolicon", textIcon );
  QToolButton * drawText
    = new QToolButton( textIcon, tr("Draw or edit text"), QString::null,
		       r, SLOT(setMode_DrawText()), drawTools, 
		       "draw or edit text" );
  QWhatsThis::add(drawText, textToolText);

  ringIcon = QPixmap( ringtool );
  myfactory->setPixmap("ringtoolicon", ringIcon );
  QToolButton * drawRing
    = new QToolButton( ringIcon, tr("Draw ring"), QString::null,
		       this, SLOT(Dummy()), drawTools, 
		       "draw ring" );
  ringmenu = BuildNewRingMenu();
  drawRing->setPopup(ringmenu);
  drawRing->setPopupDelay(1);
  QWhatsThis::add(drawRing, ringToolText);

  symbolIcon = QPixmap( symboltool );
  myfactory->setPixmap("symboltoolicon", symbolIcon );
  QToolButton * drawSymbol
    = new QToolButton( symbolIcon, tr("Draw symbol"), QString::null,
		       this, SLOT(Dummy()), drawTools, "draw symbol" );
  drawSymbol->setPopup(BuildSymbolMenu());
  drawSymbol->setPopupDelay(1);
  QWhatsThis::add(drawSymbol, symbolToolText);

  // second (left? right?) toolbar - predefined rings
  ringTools = new QToolBar( this, "canned structures" );
  ringTools->setLabel( tr( "Canned Structures" ) );
  moveDockWindow(ringTools, Qt::DockLeft);

  XDC_ToolButton *b1;
  QBitmap mask1(24, 24);
  mask1.fill(Qt::color1);

  b1 = new XDC_ToolButton( ringTools, "cyclopropane.cml");
  QToolTip::add(b1, tr("Cyclopropane") );
  px1 = QPixmap(n3ring);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "cyclobutane.cml");
  QToolTip::add(b1, tr("Cyclobutane") );
  px1 = QPixmap(n4ring);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "cyclopentane.cml");
  QToolTip::add(b1, tr("Cyclopentane") );
  px1 = QPixmap(n5ring);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "cyclopentadiene.cml");
  QToolTip::add(b1, tr("Cyclopentadiene") );
  px1 = QPixmap(n5ring_diene);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "cyclohexane.cml");
  QToolTip::add(b1, tr("Cyclohexane") );
  px1 = QPixmap(n6ring);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "6ring_chair.cml");
  QToolTip::add(b1, tr("Cyclohexane - chair conformation") );
  px1 = QPixmap(n6ring_chair);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "6ring_boat.cml");
  QToolTip::add(b1, tr("Cyclohexane - boat conformation") );
  px1 = QPixmap(n6ring_boat);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );
  b1 = new XDC_ToolButton( ringTools, "benzene.cml");
  QToolTip::add(b1, tr("Benzene") );
  px1 = QPixmap(n6ring_benzene);
  px1.setMask(mask1);
  b1->setPixmap( px1 );
  connect( b1, SIGNAL(pressed()), b1, SLOT(trigger()));
  connect( b1, SIGNAL(IncludeFile(QString)), this, 
	   SLOT(FromRingToolbar(QString)) );

  // BioTools toolbar
  bioTools = BuildBioTools();
  bioTools->setLabel( tr("Biochemical drawing tools") );
  moveDockWindow(bioTools, Qt::DockRight);

  // menus
  QPopupMenu * file = new QPopupMenu( this );
  menuBar()->insertItem( tr("&File"), file );

  file->insertItem( tr("&New"), this, SLOT(newDoc()), CTRL+Key_N );

  id = file->insertItem( openIcon, tr("&Open"),
			 this, SLOT(load()), CTRL+Key_O );
  file->setWhatsThis( id, fileOpenText );

#if QT_VERSION >= 300
  id = file->insertItem( tr("&Find on Internet"), this,
  			 SLOT(MakeNetDialog()), CTRL+Key_F );
#endif

  id = file->insertItem( saveIcon, tr("&Save"), 
			 this, SLOT(save()), CTRL+Key_S );
  file->setWhatsThis( id, fileSaveText );

  id = file->insertItem( tr("Save &as..."), this, SLOT(saveAs()) );
  file->setWhatsThis( id, fileSaveText );
  id = file->insertItem( tr("Save picture..."), this, SLOT(savePicture()) );

  file->insertSeparator();
  id = file->insertItem( tr("Send to &external program..."), 
			 this, SLOT(externalProgram()) );

  file->insertSeparator();
  id = file->insertItem( tr("Pa&ge setup"), this , SLOT(PageSetup()) );
  id = file->insertItem( printIcon, tr("&Print"),
			 this, SLOT(print()), CTRL+Key_P );
  file->setWhatsThis( id, filePrintText );
  file->insertSeparator();
  file->insertItem( tr("Close"), this, SLOT(close()), CTRL+Key_W );
  file->insertItem( tr("Quit"), qApp, SLOT( closeAllWindows() ), CTRL+Key_Q );

  QPopupMenu * rotateSub = new QPopupMenu(this);
  rotateSub->insertItem( tr("Rotate 90 degrees clockwise"), this, 
			 SLOT(Rotate90()) );
  rotateSub->insertItem( tr("Rotate 180 degrees"), this, SLOT(Rotate180()) );
  rotateSub->insertItem( tr("Rotate 90 degrees counterclockwise"), this, 
			 SLOT(Rotate270()) );

  QPopupMenu * flipSub = new QPopupMenu(this);
  id = flipSub->insertItem( tr("Flip &horizontal"), this, SLOT(FlipH()) );
  id = flipSub->insertItem( tr("Flip &vertical"), this, SLOT(FlipV()) );

  QPopupMenu * zoomSub = new QPopupMenu(this);
  id = zoomSub->insertItem(tr("Normal (100%)"),this,SLOT(Magnify100()),
			   CTRL+Key_5);
  id=zoomSub->insertItem(tr("Zoom out"),this,SLOT(MagnifyMinus()),CTRL+Key_1);
  id = zoomSub->insertItem(tr("Zoom in"),this,SLOT(MagnifyPlus()),CTRL+Key_0);
  /*
  zoomSub->insertSeparator();
  id = zoomSub->insertItem(tr("25%"),this,SLOT(Dummy()),CTRL+Key_2);
  id = zoomSub->insertItem(tr("50%"),this,SLOT(Dummy()),CTRL+Key_3);
  id = zoomSub->insertItem(tr("75%"),this,SLOT(Dummy()),CTRL+Key_4);
  id = zoomSub->insertItem(tr("100%"),this,SLOT(Dummy()) );
  id = zoomSub->insertItem(tr("125%"),this,SLOT(Dummy()),CTRL+Key_6);
  id = zoomSub->insertItem(tr("150%"),this,SLOT(Dummy()),CTRL+Key_7);
  id = zoomSub->insertItem(tr("175%"),this,SLOT(Dummy()),CTRL+Key_8);
  id = zoomSub->insertItem(tr("200%"),this,SLOT(Dummy()),CTRL+Key_9);
  */

  edit = new QPopupMenu(this);
  menuBar()->insertItem(tr("&Edit"), edit);

  edit->insertItem(tr("&Undo"), this, SLOT(Undo()), CTRL+Key_Z);
  insertsymbol_id = edit->insertItem( tr("Insert s&ymbol"), this,
				      SLOT(InsertSymbol()) );
  edit->setItemVisible(insertsymbol_id, false);
  edit->insertSeparator();
  edit->insertItem(tr("Cu&t"), this, SLOT(Cut()), CTRL+Key_X);
  edit->insertItem(tr("&Copy"), this, SLOT(Copy()), CTRL+Key_C);
  edit->insertItem(tr("&Paste"), this, SLOT(Paste()), CTRL+Key_V);
  edit->insertItem(tr("Clear"), this, SLOT(Clear()), Key_Delete);
  edit->insertSeparator();
  edit->insertItem(tr("Select &All"), this, SLOT(SelectAll()), CTRL+Key_A);
  edit->insertItem(tr("&Deselect All"), this, SLOT(DeselectAll()), 
		   CTRL+SHIFT+Key_A);
  edit->insertItem(tr("&Rotate"), rotateSub );
  edit->insertItem(tr("&Flip"), flipSub );
  edit->insertSeparator();
  edit->insertItem(tr("&Zoom"), zoomSub );

  QPopupMenu * groupmenu = new QPopupMenu(this);
  menuBar()->insertItem(tr("&Group"), groupmenu);
  groupmenu->insertItem(tr("Select &Reactant"), this, 
			SLOT(setGroup_Reactant()) );
  groupmenu->insertItem(tr("Select &Product"), this, 
			SLOT(setGroup_Product()) );
  groupmenu->insertItem(tr("Clear &group"), this, 
			SLOT(clearGroup()) );
  groupmenu->insertItem(tr("Clear all &groups"), this, 
			SLOT(clearAllGroups()) );

  format = new QPopupMenu(this);
  format->setCheckable(true);
  menuBar()->insertItem(tr("Forma&t"), format);

  fixlen_bond = format->insertItem(tr("&Bond - Fixed length and angle"),this,
			      SLOT(setFixed_bond()) );
  format->setItemChecked(fixlen_bond, preferences.getBond_fixed());	
  fixlen_arrow = format->insertItem(tr("&Arrow - Fixed length and angle"),this,
			      SLOT(setFixed_arrow()) );
  format->setItemChecked(fixlen_arrow, preferences.getArrow_fixed());	
  //fix_hydrogens = format->insertItem(tr("Automatically Add &hydrogens"),this,
  //		      SLOT(setFix_Hydrogens()) );
  //format->setItemChecked(fix_hydrogens, preferences.getFixHydrogens());
  format->insertSeparator();
  format->insertItem(tr("Set background &color"), this, 
		     SLOT(BackgroundColor()) );
  format->insertItem(tr("Toggle &grid"), this, SLOT(toggleGrid()),CTRL+Key_G );
  format->insertSeparator();
  format->insertItem(tr("&Drawing settings..."), this, 
		     SLOT(ShowFixedDialog()));
  format->insertItem(tr("&XDC settings..."), this, SLOT(XDCSettings()) );

  QPopupMenu * reactionTools = new QPopupMenu(this);
  //reactionTools->insertItem( tr("Test..."), this, 
  //		     SLOT(reactionAnalysisTest()) );
  reactionTools->insertItem( tr("Estimate gas-phase enthalphy change"),
			     this, SLOT(reactionAnalysisEnthalpy()) );
  reactionTools->insertItem( tr("Compare 1H NMR"),
			     this, SLOT(reactionAnalysis1HNMR()) );
  reactionTools->insertItem( tr("Compare 13C NMR"),
			     this, SLOT(reactionAnalysis13CNMR()) );
  reactionTools->insertSeparator();
  //reactionTools->insertItem( tr("Estimate partial charges"),
  //		     this, SLOT(reactivityPC()) );
  //reactionTools->insertItem( tr("Forward reactions"),
  //		     this, SLOT(reactivityForward()) );
  reactionTools->insertItem( tr("Reverse reactions"),
			     this, SLOT(reactivityRetro()) );

  /*
  reactionTools->insertItem( tr("Retrosynthesis"), this, SLOT(Retro()) );
  */
  reactionTools->insertItem( tr("Get bond identifier"), this, 
			     SLOT(RetroBondName()) );

  QPopupMenu * tools = new QPopupMenu(this);
  menuBar()->insertItem(tr("T&ools"), tools);
  tools->insertItem(tr("Clean up molecule"), this, SLOT(CleanUpMolecule()) );
  tools->insertItem(tr("Auto &layout"), this, SLOT(AutoLayout()), CTRL+Key_L );
  tools->insertItem(tr("Create custom ring"), this,
		    SLOT(saveCustomRing()) );
  tools->insertSeparator();
  tools->insertItem(tr("Molecule information..."),this,SLOT(MoleculeInfo()) );

  tools->insertItem(tr("Predict 1H NMR"), this, SLOT(Calc1HNMR()) );
  tools->insertItem(tr("Predict 13C NMR"), this, SLOT(Calc13CNMR()) );
  tools->insertItem(tr("Predict IR"), this, SLOT(CalcIR()) );
  tools->insertItem(tr("Predict pKa"), this, SLOT(CalcpKa()) );
  tools->insertItem(tr("Predict octanol-water partition (Kow)"), this, SLOT(CalcKOW()) );
  tools->insertItem(tr("Reaction"), reactionTools); 
  tools->insertSeparator();
  tools->insertItem(tr("Input SMILES"), this, SLOT(FromSMILES()) );
  tools->insertItem(tr("Output SMILES"), this, SLOT(ToSMILES()) );
  tools->insertItem(tr("Output InChI"), this, SLOT(ToInChI()) );
  tools->insertSeparator();
  tools->insertItem(tr("Build 3D model of molecule"), this, SLOT(To3D()) );
  //tools->insertSeparator();
  //tools->insertItem(tr("Test tool"), this, SLOT(Test()) );

  QPopupMenu * help = new QPopupMenu( this );
  menuBar()->insertSeparator();
  menuBar()->insertItem( tr("&Help"), help );

  help->insertItem( tr("&Manual"), this, SLOT(NewManual()), Key_F1 );
  help->insertItem( tr("&Did You Know?"), this, SLOT(showDYK()) );
  help->insertItem( tr("&About"), this, SLOT(about()) );
  help->insertItem( tr("&Support"), this, SLOT(support()) );
  help->insertItem( tr("&References"), this, SLOT(Refs()) );
  help->insertSeparator();
  help->insertItem( tr("What's &This"),this,SLOT(whatsThis()), SHIFT+Key_F1 );

  // create data system
  c = new ChemData;
  c->setClipboard(0);

  // connect (non-Qt) data center and render widget
  r->setChemData(c);
  c->setRender2D(r);
  connect(r, SIGNAL(XDCEventSignal(XDC_Event *)),
	  c, SLOT(XDCEventHandler(XDC_Event *)) );
  // connect renderer to application window
  connect(r, SIGNAL(TextOn(QFont)), this, SLOT(ShowTextButtons(QFont)) );
  connect(r, SIGNAL(TextOff()), this, SLOT(HideTextButtons()) );
  connect( r, SIGNAL(SignalSetStatusBar(QString)),
	   this, SLOT(SetStatusBar(QString)) );
  connect( c, SIGNAL(SignalSetStatusBar(QString)),
	   this, SLOT(SetStatusBar(QString)) );
  connect( r, SIGNAL(SignalHelpTopic(QString)),
	   this, SLOT(HelpTopic(QString)) );
  connect( c, SIGNAL(SignalHelpTopic(QString)),
	   this, SLOT(HelpTopic(QString)) );
  connect( c, SIGNAL( SignalUpdateCustomRingMenu() ),
	   this, SLOT( updateCustomRingMenu() ) );  

  //setCentralWidget( sv );
  setCentralWidget( cw );
  statusBar()->message( "Ready" );
  resize( 640, 640 );
}

/* obsolete.  removed.
QPopupMenu *ApplicationWindow::BuildRingMenu() {
  QPopupMenu * ringSub = new QPopupMenu(this);
  QDir d(RingDir, "*.png");
  ringlist = d.entryList();
  for (int cc = 0; cc < ringlist.count(); cc++) {
    ringSub->insertItem( QPixmap( QString(RingDir + ringlist[cc]) ),
			 this, SLOT(FromRingMenu(int)), 0, cc );
  }
  return ringSub;
}
*/

// externalProgram() sends to an external program
void ApplicationWindow::externalProgram() {
  // supported programs
#define PROGRAM_KRYOMOL 1
#define PROGRAM_GHEMICAL 2

  int sendto = 0, qbox, passfail = 0;
  QString exportFileName;
  qbox = QMessageBox::question( this,
				tr("Send to external program"),
				tr("This function will open a 3-D version of one molecule\nin an external modelling program.\n\nSend to program:"),
				tr("Ghemical"),
				tr("KryoMol"),
				tr("Cancel"),
				2
				);
  if (qbox == 0) sendto = 1;
  if (qbox == 1) sendto = 2;
  if (qbox == 2) sendto = 0;

  if ( sendto > 0 ) {
    exportFileName = QDateTime::currentDateTime().toString("MMddhhmmsszzz");
    exportFileName.append(".mol");
    exportFileName.prepend("/tmp/");
    std::cout << "export " << exportFileName << std::endl;
    c->firstMolecule()->Make3DVersion(exportFileName);
    if (QFile::exists(exportFileName) == false) {
      QMessageBox::critical( this,
			     tr("Send to external program failed"),
			     tr("Could not save file for external program.")
			     );
      return;
    }
    if (sendto == 1) {
      std::cout << "Ghemical: ";
      exportFileName.prepend("ghemical ");
      passfail = system(exportFileName.ascii());
      std::cout << passfail << std::endl;
    }
    if (sendto == 2) {
      std::cout << "KryoMol: ";
      exportFileName.prepend("kryomol ");
      passfail = system(exportFileName.ascii());
      std::cout << passfail << std::endl;
    }
    if (passfail != 0) {
      QMessageBox::critical( this,
			     tr("Send to external program failed"),
			     tr("Could not execute the external program.\nPlease verify that it is installed correctly.")
			     );
      return;
    }
  }
}

QPopupMenu *ApplicationWindow::BuildSymbolMenu() {
  QPopupMenu * symbolSub = new QPopupMenu(this);
  QPixmap p1;
  p1 = QPixmap(sym_plus_xpm);
  symbollist.append(QString("sym_plus"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 0);
  p1 = QPixmap(sym_minus_xpm);
  symbollist.append(QString("sym_minus"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 1);
  p1 = QPixmap(sym_delta_plus_xpm);
  symbollist.append(QString("sym_delta_plus"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 2);
  p1 = QPixmap(sym_delta_minus_xpm);
  symbollist.append(QString("sym_delta_minus"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 3);
  p1 = QPixmap(sym_1e_xpm);
  symbollist.append(QString("sym_1e"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 4);
  p1 = QPixmap(sym_2e_xpm);
  symbollist.append(QString("sym_2e"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 5);
  p1 = QPixmap(sym_2e_line_xpm);
  symbollist.append(QString("sym_2e_line"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 6);
  p1 = QPixmap(sym_ring_up_xpm);
  symbollist.append(QString("sym_ring_up"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 7);
  p1 = QPixmap(p_orbital_xpm);
  symbollist.append(QString("p_orbital"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 8);
  p1 = QPixmap(p_double_xpm);
  symbollist.append(QString("p_double_orbital"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 9);
  p1 = QPixmap(bead_xpm);
  symbollist.append(QString("bead"));
  symbolSub->insertItem( p1, this, SLOT(FromSymbolMenu(int)), 0, 10);

  symbollist.append(QString("antibody"));

  symbolSub->insertItem( QIconSet( QPixmap(sym_antibody) ), tr("Antibody"), this, SLOT(FromSymbolMenu(int)), 0, 11);

  symbollist.append(QString("newman"));
  symbolSub->insertItem( QIconSet( QPixmap(newman_xpm) ), tr("Newman projection symbol"), this, SLOT(FromSymbolMenu(int)), 0, 12);
  symbolSub->insertItem( QIconSet( QPixmap(newman_anti_xpm) ), tr("Newman projection - staggered (anti)"), r, SLOT(setMode_DrawNewmanAnti()) );
  symbolSub->insertItem( QIconSet( QPixmap(newman_eclipse_xpm) ), tr("Newman projection - eclipsed"), r, SLOT(setMode_DrawNewmanGauche()) );
  return symbolSub;
}

QPopupMenu *ApplicationWindow::BuildArrowMenu() {
  QPopupMenu * arrowSub = new QPopupMenu(this);
  QPixmap p1;
  p1 = QPixmap(arrow_regular_xpm);
  arrowlist.append(QString("REGULAR"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 0);
  p1 = QPixmap(arrow_topharpoon_xpm);
  arrowlist.append(QString("TOP_HARPOON"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 1);
  p1 = QPixmap(arrow_bottomharpoon_xpm);
  arrowlist.append(QString("BOTTOM_HARPOON"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 2);
  p1 = QPixmap(arrow_middle_xpm);
  arrowlist.append(QString("MIDDLE"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 3);
  p1 = QPixmap(arrow_didnt_work_xpm);
  arrowlist.append(QString("DIDNT_WORK"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 4);
  p1 = QPixmap(arrow_dash_xpm);
  arrowlist.append(QString("DASH"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 5);
  p1 = QPixmap(arrow_bi1_xpm);
  arrowlist.append(QString("BI1"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 6);
  p1 = QPixmap(arrow_bi2_xpm);
  arrowlist.append(QString("BI2"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 7);
  p1 = QPixmap(arrow_retro_xpm);
  arrowlist.append(QString("RETRO"));
  arrowSub->insertItem( p1, this, SLOT(FromArrowMenu(int)), 0, 8);
  return arrowSub;
}

QPopupMenu *ApplicationWindow::BuildCurveArrowMenu() {
  QPopupMenu * arrowSub = new QPopupMenu(this);
  QPixmap p1;
  p1 = QPixmap(cw90_xpm);
  curvearrowlist.append(QString("CW90"));
  arrowSub->insertItem( p1, this, SLOT(FromCurveArrowMenu(int)), 0, 0);
  p1 = QPixmap(ccw90_xpm);
  curvearrowlist.append(QString("CCW90"));
  arrowSub->insertItem( p1, this, SLOT(FromCurveArrowMenu(int)), 0, 1);
  p1 = QPixmap(cw180_xpm);
  curvearrowlist.append(QString("CW180"));
  arrowSub->insertItem( p1, this, SLOT(FromCurveArrowMenu(int)), 0, 2);
  p1 = QPixmap(ccw180_xpm);
  curvearrowlist.append(QString("CCW180"));
  arrowSub->insertItem( p1, this, SLOT(FromCurveArrowMenu(int)), 0, 3);
  p1 = QPixmap(cw270_xpm);
  curvearrowlist.append(QString("CW270"));
  arrowSub->insertItem( p1, this, SLOT(FromCurveArrowMenu(int)), 0, 4);
  p1 = QPixmap(ccw270_xpm);
  curvearrowlist.append(QString("CCW270"));
  arrowSub->insertItem( p1, this, SLOT(FromCurveArrowMenu(int)), 0, 5);
  arrowSub->insertItem( QIconSet( QPixmap(cbezier_xpm) ), tr("Cubic bezier"), 
			this, SLOT(CubicBezierTool(int)), 0, 6 );
  arrowSub->insertItem( QIconSet( QPixmap(cbezierhalf_xpm) ), 
			tr("Cubic bezier - half arrow"), 
			this, SLOT(CubicBezierTool(int)), 0, 7 );
  arrowSub->insertItem( QIconSet( QPixmap(cbezierfull_xpm) ), 
			tr("Cubic bezier - full arrow"), 
			this, SLOT(CubicBezierTool(int)), 0, 8 );
  return arrowSub;
}

QPopupMenu *ApplicationWindow::BuildBracketMenu() {
  QPopupMenu * bracketSub = new QPopupMenu(this);
  QPixmap p1;
  p1 = QPixmap(squarebracket_xpm);
  bracketlist.append(QString("SQUARE"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 0);
  p1 = QPixmap(curvebracket_xpm);
  bracketlist.append(QString("CURVE"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 1);
  p1 = QPixmap(bracebracket_xpm);
  bracketlist.append(QString("BRACE"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 2);
  p1 = QPixmap(boxbracket_xpm);
  bracketlist.append(QString("BOX"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 3);
  p1 = QPixmap(ellipsebracket_xpm);
  bracketlist.append(QString("ELLIPSE"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 4);

  p1 = QPixmap(closedsquarebracket_xpm);
  bracketlist.append(QString("CLOSEDSQUARE"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 5);
  p1 = QPixmap(circlebracket_xpm);
  bracketlist.append(QString("CIRCLE"));
  bracketSub->insertItem( p1, this, SLOT(FromBracketMenu(int)), 0, 6);
  return bracketSub;
}

// loads custom rings.
void ApplicationWindow::FromRingMenu(int x) {
  c->StartUndo(0,0);
  c->DeselectAll();
  c->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
  QString fname( ringlist[x] );
  fname.replace(fname.length() - 3, 3, QString("cml"));
  c->load( preferences.getCustomRingDir() + fname );
  r->Inserted();
}

void ApplicationWindow::updateCustomRingMenu() {
  ringmenu->removeItem(customRingMenuId);
  customRingMenuId = ringmenu->insertItem( tr("User-defined"), 
					   BuildCustomRingMenu() );
}

void ApplicationWindow::FromRingToolbar(QString fi) {
  c->DeselectAll();
  if (fi.contains("6ring") > 0) {
    r->setMode_DrawRing(RingDir + fi, fi.left(fi.length() - 4));
    return;
  }
  r->setMode_DrawRing(RingDir + fi, fi.left(fi.length() - 4), 1);
}

void ApplicationWindow::FromSymbolMenu(int x) {
  r->setMode_DrawSymbol( symbollist[x] );
}

void ApplicationWindow::FromArrowMenu(int x) {
  r->setMode_DrawArrow( arrowlist[x] );
}

void ApplicationWindow::FromCurveArrowMenu(int x) {
  r->setMode_DrawCurveArrow( curvearrowlist[x] );
}

void ApplicationWindow::FromBracketMenu(int x) {
  r->setMode_DrawBracket( bracketlist[x] );
}

void ApplicationWindow::setFixed_arrow()
{
  if (format->isItemChecked(fixlen_arrow)) {
    format->setItemChecked(fixlen_arrow, false);
    preferences.setArrow_fixed(false);
  } else {
    format->setItemChecked(fixlen_arrow, true);	
    preferences.setArrow_fixed(true);
  }
}

void ApplicationWindow::setFixed_bond()
{
  if (format->isItemChecked(fixlen_bond)) {
    format->setItemChecked(fixlen_bond, false);
    preferences.setBond_fixed(false);
  } else {
    format->setItemChecked(fixlen_bond, true);	
    preferences.setBond_fixed(true);
  }
}

void ApplicationWindow::setFix_Hydrogens()
{
  if (format->isItemChecked(fix_hydrogens)) {
    format->setItemChecked(fix_hydrogens, false);
    preferences.setFixHydrogens(false);
  } else {
    format->setItemChecked(fix_hydrogens, true);	
    preferences.setFixHydrogens(true);
  }
}

ApplicationWindow::~ApplicationWindow()
{
    delete printer;
}

void ApplicationWindow::SetFont(int m) {
  // read new font setting from combo boxes and pass to r  
  QFont f(fontList->currentText(), fontSizeList->currentText().toInt());
  if (fontList->currentText() == "Courier")
    f.setStyleHint(QFont::Courier);
  if (fontList->currentText() == "Helvetica")
    f.setStyleHint(QFont::Helvetica);
  if (fontList->currentText() == "Times")
    f.setStyleHint(QFont::Times);
  cout << f.family() << f.pointSize() << endl;
  r->SetFont(f);
}

void ApplicationWindow::SetColor(int m) {
  if (m == 0) r->SetColor(QColor(0,0,0));
  if (m == 1) r->SetColor(QColor(127,0,0));
  if (m == 2) r->SetColor(QColor(0,127,0));
  if (m == 3) r->SetColor(QColor(0,0,127));
}

void ApplicationWindow::SetThick(int t) {
  r->SetThick(t + 1);
}

void ApplicationWindow::ShowTextButtons(QFont infont) {
  edit->setItemVisible(insertsymbol_id, true);
  leftJustify->show();
  centerJustify->show();
  rightJustify->show();
  Bold->show();
  Italic->show();
  Underline->show();
  superScript->show();
  subScript->show();
  // set font combo boxes to object font (passed as infont)
  if (infont.family().lower() == QString("courier"))
    fontList->setCurrentItem(0);
  if (infont.family().lower() == QString("helvetica"))
    fontList->setCurrentItem(1);
  if (infont.family().lower() == QString("times"))
    fontList->setCurrentItem(2);
  if (infont.pointSize() == 8) fontSizeList->setCurrentItem(0);
  if (infont.pointSize() == 10) fontSizeList->setCurrentItem(1);
  if (infont.pointSize() == 12) fontSizeList->setCurrentItem(2);
  if (infont.pointSize() == 14) fontSizeList->setCurrentItem(3);
  if (infont.pointSize() == 18) fontSizeList->setCurrentItem(4);
  if (infont.pointSize() == 24) fontSizeList->setCurrentItem(5);
  if (infont.pointSize() == 32) fontSizeList->setCurrentItem(6);
}

void ApplicationWindow::HideTextButtons() {
  edit->setItemVisible(insertsymbol_id, false);
  leftJustify->hide();
  centerJustify->hide();
  rightJustify->hide();
  Bold->hide();
  Italic->hide();
  Underline->hide();
  superScript->hide();
  subScript->hide();
  // set font combo boxes to current font
  QFont infont = r->GetFont();
  if (infont.family().lower() == QString("courier"))
    fontList->setCurrentItem(0);
  if (infont.family().lower() == QString("helvetica"))
    fontList->setCurrentItem(1);
  if (infont.family().lower() == QString("times"))
    fontList->setCurrentItem(2);
  if (infont.pointSize() == 8) fontSizeList->setCurrentItem(0);
  if (infont.pointSize() == 10) fontSizeList->setCurrentItem(1);
  if (infont.pointSize() == 12) fontSizeList->setCurrentItem(2);
  if (infont.pointSize() == 14) fontSizeList->setCurrentItem(3);
  if (infont.pointSize() == 18) fontSizeList->setCurrentItem(4);
  if (infont.pointSize() == 24) fontSizeList->setCurrentItem(5);
  if (infont.pointSize() == 32) fontSizeList->setCurrentItem(6);
}

void ApplicationWindow::newDoc()
{
  ApplicationWindow *ed = new ApplicationWindow;
  ed->setClipboard(r->getClipboard());
  ed->show();
  ed->HideTextButtons();
}

void ApplicationWindow::load()
{
  OBImport();
  return;

  /* DELETE ME
  QFileDialog fd(QString::null, QString::null, 0, 0, TRUE);
  fd.setCaption("Load file");
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
  OBNewLoad(fileName, "--Select a filter-- (*)");
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
    statusBar()->message(tr("Unable to load ") + fileName);
    return;
  }
  f.close();
  SelectAll();
  Clear();

  if ( !c->load(fileName) ) {
    statusBar()->message(tr("Unable to load ") + fileName);
    return;
  }
  
  fileName = realFileName;
  setCaption( QString(XDC_VERSION) + QString(" - ") + fileName );
  statusBar()->message( tr("Loaded document ") + fileName );
  filename = fileName;
  c->DeselectAll();
  r->repaint(false);
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

  if ( !c->save(filename) ) {
    statusBar()->message( tr("Could not write to ") + filename );
    return;
  }

  setCaption( QString(XDC_VERSION) + QString(" - ") + filename );
  
  statusBar()->message( tr( "Saved file ") + filename );
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
  QString picfile;

  MyFileDialog fd(0, QString::null);
  fd.setCaption(tr("Save as picture..."));
  fd.setMode(QFileDialog::AnyFile);
  QStringList picfilters;
  picfilters.append("Portable Network Graphic (*.png)");
  picfilters.append("Windows Bitmap (*.bmp)");
  picfilters.append("Encapsulated PostScript (*.eps)");
  picfilters.append("Scalable Vector Graphics (*.svg)");
  fd.setFilters(picfilters);  
  if ( fd.exec() == QDialog::Accepted ) {
    picfile = fd.selectedFile();
    QString tmpx = picfile.right(4).lower();
    if (tmpx == QString(".png")) pm = 1;
    if (tmpx == QString(".bmp")) pm = 2;
    if (tmpx == QString(".svg")) pm = 3;
    if ( (tmpx != QString(".png")) &&
	 (tmpx != QString(".bmp")) &&
	 (tmpx != QString(".svg")) &&
	 (tmpx != QString(".eps")) ) {
      if (fd.selectedFilter().left(3) == QString("Por")) {
	picfile.append(".png"); pm = 1;
      }
      if (fd.selectedFilter().left(3) == QString("Win")) {
	picfile.append(".bmp"); pm = 2;
      }
      if (fd.selectedFilter().left(3) == QString("Enc")) {
	picfile.append(".eps");
      }
      if (fd.selectedFilter().left(3) == QString("Sca")) {
	picfile.append(".svg"); pm = 3;
      }
    }
    if (pm == 3) {
      // save as Scalable Vector Graphics
      was_saved = r->SaveSVG(picfile);
      if (was_saved)
	statusBar()->message( tr("Saved picture file ") + picfile );
      else
	statusBar()->message(tr("Unable to save picture!"));
      return;
    }
    if (pm != 0) {
      // check for "convert" (ImageMagick)
      bool imagemagick = false;
      QString n1, cmd1;
      n1.setNum( getpid() );
      n1.prepend("/tmp/");
      cmd1 = "convert > " + n1;
      system( cmd1.ascii() );

      QFile f1( n1 );
      f1.open(IO_ReadOnly);
      f1.readLine(cmd1, 256);

      if (cmd1.contains("ImageMagick") > 0) {
	std::cout << "ImageMagick is present" << std::endl;
	imagemagick = true;
      }

      cmd1 = "rm " + n1;
      system( cmd1.ascii() );

      QPixmap tosave;
      // use "tosave" with ImageMagick to specify size
      tosave = r->MakePixmap( fd.isTransparent() );
      QString sizehint;
      n1.setNum( tosave.width() );
      sizehint = n1;
      sizehint.append("x");
      n1.setNum( tosave.height() );
      sizehint.append(n1);
      if (imagemagick) {
	n1 = picfile;
	n1.append(".eps");
	r->SaveEPS(n1);
	//cmd1 = "convert -antialias " + n1 + " " + picfile;
	cmd1 = "convert -antialias -size " +sizehint + " " + n1 + " -resize " 
	  + sizehint + " " + picfile;
	system( cmd1.ascii() );
	cmd1 = "rm " + n1;
	system( cmd1.ascii() );
	was_saved = true;
      } else {
	tosave = r->MakePixmap( fd.isTransparent() );
	if (pm == 1) // PNG
	  was_saved = tosave.save(picfile, "PNG");
	if (pm == 2) // BMP
	  was_saved = tosave.save(picfile, "BMP");
      }
      if (was_saved)
	statusBar()->message( tr("Saved picture file ") + picfile );
      else
	statusBar()->message(tr("Unable to save picture!"));
    } else { // save as EPS
      was_saved = r->SaveEPS(picfile);
      if (was_saved)
	statusBar()->message( tr("Saved picture file ") + picfile );
      else
	statusBar()->message(tr("Unable to save picture!"));
    }
  }
}

void ApplicationWindow::SavePNG() {
  bool was_saved;

   // check for "convert" (ImageMagick)
  bool imagemagick = false;
  QString n1, cmd1;
  n1.setNum( getpid() );
  n1.prepend("/tmp/");
  cmd1 = "convert > " + n1;
  system( cmd1.ascii() );
  
  QFile f1( n1 );
  f1.open(IO_ReadOnly);
  f1.readLine(cmd1, 256);

  if (cmd1.contains("ImageMagick") > 0) {
    std::cout << "ImageMagick is present" << std::endl;
    imagemagick = true;
  }

  cmd1 = "rm " + n1;
  system( cmd1.ascii() );

  QPixmap tosave;
  tosave = r->MakePixmap( ni_tflag );
  QString sizehint;
  n1.setNum( tosave.width() );
  sizehint = n1;
  sizehint.append("x");
  n1.setNum( tosave.height() );
  sizehint.append(n1);
 if (imagemagick) {
    n1 = ni_savefile;
    n1.append(".eps");
    r->SaveEPS(n1);
    cmd1 = "convert -antialias -size " +sizehint + " " + n1 + " -resize " 
      + sizehint + " " + ni_savefile;
    system( cmd1.ascii() );
    cmd1 = "rm " + n1;
    system( cmd1.ascii() );
    was_saved = true;
  } else {
    was_saved = tosave.save(ni_savefile, "PNG");
  }
  
  //QPixmap tosave = r->MakePixmap( ni_tflag );
  //was_saved = tosave.save(ni_savefile, "PNG");
  if (was_saved == false) std::cout << "save PNG failed" << std::endl;
  close();  // this function is only used in non-interactive mode
}

void ApplicationWindow::Save3D() {
  bool was_saved;
  c->Save3D( ni_savefile );
  close();  // this function is only used in non-interactive mode
}

void ApplicationWindow::print()
{
  r->Print();
}

void ApplicationWindow::closeEvent( QCloseEvent* ce )
{
    if ( !c->edited() ) {
	ce->accept();
	return;
    }

    switch( QMessageBox::information( this, "XDrawChem question",
				      "The document has been changed since "
				      "the last save.",
				      "Save Now", "Cancel", "Leave Anyway",
				      0, 1 ) ) {
    case 0:
	save();
	ce->accept();
	break;
    case 1:
    default: // just for sanity
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
			QString(XDC_VERSION) + tr("\nBryan Herger\nbherger@users.sourceforge.net\n\nPlease subscribe to the mailing list for information about future releases.\nSend a message to xdrawchem-announce-request@lists.sourceforge.net with 'subscribe' as the subject.\n\nXDrawChem is copyright (C) 2004 Bryan Herger.\nPortions copyright (C) 1997-2000 Dr. Christoph Steinbeck and the JChemPaint project\nOpenBabel code copyright (C) 2003 by the OpenBabel project team.\nSee file COPYRIGHT.txt for more details") );
}

void ApplicationWindow::support() {
  QMessageBox::information( 0, tr("How to get help"),
			    tr("Current information on XDrawChem can always be found at\nhttp://xdrawchem.sourceforge.net/\nThe latest release will be posted here, as well as links to mailing lists and the bug tracker.\n\nPlease submit bugs using the SourceForge tracker: http://www.sourceforge.net/tracker/?group_id=34518\n\nThere are two mailing lists:  xdrawchem-announce, where new releases will be announced,\nand xdrawchem-user, for open discussion among XDrawChem users.\nSubscribe by sending a blank e-mail with subject \"subscribe\" to \n\"xdrawchem-announce-request@lists.sourceforge.net\" or\n\"xdrawchem-user-request@lists.sourceforge.net\"\n\nYou can contact the author directly at\nbherger@users.sourceforge.net") );
}

const char * reflist = 
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
"Lin S and Sandler SI, J. Phys. Chem. A, 2000, vol. 104, 7099-7105\n\n"
"Steinbeck C, JMDraw (software), http://jmdraw.sourceforge.net/\n\n"
"Molecules in the XDrawChem database are from the NCI Open Database, October 1999 release:  http://cactvs.cit.nih.gov/";

void ApplicationWindow::Refs()
{
  QMessageBox::about( this, "XDrawChem References", reflist);
}

void ApplicationWindow::SetStatusBar(QString s) {
  statusBar()->message(s);
}

/* obsolete.
void ApplicationWindow::MakeRingDialog() {
  RingDialog t(this, "ring dialog");
  if ( !t.exec() ) return;

  c->StartUndo(0,0);
  c->DeselectAll();
  c->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
  c->load( RingDir + t.getFile() );
  r->Inserted();
}
*/

void ApplicationWindow::ShowFixedDialog(void) {
  FixedDialog i(this, QString("fixed_dialog"));
  if ( !i.exec() ) return;
  hruler->repaint();
  vruler->repaint();
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

void ApplicationWindow::MakeNetDialog() {
  NetDialog n(this, "net dialog");
  if ( !n.exec() ) return;
  cout << "Server:" << n.getServer() << endl;
  cout << "Key   :" << n.getKey() << endl;
  cout << "Value :" << n.getValue() << endl;
  NetAccess na;
  setCursor(waitCursor);
  r->setWaitCursor();
  QStringList choices = na.getChoices(n.getServer(), n.getKey(), n.getValue(),
				      n.getExact() );
  setCursor(arrowCursor);
  r->setArrowCursor();
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
  NetChooseDialog nc(this, "net choose dialog", choices);
  if ( !nc.exec() ) return;
  cout << nc.getFile() << endl;
  // TODO: insert SMILES string returned by database
  c->fromSMILES(nc.getFile());
  r->Inserted();
  r->repaint(false);

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

void ApplicationWindow::PageSetup() {
  PageSetupDialog p(this, "page dialog");
  //p.setPageSize(preferences.getPageSize());
  //p.setOrientation(preferences.getPageOrientation());
  if ( !p.exec() ) return;
  //preferences.setPageSize(p.getPageSize());
  //preferences.setPageOrientation(p.getOrientation());
  r->UpdatePageGeometry();
}

void ApplicationWindow::NewColor() {
  QColor nc1 = QColorDialog::getColor(r->GetColor());
  if (nc1.isValid()) {
    r->SetColor(nc1);
    QPixmap *tmp_pm = new QPixmap(32, 20);
    tmp_pm->fill(QColor(210,210,210));
    QPainter pixpaint(tmp_pm);
    pixpaint.fillRect(2, 3, 14, 14, nc1);
    pixpaint.setPen(QColor(0,0,0));
    pixpaint.drawLine( 24, 2, 24, 16 );
    pixpaint.drawLine( 24, 16, 20, 12 );
    pixpaint.drawLine( 24, 16, 28, 12 );
    pb1->setPixmap(*tmp_pm);    
  }
}

void ApplicationWindow::BackgroundColor() {
  QColor nc1 = QColorDialog::getColor(r->getBGColor());
  if (nc1.isValid()) {
    r->setBGColor(nc1);
    r->repaint(false);
  }
}

// peptide builder
void ApplicationWindow::PeptideBuilder() {
  PeptDialog p1(this, "builder");
  if ( !p1.exec() ) return;
  QString seq1 = p1.getPeptide();
  cout << "peptide: " << seq1 << endl;
}

// help from help menu
void ApplicationWindow::NewManual() {
  QString home;
#ifdef UNIX
  home = RingDir + "doc/index.html";
#else
  home = RingDir + "doc\\index.html";
#endif
  HelpBrowser(home);
}

// invoke context-sensitive help
void ApplicationWindow::HelpTopic(QString topic) {
  QString home;
#ifdef UNIX
  home = RingDir + "doc/" + topic;
#else
  home = RingDir + "doc\\" + topic;
#endif
  HelpBrowser(home);
}

// new manual (based on QTextBrowser) -- derived from Qt example "helpviewer"
/****************************************************************************
** $Id: application.cpp 187 2005-11-16 03:01:55Z bherger $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/
void ApplicationWindow::HelpBrowser(QString home) {
    HelpWindow *help = new HelpWindow(home, ".", 0, "help viewer");
    help->setCaption(QString(XDC_VERSION) + " - Help viewer");
    if ( QApplication::desktop()->width() > 400
         && QApplication::desktop()->height() > 500 )
        help->show();
    else
        help->showMaximized();
}
//
// end code covered by "helpviewer" example copyright
//

// pass thru to Render2D
void ApplicationWindow::Cut() { r->Cut(); }
void ApplicationWindow::Copy() { r->Copy(); }
void ApplicationWindow::Paste() { r->Paste(); }
void ApplicationWindow::Undo() { r->Undo(); }
void ApplicationWindow::FlipH() { r->Flip(FLIP_H); }
void ApplicationWindow::FlipV() { r->Flip(FLIP_V); }
void ApplicationWindow::Rotate90() { r->Rotate90(); }
void ApplicationWindow::Rotate180() { r->Rotate180(); }
void ApplicationWindow::Rotate270() { r->Rotate270(); }
void ApplicationWindow::Clear() { r->EraseSelected(); }
void ApplicationWindow::MoleculeInfo() { r->Tool(MODE_TOOL_MOLECULE_INFO); }
void ApplicationWindow::CalcEA() { r->Tool(MODE_TOOL_CALCEA); }
void ApplicationWindow::CalcEF() { r->Tool(MODE_TOOL_CALCEF); }
void ApplicationWindow::CalcMW() { r->Tool(MODE_TOOL_CALCMW); }
void ApplicationWindow::Calc13CNMR() { r->Tool(MODE_TOOL_13CNMR); }
void ApplicationWindow::Calc1HNMR() { r->Tool(MODE_TOOL_1HNMR); }
void ApplicationWindow::CalcIR() { r->Tool(MODE_TOOL_IR); }
void ApplicationWindow::CalcpKa() { r->Tool(MODE_TOOL_PKA); }
void ApplicationWindow::CalcKOW() { r->Tool(MODE_TOOL_KOW); }
void ApplicationWindow::CalcName() { r->Tool(MODE_TOOL_NAME); }
void ApplicationWindow::ToSMILES() { r->Tool(MODE_TOOL_TOSMILES); }
void ApplicationWindow::ToInChI() { r->Tool(MODE_TOOL_TOINCHI); }
void ApplicationWindow::To3D() { r->Tool(MODE_TOOL_2D3D); }
void ApplicationWindow::CleanUpMolecule() { r->Tool(MODE_TOOL_CLEANUPMOL); }
void ApplicationWindow::saveCustomRing() { r->Tool(MODE_TOOL_CUSTOMRING); }
void ApplicationWindow::SelectAll() { r->SelectAll(); }
void ApplicationWindow::DeselectAll() { r->DeselectAll(); }
void ApplicationWindow::AutoLayout() { r->AutoLayout(); }
void ApplicationWindow::DrawRegularArrow() {
  r->setMode_DrawArrow( "REGULAR" );
}
void ApplicationWindow::DrawSquareBracket() {
  r->setMode_DrawBracket( "SQUARE" );
}
void ApplicationWindow::CubicBezierTool(int x) {
  std::cout << "Bezier:" << x - 6 << std::endl;
  r->setMode_DrawGraphicObject( TYPE_BEZIER, x - 6 );
}
void ApplicationWindow::setClipboard(Clipboard *clip1)
 { r->setClipboard(clip1); }
void ApplicationWindow::setGroup_Reactant() { 
  r->Tool(MODE_TOOL_GROUP_REACTANT);
}
void ApplicationWindow::setGroup_Product() { 
  r->Tool(MODE_TOOL_GROUP_PRODUCT);
}
void ApplicationWindow::clearGroup() { 
  r->Tool(MODE_TOOL_GROUP_CLEAR);
}
void ApplicationWindow::Retro() {
  r->Tool(MODE_TOOL_RETRO);
}
void ApplicationWindow::RetroAtomName() {
  r->Tool(MODE_TOOL_RETRO_ATOMNAME);
}
void ApplicationWindow::RetroBondName() {
  r->Tool(MODE_TOOL_RETRO_BONDNAME);
}
void ApplicationWindow::InsertSymbol() {
  r->InsertSymbol();
}
void ApplicationWindow::Test() {
  CharSelDialog cd1(this, "charsel");
  cd1.exec();
  //r->Tool(MODE_TOOL_TEST);
}
void ApplicationWindow::clearAllGroups() { 
  r->clearAllGroups();
}
void ApplicationWindow::reactionAnalysisTest() {
  r->ReactionAnalysis(RXN_TEST);
}
void ApplicationWindow::reactionAnalysisEnthalpy() {
  r->ReactionAnalysis(RXN_ENTHALPY_ESTIMATE);
}
void ApplicationWindow::reactionAnalysis1HNMR() {
  r->ReactionAnalysis(RXN_1HNMR);
}
void ApplicationWindow::reactionAnalysis13CNMR() {
  r->ReactionAnalysis(RXN_13CNMR);
}
void ApplicationWindow::reactivityForward() {
  r->Tool(MODE_TOOL_REACTIVITY_FORWARD);
}
void ApplicationWindow::reactivityRetro() {
  r->Tool(MODE_TOOL_REACTIVITY_RETRO);
}
void ApplicationWindow::reactivityPC() {
  r->Tool(MODE_TOOL_CHARGES);
}

void ApplicationWindow::toggleGrid() {
  int i1;
  i1 = preferences.getDrawGrid();
  i1++;
  if (i1 > 2) i1 = 0;
  preferences.setDrawGrid(i1);
  r->repaint(false);
}
void ApplicationWindow::svXY(int x1, int y1) {
  hruler->setRange(x1, x1+cw->width());
  hruler->repaint();
  vruler->setRange(y1, y1+cw->height());
  vruler->repaint();
}
void ApplicationWindow::showDYK() {
  DYKDialog dyk1;
  dyk1.exec();
}
void ApplicationWindow::MagnifyPlus() {
  preferences.setZoom( preferences.getZoom() + 25 );
  if (preferences.getZoom() > 200) preferences.setZoom(200);
  r->zoomEvent();
  hruler->zoomEvent();
  vruler->zoomEvent();
  SetStatusBar( tr("Zoom = %1 %").arg(preferences.getZoom()) );
}
void ApplicationWindow::MagnifyMinus() {
  preferences.setZoom( preferences.getZoom() - 25 );
  if (preferences.getZoom() < 25) preferences.setZoom(25);
  r->zoomEvent();
  hruler->zoomEvent();
  vruler->zoomEvent();
  SetStatusBar( tr("Zoom = %1 %").arg(preferences.getZoom()) );
}
void ApplicationWindow::Magnify100() {
  preferences.setZoom( 100 );
  r->zoomEvent();
  hruler->zoomEvent();
  vruler->zoomEvent();
  SetStatusBar( tr("Zoom = %1 %").arg(preferences.getZoom()) );
}
void ApplicationWindow::Zoom(int z1) {

  r->zoomEvent();
  hruler->zoomEvent();
  vruler->zoomEvent();
  SetStatusBar( tr("Zoom = %1 %").arg(preferences.getZoom()) );
}
void ApplicationWindow::XDCSettings() {
  bool ok; QFont font;
  switch( QMessageBox::information( this, tr("XDC Settings"),
				    tr("Change XDrawChem settings:"),
				    tr("&Main font"), tr("&Ruler font"), 
				    tr("Cancel"),
				    2,      // Enter == button 2
				    2 ) ) { // Escape == button 2
  case 0: 
    font = QFontDialog::getFont(&ok, preferences.getMainFont(), this );
    if ( ok ) {
      preferences.setMainFont(font);
      setFont(font);
      repaint();
    } else {
      return;
    }
    break;
  case 1: 
    font = QFontDialog::getFont(&ok, preferences.getRulerFont(), this );
    if ( ok ) {
      preferences.setRulerFont(font);
      //setFont(font);
      hruler->repaint(); vruler->repaint();
      repaint();
    } else {
      return;
    }
    break;
  case 2: 
    return;
    break;
  }
}
