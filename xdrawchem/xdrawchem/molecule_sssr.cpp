// molecule_sssr.cpp - defines class SSSR, which implements ring detection by
// breadth-first traversal.  This results in the Smallest Set of Smallest
// Rings.  This algorithm is based on J. Figueras' published method,
// "Ring Perception Using Breadth-First Search",
// J. Chem. Inf. Comput. Sci. 1996, 36, 986-991

#include "molecule_sssr.h"

#include <iostream>
using std::cout;
using std::endl;

#include "bond.h"

// Is this DPoint in a ring?
int SSSR::IsInRing(DPoint *r1) {
  int retval = 0;
  if (sssr.count() == 0) {
    return 0;
  }
  for (tmp_ring = sssr.first(); tmp_ring != NULL; tmp_ring = sssr.next()) {
    if (tmp_ring->contains(r1) > 0) retval++;
  }

  return retval;
} 

// are these three points in the same ring?  If so, return the number of
// atoms in this ring
int SSSR::InSameRing(DPoint *r1, DPoint *r2, DPoint *r3) {
  int retval;

  for (tmp_ring = sssr.first(); tmp_ring != NULL; tmp_ring = sssr.next()) {
    retval = 0;
    if (tmp_ring->contains(r1) > 0) retval++;
    if (tmp_ring->contains(r2) > 0) retval++;
    if (tmp_ring->contains(r3) > 0) retval++;
    if (retval == 3) {
      return tmp_ring->count();
    }
  }

  return 0;
}

  // print SSSR as [xyz]
  void SSSR::PrintSSSR() {
    if (sssr.count() == 0) {
      cout << "No rings." << endl;
      return;
    }
    cout << "[";
    for (tmp_ring = sssr.first(); tmp_ring != NULL; tmp_ring = sssr.next())
      cout << tmp_ring->count();
    cout << "]" << endl;
  }

  // identify aromatic rings (just benzene for now)
  void SSSR::FindAromatic(QPtrList<Bond> bl) {
    Bond *tmp_bond;
    bool o1, o2, goodring;

    for (tmp_ring = sssr.first(); tmp_ring != NULL; tmp_ring = sssr.next()) {
      goodring = true;
      if (tmp_ring->count() != 6) { goodring = false; continue; }
      for (tmp_pt = tmp_ring->first(); tmp_pt != NULL; 
	   tmp_pt = tmp_ring->next()) {
	o1 = false; o2 = false;
	for (tmp_bond = bl.first(); tmp_bond != NULL; tmp_bond = bl.next()) {
	  if (tmp_bond->Find(tmp_pt) == true) {
	    if (tmp_bond->Order() == 1) o1 = true;
	    if (tmp_bond->Order() == 2) o2 = true;
	  }
	}
	if ( (o1 == false) || (o2 == false) ) { goodring = false; break; }
      }
      if (goodring == true) {
	cout << "Aromatic ring" << endl;
	for (tmp_pt = tmp_ring->first(); tmp_pt != NULL; 
	     tmp_pt = tmp_ring->next()) {
	  tmp_pt->aromatic = true;
	}
      }
    }    
  }

  // Make sssr from given 'atoms'
  void SSSR::BuildSSSR(QPtrList<DPoint> alist) {
    // clear old SSSR
    sssr.clear();
    sssr_data.clear();

    int atomsRemoved;
    structureAtoms = alist;
    QPtrListIterator<DPoint> it(structureAtoms);
    QPtrListIterator<DPoint> idel(structureAtoms);
    // eliminate pesky chains by cutting off zero- and one-bond 'atoms'
    do {
      atomsRemoved = 0;
      for ( it.toFirst() ; it.current(); ++it ) {
	tmp_pt = it.current();
	if (tmp_pt->neighbors.count() < 2) {
	  cout << tmp_pt->serial<<":"<<tmp_pt->neighbors.count() << endl;
	  // increment atomsRemoved
	  atomsRemoved++;
	  // remove this item from structureAtoms
	  structureAtoms.remove(tmp_pt);
	  // and remove this item from all neighbor lists
	  for ( idel.toFirst() ; idel.current(); ++idel ) {
	    it.current()->neighbors.remove(tmp_pt);
	  }
	}
      }        
    } while (atomsRemoved > 0);
    cout << "There are " << structureAtoms.count() << " ring atoms" << endl;
    // now traverse rings
    for ( it.toFirst() ; it.current(); ++it ) {
      tmp_ring = GetRing( it.current() );
      if (tmp_ring != NULL)
	Add(tmp_ring);
    }        
    // find fused and bridged rings
    if (sssr.count() > 1) {
      QPtrListIterator<QPtrList<DPoint> > sssr1(sssr);
      QPtrListIterator<QPtrList<DPoint> > sssr2(sssr);
      int neighbors = 0, int1;
      bool bridged = false;
      for ( ; sssr1.current(); ++sssr1 ) {
	neighbors = 0; bridged = false;
	for ( sssr2.toFirst(); sssr2.current(); ++sssr2 ) {
	  if (sssr1.current() == sssr2.current()) continue;
	  int1 = CommonPoints(sssr1.current(), sssr2.current());
	  if (int1 > 1) neighbors++;
	  if (int1 > 2) bridged = true;
	}
	cout << neighbors << "/" << bridged << endl;
      }
    }
  }

  // Add to sssr.  Make sure this is not a duplicate of an existing ring
  void SSSR::Add(QPtrList<DPoint> *r1) {
    DPoint *flagpt;
    if (CheckRing(r1) == true) {
      sssr.append(r1);
      // mark this dpoint as in a ring
      for (flagpt = r1->first(); flagpt != 0; flagpt = r1->next() ) {
	flagpt->inring = true;
	std::cout << "flagged" << std::endl;
      }
    }
  }

  // number of points in common between two rings
  int SSSR::CommonPoints(QPtrList<DPoint> *r1, QPtrList<DPoint> *r2) {
    QPtrListIterator<DPoint> i1(*r1);
    int ol = 0;
    for ( ; i1.current(); ++i1) {
      if (r2->find(i1.current()) >= 0) ol++;
    }
    return ol;
  }

  // check if ring is already in SSSR
  bool SSSR::CheckRing(QPtrList<DPoint> *r) {
    int l2;
    bool iflag = true;
    for (tmp_ring = sssr.first(); tmp_ring != NULL; tmp_ring = sssr.next() ) {
      l2 = r->count();
      for (tmp_pt = tmp_ring->first(); tmp_pt != NULL; 
	   tmp_pt = tmp_ring->next() ) {
	if (r->find(tmp_pt) >= 0) l2--;
      }
      if (l2 <= 0) { iflag = false; break; }
    }
    return iflag;
  }

  // Find smallest unique ring containing root node *root
  // Based on Figueras' breadth-first traversal
  QPtrList<DPoint> * SSSR::GetRing(DPoint *root) {
    QPtrList<DPoint> *testring = new QPtrList<DPoint>;
    DPoint *thisnode;
    int tf = 0;
    ClearPaths();
    bfs_queue.clear();
    root->source = NULL;
    bfs_queue.append(root);
    do {
      thisnode = bfs_queue.first();
      bfs_queue.removeFirst();
      // skip eliminated atoms
      if (structureAtoms.find(thisnode) < 0) continue;
      for (tmp_pt = thisnode->neighbors.first(); tmp_pt != NULL;
	   tmp_pt = thisnode->neighbors.next() ) {
	if (thisnode->source == tmp_pt) continue; // prevent backtrack
	if (structureAtoms.find(tmp_pt) < 0) continue; // skip 'eliminated'
	if (tmp_pt->path.count() == 0) {
	  tmp_pt->source = thisnode;
	  tmp_pt->path = thisnode->path;
	  tmp_pt->path.append(thisnode);
	  bfs_queue.append(tmp_pt);
	} else { // collision
	  cout << "collide" << endl << "thisnode(" << thisnode->serial <<"):";
	  // merge path lists (note overlaps)
	  QPtrListIterator<DPoint> i(thisnode->path);
	  QPtrListIterator<DPoint> j(tmp_pt->path);
	  for ( ; i.current(); ++i) {
	    cout << i.current()->serial;
	    testring->append(i.current());
	  }
	  cout << endl << "tmp_pt(" << tmp_pt->serial << "):";
	  tf = 0;
	  for ( ; j.current(); ++j) {
	    cout << j.current()->serial;
	    if (testring->find(j.current()) < 0)
	      testring->append(j.current());
	    else
	      tf++;
	  }
	  cout << endl;
	  // but thisnode and tmp_pt have not themselves been added!
	  if (testring->find(thisnode) < 0)
	    testring->append(thisnode);
	  else
	    tf++;
	  if (testring->find(tmp_pt) < 0)
	    testring->append(tmp_pt);
	  else
	    tf++;
	  // check for singleton overlap (tf == 1)
	  if (tf == 1)
	    return testring;
	  else
	    testring->clear();
	}
      }
    } while (bfs_queue.count() > 0);
    return testring;  // to appease g++ -Wall.  Probably returns before here.
  }

  // clear paths in 'atoms'
  void SSSR::ClearPaths(void) {
    for (tmp_pt = structureAtoms.first(); tmp_pt != NULL;
	 tmp_pt = structureAtoms.next() ) {
      tmp_pt->source = NULL;
      tmp_pt->path.clear();
    }
  }




