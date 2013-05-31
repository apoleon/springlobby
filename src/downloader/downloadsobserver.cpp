#include "downloadsobserver.h"

DownloadsObserver::DownloadsObserver()
{
}

DownloadsObserver::~DownloadsObserver()
{

}
/*
static const DownloadsObserver& DownloadsObserver::Instance()
{
    static DownloadsObserver downloadsObserver;
    return downloadsObserver;
}
*/
void DownloadsObserver::Add(IDownload* dl)
{
    wxMutexLocker lock(mutex);
    m_dl_list.push_back(dl);
}

void DownloadsObserver::Remove(IDownload* dl)
{
    wxMutexLocker lock(mutex);
    m_dl_list.remove(dl);
    ObserverDownloadInfo di=GetInfo(dl);
    di.finished=1;
    finished.push_back(di);
}

void DownloadsObserver::GetList(std::list<ObserverDownloadInfo>& lst)
{
    wxMutexLocker lock(mutex);
    std::list<IDownload*>::iterator it;
    for(it=m_dl_list.begin();it!=m_dl_list.end();++it)
    {
        ObserverDownloadInfo di=GetInfo(*it);
        if(di.size>0)
            lst.push_back(GetInfo(*it));
    }

    lst.splice(lst.begin(),finished);
}
void DownloadsObserver::ClearFinished()
{
    finished.clear();
}

ObserverDownloadInfo DownloadsObserver::GetInfo(IDownload* dl)
{
    ObserverDownloadInfo di;
    di.name=wxString::FromUTF8(dl->name.c_str());
    di.size=dl->size;
    if(di.size>0)
        di.progress=dl->getProgress();
    else
        di.progress=0;

    return di;
}

DownloadsObserver& downloadsObserver()
{
    static DownloadsObserver s_DownloadsObserver;
    return s_DownloadsObserver;
}

/**
    This file is part of SpringLobby,
    Copyright (C) 2007-2010

    SpringLobby is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License version 2 as published by
    the Free Software Foundation.

    springsettings is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with SpringLobby.  If not, see <http://www.gnu.org/licenses/>.
**/

