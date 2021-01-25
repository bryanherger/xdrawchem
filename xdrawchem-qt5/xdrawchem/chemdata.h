// chemdata.h --  the class that manages all of XDrawChem's data.

#ifndef CHEMDATA_H
#define CHEMDATA_H

#include <QString>
#include <QList>
#include <QPoint>

#include "drawable.h"
#include "bond.h"
#include "text.h"
#include "gobject.h"
#include "cml.h"
#include "xdc_event.h"
#include "molinfodialog.h"

#include "prefs.h"

extern Preferences preferences;

class Render2D;
class Clipboard;

class ChemData : public QObject
{
    Q_OBJECT

public:
    void setRender2D(Render2D *r1) { r = r1; }
    Render2D *getRender2D() { return r; }
    bool edited() { return notSaved; }
    bool needsSave() { return notSaved; }
    void saved() { notSaved = false; }

    // defined in chemdata.cpp
    ChemData( QObject *parent = 0 );
    void drawAll();
    Molecule *firstMolecule();
    void addMolecule( Molecule * );
    Molecule* findMolecule(DPoint* p);
    void addBond( DPoint *, DPoint *, int, int, QColor, bool hl = false );
    void addArrow( DPoint *, DPoint *, QColor, int, int, bool hl = false );
    void addCurveArrow( DPoint *, DPoint *, QColor, QString, bool hl = false );
    void addBracket( DPoint *, DPoint *, QColor, int, bool hl = false );
    void addText( Text *);
    void addGraphicObject( GraphicObject *);
    void addSymbol( DPoint *, QString, bool hl = false );
    void Erase( Drawable * );
    void EraseSelected();
    void DetectSplit();
    DPoint *FindNearestPoint( DPoint *, double & );
    Drawable *FindNearestObject( DPoint *, double & );
    void NewSelectRect( QRect, bool );
    void SelectAll();
    void DeselectAll();
    void Move( double, double );
    void Resize( DPoint *, double );
    void Rotate( DPoint *, double );
    void Flip( DPoint *, int);
    void SetColorIfHighlighted( QColor );
    void FinishMove();
    QRect selectionBox();
    QList<DPoint *> UniquePoints();
    QList<Drawable *> UniqueObjects();
    void SetTopLeft(QPoint p) { RenderTopLeft = p; }
    Molecule *insideMolecule( DPoint * );

    // defined in chemdata_edit.cpp
    void Cut();
    void Copy();
    bool Paste();
    void StartUndo( int fn = 0, DPoint *s1 = 0 );
    bool Undo();
    void ScaleAll( double bond_length = -1.0 );

    // defined in chemdata_rw.cpp
    bool load( QString );
    bool save( QString );
    bool load_legacy( QString );

    // defined in chemdata_xdc.cpp
    bool load_native( QString );
    bool save_native( QString );

    // defined in chemdata_cdx.cpp
    bool save_cdx( QString );
    bool save_cdxml( QString );

    // defined in chemdata_mdl.cpp
    bool load_mdl( QString );
    bool ProcessMDL( QString );
    bool save_mdl( QString );

    // defined in chemdata_xml.cpp
    bool load_xml( QString );
    bool ProcessXML( QString );
    bool LoadCDXMLFile( QString, QString );
    bool SelfContainedTag( QString );
    QString ReadTag( QString, int & );
    QString ReadData( QString, int & );
    QStringList ReadAttr( QString );
    QStringList Tokenize( QString );
    void ParseBoundingBox( QString, double &, double &, double &, double & );

    // defined in chemdata_cml.cpp
    bool NewLoadCMLFile( QString );
    bool LoadCMLFile( QString, QString );
    bool save_cml( QString );
    void Convert_CML_Lists_To_Native();

    // defined in chemdata_tools.cpp
    void Save3D( QString );
    void clearAllGroups();
    void Tool( DPoint *, int );
    void AutoLayout();
    void fromSMILES( QString );
    void SmartPlace( QString, DPoint * );
    void SmartPlaceToo( QString, DPoint * );
    void SmartPlaceThree( QString, DPoint * );
    double CalculateRingAttachAngle( DPoint * );
    bool SelectWithinLoop( QVector<QPoint> );
    // defined in cdx2cdxml.cpp
    QString ParseProperty( QByteArray );
    QString CDXToCDXML( QString );
    // defined in chemdata_rxn.cpp
    void ReactionAnalysis( int );
    // defined in retro.cpp
    void Retro( Molecule * );
    // defined here
    void setThickKludge( int lt1 ) { thick_kludge = lt1; }
    Clipboard *getClipboard() { return clip; }
    void setClipboard( Clipboard *clip1 ) {
        if (clip1 == 0)
            clip = new Clipboard;
        else
            clip = clip1;
    }

public slots:
    void XDCEventHandler( XDC_Event * );
    void returnFromMID();

signals:
    void SignalSetStatusBar( QString );
    void SignalHelpTopic( QString );
    void SignalUpdateCustomRingMenu();

private:
    // the Render2D widget this ChemData is linked to
    Render2D *r;
    // Top left corner of visible part of r
    QPoint RenderTopLeft;
    // Application clipboard
    Clipboard *clip;
    // temporary/current Drawable (temporary DPoint *tmp_pt defined below)
    Drawable *tmp_draw;
    // list of Drawable objects we are tracking
    QList<Drawable *> drawlist;
    // Variables for reading CML/CDXML/XML files
    QString MoleculeTitle;
    QString MoleculeId;
    QList<CML_Atom *> CML_Atoms;
    QList<CML_Atom *> CML_Atoms_Sub;
    QList<CML_Bond *> CML_Bonds;
    QList<CML_Bond *> CML_Bonds_Sub;
    QList<int *> TagStack;
    QList<Drawable *> CDXML_Objects;
    DPoint *tmp_pt;
    CML_Atom *tmp_atom;
    CML_Bond *tmp_bond;
    // variables for Undo
    int undo_last;
    DPoint *undo_start, *undo_end;
    double undo_dx, undo_dy;
    QStringList last_states;
    QString current_undo_file;
    // Presistent dialogs
    MolInfoDialog *mi;
    Text *tt, *tt_mw, *tt_ef, *tt_ea;
    // hack for setting ring thickness
    int thick_kludge;
    // has this document changed since last save?
    bool notSaved;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
