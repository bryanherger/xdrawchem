# XDrawChem — Qt5/Qt6 Edition

XDrawChem is an open-source 2D chemical structure drawing application
supporting CML, MDL Mol/SDF, CDXML, XDC, and SMILES file formats.
It integrates with OpenBabel for format conversion, SMILES generation,
InChI, and 3D coordinate generation.

## For AI agents

XDrawChem also runs headless, for scripts and AI agents:

```bash
xdrawchem --api                                    # list every command (JSON)
xdrawchem --api name_to_structure --name aspirin   # name -> structure
xdrawchem --api describe --structure "CCO"         # formula, MW, SMILES, InChI…
xdrawchem --api render --structure "CCO" --format svg --out ethanol.svg
```

The same functions are available as an **MCP server**
(`integrations/mcp/xdrawchem_mcp.py`) and an **agent skill**
(`integrations/skills/xdrawchem/SKILL.md`).

- [doc/AGENT_API.md](doc/AGENT_API.md) — the API reference
- [doc/INTEGRATIONS.md](doc/INTEGRATIONS.md) — setup for Claude Desktop,
  Claude Code, OpenClaw, and any other MCP client or skill-based agent

---

## Requirements

| Dependency | Minimum | Recommended | Notes |
|---|---|---|---|
| Qt | 5.15 | 6.4+ | `qtbase`, `qttools`, `qtnetwork` |
| OpenBabel | 3.0 | 3.1+ | Dev headers required |
| CMake | 3.19+ | 3.28+ | For the Qt6 build |
| C++ compiler | C++14 | C++17 | GCC 9+, Clang 10+, MSVC 2019+ |

On **Debian/Ubuntu**:
```bash
# Qt6 build (recommended)
sudo apt install qt6-base-dev qt6-tools-dev qt6-tools-dev-tools \
    qt6-svg-dev \        # Ubuntu 24.04+; use libqt6svg6-dev on 22.04
    libqt6xml6t64 libqt6network6t64 libqt6printsupport6t64 \
    libopenbabel-dev libssl-dev cmake ninja-build \
    libxkbcommon-dev

# Qt5 build (legacy)
sudo apt install qt5-qmake qtbase5-dev qttools5-dev \
    libopenbabel-dev libssl-dev
```

On **Fedora/RHEL**:
```bash
sudo dnf install qt6-qtbase-devel qt6-qttools-devel qt6-qtsvg-devel \
    openbabel-devel openssl-devel cmake ninja-build \
    libxkbcommon-devel
```

On **macOS** (Homebrew):
```bash
brew install qt@6 open-babel cmake ninja
```

---

## Building

### Qt6 — CMake (recommended)

```bash
cd xdrawchem
mkdir build && cd build
cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release
ninja
```

Optional CMake variables:

| Variable | Default | Description |
|---|---|---|
| `CMAKE_BUILD_TYPE` | `Debug` | `Release`, `RelWithDebInfo`, `Debug` |
| `CMAKE_INSTALL_PREFIX` | `/usr/local` | Installation root |
| `RINGHOME` | `<prefix>/share/xdrawchem` | Ring template data directory |

To install system-wide:
```bash
sudo ninja install
```

### Qt5 — qmake (legacy)

```bash
cd xdrawchem
qmake xdrawchem.pro   # or qmake-qt5 on some distributions
make -j$(nproc)
sudo make install
```

---

## Running

```bash
# From the build directory
./xdrawchem

# After system install
xdrawchem
```

The ring template files (`ring/*.cml`) are read from the path set by
`RINGHOME` at build time (default `/usr/local/share/xdrawchem`).
If running from the build directory without installing, set the environment
variable to point at the source tree:

```bash
RINGHOME=/path/to/xdrawchem/ring ./xdrawchem
```

---

## Project structure

```
xdrawchem/
├── CMakeLists.txt          # Qt6 CMake build (primary)
├── xdrawchem.pro           # Qt5 qmake build (legacy)
├── xdrawchem/              # All C++ source and headers
│   ├── main.cpp
│   ├── application*.cpp    # Main window
│   ├── chemdata*.cpp       # Document model + file I/O
│   ├── render2d*.cpp       # Drawing canvas
│   ├── molecule*.cpp       # Chemistry algorithms
│   └── ...
├── ring/                   # CML ring templates (amino acids, nucleotides, …)
├── doc/                    # HTML documentation
└── translation/            # Qt .ts / .qm translation files (10 languages)
```

---

## Qt6 migration notes

The codebase was ported from Qt5 to Qt6 in 2026. All Qt5-only APIs have
been replaced. Key changes:

- Build system: qmake → CMake with `qt_add_executable` / `qt_add_translations`
- All `SIGNAL()`/`SLOT()` string macros replaced with pointer-to-member syntax
- `QRegExp` → `QRegularExpression`
- `QMatrix` → `QTransform`
- `QPrinter` page-size/orientation API updated to `QPageSize`/`QPageLayout`
- `QXmlSimpleReader` → `QXmlStreamReader` (done in the Qt5 modernisation pass)
- `QFontMetrics::width()` → `horizontalAdvance()`

See `CHANGELOG.md` and `NOTES.md` for the full detail.

---

## Contributing

Please file issues and pull requests on [GitHub](https://github.com/bryanherger/xdrawchem).

Bug reports should include:
- OS and distribution version
- Qt version (`qmake --version` or `cmake --find-package -DNAME=Qt6`)
- OpenBabel version (`obabel --version`)
- Full console output of the build or crash

---

Bryan Herger — bherger@users.sf.net
