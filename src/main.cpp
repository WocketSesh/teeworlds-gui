#include "gtkmm/main.h"
#include "FriendsPage.h"
#include "giomm/application.h"
#include "glibmm/optioncontext.h"
#include "glibmm/optionentry.h"
#include "glibmm/optiongroup.h"
#include "glibmm/ustring.h"
#include "gtkmm/button.h"
#include "pages/MainPage.h"
#include "pages/PageManager.h"
#include "startup/startup.h"
#include <fmt/format.h>
#include <gtkmm-3.0/gtkmm.h>
#include <gtkmm-3.0/gtkmm/application.h>
#include <iostream>
#include <memory>
#include <string>

int HandleCommands(const Glib::RefPtr<Gio::ApplicationCommandLine> &cmds, PageManager *pageManager)
{
    printf("running??\n");

    int    argc;
    char **argv = cmds->get_arguments(argc);

    bool          showServers   = false;
    Glib::ustring profileToView = "";
    Glib::ustring queryToUse    = "";

    Glib::OptionContext context;
    Glib::OptionGroup   group("options", "Base Options");

    Glib::OptionEntry servers;
    servers.set_short_name('s');
    servers.set_long_name("servers");
    servers.set_description("Opens the GUI on the servers page");
    servers.set_flags(Glib::OptionEntry::FLAG_NO_ARG);

    Glib::OptionEntry profile;
    profile.set_short_name('p');
    profile.set_long_name("profile");
    profile.set_description("Opens the GUI on the specified players profile");
    profile.set_arg_description("Players profile you wish to open");

    Glib::OptionEntry query;
    query.set_short_name('q');
    query.set_long_name("query");
    query.set_description("Sets the query (can only be used with server for now)");
    query.set_arg_description("Query to use");

    group.add_entry(servers, showServers);
    group.add_entry(profile, profileToView);
    group.add_entry(query, queryToUse);

    context.add_group(group);

    context.parse(argc, argv);

    if (showServers)
    {

        ServerPage *serverPage = pageManager->GetCastedPage<ServerPage *>("servers");

        if (serverPage != nullptr)
        {
            serverPage->ShowServers(queryToUse.c_str());
        }
    }
    else if (!profileToView.empty())
    {
        ClientProfile *clientProfile = pageManager->GetCastedPage<ClientProfile *>("client-profile");

        if (clientProfile != nullptr)
            clientProfile->ShowProfile(profileToView.c_str(), pageManager->m_Window, nullptr);
    }

    pageManager->m_App->activate();

    return 0;
}

int main(int argc, char *argv[])
{

    auto app = Gtk::Application::create(argc, argv, "com.sesh.teeworlds", Gio::APPLICATION_HANDLES_COMMAND_LINE);

    Gtk::Window  window;
    PageManager *pageManager = setupPageManager("main", &window);

    pageManager->m_App = app;

    app->signal_command_line().connect(sigc::bind(sigc::ptr_fun(HandleCommands), pageManager), false);

    // window.fullscreen();

    return app->run(window);
}
