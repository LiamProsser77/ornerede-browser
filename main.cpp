#include <QApplication>
#include "browser.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    OrneredeBrowser browser;
    browser.show();

    return app.exec();
}
