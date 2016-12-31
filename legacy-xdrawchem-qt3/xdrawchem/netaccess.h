#ifndef NETACCESS_H
#define NETACCESS_H

#include <qobject.h>
#include <qnetworkprotocol.h>
#include <qurloperator.h>
#include <qhttp.h>

class QString;
class QStringList;

class NetAccess : public QObject {
  Q_OBJECT
public:
  NetAccess();
  QStringList getChoices(QString, QString, QString, bool);
  QString getFile(QString, QString);
  QString getCanonicalSmiles(QString, QString);
  QString Rearrange(QString);
  bool getNameCAS(QString, QString);
  bool get3DMol(QString);
  bool runBuild3D(QString);
  bool runInChI(QString);
  bool status;
  QString htfile, spccompound, sname, scas, siupacname, s3dmol, fullinchi, 
    shortinchi;
  QUrlOperator *op;
  QHttp hp1;
  QNetworkOperation *op1;

public slots:
  void slotData(const QByteArray &);
  void slotFinished(QNetworkOperation *);
  void rf(bool);
};

#endif
