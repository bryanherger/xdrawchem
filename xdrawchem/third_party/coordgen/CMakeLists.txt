# coordgen — vendored 2D coordinate generation (Schrödinger, BSD-3-Clause)
#
# Built as a static library with the default options only.  The optional
# maeparser path (COORDGEN_USE_MAEPARSER) is intentionally not enabled: it is
# only used for run-time template loading and would pull in Boost.

file(GLOB COORDGEN_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
)

add_library(coordgen STATIC ${COORDGEN_SOURCES})

target_include_directories(coordgen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(coordgen PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Third-party code: its warnings are not ours to fix, and the project adds
# -Wall -Wextra -Wpedantic on other targets.
if(MSVC)
    target_compile_options(coordgen PRIVATE /W0)
else()
    target_compile_options(coordgen PRIVATE -w)
endif()
