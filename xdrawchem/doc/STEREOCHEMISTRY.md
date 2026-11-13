# Stereochemistry labels

XDrawChem can display Cahn-Ingold-Prelog descriptors next to stereocentres
(**R**/**S**) and stereogenic double bonds (**E**/**Z**). The feature is
**off by default**: turn it on with **Tools → Show CIP labels**, or set
`XDRAWCHEM_ENABLE_CIP_LABELS=1` in the environment.

It is off by default because a descriptor that disagrees with the drawing is
worse than no descriptor. Enable it, check the labels on a structure you know,
and report anything wrong.

---

## Where the answers come from

There are two independent inputs, and it matters which one applies.

**Tetrahedral (R/S).** OpenBabel's `OBStereoFacade` perceives tetrahedral
configurations from the wedge and hash bonds in the drawing. XDrawChem keeps
that representation on `Bond`:

| `Bond::order` | Meaning | `Bond::stereo` |
|---|---|---|
| 1 | plain single bond | `STEREO_UNDEFINED` |
| 5 | solid wedge (towards viewer) | `STEREO_UP` |
| 7 | hashed wedge (away from viewer) | `STEREO_DOWN` |

The wedge narrows at `Bond::Start()`. `convertToOBMol()` writes the flag on
`Start → End`, and `convertFromOBMol()` reads it back, so the round trip
through OpenBabel preserves which end is which. Bond order 5 or 7 is always
normalised to 1 before OpenBabel sees it — a wedge is a single bond, and
passing 5 through would make OpenBabel read it as aromatic.

**Imported structures.** A molfile carries wedges directly, so they map onto
`Bond::order` 5/7 on read. A SMILES string does **not**: OpenBabel stores
`@`/`@@` as a tetrahedral configuration with no wedge flags at all, and its
2-D layout generator ignores stereochemistry. `Molecule::FromSMILES()` bridges
this after its layout pass — it writes the generated coordinates back into the
OpenBabel molecule and asks `TetStereoToWedgeHash()` which bond to wedge, then
stores that as `Bond::order` 5/7. The drawing therefore shows the wedge, and
the label path sees the same representation it sees for a molfile.

**E/Z.** Computed from the 2-D geometry: for each non-ring double bond, compare
which side of the bond axis the highest-priority substituent on each end sits
on. Same side is **Z**, opposite sides is **E**. Ring double bonds are never
labelled — the ring constrains them to a cis arrangement, so the descriptor
would be noise.

For an imported SMILES this only works if the drawing matches the input, and
the 2-D generator draws every alkene as the extended zigzag. A cis alkene
therefore came out drawn trans. `FromSMILES()` mirrors the far half of the
molecule across the double-bond axis when the parsed configuration says cis and
the layout came out trans.

---

## Ranking limits

The R/S letter is `R` when the sequence 1 → 2 → 3 runs clockwise with the
lowest-priority ligand (4) pointing away. Getting 1, 2, 3 means ranking the
substituents, which Open Babel 3.1.1 does not provide, so
`molecule_cip_priority.cpp` implements the hierarchy rule:

- higher atomic number wins;
- ties broken by expanding each ligand's neighbourhood, up to four bonds deep;
- a multiple bond counts its partner twice, so `C=O` outranks `C–O`;
- implicit hydrogens on carbon rank as real hydrogen.

**Not implemented:** sequence-rule sub-rules 3–5 (geometric-mean and
most-massive-atom criteria, ring rules), stereogenic lone pairs, the
pseudoasymmetric `r`/`s` descriptors, and isotopic mass beyond atomic number.

For ordinary organic structures — C, N, O, S, halogen substituents — the
hierarchy comparison agrees with full CIP. Where it does not, the label can be
wrong; treat unusual stereocentres with suspicion. Two ligands with identical
signatures are treated as "not a stereocentre" rather than guessed at.

E/Z uses the same simplified priority comparison, so a double bond whose two
ends differ only in a subtle way may be labelled inconsistently.

---

## Tests

`tests/tst_cip.cpp` pins the chemistry, not the rendering, because a wrong
descriptor actively misleads. Fixtures are anchored to an external standard —
InChI, via Open Babel — rather than to what the SMILES notation looks like it
should mean, because `@` and `@@` do **not** map to a fixed CIP letter: the
letter depends on the order the substituents appear in the string.

```
obabel -:"C[C@H](O)C(=O)O" -oinchi
  InChI=1S/C3H6O3/c1-2(4)3(5)6/h2,4H,1H3,(H,5,6)/t2-/m0/s1
  ^ identical to PubChem CID 107689, L-(+)-lactic acid, which is (S)

obabel -:"C[C@@H](O)C(=O)O" -oinchi
  InChI=1S/C3H6O3/.../t2-/m1/s1
  ^ D-lactic acid, which is (R)
```

Covered: both lactic-acid enantiomers, (S)-alanine, threonine's two centres,
cis/trans-2-butene, the cases that must stay unlabelled (ethanol, benzene,
propene, 2-methyl-2-butene), cache invalidation, the drawing (exactly one
stereo bond, opposite for the enantiomer), and a molfile input.

---

## Known gaps

- SDG has no collision handling on fused ring systems, so a structure can be
  laid out with two atoms in the same place; see issue #30. That is a layout
  fault, not a stereo fault, but it makes any depiction unreadable.
- Labels are drawn as plain text next to the atom, so on a crowded structure
  they can overlap other labels.
- Encoding the descriptor into CML or Molfile output is not implemented.
