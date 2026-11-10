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
#include <QGuiApplication>
#include <QStyleFactory>
#include <QPalette>
#include <QString>
#include <QTextStream>
#include <QLocale>
#include <QTranslator>
#include <QTimer>

#include "application.h"
#include "clipboard.h"
#include "xdc_logging.h"
#include "xdc_api.h"

#include "defs.h"
#include "prefs.h"
#include "dyk.h"

#include <cstdio>
#if !defined(_WIN32) && !defined(_WIN64)
#include <execinfo.h>
#endif
#include <signal.h>
#include <stdlib.h>
#if !defined(_WIN32) && !defined(_WIN64)
#include <unistd.h>
#endif

QString RingDir, HomeDir;
QTextStream out( stdout );
Preferences preferences;

void usage();                 // defined below

// Headless agent API: defined in xdc_api_cli.cpp.  Returns an exit code.
int apiMain( const QStringList &args );

#if !defined(_WIN32) && !defined(_WIN64)
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
#endif
int main( int argc, char **argv )
{
#if !defined(_WIN32) && !defined(_WIN64)
    signal(SIGSEGV, handler);   // install handler for segmentation faults
#endif

    int ae;

    // ── Headless agent API ─────────────────────────────────────────────────
    // `xdrawchem --api <command> [--key value ...]` prints JSON and exits.
    // Handled before any GUI setup so scripts and MCP servers never flash a
    // window; the offscreen platform keeps Qt pixmap painting headless.
    {
        QStringList argl;
        for ( int i = 1; i < argc; ++i )
            argl.append( QString::fromLocal8Bit( argv[i] ) );
        int apiIdx = argl.indexOf( QStringLiteral( "--api" ) );
        if ( apiIdx >= 0 ) {
            for ( int i = 0; i < apiIdx; ++i ) {
                if ( argl.at( i ) == QStringLiteral( "--help" )
                     || argl.at( i ) == QStringLiteral( "-h" ) ) {
                    usage();
                    return 0;
                }
            }
            // QApplication (not QGuiApplication): the API builds Render2D,
            // which is a QWidget, so the widgets module must be initialised.
            // The offscreen platform keeps it headless — no window appears.
            if ( qEnvironmentVariableIsEmpty( "QT_QPA_PLATFORM" ) )
                qputenv( "QT_QPA_PLATFORM", "offscreen" );
            QApplication app( argc, argv );
            return apiMain( argl.mid( apiIdx + 1 ) );
        }
    }

    // parse command line options
    QStringList cmds;
    QString infile, outfile;
    bool loadflag = false, pngflag = false, quitflag = false, helpflag = false, versionflag = false, tflag = false, to3dflag = false;

    for ( int c1 = 0; c1 < argc; c1++ ) {
        cmds.append( argv[c1] );
    }
    if ( cmds.count() > 1 ) {
        for (const auto &arg : cmds) {
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
        out << XDC_VERSION << Qt::endl;
        exit( 0 );
    }

    QApplication a( argc, argv );

    // Force Fusion style with a standard light palette so the application
    // always looks consistent regardless of the system color scheme
    // (Qt6 on Linux will otherwise follow the Gnome/KDE dark/light setting).
    // Users can override this at launch with: -style <name>
    // or by setting QT_STYLE_OVERRIDE in the environment.
    if ( qEnvironmentVariableIsEmpty( "QT_STYLE_OVERRIDE" ) &&
         !QCoreApplication::arguments().contains( "-style" ) ) {
        QApplication::setStyle( QStyleFactory::create( "Fusion" ) );
        // Build an explicit light palette so dark-mode desktops don't
        // bleed through even with Fusion selected.
        QPalette lightPalette;
        lightPalette.setColor( QPalette::Window,          QColor( 240, 240, 240 ) );
        lightPalette.setColor( QPalette::WindowText,      Qt::black );
        lightPalette.setColor( QPalette::Base,            Qt::white );
        lightPalette.setColor( QPalette::AlternateBase,   QColor( 233, 233, 233 ) );
        lightPalette.setColor( QPalette::ToolTipBase,     Qt::white );
        lightPalette.setColor( QPalette::ToolTipText,     Qt::black );
        lightPalette.setColor( QPalette::Text,            Qt::black );
        lightPalette.setColor( QPalette::Button,          QColor( 240, 240, 240 ) );
        lightPalette.setColor( QPalette::ButtonText,      Qt::black );
        lightPalette.setColor( QPalette::BrightText,      Qt::red );
        lightPalette.setColor( QPalette::Link,            QColor( 0, 0, 255 ) );
        lightPalette.setColor( QPalette::Highlight,       QColor( 0, 120, 215 ) );
        lightPalette.setColor( QPalette::HighlightedText, Qt::white );
        QApplication::setPalette( lightPalette );
    }

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

    if ( !translator.load( QString::fromLatin1( "xdrawchem_" ) + QLocale::system().name(), RingDir ) ) {
        qCDebug(lcRender) << "Translator load failed for" << QLocale::system().name();
    }
    a.installTranslator( &translator );

    ApplicationWindow *mw = new ApplicationWindow;

    mw->setWindowTitle( QString( XDC_VERSION ) + QString( " - " ) + mw->tr( "untitled" ) );
    if ( loadflag )
        mw->load( infile );
    mw->show();
    if ( pngflag ) {
        mw->ni_savefile = outfile;
        mw->ni_tflag = tflag;
        QTimer::singleShot( 0, mw, &ApplicationWindow::savePNG );
    }
    if ( to3dflag ) {
        mw->ni_savefile = outfile;
        mw->ni_tflag = tflag;
        QTimer::singleShot( 0, mw, &ApplicationWindow::save3D );
    }

    if ( quitflag )
        exit( 0 );              // exit if non-interactive mode.

    a.connect( &a, &QGuiApplication::lastWindowClosed, &a, &QApplication::quit );
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
    out << XDC_VERSION << Qt::endl;
    out << "Usage:" << Qt::endl;
    out << "  xdrawchem [input file] [options]" << Qt::endl;
    out << "Command line options:" << Qt::endl;
    out << "-png <output file>: Create a PNG image of input file and exit" << Qt::endl;
    out << "-t: Make transparent PNG (also specify -png)" << Qt::endl;
    out << "-3d: Make 3D model of input file (output MDL mofile)" << Qt::endl;
    out << "--api: Headless JSON API for agents and scripts." << Qt::endl;
    out << "       xdrawchem --api                 -> command manifest" << Qt::endl;
    out << "       xdrawchem --api describe --structure CCO" << Qt::endl;
    out << "-h, --help:  Display this help" << Qt::endl;
    out << "-v, --version:  Display the version information" << Qt::endl;

    exit( 0 );
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
