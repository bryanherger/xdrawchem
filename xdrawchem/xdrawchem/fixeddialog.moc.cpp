/****************************************************************************
** FixedDialog meta object code from reading C++ file 'fixeddialog.h'
**
** Created: Wed Nov 23 20:29:59 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "fixeddialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *FixedDialog::className() const
{
    return "FixedDialog";
}

QMetaObject *FixedDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_FixedDialog( "FixedDialog", &FixedDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString FixedDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FixedDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString FixedDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "FixedDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* FixedDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"onSuccess", 0, 0 };
    static const QUMethod slot_1 = {"setDefaults", 0, 0 };
    static const QUParameter param_slot_2[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"setUnits", 1, param_slot_2 };
    static const QMetaData slot_tbl[] = {
	{ "onSuccess()", &slot_0, QMetaData::Public },
	{ "setDefaults()", &slot_1, QMetaData::Public },
	{ "setUnits(int)", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"FixedDialog", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_FixedDialog.setMetaObject( metaObj );
    return metaObj;
}

void* FixedDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "FixedDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool FixedDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: onSuccess(); break;
    case 1: setDefaults(); break;
    case 2: setUnits((int)static_QUType_int.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool FixedDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool FixedDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool FixedDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
