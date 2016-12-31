/****************************************************************************
** Molecule meta object code from reading C++ file 'molecule.h'
**
** Created: Wed Nov 23 20:29:40 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "molecule.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Molecule::className() const
{
    return "Molecule";
}

QMetaObject *Molecule::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Molecule( "Molecule", &Molecule::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Molecule::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Molecule", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Molecule::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Molecule", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Molecule::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = Drawable::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Molecule", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Molecule.setMetaObject( metaObj );
    return metaObj;
}

void* Molecule::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Molecule" ) )
	return this;
    return Drawable::qt_cast( clname );
}

bool Molecule::qt_invoke( int _id, QUObject* _o )
{
    return Drawable::qt_invoke(_id,_o);
}

bool Molecule::qt_emit( int _id, QUObject* _o )
{
    return Drawable::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Molecule::qt_property( int id, int f, QVariant* v)
{
    return Drawable::qt_property( id, f, v);
}

bool Molecule::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
