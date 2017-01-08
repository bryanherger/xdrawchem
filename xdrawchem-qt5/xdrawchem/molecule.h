// molecule.h -- subclass of Drawable which contains a molecule

#ifndef MOLECULE_H
#define MOLECULE_H

#include <openbabel/mol.h>
#include <openbabel/obconversion.h>

using namespace OpenBabel;

#include "render2d.h"
#include "chemdata.h"
#include "drawable.h"
#include "dpoint.h"
#include "bond.h"
#include "text.h"
#include "symbol.h"
#include "molecule_sssr.h"
#include "tooldialog.h"
#include "peak.h"

class Molecule : public Drawable
{
public:
    Molecule( Render2D *, QObject *parent = 0 );
    ~Molecule();
    Render2D *getRender2D() { return r; }
    void SetChemdata( ChemData *cd1 ) { cd = cd1; }
    void Render();  // draw this object
    int Type();  // return type of object
    bool Find( DPoint * );   // is this DPoint present in this Molecule?
    DPoint *FindNearestPoint( DPoint *, double & );
    Drawable *FindNearestObject( DPoint *, double & );
    void addBond( DPoint *, DPoint *, int, int, QColor, bool hl = false );
    void addBond( Bond * );
    void addText( Text * );
    void addSymbol( Symbol * );
    void CopyTextToDPoint();
    void CalcOffsets();
    void addMolecule( Drawable * );
    Bond *bondsFirst() { return bonds.first(); }
    //   Bond *bondsNext() { return bonds.next(); }

    Text *labelsFirst()
    {
        if ( !labels.isEmpty() )
            return labels.first();
        else
            return 0;
    }

    //   Text *labelsNext() { return labels.next(); }
    bool Erase( Drawable * );
    void EraseSelected();
    bool isWithinRect( QRect, bool );
    bool WithinBounds( DPoint * );
    void SelectAll();
    void DeselectAll();
    void SetColorIfHighlighted( QColor );
    void Move( double, double );
    void Rotate( DPoint *, double );
    void Rotate( double );
    void Flip( DPoint *, int );
    void Resize( DPoint *, double );
    QRect BoundingBox();
    QRect BoundingBoxAll();
    QList<DPoint *> AllPoints();
    QList<Drawable *> AllObjects();
    QList<Bond *> AllBonds();
    QList<Text *> AllLabels();
    QList<Molecule *> MakeSplit();
    int Members() { return bonds.count(); }
    QString ToXML( QString );
    QString ToCDXML( QString );
    QString ToMDLMolfile( int coords = 0 );
    void FromXML( QString );
    void Changed();
    // defined in molecule_tools.cpp
    Bond * FindBond( DPoint *, DPoint * );
    int OrderOfBond( DPoint *, DPoint * );
    void Reactivity( int );  // molecule_tools_2.cpp for now
    QList<DPoint *> BreakRingBonds( DPoint * );
    DPoint *GetRingAttachPoint();
    DPoint *GetAttachPoint(QString);
    double CalculateRingAttachAngle( DPoint * );
    void FindHybridization();
    void setGroupType( int );
    Text *CalcMW( bool from_change = false );
    Text *CalcEmpiricalFormula( bool from_mw = false );
    Text *CalcElementalAnalysis( bool show_dialog = true );
    QStringList Calc13CNMR( bool show_dialog = true );
    void CalcIR();
    QString CalcName();  // actually returns canonical SMILES
    //QString GetCASNumber();
    //QString IUPAC_Name();
    void AllNeighbors();
    void MakeSSSR();
    void AddHydrogens(bool to_carbon = false);
    double SumBondEnthalpy();
    void Scale(double bond_length = -1.0);
    void AddPeak(double, QString, QString);
    void Make3DVersion(QString fn3d = "");
    void CalcpKa();
    double CalcPartialCharge(QString);
    double CalcKOW();
    // defined in molecule_1h_nmr.cpp
    QStringList Calc1HNMR(bool show_dialog = true);
    void AddNMRprotons();
    void RemoveNMRprotons();
    void ProtonEnvironment();
    void Multiplicity_1HNMR();
    void Calc1HMultiplicityAndIntensity();
    double Calc1HShift(QString);
    // defined in molecule_smiles.cpp
    void CleanUp();
    void SDG(bool);
    QString ToSMILES();
    void FromSMILES(QString);
    QString ToInChI();
    void FromInChI(QString);
    // defined in retro.cpp
    QString BuildReactionList(QString);
    int Retro();
    QString RetroTraverseBonds(DPoint *, DPoint *, Bond *, int);
    QString RetroAtomName(DPoint *);
    QString RetroBondName(Bond *, bool runsssr = false);
    bool RetroMatch(QString, QString);
    bool RetroTreeMatch(QString, QString, QString, QString);
    // defined here:
    bool isInGroup()
    {
        if (group_type != GROUP_NONE) return true;
        return false;
    }
    int groupType() { return group_type; }
    void FormulaLabelDeleted() { qDebug() << "FLD" ; text_formula = 0; }
    void MWLabelDeleted() { text_mw = 0; }
    double Angle(Bond *a1, Bond *b1)
    {
        // from Bond a1 to Bond b1
        // determine endpoints
        DPoint *a, *c, *b;
        if (a1->Start() == b1->Start()) {
            c = a1->Start();
            a = a1->End();
            b = b1->End();
        }
        if (a1->Start() == b1->End()) {
            c = a1->Start();
            a = a1->End();
            b = b1->Start();
        }
        if (a1->End() == b1->Start()) {
            c = a1->End();
            b = a1->Start();
            a = b1->End();
        }
        if (a1->End() == b1->End()) {
            c = a1->End();
            b = a1->Start();
            a = b1->Start();
        }
        double ang1 = getAngle(c, a);
        double ang2 = getAngle(c, b);
        double ang3 = ang1 - ang2;
        if (ang3 < 0.0) ang3 += 360.0;
        return ang3;
    }

    OBMol * convertToOBMol();
    bool convertFromOBMol(OBMol *);
    static const char symbol[110][4];

    // not appropriate but best way to handle ToolDialog requests.
    QList<Peak *> peaklist;
    Peak *tmp_peak;

private:
    // Renderer
    Render2D *r;
    // ChemData
    ChemData *cd;
    // current/temporary Bond
    Bond *tmp_bond;
    // current/temporary Text
    Text *tmp_text;
    // current/temporary Symbol
    Symbol *tmp_sym;
    // current/temporary DPoint
    DPoint *tmp_pt;
    // list of elements which make up this molecule
    QList<Bond *> bonds;
    QList<Text *> labels;
    QList<Symbol *> symbols;
    // Text objects which hold MW and formula
    Text *text_mw, *text_formula;
    // list of unique points used by Move(), Resize(), Rotate(), and
    // function to fill it
    void MakeTomoveList();
    QList<DPoint *> tomove;
    // used for elemental analysis (set by CalcEmpiricalFormula)
    double nc, nh, no, nn, nmw;
    // Molecule's SSSR
    SSSR this_sssr;
    // point list for SDG, etc.
    QList<DPoint *> up;
    // group data
    int group_type;
    int group_id;
    // proton magnetic environment list(s)
    QStringList protonMagEnvList;
    QStringList protonFinalList;
    // render partial charges?
    bool showPC;
    // reactivity info
    QStringList atomRxns, bondRxns;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
