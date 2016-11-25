#ifndef COLOR_BUTTON_H
#define COLOR_BUTTON_H

#include <QPushButton>

class QColor;
class QPixmap;
class QStyleOptionButton;

class ColorButton : public QPushButton
{
    Q_OBJECT

public:
    ColorButton( QColor color = Qt::black, QWidget *parent = 0 );
    // default destructor

    void setColor( QColor color );

private:
    void paintEvent( QPaintEvent * );
    void initStyleOption(QStyleOptionButton* opt) const;

    QColor btnColor;
    QPixmap *colorIcon;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
