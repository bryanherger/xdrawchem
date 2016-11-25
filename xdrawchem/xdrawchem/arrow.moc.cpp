/****************************************************************************
** Arrow meta object code from reading C++ file 'arrow.h'
**
** Created: Wed Nov 23 20:29:34 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "arrow.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Arrow::className() const
{
    return "Arrow";
}

QMetaObject *Arrow::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Arrow( "Arrow", &Arrow::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Arrow::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Arrow", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Arrow::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Arrow", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Arrow::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Drawable::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Arrow", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Arrow.setMetaObject( metaObj );
    return metaObj;
}

void* Arrow::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Arrow" ) )
	return this;
    return Drawable::qt_cast( clname );
}

bool Arrow::qt_invoke( int _id, QUObject* _o )
{
    return Drawable::qt_invoke(_id,_o);
}

bool Arrow::qt_emit( int _id, QUObject* _o )
{
    return Drawable::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Arrow::qt_property( int id, int f, QVariant* v)
{
    return Drawable::qt_property( id, f, v);
}

bool Arrow::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
