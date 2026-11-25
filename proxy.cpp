#include "proxy.h"

#include <QUrl>
#include <QUrlQuery>
#include <QNetworkRequest>
#include <QHostAddress>
#include <QPointer>

ProxyServer::ProxyServer(QObject *parent)
    : QObject(parent) {

    server = new QTcpServer(this);
    networkManager = new QNetworkAccessManager(this);

    connect(
        server,
        &QTcpServer::newConnection,
        this,
        &ProxyServer::handleConnection
    );
}

bool ProxyServer::start(quint16 port) {

    // Only accept connections from this computer.
    return server->listen(
        QHostAddress::LocalHost,
        port
    );
}

void ProxyServer::stop() {
    server->close();
}

void ProxyServer::handleConnection() {

    while (server->hasPendingConnections()) {

        QTcpSocket *socket =
            server->nextPendingConnection();

        buffers.insert(socket, QByteArray());

        connect(
            socket,
            &QTcpSocket::readyRead,
            this,
            [this, socket]() {
                handleRequest(socket);
            }
        );

        connect(
            socket,
            &QTcpSocket::disconnected,
            this,
            [this, socket]() {
                buffers.remove(socket);
                socket->deleteLater();
            }
        );
    }
}

void ProxyServer::sendError(
    QTcpSocket *socket,
    int status,
    const QByteArray &reason
) {

    if (socket->state() !=
        QAbstractSocket::ConnectedState) {
        return;
    }

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

    if (!buffers.contains(socket))
        return;

    QByteArray &buffer = buffers[socket];

    buffer += socket->readAll();

    // Limit request headers.
    if (buffer.size() > 16384) {
        sendError(socket, 413, "Request Too Large");
        return;
    }

    // Wait until the complete HTTP headers arrive.
    int headerEnd = buffer.indexOf("\r\n\r\n");

    if (headerEnd < 0)
        return;

    QList<QByteArray> lines =
        buffer.left(headerEnd).split('\n');

    if (lines.isEmpty()) {
        sendError(socket, 400, "Bad Request");
        return;
    }

    QList<QByteArray> requestLine =
        lines.first().trimmed().split(' ');

    if (requestLine.size() < 3 ||
        requestLine[0] != "GET" ||
        !requestLine[1].startsWith("/fetch?")) {

        sendError(
            socket,
            405,
            "Only GET /fetch is supported"
        );

        return;
    }

    // Read the target URL from the local fetch request.
    QUrl localUrl(
        "http://127.0.0.1" +
        QString::fromUtf8(requestLine[1])
    );

    QUrlQuery query(localUrl);

    QUrl target(
        query.queryItemValue(
            "url",
            QUrl::FullyDecoded
        )
    );

    // Validate the target.
    if (!target.isValid() ||
        target.host().isEmpty() ||
        (target.scheme() != "http" &&
         target.scheme() != "https") ||
        !target.userInfo().isEmpty()) {

        sendError(
            socket,
            400,
            "Invalid Target URL"
        );

        return;
    }

    // Basic local-address protection.
    QString host = target.host().toLower();

    if (host == "localhost" ||
        host.endsWith(".localhost") ||
        host == "127.0.0.1" ||
        host == "::1" ||
        host == "0.0.0.0") {

        sendError(
            socket,
            403,
            "Local Destinations Blocked"
        );

        return;
    }

    // Fetch the requested website.
    QNetworkRequest request(target);

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "OrneredeBrowser/0.1"
    );

    request.setAttribute(
        QNetworkRequest::RedirectPolicyAttribute,
        QNetworkRequest::NoLessSafeRedirectPolicy
    );

    QNetworkReply *reply =
        networkManager->get(request);

    QPointer<QTcpSocket> guardedSocket(socket);

    connect(
        reply,
        &QNetworkReply::finished,
        this,
        [guardedSocket, reply]() {

            if (!guardedSocket ||
                guardedSocket->state() !=
                QAbstractSocket::ConnectedState) {

                reply->deleteLater();
                return;
            }

            QByteArray body = reply->readAll();

            int status =
                reply->attribute(
                    QNetworkRequest::HttpStatusCodeAttribute
                ).toInt();

            QByteArray contentType =
                reply->header(
                    QNetworkRequest::ContentTypeHeader
                ).toByteArray();

            if (contentType.isEmpty()) {
                contentType =
                    "application/octet-stream";
            }

            if (status == 0)
                status = 502;

            QByteArray reason = "Upstream Response";

            if (status == 200)
                reason = "OK";
            else if (status == 301)
                reason = "Moved Permanently";
            else if (status == 302)
                reason = "Found";
            else if (status == 403)
                reason = "Forbidden";
            else if (status == 404)
                reason = "Not Found";
            else if (status >= 500)
                reason = "Upstream Error";

            QByteArray response =
                "HTTP/1.1 " +
                QByteArray::number(status) +
                " " + reason + "\r\n"
                "Content-Type: " + contentType + "\r\n"
                "X-Content-Type-Options: nosniff\r\n"
                "Connection: close\r\n"
                "Content-Length: " +
                QByteArray::number(body.size()) +
                "\r\n\r\n" + body;

            guardedSocket->write(response);
            guardedSocket->disconnectFromHost();

            reply->deleteLater();
        }
    );
}
