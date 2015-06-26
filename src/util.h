/* Copyright (c) BrianHsu <brianhsu.hsu@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

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
