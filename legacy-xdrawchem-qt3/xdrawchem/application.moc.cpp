/****************************************************************************
** ApplicationWindow meta object code from reading C++ file 'application.h'
**
** Created: Wed Nov 23 20:29:32 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "application.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ApplicationWindow::className() const
{
    return "ApplicationWindow";
}

QMetaObject *ApplicationWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ApplicationWindow( "ApplicationWindow", &ApplicationWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ApplicationWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ApplicationWindow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ApplicationWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ApplicationWindow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ApplicationWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUMethod slot_0 = {"newDoc", 0, 0 };
    static const QUMethod slot_1 = {"load", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"load", 1, param_slot_2 };
    static const QUMethod slot_3 = {"save", 0, 0 };
    static const QUMethod slot_4 = {"saveAs", 0, 0 };
    static const QUMethod slot_5 = {"savePicture", 0, 0 };
    static const QUMethod slot_6 = {"externalProgram", 0, 0 };
    static const QUMethod slot_7 = {"SavePNG", 0, 0 };
    static const QUMethod slot_8 = {"Save3D", 0, 0 };
    static const QUMethod slot_9 = {"PageSetup", 0, 0 };
    static const QUMethod slot_10 = {"print", 0, 0 };
    static const QUMethod slot_11 = {"MakeNetDialog", 0, 0 };
    static const QUMethod slot_12 = {"ShowFixedDialog", 0, 0 };
    static const QUMethod slot_13 = {"showDYK", 0, 0 };
    static const QUMethod slot_14 = {"Cut", 0, 0 };
    static const QUMethod slot_15 = {"Copy", 0, 0 };
    static const QUMethod slot_16 = {"Paste", 0, 0 };
    static const QUMethod slot_17 = {"Undo", 0, 0 };
    static const QUMethod slot_18 = {"Clear", 0, 0 };
    static const QUMethod slot_19 = {"SelectAll", 0, 0 };
    static const QUMethod slot_20 = {"DeselectAll", 0, 0 };
    static const QUParameter param_slot_21[] = {
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_21 = {"ShowTextButtons", 1, param_slot_21 };
    static const QUMethod slot_22 = {"HideTextButtons", 0, 0 };
    static const QUParameter param_slot_23[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_23 = {"SetStatusBar", 1, param_slot_23 };
    static const QUParameter param_slot_24[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_24 = {"SetFont", 1, param_slot_24 };
    static const QUParameter param_slot_25[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_25 = {"SetColor", 1, param_slot_25 };
    static const QUMethod slot_26 = {"NewColor", 0, 0 };
    static const QUMethod slot_27 = {"BackgroundColor", 0, 0 };
    static const QUParameter param_slot_28[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_28 = {"SetThick", 1, param_slot_28 };
    static const QUParameter param_slot_29[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_29 = {"FromRingMenu", 1, param_slot_29 };
    static const QUParameter param_slot_30[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_30 = {"FromNewRingMenu", 1, param_slot_30 };
    static const QUParameter param_slot_31[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_31 = {"FromRingToolbar", 1, param_slot_31 };
    static const QUParameter param_slot_32[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_32 = {"FromSymbolMenu", 1, param_slot_32 };
    static const QUParameter param_slot_33[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_33 = {"FromArrowMenu", 1, param_slot_33 };
    static const QUParameter param_slot_34[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_34 = {"FromCurveArrowMenu", 1, param_slot_34 };
    static const QUParameter param_slot_35[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_35 = {"FromBracketMenu", 1, param_slot_35 };
    static const QUParameter param_slot_36[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_36 = {"CubicBezierTool", 1, param_slot_36 };
    static const QUMethod slot_37 = {"FlipH", 0, 0 };
    static const QUMethod slot_38 = {"FlipV", 0, 0 };
    static const QUMethod slot_39 = {"Rotate90", 0, 0 };
    static const QUMethod slot_40 = {"Rotate180", 0, 0 };
    static const QUMethod slot_41 = {"Rotate270", 0, 0 };
    static const QUMethod slot_42 = {"setFixed_arrow", 0, 0 };
    static const QUMethod slot_43 = {"setFixed_bond", 0, 0 };
    static const QUMethod slot_44 = {"setFix_Hydrogens", 0, 0 };
    static const QUMethod slot_45 = {"MoleculeInfo", 0, 0 };
    static const QUMethod slot_46 = {"CalcEA", 0, 0 };
    static const QUMethod slot_47 = {"CalcEF", 0, 0 };
    static const QUMethod slot_48 = {"CalcMW", 0, 0 };
    static const QUMethod slot_49 = {"Calc13CNMR", 0, 0 };
    static const QUMethod slot_50 = {"Calc1HNMR", 0, 0 };
    static const QUMethod slot_51 = {"CalcIR", 0, 0 };
    static const QUMethod slot_52 = {"CalcpKa", 0, 0 };
    static const QUMethod slot_53 = {"CalcKOW", 0, 0 };
    static const QUMethod slot_54 = {"To3D", 0, 0 };
    static const QUMethod slot_55 = {"CalcName", 0, 0 };
    static const QUMethod slot_56 = {"AutoLayout", 0, 0 };
    static const QUMethod slot_57 = {"PeptideBuilder", 0, 0 };
    static const QUMethod slot_58 = {"saveCustomRing", 0, 0 };
    static const QUMethod slot_59 = {"updateCustomRingMenu", 0, 0 };
    static const QUMethod slot_60 = {"DrawRegularArrow", 0, 0 };
    static const QUMethod slot_61 = {"DrawSquareBracket", 0, 0 };
    static const QUMethod slot_62 = {"CleanUpMolecule", 0, 0 };
    static const QUMethod slot_63 = {"Retro", 0, 0 };
    static const QUMethod slot_64 = {"RetroAtomName", 0, 0 };
    static const QUMethod slot_65 = {"RetroBondName", 0, 0 };
    static const QUMethod slot_66 = {"reactionAnalysisTest", 0, 0 };
    static const QUMethod slot_67 = {"reactionAnalysisEnthalpy", 0, 0 };
    static const QUMethod slot_68 = {"reactionAnalysis1HNMR", 0, 0 };
    static const QUMethod slot_69 = {"reactionAnalysis13CNMR", 0, 0 };
    static const QUMethod slot_70 = {"reactivityForward", 0, 0 };
    static const QUMethod slot_71 = {"reactivityRetro", 0, 0 };
    static const QUMethod slot_72 = {"reactivityPC", 0, 0 };
    static const QUMethod slot_73 = {"ToSMILES", 0, 0 };
    static const QUMethod slot_74 = {"ToInChI", 0, 0 };
    static const QUMethod slot_75 = {"FromSMILES", 0, 0 };
    static const QUMethod slot_76 = {"setGroup_Reactant", 0, 0 };
    static const QUMethod slot_77 = {"setGroup_Product", 0, 0 };
    static const QUMethod slot_78 = {"clearGroup", 0, 0 };
    static const QUMethod slot_79 = {"clearAllGroups", 0, 0 };
    static const QUMethod slot_80 = {"toggleGrid", 0, 0 };
    static const QUParameter param_slot_81[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_81 = {"svXY", 2, param_slot_81 };
    static const QUMethod slot_82 = {"MagnifyPlus", 0, 0 };
    static const QUMethod slot_83 = {"MagnifyMinus", 0, 0 };
    static const QUMethod slot_84 = {"Magnify100", 0, 0 };
    static const QUParameter param_slot_85[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_85 = {"Zoom", 1, param_slot_85 };
    static const QUMethod slot_86 = {"XDCSettings", 0, 0 };
    static const QUMethod slot_87 = {"InsertSymbol", 0, 0 };
    static const QUMethod slot_88 = {"OBGetFilters", 0, 0 };
    static const QUMethod slot_89 = {"OBImport", 0, 0 };
    static const QUParameter param_slot_90[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_90 = {"OBNewLoad", 2, param_slot_90 };
    static const QUMethod slot_91 = {"OBExport", 0, 0 };
    static const QUMethod slot_92 = {"OBNewSave", 0, 0 };
    static const QUParameter param_slot_93[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::Out },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_93 = {"OBGuessFileType", 2, param_slot_93 };
    static const QUMethod slot_94 = {"about", 0, 0 };
    static const QUMethod slot_95 = {"support", 0, 0 };
    static const QUMethod slot_96 = {"NewManual", 0, 0 };
    static const QUParameter param_slot_97[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_97 = {"HelpBrowser", 1, param_slot_97 };
    static const QUParameter param_slot_98[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_98 = {"HelpTopic", 1, param_slot_98 };
    static const QUMethod slot_99 = {"Refs", 0, 0 };
    static const QUMethod slot_100 = {"Dummy", 0, 0 };
    static const QUMethod slot_101 = {"Test", 0, 0 };
    static const QUParameter param_slot_102[] = {
	{ 0, &static_QUType_ptr, "Clipboard", QUParameter::In }
    };
    static const QUMethod slot_102 = {"setClipboard", 1, param_slot_102 };
    static const QMetaData slot_tbl[] = {
	{ "newDoc()", &slot_0, QMetaData::Public },
	{ "load()", &slot_1, QMetaData::Public },
	{ "load(QString)", &slot_2, QMetaData::Public },
	{ "save()", &slot_3, QMetaData::Public },
	{ "saveAs()", &slot_4, QMetaData::Public },
	{ "savePicture()", &slot_5, QMetaData::Public },
	{ "externalProgram()", &slot_6, QMetaData::Public },
	{ "SavePNG()", &slot_7, QMetaData::Public },
	{ "Save3D()", &slot_8, QMetaData::Public },
	{ "PageSetup()", &slot_9, QMetaData::Public },
	{ "print()", &slot_10, QMetaData::Public },
	{ "MakeNetDialog()", &slot_11, QMetaData::Public },
	{ "ShowFixedDialog()", &slot_12, QMetaData::Public },
	{ "showDYK()", &slot_13, QMetaData::Public },
	{ "Cut()", &slot_14, QMetaData::Public },
	{ "Copy()", &slot_15, QMetaData::Public },
	{ "Paste()", &slot_16, QMetaData::Public },
	{ "Undo()", &slot_17, QMetaData::Public },
	{ "Clear()", &slot_18, QMetaData::Public },
	{ "SelectAll()", &slot_19, QMetaData::Public },
	{ "DeselectAll()", &slot_20, QMetaData::Public },
	{ "ShowTextButtons(QFont)", &slot_21, QMetaData::Public },
	{ "HideTextButtons()", &slot_22, QMetaData::Public },
	{ "SetStatusBar(QString)", &slot_23, QMetaData::Public },
	{ "SetFont(int)", &slot_24, QMetaData::Public },
	{ "SetColor(int)", &slot_25, QMetaData::Public },
	{ "NewColor()", &slot_26, QMetaData::Public },
	{ "BackgroundColor()", &slot_27, QMetaData::Public },
	{ "SetThick(int)", &slot_28, QMetaData::Public },
	{ "FromRingMenu(int)", &slot_29, QMetaData::Public },
	{ "FromNewRingMenu(int)", &slot_30, QMetaData::Public },
	{ "FromRingToolbar(QString)", &slot_31, QMetaData::Public },
	{ "FromSymbolMenu(int)", &slot_32, QMetaData::Public },
	{ "FromArrowMenu(int)", &slot_33, QMetaData::Public },
	{ "FromCurveArrowMenu(int)", &slot_34, QMetaData::Public },
	{ "FromBracketMenu(int)", &slot_35, QMetaData::Public },
	{ "CubicBezierTool(int)", &slot_36, QMetaData::Public },
	{ "FlipH()", &slot_37, QMetaData::Public },
	{ "FlipV()", &slot_38, QMetaData::Public },
	{ "Rotate90()", &slot_39, QMetaData::Public },
	{ "Rotate180()", &slot_40, QMetaData::Public },
	{ "Rotate270()", &slot_41, QMetaData::Public },
	{ "setFixed_arrow()", &slot_42, QMetaData::Public },
	{ "setFixed_bond()", &slot_43, QMetaData::Public },
	{ "setFix_Hydrogens()", &slot_44, QMetaData::Public },
	{ "MoleculeInfo()", &slot_45, QMetaData::Public },
	{ "CalcEA()", &slot_46, QMetaData::Public },
	{ "CalcEF()", &slot_47, QMetaData::Public },
	{ "CalcMW()", &slot_48, QMetaData::Public },
	{ "Calc13CNMR()", &slot_49, QMetaData::Public },
	{ "Calc1HNMR()", &slot_50, QMetaData::Public },
	{ "CalcIR()", &slot_51, QMetaData::Public },
	{ "CalcpKa()", &slot_52, QMetaData::Public },
	{ "CalcKOW()", &slot_53, QMetaData::Public },
	{ "To3D()", &slot_54, QMetaData::Public },
	{ "CalcName()", &slot_55, QMetaData::Public },
	{ "AutoLayout()", &slot_56, QMetaData::Public },
	{ "PeptideBuilder()", &slot_57, QMetaData::Public },
	{ "saveCustomRing()", &slot_58, QMetaData::Public },
	{ "updateCustomRingMenu()", &slot_59, QMetaData::Public },
	{ "DrawRegularArrow()", &slot_60, QMetaData::Public },
	{ "DrawSquareBracket()", &slot_61, QMetaData::Public },
	{ "CleanUpMolecule()", &slot_62, QMetaData::Public },
	{ "Retro()", &slot_63, QMetaData::Public },
	{ "RetroAtomName()", &slot_64, QMetaData::Public },
	{ "RetroBondName()", &slot_65, QMetaData::Public },
	{ "reactionAnalysisTest()", &slot_66, QMetaData::Public },
	{ "reactionAnalysisEnthalpy()", &slot_67, QMetaData::Public },
	{ "reactionAnalysis1HNMR()", &slot_68, QMetaData::Public },
	{ "reactionAnalysis13CNMR()", &slot_69, QMetaData::Public },
	{ "reactivityForward()", &slot_70, QMetaData::Public },
	{ "reactivityRetro()", &slot_71, QMetaData::Public },
	{ "reactivityPC()", &slot_72, QMetaData::Public },
	{ "ToSMILES()", &slot_73, QMetaData::Public },
	{ "ToInChI()", &slot_74, QMetaData::Public },
	{ "FromSMILES()", &slot_75, QMetaData::Public },
	{ "setGroup_Reactant()", &slot_76, QMetaData::Public },
	{ "setGroup_Product()", &slot_77, QMetaData::Public },
	{ "clearGroup()", &slot_78, QMetaData::Public },
	{ "clearAllGroups()", &slot_79, QMetaData::Public },
	{ "toggleGrid()", &slot_80, QMetaData::Public },
	{ "svXY(int,int)", &slot_81, QMetaData::Public },
	{ "MagnifyPlus()", &slot_82, QMetaData::Public },
	{ "MagnifyMinus()", &slot_83, QMetaData::Public },
	{ "Magnify100()", &slot_84, QMetaData::Public },
	{ "Zoom(int)", &slot_85, QMetaData::Public },
	{ "XDCSettings()", &slot_86, QMetaData::Public },
	{ "InsertSymbol()", &slot_87, QMetaData::Public },
	{ "OBGetFilters()", &slot_88, QMetaData::Public },
	{ "OBImport()", &slot_89, QMetaData::Public },
	{ "OBNewLoad(QString,QString)", &slot_90, QMetaData::Public },
	{ "OBExport()", &slot_91, QMetaData::Public },
	{ "OBNewSave()", &slot_92, QMetaData::Public },
	{ "OBGuessFileType(QString)", &slot_93, QMetaData::Public },
	{ "about()", &slot_94, QMetaData::Public },
	{ "support()", &slot_95, QMetaData::Public },
	{ "NewManual()", &slot_96, QMetaData::Public },
	{ "HelpBrowser(QString)", &slot_97, QMetaData::Public },
	{ "HelpTopic(QString)", &slot_98, QMetaData::Public },
	{ "Refs()", &slot_99, QMetaData::Public },
	{ "Dummy()", &slot_100, QMetaData::Public },
	{ "Test()", &slot_101, QMetaData::Public },
	{ "setClipboard(Clipboard*)", &slot_102, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"ApplicationWindow", parentObject,
	slot_tbl, 103,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ApplicationWindow.setMetaObject( metaObj );
    return metaObj;
}

void* ApplicationWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ApplicationWindow" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool ApplicationWindow::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: newDoc(); break;
    case 1: load(); break;
    case 2: load((QString)static_QUType_QString.get(_o+1)); break;
    case 3: save(); break;
    case 4: saveAs(); break;
    case 5: savePicture(); break;
    case 6: externalProgram(); break;
    case 7: SavePNG(); break;
    case 8: Save3D(); break;
    case 9: PageSetup(); break;
    case 10: print(); break;
    case 11: MakeNetDialog(); break;
    case 12: ShowFixedDialog(); break;
    case 13: showDYK(); break;
    case 14: Cut(); break;
    case 15: Copy(); break;
    case 16: Paste(); break;
    case 17: Undo(); break;
    case 18: Clear(); break;
    case 19: SelectAll(); break;
    case 20: DeselectAll(); break;
    case 21: ShowTextButtons((QFont)(*((QFont*)static_QUType_ptr.get(_o+1)))); break;
    case 22: HideTextButtons(); break;
    case 23: SetStatusBar((QString)static_QUType_QString.get(_o+1)); break;
    case 24: SetFont((int)static_QUType_int.get(_o+1)); break;
    case 25: SetColor((int)static_QUType_int.get(_o+1)); break;
    case 26: NewColor(); break;
    case 27: BackgroundColor(); break;
    case 28: SetThick((int)static_QUType_int.get(_o+1)); break;
    case 29: FromRingMenu((int)static_QUType_int.get(_o+1)); break;
    case 30: FromNewRingMenu((int)static_QUType_int.get(_o+1)); break;
    case 31: FromRingToolbar((QString)static_QUType_QString.get(_o+1)); break;
    case 32: FromSymbolMenu((int)static_QUType_int.get(_o+1)); break;
    case 33: FromArrowMenu((int)static_QUType_int.get(_o+1)); break;
    case 34: FromCurveArrowMenu((int)static_QUType_int.get(_o+1)); break;
    case 35: FromBracketMenu((int)static_QUType_int.get(_o+1)); break;
    case 36: CubicBezierTool((int)static_QUType_int.get(_o+1)); break;
    case 37: FlipH(); break;
    case 38: FlipV(); break;
    case 39: Rotate90(); break;
    case 40: Rotate180(); break;
    case 41: Rotate270(); break;
    case 42: setFixed_arrow(); break;
    case 43: setFixed_bond(); break;
    case 44: setFix_Hydrogens(); break;
    case 45: MoleculeInfo(); break;
    case 46: CalcEA(); break;
    case 47: CalcEF(); break;
    case 48: CalcMW(); break;
    case 49: Calc13CNMR(); break;
    case 50: Calc1HNMR(); break;
    case 51: CalcIR(); break;
    case 52: CalcpKa(); break;
    case 53: CalcKOW(); break;
    case 54: To3D(); break;
    case 55: CalcName(); break;
    case 56: AutoLayout(); break;
    case 57: PeptideBuilder(); break;
    case 58: saveCustomRing(); break;
    case 59: updateCustomRingMenu(); break;
    case 60: DrawRegularArrow(); break;
    case 61: DrawSquareBracket(); break;
    case 62: CleanUpMolecule(); break;
    case 63: Retro(); break;
    case 64: RetroAtomName(); break;
    case 65: RetroBondName(); break;
    case 66: reactionAnalysisTest(); break;
    case 67: reactionAnalysisEnthalpy(); break;
    case 68: reactionAnalysis1HNMR(); break;
    case 69: reactionAnalysis13CNMR(); break;
    case 70: reactivityForward(); break;
    case 71: reactivityRetro(); break;
    case 72: reactivityPC(); break;
    case 73: ToSMILES(); break;
    case 74: ToInChI(); break;
    case 75: FromSMILES(); break;
    case 76: setGroup_Reactant(); break;
    case 77: setGroup_Product(); break;
    case 78: clearGroup(); break;
    case 79: clearAllGroups(); break;
    case 80: toggleGrid(); break;
    case 81: svXY((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 82: MagnifyPlus(); break;
    case 83: MagnifyMinus(); break;
    case 84: Magnify100(); break;
    case 85: Zoom((int)static_QUType_int.get(_o+1)); break;
    case 86: XDCSettings(); break;
    case 87: InsertSymbol(); break;
    case 88: OBGetFilters(); break;
    case 89: OBImport(); break;
    case 90: OBNewLoad((QString)static_QUType_QString.get(_o+1),(QString)static_QUType_QString.get(_o+2)); break;
    case 91: OBExport(); break;
    case 92: OBNewSave(); break;
    case 93: static_QUType_QString.set(_o,OBGuessFileType((QString)static_QUType_QString.get(_o+1))); break;
    case 94: about(); break;
    case 95: support(); break;
    case 96: NewManual(); break;
    case 97: HelpBrowser((QString)static_QUType_QString.get(_o+1)); break;
    case 98: HelpTopic((QString)static_QUType_QString.get(_o+1)); break;
    case 99: Refs(); break;
    case 100: Dummy(); break;
    case 101: Test(); break;
    case 102: setClipboard((Clipboard*)static_QUType_ptr.get(_o+1)); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ApplicationWindow::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool ApplicationWindow::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool ApplicationWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
