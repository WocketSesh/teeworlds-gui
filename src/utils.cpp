#include "utils.h"
#include "gtkmm/label.h"
#include <fmt/core.h>

std::string FormatTime(int seconds)
{
    int hours   = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs    = seconds % 60;

    return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, secs);
}

Gtk::Label *CreateAndSetLabel(const char *text, Gtk::Align alignment)
{
    Gtk::Label *l = Gtk::make_managed<Gtk::Label>(text);
    l->set_halign(alignment);
    return l;
};
