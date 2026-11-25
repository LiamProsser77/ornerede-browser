#include "proxy.h"

#include <QNetworkRequest>
#include <QUrl>
#include <QUrlQuery>
#include <QPointer>
#include <QHostAddress>

ProxyServer::ProxyServer(QObject *parent)
    : QObject(parent) {

    server = new QTcpServer(this);
    networkManager = new QNetworkAccessManager(this);

    connect(server, &QTcpServer::newConnection,
            this, &ProxyServer::handleConnection);
}

bool ProxyServer::start(quint16 port) {
    return server->listen(QHostAddress::LocalHost, port);
}

void ProxyServer::stop() {
    server->close();
}

void ProxyServer::handleConnection() {

    while (server->hasPendingConnections()) {

        QTcpSocket *socket = server->nextPendingConnection();

        connect(socket, &QTcpSocket::readyRead,
                this, [this, socket]() {
            handleRequest(socket);
        });

        connect(socket, &QTcpSocket::disconnected,
                socket, &QObject::deleteLater);
    }
}

void ProxyServer::sendError(
    QTcpSocket *socket,
    int status,
    const QByteArray &reason
) {

    QByteArray body = reason + "\n";

    QByteArray response =
        "HTTP/1.1 " + QByteArray::number(status) +
        " " + reason + "\r\n"
        "Content-Type: text/plain; charset=utf-8\r\n"
        "Connection: close\r\n"
        "Content-Length: " +
        QByteArray::number(body.size()) +
        "\r\n\r\n" + body;

    socket->write(response);
    socket->disconnectFromHost();
}

void ProxyServer::handleRequest(QTcpSocket *socket) {

    QByteArray data = socket->readAll();

    int headerEnd = data.indexOf("\r\n\r\n");

    if (headerEnd < 0)
        return;

    QList<QByteArray> lines =
        data.left(headerEnd).split('\n');

    if (lines.isEmpty()) {
        sendError(socket, 400, "Bad Request");
        return;
    }

    QList<QByteArray> requestLine =
        lines.first().trimmed().split(' ');

    if (requestLine.size() < 2 ||
        requestLine[0] != "GET") {
        sendError(socket, 405, "Method Not Allowed");
        return;
    }

    QUrl localUrl(
        "http://127.0.0.1" +
        QString::fromUtf8(requestLine[1])
    );

    QUrlQuery query(localUrl);

    QUrl target(
        query.queryItemValue("url", QUrl::FullyDecoded)
    );

    if (!target.isValid() ||
        target.host().isEmpty() ||
        (target.scheme() != "http" &&
         target.scheme() != "https") ||
        !target.userInfo().isEmpty()) {

        sendError(socket, 400, "Invalid URL");
        return;
    }

    QString host = target.host().toLower();

    if (host == "localhost" ||
        host.endsWith(".localhost") ||
        host == "127.0.0.1" ||
        host == "::1" ||
        host == "0.0.0.0") {

        sendError(socket, 403, "Local address blocked");
        return;
    }

    QNetworkRequest request(target);

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "Ornerede/0.0.1"
    );

    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy
    );

    QNetworkReply *reply = networkManager->get(request);

    QPointer<QTcpSocket> safeSocket(socket);

    connect(reply, &QNetworkReply::finished,
            this, [safeSocket, reply]() {

        if (!safeSocket ||
            safeSocket->state() !=
            QAbstractSocket::ConnectedState) {

            reply->deleteLater();
            return;
        }

        QByteArray body = reply->readAll();

        int status = reply->attribute(
            QNetworkRequest::HttpStatusCodeAttribute
        ).toInt();

        if (status == 0)
            status = 502;

        QByteArray contentType = reply->header(
            QNetworkRequest::ContentTypeHeader
        ).toByteArray();

        if (contentType.isEmpty())
            contentType = "application/octet-stream";

        QByteArray reason = "OK";

        if (status >= 400)
            reason = "Upstream Error";

        QByteArray response =
            "HTTP/1.1 " + QByteArray::number(status) +
            " " + reason + "\r\n"
            "Content-Type: " + contentType + "\r\n"
            "Connection: close\r\n"
            "Content-Length: " +
            QByteArray::number(body.size()) +
            "\r\n\r\n" + body;

        safeSocket->write(response);
        safeSocket->disconnectFromHost();

        reply->deleteLater();
    });
}
