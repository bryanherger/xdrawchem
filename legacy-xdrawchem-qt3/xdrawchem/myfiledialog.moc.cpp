/****************************************************************************
** MyFileDialog meta object code from reading C++ file 'myfiledialog.h'
**
** Created: Wed Nov 23 20:29:47 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "myfiledialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MyFileDialog::className() const
{
    return "MyFileDialog";
}

QMetaObject *MyFileDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MyFileDialog( "MyFileDialog", &MyFileDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString MyFileDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MyFileDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MyFileDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MyFileDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MyFileDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QFileDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"MyFileDialog", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MyFileDialog.setMetaObject( metaObj );
    return metaObj;
}

void* MyFileDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MyFileDialog" ) )
	return this;
    return QFileDialog::qt_cast( clname );
}

bool MyFileDialog::qt_invoke( int _id, QUObject* _o )
{
    return QFileDialog::qt_invoke(_id,_o);
}

bool MyFileDialog::qt_emit( int _id, QUObject* _o )
{
    return QFileDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MyFileDialog::qt_property( int id, int f, QVariant* v)
{
    return QFileDialog::qt_property( id, f, v);
}

bool MyFileDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
