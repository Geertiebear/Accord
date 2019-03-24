#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#include <QQmlContext>
#include <QQuickWindow>
#include <QQmlApplicationEngine>
#include "backend.h"
#include "communityimageprovider.h"

int main(int argc, char **argv)
{
    QApplication app(argc, argv);

    QApplication::addLibraryPath("./");
    QFont font("DIN Alternate", 10, QFont::Bold);
    app.setFont(font);

    QQmlApplicationEngine engine;
    BackEnd *backend = new BackEnd(engine.rootContext());
    CommunityImageProvider *communityImageProvider = new CommunityImageProvider();
    engine.rootContext()->setContextProperty("backend", backend);
    engine.rootContext()->setContextProperty("communitiesList", &backend->communitiesList);
    engine.rootContext()->setContextProperty("channelsMap", backend->channelsMap);
    engine.rootContext()->setContextProperty("messagesMap", backend->messagesMap);
    engine.rootContext()->setContextProperty("userMap", backend->userMap);
    engine.rootContext()->setContextProperty("screenGeometry", QApplication::desktop()->availableGeometry());
    engine.addImageProvider("communityImageProvider", communityImageProvider);
    qmlRegisterType<CommunitiesTable>("accord.types", 1, 0, "CommuntiesTable");
    qmlRegisterType<MessagesTable>("accord.types", 1, 0, "MessagesTable");
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    QObject *top = engine.rootObjects().first();
    QObject::connect(backend, SIGNAL(authenticated()), top, SLOT(onAuthenticated()));
    QObject::connect(backend, SIGNAL(failedAuthenticated()), top, SLOT(onFailedAuthenticated()));
    QObject::connect(backend, SIGNAL(failedRegistered()), top, SLOT(onFailedRegistered()));
    QObject::connect(backend, SIGNAL(alreadyInCommunity()), top, SLOT(onAlreadyInCommunity()));
    QObject::connect(backend, SIGNAL(communityReady(QVariant)), top, SLOT(onCommunityReady(QVariant)));
    QObject::connect(backend, SIGNAL(communityProfilepic(quint64, QByteArray)),
                     communityImageProvider, SLOT(onCommunityProfilepic(quint64,
                                                                        QByteArray)));
    QObject::connect(backend, SIGNAL(inviteReady(QVariant, QVariant)), top, SLOT(onInviteReady(
                                                                                  QVariant, QVariant)));
    return app.exec();
}
