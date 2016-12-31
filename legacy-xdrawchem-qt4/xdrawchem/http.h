// http.h -- class to get stuff from network.

#ifndef HTTP_H
#define HTTP_H

#include <iostream>
#include <QtGui>
#include <QtNetwork>

using std::cout;
using std::endl;

class HTTP : public QDialog {
  Q_OBJECT
    public:
  HTTP(QString url1) { 
    //setCaption( tr("Accessing network") );
    QLabel *l1 = new QLabel(this);
    l1->setGeometry(10,10,300,50);
    l1->setText( tr("Please wait.") );
    status = false;
    http = new QHttp(this);
    connect(http, SIGNAL(requestFinished(int, bool)),
	    this, SLOT(httpRequestFinished(int, bool)));
    connect(http, SIGNAL(dataReadProgress(int, int)),
	    this, SLOT(updateDataReadProgress(int, int)));
    connect(http, SIGNAL(responseHeaderReceived(const QHttpResponseHeader &)),
	    this, SLOT(readResponseHeader(const QHttpResponseHeader &)));

    QUrl url(url1);
    buffer = new QBuffer();
    buffer->open(QIODevice::WriteOnly);
    http->setHost(url.host(), url.port() != -1 ? url.port() : 80);
    httpRequestAborted = false;
    //httpGetId = http->get(url.path(), buffer);
    httpGetId = http->get(url1, buffer);
     /*
    connect(&op, SIGNAL(data( const QByteArray &, QNetworkOperation * )),
	    this, SLOT(slotData(const QByteArray &)) );
    connect(&op, SIGNAL(finished(QNetworkOperation *)),
	    this, SLOT(slotFinished(QNetworkOperation *)) );
    op = url1;
    op.get();
     */
  }
  void GetURL(QString url1) {
    //op = url1;
    //op.get();
  }
  QString Data() { return htfile; }
  bool done() { return status; }
  QString htfile;
  QBuffer *buffer;
  bool status, httpRequestAborted;
  int httpGetId;
  QHttp *http;
  
 public slots:
  /*
    void slotData(const QByteArray &d1) { 
   htfile.append(d1);
   std::cout << "htfile:" << htfile << std::endl;
 }
 void slotFinished(QNetworkOperation *op) {
   status = true; 
   cout << "Finished!" << endl;
   accept();
 }
  */

void httpRequestFinished(int requestId, bool error)
 {
   qDebug() << "httpRequestFinished";
   if (httpRequestAborted) {
     if (buffer) {
       buffer->close();
       delete buffer;
       buffer = 0;
     }     
     return;
   }
   
   if (requestId != httpGetId)
     return;
   
   buffer->close();
   
   if (error) {
     QMessageBox::information(this, tr("HTTP"),
			      tr("Download failed: %1.")
			      .arg(http->errorString()));
   } else {
     htfile.append( buffer->data() );
   }
   
   delete buffer;
   buffer = 0;
   accept();
 }

 void readResponseHeader(const QHttpResponseHeader &responseHeader)
 {
   if (responseHeader.statusCode() != 200) {
     QMessageBox::information(this, tr("HTTP"),
			      tr("Download failed: %1")
			      .arg(responseHeader.reasonPhrase()));
     httpRequestAborted = true;
     //progressDialog->hide();
     http->abort();
     return;
   }
 }

 void updateDataReadProgress(int bytesRead, int totalBytes)
 {
   if (httpRequestAborted)
     return;
   
   //progressDialog->setMaximum(totalBytes);
   //progressDialog->setValue(bytesRead);
 }

};

#endif
