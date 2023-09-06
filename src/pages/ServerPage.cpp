#include "ServerPage.h"
#include "BasePage.h"
#include "ClientProfile.h"
#include "PageManager.h"
#include "ServerStruct.h"
#include "gdkmm/screen.h"
#include "glibmm/main.h"
#include "glibmm/spawn.h"
#include "glibmm/threads.h"
#include "gtkmm/application.h"
#include "gtkmm/box.h"
#include "gtkmm/cssprovider.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/listboxrow.h"
#include "gtkmm/object.h"
#include "json-glib/json-glib.h"
#include "libsoup/soup-message.h"
#include "libsoup/soup-session.h"
#include "pango/pango-context.h"
#include "pangomm/context.h"
#include "pangomm/layout.h"
#include "sigc++/functors/mem_fun.h"
#include <bits/chrono.h>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <fmt/core.h>
#include <future>
#include <iostream>
#include <memory>
#include <string>

// TODO: convert this to how ClientProfile handles m_Cancelled and both back buttons

ServerPage::ServerPage(PageManager *pageManager) : BasePage("servers", pageManager)
{
    // TODO: maybe move these to reset, but remembering even across resets
    // what was selected is nice
    // TODO: change all these const char * shit ass to std::string
    bool saveSettings = m_PageManager->m_Settings->m_Data.saveServerSettings;

    m_SelectedServerIndex =
        saveSettings ? m_PageManager->m_Settings->m_Data.serverSettings.selectedAddress.c_str() : "";
    m_SelectedClientIndex = saveSettings ? m_PageManager->m_Settings->m_Data.serverSettings.selectedUser.c_str() : "";
    m_Sort                = saveSettings ? m_PageManager->m_Settings->m_Data.serverSettings.sortType : SortType::NONE;

    printf("sort: '%s'\n", std::to_string(m_Sort).c_str());
    printf("selected s: '%s'\n", m_SelectedServerIndex);
    printf("selected c: '%s'\n", m_SelectedClientIndex);
    printf("query: %s\n", m_PageManager->m_Settings->m_Data.serverSettings.serverQuery.c_str());

    if (saveSettings)
        m_SearchQuery.set_text(m_PageManager->m_Settings->m_Data.serverSettings.serverQuery.c_str());
    Reset();

    m_Box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_ServerPageContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_Box.pack_start(m_ServerPageContainer, Gtk::PACK_EXPAND_WIDGET);
    SetupLoading();
    SetupMain();

    m_Box.show();
}

void ServerPage::ShowServers(const char *query, ServerPage::SortType sort)
{
    m_Sort = sort;
    m_SearchQuery.set_text(query);

    m_PageManager->ChangePage("servers");
}

void ServerPage::ClearServerList()
{
    m_SelectedServerPtr = nullptr;
    m_ServerList.foreach ([&](Gtk::Widget &child) { m_ServerList.remove(child); });
}

void ServerPage::ClearServerClientList()
{
    m_SelectedClientPtr = nullptr;
    m_SelectedServerClientsList.foreach ([&](Gtk::Widget &child) { m_SelectedServerClientsList.remove(child); });
}

void ServerPage::Reset()
{

    m_FirstOpened = true;

    if (m_ViewClientConnection.connected())
    {
        m_ViewClientConnection.disconnect();
    }

    if (m_ConnectServerConnection.connected())
    {
        m_ViewClientConnection.disconnect();
    }

    ClearServerList();
    ClearServerClientList();

    for (int i = 0; i < m_Servers.size(); i++)
    {
        for (int c = 0; c < m_Servers[i]->info->clients.size(); c++)
        {
            delete m_Servers[i]->info->clients[c];
        }
        m_Servers[i]->info->clients.clear();
        delete m_Servers[i];
    }

    printf("freed servers\n");

    m_Servers.clear();

    m_CancelRequested = false;
    m_Msg             = nullptr;

    m_ServerPageContainer.remove(m_ServerOuterContainer);
    m_ServerPageContainer.pack_start(m_LoadingContainer, Gtk::PACK_EXPAND_WIDGET);
    m_LoadingText.set_label("Fetching Server Data...");

    m_SelectedServerName.set_label(AdjustTextFit("Selected: None", 233));
}

// This may not be the best way, but the inbuilt methods were aids to use??
// Also this is not that much slower so its fine for now
std::string ServerPage::AdjustTextFit(const char *initial_text, int size)
{
    auto        layout = Pango::Layout::create(get_pango_context());
    std::string text   = initial_text;
    layout->set_text(text);

    int width, height;
    layout->get_pixel_size(width, height);

    if (width > size)
    {
        while (width > size - 3 && text.size() > 1)
        {
            text.pop_back();
            layout->set_text(text);
            layout->get_pixel_size(width, height);
        }
        text += "...";
    }
    else if (width < size)
    {
        while (width < size)
        {
            text += " ";
            layout->set_text(text);
            layout->get_pixel_size(width, height);
        }

        text.resize(text.size() - (width - size));
    }

    return text;
}

void ServerPage::ServerListSelect(Gtk::ListBoxRow *r)
{
    // Select is triggered on reset, im not quite sure why but it does
    // Same with ClientListSelect
    if (r == nullptr)
        return;

    ServerPageBoxRow *row    = dynamic_cast<ServerPageBoxRow *>(r);
    Server           *server = ServerFromRow(row);

    if (row == nullptr || server == nullptr ||
        (strcmp(m_SelectedServerIndex, server->addresses[0]) == 0) && !m_FirstOpened)
        return;

    m_FirstOpened = false;

    m_SelectedServerPtr   = server;
    m_SelectedClientIndex = "";
    m_SelectedClientPtr   = nullptr;

    if (strcmp(m_SelectedServerIndex, server->addresses[0]) != 0)
        m_SelectedServerIndex = "";

    m_SelectedServerIndex = server->addresses[0];

    // TODO: add an overload to allow it take in a std::string as well
    m_SelectedServerName.set_label(
        AdjustTextFit(fmt::format("Selected: {}", m_SelectedServerPtr->info->name).c_str(), 233));

    ClearServerClientList();

    std::vector<Client *> clients = m_SelectedServerPtr->info->clients;

    for (int i = 0; i < clients.size(); i++)
    {
        ServerPageBoxRow *row   = Gtk::make_managed<ServerPageBoxRow>();
        Gtk::Label       *label = Gtk::make_managed<Gtk::Label>();

        row->index = i;

        clients[i]->row = row;

        label->set_xalign(0.0);
        label->set_label(fmt::format("{} ({})", clients[i]->name, clients[i]->clan).c_str());
        row->add(*label);

        row->show_all();

        m_SelectedServerClientsList.append(*row);
    }
}

void ServerPage::ClientListSelect(Gtk::ListBoxRow *r)
{
    if (r == nullptr)
        return;

    ServerPageBoxRow *row    = dynamic_cast<ServerPageBoxRow *>(r);
    Client           *client = ClientFromRow(row);

    if (row == nullptr || client == nullptr || m_SelectedClientIndex == client->name)
        return;

    m_SelectedClientPtr   = client;
    m_SelectedClientIndex = client->name;
}

void ServerPage::SetupMain()
{

    /* Title */

    m_TitleBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    m_TitleBox.set_valign(Gtk::ALIGN_START);

    auto titleFont = m_Title.get_pango_context()->get_font_description();
    titleFont.set_size(20 * PANGO_SCALE);

    m_Title.set_label("Server List");
    m_Title.override_font(titleFont);
    m_Title.set_halign(Gtk::ALIGN_CENTER);

    /* Back */

    auto backFont = m_Back.get_pango_context()->get_font_description();
    backFont.set_size(10 * PANGO_SCALE);

    m_Back.set_label("<");
    m_Back.override_font(backFont);
    m_Back.set_halign(Gtk::ALIGN_START);
    m_Back.signal_clicked().connect(sigc::mem_fun(this, &ServerPage::BackClicked));

    /* Title Separator */

    m_TitleSeparator.set_size_request(60, 1);
    m_TitleSeparator.set_halign(Gtk::ALIGN_END);
    m_TitleSeparator.set_opacity(0);

    /* Title Box */

    m_TitleBox.pack_start(m_Back, Gtk::PACK_SHRINK);
    m_TitleBox.pack_start(m_Title, Gtk::PACK_EXPAND_WIDGET);
    m_TitleBox.pack_start(m_TitleSeparator, Gtk::PACK_SHRINK);

    m_TitleBox.set_valign(Gtk::ALIGN_START);

    m_ServerOuterContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_ServerInnerContainer.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    /* Selected Server */

    m_SelectedServer.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_SelectedServerName.set_label(AdjustTextFit("Selected: None", 233));

    m_SelectedServer.pack_start(m_SelectedServerName, Gtk::PACK_SHRINK);

    m_SelectedServerClientsScrolled.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_SelectedServerClientsScrolled.add(m_SelectedServerClientsList);

    m_SelectedServer.pack_start(m_SelectedServerClientsScrolled, Gtk::PACK_EXPAND_WIDGET);

    m_SelectedClientView.set_label("View Selected User");
    m_SelectedClientView.signal_clicked().connect(sigc::mem_fun(*this, &ServerPage::ViewClientClicked));
    m_SelectedServer.pack_start(m_SelectedClientView, Gtk::PACK_SHRINK);

    m_SelectedServer.show_all();

    /* Scrollable Server List */

    m_ScrollableServerList.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

    m_ServerList.signal_row_selected().connect(sigc::mem_fun(*this, &ServerPage::ServerListSelect));
    m_ServerList.set_filter_func(sigc::mem_fun(*this, &ServerPage::FilterServerList));
    m_ServerList.set_sort_func(sigc::mem_fun(*this, &ServerPage::SortServerList));

    m_SelectedServerClientsList.signal_row_selected().connect(sigc::mem_fun(*this, &ServerPage::ClientListSelect));

    m_ScrollableServerList.add(m_ServerList);

    m_ServerInnerContainer.pack_start(m_ScrollableServerList, Gtk::PACK_EXPAND_WIDGET);
    m_ServerInnerContainer.pack_start(m_SelectedServer, Gtk::PACK_SHRINK);

    /* Server Settings */

    m_ServerSettingsContainer.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    m_SearchQuery.signal_changed().connect([&] { m_ServerList.invalidate_filter(); });
    m_ToggleSort.set_label(fmt::format("Sort: {}", m_Sort == SortType::NONE                ? "None"
                                                   : m_Sort == SortType::PLAYERS_ASCENDING ? "Ascending"
                                                                                           : "Descending"));
    m_ServerConnect.set_label("Connect To Server");
    m_ServerConnect.signal_clicked().connect(sigc::mem_fun(*this, &ServerPage::ConnectServerClicked));

    m_ToggleSort.signal_clicked().connect([&] {
        switch (m_Sort)
        {
        case SortType::NONE: {
            m_Sort = SortType::PLAYERS_ASCENDING;
            m_ToggleSort.set_label("Sort: Ascending");
            break;
        }
        case SortType::PLAYERS_ASCENDING: {
            m_Sort = SortType::PLAYERS_DESCENDING;
            m_ToggleSort.set_label("Sort: Descending");
            break;
        }
        case SortType::PLAYERS_DESCENDING: {
            m_Sort = SortType::NONE;
            m_ToggleSort.set_label("Sort: None");
            break;
        }
        }

        m_ServerList.invalidate_sort();
    });

    m_ServerSettingsContainer.pack_start(m_SearchQuery, Gtk::PACK_SHRINK);
    m_ServerSettingsContainer.pack_start(m_ToggleSort, Gtk::PACK_SHRINK);
    m_ServerSettingsContainer.pack_start(m_ServerConnect, Gtk::PACK_SHRINK);

    /* Packing */

    m_ServerOuterContainer.pack_start(m_TitleBox, Gtk::PACK_SHRINK);
    m_ServerOuterContainer.pack_start(m_ServerInnerContainer, Gtk::PACK_EXPAND_WIDGET);
    m_ServerOuterContainer.pack_start(m_ServerSettingsContainer, Gtk::PACK_SHRINK);

    m_ServerOuterContainer.show_all();
}

void ServerPage::SetupLoading()
{
    m_LoadingContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_LoadingContainerInner.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_LoadingContainerInner.set_spacing(30);

    m_LoadingContainer.set_halign(Gtk::ALIGN_CENTER);
    m_LoadingContainer.set_valign(Gtk::ALIGN_CENTER);
    m_LoadingContainer.pack_start(m_LoadingContainerInner, Gtk::PACK_EXPAND_WIDGET);

    m_LoadingText.set_label("Fetching Server Data...");

    auto loadingFont = m_LoadingText.get_pango_context()->get_font_description();
    loadingFont.set_size(20 * PANGO_SCALE);

    m_LoadingText.override_font(loadingFont);

    m_LoadingContainerInner.pack_start(m_LoadingText, Gtk::PACK_SHRINK);

    m_LoadingContainer.pack_start(m_LoadingSpinner, Gtk::PACK_SHRINK);

    m_BackLoading.set_label("Go Back");

    auto backLoadingFont = m_BackLoading.get_pango_context()->get_font_description();
    backLoadingFont.set_size(10 * PANGO_SCALE);

    m_BackLoading.override_font(backLoadingFont);

    m_BackLoading.set_halign(Gtk::ALIGN_CENTER);
    m_BackLoading.set_size_request(120, 40);

    m_BackLoading.signal_clicked().connect(sigc::mem_fun(*this, &ServerPage::BackLoadingClicked));

    m_LoadingContainerInner.pack_start(m_BackLoading, Gtk::PACK_SHRINK);

    m_LoadingContainer.show_all();
    m_ServerPageContainer.pack_start(m_LoadingContainer, Gtk::PACK_EXPAND_WIDGET);
    m_ServerPageContainer.show();
}

bool ServerPage::FilterServerList(Gtk::ListBoxRow *row)
{

    ServerPageBoxRow *casted = dynamic_cast<ServerPageBoxRow *>(row);

    if (casted == nullptr)
        return false;

    return casted->serverNameLabel->get_text().find(m_SearchQuery.get_text()) != std::string::npos ||
           casted->mapNameLabel->get_text().find(m_SearchQuery.get_text()) != std::string::npos;
}

bool ServerPage::SortServerList(Gtk::ListBoxRow *row1, Gtk::ListBoxRow *row2)
{

    if (row1 == nullptr || row2 == nullptr)
        return false;

    Server *s1 = ServerFromRow(row1);
    Server *s2 = ServerFromRow(row2);

    if (s1 == nullptr || s2 == nullptr)
        return false;

    switch (m_Sort)
    {
    case SortType::NONE:
        return false;
    case SortType::PLAYERS_ASCENDING:
        return s1->info->clients.size() > s2->info->clients.size();
    case SortType::PLAYERS_DESCENDING:
        return s2->info->clients.size() > s1->info->clients.size();
    }
}

void ServerPage::ConnectServerClicked()
{
    if (m_SelectedServerPtr == nullptr)
    {

        if (m_ConnectServerConnection.connected())
            m_ConnectServerConnection.disconnect();

        m_ServerConnect.set_label("No Server Selected");

        m_ConnectServerConnection = Glib::signal_timeout().connect(
            [&]() {
                m_ServerConnect.set_label("Connect To Server");
                return false;
            },
            1000);

        return;
    }

    Glib::Threads::Thread::create([=]() {
        const char *address = m_SelectedServerPtr->addresses[0];

        std::string command = fmt::format("ez st \"connect {}\"", address);

        const char *comm = command.c_str();

        printf("Connecting to: %s\n", comm);

        system(comm);
    });
}

void ServerPage::ViewClientClicked()
{
    if (m_SelectedClientPtr == nullptr)
    {
        if (m_ViewClientConnection.connected())
            m_ViewClientConnection.disconnect();

        m_SelectedClientView.set_label("No User Selected");

        m_ViewClientConnection = Glib::signal_timeout().connect(
            [&]() {
                m_SelectedClientView.set_label("View Selected User");
                return false;
            },
            1000);

        return;
    };

    ClientProfile *page = m_PageManager->GetCastedPage<ClientProfile *>("client-profile");

    if (page)
        page->ShowProfile(m_SelectedClientPtr->name, m_SelectedServerPtr);
}

// This will attempt to abort the sent request as well as change page
void ServerPage::BackLoadingClicked()
{
    Reset();

    if (m_Msg != nullptr)
    {
        m_CancelRequested = true;
    }

    m_LoadingSpinner.stop();

    m_PageManager->ChangePage(GetRefererName("main"));
}

void ServerPage::BackClicked()
{
    Reset();

    m_PageManager->ChangePage(GetRefererName("main"));
}

void ServerPage::FinishedLoading(std::vector<Server *> servers, gpointer calleeClass)
{
    printf("finished loading?\n");

    ServerPage *p = static_cast<ServerPage *>(calleeClass);

    if (p->m_CancelRequested)
    {
        for (Server *s : servers)
        {
            delete s;
        }
        return;
    }

    p->m_Servers = servers;
    p->PopulateMain();
}

void ServerPage::PopulateMain()
{

    for (int i = 0; i < m_Servers.size(); i++)
    {
        Server *curr = m_Servers[i];

        if (curr == nullptr)
            continue;

        ServerPageBoxRow *row = Gtk::make_managed<ServerPageBoxRow>();
        Gtk::Box         *box = Gtk::make_managed<Gtk::Box>();
        box->set_orientation(Gtk::ORIENTATION_HORIZONTAL);
        box->show();

        Gtk::Label *serverNameLabel = Gtk::make_managed<Gtk::Label>(AdjustTextFit(curr->info->name, 233));
        serverNameLabel->set_xalign(0.0);

        box->pack_start(*serverNameLabel, Gtk::PACK_EXPAND_WIDGET);
        row->serverNameLabel = serverNameLabel;

        Gtk::Label *mapNameLabel = Gtk::make_managed<Gtk::Label>(AdjustTextFit(curr->info->map.name, 100));
        mapNameLabel->set_xalign(0.0);

        box->pack_start(*mapNameLabel, Gtk::PACK_EXPAND_WIDGET);
        row->mapNameLabel = mapNameLabel;

        Gtk::Label *playerCountLabel = Gtk::make_managed<Gtk::Label>(
            fmt::format("{}/{}", curr->info->clients.size(), curr->info->max_players).c_str());
        playerCountLabel->set_xalign(0.0);

        box->pack_start(*playerCountLabel, Gtk::PACK_EXPAND_WIDGET);

        row->index = i;

        curr->row = row;

        row->add(*box);
        row->show_all();

        m_ServerList.append(*row);
    }

    m_ServerPageContainer.remove(m_LoadingContainer);
    m_ServerPageContainer.pack_start(m_ServerOuterContainer, Gtk::PACK_EXPAND_WIDGET);
    m_ServerPageContainer.show();

    printf("populated servers\n");
    m_LoadingSpinner.stop();

    if (strcmp(m_SelectedServerIndex, "") != 0)
    {
        auto toSelect = ServerRowFromAddress(m_SelectedServerIndex);

        if (toSelect == nullptr)
            return;
        m_ServerList.select_row(*toSelect);

        printf("Selected server\n");

        if (strcmp(m_SelectedClientIndex, "") != 0)
        {
            auto clientToSelect = ClientRowFromName(m_SelectedClientIndex);

            printf("Got client to select\n");

            if (clientToSelect == nullptr)
                return;

            m_SelectedServerClientsList.select_row(*clientToSelect);

            printf("selected client\n");
        }
    }
}

ServerPageBoxRow *ServerPage::ServerRowFromAddress(const char *address)
{
    for (int i = 0; i < m_Servers.size(); i++)
    {
        if (m_Servers[i] == nullptr)
            continue;

        if (strcmp(m_Servers[i]->addresses[0], address) == 0)
        {
            return m_Servers[i]->row;
        }
    }

    return nullptr;
}

ServerPageBoxRow *ServerPage::ClientRowFromName(const char *name)
{
    std::vector<Client *> clients = m_SelectedServerPtr->info->clients;

    for (int i = 0; i < clients.size(); i++)
    {
        if (clients[i] == nullptr)
            continue;

        if (strcmp(clients[i]->name, name) == 0)
        {
            return clients[i]->row;
        }
    }

    return nullptr;
}

// These will attempt to use their index first, but fallback on a loop
// If the index is incorrect for whatever reason
Client *ServerPage::ClientFromRow(Gtk::ListBoxRow *row)
{
    if (m_SelectedServerPtr == nullptr)
        return nullptr;

    ServerPageBoxRow *casted = dynamic_cast<ServerPageBoxRow *>(row);

    if (casted != nullptr && casted->index < m_SelectedServerPtr->info->clients.size() &&
        m_SelectedServerPtr->info->clients[casted->index]->row == row)
    {
        return m_SelectedServerPtr->info->clients[casted->index];
    }

    for (int i = 0; i < m_SelectedServerPtr->info->clients.size(); i++)
    {
        Client *curr = m_SelectedServerPtr->info->clients[i];

        if (curr != nullptr && curr->row == row)
            return curr;
    }

    return nullptr;
}

Server *ServerPage::ServerFromRow(Gtk::ListBoxRow *row)
{

    if (row == nullptr)
        return nullptr;

    ServerPageBoxRow *casted = dynamic_cast<ServerPageBoxRow *>(row);

    if (casted != nullptr && casted->index < m_Servers.size() && m_Servers[casted->index]->row == row)
    {
        return m_Servers[casted->index];
    }

    for (int i = 0; i < m_Servers.size(); i++)
    {
        Server *curr = m_Servers[i];

        if (curr != nullptr && curr->row == row)
            return curr;
    }

    return nullptr;
}

bool ServerPage::ShowPage(Gtk::Window *window)
{

    window->add(m_Box);
    m_Box.show();

    if (!m_Servers.empty())
        return true;

    m_LoadingSpinner.start();
    printf("Showing %s\n", m_Name);

    m_PageManager->m_HttpManager->GetServers(m_PageManager->m_Soup, &ServerPage::FinishedLoading, this);

    return true;
}
