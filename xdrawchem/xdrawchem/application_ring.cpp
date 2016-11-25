#include "application.h"

// language file
//#include "language.h"
// end language include(s)

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <qimage.h>
#include <qbitmap.h>
#include <qpixmap.h>
#include <qtooltip.h>
#include <qtoolbar.h>
#include <qtoolbutton.h>
#include <qpopupmenu.h>
#include <qmenubar.h>
#include <qkeycode.h>
#include <qmultilineedit.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qstatusbar.h>
#include <qmessagebox.h>
#include <qprinter.h>
#include <qapplication.h>
#include <qaccel.h>
#include <qtextstream.h>
#include <qpainter.h>
#include <qpushbutton.h>
#include <qcolordialog.h>
#include <qpaintdevicemetrics.h>
#include <qwhatsthis.h>
#include <qscrollview.h>
#include <qcombobox.h>
#include <qfont.h>
#include <qdir.h>
#include <qstringlist.h>
#include <qmime.h>
#include <qiconset.h>

#include "prefs.h"
#include "defs.h"
#include "chemdata.h"
#include "render2d.h"

extern Preferences preferences;

// canned structure images
#include "ring_xpm.h"
#include "aa_xpm.h"
#include "na_xpm.h"
#include "sugar_xpm.h"

// canned structures
#define RING_3 100
#define RING_4 101
#define RING_5 102
#define RING_6 103
#define RING_BOAT6 104
#define RING_CHAIR6 105
#define RING_IMIDAZOLE 106
#define RING_CYCLOPENTADIENE 107
#define RING_BENZENE 108
#define RING_INDOLE 109
#define RING_PURINE 110
#define RING_PYRIMIDINE 111
#define RING_STEROID 112
#define RING_NAPHTHALENE 113
#define RING_ANTHRACENE 114
#define RING_7 115
#define RING_8 116
#define RING_BIPHENYL 120
#define NA_ADENINE  200
#define NA_CYTOSINE 201
#define NA_GUANINE  202
#define NA_THYMINE  203
#define NA_URACIL   204
#define AA_ALANINE 300
#define AA_ARGININE 301
#define AA_ASPARAGINE 302
#define AA_ASPARTIC_ACID 303
#define AA_CYSTEINE 304
#define AA_GLUTAMIC_ACID 305
#define AA_GLUTAMINE 306
#define AA_GLYCINE 307
#define AA_HISTIDINE 308
#define AA_ISOLEUCINE 309
#define AA_LEUCINE 310
#define AA_LYSINE 311
#define AA_METHIONINE 312
#define AA_PHENYLALANINE 313
#define AA_PROLINE 314
#define AA_SERINE 315
#define AA_THREONINE 316
#define AA_TRYPTOPHAN 317
#define AA_TYROSINE 318
#define AA_VALINE 319
#define AA_NPH 320
#define AA_STATINE 321
#define S_RIBOSE 400
#define S_DEOXYRIBOSE 401
#define S_D_FRUCTOSE 402
#define S_D_GLUCOSE 403
#define FG_FMOC 500
#define FG_BOC 501
#define FG_DABCYL 502
#define FG_DABSYL 503
#define FG_DANSYL 504
#define FG_EDANS 505
#define FG_BIOTIN 506
// end structures

QPopupMenu *ApplicationWindow::BuildNewRingMenu() {
  QPopupMenu * ringSub = new QPopupMenu(this);

  // make generic ring list
  QPopupMenu * genericSub = new QPopupMenu(this);
  genericSub->insertItem( QIconSet(QPixmap(r_cyclopropane), QIconSet::Small), 
			  tr("[*] Cyclopropane"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_3 );
  genericSub->insertItem( QIconSet(QPixmap(r_cyclobutane), QIconSet::Small), 
			  tr("[*] Cyclobutane"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_4 );
  genericSub->insertItem( QIconSet(QPixmap(r_cyclopentane), QIconSet::Small), 
			  tr("[*] Cyclopentane"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_5 );
  genericSub->insertItem( QIconSet(QPixmap(r_imidazole), QIconSet::Small), 
			  tr("Imidazole"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_IMIDAZOLE );
  genericSub->insertItem(QIconSet(QPixmap(r_cyclopentadiene),QIconSet::Small), 
			  tr("[*] Cyclopentadiene"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_CYCLOPENTADIENE );
  genericSub->insertItem( QIconSet(QPixmap(r_6flat), QIconSet::Small), 
			  tr("[*] Cyclohexane (flat)"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_6 );
  genericSub->insertItem( QIconSet(QPixmap(r_6boat), QIconSet::Small), 
			  tr("Cyclohexane (boat)"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_BOAT6 );
  genericSub->insertItem( QIconSet(QPixmap(r_6chair), QIconSet::Small), 
			  tr("Cyclohexane (chair)"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_CHAIR6 );
  genericSub->insertItem( QIconSet(QPixmap(r_benzene), QIconSet::Small), 
			  tr("[*] Benzene"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_BENZENE );
  genericSub->insertItem( QIconSet(QPixmap(r_pyrimidine), QIconSet::Small), 
			  tr("Pyrimidine"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_PYRIMIDINE );
  genericSub->insertItem( QIconSet(QPixmap(ring7), QIconSet::Small), 
			  tr("[*] Cycloheptane"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_7 );
  genericSub->insertItem( QIconSet(QPixmap(ring8), QIconSet::Small), 
			  tr("[*] Cyclooctane"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_8 );
  genericSub->insertItem( QIconSet(QPixmap(r_indole), QIconSet::Small), 
			  tr("Indole"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_INDOLE );
  genericSub->insertItem( QIconSet(QPixmap(r_purine), QIconSet::Small), 
			  tr("Purine"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_PURINE );
  genericSub->insertItem( QIconSet(QPixmap(naphthalene_xpm), QIconSet::Small), 
			  tr("Naphthalene"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_NAPHTHALENE );
 genericSub->insertItem( QIconSet(QPixmap(biphenyl_xpm), QIconSet::Small), 
			  tr("Biphenyl"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_BIPHENYL );
  genericSub->insertItem( QIconSet(QPixmap(anthracene_xpm), QIconSet::Small), 
			  tr("Anthracene"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_ANTHRACENE );
  genericSub->insertItem( QIconSet(QPixmap(r_steroid), QIconSet::Small), 
			  tr("Steroid (fused ring template)"), 
			  this, SLOT(FromNewRingMenu(int)), 0,
			  RING_STEROID );
  ringSub->insertItem( tr("Rings"), genericSub );

  // make amino acid list
  QPopupMenu * aaSub = new QPopupMenu(this);
  aaSub->insertItem( QIconSet(QPixmap(aa_ala), QIconSet::Small), 
		     tr("Alanine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_ALANINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_arg), QIconSet::Small), 
		     tr("Arginine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_ARGININE );
  aaSub->insertItem( QIconSet(QPixmap(aa_asn), QIconSet::Small), 
		     tr("Asparagine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_ASPARAGINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_asp), QIconSet::Small), 
		     tr("Aspartic acid"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_ASPARTIC_ACID );
  aaSub->insertItem( QIconSet(QPixmap(aa_cys), QIconSet::Small), 
		     tr("Cysteine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_CYSTEINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_glu), QIconSet::Small), 
		     tr("Glutamic acid"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_GLUTAMIC_ACID );
  aaSub->insertItem( QIconSet(QPixmap(aa_gln), QIconSet::Small), 
		     tr("Glutamine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_GLUTAMINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_gly), QIconSet::Small), 
		     tr("Glycine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_GLYCINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_his), QIconSet::Small), 
		     tr("Histidine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_HISTIDINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_ile), QIconSet::Small), 
		     tr("Isoleucine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_ISOLEUCINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_leu), QIconSet::Small), 
		     tr("Leucine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_LEUCINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_lys), QIconSet::Small), 
		     tr("Lysine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_LYSINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_met), QIconSet::Small), 
		     tr("Methionine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_METHIONINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_nph), QIconSet::Small), 
		     tr("Nitrophenylalanine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_NPH );
  aaSub->insertItem( QIconSet(QPixmap(aa_phe), QIconSet::Small), 
		     tr("Phenylalanine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_PHENYLALANINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_pro), QIconSet::Small), 
		     tr("Proline"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_PROLINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_ser), QIconSet::Small), 
		     tr("Serine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_SERINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_statine), QIconSet::Small), 
		     tr("Statine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_STATINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_thr), QIconSet::Small), 
		     tr("Threonine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_THREONINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_trp), QIconSet::Small), 
		     tr("Tryptophan"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_TRYPTOPHAN );
  aaSub->insertItem( QIconSet(QPixmap(aa_tyr), QIconSet::Small), 
		     tr("Tyrosine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_TYROSINE );
  aaSub->insertItem( QIconSet(QPixmap(aa_val), QIconSet::Small), 
		     tr("Valine"), this, 
		     SLOT(FromNewRingMenu(int)), 0,
		     AA_VALINE );
  ringSub->insertItem( tr("Amino acids"), aaSub );

  // make nucleic acid list
  QPopupMenu * naSub = new QPopupMenu(this);
  naSub->insertItem( QIconSet(QPixmap(na_adenine), QIconSet::Small), 
		     tr("Adenine"), this, 
		     SLOT(FromNewRingMenu(int)), 0, NA_ADENINE );
  naSub->insertItem( QIconSet(QPixmap(na_cytosine), QIconSet::Small), 
		     tr("Cytosine"), this, 
		     SLOT(FromNewRingMenu(int)), 0, NA_CYTOSINE );
  naSub->insertItem( QIconSet(QPixmap(na_guanine), QIconSet::Small), 
		     tr("Guanine"), this, 
		     SLOT(FromNewRingMenu(int)), 0, NA_GUANINE );
  naSub->insertItem( QIconSet(QPixmap(na_thymine), QIconSet::Small), 
		     tr("Thymine"), this, 
		     SLOT(FromNewRingMenu(int)), 0, NA_THYMINE );
  naSub->insertItem( QIconSet(QPixmap(na_uracil), QIconSet::Small), 
		     tr("Uracil"), this, 
		     SLOT(FromNewRingMenu(int)), 0, NA_URACIL );
  ringSub->insertItem( tr("Nucleic acids"), naSub );

  // make sugar list
  QPopupMenu * sugarSub = new QPopupMenu(this);
  sugarSub->insertItem( QIconSet(QPixmap(s_ribose), QIconSet::Small), 
			tr("Ribose"), 
			this, SLOT(FromNewRingMenu(int)), 0,
			S_RIBOSE );
  sugarSub->insertItem( QIconSet(QPixmap(s_deoxyribose), QIconSet::Small), 
			tr("Deoxyribose"), 
			this, SLOT(FromNewRingMenu(int)), 0,
			S_DEOXYRIBOSE );
  sugarSub->insertItem( QIconSet(QPixmap(s_d_fructose), QIconSet::Small), 
			tr("D-fructose"), 
			this, SLOT(FromNewRingMenu(int)), 0,
			S_D_FRUCTOSE );
  sugarSub->insertItem( QIconSet(QPixmap(s_d_glucose), QIconSet::Small), 
			tr("D-glucose"), 
			this, SLOT(FromNewRingMenu(int)), 0,
			S_D_GLUCOSE );
  ringSub->insertItem( tr("Sugars"), sugarSub );

  // make function group list
  QPopupMenu * fgSub = new QPopupMenu(this);
  fgSub->insertItem( tr("[*] FMOC"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_FMOC);
  fgSub->insertItem( tr("[*] BOC"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_BOC);
  fgSub->insertItem( tr("[*] DABCYL"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_DABCYL);
  fgSub->insertItem( tr("[*] DABSYL"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_DABSYL);
  fgSub->insertItem( tr("[*] DANSYL"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_DANSYL);
  fgSub->insertItem( tr("[*] EDANS"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_EDANS);
  fgSub->insertItem( tr("[*] Biotin"),
		     this, SLOT(FromNewRingMenu(int)), 0,
		     FG_BIOTIN);
  ringSub->insertItem( tr("Useful groups"), fgSub );

  // make user-defined list
  customRingMenuId = ringSub->insertItem( tr("User-defined"), 
					  BuildCustomRingMenu() );

  return ringSub;
}

QPopupMenu *ApplicationWindow::BuildCustomRingMenu() {
  QPopupMenu * userDefSub = new QPopupMenu(this);
  QDir d(preferences.getCustomRingDir(), "*.png");
  ringlist = d.entryList();
  for (int cc = 0; cc < ringlist.count(); cc++) {
    QPixmap px1( QString(preferences.getCustomRingDir() + ringlist[cc]) );
    QBitmap mask1( px1.width(), px1.height() );
    mask1.fill(Qt::color1);
    px1.setMask(mask1);
    /*userDefSub->insertItem( QPixmap( QString(RingDir + ringlist[cc]) ),
			    ringlist[cc],
			    this, SLOT(FromRingMenu(int)), 0, cc );*/
    userDefSub->insertItem( QIconSet(px1, QIconSet::Small),
			    ringlist[cc].left(ringlist[cc].length() - 4),
			    this, SLOT(FromRingMenu(int)), 0, cc );
  }
  userDefSub->insertItem( tr("Add new..."), this, SLOT( saveCustomRing() ) );

  return userDefSub;
}

void ApplicationWindow::FromNewRingMenu(int x) {
  //c->StartUndo(0,0);
  //c->DeselectAll();
  //c->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
  QString fname;
  if (x == RING_3) fname = "cyclopropane.cml";
  if (x == RING_4) fname = "cyclobutane.cml";
  if (x == RING_5) fname = "cyclopentane.cml";
  if (x == RING_IMIDAZOLE) fname = "imidazole.cml";
  if (x == RING_CYCLOPENTADIENE) fname = "cyclopentadiene.cml";
  if (x == RING_6) fname = "cyclohexane.cml";
  if (x == RING_BOAT6) fname = "6ring_boat.cml";
  if (x == RING_CHAIR6) fname = "6ring_chair.cml";
  if (x == RING_BENZENE) fname = "benzene.cml";
  if (x == RING_7) fname = "cycloheptane.cml";
  if (x == RING_8) fname = "cyclooctane.cml";
  if (x == RING_INDOLE) fname = "indole.cml";
  if (x == RING_STEROID) fname = "steroid.cml";
  if (x == RING_PURINE) fname = "purine.cml";
  if (x == RING_PYRIMIDINE) fname = "pyrimidine.cml";
  if (x == RING_NAPHTHALENE) fname = "naphthalene.cml";
  if (x == RING_ANTHRACENE) fname = "anthracene.cml";
  if (x == RING_BIPHENYL) fname = "biphenyl.cml";
  if (x == NA_ADENINE) fname = "adenine.cml";
  if (x == NA_CYTOSINE) fname = "cytosine.cml";
  if (x == NA_GUANINE) fname = "guanine.cml";
  if (x == NA_THYMINE) fname = "thymine.cml";
  if (x == NA_URACIL) fname = "uracil.cml";
  if (x == AA_ALANINE) fname = "alanine.cml";
  if (x == AA_ARGININE) fname = "arginine.cml";
  if (x == AA_ASPARTIC_ACID) fname = "aspartic_acid.cml";
  if (x == AA_ASPARAGINE) fname = "asparagine.cml";
  if (x == AA_CYSTEINE) fname = "cysteine.cml";
  if (x == AA_GLUTAMIC_ACID) fname = "glutamic_acid.cml";
  if (x == AA_GLUTAMINE) fname = "glutamine.cml";
  if (x == AA_GLYCINE) fname = "glycine.cml";
  if (x == AA_HISTIDINE) fname = "histidine.cml";
  if (x == AA_ISOLEUCINE) fname = "isoleucine.cml";
  if (x == AA_LEUCINE) fname = "leucine.cml";
  if (x == AA_LYSINE) fname = "lysine.cml";
  if (x == AA_METHIONINE) fname = "methionine.cml";
  if (x == AA_NPH) fname = "nitrophenylalanine.cml";
  if (x == AA_PHENYLALANINE) fname = "phenylalanine.cml";
  if (x == AA_PROLINE) fname = "proline.cml";
  if (x == AA_SERINE) fname = "serine.cml";
  if (x == AA_STATINE) fname = "statine.cml";
  if (x == AA_THREONINE) fname = "threonine.cml";
  if (x == AA_TRYPTOPHAN) fname = "tryptophan.cml";
  if (x == AA_TYROSINE) fname = "tyrosine.cml";
  if (x == AA_VALINE) fname = "valine.cml";
  if (x == S_RIBOSE) fname = "ribose.cml";
  if (x == S_DEOXYRIBOSE) fname = "deoxyribose.cml";
  if (x == S_D_FRUCTOSE) fname = "d-fructose.cml";
  if (x == S_D_GLUCOSE) fname = "d-glucose.cml";
  if (x == FG_FMOC) fname = "fmoc.cml";
  if (x == FG_BOC) fname = "boc.cml";
  if (x == FG_DABCYL) fname = "dabcyl.cml";
  if (x == FG_DABSYL) fname = "dabsyl.cml";
  if (x == FG_DANSYL) fname = "dansyl.cml";
  if (x == FG_EDANS) fname = "edans.cml";
  if (x == FG_BIOTIN) fname = "biotin.cml";
  //c->load( RingDir + fname );
  //r->Inserted();
  if (fname == "cyclopropane.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "cyclobutane.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "cyclopentane.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "cyclopentadiene.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "cyclohexane.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "benzene.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "cycloheptane.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "cyclooctane.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 1);
    return;
  }
  if (fname == "fmoc.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if (fname == "boc.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if (fname == "dabcyl.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if (fname == "dabsyl.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if (fname == "dansyl.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if (fname == "edans.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if (fname == "biotin.cml") {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 2);
    return;
  }
  if ( (x > 299) && (x < 350) ) {
    r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4), 3);
    return;
  }
  r->setMode_DrawRing(RingDir + fname, fname.left(fname.length() - 4));
}
