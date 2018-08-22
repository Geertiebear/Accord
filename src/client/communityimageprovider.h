#ifndef COMMUNITYIMAGEPROVIDER_H
#define COMMUNITYIMAGEPROVIDER_H

#include <QObject>
#include <QQuickImageProvider>
#include <QString>
#include <QSize>
#include <QMap>
#include <QByteArray>

class CommunityImageProvider : public QObject, public QQuickImageProvider
{
    Q_OBJECT
public:
    CommunityImageProvider();

    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
signals:

public slots:
    void onCommunityProfilepic(quint64 id, QByteArray data);

private:
    QMap<quint64, QByteArray> communityProfilePictures;
};

#endif // COMMUNITYIMAGEPROVIDER_H
