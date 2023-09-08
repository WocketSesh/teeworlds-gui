#include "ServerStruct.h"
#include "PageManager.h"

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

    for (Client *c : info->clients)
    {
        if (settings.IsFriend(c->name) > -1)
            m_FriendCount++;
    }
}

// TODO: add a filter type for every gamemode? like fng2/ctf
void Server::CalculateFilterType(SettingsManager *settings)
{

    if (settings != nullptr)
    {
        CalculateFriendCount(*settings);
        m_Favourite = settings->IsFavourite(addresses.at(0));
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
    if (info->game_type == "DDraceNetwork")
        filterType |= Server::DDRACE;
    else if (info->game_type == "Gores")
        filterType |= Server::GORES;
    else
        filterType |= Server::OTHER;

    m_FilterType = filterType;
}
