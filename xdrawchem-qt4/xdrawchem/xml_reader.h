// XML_Reader.h - base class for XML type readers

class XML_Reader
{
public:
    // reimplement for subclasses
    virtual bool readFile( const QString & ) { return false; }
    // utility functions
    bool selfContainedTag( const QString & );
    QString readTag( const QString &, int );
    QString readData( const QString &, int );
    QStringList readAttr( const QString & );
    QStringList tokenize( const QString & );
    void parseBoundingBox( const QString &, double, double, double, double );
    int positionOfEndTag( const QString &, const QString & );
};

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
