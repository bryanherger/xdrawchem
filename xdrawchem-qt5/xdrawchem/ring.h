#ifndef RING_H
#define RING_H

/* Ring.java
 *
 * $RCSfile: ring.h,v $    $Author: bherger $    $Date: 2005/05/18 01:06:37 $    $Revision: 1.2 $
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

/** Ring.java
	A collection of nodes that make up a ring
 */

#include <QtGlobal>
#include <QVector>

#include "atom.h"

class Ring : public QVector<Atom *>
{
  /** Vector to store the nodes of this ring.
      Each node is bonded to its predecessor.
      The ring is close by connecting the first and
      the last node */
 public:
  int status; // to hold one of the three stati below
  int serial; // ring number
  double x; // x coordinate of ring center;
  double y; // y coordinate of ring center;
  /* Used for temporarily marking a ring as aromatic. Nothing to rely on. If you want to know
     if a ring is aromatic, use DatastructureTools.isAromatic(), if you trust it :-)*/
  bool aromatic;

  /** Creates a new Ring with no nodes stored */
  Ring(){
    status = 0;
    aromatic = false;
  }

  Ring(int x):QVector<Atom *>(x){
    status = 0;
    aromatic = false;
  }

  /** Returns a String with a sorted list of nodes numbers */
  QString getSortedString(){
    QString s = "", n1;
    QVector<int> rep(size());
    int f;
    for (f = 0; f < size(); f++){
      rep[f] = (at(f))->number;
    }
    qSort(rep.begin(), rep.end());
    //quickSort(rep, 0, rep.length - 1);
    for (f = 0; f < size()-1; f++){
      n1.setNum(rep[f]);
      s += n1 + "-";
    }
    n1.setNum(rep.at(size()-1));
    s += n1;
    return s;
  }

  bool isValid() {
    QVector<int> rep(size());
    for (int f = 0; f < size(); f++){
      rep[f] = (at(f))->number;
    }
    qSort(rep.begin(), rep.end());
    for (int g = 0; g < size() - 1; g++) {
      if (rep[g] == rep[g+1]) return false;
    }
    return true;
  }

  /** Returns a String representation of this ring, i.e.
      just a list of node numbers that make up the ring.
      Example: 2-7-3-9-14
  public String getString(){
    String s = "";
    int[] rep = new int[size()];
    for (int f = 0; f < size(); f++){
      rep[f] = ((Atom)at(f))->number;
    }
    for (int f = 0; f < size()-1; f++){
      s += rep[f] + "-";
    }
    s += rep[size()-1];
    return s;
    }*/

  bool isInRing(int number)
    {
      for (int f = 0; f < size(); f++){
	if ((at(f))->number == number) return true;
      }
      return false;
    }

  /** Sorts the atoms of this ring such that the two
      atoms with the lowest indices are at the beginning
      of the list. Of course the order is preserved.
  */
  void sort(){
    // choose the number of an arbitrary atom
    int lowestIndex = (at(0))->number;
    int lowest = 0;
    int sz = size();
    for (int f = 1; f < sz; f++){
      if ((at(f))->number < lowestIndex){
	lowestIndex = (at(f))->number;
	lowest = f;
      }
    }
    if (lowest == sz -1){
      shiftLeft(1);
      lowest --;
    }
    else if (lowest == 0){
      shiftRight(1);
      lowest ++;
    }
    /* if the number of the (lowest + 1)-st is smaller than the one of
       the (lowest - 1)-st then the order is correct and we just have to
       left-shift lowest times.
    */
    if ((at(lowest + 1))->number < (at(lowest - 1))->number){
      shiftLeft(lowest);
    }
    /* else if the number of the (lowest + 1)-st is greater than the one of
       the (lowest - 1)-st then we have to shift to the right (size()-1-lowest)
       times until the node with the smallest order is at the end of the list
       and then invert the list
    */
    else{
      shiftRight(size()-1-lowest);
      invert();
    }
  }

  /** Sorts the ring so that node with 'nodeNumber' is first */
  void sort(int nodeNumber){
    qInfo() << nodeNumber << " in " << getSortedString();
    int actNumber = (at(0))->number;
    while(actNumber != nodeNumber){
      shiftLeft(1);
      actNumber = (at(0))->number;
    }
  }

  // bubble sort
  void sort2() {
    Atom *tmp;
    for (int f = 1; f < size(); f++) {
      for (int g = 0; g < size()-1; g++) {
	if ((at(g))->number < (at(f))->number) {
	  tmp = at(f);
	  replace(f, at(g));
	  replace(g, tmp);
	}
      }
    }
    for (int h = 0; h < size(); h++) qInfo() << at(h)->number ;
  }

  /** shifts the node list of this ring to the left */
  void shiftLeft(int times){
    for (int f = 0; f < times; f++){
      Atom *n = at(0);
      for (int g = 0; g < size() - 1; g++)
	replace(g, at(g+1));
      replace(size() - 1, n);
    }
  }

  /** shifts the node list of this ring to the right */
  void shiftRight(int times){
    for (int f = 0; f < times; f++){
      Atom *n = at(size()-1);
      for (int g = size()-1; g > 0; g--) {
	replace(g, at(g-1));
      }
      replace(0, n);
    }
  }

  /** Inverts the order of Nodes in this ring */
  void invert(){
    Atom *t;
    int oldsize = size() / 2;
    for(int f = 0; f < oldsize; f++){
      t = at(f);
      replace(f, at(size() - 1 -f));
      replace(size() - 1 - f, t);
    }
    //resize(oldsize);
  }

  /** returns the sum of the ringCounter values
      of all the nodes in this ring */
  int getComplexity(QVector<Atom *> atomSet){
    int cmpl = 0;
    for (int f = 0; f < size(); f++){
      cmpl += atomSet[(at(f))->number]->ringCounter;
    }
    return cmpl;
  }

  /** Checks if all Nodes of this ring have not yet been finally positioned
      and returns 'true' if so. */
  bool notAllPositioned(QVector<Atom *> atomSet){
//    int test;
    for (int f = 0; f < size(); f++){
      //test = (at(f))->number;
      if (atomSet[(at(f))->number]->status < 1) {
	return true;
      }
    }
    return false;
  }

  /** returns a vector listing the numbers of the nodes
      involved in this ring */
  QVector<int> getNodeNumberList(){
    QVector<int> arr(1);
    int sz = 0;
    for (int f = 0; f < size(); f++){
      arr.resize(sz+1);
      arr[sz] = (at(f))->number;
      sz++;
    }
    return arr;
  }

  /** Returns the radius of this  ring assuming that is has been
      drawn as a regular polygon. The radius is defined as
      the distance between the ring center and one of the nodes. */
  double getRadius(double bondLength){
    double radius = sqrt(pow(bondLength, (double)2)/(2 - (2 * cos(getSectorAngle()))));
    return radius;
  }

  /** Returns the angle that is made of by a 'node', the
      ring center and a direct neighbor of 'node'. */
  double getSectorAngle(){
    double gamma = 360/size();
    gamma = 3.14159246/180*gamma;
    return gamma;
  }

  /** Returns the number (the location in 'atomSet') of the
      node which is positioned at postition 'thisNode'
      in this Vector  */
  int getNodeNumber(int thisNode){
    return (at(thisNode))->number;
  }

  QVector<Atom *> getEdge(int number) {
    Atom *from, *to;
    if (number < size() - 1){
      from = at(number);
      to =  at(number + 1);
    }
    else{
      from = at(0);
      to =  at(size()-1);
    }
    QVector<Atom *> ret(2);
    ret.replace(0, from);
    ret.replace(1, to);
    return ret;
  }
};


#endif
