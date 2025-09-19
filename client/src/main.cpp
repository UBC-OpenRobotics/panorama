#include <QApplication>
#include "client/mainwindow.hpp"
#include "client/argparser.hpp"
#include <QTimer>

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    ArgParser parser(app.arguments());

    MainWindow w;
    w.show();

    if (parser.isTestMode()) {
        QTimer::singleShot(3000, &app, &QCoreApplication::quit);
    }

    return app.exec();
}
