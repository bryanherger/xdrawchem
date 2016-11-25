#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>

#include "netchoosedialog.h"
#include "defs.h"

NetChooseDialog::NetChooseDialog( QWidget * parent, QStringList r1 )
    : QDialog( parent )
{
    results = r1;

    QPushButton *ok, *dismiss;
    QLabel *cap1;

    setWindowTitle( "Choose Structure" );

    QGridLayout *mygrid = new QGridLayout( this );

    cap1 = new QLabel( "Select a molecule:", this );
    mygrid->addWidget( cap1, 0, 0, 1, 4 );

    lv = new QListWidget( this );
    lv->addItem( "CAS" );
    lv->addItem( "IUPAC Name" );
    lv->addItem( "Formula" );
    lv->addItem( "Other names" );
    lv->addItem( "SMILES" );
//  lv->setSorting(0); ///TODO
    mygrid->addWidget( lv, 1, 0, 3, 4 );

    int i1;
    QString tmp_str, tcas, tformula, tformat, tname, taltname;
    QListWidgetItem *lvi;

    for ( QStringList::Iterator ir = r1.begin(); ir != r1.end(); ++ir ) {
        tmp_str = *ir;
        tmp_str.replace( "\",\"", "~" );
        i1 = tmp_str.indexOf( "~" );
        tcas = tmp_str.mid( 1, i1 - 1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tcas = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tname = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tformula = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        taltname = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tformat = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        //qDebug() << tcas << "," << tname <<","<< tformula << ",";
        tname.replace( "\"", "" );
        tcas.replace( "\"", "" );
        tformat.replace( "\"", "" );
        taltname.replace( "\"", "" );
        tformula.replace( "\"", "" );
        lvi = new QListWidgetItem( lv );
        lvi->setText( tcas );
        lvi->setText( tname );
        lvi->setText( tformula );
        lvi->setText( taltname );
        lvi->setText( tformat );
        //lv->addItem(lvi);
    }

    QHBoxLayout *buttonHBox = new QHBoxLayout( this );
    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );

    buttonHBox->addItem( spacer );

    ok = new QPushButton( tr( "Select" ), this );
    connect( ok, SIGNAL( clicked() ), SLOT( OK() ) );
    buttonHBox->addWidget( ok );

    dismiss = new QPushButton( tr( "Cancel" ), this );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
    buttonHBox->addWidget( dismiss );

    mygrid->addLayout( buttonHBox, 4, 0, 1, 4 );
}

void NetChooseDialog::OK()
{
    // save SMILES string of selected item
    fn = lv->currentItem()->text();
    fn.replace( "\"", "" );     // strip quotes
    accept();
}

//cmake#include "netchoosedialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
