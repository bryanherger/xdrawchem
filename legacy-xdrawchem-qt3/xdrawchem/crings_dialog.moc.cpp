/****************************************************************************
** CustomRingWidget meta object code from reading C++ file 'crings_dialog.h'
**
** Created: Wed Nov 23 20:29:50 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "crings_dialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *CustomRingWidget::className() const
{
    return "CustomRingWidget";
}

QMetaObject *CustomRingWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_CustomRingWidget( "CustomRingWidget", &CustomRingWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString CustomRingWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CustomRingWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString CustomRingWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CustomRingWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* CustomRingWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ "string", &static_QUType_QString, 0, QUParameter::InOut }
    };
    static const QUMethod slot_0 = {"slotTitle", 1, param_slot_0 };
    static const QMetaData slot_tbl[] = {
	{ "slotTitle(QString&)", &slot_0, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod signal_0 = {"signalTextChanged", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "signalTextChanged(const QString&)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"CustomRingWidget", parentObject,
	slot_tbl, 1,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_CustomRingWidget.setMetaObject( metaObj );
    return metaObj;
}

void* CustomRingWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "CustomRingWidget" ) )
	return this;
    return QWidget::qt_cast( clname );
}

// SIGNAL signalTextChanged
void CustomRingWidget::signalTextChanged( const QString& t0 )
{
    activate_signal( staticMetaObject()->signalOffset() + 0, t0 );
}

bool CustomRingWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotTitle((QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool CustomRingWidget::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: signalTextChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QWidget::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool CustomRingWidget::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool CustomRingWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES


const char *CustomRingDialog::className() const
{
    return "CustomRingDialog";
}

QMetaObject *CustomRingDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_CustomRingDialog( "CustomRingDialog", &CustomRingDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString CustomRingDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CustomRingDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString CustomRingDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "CustomRingDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* CustomRingDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    static const QUMethod slot_0 = {"slotOk", 0, 0 };
    static const QUParameter param_slot_1[] = {
	{ "string", &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"slotTextChanged", 1, param_slot_1 };
    static const QMetaData slot_tbl[] = {
	{ "slotOk()", &slot_0, QMetaData::Public },
	{ "slotTextChanged(const QString&)", &slot_1, QMetaData::Public }
    };
    static const QUParameter param_signal_0[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::InOut }
    };
    static const QUMethod signal_0 = {"signalTitle", 1, param_signal_0 };
    static const QMetaData signal_tbl[] = {
	{ "signalTitle(QString&)", &signal_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"CustomRingDialog", parentObject,
	slot_tbl, 2,
	signal_tbl, 1,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_CustomRingDialog.setMetaObject( metaObj );
    return metaObj;
}

void* CustomRingDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "CustomRingDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

#include <qobjectdefs.h>
#include <qsignalslotimp.h>

// SIGNAL signalTitle
void CustomRingDialog::signalTitle( QString& t0 )
{
    if ( signalsBlocked() )
	return;
    QConnectionList *clist = receivers( staticMetaObject()->signalOffset() + 0 );
    if ( !clist )
	return;
    QUObject o[2];
    static_QUType_QString.set(o+1,t0);
    activate_signal( clist, o );
    t0 = static_QUType_QString.get(o+1);
}

bool CustomRingDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: slotOk(); break;
    case 1: slotTextChanged((const QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QDialog::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool CustomRingDialog::qt_emit( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->signalOffset() ) {
    case 0: signalTitle((QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QDialog::qt_emit(_id,_o);
    }
    return TRUE;
}
#ifndef QT_NO_PROPERTIES

bool CustomRingDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool CustomRingDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
