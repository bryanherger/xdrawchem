/****************************************************************************
** DYKDialog meta object code from reading C++ file 'dyk.h'
**
** Created: Wed Nov 23 20:29:57 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "dyk.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *DYKDialog::className() const
{
    return "DYKDialog";
}

QMetaObject *DYKDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_DYKDialog( "DYKDialog", &DYKDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString DYKDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "DYKDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString DYKDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "DYKDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* DYKDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"nextTip", 0, 0 };
    static const QUMethod slot_1 = {"prevTip", 0, 0 };
    static const QUMethod slot_2 = {"finished", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "nextTip()", &slot_0, QMetaData::Public },
	{ "prevTip()", &slot_1, QMetaData::Public },
	{ "finished()", &slot_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"DYKDialog", parentObject,
	slot_tbl, 3,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_DYKDialog.setMetaObject( metaObj );
    return metaObj;
}

void* DYKDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "DYKDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool DYKDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: nextTip(); break;
    case 1: prevTip(); break;
    case 2: finished(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool DYKDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool DYKDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool DYKDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
