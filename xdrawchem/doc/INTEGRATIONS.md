# Using XDrawChem with AI agents

How to wire XDrawChem's chemistry tools into the agent tools people actually
use: Claude Desktop, Claude Code, OpenClaw, and anything else that speaks MCP
or loads skills.

XDrawChem offers three surfaces. Pick based on what your agent supports:

| Your agent supports | Use | Setup effort |
|---|---|---|
| MCP servers | `integrations/mcp/xdrawchem_mcp.py` | One config block |
| Skills / instruction files | `integrations/skills/xdrawchem/SKILL.md` | Copy one directory |
| Only a shell | `xdrawchem --api <command>` | None |

The MCP server and the CLI are the same code underneath, so behaviour is
identical — the MCP server just shells out to the binary.

---

## Prerequisites

Build XDrawChem and confirm the API responds:

```bash
cd xdrawchem && mkdir -p build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
./xdrawchem --api | head -5      # prints the command manifest as JSON
```

Note the **absolute path** to the binary — you will need it in the configs
below. For MCP, install the SDK in a virtualenv (recommended; the system
Python is usually PEP 668 "externally managed"):

```bash
python3 -m venv ~/.venvs/xdrawchem-mcp
~/.venvs/xdrawchem-mcp/bin/pip install "mcp>=2"
~/.venvs/xdrawchem-mcp/bin/python integrations/mcp/xdrawchem_mcp.py --check
# -> xdrawchem: /path/to/build/xdrawchem
# -> manifest:  ok=True
```

Use the venv's `python` path in every config below. Run `--check` first —
it is the fastest way to tell "misconfigured path" apart from "broken tool".

---

## Claude Desktop

Claude Desktop reads `claude_desktop_config.json`:

- **macOS**: `~/Library/Application Support/Claude/claude_desktop_config.json`
- **Windows**: `%APPDATA%\Claude\claude_desktop_config.json`
- **Linux**: `~/.config/Claude/claude_desktop_config.json`

```json
{
  "mcpServers": {
    "xdrawchem": {
      "command": "/home/you/.venvs/xdrawchem-mcp/bin/python",
      "args": ["/absolute/path/to/xdrawchem/integrations/mcp/xdrawchem_mcp.py"],
      "env": {
        "XDRAWCHEM_BIN": "/absolute/path/to/xdrawchem/build/xdrawchem"
      }
    }
  }
}
```

Restart Claude Desktop completely (quit from the tray/menu, don't just close
the window). The tools appear under the hammer icon as `xdrawchem_convert`,
`xdrawchem_describe`, and so on.

Then just ask: *"Draw caffeine"* or *"Are aspirin and this SMILES the same
molecule: CC(=O)Oc1ccccc1C(=O)O?"*

**Tip:** tell Claude to save images to a path it can read, then open the file —
Claude Desktop renders returned image paths in its UI:

> "Render caffeine as a PNG to /tmp/caffeine.png and show me."

---

## Claude Code

Claude Code manages MCP servers with a CLI, no JSON editing:

```bash
claude mcp add xdrawchem \
  --env XDRAWCHEM_BIN=/absolute/path/to/xdrawchem/build/xdrawchem \
  -- /home/you/.venvs/xdrawchem-mcp/bin/python \
     /absolute/path/to/xdrawchem/integrations/mcp/xdrawchem_mcp.py
```

Verify with `claude mcp list`, then within a session:

```
/mcp
```

Alternatively, drop a skill in and skip MCP entirely — Claude Code reads
`~/.claude/skills/`:

```bash
mkdir -p ~/.claude/skills
cp -r /absolute/path/to/xdrawchem/integrations/skills/xdrawchem ~/.claude/skills/
```

Claude Code then invokes the API through its own shell tool, following the
skill's recipes. This needs `xdrawchem` on `PATH` (or `XDRAWCHEM_BIN` set),
since the skill shells out.

---

## OpenClaw

OpenClaw supports both paths. **MCP** is the more robust choice; the skill
route works well if you already keep skills locally.

### Option A — MCP server

Register the server in your OpenClaw config:

```json
{
  "mcpServers": {
    "xdrawchem": {
      "command": "/home/you/.venvs/xdrawchem-mcp/bin/python",
      "args": ["/absolute/path/to/xdrawchem/integrations/mcp/xdrawchem_mcp.py"],
      "env": { "XDRAWCHEM_BIN": "/absolute/path/to/xdrawchem/build/xdrawchem" }
    }
  }
}
```

After restart, the tools are discoverable with a tool search
(`tool_search` for "molecule" or "structure") and callable directly.

### Option B — personal skill

Copy the skill into your agent's skill directory:

```bash
mkdir -p ~/.openclaw/agents/main/agent/workshop-skills
cp -r /absolute/path/to/xdrawchem/integrations/skills/xdrawchem \
      ~/.openclaw/agents/main/agent/workshop-skills/
```

The agent sees it in its skills list and loads the recipes on demand. This
needs `xdrawchem` resolvable — either on `PATH` or via `XDRAWCHEM_BIN`.

### Showing structures inline

OpenClaw attaches raster media inline; SVG is not attached by the media
pipeline. For chat answers use PNG:

```bash
xdrawchem --api render --structure "Cn1cnc2c1c(=O)n(c(=O)n2C)C" \
          --format png --width 520 --out /tmp/caffeine.png
```

Then attach `MEDIA:/tmp/caffeine.png` in the reply. Use `--format svg` when
the output is destined for a document or the web, not a chat bubble.

---

## Any other MCP client

Anything implementing MCP works with the same command/args/env triple. For
clients that prefer HTTP rather than spawning a process:

```bash
XDRAWCHEM_BIN=/path/to/build/xdrawchem \
  python3 integrations/mcp/xdrawchem_mcp.py \
  --transport streamable-http --host 127.0.0.1 --port 8765
```

Then point the client at `http://127.0.0.1:8765/mcp`. (`--transport sse` is
also supported for older clients.)

---

## What the agent gets

Eleven MCP tools:

| Tool | Does |
|---|---|
| `xdrawchem_convert` | SMILES ⇄ InChI ⇄ InChIKey ⇄ Molfile ⇄ SDF ⇄ CML ⇄ XYZ ⇄ formula |
| `xdrawchem_name_to_structure` | chemical name / CAS → structure (PubChem) |
| `xdrawchem_describe` | formula, MW, counts, charge, SMILES, InChI, CID, IUPAC |
| `xdrawchem_validate` | parse + valence check |
| `xdrawchem_inchi_key` | 27-character compound identifier |
| `xdrawchem_compare` | same compound? (names resolved automatically) |
| `xdrawchem_render` | 2-D depiction (SVG / PNG / PDF) |
| `xdrawchem_make_3d` | MMFF94/UFF-minimised 3-D conformer |
| `xdrawchem_clean_up` | regenerate a clean 2-D layout |
| `xdrawchem_reaction` | reaction SMILES summary |
| `xdrawchem_manifest` | version + full command list |

Every tool returns JSON led by `ok`. On failure `isError` is set and the body
carries a readable `error`, so the agent can recover rather than crash.

---

## Network and offline behaviour

| Command | Needs network? |
|---|---|
| `convert`, `validate`, `inchi_key`, `render`, `make_3d`, `reaction` | No — fully offline |
| `describe` | Core answer offline; IUPAC name / CAS / CID need PubChem |
| `name_to_structure` | **Yes** — PubChem PUG REST |
| `compare` | Only when an argument is a name rather than a structure |

A missing PubChem answer is not an error: `describe` simply omits those
fields and everything else stays valid.

---

## Troubleshooting

**"xdrawchem executable not found"** — set `XDRAWCHEM_BIN` to the absolute
path of the built binary, and re-run `--check`.

**Tools don't appear in Claude Desktop** — fully quit and relaunch; check the
JSON is valid; make sure `command` is the venv Python, not the system one.

**"No module named 'mcp'"** — the venv wasn't used, or it lacks the SDK.
Re-run `--check` with the venv interpreter.

**`--check` says `manifest: ok=False`** — the binary is older than this
feature. Rebuild; `xdrawchem --api` must print JSON.

**Image is blank or huge** — pass `--width`. Without `--out`, payloads over
256 KB are not inlined.

**Labels slightly misplaced in PNG/PDF** — a known cosmetic issue in the
headless path (documented in `doc/AGENT_API.md`); chemistry results are
unaffected. Prefer SVG for publication-quality output.

---

## Verifying a setup

```bash
# 1. Binary and API
/path/to/build/xdrawchem --api manifest | head -3

# 2. MCP server can reach it
~/.venvs/xdrawchem-mcp/bin/python integrations/mcp/xdrawchem_mcp.py --check

# 3. Chemistry sanity
/path/to/build/xdrawchem --api describe --structure "CCO"
#    expect formula C2H6O, MW 40.02, InChIKey LFQSCWFLJHTTHZ-UHFFFAOYSA-N
```

If all three pass, the agent-side config is the only remaining variable.
