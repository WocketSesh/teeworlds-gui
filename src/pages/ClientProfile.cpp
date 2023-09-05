#include "ClientProfile.h"
#include "BasePage.h"
#include "PageManager.h"
#include "gtkmm/box.h"
#include "gtkmm/entry.h"
#include "gtkmm/enums.h"
#include "gtkmm/label.h"
#include "gtkmm/object.h"
#include "gtkmm/revealer.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/togglebutton.h"
#include "utils.h"
#include <fmt/core.h>
#include <unordered_map>

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

    m_ClientExtraInformationContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);

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

        m_ClientExtraInformationContainer.pack_start(m_ClientLatestContainer, Gtk::PACK_EXPAND_WIDGET);
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

        m_ClientExtraInformationContainer.pack_start(m_ClientFavouriteContainer, Gtk::PACK_EXPAND_WIDGET);
    }

    m_ClientLatestSpacerOuter.set_size_request(-1, 50);
    m_ClientLatestSpacerOuter.set_opacity(0);
    m_ClientBaseInformationContainer.pack_start(m_ClientLatestSpacerOuter, Gtk::PACK_SHRINK);
    m_ClientBaseInformationContainer.pack_start(m_ClientExtraInformationContainer, Gtk::PACK_EXPAND_WIDGET);

    m_ClientMapContainer.set_orientation(Gtk::ORIENTATION_VERTICAL);
    m_ClientScrolledMapContainer.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    m_ClientScrolledMapContainer.add(m_ClientMapContainer);

    for (std::string t : m_MapTypes)
    {

        auto x = m_EnumMap.find(t);

        MapTypes type;

        if (x != m_EnumMap.end())
        {
            type = x->second;
        }
        else
        {
            printf("Failed to find matching enum for '%s'\n", t.c_str());
            return;
        }

        MapContainers *mapContainer = new MapContainers;

        mapContainer->m_ToggleButton   = Gtk::make_managed<Gtk::ToggleButton>();
        mapContainer->m_OuterContainer = Gtk::make_managed<Gtk::Box>();
        mapContainer->m_Scrolled       = Gtk::make_managed<Gtk::ScrolledWindow>();
        mapContainer->m_InnerContainer = Gtk::make_managed<Gtk::Box>();
        mapContainer->m_Revealer       = Gtk::make_managed<Gtk::Revealer>();
        mapContainer->m_Grid           = Gtk::make_managed<Gtk::Grid>();
        mapContainer->m_Type           = type;

        mapContainer->m_Grid->set_column_homogeneous(true);

        mapContainer->m_ToggleButton->set_label(fmt::format("Show {}", t));
        mapContainer->m_Revealer->set_reveal_child(false);
        mapContainer->m_Revealer->set_transition_duration(0);

        mapContainer->m_InnerContainer->hide();
        mapContainer->m_Revealer->add(*mapContainer->m_Grid);

        // should probably just store both string and enum in the struct to make life easier
        mapContainer->m_ToggleButton->signal_toggled().connect([type, t, this]() {
            auto container = GetContainer(type);
            if (container == nullptr)
                return;

            bool active = container->m_ToggleButton->get_active();
            container->m_ToggleButton->set_label(fmt::format("{} {}", active ? "Hide" : "Show", t).c_str());
            // I dont fucking know the revealer is kind of pointless
            if (active)
            {
                container->m_Scrolled->set_no_show_all(false);
                container->m_Scrolled->show_all();
            }
            else
            {

                container->m_Scrolled->set_no_show_all(false);
                container->m_Scrolled->hide();
            }
            container->m_Revealer->set_reveal_child(active);
        });

        mapContainer->m_OuterContainer->set_orientation(Gtk::ORIENTATION_VERTICAL);

        mapContainer->m_InnerContainer->set_orientation(Gtk::ORIENTATION_VERTICAL);

        mapContainer->m_Scrolled->hide();
        mapContainer->m_Scrolled->set_no_show_all();

        mapContainer->m_OuterContainer->pack_start(*mapContainer->m_ToggleButton, Gtk::PACK_SHRINK);
        mapContainer->m_OuterContainer->pack_start(*mapContainer->m_Scrolled, Gtk::PACK_SHRINK);

        mapContainer->m_InnerContainer->pack_start(*mapContainer->m_Revealer, Gtk::PACK_SHRINK);
        mapContainer->m_Scrolled->add(*mapContainer->m_InnerContainer);

        mapContainer->m_Scrolled->set_min_content_height(300);
        mapContainer->m_Scrolled->set_max_content_height(300);

        m_ClientMapContainer.pack_start(*mapContainer->m_OuterContainer, Gtk::PACK_SHRINK);

        m_MapContainers.push_back(mapContainer);
    }

    m_ClientInnerContainer.pack_start(m_ClientBaseInformationContainer, Gtk::PACK_SHRINK);

    m_ClientInnerContainer.pack_start(m_ClientScrolledMapContainer, Gtk::PACK_EXPAND_WIDGET);
    // m_ClientMapScrolledContainer.add(m_ClientMapsGridContainer);

    m_ClientOuterContainer.pack_start(m_TitleContainer, Gtk::PACK_SHRINK);
    m_ClientOuterContainer.pack_start(m_ClientInnerContainer, Gtk::PACK_EXPAND_WIDGET);

    m_ClientOuterContainer.show_all();
}

ClientProfile::MapContainers *ClientProfile::GetContainer(MapTypes type)
{
    for (auto container : m_MapContainers)
    {
        if (container->m_Type == type)
            return container;
    }

    return nullptr;
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

void ClientProfile::ShowProfile(const char *name, Server *server)
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

    m_ClientLatestGridContainer.attach(*CreateAndSetLabel("Map", Gtk::ALIGN_START), 0, 0);
    m_ClientLatestGridContainer.attach(*CreateAndSetLabel("Type", Gtk::ALIGN_START), 1, 0);
    m_ClientLatestGridContainer.attach(*CreateAndSetLabel("Time", Gtk::ALIGN_START), 2, 0);

    m_ClientLatestGridContainer.attach(*CreateAndSetLabel("", Gtk::ALIGN_START), 0, 1);

    for (int i = 0; i < m_Info->last_finishes.size(); i++)
    {
        auto cur = m_Info->last_finishes[i];

        m_ClientLatestGridContainer.attach(*CreateAndSetLabel(cur.map, Gtk::ALIGN_START), 0, 2 + i);
        m_ClientLatestGridContainer.attach(*CreateAndSetLabel(cur.type, Gtk::ALIGN_START), 1, 2 + i);
        m_ClientLatestGridContainer.attach(*CreateAndSetLabel(FormatTime(cur.time).c_str(), Gtk::ALIGN_START), 2,
                                           2 + i);
    }

    m_ClientFavouriteGridContainer.attach(*CreateAndSetLabel("Partner", Gtk::ALIGN_START), 0, 0);
    m_ClientFavouriteGridContainer.attach(*CreateAndSetLabel("Finishes", Gtk::ALIGN_START), 2, 0);
    m_ClientFavouriteGridContainer.attach(*CreateAndSetLabel("", Gtk::ALIGN_START), 0, 1);

    for (int i = 0; i < m_Info->favorite_partners.size(); i++)
    {
        auto cur = m_Info->favorite_partners[i];

        m_ClientFavouriteGridContainer.attach(*CreateAndSetLabel(cur.name, Gtk::ALIGN_START), 0, 2 + i);
        m_ClientFavouriteGridContainer.attach(
            *CreateAndSetLabel(std::to_string(cur.finishes).c_str(), Gtk::ALIGN_START), 2, 2 + i);
    }

    m_ClientLatestContainer.show_all();
    m_ClientFavouriteContainer.show_all();

    m_ClientMainContainer.remove(m_ClientLoadingContainer);
    m_ClientMainContainer.pack_start(m_ClientOuterContainer, Gtk::PACK_EXPAND_WIDGET);

    std::unordered_map<MapTypes, MapTypesStruct *> mapStructs = {
        {MapTypes::NOVICE, &info->types.Novice},
        {MapTypes::MODERATE, &info->types.Moderate},
        {MapTypes::BRUTAL, &info->types.Brutal},
        {MapTypes::INSANE, &info->types.Insane},
        {MapTypes::FUN, &info->types.Fun},
        {MapTypes::SOLO, &info->types.Solo},
        {MapTypes::RACE, &info->types.Race},
        {MapTypes::DUMMY, &info->types.Race},
        {MapTypes::DDMAX_NEXT, &info->types.DDmaX_Next},
        {MapTypes::DDMAX_EASY, &info->types.DDmaX_Easy},
        {MapTypes::DDMAX_NUT, &info->types.DDmaX_Nut},
        {MapTypes::DDMAX_PRO, &info->types.DDmaX_Pro},
    };

    for (auto container : m_MapContainers)
    {
        MapTypesStruct t;

        auto x = mapStructs.find(container->m_Type);

        if (x != mapStructs.end())
        {
            t = *(x->second);
        }
        else
            continue;

        container->m_Grid->attach(*CreateAndSetLabel("Map", Gtk::ALIGN_START), 0, 0);
        container->m_Grid->attach(*CreateAndSetLabel("Time", Gtk::ALIGN_START), 1, 0);
        container->m_Grid->attach(*CreateAndSetLabel("Rank", Gtk::ALIGN_START), 2, 0);
        container->m_Grid->attach(*CreateAndSetLabel("", Gtk::ALIGN_START), 0, 1);

        PopulateMapGrid(*container->m_Grid, t);
    }

    printf("Got stats for: %s\nPoints: %d/%d\nRank:%d\n", info->player, info->points.points, info->points.total,
           info->points.rank);
}

void ClientProfile::PopulateMapGrid(Gtk::Grid &grid, const MapTypesStruct &maps)
{
    int row = 2;
    for (auto m : maps.maps)
    {
        grid.attach(*CreateAndSetLabel(m.name, Gtk::ALIGN_START), 0, row);
        grid.attach(*CreateAndSetLabel(m.time == -1 ? "Unfinished" : FormatTime(m.time).c_str(), Gtk::ALIGN_START), 1,
                    row);
        grid.attach(*CreateAndSetLabel(m.rank == -1 ? "Unranked" : std::to_string(m.rank).c_str(), Gtk::ALIGN_START), 2,
                    row);
        row++;
    }
};

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

    m_ClientFavouriteGridContainer.foreach ([&](Gtk::Widget &child) { m_ClientFavouriteGridContainer.remove(child); });
}

void ClientProfile::ClearMapGrid()
{
    for (auto container : m_MapContainers)
    {
        container->m_Grid->foreach ([&](Gtk::Widget &child) { container->m_Grid->remove(child); });
    }
}

bool ClientProfile::ShowPage(Gtk::Window *window)
{
    window->add(m_Box);

    m_Cancelled = false;
    m_PageManager->m_HttpManager->GetClientInfoFromName(m_CurrentUser, m_PageManager->m_Soup,
                                                        &ClientProfile::FinishedLoading, this);

    return true;
}
