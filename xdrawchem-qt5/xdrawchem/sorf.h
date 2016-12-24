/* SetOfRingsFinder.java
 *
 * $RCSfile: sorf.h,v $    $Author: bherger $    $Date: 2005/05/18 01:06:37 $    $Revision: 1.2 $
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

#ifndef SORF_H
#define SORF_H

#include <QQueue>

#include "atom.h"
#include "setofrings.h"

class DataStructureTools {
 public:
  static QVector<Atom *> cloneAtomSet(QVector<Atom *> thisSet)
    {
      QVector<Atom *> newAtomSet(thisSet.size());
      int f;
      for (f = 0; f < thisSet.size(); f++)
	{
	  newAtomSet.replace(f, thisSet[f]->clone());
	}
      for (f = 0; f < thisSet.size(); f++)
	{
	  for (int g = 0; g < thisSet[f]->degree; g++)
	    {
	      for (int h = 0; h < thisSet.size(); h++)
		{
		  if (thisSet[h]->number == thisSet[f]->nodeTable[g]->number)
		    {
		      newAtomSet[f]->nodeTable.replace(g, newAtomSet[h]);
		      break;
		    }
		}
	    }
	}
      return newAtomSet;
    }
};

class SetOfRingsFinder
{
 public:
  SetOfRingsFinder() {
    debug = false;
    debug1 = false;
    debug2 = false;
    debug3 = false;
    ringCounter = 0;
    storeRings = true;
  }

  bool debug; // minimum details
  bool debug1;  // more details
  bool debug2; // too many details
  bool debug3; // hillarious details

  int ringCounter;
  bool storeRings;

  QVector<Atom *> FGsource;

  /** Finds the Smallest Set of Smallest Rings.
      This is an implementation of the algorithm published in
      John Figueras, "Ring Perception Using Breadth-First Search",
      J. Chem. Inf. Comput Sci. 1996, 36, 986-991.
  */
  setOfRings findSSSR(QVector<Atom *> atSet){
    ringCounter = 0;
    setOfRings sssr;
    QVector<Atom *> tempAtomSet = DataStructureTools::cloneAtomSet(atSet);
    int f;
    for (f = 0; f < tempAtomSet.size(); f++)
      {
	tempAtomSet[f]->number = f;
      }
    qInfo() << "tempAtomSet.size() = f = " << f;
    FGsource.resize(tempAtomSet.size());
    int smallestDegree;
    Atom *smallest;
    Ring *ring;
    QVector<Atom *> edgeToRemove;
    QVector<Atom *> remembernodes;
    int nodesToBreakCounter;
    /** Two Vectors - as defined in the article. One to hold the
	full set of atoms in the structure and on to store the numbers
	of the nodes that have been trimmed away.
	Furhter there is a Vector nodesN2 to store the number of N2 nodes
    */
    QList<Atom *> fullSet;
    QList<Atom *> trimSet;
    QList<Atom *> nodesN2;
    // load fullSet with the numbers of our atoms
    for (f = 0; f < tempAtomSet.size(); f++){
      fullSet.append(tempAtomSet[f]);
    }
    do{
      /* Add nodes of degree zero to trimset.
	 Also add nodes of degree 2 to nodesN2.
	 In the same run, check, which node has the lowest degree
	 greater than zero.	*/

      smallestDegree = 7;
      smallest = 0;
      nodesN2.clear();
      int f;
      for (f = 0; f < tempAtomSet.size(); f++){
	if (tempAtomSet[f]->degree == 0){
	  if (!trimSet.contains(tempAtomSet[f])){
	    trimSet.append(tempAtomSet[f]);
	  }
	}
	if (tempAtomSet[f]->degree == 2){
	  if (!trimSet.contains(tempAtomSet[f])){
	    nodesN2.append(tempAtomSet[f]);
	  }
	}
	if (tempAtomSet[f]->degree < smallestDegree && tempAtomSet[f]->degree > 0){
	  smallest = tempAtomSet[f];
	  smallestDegree = tempAtomSet[f]->degree;
	}
      }

      if (smallest == 0)	break;

			// If there are nodes of degree 1, trim them away
      if (smallest->degree == 1){
	trim(smallest, tempAtomSet);
	trimSet.append(smallest);
      }
      // if there are nodes of degree 2, find out of which rings
      // they are part of.
      else if (smallest->degree == 2){
	remembernodes.clear();
	remembernodes.resize(nodesN2.count());
	nodesToBreakCounter = 0;
	f = 0;
	for (f = 0; f < nodesN2.count(); f++){
	  ring = findSRing(nodesN2.at(f), tempAtomSet);
	  if (ring->size() > 0){
	    // check, if this ring already is in SSSR
	    qInfo() << "d2:findSRing of size: " << ring->size();
	    if (!sssr.ringAlreadyInSet(ring)){
	      sssr.addElement(ring);
	      remembernodes.replace(nodesToBreakCounter, nodesN2.at(f));
	      nodesToBreakCounter ++;
	    }
    qInfo() << "SSSRd2:" << sssr.size() ;
	  }
	}
	if (nodesToBreakCounter == 0){
	  nodesToBreakCounter = 1;
	  remembernodes.replace(0, (nodesN2.at(0)));
	}
	for (f = 0; f < nodesToBreakCounter; f++){
	  breakBond(remembernodes[f], tempAtomSet);
	}
      }
      else if (smallest->degree == 3){
	ring = findSRing(smallest, tempAtomSet);
	qInfo() << "d3:findSRing of size: " << ring->size();
	if (ring->size() > 0){
	  // check, if this ring already is in SSSR
	  if (!sssr.ringAlreadyInSet(ring)){
	    sssr.addElement(ring);
	  }
    qInfo() << "SSSRd3:" << sssr.size() ;
	}
	edgeToRemove = checkEdges(ring, tempAtomSet);
	breakBond(edgeToRemove[0], edgeToRemove[1], tempAtomSet);
      }
    }while(trimSet.count() < fullSet.count());
    ringCounter = sssr.size();
    qInfo() << "SSSR:" << sssr.size() ;
    for (int h = 0; h < sssr.size(); h++)
      qInfo() << h << ":" << sssr[h]->size() ;
    qInfo() << "SSSR ends" ;
    return sssr;
  }

  QString printPath(QList<Atom *> atoms) {
	QString thisPath;
        for (int i = 0; i < atoms.size(); i++) {
		if (i > 0) thisPath.append(" ");
		thisPath.append( QString::number(atoms[i]->number) );
	}
	return thisPath;
  }
  /** finds the smallest ring of which rootNode is part of.
	    This routine is called 'getRing() in Figueras original article */
  Ring *findSRing(Atom *rootNode, QVector<Atom *> tempAtomSet){

    //qInfo("Attempt to find Ring for " + rootNode->number);
    Atom *node, *neighbor;
    Atom *source, *m, *frontNode;
    /** OKatoms is Figueras nomenclature, giving the number of
	atoms in the structure */
    int OKatoms = tempAtomSet.size();
    /** queue for Breadth First Search of this graph */
    QQueue<Atom *> queue;
    /** ringsset stores the smallest ring found and returns it */
//    Ring *ringset;
    /* Initialize a path Vector (QPtrList) for each node
     */
    QVector<QList<Atom *> *> path(OKatoms);
    QList<Atom *> intersection;
    QList<Atom *> ring;

    // I think Qt does this for us; we're not using Vectors anyway.
    int f;
    for (f = 0; f < OKatoms; f++){
      path.replace(f, new QList<Atom *>);
    }
    // Initialize the queue with nodes attached to rootNode
    for (f = 0; f < rootNode->degree; f++){
      /* if the degree of the f-st neighbor of rootNode is greater
	 than zero (i.e., it has not yet been deleted from the list)
      */
      neighbor = rootNode->nodeTable[f];
      if (neighbor->degree > 0){
				// push the f-st node onto our FIFO queue
				// after assigning rootNode as its source
	FGsource.replace(neighbor->number, rootNode);
	queue.enqueue(neighbor);
	path[neighbor->number]->append(rootNode->nodeTable[f]);
	path[neighbor->number]->append(rootNode);
      }
    }

    while (true && queue.count() > 0){
      node = queue.dequeue();
      frontNode = node;
      source = FGsource[node->number];
      int f;
      for (f = 0; f < node->degree; f++){
	m = node->nodeTable[f];
	if (m->degree > 0 && m != source){
	  if (path[m->number]->count() > 0){
	    intersection = getIntersection(*path[frontNode->number], *path[m->number]);
	    if (intersection.count() == 1 ){
	      // we have found a valid ring closure
	      // now let's prepare the path to
	      // return in tempAtomSet
	      qInfo() << "Ring closure found: " << m->number << ", source is " << source->number;
	      qInfo() << m << ", " << source;
	      qInfo() << "Path of frontnode: " << printPath(*path[frontNode->number]);
	      qInfo() << "Path of m: " << printPath(*path[m->number]);
	      ring = getUnion(*path[frontNode->number], *path[m->number]);
	      return prepareRing(ring, tempAtomSet);
	    }
	  }
	  else { // if path[m] is not null
	    // update the path[m]
	    path.replace(m->number, merge(path[m->number], path[frontNode->number]));
	    path[m->number]->insert(0, m); // no, we really mean insert here; well, "prepend" really, but this is what the old code did, exactly.
	    // now push the node m onto the queue
	    FGsource.replace(m->number, frontNode);
	    queue.enqueue(m);
	  }
	}
      }
    }
    return new Ring;
  }

  /** Return a Vector that contains the intersection of Vectors vec1
	    and vec2 */

  QList<Atom *> getIntersection(QList<Atom *> vec1, QList<Atom *> vec2){
    QList<Atom *> is;
    for (int f = 0; f < vec1.count(); f++){
      if (vec2.contains(vec1.at(f))) is.append(vec1.at(f));
    }
    return is;
  }

  /** Return a Vector that contains the union of Vectors vec1
	    and vec2 */

  QList<Atom *> getUnion(QList<Atom *> vec1, QList<Atom *> vec2){
    QList<Atom *> is = vec1;
    for (int f = vec2.count()- 1; f > -1; f--){
      if (!vec1.contains(vec2.at(f))) is.append(vec2.at(f));
    }
    return is;
  }


  /** merges two vectors into one */

  QList<Atom *> *merge(QList<Atom *> *vec1, QList<Atom *> *vec2){
    QList<Atom *> *result = new QList<Atom *>;
    *result = *vec1 + *vec2;
    return result;
  }
  /** prepares a Ring, i.e. a Vector with Nodes from
	     a Vector with numbers of nodes */

  Ring *prepareRing(QList<Atom *> vec, QVector<Atom *> tempAtomSet)	{
    Ring *r = new Ring(vec.count());
    for (int f = 0; f < vec.count(); f++){
      r->replace(f, vec.at(f));
    }
    r->sort();
    return r;
  }

  /** removes from the connection table all connections to
	    node n, leaving this node with degree zero.*/
  void trim(Atom *n, QVector<Atom *> tempAtomSet){
    Atom *conn;
    for (int f = 0; f < n->degree; f++){
      conn = n->nodeTable[f];
      for (int g = 0; g < conn->degree; g++){
	if (conn->nodeTable[g] == n){
	  if (g < (conn->nodeTable.size() - 1)){
	    for (int h = g; h < conn->degree - 1; h++){
	      conn->nodeTable.replace(h, conn->nodeTable[h + 1]);
	    }
	  }
	  conn->nodeTable.replace(conn->degree - 1, 0);
	  conn->degree --;
	}
      }
      n->nodeTable.replace(f, 0);
    }
    n->degree = 0;
    // you are erased! Har, har, har.....  >8-)=)
  }

  /** Eliminate the last bond of this node from the connectiontable*/
  void breakBond(Atom *thisNode, QVector<Atom *> tempAtomSet){
    int degree = thisNode->degree;
    if (degree == 0) {
      qInfo() << "breakBond: invalid node" ;
      return;
    }
    // remember the bond partner of the last bond
    Atom *partner = thisNode->nodeTable[degree - 1];
    // now delete the bond
    thisNode->nodeTable.replace(degree - 1, 0);
    thisNode->degree --;
    // find the same bond for 'partner' and delete it
    for (int f = 0; f < partner->degree; f++){
      if (partner->nodeTable[f] == thisNode){
	partner->nodeTable.replace(f, 0);
	for (int g = f; g < partner->degree - 1; g++){
	  partner->nodeTable.replace(g, partner->nodeTable[g+1]);
	}
	partner->nodeTable.replace(partner->degree - 1, 0);
	partner->degree --;
	break;
      }
    }
  }

  /** Eliminate the bond between these two atom */
  void breakBond(Atom *from, Atom *to,  QVector<Atom *> tempAtomSet){
    int degree, f;
    for (f = 0; f < from->degree; f++){
      if (from->nodeTable[f] == to){
	degree = from->degree;
	for (int g = f; g < from->degree - 1; g++){
	  from->nodeTable.replace(g, from->nodeTable[g+1]);
	}
	from->nodeTable.replace(from->degree - 1, 0);
	from->degree --;
	break;
      }
    }
    for (f = 0; f < to->degree; f++){
      if (to->nodeTable[f] == from){
	for (int g = f; g < to->degree - 1; g++){
	  to->nodeTable.replace(g, to->nodeTable[g+1]);
	}
	to->nodeTable.replace(to->degree - 1, 0);
	to->degree --;
	break;
      }
    }
  }

  /** Restores a bond betwenn nodes 'from' and 'to' of an array of
	    nodes 'tempAtomSet' */
  void restoreBond(Atom *from, Atom *to,  QVector<Atom *> tempAtomSet){
    from->nodeTable.replace(from->degree, to);
    from->degree ++;
    to->nodeTable.replace(to->degree, from);
    to->degree ++;
  }

  /** Selects an optimum edge for elimination in structures without N2 nodes. */
  QVector<Atom *> checkEdges(Ring *ring, QVector<Atom *> tempAtomSet){
    QVector<Atom *> edge;
    Ring *r1, *r2;
    setOfRings tempSoR;
    int minMax, minMaxSize;
    //		Atom[] tempAtomSet = new Atom[tAS.length];
    //		for (int f = 0; f < tAS.length; f++){
    //			tempAtomSet[f] = (Atom)tAS[f].clone();
    //		}
    // Ok, we now have set of nodes in hand that can be modified.
    // Now, we in turn remove the edges of our ring r
    int f;
    for (f = 0; f < ring->size(); f++){
      edge = ring->getEdge(f);
      breakBond(edge[0], edge[1], tempAtomSet);
      r1 = findSRing(edge[0], tempAtomSet);
      r2 = findSRing(edge[1], tempAtomSet);
      if(r1->size() > r2->size())
	tempSoR.addElement(r1);
      else
	tempSoR.addElement(r1);
      restoreBond(edge[0], edge[1], tempAtomSet);

    }
    minMaxSize = (tempSoR[0])->size();
    minMax = 0;
    for (f = 0; f < tempSoR.size(); f++){
      if ((tempSoR[f])->size() < minMaxSize){
	minMaxSize = (tempSoR[f])->size();
	minMax = f;
      }
    }
    return ring->getEdge(minMax);

  }

};

#endif



