#!/usr/bin/env python3
"""Measure bond angles in an XDrawChem-generated 2-D layout.

Feeds a SMILES through `xdrawchem --api clean_up --format mol` and reports, for
every atom, the angles between its bonds.  Ideal values are 120 deg for
sp2/aromatic atoms and ~109.5 deg for sp3; a fused-ring junction that looks
wrong in a render shows up here as an angle far from those.

Usage:
    python3 tools/sdg_angles.py "c1ccc2ccccc2c1" [more smiles...]

Exit code is 0 always; this is a measurement tool, not a test.
"""

from __future__ import annotations

import json
import math
import os
import shutil
import subprocess
import sys

# Prefer an explicit override, then the in-tree build, then PATH.  Resolving
# PATH first silently picks up a system-installed copy that may predate the
# agent API (and then hangs instead of answering).
_HERE = os.path.dirname(os.path.abspath(__file__))
_LOCAL_BUILD = os.path.normpath(os.path.join(_HERE, "..", "build", "xdrawchem"))

BIN = (
    os.environ.get("XDRAWCHEM_BIN")
    or (_LOCAL_BUILD if os.access(_LOCAL_BUILD, os.X_OK) else None)
    or shutil.which("xdrawchem")
    or "xdrawchem"
)


def molblock_from_smiles(smiles: str) -> str:
    """Run XDrawChem's agent API and return the laid-out molfile."""
    proc = subprocess.run(
        [BIN, "--api", "clean_up", "--structure", smiles, "--format", "mol"],
        capture_output=True,
        text=True,
        timeout=120,
    )
    payload = json.loads(proc.stdout)
    if not payload.get("ok"):
        raise SystemExit(f"xdrawchem failed for {smiles!r}: {payload.get('error')}")
    return payload["structure"]


def parse_molblock(block: str):
    """Return (atoms, bonds) where atoms are (x, y, element) and bonds pairs."""
    lines = block.splitlines()
    counts = lines[3].split()
    natoms, nbonds = int(counts[0]), int(counts[1])
    atoms = []
    for line in lines[4 : 4 + natoms]:
        x = float(line[0:10])
        y = float(line[10:20])
        element = line[31:34].strip()
        atoms.append((x, y, element))
    bonds = []
    for line in lines[4 + natoms : 4 + natoms + nbonds]:
        a = int(line[0:3]) - 1
        b = int(line[3:6]) - 1
        order = int(line[6:9])
        bonds.append((a, b, order))
    return atoms, bonds


def angle_at(atoms, bonds, center: int):
    """Angles between every pair of bonds meeting at `center`, in degrees."""
    neighbours = [b if a == center else a for a, b, _ in bonds if center in (a, b)]
    cx, cy, _ = atoms[center]
    angles = []
    for i in range(len(neighbours)):
        for j in range(i + 1, len(neighbours)):
            ax, ay, _ = atoms[neighbours[i]]
            bx, by, _ = atoms[neighbours[j]]
            v1 = (ax - cx, ay - cy)
            v2 = (bx - cx, by - cy)
            n1 = math.hypot(*v1)
            n2 = math.hypot(*v2)
            if n1 == 0 or n2 == 0:
                continue
            cos = max(-1.0, min(1.0, (v1[0] * v2[0] + v1[1] * v2[1]) / (n1 * n2)))
            angles.append(math.degrees(math.acos(cos)))
    return angles


def report(smiles: str) -> dict:
    atoms, bonds = parse_molblock(molblock_from_smiles(smiles))
    multi = [i for i in range(len(atoms)) if
             sum(1 for a, b, _ in bonds if i in (a, b)) >= 2]

    all_angles = []
    per_atom = []
    for i in multi:
        angs = angle_at(atoms, bonds, i)
        if angs:
            all_angles.extend(angs)
            per_atom.append((atoms[i][2], min(angs), max(angs)))

    # A regular polygon gives every interior angle the same value; deviation
    # from each atom's own mean is the simplest irregularity signal.
    deviations = []
    for i in multi:
        angs = angle_at(atoms, bonds, i)
        if len(angs) >= 2:
            mean = sum(angs) / len(angs)
            deviations.extend(abs(a - mean) for a in angs)

    return {
        "smiles": smiles,
        "atoms": len(atoms),
        "bonds": len(bonds),
        "angles": all_angles,
        "min": min(all_angles) if all_angles else 0.0,
        "max": max(all_angles) if all_angles else 0.0,
        "spread": (max(all_angles) - min(all_angles)) if all_angles else 0.0,
        "mean_deviation": sum(deviations) / len(deviations) if deviations else 0.0,
        "per_atom": per_atom,
    }


def main(argv: list[str]) -> int:
    targets = argv[1:] or ["c1ccc2ccccc2c1"]
    for smiles in targets:
        r = report(smiles)
        print(f"{smiles}")
        print(f"  atoms {r['atoms']}  bonds {r['bonds']}")
        print(f"  angles {r['min']:.1f} .. {r['max']:.1f}  "
              f"spread {r['spread']:.4f}  mean-dev {r['mean_deviation']:.4f}")
        print("  values: " + " ".join(f"{a:.1f}" for a in sorted(r["angles"])))
        print()
    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv))
