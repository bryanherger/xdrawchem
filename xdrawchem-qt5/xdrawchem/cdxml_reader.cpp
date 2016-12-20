// CDXML_Reader.cpp - function definitions for class CDXML_Reader

#include "cdxml_reader.h"
#include "chemdata.h"
#include "dpoint.h"
#include "drawable.h"
#include "text.h"
#include "defs.h"

extern Preferences preferences;

CDXML_Reader::CDXML_Reader( ChemData * c1 )
{
    nodedepth = 0;
    fragdepth = 0;
    c = c1;
    tmp_color = new ColorTableEntry;
    tmp_color->index = 0;
    tmp_color->color = QColor( 0, 0, 0 );
    colors.append( tmp_color );
    tmp_color = new ColorTableEntry;
    tmp_color->index = 1;
    tmp_color->color = QColor( 255, 255, 255 );
    colors.append( tmp_color );
}

bool CDXML_Reader::ReadFile( QString fn )
{
    qDebug() << endl << "New and improved CDXML parser" << endl;
    int i1 = fn.indexOf( "<CDXML" );
    int i2 = fn.indexOf( "</CDXML>" ) + 8;

    ParseDocument( fn.mid( i1, i2 - i1 ) );
    Build();
    qDebug() << endl << "Done parsing!" << endl;
    return false;
}

void CDXML_Reader::ParseDocument( QString dtag )
{
    int i1, i2;

    // find and parse color table
    i1 = dtag.indexOf( "<colortable>" );
    i2 = dtag.indexOf( "</colortable>" ) + 13;
    if ( i1 >= 0 ) {
        ParseColorTable( dtag.mid( i1, i2 - i1 ) );
        dtag.remove( i1, i2 - i1 );
    }
    // if no color table, set default foreground and background
    if ( colors.count() < 3 ) {
        tmp_color = new ColorTableEntry;
        tmp_color->index = 2;   // background
        tmp_color->color = QColor( 255, 255, 255 );
        colors.append( tmp_color );
        tmp_color = new ColorTableEntry;
        tmp_color->index = 3;   // foreground
        tmp_color->color = QColor( 0, 0, 0 );
        colors.append( tmp_color );
    }
    // find and parse font table
    i1 = dtag.indexOf( "<fonttable>" );
    i2 = dtag.indexOf( "</fonttable>" ) + 12;
    if ( i1 >= 0 ) {
        ParseFontTable( dtag.mid( i1, i2 - i1 ) );
        dtag.remove( i1, i2 - i1 );
    }
    // find and parse page(s)
    do {
        i1 = dtag.indexOf( "<page" );
        i2 = dtag.indexOf( "</page>" ) + 7;
        if ( i1 < 0 )
            break;
        ParsePage( dtag.mid( i1, i2 - i1 ) );
        dtag.remove( i1, i2 - i1 );
    } while ( 1 );

}

void CDXML_Reader::ParseColorTable( QString ctable )
{
    //qDebug() << ctable ;
    QString nexttag;
    int ptr = 0, idx = 2;

    do {
        nexttag = readTag( ctable, ptr );
        if ( nexttag.left( 11 ) == "<colortable" )
            continue;
        if ( nexttag == "</colortable>" )
            break;
        if ( selfContainedTag( nexttag ) == false ) {
            //qDebug() << "Problem color tag: " << nexttag ;
            continue;
        }
        tmp_color = new ColorTableEntry;
        tmp_color->index = idx;
        idx++;
        tmp_color->color = ParseColor( nexttag );
        colors.append( tmp_color );
    } while ( 1 );              // yeah, I know, looks bad  :)
}

QColor CDXML_Reader::ParseColor( QString ctag )
{
    QStringList attr;
    QString a1, v1;
    QColor retval;
    int re, gr, bl;

    attr = readAttr( ctag );
    for ( int cc = 0; cc < 5; cc = cc + 2 ) {
        a1 = attr[cc];
        v1 = attr[cc + 1];
        if ( a1 == "r" )
            re = ( int ) ( v1.toDouble() * 255.0 );
        if ( a1 == "g" )
            gr = ( int ) ( v1.toDouble() * 255.0 );
        if ( a1 == "b" )
            bl = ( int ) ( v1.toDouble() * 255.0 );
    }
    retval.setRgb( re, gr, bl );
    return retval;
}

void CDXML_Reader::ParseFontTable( QString ftable )
{
    //qDebug() << ftable ;
    QString nexttag;
    int ptr = 0;

    do {
        nexttag = readTag( ftable, ptr );
        if ( nexttag.left( 10 ) == "<fonttable" )
            continue;
        if ( nexttag == "</fonttable>" )
            break;
        if ( selfContainedTag( nexttag ) == false ) {
            //qDebug() << "Problem font tag: " << nexttag ;
            continue;
        }
        fonts.append( ParseFont( nexttag ) );
    } while ( 1 );              // yeah, I know, looks bad  :)
}

FontTableEntry *CDXML_Reader::ParseFont( QString ftag )
{
    QStringList attr;
    QString a1, v1;

    attr = readAttr( ftag );
    tmp_font = new FontTableEntry;
    for ( int cc = 0; cc < attr.count(); cc = cc + 2 ) {
        a1 = attr[cc];
        v1 = attr[cc + 1];
        if ( a1 == "name" )
            tmp_font->fam = v1;
        if ( a1 == "id" )
            tmp_font->id = v1;
    }
    return tmp_font;
}

void CDXML_Reader::ParsePage( QString ptag )
{
    // parse out known subobjects
    int i1, i2;
    bool flag;

    do {
        flag = false;
        // find fragments
        i1 = ptag.indexOf( "<fragment" );
        if ( i1 >= 0 ) {
            i2 = i1 + positionOfEndTag( ptag.mid( i1 ), "fragment" );
            ParseFragment( ptag.mid( i1, i2 - i1 ) );
            ptag.remove( i1, i2 - i1 );
            flag = true;
        }
        // find text
        //i1 = ptag.indexOf("<t");
        //if (i1 >= 0) {
        //  i2 = ptag.indexOf("</t>") + 4;
        //  ParseText(ptag.mid(i1, i2 - i1));
        //  ptag.remove(i1, i2 - i1);
        //  flag = true;
        //}
    } while ( flag );           // repeat as necessary
    // find text and graphics not contained in fragments
    do {
        flag = false;
        i1 = ptag.indexOf( "<t" );
        if ( i1 >= 0 ) {
            i2 = i1 + positionOfEndTag( ptag.mid( i1 ), "t" );
            qDebug() << "toplevel";
            QString rs = ParseText( ptag.mid( i1, i2 - i1 ) );

            tmp_node = new DPoint;
            tmp_node->x = globalx;
            tmp_node->y = globaly;
            tmp_node->element = rs;
            nodelist.append( tmp_node );
            ptag.remove( i1, i2 - i1 );
            flag = true;
        }
        i1 = ptag.indexOf( "<graphic" );
        if ( i1 >= 0 ) {
            i2 = ptag.indexOf( ">", i1 ) + 1;
            if ( !selfContainedTag( ptag.mid( i1, i2 - i1 ) ) )
                qDebug() << "Malformed <graphic>";
            qDebug() << "toplevel";
            ParseGraphic( ptag.mid( i1, i2 - i1 ) );
            ptag.remove( i1, i2 - i1 );
            flag = true;
        }
    } while ( flag );
}

// warning, fragments can contain nodes, which in turn can contain fragments.
// make sure you get the level right
void CDXML_Reader::ParseFragment( QString ftag )
{
    fragdepth++;
    qDebug() << endl << "<--frag-->";
    qDebug() << ftag << endl << endl;

    int i1, i2;
    bool flag;

    do {
        flag = false;
        // find nodes
        i1 = ftag.indexOf( "<n" );
        i2 = ftag.indexOf( ">", i1 ) + 1;
        if ( i1 >= 0 ) {
            if ( !selfContainedTag( ftag.mid( i1, i2 - i1 ) ) )
                i2 = i1 + positionOfEndTag( ftag.mid( i1 ), "n" );
            ParseNode( ftag.mid( i1, i2 - i1 ) );
            ftag.remove( i1, i2 - i1 );
            flag = true;
        }
    } while ( flag );
    do {
        flag = false;
        // find bonds (typically self-contained)
        i1 = ftag.indexOf( "<b" );
        i2 = ftag.indexOf( ">", i1 ) + 1;
        if ( i1 >= 0 ) {
            if ( !selfContainedTag( ftag.mid( i1, i2 - i1 ) ) )
                qDebug() << "Malformed <b>";
            ParseBond( ftag.mid( i1, i2 - i1 ) );
            ftag.remove( i1, i2 - i1 );
            flag = true;
        }
        // find graphics
        i1 = ftag.indexOf( "<graphic" );
        if ( i1 >= 0 ) {
            i2 = ftag.indexOf( ">", i1 ) + 1;
            if ( !selfContainedTag( ftag.mid( i1, i2 - i1 ) ) )
                qDebug() << "Malformed <graphic>";
            ParseGraphic( ftag.mid( i1, i2 - i1 ) );
            ftag.remove( i1, i2 - i1 );
            flag = true;
        }
    } while ( flag );
    fragdepth--;
}

// note this reads <t> and the contained <s>
QString CDXML_Reader::ParseText( QString ttag )
{
    qDebug() << endl << "<--text-->";
    qDebug() << ttag << endl;

    int i1, i2, i3;
    QString a1, v1;

    // tokenize the <t> tag
    QStringList t_tokens;

    i1 = ttag.indexOf( ">" );
    t_tokens = readAttr( ttag.left( i1 + 1 ) );
    qDebug() << "<--t_tokens-->";
    for ( int c1 = 0; c1 < t_tokens.count(); c1 += 2 ) {
        a1 = t_tokens.at( c1 );
        v1 = t_tokens.at( c1 + 1 );
        qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
        // compare attribute a1 with list of relevant attr's
        if ( a1.toUpper() == QString( "P" ) ) {
            i1 = v1.indexOf( QString( " " ) );
            globalx = v1.mid( 0, i1 ).toDouble();
            globaly = v1.mid( i1 + 1 ).toDouble();
        }
    }
    // tokenize the <s> tag
    QString actualtext;

    //QStringList s_tokens;
    //i1 = ttag.indexOf("<s ");
    //i2 = ttag.indexOf(">", i1) + 1;
    //s_tokens = readAttr(ttag.mid(i1, i2 - i1));
    //qDebug() << "<--s_tokens-->" ;
    //for (int c1 = 0; c1 < s_tokens.count(); c1 += 2) {
    //  a1 = *(s_tokens.at(c1));
    //  v1 = *(s_tokens.at(c1 + 1));
    //  qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-" ;
    // compare attribute a1 with list of relevant attr's
    //}
    // read actual text
    //i1 = i2;
    //i2 = ttag.indexOf("<", i1);
    //QString actualtext = ttag.mid(i1, i2 - i1);
    while ( ( i1 = ttag.indexOf( "<s " ) ) > 0 ) {
        i2 = ttag.indexOf( ">", i1 );
        i3 = ttag.indexOf( "</s>", i2 );
        actualtext.append( ttag.mid( i2 + 1, i3 - i2 - 1 ) );
        ttag.remove( i1, i3 - i1 + 4 );
        qDebug() << ttag;
    };
    qDebug() << "string = " << actualtext;
    return actualtext;
}

void CDXML_Reader::ParseGraphic( QString gtag )
{
    tmp_obj = new CDXML_Object;
    tmp_obj->type = TYPE_DRAWABLE;

    // tokenize the <graphic> tag
    QStringList tokens;
    QString a1, v1;
    int i1;
    DPoint *s1, *e1;

    i1 = gtag.indexOf( ">" );
    tokens = readAttr( gtag.left( i1 + 1 ) );
    qDebug() << "<--graphictokens-->";
    for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
        a1 = tokens.at( c1 );
        v1 = tokens.at( c1 + 1 );
        qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
        // compare attribute a1 with list of relevant attr's
        if ( a1.toUpper() == "BOUNDINGBOX" ) {
            s1 = new DPoint;
            e1 = new DPoint;
            QTextStream coords( &v1, QIODevice::ReadOnly );

            coords >> s1->x >> s1->y >> e1->x >> e1->y;
            //qDebug() << s1->x << " ";
            //qDebug() << s1->y << " ";
            //qDebug() << e1->x << " ";
            //qDebug() << e1->y ;
            tmp_obj->start = s1;
            tmp_obj->end = e1;
        }
        // don't let GraphicType override special types...
        if ( ( a1.toUpper() == "GRAPHICTYPE" ) && ( tmp_obj->type == TYPE_DRAWABLE ) ) {
            if ( v1.toUpper() == "BRACKET" )
                tmp_obj->type = TYPE_BRACKET;
        }
        if ( a1.toUpper() == "ARROWTYPE" ) {
            tmp_obj->type = TYPE_ARROW;
            tmp_obj->idata2 = ARROW_REGULAR;
            if ( v1.toUpper() == "RESONANCE" )
                tmp_obj->idata2 = ARROW_BI1;
            if ( v1.toUpper() == "EQUILIBRIUM" )
                tmp_obj->idata2 = ARROW_BI2;
            if ( v1.toUpper() == "RETROSYNTHETIC" )
                tmp_obj->idata2 = ARROW_RETRO;
        }
        if ( a1.toUpper() == "BRACKETTYPE" ) {
            tmp_obj->idata2 = BRACKET_SQUARE;
            if ( v1.toUpper() == "SQUAREPAIR" )
                tmp_obj->type = TYPE_BRACKET;
            if ( v1.toUpper() == "ROUNDPAIR" ) {
                tmp_obj->type = TYPE_BRACKET;
                tmp_obj->idata2 = BRACKET_CURVE;
            }
            if ( v1.toUpper() == "CURLYPAIR" ) {
                tmp_obj->type = TYPE_BRACKET;
                tmp_obj->idata2 = BRACKET_BRACE;
            }
        }
    }

    objectlist.append( tmp_obj );
}

// warning, nodes can contain fragments, which in turn can contain nodes.
// make sure you get the level right
void CDXML_Reader::ParseNode( QString ntag )
{
    tmp_node = new DPoint;

    nodedepth++;
    qDebug() << endl << "<--node-->";
    qDebug() << ntag << endl << endl;

    int i1, i2;
    bool flag;

    // tokenize the <n> tag
    QStringList tokens;
    QString a1, v1, nodetype;

    i1 = ntag.indexOf( ">" );
    tokens = readAttr( ntag.left( i1 + 1 ) );
    qDebug() << "<--nodetokens-->";
    for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
        a1 = tokens.at( c1 );
        v1 = tokens.at( c1 + 1 );
        qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
        // compare attribute a1 with list of relevant attr's
        if ( a1.toUpper() == QString( "ID" ) )
            tmp_node->id = v1;
        if ( a1.toUpper() == QString( "NODETYPE" ) )
            nodetype = v1;
        if ( a1.toUpper() == QString( "P" ) ) {
            i1 = v1.indexOf( QString( " " ) );
            tmp_node->x = v1.mid( 0, i1 ).toDouble();
            tmp_node->y = v1.mid( i1 + 1 ).toDouble();
        }
    }

    do {
        flag = false;
        // find nodes
        i1 = ntag.indexOf( "<fragment" );
        if ( i1 >= 0 ) {
            i2 = i1 + positionOfEndTag( ntag.mid( i1 ), "fragment" );
            // right now, we only care about top-level fragment/nodes
            //ParseFragment(ntag.mid(i1, i2 - i1));
            ntag.remove( i1, i2 - i1 );
            flag = true;
        }
        // find text (strings)
        i1 = ntag.indexOf( "<t" );
        if ( i1 >= 0 ) {
            i2 = i1 + positionOfEndTag( ntag.mid( i1 ), "t" );
            QString rs = ParseText( ntag.mid( i1, i2 - i1 ) );

            tmp_node->element = rs;
            ntag.remove( i1, i2 - i1 );
            flag = true;
        }
    } while ( flag );
    nodedepth--;

    nodelist.append( tmp_node );
}

void CDXML_Reader::ParseBond( QString btag )
{
    tmp_obj = new CDXML_Object;
    tmp_obj->type = TYPE_BOND;
    tmp_obj->idata1 = 1;

    // tokenize the <b> tag
    QStringList tokens;
    QString a1, v1;
    int i1;

    i1 = btag.indexOf( ">" );
    tokens = readAttr( btag.left( i1 + 1 ) );
    qDebug() << "<--bondtokens-->";
    for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
        a1 = tokens.at( c1 );
        v1 = tokens.at( c1 + 1 );
        qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
        // compare attribute a1 with list of relevant attr's
        if ( a1.toUpper() == QString( "ID" ) )
            tmp_obj->id = v1;
        if ( a1.toUpper() == QString( "B" ) )
            tmp_obj->start_id = v1;
        if ( a1.toUpper() == QString( "E" ) )
            tmp_obj->end_id = v1;
        if ( a1.toUpper() == QString( "ORDER" ) )
            tmp_obj->idata1 = v1.toUInt();
        if ( a1.toUpper() == QString( "DISPLAY" ) ) {
            if ( v1 == QString( "WedgeBegin" ) )
                tmp_obj->idata1 = 5;
            if ( v1 == QString( "WedgedHashBegin" ) )
                tmp_obj->idata1 = 7;
        }
    }

    objectlist.append( tmp_obj );
}

DPoint *CDXML_Reader::FindNode( QString key )
{
    foreach ( tmp_node, nodelist ) {
        if ( tmp_node->id == key )
            return tmp_node;
    }
    return 0;
}

// build structure from data we read
void CDXML_Reader::Build()
{
    QList < DPoint * >up;
    DPoint *s1, *e1;
    Text *nt;
    double bondlength = 0.0;
    int nbonds = 0;

    qDebug() << "nodes: " << nodelist.count();
    qDebug() << "objects: " << objectlist.count();

    // add all non-text objects
    foreach ( tmp_obj, objectlist ) {
        if ( tmp_obj->type == TYPE_BOND ) {
            s1 = FindNode( tmp_obj->start_id );
            e1 = FindNode( tmp_obj->end_id );
            bondlength += s1->distanceTo( e1 );
            nbonds++;
            c->addBond( s1, e1, 1, tmp_obj->idata1, QColor( 0, 0, 0 ), true );
            s1->hit = true;
            e1->hit = true;
            if ( up.contains( s1 ) )
                up.append( s1 );
            if ( up.contains( e1 ) )
                up.append( e1 );
        }
        if ( tmp_obj->type == TYPE_ARROW ) {
            c->addArrow( tmp_obj->end, tmp_obj->start, QColor( 0, 0, 0 ), tmp_obj->idata2, true );
            if ( up.contains( tmp_obj->start ) )
                up.append( tmp_obj->start );
            if ( up.contains( tmp_obj->end ) )
                up.append( tmp_obj->end );
        }
        if ( tmp_obj->type == TYPE_BRACKET ) {
            c->addBracket( tmp_obj->start, tmp_obj->end, QColor( 0, 0, 0 ), tmp_obj->idata2, true );
            if ( up.contains( tmp_obj->start ) )
                up.append( tmp_obj->start );
            if ( up.contains( tmp_obj->end ) )
                up.append( tmp_obj->end );
        }
    }
    // add text
    foreach ( tmp_node, nodelist ) {
        if ( tmp_node->element != "C" ) {
            nt = new Text( c->getRender2D() );
            nt->setPoint( tmp_node );
            if ( up.contains( tmp_node ) )
                up.append( tmp_node );
            if ( tmp_node->hit ) {      // true = part of molecule
                nt->setJustify( JUSTIFY_CENTER );
            } else {            // false = free-standing text
                nt->setJustify( JUSTIFY_TOPLEFT );
            }
            nt->setText( tmp_node->element );
            tmp_node->element.fill( ' ' );
//            nt->setTextMask( tmp_node->element );
            nt->Highlight( true );
            c->addText( nt );
        }
    }
    // clear "hit" flag
    foreach ( tmp_node, nodelist ) {
        tmp_node->hit = false;
    }
    // scale drawing
    double avglen = bondlength / ( double ) nbonds;
    double curfixed = preferences.getBond_fixedlength();
    double sf = curfixed / avglen;

    qDebug() << "Scale:" << sf;
    //sf = 1.0;
    double sl = 9999.0, sr = -9999.0, st = 9999.0, sb = -9999.0;

    foreach ( tmp_node, up ) {
        tmp_node->x *= sf;
        tmp_node->y *= sf;
        if ( tmp_node->x < sl )
            sl = tmp_node->x;
        if ( tmp_node->x > sr )
            sr = tmp_node->x;
        if ( tmp_node->y < st )
            st = tmp_node->y;
        if ( tmp_node->y > sb )
            sb = tmp_node->y;
    }
    double tx = 50 - sl;
    double ty = 50 - st;

    foreach ( tmp_node, up ) {
        tmp_node->x += tx;
        tmp_node->y += ty;
    }
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
