#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <qptrlist.h>

#include "drawable.h"

class Clipboard {
 public:
  Clipboard() { }
  QPtrList<Drawable> objects;
  void clear() { objects.clear(); }
};

#endif

