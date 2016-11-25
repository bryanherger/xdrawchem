// CDXML_Reader:  XML Reader for CDXML type.

#include <QColor>
#include <QString>
#include <QList>

#include "chemdata.h"
#include "dpoint.h"
#include "xml_reader.h"

class ColorTableEntry
{
public:
    ColorTableEntry() {}
    int index;
    QColor color;
};

class FontTableEntry
{
public:
    FontTableEntry() {}
    QString id;
    QString fam;
};

class CDXML_Object
{
public:
    CDXML_Object() {}
    QString id; // XML ID
    QString start_id; // id of start and end points for bonds
    QString end_id;
    DPoint *start; // start and end points for non-bonds
    DPoint *end;
    int type;
    int idata1, idata2;
    double ddata1, ddata2;
    QString sdata;
    QColor color;
    QFont font;
};

class CDXML_Reader : public XML_Reader
{
public:
    CDXML_Reader( ChemData * );
    bool ReadFile( QString );

private:
    // basically, all member functions are private.  Not necessary but
    // probably a good idea
    void ParseDocument( QString);
    void ParseColorTable( QString );
    QColor ParseColor( QString );
    void ParseFontTable( QString );
    FontTableEntry *ParseFont( QString );
    void ParsePage( QString );
    void ParseFragment( QString );
    QString ParseText( QString );
    void ParseGraphic( QString );
    void ParseNode( QString );
    void ParseBond( QString );
    DPoint *FindNode( QString );
    void Build();
    // ----
    // data
    // ----
    // color table and temporary pointer to access it
    ColorTableEntry *tmp_color;
    QList<ColorTableEntry *> colors;
    // font table and temporary pointer to access it
    FontTableEntry *tmp_font;
    QList<FontTableEntry *> fonts;
    // node list (atoms) and temp object
    QList<DPoint *> nodelist;
    DPoint *tmp_node;
    // CDXML object list and temp object
    QList<CDXML_Object *> objectlist;
    CDXML_Object *tmp_obj;
    // ChemData object we are reading into
    ChemData *c;
    // how many levels of node/fragment are we in
    int nodedepth, fragdepth;
    // for passing data
    double globalx, globaly;
};

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
