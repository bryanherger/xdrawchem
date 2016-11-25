#include <QList>

#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "curvearrow.h"
#include "bracket.h"
#include "text.h"
#include "symbol.h"
#include "gobject.h"
#include "chemdata.h"
#include "defs.h"

void ChemData::XDCEventHandler( XDC_Event *evt )
{
    if ( evt->undoable() ) {
        StartUndo( 0, 0 );
    }

    GraphicObject *go;

    switch ( evt->type() ) {
    case EVT_ADD_BOND:
        if ( evt->Param2() < 0 )
            addBond( evt->Start(), evt->End(), evt->Param1(), 1, evt->color(), evt->bool1() );
        else
            addBond( evt->Start(), evt->End(), evt->Param1(), evt->Param2(), evt->color(), evt->bool1() );
        break;
    case EVT_ADD_BOND_UP:
        addBond( evt->Start(), evt->End(), 1, 5, evt->color() );
        break;
    case EVT_ADD_BOND_DOWN:
        addBond( evt->Start(), evt->End(), 1, 7, evt->color() );
        break;
    case EVT_ADD_BOND_WAVY:
        addBond( evt->Start(), evt->End(), 1, 6, evt->color() );
        break;
    case EVT_ADD_BOND_DASH:
        addBond( evt->Start(), evt->End(), evt->Param1(), 99, evt->color() );
        break;
    case EVT_ADD_ARROW:
        addArrow( evt->Start(), evt->End(), evt->color(), evt->Param1(), evt->Param2() );
        break;
    case EVT_ADD_BRACKET:
        addBracket( evt->Start(), evt->End(), evt->color(), evt->Param1() );
        break;
    case EVT_ADD_CURVEARROW:
        addCurveArrow( evt->Start(), evt->End(), evt->color(), evt->text() );
        break;
    case EVT_ADD_GRAPHIC:
        go = new GraphicObject( r );
        go->setObjectType( TYPE_BEZIER );       // for now!
        go->SetStyle( evt->Param1() );
        go->SetColor( evt->color() );
        go->setPointArray( evt->points() );
        addGraphicObject( go );
        break;
    default:
        qDebug() << "Unknown event type???";
        break;
    }

    // delete the event!
    delete evt;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
