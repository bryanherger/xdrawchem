/****************************************************************************
** GraphWidget meta object code from reading C++ file 'graphwidget.h'
**
** Created: Wed Nov 23 20:29:32 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "graphwidget.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *GraphWidget::className() const
{
    return "GraphWidget";
}

QMetaObject *GraphWidget::metaObj = 0;
static QMetaObjectCleanUp cleanUp_GraphWidget( "GraphWidget", &GraphWidget::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString GraphWidget::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "GraphWidget", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString GraphWidget::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "GraphWidget", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* GraphWidget::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QWidget::staticMetaObject();
    static const QUMethod slot_0 = {"Print", 0, 0 };
    static const QUMethod slot_1 = {"Export", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "Print()", &slot_0, QMetaData::Public },
	{ "Export()", &slot_1, QMetaData::Public }
    };
    metaObj = QMetaObject::new_metaobject(
	"GraphWidget", parentObject,
	slot_tbl, 2,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_GraphWidget.setMetaObject( metaObj );
    return metaObj;
}

void* GraphWidget::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "GraphWidget" ) )
	return this;
    return QWidget::qt_cast( clname );
}

bool GraphWidget::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: Print(); break;
    case 1: Export(); break;
    default:
	return QWidget::qt_invoke( _id, _o );
    }
    return TRUE;
}

bool GraphWidget::qt_emit( int _id, QUObject* _o )
{
    return QWidget::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool GraphWidget::qt_property( int id, int f, QVariant* v)
{
    return QWidget::qt_property( id, f, v);
}

bool GraphWidget::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
