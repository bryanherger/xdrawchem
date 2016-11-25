/****************************************************************************
** Tool_2D3D meta object code from reading C++ file 'tool_2d3d.h'
**
** Created: Wed Nov 23 20:29:54 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "tool_2d3d.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *Tool_2D3D::className() const
{
    return "Tool_2D3D";
}

QMetaObject *Tool_2D3D::metaObj = 0;
static QMetaObjectCleanUp cleanUp_Tool_2D3D( "Tool_2D3D", &Tool_2D3D::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString Tool_2D3D::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Tool_2D3D", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString Tool_2D3D::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "Tool_2D3D", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* Tool_2D3D::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = ToolDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"Tool_2D3D", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_Tool_2D3D.setMetaObject( metaObj );
    return metaObj;
}

void* Tool_2D3D::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "Tool_2D3D" ) )
	return this;
    return ToolDialog::qt_cast( clname );
}

bool Tool_2D3D::qt_invoke( int _id, QUObject* _o )
{
    return ToolDialog::qt_invoke(_id,_o);
}

bool Tool_2D3D::qt_emit( int _id, QUObject* _o )
{
    return ToolDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool Tool_2D3D::qt_property( int id, int f, QVariant* v)
{
    return ToolDialog::qt_property( id, f, v);
}

bool Tool_2D3D::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
