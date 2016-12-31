#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using std::cout;
using std::endl;

#include <qapplication.h>
#include <qstringlist.h>
#include <qstring.h>
#include <qtextstream.h>
#include <qnetworkprotocol.h>
#include <qurloperator.h>
#include <qurl.h>
#include <qclipboard.h>

#include "netaccess.h"
#include "http.h"

NetAccess::NetAccess() {
  status = true;
  sname = "unknown";
  siupacname = "unknown";
  scas = "unknown";
  spccompound = "unknown";
  op = new QUrlOperator("http://localhost/cgi-bin/printenv");
  connect(op, SIGNAL(data( const QByteArray &, QNetworkOperation * )),
	  this, SLOT(slotData(const QByteArray &)) );
  connect(op, SIGNAL(finished(QNetworkOperation *)),
	  this, SLOT(slotFinished(QNetworkOperation *)) );
  connect(&hp1, SIGNAL( done(bool) ),
	  this, SLOT( rf(bool) ) );
}

QStringList NetAccess::getChoices(QString server, QString key, QString value,
				  bool exact) {
  QStringList results;

  if (key == "formula") {
    // put atoms in proper order
    value = Rearrange(value);
  }

  QString cmd;

  //cmd.append("http://");
  cmd.append(server);
  //cmd.append("xdrawchem.lunarpages.net");
  cmd.append("/newdb.php?field=");
  cmd.append(key);
  cmd.append("&query=");
  if (key == "name") value = value.upper();
  cmd.append(value);
  cmd.append("&exact=");
  if (exact)
    cmd.append("on");
  else
    cmd.append("off");
  
  std::cout << "getChoices URL:" << cmd << std::endl;

  HTTP if1(cmd);
  if1.exec();

  //cout << endl << "Results:" << endl << if1.Data() << endl;

  QString testq = if1.Data();

  QTextStream in1(&testq, IO_ReadOnly);

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

QString NetAccess::getCanonicalSmiles(QString nserver, QString insmiles) {
  QString cmd, wholefile;

  cmd.append("http://");
  cmd.append(nserver);
  cmd.append("/cgi-bin/ftest.py?smiles=");
  cmd.append(insmiles);
  
  HTTP if1(cmd);
  if1.exec();

  //cout << endl << "Results:" << endl << if1.Data() << endl;

  wholefile = if1.Data();

  if (wholefile.contains("sorry") > 0) wholefile = "";

  return wholefile;
}

QString NetAccess::getFile(QString server, QString fn) {
  QString cmd, wholefile;

  cmd.append("http://");
  cmd.append(server);
  cmd.append("/cgi-bin/getfile?name=");
  cmd.append(fn);
  
  HTTP if1(cmd);
  if1.exec();

  //cout << endl << "Results:" << endl << if1.Data() << endl;

  wholefile = if1.Data();

  return wholefile;
}

bool NetAccess::runBuild3D(QString buildfile) {
  QString cmd, wholefile;

  cmd.append("http://xdrawchem.sourceforge.net/cgi-bin/runbuild?buildfile=");
  cmd.append(buildfile);
  
  HTTP if1(cmd);
  if1.exec();

  //cout << endl << "Results:" << endl << if1.Data() << endl;

  s3dmol = if1.Data();

  if (s3dmol.contains("can't write ") > 0) return false;
  if (s3dmol.contains("output file not found") > 0) return false;

  return true;
}

bool NetAccess::runInChI(QString buildfile) {
  QString cmd, wholefile;

  QTextStream ts1(&buildfile, IO_ReadOnly);

  wholefile = "";

  cmd = "";
  cmd.append("http://xdrawchem.sourceforge.net/cgi-bin/smiles2inchi?smiles=");
  cmd.append(buildfile);
  
  HTTP if1(cmd);
  if1.exec();

  std::cout << std::endl << "Results:"<<std::endl << if1.Data() << std::endl;

  fullinchi = if1.Data();

  if (fullinchi.contains("can't write ") > 0) return false;
  if (fullinchi.contains("output file not found") > 0) return false;

  QTextStream ts2(&fullinchi, IO_ReadOnly);
  do {
    shortinchi = ts2.readLine();
    if (shortinchi.left(5).lower() == "inchi") break;
  } while (!ts2.atEnd());

  std::cout << "Basic InChI: " << shortinchi << std::endl;

  return true;
}

QString NetAccess::Rearrange(QString key) {
  QString key1, subkey, subnum;
  int cc;
  bool addflag;
  
  QStringList allatoms;

  do {
    //cout << key << endl;
    // parse out element
    subkey = "";
    addflag = false;
    subkey.append(key[0]);
    for (cc = 1; cc < key.length(); cc++) {
      if (key[cc].category() == QChar::Letter_Uppercase) {
	allatoms.append(subkey);
	key.remove(0, cc);
	addflag = true;
	break;
      } else {
	subkey.append(key[cc]);
      }
    }
    if (addflag == false) {
      allatoms.append(subkey);
      key.remove(0, cc);
    }
  } while (key.length() > 0);

  //cout << allatoms.count() << endl;
  
  QString sym, n1;
  allatoms.sort();
  if (allatoms.count() > 0) {
    for (QStringList::Iterator ir = allatoms.begin(); 
	 ir != allatoms.end(); ++ir) {
      n1 = *ir;
      if (n1.contains("C") > 0)
	key1.append(n1);
      if (n1.contains("H") > 0)
	key1.append(n1);
    }
    for (QStringList::Iterator ir = allatoms.begin(); 
	 ir != allatoms.end(); ++ir) {
      n1 = *ir;
      if ( (n1.contains("C") == 0) && (n1.contains("H") == 0) )
	key1.append(n1);
    }
  }

  return key1;
}

void NetAccess::slotData(const QByteArray &d1) {
   htfile.append(d1);
}

void NetAccess::slotFinished(QNetworkOperation *op) {
  cout << "Finished network access" << endl;
  status = false;
}

bool NetAccess::getNameCAS(QString nserver, QString ssmiles) {
  // don't waste bandwidth...
  if (ssmiles.length() < 2) return false;

  runInChI(ssmiles);

  QStringList choices = getChoices(nserver, "inchi", shortinchi, true);

  QString tmp_str, tpc, tcas, tiname, tformula, tsyn;
  int i1;
  for (QStringList::Iterator ir = choices.begin(); 
       ir != choices.end(); ++ir) {
    tmp_str = *ir;
    std::cout << ">>>" << tmp_str << std::endl;
    tmp_str.replace("\",\"", "~");
    i1 = tmp_str.find("~");
    tpc = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tcas = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tiname = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tformula = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    i1 = tmp_str.find("~");
    tsyn = tmp_str.mid(0, i1);
    tmp_str.remove(0, i1 + 1);
    //std::cout << tcas << "," << tname <<","<< tformula << "," << tformat 
    //      << std::endl;
    tpc.replace("\"", "");
    tcas.replace("\"", "");
    tsyn.replace("\"", "");
    tiname.replace("\"", "");
    std::cout << "-- DB return: --" << std::endl;
    std::cout << tpc << std::endl;
    std::cout << tcas << std::endl;
    std::cout << tiname << std::endl;
    std::cout << tsyn << std::endl;
    std::cout << "-- DB end --" << std::endl;
    spccompound = tpc;
    scas = tcas;
    siupacname = tiname;
    sname = tsyn;
    return true;
  }

  return false;
}

bool NetAccess::get3DMol(QString inmol) {
  QUrl::encode(inmol);
  inmol.prepend("http://localhost/cgi-bin/nph-build.cgi?mol=");
  //QUrl::encode(inmol);
  std::cout << "Request: " << inmol << std::endl;
  HTTP if1( inmol );
  if1.exec();
  s3dmol = if1.Data();
  std::cout << "GET:" << s3dmol << std::endl;

  return false;
}

void NetAccess::rf(bool error) {
  if (error) {
    qDebug("error: %s", hp1.errorString().latin1());
  } else {
    QString result(hp1.readAll());
    s3dmol = result;
    std::cout << s3dmol << std::endl;
  }
}
