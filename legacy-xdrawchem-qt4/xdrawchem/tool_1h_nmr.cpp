#include <QGridLayout>

#include "tool_1h_nmr.h"
#include "render2d.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
//extern QString RingDir;

Tool_1HNMR_Dialog::Tool_1HNMR_Dialog( QWidget *parent )
    : ToolDialog( parent )
{
    setWindowTitle( tr( "1H NMR spectrum" ) );
    helpfile = "spectra.html";

    QGridLayout *mygrid = new QGridLayout();
    setLayout( mygrid );

    ngw = new NewGraphWidget( this );
    ngw->setDataType( DATA_1HNMR );
    mygrid->addWidget( ngw, 0, 0, 1, 3 );

    peaktext = new QTextEdit();
    peaktext->setReadOnly( true );
    mygrid->addWidget( peaktext, 1, 0 );

    QPushButton *qhelp = new QPushButton( tr( "Help" ) );
    connect( qhelp, SIGNAL( clicked() ), SLOT( SendHelp() ) );
    mygrid->addWidget( qhelp, 1, 1 );

    QPushButton *qclose = new QPushButton( tr( "Close" ) );
    connect( qclose, SIGNAL( clicked() ), SLOT( accept() ) );
    mygrid->addWidget( qclose, 1, 2 );
}

void Tool_1HNMR_Dialog::process()
{
    this_mol->Calc1HNMR( false );
    peaklist = this_mol->peaklist;
    qDebug() << "got peak list";
    if ( product_mol == 0 ) {
        foreach ( tmp_peak, peaklist ) {
            ngw->AddPeak( tmp_peak, QColor( 0, 0, 0 ) );
            peaktext->append( tmp_peak->comment );
            //peaktext->append("\n");
        }
        return;
    }
    peaktext->setTextColor( QColor( 255, 0, 0 ) );
    peaktext->insertPlainText( tr( "Peaks of reactant:\n" ) );
    foreach ( tmp_peak, peaklist ) {
        ngw->AddPeak( tmp_peak, QColor( 255, 0, 0 ) );
        peaktext->insertPlainText( tmp_peak->comment );
        peaktext->insertPlainText( "\n" );
    }
    product_mol->Calc1HNMR( false );
    peaklist.clear();
    peaklist = product_mol->peaklist;
    peaktext->setTextColor( QColor( 0, 0, 255 ) );
    peaktext->insertPlainText( tr( "Peaks of product:\n" ) );
    foreach ( tmp_peak, peaklist ) {
        ngw->AddPeak( tmp_peak, QColor( 0, 0, 255 ) );
        peaktext->insertPlainText( tmp_peak->comment );
        peaktext->insertPlainText( "\n" );
    }
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
