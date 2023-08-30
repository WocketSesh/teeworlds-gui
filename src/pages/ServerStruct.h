#ifndef SERVER_STRUCT_H
#define SERVER_STRUCT_H

#include "gtkmm/label.h"
#include "gtkmm/listboxrow.h"
#include "json-glib/json-glib.h"
#include <memory>
#include <vector>

class ServerPageBoxRow : public Gtk::ListBoxRow
{
  public:
    ServerPageBoxRow() = default;

    int index; // keeps track of index in server/client vector
               // Since sorting will modify its actual index unlike filtering
    Gtk::Label *serverNameLabel;
    Gtk::Label *mapNameLabel;
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
    std::vector<const char *> addresses;
    const char               *location;
    ServerInfo               *info;

    ServerPageBoxRow *row;
};

#endif
