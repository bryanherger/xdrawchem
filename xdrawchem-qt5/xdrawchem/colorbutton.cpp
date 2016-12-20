//
// C++ Implementation: colorbutton
//
// Description:
//
//
// Author: Gerd Fleischer <gerdfleischer@gmx.de>, (C) 2006
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <QColor>
#include <QRect>
#include <QPixmap>
#include <QIcon>
#include <QPainter>
#include <QStyle>
#include <QStyleOptionButton>
#include <qdrawutil.h>

#include "colorbutton.h"

ColorButton::ColorButton( QColor color, QWidget *parent )
    : QPushButton( parent )
{
    btnColor = color;
    setColor( btnColor );
}

void ColorButton::paintEvent( QPaintEvent * )
{
  QPainter painter(this);

  //First, we need to draw the bevel.
  QStyleOptionButton butOpt;
  initStyleOption(&butOpt);
  style()->drawControl( QStyle::CE_PushButtonBevel, &butOpt, &painter, this );

  //OK, now we can muck around with drawing out pretty little color box
  //First, sort out where it goes
  QRect labelRect = style()->subElementRect( QStyle::SE_PushButtonContents,
      &butOpt, this );
  int shift = style()->pixelMetric( QStyle::PM_ButtonMargin );
  labelRect.adjust(shift, shift, -shift, -shift);
  int x, y, w, h;
  labelRect.getRect(&x, &y, &w, &h);

  if (isChecked() || isDown()) {
    x += style()->pixelMetric( QStyle::PM_ButtonShiftHorizontal );
    y += style()->pixelMetric( QStyle::PM_ButtonShiftVertical   );
  }

  QColor fillCol = isEnabled() ? btnColor : palette().color(backgroundRole());
  qDrawShadePanel( &painter, x, y, w, h, palette(), true, 1, NULL);
  if ( fillCol.isValid() )
    painter.fillRect( x+1, y+1, w-2, h-2, fillCol );

  if ( hasFocus() ) {
    QRect focusRect = style()->subElementRect( QStyle::SE_PushButtonFocusRect, &butOpt, this );
    QStyleOptionFocusRect focusOpt;
    focusOpt.init(this);
    focusOpt.rect            = focusRect;
    focusOpt.backgroundColor = palette().background().color();
    style()->drawPrimitive( QStyle::PE_FrameFocusRect, &focusOpt, &painter, this );
  }
}

void ColorButton::initStyleOption(QStyleOptionButton* opt) const
{
    opt->init(this);
    opt->text.clear();
    opt->icon = QIcon();
    opt->features = QStyleOptionButton::None;
}

void ColorButton::setColor( QColor color )
{
    btnColor = color;
    colorIcon = new QPixmap( width(), height() );
    colorIcon->fill( btnColor );
    setIcon( QIcon( *colorIcon ) );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
