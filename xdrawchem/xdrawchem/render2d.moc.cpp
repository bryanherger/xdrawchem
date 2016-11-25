/****************************************************************************
** Render2D meta object code from reading C++ file 'render2d.h'
**
** Created: Wed Nov 23 20:29:33 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "render2d.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#include <qvariant.h>
const char *Render2D::className() const
{
    return "Render2D";
}

QMetaObject *Render2D::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Render2D( "Render2D", &Render2D::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Render2D::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Render2D", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Render2D::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Render2D", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Render2D::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"ReactionAnalysis", 1, param_slot_0 };
    static const QUMethod slot_1 = {"setMode_Select", 0, 0 };
    static const QUMethod slot_2 = {"setMode_Lasso", 0, 0 };
    static const QUMethod slot_3 = {"setMode_DrawLine", 0, 0 };
    static const QUMethod slot_4 = {"setMode_DrawDashLine", 0, 0 };
    static const QUMethod slot_5 = {"setMode_DrawChain", 0, 0 };
    static const QUMethod slot_6 = {"setMode_DrawUpLine", 0, 0 };
    static const QUMethod slot_7 = {"setMode_DrawDownLine", 0, 0 };
    static const QUMethod slot_8 = {"setMode_DrawWavyLine", 0, 0 };
    static const QUParameter param_slot_9[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_9 = {"setMode_DrawArrow", 1, param_slot_9 };
    static const QUParameter param_slot_10[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_10 = {"setMode_DrawBracket", 1, param_slot_10 };
    static const QUMethod slot_11 = {"setMode_Erase", 0, 0 };
    static const QUMethod slot_12 = {"setMode_DrawText", 0, 0 };
    static const QUParameter param_slot_13[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_13 = {"setMode_DrawSymbol", 1, param_slot_13 };
    static const QUParameter param_slot_14[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_14 = {"setMode_DrawGraphicObject", 1, param_slot_14 };
    static const QUParameter param_slot_15[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ "param1", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_15 = {"setMode_DrawGraphicObject", 2, param_slot_15 };
    static const QUParameter param_slot_16[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_16 = {"setMode_DrawRing", 2, param_slot_16 };
    static const QUParameter param_slot_17[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ "sp", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_17 = {"setMode_DrawRing", 3, param_slot_17 };
    static const QUParameter param_slot_18[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_18 = {"setMode_DrawCurveArrow", 1, param_slot_18 };
    static const QUMethod slot_19 = {"setMode_DrawNewmanAnti", 0, 0 };
    static const QUMethod slot_20 = {"setMode_DrawNewmanGauche", 0, 0 };
    static const QUParameter param_slot_21[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_21 = {"Tool", 1, param_slot_21 };
    static const QUMethod slot_22 = {"SelectAll", 0, 0 };
    static const QUMethod slot_23 = {"DeselectAll", 0, 0 };
    static const QUMethod slot_24 = {"EraseSelected", 0, 0 };
    static const QUMethod slot_25 = {"Inserted", 0, 0 };
    static const QUParameter param_slot_26[] = {
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_26 = {"SetFont", 1, param_slot_26 };
    static const QUParameter param_slot_27[] = {
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_27 = {"SetColor", 1, param_slot_27 };
    static const QUParameter param_slot_28[] = {
	{ 0, &static_QUType_QVariant, "\x05", QUParameter::Out }
    };
    static const QUMethod slot_28 = {"GetFont", 1, param_slot_28 };
    static const QUParameter param_slot_29[] = {
	{ 0, &static_QUType_QVariant, "\x0a", QUParameter::Out }
    };
    static const QUMethod slot_29 = {"GetColor", 1, param_slot_29 };
    static const QUMethod slot_30 = {"Cut", 0, 0 };
    static const QUMethod slot_31 = {"Copy", 0, 0 };
    static const QUMethod slot_32 = {"Paste", 0, 0 };
    static const QUMethod slot_33 = {"Undo", 0, 0 };
    static const QUMethod slot_34 = {"Rotate90", 0, 0 };
    static const QUMethod slot_35 = {"Rotate180", 0, 0 };
    static const QUMethod slot_36 = {"Rotate270", 0, 0 };
    static const QUMethod slot_37 = {"AutoLayout", 0, 0 };
    static const QUParameter param_slot_38[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_38 = {"Flip", 1, param_slot_38 };
    static const QUParameter param_slot_39[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_39 = {"SaveEPS", 2, param_slot_39 };
    static const QUParameter param_slot_40[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_40 = {"SaveSVG", 2, param_slot_40 };
    static const QUMethod slot_41 = {"clearAllGroups", 0, 0 };
    static const QUParameter param_slot_42[] = {
	{ 0, &static_QUType_QVariant, "\x0e", QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_42 = {"zoomCorrect", 2, param_slot_42 };
    static const QUParameter param_slot_43[] = {
	{ 0, &static_QUType_QVariant, "\x0e", QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_43 = {"zoomCorrectReverse", 2, param_slot_43 };
    static const QUParameter param_slot_44[] = {
	{ 0, &static_QUType_double, 0, QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_44 = {"getAngle", 3, param_slot_44 };
    static const QUParameter param_slot_45[] = {
	{ 0, &static_QUType_varptr, "\x15", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_45 = {"drawBezier", 2, param_slot_45 };
    static const QUParameter param_slot_46[] = {
	{ 0, &static_QUType_varptr, "\x15", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_46 = {"drawBezier", 3, param_slot_46 };
    static const QUParameter param_slot_47[] = {
	{ 0, &static_QUType_varptr, "\x15", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_bool, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_47 = {"drawBezier", 4, param_slot_47 };
    static const QUParameter param_slot_48[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_48 = {"drawLine", 4, param_slot_48 };
    static const QUParameter param_slot_49[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_49 = {"drawLine", 5, param_slot_49 };
    static const QUParameter param_slot_50[] = {
	{ 0, &static_QUType_varptr, "\x15", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_50 = {"drawPolyline", 2, param_slot_50 };
    static const QUParameter param_slot_51[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_51 = {"drawUpLine", 3, param_slot_51 };
    static const QUParameter param_slot_52[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_52 = {"drawDownLine", 3, param_slot_52 };
    static const QUParameter param_slot_53[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_53 = {"drawWavyLine", 3, param_slot_53 };
    static const QUParameter param_slot_54[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_54 = {"drawArrow", 5, param_slot_54 };
    static const QUParameter param_slot_55[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_55 = {"drawBracket", 5, param_slot_55 };
    static const QUParameter param_slot_56[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_56 = {"drawBox", 3, param_slot_56 };
    static const QUParameter param_slot_57[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_57 = {"drawFillBox", 3, param_slot_57 };
    static const QUParameter param_slot_58[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_bool, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_58 = {"drawFillBox", 6, param_slot_58 };
    static const QUParameter param_slot_59[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_59 = {"drawCircle", 3, param_slot_59 };
    static const QUParameter param_slot_60[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_bool, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_bool, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_60 = {"drawEllipse", 6, param_slot_60 };
    static const QUParameter param_slot_61[] = {
	{ 0, &static_QUType_ptr, "QChar", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_61 = {"drawText", 4, param_slot_61 };
    static const QUParameter param_slot_62[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_62 = {"drawString", 4, param_slot_62 };
    static const QUParameter param_slot_63[] = {
	{ 0, &static_QUType_ptr, "QChar", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_63 = {"drawTextReverse", 4, param_slot_63 };
    static const QUParameter param_slot_64[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x06", QUParameter::In }
    };
    static const QUMethod slot_64 = {"drawPixmap", 2, param_slot_64 };
    static const QUParameter param_slot_65[] = {
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0a", QUParameter::In },
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_65 = {"drawCurveArrow", 4, param_slot_65 };
    static const QUParameter param_slot_66[] = {
	{ 0, &static_QUType_QVariant, "\x06", QUParameter::Out }
    };
    static const QUMethod slot_66 = {"MakePixmap", 1, param_slot_66 };
    static const QUParameter param_slot_67[] = {
	{ 0, &static_QUType_QVariant, "\x06", QUParameter::Out },
	{ "transp", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_67 = {"MakePixmap", 2, param_slot_67 };
    static const QUParameter param_slot_68[] = {
	{ 0, &static_QUType_QVariant, "\x06", QUParameter::Out }
    };
    static const QUMethod slot_68 = {"MakeFullPixmap", 1, param_slot_68 };
    static const QUMethod slot_69 = {"InsertSymbol", 0, 0 };
    static const QUMethod slot_70 = {"DrawText_returnPressed", 0, 0 };
    static const QUMethod slot_71 = {"DrawText_textChanged", 0, 0 };
    static const QUMethod slot_72 = {"DrawText_FinishText", 0, 0 };
    static const QUParameter param_slot_73[] = {
	{ 0, &static_QUType_ptr, "QTextEdit::VerticalAlignment", QUParameter::In }
    };
    static const QUMethod slot_73 = {"DrawText_CVAchanged", 1, param_slot_73 };
    static const QUParameter param_slot_74[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In },
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_74 = {"EditText", 5, param_slot_74 };
    static const QUParameter param_slot_75[] = {
	{ 0, &static_QUType_QVariant, "\x08", QUParameter::Out },
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_75 = {"GetTextDimensions", 3, param_slot_75 };
    static const QUParameter param_slot_76[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_76 = {"GetTextHeight", 2, param_slot_76 };
    static const QUParameter param_slot_77[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_77 = {"GetTextFullHeight", 2, param_slot_77 };
    static const QUParameter param_slot_78[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out },
	{ 0, &static_QUType_ptr, "QChar", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_78 = {"GetCharWidth", 3, param_slot_78 };
    static const QUParameter param_slot_79[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out },
	{ 0, &static_QUType_QString, 0, QUParameter::In },
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod slot_79 = {"GetStringWidth", 3, param_slot_79 };
    static const QUParameter param_slot_80[] = {
	{ 0, &static_QUType_ptr, "QMouseEvent", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_80 = {"DrawText_mousePressEvent", 2, param_slot_80 };
    static const QUParameter param_slot_81[] = {
	{ 0, &static_QUType_ptr, "QMouseEvent", QUParameter::In },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_81 = {"DrawText_mouseReleaseEvent", 2, param_slot_81 };
    static const QUParameter param_slot_82[] = {
	{ 0, &static_QUType_ptr, "QMouseEvent", QUParameter::In }
    };
    static const QUMethod slot_82 = {"DrawText_mouseMoveEvent", 1, param_slot_82 };
    static const QUMethod slot_83 = {"Superscript", 0, 0 };
    static const QUMethod slot_84 = {"Subscript", 0, 0 };
    static const QUMethod slot_85 = {"Bold", 0, 0 };
    static const QUMethod slot_86 = {"Italic", 0, 0 };
    static const QUMethod slot_87 = {"Underline", 0, 0 };
    static const QUMethod slot_88 = {"JustifyLeft", 0, 0 };
    static const QUMethod slot_89 = {"JustifyCenter", 0, 0 };
    static const QUMethod slot_90 = {"JustifyRight", 0, 0 };
    static const QUParameter param_slot_91[] = {
	{ 0, &static_QUType_ptr, "QMouseEvent", QUParameter::In }
    };
    static const QUMethod slot_91 = {"Select_mouseMoveEvent", 1, param_slot_91 };
    static const QUParameter param_slot_92[] = {
	{ 0, &static_QUType_double, 0, QUParameter::Out },
	{ 0, &static_QUType_ptr, "DPoint", QUParameter::In },
	{ 0, &static_QUType_ptr, "DPoint", QUParameter::In },
	{ 0, &static_QUType_ptr, "DPoint", QUParameter::In }
    };
    static const QUMethod slot_92 = {"GetAngle", 4, param_slot_92 };
    static const QUMethod slot_93 = {"PrintSetup", 0, 0 };
    static const QUMethod slot_94 = {"Print", 0, 0 };
    static const QUParameter param_slot_95[] = {
	{ "epsname", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_95 = {"Print", 1, param_slot_95 };
    static const QUMethod slot_96 = {"UpdatePageGeometry", 0, 0 };
    static const QUParameter param_slot_97[] = {
	{ 0, &static_QUType_QVariant, "\x0e", QUParameter::Out },
	{ 0, &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_97 = {"GridLock", 2, param_slot_97 };
    static const QUMethod slot_98 = {"zoomEvent", 0, 0 };
    static const QUMethod slot_99 = {"molInfo", 0, 0 };
    static const QUMethod slot_100 = {"molSelect", 0, 0 };
    static const QUMethod slot_101 = {"molCut", 0, 0 };
    static const QUMethod slot_102 = {"molCopy", 0, 0 };
    static const QUMethod slot_103 = {"bondEdit", 0, 0 };
    static const QUMethod slot_104 = {"bondInfo", 0, 0 };
    static const QUMethod slot_105 = {"textShape", 0, 0 };
    static const QUMethod slot_106 = {"bracketFill", 0, 0 };
    static const QUMethod slot_107 = {"bracketFillOff", 0, 0 };
    static const QUParameter param_slot_108[] = {
	{ "dd1", &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_108 = {"setDirectDraw", 1, param_slot_108 };
    static const QUParameter param_slot_109[] = {
	{ 0, &static_QUType_double, 0, QUParameter::Out },
	{ "a", &static_QUType_varptr, "\x0e", QUParameter::In },
	{ "b", &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_109 = {"DistanceBetween", 3, param_slot_109 };
    static const QUParameter param_slot_110[] = {
	{ 0, &static_QUType_QVariant, "\x0e", QUParameter::Out },
	{ "a", &static_QUType_varptr, "\x0e", QUParameter::In },
	{ "b", &static_QUType_varptr, "\x0e", QUParameter::In }
    };
    static const QUMethod slot_110 = {"Midpoint", 3, param_slot_110 };
    static const QUParameter param_slot_111[] = {
	{ 0, &static_QUType_int, 0, QUParameter::Out },
	{ "x", &static_QUType_double, 0, QUParameter::In }
    };
    static const QUMethod slot_111 = {"RoundOff", 2, param_slot_111 };
    static const QUParameter param_slot_112[] = {
	{ "x", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_112 = {"SetOutputDevice", 1, param_slot_112 };
    static const QUParameter param_slot_113[] = {
	{ "t", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_113 = {"SetThick", 1, param_slot_113 };
    static const QUMethod slot_114 = {"resetTextOrigin", 0, 0 };
    static const QUMethod slot_115 = {"setPageSizeAndOrientation", 0, 0 };
    static const QUParameter param_slot_116[] = {
	{ "bc1", &static_QUType_varptr, "\x0a", QUParameter::In }
    };
    static const QUMethod slot_116 = {"setBGColor", 1, param_slot_116 };
    static const QUParameter param_slot_117[] = {
	{ 0, &static_QUType_QVariant, "\x0a", QUParameter::Out }
    };
    static const QUMethod slot_117 = {"getBGColor", 1, param_slot_117 };
    static const QUMethod slot_118 = {"setWaitCursor", 0, 0 };
    static const QUMethod slot_119 = {"setArrowCursor", 0, 0 };
    static const QUParameter param_slot_120[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::Out }
    };
    static const QUMethod slot_120 = {"getFontKludge", 1, param_slot_120 };
    static const QMetaData slot_tbl[] = {
	{ "ReactionAnalysis(int)", &slot_0, QMetaData::Public },
	{ "setMode_Select()", &slot_1, QMetaData::Public },
	{ "setMode_Lasso()", &slot_2, QMetaData::Public },
	{ "setMode_DrawLine()", &slot_3, QMetaData::Public },
	{ "setMode_DrawDashLine()", &slot_4, QMetaData::Public },
	{ "setMode_DrawChain()", &slot_5, QMetaData::Public },
	{ "setMode_DrawUpLine()", &slot_6, QMetaData::Public },
	{ "setMode_DrawDownLine()", &slot_7, QMetaData::Public },
	{ "setMode_DrawWavyLine()", &slot_8, QMetaData::Public },
	{ "setMode_DrawArrow(QString)", &slot_9, QMetaData::Public },
	{ "setMode_DrawBracket(QString)", &slot_10, QMetaData::Public },
	{ "setMode_Erase()", &slot_11, QMetaData::Public },
	{ "setMode_DrawText()", &slot_12, QMetaData::Public },
	{ "setMode_DrawSymbol(QString)", &slot_13, QMetaData::Public },
	{ "setMode_DrawGraphicObject(int)", &slot_14, QMetaData::Public },
	{ "setMode_DrawGraphicObject(int,int)", &slot_15, QMetaData::Public },
	{ "setMode_DrawRing(QString,QString)", &slot_16, QMetaData::Public },
	{ "setMode_DrawRing(QString,QString,int)", &slot_17, QMetaData::Public },
	{ "setMode_DrawCurveArrow(QString)", &slot_18, QMetaData::Public },
	{ "setMode_DrawNewmanAnti()", &slot_19, QMetaData::Public },
	{ "setMode_DrawNewmanGauche()", &slot_20, QMetaData::Public },
	{ "Tool(int)", &slot_21, QMetaData::Public },
	{ "SelectAll()", &slot_22, QMetaData::Public },
	{ "DeselectAll()", &slot_23, QMetaData::Public },
	{ "EraseSelected()", &slot_24, QMetaData::Public },
	{ "Inserted()", &slot_25, QMetaData::Public },
	{ "SetFont(QFont)", &slot_26, QMetaData::Public },
	{ "SetColor(QColor)", &slot_27, QMetaData::Public },
	{ "GetFont()", &slot_28, QMetaData::Public },
	{ "GetColor()", &slot_29, QMetaData::Public },
	{ "Cut()", &slot_30, QMetaData::Public },
	{ "Copy()", &slot_31, QMetaData::Public },
	{ "Paste()", &slot_32, QMetaData::Public },
	{ "Undo()", &slot_33, QMetaData::Public },
	{ "Rotate90()", &slot_34, QMetaData::Public },
	{ "Rotate180()", &slot_35, QMetaData::Public },
	{ "Rotate270()", &slot_36, QMetaData::Public },
	{ "AutoLayout()", &slot_37, QMetaData::Public },
	{ "Flip(int)", &slot_38, QMetaData::Public },
	{ "SaveEPS(QString)", &slot_39, QMetaData::Public },
	{ "SaveSVG(QString)", &slot_40, QMetaData::Public },
	{ "clearAllGroups()", &slot_41, QMetaData::Public },
	{ "zoomCorrect(QPoint)", &slot_42, QMetaData::Public },
	{ "zoomCorrectReverse(QPoint)", &slot_43, QMetaData::Public },
	{ "getAngle(QPoint,QPoint)", &slot_44, QMetaData::Public },
	{ "drawBezier(QPointArray,QColor)", &slot_45, QMetaData::Public },
	{ "drawBezier(QPointArray,QColor,bool)", &slot_46, QMetaData::Public },
	{ "drawBezier(QPointArray,QColor,bool,int)", &slot_47, QMetaData::Public },
	{ "drawLine(QPoint,QPoint,int,QColor)", &slot_48, QMetaData::Public },
	{ "drawLine(QPoint,QPoint,int,QColor,int)", &slot_49, QMetaData::Public },
	{ "drawPolyline(QPointArray,QColor)", &slot_50, QMetaData::Public },
	{ "drawUpLine(QPoint,QPoint,QColor)", &slot_51, QMetaData::Public },
	{ "drawDownLine(QPoint,QPoint,QColor)", &slot_52, QMetaData::Public },
	{ "drawWavyLine(QPoint,QPoint,QColor)", &slot_53, QMetaData::Public },
	{ "drawArrow(QPoint,QPoint,QColor,int,int)", &slot_54, QMetaData::Public },
	{ "drawBracket(QPoint,QPoint,QColor,int,QColor)", &slot_55, QMetaData::Public },
	{ "drawBox(QPoint,QPoint,QColor)", &slot_56, QMetaData::Public },
	{ "drawFillBox(QPoint,QPoint,QColor)", &slot_57, QMetaData::Public },
	{ "drawFillBox(QPoint,QPoint,QColor,bool,QColor,int)", &slot_58, QMetaData::Public },
	{ "drawCircle(QPoint,int,QColor)", &slot_59, QMetaData::Public },
	{ "drawEllipse(QPoint,QPoint,bool,QColor,bool,QColor)", &slot_60, QMetaData::Public },
	{ "drawText(QChar,QPoint,QColor,QFont)", &slot_61, QMetaData::Public },
	{ "drawString(QString,QPoint,QColor,QFont)", &slot_62, QMetaData::Public },
	{ "drawTextReverse(QChar,QPoint,QColor,QFont)", &slot_63, QMetaData::Public },
	{ "drawPixmap(QPoint,QPixmap)", &slot_64, QMetaData::Public },
	{ "drawCurveArrow(QPoint,QPoint,QColor,QString)", &slot_65, QMetaData::Public },
	{ "MakePixmap()", &slot_66, QMetaData::Public },
	{ "MakePixmap(bool)", &slot_67, QMetaData::Public },
	{ "MakeFullPixmap()", &slot_68, QMetaData::Public },
	{ "InsertSymbol()", &slot_69, QMetaData::Public },
	{ "DrawText_returnPressed()", &slot_70, QMetaData::Public },
	{ "DrawText_textChanged()", &slot_71, QMetaData::Public },
	{ "DrawText_FinishText()", &slot_72, QMetaData::Public },
	{ "DrawText_CVAchanged(QTextEdit::VerticalAlignment)", &slot_73, QMetaData::Public },
	{ "EditText(QPoint,QString,int,bool)", &slot_74, QMetaData::Public },
	{ "GetTextDimensions(QString,QFont)", &slot_75, QMetaData::Public },
	{ "GetTextHeight(QFont)", &slot_76, QMetaData::Public },
	{ "GetTextFullHeight(QFont)", &slot_77, QMetaData::Public },
	{ "GetCharWidth(QChar,QFont)", &slot_78, QMetaData::Public },
	{ "GetStringWidth(QString,QFont)", &slot_79, QMetaData::Public },
	{ "DrawText_mousePressEvent(QMouseEvent*,QPoint)", &slot_80, QMetaData::Public },
	{ "DrawText_mouseReleaseEvent(QMouseEvent*,QPoint)", &slot_81, QMetaData::Public },
	{ "DrawText_mouseMoveEvent(QMouseEvent*)", &slot_82, QMetaData::Public },
	{ "Superscript()", &slot_83, QMetaData::Public },
	{ "Subscript()", &slot_84, QMetaData::Public },
	{ "Bold()", &slot_85, QMetaData::Public },
	{ "Italic()", &slot_86, QMetaData::Public },
	{ "Underline()", &slot_87, QMetaData::Public },
	{ "JustifyLeft()", &slot_88, QMetaData::Public },
	{ "JustifyCenter()", &slot_89, QMetaData::Public },
	{ "JustifyRight()", &slot_90, QMetaData::Public },
	{ "Select_mouseMoveEvent(QMouseEvent*)", &slot_91, QMetaData::Public },
	{ "GetAngle(DPoint*,DPoint*,DPoint*)", &slot_92, QMetaData::Public },
	{ "PrintSetup()", &slot_93, QMetaData::Public },
	{ "Print()", &slot_94, QMetaData::Public },
	{ "Print(QString)", &slot_95, QMetaData::Public },
	{ "UpdatePageGeometry()", &slot_96, QMetaData::Public },
	{ "GridLock(QPoint)", &slot_97, QMetaData::Public },
	{ "zoomEvent()", &slot_98, QMetaData::Public },
	{ "molInfo()", &slot_99, QMetaData::Public },
	{ "molSelect()", &slot_100, QMetaData::Public },
	{ "molCut()", &slot_101, QMetaData::Public },
	{ "molCopy()", &slot_102, QMetaData::Public },
	{ "bondEdit()", &slot_103, QMetaData::Public },
	{ "bondInfo()", &slot_104, QMetaData::Public },
	{ "textShape()", &slot_105, QMetaData::Public },
	{ "bracketFill()", &slot_106, QMetaData::Public },
	{ "bracketFillOff()", &slot_107, QMetaData::Public },
	{ "setDirectDraw(bool)", &slot_108, QMetaData::Public },
	{ "DistanceBetween(QPoint,QPoint)", &slot_109, QMetaData::Public },
	{ "Midpoint(QPoint,QPoint)", &slot_110, QMetaData::Public },
	{ "RoundOff(double)", &slot_111, QMetaData::Public },
	{ "SetOutputDevice(int)", &slot_112, QMetaData::Public },
	{ "SetThick(int)", &slot_113, QMetaData::Public },
	{ "resetTextOrigin()", &slot_114, QMetaData::Public },
	{ "setPageSizeAndOrientation()", &slot_115, QMetaData::Public },
	{ "setBGColor(QColor)", &slot_116, QMetaData::Public },
	{ "getBGColor()", &slot_117, QMetaData::Public },
	{ "setWaitCursor()", &slot_118, QMetaData::Public },
	{ "setArrowCursor()", &slot_119, QMetaData::Public },
	{ "getFontKludge()", &slot_120, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"SignalSetStatusBar", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"SignalHelpTopic", 1, param_signal_1 };
    static const QUParameter param_signal_2[] = {
	{ 0, &static_QUType_ptr, "XDC_Event", QUParameter::In }
    };
    static const QUMethod signal_2 = {"XDCEventSignal", 1, param_signal_2 };
    static const QUParameter param_signal_3[] = {
	{ 0, &static_QUType_varptr, "\x05", QUParameter::In }
    };
    static const QUMethod signal_3 = {"TextOn", 1, param_signal_3 };
    static const QUMethod signal_4 = {"TextOff", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "SignalSetStatusBar(QString)", &signal_0, QMetaData::Public },
	{ "SignalHelpTopic(QString)", &signal_1, QMetaData::Public },
	{ "XDCEventSignal(XDC_Event*)", &signal_2, QMetaData::Public },
	{ "TextOn(QFont)", &signal_3, QMetaData::Public },
	{ "TextOff()", &signal_4, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"Render2D", parentObject,
	slot_tbl, 121,
	signal_tbl, 5,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Render2D.setMetaObject( metaObj );
    return metaObj;
}

void* Render2D::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Render2D" ) )
	return this;
    return QWidget::qt_cast( clname );
}

// SIGNAL SignalSetStatusBar
void Render2D::SignalSetStatusBar( QString t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

// SIGNAL SignalHelpTopic
void Render2D::SignalHelpTopic( QString t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 1, t0 );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL XDCEventSignal
void Render2D::XDCEventSignal( XDC_Event* t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 2 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_ptr.set(o+1,t0);
    activate_signal( clist, o );
}

// SIGNAL TextOn
void Render2D::TextOn( QFont t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 3 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_varptr.set(o+1,&t0);
    activate_signal( clist, o );
}

// SIGNAL TextOff
void Render2D::TextOff()
{
    activate_signal( staticMetaObject()->signalOffset() + 4 );
}

bool Render2D::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: ReactionAnalysis((int)static_QUType_int.get(_o+1)); break;
    case 1: setMode_Select(); break;
    case 2: setMode_Lasso(); break;
    case 3: setMode_DrawLine(); break;
    case 4: setMode_DrawDashLine(); break;
    case 5: setMode_DrawChain(); break;
    case 6: setMode_DrawUpLine(); break;
    case 7: setMode_DrawDownLine(); break;
    case 8: setMode_DrawWavyLine(); break;
    case 9: setMode_DrawArrow((QString)static_QUType_QString.get(_o+1)); break;
    case 10: setMode_DrawBracket((QString)static_QUType_QString.get(_o+1)); break;
    case 11: setMode_Erase(); break;
    case 12: setMode_DrawText(); break;
    case 13: setMode_DrawSymbol((QString)static_QUType_QString.get(_o+1)); break;
    case 14: setMode_DrawGraphicObject((int)static_QUType_int.get(_o+1)); break;
    case 15: setMode_DrawGraphicObject((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 16: setMode_DrawRing((QString)static_QUType_QString.get(_o+1),(QString)static_QUType_QString.get(_o+2)); break;
    case 17: setMode_DrawRing((QString)static_QUType_QString.get(_o+1),(QString)static_QUType_QString.get(_o+2),(int)static_QUType_int.get(_o+3)); break;
    case 18: setMode_DrawCurveArrow((QString)static_QUType_QString.get(_o+1)); break;
    case 19: setMode_DrawNewmanAnti(); break;
    case 20: setMode_DrawNewmanGauche(); break;
    case 21: Tool((int)static_QUType_int.get(_o+1)); break;
    case 22: SelectAll(); break;
    case 23: DeselectAll(); break;
    case 24: EraseSelected(); break;
    case 25: Inserted(); break;
    case 26: SetFont((QFont)(*((QFont*)static_QUType_ptr.get(_o+1)))); break;
    case 27: SetColor((QColor)(*((QColor*)static_QUType_ptr.get(_o+1)))); break;
    case 28: static_QUType_QVariant.set(_o,QVariant(GetFont())); break;
    case 29: static_QUType_QVariant.set(_o,QVariant(GetColor())); break;
    case 30: Cut(); break;
    case 31: Copy(); break;
    case 32: Paste(); break;
    case 33: Undo(); break;
    case 34: Rotate90(); break;
    case 35: Rotate180(); break;
    case 36: Rotate270(); break;
    case 37: AutoLayout(); break;
    case 38: Flip((int)static_QUType_int.get(_o+1)); break;
    case 39: static_QUType_bool.set(_o,SaveEPS((QString)static_QUType_QString.get(_o+1))); break;
    case 40: static_QUType_bool.set(_o,SaveSVG((QString)static_QUType_QString.get(_o+1))); break;
    case 41: clearAllGroups(); break;
    case 42: static_QUType_QVariant.set(_o,QVariant(zoomCorrect((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1)))))); break;
    case 43: static_QUType_QVariant.set(_o,QVariant(zoomCorrectReverse((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1)))))); break;
    case 44: static_QUType_double.set(_o,getAngle((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))))); break;
    case 45: drawBezier((QPointArray)(*((QPointArray*)static_QUType_ptr.get(_o+1))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+2)))); break;
    case 46: drawBezier((QPointArray)(*((QPointArray*)static_QUType_ptr.get(_o+1))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+2))),(bool)static_QUType_bool.get(_o+3)); break;
    case 47: drawBezier((QPointArray)(*((QPointArray*)static_QUType_ptr.get(_o+1))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+2))),(bool)static_QUType_bool.get(_o+3),(int)static_QUType_int.get(_o+4)); break;
    case 48: drawLine((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(int)static_QUType_int.get(_o+3),(QColor)(*((QColor*)static_QUType_ptr.get(_o+4)))); break;
    case 49: drawLine((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(int)static_QUType_int.get(_o+3),(QColor)(*((QColor*)static_QUType_ptr.get(_o+4))),(int)static_QUType_int.get(_o+5)); break;
    case 50: drawPolyline((QPointArray)(*((QPointArray*)static_QUType_ptr.get(_o+1))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+2)))); break;
    case 51: drawUpLine((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3)))); break;
    case 52: drawDownLine((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3)))); break;
    case 53: drawWavyLine((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3)))); break;
    case 54: drawArrow((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(int)static_QUType_int.get(_o+4),(int)static_QUType_int.get(_o+5)); break;
    case 55: drawBracket((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(int)static_QUType_int.get(_o+4),(QColor)(*((QColor*)static_QUType_ptr.get(_o+5)))); break;
    case 56: drawBox((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3)))); break;
    case 57: drawFillBox((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3)))); break;
    case 58: drawFillBox((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(bool)static_QUType_bool.get(_o+4),(QColor)(*((QColor*)static_QUType_ptr.get(_o+5))),(int)static_QUType_int.get(_o+6)); break;
    case 59: drawCircle((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(int)static_QUType_int.get(_o+2),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3)))); break;
    case 60: drawEllipse((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(bool)static_QUType_bool.get(_o+3),(QColor)(*((QColor*)static_QUType_ptr.get(_o+4))),(bool)static_QUType_bool.get(_o+5),(QColor)(*((QColor*)static_QUType_ptr.get(_o+6)))); break;
    case 61: drawText((QChar)(*((QChar*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(QFont)(*((QFont*)static_QUType_ptr.get(_o+4)))); break;
    case 62: drawString((QString)static_QUType_QString.get(_o+1),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(QFont)(*((QFont*)static_QUType_ptr.get(_o+4)))); break;
    case 63: drawTextReverse((QChar)(*((QChar*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(QFont)(*((QFont*)static_QUType_ptr.get(_o+4)))); break;
    case 64: drawPixmap((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPixmap)(*((QPixmap*)static_QUType_ptr.get(_o+2)))); break;
    case 65: drawCurveArrow((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))),(QColor)(*((QColor*)static_QUType_ptr.get(_o+3))),(QString)static_QUType_QString.get(_o+4)); break;
    case 66: static_QUType_QVariant.set(_o,QVariant(MakePixmap())); break;
    case 67: static_QUType_QVariant.set(_o,QVariant(MakePixmap((bool)static_QUType_bool.get(_o+1)))); break;
    case 68: static_QUType_QVariant.set(_o,QVariant(MakeFullPixmap())); break;
    case 69: InsertSymbol(); break;
    case 70: DrawText_returnPressed(); break;
    case 71: DrawText_textChanged(); break;
    case 72: DrawText_FinishText(); break;
    case 73: DrawText_CVAchanged((QTextEdit::VerticalAlignment)(*((QTextEdit::VerticalAlignment*)static_QUType_ptr.get(_o+1)))); break;
    case 74: static_QUType_QString.set(_o,EditText((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QString)static_QUType_QString.get(_o+2),(int)static_QUType_int.get(_o+3),(bool)static_QUType_bool.get(_o+4))); break;
    case 75: static_QUType_QVariant.set(_o,QVariant(GetTextDimensions((QString)static_QUType_QString.get(_o+1),(QFont)(*((QFont*)static_QUType_ptr.get(_o+2)))))); break;
    case 76: static_QUType_int.set(_o,GetTextHeight((QFont)(*((QFont*)static_QUType_ptr.get(_o+1))))); break;
    case 77: static_QUType_int.set(_o,GetTextFullHeight((QFont)(*((QFont*)static_QUType_ptr.get(_o+1))))); break;
    case 78: static_QUType_int.set(_o,GetCharWidth((QChar)(*((QChar*)static_QUType_ptr.get(_o+1))),(QFont)(*((QFont*)static_QUType_ptr.get(_o+2))))); break;
    case 79: static_QUType_int.set(_o,GetStringWidth((QString)static_QUType_QString.get(_o+1),(QFont)(*((QFont*)static_QUType_ptr.get(_o+2))))); break;
    case 80: DrawText_mousePressEvent((QMouseEvent*)static_QUType_ptr.get(_o+1),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2)))); break;
    case 81: DrawText_mouseReleaseEvent((QMouseEvent*)static_QUType_ptr.get(_o+1),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2)))); break;
    case 82: DrawText_mouseMoveEvent((QMouseEvent*)static_QUType_ptr.get(_o+1)); break;
    case 83: Superscript(); break;
    case 84: Subscript(); break;
    case 85: Bold(); break;
    case 86: Italic(); break;
    case 87: Underline(); break;
    case 88: JustifyLeft(); break;
    case 89: JustifyCenter(); break;
    case 90: JustifyRight(); break;
    case 91: Select_mouseMoveEvent((QMouseEvent*)static_QUType_ptr.get(_o+1)); break;
    case 92: static_QUType_double.set(_o,GetAngle((DPoint*)static_QUType_ptr.get(_o+1),(DPoint*)static_QUType_ptr.get(_o+2),(DPoint*)static_QUType_ptr.get(_o+3))); break;
    case 93: PrintSetup(); break;
    case 94: Print(); break;
    case 95: Print((QString)static_QUType_QString.get(_o+1)); break;
    case 96: UpdatePageGeometry(); break;
    case 97: static_QUType_QVariant.set(_o,QVariant(GridLock((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1)))))); break;
    case 98: zoomEvent(); break;
    case 99: molInfo(); break;
    case 100: molSelect(); break;
    case 101: molCut(); break;
    case 102: molCopy(); break;
    case 103: bondEdit(); break;
    case 104: bondInfo(); break;
    case 105: textShape(); break;
    case 106: bracketFill(); break;
    case 107: bracketFillOff(); break;
    case 108: setDirectDraw((bool)static_QUType_bool.get(_o+1)); break;
    case 109: static_QUType_double.set(_o,DistanceBetween((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2))))); break;
    case 110: static_QUType_QVariant.set(_o,QVariant(Midpoint((QPoint)(*((QPoint*)static_QUType_ptr.get(_o+1))),(QPoint)(*((QPoint*)static_QUType_ptr.get(_o+2)))))); break;
    case 111: static_QUType_int.set(_o,RoundOff((double)static_QUType_double.get(_o+1))); break;
    case 112: SetOutputDevice((int)static_QUType_int.get(_o+1)); break;
    case 113: SetThick((int)static_QUType_int.get(_o+1)); break;
    case 114: resetTextOrigin(); break;
    case 115: setPageSizeAndOrientation(); break;
    case 116: setBGColor((QColor)(*((QColor*)static_QUType_ptr.get(_o+1)))); break;
    case 117: static_QUType_QVariant.set(_o,QVariant(getBGColor())); break;
    case 118: setWaitCursor(); break;
    case 119: setArrowCursor(); break;
    case 120: static_QUType_bool.set(_o,getFontKludge()); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool Render2D::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: SignalSetStatusBar((QString)static_QUType_QString.get(_o+1)); break;
    case 1: SignalHelpTopic((QString)static_QUType_QString.get(_o+1)); break;
    case 2: XDCEventSignal((XDC_Event*)static_QUType_ptr.get(_o+1)); break;
    case 3: TextOn((QFont)(*((QFont*)static_QUType_ptr.get(_o+1)))); break;
    case 4: TextOff(); break;
    default:
	return QWidget::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool Render2D::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool Render2D::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
