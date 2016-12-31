/****************************************************************************
** Tool_IR_Dialog meta object code from reading C++ file 'tool_ir.h'
**
** Created: Wed Nov 23 20:29:58 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tool_ir.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Tool_IR_Dialog::className() const
{
    return "Tool_IR_Dialog";
}

QMetaObject *Tool_IR_Dialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Tool_IR_Dialog( "Tool_IR_Dialog", &Tool_IR_Dialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Tool_IR_Dialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Tool_IR_Dialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Tool_IR_Dialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Tool_IR_Dialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Tool_IR_Dialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = ToolDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Tool_IR_Dialog", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Tool_IR_Dialog.setMetaObject( metaObj );
    return metaObj;
}

void* Tool_IR_Dialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Tool_IR_Dialog" ) )
	return this;
    return ToolDialog::qt_cast( clname );
}

bool Tool_IR_Dialog::qt_invoke( int _id, QUObject* _o )
{
    return ToolDialog::qt_invoke(_id,_o);
}

bool Tool_IR_Dialog::qt_emit( int _id, QUObject* _o )
{
    return ToolDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Tool_IR_Dialog::qt_property( int id, int f, QVariant* v)
{
    return ToolDialog::qt_property( id, f, v);
}

bool Tool_IR_Dialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
