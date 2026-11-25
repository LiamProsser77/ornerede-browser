#include "browser.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

OrneredeBrowser::OrneredeBrowser(QWidget *parent)
    : QMainWindow(parent) {

    setWindowTitle("Ornerede Browser");
    resize(1200, 800);

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
    addressBar->setPlaceholderText("Search or enter address...");

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

    addNewTab();
}

void OrneredeBrowser::addNewTab() {
    QWidget *page = new QWidget();
    QVBoxLayout *layout = new QVBoxLayout(page);

    QLabel *content = new QLabel(
        "Ornerede Browser\n\n"
        "Powered by Mozilla Gecko\n"
        "Welcome to Ornerede."
    );

    content->setObjectName("pageContent");
    content->setAlignment(Qt::AlignCenter);

    layout->addStretch();
    layout->addWidget(content);
    layout->addStretch();

    int index = tabs->addTab(page, "New Tab");
    tabs->setCurrentIndex(index);

    addressBar->clear();
}

void OrneredeBrowser::navigateToAddress() {
    QString address = addressBar->text().trimmed();

    if (address.isEmpty())
        return;

    if (!address.contains("://") && !address.contains(" ")) {
        address = "https://" + address;
    }

    QWidget *page = tabs->currentWidget();

    if (page) {
        QLabel *content = page->findChild<QLabel *>("pageContent");

        if (content) {
            content->setText(
                "Requested address:\n" + address +
                "\n\nGecko rendering is not connected yet."
            );
        }
    }

    addressBar->setText(address);
}

void OrneredeBrowser::goBack() {
    QWidget *page = tabs->currentWidget();

    if (page) {
        QLabel *content = page->findChild<QLabel *>("pageContent");
        if (content)
            content->setText("Back navigation will use Gecko.");
    }
}

void OrneredeBrowser::goForward() {
    QWidget *page = tabs->currentWidget();

    if (page) {
        QLabel *content = page->findChild<QLabel *>("pageContent");
        if (content)
            content->setText("Forward navigation will use Gecko.");
    }
}

void OrneredeBrowser::refreshPage() {
    QWidget *page = tabs->currentWidget();

    if (page) {
        QLabel *content = page->findChild<QLabel *>("pageContent");
        if (content)
            content->setText("Refresh will use Gecko.");
    }
}

void OrneredeBrowser::goHome() {
    addressBar->clear();

    QWidget *page = tabs->currentWidget();

    if (page) {
        QLabel *content = page->findChild<QLabel *>("pageContent");
        if (content)
            content->setText("Ornerede Browser\n\nWelcome home.");
    }
}
