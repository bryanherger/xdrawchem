/****************************************************************************
** SmilesDialog meta object code from reading C++ file 'smilesdialog.h'
**
** Created: Wed Nov 23 20:29:38 2016
**      by: The Qt MOC ($Id: qt/moc_yacc.cpp   3.3.8   edited Feb 2 14:59 $)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "smilesdialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 26)
#error "This file was generated using the moc from 3.3.8b. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *SmilesDialog::className() const
{
    return "SmilesDialog";
}

QMetaObject *SmilesDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_SmilesDialog( "SmilesDialog", &SmilesDialog::staticMetaObject );

#ifndef QT_NO_TRANSLATION
QString SmilesDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SmilesDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString SmilesDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "SmilesDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* SmilesDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = QDialog::staticMetaObject();
    metaObj = QMetaObject::new_metaobject(
	"SmilesDialog", parentObject,
	0, 0,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_SmilesDialog.setMetaObject( metaObj );
    return metaObj;
}

void* SmilesDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "SmilesDialog" ) )
	return this;
    return QDialog::qt_cast( clname );
}

bool SmilesDialog::qt_invoke( int _id, QUObject* _o )
{
    return QDialog::qt_invoke(_id,_o);
}

bool SmilesDialog::qt_emit( int _id, QUObject* _o )
{
    return QDialog::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool SmilesDialog::qt_property( int id, int f, QVariant* v)
{
    return QDialog::qt_property( id, f, v);
}

bool SmilesDialog::qt_static_property( QObject* , int , int , QVariant* ){ return FALSE; }
#endif // QT_NO_PROPERTIES
