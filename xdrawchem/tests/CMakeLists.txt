# tests/CMakeLists.txt — Qt6 test suite for xdrawchem
cmake_minimum_required(VERSION 3.19)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Test Gui Widgets)

include(CTest)
enable_testing()

# ── Helper macro ─────────────────────────────────────────────────────────────
macro(xdc_test name)
    cmake_parse_arguments(ARG "" "" "LIBS" ${ARGN})
    add_executable(${name} ${ARG_UNPARSED_ARGUMENTS})
    target_include_directories(${name} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../xdrawchem
    )
    target_link_libraries(${name} PRIVATE
        Qt6::Core Qt6::Test Qt6::Gui Qt6::Widgets
        ${ARG_LIBS}
    )
    target_compile_options(${name} PRIVATE
        $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra>
    )
    add_test(NAME ${name} COMMAND ${name} -v2)
    set_tests_properties(${name} PROPERTIES
        FAIL_REGULAR_EXPRESSION "FAIL!"
    )
endmacro()

# ── Test: Vector2D — pure math, no Qt widget dependency ──────────────────────
xdc_test(tst_vector2d
    tst_vector2d.cpp
    ../xdrawchem/vector2D.cpp
)

# ── Test: DPoint — geometry and element methods ───────────────────────────────
xdc_test(tst_dpoint
    tst_dpoint.cpp
)
target_link_libraries(tst_dpoint PRIVATE Qt6::Widgets)

# ── Test: Bond geometry — midpoint regression, toVector, angles ───────────────
xdc_test(tst_bondgeometry
    tst_bondgeometry.cpp
    ../xdrawchem/vector2D.cpp
)

# ── Test: CML parser — atom/bond parsing, stereo bonds, ring file fixtures ────
xdc_test(tst_cmlparser
    tst_cmlparser.cpp
    globals_stub.cpp
)
target_link_libraries(tst_cmlparser PRIVATE xdrawchem_obj)
target_compile_definitions(tst_cmlparser PRIVATE
    RING_DIR="${CMAKE_SOURCE_DIR}/ring"
)

# ── Test: SSSR ring-detection algorithm ──────────────────────────────────────
# Pure graph algorithm — DPoint neighbors lists, no Render2D.
# Needs xdrawchem_obj because molecule_sssr.cpp includes bond.h→render2d.h
# (transitive header dep), but never dereferences Render2D at runtime.
xdc_test(tst_sssr
    tst_sssr.cpp
    globals_stub.cpp
)
target_link_libraries(tst_sssr PRIVATE xdrawchem_obj)

# ── Test: SMILES round-trip via OpenBabel ─────────────────────────────────────
# Uses QTEST_MAIN (needs QApplication); run with offscreen platform to avoid
# requiring a physical display in CI environments.
# Uses QTEST_MAIN (not APPLESS) because ToMDLMolfile creates QTextDocument.
# Links xdrawchem_obj for the full Molecule/Bond stack + OpenBabel.
xdc_test(tst_smiles
    tst_smiles.cpp
    globals_stub.cpp
)
target_link_libraries(tst_smiles PRIVATE xdrawchem_obj)
# QApplication requires a display — use offscreen platform in headless environments
set_tests_properties(tst_smiles PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)

# ── Test: MolData static lookup functions ─────────────────────────────────────
# Pure header-only static class — zero Qt widget or OpenBabel dependency.
xdc_test(tst_moldata
    tst_moldata.cpp
)

# ── Test: Aromaticity detection (SSSR::FindAromatic) ─────────────────────────
# Uses Bond objects (Render2D* = nullptr), no Render2D dereference at test time.
xdc_test(tst_aromaticity
    tst_aromaticity.cpp
    globals_stub.cpp
)
target_link_libraries(tst_aromaticity PRIVATE xdrawchem_obj)

# ── Test: XML_Reader string-parsing utilities ─────────────────────────────────
# Pure QString manipulation — no Qt widget, file, or OpenBabel dependency.
xdc_test(tst_xml_reader
    tst_xml_reader.cpp
    ../xdrawchem/xml_reader.cpp
)

# ── Test: Bond::Enthalpy, Bond::Length, Molecule::OrderOfBond / SumBondEnthalpy
xdc_test(tst_bond_enthalpy
    tst_bond_enthalpy.cpp
    globals_stub.cpp
)
target_link_libraries(tst_bond_enthalpy PRIVATE xdrawchem_obj)

# ── Test: ValenceErrors — atom over-bonding detection ────────────────────────
# Pure molecule graph traversal — no network, no display.
# QTEST_APPLESS_MAIN; links xdrawchem_obj for Molecule/Bond/MolData.
xdc_test(tst_valence
    tst_valence.cpp
    globals_stub.cpp
)
target_link_libraries(tst_valence PRIVATE xdrawchem_obj)

# ── Test: SDG layout quality — bond angles must not collapse ─────────────────
# Regression guard for a substituent-placement defect that drew a methyl on top
# of an existing ring bond in caffeine (180 deg / 60 deg angles).  Parses SMILES
# through the normal pipeline and measures the resulting coordinates.
xdc_test(tst_sdg_layout
    tst_sdg_layout.cpp
    globals_stub.cpp
)
target_link_libraries(tst_sdg_layout PRIVATE xdrawchem_obj)
set_tests_properties(tst_sdg_layout PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 120
)

# ── Test: IUPAC naming and PubChem integration ────────────────────────────────
# Unit tests run offline; network tests auto-skip when PubChem unreachable.
# QTEST_MAIN (needs QApplication for QNetworkAccessManager event loop).
xdc_test(tst_iupac_pubchem
    tst_iupac_pubchem.cpp
    globals_stub.cpp
)
target_link_libraries(tst_iupac_pubchem PRIVATE xdrawchem_obj)
set_tests_properties(tst_iupac_pubchem PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 60
)

# ── Test: CIP stereochemistry descriptors (R/S, E/Z) ──────────────────────────
# Pins the chemistry, not the rendering: a wrong R/S descriptor actively
# misleads, so each fixture has one unambiguous correct answer.  Covers lactic
# acid and alanine handedness, threonine's two centres, E/Z on 2-butene, the
# achiral/ring/terminal cases that must stay unlabelled, and cache invalidation.
xdc_test(tst_cip
    tst_cip.cpp
    globals_stub.cpp
)
target_link_libraries(tst_cip PRIVATE xdrawchem_obj)
set_tests_properties(tst_cip PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 120
)

# ── Test: aromatic ring depiction ────────────────────────────────────────────
# Issue #33.  A substituted benzene ring drew two double bonds meeting at one
# atom, which a six-membered ring cannot have.
xdc_test(tst_aromatic
    tst_aromatic.cpp
    globals_stub.cpp
)
target_link_libraries(tst_aromatic PRIVATE xdrawchem_obj)
set_tests_properties(tst_aromatic PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 120
)

# ── Test: atom label placement ────────────────────────────────────────────────
# Issue #34.  Labels were drawn on their atom, so bonds ran through the glyphs
# and neighbouring labels merged.
xdc_test(tst_labels
    tst_labels.cpp
    globals_stub.cpp
)
target_link_libraries(tst_labels PRIVATE xdrawchem_obj)
set_tests_properties(tst_labels PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 120
)

# ── Test: MDL import — labels, wedges, hydrogen handling ─────────────────────
# Issue #32.  A molfile rendered with no heteroatom labels, an invisible wedge,
# and every explicit hydrogen drawn as a vertex.
xdc_test(tst_molfile
    tst_molfile.cpp
    globals_stub.cpp
)
target_link_libraries(tst_molfile PRIVATE xdrawchem_obj)
set_tests_properties(tst_molfile PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
    TIMEOUT 120
)

# ── Test: ACS style, canonical SMILES, dative bond ────────────────────────────
# Tests Preferences values (no display), ToCanonicalSMILES (needs OB),
# and dative bond order storage. QTEST_APPLESS_MAIN.
xdc_test(tst_drawing_styles
    tst_drawing_styles.cpp
    globals_stub.cpp
)
target_link_libraries(tst_drawing_styles PRIVATE xdrawchem_obj Qt6::Svg)
set_tests_properties(tst_drawing_styles PROPERTIES
    ENVIRONMENT "QT_QPA_PLATFORM=offscreen"
)
