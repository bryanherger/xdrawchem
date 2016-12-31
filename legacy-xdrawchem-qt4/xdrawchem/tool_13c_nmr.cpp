#include <QGridLayout>

#include "tool_13c_nmr.h"
#include "render2d.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
//extern QString RingDir;

Tool_13CNMR_Dialog::Tool_13CNMR_Dialog( QWidget * parent ):
ToolDialog( parent )
{
    setWindowTitle( tr( "13C NMR spectrum" ) );
    helpfile = "spectra.html";
    QGridLayout *mygrid;

    mygrid = new QGridLayout( this );
    ngw = new NewGraphWidget( this );
    ngw->setDataType( DATA_13CNMR );
    mygrid->addWidget( ngw, 0, 0, 0, 1 );
    peaktext = new QTextEdit();
    peaktext->setReadOnly( true );
    mygrid->addWidget( peaktext, 1, 1, 0, 1 );
    QPushButton *qhelp = new QPushButton( tr( "Help" ) );

    mygrid->addWidget( qhelp, 2, 0 );
    connect( qhelp, SIGNAL( clicked() ), SLOT( SendHelp() ) );
    QPushButton *qclose = new QPushButton( tr( "Close" ) );

    mygrid->addWidget( qclose, 2, 1 );
    connect( qclose, SIGNAL( clicked() ), SLOT( accept() ) );
}

void Tool_13CNMR_Dialog::process()
{
    QString n1;

    this_mol->Calc13CNMR( false );
    peaklist = this_mol->peaklist;
    qDebug() << "got peak list";
    if ( product_mol == 0 ) {
        foreach ( tmp_peak, peaklist ) {
            ngw->AddPeak( tmp_peak, QColor( 0, 0, 0 ) );
            n1.setNum( tmp_peak->intensity );
            peaktext->append( tmp_peak->comment + tr( ", intensity " ) + n1 );
            //peaktext->append("\n");
        }
        return;
    }
    peaktext->setTextColor( QColor( 255, 0, 0 ) );
    peaktext->insertPlainText( tr( "Peaks of reactant:\n" ) );
    foreach ( tmp_peak, peaklist ) {
        ngw->AddPeak( tmp_peak, QColor( 255, 0, 0 ) );
        n1.setNum( tmp_peak->intensity );
        peaktext->insertPlainText( tmp_peak->comment + tr( ", intensity " ) + n1 );
        peaktext->insertPlainText( "\n" );
    }
    product_mol->Calc13CNMR( false );
    peaklist = product_mol->peaklist;
    peaktext->setTextColor( QColor( 0, 0, 255 ) );
    peaktext->insertPlainText( tr( "Peaks of product:\n" ) );
    foreach ( tmp_peak, peaklist ) {
        ngw->AddPeak( tmp_peak, QColor( 0, 0, 255 ) );
        n1.setNum( tmp_peak->intensity );
        peaktext->insertPlainText( tmp_peak->comment + tr( ", intensity " ) + n1 );
        peaktext->insertPlainText( "\n" );
    }
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
