#include <QClipboard>
#include <QGridLayout>
#include <QSpacerItem>
#include <QHBoxLayout>
#include <QApplication>

#include "molinfodialog.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

MolInfoDialog::MolInfoDialog( QWidget * parent )
    : QDialog( parent )
{
    setWindowTitle( tr( "Molecule Info" ) );
    setMinimumWidth( 400 );

    QGridLayout *grid = new QGridLayout();

    nameLabel = new QLabel( tr( "Name: " ) );
    nameLabel->setText( tr( "Name: " ) );
    grid->addWidget( nameLabel, 0, 0, 1, 1 );

    synLabel = new QLabel( tr( "Other names: " ) );
//    synLabel->setAlignment( synLabel->alignment() | Qt::TextWordWrap );
    grid->addWidget( synLabel, 1, 0, 1, 1 );

    casLabel = new QLabel( tr( "CAS: " ) );
    grid->addWidget( casLabel, 2, 0, 1, 1 );

    pcLabel = new QLabel( tr( "PubChem compound: " ) );
    grid->addWidget( pcLabel, 3, 0, 1, 1 );

    molWeight = tr( "Molecular weight = " );
    molWeightCheckBox = new QCheckBox( molWeight );
    grid->addWidget( molWeightCheckBox, 4, 0, 1, 1 );

    ef = tr( "Formula = " );
    ef_check = new QCheckBox( ef );
    grid->addWidget( ef_check, 5, 0, 1, 1 );

    ea_check = new QCheckBox( tr( "Analysis" ) );
    grid->addWidget( ea_check, 6, 0, 1, 1 );

    eaLabel = new QLabel( ea );
    grid->addWidget( eaLabel, 7, 0, 1, 1 );
    QPushButton *okBtn, *dismissBtn, *helpBtn;

    QHBoxLayout *btnLayout = new QHBoxLayout();

    getpcc = new QPushButton( tr( "Search PubChem" ) );
    connect( getpcc, SIGNAL( clicked() ), SLOT( doPubChem() ) );
    btnLayout->addWidget( getpcc );
    getpcc->setEnabled( false );

    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum );
    btnLayout->addItem( spacer );

    helpBtn = new QPushButton( tr( "Help" ) );
    connect( helpBtn, SIGNAL( clicked() ), SLOT( SendHelp() ) );
    btnLayout->addWidget( helpBtn );

    okBtn = new QPushButton( tr( "OK" ) );
    connect( okBtn, SIGNAL( clicked() ), SLOT( doAccept() ) );
    btnLayout->addWidget( okBtn );

    dismissBtn = new QPushButton( tr( "Cancel" ) );
    connect( dismissBtn, SIGNAL( clicked() ), SLOT( reject() ) );
    btnLayout->addWidget( dismissBtn );

    grid->addLayout( btnLayout, 8, 0, 1, 1 );
    setLayout( grid );
}

void MolInfoDialog::doAccept()
{
    emit MIDClose();
}

void MolInfoDialog::doPubChem()
{
    QString ffurl, ffcmd;

    ffurl = "http://pubchem.ncbi.nlm.nih.gov/summary/summary.cgi?cid=" + pcc;
    ffcmd = "firefox " + ffurl + " &";
    qDebug() << ffcmd;
    system( ffcmd.toLatin1() );
    QClipboard *cb = QApplication::clipboard();

    // Copy text into the clipboard
    cb->setText( ffurl, QClipboard::Clipboard );
}

void MolInfoDialog::SendHelp()
{
    QString home, topic;

    topic = "molinfo.html";
#ifdef UNIX
    home = RingDir + "doc/" + topic;
#else
    home = RingDir + "doc\\" + topic;
#endif
    HelpWindow *help = new HelpWindow( home, ".", 0 );

    help->setWindowTitle( QString( XDC_VERSION ) + " - Help viewer" );
    help->show();
}

//cmake#include "molinfodialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
