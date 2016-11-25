// http.h -- class to get stuff from network.

#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <qstring.h>
#include <qurloperator.h>
#include <qcstring.h>
#include <qnetworkprotocol.h>
#include <qdialog.h>
#include <qlabel.h>

using std::cout;
using std::endl;

class HTTP : public QDialog {
  Q_OBJECT
    public:
  HTTP(QString url1) { 
    setCaption( tr("Accessing network") );
    QLabel *l1 = new QLabel(this, "wait label");
    l1->setGeometry(10,10,300,50);
    l1->setText( tr("Please wait.") );
    status = false; 
    connect(&op, SIGNAL(data( const QByteArray &, QNetworkOperation * )),
	    this, SLOT(slotData(const QByteArray &)) );
    connect(&op, SIGNAL(finished(QNetworkOperation *)),
	    this, SLOT(slotFinished(QNetworkOperation *)) );
    op = url1;
    op.get();
  }
  void GetURL(QString url1) {
    op = url1;
    op.get();
  }
  QString Data() { return htfile; }
  bool done() { return status; }
  QString htfile;
  bool status;
  QUrlOperator op;
  
 public slots:
    void slotData(const QByteArray &d1) { 
   htfile.append(d1);
   std::cout << "htfile:" << htfile << std::endl;
 }
 void slotFinished(QNetworkOperation *op) {
   status = true; 
   cout << "Finished!" << endl;
   accept();
 }
};

#endif
