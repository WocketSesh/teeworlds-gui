#ifndef CLIENT_PROFILE_H
#define CLIENT_PROFILE_H

#include "BasePage.h"
#include "ServerStruct.h"
#include "gtkmm/button.h"
#include "gtkmm/grid.h"
#include "gtkmm/scrolledwindow.h"
#include "gtkmm/separator.h"
#include "gtkmm/spinner.h"

// Members are named to match their JSON counterpart

struct Points
{
    int total;
    int points;
    int rank;
};

struct LastFinishes
{
    int         timestamp;
    const char *map;
    int         time;
    const char *country;
    const char *type;
};

struct FavouritePartners
{
    const char *name;
    int         finishes;
};

struct MapInfo
{
    const char *name;
    int         points;
    int         total_finishes;
    int         finishes;
    // These three are only present if finishes > 0
    int rank;
    int time;
    int first_finish;
};

struct MapTypes
{
    const char          *type;
    struct Points        points;
    std::vector<MapInfo> maps;
};
class ClientInfo
{
  public:
    const char   *player;
    struct Points points;
    struct PointsLastMonth
    {
        int points;
        int rank;
    } points_last_month;
    struct PointsLastWeek
    {
        int points;
        int rank;
    } points_last_week;
    struct FirstFinish
    {
        int         timestamp;
        const char *map;
        int         time;
    } first_finish;
    std::vector<LastFinishes>      last_finishes;
    std::vector<FavouritePartners> favorite_partners;
    struct Types
    {
        MapTypes Novice;
        MapTypes Moderate;
        MapTypes Brutal;
        MapTypes Insane;
        MapTypes Dummy;
        MapTypes DDmaX_Easy;
        MapTypes DDmaX_Next;
        MapTypes DDmaX_Pro;
        MapTypes DDmaX_Nut;
        MapTypes Oldschool;
        MapTypes Solo;
        MapTypes Race;
        MapTypes Fun;
    } types;
};

class ClientProfile : public BasePage
{

  public:
    ClientProfile(PageManager *pageManager);

    bool m_Cancelled;

    ClientInfo *m_Info;
    // This will not attempt to get a clients server itself if one is not provided
    void ShowProfile(const char *name, Gtk::Window *window, Server *server);
    bool ShowPage(Gtk::Window *window) override;

    static void FinishedLoading(ClientInfo *info, gpointer caleeClass);

  private:
    Gtk::Box m_ClientMainContainer; // VERT

    Gtk::Box     m_ClientLoadingContainer; // VERT;
    Gtk::Label   m_ClientLoadingLabel;     // PARENT:LOADING
    Gtk::Button  m_BackLoadingButton;      // PARENT:LOADING
    Gtk::Spinner m_LoadingSpinner;         // PARENT:LOADING

    Gtk::Box            m_ClientOuterContainer;             // VERT PARENT:MAIN
    Gtk::Box            m_TitleContainer;                   // HORIZ PARENT:OUTER
    Gtk::Button         m_BackButton;                       // PARENT:TITLE
    Gtk::Label          m_TitleLabel;                       // PARENT:TITLE
    Gtk::Box            m_ClientInnerContainer;             // HORIZ PARENT:OUTER
    Gtk::Box            m_ClientBaseInformationContainer;   // VERT PARENT:INNER
    Gtk::Label          m_ClientName;                       // PARENT:BASEINFO
    Gtk::Label          m_ClientPoints;                     // PARENT:BASEINFO
    Gtk::Label          m_ClientRank;                       // PARENT:BASEINFO
    Gtk::Box            m_ClientExtraInformationContainer;  // HORIZ PARENT:BASEINFO
    Gtk::ScrolledWindow m_ClientFavouriteScrolledContainer; // PARENT:EXTRAINFO
    Gtk::Grid           m_ClientFavouriteGridContainer;     // PARENT:SCROLLED

    Gtk::Separator      m_ClientLatestSpacerOuter;       // PARENT:BASEINFO
    Gtk::Separator      m_ClientLatestSpacerInner;       // PARENT:LATEST
    Gtk::Box            m_ClientLatestContainer;         // VERT PARENT:EXTRAINFO
    Gtk::Label          m_ClientLatestLabel;             // PARENT LATEST
    Gtk::ScrolledWindow m_ClientLatestScrolledContainer; // PARENT:LATEST
    Gtk::Grid           m_ClientLatestGridContainer;     // PARENT:LATEST

    Gtk::Separator      m_ClientFavouriteSpacerInner; // PARENT:FAVOURITE
    Gtk::Box            m_ClientFavouriteContainer;   // VERT PARENT:EXTRAINFO
    Gtk::Label          m_ClientFavouriteLabel;       // PARENT FAVOURITE
    Gtk::ScrolledWindow m_ClientMapScrolledContainer; // PARENT:INNER
    Gtk::Grid           m_ClientMapsGridContainer;    // PARENT:SCROLLED

    Server     *m_CurrentServer;
    const char *m_CurrentUser;

    void BackClicked();
    void ClearFavouriteGrid();
    void ClearLatestGrid();
    void ClearMapGrid();

    // Setup Methods
    void PopulateMain(ClientInfo *info);
    void SetupLoading();
    void SetupMain();
    void Reset();
};

#endif // CLIENT_PROFILE_H
