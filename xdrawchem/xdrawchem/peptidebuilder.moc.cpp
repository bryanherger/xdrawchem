/****************************************************************************
** PeptDialog meta object code from reading C++ file 'peptidebuilder.h'
**
** Created: Wed Nov 23 20:30:00 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "peptidebuilder.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *PeptDialog::className() const
{
    return "PeptDialog";
}

QMetaObject *PeptDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_PeptDialog( "PeptDialog", &PeptDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString PeptDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "PeptDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString PeptDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "PeptDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* PeptDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"buttonPress", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "buttonPress(int)", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"PeptDialog", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_PeptDialog.setMetaObject( metaObj );
    return metaObj;
}

void* PeptDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "PeptDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool PeptDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: buttonPress((int)static_QUType_int.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool PeptDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool PeptDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool PeptDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
