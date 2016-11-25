#include <qpixmap.h>

#include "previewwidget.h"
#include "defs.h"

// arrows
#include "arrows.h"
#include "cw90.xpm"
#include "ccw90.xpm"
#include "cw180.xpm"
#include "ccw180.xpm"
#include "cw270.xpm"
#include "ccw270.xpm"

// brackets
#include "brackets.h"

// bonds
/* XPM */
static const char *upline_xpm[] = {
/* width height num_colors chars_per_pixel */
"    64    24        2            1",
/* colors */
". c #000000",
"# c #ffffff",
/* pixels */
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"##################################################.........#####",
"##################################.........................#####",
"##################.........................................#####",
"#########..................................................#####",
"######################.....................................#####",
"##############################################.............#####",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################"
};
/* XPM */
static const char *downline_xpm[] = {
/* width height num_colors chars_per_pixel */
"    64    24        2            1",
/* colors */
". c #000000",
"# c #ffffff",
/* pixels */
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"#############################################.###.###.###.######",
"#########################.###.###.###.###.###.###.###.###.######",
"#########.###.###.###.###.###.###.###.###.###.###.###.###.######",
"#########################.###.###.###.###.###.###.###.###.######",
"#############################################.###.###.###.######",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
"################################################################",
};
/* XPM */
static const char *wavytool[] =
{
"16 13 3 1",
"A c #040404040404",
"B c #808083830404",
"C c #FFFFFFFFFFFF",
"CCCCCCCCCCCCCCCC",
"CCCCCCCCCCCCCCCC",
"CCCCCCCCCCCCCCCC",
"CCCCCCCCCCCCCCCC",
"CCAACCCCAACCCCAC",
"CACCACCACCACCACC",
"CACCACCACCACCACC",
"CCCCACCACCACCACC",
"CCCCCAACCCCAACCC",
"CCCCCCCCCCCCCCCC",
"CCCCCCCCCCCCCCCC",
"CCCCCCCCCCCCCCCC",
"CCCCCCCCCCCCCCCC",
};

// symbols
#include "symbol_xpm.h"

PreviewWidget::PreviewWidget(QWidget *parent, const char *name)
  : QWidget(parent, name)
{
  resize(50,50);
}

void PreviewWidget::paintEvent(QPaintEvent *qp1) {
  int ofs = 2;
  QPainter p(this);

  if (type == TYPE_ARROW) {
    switch(style) {
    case ARROW_REGULAR:
      p.drawPixmap(10, 10, QPixmap(arrow_regular_xpm));
      break;
    case ARROW_DIDNT_WORK:
      p.drawPixmap(10, 10, QPixmap(arrow_didnt_work_xpm));
      break;
    case ARROW_DASH:
      p.drawPixmap(10, 10, QPixmap(arrow_dash_xpm));
      break;
    case ARROW_BI1:
      p.drawPixmap(10, 10, QPixmap(arrow_bi1_xpm));
      break;
    case ARROW_BI2:
      p.drawPixmap(10, 10, QPixmap(arrow_bi2_xpm));
      break;
    case ARROW_RETRO:
      p.drawPixmap(10, 10, QPixmap(arrow_retro_xpm));
      break;
    }
    return;
  }
  if (type == TYPE_BOND) {
    switch (_or) {
    case 1:
      if (da == 1) {
	p.setPen(QPen(QColor(0,0,0),th,DotLine));
	p.drawLine( QPoint(10,30), QPoint(60,30) );
      } else {
	p.setPen(QPen(QColor(0,0,0),th));
	p.drawLine( QPoint(10,30), QPoint(60,30) );
      }
      break;
    case 2:
      if (da == 1) {
	p.setPen(QPen(QColor(0,0,0),th,DotLine));
	p.drawLine( QPoint(10,30), QPoint(60,30) );
	p.setPen(QPen(QColor(0,0,0),th));
	p.drawLine( QPoint(10,30 + th + ofs), QPoint(60,30 + th + ofs) );
      } else {
	p.setPen(QPen(QColor(0,0,0),th));
	p.drawLine( QPoint(10,30), QPoint(60,30) );
	p.drawLine( QPoint(10,30 + th + ofs), QPoint(60,30 + th + ofs) );
      }
      break;	
    case 3:
      if (da == 1) {
	p.setPen(QPen(QColor(0,0,0),th,DotLine));
	p.drawLine( QPoint(10,30 - th - ofs), QPoint(60,30 - th - ofs) );
	p.setPen(QPen(QColor(0,0,0),th));
	p.drawLine( QPoint(10,30), QPoint(60,30) );
	p.drawLine( QPoint(10,30 + th + ofs), QPoint(60,30 + th + ofs) );
      } else {
	p.setPen(QPen(QColor(0,0,0),th));
	p.drawLine( QPoint(10,30 - th - ofs), QPoint(60,30 - th - ofs) );
	p.drawLine( QPoint(10,30), QPoint(60,30) );
	p.drawLine( QPoint(10,30 + th + ofs), QPoint(60,30 + th + ofs) );
      }
      break;
    case 5:
      p.drawPixmap(10, 10, QPixmap(upline_xpm));
      break;
    case 6:
      p.drawPixmap(10, 10, QPixmap(wavytool));
      break;
    case 7:
      p.drawPixmap(10, 10, QPixmap(downline_xpm));
      break;
    }
    return;
  }
  if (type == TYPE_BRACKET) {
    switch(style) {
    case BRACKET_SQUARE:
      p.drawPixmap(10, 10, QPixmap(squarebracket_xpm));
      break;
    case BRACKET_CURVE:
      p.drawPixmap(10, 10, QPixmap(curvebracket_xpm));
      break;
    case BRACKET_BRACE:
      p.drawPixmap(10, 10, QPixmap(bracebracket_xpm));
      break;
    case BRACKET_BOX:
      p.drawPixmap(10, 10, QPixmap(boxbracket_xpm));
      break;
    case BRACKET_ELLIPSE:
      p.drawPixmap(10, 10, QPixmap(ellipsebracket_xpm));
      break;
    }
    return;
  }
  if (type == TYPE_CURVEARROW) {
    switch(style) {
    case CA_CW90:
      p.drawPixmap(10, 10, QPixmap(cw90_xpm));
      break;
    case CA_CW180:
      p.drawPixmap(10, 10, QPixmap(cw180_xpm));
      break;
    case CA_CW270:
      p.drawPixmap(10, 10, QPixmap(cw270_xpm));
      break;
    case CA_CCW90:
      p.drawPixmap(10, 10, QPixmap(ccw90_xpm));
      break;
    case CA_CCW180:
      p.drawPixmap(10, 10, QPixmap(ccw180_xpm));
      break;
    case CA_CCW270:
      p.drawPixmap(10, 10, QPixmap(ccw270_xpm));
      break;
    }
    return;
  }
  if (type == TYPE_SYMBOL) {
    switch (style) {
    case SYM_PLUS:
      p.drawPixmap(10, 10, QPixmap(sym_plus_xpm));
      break;
    case SYM_MINUS:
      p.drawPixmap(10, 10, QPixmap(sym_minus_xpm));
      break;
    case SYM_DELTA_PLUS:
      p.drawPixmap(10, 10, QPixmap(sym_delta_plus_xpm));
      break;
    case SYM_DELTA_MINUS:
      p.drawPixmap(10, 10, QPixmap(sym_delta_minus_xpm));
      break;
    case SYM_1E:
      p.drawPixmap(10, 10, QPixmap(sym_1e_xpm));
      break;
    case SYM_2E:
      p.drawPixmap(10, 10, QPixmap(sym_2e_xpm));
      break;
    case SYM_RING_UP:
      p.drawPixmap(10, 10, QPixmap(sym_ring_up_xpm));
      break;
    case SYM_P_ORBITAL:
      p.drawPixmap(10, 8, QPixmap(p_orbital_xpm));
      break;
    case SYM_P_DOUBLE:
      p.drawPixmap(10, 8, QPixmap(p_double_xpm));
      break;
    case SYM_BEAD:
      p.drawPixmap(10, 8, QPixmap(bead_xpm));
      break;
    }
    return;
  }
}
