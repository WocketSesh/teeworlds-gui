#ifndef SETTINGS_MANAGER_H
#define SETTINGS_MANAGER_H

#include "ServerPage.h"
#include <string>
#include <vector>
class SettingsManager
{
  public:
    SettingsManager();
    void Init(std::string filePath);
    bool AddFriend(std::string name);
    bool RemoveFriend(std::string name);
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
            ServerPage::SortType     sortType;
        } serverSettings;
    } m_Data;

  private:
    void        DefaultSettings();
    std::string m_Path;
};

#endif // SETTINGS_MANAGER_H
