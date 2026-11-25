#ifndef ORNEREDE_BROWSER_H
#define ORNEREDE_BROWSER_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QNetworkAccessManager>
#include <QTextBrowser>
#include <QUrl>
#include <QMenu>

class OrneredeBrowser : public QMainWindow {
    Q_OBJECT

public:
    explicit OrneredeBrowser(QWidget *parent = nullptr);

private:
    QTabWidget *tabs;
    QLineEdit *addressBar;
    QNetworkAccessManager *networkManager;
    QMenu *bookmarksMenu;

    void addNewTab();
    void navigateToAddress();
    void loadPage(const QUrl &url);
    void displayPage(QNetworkReply *reply, const QUrl &url);
    void addBookmark();
    void updateBookmarksMenu();
    void openBookmark(const QString &url);
};

#endif
