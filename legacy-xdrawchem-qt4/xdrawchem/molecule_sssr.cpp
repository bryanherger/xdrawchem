// molecule_sssr.cpp - defines class SSSR, which implements ring detection by
// breadth-first traversal.  This results in the Smallest Set of Smallest
// Rings.  This algorithm is based on J. Figueras' published method,
// "Ring Perception Using Breadth-First Search",
// J. Chem. Inf. Comput. Sci. 1996, 36, 986-991

/*
//Added by qt3to4:
#include <Q3PtrList>
*/

#include "bond.h"
#include "molecule_sssr.h"

// Is this DPoint in a ring?
int SSSR::IsInRing( DPoint * r1 )
{
    int retval = 0;

    if ( sssr.count() == 0 ) {
        return 0;
    }
    foreach ( tmp_ring, sssr ) {
        if ( tmp_ring->contains( r1 ) > 0 )
            retval++;
    }

    return retval;
}

// are these three points in the same ring?  If so, return the number of
// atoms in this ring
int SSSR::InSameRing( DPoint * r1, DPoint * r2, DPoint * r3 )
{
    int retval;

    foreach ( tmp_ring, sssr ) {
        retval = 0;
        if ( tmp_ring->contains( r1 ) > 0 )
            retval++;
        if ( tmp_ring->contains( r2 ) > 0 )
            retval++;
        if ( tmp_ring->contains( r3 ) > 0 )
            retval++;
        if ( retval == 3 ) {
            return tmp_ring->count();
        }
    }

    return 0;
}

  // print SSSR as [xyz]
void SSSR::PrintSSSR()
{
    if ( sssr.count() == 0 ) {
        qDebug() << "No rings.";
        return;
    }
    qDebug() << "[";
    foreach ( tmp_ring, sssr )
        qDebug() << tmp_ring->count();
    qDebug() << "]";
}

  // identify aromatic rings (just benzene for now)
void SSSR::FindAromatic( QList < Bond * >bl )
{
    Bond *tmp_bond;
    bool o1, o2, goodring;

    foreach ( tmp_ring, sssr ) {
        goodring = true;
        if ( tmp_ring->count() != 6 ) {
            goodring = false;
            continue;
        }
        foreach ( tmp_pt, *tmp_ring ) {
            o1 = false;
            o2 = false;
            foreach ( tmp_bond, bl ) {
                if ( tmp_bond->Find( tmp_pt ) == true ) {
                    if ( tmp_bond->Order() == 1 )
                        o1 = true;
                    if ( tmp_bond->Order() == 2 )
                        o2 = true;
                }
            }
            if ( ( o1 == false ) || ( o2 == false ) ) {
                goodring = false;
                break;
            }
        }
        if ( goodring == true ) {
            qDebug() << "Aromatic ring";
            foreach ( tmp_pt, *tmp_ring ) {
                tmp_pt->aromatic = true;
            }
        }
    }
}

  // Make sssr from given 'atoms'
void SSSR::BuildSSSR( QList < DPoint * >alist )
{
    // clear old SSSR
    sssr.clear();
    sssr_data.clear();

    int atomsRemoved;

    structureAtoms = alist;
    // eliminate pesky chains by cutting off zero- and one-bond 'atoms'
    do {
        atomsRemoved = 0;
        foreach ( tmp_pt, structureAtoms ) {
            if ( tmp_pt->neighbors.count() < 2 ) {
                qDebug() << tmp_pt->serial << ":" << tmp_pt->neighbors.count();
                // increment atomsRemoved
                atomsRemoved++;
                // remove this item from structureAtoms
                structureAtoms.removeAll( tmp_pt );
                // and remove this item from all neighbor lists
                foreach ( DPoint * tmp_del, structureAtoms ) {
                    tmp_del->neighbors.removeAll( tmp_pt );
                }
            }
        }
    } while ( atomsRemoved > 0 );
    qDebug() << "There are " << structureAtoms.count() << " ring atoms";
    // now traverse rings
    foreach ( DPoint * tmp_it, structureAtoms ) {
        tmp_ring = GetRing( tmp_it );
        if ( tmp_ring != 0 )
            Add( tmp_ring );
    }
    // find fused and bridged rings
    if ( sssr.count() > 1 ) {
        int neighbors = 0, int1;
        bool bridged = false;

        foreach ( QList < DPoint * >*tmp_sssr1, sssr ) {
            neighbors = 0;
            bridged = false;
            foreach ( QList < DPoint * >*tmp_sssr2, sssr ) {
                if ( tmp_sssr1 == tmp_sssr2 )
                    continue;
                int1 = CommonPoints( tmp_sssr1, tmp_sssr2 );
                if ( int1 > 1 )
                    neighbors++;
                if ( int1 > 2 )
                    bridged = true;
            }
            qDebug() << neighbors << "/" << bridged;
        }
    }
}

  // Add to sssr.  Make sure this is not a duplicate of an existing ring
void SSSR::Add( QList < DPoint * >*r1 )
{
    DPoint *flagpt;

    if ( CheckRing( r1 ) == true ) {
        sssr.append( r1 );
        // mark this dpoint as in a ring
        foreach ( flagpt, *r1 ) {
            flagpt->inring = true;
            qDebug() << "flagged";
        }
    }
}

  // number of points in common between two rings
int SSSR::CommonPoints( QList < DPoint * >*r1, QList < DPoint * >*r2 )
{
    int ol = 0;

    foreach ( DPoint * tmp_r1, *r1 ) {
        if ( r2->count( tmp_r1 ) >= 0 )
            ol++;
    }
    return ol;
}

  // check if ring is already in SSSR
bool SSSR::CheckRing( QList < DPoint * >*r )
{
    int l2;
    bool iflag = true;

    foreach ( tmp_ring, sssr ) {
        l2 = r->count();
        foreach ( tmp_pt, *tmp_ring ) {
            if ( r->count( tmp_pt ) >= 0 )
                l2--;
        }
        if ( l2 <= 0 ) {
            iflag = false;
            break;
        }
    }
    return iflag;
}

  // Find smallest unique ring containing root node *root
  // Based on Figueras' breadth-first traversal
QList < DPoint * >*SSSR::GetRing( DPoint * root )
{
    QList < DPoint * >*testring = new QList < DPoint * >;
    DPoint *thisnode;
    int tf = 0;

    ClearPaths();
    bfs_queue.clear();
    root->source = 0;
    bfs_queue.append( root );
    do {
        thisnode = bfs_queue.first();
        bfs_queue.removeFirst();
        // skip eliminated atoms
        if ( structureAtoms.contains( thisnode ) )
            continue;
        foreach ( tmp_pt, thisnode->neighbors ) {
            if ( thisnode->source == tmp_pt )
                continue;       // prevent backtrack
            if ( structureAtoms.indexOf( tmp_pt ) < 0 )
                continue;       // skip 'eliminated'
            if ( tmp_pt->path.count() == 0 ) {
                tmp_pt->source = thisnode;
                tmp_pt->path = thisnode->path;
                tmp_pt->path.append( thisnode );
                bfs_queue.append( tmp_pt );
            } else {            // collision
                qDebug() << "collide" << endl << "thisnode(" << thisnode->serial << "):";
                // merge path lists (note overlaps)
                foreach ( DPoint * tmpPoint, thisnode->path ) {
                    qDebug() << tmpPoint->serial;
                    testring->append( tmpPoint );
                }
                qDebug() << endl << "tmp_pt(" << tmp_pt->serial << "):";
                tf = 0;
                foreach ( DPoint * tmpPoint, tmp_pt->path ) {
                    qDebug() << tmpPoint->serial;
                    if ( testring->count( tmpPoint ) < 0 )
                        testring->append( tmpPoint );
                    else
                        tf++;
                }
                qDebug();
                // but thisnode and tmp_pt have not themselves been added!
                if ( testring->count( thisnode ) < 0 )
                    testring->append( thisnode );
                else
                    tf++;
                if ( testring->count( tmp_pt ) < 0 )
                    testring->append( tmp_pt );
                else
                    tf++;
                // check for singleton overlap (tf == 1)
                if ( tf == 1 )
                    return testring;
                else
                    testring->clear();
            }
        }
    } while ( bfs_queue.count() > 0 );
    return testring;            // to appease g++ -Wall.  Probably returns before here.
}

  // clear paths in 'atoms'
void SSSR::ClearPaths()
{
    foreach ( tmp_pt, structureAtoms ) {
        tmp_pt->source = 0;
        tmp_pt->path.clear();
    }
}





// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
