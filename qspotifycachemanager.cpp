#include "qspotifycachemanager.h"

#include <QtCore/QObject>
#include <QtCore/QEvent>
#include <QtCore/QCoreApplication>
#include <QtCore/QTime>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include <QtCore/QThread>

#include <QtCore/QDebug>

#include <libspotify/api.h>

#include "qspotifytrack.h"
#include "qspotifyplaylist.h"
#include "qspotifyartist.h"
#include "qspotifyalbum.h"

void QSpotifyCacheManager::removeObject(QSpotifyObject *obj)
{
    if (auto track = dynamic_cast<QSpotifyTrack*>(obj)) {
        auto it = m_tracks.find(track->sptrack());
        if (it != m_tracks.end()) m_tracks.erase(it);
    } else if (auto album = dynamic_cast<QSpotifyAlbum*>(obj)) {
        auto it = m_albums.find(album->spalbum());
        if (it != m_albums.end()) m_albums.erase(it);
    } else if (auto artist = dynamic_cast<QSpotifyArtist*>(obj)) {
        auto it = m_artists.find(artist->spartist());
        if (it != m_artists.end()) m_artists.erase(it);
    } else {
        qDebug() << "Try to remove unknown object from cache";
    }
}

QSpotifyTrack *QSpotifyCacheManager::getTrack(sp_track *t, QSpotifyPlaylist *playlist)
{
    Q_ASSERT(t);
    auto iter = m_tracks.find(t);
    if(iter != m_tracks.end()) {
        if(auto ptr = iter.value()) {
            ptr->addRef();
            return ptr;
        }
    }

    auto qtrack = new QSpotifyTrack(t, playlist);
    qtrack->init();

    m_tracks.insert(t, qtrack);

    if(playlist) {
        playlist->registerTrackType(qtrack);
    }
    return qtrack;
}

QSpotifyArtist *QSpotifyCacheManager::getArtist(sp_artist *a)
{
    Q_ASSERT(a);
    if (!a) {
        return nullptr;
    }

    auto iter = m_artists.find(a);
    if(iter != m_artists.end()) {
        if(auto ptr = iter.value()) {
            ptr->addRef();
            return ptr;
        }
    }

    auto artPtr = new QSpotifyArtist(a);
    artPtr->init();

    m_artists.insert(a, artPtr);

    return artPtr;
}

QSpotifyAlbum *QSpotifyCacheManager::getAlbum(sp_album *a)
{
    Q_ASSERT(a);
    if (!a) {
        return nullptr;
    }

    auto iter = m_albums.find(a);
    if(iter != m_albums.end()) {
        if(auto ptr = iter.value()) {
            ptr->addRef();
            return ptr;
        }
    }

    auto albPtr = new QSpotifyAlbum(a);
    albPtr->init();

    m_albums.insert(a, albPtr);

    return albPtr;
}

void QSpotifyCacheManager::clearTables()
{
    for (auto it = m_tracks.begin(); it != m_tracks.end(); ++it)
        it.value()->release();
    m_tracks.clear();
    for (auto it = m_albums.begin(); it != m_albums.end(); ++it)
        it.value()->release();
    m_albums.clear();
    for (auto it = m_artists.begin(); it != m_artists.end(); ++it)
        it.value()->release();
    m_artists.clear();
}

void QSpotifyCacheManager::cacheInfo()
{
    qDebug() << "#Cache Info: Tracks" << m_tracks.size() << "Artists" << m_artists.size() << "Ablums" << m_albums.size();
}
