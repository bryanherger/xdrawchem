/* setOfRings.java
 *
 * $RCSfile$    $Author: bherger $    $Date: 2005-05-17 21:06:37 -0400 (Tue, 17 May 2005) $    $Revision: 148 $
 * 
 * Copyright (C) 1997-2000  The JChemPaint project
 * 
 * Contact: steinbeck@ice.mpg.de
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * All I ask is that proper credit is given for my work, which includes
 * - but is not limited to - adding the above copyright notice to the beginning
 * of your source code files, and to any copyright notice that you may distribute
 * with programs based on this work.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA. 
 * 
 */

#ifndef SOR_H
#define SOR_H

/** Implementation of a set of Rings.
    Maintains a Vector "rings" to store "ring" objects
    */

#include <qptrvector.h>
#include <qstring.h>
#include <iostream>

using std::cout;
using std::endl;

#include "atom.h"
#include "ring.h"

class setOfRings : public QPtrVector<Ring>
{
 public:
  setOfRings(){
  }

  void addElement(Ring *new1) {
    cout << "setOfRings::addElement:" << new1->getSortedString() << endl;
    if (new1->isValid() == false) return;
    new1->serial = size();
    resize(size() + 1);
    insert(size() - 1, new1);
  }

  /** Checks - and returns 'true' - if a certain ring is 
      already stored in this setOfRings */
  bool ringAlreadyInSet(Ring *r){
    Ring *ring;
    QString s1, s2;
    s1 = r->getSortedString();
				
    //		System.out.println(this.size());
    for (int f = 0; f < size(); f++){
      ring = at(f);
			
      s2 = ring->getSortedString();
      //cout << s1 << "-vs-" << s2 << endl;
      if (s1 == s2){
	return true;
      }
    }			
    return false;
  }

  /** Uses the private quicksort method of this class to sort the rings in the
      set by size */
  void sort(){
    int left = 0;
    int right = size() - 1;
    quickSort(left, right);
  }		

  /** Sorts the rings by size 
      Quick Sort implementation
  */
  void quickSort(int left, int right){
    int leftIndex = left;
    int rightIndex = right;
    float partionElement;
    if ( right > left){		
      /* Arbitrarily establishing partition element as the midpoint of
       * the array.
       */
      partionElement = (at( ( left + right ) / 2 ))->size();
      // loop through the array until indices cross
      while( leftIndex <= rightIndex ){
	/* find the first element that is greater than or equal to
	 * the partionElement starting from the leftIndex.
	 */
	while( ( leftIndex < right ) && ((at(leftIndex))->size() < partionElement))
	  ++leftIndex;
	/* find an element that is smaller than or equal to
	 * the partionElement starting from the rightIndex.
	 */
	while( ( rightIndex > left ) && ((at(rightIndex))->size() > partionElement ) )
	  --rightIndex;
	// if the indexes have not crossed, swap
	if( leftIndex <= rightIndex ){
	  swap(leftIndex, rightIndex);
	  ++leftIndex;
	  --rightIndex;
	}
      }
      /* If the right index has not reached the left side of array
       * must now sort the left partition.
       */
      if( left < rightIndex )
	quickSort(left, rightIndex );
      /* If the left index has not reached the right side of array
       * must now sort the right partition.
       */
      if( leftIndex < right )
	quickSort(leftIndex, right );
    }
  }
	

  /** swap for quicksort */
  void swap(int i, int j){
    Ring *r; 
    r = at(i);
    insert(i, at(j));
    insert(j, r);
  }
	
  /** Lists the rings in this set to the console */
  void reportRingList(){
    for (int f = 0; f < size(); f++){
      //System.out.println(((Ring)elementAt(f)).getString());	
    }
  }
	
  /** For each node in each ring of the list, make a list of rings 
      that it is part of. */
		
  void makeNodeRingLists(QPtrVector<Atom> thisAtomSet){
    Ring *ring = 0;
    int nodeNo = -1;
    for (int f = 0; f < size(); f++){
      ring = at(f);	
      for (int g = 0; g < ring->size(); g++){
	nodeNo = (ring->at(g))->number;
	thisAtomSet[nodeNo]->inRings[thisAtomSet[nodeNo]->ringCounter] = f;
	thisAtomSet[nodeNo]->ringCounter++;			
      }
    }	
  }	
};

#endif
