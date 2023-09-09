#ifndef SERVER_STRUCT_H
#define SERVER_STRUCT_H

#include "BasePage.h"
#include "gtkmm/label.h"
#include "gtkmm/listboxrow.h"
#include "json-glib/json-glib.h"
#include <memory>
#include <vector>

// Annoying but circular dependancy shit ass
class SettingsManager;

class ServerPageBoxRow : public Gtk::ListBoxRow
{
  public:
    ServerPageBoxRow() = default;

    int index; // keeps track of index in server/client vector
               // Since sorting will modify its actual index unlike filtering
    Gtk::Label *serverNameLabel;
    Gtk::Label *mapNameLabel;
    Gtk::Label *playerCountLabel;
    Gtk::Label *glyphLabel;
};

// May just change these to structs, maybe not
// Depends if i want to add methods later
class Client
{
  public:
    const char *name;
    const char *clan;
    int         score;

    ServerPageBoxRow *row;
};

class ServerInfo
{
  public:
    ServerInfo();

    gint64      max_players;
    const char *name;
    std::string game_type;
    struct
    {
        const char *name;
    } map;
    std::vector<Client *> clients;
};

class Server
{
  public:
    Server();

    enum SortType
    {
        NONE,
        PLAYERS_ASCENDING,
        PLAYERS_DESCENDING
    };

    enum ServerFilterTypes
    {
        ALL       = 1 << 0,
        GORES     = 1 << 1,
        DDRACE    = 1 << 2,
        FAVOURITE = 1 << 3,
        FRIENDS   = 1 << 4,
        OTHER     = 1 << 5
    };

    int  m_FilterType;
    bool m_Favourite;
    int  m_FriendCount;

    bool ShouldShow(std::string strFilter = "", ServerFilterTypes filterType = ALL);
    void CalculateFilterType(SettingsManager *settingsManager);
    void CalculateFriendCount(SettingsManager &settingsManager);
    // shit solution but until i move AdjustTextFit somewhere else or some shit idk
    void SetupText(BasePage &page);

    std::vector<const char *> m_Addresses;
    const char               *m_Location;
    ServerInfo               *m_Info;

    ServerPageBoxRow *row;
};

#endif
