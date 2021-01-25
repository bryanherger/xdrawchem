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

#include <stdio.h>
#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

QString RingDir, HomeDir;
QTextStream out( stdout );
Preferences preferences;

void usage();                 // defined below

void handler(int sig) {
  void *array[10];
  size_t size;

  // get void*'s for all entries on the stack
  size = backtrace(array, 10);

  // print out all the frames to stderr
  fprintf(stderr, "Error: signal %d:\n", sig);
  backtrace_symbols_fd(array, size, STDERR_FILENO);
  exit(1);
}
int main( int argc, char **argv )
{
    signal(SIGSEGV, handler);   // install handler for segmentation faults

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

    QApplication a( argc, argv );

    // set library directory (RingDir = default RINGHOME)
    QString dname( RINGHOME );
    if ( dname.right( 1 ) != QString( "/" ) )
        dname.append( QString( "/" ) );
    //dname.append( "ring/" );

    qInfo() << "appDirPath::" << QApplication::applicationDirPath();
    QString altdname = QApplication::applicationDirPath();
    if (altdname.contains("Contents/MacOS")) {
        dname = altdname.replace("Contents/MacOS","Contents/Resources");
        if ( dname.right( 1 ) != QString( "/" ) )
                dname.append( QString( "/" ) );
    }
    if (altdname.contains("Program Files")) {
        dname = altdname;
        if ( dname.right( 1 ) != QString( "/" ) )
                dname.append( QString( "/" ) );
        dname.append( "ring/" );
    }
    qInfo() << "dname = " << dname;
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
