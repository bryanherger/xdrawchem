// bond.h -- subclass of Drawable for lines/bonds

#ifndef BOND_H
#define BOND_H

#include <QRect>
#include <QPoint>

#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"
#include "moldata.h"
#include "vector2D.h"
#include "algorithm"

#define BOND_LEFT 0
#define BOND_RIGHT 1
#define BOND_CENTER 2

class Bond : public Drawable
{
public:
    Bond( Render2D *, QObject *parent = 0 );
    void Render();  // draw this object
    void Edit();  // open edit window

    void RenderSide( int w )
    {
        if ( auto_wside == 1 )
            wside = w;
    }

    int Type();  // return type of object
    bool Find( DPoint * ); // does this Bond contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    Drawable *FindNearestObject( DPoint *, double & );
    void setPoints( DPoint *, DPoint * );
    bool Equals( Bond * );
    bool isWithinRect( QRect, bool );
    int Order() { return order; }

    int baseOrder()
    {
        if (order == 5) return 1;
        if (order == 7) return 1;
        return order;
    }

    void setOrder( int );
    int Stereo() { return stereo; }
    void setStereo( int a ) { stereo = a; }
    int Dash() { return dashed; }
    void setDash( int d ) { dashed = d; }
    QRect BoundingBox();
    DPoint *otherPoint( DPoint * );
    void setIR_shift( double in1 ) { IR_shift = in1; }
    void setIR_intensity( QString in1 ) { IR_intensity = in1; }
    double getIR_shift() { return IR_shift; }
    QString getIR_intensity() { return IR_intensity; }
    double Enthalpy();
    double Length();
    void setDoubleBondAlign( int db1 ) { wside = db1; }
    int getDoubleBondAlign() { return wside; }
    void setDoubleBondAuto( int db1 ) { auto_wside = db1; }
    int getDoubleBondAuto() { return auto_wside; }
    void resetReactions() { partial_charge = -999.0; rxnlist = "NONE"; }
    void setPC( double x1 ) { partial_charge = x1; }
    double getPC() { return partial_charge; }
    void setReactions( QString rxn1 ) { rxnlist = rxn1; }
    QString getReactions() { return rxnlist; }
    void setCName( QString cn1 ) { cname = cn1; }
    QString CName() { return cname; }

    bool isCHBond()
    {
        if (start->element == "H") return true;
        if (end->element == "H") return true;
        return false;
    }

    bool aromatic()
    {
        if ( (start == 0) || (end == 0) )
            return false;
        if (start->aromatic && end->aromatic)
            return true;
        return false;
    }

    double length()
    {
        int e1, e2;
        e1 = start->getAtomicNumber();
        e2 = end->getAtomicNumber();
        return MolData::bondLength(e1, e2, order);
    }

    QPoint midpoint()
    {
        double dx, dy;
        dx = ( end->x + start->x ) / 2.0;
        dy = ( end->x + start->x ) / 2.0;
        QPoint mp1( (int)dx, (int)dy );
        return mp1;
    }

    /**
     * @brief getAngleBetween Gets the angle between this bond and another one
     * @param bond The bond to calculate the angle with
     * @return The angle between the bonds in radians
     */
    double getAngleBetween(Bond* bond) {
        Vector2D* thisVector = this->toVector();
        Vector2D* otherVector = bond->toVector();

        thisVector->normalize();
        otherVector->normalize();

        thisVector->normalize();
        otherVector->normalize();
        double angle = acos(thisVector->dotProduct(otherVector));

        delete thisVector;
        delete otherVector;

        return angle;
    }

    /**
     * @brief getAngleBetweenDirectional Gets the angle this bond has to be rotated (via a standard rotational matrix) in order
     * to point int the same direction as the given one
     * @param bond The bond to calculate the angle to
     * @return The respective angle
     */
    double getAngleBetweenDirectional(Bond* bond) {
        Vector2D* thisVector = this->toVector();
        Vector2D* otherVector = bond->toVector();

        if (this->start != bond->start) {
            thisVector->reverse();
        }

        thisVector->normalize();
        otherVector->normalize();

        double angle = atan2(thisVector->x * otherVector->y - thisVector->y * otherVector->x,
                             thisVector->x * otherVector->x + thisVector->y * otherVector->y);

        delete thisVector;
        delete otherVector;

        return angle < 0 ? 2 * M_PI + angle : angle;
    }

    /**
     * @brief getAngleBetweenDirectional Gets the angle this bond has to be rotated
     * (via a standard rotational matrix in a left-handed coordinate system) in order
     * to point int the same direction as the given one
     * @param bond The bond to calculate the angle to
     * @return The respective angle
     */
    double getAngleBetweenDirectionalLeftHanded(Bond* bond) {
        Vector2D* thisVector = this->toVector();
        Vector2D* otherVector = bond->toVector();

        if (this->start != bond->start) {
            thisVector->reverse();
        }

        thisVector->normalize();
        otherVector->normalize();

        // minus because of left-handed coordinate system
        double angle = atan2(- thisVector->crossProduct(otherVector), thisVector->dotProduct(otherVector));

        delete thisVector;
        delete otherVector;

        return angle < 0 ? 2 * M_PI + angle : angle;
    }

    /**
     * @brief toVector Transforms this bond into a vector pointing from the starting point to the end point
     * @return The respective vector
     */
    Vector2D* toVector() {
        return new Vector2D(end->x - start->x, end->y - start->y);
    }

    /**
     * @brief reverse Reverses the direction of this bond by switching its start and end point
     */
    void reverse();

private:
    // Renderer
    Render2D *r;
    // order of bond (1-3, or 5 if up, or 7 if down)
    int order;
    // stereochemistry flag
    // values could be STEREO_UNDEFINED, STEREO_UP, STEREO_DOWN, STEREO_E,
    // STEREO_Z
    int stereo;
    // is one or more of the lines dashed
    int dashed;
    // which side to draw double bond on?
    // standing at start and looking at end
    // 0 = left, 2 = center, 1 = right; see #define's above
    // auto_wside: 0 = no auto, 1 = auto (false/true)
    int wside, auto_wside;
    // IR data
    double IR_shift;
    QString IR_intensity;
    // reactivity
    double partial_charge;
    QString rxnlist;
    // bond name
    QString cname;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
