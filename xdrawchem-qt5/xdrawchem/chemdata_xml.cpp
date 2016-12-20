#include <iostream>

#include "chemdata.h"
#include "cdxml_reader.h"
#include "dpoint.h"
#include "text.h"
#include "defs.h"

bool ChemData::load_xml( QString fn )
{
    QFile f( fn );

    if ( !f.open( QIODevice::ReadOnly ) ) {
        return false;
    }

    // Read entire file to string
    QString wholefile = f.readAll();

    // We're done with the file.  Close it
    f.close();

    // try and guess whether (new) CML file
    bool cmlfile = false;

    if ( wholefile.toUpper().contains( "<ATOM" ) > 0 )
        cmlfile = true;
    if ( wholefile.toUpper().contains( "<XDRAWCHEM" ) > 0 )
        cmlfile = false;
    if ( wholefile.toUpper().contains( "<CDXML" ) > 0 )
        cmlfile = false;
    if ( wholefile.toUpper().contains( "ATOMREFS=" ) > 0 )
        cmlfile = false;
    if ( wholefile.toUpper().contains( "<STRINGARRAY" ) > 0 )
        cmlfile = false;

    if ( cmlfile == true ) {
        bool r1 = NewLoadCMLFile( fn );

        return r1;
    }
    // trasfer control to process_xml()
    return ProcessXML( wholefile );
}

// if you have already read in a file (e.g., from network), call this
bool ChemData::ProcessXML( QString wholefile )
{
    QString line;
    QString doctype, thistag, nexttag, cml_doctype, cdxml_doctype, xdc_doctype;
    QStringList tokens, refs;
    int d1, d2, d3, ptr = 0, cml_type = 0;

    //double l1, l2, l3, l4;

    // clear all lists
    CML_Atoms.clear();
    CML_Atoms_Sub.clear();
    CML_Bonds.clear();
    CML_Bonds_Sub.clear();
    CDXML_Objects.clear();

    // kludge!
    cml_doctype = QString( "idgniosdnvi" );
    cdxml_doctype = QString( "idgniosdnvi" );
    xdc_doctype = QString( "idgniosdnvi" );

    do {
        thistag = ReadTag( wholefile, ptr );
        // ReadTag returns a null at EOF (ideally).
        if ( thistag.isNull() )
            break;
        // check for 'special' tags...
        // continue past <?xml...> tag
        if ( thistag.indexOf( QString( "?xml" ) ) >= 0 )
            continue;
        // but we really want the <!DOCTYPE...> tag
        if ( thistag.indexOf( QString( "!DOCTYPE" ) ) >= 0 ) {
            // read the actual document type
            d1 = thistag.indexOf( QString( "!DOCTYPE" ) );
            d2 = thistag.indexOf( QString( " " ), d1 );
            d3 = thistag.indexOf( QString( " " ), d2 + 1 );
            doctype = thistag.mid( d2 + 1, d3 - d2 - 1 );
            if ( thistag.toUpper().indexOf( "CML.DTD" ) >= 0 )
                cml_type = 1;
            if ( thistag.toUpper().indexOf( "CML-1999-05-15" ) >= 0 )
                cml_type = 1;
            if ( thistag.toUpper().indexOf( "CDXML.DTD" ) >= 0 )
                cml_type = 2;
            if ( thistag.toUpper().indexOf( "XDRAWCHEM-1" ) >= 0 )
                cml_type = 3;
            if ( cml_type == 1 )
                cml_doctype = doctype;
            if ( cml_type == 2 )
                cdxml_doctype = doctype;
            if ( cml_type == 3 )
                xdc_doctype = doctype;
            if ( cml_type == 0 ) {
                return false;
            }
            continue;
        }
        // Extract CML documents, pass to LoadCMLFile
        if ( thistag.indexOf( QString( "<" ) + cml_doctype ) >= 0 ) {
            d1 = ptr - thistag.length();
            d2 = wholefile.indexOf( QString( "</" ) + cml_doctype + QString( ">" ), ptr ) + 3 + cml_doctype.length();
            LoadCMLFile( wholefile.mid( d1, d2 - d1 ), cml_doctype );
            wholefile.remove( d1, d2 - d1 );
        }
        // Extract CDXML documents, pass to LoadCDXMLFile
        if ( thistag.indexOf( QString( "<" ) + cdxml_doctype ) >= 0 ) {
            // test functionality
            CDXML_Reader reader( this );

            reader.ReadFile( wholefile );
            // end test
            d1 = ptr - thistag.length();
            d2 = wholefile.indexOf( QString( "</" ) + cdxml_doctype + QString( ">" ), ptr ) + 3 + cml_doctype.length();
            //LoadCDXMLFile(wholefile.mid(d1, d2 - d1), cdxml_doctype);
            wholefile.remove( d1, d2 - d1 );
        }
        // Extract native (XDC) documents, pass to load_native()
        // note that native documents expect <xdrawchem>...</xdrawchem>
        // and that this pair should contain all elements in the document
        if ( thistag.indexOf( QString( "<" ) + xdc_doctype ) >= 0 ) {
            d1 = ptr - thistag.length();
            d2 = wholefile.indexOf( QString( "</" ) + xdc_doctype + QString( ">" ), ptr ) + 3 + cml_doctype.length();
            load_native( wholefile.mid( d1, d2 - d1 ) );
            wholefile.remove( d1, d2 - d1 );
        }
    } while ( ptr < wholefile.length() );
    r->update();

    return true;
}

// *****************************
// General XML parsing functions
// *****************************

// function to see if tag is 'self-contained', i.e.:
// <bond>data</bond> returns false
// <bond data="x"/> returns true
// (returns true if tag ends in "/>"
bool ChemData::SelfContainedTag( QString tag )
{
    if ( tag.right( 2 ) == "/>" )
        return true;
    else
        return false;
}

// function to extract tags from stream
QString ChemData::ReadTag( QString f, int &ptr )
{
    int t1 = f.indexOf( "<", ptr );

    ptr = t1;
    int t2 = f.indexOf( ">", ptr );

    ptr = t2 + 1;
    return f.mid( t1, ptr - t1 ).toLatin1();
}

// function to extract data between tags
QString ChemData::ReadData( QString f, int &ptr )
{
    int t1 = f.indexOf( "<", ptr );

    return f.mid( ptr, t1 - ptr );
}

// function to extract attributes from tag
// Return format:  list item 0=first attribute, 1=value of first attribute,
//   2=second attribute, 3=value of second attribute, you get it, right?
QStringList ChemData::ReadAttr( QString tag )
{
    QStringList l1;
    QString attr;
    QString a1, v1;
    int t1, eq1, te;

    // where to start tokenizing
    int ptr = tag.indexOf( QString( " " ) );

    // where to end tokenizing
    int eol = tag.indexOf( QString( ">" ) );

    do {
        while ( tag[ptr].isSpace() ) {
            ptr++;
        }
        t1 = tag.indexOf( QString( "\"" ), ptr );
        te = tag.indexOf( QString( "\"" ), t1 + 1 );
        attr = tag.mid( ptr, te - ptr + 1 );
        ptr = te + 1;
        eq1 = attr.indexOf( QString( "=" ) );
        a1 = attr.left( eq1 );
        v1 = attr.right( attr.length() - eq1 - 1 );
        v1 = v1.mid( 1, v1.length() - 2 );
        if ( a1.indexOf( QString( ">" ) ) >= 0 )
            break;              // end of tags
        //qDebug() << "[" << a1 << "=" << v1 << "]" ;
        l1.append( a1 );
        l1.append( v1 );
    } while ( ptr < eol );

    return l1;
}

// function to tokenize
QStringList ChemData::Tokenize( QString tag )
{
    QStringList l1;
    QString a1, tag1;
    int t1;

    // strip leading and trailing white space
    tag1 = tag.simplified();
    // where to start tokenizing
    int ptr = 0;

    // where to end tokenizing
    int eol = tag1.lastIndexOf( QString( " " ) );

    do {
        t1 = tag1.indexOf( QString( " " ), ptr );
        a1 = tag1.mid( ptr, t1 - ptr );
        ptr = t1 + 1;
        l1.append( a1 );
        qDebug() << "[" << a1 << "]";
    } while ( ptr < eol );

    a1 = tag1.mid( ptr );
    l1.append( a1 );

    return l1;
}

// ParseBoundingBox(): parse CDXML BoundingBox element.
void ChemData::ParseBoundingBox( QString BBox, double &x1, double &y1, double &x2, double &y2 )
{
    int s1 = BBox.indexOf( QString( " " ), 0 );
    int s2 = BBox.indexOf( QString( " " ), s1 + 1 );
    int s3 = BBox.indexOf( QString( " " ), s2 + 1 );

    //qDebug() << BBox.mid(0, s1) << "|";
    //qDebug() << BBox.mid(s1 + 1, s2 - s1) << "|";
    //qDebug() << BBox.mid(s2 + 1, s3 - s2) << "|";
    //qDebug() << BBox.mid(s3 + 1) << "|" ;
    x1 = BBox.mid( 0, s1 ).toDouble();
    y1 = BBox.mid( s1 + 1, s2 - s1 ).toDouble();
    x2 = BBox.mid( s2 + 1, s3 - s2 ).toDouble();
    y2 = BBox.mid( s3 + 1 ).toDouble();
}

// *********************
// end parsing functions
// *********************

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
