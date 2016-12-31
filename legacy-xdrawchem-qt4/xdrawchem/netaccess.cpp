#include <QHttp>
#include <QProgressDialog>
#include <QBuffer>
#include <QUrl>
#include <QDebug>

#include "http.h"
#include "netaccess.h"

NetAccess::NetAccess():QDialog()
{
    status = true;
    sname = "unknown";
    siupacname = "unknown";
    scas = "unknown";
    spccompound = "unknown";
    buffer = 0;
    http = new QHttp( this );
    progressDialog = new QProgressDialog( this );
    connect( http, SIGNAL( dataReadProgress( int, int ) ), SLOT( slotUpdateDataReadProgress( int, int ) ) );
    connect( http, SIGNAL( requestFinished( int, bool ) ), SLOT( slotFinished( int, bool ) ) );

    connect( http, SIGNAL( done( bool ) ), SLOT( rf( bool ) ) );
}

QStringList NetAccess::getChoices( QString server, QString key, QString value, bool exact )
{
    if ( key == "formula" ) {
        // put atoms in proper order
        value = Rearrange( value );
    }

    QString cmd;

    cmd.append(server);
    cmd.append( "/newdb.php?field=" );
    cmd.append( key );
    cmd.append( "&query=" );
    if ( key == "name" )
        value = value.toUpper();
    cmd.append( value );
    cmd.append( "&exact=" );
    if ( exact )
        cmd.append( "on" );
    else
        cmd.append( "off" );

    QUrl url = QUrl( cmd );

    qDebug() << "getChoices URL:" << cmd;

  HTTP if1(cmd);
  if1.exec();
  httpRequestAborted = false;
  http->setHost( url.host(), url.port() != -1 ? url.port() : 80 );
  qDebug() << endl << "Results:" << endl << if1.Data() ;

  //connect( progressDialog, SIGNAL( canceled() ), this, SLOT( slotCancelDownload() ) );

  //buffer = new QBuffer( this );
  //buffer->open( QIODevice::WriteOnly );
  QString testq = if1.Data();
  //choicesId = http->get( url.path(), buffer );
  //  QTextStream in1(buffer, QIODevice::ReadOnly);
  //progressDialog->setLabelText( tr( "Getting choices" ) );
  //    progressDialog->show();
  //bool saveflag = false;

  QTextStream in1(&testq, QIODevice::ReadOnly);
  QStringList results;

  //bool saveflag = false;
  QString tmpl;
  do {
    tmpl = in1.readLine();
    //if (tmpl == "XXX") { saveflag = true; continue; }
    //if (tmpl == "YYY") { saveflag = false; continue; }
    //if (saveflag) results.append(tmpl);
    if (tmpl.length() > 5)
      results.append(tmpl);
  } while (!in1.atEnd());

  return results;  // what a program should always do!
}

QString NetAccess::getCanonicalSmiles( QString nserver, QString insmiles )
{
/*  QString cmd, wholefile;

  QHttpRequestHeader header("GET", "/index.html");
    header.setValue("Host", "www.trolltech.com");
    http->setHost("www.trolltech.com");
    http->request(header);

  cmd.append("http://");
  cmd.append(nserver);
  cmd.append("/cgi-bin/ftest.py?smiles=");
  cmd.append(insmiles);

  HTTP if1(cmd);
  if1.exec();

  //qDebug() << endl << "Results:" << endl << if1.Data() ;

  wholefile = if1.Data();

  if (wholefile.contains("sorry") > 0) wholefile = "";

  return wholefile;*/
  qDebug() << "NetAccess::getCanonicalSMILES() is obsolete";
  return QString();
}

QString NetAccess::getFile( QString server, QString fn )
{
  QString cmd, wholefile;

  cmd.append("http://");
  cmd.append(server);
  cmd.append("/cgi-bin/getfile?name=");
  cmd.append(fn);

  HTTP if1(cmd);
  if1.exec();

  //qDebug() << endl << "Results:" << endl << if1.Data() ;

  wholefile = if1.Data();

  return wholefile;
  return QString();
}

bool NetAccess::runBuild3D( QString buildfile )
{
  QString cmd, wholefile;

  cmd.append("http://xdrawchem.sourceforge.net/cgi-bin/runbuild?buildfile=");
  cmd.append(buildfile);

  HTTP if1(cmd);
  if1.exec();

  //qDebug() << endl << "Results:" << endl << if1.Data() ;

  s3dmol = if1.Data();

  if (s3dmol.contains("can't write ") > 0) return false;
  if (s3dmol.contains("output file not found") > 0) return false;

  return true;
}

bool NetAccess::runInChI( QString buildfile )
{
/*  QString cmd, wholefile;

  QTextStream ts1(&buildfile, QIODevice::ReadOnly);

  wholefile = "";

  cmd = "";
  cmd.append("http://xdrawchem.sourceforge.net/cgi-bin/smiles2inchi?smiles=");
  cmd.append(buildfile);

  HTTP if1(cmd);
  if1.exec();

  qDebug() << "Results:" << if1.Data();

  fullinchi = if1.Data();

  if (fullinchi.contains("can't write ") > 0) return false;
  if (fullinchi.contains("output file not found") > 0) return false;

  QTextStream ts2(&fullinchi, QIODevice::ReadOnly);
  do {
    shortinchi = ts2.readLine();
    if (shortinchi.left(5).toLower() == "inchi") break;
  } while (!ts2.atEnd());

  qDebug() << "Basic InChI: " << shortinchi;
*/
  qDebug() << "obsolete function call: NetAccess::runInChI()";

  return true;
}

QString NetAccess::Rearrange( QString key )
{
    QString key1, subkey, subnum;
    int cc;
    bool addflag;

    QStringList allatoms;

    do {
        //qDebug() << key ;
        // parse out element
        subkey = "";
        addflag = false;
        subkey.append( key[0] );
        for ( cc = 1; cc < key.length(); cc++ ) {
            if ( key[cc].category() == QChar::Letter_Uppercase ) {
                allatoms.append( subkey );
                key.remove( 0, cc );
                addflag = true;
                break;
            } else {
                subkey.append( key[cc] );
            }
        }
        if ( addflag == false ) {
            allatoms.append( subkey );
            key.remove( 0, cc );
        }
    } while ( key.length() > 0 );

    //qDebug() << allatoms.count() ;

    QString sym, n1;

    allatoms.sort();
    if ( allatoms.count() > 0 ) {
        for ( QStringList::Iterator ir = allatoms.begin(); ir != allatoms.end(); ++ir ) {
            n1 = *ir;
            if ( n1.contains( "C" ) > 0 )
                key1.append( n1 );
            if ( n1.contains( "H" ) > 0 )
                key1.append( n1 );
        }
        for ( QStringList::Iterator ir = allatoms.begin(); ir != allatoms.end(); ++ir ) {
            n1 = *ir;
            if ( ( n1.contains( "C" ) == 0 ) && ( n1.contains( "H" ) == 0 ) )
                key1.append( n1 );
        }
    }

    return key1;
}

void NetAccess::slotData( const QByteArray & d1 )
{
    htfile.append( d1 );
}

void NetAccess::slotUpdateDataReadProgress( int bytesRead, int totalBytes )
{
    if ( httpRequestAborted )
        return;

    progressDialog->setMaximum( totalBytes );
    progressDialog->setValue( bytesRead );
}

void NetAccess::slotCancelDownload()
{
    qDebug() << "slotCancelDownload";
    httpRequestAborted = true;
    http->abort();
    emit( choicesFinished( QStringList() ) );
}

void NetAccess::slotFinished( int httpId, bool )
{
  qDebug() << "Finished network access";
  status = false;

  if ( httpId == choicesId ) {
    buffer->close();

    QString tmpl;
    QStringList results = QStringList( "test" );

    buffer->open( QIODevice::ReadOnly );
    do {
      tmpl = buffer->readLine();
      qDebug() << tmpl;
      //if (tmpl == "XXX") { saveflag = true; continue; }
      //if (tmpl == "YYY") { saveflag = false; continue; }
      //if (saveflag) results.append(tmpl);
      if ( tmpl.length() > 5 )
	results.append( tmpl );
    } while ( !buffer->atEnd() );
    emit( choicesFinished( results ) );
    delete buffer;

    buffer = 0;
  }
}

bool NetAccess::getNameCAS( QString nserver, QString sinchi )
{
    // don't waste bandwidth...
    if ( sinchi.length() < 2 )
        return false;

    //runInChI( ssmiles );  obsolete, we use OpenBabel now!
    sinchi = sinchi.trimmed();

    QStringList choices = getChoices( nserver, "inchi", sinchi, true );

    QString tmp_str, tpc, tcas, tiname, tformula, tsyn;
    int i1;

    for ( QStringList::Iterator ir = choices.begin(); ir != choices.end(); ++ir ) {
        tmp_str = *ir;
        qDebug() << ">>>" << tmp_str;
        tmp_str.replace( "\",\"", "~" );
        i1 = tmp_str.indexOf( "~" );
        tpc = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tcas = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tiname = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tformula = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        i1 = tmp_str.indexOf( "~" );
        tsyn = tmp_str.mid( 0, i1 );
        tmp_str.remove( 0, i1 + 1 );
        //qDebug() << tcas << "," << tname <<","<< tformula << "," << tformat
        //     ;
        tpc.replace( "\"", "" );
        tcas.replace( "\"", "" );
        tsyn.replace( "\"", "" );
        tiname.replace( "\"", "" );
        qDebug() << "-- DB return: --";
        qDebug() << tpc;
        qDebug() << tcas;
        qDebug() << tiname;
        qDebug() << tsyn;
        qDebug() << "-- DB end --";
        spccompound = tpc;
        scas = tcas;
        siupacname = tiname;
        sname = tsyn;
        return true;
    }

    return false;
}

bool NetAccess::get3DMol( QString inmol )
{
/*  Q3Url::encode(inmol);
  inmol.prepend("http://localhost/cgi-bin/nph-build.cgi?mol=");
  //QUrl::encode(inmol);
  qDebug() << "Request: " << inmol;
  HTTP if1( inmol );
  if1.exec();
  s3dmol = if1.Data();
  qDebug() << "GET:" << s3dmol;
*/
    return false;
}

void NetAccess::rf( bool error )
{
/*  if (error) {
    qDebug("error: %s", hp1.errorString().toLatin1());
  } else {
    QString result(hp1.readAll());
    s3dmol = result;
    qDebug() << s3dmol;
  }*/
}

//cmake#include "netaccess.moc"

// kate: tab-width 4; indent-width 4; space-indent on; replace-trailing-space-save on;
