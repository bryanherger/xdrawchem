# Third-party notices

XDrawChem's own source is BSD 3-Clause (see [LICENSE](LICENSE)). The components
below carry other terms and are **not** relicensed by that file. Their notices
must be preserved in source and binary distributions.

## Source files with their own licence

| Files | Licence | Copyright |
|---|---|---|
| `xdrawchem/xdrawchem/ring.h`, `setofrings.h`, `sorf.h` (and `sdg.h` until it is removed) | LGPL-2.1 (JChemPaint) | © 1997–2000 The JChemPaint project / Dr. Christoph Steinbeck |
| `xdrawchem/xdrawchem/ioiface.cpp`, `ioiface.h`, `crings_dialog.cpp`, `crings_dialog.h` | GPL-2.0-or-later | © 2002 Adam Tenderholt |

Those files keep their original headers. Full texts: `LICENSES/GPL-2.0.txt` and
`LICENSES/LGPL-2.1.txt`. Packaged builds carry `xdrawchem/GPL.txt` and
`xdrawchem/LICENSES/LGPL-2.1.txt` for the same purpose.

## Bundled source

| Component | Licence | Location |
|---|---|---|
| CoordGen (Schrödinger) | BSD-3-Clause | `xdrawchem/third_party/coordgen` |

## Linked libraries

| Component | Licence | Notes |
|---|---|---|
| OpenBabel 3.x | **GPL-2.0** | linked into the application |
| Qt 6 | LGPL-3.0 | dynamically linked |

## What this means for binaries

OpenBabel is GPL-2.0 and is linked into `xdrawchem`. A binary that links it is a
combined work governed by the GPL, so **distributed binaries remain GPL-2.0**
even though this project's own code is BSD-3-Clause. BSD-3-Clause is compatible
with the GPL, so shipping under both is fine — the BSD grant simply also lets the
XDrawChem code be reused elsewhere without GPL terms attached.

Removing the GPL from the shipped binary would require replacing OpenBabel with a
permissive toolkit and rewriting the GPL-2.0 source files listed above.
