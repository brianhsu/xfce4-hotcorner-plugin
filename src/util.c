#include <libwnck/libwnck.h>
#include "util.h"

gboolean is_upper_left(GdkRectangle monitorInfo, gint x, gint y) {
    return (x == monitorInfo.x) && (y == monitorInfo.y);
}

gboolean is_upper_right(GdkRectangle monitorInfo, gint x, gint y) {
    return (x == monitorInfo.x + monitorInfo.width - 1) && (y == monitorInfo.y);
}

gboolean is_lower_right(GdkRectangle monitorInfo, gint x, gint y) {
    return (x == monitorInfo.x + monitorInfo.width - 1) && (y == monitorInfo.y + monitorInfo.height - 1);
}

gboolean is_lower_left(GdkRectangle monitorInfo, gint x, gint y) {
    return (x == monitorInfo.x) && (y == monitorInfo.y + monitorInfo.height - 1);
}

void toggle_desktop(int spot, HotCorner * hotCorner) {
    WnckScreen * wnck = wnck_screen_get_default();
    gboolean is_showing = wnck_screen_get_showing_desktop(wnck);
    wnck_screen_toggle_showing_desktop(wnck, !is_showing);
}

void turn_off_monitor(int spot, HotCorner * hotCorner) {
    system("xset dpms force off");
}

void start_screensaver(int spot, HotCorner * hotCorner) {
    system("xscreensaver-command -activate");
}

void start_dashboard(int spot, HotCorner * hotCorenr) {
    system("xfdashboard");
}

