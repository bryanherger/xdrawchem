# ── Version: set by rpmbuild --define "xdcver X.Yrcz" or edit here ──────────
%{!?xdcver: %global xdcver 2.1.1}
# Derive major.minor (numeric only) for the RPM Version: field
%global xdcmajmin %(echo %{xdcver} | sed "s/[^0-9.].*$//")
# Derive rc tag (everything after the numeric part) for Release:
%global xdcrc    %(echo %{xdcver} | sed "s/^[0-9.]*//")
# Final releases have empty xdcrc → Release: 1; pre-releases → Release: 0.1.rcN
%global xdcrel   %(if [ -z "%{xdcrc}" ]; then echo "1"; else echo "0.1.%{xdcrc}"; fi)
# Reverse-DNS app ID — matches Flatpak manifest and second install target
# added in CMakeLists.txt for desktop/metainfo/icon (Flathub requires these).
%global appid    io.github.bryanherger.xdrawchem

Name:           xdrawchem
Version:        %{xdcmajmin}
Release:        %{xdcrel}%{?dist}
Summary:        Two-dimensional chemical structure drawing program
License:        GPL-2.0-only
URL:            https://github.com/bryanherger/xdrawchem
# Tarball created from the git tag v{xdcver} by the release workflow.
# For local builds: tar -czf ~/rpmbuild/SOURCES/xdrawchem-%{xdcver}.tar.gz \
#   --transform='s|^xdrawchem|xdrawchem-%{xdcver}|' xdrawchem
Source0:        https://github.com/bryanherger/%{name}/archive/refs/tags/v%{xdcver}.tar.gz#/%{name}-%{xdcver}.tar.gz

# ── RHEL 8 / CentOS Stream 8 / Rocky 8 / Alma 8 ──────────────────────────────
# Requires EPEL 8 for:  qt6-qtbase  openbabel  cmake >= 3.16
# Enable with:  dnf install epel-release
#
# ── RHEL 9 / CentOS Stream 9 / Rocky 9 / Alma 9 ──────────────────────────────
# qt6-qtbase is in AppStream; openbabel is in EPEL 9
# Enable EPEL 9 with:  dnf install epel-release
#
# ── Fedora 38+ ───────────────────────────────────────────────────────────────
# All dependencies available in the default repos.

BuildRequires:  cmake >= 3.16
BuildRequires:  ninja-build
BuildRequires:  gcc-c++
BuildRequires:  qt6-qtbase-devel
BuildRequires:  qt6-qttools-devel
BuildRequires:  openbabel-devel >= 3.0
BuildRequires:  pkgconfig(openbabel-3)
BuildRequires:  desktop-file-utils
BuildRequires:  libGL-devel
BuildRequires:  libEGL-devel
BuildRequires:  qt6-qtsvg-devel
BuildRequires:  libxkbcommon-devel

Requires:       qt6-qtbase%{?_isa}
Requires:       qt6-qtsvg%{?_isa}
Requires:       openbabel-libs%{?_isa} >= 3.0

# Explicit Provides for the ring data directory path compiled in at build time
Provides:       xdrawchem-data = %{version}-%{release}

%description
XDrawChem is a two-dimensional molecule drawing program for Unix/Linux.
It mirrors the abilities of the commercial ChemDraw suite and has file
compatibility with it as well as other chemical formats through OpenBabel.

Features:
  - Fixed-length, fixed-angle drawing with automatic figure alignment
  - Automatic detection of structures, text, and arrows
  - Ring library including all standard amino acids and nucleic acids
  - Curved arrows, Bezier arrows, and stereo bond drawing
  - MDL Molfile, CML, and ChemDraw XML format support
  - OpenBabel integration for 20+ additional file formats
  - SMILES and InChI string generation
  - 13C NMR and IR spectrum prediction
  - Image export to PNG, EPS, SVG

%prep
# Source tarball top-level directory is xdrawchem-2.0rc1/
%autosetup -n %{name}-%{xdcver}

%build
%cmake \
    -GNinja \
    -DCMAKE_BUILD_TYPE=RelWithDebInfo \
    -DAPP_VERSION=%{xdcver} \
    -DRINGHOME=%{_datadir}/%{name} \
    -DCMAKE_INSTALL_PREFIX=%{_prefix}
%cmake_build

%install
%cmake_install
desktop-file-validate %{buildroot}%{_datadir}/applications/%{name}.desktop

%check
# Run the unit test suite (261 tests across 10 suites)
# tst_smiles needs an offscreen Qt platform for headless builds
cd %{_vpath_builddir}
QT_QPA_PLATFORM=offscreen ctest --output-on-failure -j$(nproc)

%files
%license GPL.txt COPYRIGHT.txt
%doc README.md CHANGELOG.md HISTORY.txt INSTALL.txt
%{_bindir}/xdrawchem
%{_datadir}/%{name}/
# Bare-name files (traditional system packaging convention)
%{_datadir}/applications/%{name}.desktop
%{_datadir}/pixmaps/%{name}.png
%{_datadir}/metainfo/%{name}.metainfo.xml
# Reverse-DNS copies (Flathub/AppStream requirement). Glob patterns tolerate
# the build environment even if the %appid macro is not defined for some reason.
%{_datadir}/applications/io.github.bryanherger.*.desktop
%{_datadir}/icons/hicolor/256x256/apps/io.github.bryanherger.*.png
%{_datadir}/metainfo/io.github.bryanherger.*.metainfo.xml

%changelog
* Thu Mar 26 2026 Bryan Herger <bherger@users.sf.net> - 2.0-0.1.rc1
- Bump to 2.0rc1: Qt6 + CMake port (zero warnings, zero errors)
- Fix #9: double bond inner-line geometry at non-linear angles
- Fix #10: Clean up molecule now preserves molecule orientation (Procrustes)
- Fix #13: curved arrow tips aligned using arc tangent direction
- Fix #14: implement Bezier arrow drawing (was entirely non-functional)
- Fix #15: SMILES output no longer produces ** (MDL Molfile column fix)
- Fix #18: OpenBabel 3 build works without manual .pro editing
- Add 261-test unit suite (10 suites) run during %%check
- Port QXmlSimpleReader -> QXmlStreamReader, remove QHttp, update signals
- Add CMakeLists.txt for modern CMake/Ninja build alongside qmake

* Sun Dec 11 2016 Bryan Herger <bherger@users.sf.net> - 1.10.2-1
- Initial RPM spec file
