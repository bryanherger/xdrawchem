/****************************************************************************
** PageSetupDialog meta object code from reading C++ file 'pagesetupdialog.h'
**
** Created: Wed Nov 23 20:29:59 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "pagesetupdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *PageSetupDialog::className() const
{
    return "PageSetupDialog";
}

QMetaObject *PageSetupDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_PageSetupDialog( "PageSetupDialog", &PageSetupDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString PageSetupDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "PageSetupDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString PageSetupDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "PageSetupDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* PageSetupDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"onSuccessful", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"SwapWH", 1, param_slot_1 };
    static const QMetaData slot_tbl[] = {
	{ "onSuccessful()", &slot_0, QMetaData::Public },
	{ "SwapWH(int)", &slot_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"PageSetupDialog", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_PageSetupDialog.setMetaObject( metaObj );
    return metaObj;
}

void* PageSetupDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "PageSetupDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool PageSetupDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: onSuccessful(); break;
    case 1: SwapWH((int)static_QUType_int.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool PageSetupDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool PageSetupDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool PageSetupDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
