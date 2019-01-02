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

#ifndef __HOTCORNER_PLUGIN__
#define __HOTCORNER_PLUGIN__

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libwnck/libwnck.h>

#define UPDATE_TIMEOUT 50

typedef struct HotCorner HotCorner;
typedef void (* ActionCallback)(int spot, HotCorner * hotCorner);

struct HotCorner {

  XfcePanelPlugin * plugin;

  guint timeout_id;
  guint ticket;

  gboolean isExecuted;

  GtkWidget * icon;

  GdkRectangle monitorInfo;

  ActionCallback upperLeftCallback;
  ActionCallback lowerLeftCallback;
  ActionCallback upperRightCallback;
  ActionCallback lowerRightCallback;

  gint upperLeftActionID;
  gint upperRightActionID;
  gint lowerLeftActionID;
  gint lowerRightActionID;

  GtkWidget * upperLeftCommand;
  GtkWidget * upperRightCommand;
  GtkWidget * lowerLeftCommand;
  GtkWidget * lowerRightCommand;

  const gchar * upperLeftSavedCommand;
  const gchar * upperRightSavedCommand;
  const gchar * lowerLeftSavedCommand;
  const gchar * lowerRightSavedCommand;

};

#endif
