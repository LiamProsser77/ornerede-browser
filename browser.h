#ifndef ORNEREDE_BROWSER_H
#define ORNEREDE_BROWSER_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>
#include <QTextBrowser>
#include <QUrl>

class OrneredeBrowser : public QMainWindow {
    Q_OBJECT

public:
    explicit OrneredeBrowser(QWidget *parent = nullptr);

private:
    QTabWidget *tabs;
    QLineEdit *addressBar;
    QNetworkAccessManager *networkManager;

    QString proxyHost = "127.0.0.1";
    quint16 proxyPort = 8080;

    void configureProxy();

    void addNewTab();
    void navigateToAddress();
    void loadPage(const QUrl &url);
    void displayPage(QNetworkReply *reply);

    void goBack();
    void goForward();
    void refreshPage();
    void goHome();
};

#endif
