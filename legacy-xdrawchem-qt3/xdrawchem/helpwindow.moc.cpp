/****************************************************************************
** HelpWindow meta object code from reading C++ file 'helpwindow.h'
**
** Created: Wed Nov 23 20:29:35 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "helpwindow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *HelpWindow::className() const
{
    return "HelpWindow";
}

QMetaObject *HelpWindow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_HelpWindow( "HelpWindow", &HelpWindow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString HelpWindow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "HelpWindow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString HelpWindow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "HelpWindow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* HelpWindow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QMainWindow::staticMetaObject();
    static const QUParameter param_slot_0[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_0 = {"setBackwardAvailable", 1, param_slot_0 };
    static const QUParameter param_slot_1[] = {
	{ 0, &static_QUType_bool, 0, QUParameter::In }
    };
    static const QUMethod slot_1 = {"setForwardAvailable", 1, param_slot_1 };
    static const QUMethod slot_2 = {"textChanged", 0, 0 };
    static const QUMethod slot_3 = {"about", 0, 0 };
    static const QUMethod slot_4 = {"print", 0, 0 };
    static const QUParameter param_slot_5[] = {
	{ 0, &static_QUType_QString, 0, QUParameter::In }
    };
    static const QUMethod slot_5 = {"pathSelected", 1, param_slot_5 };
    static const QMetaData slot_tbl[] = {
	{ "setBackwardAvailable(bool)", &slot_0, QMetaData::Private },
	{ "setForwardAvailable(bool)", &slot_1, QMetaData::Private },
	{ "textChanged()", &slot_2, QMetaData::Private },
	{ "about()", &slot_3, QMetaData::Private },
	{ "print()", &slot_4, QMetaData::Private },
	{ "pathSelected(const QString&)", &slot_5, QMetaData::Private }
    };
    metaObj = QMetaObject::new_metaobject(
	"HelpWindow", parentObject,
	slot_tbl, 6,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_HelpWindow.setMetaObject( metaObj );
    return metaObj;
}

void* HelpWindow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "HelpWindow" ) )
	return this;
    return QMainWindow::qt_cast( clname );
}

bool HelpWindow::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: setBackwardAvailable((bool)static_QUType_bool.get(_o+1)); break;
    case 1: setForwardAvailable((bool)static_QUType_bool.get(_o+1)); break;
    case 2: textChanged(); break;
    case 3: about(); break;
    case 4: print(); break;
    case 5: pathSelected((const QString&)static_QUType_QString.get(_o+1)); break;
    default:
	return QMainWindow::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool HelpWindow::qt_emit( int _id, QUObject* _o )
{
    return QMainWindow::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool HelpWindow::qt_property( int id, int f, QVariant* v)
{
    return QMainWindow::qt_property( id, f, v);
}

bool HelpWindow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
