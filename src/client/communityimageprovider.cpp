#include "communityimageprovider.h"
#include <QDebug>

CommunityImageProvider::CommunityImageProvider()
    : QQuickImageProvider(QQuickImageProvider::Image)
{

}

QImage CommunityImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    quint64 intId = id.toULongLong();
    if (size)
        *size = QSize(200, 200);
    QByteArray data = communityProfilePictures.value(intId);
    QImage image =  QImage::fromData(data);
    return image;
}

void CommunityImageProvider::onCommunityProfilepic(quint64 id, QByteArray data)
{
    communityProfilePictures.insert(id, data);
}
