#ifndef __HOTCORNER_H__
#define __HOTCORNER_H__

#include <gtk/gtk.h>

gboolean is_upper_left(GdkRectangle monitorInfo, gint x, gint y);
gboolean is_upper_right(GdkRectangle monitorInfo, gint x, gint y);
gboolean is_lower_right(GdkRectangle monitorInfo, gint x, gint y);
gboolean is_lower_left(GdkRectangle monitorInfo, gint x, gint y);
void toggle_desktop();
void turn_off_monitor();
void start_screensaver();
void start_dashboard();

#endif
