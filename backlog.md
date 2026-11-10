# XDrawChem — Consolidated Backlog
*Compiled from all session history, GitHub issues, Debian tracker, and competitive analysis*
*As of v2.0.1 — April 2026*

[BryanH - I will work on some or all of these eventually, but if anything here should be a priority, copy the item and create an issue]

---

## Status at v2.0.1

**Completed in v2.1 cycle:**
- Headless agent API (`xdrawchem --api`) exposing 9 chemistry commands as JSON
- MCP server (`integrations/mcp/xdrawchem_mcp.py`) exposing the same commands as tools
- Agent skill (`integrations/skills/xdrawchem/SKILL.md`) + `doc/AGENT_API.md`
- 3D structure generation restored using local OpenBabel (OBBuilder + MMFF94/UFF minimization)
- MDL Mol/SDF I/O fully implemented via OpenBabel (was previously stubbed out)
- SDF multi-record browser UI for browsing and importing individual records
- CIP R/S and E/Z stereochemistry labels computed via OpenBabel stereo API
  - Tetrahedral R/S labels shown next to chiral centers
  - Cis/trans E/Z labels shown at double-bond midpoints
  - Toggle via Tools → Show CIP labels
- Memory leaks in `OBNewLoad`, `OBNewSave`, `convertToOBMol()` fixed
- All `SIGNAL()`/`SLOT()` macros converted to pointer-to-member syntax
- `qDebug()` calls gated behind `QLoggingCategory` (`xdc_logging.h/cpp`)
- Legacy Qt3/Qt4 source trees removed from `master`
- Source directory renamed from `xdrawchem-qt5/` to `xdrawchem/`
- Flatpak appstream duplicate-component issue fixed
- `debian/watch` file added for Debian tracker compatibility
- Release build: zero compiler warnings

**Completed in v2.0.1 cycle (post-v2.0):**
- Security: buffer-overflow UB in OpenBabel format resolution + atom-label strcpy fixed
- Security: shell injection risk in Ghemical/KryoMol launch eliminated (QProcess::startDetached)
- Ring menu: AA / NA / Sugars / Useful Groups / Custom rings dispatch restored
- Property panel updates on every data-mutation path (draw, ring, NTS, paste, undo, delete, selection)
- Flatpak: full Flathub-ready manifest with reverse-DNS app-id
- DEB / RPM: reverse-DNS file copies included alongside bare names
- Debian Standards-Version bumped to 4.7.2

**Completed in rc3→rc4 cycle:**
- Replaced dead woodsidelabs.com endpoints with PubChem REST API (2.2, 2.3)
  - "Find on PubChem…" (Ctrl+F) searches by name, CAS, or formula
  - Molecule Information dialog (Ctrl+I) populates CAS, IUPAC name, synonyms
- Dative bond toolbar icon now shows a distinct half-arrow XPM (2.4)
- Dative bond rubber-band preview fixed during drag
- Arrow tool: button-click now activates last-chosen arrow type
- Arrow tool: `bracket_type` initialised so first use draws correct arrow
- Live property panel (4.5): docked widget showing MW and formula, hidden by default; enable via Tools → Properties
- CHANGELOG.md created from debian/changelog and session history (2.5)
- Ubuntu 22.04 (Jammy) DEB added to CI matrix (3.1)
- Fedora 41 RPM added to CI matrix (3.2)
- Flathub manifest `io.github.bryanherger.xdrawchem.yml` + CI workflow (3.5)
- AppStream metainfo.xml: fixed name tags, added rc3/rc4 releases, screenshot URL updated
- Lone pairs / orbitals / charges: confirmed already implemented via SYM_* symbol system (SYM_2E, SYM_PLUS, SYM_MINUS, p_orbital, etc.) — no further work needed (5.3)

**Completed previously (rc1–rc3):**
- Qt6 + CMake port, all deprecation warnings cleared on all platforms
- 13 test suites, 274 tests, 100% passing
- Bugs fixed: #9–#19 (geometry, arrows, SMILES, OpenBabel 3, translations)
- Features added: IUPAC/PubChem naming, valence checking, PDF/SVG/PNG export, ACS style, dative bond, canonical SMILES, name-to-structure, InChI input
- CI/CD: Windows (MSVC + NSIS), macOS (DMG), DEB (Ubuntu 24.04), RPM (Rocky 9), Release workflow
- Single VERSION file at repo root; About/Support dialogs updated with GitHub links

---

## SECTION 1 — Open GitHub Issues

Check https://github.com/bryanherger/xdrawchem/issues for any new ones.

---

## SECTION 2 — Code Quality / Technical Debt

**2.5 CHANGELOG.md** — Created. Keep updated going forward; debian/changelog is the source of truth.

**2.6 Help window HTML docs** — The `doc/` folder contains aging HTML that references Qt3-era UI. Could be modernised in place or linked to a GitHub Wiki.

---

## SECTION 3 — Platform & Distribution

**3.4 `debian/watch` file** — ✅ Added. The Debian tracker can now track upstream releases automatically.

**3.5 Flathub submission** — Manifest created and CI workflow added. Actual submission requires a PR to `github.com/flathub/flathub` — do this after v2.1 ships.

**3.8 Repository screenshot** — Added (`xdrawchem_screenshot_0.png`). AppStream metainfo updated to reference it.

---

## SECTION 4 — Feature Parity: Medium Priority

**4.1 CIP R/S and E/Z stereochemistry labels** — ✅ Implemented. OpenBabel's `OBStereoFacade` perceives tetrahedral (R/S) and cis/trans (E/Z) configurations. Labels are rendered in bold blue text next to chiral centers and at double-bond midpoints. Toggle via Tools → Show CIP labels. Encoding in CML/Molfile output remains for future work. **Target: v2.1**

**4.2 Atom-to-atom reaction mapping** — Numbers atoms across a reaction arrow. Needs new UI mode, DPoint field, and RXN format. High effort. **Target: v2.1 or later**

**4.3 MDL SDF / RXN file support** — ✅ Core I/O implemented in `chemdata_mdl.cpp` via OpenBabel. SDF browser UI added for multi-record navigation. RXN format support remains for future release. **Target: v2.1**

**4.4 Formal charges and isotope labels on atoms** — The bond-edit dialog has +/− radio buttons but they insert cosmetic text, not chemically meaningful Molfile charge fields. `DPoint` needs a `formal_charge` member and serialization in XDC/CML/MDL formats. **Target: v2.1**

**4.5 Live property panel** — Implemented (hidden by default). Update trigger coverage confirmed on all major paths. Could be promoted to enabled-by-default after wider testing. **Polish in v2.1**

**4.6 Functional group / fragment template browser** — Categorised palette for Boc, Fmoc, TBS, Bn, etc. Library curation + browseable UI. **Target: v2.1 or later**

**4.7 S-Groups and polymer notation** — SRU brackets, superatom abbreviations. Requires new data model. **Target: v2.2**

---

## SECTION 5 — Feature Parity: Lower Priority

**5.1 Structure-from-image OCR (OSRA)** — Large integration project. **Target: v2.2**

**5.2 Fischer, Haworth, and Newman projection tools** — Medium effort for drawing; auto stereo assignment is harder. **Target: v2.1 or later**

**5.4 Electron pushing arrows (mechanism tools)** — Fishhook and two-electron arrows anchored to bonds/atoms. Existing curved arrow infrastructure is a starting point. **Target: v2.1**

**5.5 Superscript/subscript in text labels** — Qt `QTextDocument` rich text could handle this. **Target: v2.1**

**5.6 SMARTS input** — OB SMILES reader accepts most SMARTS already. True SMARTS output needs OB's writer. Low priority unless requested.

---

## SECTION 6 — CI/CD & Infrastructure

**6.1 GPG-signed packages** — Neither RPM nor DEB are signed. Needed for proper apt/dnf repository hosting.

**6.2 Apt/DNF repository hosting** — Host on GitHub Pages so users can `apt install xdrawchem`.

**6.3 Release workflow robustness** — Current poller has a 20-minute timeout. `workflow_run` trigger would be more reliable once GitHub supports tags.

**6.4 Ubuntu 22.04 DEB in release** — Added to CI. Wire into release artifact download in release.yml once confirmed stable.

**6.5 Code coverage reporting** — `gcov`/`lcov` + Codecov upload.

**6.6 Lintian clean** — One lintian warning outstanding on the DEB.

---

## SECTION 7 — Documentation

**7.1 GitHub Wiki** — Replace aging HTML help docs.

**7.3 PACKAGING.md** — Add Windows and macOS sections.

**7.4 Keyboard shortcuts reference** — One-page cheat sheet.

---

## Priority Summary for v2.1 Planning

| Priority | Item | Effort | Notes |
|---|---|---|---|
| 🔴 High | 4.1 CIP R/S, E/Z labels | Medium | ✅ Labels rendered; CML/Molfile encoding remains |
| 🔴 High | 4.3 SDF / RXN file support | Medium | ✅ Core I/O + browser UI done. RXN remains |
| 🟡 Medium | 4.4 Formal charges + isotopes | Medium | Molfile chemical correctness |
| 🟡 Medium | 4.5 Property panel polish | Low | Could be enabled-by-default |
| 🟡 Medium | 3.5 Flathub PR submission | Low | Submit after v2.1 ships |
| 🟡 Medium | 5.4 Electron pushing arrows | Medium | Mechanism drawing |
| 🟢 Lower | 4.2 Reaction atom mapping | High | Important for reaction DBs |
| 🟢 Lower | 4.6 Fragment template browser | Medium | Organic chemistry workflows |
| 🟢 Lower | 6.1 GPG-signed packages | Medium | Security/trust |
| 🟢 Lower | 3.4 debian/watch file | Low | ✅ Done |
| 🟢 Lower | 5.1 OSRA image OCR | High | Very useful but large |
