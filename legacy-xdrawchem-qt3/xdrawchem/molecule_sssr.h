// molecule_sssr.cpp - defines class SSSR, which implements ring detection by
// breadth-first traversal.  This results in the Smallest Set of Smallest
// Rings.  This algorithm is based on J. Figueras' published method,
// "Ring Perception Using Breadth-First Search",
// J. Chem. Inf. Comput. Sci. 1996, 36, 986-991

#ifndef MOL_SSSR_H
#define MOL_SSSR_H

#include <qptrlist.h>
#include "dpoint.h"

class RingData {
 public:
  RingData() { neighbors = 0; bridge = false; }
  int neighbors;  // number of neighbor rings
  bool bridge;    // bridged ring? (e.g. camphor)
};

class SSSR {
public:
  QPtrList<QPtrList<DPoint> > sssr;
  QPtrList<RingData> sssr_data;
  QPtrList<DPoint> *tmp_ring;
  RingData *tmp_ringdata;
  DPoint *tmp_pt;
  DPoint *lastnode;  // 'source' node in BFS
  // Build SSSR from these lists
  QPtrList<DPoint> structureAtoms;
  // BFS queue
  QPtrList<DPoint> bfs_queue;

  int IsInRing(DPoint *);
  int InSameRing(DPoint *, DPoint *, DPoint *);
  void PrintSSSR();
  void FindAromatic(QPtrList<Bond>);
  void BuildSSSR(QPtrList<DPoint>);
  void Add(QPtrList<DPoint> *);
  int CommonPoints(QPtrList<DPoint> *, QPtrList<DPoint> *);
  bool CheckRing(QPtrList<DPoint> *);
  QPtrList<DPoint> * GetRing(DPoint *);
  void ClearPaths(void);
};

#endif
