/****************************************************************************
** CharSelLabel meta object code from reading C++ file 'charsel.h'
**
** Created: Wed Nov 23 20:30:02 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "charsel.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *CharSelLabel::className() const
{
    return "CharSelLabel";
}

QMetaObject *CharSelLabel::metaObj = 0;
static QMetaObjectCleanUp cleanUp_CharSelLabel( "CharSelLabel", &CharSelLabel::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString CharSelLabel::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CharSelLabel", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString CharSelLabel::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CharSelLabel", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* CharSelLabel::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QLabel::staticMetaObject();
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"thisChar", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "thisChar(int)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"CharSelLabel", parentObject,
	0, 0,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_CharSelLabel.setMetaObject( metaObj );
    return metaObj;
}

void* CharSelLabel::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "CharSelLabel" ) )
	return this;
    return QLabel::qt_cast( clname );
}

// SIGNAL thisChar
void CharSelLabel::thisChar( int t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

bool CharSelLabel::qt_invoke( int _id, QUObject* _o )
{
    return QLabel::qt_invoke(_id,_o);
}

bool CharSelLabel::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: thisChar((int)static_QUType_int.get(_o+1)); break;
    default:
	return QLabel::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool CharSelLabel::qt_property( int id, int f, QVariant* v)
{
    return QLabel::qt_property( id, f, v);
}

bool CharSelLabel::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *CharSelDialog::className() const
{
    return "CharSelDialog";
}

QMetaObject *CharSelDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_CharSelDialog( "CharSelDialog", &CharSelDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString CharSelDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CharSelDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString CharSelDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CharSelDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* CharSelDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"fontChange", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_int, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"chosenChar", 1, param_slot_1 };
    static const QMetaData slot_tbl[] = {
	{ "fontChange(int)", &slot_0, QMetaData::Public },
	{ "chosenChar(int)", &slot_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"CharSelDialog", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_CharSelDialog.setMetaObject( metaObj );
    return metaObj;
}

void* CharSelDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "CharSelDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool CharSelDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: fontChange((int)static_QUType_int.get(_o+1)); break;
    case 1: chosenChar((int)static_QUType_int.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool CharSelDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool CharSelDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool CharSelDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
