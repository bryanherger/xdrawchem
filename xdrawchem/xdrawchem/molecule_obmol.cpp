#include "ob_compat.h"
OB_COMPAT_BEGIN
#include <openbabel/atom.h>
#include <openbabel/bond.h>
#include <openbabel/elements.h>
#include <openbabel/math/vector3.h>
#include <openbabel/mol.h>
#include <openbabel/obconversion.h>
OB_COMPAT_END
using namespace OpenBabel;

#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

#include "dpoint.h"
#include "chemdata.h"
#include "application.h"
#include "bond.h"
#include "molecule.h"
#include "defs.h"   // STEREO_UP / STEREO_DOWN

using namespace OpenBabel;

// Build this Molecule from an MDL molfile/SDF block held in a string.
// Headless/agent callers hand us structure text rather than a filename, so
// this bypasses ChemData::load_mdl() (which needs a file and pops dialogs).
// Accepts both V2000 and V3000 blocks; a leading blank line is optional.
bool Molecule::FromMolfile( const QString &molblock )
{
    if ( molblock.trimmed().isEmpty() )
        return false;

    QByteArray data = molblock.toUtf8();

    OBConversion conv;
    if ( !conv.SetInFormat( "mol" ) )
        return false;

    std::istringstream iss( data.constData() );
    OBMol obmol;
    if ( !conv.Read( &obmol, &iss ) || obmol.NumAtoms() == 0 )
        return false;

    /*  Hydrogens stay implicit.

        Two things were wrong here.  Calling OpenBabel's AddHydrogens() on
        import materialised every hydrogen as a real atom, so a six-atom
        molfile became a twelve-atom drawing with a vertex and an "H" label per
        hydrogen — the opposite of a skeletal formula.  And molfiles written by
        most tools (PubChem among them) already contain explicit hydrogens, so
        those arrived as atoms too and were drawn as "H3C", "CH" fragments
        scattered over the structure.

        XDrawChem's model is a 2-D drawing: a DPoint carries an element and the
        implicit hydrogen count follows from the element and the bond orders
        (MolData::Hydrogens() minus the summed orders).  That is how hand-drawn
        structures work and how SMILES imports work, so drop the explicit ones
        on the way in and let them stay implicit.  DeleteHydrogens() also
        rewrites any stereo reference to the deleted atom as an implicit one,
        so a wedge to a hydrogen survives as a wedge to an implicit hydrogen.

        Nothing is lost: Tools -> Add hydrogens can still materialise them, and
        exporting goes through convertToOBMol(), which writes what it is given.
                                                                        */
    obmol.DeleteHydrogens();

    if ( !convertFromOBMol( &obmol ) )
        return false;

    if ( preferences.getFixHydrogens() )
        AddStereocenterHydrogens();

    return true;
}

const char Molecule::symbol[110][4] = {
    "H", "He", "Li", "Be", "B", "C", "N", "O", "F", "Ne",
    "Na", "Mg", "Al", "Si", "P", "S", "Cl", "Ar", "K", "Ca",    // 20
    "Sc", "Ti", "V", "Cr", "Mn", "Fe", "Co", "Ni", "Cu", "Zn",
    "Ga", "Ge", "As", "Se", "Br", "Kr", "Rb", "Sr", "Y", "Zr",  // 40
    "Nb", "Mo", "Tc", "Ru", "Rh", "Pd", "Ag", "Cd", "In", "Sn",
    "Sb", "Te", "I", "Xe", "Cs", "Ba", "La", "Ce", "Pr", "Nd",  // 60
    "Pm", "Sm", "Eu", "Gd", "Tb", "Dy", "Ho", "Er", "Tm", "Yb",
    "Lu", "Hf", "Ta", "W", "Re", "Os", "Ir", "Pt", "Au", "Hg",  // 80
    "Tl", "Pb", "Bi", "Po", "At", "Rn", "Fr", "Ra", "Ac", "Th",
    "Pa", "U", "Np", "Pu", "Am", "Cm", "Bk", "Cf", "Es", "Fm",  // 100
    "Md", "No", "Lr", "Rf", "Db", "Sg", "Bh", "Hs", "Mt", "Uun"
};

bool Molecule::convertFromOBMol( OBMol * obmol )
{

    OBBond *bond;
    OBAtom *atom1, *atom2;
    DPoint *s, *e;
    Text *text;

    std::vector < OBBond * >::iterator bonditr;

    /*  Map OpenBabel atoms to the points that represent them.

        This used to be a std::map keyed on the atom's screen position, with a
        comparator ordering points by squared distance from the origin
        (`x*x + y*y`).  That is not a strict weak ordering: two distinct atoms
        can share a distance and are then treated as the same key, so the map's
        internal ordering was undefined.  The bond loop below then dereferenced
        find() without checking for end().

        That crashed with SIGSEGV on ubuntu-22.04 but not on 24.04 for a plain
        lactic-acid molfile — the signature of undefined behaviour rather than a
        wrong branch.  Keying on the atom removes the question.            */
    std::map < OBAtom *, DPoint * > atomPoints;

    float x, y;

    /* look through bonds and store points found since ChemData::addBond() requires
       DPoint*, and not DPoint. */

    for ( bond = obmol->BeginBond( bonditr ); bond; bond = obmol->NextBond( bonditr ) ) {

        atom1 = bond->GetBeginAtom();
        atom2 = bond->GetEndAtom();

        x = atom1->GetX();
        x = ( x + 5 ) * 15 + 100;
        y = atom1->GetY();
        y = ( y + 5 ) * 15 + 100;

        if ( atomPoints.find( atom1 ) == atomPoints.end() )
            atomPoints[atom1] = new DPoint( x, y );

        x = atom2->GetX();
        x = ( x + 5 ) * 15 + 100;
        y = atom2->GetY();
        y = ( y + 5 ) * 15 + 100;

        if ( atomPoints.find( atom2 ) == atomPoints.end() )
            atomPoints[atom2] = new DPoint( x, y );

    }

    /* use information from previous for-loop and add bonds and labels */

    for ( bond = obmol->BeginBond( bonditr ); bond; bond = obmol->NextBond( bonditr ) ) {

        atom1 = bond->GetBeginAtom();
        atom2 = bond->GetEndAtom();

        auto sit = atomPoints.find( atom1 );
        auto eit = atomPoints.find( atom2 );
        if ( sit == atomPoints.end() || eit == atomPoints.end() )
            continue;
        s = sit->second;
        e = eit->second;
        if ( s == nullptr || e == nullptr )
            continue;

        int bondorder = bond->GetBondOrder();

        //set elements
        if ( atom1->GetAtomicNum() != 6 ) {
            QString str( "<element>" );

            str += symbol[atom1->GetAtomicNum() - 1];
            str += "</element>";
            s->SetElementFromXML( str );

        }

        if ( atom2->GetAtomicNum() != 6 ) {
            QString str( "<element>" );

            str += symbol[atom2->GetAtomicNum() - 1];
            str += "</element>";
            e->SetElementFromXML( str );

        }

        addBond( s, e, 1, bondorder, QColor( "black" ), true );

        /*  Carry 2-D stereochemistry across into the internal model.
            Without this the wedge/hash flags on the OBBond are dropped, every
            Bond::stereo stays STEREO_UNDEFINED, ConvertToOBMol() writes no
            wedge back out, and Molecule::CalcCIPLabels() sees a flat drawing
            and reports every stereocentre as unspecified.  That is why R/S
            labels never appeared for structures opened from a molfile.     */
        /*  A wedge narrows at the bond's begin atom, which is the order
            addBond() above used, so the flags map straight onto Bond::order
            5 (up) and 7 (down).  setOrder() sets Bond::stereo as well, so the
            wedge reaches the drawing *and* the stereochemistry perception —
            setting only the stereo member left the bond drawn as a plain
            line, with a descriptor that pointed at an invisible wedge.     */
        if ( bond->IsWedge() || bond->IsHash() ) {
            Bond *xdcBond = FindBond( s, e );
            if ( xdcBond != nullptr ) {
                if ( bond->IsWedge() )
                    xdcBond->setOrder( 5 );
                else
                    xdcBond->setOrder( 7 );
            }
        }

        //label atoms if not Carbon

        if ( atom1->GetAtomicNum() != 6 ) {

            /*  Order matters: Text::setText() writes through `start` — the
                point the Text is attached to — and returns silently when that
                is still null.  Setting the text before the point left every
                heteroatom label empty, so a molfile carrying three oxygens
                rendered as a bare hydrocarbon skeleton.  Attach the point
                first, then the text.                                    */
            text = new Text( r );
            text->setPoint( s );
            text->setText( symbol[atom1->GetAtomicNum() - 1] );
            text->setJustify( JUSTIFY_CENTER );
            addText( text );
        }

        if ( atom2->GetAtomicNum() != 6 ) {

            text = new Text( r );
            text->setPoint( e );
            text->setText( symbol[atom2->GetAtomicNum() - 1] );
            text->setJustify( JUSTIFY_CENTER );
            addText( text );

        }
    }

    return true;
}

//
// TODO :)
//
OBMol *Molecule::convertToOBMol()
{
    QList < DPoint * >allpoints;
    QList < Bond * >allbonds;
    DPoint *tmp_atom;
    Bond *tmp_bond;
//    Molecule *this_mol;
    OBMol *obmol = new OBMol;

    allpoints = AllPoints();
    allbonds = AllBonds();

    // adapted from openbabel/mdl.cpp, most similar format to this...
    // (i.e., MDL is atom list followed by bond list)

    obmol->BeginModify();
    obmol->ReserveAtoms( allpoints.count() );
    vector3 v;
    OBAtom atom;

    for ( int i = 0; i < allpoints.count(); ++i ) {
        DPoint *tmp_atom = allpoints[i];
        v.SetX( tmp_atom->x );
        v.SetY( tmp_atom->y );
        v.SetZ( tmp_atom->z );
        atom.SetVector( v );
        atom.SetAtomicNum( tmp_atom->getAtomicNumber() );
        // Keep QByteArray alive for SetType; previous char type[5] strcpy
        // could overflow on user-defined atom labels (baseElement can
        // return arbitrary strings for atomic_number==999).
        QByteArray typeBytes = tmp_atom->baseElement().toLatin1();
        atom.SetType( typeBytes.constData() );
        // Set explicit atom ID so stereo configs can reference this atom.
        atom.SetId( static_cast<unsigned long>( i ) );

        if ( !obmol->AddAtom( atom ) )
            return NULL;
        atom.Clear();
    }

    int start, end, order, flag, bst;

    for (Bond *tmp_bond : allbonds) {
        flag = 0;
        start = allpoints.indexOf( tmp_bond->Start() ) + 1;
        end = allpoints.indexOf( tmp_bond->End() ) + 1;
        order = tmp_bond->Order();
        bst = tmp_bond->Stereo();
        /*  A wedge or hash is a single bond that happens to be drawn as one.
            XDrawChem stores that as Bond::order 5 (up) or 7 (down) — the same
            representation the CML reader uses — and sets Bond::stereo to
            match.  Open Babel's bond order must stay 1 or the bond is read as
            aromatic (5) or as a nonsense order (7), which wrecks perception
            and any downstream chemistry.  The wedge itself travels in the
            flags below.                                                */
        if ( order == 5 || order == 7 )
            order = 1;
        if ( bst == STEREO_UP )
            flag |= OB_WEDGE_BOND;
        if ( bst == STEREO_DOWN )
            flag |= OB_HASH_BOND;
        if ( !obmol->AddBond( start, end, order, flag ) )
            return NULL;
    }

    obmol->EndModify();
    // Coordinates are 2D (x,y with z=0), tell OpenBabel so it uses
    // StereoFrom2D() for stereochemistry perception.
    obmol->SetDimension(2);

    return obmol;
}

//eof

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
