/***************************************************************************
                          crings_dialog.cpp  -  Dialog for Custom Rigns
                             -------------------
    begin                : Sat Aug 31 2002
    copyright            : (C) 2002 by Adam Tenderholt
    email                : atenderholt@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include <QGridLayout>
#include <QSpacerItem>
#include <QPainter>

#include "crings_dialog.h"
#include "defs.h"
#include "prefs.h"

extern Preferences preferences;

CustomRingWidget::CustomRingWidget( QWidget *parent )
    : QWidget( parent )
{
    QGridLayout *layout = new QGridLayout();
    setLayout( layout );
    layout->setMargin( 0 );

    setMinimumSize( 320, 310 );

    render = new Render2D();
    data = new ChemData( this );

    widget = new QFrame();
    widget->setFrameStyle( QFrame::Box );
    layout->addWidget( widget, 0, 0, 1, 2 );
    widget->show();

    render->setChemData( data );
    data->setRender2D( render );

    label = new QLabel( tr( "Title: " ) );
    layout->addWidget( label, 1, 0, 1, 1 );

    title = new QLineEdit();
    layout->addWidget( title, 1, 1, 1, 1 );

    connect( title, SIGNAL( textChanged( const QString & ) ), this, SIGNAL( signalTextChanged( const QString & ) ) );

    //render->setMode_SelectNone();

}

CustomRingWidget::~CustomRingWidget()
{

    if ( render )
        delete render;

    if ( data )
        delete data;

    if ( title )
        delete title;

    if ( label )
        delete label;

}

void CustomRingWidget::slotTitle( QString & string )
{

    data->save( string );
    qDebug() << "saved: " << string << endl << "done with crwidget";
    string.truncate( string.length() - 4 );
    string.append( ".png" );
    qDebug() << "png: " << string;
    pixmap.save( string, "PNG" );
}

void CustomRingWidget::setMolecule( Molecule *m )
{
    data->addMolecule( m );
    Render2D *real_render = m->getRender2D();

    //QPixmap pixmap=real_render->MakePixmap();

    QPixmap mol = real_render->MakeFullPixmap();
    QRect rr1 = m->BoundingBoxAll();

    rr1.setLeft( rr1.left() - 4 );
    rr1.setTop( rr1.top() - 4 );
    rr1.setBottom( rr1.bottom() + 4 );
    rr1.setRight( rr1.right() + 4 );
//    QPixmap mol1( rr1.size() );

//    bitBlt( &mol1, QPoint( 0, 0 ), &mol, rr1 );

//    pixmap = mol1;
    //pixmap.fill(Qt::white);
    //if(pixmap.isNull())
    //cerr << "bad pixmpa!!!" ;
    //m->SelectAll();
    //QRect rect=m->BoundingBox();

    QPainter painter( widget );

    //painter.drawPixmap(QPoint(0,0),pixmap,QRect(0,0,rect.width(),rect.height()));
    //cerr << rect.x() << " " << rect.y() << " " << rect.width() << " " ;
    //bitBlt(widget,QPoint(0,0),&pixmap,rect);
    painter.drawPixmap( QPoint( 0, 0 ), mol, rr1 );
    widget->show();
}

void CustomRingWidget::paintEvent( QPaintEvent * pe )
{
    QPainter p( widget );

    p.drawPixmap( QPoint( 0, 0 ), pixmap );
}

CustomRingDialog::CustomRingDialog( QWidget * parent )
:  QDialog( parent )
{
    setWindowTitle( tr( "Add custom ring to menu list" ) );

    QGridLayout *mygrid = new QGridLayout( this );

    m_widget = new CustomRingWidget( this );
    mygrid->addWidget( m_widget, 0, 0, 1, 3 );

    connect( m_widget, SIGNAL( signalTextChanged( const QString & ) ), this, SLOT( slotTextChanged( const QString & ) ) );

    connect( this, SIGNAL( signalTitle( QString & ) ), m_widget, SLOT( slotTitle( QString & ) ) );

    //disableResize();
    //enableButtonOK(false);


    QSpacerItem *spacer = new QSpacerItem( 1, 1, QSizePolicy::Expanding, QSizePolicy::Minimum );

    mygrid->addItem( spacer, 2, 0 );

    QPushButton *ok, *dismiss;

    ok = new QPushButton( tr( "OK" ), this );
    connect( ok, SIGNAL( clicked() ), SLOT( slotOk() ) );
    mygrid->addWidget( ok, 2, 1 );

    dismiss = new QPushButton( tr( "Cancel" ), this );
    connect( dismiss, SIGNAL( clicked() ), SLOT( reject() ) );
    mygrid->addWidget( dismiss, 2, 2 );
}

CustomRingDialog::~CustomRingDialog()
{

    if ( m_widget )
        delete m_widget;
}

void CustomRingDialog::slotTextChanged( const QString & string )
{

    //if(string.isEmpty())
    //  enableButtonOK(false);
    //else //if string!=QString::null
    //  enableButtonOK(true);
}

void CustomRingDialog::slotOk()
{

    QString title = m_widget->getTitle();

    qDebug() << title;
    QString filename = title.toLower();

    filename.append( ".cml" );

    QString file = preferences.getCustomRingDir();

    file += filename;

    emit signalTitle( file );

    accept();
}

//cmake#include "crings_dialog.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
