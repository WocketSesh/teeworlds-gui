#include "utils.h"
#include <fmt/core.h>

std::string FormatTime(int seconds)
{
    int hours   = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs    = seconds % 60;

    return fmt::format("{:02d}:{:02d}:{:02d}", hours, minutes, secs);
}
