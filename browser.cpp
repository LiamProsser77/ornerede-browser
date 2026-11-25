#include "browser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QWidget>
#include <QMessageBox>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrlQuery>
#include <QSettings>
#include <QFile>
#include <QInputDialog>
#include <QAction>
#include <QTextDocument>

OrneredeBrowser::OrneredeBrowser(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Ornerede");
    resize(1100, 750);

    networkManager = new QNetworkAccessManager(this);

    QWidget *central = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(central);

    QHBoxLayout *navigation = new QHBoxLayout();

    QPushButton *back = new QPushButton("<");
    QPushButton *forward = new QPushButton(">");
    QPushButton *refresh = new QPushButton("Refresh");
    QPushButton *home = new QPushButton("Home");
    QPushButton *newTab = new QPushButton("+");

    addressBar = new QLineEdit();
    addressBar->setPlaceholderText("Enter website address...");

    navigation->addWidget(back);
    navigation->addWidget(forward);
    navigation->addWidget(refresh);
    navigation->addWidget(home);
    navigation->addWidget(addressBar);
    navigation->addWidget(newTab);

    tabs = new QTabWidget();
    tabs->setTabsClosable(true);
    tabs->setMovable(true);

    layout->addLayout(navigation);
    layout->addWidget(tabs);
    setCentralWidget(central);

    bookmarksMenu = menuBar()->addMenu("Bookmarks");

    bookmarksMenu->addAction(
        "Add Bookmark",
        this,
        &OrneredeBrowser::addBookmark
    );

    bookmarksMenu->addSeparator();
    updateBookmarksMenu();

    connect(back, &QPushButton::clicked, this, [this]() {
        auto *viewer = qobject_cast<QTextBrowser *>(
            tabs->currentWidget()
        );

        if (viewer)
            viewer->backward();
    });

    connect(forward, &QPushButton::clicked, this, [this]() {
        auto *viewer = qobject_cast<QTextBrowser *>(
            tabs->currentWidget()
        );

        if (viewer)
            viewer->forward();
    });

    connect(refresh, &QPushButton::clicked,
            this, &OrneredeBrowser::navigateToAddress);

    connect(home, &QPushButton::clicked, this, [this]() {
        addNewTab();
    });

    connect(newTab, &QPushButton::clicked,
            this, &OrneredeBrowser::addNewTab);

    connect(addressBar, &QLineEdit::returnPressed,
            this, &OrneredeBrowser::navigateToAddress);

    connect(tabs, &QTabWidget::tabCloseRequested,
            this, [this](int index) {
        if (tabs->count() > 1) {
            QWidget *page = tabs->widget(index);
            tabs->removeTab(index);
            page->deleteLater();
        }
    });

    connect(tabs, &QTabWidget::currentChanged,
            this, [this](int index) {
        if (index < 0)
            return;

        auto *viewer = qobject_cast<QTextBrowser *>(
            tabs->widget(index)
        );

        if (viewer)
            addressBar->setText(
                viewer->property("pageUrl").toString()
            );
    });

    addNewTab();
}


// New tab
void OrneredeBrowser::addNewTab() {

    auto *viewer = new QTextBrowser();

    viewer->setOpenLinks(false);
    viewer->setOpenExternalLinks(false);

    QString html =
        "<html><body style='text-align:center;"
        "font-family:sans-serif;padding-top:60px;'>";

    if (QFile::exists("Ornerede.png")) {
        html += "<img src='Ornerede.png' width='180'><br>";
    }

    html +=
        "<h1>Ornerede</h1>"
        "<p>Enter a website address above to begin.</p>"
        "</body></html>";

    viewer->setHtml(html);
    viewer->document()->addResource(
        QTextDocument::ImageResource,
        QUrl("Ornerede.png"),
        QImage("Ornerede.png")
    );

    viewer->setProperty("pageUrl", "");

    connect(viewer, &QTextBrowser::anchorClicked,
            this, [this](const QUrl &url) {
        addressBar->setText(url.toString());
        loadPage(url);
    });

    int index = tabs->addTab(viewer, "New Tab");
    tabs->setCurrentIndex(index);
    addressBar->clear();
}


// Navigate
void OrneredeBrowser::navigateToAddress() {

    QString address = addressBar->text().trimmed();

    if (address.isEmpty())
        return;

    if (!address.contains("://"))
        address = "https://" + address;

    QUrl url(address);

    if (!url.isValid() || url.host().isEmpty()) {
        QMessageBox::warning(
            this,
            "Ornerede",
            "Invalid website address."
        );
        return;
    }

    loadPage(url);
}


// Load through local proxy
void OrneredeBrowser::loadPage(const QUrl &url) {

    auto *viewer = qobject_cast<QTextBrowser *>(
        tabs->currentWidget()
    );

    if (!viewer)
        return;

    QUrl proxyUrl("http://127.0.0.1:8080/fetch");

    QUrlQuery query;
    query.addQueryItem("url", url.toString());
    proxyUrl.setQuery(query);

    QNetworkRequest request(proxyUrl);

    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished,
            this, [this, reply, url, viewer]() {

        displayPage(reply, url);

        reply->deleteLater();
    });

    viewer->setProperty("pageUrl", url.toString());
    addressBar->setText(url.toString());
}


// Display loaded page
void OrneredeBrowser::displayPage(
    QNetworkReply *reply,
    const QUrl &url
) {

    auto *viewer = qobject_cast<QTextBrowser *>(
        reply->property("viewer").value<QWidget *>()
    );

    if (!viewer)
        viewer = qobject_cast<QTextBrowser *>(
            tabs->currentWidget()
        );

    if (!viewer)
        return;

    if (reply->error() != QNetworkReply::NoError) {
        viewer->setPlainText(
            "Unable to load page:\n" +
            reply->errorString()
        );
        return;
    }

    QByteArray data = reply->readAll();

    QString contentType = reply->header(
        QNetworkRequest::ContentTypeHeader
    ).toString();

    if (contentType.contains("html", Qt::CaseInsensitive)) {
        viewer->setHtml(QString::fromUtf8(data));
    } else {
        viewer->setPlainText(QString::fromUtf8(data));
    }

    viewer->setProperty("pageUrl", url.toString());

    int index = tabs->indexOf(viewer);

    if (index >= 0)
        tabs->setTabText(index, url.host());

    if (tabs->currentWidget() == viewer)
        addressBar->setText(url.toString());
}


// Add bookmark
void OrneredeBrowser::addBookmark() {

    QString url = addressBar->text().trimmed();

    if (url.isEmpty())
        return;

    QString name = QInputDialog::getText(
        this,
        "Add Bookmark",
        "Bookmark name:",
        QLineEdit::Normal,
        QUrl(url).host()
    );

    if (name.trimmed().isEmpty())
        return;

    QSettings settings("Ornerede", "OrneredeBrowser");

    int count = settings.beginReadArray("bookmarks");
    settings.endArray();

    settings.beginWriteArray("bookmarks", count + 1);
    settings.setArrayIndex(count);
    settings.setValue("name", name);
    settings.setValue("url", url);
    settings.endArray();

    updateBookmarksMenu();
}


// Update bookmarks menu
void OrneredeBrowser::updateBookmarksMenu() {

    QList<QAction *> actions = bookmarksMenu->actions();

    for (int i = 2; i < actions.size(); ++i) {
        bookmarksMenu->removeAction(actions[i]);
        delete actions[i];
    }

    QSettings settings("Ornerede", "OrneredeBrowser");

    int count = settings.beginReadArray("bookmarks");

    for (int i = 0; i < count; ++i) {
        settings.setArrayIndex(i);

        QString name = settings.value("name").toString();
        QString url = settings.value("url").toString();

        QAction *action = bookmarksMenu->addAction(name);

        connect(action, &QAction::triggered,
                this, [this, url]() {
            openBookmark(url);
        });
    }

    settings.endArray();
}


// Open bookmark
void OrneredeBrowser::openBookmark(const QString &url) {
    addressBar->setText(url);
    navigateToAddress();
}
