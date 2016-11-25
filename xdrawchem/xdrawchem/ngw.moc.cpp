/****************************************************************************
** NewGraphWidget meta object code from reading C++ file 'ngw.h'
**
** Created: Wed Nov 23 20:29:57 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "ngw.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *NewGraphWidget::className() const
{
    return "NewGraphWidget";
}

QMetaObject *NewGraphWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_NewGraphWidget( "NewGraphWidget", &NewGraphWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString NewGraphWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NewGraphWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString NewGraphWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "NewGraphWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* NewGraphWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"Print", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "Print()", &slot_0, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"NewGraphWidget", parentObject,
	slot_tbl, 1,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_NewGraphWidget.setMetaObject( metaObj );
    return metaObj;
}

void* NewGraphWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "NewGraphWidget" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool NewGraphWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: Print(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool NewGraphWidget::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool NewGraphWidget::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool NewGraphWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
