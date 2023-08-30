
#include "FriendsPage.h"
#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/enums.h"
#include "gtkmm/window.h"
#include "sigc++/functors/mem_fun.h"

FriendsPage::FriendsPage(PageManager *pageManager) : BasePage("friends", pageManager)
{
    m_SelectableUser = true;

    m_Box.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_Title.set_label("Friends");

    m_Back.set_label("Go Back");

    m_Back.signal_clicked().connect(sigc::mem_fun(*this, &FriendsPage::BackClicked));

    m_Box.add(m_Back);
    m_Box.add(m_Title);

    m_Box.show_all();
}

void FriendsPage::BackClicked()
{
    m_PageManager->ChangePage("main");
}
