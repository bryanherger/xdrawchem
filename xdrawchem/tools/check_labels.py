#!/usr/bin/env python3
"""Label collision audit for structure depictions (issue #34).

Renders a structure and reports:

  * pairs of atom labels whose bounding boxes overlap, and by how much;
  * labels whose box is crossed by a bond line.

Glyph widths are estimated (the SVG carries no per-glyph metrics), so the
numbers are approximate — good enough to tell "these two labels collide" from
"these two labels are clear of each other", which is the question.

Usage:
    python3 check_labels.py "OC[C@H]1OC(O)[C@H](O)[C@@H](O)[C@@H]1O"
    python3 check_labels.py --all        # the fixture set
"""
import os
import re
import subprocess
import sys
import tempfile

XDC = os.environ.get("XDC_BIN", "/home/bryan/xdrawchem/build/xdrawchem")

# Helvetica average advance width, as a fraction of the point size.
ADVANCE = 0.58


def render(smiles):
    with tempfile.NamedTemporaryFile(suffix=".svg", delete=False) as tmp:
        path = tmp.name
    subprocess.run([XDC, "--api", "render", smiles, "--input_format", "smiles",
                    "--out", path],
                   capture_output=True, text=True,
                   env={**os.environ, "QT_QPA_PLATFORM": "offscreen"})
    try:
        return open(path).read()
    finally:
        os.unlink(path)


def matrix_of(svg):
    m = re.search(r'transform="matrix\(([-\d.]+),([-\d.]+),([-\d.]+),'
                  r'([-\d.]+),([-\d.]+),([-\d.]+)\)"', svg)
    if not m:
        return (1, 0, 0, 1, 0, 0)
    return tuple(float(v) for v in m.groups())


def apply(m, x, y):
    a, b, c, d, e, f = m
    return (a * x + c * y + e, b * x + d * y + f)


def parse(svg):
    M = matrix_of(svg)
    labels = []
    for m in re.finditer(r'<text([^>]*)>(.*?)</text>', svg, re.S):
        attrs, text = m.group(1), m.group(2).strip()
        if not text or text.startswith("<!--"):
            continue
        xr = float(re.search(r'\sx="([-\d.]+)"', attrs).group(1))
        yr = float(re.search(r'\sy="([-\d.]+)"', attrs).group(1))
        x, y = apply(M, xr, yr)
        size = float(re.search(r'\sfont-size="([-\d.]+)"', attrs).group(1)) * 2.0
        w = ADVANCE * size * len(text)
        h = size
        box = (x - w / 2, y - h, x + w / 2, y)
        # the writer can emit the same label twice; keep one
        if any(l["text"] == text and abs(l["x"] - x) < 1 and abs(l["y"] - y) < 1
               for l in labels):
            continue
        labels.append({"text": text, "x": x, "y": y, "w": w, "h": h, "box": box})

    bonds = []
    for m in re.finditer(r'<polyline[^>]*points="([^"]+)"', svg):
        pts = [apply(M, float(q.split(",")[0]), float(q.split(",")[1]))
               for q in m.group(1).split()]
        for a, b in zip(pts, pts[1:]):
            bonds.append((a, b))
    return labels, bonds


def overlap(a, b):
    dx = min(a[2], b[2]) - max(a[0], b[0])
    dy = min(a[3], b[3]) - max(a[1], b[1])
    if dx <= 0 or dy <= 0:
        return 0.0
    return dx * dy


def seg_intersects_box(seg, box):
    (x0, y0), (x1, y1) = seg
    bx0, by0, bx1, by1 = box

    # Liang-Barsky clip
    dx, dy = x1 - x0, y1 - y0
    t0, t1 = 0.0, 1.0
    for p, q in ((-dx, x0 - bx0), (dx, bx1 - x0), (-dy, y0 - by0), (dy, by1 - y0)):
        if p == 0:
            if q < 0:
                return False
            continue
        t = q / p
        if p < 0:
            t0 = max(t0, t)
        else:
            t1 = min(t1, t)
        if t0 > t1:
            return False
    return True


def audit(name, smiles, verbose=True):
    svg = render(smiles)
    labels, bonds = parse(svg)

    collisions = []
    for i in range(len(labels)):
        for j in range(i + 1, len(labels)):
            a = overlap(labels[i]["box"], labels[j]["box"])
            if a > 0:
                collisions.append((labels[i], labels[j], a))

    on_bonds = []
    for lab in labels:
        for seg in bonds:
            if seg_intersects_box(seg, lab["box"]):
                on_bonds.append((lab, seg))
                break

    if verbose:
        print(f"{name:14s} labels={len(labels)} bonds={len(bonds)} "
              f"label/label overlaps={len(collisions)} labels on bonds={len(on_bonds)}")
        for a, b, area in collisions:
            print(f'   "{a["text"]}" @({a["x"]:.0f},{a["y"]:.0f}) overlaps '
                  f'"{b["text"]}" @({b["x"]:.0f},{b["y"]:.0f})  ~{area:.0f} units^2')
        for lab, seg in on_bonds:
            print(f'   "{lab["text"]}" @({lab["x"]:.0f},{lab["y"]:.0f}) crossed by '
                  f'bond {seg[0]}->{seg[1]}')
    return len(collisions), len(on_bonds)


FIXTURES = [
    ("glucose", "OC[C@H]1OC(O)[C@H](O)[C@@H](O)[C@@H]1O"),
    ("ribose", "OC[C@H]1O[C@@H](O)[C@H](O)[C@@H]1O"),
    ("alanine", "N[C@@H](C)C(=O)O"),
    ("aspirin", "CC(=O)Oc1ccccc1C(=O)O"),
    ("benzene", "c1ccccc1"),
]

if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] != "--all":
        audit("structure", sys.argv[1])
    else:
        for name, smi in FIXTURES:
            audit(name, smi)
