#include "application.h"
#include "biotools.h"
#include "defs.h"

QToolBar *ApplicationWindow::BuildBioTools()
{
    QToolBar *localbar = new QToolBar( this );

    return localbar;
}

BioTools::BioTools( Render2D *, QObject * parent )
    : Drawable( parent )
{
    which = "";
}

int BioTools::Type()
{
    return TYPE_BIOTOOLS;
}

void BioTools::SetWhich( QString w1 )
{
    which = w1;
}

void BioTools::Render()
{
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
