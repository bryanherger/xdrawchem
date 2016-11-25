#ifndef DEFS_H
#define DEFS_H

#include <qstring.h>

// Ring directories (global, user)
extern QString RingDir, HomeDir;

// version string (makes updates easier :)
#ifdef UNIX
#define XDC_VERSION "XDrawChem 1.9.10"
#endif

#ifdef WIN32
#define XDC_VERSION "WinDrawChem 1.9.10"
#endif

#ifdef MACOSX
#define XDC_VERSION "MacDrawChem 1.9.10"
#endif

// define XDC_SERVER for Windows
#ifndef UNIX
#define XDC_SERVER "http://www.woodsidelabs.com/chemistry"
#endif

// lround() for all platforms (I hope!)
int my_round(double);

// units
#define PIXELS 0
#define ENGLISH 1
#define METRIC 2

// grid settings
#define GRID_NONE 0
#define GRID_SQUARE 1
#define GRID_HEX 2

// math constant
#define MOL_ARAD 57.29578
#ifndef UNIX  // M_PI is usually defined on Unix, but not Windows
#define M_PI 3.14159246
#endif

// modes (int Render2D::mode)
#define MODE_SELECT 90
#define MODE_SELECT_LASSO 95
#define MODE_SELECT_MOVE_POINT 110
#define MODE_SELECT_MOVE_OBJECT 111

#define MODE_SELECT_MULTIPLE 120
#define MODE_SELECT_MULTIPLE_SELECTED 91
#define MODE_SELECT_MOVE_MULTIPLE 122
#define MODE_SELECT_MULTIPLE_RESIZE 123
#define MODE_SELECT_RESIZE_MULTIPLE 124
#define MODE_SELECT_MULTIPLE_ROTATE 125
#define MODE_SELECT_ROTATE_MULTIPLE 126

#define MODE_DRAWLINE 200
#define MODE_DRAWLINE_DRAWING 201

#define MODE_DRAWCHAIN 205
#define MODE_DRAWCHAIN_DRAWING 206

#define MODE_DRAWLINE_DASH 210
#define MODE_DRAWLINE_DASH_DRAWING 211

#define MODE_DRAWLINE_UP  220
#define MODE_DRAWLINE_UP_DRAWING 221

#define MODE_DRAWLINE_DOWN  240
#define MODE_DRAWLINE_DOWN_DRAWING 241

#define MODE_DRAWWAVYLINE  250
#define MODE_DRAWWAVYLINE_DRAWING 251

#define MODE_DRAWARROW  260
#define MODE_DRAWARROW_DRAWING 261

#define MODE_DRAWCURVEARROW 270
#define MODE_DRAWCURVEARROW_DRAWING 271

#define MODE_DRAWBRACKET  280
#define MODE_DRAWBRACKET_DRAWING 281

#define MODE_DRAWBEZIER 290
#define MODE_DRAWBEZIER_DRAWING 291

#define MODE_TEXT 298

#define MODE_ERASE 300
#define MODE_ERASING 301

#define MODE_SYMBOL 500
#define MODE_DRAWNEWMAN_ANTI 510
#define MODE_DRAWNEWMAN_GAUCHE 511

#define MODE_RING 550

#define MODE_TOOL 600
#define MODE_TOOL_CALCMW 601
#define MODE_TOOL_CALCEF 602
#define MODE_TOOL_13CNMR 603
#define MODE_TOOL_CALCEA 604
#define MODE_TOOL_1HNMR  605
#define MODE_TOOL_IR     606
#define MODE_TOOL_NAME   607
#define MODE_TOOL_TOSMILES 608
#define MODE_TOOL_FROMSMILES 609
#define MODE_TOOL_CLEANUPMOL 610
#define MODE_TOOL_MOLECULE_INFO 611
#define MODE_TOOL_GROUP_REACTANT 612
#define MODE_TOOL_GROUP_PRODUCT 613
#define MODE_TOOL_CUSTOMRING 614
#define MODE_TOOL_TOINCHI 615
#define MODE_TOOL_GROUP_CLEAR 620
#define MODE_TOOL_2D3D 630
#define MODE_TOOL_PKA 640
#define MODE_TOOL_KOW 650
#define MODE_TOOL_RETRO 660
#define MODE_TOOL_RETRO_ATOMNAME 698
#define MODE_TOOL_RETRO_BONDNAME 699

// list (unique?) reactions/sites this molecule is susceptible to
#define MODE_TOOL_REACTIVITY_FORWARD 670
// list (unique?) sites that could have been created by known rxn's
#define MODE_TOOL_REACTIVITY_RETRO 671
// partial charges, maybe do colors, maybe base on AMBER or other FF
#define MODE_TOOL_CHARGES 672

#define MODE_TOOL_TEST 669

// reaction analysis
#define RXN_ENTHALPY_ESTIMATE 1
#define RXN_1HNMR 2
#define RXN_13CNMR 3
#define RXN_TEST 69

// types (Drawable::Type() and subclasses)
#define TYPE_DRAWABLE 0
#define TYPE_MOLECULE 1
#define TYPE_BOND 10
#define TYPE_TEXT 11
#define TYPE_ARROW 12
#define TYPE_BRACKET 13
#define TYPE_SYMBOL 14
#define TYPE_CURVEARROW 15
#define TYPE_BEZIER 16
#define TYPE_BIOTOOLS 20
#define TYPE_GRAPHIC_OBJECT 99
#define TYPE_GRAPHIC_LINE 100
#define TYPE_GRAPHIC_ATOMMARK 101
#define TYPE_GRAPHIC_BONDMARK 102

// bond stereochemistry (Bond::stereo)
#define STEREO_UNDEFINED 0
#define STEREO_UP 1
#define STEREO_DOWN 2
#define STEREO_E 3
#define STEREO_Z 4

// bracket types (Bracket::style)
#define BRACKET_SQUARE  1
#define BRACKET_CURVE   2
#define BRACKET_BRACE   3
#define BRACKET_BOX     4
#define BRACKET_ELLIPSE 5
#define BRACKET_CLOSEDSQUARE  6
#define BRACKET_CIRCLE  7
#define BRACKET_BOX_SHADOW 8
#define BRACKET_ELLIPSE_SHADOW 9
#define BRACKET_SQUARE_SHADOW 10
#define BRACKET_CIRCLE_SHADOW 11

// arrow styles (Arrow::style)
#define ARROW_REGULAR 1
#define ARROW_DIDNT_WORK 69
#define ARROW_DASH 2
#define ARROW_BI1 3
#define ARROW_BI2 4
#define ARROW_RETRO 5
#define ARROW_TOPHARPOON 6
#define ARROW_BOTTOMHARPOON 7
#define ARROW_MIDDLE 8

// curve arrow styles
#define CA_CW90 1
#define CA_CW180 2
#define CA_CW270 3
#define CA_CCW90 4
#define CA_CCW180 5
#define CA_CCW270 6

// symbol styles
#define SYM_PLUS 1
#define SYM_MINUS 2
#define SYM_DELTA_PLUS 3
#define SYM_DELTA_MINUS 4
#define SYM_2E 5
#define SYM_1E 6
#define SYM_RING_UP 7
#define SYM_P_ORBITAL 8
#define SYM_BEAD 9
#define SYM_P_DOUBLE 10
#define SYM_2E_LINE 11

// justification flags (Text::justify)
#define JUSTIFY_CENTER 1
#define JUSTIFY_TOPLEFT 2

// Sides (Text::NearestCenter)
#define TEXT_LEFT 1
#define TEXT_RIGHT 2
#define TEXT_TOP 3
#define TEXT_BOTTOM 4

// data held in text (Text::DataType)
#define TEXT_DATA_NORMAL 1
#define TEXT_DATA_MW 2
#define TEXT_DATA_FORMULA 3

// Text shape styles (Text::shape)
#define TEXT_SHAPE_NONE 0
#define TEXT_SHAPE_ELLIPSE 1
#define TEXT_SHAPE_RECTANGLE 2
#define TEXT_SHAPE_CIRCLE 3
#define TEXT_SHAPE_SQUARE 4

// Horizontal or vertical arrow (Arrow::Orientation)
#define ARROW_HORIZONTAL 1
#define ARROW_VERTICAL 2

// Flip direction
#define FLIP_H 1
#define FLIP_V 2

// Group types (Molecule::group_type)
#define GROUP_NONE 0
#define GROUP_REACTANT 1
#define GROUP_PRODUCT 2
#define GROUP_SOLVENT 3
#define GROUP_CATALYST 4

// output devices
#define OUTPUT_SCREEN  1
#define OUTPUT_IMAGE   2
#define OUTPUT_PRINTER 3
#define OUTPUT_EPS     4
#define OUTPUT_SVG     5

// Paintable::op Operation types
/*  OBSOLETE
#define OP_LINE 1
#define OP_DASH_LINE 2
#define OP_TEXT 3
#define OP_STRING 100
#define OP_CURVE_CW90 4
#define OP_CURVE_CCW90 5
#define OP_CURVE_CW180 6
#define OP_CURVE_CCW180 7
#define OP_CURVE_CW270 8
#define OP_CURVE_CCW270 9
#define OP_PIXMAP 10
#define OP_FILLBOX 11
#define OP_UP_LINE 12
*/

// page setup
#define PAGE_LETTER 1
#define PAGE_LEGAL  2
#define PAGE_A4     3
#define PAGE_640    4
#define PAGE_800    5
#define PAGE_1024   6
#define PAGE_CUSTOM 7
#define PAGE_PORTRAIT  100
#define PAGE_LANDSCAPE 200

// CML stuff
// track what tag(s) we're in (private int CMLParser::states)
#define CML_MOLECULE 10
#define CML_NONE   1
#define CML_ATOM   2
#define CML_BOND   3
#define CML_ATOMARRAY 4
#define CML_BONDARRAY 5
#define CDXML_COLORTABLE  100
#define CDXML_FONTTABLE   101
#define CDXML_PAGE        102
#define CDXML_FRAGMENT    103
#define CDXML_NODE        104
#define CDXML_BOND        105
#define CDXML_GRAPHIC     106
#define CDXML_TEXT        107

#endif // ifndef DEFS_H
