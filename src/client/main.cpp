#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlApplicationEngine>
#include "backend.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCoreApplication::addLibraryPath("./");

    QQmlApplicationEngine engine;
    BackEnd *backend = new BackEnd;
    engine.rootContext()->setContextProperty("backend", backend);
    engine.load(QUrl(QStringLiteral("qrc:/Login.qml")));
    return app.exec();
}
