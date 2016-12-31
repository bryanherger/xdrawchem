// atom.h -- Atom class comparable to that in CompChem package.

// (References...)

#ifndef ATOM_H
#define ATOM_H

#include <qmemarray.h>
#include <qptrvector.h>
#include <qstring.h>

class Atom {
 public:
  QString name;
  QString symbol;
  int number;
  int charge;

  /** list of adjacent nodes */
  QPtrVector<Atom> nodeTable;
  /** list of the bondorders of the bonds listed in nodeTable */
  QMemArray<int> bondTable;
  /** list of numbers of adjacent node in an array
   *	This is kept for historic reasons and should not be used anymore 
   */
  QMemArray<int> intnodeTable;

  /** degree of this node, i.e. the number of attached nodes */
  int degree;
  /** Possiblity to indicate the number of implicit hydrogen atoms 
      attached to this node. This is used to check is the sum of bond orders
      of all bonds of this node to other heavy atoms plus the sum of implicit
      hydrogens euqals maxBondOrderSum */
  int HCount;
  /** Maximum allowed sum of bond orders of all bonds of this node. 
      This should for example be set to 4 (four) for carbon atoms */
  int maxBondOrderSum;
  /** Maximum allow BondOrder for this node. Would e.g. be 2 for oxygens */
  int maxBondOrder;
	
  /** current sum of bondorders for this node.  */
  int bondOrderSum;
 
  /** to store the numbers of the rings of which this node is part of.
      BTW, what is the maximum number of Rings that an atom with, say, 
      octahedral configuration can be part of? Ten is certainly too much. */ 
  QMemArray<int> inRings;

  /** x coordinate of this node */
  double x; 
  /** y coordinate of this node */    
  double y;
  /** z coordinate of this node */   
  double z;

  int ringCounter;
	
  int status; // to hold one of the three stati below
    
  /** Constructs an empty Atom */	
  Atom() {
    //bondOrderSum = getBondCount();
    //physicalProperties = new Hashtable();
    init();
  }
	
  /** Constructs a Atom with a given element symbol*/	
  Atom(QString s){
    symbol = s;		
    init();
  }
	
  /** Constructs a Atom with a given element Symbol and node number */
  Atom(QString s, int n){
    symbol = s;
    number = n;		
    init();
  }
  
  /** Constructs a Atom with a given element Symbol and node number */
  Atom(QString s, double x1, double y1, double z1){
    symbol = s;
    x = x1;        
    y = y1;
    z = z1;
    init();
  }
  
  /** Constructs a Atom with a given node number */

  Atom(int n){
    number = n;	
    init();
  }

  void init() {
    nodeTable.resize(10);
    bondTable.resize(10);
    intnodeTable.resize(10);
    inRings.resize(10);
  }

  Atom *clone() {
    Atom *n = new Atom;
    n->name = name;
    n->symbol = symbol;
    n->number = number;
    n->charge = charge;
    n->status = status;
    n->degree = degree;
    n->x = x;
    n->y = y;
    n->z = z;
    n->ringCounter = ringCounter;
    n->HCount = HCount;
    n->maxBondOrderSum = maxBondOrderSum;
    n->maxBondOrder = maxBondOrder;
    n->bondOrderSum = bondOrderSum;
    //System->arraycopy(nodeTable, 0, n->nodeTable, 0, 10);
    for (int f = 0; f < 10; f++)
      n->nodeTable.insert(f, nodeTable[f]);  // shallow copy!
    // the rest are deep copies.
    n->intnodeTable = intnodeTable.copy();
    n->bondTable = bondTable.copy();
    n->inRings = inRings.copy();
    return n;
  }
};

#endif
