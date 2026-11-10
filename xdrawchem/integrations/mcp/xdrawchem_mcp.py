#!/usr/bin/env python3
"""XDrawChem MCP server — exposes XDrawChem's chemistry API to AI agents.

This is a thin Model Context Protocol wrapper around the XDrawChem headless
CLI (`xdrawchem --api <command>`).  All chemistry is done by XDrawChem and
OpenBabel; this file only adapts the CLI's JSON to MCP tool calls, so the
CLI and the MCP server can never drift apart.

Design notes
------------
* One MCP tool per XDrawChem API command, with a JSON schema mirroring the
  command's arguments (see `xdrawchem --api` for the live manifest).
* Every call shells out to the binary once — no long-lived state, so the
  server is safe to restart and easy to run from any agent framework.
* Errors from XDrawChem come back as `isError: true` with the CLI's message
  instead of raising, so an agent always gets a usable reply.

Run
---
    # stdio (what most MCP clients expect)
    python3 integrations/mcp/xdrawchem_mcp.py

    # point at a specific binary
    XDRAWCHEM_BIN=/path/to/xdrawchem python3 integrations/mcp/xdrawchem_mcp.py

    # streamable HTTP, for clients that prefer a URL
    python3 integrations/mcp/xdrawchem_mcp.py --transport streamable-http --port 8765

Requirements: `pip install "mcp>=2"` (Python 3.10+).
"""

from __future__ import annotations

import argparse
import json
import os
import shlex
import shutil
import subprocess
import sys
from dataclasses import dataclass
from typing import Any

from mcp.server.mcpserver import MCPServer

# ─────────────────────────────────────────────────────────────────────────────
# Locating the XDrawChem binary
# ─────────────────────────────────────────────────────────────────────────────


def _find_binary() -> str:
    """Find the xdrawchem executable: env override, then PATH, then build dir."""
    env = os.environ.get("XDRAWCHEM_BIN")
    if env:
        return env

    on_path = shutil.which("xdrawchem")
    if on_path:
        return on_path

    # Running from a source checkout: use the local build if it exists.
    here = os.path.dirname(os.path.abspath(__file__))
    repo = os.path.abspath(os.path.join(here, "..", ".."))
    for candidate in (
        os.path.join(repo, "build", "xdrawchem"),
        os.path.join(repo, "build", "xdrawchem", "xdrawchem"),
    ):
        if os.path.isfile(candidate) and os.access(candidate, os.X_OK):
            return candidate

    return "xdrawchem"  # let the spawn fail with a clear message


XDRAWCHEM_BIN = _find_binary()
DEFAULT_TIMEOUT = float(os.environ.get("XDRAWCHEM_API_TIMEOUT", "120"))


# ─────────────────────────────────────────────────────────────────────────────
# CLI bridge
# ─────────────────────────────────────────────────────────────────────────────


@dataclass
class ApiResult:
    ok: bool
    payload: dict[str, Any]
    text: str

    def as_tool_result(self) -> tuple[str, bool]:
        """Return (text_for_agent, is_error)."""
        return self.text, not self.ok


def call_api(command: str, **options: Any) -> ApiResult:
    """Run one XDrawChem API command and return its parsed JSON."""
    argv: list[str] = [XDRAWCHEM_BIN, "--api", command]

    for key, value in options.items():
        if value is None:
            continue
        if isinstance(value, bool):
            if value:
                argv.append(f"--{key}")
            continue
        if isinstance(value, (list, tuple)):
            value = ",".join(str(v) for v in value)
        argv.append(f"--{key}")
        argv.append(str(value))

    try:
        proc = subprocess.run(
            argv,
            capture_output=True,
            text=True,
            timeout=DEFAULT_TIMEOUT,
        )
    except FileNotFoundError:
        return ApiResult(
            False,
            {"error": f"xdrawchem executable not found (tried {XDRAWCHEM_BIN!r})"},
            json.dumps(
                {
                    "ok": False,
                    "error": f"xdrawchem executable not found (tried {XDRAWCHEM_BIN!r}). "
                    "Set XDRAWCHEM_BIN or build the project first.",
                },
                indent=2,
            ),
        )
    except subprocess.TimeoutExpired:
        return ApiResult(
            False,
            {"error": "timeout"},
            json.dumps(
                {"ok": False, "error": f"xdrawchem {command} timed out after "
                 f"{DEFAULT_TIMEOUT:.0f}s"},
                indent=2,
            ),
        )

    stdout = proc.stdout.strip()
    if not stdout:
        message = (proc.stderr or "").strip().splitlines()
        detail = message[-1] if message else f"exit code {proc.returncode}"
        return ApiResult(
            False,
            {"error": detail},
            json.dumps(
                {"ok": False, "command": command, "error": detail}, indent=2
            ),
        )

    try:
        payload = json.loads(stdout)
    except json.JSONDecodeError:
        # Not JSON — most likely a Qt/platform error before the API ran.
        detail = (proc.stderr or stdout).strip().splitlines()
        return ApiResult(
            False,
            {"error": "unparseable output"},
            json.dumps(
                {
                    "ok": False,
                    "command": command,
                    "error": "xdrawchem did not return JSON",
                    "detail": detail[-3:] if detail else [],
                },
                indent=2,
            ),
        )

    ok = bool(payload.get("ok", False))
    return ApiResult(ok, payload, json.dumps(payload, indent=2))


# ─────────────────────────────────────────────────────────────────────────────
# MCP server and tools
# ─────────────────────────────────────────────────────────────────────────────

server = MCPServer(
    "xdrawchem",
    title="XDrawChem chemistry tools",
    description=(
        "Chemical structure tools for agents: convert SMILES/InChI/Molfile, "
        "resolve chemical names to structures, describe and validate "
        "structures, compare compounds, draw 2-D depictions (SVG/PNG/PDF) and "
        "build 3-D conformers. Backed by XDrawChem and OpenBabel."
    ),
)

STRUCTURE_DOC = (
    "Structure input. Accepts SMILES (e.g. 'c1ccccc1O'), InChI "
    "('InChI=1S/...'), an MDL molfile block, or '@path' to a structure file."
)


@server.tool(description="Convert a structure between representation formats.")
def xdrawchem_convert(structure: str, to: str, from_format: str | None = None) -> tuple[str, bool]:
    """Convert structures between SMILES, InChI, InChIKey, Molfile, SDF, CML, XYZ.

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        to: Target format: smiles, canonical, inchi, inchikey, mol, sdf, cml, xyz, formula.
        from_format: Optional input format hint: smiles, inchi, molfile, file.
    """
    return call_api("convert", structure=structure, to=to, **{"from": from_format}).as_tool_result()


@server.tool(description="Resolve a chemical name, IUPAC name, or CAS number to a structure via PubChem.")
def xdrawchem_name_to_structure(name: str, to: str = "smiles") -> tuple[str, bool]:
    """Look up a structure by name.

    Args:
        name: Chemical name, trade name, IUPAC name, or CAS number (e.g. 'aspirin').
        to: Output format (default 'smiles').
    """
    return call_api("name_to_structure", name=name, to=to).as_tool_result()


@server.tool(description="Summarise a structure: formula, molecular weight, counts, charge, SMILES, InChI, InChIKey, IUPAC name, CID.")
def xdrawchem_describe(structure: str, format: str | None = None) -> tuple[str, bool]:
    """Describe a molecule.

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        format: Optional input format hint.
    """
    return call_api("describe", structure=structure, format=format).as_tool_result()


@server.tool(description="Validate a structure: parse check plus valence check; lists offending atoms.")
def xdrawchem_validate(structure: str, format: str | None = None) -> tuple[str, bool]:
    """Check that a structure parses and obeys valence rules.

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        format: Optional input format hint.
    """
    return call_api("validate", structure=structure, format=format).as_tool_result()


@server.tool(description="Compute the standard 27-character InChIKey for a structure.")
def xdrawchem_inchi_key(structure: str, format: str | None = None) -> tuple[str, bool]:
    """Get the InChIKey (a stable compound identifier).

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        format: Optional input format hint.
    """
    return call_api("inchi_key", structure=structure, format=format).as_tool_result()


@server.tool(description="Decide whether two structures are the same compound (names are resolved via PubChem).")
def xdrawchem_compare(
    structure_a: str,
    structure_b: str,
    format_a: str | None = None,
    format_b: str | None = None,
) -> tuple[str, bool]:
    """Compare two structures.

    Args:
        structure_a: First structure or chemical name.
        structure_b: Second structure or chemical name.
        format_a: Optional format hint for A.
        format_b: Optional format hint for B.
    """
    return call_api(
        "compare",
        structure_a=structure_a,
        structure_b=structure_b,
        format_a=format_a,
        format_b=format_b,
    ).as_tool_result()


@server.tool(description="Draw a 2-D depiction of a structure as SVG, PNG, or PDF.")
def xdrawchem_render(
    structure: str,
    format: str = "svg",
    out: str | None = None,
    width: int | None = None,
    height: int | None = None,
    transparent: bool = False,
    input_format: str | None = None,
) -> tuple[str, bool]:
    """Render a structure image.

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        format: Output format: svg, png, or pdf (default svg).
        out: Optional file path to write to. If omitted, small images are inlined.
        width: Target width in pixels.
        height: Target height in pixels.
        transparent: PNG only — transparent background.
        input_format: Optional input format hint.
    """
    return call_api(
        "render",
        structure=structure,
        format=format,
        out=out,
        width=width,
        height=height,
        transparent=transparent or None,
        input_format=input_format,
    ).as_tool_result()


@server.tool(description="Generate a 3-D conformer (MMFF94/UFF minimised) as Molfile, SDF, or XYZ.")
def xdrawchem_make_3d(
    structure: str,
    format: str = "mol",
    input_format: str | None = None,
) -> tuple[str, bool]:
    """Build an optimised 3-D model.

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        format: Output format: mol, sdf, or xyz (default mol).
        input_format: Optional input format hint.
    """
    return call_api(
        "make_3d", structure=structure, format=format, input_format=input_format
    ).as_tool_result()


@server.tool(description="Regenerate a clean 2-D layout (like Tools -> Clean Up Molecule); most useful for molfiles with poor geometry.")
def xdrawchem_clean_up(
    structure: str, format: str = "smiles", input_format: str | None = None
) -> tuple[str, bool]:
    """Re-run structure-diagram generation on a structure.

    Args:
        structure: Structure input. SMILES, InChI, molfile block, or '@path'.
        format: Output format: smiles, canonical, mol, or sdf (default smiles).
        input_format: Optional input format hint.
    """
    return call_api(
        "clean_up", structure=structure, format=format, input_format=input_format
    ).as_tool_result()


@server.tool(description="Summarise a reaction given as reaction SMILES ('reactants>>products').")
def xdrawchem_reaction(reaction: str) -> tuple[str, bool]:
    """Describe a reaction.

    Args:
        reaction: Reaction SMILES, e.g. 'CC(=O)OC(C)=O.Oc1ccccc1C(=O)O>>CC(=O)Oc1ccccc1C(=O)O.CC(=O)O'.
    """
    return call_api("reaction", reaction=reaction).as_tool_result()


@server.tool(description="Report XDrawChem's version and the full command manifest (call this first).")
def xdrawchem_manifest() -> tuple[str, bool]:
    """Return the machine-readable command manifest."""
    return call_api("manifest").as_tool_result()


# ─────────────────────────────────────────────────────────────────────────────
# Entry point
# ─────────────────────────────────────────────────────────────────────────────


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__.splitlines()[0])
    parser.add_argument(
        "--transport",
        default=os.environ.get("XDRAWCHEM_MCP_TRANSPORT", "stdio"),
        choices=["stdio", "streamable-http", "sse"],
    )
    parser.add_argument("--port", type=int,
                        default=int(os.environ.get("XDRAWCHEM_MCP_PORT", "8765")))
    parser.add_argument("--host", default=os.environ.get("XDRAWCHEM_MCP_HOST", "127.0.0.1"))
    parser.add_argument("--check", action="store_true",
                        help="Print the resolved xdrawchem binary and exit")
    args = parser.parse_args()

    if args.check:
        print(f"xdrawchem: {XDRAWCHEM_BIN}")
        print(f"timeout:   {DEFAULT_TIMEOUT:.0f}s")
        result = call_api("manifest")
        print(f"manifest:  ok={result.ok}")
        return 0 if result.ok else 1

    if args.transport == "stdio":
        server.run("stdio")
    else:
        server.run(args.transport, host=args.host, port=args.port)
    return 0


if __name__ == "__main__":
    sys.exit(main())
