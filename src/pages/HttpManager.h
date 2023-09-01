#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H

#include "ClientProfile.h"
#include "ServerStruct.h"
#include "json-glib/json-glib.h"
#include "libsoup/soup-message.h"
#include "libsoup/soup-method.h"
#include "libsoup/soup-session.h"
#include "libsoup/soup-types.h"
#include <functional>
#include <vector>

typedef void (*HttpManagerCallback)(JsonParser *parser, gpointer calleeClass);
typedef void (*HttpManagerServerCallback)(std::vector<Server *>, gpointer calleeClass);
typedef void (*HttpManagerClientCallback)(ClientInfo *client, gpointer calleeClass);

class HttpManager
{

  public:
    enum ServerRequestType
    {
        ALL,
        SINGLE
    };

    SoupMessage *GetAsync(const char *uri, SoupSession *session, HttpManagerCallback callback, gpointer calleeClass);
    void         GetServers(SoupSession *session, HttpManagerServerCallback, gpointer calleeClass);
    // For ease of use even when just getting a singular server you will get a vector, which will either
    // contain 1 server, or be empty if none were found
    void GetServerFromAddress(const char *address, SoupSession *session, HttpManagerServerCallback callback,
                              gpointer calleeClass);
    void GetServerFromClientName(const char *name, SoupSession *session, HttpManagerServerCallback callback,
                                 gpointer calleeClass);
    void GetClientInfoFromName(const char *name, SoupSession *session, HttpManagerClientCallback callback,
                               gpointer caleeClass);

    static JsonParser *ToJson(GInputStream *stream);

  private:
    static std::vector<MapInfo> JsonToMapStruct(JsonObject *object);
    static void                 GotClient(GObject *source, GAsyncResult *result, gpointer userData);
    static void                 GetFinished(GObject *source, GAsyncResult *result, gpointer userData);
    static void                 GotServers(GObject *source, GAsyncResult *result, gpointer userData);
};

// I dont fucking know try merge these two at some point
struct HttpManagerData
{
    gpointer            calleeClass;
    HttpManagerCallback callback;
};

struct ClientRequestData
{
    gpointer                  calleeClass;
    HttpManagerClientCallback callback;
    const char               *query;
};

struct ServerRequestData
{
    gpointer                       calleeClass;
    HttpManagerServerCallback      callback;
    HttpManager::ServerRequestType type;
    const char                    *query;
};

#endif // HTTP_MANAGER_H
