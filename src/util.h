#ifndef __HOTCORNER_H__
#define __HOTCORNER_H__

#include <gtk/gtk.h>
#include "hotcorner.h"

gboolean is_upper_left(GdkRectangle monitorInfo, gint x, gint y);
gboolean is_upper_right(GdkRectangle monitorInfo, gint x, gint y);
gboolean is_lower_right(GdkRectangle monitorInfo, gint x, gint y);
gboolean is_lower_left(GdkRectangle monitorInfo, gint x, gint y);
void toggle_desktop(int spot, HotCorner * hotCorenr);
void turn_off_monitor(int spot, HotCorner * hotCorner);
void start_screensaver(int spot, HotCorner * hotCorner);
void start_dashboard(int spot, HotCorner * hotCorner);

#endif
