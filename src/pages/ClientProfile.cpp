#include "ClientProfile.h"
#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/box.h"
#include "gtkmm/entry.h"
#include "gtkmm/enums.h"
#include "gtkmm/object.h"
#include "utils.h"
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

    {

        m_ClientLatestScrolledContainer.set_policy(Gtk::PolicyType::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

        m_ClientLatestContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);
        m_ClientLatestGridContainer.set_column_homogeneous(true);
        m_ClientLatestLabel.set_label("Latest Finishes");

        auto latestLabelFont = m_ClientLatestLabel.get_pango_context()->get_font_description();
        latestLabelFont.set_size(15 * PANGO_SCALE);
        m_ClientLatestLabel.override_font(latestLabelFont);
        m_ClientLatestLabel.set_halign(Gtk::ALIGN_START);

        m_ClientLatestSpacerInner.set_size_request(-1, 25);
        m_ClientLatestSpacerInner.set_opacity(0);

        m_ClientLatestContainer.pack_start(m_ClientLatestLabel, Gtk::PACK_SHRINK);
        m_ClientLatestContainer.pack_start(m_ClientLatestSpacerInner, Gtk::PACK_SHRINK);
        m_ClientLatestContainer.pack_start(m_ClientLatestScrolledContainer, Gtk::PACK_EXPAND_WIDGET);

        m_ClientLatestScrolledContainer.add(m_ClientLatestGridContainer);

        m_ClientExtraInformationContainer.pack_start(m_ClientLatestContainer, Gtk::PACK_SHRINK);
    }

    {
        m_ClientFavouriteScrolledContainer.set_policy(Gtk::PolicyType::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);

        m_ClientFavouriteContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);
        m_ClientFavouriteGridContainer.set_column_homogeneous(true);
        m_ClientFavouriteLabel.set_label("Favourite Partners");

        auto favouriteLabelFont = m_ClientFavouriteLabel.get_pango_context()->get_font_description();
        favouriteLabelFont.set_size(15 * PANGO_SCALE);
        m_ClientFavouriteLabel.override_font(favouriteLabelFont);
        m_ClientFavouriteLabel.set_halign(Gtk::ALIGN_START);

        m_ClientFavouriteSpacerInner.set_size_request(-1, 25);
        m_ClientFavouriteSpacerInner.set_opacity(0);

        m_ClientFavouriteContainer.pack_start(m_ClientFavouriteLabel, Gtk::PACK_SHRINK);
        m_ClientFavouriteContainer.pack_start(m_ClientFavouriteSpacerInner, Gtk::PACK_SHRINK);
        m_ClientFavouriteContainer.pack_start(m_ClientFavouriteScrolledContainer, Gtk::PACK_EXPAND_WIDGET);

        m_ClientFavouriteScrolledContainer.add(m_ClientFavouriteGridContainer);

        m_ClientExtraInformationContainer.pack_start(m_ClientFavouriteContainer, Gtk::PACK_SHRINK);
    }

    m_ClientLatestSpacerOuter.set_size_request(-1, 50);
    m_ClientLatestSpacerOuter.set_opacity(0);
    m_ClientBaseInformationContainer.pack_start(m_ClientLatestSpacerOuter, Gtk::PACK_SHRINK);
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

    m_ClientName.set_label(fmt::format("User: {}", info->player));
    m_ClientPoints.set_label(fmt::format("Points: {}/{}", info->points.points, info->points.total));
    m_ClientRank.set_label(fmt::format("Rank: {}", info->points.rank));

    Gtk::Label *latestNameHeader = Gtk::make_managed<Gtk::Label>("Map");
    Gtk::Label *latestTimeHeader = Gtk::make_managed<Gtk::Label>("Time");
    Gtk::Label *latestSpacer     = Gtk::make_managed<Gtk::Label>();

    latestNameHeader->set_halign(Gtk::ALIGN_START);
    latestTimeHeader->set_halign(Gtk::ALIGN_START);

    m_ClientLatestGridContainer.attach(*latestNameHeader, 0, 0);
    m_ClientLatestGridContainer.attach(*latestTimeHeader, 2, 0);
    m_ClientLatestGridContainer.attach(*latestSpacer, 0, 1);

    for (int i = 0; i < m_Info->last_finishes.size(); i++)
    {
        auto        cur = m_Info->last_finishes[i];
        Gtk::Label *n   = Gtk::make_managed<Gtk::Label>(cur.map);
        Gtk::Label *t   = Gtk::make_managed<Gtk::Label>(FormatTime(cur.time));

        n->set_halign(Gtk::ALIGN_START);
        t->set_halign(Gtk::ALIGN_START);

        m_ClientLatestGridContainer.attach(*n, 0, 2 + i);
        m_ClientLatestGridContainer.attach(*t, 2, 2 + i);
    }

    Gtk::Label *favouriteNameHeader = Gtk::make_managed<Gtk::Label>("Partner");
    Gtk::Label *favouriteTimeHeader = Gtk::make_managed<Gtk::Label>("Finishes");
    Gtk::Label *favouriteSpacer     = Gtk::make_managed<Gtk::Label>();

    favouriteNameHeader->set_halign(Gtk::ALIGN_START);
    favouriteTimeHeader->set_halign(Gtk::ALIGN_START);

    m_ClientFavouriteGridContainer.attach(*favouriteNameHeader, 0, 0);
    m_ClientFavouriteGridContainer.attach(*favouriteTimeHeader, 2, 0);
    m_ClientFavouriteGridContainer.attach(*favouriteSpacer, 0, 1);

    for (int i = 0; i < m_Info->favorite_partners.size(); i++)
    {
        auto        cur = m_Info->favorite_partners[i];
        Gtk::Label *n   = Gtk::make_managed<Gtk::Label>(cur.name);
        Gtk::Label *t   = Gtk::make_managed<Gtk::Label>(std::to_string(cur.finishes));

        printf("%s %d\n", cur.name, cur.finishes);

        n->set_halign(Gtk::ALIGN_START);
        t->set_halign(Gtk::ALIGN_START);

        m_ClientFavouriteGridContainer.attach(*n, 0, 2 + i);
        m_ClientFavouriteGridContainer.attach(*t, 2, 2 + i);
    }

    m_ClientLatestContainer.show_all();
    m_ClientFavouriteContainer.show_all();
    m_ClientMapScrolledContainer.show_all();

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

    ClearLatestGrid();
    ClearFavouriteGrid();
    ClearMapGrid();
    if (m_Info != nullptr)
    {
        delete m_Info;
        m_Info = nullptr;
    }

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
