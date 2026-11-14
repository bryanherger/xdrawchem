#include "sdg_coordgen.h"

#include "atom.h"

#include <openbabel/elements.h>

#include <QHash>

#include <cmath>

#include "sketcherMinimizer.h"
#include "sketcherMinimizerAtom.h"
#include "sketcherMinimizerBond.h"
#include "sketcherMinimizerMolecule.h"

namespace {

int atomicNumber( const QString &symbol )
{
    const QByteArray s = symbol.trimmed().toLatin1();
    if ( s.isEmpty() ) return 6;
    const unsigned int z = OpenBabel::OBElements::GetAtomicNum( s.constData() );
    return z > 0 ? static_cast<int>( z ) : 6;
}

// Bond orders 5 and 7 are XDrawChem's up/down stereo markers, not real order 5
// and 7 bonds; CoordGen only lays out the skeleton, so they collapse to single.
int layoutBondOrder( int order )
{
    return ( order >= 1 && order <= 3 ) ? order : 1;
}

} // namespace

bool SDGCoordgen( QVector<Atom *> &atoms, double bondLength )
{
    if ( atoms.isEmpty() ) return false;

    QHash<Atom *, int> index;
    index.reserve( atoms.size() );
    for ( int i = 0; i < atoms.size(); i++ )
        index.insert( atoms.at( i ), i );

    auto *mol = new sketcherMinimizerMolecule();
    QVector<sketcherMinimizerAtom *> cg( atoms.size() );
    for ( int i = 0; i < atoms.size(); i++ ) {
        auto *a = mol->addNewAtom();
        a->setAtomicNumber( atomicNumber( atoms.at( i )->symbol ) );
        cg[i] = a;
    }

    for ( int i = 0; i < atoms.size(); i++ ) {
        Atom *a = atoms.at( i );
        const int degree = qMin( a->degree, static_cast<int>( a->nodeTable.size() ) );
        for ( int k = 0; k < degree; k++ ) {
            Atom *nb = a->nodeTable.at( k );
            if ( nb == nullptr ) continue;
            auto it = index.constFind( nb );
            if ( it == index.constEnd() ) continue;
            const int j = it.value();
            if ( j <= i ) continue;  // each bond is listed at both ends

            const int order = k < a->bondTable.size() ? a->bondTable.at( k ) : 1;
            auto *b = mol->addNewBond( cg.at( i ), cg.at( j ) );
            b->setBondOrder( layoutBondOrder( order ) );
        }
    }

    // Ring perception is the caller's job in CoordGen's API; without it every
    // ring is laid out as an open chain and closed afterwards, which collapses
    // fused systems.
    sketcherMinimizerMolecule::forceUpdateStruct( mol->getAtoms(), mol->getBonds(),
                                                  mol->getRings() );

    sketcherMinimizer minimizer;
    minimizer.initialize( mol );
    minimizer.runGenerateCoordinates();

    // Scale so the mean bond length matches the user's bond length setting.
    double sum = 0.0;
    int counted = 0;
    for ( auto *b : mol->getBonds() ) {
        const auto p1 = b->getStartAtom()->getCoordinates();
        const auto p2 = b->getEndAtom()->getCoordinates();
        sum += std::hypot( p1.x() - p2.x(), p1.y() - p2.y() );
        counted++;
    }
    const double scale = ( counted > 0 && sum > 0.0 && bondLength > 0.0 )
                             ? bondLength / ( sum / counted )
                             : 1.0;

    bool ok = true;
    for ( int i = 0; i < atoms.size(); i++ ) {
        const auto c = cg.at( i )->getCoordinates();
        if ( !std::isfinite( c.x() ) || !std::isfinite( c.y() ) ) {
            ok = false;
            break;
        }
        atoms.at( i )->x = c.x() * scale;
        atoms.at( i )->y = c.y() * scale;
    }

    // Ownership: sketcherMinimizer::initialize() splits the input into
    // fragment molecules and moves atoms and bonds between them, so freeing
    // either side by hand double-frees.  Upstream's own example leaks the
    // graph deliberately; a layout is a few hundred bytes and XDrawChem
    // generates coordinates per structure, not in bulk, so follow that.
    return ok;
}
