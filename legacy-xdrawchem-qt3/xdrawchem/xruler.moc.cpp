/****************************************************************************
** XRuler meta object code from reading C++ file 'xruler.h'
**
** Created: Wed Nov 23 20:30:01 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "xruler.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *XRuler::className() const
{
    return "XRuler";
}

QMetaObject *XRuler::metaObj = 0;
static QMetaObjectCleanUp cleanUp_XRuler( "XRuler", &XRuler::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString XRuler::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "XRuler", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString XRuler::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "XRuler", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* XRuler::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"setHV", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In },
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setRange", 2, param_slot_1 };
    static const QUMethod slot_2 = {"zoomEvent", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "setHV(int)", &slot_0, QMetaData::Public },
	{ "setRange(int,int)", &slot_1, QMetaData::Public },
	{ "zoomEvent()", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"XRuler", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_XRuler.setMetaObject( metaObj );
    return metaObj;
}

void* XRuler::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "XRuler" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool XRuler::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setHV((int)static_QUType_int.get(_o+1)); break;
    case 1: setRange((int)static_QUType_int.get(_o+1),(int)static_QUType_int.get(_o+2)); break;
    case 2: zoomEvent(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool XRuler::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool XRuler::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool XRuler::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
