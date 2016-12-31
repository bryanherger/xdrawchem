// to3d.cpp - Molecule's implementation of (more) functions
// this code derives from BUILD3D supplied by Thomas Shattuck

#include <stdlib.h>
#include <qobject.h>
#include <qptrlist.h>
#include <qstring.h>
#include <qmessagebox.h>
#include <iostream>
#include <math.h>
#include <qdir.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qtextstream.h>

using std::cout;
using std::endl;

#include "graphdialog.h"
#include "molinfodialog.h"
#include "moldata.h"
#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "dpoint.h"
#include "defs.h"
#include "netaccess.h"
//#include "namer.h"

// Preferences
extern Preferences preferences;

/*
 * The following code was obtained from:
 * http://astronomy.swin.edu.au/~pbourke/geometry/rotate/source.c
 * Code attributed to Ron Goldman
 * Web page describing this solution:
 * http://astronomy.swin.edu.au/~pbourke/geometry/rotate/
 * Web page written by Paul Bourke, December 1992, updated August 2002
 */

/*
   Rotate a point p by angle theta around an arbitrary axis r
   Return the rotated point.
   Positive angles are anticlockwise looking down the axis
   towards the origin.
   Assume right hand coordinate system.
*/
void Normalise(DPoint* n1) {
  double dst = sqrt( (n1->x3*n1->x3) + (n1->y3*n1->y3) + (n1->z3*n1->z3) );
  n1->x3 /= dst;
  n1->y3 /= dst;
  n1->z3 /= dst;
}

void Translate(DPoint* n1, double dx, double dy, double dz) {
  n1->x3 += dx;
  n1->y3 += dy;
  n1->z3 += dz;
}

DPoint ArbitraryRotate(DPoint p,double theta,DPoint r)
{
   DPoint q;
   double costheta,sintheta;

   Normalise(&r);
   costheta = cos(theta);
   sintheta = sin(theta);

   q.x3 += (costheta + (1 - costheta) * r.x3 * r.x3) * p.x3;
   q.x3 += ((1 - costheta) * r.x3 * r.y3 - r.z3 * sintheta) * p.y3;
   q.x3 += ((1 - costheta) * r.x3 * r.z3 + r.y3 * sintheta) * p.z3;

   q.y3 += ((1 - costheta) * r.x3 * r.y3 + r.z3 * sintheta) * p.x3;
   q.y3 += (costheta + (1 - costheta) * r.y3 * r.y3) * p.y3;
   q.y3 += ((1 - costheta) * r.y3 * r.z3 - r.x3 * sintheta) * p.z3;

   q.z3 += ((1 - costheta) * r.x3 * r.z3 - r.y3 * sintheta) * p.x3;
   q.z3 += ((1 - costheta) * r.y3 * r.z3 + r.x3 * sintheta) * p.y3;
   q.z3 += (costheta + (1 - costheta) * r.z3 * r.z3) * p.z3;

   return(q);
}

/*
   Rotate a point p by angle theta around an arbitrary line segment p1-p2
   Return the rotated point.
   Positive angles are anticlockwise looking down the axis
   towards the origin.
   Assume right hand coordinate system.  
*/
DPoint ArbitraryRotate2(DPoint p,double theta,DPoint p1,DPoint p2)
{
   DPoint q;
   double costheta,sintheta;
   DPoint r;

   r.x3 = p2.x3 - p1.x3;
   r.y3 = p2.y3 - p1.y3;
   r.z3 = p2.z3 - p1.z3;
   p.x3 -= p1.x3;
   p.y3 -= p1.y3;
   p.z3 -= p1.z3;
   Normalise(&r);

   costheta = cos(theta);
   sintheta = sin(theta);

   q.x3 += (costheta + (1 - costheta) * r.x3 * r.x3) * p.x3;
   q.x3 += ((1 - costheta) * r.x3 * r.y3 - r.z3 * sintheta) * p.y3;
   q.x3 += ((1 - costheta) * r.x3 * r.z3 + r.y3 * sintheta) * p.z3;

   q.y3 += ((1 - costheta) * r.x3 * r.y3 + r.z3 * sintheta) * p.x3;
   q.y3 += (costheta + (1 - costheta) * r.y3 * r.y3) * p.y3;
   q.y3 += ((1 - costheta) * r.y3 * r.z3 - r.x3 * sintheta) * p.z3;

   q.z3 += ((1 - costheta) * r.x3 * r.z3 - r.y3 * sintheta) * p.x3;
   q.z3 += ((1 - costheta) * r.y3 * r.z3 + r.x3 * sintheta) * p.y3;
   q.z3 += (costheta + (1 - costheta) * r.z3 * r.z3) * p.z3;

   q.x3 += p1.x3;
   q.y3 += p1.y3;
   q.z3 += p1.z3;
   return(q);
}

/*
 * end arbitrary rotation code
 */

// invoked from tool_2d3d.cpp...
// Create Undo() point before executing  :)
// actually, we're exporting this to BUILD3D...
void Molecule::Make3DVersion(QString fn3d) {
  // add NMR protons
  up = AllPoints();
  AddNMRprotons();

  // find rings (esp. find aromaticity) - do after CopyTextToDPoint()
  MakeSSSR();

  // get list of unique points
  up = AllPoints();
  AllNeighbors();

  // create input data
  //t natoms = up.count();
  QStringList atomEntries;
  QString atomEntry, s1;
  DPoint *tmp_pt_1, *tmp_pt_2;
  QPtrListIterator<DPoint> it1(up);
  int lc = 0;

  // let's not abandon the idea of doing it ourselves...
  this_sssr.PrintSSSR();
  DPoint *first_pt, *curr_pt;
  QPtrList<DPoint> placelist;
  double RMS = 999.0, RMSok;
  int i = 0, j = 0, chg = 0, iter = 0;
  int nplaced, ncount;
  bool inring = false, finished = true;
  double vecx, vecy, vecz, dst, dstmin, dstmax;
  QPtrListIterator<DPoint> it(up);

  // serialize, and set initial positions
  srand( (int)time(0) );

  i = 1;  // BUILD3D expects the first atom to be 1
  for ( it.toFirst(); it.current(); ++it ) {
    tmp_pt = it.current();
    tmp_pt->serial = i; i++;  // serialize
    tmp_pt->new_order = 0;  // we'll use this as a status flag
    tmp_pt->hit = false;
  }

  QString n1, buildpath, buildfile, xyzfile;
  QTextStream tout(buildfile, IO_WriteOnly);

  int an, nc = 0, nh = 0, nn = 0, no = 0, np = 0, ns = 0, atypes = 0;
  for ( tmp_pt = up.first(); tmp_pt != 0; tmp_pt = up.next() ) {
    an = tmp_pt->getAtomicNumber();
    if (an == 1) {
      if (nh == 0) atypes++;
      nh++;
    }
    if (an == 6) {
      if (nc == 0) atypes++;
      nc++;
    }
    if (an == 7) {
      if (nn == 0) atypes++;
      nn++;
    }
    if (an == 8) {
      if (no == 0) atypes++;
      no++;
    }
    if (an == 14) {
      if (np == 0) atypes++;
      np++;
    }
    if (an == 15) {
      if (ns == 0) atypes++;
      ns++;
    }
  }

  tout << atypes << "Z";
  if (nh > 0) tout << "H 1 ";
  if (nc > 0) tout << "C 4 ";
  if (nn > 0) tout << "N 3 ";
  if (no > 0) tout << "O 2 ";
  if (np > 0) tout << "P 5 ";
  if (ns > 0) tout << "S 2 ";
  tout << "Z";
  tout << rand() << "Z";
  tout << up.count() << "Z";

  // connection table
  DPoint *n_pt;
  for ( tmp_pt = up.first(); tmp_pt != 0; tmp_pt = up.next() ) {
    if (tmp_pt->getAtomicNumber() == 6) {
      if (tmp_pt->neighbors.count() == 4) {
	// NO chirality.
	tout << tmp_pt->baseElement() << " -1 1 ";
      } else {
	// Consider chirality.
	tout << tmp_pt->baseElement() << " 0 1 ";
      }
    } else {
      tout << tmp_pt->baseElement() << " -1 1 ";
    }
    for ( n_pt = tmp_pt->neighbors.first(); n_pt != 0; 
	  n_pt = tmp_pt->neighbors.next() ) {
      tout << n_pt->serial << " ";
    }
    tout << "Z";
  }

  // comment -- free advertising  :)
  tout << "generated by " << QString(XDC_VERSION).lower() << "Z";

  // renumbering list
  for (nh = 0; nh < up.count(); nh++) {
    tout << nh + 1 << " " << nh + 1 << "Z";
  }
  // distance constraints
  tout << "0" << "Z";

  // run BUILD3D and verify 00XYZ.MOL was created
  buildfile.replace(" ", "X");
  std::cout << std::endl;
  std::cout << buildfile << std::endl;
  std::cout << std::endl;

  NetAccess na;
  int ge1;
  if (na.runBuild3D(buildfile)) {
    xyzfile = na.s3dmol;
    ge1 = xyzfile.find("generated");
    std::cout << "gen: " << ge1 << std::endl;
    if (ge1 > 0) {
      xyzfile.remove(0, ge1);
    }
  } else {
    std::cout << "Build3D failed!" << std::endl;
    return;
  }

  // if a filename was passed in, copy to that
  if (fn3d.length() > 1) {
    if (fn3d.right(4).lower() != ".mol")
      fn3d.append(".mol");
  } else {
    // success -- now ask where to move output
    QFileDialog fd(QString::null, QString::null, 0, 0, TRUE);
    QString str1;
    fd.setCaption( tr("Save 3D file as...") );
    fd.setMode(QFileDialog::AnyFile);
    fd.setFilters( tr("MDL molfile (*.mol)") );  
    if ( fd.exec() == QDialog::Accepted ) {
      str1 = fd.selectedFile();
      if (str1.right(4).lower() != ".mol")
	str1.append(".mol");
      fn3d = str1;
    } else {
      std::cout << "To3D cancelled by user" << std::endl;
      RemoveNMRprotons();
      return;
    }
  }

  std::cout << "3D file: " << fn3d << std::endl;
  QFile fout(fn3d);
  if ( !fout.open(IO_WriteOnly) ) {
    std::cout << "File save failed!" << std::endl;
  } else {
    QTextStream tout(&fout);
    tout << xyzfile << endl;
    fout.close();
  }

  // Remove NMR protons
  RemoveNMRprotons();
}
