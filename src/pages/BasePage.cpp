#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/window.h"

BasePage::BasePage(const char *name, PageManager *pageManager)
    : m_Name(name), m_PageManager(pageManager), m_Referer(-1), m_Active(false)
{
}

std::ostream &operator<<(std::ostream &os, const BasePage &obj)
{
    os << obj.m_Name << std::endl;
    return os;
}

const char *BasePage::GetRefererName(const char *defaultTo)
{
    BasePage *referer = GetReferer();
    return referer == nullptr ? defaultTo : referer->m_Name;
}

BasePage *BasePage::GetReferer()
{
    return m_PageManager->GetRawPage(m_Referer);
}

bool BasePage::ShowPage(Gtk::Window *window)
{
    window->add(m_Box);
    printf("Showing %s\n", m_Name);

    return true;
}

bool BasePage::HidePage(Gtk::Window *window)
{
    window->remove();
    printf("Hiding %s\n", m_Name);

    return true;
}

void BasePage::SetName(const char *name)
{
    m_Name = name;
}
