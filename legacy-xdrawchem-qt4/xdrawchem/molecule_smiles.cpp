// molecule_smiles.cpp - Molecule's implementation of even more functions,
// notably, Structural Diagram Generation and SMILES

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include "render2d.h"
#include "drawable.h"
#include "molecule.h"
#include "dpoint.h"
#include "text.h"
#include "atom.h"
#include "ring.h"
#include "setofrings.h"
#include "sdg.h"
#include "defs.h"

using namespace OpenBabel;

// CleanUp(): Invoke SDG() to clean up structure
void Molecule::CleanUp()
{
    SDG( true );
}

// Structure Diagram Generation - add coordinates to connectivity
// generally called after reading in a file or SMILES string which did
// not supply coordinates.  Could also be used to clean up Molecule.
// bool coord: have coordinates been set?  (true = coordinates exist, e.g.
// from file or hand drawing; false = no, strcuture supplied as connection
// table from SMILES or file)
// Method references:
// 1) Ugi I et al., Journal of Chemical research (M), 1991, 2601-2689
// 2) Christoph Steinbeck's Java implentation of above, JMDraw.
//    http://jmdraw.sourceforge.net/
void Molecule::SDG( bool coord )
{
    QRect bb1;
    QPoint center1, center2;

    if ( coord == true ) {      // if coordinates exist, save center of bounding box
        bb1 = BoundingBoxAll();
        center1 = bb1.center();
    }
    //DPoint *t2;
    Atom *a1;

    // get unique points
    up = AllPoints();
    QVector < Atom * >atoms( up.count() );

    // clear "hit" flag on all atoms
    foreach ( tmp_pt, up )
        tmp_pt->hit = false;
    // find rings (esp. find aromaticity) - do after CopyTextToDPoint()
    MakeSSSR();

    // convert "up" to JMDraw-friendly Qlist<Atom>
    // rebuild neighbors list (usually mangled by MakeSSSR)
    foreach ( tmp_pt, up ) {
        tmp_pt->neighbors.clear();
        foreach ( tmp_bond, bonds ) {
            if ( tmp_bond->Find( tmp_pt ) == true ) {
                tmp_pt->neighbors.append( tmp_bond->otherPoint( tmp_pt ) );
                tmp_pt->bondorder[( tmp_pt->neighbors.count() - 1 )] = tmp_bond->Order();
            }
        }
    }
    // first copy all DPoints
    int c1, c2, refnum;

    for ( c1 = 0; c1 < up.count(); c1++ ) {
        tmp_pt = up.at( c1 );
        a1 = new Atom( tmp_pt->element, tmp_pt->x, tmp_pt->y, tmp_pt->z );
        a1->number = tmp_pt->serial;
        a1->degree = tmp_pt->neighbors.count();
        //qDebug() << c1 << "-degree-" << a1->degree ;
        atoms.insert( c1, a1 );
    }
    // now build connectivity table
    for ( c1 = 0; c1 < up.count(); c1++ ) {
        tmp_pt = up.at( c1 );
        a1 = atoms.at( c1 );
        for ( c2 = 0; c2 < tmp_pt->neighbors.count(); c2++ ) {
            refnum = tmp_pt->neighbors.at( c2 )->serial;
            a1->nodeTable.insert( c2, atoms.at( refnum ) );
            a1->bondTable[c2] = tmp_pt->bondorder[c2];
            a1->intnodeTable[c2] = refnum;
        }
    }
    /* convert this_sssr to setOfRings
       setOfRings s1;
       s1.resize(this_sssr.sssr.count());
       Ring *ring1;
       QPtrList<DPoint> *tmp_ring;
       c1 = 0;
       for (tmp_ring = this_sssr.sssr.first(); tmp_ring != 0;
       tmp_ring = this_sssr.sssr.next()) {
       ring1 = new Ring;
       ring1->resize(tmp_ring->count());
       c2 = 0;
       for (tmp_pt = tmp_ring->first(); tmp_pt != 0; tmp_pt = tmp_ring->next()) {
       a1 = atoms.at(tmp_pt->serial);
       ring1->insert(c2, a1);
       qDebug() << "RA" << c2 << ":" << tmp_pt->serial ;
       c2++;
       }
       ring1->sort2();
       s1.insert(c1, ring1);
       c1++;
       }
     */
    // pass to SDG class.
    class SDG sdg1;

    sdg1.setAtoms( atoms );
    //sdg1.setRings(s1);
    sdg1.setBL( preferences.getBond_fixedlength() );
    sdg1.exec();
    atoms = sdg1.getAtoms();

    qDebug() << "SDG succeeded!";

    // convert atoms back to DPoint (essentially, just update x,y coordinates)
    for ( c1 = 0; c1 < up.count(); c1++ ) {
        tmp_pt = up.at( c1 );
        a1 = atoms.at( c1 );
        tmp_pt->x = a1->x;
        tmp_pt->y = a1->y;
    }

    bb1 = BoundingBoxAll();
    int xmove = 0, ymove = 0;

    if ( coord == true ) {      // if coordinates existed, move back into place
        center2 = bb1.center();
        xmove = center1.x() - center2.x();
        ymove = center1.y() - center2.y();
    } else {                    // move to top left of screen
        if ( bb1.left() < 10 )
            xmove = 10 - bb1.left();
        if ( bb1.top() < 10 )
            ymove = 10 - bb1.top();
    }
    foreach ( tmp_pt, up ) {
        tmp_pt->x += xmove;
        tmp_pt->y += ymove;
    }

    // add hydrogens
    AddHydrogens();
}

// cheat: use Babel to make InChI strings...
QString Molecule::ToInChI() {
  std::istringstream istream( ToMDLMolfile().toLatin1().constData() );
  std::ostringstream ostream;
  
  OBConversion Conv( &istream, &ostream );
  OBFormat *pInFormat = 0, *pOutFormat = 0;
  
  pInFormat = Conv.FindFormat( "mol" );
  pOutFormat = Conv.FindFormat( "inchi" );
  
  Conv.SetInAndOutFormats( pInFormat, pOutFormat );
  //Conv.Convert();
  OBMol mol;
  Conv.Read(&mol);
  Conv.Write(&mol);
  
  // convert the string into a terminated c string
  
  std::string s = ostream.str();
  s[s.length() - 1] = '\0';
  return ( QString( s.c_str() ) );
}

// cheat: use Babel to make SMILES strings.
QString Molecule::ToSMILES()
{
    QString smiles;

    QByteArray molfile = ToMDLMolfile().toLatin1();

    qDebug() << molfile;

    std::istringstream istream( ToMDLMolfile().toLatin1().constData() );
    std::ostringstream ostream;

    /* obsolete?
       OpenBabel::OBMol myMol(OpenBabel::SDF,OpenBabel::SMI);
       OpenBabel::OBFileFormat fileFormat;

       fileFormat.ReadMolecule(istream, myMol, "tmpname");
       fileFormat.WriteMolecule(ostream, myMol);
     */

    OBConversion Conv( &istream, &ostream );
    OBFormat *pInFormat = 0, *pOutFormat = 0;

    pInFormat = Conv.FindFormat( "mol" );
    pOutFormat = Conv.FindFormat( "smi" );

    Conv.SetInAndOutFormats( pInFormat, pOutFormat );
    //Conv.Convert();
    OBMol mol;
    Conv.Read(&mol);
    Conv.Write(&mol);

    // convert the string into a terminated c string

    std::string s = ostream.str();
    s[s.length() - 1] = '\0';
    return ( QString( s.c_str() ) );
}

// convert InChI string to Molecule (using Babel!)
// (Ideally, you should call this function just after creating)
void Molecule::FromInChI( QString sm )
{
}

// convert SMILES string to Molecule
// (Ideally, you should call this function just after creating)
void Molecule::FromSMILES( QString sm )
{

    QByteArray smArray = sm.toLatin1();

    std::istringstream istream( smArray.constData() );        // build a stream on the string

    /* obsolete?
       OpenBabel::OBMol myMol(OpenBabel::SMI, OpenBabel::SDF);
       OpenBabel::OBFileFormat fileFormat;

       fileFormat.ReadMolecule(istream, myMol, "tmpname");
     */

    OBMol myMol;
    OBConversion conv;
    OBFormat *format = conv.FindFormat( "smi" );

    conv.SetInAndOutFormats( format, format );
    myMol.Clear();
    conv.Read( &myMol, &istream );

    // now convert the molecule into XDC's internal representation

    OpenBabel::OBAtom * thisAtom;
    //int i;

    std::vector < DPoint * >avec;

    QString tmp_element, tmp_element_mask;

    DPoint *thisDPoint;

    OpenBabel::OBElementTable etable;

    std::vector < OpenBabel::OBNodeBase * >::iterator ait;

    //i = 0;

    std::map < OpenBabel::OBAtom *, DPoint * >hashit;

    for ( thisAtom = myMol.BeginAtom( ait ); thisAtom; thisAtom = myMol.NextAtom( ait ) ) {

        //qDebug() << "Adding OBAtom: " << i++ << " of element#: " <<
        //thisAtom->GetAtomicNum() << " type: " <<
        //  etable.GetSymbol(thisAtom->GetAtomicNum()) ;
        thisDPoint = new DPoint;
        tmp_element = etable.GetSymbol( thisAtom->GetAtomicNum() );

        tmp_element_mask = tmp_element;
        tmp_element_mask.fill( ' ' );   // fix the mask characters

        thisDPoint->element = tmp_element;
        thisDPoint->elementmask = tmp_element_mask;

        if ( tmp_element != "C" ) {
            Text *nt = new Text( r );

            nt->setPoint( thisDPoint );
            nt->setJustify( JUSTIFY_CENTER );
            nt->Highlight( false );
            nt->setText( tmp_element );
//            nt->setTextMask( tmp_element_mask );
            labels.append( nt );
        }

        avec.push_back( thisDPoint );

        hashit[thisAtom] = thisDPoint;
    }

    OpenBabel::OBBond * thisBond;

    std::vector < OpenBabel::OBEdgeBase * >::iterator bit;
    for ( thisBond = myMol.BeginBond( bit ); thisBond; thisBond = myMol.NextBond( bit ) ) {
        addBond( hashit[thisBond->GetBeginAtom()], hashit[thisBond->GetEndAtom()], 1, thisBond->GetBondOrder(), QColor( 0, 0, 0 ), true );
    }

    SDG( false );               // generate structure coordinates
}

//
// OLD CODE
//
/* convert SMILES string to Molecule
// (Ideally, you should call this function just after creating)
void Molecule::FromSMILES(QString sm) {
  QStringList tokens;
  QPtrStack<DPoint> branch_tree;
  QPtrVector<DPoint> ring_closure_array(10);
  bool ring_array_status[10]; // track which elements of ring array are used

  for (int cc = 0; cc < 10; cc++)
    ring_array_status[cc] = false;

  // tokenize
  // tokens: atoms, groups enclosed in [], (, )
  // note that numbers and symbols outside [] will break the tokenizer
  QStringList smilesTokens;
  QString prev_token, tmp_token;
  int i1;

  qDebug() << "SMILES:" << sm << "|" ;

  do {
    tmp_token = "";
    if (sm[0] == '=') { // double bond
      tmp_token.append("=");
      sm.remove(0,1);
    }
    if (sm[0] == '#') { // triple bond
      tmp_token.append("#");
      sm.remove(0,1);
    }
    if (sm[0].isLetter()) {
      // extract letter token
      // look for single-letter aromatic
      if (sm[0].toLower() == sm[0]) {
	tmp_token.append(sm.left(1));
	sm.remove(0,1);
      } else {
	if ( (sm[1].toLower() == sm[1]) &&
	     (sm[1].isLetter()) ) { // lowercase; two-letter symbol
	  tmp_token.append(sm.left(2));
	  sm.remove(0,2);
	} else { //
	  tmp_token.append(sm.left(1));
	  sm.remove(0,1);
	}
      }
      // extract ring closure numbers
      if (sm.length() > 0) {
	do {
	  if (sm[0].isNumber()) {
	    tmp_token.append(sm.left(1));
	    sm.remove(0,1);
	  } else {
	    break;
	  }
	} while (sm.length() > 0);
      }
      smilesTokens.append(tmp_token);
    }
    if (sm[0] == '(') {
      tmp_token = "(";
      smilesTokens.append(tmp_token);
      sm.remove(0,1);
    }
    if (sm[0] == ')') {
      tmp_token = ")";
      smilesTokens.append(tmp_token);
      sm.remove(0,1);
    }
    if (sm[0] == '[') {
      i1 = sm.indexOf("]");
      tmp_token = sm.left(i1);
      smilesTokens.append(tmp_token);
    }
    //qDebug() << "token: " << tmp_token << endl << "left: " << sm ;
  } while (sm.length() > 0);

  DPoint *prev_pt = 0, *new_pt = 0;
  QString tmp_element, tmp_element_mask;
  tmp_token = "";
  prev_token = "";
  int bond_order = 0;
  bool aromatic = false, flag = false;

  for ( QStringList::Iterator it = smilesTokens.begin();
	it != smilesTokens.end();
	++it ) {
    prev_token = tmp_token;
    tmp_token = (*it).toLatin1();

    qDebug() << "token: " << tmp_token ;

    // process tokens
    if (tmp_token == "(") { // start branch
      branch_tree.push(prev_pt);
      continue;
    }
    if (tmp_token == ")") { // end branch
      prev_pt = branch_tree.pop();
      continue;
    }
    // if not a branch, it's probably an atom
    new_pt = new DPoint;
    // calculate bond order
    aromatic = false;
    if (tmp_token[0].isLetter()) {
      if (tmp_token[0].toLower() == tmp_token[0]) new_pt->aromatic = true;
    }
    if (prev_pt != 0) {
      bond_order = 1;
      if ( prev_pt->aromatic && new_pt->aromatic ) {bond_order = 4;}
    }
    if (tmp_token.left(1) == "=") {
      bond_order = 2;
      tmp_token.remove(0,1);
    }
    if (tmp_token.left(1) == "#") {
      bond_order = 3;
      tmp_token.remove(0,1);
    }
    // extract element info
    tmp_element = ""; tmp_element_mask = "";
    if (tmp_token[0].isLetter()) {
      tmp_element = tmp_token.left(1);
      tmp_element = tmp_element.toUpper();
    }
    if (tmp_token[1].isLetter()) tmp_element.append(tmp_token[1]);
    if (tmp_token[0] == '[') {
      int i1 = tmp_token.indexOf("]");
      tmp_element = tmp_token.mid(1, i1 - 2);
    }
    qDebug() << "element: " << tmp_element ;
    tmp_element_mask = tmp_element;
    tmp_element_mask.fill(' ');
    // add super/sub-script where appropriate

    new_pt->element = tmp_element;
    new_pt->elementmask = tmp_element_mask;
    // create Text where appropriate
    if (new_pt->element != "C") {
      Text *nt = new Text(r);
      nt->setPoint(new_pt);
      nt->setJustify(JUSTIFY_CENTER);
      nt->Highlight(false);
      nt->setText(tmp_element);
      nt->setTextMask(tmp_element_mask);
      labels.append(nt);
    }
    // create bond
    if (prev_pt != 0) {
      addBond(prev_pt, new_pt, 1, bond_order, QColor(0,0,0), true);
      prev_pt = new_pt;
    } else {
      prev_pt = new_pt;
    }
    // handle ring closure
    do {
      flag = false;
      if (tmp_token.at(tmp_token.length() - 1).isNumber()) {
	int ringnum = tmp_token.right(1).toInt();
	flag = true;
	if (ring_array_status[ringnum] == false) { // save this atom
	  ring_closure_array.insert(ringnum, new_pt);
	  ring_array_status[ringnum] = true;
	} else { // do ring closure
	  tmp_pt = ring_closure_array.at(ringnum);
	  if (tmp_pt->aromatic && new_pt->aromatic)
	    addBond(tmp_pt, new_pt, 1, 4, QColor(0,0,0), true);
	  else
	    addBond(tmp_pt, new_pt, 1, 1, QColor(0,0,0), true);
	  ring_array_status[ringnum] = false;
	}
	tmp_token.remove(tmp_token.length() - 1, 1);
      }
    } while (flag == true);
  }

  // print atom list and connection table?
  qDebug() << ToXML("smiles") ;

  SDG(false);  // generate structure coordinates
}
//
// OLD CODE
//
*/

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
