/****************************************************************************
** Bracket meta object code from reading C++ file 'bracket.h'
**
** Created: Wed Nov 23 20:29:43 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "bracket.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Bracket::className() const
{
    return "Bracket";
}

QMetaObject *Bracket::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Bracket( "Bracket", &Bracket::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Bracket::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Bracket", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Bracket::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Bracket", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Bracket::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Drawable::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Bracket", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Bracket.setMetaObject( metaObj );
    return metaObj;
}

void* Bracket::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Bracket" ) )
	return this;
    return Drawable::qt_cast( clname );
}

bool Bracket::qt_invoke( int _id, QUObject* _o )
{
    return Drawable::qt_invoke(_id,_o);
}

bool Bracket::qt_emit( int _id, QUObject* _o )
{
    return Drawable::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Bracket::qt_property( int id, int f, QVariant* v)
{
    return Drawable::qt_property( id, f, v);
}

bool Bracket::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
