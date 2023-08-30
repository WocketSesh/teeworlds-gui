#ifndef FRIENDS_PAGE_H
#define FRIENDS_PAGE_H

#include "BasePage.h"
#include "gtkmm/button.h"
#include "gtkmm/label.h"
#include "gtkmm/window.h"
class FriendsPage : public BasePage
{
  public:
    FriendsPage(PageManager *pageManager);

  private:
    Gtk::Label m_Title;

    Gtk::Button m_Back;

    void BackClicked();
};

#endif
