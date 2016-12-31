#include <iostream>

#include <QMessageBox>
#include <QClipboard>
#include <QApplication>

#include "netaccess.h"
#include "crings_dialog.h"
#include "tooldialog.h"
#include "tool_1h_nmr.h"
#include "tool_13c_nmr.h"
#include "tool_ir.h"
#include "tool_2d3d.h"
#include "drawable.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "bracket.h"
#include "text.h"
#include "gobject.h"
#include "chemdata.h"
#include "defs.h"

// For AutoLayout
class LayoutGroup
{
public:
    Drawable * tmp_draw;
    QList < Drawable * >items;
    LayoutGroup *left;
    LayoutGroup *right;
    LayoutGroup *above;
    LayoutGroup *below;
    bool placed;
    void Move( double x, double y )
    {
        qDebug() << "MOVE:" << x << " " << y;
        foreach ( tmp_draw, items )
        {
            tmp_draw->SelectAll();
            tmp_draw->Move( x, y );
            tmp_draw->DeselectAll();
        }
    }
    QRect BoundingBox()
    {
        int top = 99999, bottom = 0, left = 99999, right = 0;
        QRect tmprect;

        foreach ( tmp_draw, items ) {
            tmp_draw->SelectAll();
            tmprect = tmp_draw->BoundingBox();
            tmp_draw->DeselectAll();
            if ( tmprect.isValid() ) {
                if ( tmprect.left() < left )
                    left = tmprect.left();
                if ( tmprect.right() > right )
                    right = tmprect.right();
                if ( tmprect.top() < top )
                    top = tmprect.top();
                if ( tmprect.bottom() > bottom )
                    bottom = tmprect.bottom();
            }
        }
        return QRect( QPoint( left, top ), QPoint( right, bottom ) );
    }
    QPoint Center()
    {
        return BoundingBox().center();
    }
};

// Determine Molecule clicked, do Tool action
void ChemData::Tool( DPoint *target, int mode )
{
    QClipboard *cb = QApplication::clipboard();

    qDebug() << "ChemData::Tool: " << mode;
    Molecule *m = 0;
    NetAccess *na = new NetAccess();
    CustomRingDialog cr1;
    ToolDialog tool1;
    Tool_1HNMR_Dialog *tool1hnmr;
    Tool_13CNMR_Dialog *tool13cnmr;
    Tool_IR_Dialog *toolir;
    Tool_2D3D tool2d3d;
    QString tmpname, serverName;
    QStringList choices;
    Bond *tmp_bond;
    int dret;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            m = ( Molecule * ) tmp_draw;
	    if ( m->BoundingBoxAll().contains( target->toQPoint(), false ) )
	      break;
            m = 0;
        }
    }
    if ( m == 0 )
        return;
    double kow = 0.0;

    switch ( mode ) {
    case MODE_TOOL_MOLECULE_INFO:
        mi = new MolInfoDialog( r );
        tt_mw = m->CalcMW();
        tt_ef = m->CalcEmpiricalFormula();
        tt_ea = m->CalcElementalAnalysis();
        mi->setMW( tt_mw->getText() );
        mi->setEF( tt_ef->getText() );
        mi->setEA( tt_ea->getText() );

        serverName = getenv( "XDC_SERVER" );
        if ( serverName.length() < 2 )
            serverName = XDC_SERVER;
        if ( na->getNameCAS( serverName, m->ToInChI() ) ) {
            mi->setCAS( na->scas );
            mi->setName( na->siupacname );
            mi->setSynonyms( na->sname );
            mi->setPCC( na->spccompound );
        }

        connect( mi, SIGNAL( MIDClose() ), this, SLOT( returnFromMID() ) );

        mi->show();
        //if ( !mi->exec() ) return;
        //if (mi->isMWChecked()) drawlist.append(tt_mw);
        //if (mi->isEFChecked()) drawlist.append(tt_ef);
        //delete mi;
        break;
    case MODE_TOOL_CALCMW:
        tt = m->CalcMW();
        if ( tt != 0 )
            drawlist.append( tt );
        break;
    case MODE_TOOL_CALCEF:
        tt = m->CalcEmpiricalFormula();
        if ( tt != 0 )
            drawlist.append( tt );
        break;
    case MODE_TOOL_CALCEA:
        tt = m->CalcElementalAnalysis();
        if ( tt != 0 )
            drawlist.append( tt );
        break;
    case MODE_TOOL_13CNMR:
        tool13cnmr = new Tool_13CNMR_Dialog;
        tool13cnmr->setMolecule( m );
        tool13cnmr->show();
        //m->Calc13CNMR();
        break;
    case MODE_TOOL_1HNMR:
        tool1hnmr = new Tool_1HNMR_Dialog;
        tool1hnmr->setMolecule( m );
        tool1hnmr->show();
        //m->Calc1HNMR();
        break;
    case MODE_TOOL_IR:
        toolir = new Tool_IR_Dialog;
        toolir->setMolecule( m );
        toolir->show();
        //m->CalcIR();
        break;
    case MODE_TOOL_PKA:
        m->CalcpKa();
        break;
    case MODE_TOOL_RETRO:
        Retro( m );
        break;
    case MODE_TOOL_REACTIVITY_FORWARD:
        m->Reactivity( mode );
        break;
    case MODE_TOOL_REACTIVITY_RETRO:
        m->Reactivity( mode );
        for ( tmp_bond = m->bondsFirst(); tmp_bond != 0; tmp_bond = m->bondsNext() ) {
            if ( tmp_bond->getReactions().length() > 2 ) {
                tmp_bond->SetColor( QColor( 124, 252, 0 ) );
            }
        }
        r->update();
        break;
    case MODE_TOOL_CHARGES:
        m->Reactivity( mode );
        break;
    case MODE_TOOL_KOW:
        kow = m->CalcKOW();
        QMessageBox::information( r, tr( "Octanol-water partition" ), tr( "Estimated octanol-water partition constant (log Kow) = %1" ).arg( kow ) );
        break;
    case MODE_TOOL_2D3D:
        m->Make3DVersion();
        break;
    case MODE_TOOL_NAME:
        m->CalcName();
        break;
    case MODE_TOOL_CUSTOMRING:
        cr1.setMolecule( m );
        if ( cr1.exec() == QDialog::Accepted )
            emit SignalUpdateCustomRingMenu();

        break;
    case MODE_TOOL_TOSMILES:
        tmpname = m->ToSMILES();
        if ( tmpname.length() == 0 ) {
            qDebug() << "Could not get SMILES string!";
        }
        QMessageBox::information( r, tr( "SMILES string" ), tr( "SMILES string for selected molecule:" ) + "\n\n" + tmpname );
        break;
    case MODE_TOOL_TOINCHI:
        //m->AddNMRprotons();
        tmpname = m->ToInChI();
        //m->RemoveNMRprotons();
        if ( tmpname.length() == 0 ) {
            qDebug() << "Could not get InChI string!";
        }
        QMessageBox::information( r, tr( "InChI string" ), tr( "InChI string for selected molecule:" ) + "\n\n" + tmpname );
        break;
    case MODE_TOOL_CLEANUPMOL:
        m->CleanUp();
        break;
    case MODE_TOOL_GROUP_REACTANT:
        m->setGroupType( GROUP_REACTANT );
        break;
    case MODE_TOOL_GROUP_PRODUCT:
        m->setGroupType( GROUP_PRODUCT );
        break;
    case MODE_TOOL_GROUP_CLEAR:
        m->setGroupType( GROUP_NONE );
        break;
    case MODE_TOOL_TEST:
        tool1.setMolecule( m );
        tool1.exec();
        break;
    }
    // Need to pick next tool manually
}

void ChemData::Save3D( QString fn3d )
{
    // save 3D image of first molecule
    Molecule *m = 0;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            m = ( Molecule * ) tmp_draw;
            break;
        }
    }

    if ( m != 0 ) {
        m->Make3DVersion( fn3d );
    } else {
        qDebug() << "No molecule in input file!";
    }
}

void ChemData::returnFromMID()
{
    Q_CHECK_PTR( mi );
    Q_CHECK_PTR( tt_mw );
    Q_CHECK_PTR( tt_ef );
    Q_CHECK_PTR( tt_ea );
    if ( mi->isMWChecked() )
        drawlist.append( tt_mw );
    if ( mi->isEFChecked() )
        drawlist.append( tt_ef );
    if ( mi->isEAChecked() )
        drawlist.append( tt_ea );
    mi->hide();
    delete mi;
}

void ChemData::clearAllGroups()
{
    Molecule *m = 0;
    QString tmpname;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            m = ( Molecule * ) tmp_draw;
            m->setGroupType( GROUP_NONE );
        }
    }
}

// calculate molecular weights of Molecules
// AutoLayout
void ChemData::AutoLayout()
{
    QList < LayoutGroup * >layout;
    LayoutGroup *tmp_lo, *tl, *tr, *ta, *tb, *tl1;
    int dista, distb, distl, distr, d1, d2, d3, d4, ds;
    Text *tmp_text;
    Arrow *tmp_arrow;
    Drawable *td2;

    // first, put Arrows and Molecules into LayoutGroups
    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_ARROW ) {
            tmp_lo = new LayoutGroup;
            tmp_lo->items.append( tmp_draw );
            tmp_lo->placed = false;
            tmp_lo->left = 0;
            tmp_lo->right = 0;
            tmp_lo->above = 0;
            tmp_lo->below = 0;
            layout.append( tmp_lo );
        }
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_lo = new LayoutGroup;
            tmp_lo->items.append( tmp_draw );
            tmp_lo->placed = false;
            tmp_lo->left = 0;
            tmp_lo->right = 0;
            tmp_lo->above = 0;
            tmp_lo->below = 0;
            layout.append( tmp_lo );
        }
    }
    // now, attach Text to Arrows as needed
    foreach ( tmp_lo, layout ) {
        td2 = tmp_lo->items.first();
        if ( td2->Type() == TYPE_ARROW ) {
            tmp_arrow = ( Arrow * ) td2;
            foreach ( tmp_draw, drawlist ) {
                if ( tmp_draw->Type() == TYPE_TEXT ) {
                    tmp_text = ( Text * ) tmp_draw;
                    int ns;
                    QPoint amid = tmp_arrow->Midpoint();
                    QPoint tcenter = tmp_text->NearestCenter( amid,
                                                              tmp_arrow->Orientation(),
                                                              ns );
                    int dx = tcenter.x() - amid.x();
                    int dy = tcenter.y() - amid.y();
                    double dist = sqrt( ( double ) ( dx * dx + dy * dy ) );

                    qDebug() << dist;
                    if ( dist < 25 ) {
                        if ( tmp_arrow->Orientation() == ARROW_HORIZONTAL ) {
                            if ( dy < 0 ) {     // above arrow
                                dy = dy + 12;
                                tmp_text->ForceMove( -dx, -dy );
                            } else {    // below arrow
                                dy = dy - 12;
                                tmp_text->ForceMove( -dx, -dy );
                            }
                        } else {        // ARROW_VERTICAL
                            if ( dx < 0 ) {     // above arrow
                                dx = dx + 12;
                                tmp_text->ForceMove( -dx, -dy );
                            } else {    // below arrow
                                dx = dx - 12;
                                tmp_text->ForceMove( -dx, -dy );
                            }
                        }
                        tmp_lo->items.append( tmp_text );       // add Text to LayoutGroup
                    }           // if (dist...)
                }               // if (...TYPE_TEXT)
            }                   // for (...)
        }                       // if (...TYPE_ARROW)
    }                           // for(...)
    // Now determine position of LayoutGroups
    foreach ( tmp_lo, layout ) {
        QRect box = tmp_lo->BoundingBox();
        QPoint l1( box.left(), box.center().y() );
        QPoint r1( box.right(), box.center().y() );
        QPoint a1( box.top(), box.center().x() );
        QPoint b1( box.bottom(), box.center().x() );

        // check sides
        dista = 9999;
        distb = 9999;
        distl = 9999;
        distr = 9999;
        ds = 9999;
        tl = 0;
        tr = 0;
        ta = 0;
        tb = 0;
        foreach ( tl1, layout ) {
            QRect box1 = tl1->BoundingBox();
            QPoint l2( box1.left(), box1.center().y() );
            QPoint r2( box1.right(), box1.center().y() );
            QPoint a2( box1.top(), box1.center().x() );
            QPoint b2( box1.bottom(), box1.center().x() );

            d1 = ( int ) ( r->DistanceBetween( l1, r2 ) );
            d2 = ( int ) ( r->DistanceBetween( r1, l2 ) );
            d3 = ( int ) ( r->DistanceBetween( a1, b2 ) );
            d4 = ( int ) ( r->DistanceBetween( b1, a2 ) );
            if ( d1 < ds )
                ds = d1;
            if ( d2 < ds )
                ds = d2;
            if ( d3 < ds )
                ds = d3;
            if ( d4 < ds )
                ds = d4;
            if ( d1 == ds )
                tl = tl1;
            if ( d2 == ds )
                tr = tl1;
            if ( d3 == ds )
                ta = tl1;
            if ( d4 == ds )
                tb = tl1;
        }
        if ( tl != 0 ) {
            qDebug() << "left";
            tl->right = tmp_lo;
            tmp_lo->left = tl;
        }
        if ( tr != 0 ) {
            qDebug() << "right";
            tr->left = tmp_lo;
            tmp_lo->right = tr;
        }
        if ( ta != 0 ) {
            qDebug() << "above";
            ta->below = tmp_lo;
            tmp_lo->above = ta;
        }
        if ( tb != 0 ) {
            qDebug() << "below";
            tb->above = tmp_lo;
            tmp_lo->below = tb;
        }
        qDebug();
    }
    // Place everything
    // Start with things near arrows
    foreach ( tmp_lo, layout ) {
        if ( tmp_lo->items.first()->Type() == TYPE_ARROW ) {
            tmp_lo->placed = true;
            tmp_arrow = ( Arrow * ) ( tmp_lo->items.first() );
            if ( tmp_arrow->Orientation() == ARROW_HORIZONTAL ) {
                // adjust position according to already placed Molecule or Arrow
                if ( tmp_lo->left != 0 ) {
                    if ( tmp_lo->left->placed == true ) {
                        int dy = tmp_lo->Center().y() - tmp_lo->left->Center().y();

                        tmp_lo->Move( 0, -dy );
                    }
                }
                if ( tmp_lo->right != 0 ) {
                    if ( tmp_lo->right->placed == true ) {
                        int dy = tmp_lo->Center().y() - tmp_lo->right->Center().y();

                        tmp_lo->Move( 0, -dy );
                    }
                }
                if ( tmp_lo->left != 0 ) {
                    if ( tmp_lo->left->placed == false ) {
                        int dy = tmp_lo->left->Center().y() - tmp_lo->Center().y();

                        tmp_lo->left->Move( 0, -dy );
                        tmp_lo->left->placed = true;
                    }
                }
                if ( tmp_lo->right != 0 ) {
                    if ( tmp_lo->right->placed == false ) {
                        int dy = tmp_lo->right->Center().y() - tmp_lo->Center().y();

                        tmp_lo->right->Move( 0, -dy );
                        tmp_lo->right->placed = true;
                    }
                }
            } else {            // ARROW_VERTICAL
                // adjust position according to already placed Molecule or Arrow
                if ( tmp_lo->above != 0 ) {
                    if ( tmp_lo->above->placed == true ) {
                        int dx = tmp_lo->Center().y() - tmp_lo->above->Center().y();

                        tmp_lo->Move( -dx, 0 );
                    }
                }
                if ( tmp_lo->below != 0 ) {
                    if ( tmp_lo->below->placed == true ) {
                        int dx = tmp_lo->Center().x() - tmp_lo->below->Center().x();

                        tmp_lo->Move( -dx, 0 );
                    }
                }
                if ( tmp_lo->above != 0 ) {
                    if ( tmp_lo->above->placed == false ) {
                        int dx = tmp_lo->above->Center().x() - tmp_lo->Center().x();

                        tmp_lo->above->Move( -dx, 0 );
                        tmp_lo->above->placed = true;
                    }
                }
                if ( tmp_lo->below != 0 ) {
                    if ( tmp_lo->below->placed == false ) {
                        int dx = tmp_lo->below->Center().x() - tmp_lo->Center().x();

                        tmp_lo->below->Move( -dx, 0 );
                        tmp_lo->below->placed = true;
                    }
                }
            }                   // if (tmp_arrow...)
        }                       // if (...TYPE_ARROW)
    }                           // for (...)
}

void ChemData::fromSMILES( QString sm )
{
    Molecule *m1 = new Molecule( r );

    m1->FromSMILES( sm );
    m1->SelectAll();
    drawlist.append( m1 );
}

void ChemData::SmartPlace( QString sf, DPoint * t1 )
{
    if ( sf.contains( "cyclopentadiene" ) > 0 )
        sf.replace( QRegExp( "diene" ), "diene-sp" );
    double ang1 = -CalculateRingAttachAngle( t1 ) + 1.5708;

    load( sf );
    tmp_draw = drawlist.last();
    Molecule *m1 = ( Molecule * ) tmp_draw;

    if ( fabs( ang1 ) < 0.1 ) {
        ang1 = 3.14159;
    } else {
        if ( fabs( ang1 ) > 3.13 ) {
            ang1 = 0.0;
        }
    }
    qDebug() << "angle = " << ( ang1 * 180.0 / 3.14159 ) << " degrees!";
    tmp_pt = m1->GetRingAttachPoint();
    m1->Rotate( ang1 );
    double dx, dy;

    dx = t1->x - tmp_pt->x;
    dy = t1->y - tmp_pt->y;
    m1->Move( dx, dy );
    QList < DPoint * >nb = m1->BreakRingBonds( tmp_pt );
    foreach ( tmp_pt, nb ) {
        addBond( tmp_pt, t1, 1, tmp_pt->new_order, QColor( 0, 0, 0 ), true );
        qDebug() << "added a bond";
    }
}

void ChemData::SmartPlaceToo( QString sf, DPoint * t1 )
{
    double ang1 = -CalculateRingAttachAngle( t1 ) + 3.14159;

    load( sf );
    tmp_draw = drawlist.last();
    Molecule *m1 = ( Molecule * ) tmp_draw;

    qDebug() << "angle = " << ( ang1 * 180.0 / 3.14159 ) << " degrees!";
    //tmp_pt = m1->GetRingAttachPoint();
    tmp_pt = m1->GetAttachPoint( sf );
    m1->Rotate( ang1 );
    double dx, dy;

    dx = t1->x - tmp_pt->x;
    dy = t1->y - tmp_pt->y;
    m1->Move( dx, dy );
    QList < DPoint * >nb = m1->BreakRingBonds( tmp_pt );
    foreach ( tmp_pt, nb ) {
        addBond( tmp_pt, t1, 1, tmp_pt->new_order, QColor( 0, 0, 0 ), true );
        qDebug() << "added a bond";
    }
    // need to put back N if EDANS
    if ( sf.contains( "edans" ) > 0 ) {
        t1->element = "NH";
        Text *nt = new Text( r );

        nt->setPoint( t1 );
        nt->setJustify( JUSTIFY_CENTER );
        nt->setText( "NH" );
//        nt->setTextMask( "  " );
        addText( nt );
    }
}

void ChemData::SmartPlaceThree( QString sf, DPoint * t1 )
{
    double ang1 = -CalculateRingAttachAngle( t1 );

    load( sf );
    tmp_draw = drawlist.last();
    Molecule *m1 = ( Molecule * ) tmp_draw;

    qDebug() << "angle = " << ( ang1 * 180.0 / 3.14159 ) << " degrees!";
    //tmp_pt = m1->GetRingAttachPoint();
    tmp_pt = m1->GetAttachPoint( sf );
    m1->Rotate( ang1 );
    double dx, dy;

    dx = t1->x - tmp_pt->x;
    dy = t1->y - tmp_pt->y;
    m1->Move( dx, dy );
    QList < DPoint * >nb = m1->BreakRingBonds( tmp_pt );
    foreach ( tmp_pt, nb ) {
        addBond( tmp_pt, t1, 1, tmp_pt->new_order, QColor( 0, 0, 0 ), true );
        qDebug() << "added a bond";
    }
    // need to put back N
    t1->element = "NH";
    Text *nt = new Text( r );

    nt->setPoint( t1 );
    nt->setJustify( JUSTIFY_CENTER );
    nt->setText( "NH" );
//    nt->setTextMask( "  " );
    addText( nt );
}

double ChemData::CalculateRingAttachAngle( DPoint * t1 )
{
    double a1;
    Molecule *m = 0;

    foreach ( tmp_draw, drawlist ) {
        if ( ( tmp_draw->Type() == TYPE_MOLECULE ) && ( tmp_draw->Find( t1 ) == true ) ) {
            m = ( Molecule * ) tmp_draw;
            break;
        }
    }
    if ( m == 0 )
        return 0.0;

    a1 = m->CalculateRingAttachAngle( t1 );

    return a1;
}

bool ChemData::SelectWithinLoop( QVector<QPoint> tmp_lasso )
{
    // how to implement this?
    // maybe check whether given DPoint is between two QPoints in tmp_lasso?
    // will be costly (in terms of order), but hey, it's freeware

    int side_up, side_down, side_left, side_right;
    bool retval;
    double xmin, xmax, ymin, ymax, tmp_swap;    // xcenter, ycenter;
    int lcount;
    QPoint tmp_bb;
    DPoint *t1, *t2;

    QList<Drawable *> obj_list = UniqueObjects();
    retval = false;

    foreach ( tmp_draw, obj_list ) {
        side_up = 0;
        side_down = 0;
        side_left = 0;
        side_right = 0;
        if ( tmp_draw->End() != 0 ) {
            t1 = tmp_draw->Start();
            t2 = tmp_draw->End();
            tmp_bb = r->Midpoint( t1->toQPoint(), t2->toQPoint() );
        } else {
            tmp_bb = tmp_draw->Start()->toQPoint();
        }
        for ( lcount = 0; lcount < tmp_lasso.size() - 1; lcount++ ) {
            xmin = tmp_lasso.at(lcount).x();
            xmax = tmp_lasso.at(lcount + 1).x();
            ymin = tmp_lasso.at(lcount).y();
            ymax = tmp_lasso.at(lcount + 1).y();
            if ( xmax < xmin ) {
                tmp_swap = xmin;
                xmin = xmax;
                xmax = tmp_swap;
            }
            if ( ymax < ymin ) {
                tmp_swap = ymin;
                ymin = ymax;
                ymax = tmp_swap;
            }
            // check left and right
            if ( ( tmp_bb.y() >= ymin ) && ( tmp_bb.y() <= ymax ) ) {
                //qDebug() << xmin << ":" << xmax << "-" << ymin << ":" << ymax;
                //qDebug() << "xcheck: " << tmp_bb.x() << "," << tmp_bb.y();
                if ( tmp_bb.x() > xmin )
                    side_left = 1;
                if ( tmp_bb.x() < xmax )
                    side_right = 1;
            }
            // check top and bottom
            if ( ( tmp_bb.x() >= xmin ) && ( tmp_bb.x() <= xmax ) ) {
                //qDebug() << xmin << ":" << xmax << "-" << ymin << ":" << ymax;
                //qDebug() << "ycheck: " << tmp_bb.x() << "," << tmp_bb.y();
                if ( tmp_bb.y() > ymin )
                    side_up = 1;
                if ( tmp_bb.y() < ymax )
                    side_down = 1;
            }
        }
        if ( ( side_up + side_down + side_left + side_right ) > 3 ) {
            tmp_draw->Highlight( true );
            retval = true;
        }
    }

    return retval;
}


// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
