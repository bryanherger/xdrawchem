#include <QGridLayout>
#include <QLabel>
#include <QHBoxLayout>
#include <QSpacerItem>

#include "dyk.h"

DYKDialog::DYKDialog( QWidget *parent )
    : QDialog( parent )
{
    populateTipList();
    setWindowTitle( tr( "Did You Know?" ) );

    QGridLayout *dykLayout = new QGridLayout();

    QLabel *dykLabel = new QLabel( tr( "Did You Know?") );
    dykLayout->addWidget( dykLabel, 0, 0, 1, 1);

    tipcount = preferences.getDYKCount();
    if ( tipcount >= tips.count() )
        tipcount = 0;
    curtip = new QLabel();
    curtip->setText( tips[tipcount] );
    dykLayout->addWidget( curtip, 1, 0, 1, 2 );

    QSpacerItem *dykSpacer = new QSpacerItem( 1, 1, QSizePolicy::Minimum, QSizePolicy::MinimumExpanding );
    dykLayout->addItem( dykSpacer, 2, 0, 1, 2 );

    futureshow = new QCheckBox( tr( "Don't show this dialog at startup" ) );
    futureshow->setChecked( !preferences.getDYK() );
    dykLayout->addWidget( futureshow, 3, 0, 1, 2 );

    QHBoxLayout *btnLayout = new QHBoxLayout();

    QPushButton *prevTipBtn = new QPushButton( tr( "Previous tip" ) );
    connect( prevTipBtn, SIGNAL( clicked() ), SLOT( prevTip() ) );
    btnLayout->addWidget( prevTipBtn );

    QPushButton *nextTipBtn = new QPushButton( tr( "Next tip" ) );
    connect( nextTipBtn, SIGNAL( clicked() ), SLOT( nextTip() ) );
    btnLayout->addWidget( nextTipBtn );

    QSpacerItem *btnSpacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );
    btnLayout->addItem( btnSpacer );

    QPushButton *okBtn = new QPushButton( tr( "OK" ) );
    connect( okBtn, SIGNAL( clicked() ), SLOT( finished() ) );
    btnLayout->addWidget( okBtn );

    dykLayout->addLayout(btnLayout, 4, 0, 1, 2 );
    setLayout( dykLayout );
}

void DYKDialog::finished()
{
    preferences.setDYK( !futureshow->isChecked() );
    tipcount++;
    if ( tipcount >= tips.count() )
        tipcount = 0;
    preferences.setDYKCount( tipcount );
    accept();
}

void DYKDialog::populateTipList()
{
    tips.append( tr( "XDrawChem can be run from the command line\n"
                     "to produce images.\n"
                     "Type \"xdrawchem --help\" for details." ) );
    tips.append( tr( "When in select mode, you can edit most objects\n"
                     "by right-clicking on them." ) );
    tips.append( tr( "When drawing bonds, you can edit the bond style\n"
                     "by right-clicking on the bond." ) );
    tips.append( tr( "When in select mode, press Shift+LeftButton and drag\n"
                     "over part of a molecule to select the entire molecule." ) );
    tips.append( tr( "When in select mode, press Ctrl+LeftButton\n"
                     "on a molecule to get molecule information." ) );
    tips.append( tr( "Bond length, bond angle, and units are set\n"
                     "in the Drawing Settings option under the Format menu." ) );
    tips.append( tr( "You can attach a subscript to a bracket by selecting\n"
                     "the text tool, then clicking on the bracket." ) );
    tips.append( tr( "You can toggle the grid display by pressing Ctrl+G.\n"
                     "Turning the grid on or off does not affect whether\n"
                     "objects snap to the grid." ) );
    tips.append( tr( "A number of ring and molecule templates are available\n"
                     "from the ring tool, including single and fused rings,\n"
                     "amino acids, and nucleosides." ) );
    tips.append( tr( "Remember to select a file type when\n"
                     "opening or saving documents." ) );
    tips.append( tr( "The option \"Clean up molecule\" under\n"
                     "the Tools menu can be used to refine structures\n"
                     "such as rings and macromolecules." ) );
    tips.append( tr( "It's possible to attach rings to structures\n"
                     "by selecting a ring and clicking on an atom." ) );
    tips.append( tr( "XDrawChem can automatically attach\n"
                     "amino acids from N-terminal to C-terminal\n"
                     "(but the result isn't always pretty :)" ) );
    tips.append( tr( "Be sure to check for updates at\n"
                     "http://xdrawchem.sourceforge.net/" ) );
    tips.append( tr( "XDrawChem can draw over 100,000 substances using\n"
                     "its Internet database.  Select Find on internet\n"
                     "from the File menu to search for compounds." ) );
    tips.append( tr( "XDrawChem can identify compounds by CAS number\n"
                     "and occasionally by name.\n"
                     "Select Molecule information from the Tools menu\n"
                     "and click on a molecule.  However, this only works\n"
                     "if the compound is listed in the database." ) );
}

void DYKDialog::nextTip()
{
    tipcount++;
    if ( tipcount >= tips.count() )
        tipcount = 0;
    curtip->setText( tips[tipcount] );
    updateGeometry();
}

void DYKDialog::prevTip()
{
    tipcount--;
    if ( tipcount < 0 )
        tipcount = tips.count() - 1;
    curtip->setText( tips[tipcount] );
    updateGeometry();
}

//cmake#include "dyk.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
