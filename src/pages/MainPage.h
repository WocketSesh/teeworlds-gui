
#ifndef MAIN_PAGE_H
#define MAIN_PAGE_H

#include "BasePage.h"
#include "gtkmm/box.h"
#include "gtkmm/button.h"
#include "gtkmm/label.h"

class MainPage : public BasePage
{
  public:
    MainPage(PageManager *pageManager);

  private:
    Gtk::Label m_Title;
    Gtk::Box   m_TitleBox;

    Gtk::Box m_ButtonBoxOuter;
    Gtk::Box m_ButtonBoxInner;

    Gtk::Button m_Friends;
    Gtk::Box    m_FriendsBox;

    Gtk::Button m_Servers;
    Gtk::Box    m_ServersBox;

    Gtk::Button m_Asset;
    Gtk::Box    m_AssetBox;

    /* Button Clicks */

    void ServersClicked();
    void FriendsClicked();
};

#endif
