#ifndef DPOINT_H
#define DPOINT_H

#include <cmath>

#include <QString>
#include <QColor>
#include <QFont>
#include <QList>
#include <QVector>
#include <QPoint>
#include <QTextStream>
#include <QTextDocument>
#include <QDebug>

class Bond;

class DPoint
{
public:
    double x, y, z;
    double x3, y3, z3;
    double dx, dy, dz;  // where to go on this iteration of (3D/minimize)
    QString id; // XML ID field
    QString element;  // OR text string if associated with Text
    QString elementmask; // (updated) actual display text
    QString symbol;  // only used when saving Molecules as XDC-XML
    QString hosecode; // save HOSE codes made in molecule_tools.cpp
    QString tmphose; // temp HOSE code (if in first sphere)
    int hosescore;  // HOSE score
    int ketos; // number of keto groups attached (if any)
    bool aromatic; // part of an aromatic system?
    bool inring;  // part of a ring?
    QString ringset;  // SMILES ring closure code(s) for this atom
    QString hybrid;  // hybridization (sp2, etc...) for 2D and 3D layout
    QColor color;
    QFont font;
    bool hit;
    int substituents;  // how many bonds to this atom (sum of order)
    DPoint *other;  // other end of single bond.  See molecule.cpp
    Bond *otherBond;  // single Bond containing this DPoint
    // for SDG and SSSR
    DPoint *source;
    int serial;
    QList<DPoint *> neighbors;
    QVector<int> bondorder;
    QList<DPoint *> path;
    // for reconstructing rings...
    int new_order;
    // reactivity
    double partial_charge;  // + or -, should also set "color" above
    QString react_atom_type;  // AMBER atom type?  Custom?
    // spectral data
    bool nmr_proton;  // is this DPoint a proton added for 1H NMR prediction?
    double H1_shift;
    int H1_multiplicity;
    QString protonHOSECode;
    double C13_shift;
    QString C13_multiplicity;
    double partialCharge;
    double getC13_shift() { return C13_shift; }
    QString getC13_multiplicity() { return C13_multiplicity; }
    void setC13_shift( double in1 ) { C13_shift = in1; }
    void setC13_multiplicity( QString in1 ) { C13_multiplicity = in1; }
    bool highlighted;
    // functions
    DPoint() {
        x = 0.0; y = 0.0; z = 0.0;
        init();
    }

    DPoint( double a, double b ) {
        x = a; y = b; init();
    }

    DPoint( QPoint q ) {
        x = q.x(); y = q.y(); init();
    }

    DPoint( DPoint *op ) {
        x = op->x;
        y = op->y;
        z = op->z;
        id = op->id;
        element = op->element;
        elementmask = op->elementmask;
        symbol = op->symbol;
        hosecode = op->hosecode;
        tmphose = op->tmphose;
        hosescore = op->hosescore;
        ketos = op->ketos;
        aromatic = op->aromatic;
        inring = op->inring;
        ringset = op->ringset;
        color = op->color;
        font = op->font;
        hit = op->hit;
        substituents = op->substituents;
        other = op->other;
        otherBond = op->otherBond;
        serial = op->serial;
        bondorder.resize(6);
    }

    void init() {
        highlighted = false;
        nmr_proton = false;
        element = QString("C");
        elementmask = QString(" ");
        symbol = QString("");
        color = QColor(0,0,0);
        font = QFont("Helvetica", 12);
        C13_shift = 0.0;
        C13_multiplicity = "NA";
        aromatic = false;
        serial = 0;
        hit = false;
        bondorder.resize(6);
        partialCharge = 0.0;
    }
    // these would be more relevant... if "substituents" were private...
    int degree() { return substituents; }
    void setDegree( int neworder ) { substituents = neworder; }
    // same here -- highlighted should be private
    bool isHighlighted() { return highlighted; }
    void setHighlighted( bool neworder ) { highlighted = neworder; }
    // same here -- partialCharge should be private
    double getCharge() { return partialCharge; }
    void setCharge( double nc1 ) { partialCharge = nc1; }

    void set( QPoint p ) {
        x = (double)p.x();
        y = (double)p.y();
    }

    void set( double xn, double yn, double zn ) {
        x3 = xn;
        y3 = yn;
        z3 = zn;
    }

    QPoint toQPoint() {
        int x1 = (int)x;
        int y1 = (int)y;
        double xf = x - (double)x;
        double yf = y - (double)y;
        if (xf > 0.5) x1++;
        if (yf > 0.5) y1++;
        return QPoint( x1, y1 );
    }

    void zero() { x = 0.0; y = 0.0; z = 0.0; }

    bool isZero() {
        if ( ( x < 0.01 ) && ( x > -0.01 ) &&
        ( y < 0.01 ) && ( y > -0.01 ) )
        return true;
        else
        return false;
    }

    double distanceTo( DPoint *t ) {
        double dx = x - t->x;
        double dy = y - t->y;
        return (double)sqrt( dx * dx + dy * dy );
    }

    double Magnitude() {
        return sqrt( (x3*x3) + (y3*y3) + (z3*z3) );
    }

    void Normalize() {
        double dst = Magnitude();
        x3 /= dst;
        y3 /= dst;
        z3 /= dst;
    }

    void Scale( double sf ) {
        double dst = Magnitude();
        Normalize();
        dst = dst * sf;
        x3 *= dst;
        y3 *= dst;
        z3 *= dst;
    }

    void SetMagnitude( double sf ) {
        double dst = Magnitude();
        sf = sf / dst;
        Normalize();
        dst = dst * sf;
        x3 *= dst;
        y3 *= dst;
        z3 *= dst;
    }

    void SetFontFromXML( QString xml_tag ) {
        qDebug() << "DP::SetFontFromXML:" << xml_tag;
        int i1, i2;

        i1 = xml_tag.indexOf( "<font>" );
        i2 = xml_tag.indexOf( "</font>" );
        xml_tag.remove( i2, 999 );
        xml_tag.remove( i1, 6 );
        i1 = xml_tag.indexOf( "#" );
        qDebug() << xml_tag.mid( 0, i1 ) << "*" << xml_tag.mid( i1 + 1 );
        font = QFont(xml_tag.mid( 0, i1 ), xml_tag.mid( i1 + 1 ).toInt());
    }

    void SetColorFromXML( QString xml_tag ) {
        qDebug() << "DP::SetColorFromXML:" << xml_tag;
        int i1, i2;
        int d1, d2, d3;

        i1 = xml_tag.indexOf( "<color>" );
        i2 = xml_tag.indexOf( "</color>" );
        xml_tag.remove( i2, 999 );
        xml_tag.remove( i1, 7 );

        QTextStream ts( &xml_tag, QIODevice::ReadOnly );

        ts >> d1 >> d2 >> d3;

        color = QColor(d1, d2, d3);
    }

    // set DPoint coordinates from XML
    void SetCoordFromXML( QString xml_tag ) {
        qDebug() << "DP::SetCoordFromXML:" << xml_tag;
        int i1, i2;

        i1 = xml_tag.indexOf( "<coordinate2>" );
        i2 = xml_tag.indexOf( "</coordinate2>" );
        xml_tag.remove( i2, 999 );
        xml_tag.remove( i1, 13 );

        QTextStream ts( &xml_tag, QIODevice::ReadOnly );

        ts >> x >> y;
    }

    void SetElementFromXML( QString xml_tag ) {
        qDebug() << "DP::SetElementFromXML:" << xml_tag;
        int i1, i2;

        i1 = xml_tag.indexOf( "<element>" );
        i2 = xml_tag.indexOf( "</element>" );
        xml_tag.remove(i2, 999);
        xml_tag.remove(i1, 9);

        element = xml_tag;
    }

    void SetElementMaskFromXML( QString xml_tag ) {
        qDebug() << "DP::SetElementMaskFromXML:" << xml_tag;
        int i1, i2;

        i1 = xml_tag.indexOf( "<elemask>" );
        i2 = xml_tag.indexOf( "</elemask>" );
        xml_tag.remove(i2, 999);
        xml_tag.remove(i1, 9);

        elementmask = xml_tag;
    }

    void SetSymbolFromXML( QString xml_tag ) {
        qDebug() << "DP::SetSymbolFromXML:" << xml_tag;
        int i1, i2;

        i1 = xml_tag.indexOf( "<symtype>" );
        i2 = xml_tag.indexOf( "</symtype>" );
        xml_tag.remove( i2, 999 );
        xml_tag.remove( i1, 9 );

        symbol = xml_tag;
    }

    void FromXML( QString xml_tag ) {
        int i1, i2, ib, ie;

        i1 = xml_tag.indexOf("<atom ");
        ib = xml_tag.indexOf("\"", i1);
        ie = xml_tag.indexOf("\"", ib + 1);
        id = xml_tag.mid(ib + 1, ie - ib - 1);
        qDebug() << "DP::AtomID:" << id;
        i1 = xml_tag.indexOf("<coordinate2>");
        i2 = xml_tag.indexOf("</coordinate2>") + 14;
        SetCoordFromXML(xml_tag.mid(i1, i2 - i1));
        i1 = xml_tag.indexOf("<color>");
        i2 = xml_tag.indexOf("</color>") + 8;
        SetColorFromXML(xml_tag.mid(i1, i2 - i1));
        i1 = xml_tag.indexOf("<font>");
        i2 = xml_tag.indexOf("</font>") + 7;
        SetFontFromXML(xml_tag.mid(i1, i2 - i1));
        i1 = xml_tag.indexOf("<element>");
        i2 = xml_tag.indexOf("</element>") + 10;
        SetElementFromXML(xml_tag.mid(i1, i2 - i1));
        i1 = xml_tag.indexOf("<elemask>");
        i2 = xml_tag.indexOf("</elemask>") + 10;
        SetElementMaskFromXML(xml_tag.mid(i1, i2 - i1));
        i1 = xml_tag.indexOf("<symtype>");
        i2 = xml_tag.indexOf("</symtype>") + 10;
        if (i1 >= 0)
        SetSymbolFromXML(xml_tag.mid(i1, i2 - i1));
        i1 = xml_tag.indexOf("<textobject>yes</textobject>");
        if (i1 >= 0) hit = true;  // Create Text object associated with this DPoint
    }
    // return atomic number of atom connected in this group
    int getAtomicNumber() {
      QString localElement;
      QTextDocument localText;
      localText.setHtml(element);
      //localElement = localText.toPlainText();
      localElement = element;
      qDebug() << "localElement:" << localElement;
      int atom1 = 999;
      if (localElement == "H") atom1 = 1;
      if (localElement == "C") atom1 = 6;
      if (localElement == "CN") atom1 = 6;
      if (localElement == "NC") atom1 = 6;
      if (localElement == "CO") atom1 = 6;
      if (localElement == "OC") atom1 = 6;
      if (localElement == "HC") atom1 = 6;
      if (localElement == "CH") atom1 = 6;
      if (localElement == "H2C") atom1 = 6;
      if (localElement == "CH2") atom1 = 6;
      if (localElement == "H3C") atom1 = 6;
      if (localElement == "CH3") atom1 = 6;
      if (localElement == "N") atom1 = 7;
      if (localElement == "NH") atom1 = 7;
      if (localElement == "HN") atom1 = 7;
      if (localElement == "NO") atom1 = 7;
      if (localElement == "ON") atom1 = 7;
      if (localElement == "NH2") atom1 = 7;
      if (localElement == "H2N") atom1 = 7;
      if (localElement == "NO2") atom1 = 7;
      if (localElement == "O2N") atom1 = 7;
      if (localElement == "O") atom1 = 8;
      if (localElement == "OH") atom1 = 8;
      if (localElement == "HO") atom1 = 8;
      if (localElement == "F") atom1 = 9;
      if (localElement == "P") atom1 = 15;
      if (localElement == "S") atom1 = 16;
      if (localElement == "SH") atom1 = 16;
      if (localElement == "HS") atom1 = 16;
      if (localElement == "Cl") atom1 = 17;
      if (localElement == "Br") atom1 = 35;
      if (localElement == "I") atom1 = 53;
      return atom1;
    }
    QString baseElement() {
        int tc1 = getAtomicNumber();
        if (tc1 == 1) return "H";
        if (tc1 == 6) return "C";
        if (tc1 == 7) return "N";
        if (tc1 == 8) return "O";
        if (tc1 == 9) return "F";
        if (tc1 == 15) return "P";
        if (tc1 == 16) return "S";
        if (tc1 == 17) return "Cl";
        if (tc1 == 35) return "Br";
        if (tc1 == 53) return "I";
        if (tc1 == 999) {
        // let's try this...
        QString bele = element;
        bele.remove("+");
        bele.remove("-");
        bele.remove("H");
        bele.remove( QRegExp("\\d+") );
        return bele;
        }
        return "X";
    }
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
