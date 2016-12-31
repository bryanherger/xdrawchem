#ifndef NETACCESS_H
#define NETACCESS_H

#include <QDialog>

class QString;
class QStringList;
class QProgressDialog;
class QHttp;
class QBuffer;

class NetAccess : public QDialog
{
    Q_OBJECT

public:
    NetAccess();
    QStringList getChoices( QString, QString, QString, bool );
    QString getFile( QString, QString );
    QString getCanonicalSmiles( QString, QString );
    QString Rearrange( QString );
    bool getNameCAS( QString, QString );
    bool get3DMol( QString );
    bool runBuild3D( QString );
    bool runInChI( QString );
    bool status;
    QString htfile, spccompound, sname, scas, siupacname,
            s3dmol, fullinchi, shortinchi;
    QHttp *http;

public slots:
    void slotData( const QByteArray & );
    void slotFinished( int, bool );
    void rf( bool );

private:
    QProgressDialog *progressDialog;
    bool httpRequestAborted;
    QBuffer *buffer;
    int choicesId;

private slots:
    void slotUpdateDataReadProgress( int, int );
    void slotCancelDownload();

signals:
    void choicesFinished( const QStringList & );
};

#endif

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
