#include "ClientProfile.h"
#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/box.h"
#include "gtkmm/enums.h"
#include <fmt/core.h>

ClientProfile::ClientProfile(PageManager *pageManager) : BasePage("client-profile", pageManager)
{
    m_Info = nullptr;
    m_Box.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_ClientMainContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_Box.pack_start(m_ClientMainContainer, Gtk::PACK_EXPAND_WIDGET);

    SetupLoading();
    SetupMain();

    m_ClientMainContainer.show();
    m_Box.show();
}

void ClientProfile::SetupMain()
{
    m_ClientOuterContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_TitleContainer.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    m_BackButton.set_label("<");
    m_BackButton.set_halign(Gtk::Align::ALIGN_START);
    m_BackButton.signal_clicked().connect(sigc::mem_fun(*this, &ClientProfile::BackClicked));

    m_TitleLabel.set_label("Viewing Profile");
    m_TitleLabel.set_halign(Gtk::Align::ALIGN_CENTER);

    m_TitleContainer.pack_start(m_BackButton, Gtk::PACK_SHRINK);
    m_TitleContainer.pack_start(m_TitleLabel, Gtk::PACK_EXPAND_WIDGET);

    m_ClientInnerContainer.set_orientation(Gtk::ORIENTATION_HORIZONTAL);

    m_ClientBaseInformationContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_ClientName.set_halign(Gtk::Align::ALIGN_START);
    m_ClientPoints.set_halign(Gtk::Align::ALIGN_START);
    m_ClientRank.set_halign(Gtk::Align::ALIGN_START);

    m_ClientBaseInformationContainer.pack_start(m_ClientName, Gtk::PACK_SHRINK);
    m_ClientBaseInformationContainer.pack_start(m_ClientPoints, Gtk::PACK_SHRINK);
    m_ClientBaseInformationContainer.pack_start(m_ClientRank, Gtk::PACK_SHRINK);

    m_ClientExtraInformationContainer.set_orientation(Gtk::ORIENTATION_HORIZONTAL);
    m_ClientExtraInformationContainer.pack_start(m_ClientLatestScrolledContainer, Gtk::PACK_SHRINK);
    m_ClientExtraInformationContainer.pack_start(m_ClientFavouriteScrolledContainer, Gtk::PACK_SHRINK);
    m_ClientLatestScrolledContainer.add(m_ClientLatestGridContainer);
    m_ClientFavouriteScrolledContainer.add(m_ClientFavouriteGridContainer);

    m_ClientBaseInformationContainer.pack_start(m_ClientExtraInformationContainer, Gtk::PACK_EXPAND_WIDGET);

    m_ClientInnerContainer.pack_start(m_ClientBaseInformationContainer, Gtk::PACK_SHRINK);

    m_ClientMapScrolledContainer.add(m_ClientMapsGridContainer);

    m_ClientInnerContainer.pack_start(m_ClientMapScrolledContainer, Gtk::PACK_EXPAND_WIDGET);

    m_ClientOuterContainer.pack_start(m_TitleContainer, Gtk::PACK_SHRINK);
    m_ClientOuterContainer.pack_start(m_ClientInnerContainer, Gtk::PACK_EXPAND_WIDGET);

    m_ClientOuterContainer.show_all();
}

void ClientProfile::SetupLoading()
{
    m_ClientLoadingContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);

    m_ClientLoadingLabel.set_label("Fetching User Data...");
    auto fontDesc = m_ClientLoadingLabel.get_pango_context()->get_font_description();
    fontDesc.set_size(20 * PANGO_SCALE);
    m_ClientLoadingLabel.override_font(fontDesc);

    m_BackLoadingButton.set_label("Back");
    m_BackLoadingButton.set_size_request(120, 40);

    m_BackLoadingButton.signal_clicked().connect(sigc::mem_fun(*this, &ClientProfile::BackClicked));

    m_ClientLoadingContainer.set_valign(Gtk::ALIGN_CENTER);
    m_ClientLoadingContainer.set_halign(Gtk::ALIGN_CENTER);

    m_ClientLoadingContainer.pack_start(m_ClientLoadingLabel, Gtk::PACK_SHRINK);
    m_ClientLoadingContainer.pack_start(m_BackLoadingButton, Gtk::PACK_SHRINK);
    m_ClientLoadingContainer.pack_start(m_LoadingSpinner, Gtk::PACK_SHRINK);

    m_ClientLoadingContainer.show_all();

    m_ClientMainContainer.pack_start(m_ClientLoadingContainer, Gtk::PACK_EXPAND_WIDGET);
}

void ClientProfile::BackClicked()
{
    m_Cancelled = true;
    Reset();
    m_PageManager->ChangePage(GetRefererName("main"));
}

void ClientProfile::ShowProfile(const char *name, Gtk::Window *window, Server *server)
{
    m_CurrentServer = server;
    m_CurrentUser   = name;

    m_LoadingSpinner.start();
    m_PageManager->ChangePage(m_Name);
}

void ClientProfile::PopulateMain(ClientInfo *info)
{

    m_Info = info;

    m_ClientName.set_label(info->player);
    m_ClientPoints.set_label(fmt::format("Points: {}/{}", info->points.points, info->points.total));
    m_ClientRank.set_label(fmt::format("Rank: {}", info->points.rank));

    m_ClientMainContainer.remove(m_ClientLoadingContainer);
    m_ClientMainContainer.pack_start(m_ClientOuterContainer, Gtk::PACK_EXPAND_WIDGET);
    printf("Got stats for: %s\nPoints: %d/%d\nRank:%d\n", info->player, info->points.points, info->points.total,
           info->points.rank);
}

void ClientProfile::FinishedLoading(ClientInfo *info, gpointer calleeClass)
{

    ClientProfile *profile = static_cast<ClientProfile *>(calleeClass);

    if (profile->m_Cancelled || profile->m_Info != nullptr)
    {
        delete info;
        return;
    }

    profile->PopulateMain(info);
}

void ClientProfile::Reset()
{
    if (m_Info != nullptr)
        delete m_Info;

    m_ClientMainContainer.remove(m_ClientOuterContainer);
    m_ClientMainContainer.pack_start(m_ClientLoadingContainer, Gtk::PACK_EXPAND_WIDGET);
}

void ClientProfile::ClearLatestGrid()
{
    m_ClientLatestGridContainer.foreach ([&](Gtk::Widget &child) { m_ClientLatestGridContainer.remove(child); });
}

void ClientProfile::ClearFavouriteGrid()
{
    m_ClientMapsGridContainer.foreach ([&](Gtk::Widget &child) { m_ClientMapsGridContainer.remove(child); });
}

void ClientProfile::ClearMapGrid()
{
    m_ClientFavouriteGridContainer.foreach ([&](Gtk::Widget &child) { m_ClientFavouriteGridContainer.remove(child); });
}

bool ClientProfile::ShowPage(Gtk::Window *window)
{
    window->add(m_Box);

    m_Cancelled = false;
    m_PageManager->m_HttpManager->GetClientInfoFromName(m_CurrentUser, m_PageManager->m_Soup,
                                                        &ClientProfile::FinishedLoading, this);

    return true;
}
