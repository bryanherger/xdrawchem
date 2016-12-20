#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "application.h"
#include "chemdata.h"
#include "render2d.h"
#include "smilesdialog.h"

void ApplicationWindow::FromSMILES()
{
    SmilesDialog i( this );

    if ( !i.exec() )
        return;
    QString sm = i.getSMILES();

    m_chemData->fromSMILES( sm );
    m_renderer->Inserted();
    m_renderer->update();
}

SmilesDialog::SmilesDialog( QWidget *parent )
    : QDialog( parent )
{
    setWindowTitle( tr( "Enter SMILES string" ) );

    QVBoxLayout *smilesLayout = new QVBoxLayout();

    QLabel *smilesLabel = new QLabel( tr( "Enter SMILES string:" ), this );
    smilesLayout->addWidget( smilesLabel );

    smilesInput = new QLineEdit( this );
    connect( smilesInput, SIGNAL( returnPressed() ), this, SLOT( accept() ) );
    smilesLayout->addWidget( smilesInput );

    QHBoxLayout *btnLayout = new QHBoxLayout();
    QSpacerItem *btnSpacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
    btnLayout->addItem( btnSpacer );

    QPushButton *okBtn = new QPushButton( tr( "OK" ), this );
    connect( okBtn, SIGNAL( clicked() ), SLOT( accept() ) );
    btnLayout->addWidget( okBtn );

    QPushButton *cancelBtn = new QPushButton( tr( "Cancel" ), this );
    connect( cancelBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    btnLayout->addWidget( cancelBtn );

    smilesLayout->addLayout( btnLayout );
    setLayout( smilesLayout );
}

QString SmilesDialog::getSMILES()
{
    return smilesInput->text();
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
