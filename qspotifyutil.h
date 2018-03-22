#ifndef QSPOTIFYUTIL_H
#define QSPOTIFYUTIL_H

#include <functional>

class QString;
struct sp_link;

class QSpotifyUtil
{
public:
    QSpotifyUtil();
    static void spLinkToQString(sp_link *link, std::function<void(const QString&)> consumer);
};

#endif // QSPOTIFYUTIL_H
