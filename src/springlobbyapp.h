/*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


//
// Class: SpringLobbyApp
// Created on: Fri May 11 19:19:23 2007
//

#ifndef _SPRINGLOBBYAPP_H_
#define _SPRINGLOBBYAPP_H_

#include <wx/wx.h>
#include "settings.h"

class MainWindow;
class ConnectWindow;
  
//! @brief Spring lobby wxApp, this class connects all the other classes
class SpringLobbyApp : public wxApp
{
  public:
    
    // SpringLobbyApp interface
    
    virtual bool OnInit();
    virtual bool OnQuit();
  
    Settings& Sett();
  
    void OpenMainWindow();
    void OpenConnectWindow();
  
    void Quit();
  
  protected:
    // SpringLobbyApp variables
    
    Settings m_settings;
    MainWindow* m_main_win;
    ConnectWindow* m_con_win;
  
};



DECLARE_APP(SpringLobbyApp)

#endif  //_SPRINGLOBBYAPP_H_

