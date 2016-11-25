/****************************************************************************
** XDC_ToolButton meta object code from reading C++ file 'xdc_toolbutton.h'
**
** Created: Wed Nov 23 20:29:59 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "xdc_toolbutton.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *XDC_ToolButton::className() const
{
    return "XDC_ToolButton";
}

QMetaObject *XDC_ToolButton::metaObj = 0;
static QMetaObjectCleanUp cleanUp_XDC_ToolButton( "XDC_ToolButton", &XDC_ToolButton::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString XDC_ToolButton::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "XDC_ToolButton", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString XDC_ToolButton::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "XDC_ToolButton", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* XDC_ToolButton::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QToolButton::staticMetaObject();
    static const QUMethod slot_0 = {"trigger", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "trigger()", &slot_0, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"IncludeFile", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "IncludeFile(QString)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"XDC_ToolButton", parentObject,
	slot_tbl, 1,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_XDC_ToolButton.setMetaObject( metaObj );
    return metaObj;
}

void* XDC_ToolButton::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "XDC_ToolButton" ) )
	return this;
    return QToolButton::qt_cast( clname );
}

// SIGNAL IncludeFile
void XDC_ToolButton::IncludeFile( QString t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

bool XDC_ToolButton::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: trigger(); break;
    default:
	return QToolButton::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool XDC_ToolButton::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: IncludeFile((QString)static_QUType_QString.get(_o+1)); break;
    default:
	return QToolButton::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool XDC_ToolButton::qt_property( int id, int f, QVariant* v)
{
    return QToolButton::qt_property( id, f, v);
}

bool XDC_ToolButton::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
