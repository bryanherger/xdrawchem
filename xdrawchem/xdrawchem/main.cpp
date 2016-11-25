/****************************************************************************
** $Id: main.cpp 105 2005-02-09 00:59:36Z bherger $
**
** Copyright (C) 1992-2000 Trolltech AS.  All rights reserved.
**
** This file is part of an example program for Qt.  This example
** program may be used, distributed and modified without limitation.
**
*****************************************************************************/

#include <qapplication.h>
#include <qnetwork.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qfile.h>
#include <qtimer.h>
#ifdef UNIX
#include <stdlib.h>
#include <unistd.h>
#endif
//#include <time.h>
#include <qtranslator.h>
#include <qtextcodec.h>
#include <iostream>

using std::cerr;
using std::endl;

#ifdef WIN32
#include <malloc.h>
#include <direct.h>
#endif

#include "application.h"
#include "clipboard.h"
#include "defs.h"
#include "prefs.h"
#include "dyk.h"

QString RingDir, HomeDir;
Preferences preferences;

void usage(); // defined below

int main( int argc, char ** argv ) {
  int ae;

  // parse command line options
  QStringList cmds;
  QString infile, outfile;
  bool loadflag = false, pngflag = false, quitflag = false, helpflag = false, 
    versionflag = false, tflag = false, to3dflag = false;
  for (int c1 = 0; c1 < argc; c1++) {
    cmds.append(argv[c1]);
  }
  if (cmds.count() > 1) {
    for ( QStringList::Iterator it = cmds.begin(); it != cmds.end(); ++it ) {
        cout << *it << ":";
	if (*it == "-h") { helpflag = true; continue; }
	if (*it == "--help") { helpflag = true; continue; }
	if (*it == "-v") { versionflag = true; continue; }
	if (*it == "--version") { versionflag = true; continue; }
	if (*it == "-t") { tflag = true; continue; }
	if (*it == "-png") {
	  pngflag = true;
	  it++;
	  outfile = *it;
	  continue;
	}
	if (*it == "-3d") {
	  to3dflag = true;
	  it++;
	  outfile = *it;
	  continue;
	}
	loadflag = true; infile = *it;
    } 
    cout << endl;
  }

  if (helpflag) usage();
  if (versionflag) {
    cout << XDC_VERSION << endl;
    exit(0);
  }

  // set library directory (RINGDIR)
#ifdef UNIX
  QString dname(RINGHOME);
  if (dname.right(1) != QString("/"))
    dname.append(QString("/"));
#else
  char *pwd = (char*)malloc(sizeof(char) * 80);
  _getcwd(pwd, 80);
  QString dname;
  dname = pwd;
  dname.append("\\ring\\");
#endif
  RingDir = dname;

  // set home directory/pref file and fallback dir/pref file
#ifdef UNIX
  HomeDir = getenv("HOME");
  QString cRingDir = HomeDir;
  cRingDir.append("/.xdrawchem/");
  preferences.setCustomRingDir(cRingDir);
  HomeDir = HomeDir + "/.xdrawchemrc";
  preferences.setSaveFile(HomeDir);
  QFile f1(HomeDir);
  if (f1.open(IO_ReadOnly) == false) {
    HomeDir = RingDir + "xdrawchemrc";
    preferences.setFile(HomeDir, true);
  } else {
    f1.close();
    preferences.setFile(HomeDir, false);
  }
#else  // Windows, Mac?
  HomeDir = "xdrawchemrc";
  preferences.setCustomRingDir(RingDir);
  preferences.setSaveFile(HomeDir);
  QFile f1(HomeDir);
  if (f1.open(IO_ReadOnly) == false) {
    HomeDir = RingDir + "xdrawchemrc";
    preferences.setFile(HomeDir, true);
  } else {
    f1.close();
    preferences.setFile(HomeDir, false);
  }
#endif

  QApplication a( argc, argv );
  qInitNetworkProtocols();
  if (preferences.LoadPrefs() == false) {
    std::cerr << "Unable to load preferences file" << endl;
    preferences.Defaults();
  }

  // translation file for application strings
  QTranslator myapp( 0 );
  myapp.load( QString( "xdrawchem_" ) + QTextCodec::locale(), RingDir );
  a.installTranslator( &myapp );

  ApplicationWindow * mw = new ApplicationWindow;
  mw->setCaption( QString(XDC_VERSION) + QString(" - ") + 
		  mw->tr("untitled") );
  if (loadflag)
    mw->load(infile);
  mw->show();
  if (pngflag) {
    mw->ni_savefile = outfile;
    mw->ni_tflag = tflag;
    QTimer::singleShot(0, mw, SLOT(SavePNG()) );
  }
  if (to3dflag) {
    mw->ni_savefile = outfile;
    mw->ni_tflag = tflag;
    QTimer::singleShot(0, mw, SLOT(Save3D()) );
  }

  if (quitflag) exit(0);  // exit if non-interactive mode.

  a.connect( &a, SIGNAL(lastWindowClosed()), &a, SLOT(quit()) );
  mw->HideTextButtons();

  if (preferences.getDYK()) {
    DYKDialog dyk1;
    dyk1.exec();
  }

  ae = a.exec();
  if (preferences.SavePrefs() == false) {
    std::cerr << "Unable to save preferences file" << endl;
  }
  return ae;
}

void usage() {
  cout << XDC_VERSION << endl << endl;
  cout << "Usage:" << endl << endl;
  cout << "  xdrawchem [input file] [options]" << endl << endl;
  cout << "Command line options:" << endl;
  cout << "-png <output file>: Create a PNG image of input file and exit" << endl;
  cout << "-t: Make transparent PNG (also specify -png)" << endl;
  cout << "-3d: Make 3D model of input file (output MDL mofile)" << endl;
  cout << "-h, --help:  Display this help" << endl;
  cout << "-v, --version:  Display the version information" << endl;
  
  exit(0);
}

int my_round(double d1) {
  int i1 = (int)d1;
  int i2, s1;
  if (d1 < 0.0)
    s1 = -1;
  else
    s1 = 1;
  double d2 = d1 - (double)i1;
  if ( fabs(d2) < 0.5 )
    i2 = i1;
  else
    i2 = i1 + s1;
  //std::cout << "my_lround(" << d1 << ") = " <<d2 << "->" << i2 << std::endl;
  return i2;
}
