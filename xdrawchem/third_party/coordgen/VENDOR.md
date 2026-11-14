# CoordGen — 2D coordinate generation (vendored)

Upstream: https://github.com/schrodinger/coordgenlibs
Commit:   d20e735d96480385b2e257522288004038a08cc9 (v3.0.2)
Licence:  BSD-3-Clause — see LICENSE (Copyright (c) 2017, Schrödinger, Inc.)

Vendored with two local patches, both marked `Local patch:` in place:
`CoordgenMacrocycleBuilder.cpp` and `sketcherMinimizerMarchingSquares.cpp`
multiply as `size_t` before resizing a vector, which silences two CodeQL
integer-overflow alerts.  Behaviour is unchanged; re-apply on update.

Otherwise unmodified. Layout only: the renderer, example, tests and the optional
maeparser/template-loading path are not built, so no Boost dependency is needed.

Used by `xdrawchem/sdg_coordgen.cpp` as the structure-diagram generator.
