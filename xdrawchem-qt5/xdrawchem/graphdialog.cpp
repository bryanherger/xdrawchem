#include <QGridLayout>
#include <QPushButton>
#include <QPixmap>

#include "render2d.h"
#include "graphdialog.h"
#include "graphdata.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

GraphDialog::GraphDialog( QWidget *parent, const QString &name )
    : QDialog( parent )
{
    setWindowTitle( name );

    QGridLayout *dialogLayout = new QGridLayout();
    setLayout( dialogLayout );

    g = new GraphWidget();
    dialogLayout->addWidget( g, 0, 0, 1, 2 );
//    g->setGeometry( 0, 0, 600, 500 );
    //QPushButton *showhide = new QPushButton("Show/hide molecule", this);
    //showhide->setGeometry(20,320,150,40);
    //showhide->setPalette(QPalette(QPalette::Midlight));
    QPushButton *qprint = new QPushButton( tr( "Print" ), this );
    connect( qprint, SIGNAL( clicked() ), g, SLOT( Print() ) );
    dialogLayout->addWidget( qprint, 1, 0 );

    QPushButton *qexport = new QPushButton( tr( "Export Peak List" ), this );
    connect( qexport, SIGNAL( clicked() ), g, SLOT( Export() ) );
    dialogLayout->addWidget( qexport, 1, 1 );

    QPushButton *qhelp = new QPushButton( tr( "Help" ), this );
    connect( qhelp, SIGNAL( clicked() ), SLOT( SendHelp() ) );
    dialogLayout->addWidget( qhelp, 2, 0 );

    QPushButton *qclose = new QPushButton( tr( "Close" ), this );
    connect( qclose, SIGNAL( clicked() ), SLOT( accept() ) );
    dialogLayout->addWidget( qclose, 2, 1 );

    if ( name.contains( "13C-NMR" ) > 0 )
        g->setDataType( 1 );
    if ( name.contains( "IR" ) > 0 )
        g->setDataType( 2 );
    if ( name.contains( "1H-NMR" ) > 0 )
        g->setDataType( 3 );
}

void GraphDialog::AddPeak( double v1, QString l1, QString t1 )
{
    AddPeak( v1, QColor( 0, 0, 0 ), l1, t1 );
}

void GraphDialog::AddPeak( double v1, QColor c1, QString l1, QString t1 )
{
    if ( v1 < 0 )
        return;                 // negative indicates invalid peak.
    g->AddPeak( v1, c1, l1, t1 );
}

void GraphDialog::AddPeak( double v1, int m1, QColor c1, QString l1, QString t1 )
{
    g->AddPeak( v1, m1, c1, l1, t1 );
}

void GraphDialog::AddPixmap( QPixmap p1 )
{
    g->AddPixmap( p1 );
}

void GraphDialog::SendHelp()
{
    QString home, topic;

    topic = "spectra.html";
#ifdef UNIX
    home = RingDir + "doc/" + topic;
#else
    home = RingDir + "doc\\" + topic;
#endif
    HelpWindow *help = new HelpWindow( home, ".", 0 );

    help->setWindowTitle( QString( XDC_VERSION ) + " - Help viewer" );
    help->show();
}

//cmake#include "graphdialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
