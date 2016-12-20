// sdg.h is largely based on JMDraw by Christoph Steinbeck.
// License notice follows:
/* JMDraw.java
 *
 * $RCSfile: sdg.h,v $    $Author: bherger $    $Date: 2005/05/18 01:06:37 $    $Revision: 1.2 $
 *
 * Copyright (C) 1997, 1998, 1999, 2000  The JChemPaint project
 *
 * Contact: steinbeck@ice.mpg.de
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * as published by the Free Software Foundation; either version 2.1
 * of the License, or (at your option) any later version.
 * All I ask is that proper credit is given for my work, which includes
 * - but is not limited to - adding the above copyright notice to the beginning
 * of your source code files, and to any copyright notice that you may
 * distribute with programs based on this work.
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

#ifndef SDG_H
#define SDG_H

#include "atom.h"
#include "ring.h"
#include "setofrings.h"
#include "sorf.h"

class Integer {
 public:
  int eger;
  Integer() { }
  Integer(int x) { eger = x; }
  int intValue() { return eger; }
};

class SPoint {
 public:
  /** x coordinate */
  double x;
  /** y coordinate */
  double y;
  // substituents
  int subst;

  /** Constructs an empty point */
  SPoint(){ }

  /** Constructs a point with given double precision x and y coordinates */
  SPoint(double x1, double y1){
    x = x1;
    y = y1;
  }

  /** Constructs a point with given integer x and y coordinates */
  SPoint(int x1, int y1){
    x = (double)x1;
    y = (double)y1;
  }

  /** Constructs a point as a dublicate of another point */
  SPoint(const SPoint &p){
    x = p.x;
    y = p.y;
  }
};

/** Construct that stores the parameters needed for the placement of
    any ring n attached to an (n-1)-st ring (or so...). The parameters are
    originally (in Denglers program) called phi, iph, b and ibb and are
    returned by the routine phiwa */

class FusedRingParams{
 public:
  FusedRingParams() { }
  double startAngle; /* start angle for the method positionRingAtoms().
			Originally called 'phi'	*/
  int annulAxisOrientation; /* indicates if the axis that links the two
			       ring centers is perpendicular to the x axis.
			       Originally called 'iph' */
  double toggleStartAngleSign; /* -1 if startAngle has to be taken used
				  as a negative value by the method
				  transform(). Positive otherwise. Don't know
				  why this has to be 'double'.
				  Originally called 'b' */
  int startAtomNumber; 	/* Number of StartAtom for the ringSort.
			   Originally called 'ibb' */
  QString toString(){
    QString s = "";
    s += QString("startAngle = %lf\n").arg(startAngle);
    s += QString("annulAxisOrientation = %d\n").arg(annulAxisOrientation);
    s += QString("toggleStartAngleSign = %lf\n").arg(toggleStartAngleSign);
    s += QString("startAtomNumber = %d\n").arg(startAtomNumber);
    return s;
  }
};

class SDG {
 public:
  SDG() { TWO = (double)2.0; prevang = 0.0; ncalls = 0; leftright = false;
  arerings = true; }
  void setAtoms(QVector<Atom *> a1) { atomSet = a1; }
  void setRings(setOfRings s1) { sssr = s1; }
  QVector<Atom *> getAtoms() { return atomSet; }
  void setBL(double bl) { bondLength = bl; }

  QVector<Atom *> atomSet;
  setOfRings sssr;
  double bondLength;
  double bondLineSep;
  double roundFactor;
  double TWO;
  int ringTracker;
  int ddebug;
  bool allDone;

  // aliphatic
  double prevang;
  bool arerings, leftright;
  int ncalls;

  void exec() {
    bondLineSep = 0.05;
    roundFactor = bondLength * 10000;
    ringTracker = 0;
    ddebug = 0;
    draw();
  }

  /** Performs the actual calculation of the coordinated based on a
      given set of Nodes */

  void draw(){
	qInfo() << "Starting SDG::draw() with atomSet: " << atomSet.size();
    int thisAtom = 0, neighbor = 0;
    QVector<int> ilist(10);
    int counter;
    SPoint tempPoint;
    bool vmit;
    QVector<double> rot(10);
    int nrOfEdges = 0;
    for (int i = 0; i < atomSet.count(); i++)
      {
	atomSet[i]->status = 0;
	atomSet[i]->ringCounter = 0;
      }

    sortBondList();
    for (int f = 0; f < atomSet.count(); f++)
      {
	nrOfEdges += atomSet[f]->degree;
      }
    nrOfEdges /= 2;
    /* compute the minimum number of rings as
       given by Frerejacque, Bull. Soc. Chim. Fr., 5, 1008 (1939) */
    int zRing = nrOfEdges - atomSet.count() + 1;
    // if there are rings, get them...
    if (zRing > 0){
      SetOfRingsFinder sorf;
      sssr = sorf.findSSSR(atomSet); //should be passed from Molecule()
      sssr.sort();
      sssr.makeNodeRingLists(atomSet);

      if (sssr.size() > 0){
	/* We have rings
	   Now, let's find the atom that is member of
	   highest number of largest rings	(Is that clear? :-))
	*/
	int fatest = 0; // the complexity of the 'fatest' atom;
	int fatestIndex; // the index of the fatest atom;
	int cmpl, sum = 0; //temp variables for ring complexity comparison
	int complRing = 0; // the most complext ring so far and finally
	int complRingSize = 0;
//	int lastRing;
//	bool noMore;
	bool stillThere;
//	bool drawn;
	int toDraw = -1;
//	int debug = 0;

	Ring *ring;
	// find the ring with the largest number of fatest atoms
	int f;
	for (f = sssr.size() - 1; f > -1; f--){
	  ring = sssr.at(f);
	  cmpl = ring->getComplexity(atomSet);
	  if (cmpl > sum){
	    sum = cmpl;
	    complRing = f;
	    complRingSize = ring->size();
	    fatest = (ring->at(0))->number;
	    fatestIndex = (ring->at(0))->ringCounter;
	  }
	}
	positionRingAtoms(complRing, SPoint(0,0), (double)0, 0, complRingSize - 1);
	ring = sssr.at(complRing);
	ring->status = 1;

	for (f = 0; f < ring->size(); f++){
	  atomSet.at(ring->getNodeNumber(f))->status = 1;
	}
	do{
	  do{
	    stillThere = false;
	    for (int f = 0; f < sssr.size(); f++){
	      if ((sssr.at(f))->status == 1) {
		positionRingSubstituents(f);
		//if (true) return;
		stillThere = true;
	      }
	    }
	  }while(stillThere);
	  toDraw = whatToDraw();
	  if (toDraw != -1) positionAtom(toDraw);
	  //reportAtomSet(atomSet);
	}while(whatToDraw() != -1);
      }
    }

    // if there are no rings
    else{
      // (Herger) make aliphatics pretty
      arerings = false;
      for (thisAtom = 0; thisAtom < atomSet.count(); thisAtom++){
	if (atomSet[thisAtom]->degree > 1) break;
      }
      // error check for moelcules with only two atoms
      if (thisAtom >= atomSet.count()) thisAtom = 0;
      atomSet[thisAtom]->x = (double)0;
      atomSet[thisAtom]->y = (double)0;
      for (neighbor = 0; neighbor < atomSet[thisAtom]->degree; neighbor++){
	if (atomSet[atomSet[thisAtom]->intnodeTable[neighbor]]->degree > 1 && atomSet[thisAtom]->intnodeTable[neighbor] != thisAtom) break;
      }
      // (Herger) error check here (next 1 line)
      if (neighbor == atomSet[thisAtom]->degree) neighbor = 0;
      neighbor = atomSet[thisAtom]->intnodeTable[neighbor];
      // put neighbor onto positive x axis
      atomSet[neighbor]->x = (double)bondLength;
      atomSet[neighbor]->y = (double)0;
      atomSet[neighbor]->status = 1;
      atomSet[thisAtom]->status = 1;
      for (int f = 0; f < 10; f++) ilist[f] = 0;
      counter = atomSet[neighbor]->degree;
      vmit = true;
      rot = angleOpt(neighbor, ilist, vmit);
      if (counter > 0){
	for (int f = 0; f < counter; f++){
	  if (atomSet[atomSet[neighbor]->intnodeTable[f]]->status < 1){
	    tempPoint = placeAtom(SPoint(atomSet[thisAtom]->x, atomSet[thisAtom]->y),
				  SPoint(atomSet[neighbor]->x, atomSet[neighbor]->y), bondLength);
	    tempPoint = rotAtom(SPoint(atomSet[neighbor]->x, atomSet[neighbor]->y), tempPoint, rot[f]);
	    atomSet[atomSet[neighbor]->intnodeTable[f]]->x = tempPoint.x;
	    atomSet[atomSet[neighbor]->intnodeTable[f]]->y = tempPoint.y;

	    atomSet[atomSet[neighbor]->intnodeTable[f]]->status = 1;
	  }
	}
      }
      atomSet[neighbor]->status = 2;
      doTheRest();
    }

    //if (true) return;
  }

  /** Scans the array of nodes for those that have not been finally drawn */

  int whatToDraw(){
    for (int f = 0; f < atomSet.count(); f++){
      if (atomSet[f]->status == 1){
	qDebug() << "whatToDraw(): " << f ;
	return f;
      }
    }
    qDebug() << "whatToDraw(): -1" ;
    return -1;
  }

  /** Takes care of drawing an ring-less graph once one start
	    atom has been placed. */

  int doTheRest(){
    int counter, result = -1;
    bool allDone;
    do{
      allDone = true;
      counter = -1;
      do{
	counter++;
	if (atomSet[counter]->status == 1)	 allDone = false;
      }while(!((counter == atomSet.size() - 1) || (atomSet[counter]->status == 1)));
      if (allDone){
	return result;
      }
      if (counter == atomSet.size()) return result;
      result = positionAtom(counter);

    }while (allDone == false);
    return result;
  }


  /** Constructs a linked list of nodes from a
	    Connectivity Table 'ConMat'. The element symbols are given as
	    strings in the Vector 'symbols'.
  public static Atom[] initAtomSet(int[][] conMat, Vector symbols){
    Atom[] atSet;
    atSet = new Atom[conMat.length];
    for (int f = 0; f < conMat.length; f ++){
      atSet[f] = new Atom();
      atSet[f].symbol = (String)symbols.elementAt(f);
      atSet[f].number = f;
      atSet[f].degree = 0;
      for (int g = 0; g < conMat.length; g ++){
	if (conMat[f][g] > 0){
	  atSet[f]->intnodeTable[atSet[f].degree] = g;
	  atSet[f]->bondTable[atSet[f].degree] = conMat[f][g];
	  atSet[f].degree ++;
	}
      }
    }
    return atSet;
    }*/



  /** lists the given atomSet to the system console
  void reportAtomSet(Atom[] as){
    Atom node;
    String line;
    for (int f = 0; f < as.length; f++){
      node = (Atom)as[f];
      line = "Atom " + f + ":";
      for (int g = 0; g < node.degree; g++){
	line += node.nodeTable[g] + " ";
      }
      line += "; Status: " + node.status;
      line += "; In Rings: ";
      for (int g = 0; g < node.inRings.length; g++){
	line += node.inRings[g] + " ";
      }
      line += "; X: " + node->x;
      line += "; Y: " + node.y;
      System.out.println(line);
    }
    }

  protected Atom[] cloneAtomSet(Atom[] thisSet){
    Atom[] newAtomSet = new Atom[thisSet.length];
    for (int f = 0; f < thisSet.length; f++){
      newAtomSet[f] = (Atom)thisSet[f].clone();
    }
    return newAtomSet;
    }*/

  /** Positions the atoms 'startAtom' up to 'endAtom' in a regular polygon around
	    the point 'center' with 'angleOffSet' as the start angle.
	    Called 'rier' in Denglers C code */
  void positionRingAtoms(int thisRing, SPoint center, double angleOffSet, int startAtom, int endAtom){
    Atom node;
    Ring *ring = sssr.at(thisRing);
    double ph = 1/(360/3.14159246/2), anz =  0, alph = 0, rad = 0, x, y;
    int i, nn;
    angleOffSet = ((angleOffSet/360) - floor(angleOffSet/360)) * 360;
    anz = (double)(180.0/ring->size());
    alph = anz * 2;
    rad = (0.5 * bondLength)/sin(ph*anz);
    for (i = startAtom; i <= endAtom; i++){
      nn = ring->getNodeNumber(i);
      if (atomSet[nn]->status <1){
	x = rad * cos(angleOffSet*ph);
	y = rad * sin(angleOffSet*ph);
	atomSet[nn]->x = x;
	atomSet[nn]->y = y;
      }
      angleOffSet = angleOffSet + alph;
    }
  }

  /** Originally called 'subrin' by Dengler.
	    Takes care of placement of substituents of ring with
	    index 'ringIndex'. Substitutents may be isolated rings,
	    spiro-attached rings, annelated rings, bridging rings
	    or simply aliphatic substitutents.
	    Modified 05-12-98 by Steinbeck to return the number of the ring
	    that has been positioned. Returns -1 if there was no more ring
	    to postition */
  void positionRingSubstituents(int ringIndex){
    // ringType = 1 : spiro annulation requested;
    // ringType = 2 : regular 1-bond annulation requested;
    // ringType = 3 : annulation with more than two shared atoms
    //System.out.println("Ring to substitute: " + ringIndex);
    double ph = 0.0174532925;
    Ring *ring = sssr.at(ringIndex);
    int k = ring->size();
    Ring *secondRing = new Ring;
    int secondRingNumber = -1;
    QList<Integer *> complexAtoms, vect1, vect2, extComplAtoms;
    QVector<int> ilist(10);
    SPoint point, temp2Point, tempPoint;
    int atom, ringCounter, tempRing = -1, atom1 = -1, atom2 = -1, tempAtom = -1, il;
    int /*alreadyPositioned = -1,*/ stillToPos, j, km, iz1, iz2, igr, /*ib = 0,*/ ik = 0;
    int isum1, isum2;
//    int returnRingNumber = -1;
    int size;
    double dist, radius, r1, r2,/* dd,*/ geomDist;
    double rad1, rad2, sux, suy, r, alph, phi = 0, rad;
    bool isSpiro, /*isAlreadyPlaced,*/ ums, vmit, doneSomething = false;
    FusedRingParams fRP;
    Atom n1, n2;
    QVector<double> rot(10);
    QVector<int> tempStatus(10);

    /* Search complex atoms (that are members of more than one ring)
       and preliminarily store their numbers in vector complexAtoms */
    for (int f = 0; f < ring->size(); f++){
      atom = (ring->at(f))->number;
      if (atomSet[atom]->ringCounter > 1){
	extComplAtoms.append(new Integer(atom));
      }
    }

    if (extComplAtoms.count() == 0){
      /**	Atoms of this ring are part of only one ring,
		which already has been positioned by the calling
		method */
      handleRestOfRingSubstituents(ringIndex);
      return;

    }
    else{
      /* ringType = 1 (regular), ringType = 3 (spiro), ringType = 2 (bridged)  */
      int ringType = 0;
      do{
	ringType ++;
	do{
	  doneSomething = false;
				/** there are atoms belonging to more than one ring.
				    Cycle through these atoms */
	  for (int f = 0; f < extComplAtoms.count(); f++){
	    atom = (extComplAtoms.at(f))->intValue();
	    /* find the second ring to which this atom belongs */
	    ringCounter = atomSet[atom]->ringCounter;
	    for (int g = 0; g < ringCounter; g ++){
	      tempRing = atomSet[atom]->inRings[g];
	      if (tempRing != ringIndex &&
		  (sssr.at(tempRing))->status < 1){
		/* we found another ring to which atom belongs
		   which is not yet positioned. Let's remember it. */
		secondRing = sssr.at(tempRing);
		qDebug() << "secondRing:" << secondRing->getSortedString() ;
		secondRingNumber = tempRing;
		size = (getIntersection(ring->getNodeNumberList(), secondRing->getNodeNumberList())).count();
		if (secondRing->notAllPositioned(atomSet)){
		  if (size == 2 && ringType == 1) break;
		  if (size > 2 && ringType == 2) break;
		  if (size == 1 && ringType == 3) break;
		}
	      }
	    }
	    if (secondRing->size() > 0){ // There is a second ring
	      if (secondRing->notAllPositioned(atomSet)){
		complexAtoms = getIntersection(ring->getNodeNumberList(), secondRing->getNodeNumberList());
		//System.out.println(complexAtoms.toString());
		if (complexAtoms.count() == 1 && ringType ==3){
		  /*---------------------*/
		  /* Spiro - Connection  */
		  /*---------------------*/
		  ik = atomSet[(complexAtoms.at(0))->intValue()]->degree;
		  ilist[0] = k;
		  for (j = 1; j < 10; j++)
		    ilist[j] = 0;
		  /* For all atoms bonded to the spiro-atom, store if the
		     partner is part of a ring. If so store the size of this ring.
		  */
		  tempAtom = (complexAtoms.at(0))->intValue();
		  for (int jj = 0; jj < k; jj++) {
		    for (j = 0; j < ik; j++){
		      if (atomSet[tempAtom]->intnodeTable[j] == ring->getNodeNumber(jj)){
			ilist[j + 1] = k;
		      }
		    }
		  }
		  for (j = 0; j < secondRing->size(); j++){
		    tempStatus[j] = atomSet[secondRing->getNodeNumber(j)]->status;

		  }

		  /*------------------------------------*/
		  /*  Position neighbors of spiro-Atom  */
		  /*------------------------------------*/
		  tempAtom = (complexAtoms.at(0))->intValue();
		  positionAtomSubstituents(tempAtom, SPoint(atomSet[tempAtom]->x, atomSet[tempAtom]->y),
					   SPoint(ring->x, ring->y), ilist, ring->size(), secondRingNumber, secondRing->size());

		  //if (secondRingNumber == 0) return;
		  /*----------------------------------------------------*/
		  /* Reset status of atoms of second Ring to zero       */
		  /*----------------------------------------------------*/
		  for (j = 0; j < secondRing->size(); j++){
		    atomSet[secondRing->getNodeNumber(j)]->status = tempStatus[j];
		  }
		  /*----------------------------------------------------*/
		  /*   get the angle for rotAtom() based on the new     */
		  /*   Ring center         .                            */
		  /*----------------------------------------------------*/
		  vmit = true    /* 1 = true */  ;
		  rot = angleOpt(tempAtom, ilist, vmit);
		  alph = 0.0;
		  for (j = 0; j < ik; j++)
		    if (atomSet[atomSet[tempAtom]->intnodeTable[j]]->status == 0)
		      alph = alph + rot[j];
		  alph = alph / 2.0;
		  /*----------------------------------------------------*/
		  /*    Position the new ring center                    */
		  /*----------------------------------------------------*/
		  rad = (bondLength / 2) / sin(ph * (180.0 / secondRing->size()));

		  tempPoint = placeAtom(SPoint(ring->x, ring->y),
					SPoint(atomSet[tempAtom]->x, atomSet[tempAtom]->y), rad);

		  if (alph != (double) 0.0)
		    //qDebug() << "Invoking rot-Atom() from line 538" ;
		    tempPoint = rotAtom(SPoint(atomSet[tempAtom]->x, atomSet[tempAtom]->y), tempPoint, alph);

		  secondRing->x = tempPoint.x;
		  secondRing->y = tempPoint.y;
		  /*----------------------------------------------------*/
		  /*   Position atoms of new Ring and transfer to 	   */
		  /*   their correct place                              */
		  /*----------------------------------------------------*/
		  isSpiro = true;
		  fRP = getFusedRingParams(SPoint(ring->x, ring->y),
					   SPoint(atomSet[tempAtom]->x, atomSet[tempAtom]->y),
					   tempAtom, tempAtom, isSpiro, secondRingNumber);
		  phi = fRP.startAngle + 360.0 / secondRing->size();



		  secondRing->sort(fRP.startAtomNumber);
		  positionRingAtoms(secondRingNumber, SPoint(0,0), phi, 1, secondRing->size() - 1);
		  secondRing->status = 1;
		  transfer(1, secondRing->size() -1, secondRingNumber, SPoint(atomSet[fRP.startAtomNumber]->x, atomSet[fRP.startAtomNumber]->y), fRP.annulAxisOrientation, fRP.toggleStartAngleSign);
		  //System.out.println("Spiro");
		  doneSomething = true;
		}
		else if (complexAtoms.count() == 2 && ringType == 1){
		  /* 'Regular' 2-Ring annelation found
		     Ring 'ring' and 'secondRing' do have exactly
		     one bond in common. The center of 'secondRing' is
		     now positioned on the line going from the center
		     of ring via the center of the common bond */
		  //fRP = new FusedRingParams();
		  atom1 = (complexAtoms.at(0))->intValue();
		  atom2 = (complexAtoms.at(1))->intValue();
		  dist = sqrt(pow(atomSet[atom1]->x - atomSet[atom2]->x, TWO) +
				   pow(atomSet[atom1]->y - atomSet[atom2]->y, TWO));
		  tempPoint = placeAtom(SPoint(atomSet[atom1]->x, atomSet[atom1]->y), SPoint(atomSet[atom2]->x, atomSet[atom2]->y), -dist/2);
		  radius = secondRing->getRadius(bondLength);
		  r1 = radius * radius;
		  r2 = dist * dist / 4;
		  if (r2 < r1){
		    radius = sqrt(r1 -r2);
		  }
		  tempPoint = placeAtom(SPoint(ring->x, ring->y), tempPoint, radius);
		  secondRing->x = tempPoint.x;
		  secondRing->y = tempPoint.y;
		  isSpiro = false;
		  fRP = getFusedRingParams(SPoint(ring->x, ring->y), tempPoint, atom1, atom2, isSpiro, secondRingNumber);
		  secondRing->sort(fRP.startAtomNumber);
		  if (fRP.startAtomNumber == atom1) tempAtom = atom2;
		  else tempAtom = atom1;
		  if ((secondRing->at(1))->number == tempAtom){
		    secondRing->invert();
		    secondRing->sort(fRP.startAtomNumber);
		  }
		  positionRingAtoms(secondRingNumber, SPoint(0,0), fRP.startAngle, 1, secondRing->size() - 2);
		  secondRing->status = 1;
		  transfer(1, secondRing->size() - 2, secondRingNumber, SPoint(ring->x, ring->y), fRP.annulAxisOrientation, fRP.toggleStartAngleSign);
		  //System.out.println("Regular " + secondRingNumber);
		  doneSomething = true;

		  handleBridgeHeads(atom1, atomSet[atom1]->x, atomSet[atom1]->y,
				    atomSet[atom2]->x, atomSet[atom2]->y, ring->size(), secondRing->size());
		  handleBridgeHeads(atom2, atomSet[atom2]->x, atomSet[atom2]->y,
				    atomSet[atom1]->x, atomSet[atom1]->y, ring->size(), secondRing->size());

		}
		else if (complexAtoms.count() > 2 && ringType ==2){
		  /* The two rings join more than one bond. Find the two
		     bridge atoms... */
		  // ?! Dengler does this by looking for the two atoms in
		  // ?! the set of complex atoms that are !geometrically! most
		  // ?! far away from each other. Is this always a valid assumption.
		  //System.out.println("Bridged ring systems are not yet implemented");
		  vect1.clear();
		  vect2.clear();
		  geomDist =  0.0;
		  atom = -1;
		  tempAtom = -1;
		  for (int ii = 0; ii <complexAtoms.count(); ii++){
		    atom = (complexAtoms.at(ii))->intValue();
		    for (j = ii + 1; j < complexAtoms.count(); j++){
		      tempAtom = (complexAtoms.at(j))->intValue();
		      dist = sqrt(sqr(atomSet[atom]->x - atomSet[tempAtom]->x) +
				       sqr(atomSet[atom]->y - atomSet[tempAtom]->y));
		      if (dist > geomDist){
			atom1 = atom;
			atom2 = tempAtom;
			geomDist = dist;
		      }
		    }
		  }
		  /* stillToPos atoms still have to be positioned */
		  stillToPos = 0;
		  for (il = 0; il < secondRing->size(); il ++){
		    if (atomSet[secondRing->getNodeNumber(il)]->status > 0) stillToPos ++;
		  }
		  stillToPos = secondRing->size() - stillToPos;
		  if (stillToPos != 0){
		    il = -1;
		    do {
		      il++;
		      atom = (ring->at(il))->number;
		    }while (atom != atom1 && il < ring->size() - 1);
		    /* Calculate the coordinates of the center
		       of the second ring */
		    ums = true;
		    km = il;
		    iz1 = 0;
		    iz2 = 0;
		    igr = ring->size();
		    for (j = 1; j < igr; j++) {
		      km ++;
		      if (km >= igr) km = 0;
		      if (ring->getNodeNumber(km) == atom2) {
			ums = false  /* 0 = false */ ;
			if (km >= igr) km = -1;
		      }
		      else {
			if (ums) {
			  iz1 ++;
			  vect1.append(new Integer(ring->getNodeNumber(km)));
			}  /* if (ums) */
			if (!ums) {
			  iz2 ++;
			  vect2.append(new Integer(ring->getNodeNumber(km)));
			}
		      }
		    }
		    /* At this point, the first Ring 'ring' has been partioned into
		       the two chains which are intersected by
		       the two bridgeheads. The two chains are stored in vect1 and vect2. */
		    isum1 = 0;
		    isum2 = 0;
		    for (il = 0; il < iz1; il++)
		      isum1 = isum1 + atomSet[(vect1.at(il))->intValue()]->degree;
		    for (il = 0; il < iz2; il++)
		      isum2 = isum2 + atomSet[(vect2.at(il))->intValue()]->degree;
		    /* We now have summed up the number of neighbors
		       for all the atoms in each chain and stored them in isum1 and isum2. */
		    sux = (double)0.0;
		    suy = (double)0.0;
		    /* Use the least hindered side for the placement
		       of 'secondRing' */
		    if (isum1 > isum2) {
		      for (il = 0; il < iz2; il++) {
			sux = sux + atomSet[(vect2.at(il))->intValue()]->x - ring->x;
			suy = suy + atomSet[(vect2.at(il))->intValue()]->y - ring->y;
		      } /* for (il = 1; il <= iz2; il++) */
		      sux = sux / iz2;
		      suy = suy / iz2;
		    }
		    else {
		      for (il = 0; il < iz1; il++) {
			sux = sux + atomSet[(vect1.at(il))->intValue()]->x - ring->x;
			suy = suy + atomSet[(vect1.at(il))->intValue()]->y - ring->y;
		      } /* for (il = 1; il <= iz1; il++) */
		      sux = sux / iz1;
		      suy = suy / iz1;
		    }  /* if (isum1 > isum2) */
		    r = sqrt(sux * sux + suy * suy);
		    rad1 = (bondLength /2) / sin(ph * (180.0 / ring->size()));
		    rad2 = (bondLength /2) / sin(ph * (180.0 / secondRing->size()));
		    secondRing->x = ring->x + (0.8 * (rad1 + rad2) * sux / r);
		    secondRing->y = ring->y + (0.8 * (rad1 + rad2) * suy / r);
		    tempPoint = SPoint(secondRing->x, secondRing->y);
		    /*------------------------------------*/
		    /* Position ring atoms of second Ring */
		    /*------------------------------------*/
		    isSpiro = false;
		    fRP = getFusedRingParams(SPoint(ring->x, ring->y), tempPoint, atom1, atom2, isSpiro, secondRingNumber);
		    secondRing->sort(fRP.startAtomNumber);
		    km = secondRing->size();
		    if (atomSet[secondRing->getNodeNumber(1)]->status > 0){
		      /* sort ring */
		      secondRing->invert();
		      secondRing->sort(fRP.startAtomNumber);
		    }
		    alph = -(360.0 /km) * (stillToPos -1) / 2.0;
		    tempPoint = placeAtom(SPoint(ring->x, ring->y), SPoint(secondRing->x, secondRing->y), rad2);
		    secondRing->status = 1;
		    //System.out.println("Bicyclo");
		    for (il = 0; il < km; il++) {
		      if (atomSet[secondRing->getNodeNumber(il)]->status <= 0) {
			//qDebug() << "Invoking rot-Atom() from line 719" ;
			temp2Point = rotAtom(SPoint(secondRing->x,secondRing->y), tempPoint, alph);
			atomSet[secondRing->getNodeNumber(il)]->status = 1;
			atomSet[secondRing->getNodeNumber(il)]->x = temp2Point.x;
			atomSet[secondRing->getNodeNumber(il)]->y = temp2Point.y;
			alph = alph + 360.0 / km;
		      }
		    }
		    doneSomething = true;
		    handleBridgeHeads(atom1, atomSet[atom1]->x, atomSet[atom1]->y,
				      atomSet[atom2]->x, atomSet[atom2]->y, ring->size(), secondRing->size());
		    handleBridgeHeads(atom2, atomSet[atom2]->x, atomSet[atom2]->y,
				      atomSet[atom1]->x, atomSet[atom1]->y, ring->size(), secondRing->size());

		  }
		}
	      }
	    }
	  }
	}while(doneSomething);
      }while(ringType < 3);
      handleRestOfRingSubstituents(ringIndex);
    }
    //return returnRingNumber;
  }

  int round(double rin) {
    double d1;
    int rout, sgn;

    if (rin < 0.0) {
      sgn = -1; rin = -rin;
    } else {
      sgn = 1;
    }

    rout = (int)rin;

    d1 = rin - (double)rout;

    if (d1 > 0.5) rout++;

    rout *= sgn;

    return rout;
  }

  /** Returns an object storing the parameters for drawing the second Ring.
	    Information about whether to start clockwise, etc. Originally
	    called 'phiwa' */
  FusedRingParams getFusedRingParams(SPoint center1, SPoint center2,
				     int bridgeAtom1, int bridgeAtom2,
				     bool isSpiro, int numberOfSecondRing){
    /* 	center 1, center 2:			Centers of the two rings.
	bridgeAtom1, bridgeAtom2:	Number of the two bridging atoms
	isSpiro:			Are the rings spiro-attached
	numberOfSecondRing:	     Number (in sssr) of second Ring
    */
    double angle, coord, x1, x2, y1, y2;
    x1 = round(center1.x * roundFactor) / roundFactor;
    y1 = round(center1.y * roundFactor) / roundFactor;
    x2 = round(center2.x * roundFactor) / roundFactor;
    y2 = round(center2.y * roundFactor) / roundFactor;
    angle = 360 / (sssr.at(numberOfSecondRing))->size();
    qDebug() << "FRP:" << x1 << "," << y1 << ";" << x2 << "," << y2 ;
    FusedRingParams fRP;
    // First block
    if (x2 > x1 && y2 >= y1){
      fRP.toggleStartAngleSign = (double)1.0; // b
      fRP.annulAxisOrientation = 1; // iph
      if (isSpiro){
	fRP.startAtomNumber = bridgeAtom1;	// ibb
	fRP.startAngle = (double)180.0;    // phi
	return fRP;
      }
      fRP.startAngle = (double)(180.0 + angle + 0.5 * angle);
      if (atomSet[bridgeAtom1]->y > atomSet[bridgeAtom2]->y){
	coord = atomSet[bridgeAtom2]->y;
	fRP.startAtomNumber = bridgeAtom2;	// ibb
      }
      else{
	coord = atomSet[bridgeAtom1]->y;
	fRP.startAtomNumber = bridgeAtom1;	// ibb
      }
      //System.out.println("First block. " + numberOfSecondRing + " . " + fRP.startAtomNumber + " . " + (fRP.startAtomNumber == bridgeAtom1) + ", " + x1 + ", " + y1 +", " + x2 +", " + y2);
      return fRP;
    }
    // Second block
    if (x2 > x1 && y2 < y1){
      fRP.toggleStartAngleSign = (double)-1.0; // b
      fRP.annulAxisOrientation = 1; // iph
      if (isSpiro){
	fRP.startAtomNumber = bridgeAtom1;	// ibb
	fRP.startAngle = (double)180.0;    // phi
	return fRP;
      }
      fRP.startAngle = (double)(180.0 + angle + 0.5 * angle);
      if (atomSet[bridgeAtom1]->y > atomSet[bridgeAtom2]->y){
	coord = atomSet[bridgeAtom2]->y;
	fRP.startAtomNumber = bridgeAtom2;	// ibb
      }
      else{
	coord = atomSet[bridgeAtom1]->y;
	fRP.startAtomNumber = bridgeAtom1;	// ibb
      }
      //System.out.println("Second block. " + numberOfSecondRing + " . " + fRP.startAtomNumber + " . " + (fRP.startAtomNumber == bridgeAtom1));
      return fRP;
    }
    // Third Block
    if (x2 < x1 && y2 <= y1){
      fRP.toggleStartAngleSign = (double)1.0; // b
      fRP.annulAxisOrientation = 1; // iph
      if (isSpiro){
	fRP.startAtomNumber = bridgeAtom1;	// ibb
	fRP.startAngle = (double)0.0;    // phi
	return fRP;
      }
      fRP.startAngle = (double)(angle + 0.5 * angle);
      if (atomSet[bridgeAtom1]->y > atomSet[bridgeAtom2]->y){
	coord = atomSet[bridgeAtom1]->y;
	fRP.startAtomNumber = bridgeAtom1;	// ibb
      }
      else{
	coord = atomSet[bridgeAtom2]->y;
	fRP.startAtomNumber = bridgeAtom2;	// ibb
      }
      //System.out.println("Third block. " + numberOfSecondRing + " . " + fRP.startAtomNumber + " . " + (fRP.startAtomNumber == bridgeAtom1));
      return fRP;
    }
    // Fourth block
    if (x2 < x1 && y2 > y1){
      fRP.toggleStartAngleSign = (double)-1.0; // b
      fRP.annulAxisOrientation = 1; // iph
      if (isSpiro){
	fRP.startAtomNumber = bridgeAtom1;	// ibb
	fRP.startAngle = (double)0.0;    // phi
	return fRP;
      }
      fRP.startAngle = (double)(angle + (0.5 * angle));
      if (atomSet[bridgeAtom1]->y > atomSet[bridgeAtom2]->y){
	coord = atomSet[bridgeAtom1]->y;
	fRP.startAtomNumber = bridgeAtom1;	// ibb
      }
      else{
	coord = atomSet[bridgeAtom2]->y;
	fRP.startAtomNumber = bridgeAtom2;	// ibb
      }
      //System.out.println("Fourth block. " + numberOfSecondRing + " . " + fRP.startAtomNumber + " . " + (fRP.startAtomNumber == bridgeAtom1));
      return fRP;
    }
    // Fifth block
    if (x2 == x1 && y1 > y2){
      fRP.toggleStartAngleSign = (double)-1.0; // b
      fRP.annulAxisOrientation = 0; // iph
      if (isSpiro){
	fRP.startAtomNumber = bridgeAtom1;	// ibb
	fRP.startAngle = (double)180.0;    // phi
	return fRP;
      }
      fRP.startAngle = (double)(180 + angle + 0.5 * angle);
      if (atomSet[bridgeAtom1]->x > atomSet[bridgeAtom2]->x){
	coord = atomSet[bridgeAtom2]->x;
	fRP.startAtomNumber = bridgeAtom2;	// ibb
      }
      else{
	coord = atomSet[bridgeAtom1]->x;
	fRP.startAtomNumber = bridgeAtom1;	// ibb
      }
      //System.out.println("Fifth block. " + numberOfSecondRing + " . " + fRP.startAtomNumber + " . " + (fRP.startAtomNumber == bridgeAtom1));
      return fRP;
    }
    // Sixth block
    if (x2 == x1 && y2 > y1){
      fRP.toggleStartAngleSign = (double)1.0; // b
      fRP.annulAxisOrientation = 0; // iph
      if (isSpiro){
	fRP.startAtomNumber = bridgeAtom1;	// ibb
	fRP.startAngle = (double)180.0;    // phi
	return fRP;
      }
      fRP.startAngle = (double)(180 + angle + (0.5 * angle));
      if (atomSet[bridgeAtom1]->x > atomSet[bridgeAtom2]->x){
	coord = atomSet[bridgeAtom1]->x;
	fRP.startAtomNumber = bridgeAtom1;	// ibb
      }
      else{
	coord = atomSet[bridgeAtom2]->x;
	fRP.startAtomNumber = bridgeAtom2;	// ibb
      }
      //System.out.println("Sixth block. " + numberOfSecondRing + " . " + fRP.startAtomNumber + " . " + (fRP.startAtomNumber == bridgeAtom1));
      return fRP;
    }

    if (isSpiro) {
      fRP.startAtomNumber = bridgeAtom1;	// ibb
      fRP.startAngle = (double)180.0;    // phi
    }

    return fRP;
  }

  /** transfers the atoms of the given ring from the origin of the
	    coordinate system to the point at target.x, target.y */
  void transfer(int startAtom, int endAtom,
		int ringNumber, SPoint target,
		int annulAxisOrientation, // iph
		double toggleStartAngleSign ){ // b
    int i;
    double phi = 0, aaa, bbb, ax, ay, x, y;
    Ring *ring = sssr.at(ringNumber);
    //Atom node = new Atom();
    int nn = 0;
    ring->x = round(ring->x * roundFactor) / roundFactor;
    ring->y = round(ring->y * roundFactor) / roundFactor;
    target.x = round(target.x * roundFactor) / roundFactor;
    target.y = round(target.y * roundFactor) / roundFactor;

    aaa = ring->x - target.x;
    bbb = ring->y - target.y;
    //aaa = Math.round(aaa * roundFactor) / roundFactor;
    //bbb = Math.round(bbb * roundFactor) / roundFactor;
    //if(ringNumber != 3){
    if (annulAxisOrientation == 1 && aaa == (double) 0) phi = 3.14159246;
    if (annulAxisOrientation == 1 && aaa != (double) 0){
      phi = atan2(bbb, aaa);
      if (phi < 0.0) phi = -phi;
    }
    if (annulAxisOrientation == 0.0) phi = 3.14159246 / 2;
    if (phi > 3.14159246/2) phi = 3.14159246 - phi;
    if (toggleStartAngleSign < 0.0) phi = -phi;
    for (int f = startAtom; f <= endAtom; f++){
      nn = ring->getNodeNumber(f);
      if (atomSet[nn]->status == 0){
	x = atomSet[nn]->x;
	y = atomSet[nn]->y;
	ax = x * cos(phi) -
	  y * sin(phi) + ring->x;
	ay = x * sin(phi) +
	  y * cos(phi) + ring->y;
	atomSet[nn]->x = ax;
	atomSet[nn]->y = ay;
	atomSet[nn]->status = 1;
      }
    }
  }

  /** Returns the point that is positioned on the line between 'origin' and
    'vector' in distance 'distance' from origin. Originally called
    'neuat()' */
  SPoint placeAtom(SPoint origin, SPoint vector, double distance){
    double a, b, cc;
    a = vector.x - origin.x;
    b = vector.y - origin.y;
    cc = distance / sqrt(a*a + b*b);
    return SPoint(vector.x + a * cc, vector.y + b * cc);
  }



  /** Rotates the atom at Point 'point' around the Point 'center' with the angle 'angle' */
  SPoint rotAtom(SPoint center, SPoint point, double angle){
    double ph = 0.0174532925;
    double  p, co, si, x1, y1, x, y;

    // make aliphatics look good
    //qDebug() << "ANGLE:" << angle;

    if (arerings == true) {
      if (ncalls == 0) {
	ncalls = 1;
      } else {
	if (fabs(angle) < 1.0) {
	  ncalls++;
	  if (prevang == 0.0) angle = 60.0;
	  if (prevang == 60.0) angle = -60.0;
	  if (prevang == -60.0) angle = 60.0;
	  prevang = angle;
	}
      }
    } else {  // no rings
      if (fabs(angle) < 1.0) {
	ncalls++;
	if (ncalls == 1) angle = 60.0;
	if (ncalls == 2) angle = 60.0;
	if (ncalls == 3) angle = -60.0;
	if (ncalls == 4) angle = 60.0;
	if (ncalls == 5) angle = -60.0;
	if (ncalls == 6) angle = 60.0;
	if (ncalls > 6) {
	  if ((ncalls % 2) == 0)
	    angle = 60.0;
	  else
	    angle = -60.0;
	}
      }
    }

    //qDebug() << " " << angle ;
    // end aliphatics garbling

    p = ph * angle;
    si = sin(p);
    co = cos(p);
    x = point.x - center.x;
    y = point.y - center.y;
    x1 = co * (x) - si * (y);
    y1 = si * (x) + co * (y);
    x = x1 + center.x;
    y = y1 + center.y;
    return SPoint(x, y);
  }

  /** Positions substituents around atom 'atomIndex'
	    Returns "-1" if no ring was drawn or the ring number of the ring draw */
  int positionAtomSubstituents(int atomIndex, SPoint point, SPoint helpPoint,
			       QVector<int> ilist, int modus, int ir, int igr){
    double ph =  0.0174532925;
    int    i, ib, /*ibb, iii, iph,*/ k;
    double alph, /*b, phi,*/ rad/*, xx, yy*/;
    double x = point.x;
    double y = point.y;
    double xh = helpPoint.x;
    double yh = helpPoint.y;
    bool   vmit, ii;
    SPoint tempPoint;
    FusedRingParams fRP;
    Ring *ring = new Ring;
    SPoint test;
    int tempAtom;
    if (sssr.size() > 0){
      ring = sssr.at(ir);
    }
    QVector<double> rot(10);
    ib=0;
    k = atomSet[atomIndex]->degree;
    if (k <= 1) {
      atomSet[atomIndex]->status = 2;
      return -1;
    }  /* if (k <= 1) */
    /*---------------------------------------------------------------*/
    /*   'vmit' is false in case of substitution type Atom -> Atom   */
    /*                     and substitution   Atom -> Ring       	   */
    /*               true  with substitution type  Ring -> Atom      */
    /*---------------------------------------------------------------*/
    vmit = false;
    if ((modus > 1) && (k <= 2)) {
      // invoking aliphatic
      if (arerings == true) ncalls = 0;
      //
      atomSet[atomIndex]->status = 2;
      return -1;
    } /* if ((modus > 1) && (k <= 2))  */
    if (modus > 1) vmit = true;
    rot = angleOpt(atomIndex, ilist, vmit);
    /*------------------------------------*/
    /* Positioning of Neighbors           */
    /*------------------------------------*/
    for (i = 0; i < k; i++) {
      tempAtom = atomSet[atomIndex]->intnodeTable[i];
      if (atomSet[tempAtom]->status <= 0) {
	tempPoint = placeAtom(helpPoint, point, bondLength);
	//qDebug() << "Invoking rot-Atom() from line 1079" ;
	test = rotAtom(point, tempPoint, rot[i]);
	atomSet[tempAtom]->x = test.x;
	atomSet[tempAtom]->y = test.y;
	atomSet[tempAtom]->status = 1;
	if (atomSet[tempAtom]->degree == 1)
	  atomSet[tempAtom]->status = 2;
      }  /* if (IATGEZ[atom[thisAtom]->bindpart[i]] <= 0) */
    }  /* for (i = 0; i < k; i++) */
    atomSet[atomIndex]->status = 2;
    if (modus != 1) return -1;
    /*---------------------------------------------------------------*/
    /*      Substitution type  Atom -> Ring                          */
    /*      Ring is being positioned.                                */
    /*---------------------------------------------------------------*/
    alph = 0.0;
    for (i = 0; i < k; i++) {
      if (ilist[i + 1] > 0) {
	alph = alph + rot[i];
	atomSet[atomSet[atomIndex]->intnodeTable[i]]->status = 0;
      }  /* if (ilist[i+1] > 0) */
    }  /* for (i = 0; i < k; i++) */
    alph = alph * 0.5;
    /*---------------------------------------------------------------*/
    /*                position ring center                           */
    /*  -------------------------------------------------------------*/
    rad = bondLength/ 2 / sin(ph * (180.0 / igr));
    tempPoint = placeAtom(SPoint(xh, yh), SPoint(x, y), rad);
    //qDebug() << "Invoking rot-Atom() from line 1106" ;
    tempPoint = rotAtom(SPoint(x, y), tempPoint, alph);
    ring->x = tempPoint.x;
    ring->y = tempPoint.y;
    ii = true;
    fRP = getFusedRingParams(SPoint(x, y), tempPoint, atomIndex, ib, ii, ir);
    ring->sort(fRP.startAtomNumber);
    positionRingAtoms(ir, SPoint(0,0), fRP.startAngle, 0, igr - 1);
    if (ir == 4){
      ir = ir;
    }
    ring->status = 1;
    transfer(1, ring->size()-1 , ir,SPoint(x, y) , fRP.annulAxisOrientation, fRP.toggleStartAngleSign);
    return ir;
  }


  /* Checks the needs of zatoms neighbors in terms of space and
	   calculates a list of angles the their positioning in rotAtom() .
	   Angles are returned in int[] rot.
	   int[] rili contains information about zatom and its neighbors.
	   rili[i] = 0 -> it's no ringatom
	   rili[i] = x (x > 0) -> x is part of ring with size x
	   */
  QVector<double> angleOpt(int zatom, QVector<int> rili, bool vmit){
    QVector<double> rot(10);
    int    hatom = 0, i, kk, ll, neigh;
    bool   wahl;
    double sum, fewi, phi, alpha;
    QVector<int> i1(10), i2(10);
    QVector<double> i3(10), nclass(10);
    for (int f = 0; f < 10; f++) rot[f] = (double)0;
    wahl = true;
    neigh = atomSet[zatom]->degree;
    if (neigh == 1) {
      rot[0] = 0.0;
      return rot;
    }  /* if (neigh == 1) */
    /*---------------------------------------------*/
    /* Look for atom that is already positioned    */
    /*---------------------------------------------*/
    kk = 0;
    for (i = 0; i < neigh; i++) {
      if (atomSet[atomSet[zatom]->intnodeTable[i]]->status != 0) {
	hatom = i;
	kk ++;
      }
    }
    if (kk == neigh) return rot;
    sum = 0.0;
    /*-----------------------------------*/
    /*   weights for n-substituted atoms */
    /*-----------------------------------*/
    for (i = 0; i < neigh; i++) {
      nclass[i] = sqrt((double)atomSet[atomSet[zatom]->intnodeTable[i]]->degree);
      /*------------------------------------------------------------*/
      /*   if neighbor is part of ring  -> class[] = 100000.        */
      /*------------------------------------------------------------*/
      if ((rili[i + 1] != 0) && (rili[i + 1] != 100))
	nclass[i] = 100000;
      /*------------------------------------------------------------*/
      /*  if neighbor is part of new ring   -> class[] = 500000.    */
      /*  ( makes, that rings that are connected via one atom are   */
      /*    not draw close to each other )                          */
      /*------------------------------------------------------------*/
      if (rili[i + 1] == 100) nclass[i] = nclass[i] + 500000.;
      /*------------------------------------------------------------*/
      /*      if vmit = true  -> Substitution of a ring             */
      /*------------------------------------------------------------*/
      if ((vmit) && (rili[i + 1] != 0) && (rili[i + 1] != 100))
	nclass[i] = -nclass[i];
    }  /* for (i = 0; i < neigh; i++) */
    neigh = resort(nclass, neigh, zatom, hatom, rili);
    ll = neigh + 1;
    for (i = 0; i < ll; i++) {
      if (nclass[i] > 500000.0) nclass[i] -= 500000.0;
      sum = sum + nclass[i];
    } /* for (i = 0; i < ll; i++) */
    if (rili[0] == 0) {
      /*------------------------------------------------------------*/
      /*                         No Ring                            */
      /*  To avoid formation of spirals we resort                   */
      /*  the bond lists for every second node in the chain.        */
      /*------------------------------------------------------------*/
      if (!wahl) {
	wahl = true;
	for (i = 0; i < neigh; i++) {
	  i1[i] = atomSet[zatom]->intnodeTable[i];
	  i2[i] = atomSet[zatom]->bondTable[i];
	  i3[i] = nclass[i];
	}
	kk = neigh + 1;
	for (i = 0; i < neigh; i++) {
	  kk = kk - 1;
	  atomSet[zatom]->intnodeTable[i] = i1[kk];
	  atomSet[zatom]->bondTable[i] = i2[kk];
	  nclass[i] = i3[kk];
	}  /* for (i = 0; i < neigh; i++) */
      }
      else wahl = false;  /* if (!wahl) */
      /*--------------------------------*/
      /*  Load angles into rot          */
      /*--------------------------------*/
      phi = 360. / sum;
      alpha = 180.0 - nclass[neigh] * phi / 2.0;
      for (i = 0; i < neigh; i++) {
	rot[i] = alpha - nclass[i] * phi / 2.0;
	alpha = alpha - nclass[i] * phi;
      }  /* for (i = 0; i < neigh; i++)  */
      rot[neigh ] = 180.0;
      return rot;
    }  /* if (rili[0] == 0) */
    /*-----------------------------------------------------------*/
    /*                 'zatom' is part of a ring                 */
    /*-----------------------------------------------------------*/
    sum = 0.0;
    ll = neigh + 1;
    if (vmit) ll = neigh;
    for (i = 0; i < ll; i++)
      if (fabs(nclass[i]) != 100000.0) sum = sum + nclass[i];
    phi = 180.0 / sum;
    if (!vmit) {
      /*------------------------------------------------------------*/
      /*           The ring has not yet been drawn, i.e.            */
      /*           substitution type  Atom -> Ring                  */
      /*------------------------------------------------------------*/
      fewi = 180.0 / rili[0];
      alpha = 180.0 - nclass[ll-1] * phi / 2.0;
      for (i = 0; i < neigh; i++) {
	if (rili[i + 1] == 0) {
	  rot[i] = alpha - nclass[i] * phi / 2.0;
	  alpha = alpha - nclass[i] * phi;
	}
	else {
	  rot[i] = alpha - fewi;
	  alpha = alpha - 90.0;
	  fewi = 90.0 - fewi;
	}
      }
      return rot;
    }
    /*---------------------------------------------------------------*/
    /*                   ring substitution                   */
    /*---------------------------------------------------------------*/
    alpha = 90.0;
    for (i = 1; i < neigh; i++) {
      rot[i] = alpha - nclass[i] * phi / 2.0;
      alpha = alpha - nclass[i] * phi;
    }

    return rot;
  }

  /** Takes care of positioning substituents at bridgeheads
      'bridgeAtomNumber' is the number of the bridgeAtom to handle,
      'x', 'y', 'xh', 'yh' are the coordinates of the two bridgeHeadAtoms,
      'ringSize1' and 'ringSize2' are the ring sizes of the two adjecent
      rings.
  */
  void handleBridgeHeads(int bridgeAtomNumber, double x, double y,
			 double xh, double yh, int ringSize1, int ringSize2){
    int    i, ll, iz;
    double xr = 0, yr = 0, an = 0, phi = 0, xx = 0, yy = 0, pp = 0;
    SPoint tempPoint;

    ll = atomSet[bridgeAtomNumber]->degree;
    if (ll <= 3) {
      atomSet[bridgeAtomNumber]->status = 2;
      return;
    }
    xr = x - xh;
    yr = y - yh;
    an = sqrt(xr * xr + yr * yr);
    iz = 0;
    /*-------------------------------------------------------*/
    /*   get the remaining angle between the two rings       */
    /*-------------------------------------------------------*/
    /* Steinbeck says: strange formula... any suggestions for
       something better...

		*/
    phi = 360.0 - (360.0 / ringSize1) - (360.0 / ringSize2);
    /*----------------------------------------------------*/
    /*   loop through all the neighbors                   */
    /*----------------------------------------------------*/
    for (i = 0; i < ll; i++) {
      if ((atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->status < 1) &&
	  (atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->ringCounter == 0)){

				/*---------------------------------------------------------*/
	 			/*          Positioning of first neighbor                  */
	 			/*---------------------------------------------------------*/
	xx = x + bondLength * xr / an;
	yy = y + bondLength * yr / an;
	iz = iz + 1;
	switch (ll) {
	case 1:
	case 2:
	case 3:
	case 4:
	  atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->x = xx;
	  atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->y = yy;
	  break;
	case 5:
	  if (iz == 1) pp = phi / 3.0;
	  if (iz == 2) pp = -phi/ 3.0;
	  /*----------------------------------------------*/
	  /* rotate substituents by angle pp              */
	  /* if more than one neighbor has to be drawn    */
	  /*----------------------------------------------*/
	  //qDebug() << "Invoking rot-Atom() from line 1317" ;
	  tempPoint = rotAtom(SPoint(x, y), SPoint(xx, yy), pp);
	  atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->x = tempPoint.x;
	  atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->y = tempPoint.y;
	  /*  case 5:  */
	  break;
	case 6:
	  if (iz == 1) {
	    atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->x = xx;
	    atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->y = yy;
	  } /*  if (iz == 1)  */
	  if (iz == 2) pp = phi / 4.0;
	  if (iz == 3) pp = -phi/ 4.0;
	  if ((iz == 2) || (iz == 3)){
	    /*----------------------------------------------*/
	    /* rotate substituents by angle pp              */
	    /* if more than one neighbor has to be drawn    */
	    /*----------------------------------------------*/
	    //qDebug() << "Invoking rot-Atom() from line 1335" ;
	    tempPoint = rotAtom(SPoint(x, y), SPoint(xx, yy), pp);
	    atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->x = tempPoint.x;
	    atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->y = tempPoint.y;
	  }
	  break;
	  /*  case 6:  */
	}  /* switch (ll) */
	atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->status = 1;
	if (atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->degree == 1){
	  atomSet[atomSet[bridgeAtomNumber]->intnodeTable[i]]->status = 2;
	}
      }
    }
    atomSet[bridgeAtomNumber]->status = 2;
  }
  /** This routine handles ring substituents which are not rings themselves
	    and which are not located at bridgeheads */
  int handleRestOfRingSubstituents(int thisRingNumber){
//    double ph = 0.0174532925;
    int    i, j, k, iz, ir, igr, /*id1, endlos,*/ l, jj/*, nanz, ih*/;
    int    /*ik, iph, ibb,*/ ll, /*km, mm,*/ result = -1;
//    int    nn, ia, ib, ii, il, kmm, iz1, iz2, isum1, isum2;
//    double sux, suy, r, rad1, rad2;
//    double alph, rad, phi, b, dist, xx, yy, r1, r2, dd;
    QVector<int> ilist(10);
    bool  vmit/*, ums, okay*/;
    Ring *ring = sssr.at(thisRingNumber);
    iz = 0;
    ir=  0;
    igr=0;
    if (thisRingNumber == 7){
      ir = ir;
    }

    k = ring->size();
    for (i = 0; i < k; i++){
      if (atomSet[ring->getNodeNumber(i)]->status != 2){
	vmit = true;
				/*---------------------------------------------------------*/
	 			/*  load 'ilist' with infos for angleOpt                   */
	 			/*---------------------------------------------------------*/
	for (j = 0; j < 10; j++) ilist[j] = 0;
	l = atomSet[ring->getNodeNumber(i)]->degree;
	if (l != 0){
	  ilist[0] = k;
	  for (j = 0; j < l; j++){
	    ll = atomSet[atomSet[ring->getNodeNumber(i)]->intnodeTable[j]]->ringCounter;
	    if (ll != 0){
	      jj = 0;
	      do {
		jj = jj + 1;
		if (atomSet[atomSet[ring->getNodeNumber(i)]->intnodeTable[j]]->inRings[jj-1]== thisRingNumber)
		  ilist[j + 1] = k;
	      }  while (!(jj >= ll)&& (!(atomSet[atomSet[ring->getNodeNumber(i)]->intnodeTable[j]]->inRings[jj-1]== thisRingNumber)));
	    }
	  }
	  result = positionAtomSubstituents(ring->getNodeNumber(i), SPoint(atomSet[ring->getNodeNumber(i)]->x, atomSet[ring->getNodeNumber(i)]->y),
					    SPoint(ring->x, ring->y), ilist, ring->size(), ir, igr);
	}
      }
    }
    ring->status = 2; /* Ring positioniert */
    return result;
  }

  /** positions substituent at 'thisAtom'.
	    Originally called sublin(ita) by Dengler
	    */
  int positionAtom(int thisAtom){
//    double ph = 0.0174532925;
    int i = 0, id = 0, igr = 0, ihil = 0, ir = 0, iz = 0;
    int	k = 0, kk = 0, l = 0, ll = 0, modus = 0, result = -1;
    QVector<int> remem(10);
    QVector<int> ilist(10);
    Ring *ring = new Ring;
    k = atomSet[thisAtom]->degree;
    if (k == 0) {
      atomSet[thisAtom]->status = 2;
      return result;
    }
    if (atomSet[thisAtom]->ringCounter == 0) {
      /*-----------------------------------------------------*/
      /*             'thisAtom' is no ringAtom                */
      /*-----------------------------------------------------*/
      iz = 0;
      /*----------------------------------------------------*/
      /*            Check neighbors of 'thisAtom            */
      /*----------------------------------------------------*/
      for (i = 0; i < k; i++) {
	if (atomSet[atomSet[thisAtom]->intnodeTable[i]]->status > 0) {
	  iz ++;
	  ihil = atomSet[thisAtom]->intnodeTable[i];
	}
      }
      if (iz == k) {
	 			/*-----------------------------------------------*/
	 			/*    All neighbors are already positioned       */
	 			/*-----------------------------------------------*/
	atomSet[thisAtom]->status = 2;
	return result;
      }  /* if (iz == k) */
      /*-----------------------------------------------------*/
      /*  Assumption is that one atom has been positioned    */
      /*  This atom is set to 100 in ilist                   */
      /*-----------------------------------------------------*/
      for (i = 0; i < 7; i++)
	ilist[i] = 0;
      for (i = 0; i < k; i++)
	if (atomSet[atomSet[thisAtom]->intnodeTable[i]]->status > 0) ilist[i + 1] = 100;
      modus = 0;

      result = positionAtomSubstituents(thisAtom, SPoint(atomSet[thisAtom]->x, atomSet[thisAtom]->y),
					SPoint(atomSet[ihil]->x, atomSet[ihil]->y), ilist, modus, ir, igr);

      return result;
    }
    /*------------------------------------------*/
    /*    Atom is ringatom                      */
    /*------------------------------------------*/
    iz = 0;
    /*---------------------------------------------------------------*/
    /*  find neighbors of 'thisAtom' that are also part of a ring    */
    /*---------------------------------------------------------------*/
    for (i = 0; i < k; i++) {
      if ((atomSet[atomSet[thisAtom]->intnodeTable[i]]->status < 1) &&
	  (atomSet[atomSet[thisAtom]->intnodeTable[i]]->ringCounter != 0)){
	iz = iz + 1;
	remem[iz] = atomSet[thisAtom]->intnodeTable[i];
      }
    }
    /*---------------------------------------------------------------*/
    /*  find the ring to which the atoms stored in 'remem' belong    */
    /* --------------------------------------------------------------*/
    ir = -1;
    igr = 30;
    for (i = 0; i < sssr.size(); i++) {
      ring = sssr.at(i);
      if (ring->status < 1) {
	ll = ring->size();
	for (l = 0; l < ll; l++) {
	  if (ring->getNodeNumber(l) == thisAtom) {
	    id = 0;
	    for (kk = 0; kk < ll; kk++) {
	      if ((ring->getNodeNumber(kk) == remem[1])||(ring->getNodeNumber(kk) == remem[2]))
		id = id + 1;
	    } /*  for (kk = 0; kk < ll; kk++) */
	    if ((id != 2) || (ll > igr)) break;
	    igr = ll;
	    ir = i;
	    break;
	  }
	}
      }
    }
    if (ir == -1) {
      /*------------------------------------------------------------*/
      /*                 Ring nicht gefunden                        */
      /*------------------------------------------------------------*/
      atomSet[thisAtom]->status = 2;
      return result;
    }
    /*---------------------------------------------------------------*/
    /* Positioning of neighbors with 'subat' 						   */
    /* 'ilist' contains ring sizes for ring-member-atoms             */
    /*---------------------------------------------------------------*/
    for (i = 0; i < k; i++)
      if (atomSet[atomSet[thisAtom]->intnodeTable[i]]->status !=0)
	id = atomSet[thisAtom]->intnodeTable[i];
    ilist[0] = igr;
    for (i = 1; i < 7; i++)
      ilist[i] = 0;
    for (i = 0; i < k; i++){
      if ((remem[1] == atomSet[thisAtom]->intnodeTable[i]) ||
	  (remem[2] == atomSet[thisAtom]->intnodeTable[i])){
	ilist[i + 1] = igr;
      }
    }
    modus = 1;
    result = positionAtomSubstituents(thisAtom, SPoint(atomSet[thisAtom]->x, atomSet[thisAtom]->y),
				      SPoint(atomSet[id]->x, atomSet[id]->y), ilist, modus, ir, igr);
    return result;
  }

  double sqr(double b){
    return b*b;
  }

  /** This method resorts the node table of atom 'zatom' so that the most complex
      partner is located in the center of nodetable and the least complex partners
      are located at the beginning and the end of the nodetable.  */
  int resort(QVector<double> nclass, int neigh, int zatom, int hatom,
	     QVector<int> rili){
    int    i, j, k, kzaehl, kl;
    double min;

    QVector<int> sbinex(10);
    QVector<int> sbiord(10);
    QVector<int> srili(10);
    QVector<double> sclass(10);
    k = 0;
    for (i = 0; i < neigh; i++){
      if (i != hatom)	{
	sclass[k] = nclass[i];
	sbinex[k] = atomSet[zatom]->intnodeTable[i];
	sbiord[k] = atomSet[zatom]->bondTable[i];
	srili[k] = rili[i + 1];
	k++;
      }
    }
    nclass[neigh - 1] = nclass[hatom];
    atomSet[zatom]->intnodeTable[neigh-1] = atomSet[zatom]->intnodeTable[hatom];
    atomSet[zatom]->bondTable[neigh-1] = atomSet[zatom]->bondTable[hatom];
    rili[neigh] = rili[hatom + 1];
    neigh --;
    kzaehl = 0;
    for (i = 0; i < neigh; i++){
      min = sclass[0];
      kl = 0;
      for (j = 0; j < neigh; j++){
	if (sclass[j] < min){
	  min = sclass[j];
	  kl = j;
	}
      }
      sclass[kl] = 1.0e30;
      if ((i+1) % 2 != 0){
	nclass[kzaehl] = min;
	atomSet[zatom]->intnodeTable[kzaehl] = sbinex[kl];
	atomSet[zatom]->bondTable[kzaehl] = sbiord[kl];
	rili[kzaehl + 1] = srili[kl];
      }
      else{
	nclass[neigh - kzaehl - 1] = min;
	atomSet[zatom]->intnodeTable[neigh - kzaehl - 1] = sbinex[kl];
	atomSet[zatom]->bondTable[neigh - kzaehl -1 ] = sbiord[kl];
	rili[neigh - kzaehl] = srili[kl];
	kzaehl = kzaehl + 1;
      }
    }
    return neigh;
  }

  void sortBondList(){
    bool somethingDone;
    int bond, type;
    for (int e = 0; e < atomSet.size(); e++){
      do{
	somethingDone = false;
	for (int f = 0; f < atomSet[e]->degree - 1; f++){
	  if (atomSet[e]->intnodeTable[f] > atomSet[e]->intnodeTable[f + 1]){
	    bond = atomSet[e]->intnodeTable[f];
	    type = atomSet[e]->bondTable[f];
	    atomSet[e]->intnodeTable[f] = atomSet[e]->intnodeTable[f + 1];
	    atomSet[e]->bondTable[f] = atomSet[e]->bondTable[f + 1];
	    atomSet[e]->intnodeTable[f + 1] = bond;
	    atomSet[e]->bondTable[f + 1] = type;
	    somethingDone = true;
	  }
	}
      }while(somethingDone);
    }
  }

  /*
  void setPreferredSize(Dimension d){
    preferredSize = d;
  }

  Dimension getPreferredSize(){
    return preferredSize;
  }
  */

  /** Method responsible for rendering the graphical output of structures
		Please let this be the last method in this class

  public void paint(Graphics g){
    //		super.paint(g);
    if (verbose) System.out.println("JMDraw->paint(): Drawing of structure finished.");
    Molecule molecule = new Molecule(atomSet);
    RenderingEngine strdrw = new RenderingEngine();
    RenderingEngine.scaleIntoArea(molecule, g.getClipBounds().getSize(), 0.8);
    RenderingEngine.translateAllPositive(molecule);
    RenderingEngine.center(molecule, g.getClipBounds().getSize());
    strdrw.drawMolecule(g, molecule, rA);
    if (verbose) System.out.println("JMDraw->paint(): Drawing of structure finished.");
    }*/



  /*
  public RenderingAttributes getRenderingAttributes()
    {
      return this.rA;
    }

  public void setRenderingAttributes(RenderingAttributes rA)
    {
      this.rA = rA;
    }
  */

  // copied from SetOfRingsFinder.java
  /** Return a Vector that contains the intersection of Vectors vec1
      and vec2 */

  QList<Integer *> getIntersection(QVector<int> vec1, QVector<int> vec2){
    QList<Integer *> is;
    for (int f = 0; f < vec1.size(); f++){
      if (vec2.contains(vec1.at(f)) > 0) is.append(new Integer(vec1.at(f)));
    }
    return is;
  }


}; // class SDG

#endif
