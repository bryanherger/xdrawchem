#include <QDateTime>
#include <QXmlInputSource>
#include <QXmlSimpleReader>

#include "defs.h"
#include "chemdata.h"
#include "dpoint.h"
#include "text.h"
#include "cml.h"
#include "xml_cml.h"

bool ChemData::NewLoadCMLFile( QString fn )
{
    CMLParser handler( r );
    QFile xmlFile( fn );

    if ( !xmlFile.open( QIODevice::ReadOnly ) )
        return false;
    QXmlInputSource source( &xmlFile );
    QXmlSimpleReader reader;

    reader.setFeature( "http://trolltech.com/xml/features/report-whitespace-only-CharData", false );
    reader.setContentHandler( &handler );
    reader.parse( source );

    qDebug() << "done parsing";

    // add bonds and labels to main drawing
    double totlen = 0.0, dx = 99999.0, dy = 99999.0;
    int n = 0;
    int th1;

    qDebug() << "thick_kludge: " << thick_kludge;
    if ( thick_kludge > 0 ) {
        th1 = thick_kludge;
    } else {
        th1 = 1;
    }
    QList < Bond * >parsedBonds;
    Bond *tbond;

    parsedBonds = handler.getBonds();
    foreach ( tbond, parsedBonds ) {
        addBond( tbond->Start(), tbond->End(), th1, tbond->Order(), QColor( 0, 0, 0 ), true );
        if ( tbond->Start()->x < dx )
            dx = tbond->Start()->x;
        if ( tbond->End()->x < dx )
            dx = tbond->End()->x;
        if ( tbond->Start()->y < dy )
            dy = tbond->Start()->y;
        if ( tbond->End()->y < dy )
            dy = tbond->End()->y;
        totlen += tbond->Start()->distanceTo( tbond->End() );
        n++;
    }
    QList < DPoint * >parsedPoints;
    parsedPoints = handler.getPoints();
    QString makesub, ms2;

    foreach ( tmp_pt, parsedPoints ) {
        if ( tmp_pt->element != "C" ) {
            Text *nt = new Text( r );

            nt->setPoint( tmp_pt );
            nt->setJustify( JUSTIFY_CENTER );
            nt->Highlight( true );
            nt->setText( tmp_pt->element );
            makesub = tmp_pt->element;
            ms2 = "";
            for ( int cc = 0; cc < makesub.length(); cc++ ) {
                if ( makesub[cc].isLetter() )
                    ms2.append( " " );
                if ( makesub[cc].isNumber() )
                    ms2.append( "-" );
                if ( makesub[cc] == '+' )
                    ms2.append( "+" );
                if ( makesub[cc] == '-' )
                    ms2.append( "+" );
            }
//            nt->setTextMask( ms2 );
            addText( nt );
        }
    }

    // scale drawing
    // scale points to match current fixed length
    double curfixed = preferences.getBond_fixedlength();
    double avglen = totlen / ( double ) n;
    double sf = curfixed / avglen;

    qDebug() << sf;
    foreach ( tmp_pt, parsedPoints ) {
        tmp_pt->x *= sf;
        tmp_pt->y *= sf;
    }

    Move( -dx * sf, -dy * sf );
    Move( 40, 40 );
    return true;
}

// load CML format (save is waaaay down at the end of this file :)
// 'legacy' CML support.  Current CML format handled by LoadNew...() above
bool ChemData::LoadCMLFile( QString wholefile, QString doctype )
{
    QString line;
    QString tmpline, tfont, tdata, a1, v1;
    QString thistag, nexttag;
    QStringList tokens, refs;
    QStringList elementlist, fclist, x2list, y2list, idlist;
    QStringList a1list, a2list, olist;
    QString str_id, str_title, str_builtin, atomrefs1, atomrefs2, str_order, str_element;
    QString arr_fc, arr_x2, arr_y2;
    int d1, d5, ptr = 0;
    int *st1;

    do {
        thistag = ReadTag( wholefile, ptr );
        qDebug() << thistag.toLatin1();
        // ReadTag returns a null at EOF (ideally).
        if ( thistag.isNull() )
            break;
        // Look for beginning of document
        if ( thistag.indexOf( QString( "<" ) + doctype ) >= 0 ) {
            // push this tag on stack
            st1 = new int;

            *st1 = CML_MOLECULE;
            TagStack.append( st1 );
            // parse attributes in tag
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    MoleculeId = v1;
                if ( a1.toUpper() == QString( "TITLE" ) )
                    MoleculeTitle = v1;
            }
        }
        // handle <atom> records
        if ( thistag.indexOf( QString( "<atom " ) ) >= 0 ) {
            st1 = new int;

            *st1 = CML_ATOM;
            TagStack.append( st1 );
            tmp_atom = new CML_Atom;
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    tmp_atom->id = v1;
            }
        }
        // handle </atom>
        if ( thistag == QString( "</atom>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CML_ATOM ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <atom> tags!";
            }
            CML_Atoms.append( tmp_atom );
        }
        // handle <bond> records
        if ( thistag.indexOf( QString( "<bond " ) ) >= 0 ) {
            st1 = new int;

            *st1 = CML_BOND;
            TagStack.append( st1 );
            tmp_bond = new CML_Bond;
            tmp_bond->a1 = "";
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "ID" ) )
                    tmp_bond->id = v1;
                if ( a1.toUpper() == QString( "ATOMREFS" ) ) {
                    d5 = v1.indexOf( QString( " " ) );
                    tmp_bond->a1 = v1.left( d5 );
                    tmp_bond->a2 = v1.mid( d5 + 1 );
                    qDebug() << "bond[" << tmp_bond->a1.toLatin1() << "|";
                    qDebug() << tmp_bond->a2.toLatin1() << "]";
                }
            }
        }
        // handle </bond>
        if ( thistag == QString( "</bond>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CML_BOND ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <bond> tags!";
            }
            CML_Bonds.append( tmp_bond );
        }
        // handle <atomArray> records
        if ( thistag == QString( "<atomArray>" ) ) {
            st1 = new int;

            *st1 = CML_ATOMARRAY;
            TagStack.append( st1 );
            str_id.remove( 0, 9999 );
            str_element.remove( 0, 9999 );
        }
        // handle </atomArray>
        if ( thistag == QString( "</atomArray>" ) ) {
            st1 = TagStack.last();
            if ( *st1 == CML_ATOMARRAY ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <atomArray> tags!";
            }
            // create lists of tokens in arr_x2, arr_y2, str_id, str_element, arr_fc
            idlist = Tokenize( str_id );
            elementlist = Tokenize( str_element );
            x2list = Tokenize( arr_x2 );
            y2list = Tokenize( arr_y2 );
            fclist = Tokenize( arr_fc );
            for ( d1 = 0; d1 < idlist.count(); d1++ ) {
                tmp_atom = new CML_Atom;
                tmp_atom->id = idlist[d1];
                tmp_atom->element = elementlist[d1];
                tmp_atom->formalCharge = fclist[d1].toInt();
                tmp_atom->x = x2list[d1].toDouble();
                tmp_atom->y = y2list[d1].toDouble();
                CML_Atoms.append( tmp_atom );
            }
        }
        // handle <bondArray> records
        if ( thistag == QString( "<bondArray>" ) ) {
            st1 = new int;

            *st1 = CML_BONDARRAY;
            TagStack.append( st1 );
            atomrefs1.remove( 0, 9999 );
            atomrefs2.remove( 0, 9999 );
            str_order.remove( 0, 9999 );
        }
        // handle </bondArray>
        if ( thistag == QString( "</bondArray>" ) ) {
            qDebug() << atomrefs1;
            qDebug() << atomrefs2;
            qDebug() << str_order;
            st1 = TagStack.last();
            if ( *st1 == CML_BONDARRAY ) {
                TagStack.removeLast();
            } else {
                qDebug() << "Imbalanced <bondArray> tags!";
            }
            // tokenize data
            a1list = Tokenize( atomrefs1 );
            a2list = Tokenize( atomrefs2 );
            olist = Tokenize( str_order );
            for ( d1 = 0; d1 < a1list.count(); d1++ ) {
                tmp_bond = new CML_Bond;
                tmp_bond->a1 = a1list[d1];
                tmp_bond->a2 = a2list[d1];
                tmp_bond->order = olist[d1].toInt();
                CML_Bonds.append( tmp_bond );
            }
        }
        // handle <coordinate2>-</coordinate2>
        if ( thistag.indexOf( QString( "<coordinate2 " ) ) >= 0 ) {
            st1 = TagStack.last();
            if ( *st1 != CML_ATOM )
                qDebug() << "Improperly placed <coordinate2> tag!";
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "COORD2-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "TITLE" ) )
                    str_title = v1;
                if ( a1.toUpper() == QString( "BUILTIN" ) )
                    str_builtin = v1;
            }
            tdata = ReadData( wholefile, ptr );
            nexttag = ReadTag( wholefile, ptr );
            if ( nexttag != QString( "</coordinate2>" ) )
                qDebug() << "Imbalanced <coordinate2> tags!";
            if ( str_builtin != QString( "xy2" ) )
                qDebug() << "Unrecognized builtin type in <coordinate2>!";
            d5 = tdata.indexOf( QString( " " ) );
            tmp_atom->x = tdata.left( d5 ).toDouble();
            tmp_atom->y = tdata.mid( d5 ).toDouble();
        }
        // handle <coordinate3>-</coordinate3>
        if ( thistag.indexOf( QString( "<coordinate3 " ) ) >= 0 ) {
            st1 = TagStack.last();
            if ( *st1 != CML_ATOM )
                qDebug() << "Improperly placed <coordinate3> tag!";
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "COORD3-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "TITLE" ) )
                    str_title = v1;
                if ( a1.toUpper() == QString( "BUILTIN" ) )
                    str_builtin = v1;
            }
            tdata = ReadData( wholefile, ptr );
            nexttag = ReadTag( wholefile, ptr );
            if ( nexttag != QString( "</coordinate3>" ) )
                qDebug() << "Imbalanced <coordinate3> tags!";
            if ( ( str_builtin != QString( "xyz3" ) ) && ( str_builtin != QString( "xyzFract" ) ) )
                qDebug() << "Unrecognized builtin type in <coordinate3>!";
            tokens = Tokenize( tdata );
            tmp_atom->x = tokens[0].toDouble();
            tmp_atom->y = tokens[1].toDouble();
            tmp_atom->z = tokens[2].toDouble();
        }
        // handle <string>-</string> AND <stringArray>-</stringArray>
        if ( ( thistag.indexOf( QString( "<string " ) ) >= 0 ) || ( thistag.indexOf( QString( "<stringArray " ) ) >= 0 ) ) {
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                qDebug() << "STRING-" << a1.toLatin1() << "|" << v1.toLatin1() << "-";
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "TITLE" ) )
                    str_title = v1;
                if ( a1.toUpper() == QString( "BUILTIN" ) )
                    str_builtin = v1;
            }
            tdata = ReadData( wholefile, ptr );
            nexttag = ReadTag( wholefile, ptr );
            qDebug() << "DATA<string>[" << tdata << "]";
            if ( nexttag.left( 8 ) != QString( "</string" ) )
                qDebug() << "Imbalanced <string> tags!";
            // known cases of <string>
            st1 = TagStack.last();
            if ( ( *st1 == CML_BOND ) && ( str_builtin == QString( "atomRef" ) ) ) {
                if ( tmp_bond->a1 == "" )
                    tmp_bond->a1 = tdata;
                else
                    tmp_bond->a2 = tdata;
            }
            if ( ( *st1 == CML_BOND ) && ( str_builtin == QString( "order" ) ) )
                tmp_bond->order = tdata.toInt();
            if ( ( *st1 == CML_ATOM ) && ( str_builtin == QString( "elementType" ) ) )
                tmp_atom->element = tdata;
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "elementType" ) ) )
                str_element = tdata;
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "id" ) ) )
                str_id = tdata;
            if ( ( *st1 == CML_BONDARRAY ) && ( str_builtin == QString( "atomRefs" ) ) ) {
                if ( atomrefs1.length() == 0 )
                    atomrefs1 = tdata;
                else
                    atomrefs2 = tdata;
            }
            if ( ( *st1 == CML_BONDARRAY ) && ( str_builtin == QString( "order" ) ) )
                str_order = tdata;
        }
        // handle <integerArray>-</integerArray> AND
        // <floatArray>-</floatArray>
        if ( ( thistag.indexOf( QString( "<integerArray " ) ) >= 0 ) || ( thistag.indexOf( QString( "<floatArray " ) ) >= 0 ) ) {
            tokens = ReadAttr( thistag );
            for ( int c1 = 0; c1 < tokens.count(); c1 += 2 ) {
                a1 = tokens.at( c1 );
                v1 = tokens.at( c1 + 1 );
                // compare attribute a1 with list of relevant attr's
                if ( a1.toUpper() == QString( "TITLE" ) )
                    str_title = v1;
                if ( a1.toUpper() == QString( "BUILTIN" ) )
                    str_builtin = v1;
            }
            tdata = ReadData( wholefile, ptr );
            nexttag = ReadTag( wholefile, ptr );
            qDebug() << "DATA<numArray>[" << tdata << "]";
            if ( ( nexttag != QString( "</integerArray>" ) ) && ( nexttag != QString( "</floatArray>" ) ) )
                qDebug() << "Imbalanced <integerArray> or <floatArray> tags!";
            // known cases of <integerArray> and <floatArray>
            st1 = TagStack.last();
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "formalCharge" ) ) )
                arr_fc = tdata;
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "x2" ) ) )
                arr_x2 = tdata;
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "y2" ) ) )
                arr_y2 = tdata;
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "x3" ) ) )
                arr_x2 = tdata;
            if ( ( *st1 == CML_ATOMARRAY ) && ( str_builtin == QString( "y3" ) ) )
                arr_y2 = tdata;
            if ( ( *st1 == CML_BONDARRAY ) && ( str_builtin == QString( "order" ) ) )
                str_order = tdata;
        }
        // Look for end of document
        if ( thistag.indexOf( QString( "</" ) + doctype ) >= 0 ) {
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

    // convert CML_Atom and CML_Bond to DPoint and customObject, respectively
    Convert_CML_Lists_To_Native();

    return true;
}

void ChemData::Convert_CML_Lists_To_Native()
{
    double totlen = 0.0;
    int n = 0;
    QString makesub, ms2;
    int th1;

    qDebug() << "thick_kludge: " << thick_kludge;
    if ( thick_kludge > 0 ) {
        th1 = thick_kludge;
    } else {
        th1 = 1;
    }

    // convert CML_Atoms to DPoints
    QList < DPoint * >points;
    foreach ( tmp_atom, CML_Atoms ) {
        points.append( tmp_atom->toDPoint() );
    }
    // add Bonds
    DPoint *end1, *end2;        // Bond endpoints

    foreach ( tmp_bond, CML_Bonds ) {
        // find first atom
        foreach ( tmp_pt, points ) {
            if ( tmp_bond->a1 == tmp_pt->id ) {
                end1 = tmp_pt;
                break;
            }
        }
        // find second atom
        foreach ( tmp_pt, points ) {
            if ( tmp_bond->a2 == tmp_pt->id ) {
                end2 = tmp_pt;
                break;
            }
        }
        addBond( end1, end2, th1, tmp_bond->order, QColor( 0, 0, 0 ), true );
        totlen += end1->distanceTo( end2 );
        n++;
    }
    // add Text labels
    foreach ( tmp_pt, points ) {
        if ( tmp_pt->element != QString( "C" ) ) {
            qDebug() << tmp_pt->element;
            Text *nt = new Text( r );

            nt->setPoint( tmp_pt );
            nt->setJustify( JUSTIFY_CENTER );
            nt->Highlight( true );
            nt->setText( tmp_pt->element );
            makesub = tmp_pt->element;
            ms2 = "";
            for ( int cc = 0; cc < makesub.length(); cc++ ) {
                if ( makesub[cc].isLetter() )
                    ms2.append( " " );
                if ( makesub[cc].isNumber() )
                    ms2.append( "-" );
                if ( makesub[cc] == '+' )
                    ms2.append( "+" );
                if ( makesub[cc] == '-' )
                    ms2.append( "+" );
            }
//            nt->setTextMask( ms2 );
            addText( nt );
        }
    }

    // scale points to match current fixed length
    double curfixed = preferences.getBond_fixedlength();
    double avglen = totlen / ( double ) n;
    double sf = curfixed / avglen;

    qDebug() << sf;
    foreach ( tmp_draw, CDXML_Objects ) {
        points.append( tmp_draw->Start() );
        points.append( tmp_draw->End() );
    }
    foreach ( tmp_pt, points ) {
        tmp_pt->x *= sf;
        tmp_pt->y *= sf;
    }
    foreach ( tmp_draw, CDXML_Objects ) {
        drawlist.append( tmp_draw );
    }

    QRect sb = selectionBox();

    qDebug() << sb.left();
    qDebug() << sb.right();
    qDebug() << sb.top();
    qDebug() << sb.bottom();
    double tx = RenderTopLeft.x() + 50 - sb.left();
    double ty = RenderTopLeft.y() + 50 - sb.top();

    Move( tx, ty );
}

// Save CML format
bool ChemData::save_cml( QString fn )
{
    QList < DPoint * >up;
    QList < Drawable * >uo;
    Text *tmp_text;
    Bond *tmp_bond;
    QString n1, nfull;
    int n = 0;

    up = UniquePoints();
    uo = UniqueObjects();

    if ( up.count() == 0 )
        return false;           // don't write an empty file

    QFile f( fn );

    if ( !f.open( QIODevice::WriteOnly ) ) {
        return false;
    }

    QTextStream t( &f );

    // introductory XML stuff -- declare DTD, start molecule
    t << "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>";
    t << "<!DOCTYPE molecule SYSTEM \"cml.dtd\" []>";
    t << "<molecule ";
    QString MoleculeId = fn.right( fn.length() - fn.lastIndexOf( "/" ) - 1 );

    t << "id=\"" << MoleculeId.toLatin1() << "\">";
    // add miscellaneous info, a la JChemPaint
    t << "<string title=\"GenerationDate\">" << QDateTime::currentDateTime().toString() << "</string>";
    t << "<string title=\"GenerationSoftware\">" << XDC_VERSION << "</string>" << endl;

    // Copy text from Text objects to element field in DPoint
    foreach ( tmp_draw, uo ) {
        if ( tmp_draw->Type() == TYPE_TEXT ) {
            tmp_text = ( Text * ) tmp_draw;     // is this cheating?
            tmp_text->Start()->element = tmp_text->getText();
        }
    }
    // Add XML ID's to DPoint's, write as we go
    t << "<atomArray>";
    foreach ( tmp_pt, up ) {
        n1.setNum( n );
        nfull = QString( "a" ) + n1;
        tmp_pt->id = nfull;
        n++;
        t << "<atom id=\"" << tmp_pt->id << "\">";
        t << "<string builtin=\"elementType\">";
        t << tmp_pt->element << "</string>";
        t << "<float builtin=\"x2\">";
        t << tmp_pt->x;
        t << "</float>" << endl << "<float builtin=\"y2\">";
        t << tmp_pt->y;
        t << "</float>";
        //t << "<integer builtin=\"formalCharge\">0</integer>" ;
        t << "</atom>";
    }
    t << "</atomArray>";

    // add XML ID's to Bond's, write as we go
    n = 0;
    t << "<bondArray>";
    foreach ( tmp_draw, uo ) {
        if ( tmp_draw->Type() == TYPE_BOND ) {
            tmp_bond = ( Bond * ) tmp_draw;     // I ask again, is this cheating?
            n1.setNum( n );
            nfull = QString( "b" ) + n1;
            tmp_draw->setID( nfull );
            n++;
            t << "<bond id=\"" << tmp_bond->getID() << "\">";
            t << "<string builtin=\"atomRef\">" << tmp_bond->Start()->id << "</string>";
            t << "<string builtin=\"atomRef\">" << tmp_bond->End()->id << "</string>";
            t << "<string builtin=\"order\">" << tmp_bond->Order()
                << "</string>";
            if ( tmp_bond->Order() == 5 ) {   // stereo-up bonds
                t << "<string builtin=\"stereo\" convention=\"MDLMol\">W</string>";
            }
            if ( tmp_bond->Order() == 7 ) {   // stereo-down bonds
                t << "<string builtin=\"stereo\" convention=\"MDLMol\">H</string>";
            }
            t << "</bond>";
        }
    }
    t << "</bondArray>";
    // End file...
    t << "</molecule>";

    f.close();
    return true;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
