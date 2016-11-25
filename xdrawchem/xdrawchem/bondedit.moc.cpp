/****************************************************************************
** BondEditDialog meta object code from reading C++ file 'bondedit.h'
**
** Created: Wed Nov 23 20:29:36 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "bondedit.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *BondEditDialog::className() const
{
    return "BondEditDialog";
}

QMetaObject *BondEditDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_BondEditDialog( "BondEditDialog", &BondEditDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString BondEditDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "BondEditDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString BondEditDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "BondEditDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* BondEditDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"GetNewColor", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ "s1", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"StyleChanged", 1, param_slot_1 };
    static const QUParameter param_slot_2[] = {
	{ "o1", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_2 = {"OrderChanged", 1, param_slot_2 };
    static const QUParameter param_slot_3[] = {
	{ "tnew", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_3 = {"SetThick", 1, param_slot_3 };
    static const QUParameter param_slot_4[] = {
	{ "tnew", &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_4 = {"SetDoubleBond", 1, param_slot_4 };
    static const QMetaData slot_tbl[] = {
	{ "GetNewColor()", &slot_0, QMetaData::Public },
	{ "StyleChanged(int)", &slot_1, QMetaData::Public },
	{ "OrderChanged(int)", &slot_2, QMetaData::Public },
	{ "SetThick(int)", &slot_3, QMetaData::Public },
	{ "SetDoubleBond(int)", &slot_4, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"BondEditDialog", parentObject,
	slot_tbl, 5,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_BondEditDialog.setMetaObject( metaObj );
    return metaObj;
}

void* BondEditDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "BondEditDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool BondEditDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: GetNewColor(); break;
    case 1: StyleChanged((int)static_QUType_int.get(_o+1)); break;
    case 2: OrderChanged((int)static_QUType_int.get(_o+1)); break;
    case 3: SetThick((int)static_QUType_int.get(_o+1)); break;
    case 4: SetDoubleBond((int)static_QUType_int.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool BondEditDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool BondEditDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool BondEditDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
