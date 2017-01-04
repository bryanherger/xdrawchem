#include <cstdio>
#include <cstdlib>
#include <cerrno>

#include <QtGlobal>
#include <QBitmap>

#include "application.h"
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

QMenu *ApplicationWindow::BuildNewRingMenu()
{
    QMenu *ringSub = new QMenu( this );
    qInfo() << "BuildNewRingMenu";
    // make ring list
    QMenu *genericSub = new QMenu( tr( "Rings" ), ringSub );
    ring3Action = genericSub->addAction( QIcon( QPixmap( r_cyclopropane ) ), tr( "[*] Cyclopropane") );
    ring4Action = genericSub->addAction( QIcon( QPixmap( r_cyclobutane ) ), tr( "[*] Cyclobutane") );
    ring5Action = genericSub->addAction( QIcon( QPixmap( r_cyclopentane ) ), tr( "[*] Cyclopentane" ) );
    ringImidazoleAction = genericSub->addAction( QIcon( QPixmap( r_imidazole ) ), tr( "Imidazole" ) );
    ringCyclopentadieneAction = genericSub->addAction( QIcon( QPixmap( r_cyclopentadiene ) ), tr( "[*] Cyclopentadiene" ) );
    ring6Action = genericSub->addAction( QIcon( QPixmap( r_6flat ) ), tr( "[*] Cyclohexane (flat)" ) );
    ring6BoatAction = genericSub->addAction( QIcon( QPixmap( r_6boat ) ), tr( "Cyclohexane (boat)" ) );
    ring6ChairAction = genericSub->addAction( QIcon( QPixmap( r_6chair ) ), tr( "Cyclohexane (chair)" ) );
    ringBenzeneAction = genericSub->addAction( QIcon( QPixmap( r_benzene ) ), tr( "[*] Benzene" ) );
    ringPyrimidineAction = genericSub->addAction( QIcon( QPixmap( r_pyrimidine ) ), tr( "Pyrimidine" ) );
    ring7Action = genericSub->addAction( QIcon( QPixmap( ring7 ) ), tr( "[*] Cycloheptane" ) );
    ring8Action = genericSub->addAction( QIcon( QPixmap( ring8 ) ), tr( "[*] Cyclooctane" ) );
    ringIndoleAction = genericSub->addAction( QIcon( QPixmap( r_indole ) ), tr( "Indole" ) );
    ringPurineAction = genericSub->addAction( QIcon( QPixmap( r_purine ) ), tr( "Purine" ) );
    ringNaphAction = genericSub->addAction( QIcon( QPixmap( naphthalene_xpm ) ), tr( "Naphthalene" ) );
    ringBiphenylAction = genericSub->addAction( QIcon( QPixmap( biphenyl_xpm ) ), tr( "Biphenyl" ) );
    ringAnthraAction = genericSub->addAction( QIcon( QPixmap( anthracene_xpm ) ), tr( "Anthracene" ) );
    ringSteroidAction = genericSub->addAction( QIcon( QPixmap( r_steroid ) ), tr( "Steroid (fused ring template)" ) );

    connect( genericSub, SIGNAL( triggered( QAction * ) ), SLOT( setRingAction( QAction * ) ) );
    ringSub->addMenu( genericSub );

    // make amino acid list
    QAction *aaAction;
    QMenu *aaSub = new QMenu( tr( "Amino acids" ), this );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_ala ) ), tr( "Alanine" ) );
    aaAction->setData( AA_ALANINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_arg ) ), tr( "Arginine" ) );
    aaAction->setData( AA_ARGININE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_asn ) ), tr( "Asparagine" ) );
    aaAction->setData( AA_ASPARAGINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_asp ) ), tr( "Aspartic acid" ) );
    aaAction->setData( AA_ASPARTIC_ACID );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_cys ) ), tr( "Cysteine" ) );
    aaAction->setData( AA_CYSTEINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_glu ) ), tr( "Glutamic acid" ) );
    aaAction->setData( AA_GLUTAMIC_ACID );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_gln ) ), tr( "Glutamine" ) );
    aaAction->setData( AA_GLUTAMINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_gly ) ), tr( "Glycine" ) );
    aaAction->setData( AA_GLYCINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_his ) ), tr( "Histidine" ) );
    aaAction->setData( AA_HISTIDINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_ile ) ), tr( "Isoleucine" ) );
    aaAction->setData( AA_ISOLEUCINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_leu ) ), tr( "Leucine" ) );
    aaAction->setData( AA_LEUCINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_lys ) ), tr( "Lysine" ) );
    aaAction->setData( AA_LYSINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_met ) ), tr( "Methionine" ) );
    aaAction->setData( AA_METHIONINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_nph ) ), tr( "Nitrophenylalanine" ) );
    aaAction->setData( AA_NPH );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_phe ) ), tr( "Phenylalanine" ) );
    aaAction->setData( AA_PHENYLALANINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_pro ) ), tr( "Proline" ) );
    aaAction->setData( AA_PROLINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_ser ) ), tr( "Serine" ) );
    aaAction->setData( AA_SERINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_statine ) ), tr( "Statine" ) );
    aaAction->setData( AA_STATINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_thr ) ), tr( "Threonine" ) );
    aaAction->setData( AA_THREONINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_trp ) ), tr( "Tryptophan" ) );
    aaAction->setData( AA_TRYPTOPHAN );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_tyr ) ), tr( "Tyrosine" ) );
    aaAction->setData( AA_TYROSINE );
    aaAction = aaSub->addAction( QIcon( QPixmap( aa_val ) ), tr( "Valine" ) );
    aaAction->setData( AA_VALINE );

    ringSub->addMenu( aaSub );

    // make nucleic acid list
    QMenu *naSub = new QMenu( tr( "Nucleic acids" ), this );
    naAdenineAction = naSub->addAction( QIcon( QPixmap( na_adenine ) ), tr( "Adenine" ) );
    naCytosineAction = naSub->addAction( QIcon( QPixmap( na_cytosine ) ), tr( "Cytosine" ) );
    naGuanineAction = naSub->addAction( QIcon( QPixmap( na_guanine ) ), tr( "Guanine" ) );
    naThymineAction = naSub->addAction( QIcon( QPixmap( na_thymine ) ), tr( "Thymine" ) );
    naUracilAction = naSub->addAction( QIcon( QPixmap( na_uracil ) ), tr( "Uracil" ) );

    ringSub->addMenu( naSub );

    // make sugar list
    QMenu *sugarSub = new QMenu( tr( "Sugars" ), this );
    srAction = sugarSub->addAction( QIcon( QPixmap( s_ribose ) ), tr( "Ribose" ) );
    sdAction = sugarSub->addAction( QIcon( QPixmap( s_deoxyribose ) ), tr( "Deoxyribose" ) );
    sfAction = sugarSub->addAction( QIcon( QPixmap( s_d_fructose ) ), tr( "D-fructose" ) );
    sgAction = sugarSub->addAction( QIcon( QPixmap( s_d_glucose ) ), tr( "D-glucose" ) );

    ringSub->addMenu( sugarSub );

    // make function group list
    QMenu *fgSub = new QMenu( tr( "Useful groups" ), this );
    fgSub->addAction( tr( "[*] FMOC" ) );
    fgSub->addAction( tr( "[*] BOC" ) );
    fgSub->addAction( tr( "[*] DABCYL" ) );
    fgSub->addAction( tr( "[*] DABSYL" ) );
    fgSub->addAction( tr( "[*] DANSYL" ) );
    fgSub->addAction( tr( "[*] EDANS" ) );
    fgSub->addAction( tr( "[*] Biotin" ) );

    ringSub->addMenu( fgSub );

    // make user-defined list
    ringSub->addMenu( BuildCustomRingMenu() );

    return ringSub;
}

QMenu *ApplicationWindow::BuildCustomRingMenu()
{
    QMenu *userDefSub = new QMenu( tr( "User-defined" ), this );
    QDir d( preferences.getCustomRingDir(), "*.png" );

    ringlist = d.entryList();
    QAction *customAction;
    for ( int cc = 0; cc < ringlist.count(); cc++ ) {
        QPixmap px1( QString( preferences.getCustomRingDir() + ringlist[cc] ) );
        QBitmap mask1( px1.width(), px1.height() );

        mask1.fill( Qt::color1 );
        px1.setMask( mask1 );
        /*userDefSub->addAction( QPixmap( QString(RingDir + ringlist[cc]) ),
           ringlist[cc],
           this, FromRingMenu(int)), 0, cc ); */
        customAction = userDefSub->addAction( QIcon( px1 ), ringlist[cc].left( ringlist[cc].length() - 4 ) );
        customAction->setData( cc );
    }
    userDefSub->addAction( tr( "Add new..." ), this, SLOT( saveCustomRing() ), 0 );

    return userDefSub;
}

void ApplicationWindow::FromNewRingMenu( int x )
{
    //c->StartUndo(0,0);
    //c->DeselectAll();
    //c->SetTopLeft(sv->viewportToContents(QPoint(0,0)));
    QString fname;

    if ( x == RING_3 )
        fname = "cyclopropane.cml";
    if ( x == RING_4 )
        fname = "cyclobutane.cml";
    if ( x == RING_5 )
        fname = "cyclopentane.cml";
    if ( x == RING_IMIDAZOLE )
        fname = "imidazole.cml";
    if ( x == RING_CYCLOPENTADIENE )
        fname = "cyclopentadiene.cml";
    if ( x == RING_6 )
        fname = "cyclohexane.cml";
    if ( x == RING_BOAT6 )
        fname = "6ring_boat.cml";
    if ( x == RING_CHAIR6 )
        fname = "6ring_chair.cml";
    if ( x == RING_BENZENE )
        fname = "benzene.cml";
    if ( x == RING_7 )
        fname = "cycloheptane.cml";
    if ( x == RING_8 )
        fname = "cyclooctane.cml";
    if ( x == RING_INDOLE )
        fname = "indole.cml";
    if ( x == RING_STEROID )
        fname = "steroid.cml";
    if ( x == RING_PURINE )
        fname = "purine.cml";
    if ( x == RING_PYRIMIDINE )
        fname = "pyrimidine.cml";
    if ( x == RING_NAPHTHALENE )
        fname = "naphthalene.cml";
    if ( x == RING_ANTHRACENE )
        fname = "anthracene.cml";
    if ( x == RING_BIPHENYL )
        fname = "biphenyl.cml";
    if ( x == NA_ADENINE )
        fname = "adenine.cml";
    if ( x == NA_CYTOSINE )
        fname = "cytosine.cml";
    if ( x == NA_GUANINE )
        fname = "guanine.cml";
    if ( x == NA_THYMINE )
        fname = "thymine.cml";
    if ( x == NA_URACIL )
        fname = "uracil.cml";
    if ( x == AA_ALANINE )
        fname = "alanine.cml";
    if ( x == AA_ARGININE )
        fname = "arginine.cml";
    if ( x == AA_ASPARTIC_ACID )
        fname = "aspartic_acid.cml";
    if ( x == AA_ASPARAGINE )
        fname = "asparagine.cml";
    if ( x == AA_CYSTEINE )
        fname = "cysteine.cml";
    if ( x == AA_GLUTAMIC_ACID )
        fname = "glutamic_acid.cml";
    if ( x == AA_GLUTAMINE )
        fname = "glutamine.cml";
    if ( x == AA_GLYCINE )
        fname = "glycine.cml";
    if ( x == AA_HISTIDINE )
        fname = "histidine.cml";
    if ( x == AA_ISOLEUCINE )
        fname = "isoleucine.cml";
    if ( x == AA_LEUCINE )
        fname = "leucine.cml";
    if ( x == AA_LYSINE )
        fname = "lysine.cml";
    if ( x == AA_METHIONINE )
        fname = "methionine.cml";
    if ( x == AA_NPH )
        fname = "nitrophenylalanine.cml";
    if ( x == AA_PHENYLALANINE )
        fname = "phenylalanine.cml";
    if ( x == AA_PROLINE )
        fname = "proline.cml";
    if ( x == AA_SERINE )
        fname = "serine.cml";
    if ( x == AA_STATINE )
        fname = "statine.cml";
    if ( x == AA_THREONINE )
        fname = "threonine.cml";
    if ( x == AA_TRYPTOPHAN )
        fname = "tryptophan.cml";
    if ( x == AA_TYROSINE )
        fname = "tyrosine.cml";
    if ( x == AA_VALINE )
        fname = "valine.cml";
    if ( x == S_RIBOSE )
        fname = "ribose.cml";
    if ( x == S_DEOXYRIBOSE )
        fname = "deoxyribose.cml";
    if ( x == S_D_FRUCTOSE )
        fname = "d-fructose.cml";
    if ( x == S_D_GLUCOSE )
        fname = "d-glucose.cml";
    if ( x == FG_FMOC )
        fname = "fmoc.cml";
    if ( x == FG_BOC )
        fname = "boc.cml";
    if ( x == FG_DABCYL )
        fname = "dabcyl.cml";
    if ( x == FG_DABSYL )
        fname = "dabsyl.cml";
    if ( x == FG_DANSYL )
        fname = "dansyl.cml";
    if ( x == FG_EDANS )
        fname = "edans.cml";
    if ( x == FG_BIOTIN )
        fname = "biotin.cml";
    //c->load( RingDir + fname );
    //m_renderer->Inserted();
    if ( fname == "cyclopropane.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "cyclobutane.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "cyclopentane.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "cyclopentadiene.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "cyclohexane.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "benzene.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "cycloheptane.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "cyclooctane.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 1 );
        return;
    }
    if ( fname == "fmoc.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( fname == "boc.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( fname == "dabcyl.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( fname == "dabsyl.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( fname == "dansyl.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( fname == "edans.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( fname == "biotin.cml" ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 2 );
        return;
    }
    if ( ( x > 299 ) && ( x < 350 ) ) {
        m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ), 3 );
        return;
    }
    m_renderer->setMode_DrawRing( RingDir + fname, fname.left( fname.length() - 4 ) );
}

void ApplicationWindow::FromNewerRingMenu( QAction * action )
{
  qInfo() << "FromNewerRingMenu() invoked";
  if (action == ring3Action) {
    FromNewRingMenu( RING_3 );
    return;
  }
}

void ApplicationWindow::setRingAction( QAction * action )
{
  qInfo() << "setRingAction() invoked for action " << action->objectName() << " with data " << action->data();

  int actionData = (action->data()).toInt();
  if (actionData > 0) {
    FromNewRingMenu( actionData );
    drawRingButton->setDefaultAction( action );
    return;
  }
  if (action == ring3Action) {
    FromNewRingMenu( RING_3 );
    drawRingButton->setIcon( QIcon( QPixmap( r_cyclopropane ) ) );
    drawRingButton->setDefaultAction( ring3Action );
    return;
  }
  if (action == ring4Action) {
    FromNewRingMenu( RING_4 );
    drawRingButton->setDefaultAction( ring4Action );
    return;
  }
  if (action == ring5Action) {
    FromNewRingMenu( RING_5 );
    drawRingButton->setDefaultAction( ring5Action );
    return;
  }
  if (action == ringImidazoleAction) {
    FromNewRingMenu( RING_IMIDAZOLE );
    drawRingButton->setDefaultAction( ringImidazoleAction );
    return;
  }
  if (action == ringCyclopentadieneAction) {
    FromNewRingMenu( RING_CYCLOPENTADIENE );
    drawRingButton->setDefaultAction( ringCyclopentadieneAction );
    return;
  }
  if (action == ring6Action) {
    FromNewRingMenu( RING_6 );
    drawRingButton->setDefaultAction( ring6Action );
    return;
  }
  if (action == ring6BoatAction) {
    FromNewRingMenu( RING_BOAT6 );
    drawRingButton->setDefaultAction( ring6BoatAction );
    return;
  }
  if (action == ring6ChairAction) {
    FromNewRingMenu( RING_CHAIR6 );
    drawRingButton->setDefaultAction( ring6ChairAction );
    return;
  }
  if (action == ringBenzeneAction) {
    FromNewRingMenu( RING_BENZENE );
    drawRingButton->setDefaultAction( ringBenzeneAction );
    return;
  }
  if (action == ringPyrimidineAction) {
    FromNewRingMenu( RING_PYRIMIDINE );
    drawRingButton->setDefaultAction( ringPyrimidineAction );
    return;
  }
  if (action == ringIndoleAction) {
    FromNewRingMenu( RING_INDOLE );
    drawRingButton->setDefaultAction( ringIndoleAction );
    return;
  }
  if (action == ringNaphAction) {
    FromNewRingMenu( RING_NAPHTHALENE );
    drawRingButton->setDefaultAction( ringNaphAction );
    return;
  }
  if (action == ringBiphenylAction) {
    FromNewRingMenu( RING_BIPHENYL );
    drawRingButton->setDefaultAction( ringBiphenylAction );
    return;
  }
  if (action == ringPurineAction) {
    FromNewRingMenu( RING_PURINE );
    drawRingButton->setDefaultAction( ringPurineAction );
    return;
  }
  if (action == ringAnthraAction) {
    FromNewRingMenu( RING_ANTHRACENE );
    drawRingButton->setDefaultAction( ringAnthraAction );
    return;
  }
  if (action == ringSteroidAction) {
    FromNewRingMenu( RING_STEROID );
    drawRingButton->setDefaultAction( ringSteroidAction );
    return;
  }

  if (action == naAdenineAction) {
    FromNewRingMenu( NA_ADENINE );
    drawRingButton->setDefaultAction( naAdenineAction );
    return;
  }
  if (action == naCytosineAction) {
    FromNewRingMenu( NA_CYTOSINE );
    drawRingButton->setDefaultAction( naCytosineAction );
    return;
  }
  if (action == naGuanineAction) {
    FromNewRingMenu( NA_GUANINE );
    drawRingButton->setDefaultAction( naGuanineAction );
    return;
  }
  if (action == naThymineAction) {
    FromNewRingMenu( NA_THYMINE );
    drawRingButton->setDefaultAction( naThymineAction );
    return;
  }
  if (action == naUracilAction) {
    FromNewRingMenu( NA_URACIL );
    drawRingButton->setDefaultAction( naUracilAction );
    return;
  }

  if (action == srAction) {
    FromNewRingMenu( S_RIBOSE );
    drawRingButton->setDefaultAction( srAction );
    return;
  }
  if (action == sdAction) {
    FromNewRingMenu( S_DEOXYRIBOSE );
    drawRingButton->setDefaultAction( sdAction );
    return;
  }
  if (action == sfAction) {
    FromNewRingMenu( S_D_FRUCTOSE );
    drawRingButton->setDefaultAction( sfAction );
    return;
  }
  if (action == sgAction) {
    FromNewRingMenu( S_D_GLUCOSE );
    drawRingButton->setDefaultAction( sgAction );
    return;
  }
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
