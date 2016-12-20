#include "chemdata.h"
#include "dpoint.h"
#include "arrow.h"
#include "bracket.h"
#include "text.h"
#include "defs.h"
#include "cml.h"

// handle CDXML format (used by ChemDraw, TM Cambridge Software)
// assume Cambridge Software CDXML.DTD
bool ChemData::LoadCDXMLFile( QString wholefile, QString doctype )
{
    qDebug() << "CDXML";
    int num_pages = 0;
    int currentpage;
    int currentfragment;
    int nodedepth = 0;
    QString line;
    QString tmpline, tfont, tdata, a1, v1, textpos, textstr, textid;
    QString graphic_id, graphictype, arrowtype, boundingbox;
    QString nodetype;
    DPoint *s1, *e1;
    QString thistag, nexttag;
    QStringList tokens, refs;
    int d1, ptr = 0;
    int *st1;
    double l1, l2, l3, l4;

    do {
        thistag = ReadTag( wholefile, ptr );
        // ReadTag returns a null at EOF (ideally).
        if ( thistag.isNull() )
            break;
        // Look for beginning of document
        if ( thistag.indexOf( QString( "<" ) + doctype ) >= 0 ) {
            qDebug() << "<CDXML>";
            // push this tag on stack
            st1 = new int;

            *st1 = CML_MOLECULE;
            TagStack.append( st1 );
            // parse attributes in tag
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "NAME" ) )
                    MoleculeId = v1;
                //      if (a1.toUpper() == QString("TITLE")) MoleculeTitle = v1;
            }
        }
        // <colortable>, <fonttable> and contained objects get ignored
        // (for now - may implement them later)
        //
        // handle <page ...>
        if ( thistag.indexOf( QString( "<page" ) ) >= 0 ) {
            num_pages++;
            st1 = new int;

            *st1 = CDXML_PAGE;
            TagStack.append( st1 );
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    currentpage = v1.toUInt();
            }
        }
        // handle </page>
        if ( thistag == QString( "</page>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CDXML_PAGE ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <page> tags!";
            }
        }
        // handle <fragment>
        if ( thistag.indexOf( QString( "<fragment" ) ) >= 0 ) {
            st1 = new int;

            *st1 = CDXML_FRAGMENT;
            TagStack.append( st1 );
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    currentfragment = v1.toUInt();
            }
        }
        // handle </fragment>
        if ( thistag == QString( "</fragment>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CDXML_FRAGMENT ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <fragment> tags!";
            }
        }
        // handle <graphic> and <graphic/>
        if ( thistag.indexOf( QString( "<graphic" ) ) >= 0 ) {
            qDebug() << thistag;
            st1 = new int;

            *st1 = CDXML_GRAPHIC;
            TagStack.append( st1 );
            tokens = ReadAttr( thistag );
            tmp_bond = new CML_Bond;
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-" ;
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    graphic_id = v1;
                if ( a1.toUpper() == QString( "BOUNDINGBOX" ) )
                    boundingbox = v1;
                if ( a1.toUpper() == QString( "GRAPHICTYPE" ) )
                    graphictype = v1;
                if ( a1.toUpper() == QString( "ARROWTYPE" ) )
                    arrowtype = v1;
                if ( a1.toUpper() == QString( "BRACKETTYPE" ) )
                    arrowtype = v1;
            }
            if ( SelfContainedTag( thistag ) ) {
                TagStack.removeLast();
                qDebug() << "ID = " << graphic_id;
                qDebug() << "BBox = " << boundingbox;
                qDebug() << "GType = " << graphictype;
                qDebug() << "AType = " << arrowtype;
                ParseBoundingBox( boundingbox, l1, l2, l3, l4 );
                qDebug() << l1 << " " << l2 << " ";
                qDebug() << l3 << " " << l4;
                if ( ( graphictype.toUpper() == QString( "LINE" ) ) && ( arrowtype.toUpper() == QString( "FULLHEAD" ) ) ) {
                    // create end points of this arrow
                    s1 = new DPoint( l1, l2 );
                    e1 = new DPoint( l3, l4 );
                    Arrow *dr1 = new Arrow( r );

                    dr1->setPoints( s1, e1 );
                    dr1->Highlight( true );
                    CDXML_Objects.append( dr1 );
                }
                if ( ( graphictype.toUpper() == QString( "BRACKET" ) ) && ( arrowtype.toUpper() == QString( "SQUAREPAIR" ) ) ) {
                    // create corner points of this bracket
                    qDebug() << "bracket";
                    s1 = new DPoint( l1, l2 );
                    e1 = new DPoint( l3, l4 );
                    Bracket *dr1 = new Bracket( r );

                    dr1->setPoints( s1, e1 );
                    dr1->Highlight( true );
                    CDXML_Objects.append( dr1 );
                }
            }
        }
        // handle </graphic>
        if ( thistag == QString( "</graphic>" ) ) {
            qDebug() << thistag;
            st1 = TagStack.last();
            if ( *st1 == CDXML_GRAPHIC ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <graphic> tags!";
            }
        }
        // handle <s> (string)
        if ( thistag.indexOf( QString( "<s" ) ) >= 0 ) {
            textstr.append( ReadData( wholefile, ptr ) );
            nexttag = ReadTag( wholefile, ptr );
            if ( nexttag != QString( "</s>" ) )
                qDebug() << "Imbalanced <s> tags!";
        }
        // handle <t> (text box)
        if ( thistag.indexOf( QString( "<t" ) ) >= 0 ) {
            st1 = new int;

            *st1 = CDXML_TEXT;
            TagStack.append( st1 );
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    textid = v1;
                if ( a1.toUpper() == QString( "P" ) )
                    textpos = v1;
            }
        }
        // handle </t>
        if ( thistag == QString( "</t>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CDXML_TEXT ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <t> tags!";
            }
            if ( ( tmp_atom != 0 ) && ( nodedepth > 0 ) )
                tmp_atom->element = textstr;
            if ( nodedepth == 0 ) {
                qDebug() << "TEXT:" << textid << "|" << textpos << "|" << textstr;
                tmp_atom = new CML_Atom;
                tmp_atom->atom = false;
                tmp_atom->element = textstr;
                d1 = textpos.indexOf( QString( " " ) );
                tmp_atom->x = textpos.mid( 0, d1 ).toDouble();
                tmp_atom->y = textpos.mid( d1 ).toDouble();
                CML_Atoms.append( tmp_atom );
            }
            textstr = QString( "" );
        }
        // handle node <n>
        if ( thistag.indexOf( QString( "<n" ) ) >= 0 ) {
            st1 = new int;

            *st1 = CDXML_NODE;
            TagStack.append( st1 );
            nodedepth++;
            tmp_atom = new CML_Atom;
            tokens = ReadAttr( thistag );
            nodetype = QString( "" );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    tmp_atom->id = v1;
                if ( a1.toUpper() == QString( "NODETYPE" ) )
                    nodetype = v1;
                if ( a1.toUpper() == QString( "P" ) ) {
                    d1 = v1.indexOf( QString( " " ) );
                    tmp_atom->x = v1.mid( 0, d1 ).toDouble();
                    tmp_atom->y = v1.mid( d1 + 1 ).toDouble();
                }
                if ( a1.toUpper() == QString( "ELEMENT" ) ) {
                    if ( v1 == QString( "1" ) )
                        tmp_atom->element = QString( "H" );
                    if ( v1 == QString( "5" ) )
                        tmp_atom->element = QString( "B" );
                    if ( v1 == QString( "7" ) )
                        tmp_atom->element = QString( "N" );
                    if ( v1 == QString( "8" ) )
                        tmp_atom->element = QString( "O" );
                    if ( v1 == QString( "9" ) )
                        tmp_atom->element = QString( "F" );
                    if ( v1 == QString( "11" ) )
                        tmp_atom->element = QString( "Na" );
                    if ( v1 == QString( "15" ) )
                        tmp_atom->element = QString( "P" );
                    if ( v1 == QString( "16" ) )
                        tmp_atom->element = QString( "S" );
                    if ( v1 == QString( "17" ) )
                        tmp_atom->element = QString( "Cl" );
                    if ( v1 == QString( "19" ) )
                        tmp_atom->element = QString( "K" );
                    if ( v1 == QString( "35" ) )
                        tmp_atom->element = QString( "Br" );
                    if ( v1 == QString( "53" ) )
                        tmp_atom->element = QString( "I" );
                }
            }
            if ( SelfContainedTag( thistag ) ) {
                TagStack.removeLast();
                nodedepth--;
                if ( nodedepth == 0 )   // onl add if not nested
                    CML_Atoms.append( tmp_atom );
            }
        }
        // handle </node> which is really </n>
        if ( thistag == QString( "</n>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CDXML_NODE ) {
                TagStack.removeLast();
                nodedepth--;
            } else {
                qDebug() << "Imbalanced </n> (node) tags!";
            }
            if ( nodedepth == 0 )       // only add if not nested
                CML_Atoms.append( tmp_atom );
        }
        // handle bonds <b>...</b> or <b/>
        if ( thistag.indexOf( QString( "<b" ) ) >= 0 ) {
            qDebug() << thistag;
            st1 = new int;

            *st1 = CDXML_BOND;
            TagStack.append( st1 );
            tmp_bond = new CML_Bond;
            tmp_bond->a1 = "";
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    tmp_bond->id = v1;
                if ( a1.toUpper() == QString( "B" ) )
                    tmp_bond->a1 = v1;
                if ( a1.toUpper() == QString( "E" ) )
                    tmp_bond->a2 = v1;
                if ( a1.toUpper() == QString( "ORDER" ) )
                    tmp_bond->order = v1.toUInt();
                if ( a1.toUpper() == QString( "DISPLAY" ) ) {
                    if ( v1 == QString( "WedgeBegin" ) )
                        tmp_bond->order = 5;
                    if ( v1 == QString( "WedgedHashBegin" ) )
                        tmp_bond->order = 7;
                }
            }
            if ( SelfContainedTag( thistag ) ) {
                TagStack.removeLast();
                if ( nodedepth == 0 )   // only add if not in a node
                    CML_Bonds.append( tmp_bond );
            }
        }
        // handle </bond>
        if ( thistag == QString( "</b>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CDXML_BOND ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <bond> tags!";
            }
            if ( nodedepth == 0 )       // only add if not in a node
                CML_Bonds.append( tmp_bond );
        }
        // Look for end of document
        if ( thistag.indexOf( QString( "</" ) + doctype ) >= 0 ) {
            qDebug() << "</CDXML>";
            st1 = TagStack.last();
            if ( *st1 == CML_MOLECULE ) {
                TagStack.removeLast();
                // should be last tag...
                if ( !TagStack.isEmpty() )
                    qDebug() << "Imbalanced tags!";
                continue;
            }
        }
    } while ( ptr < wholefile.length() );

    qDebug() << CML_Atoms.count() << " atoms";
    qDebug() << CML_Bonds.count() << " bonds";

    // convert CML_Atom and CML_Bond to DPoint and customObject, respectively
    Convert_CML_Lists_To_Native();

    return false;
}

/*
/ Save CDXML file (fn = filename)
bool ChemData::SaveCDXML(QString fn) {
  QPtrList<DPoint> up;
  QPtrList<Drawable> uo;
  Text *tmp_text;
  Bond *tmp_bond;
  QString n1, nfull;
  int n = 0;

  up = UniquePoints();
  uo = UniqueObjects();

  if (up.count() == 0) return false;  // don't write an empty file

  QFile f(fn);
  if (!f.open(IO_WriteOnly)) {
    return false;
  }

  QTextStream t(&f);
  // introductory XML stuff -- declare DTD, start molecule
  t << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>" ;
  t << "<!DOCTYPE molecule SYSTEM \"cml.dtd\" []>" ;
  t << "</molecule>" ;

  f.close();
  return true;
}
*/


// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
