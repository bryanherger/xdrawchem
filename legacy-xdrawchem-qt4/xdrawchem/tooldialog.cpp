#include "tooldialog.h"
#include "render2d.h"
#include "helpwindow.h"
#include "defs.h"

// defined in main.cpp
extern QString RingDir;

ToolDialog::ToolDialog( QWidget * parent ):
QDialog( parent )
{
    helpfile = "spectra.html";
    this_mol = 0;
    product_mol = 0;
    /* This should be done in sub-classes
       QGridLayout *mygrid;
       mygrid = new QGridLayout(this,2,2,5);
       QPushButton *qhelp = new QPushButton( tr("Help"), this);
       mygrid->addWidget(qhelp,2,1);
       connect(qhelp, SIGNAL(clicked()), SLOT(SendHelp()));
       QPushButton *qclose = new QPushButton( tr("Close"), this);
       mygrid->addWidget(qclose,2,2);
       connect(qclose, SIGNAL(clicked()), SLOT(accept()));
     */
}

void ToolDialog::process()
{
    qDebug() << "Nothing happens";
}

void ToolDialog::setMolecule( Molecule * m1 )
{
    this_mol = m1;
    process();
}

void ToolDialog::setMolecule( OBMol * ob1 )
{
    this_mol = new Molecule( 0 );
    this_mol->convertFromOBMol( ob1 );
    process();
}

// note: must set product molecule first for correct behavior!
void ToolDialog::setProductMolecule( Molecule * m1 )
{
    product_mol = m1;
}

void ToolDialog::setProductMolecule( OBMol * ob1 )
{
    product_mol = new Molecule( 0 );
    product_mol->convertFromOBMol( ob1 );
}

void ToolDialog::SendHelp()
{
    QString home;

#ifdef UNIX
    home = RingDir + "doc/" + helpfile;
#else
    home = RingDir + "doc\\" + helpfile;
#endif
    HelpWindow *help = new HelpWindow( home, ".", 0 );

    help->setWindowTitle( QString( XDC_VERSION ) + " - Help viewer" );
    help->show();
}

//cmake#include "tooldialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
