#include "ServerStruct.h"
#include "BasePage.h"
#include "PageManager.h"
#include "pangomm/layout.h"
#include <cstdio>
#include <fmt/core.h>

ServerInfo::ServerInfo()
{
}
Server::Server()
{
}

// Ideally this would be done in HttpManager json parsing of the server struct
// But that does not have access to the settings/PageManager, and would be aids to give it access to it
void Server::CalculateFriendCount(SettingsManager &settings)
{
    m_FriendCount = 0;

    for (Client *c : m_Info->clients)
    {
        if (settings.IsFriend(c->name) > -1)
            m_FriendCount++;
    }
}

void Server::SetupText(BasePage &page)
{
    row->serverNameLabel->set_text(page.AdjustTextFit(m_Info->name, 233));
    row->mapNameLabel->set_text(page.AdjustTextFit(m_Info->map.name, 100));
    row->playerCountLabel->set_text(
        page.AdjustTextFit(fmt::format("{}/{}", m_Info->clients.size(), m_Info->max_players).c_str(), 50).c_str());

    row->glyphLabel->set_text(
        page.AdjustTextFit(fmt::format("{} {}", m_Favourite ? "★" : " ",
                                       m_FriendCount > 0 ? "♥" + std::to_string(m_FriendCount) : "")
                               .c_str(),
                           50)
            .c_str());
}

// return casted->serverNameLabel->get_text().find(m_SearchQuery.get_text()) != std::string::npos ||
//        casted->mapNameLabel->get_text().find(m_SearchQuery.get_text()) != std::string::npos;
bool Server::ShouldShow(std::string strFilter, ServerFilterTypes filterType)
{
    bool inName = row->serverNameLabel->get_text().find(strFilter) != std::string::npos;
    bool inMap  = row->mapNameLabel->get_text().find(strFilter) != std::string::npos;

    bool correctFilterType = m_FilterType & filterType;

    bool inClientNames = false;
    bool inClanNames   = false;

    for (Client *c : m_Info->clients)
    {
        if (std::string(c->name).find(strFilter) != std::string::npos)
        {
            inClientNames = true;
            break;
        }
        if (std::string(c->clan).find(strFilter) != std::string::npos)
        {
            inClanNames = true;
            break;
        }
    }

    return correctFilterType && (inName || inMap || inClientNames || inClanNames);
}

// TODO: add a filter type for every gamemode? like fng2/ctf
void Server::CalculateFilterType(SettingsManager *settings)
{

    if (settings != nullptr)
    {
        CalculateFriendCount(*settings);
        m_Favourite = settings->IsFavourite(m_Addresses.at(0));
    }
    else
    {
        m_FriendCount = 0;
        m_Favourite   = false;
    }

    int filterType = Server::ALL;

    if (m_Favourite)
        filterType |= Server::FAVOURITE;
    if (m_FriendCount > 0)
        filterType |= Server::FRIENDS;
    if (m_Info->game_type == "DDraceNetwork")
        filterType |= Server::DDRACE;
    else if (m_Info->game_type == "Gores")
        filterType |= Server::GORES;
    else
        filterType |= Server::OTHER;

    m_FilterType = filterType;
}
