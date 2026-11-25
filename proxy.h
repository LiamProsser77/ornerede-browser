#ifndef ORNEREDE_PROXY_H
#define ORNEREDE_PROXY_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QHash>

class ProxyServer : public QObject {
    Q_OBJECT

public:
    explicit ProxyServer(QObject *parent = nullptr);

    bool start(quint16 port = 8080);
    void stop();

private:
    QTcpServer *server;
    QNetworkAccessManager *networkManager;

    QHash<QTcpSocket *, QByteArray> buffers;

    void handleConnection();
    void handleRequest(QTcpSocket *socket);

    void sendError(
        QTcpSocket *socket,
        int status,
        const QByteArray &reason
    );
};

#endif
