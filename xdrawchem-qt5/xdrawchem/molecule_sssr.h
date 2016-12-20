// molecule_sssr.cpp - defines class SSSR, which implements ring detection by
// breadth-first traversal.  This results in the Smallest Set of Smallest
// Rings.  This algorithm is based on J. Figueras' published method,
// "Ring Perception Using Breadth-First Search",
// J. Chem. Inf. Comput. Sci. 1996, 36, 986-991

#ifndef MOL_SSSR_H
#define MOL_SSSR_H

#include <QList>

#include "dpoint.h"

class RingData
{
public:
    RingData() { neighbors = 0; bridge = false; }
    int neighbors;  // number of neighbor rings
    bool bridge;    // bridged ring? (e.g. camphor)
};

class SSSR
{
public:
    QList<QList<DPoint *> *> sssr;
    QList<RingData *> sssr_data;
    QList<DPoint *> *tmp_ring;
    RingData *tmp_ringdata;
    DPoint *tmp_pt;
    DPoint *lastnode;  // 'source' node in BFS
    // Build SSSR from these lists
    QList<DPoint *> structureAtoms;
    // BFS queue
    QList<DPoint *> bfs_queue;

    int IsInRing(DPoint *);
    int InSameRing(DPoint *, DPoint *, DPoint *);
    void PrintSSSR();
    void FindAromatic(QList<Bond *>);
    void BuildSSSR(QList<DPoint *>);
    void Add(QList<DPoint *> *);
    int CommonPoints(QList<DPoint *> *, QList<DPoint *> *);
    bool CheckRing(QList<DPoint *> *);
    QList<DPoint *> * GetRing(DPoint *);
    void ClearPaths();
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
