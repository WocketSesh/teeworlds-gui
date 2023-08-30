#include "gtkmm/main.h"
#include "FriendsPage.h"
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

int main(int argc, char *argv[])
{

    auto        app = Gtk::Application::create(argc, argv, "com.sesh.teeworlds");
    Gtk::Window window;

    PageManager *pageManager = setupPageManager("main", &window);

    // window.fullscreen();

    return app->run(window);
}
