/****************************************************************************
**
** Copyright (c) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Yoann Lopes (yoann.lopes@nokia.com)
**
** This file is part of the MeeSpot project.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
**
** Redistributions of source code must retain the above copyright notice,
** this list of conditions and the following disclaimer.
**
** Redistributions in binary form must reproduce the above copyright
** notice, this list of conditions and the following disclaimer in the
** documentation and/or other materials provided with the distribution.
**
** Neither the name of Nokia Corporation and its Subsidiary(-ies) nor the names of its
** contributors may be used to endorse or promote products derived from
** this software without specific prior written permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
** FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
** TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
** PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
** LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
** NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
** SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
****************************************************************************/


#include "qspotifytracklist.h"

#include "qspotifysession.h"
#include "qspotifyplayqueue.h"

QSpotifyTrackList::QSpotifyTrackList(QObject *parent)
    : ListModelBase<QSpotifyTrack>(parent)
{
    m_roles[NameRole] = "trackName";
    m_roles[ArtistsRole] = "artists";
    m_roles[AlbumRole] = "album";
    m_roles[AlbumCoverRole] = "albumCoverId";
    m_roles[DiscNumberRole] = "discNumber";
    m_roles[DurationRole] = "trackDuration";
    m_roles[DurationMsRole] = "durationMs";
    m_roles[ErrorRole] = "error";
    m_roles[DiscIndexRole] = "discIndex";
    m_roles[IsAvailableRole] = "isAvailable";
    m_roles[IsStarredRole] = "isStarred";
    m_roles[PopularityRole] = "popularity";
    m_roles[IsCurrentPlayingTrackRole] = "isCurrentPlayingTrack";
    m_roles[SeenRole] = "seen";
    m_roles[CreatorRole] = "creator";
    m_roles[CreationDateRole] = "creationDate";
    m_roles[AlbumObjectRole] = "albumObject";
    m_roles[ArtistObjectRole] = "artistObject";
    m_roles[OfflineStatusRole] = "offlineStatus";
    m_roles[RawPtrRole] = "rawPtr";
}

QVariant QSpotifyTrackList::data(const QModelIndex &index, int role) const
{
    if(index.row() < 0 || index.row() >= m_dataList.size())
        return QVariant();
    auto track = m_dataList.at(index.row());
    switch(role) {
    case NameRole:
        return track->name();
    case ArtistsRole:
        return track->artists();
    case AlbumRole:
        return track->album();
    case AlbumCoverRole:
        return track->albumCoverId();
    case DiscNumberRole:
        return track->discNumber();
    case DurationRole:
        return track->durationString();
    case DurationMsRole:
        return track->duration();
    case ErrorRole:
        return track->error();
    case DiscIndexRole:
        return track->discIndex();
    case IsAvailableRole:
        return track->isAvailable();
    case IsStarredRole:
        return track->isStarred();
    case PopularityRole:
        return track->popularity();
    case IsCurrentPlayingTrackRole:
        return track->isCurrentPlayingTrack();
    case SeenRole:
        return track->seen();
    case CreatorRole:
        return track->creator();
    case CreationDateRole:
        return track->creationDate();
        // TODO
    case AlbumObjectRole:
        return QVariant();
    case ArtistObjectRole:
        return QVariant();
    case OfflineStatusRole:
        return QVariant();
    case RawPtrRole:
        return QVariant::fromValue<QSpotifyTrack *>(track);
    default:
        return QVariant();
    }
}

void QSpotifyTrackList::play()
{
    if (count() == 0) return;
    playTrack(nextAvailable(-1));
}

void QSpotifyTrackList::playTrack(int index)
{
    if(at(index)->isCurrentPlayingTrack()) {
        // just adapt the tracklist (it might be that we have now a diffrent tracklist)
        QSpotifySession::instance()->m_playQueue->adaptTrackList(this);
    } else {
        QSpotifySession::instance()->m_playQueue->playTrack(this, index);
    }
}

int QSpotifyTrackList::removeAll(QSpotifyTrack *ptr)
{
    int count = 0;
    beginResetModel();
    count = m_dataList.removeAll(ptr);
    endResetModel();
    return count;   
}

int QSpotifyTrackList::totalDuration() const
{
    return std::accumulate(begin(), end(), 0,
                           [](int total, QSpotifyTrack *t){ return total + t->duration();});
}

int QSpotifyTrackList::nextAvailable(int i)
{
    do { ++i; } while (i < count() && !at(i)->isAvailable());
    return i;
}

int QSpotifyTrackList::previousAvailable(int i)
{
    do { --i; } while (i > -1 && !at(i)->isAvailable());
    return i;
}
