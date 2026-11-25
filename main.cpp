#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTabWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>

class OrneredeBrowser : public QMainWindow {
public:
    OrneredeBrowser(QWidget *parent = nullptr)
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

        addressBar = new QLineEdit();
        addressBar->setPlaceholderText("Enter a website address...");

        navigation->addWidget(backButton);
        navigation->addWidget(forwardButton);
        navigation->addWidget(refreshButton);
        navigation->addWidget(homeButton);
        navigation->addWidget(addressBar);

        // Browser tabs
        tabs = new QTabWidget();
        tabs->setTabsClosable(true);
        tabs->setMovable(true);

        QPushButton *newTabButton = new QPushButton("+");

        QWidget *tabBarWidget = new QWidget();
        QHBoxLayout *tabBarLayout = new QHBoxLayout(tabBarWidget);
        tabBarLayout->setContentsMargins(0, 0, 0, 0);

        tabBarLayout->addWidget(tabs);
        tabBarLayout->addWidget(newTabButton);

        mainLayout->addLayout(navigation);
        mainLayout->addWidget(tabBarWidget);

        setCentralWidget(central);

        // Create first tab
        addNewTab();

        // Navigation actions
        connect(backButton, &QPushButton::clicked, this, [this]() {
            showMessage("Back navigation will be connected to Gecko.");
        });

        connect(forwardButton, &QPushButton::clicked, this, [this]() {
            showMessage("Forward navigation will be connected to Gecko.");
        });

        connect(refreshButton, &QPushButton::clicked, this, [this]() {
            showMessage("Page refresh will be connected to Gecko.");
        });

        connect(homeButton, &QPushButton::clicked, this, [this]() {
            addressBar->clear();
            showMessage("Ornerede Browser home page");
        });

        connect(addressBar, &QLineEdit::returnPressed,
                this, [this]() {
            QString address = addressBar->text().trimmed();

            if (!address.isEmpty()) {
                showMessage(
                    "Ready to load: " + address +
                    "\nGecko integration is not connected yet."
                );
            }
        });

        // New tab
        connect(newTabButton, &QPushButton::clicked,
                this, [this]() {
            addNewTab();
        });

        // Close tabs
        connect(tabs, &QTabWidget::tabCloseRequested,
                this, [this](int index) {
            if (tabs->count() > 1) {
                tabs->removeTab(index);
            }
        });

        // Update address bar when changing tabs
        connect(tabs, &QTabWidget::currentChanged,
                this, [this](int) {
            addressBar->clear();
        });
    }

private:
    QTabWidget *tabs;
    QLineEdit *addressBar;

    void addNewTab() {
        QWidget *page = new QWidget();
        QVBoxLayout *layout = new QVBoxLayout(page);

        QLabel *title = new QLabel("Ornerede Browser");
        title->setAlignment(Qt::AlignCenter);

        QLabel *description = new QLabel(
            "Your browser is starting.\n\n"
            "Powered by Mozilla Gecko (planned).\n"
            "Webpage rendering is not connected yet."
        );

        description->setAlignment(Qt::AlignCenter);

        layout->addStretch();
        layout->addWidget(title);
        layout->addWidget(description);
        layout->addStretch();

        int index = tabs->addTab(page, "New Tab");
        tabs->setCurrentIndex(index);
    }

    void showMessage(const QString &message) {
        QWidget *page = tabs->currentWidget();

        if (!page)
            return;

        QLabel *description = page->findChild<QLabel *>();

        if (description)
            description->setText(message);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    OrneredeBrowser browser;
    browser.show();

    return app.exec();
}
