
#include "MainPage.h"
#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/alignment.h"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/window.h"
#include "pango/pango-font.h"
#include "pango/pango-types.h"
#include "pangomm/font.h"
#include "pangomm/fontdescription.h"
#include "sigc++/functors/mem_fun.h"

MainPage::MainPage(PageManager *pageManager) : BasePage("main", pageManager)
{

    m_Box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_Box.set_spacing(20);

    /* */

    auto titleFont = m_Title.get_pango_context()->get_font_description();
    titleFont.set_size(20 * PANGO_SCALE);
    titleFont.set_family("Comic Mono");

    m_TitleBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    m_Title.set_halign(Gtk::Align::ALIGN_CENTER);
    m_Title.set_label("Teeworlds Utilities");
    m_Title.override_font(titleFont);

    m_TitleBox.pack_start(m_Title, Gtk::PackOptions::PACK_EXPAND_WIDGET);

    m_Box.pack_start(m_TitleBox, Gtk::PackOptions::PACK_SHRINK);

    /* */

    m_ButtonBoxOuter.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    m_ButtonBoxInner.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_ButtonBoxInner.set_halign(Gtk::Align::ALIGN_CENTER);
    m_ButtonBoxInner.set_spacing(10);

    m_ButtonBoxOuter.pack_start(m_ButtonBoxInner, Gtk::PackOptions::PACK_SHRINK);

    m_Box.pack_start(m_ButtonBoxOuter, Gtk::PackOptions::PACK_SHRINK);

    /* */

    auto friendsFont = m_Friends.get_pango_context()->get_font_description();
    friendsFont.set_size(16 * PANGO_SCALE);

    m_FriendsBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    m_Friends.set_halign(Gtk::Align::ALIGN_CENTER);
    m_Friends.set_size_request(240, 30);
    m_Friends.set_label("Friends");
    m_Friends.override_font(friendsFont);

    m_FriendsBox.pack_start(m_Friends, Gtk::PackOptions::PACK_EXPAND_WIDGET);

    m_Friends.signal_clicked().connect(sigc::mem_fun(*this, &MainPage::FriendsClicked));

    m_Box.pack_start(m_FriendsBox, Gtk::PackOptions::PACK_SHRINK);

    /* */

    auto serversFont = m_Servers.get_pango_context()->get_font_description();
    serversFont.set_size(16 * PANGO_SCALE);

    m_ServersBox.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    m_Servers.set_halign(Gtk::Align::ALIGN_CENTER);
    m_Servers.set_size_request(240, 30);
    m_Servers.set_label("Servers");
    m_Servers.override_font(serversFont);

    m_ServersBox.pack_start(m_Servers, Gtk::PackOptions::PACK_EXPAND_WIDGET);

    m_Servers.signal_clicked().connect(sigc::mem_fun(*this, &MainPage::ServersClicked));

    m_Box.pack_start(m_ServersBox, Gtk::PackOptions::PACK_SHRINK);

    /* */

    m_Box.show_all();
}

void MainPage::ServersClicked()
{
    m_PageManager->ChangePage("servers");
}

void MainPage::FriendsClicked()
{
    m_PageManager->ChangePage("friends");
}
