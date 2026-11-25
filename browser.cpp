#include "browser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QWidget>
#include <QUrl>
#include <QMessageBox>

OrneredeBrowser::OrneredeBrowser(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Ornerede Browser");
    resize(1200, 800);

    networkManager = new QNetworkAccessManager(this);

    QWidget *central = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(central);

    // Navigation bar
    QHBoxLayout *navigation = new QHBoxLayout();

    QPushButton *backButton = new QPushButton("<");
    QPushButton *forwardButton = new QPushButton(">");
    QPushButton *refreshButton = new QPushButton("Refresh");
    QPushButton *homeButton = new QPushButton("Home");
    QPushButton *newTabButton = new QPushButton("+");

    addressBar = new QLineEdit();
    addressBar->setPlaceholderText("Enter website address...");

    navigation->addWidget(backButton);
    navigation->addWidget(forwardButton);
    navigation->addWidget(refreshButton);
    navigation->addWidget(homeButton);
    navigation->addWidget(addressBar);
    navigation->addWidget(newTabButton);

    // Browser tabs
    tabs = new QTabWidget();
    tabs->setTabsClosable(true);
    tabs->setMovable(true);

    mainLayout->addLayout(navigation);
    mainLayout->addWidget(tabs);

    setCentralWidget(central);

    // Button connections
    connect(backButton, &QPushButton::clicked,
            this, &OrneredeBrowser::goBack);

    connect(forwardButton, &QPushButton::clicked,
            this, &OrneredeBrowser::goForward);

    connect(refreshButton, &QPushButton::clicked,
            this, &OrneredeBrowser::refreshPage);

    connect(homeButton, &QPushButton::clicked,
            this, &OrneredeBrowser::goHome);

    connect(newTabButton, &QPushButton::clicked,
            this, &OrneredeBrowser::addNewTab);

    connect(addressBar, &QLineEdit::returnPressed,
            this, &OrneredeBrowser::navigateToAddress);

    connect(tabs, &QTabWidget::tabCloseRequested,
            this, [this](int index) {
        if (tabs->count() > 1) {
            tabs->removeTab(index);
        }
    });

    connect(tabs, &QTabWidget::currentChanged,
            this, [this](int index) {
        if (index < 0)
            return;

        QTextBrowser *viewer =
            qobject_cast<QTextBrowser *>(tabs->widget(index));

        if (viewer)
            addressBar->setText(
                viewer->property("pageUrl").toString()
            );
    });

    addNewTab();
}

// Create a new tab
void OrneredeBrowser::addNewTab() {

    QTextBrowser *viewer = new QTextBrowser();

    viewer->setOpenExternalLinks(false);
    viewer->setHtml(
        "<h1>Ornerede Browser</h1>"
        "<p>Welcome to Ornerede.</p>"
        "<p>Enter a website address to begin.</p>"
    );

    viewer->setProperty("pageUrl", "");

    int index = tabs->addTab(viewer, "New Tab");
    tabs->setCurrentIndex(index);

    addressBar->clear();
}

// Navigate to an address
void OrneredeBrowser::navigateToAddress() {

    QString address = addressBar->text().trimmed();

    if (address.isEmpty())
        return;

    if (!address.contains("://") && !address.contains(" ")) {
        address = "https://" + address;
    }

    QUrl url(address);

    if (!url.isValid() || url.host().isEmpty()) {
        QMessageBox::warning(
            this,
            "Invalid address",
            "Please enter a valid website address."
        );
        return;
    }

    loadPage(url);
}

// Request a webpage
void OrneredeBrowser::loadPage(const QUrl &url) {

    QNetworkRequest request(url);

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "OrneredeBrowser/0.1"
    );

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
            this, [this, reply]() {
        displayPage(reply);
    });
}

// Display returned webpage content
void OrneredeBrowser::displayPage(QNetworkReply *reply) {

    if (reply->error() != QNetworkReply::NoError) {

        QString error = reply->errorString();

        if (auto *viewer =
                qobject_cast<QTextBrowser *>(tabs->currentWidget())) {
            viewer->setPlainText(
                "Unable to load this page.\n\n" + error
            );
        }

        reply->deleteLater();
        return;
    }

    QByteArray content = reply->readAll();

    QString html = QString::fromUtf8(content);

    QTextBrowser *viewer =
        qobject_cast<QTextBrowser *>(tabs->currentWidget());

    if (viewer) {

        viewer->setHtml(html);

        viewer->setProperty(
            "pageUrl",
            reply->url().toString()
        );

        tabs->setTabText(
            tabs->currentIndex(),
            reply->url().host()
        );

        addressBar->setText(
            reply->url().toString()
        );
    }

    reply->deleteLater();
}

// Back navigation
void OrneredeBrowser::goBack() {

    QTextBrowser *viewer =
        qobject_cast<QTextBrowser *>(tabs->currentWidget());

    if (viewer)
        viewer->backward();
}

// Forward navigation
void OrneredeBrowser::goForward() {

    QTextBrowser *viewer =
        qobject_cast<QTextBrowser *>(tabs->currentWidget());

    if (viewer)
        viewer->forward();
}

// Refresh current page
void OrneredeBrowser::refreshPage() {

    navigateToAddress();
}

// Home page
void OrneredeBrowser::goHome() {

    QTextBrowser *viewer =
        qobject_cast<QTextBrowser *>(tabs->currentWidget());

    if (viewer) {
        viewer->setHtml(
            "<h1>Ornerede Browser</h1>"
            "<p>Welcome home.</p>"
        );

        viewer->setProperty("pageUrl", "");
    }

    addressBar->clear();
}
