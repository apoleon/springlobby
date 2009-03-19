/* Copyright (C) 2007 The SpringLobby Team. All rights reserved. */

#include <wx/intl.h>
#include <wx/menu.h>
#include <wx/filename.h>
#include <wx/log.h>

#include "replaylistctrl.h"
#include "replaylist.h"
#include "../utils.h"
#include "../user.h"
#include "../iconimagelist.h"
#include "../uiutils.h"
#include "../ui.h"


BEGIN_EVENT_TABLE(ReplayListCtrl, CustomVirtListCtrl<const Replay*>)

  EVT_LIST_ITEM_RIGHT_CLICK( RLIST_LIST, ReplayListCtrl::OnListRightClick )
  EVT_MENU                 ( RLIST_DLMAP, ReplayListCtrl::OnDLMap )
  EVT_MENU                 ( RLIST_DLMOD, ReplayListCtrl::OnDLMod )
#if wxUSE_TIPWINDOW
#ifndef __WXMSW__ //disables tooltips on win
  EVT_MOTION(ReplayListCtrl::OnMouseMotion)
#endif
#endif
END_EVENT_TABLE()

template<> SortOrder CustomVirtListCtrl<const Replay*>::m_sortorder = SortOrder();

ReplayListCtrl::ReplayListCtrl( wxWindow* parent, ReplayList& replaylist  ):
  CustomVirtListCtrl<const Replay*>(parent, RLIST_LIST, wxDefaultPosition, wxDefaultSize,
                wxSUNKEN_BORDER | wxLC_REPORT | wxLC_SINGLE_SEL | wxLC_ALIGN_LEFT,
                _T("replaylistctrl"), 8, 4, &CompareOneCrit ),
  m_replaylist(replaylist)
{
    const int hd = wxLIST_AUTOSIZE_USEHEADER;
#ifdef __WXMSW__
    const int widths[8] = {80,140,141,hd,160,hd,70,180};
#else
    const int widths[8] = {80,140,141,50,160,50,70,180};

#endif

    AddColumn( 0, widths[0], _("Date"), _("Date of recording") );
    AddColumn( 1, widths[1], _("Mod"), _("Modname") );
    AddColumn( 2, widths[2], _("Map"), _("Mapname") );
    AddColumn( 3, widths[3], _("Players"), _("Number of players") );
    AddColumn( 4, widths[4], _("Duration"), _T("Duration") );
    AddColumn( 5, widths[5], _("Spring Version"), _("Spring Version") );
    AddColumn( 6, widths[6], _("Filesize"), _("Filesize in kilobyte") );
    AddColumn( 7, widths[7], _("File"), _T("Filename") );

    if ( m_sortorder.size() == 0 ) {
      m_sortorder[0].col = 0;
      m_sortorder[0].direction = false;
      m_sortorder[1].col = 1;
      m_sortorder[1].direction = true;
      m_sortorder[2].col = 2;
      m_sortorder[2].direction = true;
      m_sortorder[3].col = 3;
      m_sortorder[3].direction = true;
      Sort( );
    }


    m_popup = new wxMenu( _T("") );
    // &m enables shortcout "alt + m" and underlines m
    m_popup->Append( RLIST_DLMAP, _("Download &map") );
    m_popup->Append( RLIST_DLMOD, _("Download m&od") );
}


ReplayListCtrl::~ReplayListCtrl()
{
  delete m_popup;
}

void ReplayListCtrl::OnListRightClick( wxListEvent& event )
{
  PopupMenu( m_popup );
}

void ReplayListCtrl::AddReplay( const Replay& replay )
{
    if ( GetIndexFromData( &replay ) != -1 ) {
        wxLogWarning( _T("Replay already in list.") );
        return;
    }
    m_data.push_back( &replay );
    SetItemCount( m_data.size() );
    RefreshItem( m_data.size() );
}

void ReplayListCtrl::OnDLMap( wxCommandEvent& event )
{
    if ( m_selected_index > 0 &&  m_data.size() > m_selected_index ) {
        OfflineBattle battle = m_data[m_selected_index]->battle;
        ui().DownloadMap( battle.GetHostMapHash(), battle.GetHostMapName() );
    }
}

void ReplayListCtrl::OnDLMod( wxCommandEvent& event )
{
    if ( m_selected_index > 0 &&  m_data.size() > m_selected_index ) {
        OfflineBattle battle = m_data[m_selected_index]->battle;
        ui().DownloadMod( battle.GetHostModHash(), battle.GetHostModName() );
    }
}


void ReplayListCtrl::SetUnsorted(){
  wxListItem col;
  GetColumn( m_sortorder[0].col, col );
  col.SetImage( icons().ICON_NONE );
  SetColumn( m_sortorder[0].col, col );
}

void ReplayListCtrl::Sort()//needs adjusting when column order etc is stable
{
    if ( m_data.size() > 0 ) {
        SaveSelection();
        SLInsertionSort( m_data, m_comparator );
        RestoreSelection();
    }
}

template<class T>
inline int MyCompare(T a, T b){
  return a<b?-1:(b<a?1:0);
}

int ReplayListCtrl::CompareOneCrit( DataType u1, DataType u2, int col, int dir )
{
    switch ( col ) {
        case 0: return dir * compareSimple( u1->date, u2->date );
        case 1: return dir * compareSimple( u1->battle.GetHostModName(),u2->battle.GetHostModName() );
        case 2: return dir * compareSimple( u1->battle.GetHostMapName(),u2->battle.GetHostMapName() );
        case 3: return dir * compareSimple( u1->battle.GetNumUsers() - u1->battle.GetSpectators(), u2->battle.GetNumUsers() - u2->battle.GetSpectators() );
        case 4: return dir * compareSimple( u1->duration,u2->duration );
        case 5: return dir * compareSimple( u1->SpringVersion, u2->SpringVersion ) ;
        case 6: return dir * compareSimple( u1->size, u2->size ) ;
        case 7: return dir * compareSimple( u1->Filename.AfterLast( wxFileName::GetPathSeparator() ), u2->Filename.AfterLast( wxFileName::GetPathSeparator() ) );
        default: return 0;
    }
}

void ReplayListCtrl::OnMouseMotion(wxMouseEvent& event)
{
#if wxUSE_TIPWINDOW
	wxPoint position = event.GetPosition();

	try{
		m_tiptimer.Start(m_tooltip_delay, wxTIMER_ONE_SHOT);
		int flag = wxLIST_HITTEST_ONITEM;
		long *ptrSubItem = new long;
#ifdef HAVE_WX28
		long item_hit = HitTest(position, flag, ptrSubItem);
#else
		long item_hit = HitTest(position, flag);
#endif

		if (item_hit != wxNOT_FOUND)
		{
			long item = GetItemData(item_hit);

			Replay replay = m_replaylist.GetReplayById(item);
			int coloumn = getColoumnFromPosition(position);
			switch (coloumn)
			{
			case 0: // date
			m_tiptext = replay.date;
				break;
			case 1: // modname
				m_tiptext = replay.ModName;
				break;
			case 2: // mapname
				m_tiptext = replay.MapName;
				break;
			case 3: //playernum
				m_tiptext = RefineModname(replay.ModName);
				break;
			case 4: // spring version
				m_tiptext = replay.SpringVersion;
				break;
			case 5: // filenam
				m_tiptext = replay.Filename;
				break;

			default: m_tiptext = _T("");
				break;
			}
		}
	}
	catch(...){}
#endif
}

/*
wxString duration = wxString::Format(_T("%02ld:%02ld:%02ld"), replay.duration / 3600,
                        (replay.duration%3600)/60, (replay.duration%60)/60 ) ;
    m_replay_listctrl->SetItem( index, 0, replay.date );
    m_replay_listctrl->SetItem( index, 1, replay.battle.GetHostModName() );
    m_replay_listctrl->SetItem( index, 2, replay.battle.GetHostMapName() );
    m_replay_listctrl->SetItem( index, 3, wxString::Format(_T("%d"),replay.battle.GetNumUsers() - replay.battle.GetSpectators() ) );
    m_replay_listctrl->SetItem( index, 4, duration );
    m_replay_listctrl->SetItem( index, 5, replay.SpringVersion );
    m_replay_listctrl->SetItem( index, 6, wxString::Format( _T("%d KB"),replay.size/1024 ) );
    m_replay_listctrl->SetItem( index, 7, replay.Filename.AfterLast( wxFileName::GetPathSeparator() ) );
*/

wxString ReplayListCtrl::OnGetItemText(long item, long column) const
{
    if ( m_data[item] == NULL )
        return wxEmptyString;

    const Replay& replay = *m_data[item];
    wxString duration = wxString::Format(_T("%02ld:%02ld:%02ld"), replay.duration / 3600,
                        (replay.duration%3600)/60, (replay.duration%60)/60 ) ;

    switch ( column ) {
        case 0: return replay.date;
        case 1: return replay.battle.GetHostModName();
        case 2: return replay.battle.GetHostMapName();
        case 3: return wxString::Format(_T("%d"),replay.battle.GetNumUsers() - replay.battle.GetSpectators() );
        case 4: return duration;
        case 5: return replay.SpringVersion;
        case 6: return wxString::Format( _T("%d KB"),replay.size/1024 );
        case 7: return replay.Filename.AfterLast( wxFileName::GetPathSeparator() );

        default: return wxEmptyString;
    }
}

int ReplayListCtrl::OnGetItemImage(long item) const
{
    if ( m_data[item] == NULL )
        return -1;

    return -1;//icons().GetBattleStatusIcon( *m_data[item] );
}

int ReplayListCtrl::OnGetItemColumnImage(long item, long column) const
{
    if ( m_data[item] == NULL )
        return -1;

    const Replay& replay = *m_data[item];

    switch ( column ) {
        default: return -1;

//        case 0: return icons().GetBattleStatusIcon( battle );
//        case 1: return icons().GetFlagIcon( battle.GetFounder().GetCountry() );
//        case 2: return icons().GetRankIcon( battle.GetRankNeeded(), false );
//        case 4: return battle.MapExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
//        case 5: return battle.ModExists() ? icons().ICON_EXISTS : icons().ICON_NEXISTS;
    }
}

wxListItemAttr* ReplayListCtrl::OnGetItemAttr(long item) const
{
    if ( item < m_data.size() && item > -1 ) {
        const Replay& replay = *m_data[item];
//        wxString host = b.GetFounder().GetNick();
//        wxListItemAttr* attr = HighlightItemUser( item, host );
//        if ( attr != NULL )
//            return attr;
//
//        //to avoid color flicker check first if highlighting should be done
//        //and return if it should
//        for ( unsigned int i = 0; i < b.GetNumUsers(); ++i){
//            wxString name = b.GetUser(i).GetNick();
//            attr = HighlightItemUser( item, name );
//            if ( attr != NULL )
//                return attr;
//
//        }
    }
    return NULL;
}


int ReplayListCtrl::GetIndexFromData( const DataType& data ) const
{
    DataCIter it = m_data.begin();
    for ( int i = 0; it != m_data.end(); ++it, ++i ) {
        if ( *it != 0 && data->Equals( *(*it) ) )
            return i;
    }
    wxLogError( _T("didn't find the battle.") );
    return -1;
}
