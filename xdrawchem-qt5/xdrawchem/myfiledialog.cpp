#include <QtGlobal>
#include <QLayout>

#include "myfiledialog.h"

MyFileDialog::MyFileDialog( QWidget *parent )
    : QFileDialog( parent )
{
    setOption( QFileDialog::DontUseNativeDialog, true );
    setAcceptMode( QFileDialog::AcceptSave );
    trans_toggle = new QCheckBox( tr( "Transparent" ), this );
    trans_toggle->setChecked( false );
    layout()->addWidget( trans_toggle ); ///TODO: place
}

bool MyFileDialog::isTransparent()
{
    return trans_toggle->isChecked();
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
