#include <stdio.h>

#include "http.h"

HTTP::HTTP()
{
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            SLOT(downloadFinished(QNetworkReply*)));
}

HTTP::HTTP(QString url1)
{
finished = false;
//url1 = "http://www.woodsidelabs.com/";
qDebug() << "HTTP:" << url1;
manager = new QNetworkAccessManager(this);
connect(manager, SIGNAL(finished(QNetworkReply*)),
        this, SLOT(downloadFinished(QNetworkReply*)));
QNetworkReply *reply = manager->get(QNetworkRequest(QUrl(url1)));
connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
        this, SLOT(slotError(QNetworkReply::NetworkError)));
#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif
}

void HTTP::slotError(const QNetworkReply::NetworkError e)
{
qDebug() << "An error happened!";
}

void HTTP::doDownload(const QUrl &url)
{
    qDebug() << "doDownload: " << url;
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);

#ifndef QT_NO_SSL
    connect(reply, SIGNAL(sslErrors(QList<QSslError>)), SLOT(sslErrors(QList<QSslError>)));
#endif

    currentDownloads.append(reply);
}

void HTTP::execute(QString xurl)
{
        QUrl url = QUrl::fromUserInput(xurl);
        doDownload(url);
        while (!finished) { qDebug() << "Waiting..."; }
}

void HTTP::sslErrors(const QList<QSslError> &sslErrors)
{
#ifndef QT_NO_SSL
    foreach (const QSslError &error, sslErrors)
        fprintf(stderr, "SSL error: %s\n", qPrintable(error.errorString()));
#else
    Q_UNUSED(sslErrors);
#endif
}

void HTTP::downloadFinished(QNetworkReply *reply)
{
    replyText = reply->readAll();
    qDebug() << "downloadFinished:" << replyText;
    currentDownloads.removeAll(reply);
    reply->deleteLater();
    accept();
}

QString HTTP::Data() { return replyText; }
