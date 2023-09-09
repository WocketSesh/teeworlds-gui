#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "ServerStruct.h"
#include <string>
#include <vector>
class SettingsManager
{
  public:
    SettingsManager();
    void Init(std::string filePath);
    bool AddFriend(std::string name);
    bool RemoveFriend(std::string name);
    bool IsFavourite(std::string address);
    bool AddFavourite(std::string address);
    bool RemoveFavourite(std::string address);
    bool WriteFile();
    int  IsFriend(std::string name); // Returns the index of friend in  the vector, -1 means not friend

    struct Data
    {
        std::vector<std::string> friends;
        bool                     saveServerSettings;
        struct ServerSettings
        {
            std::vector<std::string> favouriteServers;
            std::string              selectedAddress;
            std::string              selectedUser;
            std::string              serverQuery;
            Server::SortType sortType; // This creates a circular dependancy since Server::CalculateFilterType takes
                                       // SettingsManager as a param
        } serverSettings;
    } m_Data;

  private:
    void        DefaultSettings();
    std::string m_Path;
};

#endif // SETTINGS_MANAGER_H
