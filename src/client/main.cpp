#include <QGuiApplication>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include "backend.h"
#include "communityimageprovider.h"

int main(int argc, char **argv)
{
    QGuiApplication app(argc, argv);

    QCoreApplication::addLibraryPath("./");

    QQmlApplicationEngine engine;
    BackEnd *backend = new BackEnd(engine.rootContext());
    CommunityImageProvider *communityImageProvider = new CommunityImageProvider();
    engine.rootContext()->setContextProperty("backend", backend);
    engine.rootContext()->setContextProperty("communitiesList", &backend->communitiesList);
    engine.rootContext()->setContextProperty("channelsMap", backend->channelsMap);
    engine.addImageProvider("communityImageProvider", communityImageProvider);
    qmlRegisterType<CommunitiesTable>("accord.types", 1, 0, "CommuntiesTable");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *top = engine.rootObjects().first();
    QObject::connect(backend, SIGNAL(authenticated()), top, SLOT(onAuthenticated()));
    QObject::connect(backend, SIGNAL(failedAuthenticated()), top, SLOT(onFailedAuthenticated()));
    QObject::connect(backend, SIGNAL(failedRegistered()), top, SLOT(onFailedRegistered()));
    QObject::connect(backend, SIGNAL(communityReady(QVariant)), top, SLOT(onCommunityReady(QVariant)));
    QObject::connect(backend, SIGNAL(communityProfilepic(quint64, QByteArray)), communityImageProvider, SLOT(onCommunityProfilepic(quint64, QByteArray)));
    return app.exec();
}
