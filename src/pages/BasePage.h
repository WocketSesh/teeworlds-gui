#ifndef BASE_PAGE_H
#define BASE_PAGE_H

#include "gtkmm/box.h"
#include "gtkmm/window.h"

class PageManager;

class BasePage : public Gtk::Box
{

  public:
    virtual bool ShowPage(Gtk::Window *window);
    virtual bool HidePage(Gtk::Window *window);

    int m_ElipsisWidth;

    std::string AdjustTextFit(const char *initial_text, int size);

    void SetName(const char *name);

    const char  *m_Name;
    PageManager *m_PageManager;
    int          m_Referer;

    // Used to determine whether the current page can have a selected
    // user/server
    // might move
    bool        m_SelectableUser;
    bool        m_SelectableServer;
    const char *m_SelectedUser;
    const char *m_SelectedServer;

    friend std::ostream &operator<<(std::ostream &os, const BasePage &obj);

    BasePage *GetReferer();
    // The body has to be in the header afaik otherwise i would need to make a
    // definition for every possible type??
    template <typename T> T *GetCastedReferer()
    {
        BasePage *referer = GetReferer();

        if (referer == nullptr)
            return nullptr;

        return dynamic_cast<T *>(referer);
    };
    const char *GetRefererName(const char *defaultTo);

    Gtk::Box m_Box;

  private:
    bool m_Active;

  protected:
    BasePage(const char *name, PageManager *pageManager);
};

#endif // BASE_PAGE_H
