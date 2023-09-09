#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/window.h"

BasePage::BasePage(const char *name, PageManager *pageManager)
    : m_Name(name), m_PageManager(pageManager), m_Referer(-1), m_Active(false)
{

    auto elip = Pango::Layout::create(get_pango_context());
    elip->set_text("...");
    int w, h;
    elip->get_size(w, h);
    m_ElipsisWidth = w / PANGO_SCALE;
}

std::ostream &operator<<(std::ostream &os, const BasePage &obj)
{
    os << obj.m_Name << std::endl;
    return os;
}

// This may not be the best way, but the inbuilt methods were aids to use??
// Also this is not that much slower so its fine for now
std::string BasePage::AdjustTextFit(const char *initial_text, int size)
{

    auto        layout = Pango::Layout::create(get_pango_context());
    std::string text   = initial_text;
    layout->set_text(text);

    int width, height;
    layout->get_size(width, height);

    if (width / PANGO_SCALE > size)
    {
        while (width / PANGO_SCALE > size - m_ElipsisWidth && text.size() > 1)
        {
            text.pop_back();
            layout->set_text(text);
            layout->get_size(width, height);
        }
        text += "...";
    }
    else if (width / PANGO_SCALE < size)
    {
        while (width / PANGO_SCALE < size)
        {
            text += " ";
            layout->set_text(text);
            layout->get_size(width, height);
        }

        text.resize(text.size() - ((width / PANGO_SCALE) - size));
    }

    return text;
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
