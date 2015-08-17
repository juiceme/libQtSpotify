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


#include "qspotifyplayqueue.h"

#include <QtCore/QDebug>

#include "qspotifysession.h"
#include "qspotifytracklist.h"

QSpotifyPlayQueue::QSpotifyPlayQueue(QObject *parent)
    : QSpotifyTrackList(parent)
{}

void QSpotifyPlayQueue::playTrack(QSpotifyTrackList *list, int index)
{
    adaptTrackList(list);

    playTrackAt(index);

    if (m_shuffle) setShuffle(m_shuffle, true);
}

void QSpotifyPlayQueue::adaptTrackList(QSpotifyTrackList *list)
{
    qDebug() << "QSpotifyPlayQueue::adaptTrackList" << list << "m_sourceTrackList" << m_sourceTrackList;
    if (!list) return;
    if (m_sourceTrackList != list) {
        clearQueue();

        m_sourceTrackList = list;

        int size = list->count();
        m_originalIndexes.reserve(size);
        m_initialTracks.reserve(size);

        int index = 0;
        for (int i = 0; i < size; ++i) {
            auto t = list->at(i);
            if (t->isAvailable()) {
                m_originalIndexes.push_back(index++);
                t->addRef();
                m_initialTracks.push_back(t);
            }
        }

        appendRows(m_initialTracks);

        qDebug() << "Added playlist: " << count();
    }
}

void QSpotifyPlayQueue::enqueueTrack(QSpotifyTrack *track)
{
    qDebug() << "QSpotifyPlayQueue::enqueueTrack" << track;
    if (track) {
        track->addRef();
        int insertIndex = 0;
        if (m_currentTrack && (m_explicitTracks.empty() || m_currentTrack != m_explicitTracks.front()))
            ++insertIndex;
        insertIndex += m_explicitTracks.size();

        m_explicitTracks.enqueue(track);

        insertRow(insertIndex, track);
    }
}

void QSpotifyPlayQueue::enqueueTracks(QSpotifyTrackList *list)
{
    if (list) {
        int insertIndex = 0;
        if (m_currentTrack && (m_explicitTracks.empty() || m_currentTrack != m_explicitTracks.front()))
            ++insertIndex;
        insertIndex += m_explicitTracks.size();

        for (auto t : *list) {
            t->addRef();
            m_explicitTracks.enqueue(t);
            insertRow(insertIndex++, t);
        }
    }
}

bool QSpotifyPlayQueue::playTrackAt(int i)
{
    qDebug() << "playTrackAtIndex" << i;
    if (m_currentTrack) {
        m_currentTrack->release();
        m_currentTrack = nullptr;
    }

    if (i < 0 || i >= count()) {
        m_currentIndex = 0;
        emit currentPlayIndexChanged();
        return false;
    }

    while (i > 0) {
        auto t = takeRow(0);
        if (!m_explicitTracks.empty() && t == m_explicitTracks.front()) {
            t->release();
            m_explicitTracks.takeFirst();
        }
        --i;
    }

    m_currentTrack = at(0);
    m_currentTrack->addRef();
//    if (!m_explicitTracks.empty() && m_currentTrack == m_explicitTracks.front())
//        m_explicitTracks.takeFirst();
//    else
//        Q_ASSERT(!m_explicitTracks.contains(m_currentTrack)); // TODO fix this
    m_currentIndex = 0;
    emit currentPlayIndexChanged();
    playCurrentTrack();
    return true;
}

void QSpotifyPlayQueue::playCurrentTrack()
{
    qDebug() << "playCurrentTrack";
    if (!m_currentTrack)
        return;

    if (m_currentTrack->isLoaded())
        onTrackReady();
    else
        connect(m_currentTrack, SIGNAL(isLoadedChanged()), this, SLOT(onTrackReady()));
}

void QSpotifyPlayQueue::shuffleInitialTracks()
{
    // Fisher Yates algo
    //    To shuffle an array a of n elements (indices 0..n-1):
    //      for i from n − 1 downto 1 do
    //           j ← random integer with 0 ≤ j ≤ i
    //           exchange a[j] and a[i]
    qsrand(QTime::currentTime().msec());

    int size = m_initialTracks.size();

    for (int i = size - 1; i > 0; --i) {
        int  j = qrand() % i;
        if (i != j) {
            std::swap(m_originalIndexes[i], m_originalIndexes[j]);
            std::swap(m_initialTracks[i], m_initialTracks[j]);
        }
    }
}

void QSpotifyPlayQueue::clear()
{
    qDebug() << "QSpotifyPlayQueue::clear()";
    clearQueue();
}

void QSpotifyPlayQueue::playNext(bool repeatOne)
{
    if (repeatOne) {
        QSpotifySession::instance()->play(m_currentTrack, true);
    } else {
        if (1 < count()) {
            playTrackAt(1);
        } else {
            if (m_repeat) {
                takeRow(0);
                appendRows(m_initialTracks);
                playTrackAt(0);
            } else {
                QSpotifySession::instance()->stop();
                clearQueue();
            }
        }
    }
}

void QSpotifyPlayQueue::playPrevious()
{
    int currentIndex = m_initialTracks.indexOf(m_currentTrack);
    if (currentIndex - 1 >= 0) {
        auto trackToAdd = m_initialTracks.at(currentIndex - 1);
        int index = indexOf(trackToAdd);
        if (index > - 1 && index < count()) takeRow(index);
        insertRow(0, trackToAdd);
        playTrackAt(0);
    } else {
        if (m_repeat) {
            auto trackToAdd = m_initialTracks.at(m_initialTracks.size()-1);
            int index = indexOf(trackToAdd);
            if (index > - 1 && index < count()) takeRow(index);
            insertRow(0, trackToAdd);
            playTrackAt(0);
        } else {
            QSpotifySession::instance()->stop();
            clearQueue();
        }
    }
}

void QSpotifyPlayQueue::clearQueue()
{
    qDebug() << "QSpotifyPlayQueue::clearQueue()";
    if (m_dataList.size()) {
        beginRemoveRows(QModelIndex(),0, m_dataList.size()-1);
        for (auto t : m_dataList)
            disconnect(t, &QSpotifyObject::dataChanged, this, &QSpotifyPlayQueue::itemDataChanged);
        m_dataList.clear();
        endRemoveRows();
    }

    if (m_currentTrack) {
        m_currentTrack->release();
        m_currentTrack = nullptr;
        m_currentIndex = 0;
    }

    for (auto t : m_initialTracks)
        t->release();
    m_initialTracks.clear();
    m_originalIndexes.clear();
    m_sourceTrackList = nullptr;

    while (!m_explicitTracks.empty())
        m_explicitTracks.takeFirst()->release();
}

void QSpotifyPlayQueue::setShuffle(bool s, bool force)
{
    if (!force && s == m_shuffle) return;
    qDebug() << "QSpotifyPlayQueue::setShuffle" << s;

    m_shuffle = s;
    if (s) {
        shuffleInitialTracks();
        int currentIndex = m_initialTracks.indexOf(m_currentTrack);
        if (currentIndex >= 0) {
            int insertIndex = 1 + m_explicitTracks.size();
            while (count() > insertIndex) takeRow(insertIndex);
            for (int i = currentIndex + 1; i < m_initialTracks.size(); ++i)
                appendRow(m_initialTracks.at(i));
        } else {
            // Current track is explicit.
            int insertIndex = 1 + m_explicitTracks.size();
            if (insertIndex < count()) {
                auto track = at(insertIndex);
                int initialIndex = m_initialTracks.indexOf(track);
                while (count() > insertIndex) takeRow(insertIndex);
                for (int i = initialIndex; i < m_initialTracks.size(); ++i)
                    appendRow(m_initialTracks.at(i));
            }

        }
    } else {
        int size = m_initialTracks.size();
        // TODO fix for explicit
        int trackIndex = m_initialTracks.indexOf(m_currentTrack);
        trackIndex = m_originalIndexes[trackIndex];

        for (int i = 0; i < size; ++i) {
            int origIndex = m_originalIndexes[i];
            while (origIndex != i) {
                // we place the current item at its correct position.
                std::swap(m_initialTracks[i], m_initialTracks[origIndex]);
                // now we want to find the correct one for our location.
                int nextIndex = m_originalIndexes[origIndex];
                m_originalIndexes[origIndex] = origIndex;
                origIndex = nextIndex;
            }
        }
        for (int i = 0; i < m_originalIndexes.size(); ++i) m_originalIndexes[i] = i;

        // TODO fix for explicit
        int insertIndex = 1 + m_explicitTracks.size();
        while (count() > insertIndex) takeRow(insertIndex);
        for (int i = trackIndex + 1; i < m_initialTracks.size(); ++i)
            appendRow(m_initialTracks.at(i));
    }
}

void QSpotifyPlayQueue::setRepeat(bool r)
{
    if (m_repeat == r) return;

    m_repeat = r;
}

void QSpotifyPlayQueue::onTrackReady()
{
    qDebug() << "QSpotifyPlayQueue::onTrackReady";
    disconnect(this, SLOT(onTrackReady()));
    if (m_currentTrack) QSpotifySession::instance()->play(m_currentTrack);
}

bool QSpotifyPlayQueue::isCurrentTrackList(QSpotifyTrackList *tl)
{
    return m_sourceTrackList == tl;
}

bool QSpotifyPlayQueue::isExplicitTrack(int index)
{
    return index >= 0 && index < count() && m_explicitTracks.contains(at(index));
}

void QSpotifyPlayQueue::onOfflineModeChanged()
{
    qDebug() << "NYI";
//    if (m_shuffle && m_implicitTracks)
//        m_implicitTracks->setShuffle(true);
    //    emit tracksChanged();
}
