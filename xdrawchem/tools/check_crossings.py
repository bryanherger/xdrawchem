#!/usr/bin/env python3
"""Crossing audit for double-bond inner lines.

For every structure: render SVG, collect polyline segments, and report genuine
crossings — two segments that intersect at a point that is not an endpoint of
either.  Shared endpoints are normal (bonds meet at atoms); a crossing anywhere
else means a second line is drawn through another bond.
"""
import re, subprocess, sys, os, tempfile

XDC = "/home/bryan/xdrawchem/build/xdrawchem"


def segments(smiles):
    with tempfile.NamedTemporaryFile(suffix=".svg", delete=False) as tmp:
        path = tmp.name
    subprocess.run(
        [XDC, "--api", "render", smiles, "--input_format", "smiles", "--out", path],
        capture_output=True, text=True,
        env={**os.environ, "QT_QPA_PLATFORM": "offscreen"},
    )
    try:
        svg = open(path).read()
    finally:
        os.unlink(path)
    polys = [[tuple(round(float(v), 2) for v in q.split(',')) for q in p.split()]
             for p in re.findall(r'<polyline[^>]*points="([^"]+)"', svg)]
    return [(p[0], p[1]) for p in polys if len(p) == 2]


def orient(a, b, c):
    return (b[0] - a[0]) * (c[1] - a[1]) - (b[1] - a[1]) * (c[0] - a[0])


def on_seg(a, b, p, eps=0.4):
    return (abs(orient(a, b, p)) < eps
            and min(a[0], b[0]) - eps <= p[0] <= max(a[0], b[0]) + eps
            and min(a[1], b[1]) - eps <= p[1] <= max(a[1], b[1]) + eps)


def crosses(s1, s2, eps=0.6):
    a, b = s1
    c, d = s2
    # segments that merely touch at a shared endpoint are fine
    for p in (a, b):
        for q in (c, d):
            if abs(p[0] - q[0]) < eps and abs(p[1] - q[1]) < eps:
                return False
    d1, d2 = orient(a, b, c), orient(a, b, d)
    d3, d4 = orient(c, d, a), orient(c, d, b)
    return (d1 * d2 < 0) and (d3 * d4 < 0)


STRUCTURES = [
    ("benzene", "c1ccccc1"),
    ("pyridine", "c1ccncc1"),
    ("naphthalene", "c1ccc2ccccc2c1"),
    ("indole", "c1ccc2[nH]ccc2c1"),
    ("quinoline", "c1ccc2ncccc2c1"),
    ("anthracene", "c1ccc2cc3ccccc3cc2c1"),
    ("benzofuran", "c1ccc2occc2c1"),
    ("aspirin", "CC(=O)Oc1ccccc1C(=O)O"),
]

bad = 0
for name, smi in STRUCTURES:
    segs = segments(smi)
    hits = []
    for i in range(len(segs)):
        for j in range(i + 1, len(segs)):
            if crosses(segs[i], segs[j]):
                hits.append((segs[i], segs[j]))
    status = "OK  " if not hits else "FAIL"
    if hits:
        bad += 1
    print(f"{status} {name:12s} segments={len(segs):2d} crossings={len(hits)}")
    for h in hits:
        print(f"       {h[0]} x {h[1]}")

print("\nstructures with crossings:", bad)
sys.exit(1 if bad else 0)
