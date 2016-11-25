/****************************************************************************
** $Id: main.cpp,v 1.6 2005/02/09 00:59:36 bherger Exp $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <QApplication>
#include <QString>
#include <QTextStream>
#include <QLocale>
#include <QTranslator>
#include <QTimer>

#include "application.h"
#include "clipboard.h"
#include "defs.h"
#include "prefs.h"
#include "dyk.h"

QString RingDir, HomeDir;
QTextStream out( stdout );
Preferences preferences;

void usage();                 // defined below

int main( int argc, char **argv )
{
    int ae;

    // parse command line options
    QStringList cmds;
    QString infile, outfile;
    bool loadflag = false, pngflag = false, quitflag = false, helpflag = false, versionflag = false, tflag = false, to3dflag = false;

    for ( int c1 = 0; c1 < argc; c1++ ) {
        cmds.append( argv[c1] );
    }
    if ( cmds.count() > 1 ) {
        foreach ( QString arg, cmds ) {
            qDebug() << arg << ":";
            if ( arg == "-h" ) {
                helpflag = true;
                continue;
            }
            if ( arg == "--help" ) {
                helpflag = true;
                continue;
            }
            if ( arg == "-v" ) {
                versionflag = true;
                continue;
            }
            if ( arg == "--version" ) {
                versionflag = true;
                continue;
            }
            if ( arg == "-t" ) {
                tflag = true;
                continue;
            }
            if ( arg == "-png" ) {
                pngflag = true;
                outfile = arg;
                continue;
            }
            if ( arg == "-3d" ) {
                to3dflag = true;
                outfile = arg;
                continue;
            }
            loadflag = true;
            infile = arg;
        }
    }

    if ( helpflag )
        usage();
    if ( versionflag ) {
        out << XDC_VERSION << endl;
        exit( 0 );
    }
    // set library directory (RINGDIR)
#ifdef UNIX
    QString dname( "/usr/local/share/xdrawchem2" );

    if ( dname.right( 1 ) != QString( "/" ) )
        dname.append( QString( "/" ) );
#else
    char *pwd = ( char * ) malloc( sizeof( char ) * 80 );

    _getcwd( pwd, 80 );
    QString dname;

    dname = pwd;
    dname.append( "\\ring\\" );
#endif
    RingDir = dname;

    // set home directory/pref file and fallback dir/pref file
#ifdef UNIX
    HomeDir = getenv( "HOME" );
    QString cRingDir = HomeDir;

    cRingDir.append( "/.xdrawchem/" );
    preferences.setCustomRingDir( cRingDir );
    HomeDir = HomeDir + "/.xdrawchemrc";
    preferences.setSaveFile( HomeDir );
    QFile f1( HomeDir );

    if ( f1.open( QIODevice::ReadOnly ) == false ) {
        HomeDir = RingDir + "xdrawchemrc";
        preferences.setFile( HomeDir, true );
    } else {
        f1.close();
        preferences.setFile( HomeDir, false );
    }
#else // Windows, Mac?
    HomeDir = "xdrawchemrc";
    preferences.setCustomRingDir( RingDir );
    preferences.setSaveFile( HomeDir );
    QFile f1( HomeDir );

    if ( f1.open( QIODevice::ReadOnly ) == false ) {
        HomeDir = RingDir + "xdrawchemrc";
        preferences.setFile( HomeDir, true );
    } else {
        f1.close();
        preferences.setFile( HomeDir, false );
    }
#endif

    QApplication a( argc, argv );

    if ( preferences.LoadPrefs() == false ) {
        qWarning() << "Unable to load preferences file";
        preferences.Defaults();
    }
    // translation file for application strings
    QTranslator translator;

    translator.load( QString::fromLatin1( "xdrawchem_" ) + QLocale::system().name(), RingDir );
    a.installTranslator( &translator );

    ApplicationWindow *mw = new ApplicationWindow;

    mw->setWindowTitle( QString( XDC_VERSION ) + QString( " - " ) + mw->tr( "untitled" ) );
    if ( loadflag )
        mw->load( infile );
    mw->show();
    if ( pngflag ) {
        mw->ni_savefile = outfile;
        mw->ni_tflag = tflag;
        QTimer::singleShot( 0, mw, SLOT( savePNG() ) );
    }
    if ( to3dflag ) {
        mw->ni_savefile = outfile;
        mw->ni_tflag = tflag;
        QTimer::singleShot( 0, mw, SLOT( save3D() ) );
    }

    if ( quitflag )
        exit( 0 );              // exit if non-interactive mode.

    a.connect( &a, SIGNAL( lastWindowClosed() ), &a, SLOT( quit() ) );
    mw->HideTextButtons();

    if ( preferences.getDYK() ) {
        DYKDialog dyk1;

        dyk1.exec();
    }

    ae = a.exec();
    if ( preferences.SavePrefs() == false ) {
        qWarning() << "Unable to save preferences file";
    }
    return ae;
}

void usage()
{
    out << XDC_VERSION << endl;
    out << "Usage:" << endl;
    out << "  xdrawchem [input file] [options]" << endl;
    out << "Command line options:" << endl;
    out << "-png <output file>: Create a PNG image of input file and exit" << endl;
    out << "-t: Make transparent PNG (also specify -png)" << endl;
    out << "-3d: Make 3D model of input file (output MDL mofile)" << endl;
    out << "-h, --help:  Display this help" << endl;
    out << "-v, --version:  Display the version information" << endl;

    exit( 0 );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
