#ifndef ORNEREDE_BROWSER_H
#define ORNEREDE_BROWSER_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QTextBrowser>
#include <QNetworkAccessManager>
#include <QNetworkReply>

class OrneredeBrowser : public QMainWindow {
    Q_OBJECT

public:
    explicit OrneredeBrowser(QWidget *parent = nullptr);

private:
    QTabWidget *tabs;
    QLineEdit *addressBar;
    QNetworkAccessManager *networkManager;

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
