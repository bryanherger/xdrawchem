// biotools.h -- subclass of Drawable for representing various biochemical
// structures:  membranes, antibodies, etc.

#ifndef BIOTOOLS_H
#define BIOTOOLS_H

#include <qobject.h>
#include <qrect.h>
#include <qfont.h>
#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class Molecule;

class BioTools : public Drawable {
  Q_OBJECT
    public:
  BioTools(Render2D *, QObject *parent = 0, const char *name = 0);
  void Render();  // draw this object
  int Type();  // return type of object
  void SetWhich(QString);
  QString GetWhich() { return which; }

 private:
  // Renderer
  Render2D *r;
};

#endif
