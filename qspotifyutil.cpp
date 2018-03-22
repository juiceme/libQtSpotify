#include "qspotifyutil.h"

#include <QtCore/QString>
#include <QtCore/QDebug>

#include <libspotify/api.h>

QSpotifyUtil::QSpotifyUtil()
{
}

void QSpotifyUtil::spLinkToQString(sp_link *link, std::function<void(const QString &)> consumer)
{
    const int bufferSize = 200;
    if (link) {
        char buffer[bufferSize];
        int uriSize = sp_link_as_string(link, &buffer[0], bufferSize);
        if(uriSize >= bufferSize) {
            qWarning() << "Link is larger than buffer !!";
        }
        consumer(QString::fromUtf8(&buffer[0], uriSize));
        sp_link_release(link);
    }
}
