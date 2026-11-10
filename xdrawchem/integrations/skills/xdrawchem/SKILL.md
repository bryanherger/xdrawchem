---
name: xdrawchem
description: Chemical structure toolkit for agents — convert SMILES/InChI/Molfile, resolve chemical names to structures, describe and validate molecules, compare compounds, draw 2-D depictions (SVG/PNG/PDF), and build 3-D conformers. Use for chemistry reasoning, structure lookups, or making structure images.
---

# XDrawChem: chemistry for agents

XDrawChem is a 2-D molecule editor. This skill uses its **headless API** —
no GUI, no dialogs, JSON out — plus OpenBabel for the chemistry itself and
PubChem for registry lookups.

Everything returns a JSON envelope. Check `ok` first:

```json
{ "ok": true, "command": "describe", "formula": "C9H8O4", ... }
{ "ok": false, "command": "convert", "error": "could not parse input structure" }
```

## Locating the tool

```bash
xdrawchem --api            # prints the full machine-readable manifest, then exits
```

If `xdrawchem` is not on PATH, use a local build (`build/xdrawchem`) or set
`XDRAWCHEM_BIN`. The API needs no display — it sets `QT_QPA_PLATFORM=offscreen`
itself.

## Commands

| Command | Purpose |
|---|---|
| `convert` | SMILES ⇄ InChI ⇄ InChIKey ⇄ Molfile ⇄ SDF ⇄ CML ⇄ XYZ ⇄ formula |
| `name_to_structure` | chemical name / IUPAC name / CAS number → structure (PubChem) |
| `describe` | formula, MW, atom/bond counts, charge, SMILES, InChI, InChIKey, IUPAC name, CID |
| `validate` | does it parse, and does every atom obey valence rules |
| `inchi_key` | the 27-character compound identifier |
| `compare` | are these two the same compound (names resolved automatically) |
| `render` | draw it — SVG, PNG, or PDF |
| `make_3d` | MMFF94/UFF-minimised 3-D conformer (Molfile, SDF, XYZ) |
| `reaction` | summarise reaction SMILES `reactants>>products` |

## Structure input

Every `structure` argument accepts any of:

- SMILES — `c1ccccc1O`
- InChI — `InChI=1S/C6H6O/c7-6-4-2-1-3-5-6/h1-5,7H`
- an MDL molfile block (text containing `M  END`)
- `@path` to a `.mol` / `.sdf` / `.xdc` / `.cml` / `.cdxml` / `.cdx` file

Format is auto-detected; override with `--from` / `--input_format`.

## Recipes

**"What is aspirin and can you draw it?"**

```bash
xdrawchem --api name_to_structure --name aspirin        # name -> canonical SMILES
xdrawchem --api describe   --structure "CC(=O)Oc1ccccc1C(=O)O"
xdrawchem --api render     --structure "CC(=O)Oc1ccccc1C(=O)O" --format svg --out aspirin.svg
```

`describe` returns the canonical SMILES. Feed that into `render` so you draw
exactly the compound you described.

**"Are 'aspirin' and this SMILES the same?"** — names work directly:

```bash
xdrawchem --api compare --structure_a aspirin --structure_b "CC(=O)Oc1ccccc1C(=O)O"
# -> same_structure: true, same_connectivity: true
```

Read `same_connectivity` to ignore stereochemistry, `same_structure` to
require it.

**"Is my SMILES valid?"**

```bash
xdrawchem --api validate --structure "CCO"
# -> valid: true, problems: []
```

`problems` lists each over-bonded atom in plain language
(`"C atom has 5 bonds (maximum is 4)"`).

**Images for a report** — SVG for documents (vector, editable), PNG for chat
previews, PDF for print:

```bash
xdrawchem --api render --structure "CCO" --format png --width 600 --out ethanol.png
xdrawchem --api render --structure "CCO" --format svg --out ethanol.svg
```

Without `--out`, images come back inline: `svg` as text, PNG/PDF as
`data_base64`. Anything over 256 KB is not inlined — rerun with `--out`.

**3-D for docking / geometry checks:**

```bash
xdrawchem --api make_3d --structure "CCO" --format xyz
```

The reply names the `force_field` actually used (MMFF94, falling back to UFF).

**Reactions:**

```bash
xdrawchem --api reaction --reaction "CC(=O)OC(C)=O.Oc1ccccc1C(=O)O>>CC(=O)Oc1ccccc1C(=O)O.CC(=O)O"
```

Returns per-side formulas, atom counts, `atom_delta`, and `formula_changed`.

## MCP

If the agent runtime speaks MCP, use `integrations/mcp/xdrawchem_mcp.py`
instead of shelling out. It exposes the same nine commands as tools named
`xdrawchem_convert`, `xdrawchem_describe`, … plus `xdrawchem_manifest`.

```bash
pip install "mcp>=2"
python3 integrations/mcp/xdrawchem_mcp.py          # stdio
python3 integrations/mcp/xdrawchem_mcp.py --check  # verify it finds the binary
```

## Pitfalls

- **Names are not structures.** `convert` and `describe` need SMILES/InChI/
  molfile. Use `name_to_structure` first (or `compare`, which resolves names).
  A failed parse returns `ok: false` — it does not silently guess.
- **Network.** `name_to_structure` and the PubChem annotations in `describe`
  need internet. Everything else (convert, validate, inchi_key, render,
  make_3d, reaction) is fully offline via OpenBabel.
- **Missing PubChem is not an error.** `describe` omits `iupac_name`, `cas`,
  and `pubchem_cid` when PubChem has no match; the rest of the answer is
  still valid.
- **Stereochemistry.** `compare` reports `same_connectivity` (ignores stereo)
  and `same_structure` (requires it). Pick deliberately.
- **Big images.** Pass `--out` for anything large; inline payloads are capped.
- **Exit code.** `0` on success, `1` on failure — but always parse `ok`, since
  the JSON is the contract.

## Quick reference

```bash
xdrawchem --api convert  --structure "CCO" --to canonical
xdrawchem --api name_to_structure --name "acetylsalicylic acid"
xdrawchem --api describe --structure "CCO"
xdrawchem --api validate --structure "CCO"
xdrawchem --api inchi_key --structure "CCO"
xdrawchem --api compare  --structure_a "CCO" --structure_b "CCO"
xdrawchem --api render   --structure "CCO" --format svg
xdrawchem --api make_3d  --structure "CCO" --format mol
xdrawchem --api reaction --reaction "CCO>>CC=O"
```
