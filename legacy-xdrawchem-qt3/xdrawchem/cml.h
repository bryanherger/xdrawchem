#ifndef CML_H
#define CML_H

#include "defs.h"
#include "dpoint.h"

// temporary data structures to hold data in CML formats
class CML_Atom {
public:
  CML_Atom() { 
    x = 987654321.0; 
    y = 123456789.0; 
    element = QString("C"); 
    atom = true;
  } 
  QString id;
  QString element;
  int formalCharge;
  double x, y, z;
  bool atom;
  DPoint *toDPoint() {
    DPoint * d = new DPoint;
    d->x = x;
    d->y = y;
    d->id = id;
    d->element = element;
    return d;
  }
};

class CML_Bond {
public:
  CML_Bond() { order = 1; type = TYPE_BOND; } 
  QString id;
  QString a1;
  QString a2;
  int type;  // for non-line types
  int order;
};

class CDXML_Node {
public:
  QString pos;
  QString text;
};

#endif
