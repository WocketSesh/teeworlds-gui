
#include "startup.h"
#include "FriendsPage.h"
#include "MainPage.h"
#include "PageManager.h"
#include "ServerPage.h"
#include "startup/startup.h"
#include <memory>

PageManager *setupPageManager(const char *defaultPage, Gtk::Window *window)
{
    PageManager *pageManager = new PageManager(window);

    pageManager->RegisterPage(std::make_unique<MainPage>(pageManager));
    pageManager->RegisterPage(std::make_unique<FriendsPage>(pageManager));
    pageManager->RegisterPage(std::make_unique<ServerPage>(pageManager));

    pageManager->ChangePage(defaultPage);

    return pageManager;
}
