// xdc_api.h — headless, machine-readable XDrawChem API for AI agents
//
// XDrawChem's normal entry point is a GUI.  AI agents and scripts need
// something else: a non-interactive layer that takes structure text in one
// form and returns structure text/facts/images in another form, with no
// windows, no dialogs, and results on stdout in JSON.
//
// This header declares that layer.  It is deliberately plain data-in /
// data-out: every function takes strings/numbers and returns strings/numbers.
// xdc_api_cli.cpp turns them into `xdrawchem --api <command> ...` and
// integrations/mcp/ wraps the CLI as Model Context Protocol tools.
//
// Nothing here opens a dialog, and nothing here throws: failures are
// reported by ok=false plus message, so an agent can always parse the reply.

#ifndef XDC_API_H
#define XDC_API_H

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

// ── Canonical JSON envelope ──────────────────────────────────────────────────
// { "ok": true|false, "command": "...", "error": "...", ...payload fields }
// Agents should test "ok" first, then read the payload fields.
static const char * const XDC_API_SCHEMA_VERSION = "1.0";

// ── Structure input ──────────────────────────────────────────────────────────
// All functions that take a "structure" accept any of:
//   * SMILES, e.g. "c1ccccc1O"
//   * InChI,  e.g. "InChI=1S/C6H6O/c7-6-4-2-1-3-5-6/h1-5,7H"
//   * an MDL molfile/SDF block (text)
//   * a path to a .mol/.sdf/.xdc/.cml/.cdxml/.cdx file
// `format` (optional) forces the interpretation: "smiles" | "inchi" |
// "molfile" | "file".  Empty/absent means auto-detect.

struct XdcApiResult {
    bool    ok = false;
    QString command;
    QString error;
    QJsonObject payload;
};

// ── Structure conversion & inspection ───────────────────────────────────────
// Every one of these is a separate, documented agent-facing function.

// SMILES <-> InChI <-> Molfile conversion and canonicalisation.
XdcApiResult apiConvert( const QString &structure, const QString &fromFormat,
                         const QString &toFormat );

// Resolve a chemical name / CAS number to a structure, via PubChem.
XdcApiResult apiNameToStructure( const QString &name, const QString &toFormat );

// Describe a structure: formula, molecular weight, atom/bond counts, charge,
// canonical SMILES, InChI, and (when PubChem knows it) IUPAC name, CAS, CID.
XdcApiResult apiDescribe( const QString &structure, const QString &format );

// Validate a structure: parse errors and valence violations, atom by atom.
XdcApiResult apiValidate( const QString &structure, const QString &format );

// Standard InChIKey (27-char, e.g. LFQSCWFLJHTTHZ-UHFFFAOYSA-N).
XdcApiResult apiInchiKey( const QString &structure, const QString &format );

// Structural comparison: same connectivity / same stereochemistry, with a
// PubChem-backed fallback so "aspirin" and "CC(=O)Oc1ccccc1C(=O)O" match.
XdcApiResult apiCompare( const QString &structureA, const QString &structureB,
                         const QString &formatA, const QString &formatB );

// ── Rendering ───────────────────────────────────────────────────────────────
// Produce a 2-D depiction of a structure as SVG, PNG, or PDF bytes/text.
XdcApiResult apiRender( const QString &structure, const QString &format,
                        const QString &output, const QString &outPath,
                        int widthPx, int heightPx, bool transparent );

// ── 3-D ────────────────────────────────────────────────────────────────────
// Build an optimised 3-D conformer and return it as an MDL molfile, XYZ,
// or SDF string (MMFF94 minimisation, UFF fallback — same path as the GUI).
XdcApiResult apiMake3D( const QString &structure, const QString &format,
                        const QString &output );

// ── Layout cleanup ─────────────────────────────────────────────────────────
// Re-run structure-diagram generation on a structure, replacing whatever
// coordinates it came with (hand-drawn, file-supplied, or absent) with a
// clean 2-D layout.  This is the same operation as the GUI's
// Tools -> Clean Up Molecule.  Useful for molfiles with poor geometry; for
// SMILES input the parse already generated coordinates, so the result is
// usually equivalent to the input layout.
XdcApiResult apiCleanUp( const QString &structure, const QString &format,
                         const QString &output );

// ── Reactions ──────────────────────────────────────────────────────────────
// Describe a reaction SMILES/reaction molfile: reactants, products, atom and
// bond changes, stoichiometry.
XdcApiResult apiReaction( const QString &reaction, const QString &format );

// ── Introspection ──────────────────────────────────────────────────────────
// Machine-readable list of every command, its arguments, and I/O types.
// Agents call this first; the MCP server derives its JSON schema from it.
QJsonObject apiManifest();

// ── Command dispatch ───────────────────────────────────────────────────────
// Used by the CLI: takes the argv tail after "--api" and returns the JSON
// text to print on stdout.  `exitCode` is 0 on success, non-zero on failure.
QString apiDispatch( const QStringList &args, int *exitCode );

// Convenience wrappers shared by the CLI commands.
QJsonObject resultToJson( const XdcApiResult &r );
QString     resultToJsonText( const XdcApiResult &r );

// ── Shared helpers ─────────────────────────────────────────────────────────
// Load a structure (any accepted form) into a temporary document and return
// the parsed Molecule, or nullptr.  Caller owns the returned ChemData.
class ChemData;
class Render2D;
class Molecule;
Molecule   *apiLoadMolecule( ChemData *cd, const QString &structure,
                             const QString &format, QString *error );

// Render the document that holds `molecule` to SVG text / PNG bytes.
QByteArray  apiRenderSVG( ChemData *cd, Render2D *r );
QByteArray  apiRenderPDF( ChemData *cd, Render2D *r );
QImage      apiRenderPNG( ChemData *cd, Render2D *r, int widthPx, int heightPx,
                          bool transparent );

#endif // XDC_API_H

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
