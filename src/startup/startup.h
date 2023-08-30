#ifndef STARTUP_H
#define STARTUP_H

#include "PageManager.h"
#include "gtkmm/window.h"
PageManager *setupPageManager(const char *defaultPage, Gtk::Window *window);

#endif
