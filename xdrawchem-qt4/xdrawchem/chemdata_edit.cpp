#include "drawable.h"
#include "clipboard.h"
#include "molecule.h"
#include "bond.h"
#include "arrow.h"
#include "bracket.h"
#include "curvearrow.h"
#include "symbol.h"
#include "text.h"
#include "chemdata.h"
#include "defs.h"

void ChemData::ScaleAll( double bond_length )
{
    Molecule *tmp_mol;

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() == TYPE_MOLECULE ) {
            tmp_mol = ( Molecule * ) tmp_draw;
            tmp_mol->Scale( bond_length );
        }
    }
}

void ChemData::Cut()
{
    Copy();
    EraseSelected();
}

void ChemData::Copy()
{
    QList < Drawable * >unique;
    QList < DPoint * >oldPoints, newPoints;
    Drawable *td2, *tdx;
    DPoint *n;

    clip->clear();

    foreach ( tmp_draw, drawlist ) {
        if ( tmp_draw->Type() != TYPE_MOLECULE ) {
            // Copy to clipboard if selected
            if ( tmp_draw->Highlighted() == true )
                clip->objects.append( tmp_draw );
        } else {
            // Get list of objects, then copy to clipboard if selected
            unique = tmp_draw->AllObjects();
            oldPoints = tmp_draw->AllPoints();
            // make new DPoint's which correspond to old DPoint's
            newPoints.clear();
            foreach ( tmp_pt, oldPoints ) {
                n = new DPoint( tmp_pt );
                newPoints.append( n );
            }
            qDebug() << "unique:" << unique.count();
            qDebug() << "newPoints:" << newPoints.count();
            foreach ( td2, unique ) {
                if ( td2->Highlighted() == true ) {
                    tdx = td2;
                    tdx->setStart( newPoints.at( oldPoints.indexOf( td2->Start() ) ) );
                    if ( td2->End() != 0 )
                        tdx->setEnd( newPoints.at( oldPoints.indexOf( td2->End() ) ) );
                    clip->objects.append( tdx );
                }
            }
        }
    }
}

bool ChemData::Paste()
{
    DeselectAll();
    QList < DPoint * >oldPoints;
    QList < DPoint * >newPoints;
    DPoint *n;
    Bond *b;
    Text *ot, *t;

    // need to deep copy stuff coming off the Clipboard
    // first, find all unique DPoint's
    foreach ( tmp_draw, clip->objects ) {
        if ( oldPoints.contains( tmp_draw->Start() ) == 0 )
            oldPoints.append( tmp_draw->Start() );
        if ( tmp_draw->End() != 0 ) {
            if ( oldPoints.contains( tmp_draw->End() ) == 0 )
                oldPoints.append( tmp_draw->End() );
        }
    }
    qDebug() << oldPoints.count();
    if ( oldPoints.count() == 0 )
        return false;

    // make new DPoint's which correspond to old DPoint's
    foreach ( tmp_pt, oldPoints ) {
        n = new DPoint( tmp_pt );
        newPoints.append( n );
    }
    // now add all non-TYPE_TEXT objects back to current
    Drawable *td1;

    foreach ( td1, clip->objects ) {
        if ( td1->Type() == TYPE_ARROW ) {
            Arrow *tmp_arrow = ( Arrow * ) td1;

            addArrow( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), td1->GetColor(), tmp_arrow->Style(), true );
        }
        if ( td1->Type() == TYPE_BRACKET ) {
            Bracket *tmp_bracket = ( Bracket * ) td1;

            addBracket( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), td1->GetColor(), tmp_bracket->Style(), true );
        }
        if ( td1->Type() == TYPE_BOND ) {
            b = ( Bond * ) td1;
            addBond( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), b->Thick(), b->Order(), td1->GetColor(), true );
        }
        if ( td1->Type() == TYPE_CURVEARROW ) {
            CurveArrow *tmp_arrow = ( CurveArrow * ) td1;

            addCurveArrow( newPoints.at( oldPoints.indexOf( td1->Start() ) ), newPoints.at( oldPoints.indexOf( td1->End() ) ), td1->GetColor(), tmp_arrow->GetCurve(), true );
        }
        if ( td1->Type() == TYPE_SYMBOL ) {
            Symbol *tmp_sym = ( Symbol * ) td1;

            addSymbol( newPoints.at( oldPoints.indexOf( td1->Start() ) ), tmp_sym->GetSymbol(), true );
        }
    }
    //add all TYPE_TEXT objects (ensures bonds and molecules exist before labels)
    foreach ( td1, clip->objects ) {
        if ( td1->Type() == TYPE_TEXT ) {
            ot = ( Text * ) td1;
            t = new Text( r );
            t->setPoint( newPoints.at( oldPoints.indexOf( td1->Start() ) ) );
            t->setText( ot->getText() );
//            t->setTextMask( ot->getTextMask() );
            t->SetColor( ot->GetColor() );
            t->setFont( ot->getFont() );
            t->setJustify( ot->Justify() );
            /* need to attach correct point if part of molecule
               if (t->Justify() == JUSTIFY_CENTER) {
               DPoint *tp1 = 0;
               for (tmp_pt = newPoints.first(); tmp_pt != 0;
               tmp_pt = newPoints.next()) {
               if (tmp_pt->serial == t->Start()->serial)
               tp1 = tmp_pt;
               }
               if (tp1 != 0) {
               t->setPoint(tp1);
               }
               }
             */
            t->Highlight( true );
            addText( t );
        }
    }
    return true;
}

void ChemData::StartUndo( int fn, DPoint * s1 )
{
    QString last_undo_file;

    //qDebug() << "Saved undo file" ;
    // checkpoint!
    save_native( "" );
    //qDebug() << "File length: " << current_undo_file.length() ;
    if ( last_states.count() > 0 ) {
        last_undo_file = last_states[last_states.count() - 1];
        if ( last_undo_file == current_undo_file )
            return;
    }
    last_states.append( current_undo_file );
    if ( last_states.count() > 32 )
        last_states.removeFirst();
}

bool ChemData::Undo()
{
    if ( last_states.count() == 0 ) {
        return false;
    }
    current_undo_file = last_states[last_states.count() - 1];
    last_states.removeLast();
    load_native( current_undo_file );
    return true;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
