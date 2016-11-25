#include <QGridLayout>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QDir>

#include "ringdialog.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

RingDialog::RingDialog( QWidget *parent )
    : QDialog( parent )
{
    // Init variables which tell which ring was picked
    fn = QString( "noring" );

    QPixmap pix;
    QLabel *caption;
    QPushButton *dismiss;

    setWindowTitle( tr( "Ring tool" ) );

    QGridLayout *mygrid = new QGridLayout( this );

    caption = new QLabel( tr( "Choose a structure from list:" ) );
    mygrid->addWidget( caption, 0, 0, 0, 4 );

    QDir dr( RingDir, "*.cml" );
    QStringList dl = dr.entryList();

    filelist = new QComboBox();
    filelist->addItem( "(pick)" );
    for ( int cc = 0; cc < dl.count(); cc++ )
        filelist->addItem( dl[cc].left( dl[cc].length() - 4 ) );
    connect( filelist, SIGNAL( activated( int ) ), this, SLOT( fromComboBox( int ) ) );

    mygrid->addWidget( filelist, 1, 1, 0, 4 );

    dismiss = new QPushButton( tr( "Cancel" ) );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
    mygrid->addWidget( dismiss, 2, 2, 1, 3 );
}

void RingDialog::fromComboBox( int i )
{
    if ( filelist->currentText() == "(pick)" )
        return;
    fn = filelist->currentText() + QString( ".cml" );
    //qDebug() << "File: " << fn ;
    accept();
}

//cmake#include "ringdialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
