/****************************************************************************
** NetAccess meta object code from reading C++ file 'netaccess.h'
**
** Created: Wed Nov 23 20:29:51 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "netaccess.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *NetAccess::className() const
{
    return "NetAccess";
}

QMetaObject *NetAccess::metaObj = 0;
static QMetaObjectCleanUp cleanUp_NetAccess( "NetAccess", &NetAccess::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString NetAccess::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NetAccess", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString NetAccess::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NetAccess", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* NetAccess::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_varptr, "\x1d", QUParameter::In }
    };
    static const QUMethod slot_0 = {"slotData", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_ptr, "QNetworkOperation", QUParameter::In }
    };
    static const QUMethod slot_1 = {"slotFinished", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"rf", 1, param_slot_2 };
    static const QMetaData slot_tbl[] = {
	{ "slotData(const QByteArray&)", &slot_0, QMetaData::Public },
	{ "slotFinished(QNetworkOperation*)", &slot_1, QMetaData::Public },
	{ "rf(bool)", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"NetAccess", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_NetAccess.setMetaObject( metaObj );
    return metaObj;
}

void* NetAccess::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "NetAccess" ) )
	return this;
    return QObject::qt_cast( clname );
}

bool NetAccess::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotData((const QByteArray&)*((const QByteArray*)static_QUType_ptr.get(_o+1))); break;
    case 1: slotFinished((QNetworkOperation*)static_QUType_ptr.get(_o+1)); break;
    case 2: rf((bool)static_QUType_bool.get(_o+1)); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool NetAccess::qt_emit( int _id, QUObject* _o )
{
    return QObject::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool NetAccess::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool NetAccess::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
