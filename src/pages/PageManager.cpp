
#include "PageManager.h"
#include "BasePage.h"
#include "HttpManager.h"
#include "SettingsManager.h"
#include "gtkmm/stylecontext.h"
#include "gtkmm/styleprovider.h"
#include "gtkmm/window.h"
#include "libsoup/soup-session.h"
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>

PageManager::PageManager(Gtk::Window *window) : m_Pages(5)
{
    m_Css             = Gtk::CssProvider::create();
    m_Style           = Gtk::StyleContext::create();
    m_Window          = window;
    m_Active          = -1;
    m_RegisteredPages = 0;
    m_Soup            = soup_session_new();
    m_Loop            = g_main_loop_new(NULL, false);
    m_HttpManager     = new HttpManager();
    m_Settings        = new SettingsManager();

    // g_main_loop_run(m_Loop);
}

int PageManager::IndexOf(const char *name)
{
    for (int i = 0; i < m_RegisteredPages; i++)
    {
        if (m_Pages[i] == nullptr)
            continue;

        if (strcmp(m_Pages[i]->m_Name, name) == 0)
            return i;
    }

    return -1;
}

int PageManager::IndexOf(BasePage *page)
{
    return IndexOf(page->m_Name);
}

BasePage *PageManager::GetRawPage(int index)
{
    if (index < 0 || index > m_RegisteredPages)
        return nullptr;

    return m_Pages[index].get();
}

BasePage *PageManager::GetRawPage(const char *name)
{
    for (int i = 0; i < m_RegisteredPages; i++)
    {
        if (m_Pages[i] == nullptr)
            continue;

        if (strcmp(m_Pages[i]->m_Name, name) == 0)
            return m_Pages[i].get();
    }

    return nullptr;
}

bool PageManager::RegisterPage(std::unique_ptr<BasePage> page)
{
    if (GetRawPage(page->m_Name) != nullptr)
        return false;

    if (m_RegisteredPages >= PAGES)
        return false;

    m_Pages[m_RegisteredPages] = std::move(page);

    printf("Registered page %s\n", m_Pages[m_RegisteredPages]->m_Name);

    m_RegisteredPages++;

    return true;
}

bool PageManager::ChangePage(const char *name)
{
    if (m_Active != -1 && strcmp(m_Pages[m_Active]->m_Name, name) == 0)
        return false;

    for (int i = 0; i < m_RegisteredPages; i++)
    {
        BasePage *c = m_Pages[i].get();

        if (strcmp(c->m_Name, name) == 0)
        {
            if (m_Active != -1)
            {
                m_Pages[m_Active]->HidePage(m_Window);
                m_Pages[i]->m_Referer = m_Active;
            }
            m_Active = i;
            m_Pages[i]->ShowPage(m_Window);

            return true;
        }
    }

    return false;
}
