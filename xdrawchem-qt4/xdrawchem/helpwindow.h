/****************************************************************************
** $Id: helpwindow.h,v 1.1.1.1 2004/01/24 17:15:49 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#ifndef HELPWINDOW_H
#define HELPWINDOW_H

#include <QMainWindow>

class QComboBox;
class QTextBrowser;
class QUrl;

class HelpWindow : public QMainWindow
{
    Q_OBJECT

public:
    HelpWindow( const QString& home_,  const QString& path, QWidget* parent = 0 );
    ~HelpWindow();

private slots:
    void setBackwardAvailable( bool );
    void setForwardAvailable( bool );

    void sourceChanged(const QUrl &);
    void about();
    void print();

    void pathSelected( const QString & );

private:
    QTextBrowser* browser;
    QComboBox *pathCombo;
    QAction *backwardAction, *forwardAction, *homeAction;
    QString selectedURL;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;

