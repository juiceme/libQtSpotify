#ifndef QSPOTIFYCACHEMANAGER_H
#define QSPOTIFYCACHEMANAGER_H

#include <QtCore/QHash>

class QSpotifyTrack;
class QSpotifyPlaylist;
class QSpotifyArtist;
class QSpotifyAlbum;

struct sp_track;
struct sp_artist;
struct sp_album;


class QSpotifyObject;

class QSpotifyCacheManager
{
public:
    static QSpotifyCacheManager& instance();

    void removeObject(QSpotifyObject *obj);
    QSpotifyTrack *getTrack(sp_track *t, QSpotifyPlaylist *playlist = nullptr);
    QSpotifyArtist *getArtist(sp_artist *a);
    QSpotifyAlbum *getAlbum(sp_album *a);

    void cacheInfo();

private:
    QSpotifyCacheManager() = default;

    int numTracks();
    int numAlbums();
    int numArtists();

    QHash<sp_track *, QSpotifyTrack *> m_tracks;
    QHash<sp_artist *, QSpotifyArtist *> m_artists;
    QHash<sp_album *, QSpotifyAlbum *> m_albums;
};

#endif // QSPOTIFYCACHEMANAGER_H
