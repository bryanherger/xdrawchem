#!/usr/bin/env python3
"""Ring geometry report — issue #34.

Renders a structure to SVG and reports, for each ring, the bond lengths and
interior angles as drawn.  Used to decide whether a crowded depiction is a
layout fault (uneven ring) or only a label-placement fault.

Canvas units; the drawing is uniformly scaled, so relative values are what
matter.  A regular hexagon has all edges equal and all interior angles at 120.
"""
import math
import os
import re
import subprocess
import sys
import tempfile

XDC = os.environ.get("XDC_BIN", "/home/bryan/xdrawchem/build/xdrawchem")


def segments(svg):
    out = []
    for m in re.finditer(r'<polyline[^>]*points="([^"]+)"', svg):
        pts = [tuple(float(v) for v in q.split(",")) for q in m.group(1).split()]
        if len(pts) == 2:
            out.append((pts[0], pts[1]))
    return out


def render(smiles):
    with tempfile.NamedTemporaryFile(suffix=".svg", delete=False) as tmp:
        path = tmp.name
    subprocess.run([XDC, "--api", "render", smiles, "--input_format", "smiles",
                    "--out", path], capture_output=True, text=True,
                   env={**os.environ, "QT_QPA_PLATFORM": "offscreen"})
    try:
        return open(path).read()
    finally:
        os.unlink(path)


def cluster_nodes(segs, tol=4.0):
    """Merge segment endpoints that are within `tol` into single nodes.

    A double bond contributes two nearly parallel lines whose ends do not
    coincide exactly, and a ring vertex joins two or three bonds; clustering
    turns all of that into a clean graph.
    """
    nodes = []          # list of (x, y) cluster centres
    index = {}          # segment endpoint index -> node id

    def node_for(pt):
        for i, c in enumerate(nodes):
            if math.dist(pt, c) < tol:
                index[pt] = i
                return i
        nodes.append(pt)
        index[pt] = len(nodes) - 1
        return index[pt]

    edges = set()
    for a, b in segs:
        ia, ib = node_for(a), node_for(b)
        if ia != ib:
            edges.add((min(ia, ib), max(ia, ib)))
    return nodes, edges


def find_rings(nodes, edges, minlen=5, maxlen=7):
    adj = {}
    for a, b in edges:
        adj.setdefault(a, set()).add(b)
        adj.setdefault(b, set()).add(a)

    rings = []
    for start in adj:
        stack = [(start, [start])]
        while stack:
            cur, path = stack.pop()
            if len(path) > maxlen:
                continue
            for nb in adj[cur]:
                if nb == start and minlen <= len(path) <= maxlen:
                    ring = sorted(path)
                    if ring not in rings:
                        rings.append(ring)
                elif nb not in path:
                    stack.append((nb, path + [nb]))
    return rings


def report(name, smiles):
    svg = render(smiles)
    segs = segments(svg)
    nodes, edges = cluster_nodes(segs)
    rings = find_rings(nodes, edges)

    print(f"\n=== {name}  ({len(segs)} drawn segments, {len(nodes)} vertices, "
          f"{len(edges)} edges, {len(rings)} ring(s))")
    if not rings:
        print("  no ring found")
        return

    for r_i, ring_ids in enumerate(rings):
        ring = [nodes[i] for i in ring_ids]
        n = len(ring)
        lengths = []
        for k in range(n):
            lengths.append(math.dist(ring[k], ring[(k + 1) % n]))
        angles = []
        for k in range(n):
            p, c, q = ring[k], ring[(k + 1) % n], ring[(k + 2) % n]
            v1 = (p[0] - c[0], p[1] - c[1])
            v2 = (q[0] - c[0], q[1] - c[1])
            ang = math.degrees(math.acos(
                max(-1, min(1, (v1[0] * v2[0] + v1[1] * v2[1]) /
                           (math.hypot(*v1) * math.hypot(*v2))))))
            angles.append(ang)

        avg = sum(lengths) / n
        spread = max(lengths) / min(lengths)
        print(f"  ring {r_i}: {n}-membered")
        print(f"    edge lengths: " + " ".join(f"{v:.1f}" for v in lengths))
        print(f"    mean {avg:.1f}  min {min(lengths):.1f}  max {max(lengths):.1f}"
              f"  max/min {spread:.2f}")
        print(f"    interior angles: " + " ".join(f"{v:.0f}" for v in angles))
        print(f"    regular would be "
              + f"{(n - 2) * 180 / n:.0f} for all")


if __name__ == "__main__":
    if len(sys.argv) > 1 and sys.argv[1] != "--fixtures":
        report("structure", sys.argv[1])
    else:
        for name, smi in [
            ("glucose", "OC[C@H]1OC(O)[C@H](O)[C@@H](O)[C@@H]1O"),
            ("cyclohexane", "C1CCCCC1"),
            ("benzene", "c1ccccc1"),
        ]:
            report(name, smi)
