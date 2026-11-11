// molecule_cip_priority.cpp — CIP priority ranking for stereocentre ligands
//
// Why this exists
// ---------------
// Molecule::CalcCIPLabels() has to decide R/S, which requires the four
// substituents of a stereocentre ranked by CIP priority.  Open Babel 3.1.1
// ships no R/S generator (no CIP symbol in libopenbabel, no CIP descriptor or
// op in obabel), so the ranking cannot be borrowed and is implemented here.
//
// Earlier attempts read only OpenBabel's `winding`, or only the 2-D
// coordinates, and got the same letter for both enantiomers — because the
// (S) and (R) files have identical atom positions and differ only in which
// bond is drawn as a wedge.  This file supplies only the *ranking*; combining
// it with the winding/permutation is done in CalcCIPLabels().
//
// Scope and limits — see doc/STEREOCHEMISTRY.md
// --------------------------------------------
// Implements the hierarchy rule only, expanded to a bounded depth:
//   * higher atomic number wins;
//   * ties broken by comparing each atom's neighbours, descending, expanding
//     shells up to four bonds deep;
//   * multiple bonds duplicate their partner, so C=O outranks C-O;
//   * implicit hydrogens are ranked as real H atoms.
//
// Not implemented: stereogenic lone pairs and the pseudoasymmetric r/s rules;
// sequence-rule sub-rules 3-5 (geometric-mean / most-massive-atom criteria and
// ring rules); isotopic mass beyond atomic number.  For ordinary organics
// (C/N/O/S/halogen substituents) the hierarchy comparison agrees with full CIP.

#include <algorithm>

#include <QList>
#include <QPair>
#include <QSet>
#include <QString>

#include "molecule.h"
#include "dpoint.h"
#include "bond.h"

namespace {

// Atomic number for an XDrawChem element label.  DPoint::getAtomicNumber()
// returns 999 for anything it does not recognise, which would sort every
// unknown element above carbon; map that to 0 so unknowns rank low.
int atomicNumberOf( const QString &element )
{
    const int n = DPoint::atomicNumberFor( element );
    if ( n > 0 && n < 999 )
        return n;
    return 0;
}

// Recursive priority signature: this atom's atomic number, then its
// neighbours' signatures in descending order.  `visited` stops the walk from
// doubling back through the bond it arrived on; depth bounds ring recursion.
QString signatureOf( DPoint *atom, DPoint *from, const QList<Bond *> &bonds,
                     QSet<DPoint *> &visited, int depth )
{
    const int z = atomicNumberOf( atom->element );
    QString sig = QStringLiteral( "%1" ).arg( z, 3, 10, QChar( '0' ) );

    if ( depth <= 0 )
        return sig;

    QList<DPoint *> neighbours;
    for ( Bond *b : bonds ) {
        if ( !b->Find( atom ) )
            continue;
        DPoint *other = b->otherPoint( atom );
        if ( other == nullptr || other == from || visited.contains( other ) )
            continue;
        // A multiple bond counts its partner more than once in CIP terms.
        const int copies = qBound( 1, b->Order(), 3 );
        for ( int i = 0; i < copies; ++i )
            neighbours.append( other );
    }

    // Implicit hydrogens on a skeletal carbon are real ligands for ranking.
    if ( atom->element == QLatin1String( "C" ) ) {
        const int drawn = 0;
        Q_UNUSED( drawn );
        int connections = 0;
        for ( Bond *b : bonds ) {
            if ( !b->Find( atom ) )
                continue;
            connections += qBound( 1, b->Order(), 3 );
        }
        for ( int i = connections; i < 4; ++i )
            neighbours.append( nullptr );      // nullptr means implicit H
    }

    QList<QString> childSigs;
    for ( DPoint *nb : neighbours ) {
        if ( nb == nullptr ) {
            childSigs.append( QStringLiteral( "%1" ).arg( 1, 3, 10, QChar( '0' ) ) );
            continue;
        }
        visited.insert( nb );
        childSigs.append( signatureOf( nb, atom, bonds, visited, depth - 1 ) );
        visited.remove( nb );
    }

    std::sort( childSigs.begin(), childSigs.end(),
               []( const QString &a, const QString &b ) { return a > b; } );

    for ( const QString &c : childSigs )
        sig += QLatin1Char( '|' ) + c;
    return sig;
}

} // namespace

// Rank `refs` by descending CIP priority.  A nullptr entry means an implicit
// hydrogen and ranks as H.  The caller passes the centre separately so the
// walk cannot double back to it.
QList<QPair<DPoint *, QString>>
Molecule::cipRankLigands( DPoint *center, const QList<DPoint *> &refs ) const
{
    const QList<Bond *> allBonds = getBonds();
    QList<QPair<DPoint *, QString>> ranked;

    for ( DPoint *ligand : refs ) {
        QString sig;
        if ( ligand == nullptr ) {
            sig = QStringLiteral( "%1" ).arg( 1, 3, 10, QChar( '0' ) );
        } else {
            QSet<DPoint *> visited;
            visited.insert( center );
            visited.insert( ligand );
            sig = signatureOf( ligand, center, allBonds, visited, 4 );
        }
        ranked.append( qMakePair( ligand, sig ) );
    }

    // Descending by signature.  Equal signatures mean the ligands are
    // indistinguishable, which the caller must treat as "not a stereocentre".
    std::stable_sort( ranked.begin(), ranked.end(),
                      []( const QPair<DPoint *, QString> &a,
                          const QPair<DPoint *, QString> &b ) {
                          return a.second > b.second;
                      } );
    return ranked;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
