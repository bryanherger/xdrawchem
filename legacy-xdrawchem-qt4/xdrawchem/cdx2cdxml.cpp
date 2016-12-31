#include <QStack>

#include "chemdata.h"
#include "dpoint.h"
#include "text.h"
#include "defs.h"

// Parse property (2 byte property + 2 byte length + value)
QString ChemData::ParseProperty( QByteArray a )
{
    QString propvalue;
    QString sx, sy;

    propvalue = "";

    if ( ( a[0] == '0' ) && ( a[1] == '2' ) ) { // position, p
        int y1 = ( unsigned char ) a[4] + ( unsigned char ) a[5] * 256 + ( unsigned char ) a[6] * 65536 + ( unsigned char ) a[7] * 16777216;
        int x1 = ( unsigned char ) a[8] + ( unsigned char ) a[9] * 256 + ( unsigned char ) a[10] * 65536 + ( unsigned char ) a[11] * 16777216;

        sx.setNum( ( double ) x1 / 65536.0 );
        sy.setNum( ( double ) y1 / 65536.0 );
        propvalue.append( "p=\"" );
        propvalue.append( sx );
        propvalue.append( " " );
        propvalue.append( sy );
        propvalue.append( "\"" );
        return propvalue;
    }
    if ( ( a[0] == '4' ) && ( a[1] == '2' ) ) { // bounding box
        int y1 = ( unsigned char ) a[4] + ( unsigned char ) a[5] * 256 + ( unsigned char ) a[6] * 65536 + ( unsigned char ) a[7] * 16777216;
        int x1 = ( unsigned char ) a[8] + ( unsigned char ) a[9] * 256 + ( unsigned char ) a[10] * 65536 + ( unsigned char ) a[11] * 16777216;
        int y2 = ( unsigned char ) a[12] + ( unsigned char ) a[13] * 256 + ( unsigned char ) a[14] * 65536 + ( unsigned char ) a[15] * 16777216;
        int x2 = ( unsigned char ) a[16] + ( unsigned char ) a[17] * 256 + ( unsigned char ) a[18] * 65536 + ( unsigned char ) a[19] * 16777216;

        sx.setNum( ( double ) x1 / 65536.0 );
        sy.setNum( ( double ) y1 / 65536.0 );
        QString sx2, sy2;

        sx2.setNum( ( double ) x2 / 65536.0 );
        sy2.setNum( ( double ) y2 / 65536.0 );
        propvalue.append( "BoundingBox=\"" );
        propvalue.append( sx );
        propvalue.append( " " );
        propvalue.append( sy );
        propvalue.append( " " );
        propvalue.append( sx2 );
        propvalue.append( " " );
        propvalue.append( sy2 );
        propvalue.append( "\"" );
        return propvalue;
    }
    if ( ( a[0] == '4' ) && ( a[1] == '6' ) ) { // bond begin B
        propvalue.append( "B=\"" );
        int x1 = ( unsigned char ) a[4] + ( unsigned char ) a[5] * 256 + ( unsigned char ) a[6] * 65536 + ( unsigned char ) a[7] * 16777216;

        sx.setNum( x1 );
        propvalue.append( sx );
        propvalue.append( "\"" );
        return propvalue;
    }
    if ( ( a[0] == '5' ) && ( a[1] == '6' ) ) { // bond end E
        propvalue.append( "E=\"" );
        int x1 = ( unsigned char ) a[4] + ( unsigned char ) a[5] * 256 + ( unsigned char ) a[6] * 65536 + ( unsigned char ) a[7] * 16777216;

        sx.setNum( x1 );
        propvalue.append( sx );
        propvalue.append( "\"" );
        return propvalue;
    }
    if ( ( a[0] == '0' ) && ( a[1] == '6' ) ) { // bond order
        propvalue.append( "Order=\"" );
        int x1 = ( unsigned char ) a[4];

        sx.setNum( x1 );
        propvalue.append( sx );
        propvalue.append( "\"" );
        return propvalue;
    }
    if ( ( a[0] == '1' ) && ( a[1] == '6' ) ) { // bond display property
        int x1 = ( unsigned char ) a[4];

        if ( x1 == 3 )
            propvalue = "Display=\"WedgedHashBegin\"";
        if ( x1 == 6 )
            propvalue = "Display=\"WedgeBegin\"";
        return propvalue;
    }
    if ( ( a[0] == '0' ) && ( a[1] == 'a' ) ) { // graphic type
        if ( a[4] == '1' )
            propvalue = "GraphicType=\"Line\"";
        if ( a[4] == '2' )
            propvalue = "GraphicType=\"Arc\"";
        if ( a[4] == '6' )
            propvalue = "GraphicType=\"Bracket\"";
        if ( a[4] == '7' )
            propvalue = "GraphicType=\"Symbol\"";
        return propvalue;
    }
    if ( ( a[0] == '6' ) && ( a[1] == 'a' ) ) { // bracket type
        if ( a[4] == '0' )
            propvalue = "BracketType=\"RoundPair\"";
        if ( a[4] == '1' )
            propvalue = "BracketType=\"SquarePair\"";
        if ( a[4] == '2' )
            propvalue = "BracketType=\"CurlyPair\"";
        return propvalue;
    }
    if ( ( a[0] == '2' ) && ( a[1] == 'a' ) ) { // arrow type
        propvalue = "ArrowType=\"FullHead\"";
        return propvalue;
    }
    if ( ( a[0] == '0' ) && ( a[1] == '7' ) ) { // <s>
        propvalue.append( "<s size=\"12\">" );
        int x1 = ( a[4] * 10 ) + 6;

        for ( int d = x1; d < a.size(); d++ )
            propvalue += a.at( d );
        propvalue.append( "</s>" );
        return propvalue;
    }
    qDebug() << "Unrecognized property: " << hex << ( int ) a[1] << " " << ( int ) a[0] << " [" << dec << a.size() - 4 << " bytes]";
    return QString( "" );
}

// Convert binary CDX to text CDXML
QString ChemData::CDXToCDXML( QString fn )
{
    QFile f( fn );
    QString wholefile, proptag, stag;

    QStack<QString *> tagstack;
    QString *ts;

    if ( !f.open( QIODevice::ReadOnly ) )
        return wholefile;

    qDebug() << "CDX ChemDraw binary file format.";
    //QMessageBox::warning(r, "Can't read binary format",
    //           "This file is a ChemDraw (CDX) binary file, but this program cannot read it (yet :)");

    // make a QByteArray
    QByteArray cdxfile = f.readAll();

//    for ( int c = 0; c < f.size(); c++ ) {
//        cdxfile[c] = f.getch();
//    }
    bool opentag = false;

    // start reading tags.
    int ptr;

    ptr = 28;                   // theoretically, position 28 is where the Document tag is
    // the Document tag doesn't really matter to us; we ignore most of that
    // stuff anyways...
    wholefile.append( "<CDXML>" );

    if ( ( cdxfile[ptr] == '0' ) && ( cdxfile[ptr + 1] == 'b' ) )
        ptr = 30;
    do {                        // until a <colortable>, <fonttable>, or <page> tag is found
        if ( ( cdxfile[ptr] == '0' ) && ( cdxfile[ptr + 1] == '0' ) ) { // end tag?
            if ( tagstack.count() > 0 ) {
                if ( opentag )
                    wholefile.append( ">" );
                ts = tagstack.top();
                if ( ( *ts == "</t>" ) && ( stag.length() > 1 ) ) {
                    wholefile.append( stag );
                    stag = "";
                }
                ts = tagstack.pop();
                wholefile.append( *ts );
                ptr += 2;
                opentag = false;
            } else {
                ptr += 2;
            }
            continue;
        }
        if ( cdxfile[ptr + 1] >= '0' ) {        // it's a property
            int moveforward = cdxfile[ptr + 3] * 256;   // TODO + cdxfile[ptr+2] + 4;
            QByteArray b( moveforward, '0' );

            for ( int d = 0; d < moveforward; d++ )
                b[d] = cdxfile[ptr + d];
            proptag = ParseProperty( b );
            if ( ( cdxfile[ptr] == '0' ) && ( cdxfile[ptr + 1] == '7' ) ) {
                stag = proptag;
                ptr += moveforward;     // move past data
                continue;
            }
            if ( proptag.length() > 1 ) {
                wholefile.append( " " );
                wholefile.append( proptag );
            }
            ptr += moveforward; // move past data
            continue;
        }
        if ( ( unsigned char ) cdxfile[ptr + 1] == 'c' ) {      // it's an object
            if ( ( tagstack.count() > 0 ) && opentag )
                wholefile.append( ">" );
            opentag = true;
            int local_id = 0;

            local_id = local_id + ( unsigned char ) cdxfile[ptr + 2];
            local_id = local_id + ( unsigned char ) cdxfile[ptr + 3] * 256;
            QString s_id;

            s_id.setNum( local_id );
            if ( cdxfile[ptr] == '1' ) {        // page object
                wholefile.append( "<page " );
                wholefile.append( "id=\"" );
                wholefile.append( s_id );
                wholefile.append( "\"" );
                ts = new QString( "</page>" );
                tagstack.push( ts );
                ptr += 6;       // move past tag and ID
                continue;
            }
            if ( cdxfile[ptr] == '3' ) {        // fragment object
                wholefile.append( "<fragment " );
                wholefile.append( "id=\"" );
                wholefile.append( s_id );
                wholefile.append( "\"" );
                ts = new QString( "</fragment>" );
                tagstack.push( ts );
                ptr += 6;       // move past tag and ID
                continue;
            }
            if ( cdxfile[ptr] == '4' ) {        // node object
                wholefile.append( "<n " );
                wholefile.append( "id=\"" );
                wholefile.append( s_id );
                wholefile.append( "\"" );
                ts = new QString( "</n>" );
                tagstack.push( ts );
                ptr += 6;       // move past tag and ID
                continue;
            }
            if ( cdxfile[ptr] == '5' ) {        // bond object
                wholefile.append( "<b " );
                wholefile.append( "id=\"" );
                wholefile.append( s_id );
                wholefile.append( "\"" );
                opentag = false;
                ts = new QString( "/>" );
                tagstack.push( ts );
                ptr += 6;       // move past tag and ID
                continue;
            }
            if ( cdxfile[ptr] == '6' ) {        // text object
                wholefile.append( "<t " );
                wholefile.append( "id=\"" );
                wholefile.append( s_id );
                wholefile.append( "\"" );
                ts = new QString( "</t>" );
                tagstack.push( ts );
                ptr += 6;       // move past tag and ID
                continue;
            }
            if ( cdxfile[ptr] == '7' ) {        // graphic object
                wholefile.append( "<graphic " );
                wholefile.append( "id=\"" );
                wholefile.append( s_id );
                wholefile.append( "\"" );
                opentag = false;
                ts = new QString( "/>" );
                tagstack.push( ts );
                ptr += 6;       // move past tag and ID
                continue;
            }
        }
    } while ( ptr < cdxfile.size() );

    wholefile.append( "</CDXML>" );

    qDebug() << wholefile;
    return wholefile;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
