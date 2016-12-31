#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QDebug>

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

    tw = new QTableWidget( this );
    tw->setRowCount( r1.size() );
    tw->setColumnCount(5);
    QStringList m_TableHeader;
    m_TableHeader << "CAS" << "IUPAC Name" << "Formula" << "Other names" << "SMILES";
    tw->setHorizontalHeaderLabels(m_TableHeader);
    tw->verticalHeader()->setVisible(false);
    tw->setEditTriggers(QAbstractItemView::NoEditTriggers);
    tw->setSelectionBehavior(QAbstractItemView::SelectRows);
    tw->setSelectionMode(QAbstractItemView::SingleSelection);
    tw->setShowGrid(false);
//  lv->setSorting(0); ///TODO; this should be set after loading data anyways.
    mygrid->addWidget( tw, 1, 0, 3, 4 );

    int i1, iRow = 0;
    QString tmp_str, tcas, tformula, tformat, tname, taltname;
    //QListWidgetItem *lvi;

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
	tw->setItem(iRow, 0, new QTableWidgetItem( tcas ));
	tw->setItem(iRow, 1, new QTableWidgetItem( tname ));
	tw->setItem(iRow, 2, new QTableWidgetItem( tformula ));
	tw->setItem(iRow, 3, new QTableWidgetItem( taltname ));
	tw->setItem(iRow, 4, new QTableWidgetItem( tformat ));
	iRow++;
        /*lvi = new QListWidgetItem( lv );
        lvi->setText( tcas );
        lvi->setText( tname );
        lvi->setText( tformula );
        lvi->setText( taltname );
        lvi->setText( tformat );*/
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
    int iRow = tw->currentItem()->row();
    fn = tw->item(iRow, 4)->text();
    fn.replace( "\"", "" );     // strip quotes
	qDebug() << "fn = " << fn << ", row = " << iRow;
    accept();
}

//cmake#include "netchoosedialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
