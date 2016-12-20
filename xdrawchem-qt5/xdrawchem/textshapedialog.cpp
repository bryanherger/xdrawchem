#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QColorDialog>

#include "textshapedialog.h"
#include "defs.h"

TextShapeDialog::TextShapeDialog( QWidget * parent )
    : QDialog( parent )
{
    QGridLayout *tsgl = new QGridLayout( this );

    setWindowTitle( tr( "Edit text shape" ) );

    stype = new QComboBox();
    stype->addItem( tr( "No shape" ) );
    stype->addItem( tr( "Ellipse" ) );
    stype->addItem( tr( "Rectangle" ) );
    stype->addItem( tr( "Circle" ) );
    stype->addItem( tr( "Square" ) );
    tsgl->addWidget( stype, 0, 0 );

    borderCheck = new QCheckBox();
    borderCheck->setText( tr( "Border" ) );
    tsgl->addWidget( borderCheck, 1, 0 );
    bbutton = new QPushButton( this );
    bbutton->setText( tr( "Border color" ) );
    tsgl->addWidget( bbutton, 1, 1 );
    connect( bbutton, SIGNAL( clicked() ), SLOT( editBorderColor() ) );

    fillCheck = new QCheckBox();
    fillCheck->setText( tr( "Fill" ) );
    tsgl->addWidget( fillCheck, 2, 0 );
    fbutton = new QPushButton( this );
    fbutton->setText( tr( "Fill color" ) );
    tsgl->addWidget( fbutton, 2, 1 );
    connect( fbutton, SIGNAL( clicked() ), SLOT( editFillColor() ) );

    QLabel *lw1, *lh1;

    lw1 = new QLabel();
    lw1->setText( tr( "Width:" ) );
    tsgl->addWidget( lw1, 3, 0 );
    lh1 = new QLabel();
    lh1->setText( tr( "Height:" ) );
    tsgl->addWidget( lh1, 3, 1 );

    swidth = new QLineEdit();
    tsgl->addWidget( swidth, 4, 0 );

    sheight = new QLineEdit();
    tsgl->addWidget( sheight, 4, 1 );

    QPushButton *ok, *dismiss;

    ok = new QPushButton( tr( "OK" ) );
    tsgl->addWidget( ok, 5, 0 );
    connect( ok, SIGNAL( clicked() ), SLOT( accept() ) );
    dismiss = new QPushButton( tr( "Cancel" ) );
    tsgl->addWidget( dismiss, 5, 1 );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
}

void TextShapeDialog::editBorderColor()
{
    QColor c1;

    c1 = QColorDialog::getColor( bc );

    bc = c1;
}

void TextShapeDialog::editFillColor()
{
    QColor c1;

    c1 = QColorDialog::getColor( fc );

//    fbutton->setPaletteBackgroundColor( c1 );
    fc = c1;
}

void TextShapeDialog::setBorderColor( QColor c1 )
{
    bc = c1;
}

void TextShapeDialog::setFillColor( QColor c1 )
{
    fc = c1;
}

//cmake#include "textshapedialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
