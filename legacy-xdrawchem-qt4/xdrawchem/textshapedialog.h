#ifndef TSDIALOG_H
#define TSDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

class TextShapeDialog : public QDialog
{
    Q_OBJECT

public:
    TextShapeDialog( QWidget *parent );
    void setBorderColor( QColor );
    void setFillColor( QColor );
    QColor getBorderColor() { return bc; }
    QColor getFillColor() { return fc; }
    void setBorderCheck( bool b1 ) { borderCheck->setChecked( b1 ); }
    void setFillCheck( bool b1 ) { fillCheck->setChecked( b1 ); }
    bool getBorderCheck() { return borderCheck->isChecked(); }
    bool getFillCheck() { return fillCheck->isChecked(); }
    void set_stype( int s1 ) { stype->setCurrentIndex( s1 ); }
    int get_stype() { return stype->currentIndex(); }

    void setWidth( int n1 )
    {
        QString nx;
        nx.setNum( n1 );
        swidth->setText( nx );
    }

    void setHeight( int n1 )
    {
        QString nx;
        nx.setNum(n1);
        sheight->setText(nx);
    }

    int getWidth() { return swidth->text().toInt(); }
    int getHeight() { return sheight->text().toInt(); }

public slots:
    void editFillColor();
    void editBorderColor();

private:
    QComboBox *stype;
    QPushButton *bbutton, *fbutton;
    QCheckBox *borderCheck, *fillCheck;
    QLineEdit *swidth, *sheight;
    QColor fc, bc;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
