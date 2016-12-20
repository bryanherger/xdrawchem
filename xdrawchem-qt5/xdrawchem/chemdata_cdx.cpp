#include "chemdata.h"
#include "dpoint.h"
#include "text.h"
#include "defs.h"

// save as ChemDraw binary format (IBM PC byte order)
bool ChemData::save_cdx( QString fn )
{
    QList < DPoint * >up;
    QList < Drawable * >uo;
    QString tmpline;

    // get all unique points and objects
    up = UniquePoints();
    uo = UniqueObjects();

    if ( up.count() == 0 )
        return false;           // don't write an empty file

    // open file and text stream
    QFile f( fn );

    if ( !f.open( QIODevice::WriteOnly ) ) {
        return false;
    }

    f.close();
    return false;
}

// save as ChemDraw XML text format
bool ChemData::save_cdxml( QString fn )
{
    QList < DPoint * >up;
    QList < Drawable * >uo;
    QString tmpline, cur_id;
    int idcount = 1;

    // open file and text stream
    QFile f( fn );

    if ( !f.open( QIODevice::WriteOnly ) ) {
        return false;
    }

    QTextStream t( &f );

    t << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    t << "<!DOCTYPE CDXML SYSTEM \"http://www.camsoft.com/xml/cdxml.dtd\">";
    t << "<CDXML CreationProgram=\"" << XDC_VERSION << "\">";
    t << "<fonttable>";
    t << "<font id=\"21\" charset=\"x-mac-roman\" name=\"Helvetica\"/>";
    t << "</fonttable>";
    t << "<page>";

    // save document here
    foreach ( tmp_draw, drawlist ) {
        cur_id.setNum( idcount );
        tmpline = tmp_draw->ToCDXML( cur_id );
        if ( tmp_draw->Type() == TYPE_MOLECULE )
            idcount += 500;
        else
            idcount += 2;
        t << tmpline;
    }

    t << "</page>";
    t << "</CDXML>";

    f.close();
    return true;
}

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
