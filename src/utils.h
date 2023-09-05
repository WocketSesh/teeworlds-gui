#ifndef UTILS_H
#define UTILS_H

#include "gtkmm/alignment.h"
#include "gtkmm/enums.h"
#include <string>

std::string FormatTime(int seconds);
Gtk::Label *CreateAndSetLabel(const char *text, Gtk::Align alignment);

#endif // UTILS_H
