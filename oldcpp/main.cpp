#include <QApplication>
#include <QMessageBox>

#include "browser.h"
#include "proxy.h"

int main(int argc, char *argv[]) {

    QApplication app(argc, argv);

    QApplication::setApplicationName("Ornerede");
    QApplication::setOrganizationName("Ornerede");

    // Start local proxy
    ProxyServer proxy;

    if (!proxy.start(8080)) {
        QMessageBox::critical(
            nullptr,
            "Ornerede",
            "Could not start the local proxy."
        );

        return 1;
    }

    // Open browser
    OrneredeBrowser browser;
    browser.show();

    return app.exec();
}
