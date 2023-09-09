#ifndef SERVER_PAGE_H
#define SERVER_PAGE_H

#include "BasePage.h"
#include "ServerStruct.h"
#include "gtkmm/box.h"
#include "gtkmm/button.h"
#include "gtkmm/entry.h"
#include "gtkmm/label.h"
#include "gtkmm/listbox.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/scrollable.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/separator.h"
#include "gtkmm/spinner.h"
#include "json-glib/json-glib.h"
#include "libsoup/soup-types.h"
#include "libsoup/soup.h"
#include "pango/pango-types.h"
#include "pangomm/fontdescription.h"
#include "sigc++/connection.h"
#include <atomic>
#include <memory>
#include <vector>

class ServerPage : public BasePage
{

  public:
    ServerPage(PageManager *pageManager);

    void ShowServers(const char *query = "", Server::SortType sort = Server::SortType::NONE);

  private:
    Server::SortType m_Sort;

    SoupMessage *m_Msg;
    bool         m_CancelRequested;
    bool         m_FirstOpened;
    const char  *m_SelectedServerIndex; // Not actually index but address, only consistent data
    const char  *m_SelectedClientIndex; // Not actually index but just user name

    Gtk::Box m_ServerPageContainer; // VERT

    // Initial page (retrieving server data)
    Gtk::Box     m_LoadingContainer;      // VERT
    Gtk::Box     m_LoadingContainerInner; // VERT
    Gtk::Label   m_LoadingText;
    Gtk::Button  m_BackLoading;
    Gtk::Spinner m_LoadingSpinner;

    // Main Page (displaying server data)

    Gtk::Box       m_TitleBox; // HORIZ
    Gtk::Label     m_Title;
    Gtk::Separator m_TitleSeparator;
    Gtk::Button    m_Back;

    Gtk::Box            m_ServerOuterContainer;          // VERT
    Gtk::Box            m_ServerInnerContainer;          // HORIZ PARENT:OUTER
    Gtk::ScrolledWindow m_ScrollableServerList;          // PARENT:INNER
    Gtk::ListBox        m_ServerList;                    // PARENT:SCROLLED
    Gtk::Box            m_SelectedServer;                // VERT PARENT:INNER
    Gtk::Label          m_SelectedServerName;            // PARENT:SELECTED
    Gtk::ScrolledWindow m_SelectedServerClientsScrolled; // PARENT:SELECTED
    Gtk::ListBox        m_SelectedServerClientsList;     // PARENT:SELECTED
    Gtk::Button         m_SelectedClientView;            // PARENT:SELECTED
    Gtk::Button         m_FavouriteServer;               // PARENT:SELECTED
    Gtk::Box            m_ServerSettingsContainer;       // HORIZ PARENT:OUTER
    Gtk::Entry          m_SearchQuery;                   // PARENT:SETTINGS
    Gtk::Button         m_ToggleSort;                    // PARENT:SETTINGS
    Gtk::Button         m_ServerConnect;                 // PARENT:SETTINGS

    std::vector<Server *> m_Servers;
    Server               *m_SelectedServerPtr;
    Client               *m_SelectedClientPtr;

    ServerPageBoxRow *ServerRowFromAddress(const char *address);
    ServerPageBoxRow *ClientRowFromName(const char *name);
    Server           *ServerFromRow(Gtk::ListBoxRow *row);
    Client           *ClientFromRow(Gtk::ListBoxRow *row);

    void BackLoadingClicked();
    void BackClicked();

    sigc::connection m_ViewClientConnection;
    void             ViewClientClicked();

    sigc::connection m_ConnectServerConnection;
    void             ConnectServerClicked();

    void ServerListSelect(Gtk::ListBoxRow *row);
    void ClientListSelect(Gtk::ListBoxRow *row);
    bool FilterServerList(Gtk::ListBoxRow *row);
    bool SortServerList(Gtk::ListBoxRow *row1, Gtk::ListBoxRow *row2);

    // Setup methods
    void Reset();

    bool        ShowPage(Gtk::Window *window) override;
    void        SetupLoading();
    void        SetupMain();
    void        PopulateMain();
    void        ClearServerList();
    void        ClearServerClientList();
    static void FinishedLoading(std::vector<Server *>, gpointer calleeClass);
};

#endif // SERVER_PAGE_H
