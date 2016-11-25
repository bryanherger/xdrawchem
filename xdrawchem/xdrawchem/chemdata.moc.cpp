/****************************************************************************
** ChemData meta object code from reading C++ file 'chemdata.h'
**
** Created: Wed Nov 23 20:29:46 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "chemdata.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *ChemData::className() const
{
    return "ChemData";
}

QMetaObject *ChemData::metaObj = 0;
static QMetaObjectCleanUp cleanUp_ChemData( "ChemData", &ChemData::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString ChemData::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ChemData", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString ChemData::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "ChemData", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* ChemData::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QObject::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_ptr, "XDC_Event", QUParameter::In }
    };
    static const QUMethod slot_0 = {"XDCEventHandler", 1, param_slot_0 };
    static const QUMethod slot_1 = {"returnFromMID", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "XDCEventHandler(XDC_Event*)", &slot_0, QMetaData::Public },
	{ "returnFromMID()", &slot_1, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"SignalSetStatusBar", 1, param_signal_0 };
    static const QUParameter param_signal_1[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_1 = {"SignalHelpTopic", 1, param_signal_1 };
    static const QUMethod signal_2 = {"SignalUpdateCustomRingMenu", 0, 0 };
    static const QMetaData signal_tbl[] = {
	{ "SignalSetStatusBar(QString)", &signal_0, QMetaData::Public },
	{ "SignalHelpTopic(QString)", &signal_1, QMetaData::Public },
	{ "SignalUpdateCustomRingMenu()", &signal_2, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"ChemData", parentObject,
	slot_tbl, 2,
	signal_tbl, 3,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_ChemData.setMetaObject( metaObj );
    return metaObj;
}

void* ChemData::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "ChemData" ) )
	return this;
    return QObject::qt_cast( clname );
}

// SIGNAL SignalSetStatusBar
void ChemData::SignalSetStatusBar( QString t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

// SIGNAL SignalHelpTopic
void ChemData::SignalHelpTopic( QString t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 1, t0 );
}

// SIGNAL SignalUpdateCustomRingMenu
void ChemData::SignalUpdateCustomRingMenu()
{
    activate_signal( staticMetaObject()->signalOffset() + 2 );
}

bool ChemData::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: XDCEventHandler((XDC_Event*)static_QUType_ptr.get(_o+1)); break;
    case 1: returnFromMID(); break;
    default:
	return QObject::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool ChemData::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: SignalSetStatusBar((QString)static_QUType_QString.get(_o+1)); break;
    case 1: SignalHelpTopic((QString)static_QUType_QString.get(_o+1)); break;
    case 2: SignalUpdateCustomRingMenu(); break;
    default:
	return QObject::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool ChemData::qt_property( int id, int f, QVariant* v)
{
    return QObject::qt_property( id, f, v);
}

bool ChemData::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
