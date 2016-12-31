#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QButtonGroup>

#include "peptidebuilder.h"

PeptDialog::PeptDialog( QWidget *parent )
{
    setWindowTitle( tr( "Peptide Builder" ) );

    QGridLayout *mygrid = new QGridLayout( this );

    QLabel *ltop = new QLabel( tr( "Enter peptide sequence -\nthree-letter abbreviations:" ) );
    mygrid->addWidget( ltop, 0, 0, 0, 7 );

    peptide = new QLineEdit();
    mygrid->addWidget( peptide, 1, 1, 0, 7 );

    QLabel *lne = new QLabel( tr( "Shortcut: Hold 'Alt' and type one-letter codes!" ) );
    mygrid->addWidget( lne, 2, 2, 0, 7 );

    QVBoxLayout *btnBoxLayout = new QVBoxLayout();
    QButtonGroup *bgroup = new QButtonGroup( this );
    QPushButton *b1;

    b1 = new QPushButton( "Ala (&A)" );
    btnBoxLayout->addWidget( b1 );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Arg (&R)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Asn (&N)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Asp (&D)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Cys (&C)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Gln (&Q)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Glu (&E)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Gly (&G)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "His (&H)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Ile (&I)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Leu (&L)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Lys (&K)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Met (&M)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Nph" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Phe (&F)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Pro (&P)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Ser (&S)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Sta" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Thr (&T)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Trp (&W)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Tyr (&Y)" );
    bgroup->addButton( b1 );
    b1 = new QPushButton( "Val (&V)" );
    bgroup->addButton( b1 );
    connect( bgroup, SIGNAL( buttonClicked( int ) ), this, SLOT( buttonPress( int ) ) );

    mygrid->addLayout( btnBoxLayout, 3, 4, 0, 7 );
    QPushButton *ok, *dismiss;

    ok = new QPushButton( tr( "OK" ), this );
    connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
    mygrid->addWidget( ok, 5, 5, 0, 1 );
    dismiss = new QPushButton( tr( "Cancel" ), this );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
    mygrid->addWidget( dismiss, 5, 5, 6, 7 );
}

QString PeptDialog::getPeptide()
{
    return peptide->text();
}

void PeptDialog::buttonPress( int b1 )
{
    switch ( b1 ) {
    case 0:
        peptide->insert( "Ala" );
        break;
    case 1:
        peptide->insert( "Arg" );
        break;
    case 2:
        peptide->insert( "Asn" );
        break;
    case 3:
        peptide->insert( "Asp" );
        break;
    case 4:
        peptide->insert( "Cys" );
        break;
    case 5:
        peptide->insert( "Gln" );
        break;
    case 6:
        peptide->insert( "Glu" );
        break;
    case 7:
        peptide->insert( "Gly" );
        break;
    case 8:
        peptide->insert( "His" );
        break;
    case 9:
        peptide->insert( "Ile" );
        break;
    case 10:
        peptide->insert( "Leu" );
        break;
    case 11:
        peptide->insert( "Lys" );
        break;
    case 12:
        peptide->insert( "Met" );
        break;
    case 13:
        peptide->insert( "Nph" );
        break;
    case 14:
        peptide->insert( "Phe" );
        break;
    case 15:
        peptide->insert( "Pro" );
        break;
    case 16:
        peptide->insert( "Ser" );
        break;
    case 17:
        peptide->insert( "Sta" );
        break;
    case 18:
        peptide->insert( "Thr" );
        break;
    case 19:
        peptide->insert( "Trp" );
        break;
    case 20:
        peptide->insert( "Tyr" );
        break;
    case 21:
        peptide->insert( "Val" );
        break;
    default:
        break;
    }
}

//cmake#include "peptidebuilder.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
