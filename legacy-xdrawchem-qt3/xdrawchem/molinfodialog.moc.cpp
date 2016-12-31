/****************************************************************************
** MolInfoDialog meta object code from reading C++ file 'molinfodialog.h'
**
** Created: Wed Nov 23 20:29:44 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "molinfodialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MolInfoDialog::className() const
{
    return "MolInfoDialog";
}

QMetaObject *MolInfoDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MolInfoDialog( "MolInfoDialog", &MolInfoDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MolInfoDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MolInfoDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MolInfoDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MolInfoDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MolInfoDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"SendHelp", 0, 0 };
    static const QUMethod slot_1 = {"doAccept", 0, 0 };
    static const QUMethod slot_2 = {"doPubChem", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "SendHelp()", &slot_0, QMetaData::Public },
	{ "doAccept()", &slot_1, QMetaData::Public },
	{ "doPubChem()", &slot_2, QMetaData::Public }
    };
    static const QUMethod signal_0 = {"MIDClose", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "MIDClose()", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"MolInfoDialog", parentObject,
	slot_tbl, 3,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MolInfoDialog.setMetaObject( metaObj );
    return metaObj;
}

void* MolInfoDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MolInfoDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

// SIGNAL MIDClose
void MolInfoDialog::MIDClose()
{
    activate_signal( staticMetaObject()->signalOffset() + 0 );
}

bool MolInfoDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: SendHelp(); break;
    case 1: doAccept(); break;
    case 2: doPubChem(); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool MolInfoDialog::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: MIDClose(); break;
    default:
	return QDialog::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool MolInfoDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool MolInfoDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
