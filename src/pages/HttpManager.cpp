#include "HttpManager.h"
#include "libsoup/soup-message.h"
#include "libsoup/soup-method.h"
#include "libsoup/soup-session.h"
#include "libsoup/soup-types.h"
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

SoupMessage *HttpManager::GetAsync(const char *uri, SoupSession *session, HttpManagerCallback callback,
                                   gpointer calleeClass)
{

    HttpManagerData *data = new HttpManagerData{calleeClass, callback};

    SoupMessage *message = soup_message_new(SOUP_METHOD_GET, uri);

    soup_session_send_async(session, message, G_PRIORITY_DEFAULT, NULL, &HttpManager::GetFinished, data);

    return message;
}

void HttpManager::GetServers(SoupSession *session, HttpManagerServerCallback callback, gpointer calleeClass)
{
    ServerRequestData *data =
        new ServerRequestData{calleeClass, callback, HttpManager::ServerRequestType::ALL, nullptr};

    SoupMessage *message = soup_message_new(SOUP_METHOD_GET, "http://master1.ddnet.org/ddnet/15/servers.json");

    soup_session_send_async(session, message, G_PRIORITY_DEFAULT, NULL, &HttpManager::GotServers, data);
}

// this a fucking mess
void HttpManager::GotServers(GObject *source, GAsyncResult *result, gpointer userData)
{
    ServerRequestData *data = static_cast<ServerRequestData *>(userData);

    GError       *error  = NULL;
    GInputStream *stream = soup_session_send_finish(SOUP_SESSION(source), result, &error);

    if (error)
    {
        g_error_free(error);
        return;
    }

    JsonParser *parser = ToJson(stream);

    std::vector<Server *> servers;

    JsonNode   *root    = json_parser_get_root(parser);
    JsonObject *rootObj = json_node_get_object(root);
    JsonArray  *arr     = json_object_get_array_member(rootObj, "servers");

    for (int i = 0; i < json_array_get_length(arr); i++)
    {
        JsonObject *curr = json_array_get_object_element(arr, i);

        JsonArray  *addresses  = json_object_get_array_member(curr, "addresses");
        JsonObject *info       = json_object_get_object_member(curr, "info");
        JsonArray  *clientsArr = json_object_get_array_member(info, "clients");

        // dont really know if they are calling with address or name
        // but the chances of someone having a name the same as an address is low as fuck
        // if its even possible at all
        if (data->type == ServerRequestType::SINGLE)
        {
            bool inServer = false;

            for (int i = 0; i < json_array_get_length(addresses); i++)
            {
                if (strcmp(data->query, json_array_get_string_element(addresses, i)) == 0)
                {
                    inServer = true;
                    i        = json_array_get_length(addresses);
                    continue;
                }
            }

            if (!inServer)
                for (i = 0; i < json_array_get_length(clientsArr); i++)
                {
                    JsonObject *c = json_array_get_object_element(clientsArr, i);
                    if (strcmp(data->query, json_object_get_string_member(c, "name")) == 0)
                    {
                        inServer = true;
                        i        = json_array_get_length(clientsArr);
                        continue;
                    }
                }

            if (!inServer)
                continue;
        }

        Server *currStruct = new Server;

        for (int i = 0; i < json_array_get_length(addresses); i++)
        {
            currStruct->addresses.push_back(json_array_get_string_element(addresses, i));
        }
        currStruct->location = json_object_get_string_member(curr, "location");

        currStruct->info = new ServerInfo;

        currStruct->info->max_players = json_object_get_int_member(info, "max_players");
        currStruct->info->name        = json_object_get_string_member(info, "name");

        for (int j = 0; j < json_array_get_length(clientsArr); j++)
        {
            JsonObject *currClient = json_array_get_object_element(clientsArr, j);

            Client *clientPtr = new Client;

            clientPtr->name  = json_object_get_string_member_with_default(currClient, "name", "undefined");
            clientPtr->clan  = json_object_get_string_member_with_default(currClient, "clan", "undefined");
            clientPtr->score = json_object_get_int_member_with_default(currClient, "score", -1);

            currStruct->info->clients.push_back(clientPtr);
        }

        JsonObject *map = json_object_get_object_member(info, "map");

        currStruct->info->map.name = json_object_get_string_member(map, "name");

        servers.push_back(currStruct);

        if (data->type == ServerRequestType::SINGLE)
        {
            (*data->callback)(servers, data->calleeClass);
            delete data;
            return;
        }
    }

    (*data->callback)(servers, data->calleeClass);
    delete data;
}

JsonParser *HttpManager::ToJson(GInputStream *stream)
{

    GDataInputStream *dataStream = g_data_input_stream_new(stream);
    gsize             length     = 0;
    gchar            *data       = g_data_input_stream_read_line(dataStream, &length, nullptr, nullptr);

    JsonParser *parser = json_parser_new();
    json_parser_load_from_data(parser, data, length, nullptr);

    return parser;
}

void HttpManager::GetFinished(GObject *source, GAsyncResult *result, gpointer userData)
{
    HttpManagerData *p = static_cast<HttpManagerData *>(userData);

    GError       *error  = NULL;
    GInputStream *stream = soup_session_send_finish(SOUP_SESSION(source), result, &error);

    if (error)
    {
        g_error_free(error);
        return;
    }

    (*p->callback)(ToJson(stream), p->calleeClass);

    delete p;
}
