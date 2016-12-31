#include "application.h"
#include "biotools.h"
#include "defs.h"

QToolBar * ApplicationWindow::BuildBioTools() {
  QToolBar *localbar = new QToolBar( this, "file operations" );

  return localbar;
}

BioTools::BioTools(Render2D *r1, QObject *parent, const char *name)
  : Drawable(parent, name)
{
  r = r1;
  which = "";
}

int BioTools::Type() {
  return TYPE_BIOTOOLS;
}

void BioTools::SetWhich(QString w1) {
  which = w1;
}

void BioTools::Render() {

}
