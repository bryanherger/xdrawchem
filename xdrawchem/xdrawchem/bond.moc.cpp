/****************************************************************************
** Bond meta object code from reading C++ file 'bond.h'
**
** Created: Wed Nov 23 20:29:39 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "bond.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Bond::className() const
{
    return "Bond";
}

QMetaObject *Bond::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Bond( "Bond", &Bond::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Bond::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Bond", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Bond::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Bond", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Bond::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Drawable::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Bond", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Bond.setMetaObject( metaObj );
    return metaObj;
}

void* Bond::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Bond" ) )
	return this;
    return Drawable::qt_cast( clname );
}

bool Bond::qt_invoke( int _id, QUObject* _o )
{
    return Drawable::qt_invoke(_id,_o);
}

bool Bond::qt_emit( int _id, QUObject* _o )
{
    return Drawable::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Bond::qt_property( int id, int f, QVariant* v)
{
    return Drawable::qt_property( id, f, v);
}

bool Bond::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
