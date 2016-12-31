/****************************************************************************
** $Id: application.h 185 2005-10-30 23:47:31Z bherger $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef APPLICATION_H
#define APPLICATION_H

#include <qmainwindow.h>
#include <qstringlist.h>

class QMultiLineEdit;
class QScrollView;
class QToolBar;
class QPopupMenu;
class Render2D;
class ChemData;
class Clipboard;
class QComboBox;
class QPushButton;
class QToolButton;
class Clipboard;
class XRuler;

class ApplicationWindow: public QMainWindow
{
    Q_OBJECT
public:
    ApplicationWindow();
    ~ApplicationWindow();
    QString ni_savefile;
    bool ni_tflag;
    
protected:
    void closeEvent( QCloseEvent* );

public slots:
    void newDoc();
    void load();
    void load( QString );
    void save();
    void saveAs();
    void savePicture();
    void externalProgram();
    void SavePNG();
    void Save3D();
    void PageSetup();
    void print();
    //void MakeRingDialog();  *** obsolete.  removed.
    void MakeNetDialog();
    void ShowFixedDialog();
    void showDYK();
    void Cut();
    void Copy();
    void Paste();
    void Undo();
    void Clear();
    void SelectAll();
    void DeselectAll();
    void ShowTextButtons(QFont);
    void HideTextButtons();
    void SetStatusBar(QString);
    void SetFont(int);
    void SetColor(int);
    void NewColor();
    void BackgroundColor();
    void SetThick(int);
    void FromRingMenu(int);
    void FromNewRingMenu(int); // in application_ring.cpp
    void FromRingToolbar(QString);
    void FromSymbolMenu(int);
    void FromArrowMenu(int);
    void FromCurveArrowMenu(int);
    void FromBracketMenu(int);
    void CubicBezierTool(int);
    void FlipH();
    void FlipV();
    void Rotate90();
    void Rotate180();
    void Rotate270();
    void setFixed_arrow();
    void setFixed_bond();
    void setFix_Hydrogens();
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
    void NewManual();
    void HelpBrowser(QString);
    void HelpTopic(QString);
    void Refs();
    void Dummy() { }
    void Test();
    void setClipboard(Clipboard *);

private:
    QWidget *cw;
    XRuler *hruler, *vruler;
    //QPopupMenu *BuildRingMenu();  obsolete...
    QPopupMenu *BuildNewRingMenu(); // in application_ring.cpp
    QPopupMenu *BuildCustomRingMenu();
    QPopupMenu *BuildArrowMenu();
    QPopupMenu *BuildCurveArrowMenu();
    QPopupMenu *BuildSymbolMenu();
    QPopupMenu *BuildBracketMenu();
    QPopupMenu *format, *ringmenu, *edit;
    QToolBar *BuildBioTools();
    QPushButton *pb1;
    int fixlen_bond, fixlen_arrow, fix_hydrogens, customRingMenuId;
    int insertsymbol_id;
    QPrinter *printer;
    QScrollView *sv;
    Render2D *r;
    ChemData *c;
    QToolBar *fileTools, *drawTools, *ringTools, *bioTools;
    QString filename, filefilter;
    QToolButton *Bold, *Italic, *Underline, *superScript, *subScript,
      *leftJustify, *centerJustify, *rightJustify;
    QComboBox *colorList, *fontList, *fontSizeList, *ltList;
    QStringList ringlist, symbollist, arrowlist, curvearrowlist, bracketlist;
    QStringList filters, readFilters, writeFilters;
};


#endif
