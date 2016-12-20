#include <cstdlib>

#include <QLineEdit>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QSpacerItem>
#include <QPushButton>
#include <QLabel>
#include <QCheckBox>

#include "netdialog.h"
#include "defs.h"
//#include "http.h"

NetDialog::NetDialog( QWidget * parent )
:  QDialog( parent )
{
    QString serverName;

    serverName = getenv( "XDC_SERVER" );

    if ( serverName.length() < 2 )
        serverName = XDC_SERVER;

    QLabel *cap1, *caption, *l1;

    setWindowTitle( tr( "Find structure via Internet" ) );

    QGridLayout *mygrid = new QGridLayout( this );

    cap1 = new QLabel( tr( "XDC database server:" ) );
    mygrid->addWidget( cap1, 0, 0, 1, 4 );

    serverkey = new QLineEdit();
    serverkey->setText( serverName );
    mygrid->addWidget( serverkey, 0, 4, 1, 4 );

    caption = new QLabel( tr( "Search type:" ) );
    mygrid->addWidget( caption, 1, 0, 1, 4 );

    keylist = new QComboBox();
    keylist->addItem( tr( "Chemical name" ) );
    keylist->addItem( tr( "CAS Number" ) );
    keylist->addItem( tr( "Formula" ) );
    mygrid->addWidget( keylist, 1, 4, 1, 4 );

    l1 = new QLabel( tr( "Look for:" ), this );
    mygrid->addWidget( l1, 2, 0, 1, 4 );

    searchkey = new QLineEdit();
    mygrid->addWidget( searchkey, 2, 4, 1, 4 );
    connect( searchkey, SIGNAL( returnPressed() ), SLOT( accept() ) );
    searchkey->setFocus();

    emcheck = new QCheckBox( tr( "Exact matches only" ) );
    mygrid->addWidget( emcheck, 3, 0, 1, 8 );

    QHBoxLayout *buttonHBox = new QHBoxLayout();
    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );

    buttonHBox->addItem( spacer );

    QPushButton *ok, *dismiss;

    ok = new QPushButton( tr( "Search" ) );
    connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
    buttonHBox->addWidget( ok );

    dismiss = new QPushButton( tr( "Cancel" ) );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
    buttonHBox->addWidget( dismiss );

    mygrid->addLayout( buttonHBox, 4, 0, 1, 8 );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
