#include "browser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QUrlQuery>
#include <QTextBrowser>

OrneredeBrowser::OrneredeBrowser(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Ornerede Browser");
    resize(1100, 750);

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
    addressBar->setPlaceholderText("Enter a website address...");

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

    // Close tabs
    connect(tabs, &QTabWidget::tabCloseRequested,
            this, [this](int index) {
        if (tabs->count() > 1) {
            tabs->removeTab(index);
        }
    });

    // Update address bar when changing tabs
    connect(tabs, &QTabWidget::currentChanged,
            this, [this](int index) {

        if (index < 0)
            return;

        QTextBrowser *viewer =
            qobject_cast<QTextBrowser *>(tabs->widget(index));

        if (viewer) {
            addressBar->setText(
                viewer->property("pageUrl").toString()
            );
        }
    });

    addNewTab();
}


// Create a new tab
void OrneredeBrowser::addNewTab() {

    QTextBrowser *viewer = new QTextBrowser();

    viewer->setOpenLinks(false);
    viewer->setOpenExternalLinks(false);

    viewer->setHtml(
        "<h1>Ornerede Browser</h1>"
        "<p>Welcome to Ornerede.</p>"
        "<p>Enter a website address to begin.</p>"
    );

    viewer->setProperty("pageUrl", "");

    // Handle clicked links
    connect(viewer, &QTextBrowser::anchorClicked,
            this, [this](const QUrl &url) {
        addressBar->setText(url.toString());
        loadPage(url);
    });

    int index = tabs->addTab(viewer, "New Tab");

    tabs->setCurrentIndex(index);
    addressBar->clear();
}


// Navigate to an address
void OrneredeBrowser::navigateToAddress() {

    QString address = addressBar->text().trimmed();

    if (address.isEmpty())
        return;

    if (!address.contains("://") &&
        !address.contains(" ")) {

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


// Load a page through the local proxy
void OrneredeBrowser::loadPage(const QUrl &url) {

    QUrl proxyUrl("http://127.0.0.1:8080/fetch");

    QUrlQuery query;
    query.addQueryItem(
        "url",
        url.toString(QUrl::FullyEncoded)
    );

    proxyUrl.setQuery(query);

    QNetworkRequest request(proxyUrl);

    request.setHeader(
        QNetworkRequest::UserAgentHeader,
        "OrneredeBrowser/0.1"
    );

    QNetworkReply *reply =
        networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
            this, [this, reply, url]() {
        displayPage(reply, url);
    });

    addressBar->setText(url.toString());
}


// Display returned webpage content
void OrneredeBrowser::displayPage(
    QNetworkReply *reply,
    const QUrl &originalUrl
) {

    QTextBrowser *viewer =
        qobject_cast<QTextBrowser *>(tabs->currentWidget());

    if (!viewer) {
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {

        viewer->setPlainText(
            "Unable to load this page.\n\n" +
            reply->errorString()
        );

        reply->deleteLater();
        return;
    }

    QByteArray content = reply->readAll();

    QString contentType =
        reply->header(
            QNetworkRequest::ContentTypeHeader
        ).toString();

    if (contentType.contains("text/html",
                             Qt::CaseInsensitive)) {

        viewer->setHtml(QString::fromUtf8(content));

    } else if (contentType.contains("text/plain",
                                    Qt::CaseInsensitive)) {

        viewer->setPlainText(QString::fromUtf8(content));

    } else {

        viewer->setPlainText(
            "This page returned a file type that "
            "Ornerede cannot display yet.\n\n"
            "Content type: " + contentType
        );
    }

    viewer->setProperty(
        "pageUrl",
        originalUrl.toString()
    );

    tabs->setTabText(
        tabs->currentIndex(),
        originalUrl.host()
    );

    addressBar->setText(originalUrl.toString());

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
            "<p>Enter a website address to begin.</p>"
        );

        viewer->setProperty("pageUrl", "");
    }

    addressBar->clear();
}
