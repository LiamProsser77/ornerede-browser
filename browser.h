#ifndef ORNEREDE_BROWSER_H
#define ORNEREDE_BROWSER_H

#include <QMainWindow>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>

class OrneredeBrowser : public QMainWindow {
    Q_OBJECT

public:
    explicit OrneredeBrowser(QWidget *parent = nullptr);

private:
    QTabWidget *tabs;
    QLineEdit *addressBar;

    void addNewTab();
    void navigateToAddress();
    void goBack();
    void goForward();
    void refreshPage();
    void goHome();
};

#endif
