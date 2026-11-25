#include "proxy.h"

#include <QUrl>
#include <QNetworkRequest>
#include <QRegularExpression>
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

void ProxyServer::handleRequest(QTcpSocket *socket) {

    QByteArray requestData = socket->readAll();

    QList<QByteArray> lines = requestData.split('\n');

    if (lines.isEmpty()) {
        socket->disconnectFromHost();
        return;
    }

    QList<QByteArray> requestLine = lines[0].trimmed().split(' ');

    if (requestLine.size() < 2) {
        socket->disconnectFromHost();
        return;
    }

    QUrl url(QString::fromUtf8(requestLine[1]));

    if (!url.isValid() || url.host().isEmpty() ||
        (url.scheme() != "http" && url.scheme() != "https")) {

        socket->write(
            "HTTP/1.1 400 Bad Request\r\n"
            "Connection: close\r\n"
            "Content-Length: 0\r\n\r\n"
        );

        socket->disconnectFromHost();
        return;
    }

    QNetworkRequest request(url);

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "OrneredeProxy/0.1"
    );

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
            this, [socket, reply]() {

        QByteArray response;

        if (reply->error() != QNetworkReply::NoError) {

            response =
                "HTTP/1.1 502 Bad Gateway\r\n"
                "Connection: close\r\n"
                "Content-Length: 0\r\n\r\n";

        } else {

            QByteArray body = reply->readAll();

            response =
                "HTTP/1.1 200 OK\r\n"
                "Content-Type: text/html; charset=utf-8\r\n"
                "Connection: close\r\n"
                "Content-Length: " +
                QByteArray::number(body.size()) +
                "\r\n\r\n" + body;
        }

        if (socket->state() == QAbstractSocket::ConnectedState) {
            socket->write(response);
            socket->disconnectFromHost();
        }

        reply->deleteLater();
    });
}
