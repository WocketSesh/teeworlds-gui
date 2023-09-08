
#include "startup.h"
#include "ClientProfile.h"
#include "FriendsPage.h"
#include "MainPage.h"
#include "PageManager.h"
#include "ServerPage.h"
#include "startup/startup.h"
#include <memory>

PageManager *setupPageManager(const char *defaultPage, Gtk::Window *window)
{
    PageManager *pageManager = new PageManager(window);

    // This needs to be done first for pages that may depend on setting information
    pageManager->m_Settings->Init(std::string(getenv("HOME")).append("/.teeworlds-utils.txt"));

    pageManager->RegisterPage(std::make_unique<MainPage>(pageManager));
    pageManager->RegisterPage(std::make_unique<FriendsPage>(pageManager));
    pageManager->RegisterPage(std::make_unique<ServerPage>(pageManager));
    pageManager->RegisterPage(std::make_unique<ClientProfile>(pageManager));

    pageManager->ChangePage(defaultPage);

    return pageManager;
}
