// gobject.h -- subclass of Drawable for graphic objects

#ifndef GOBJECT_H
#define GOBJECT_H

#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class QRect;
class QString;
class QPolygon;

class GraphicObject : public Drawable
{
public:
    GraphicObject( Render2D *, QObject *parent = 0 );
    void Render();  // draw this object
    void Edit();
    int Type();  // return type of object
    bool Find( DPoint * ); // does this GraphicObject contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    Drawable *FindNearestObject( DPoint *, double & );
    void setPoints( DPoint *, DPoint * );
    void setPointArray( QPolygon );
    bool isWithinRect( QRect, bool );
    QRect BoundingBox();
    QString ToXML( QString );
    QString ToCDXML( QString );
    void FromXML( QString );
    int Style() { return style; }
    void SetStyle( int s ) { style = s; }
    int objectType() { return ot; }
    void setObjectType( int t ) { ot = t; }
private:
    // Renderer
    Render2D *r;
    // Point array (for cubic bezier and possibly others)
    QPolygon objectPoints;
    // graphic object type
    int ot;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
