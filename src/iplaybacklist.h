/* This file is part of the Springlobby (GPL v2 or later), see COPYING */

#ifndef SL_PLAYBACKLIST_H_INCLUDED
#define SL_PLAYBACKLIST_H_INCLUDED

#include <map>
#include <vector>
#include <wx/event.h>
#include "storedgame.h"

class IPlaybackList : public wxEvtHandler
{
public:
	IPlaybackList()
	    : wxEvtHandler()
	{
	}

	virtual void LoadPlaybacks(const std::vector<std::string>& filenames);

	StoredGame& AddPlayback();
	void RemovePlayback(unsigned int const id);

	StoredGame& GetPlaybackById(unsigned int const id);

	bool PlaybackExists(unsigned int const id) const;
	bool DeletePlayback(unsigned int const id);
	size_t GetNumPlaybacks() const;

	void RemoveAll();

	const std::map<size_t, StoredGame>& GetPlaybacksMap() const;
	virtual bool GetReplayInfos(const std::string& ReplayPath, StoredGame& ret) const = 0;

protected:
	std::map<size_t, StoredGame> m_replays;
private:
	int FindPlayback(const std::string& filename) const;
	bool FindFilename(const std::vector<std::string>& filenames, const std::string& filename) const;

};

#endif // SL_PLAYBACKLIST_H_INCLUDED
