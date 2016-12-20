#ifndef HTTP_H
#define HTTP_H

#include <QCoreApplication>
#include <QDialog>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslError>
#include <QStringList>
#include <QTimer>
#include <QUrl>

#include <stdio.h>

class QSslError;

QT_USE_NAMESPACE

class HTTP : public QDialog
{
    Q_OBJECT
    QNetworkAccessManager *manager;
    QList<QNetworkReply *> currentDownloads;
    QString replyText;
    QString cUrl;
    bool finished;

public:
    HTTP();
    HTTP(QString);
    void doDownload(const QUrl &url);
    QString Data();
    void execute(QString);

public slots:
    void downloadFinished(QNetworkReply *reply);
    void slotError(QNetworkReply::NetworkError e);
    void sslErrors(const QList<QSslError> &errors);
};

#endif
