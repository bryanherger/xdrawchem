// curvearrow.h -- subclass of Drawable for curvearrows

#ifndef CURVEARROW_H
#define CURVEARROW_H

#include <QRect>
#include <QString>

#include "render2d.h"
#include "drawable.h"
#include "dpoint.h"

class CurveArrow : public Drawable
{
public:
    CurveArrow( Render2D *, QObject *parent = 0 );
    void Render();  // draw this object
    void Edit();
    int Type();  // return type of object
    bool Find( DPoint * ); // does this CurveArrow contain this DPoint?
    DPoint *FindNearestPoint( DPoint *, double & );
    Drawable *FindNearestObject( DPoint *, double & );
    void setPoints( DPoint *, DPoint * );
    QRect BoundingBox();
    bool isWithinRect( QRect, bool );
    QString ToXML( QString );
    void FromXML( QString );
    void SetCurve( const QString &t ) { which = t; }
    QString GetCurve() { return which; }

private:
    // Renderer
    Render2D *r;
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
