// biotools.h -- subclass of Drawable for representing various biochemical
// structures:  membranes, antibodies, etc.

#ifndef BIOTOOLS_H
#define BIOTOOLS_H

#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class Molecule;

class BioTools : public Drawable
{
public:
  BioTools( Render2D *, QObject *parent = 0 );
  void Render();  // draw this object
  int Type();  // return type of object
  void SetWhich( QString );
  QString GetWhich() { return which; }

private:
  // Renderer
//  Render2D *r;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
