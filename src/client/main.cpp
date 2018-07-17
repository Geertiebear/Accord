#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include "backend.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCoreApplication::addLibraryPath("./");

    QQmlApplicationEngine engine;
    BackEnd *backend = new BackEnd;
    engine.rootContext()->setContextProperty("backend", backend);
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *top = engine.rootObjects().first();
    QObject::connect(backend, SIGNAL(authenticated()), top, SLOT(onAuthenticated()));
    QObject::connect(backend, SIGNAL(failedAuthenticated()), top, SLOT(onFailedAuthenticated()));
    return app.exec();
}
