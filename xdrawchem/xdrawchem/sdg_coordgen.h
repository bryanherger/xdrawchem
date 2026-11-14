#ifndef SDG_COORDGEN_H
#define SDG_COORDGEN_H

#include <QVector>

class Atom;

// ── Structure diagram generation ────────────────────────────────────────────
// Replaces the JMDraw-derived layout in sdg.h with Schrödinger's CoordGen
// (vendored under third_party/coordgen, BSD-3-Clause).
//
// `atoms` must already carry connectivity in nodeTable/bondTable with `degree`
// set — the same table Molecule::SDG() builds for the old engine.  On success
// each Atom's x/y is overwritten, scaled so the mean bond length equals
// `bondLength`.  Returns false (leaving the atoms untouched) if CoordGen
// produced no usable coordinates.
bool SDGCoordgen( QVector<Atom *> &atoms, double bondLength );

#endif
