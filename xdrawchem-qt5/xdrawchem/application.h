/****************************************************************************
** $Id: application.h,v 1.15 2005/10/30 23:47:31 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <QtGlobal>
#include <QString>
#include <QLabel>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QComboBox>
#include <QAction>
#include <QPrinter>
#include <QToolBar>
#include <QMainWindow>

class RenderArea;
class Render2D;
class ChemData;
class Clipboard;
class Clipboard;
class XRuler;
class NetDialog;
class ColorButton;

class ApplicationWindow : public QMainWindow
{
    Q_OBJECT

public:
    ApplicationWindow();
    ~ApplicationWindow();
    QString ni_savefile;
    bool ni_tflag;

protected:
    void closeEvent( QCloseEvent * );

public slots:
    void newDoc();
    void load();
    void load( QString );
    void save();
    void saveAs();
    void savePicture();
    void externalProgram();
    void savePNG();
    void save3D();
    void pageSetup();
    void print();
    //void MakeRingDialog();  *** obsolete.  removed.
    void MakeNetDialog();
    void slotChoicesFinished(const QStringList &);
    void ShowFixedDialog();
    void showDYK();
    void Cut();
    void Copy();
    void Paste();
    void Undo();
    void Clear();
    void SelectAll();
    void DeselectAll();
    void showTextButtons( QFont );
    void HideTextButtons();
    void SetStatusBar( const QString & );
//    void setRenderFont();
    void SetColor( int );
    void NewColor();
    void BackgroundColor();
    void SetThick( int );
    void FromRingMenu( int );
    void FromNewRingMenu( int ); // in application_ring.cpp
    void FromRingToolbar( QString );
    void FromSymbolMenu( QAction * );
    void FromArrowMenu( QAction * );
    void FromCurveArrowMenu( QAction * );
    void FromBracketMenu( QAction * );
    void FromNewerRingMenu( QAction * );
    void CubicBezierTool(int);
    void FlipH();
    void FlipV();
    void Rotate90();
    void Rotate180();
    void Rotate270();
    void setFixed_arrow();
    void setFixed_bond();
//    void setFix_Hydrogens();
    void MoleculeInfo();
    void CalcEA();
    void CalcEF();
    void CalcMW();
    void Calc13CNMR();
    void Calc1HNMR();
    void CalcIR();
    void CalcpKa();
    void CalcKOW();
    void To3D();
    void CalcName();
    void AutoLayout();
    void PeptideBuilder();
    void saveCustomRing();
    void updateCustomRingMenu();
    void DrawRegularArrow();
    void DrawSquareBracket();
    void CleanUpMolecule();
    void Retro();
    void RetroAtomName();
    void RetroBondName();
    void reactionAnalysisTest();
    void reactionAnalysisEnthalpy();
    void reactionAnalysis1HNMR();
    void reactionAnalysis13CNMR();
    void reactivityForward();
    void reactivityRetro();
    void reactivityPC();
    void ToSMILES();
    void ToInChI();
    void FromSMILES(); // defined in smilesdialog.cpp
    void setGroup_Reactant();
    void setGroup_Product();
    void clearGroup();
    void clearAllGroups();
    void toggleGrid();
    void svXY(int, int);
    void MagnifyPlus();
    void MagnifyMinus();
    void Magnify100();
    void Zoom(int);
    void XDCSettings();
    void InsertSymbol();

    void OBGetFilters();
    void OBImport();
    void OBNewLoad(QString, QString);
    void OBExport();
    void OBNewSave();
    QString OBGuessFileType(QString);

    void about();
    void support();
    void whatsThis();
    void NewManual();
    void HelpBrowser(QString);
    void HelpTopic(QString);
    void Refs();
    void Dummy() { }
    void Test();
    void setClipboard(Clipboard *);

private slots:
    void setRegularArrowAction(QAction *);
    void setCurveArrowAction(QAction *);
    void setBracketAction(QAction *);
    void setSymbolAction( QAction * );
    void setRingAction( QAction * );
    void setFontFace( int );
    void setFontPoints( int );

private:
    //QPopupMenu *BuildRingMenu();  obsolete...
    QMenu *BuildCustomRingMenu();
    QMenu *BuildArrowMenu();
    QMenu *BuildCurveArrowMenu();
    QMenu *BuildBracketMenu();
    QMenu *BuildNewRingMenu(); // in application_ring.cpp
    QMenu *BuildSymbolMenu();

    QWidget *m_centralWidget;
    XRuler *hruler, *vruler;
    QLabel *m_rulerUnitLabel;
    QMenu *edit, *format;
    QMenu *ringmenu;
    QToolBar *BuildBioTools();
    int fix_hydrogens;
    QPrinter *printer;
    RenderArea *m_renderArea;
    Render2D *m_renderer;
    ChemData *m_chemData;
    QToolBar *fileTools, *drawTools, *ringTools, *bioTools;
    QString filename, filefilter;
    QToolButton *Bold, *TextUnderline, *Italic, *Underline, *superScript, *subScript,
                *leftJustify, *centerJustify, *rightJustify;

    ColorButton *colorBtn;

    QAction *justifyLeftAction, *justifyCenterAction, *justifyRightAction,
            *boldAction, *italicAction, *underlineAction, *superscriptAction, *subscriptAction,
            *insertSymbolAction, *fixedBondAction, *fixedArrowAction;

    /// regular arrows
    QToolButton *drawArrowButton;
    QMenu *regularArrowMenu;
    QAction *regularArrowAction, *topharpoonArrowAction, * bottomharpoonAction, * middleArrowAction,
            *didntworkArrowAction, * dashedArrowAction, * bi1ArrowAction, * bi2ArrowAction, * retroArrowAction;

    /// curve arrows
    QToolButton *drawCurveArrowButton;
    QMenu *curveArrowMenu;
    QAction *carrowCW90Action, *carrowCCW90Action, *carrowCW180Action, *carrowCCW180Action,
            *carrowCW270Action, *carrowCCW270Action, *carrowBezierAction, *carrowBezierhalfAction,
            *carrowBezierfullAction;

    /// brackets
    QToolButton *drawBracketButton;
    QMenu *bracketMenu;
    QAction *squareBracketAction, *curveBracketAction, *braceBracketAction, *boxBracketAction,
            *ellipseBracketAction, *closedsquareBracketAction, *circleBracketAction;

    /// symbols
    QToolButton *drawSymbolButton;
    QMenu *symbolMenu;
    QAction *symbolPlusAction, *symbolMinusAction, *symbolDeltaPlusAction, *symbolDeltaMinusAction,
            *symbol1EAction, *symbol2EAction, *symbol2ELineAction, *symbolRingUpAction,
            *symbolPOrbitalAction, *symbolPDoubleAction, *symbolBeadAction, *symbolAntibodyAction,
            *symbolNewmanAction, *symbolNewmanAntiAction, *symbolNewmanEclipseAction;

    /// rings
    QToolButton *drawRingButton;
    QMenu *ringMenu;
    QAction *ring3Action, *ring4Action, *ring5Action, *ring6Action, *ringBenzeneAction, *ringImidazoleAction, *ringCyclopentadieneAction, *ring6BoatAction, *ring6ChairAction, *ringPyrimidineAction, *ring7Action, *ring8Action, *ringIndoleAction, *ringNaphAction, *ringPurineAction, *ringBiphenylAction, *ringAnthraAction, *ringSteroidAction;
    QAction *naAdenineAction, *naCytosineAction, *naGuanineAction, *naThymineAction, *naUracilAction;
    QAction *srAction, *sdAction, *sfAction, *sgAction;
    QAction *customRingMenuAction;

    QComboBox *colorList, *fontList, *fontSizeList, *ltList;
    QStringList ringlist, symbollist, arrowlist, curvearrowlist, bracketlist;
    QStringList filters, readFilters, writeFilters;
    NetDialog *netDlg;
};

#endif

