#include <QFontDatabase>
#include <QLineEdit>
#include <QMouseEvent>
#include <QPainter>
#include <QDebug>

#include "charsel.h"
#include "defs.h"
#include "prefs.h"

extern Preferences preferences;

CharSelLabel::CharSelLabel( QWidget *parent )
    : QLabel( parent )
{
}

void CharSelLabel::mousePressEvent( QMouseEvent * e1 )
{
    int c1;

    c1 = ( int ) ( ( double ) e1->y() / 16.0 );
    c1 *= 32;
    c1 += ( int ) ( ( double ) e1->x() / 16.0 );

    emit thisChar( c1 );
}

CharSelDialog::CharSelDialog( QWidget *parent )
    : QDialog( parent )
{
    QFontDatabase qfd;
    QStringList families = qfd.families();
    int firstsym = -1, cc1 = -1;

    thischar = "spam";
    setWindowTitle( tr( "Insert symbol" ) );
    QLabel *l1 = new QLabel();

    l1->setGeometry( 10, 10, 80, 30 );
    l1->setText( tr( "Font:" ) );
    charfont = new QComboBox();
    charfont->setGeometry( 100, 10, 180, 30 );
    QString family;

    for ( QStringList::Iterator f = families.begin(); f != families.end(); ++f ) {
        cc1++;
        family = *f;
        charfont->addItem( family );
        if ( ( family.toLower().contains( "symbol" ) > 0 ) && ( firstsym < 0 ) )
            firstsym = cc1;
    }
    charfont->setCurrentIndex( firstsym );
    connect( charfont, SIGNAL( activated( int ) ), this, SLOT( fontChange( int ) ) );

    letters = new CharSelLabel( this );
    letters->setGeometry( 10, 50, 512, 128 );
    //letters->setText( "sample" );
    fontChange( firstsym );
    connect( letters, SIGNAL( thisChar( int ) ), this, SLOT( chosenChar( int ) ) );

    QPushButton *dismiss;

    dismiss = new QPushButton( tr( "Cancel" ), this );
    dismiss->setGeometry( 140, 220, 80, 30 );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
}

void CharSelDialog::fontChange( int n1 )
{
    symbolFont = QFont( charfont->currentText(), 12 );

    QPixmap p1( 512, 128 );

    p1.fill();

    int i, j;

    QPainter pp;
    QString l1;
    QChar qc1;
    char c1;

    pp.begin( &p1 );
    pp.setFont( symbolFont );
    for ( i = 0; i < 32; i++ ) {
        pp.drawLine( i * 16, 0, i * 16, 200 );
    }
    for ( i = 0; i < 8; i++ ) {
        pp.drawLine( 0, i * 16, 600, i * 16 );
    }
    for ( j = 0; j < 8; j++ ) {
        for ( i = 0; i < 32; i++ ) {
            c1 = ( char ) ( j * 32 + i );
            qc1 = c1;
            l1 = qc1;
            pp.drawText( i * 16 + 2, j * 16 + 12, l1 );
        }
    }
    pp.end();

    letters->setPixmap( p1 );
}

void CharSelDialog::chosenChar( int c1 )
{
    qDebug() << "charsel: " << c1;
    QString l1;
    QChar qc1;
    char cc1;

    cc1 = ( char ) c1;
    qc1 = cc1;
    thischar = qc1;

    QLineEdit *lined = new QLineEdit( this );

    lined->setText( thischar );
    lined->setFont( symbolFont );
    thischar = lined->text();

    delete lined;

    accept();
}

//cmake#include "charsel.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
