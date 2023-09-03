#include "HttpManager.h"
#include "ClientProfile.h"
#include "libsoup/soup-message.h"
#include "libsoup/soup-method.h"
#include "libsoup/soup-session.h"
#include "libsoup/soup-types.h"
#include <cstdio>
#include <cstring>
#include <fmt/core.h>
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

void HttpManager::GetClientInfoFromName(const char *name, SoupSession *session, HttpManagerClientCallback callback,
                                        gpointer calleeClass)
{

    ClientRequestData *data = new ClientRequestData{calleeClass, callback, name};

    SoupMessage *message =
        soup_message_new(SOUP_METHOD_GET, fmt::format("http://ddnet.org/players/?json2={}", name).c_str());

    soup_session_send_async(session, message, G_PRIORITY_DEFAULT, NULL, &HttpManager::GotClient, data);
    printf("sent request\n");
};

void HttpManager::GotClient(GObject *source, GAsyncResult *result, gpointer userData)
{
    printf("starting json parse\n");
    ClientRequestData *data = static_cast<ClientRequestData *>(userData);

    GError       *error  = NULL;
    GInputStream *stream = soup_session_send_finish(SOUP_SESSION(source), result, &error);

    if (error)
    {
        printf("Error gotclient\n");
        g_error_free(error);
        return;
    }

    JsonParser *parser = ToJson(stream);
    ClientInfo *client = new ClientInfo;

    JsonNode   *root    = json_parser_get_root(parser);
    JsonObject *rootObj = json_node_get_object(root);

    client->player = json_object_get_string_member_with_default(rootObj, "player", "undefined");

    // TODO: idk if i like this style to separate up code blocks, maybe keep maybe dont

    {
        JsonObject *clientPoints = json_object_get_object_member(rootObj, "points");

        client->points.total  = json_object_get_int_member_with_default(clientPoints, "total", -1);
        client->points.points = json_object_get_int_member_with_default(clientPoints, "points", -1);
        client->points.rank   = json_object_get_int_member_with_default(clientPoints, "rank", -1);
    }

    {
        JsonObject *clientPointsLastMonth = json_object_get_object_member(rootObj, "points_last_month");

        client->points_last_month.points = json_object_get_int_member_with_default(clientPointsLastMonth, "points", -1);
        client->points_last_month.rank   = json_object_get_int_member_with_default(clientPointsLastMonth, "rank", -1);
    }

    {
        JsonObject *clientPointsLastWeek = json_object_get_object_member(rootObj, "points_last_week");

        client->points_last_week.points = json_object_get_int_member_with_default(clientPointsLastWeek, "points", -1);
        client->points_last_week.rank   = json_object_get_int_member_with_default(clientPointsLastWeek, "rank", -1);
    }

    {
        JsonObject *clientFirstFinish = json_object_get_object_member(rootObj, "first_finish");

        client->first_finish.timestamp = json_object_get_int_member_with_default(clientFirstFinish, "timestamp", -1);
        client->first_finish.map  = json_object_get_string_member_with_default(clientFirstFinish, "map", "undefined");
        client->first_finish.time = json_object_get_int_member_with_default(clientFirstFinish, "time", -1);
    }

    JsonArray *clientLastFinishes = json_object_get_array_member(rootObj, "last_finishes");

    for (int i = 0; i < json_array_get_length(clientLastFinishes); i++)
    {
        JsonObject *cur = json_array_get_object_element(clientLastFinishes, i);

        LastFinishes lf;
        lf.time      = json_object_get_int_member_with_default(cur, "time", -1);
        lf.map       = json_object_get_string_member_with_default(cur, "map", "undefined");
        lf.timestamp = json_object_get_int_member_with_default(cur, "timestamp", -1);
        lf.type      = json_object_get_string_member_with_default(cur, "type", "undefined");
        lf.country   = json_object_get_string_member_with_default(cur, "contry", "undefined");

        client->last_finishes.push_back(lf);
    }

    JsonArray *favouritePartners = json_object_get_array_member(rootObj, "favorite_partners");

    for (int i = 0; i < json_array_get_length(favouritePartners); i++)
    {
        JsonObject *cur = json_array_get_object_element(favouritePartners, i);

        FavouritePartners fp;
        fp.name     = json_object_get_string_member_with_default(cur, "name", "undefined");
        fp.finishes = json_object_get_int_member_with_default(cur, "finishes", -1);

        client->favorite_partners.push_back(fp);
    }

    JsonObject *types = json_object_get_object_member(rootObj, "types");

    {
        JsonObject *typeObj = json_object_get_object_member(types, "Novice");

        MapTypes mapType;
        mapType.type = "Novice";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Novice = mapType;
    }

    {
        JsonObject *typeObj = json_object_get_object_member(types, "Moderate");

        MapTypes mapType;
        mapType.type = "Moderate";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Moderate = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "Brutal");

        MapTypes mapType;
        mapType.type = "Brutal";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Brutal = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "Insane");

        MapTypes mapType;
        mapType.type = "Insane";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Insane = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "Dummy");

        MapTypes mapType;
        mapType.type = "Dummy";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Dummy = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "Solo");

        MapTypes mapType;
        mapType.type = "Solo";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Solo = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "Race");

        MapTypes mapType;
        mapType.type = "Race";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Race = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "DDmaX.Easy");

        MapTypes mapType;
        mapType.type = "DDmaX.Easy";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.DDmaX_Easy = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "DDmaX.Next");

        MapTypes mapType;
        mapType.type = "DDmaX.Next";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.DDmaX_Next = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "DDmaX.Pro");

        MapTypes mapType;
        mapType.type = "DDmaX.Pro";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.DDmaX_Pro = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "DDmaX.Nut");

        MapTypes mapType;
        mapType.type = "DDmaX.Nut";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.DDmaX_Nut = mapType;
    }
    {
        JsonObject *typeObj = json_object_get_object_member(types, "Oldschool");

        MapTypes mapType;
        mapType.type = "Oldschool";

        JsonObject *points = json_object_get_object_member(typeObj, "points");

        mapType.points.points = json_object_get_int_member_with_default(points, "points", -1);
        mapType.points.rank   = json_object_get_int_member_with_default(points, "rank", -1);
        mapType.points.total  = json_object_get_int_member_with_default(points, "total", -1);

        JsonObject *obj = json_object_get_object_member(typeObj, "maps");

        mapType.maps = JsonToMapStruct(obj);

        client->types.Oldschool = mapType;
    }

    (*data->callback)(client, data->calleeClass);
}

std::vector<MapInfo> HttpManager::JsonToMapStruct(JsonObject *object)
{

    std::vector<MapInfo> mapInfos;

    GList *maps = json_object_get_members(object);

    GList *iter;

    int i = 0;

    for (iter = maps; iter != NULL; iter = iter->next)
    {
        printf("%d\n", i);
        const gchar *key = (const gchar *)iter->data;

        JsonObject *mapInfoObj = json_object_get_object_member(object, key);

        MapInfo mapInfo;

        mapInfo.rank           = json_object_get_int_member_with_default(mapInfoObj, "rank", -1);
        mapInfo.points         = json_object_get_int_member_with_default(mapInfoObj, "points", -1);
        mapInfo.finishes       = json_object_get_int_member_with_default(mapInfoObj, "finishes", 0);
        mapInfo.name           = key;
        mapInfo.time           = json_object_get_int_member_with_default(mapInfoObj, "time", -1);
        mapInfo.first_finish   = json_object_get_int_member_with_default(mapInfoObj, "first_finish", -1);
        mapInfo.total_finishes = json_object_get_int_member_with_default(mapInfoObj, "total_finishes", -1);

        mapInfos.push_back(mapInfo);

        i++;
    }

    return mapInfos;
}

// this a fucking mess
void HttpManager::GotServers(GObject *source, GAsyncResult *result, gpointer userData)
{
    ServerRequestData *data = static_cast<ServerRequestData *>(userData);

    GError       *error  = NULL;
    GInputStream *stream = soup_session_send_finish(SOUP_SESSION(source), result, &error);

    if (error)
    {
        printf("Error gotservers\n");
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
