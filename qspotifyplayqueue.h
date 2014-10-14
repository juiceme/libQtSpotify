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


#ifndef QSPOTIFYPLAYQUEUE_H
#define QSPOTIFYPLAYQUEUE_H

#include <QtCore/QQueue>

#include "qspotifytracklist.h"

class QSpotifyPlayQueue : public QSpotifyTrackList
{
    Q_OBJECT
    Q_PROPERTY(int currentPlayIndex READ currentPlayIndex NOTIFY currentPlayIndexChanged)
public:
    QSpotifyPlayQueue(QObject *parent = nullptr);

    void playTrack(QSpotifyTrackList *list, int index);
    // if we want to play a track which is playing but we want to set
    // a different tracklist.
    void adaptTrackList(QSpotifyTrackList *list);
    void enqueueTrack(QSpotifyTrack *track);
    void enqueueTracks(QSpotifyTrackList *list);

    Q_INVOKABLE bool playTrackAt(int i);

    virtual void clear() override;

    void playNext(bool repeatOne);
    void playPrevious();

    void clearQueue();

    void setShuffle(bool s, bool force = false);
    void setRepeat(bool r);

    bool isCurrentTrackList(QSpotifyTrackList *tl);

    int currentPlayIndex() const { return m_currentIndex; }

    Q_INVOKABLE bool isExplicitTrack(int index);

Q_SIGNALS:
    void currentPlayIndexChanged();

private Q_SLOTS:
    void onTrackReady();
    void onOfflineModeChanged();

private:
    void playCurrentTrack();

    void shuffleInitialTracks();

    QVector<int> m_originalIndexes;
    QQueue<QSpotifyTrack *> m_explicitTracks;
    QList<QSpotifyTrack *> m_initialTracks;

    // The tracklist from which the current tracks are from, this should NEVER be accessed!
    QSpotifyTrackList* m_sourceTrackList{};

    bool m_repeat{};

    QSpotifyTrack *m_currentTrack{};
    int m_currentIndex{};

    friend class QSpotifyPlaylist;
};

#endif // QSPOTIFYPLAYQUEUE_H
