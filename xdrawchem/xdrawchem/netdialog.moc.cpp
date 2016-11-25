/****************************************************************************
** NetDialog meta object code from reading C++ file 'netdialog.h'
**
** Created: Wed Nov 23 20:29:55 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "netdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *NetDialog::className() const
{
    return "NetDialog";
}

QMetaObject *NetDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_NetDialog( "NetDialog", &NetDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString NetDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NetDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString NetDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NetDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* NetDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"OK", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "OK()", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"NetDialog", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_NetDialog.setMetaObject( metaObj );
    return metaObj;
}

void* NetDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "NetDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool NetDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: OK(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool NetDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool NetDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool NetDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
