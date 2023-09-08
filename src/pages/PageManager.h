#ifndef PAGE_MANAGER_H
#define PAGE_MANAGER_H

const int PAGES = 4;

#include "BasePage.h"
#include "HttpManager.h"
#include "MainPage.h"
#include "SettingsManager.h"
#include "gtkmm/cssprovider.h"
#include "gtkmm/window.h"
#include "libsoup/soup.h"
#include <memory>
#include <vector>

class PageManager
{

  public:
    PageManager(Gtk::Window *window);

    Gtk::Window                   *m_Window;
    Glib::RefPtr<Gtk::Application> m_App;

    bool ChangePage(const char *name);

    bool RegisterPage(std::unique_ptr<BasePage> page);

    int IndexOf(const char *name);
    int IndexOf(BasePage *page);

    SoupSession     *m_Soup;
    SettingsManager *m_Settings;
    GMainLoop       *m_Loop;
    HttpManager     *m_HttpManager;

    Glib::RefPtr<Gtk::CssProvider>  m_Css;
    Glib::RefPtr<Gtk::StyleContext> m_Style;

    // Used to get a raw pointer of a page if needed
    BasePage               *GetRawPage(int index);
    BasePage               *GetRawPage(const char *name);
    template <typename T> T GetCastedPage(const char *name)
    {
        return dynamic_cast<T>(GetRawPage(name));
    }

  private:
    int                                    m_Active;
    int                                    m_RegisteredPages;
    std::vector<std::unique_ptr<BasePage>> m_Pages;
};

#endif // PAGE_MANAGER_H
