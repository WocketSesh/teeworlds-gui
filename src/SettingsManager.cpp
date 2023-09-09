#include "SettingsManager.h"
#include "ServerStruct.h"
#include <cstddef>
#include <cstring>
#include <filesystem>
#include <fmt/core.h>
#include <fstream>
#include <iterator>
#include <sstream>

SettingsManager::SettingsManager()
{
}

void SettingsManager::DefaultSettings()
{
    m_Data.serverSettings.sortType        = Server::NONE;
    m_Data.serverSettings.serverQuery     = "";
    m_Data.serverSettings.selectedUser    = "";
    m_Data.serverSettings.selectedAddress = "";

    m_Data.saveServerSettings = false;
}

bool SettingsManager::IsFavourite(std::string address)
{
    for (std::string a : m_Data.serverSettings.favouriteServers)
    {
        if (a == address)
            return true;
    }

    return false;
}

bool SettingsManager::AddFavourite(std::string address)
{
    if (IsFavourite(address))
        return false;

    m_Data.serverSettings.favouriteServers.push_back(address);

    return true;
}

// Maybe make IsFavourite return index like IsFriend to make this easier?
bool SettingsManager::RemoveFavourite(std::string address)
{
    if (!IsFavourite(address))
        return false;

    for (int i = 0; i < m_Data.serverSettings.favouriteServers.size(); i++)
    {
        if (m_Data.serverSettings.favouriteServers[i] == address)
        {
            m_Data.serverSettings.favouriteServers.erase(m_Data.serverSettings.favouriteServers.begin() + i);
        }
    }

    return true;
}

void SettingsManager::Init(std::string filePath)
{

    DefaultSettings();

    std::filesystem::path path = std::filesystem::path(filePath);

    std::ofstream create(filePath, std::ofstream::app);

    if (!create.is_open())
    {
        printf("Error opening file at %s\n", filePath.c_str());
        return;
    }
    else
        create.close();

    m_Path = filePath;

    std::ifstream file(m_Path);

    if (!file.is_open())
    {
        printf("Error opening file for read at %s\n", m_Path.c_str());
        return;
    }
    else
        printf("Opened file for read at %s\n", m_Path.c_str());

    std::string line;

    // TODO: surely theres a better way to handle this shit

    while (std::getline(file, line))
    {
        if (line.substr(0, strlen("friend")) == "friend")
        {
            m_Data.friends.push_back(line.substr(strlen("friend") + 1, line.length() - 1).c_str());
        }
        else if (line.substr(0, strlen("save_server_settings")) == "save_server_settings")
        {
            m_Data.saveServerSettings = std::stoi(line.substr(strlen("save_server_settings") + 1, line.length() - 1));
        }
        else if (line.substr(0, strlen("server_query")) == "server_query")
        {
            m_Data.serverSettings.serverQuery = line.substr(strlen("server_query") + 1, line.length() - 1);
        }
        else if (line.substr(0, strlen("favourite_server")) == "favourite_server")
        {
            m_Data.serverSettings.favouriteServers.push_back(
                line.substr(strlen("favourite_server") + 1, line.length() - 1));
        }
    }

    file.close();
}

bool SettingsManager::WriteFile()
{
    // Maybe open it with ::app, but that means id have to check whether if a friend
    // is already in the file or not, so overwriting the whole file is prob easier
    std::ofstream file(m_Path);

    if (!file.is_open())
    {
        printf("Cannot write file: %s\n", m_Path.c_str());
        return false;
    }

    std::string content;

    for (std::string f : m_Data.friends)
    {
        content.append("friend " + f + "\n");
    }

    content.append("save_server_settings " + std::to_string(m_Data.saveServerSettings) + "\n");
    content.append("selected_server " + m_Data.serverSettings.selectedAddress + "\n");
    content.append("selected_user " + m_Data.serverSettings.selectedUser + "\n");
    content.append("server_query " + m_Data.serverSettings.serverQuery + "\n");
    content.append("server_sort " + std::to_string(m_Data.serverSettings.sortType) + "\n");

    for (std::string f : m_Data.serverSettings.favouriteServers)
    {
        content.append("favourite_server " + f + "\n");
    }

    file << content << std::endl;

    file.close();

    return false;
}

int SettingsManager::IsFriend(std::string name)
{
    for (int i = 0; i < m_Data.friends.size(); i++)
    {
        if (m_Data.friends[i] == name)
            return i;
    }

    return -1;
}

bool SettingsManager::RemoveFriend(std::string name)
{
    int ind = IsFriend(name);

    if (ind == -1)
        return false;

    m_Data.friends.erase(m_Data.friends.begin() + ind);

    return true;
}

bool SettingsManager::AddFriend(std::string name)
{
    if (IsFriend(name) > -1)
        return false;

    m_Data.friends.push_back(name);
    return true;
}
