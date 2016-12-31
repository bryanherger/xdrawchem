/***************************************************************************
                          crings_dialog.h  -  Dialog for creating CustomRings
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

#ifndef _CRINGS_DIALOG_H
#define _CRINGS_DIALOG_H

#include <QFrame>
#include <QLabel>
#include <QLineEdit>
#include <QPixmap>
#include <QDialog>

#include "render2d.h"
#include "chemdata.h"
#include "molecule.h"

class CustomRingWidget : public QWidget
{
    Q_OBJECT

public:
    CustomRingWidget( QWidget* parent=0 );
    ~CustomRingWidget();

    QString getTitle() { return title->text(); }
    void setMolecule(Molecule* m);


public slots:
    void slotTitle( QString &string );

signals:
    void signalTextChanged( const QString & );

protected:
    void paintEvent( QPaintEvent * );

private:
    QFrame *widget;
    Render2D *render;
    ChemData *data;
    QLineEdit *title;
    QLabel *label;
    QPixmap pixmap;
};

class CustomRingDialog : public QDialog
{
    Q_OBJECT

public:
    CustomRingDialog( QWidget *parent=0 );
    ~CustomRingDialog();

    void setMolecule( Molecule *m ) { m_widget->setMolecule(m); }

public slots:
    void slotOk();
    void slotTextChanged( const QString &string );

signals:
    void signalTitle( QString & );

private:
    CustomRingWidget *m_widget;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
