# XDrawChem for AI agents

XDrawChem exposes a **headless, machine-readable API** so AI agents and scripts
can use its chemistry without a GUI. The same functions are available three
ways:

| Surface | Entry point |
|---|---|
| CLI | `xdrawchem --api <command>` → JSON on stdout |
| MCP | `integrations/mcp/xdrawchem_mcp.py` → 10 MCP tools |
| Agent skill | `integrations/skills/xdrawchem/SKILL.md` |

All three call the same implementation (`xdrawchem/xdc_api.cpp`), so they
cannot drift apart.

---

## Quick start

```bash
# What can it do?
xdrawchem --api

# Draw a compound from its name — one call
xdrawchem --api draw --query aspirin --format png --out aspirin.png

# Name → structure, when you want the structure not the picture
xdrawchem --api name_to_structure --name aspirin

# Everything about a molecule
xdrawchem --api describe --structure "CC(=O)Oc1ccccc1C(=O)O"

# Where is this in PubChem?
xdrawchem --api pubchem_lookup --query aspirin

# Draw an already-known structure
xdrawchem --api render --structure "CC(=O)Oc1ccccc1C(=O)O" --format svg --out aspirin.svg
```

No display is required: the CLI sets `QT_QPA_PLATFORM=offscreen` itself.

---

## The functions

Every command accepts a **structure** as SMILES, InChI, an MDL molfile block,
or `@path` to a `.mol`/`.sdf`/`.xdc`/`.cml`/`.cdxml`/`.cdx` file. The format is
auto-detected unless you pass `--from` / `--input_format`.

### `draw` — name or structure → picture, in one call

```bash
xdrawchem --api draw --query aspirin --format png --width 900 --out aspirin.png
xdrawchem --api draw --query "50-78-2" --format svg          # by CAS number
xdrawchem --api draw --query "CCO" --format png --width 400  # by SMILES
xdrawchem --api draw --query "@molecule.mol" --format svg    # by file
```

Accepts a chemical name, CAS number, SMILES, InChI, molfile, or `@path`.
Names are resolved through PubChem; anything that already parses as a
structure is drawn directly. The reply reports `resolved_by`
(`structure`, `pubchem_name`, `file` or `molfile`) plus the `smiles` and
`formula` it settled on, so you can confirm what got drawn.

This is a convenience wrapper around `name_to_structure` + `render`. Use
`render` when you already hold a structure, and `name_to_structure` when you
want the structure text rather than an image.

### `convert` — move between representations

```bash
xdrawchem --api convert --structure "c1ccccc1O" --to canonical
```

`--to` accepts `smiles`, `canonical`, `inchi`, `inchikey`, `mol`, `sdf`,
`cml`, `xyz`, `formula`.

### `name_to_structure` — chemical name → structure

```bash
xdrawchem --api name_to_structure --name "acetylsalicylic acid" --to canonical
```

Accepts IUPAC names, common names, trade names, and CAS numbers. Uses the
PubChem PUG REST service (`molecule_nts.cpp`), so it needs network access.

### `describe` — the summary an agent usually wants

```bash
xdrawchem --api describe --structure "CCO"
```

Returns formula, molecular weight, atom/heavy-atom/bond counts, total charge,
spin multiplicity, per-element counts, SMILES, canonical SMILES, InChI,
InChIKey, chirality, and — when PubChem knows the compound — IUPAC name, CAS
number, and CID.

### `validate` — parse and valence check

```bash
xdrawchem --api validate --structure "CCO"
# -> { "valid": true, "problems": [] }
```

Uses `Molecule::ValenceErrors()` — the same check as the GUI's valence tool —
so `problems` reads like `"C atom has 5 bonds (maximum is 4)"`.

### `inchi_key` — stable compound identifier

```bash
xdrawchem --api inchi_key --structure "CCO"
# -> "LFQSCWFLJHTTHZ-UHFFFAOYSA-N"
```

### `compare` — are these the same compound?

```bash
xdrawchem --api compare --structure_a aspirin --structure_b "CC(=O)Oc1ccccc1C(=O)O"
```

Reports `same_connectivity` (ignores stereochemistry) and `same_structure`
(requires it). If an argument is not a structure, it is resolved as a chemical
name via PubChem — so `compare aspirin <smiles>` just works.

### `render` — 2-D depiction

```bash
xdrawchem --api render --structure "CCO" --format svg --out ethanol.svg
xdrawchem --api render --structure "CCO" --format png --width 600 --out ethanol.png
xdrawchem --api render --structure "CCO" --format pdf --out ethanol.pdf
```

SVG output is true vector. Without `--out`, small images are inlined in the
reply (`svg` as text, PNG/PDF as `data_base64`); anything over 256 KB asks you
to re-run with `--out`.

### `make_3d` — 3-D conformer

```bash
xdrawchem --api make_3d --structure "CCO" --format xyz
```

Same path as the GUI's "Build 3D model": OpenBabel `OBBuilder`, then MMFF94
minimisation (UFF fallback). The reply reports which `force_field` was used.

### `pubchem_lookup` — registry URI for a compound

```bash
xdrawchem --api pubchem_lookup --query aspirin
xdrawchem --api pubchem_lookup --query "64-17-5"
xdrawchem --api pubchem_lookup --query "InChI=1S/C2H6O/c1-2-3/h3H,2H2,1H3"
```

Returns `pubchem_cid` and `pubchem_uri` — the canonical
`https://pubchem.ncbi.nlm.nih.gov/compound/<cid>` page — plus structure
properties when available (`smiles`, `inchikey`, `formula`, `title`). Use it
when an agent needs to cite a source or hand a human a link.

Query kinds are auto-detected; override with `--format name|cas|smiles|inchi`.
InChI queries are POSTed (a path-form InChI is rejected by PUG REST with
HTTP 400). A name PubChem does not know returns `ok: false` with a
"no compound matching" message rather than a network error.

### `clean_up` — regenerate a 2-D layout

```bash
xdrawchem --api clean_up --structure "@messy.mol" --format mol
```

Runs structure-diagram generation again — the same operation as the GUI's
**Tools → Clean Up Molecule**. Most useful for molfiles whose coordinates are
hand-drawn or poorly laid out.

Be aware: SMILES input already gets a clean layout at parse time, so cleanup
on SMILES usually returns an identical arrangement (`connectivity_unchanged`
will be `true`). Pass molfile coordinates to actually change geometry.

### `reaction` — reaction summary

```bash
xdrawchem --api reaction \
  --reaction "CC(=O)OC(C)=O.Oc1ccccc1C(=O)O>>CC(=O)Oc1ccccc1C(=O)O.CC(=O)O"
```

Returns per-side formulas, atom counts, `atom_delta`, and `formula_changed`.

---

## Output contract

Always JSON, always the same envelope:

```json
{
  "ok": true,
  "command": "describe",
  "schema_version": "1.0",
  "formula": "C2H6O",
  "...": "..."
}
```

On failure:

```json
{ "ok": false, "command": "convert", "error": "could not parse input structure" }
```

**Test `ok` first.** The process exit code is `0`/`1` for shell convenience,
but the JSON is the contract. `ok` and `command` are written last and always
win, so a payload can never forge them.

---

## MCP server

```bash
pip install "mcp>=2"
python3 integrations/mcp/xdrawchem_mcp.py --check    # verify it finds the binary
python3 integrations/mcp/xdrawchem_mcp.py            # stdio
python3 integrations/mcp/xdrawchem_mcp.py --transport streamable-http --port 8765
```

Tools: `xdrawchem_convert`, `xdrawchem_name_to_structure`,
`xdrawchem_describe`, `xdrawchem_validate`, `xdrawchem_inchi_key`,
`xdrawchem_compare`, `xdrawchem_render`, `xdrawchem_make_3d`,
`xdrawchem_reaction`, `xdrawchem_manifest`.

Binary discovery order: `XDRAWCHEM_BIN` → `xdrawchem` on `PATH` → the local
`build/xdrawchem`. Timeouts are controlled with `XDRAWCHEM_API_TIMEOUT`
(default 120 s).

### Client configuration

```json
{
  "mcpServers": {
    "xdrawchem": {
      "command": "python3",
      "args": ["/absolute/path/to/integrations/mcp/xdrawchem_mcp.py"],
      "env": { "XDRAWCHEM_BIN": "/absolute/path/to/build/xdrawchem" }
    }
  }
}
```

---

## Agent skill

`integrations/skills/xdrawchem/SKILL.md` is a self-contained instruction file
for agent runtimes that load skills rather than MCP servers. It carries the
command table, worked recipes, and the pitfalls (names vs. structures,
network dependency, stereochemistry in `compare`).

---

## Implementation notes

| File | Role |
|---|---|
| `xdrawchem/xdc_api.h` | Public API declarations and the result struct |
| `xdrawchem/xdc_api.cpp` | Implementation + command dispatch + manifest |
| `xdrawchem/xdc_api_cli.cpp` | `--api` entry point called from `main.cpp` |
| `integrations/mcp/xdrawchem_mcp.py` | MCP server (wraps the CLI) |
| `integrations/skills/xdrawchem/SKILL.md` | Agent skill |

Supporting changes made to the core:

- `ChemData::fromString()` — parse a structure string without GUI interaction
  (`chemdata_tools.cpp`).
- `Molecule::FromMolfile()` — build a molecule from a molfile block held in a
  string, with explicit-H perception so drawn structures match the input
  (`molecule_obmol.cpp`).
- `ChemData::rebindRender2D()`, `Molecule::setRenderer()`,
  `Render2D::setExternalPainter()` — let an export path paint a document into
  a caller-owned target instead of the on-screen canvas.

Chemistry comes from OpenBabel; PubChem is used only where a registry answer is
genuinely required. Everything except `name_to_structure` (and the PubChem
annotations inside `describe`) works fully offline.

---

## Known limitation

PNG/PDF/SVG label placement: terminal-atom glyph offsets are computed by
`Symbol::Render()` against live on-screen text metrics, so in the headless
export path a terminal label can be drawn slightly off from its bond. Bonds,
connectivity, and all textual results (`convert`, `describe`, `validate`,
`inchi_key`, `compare`, `make_3d`, `reaction`) are unaffected. SVG output is
the most reliable rendering choice today; track progress in the backlog.

---

## Tests

```bash
cd build && ninja && ctest
```

The API itself is exercised end-to-end by driving the binary; a smoke run:

```bash
for c in convert describe validate inchi_key compare make_3d reaction manifest; do
  ./build/xdrawchem --api "$c" >/dev/null 2>&1 || echo "FAILED $c"
done
```
