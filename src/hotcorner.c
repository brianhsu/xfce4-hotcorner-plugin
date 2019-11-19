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

// #include <stdio.h>
// #include <stdlib.h>
// #include <time.h>
// #include <string.h>

#include <gtk/gtk.h>
// #include <glib/gprintf.h>  // for gcc's warning: implicit declaration of function 'g_sprintf'
#include <libxfce4util/libxfce4util.h>
#include <libxfce4ui/libxfce4ui.h>
#include <libxfce4panel/libxfce4panel.h>

#include <stdio.h>
#include "hotcorner.h"
#include "util.h"

#define BORDER 4

#define UPPER_LEFT  0
#define UPPER_RIGHT 1
#define LOWER_LEFT  2
#define LOWER_RIGHT 3


void run_command(const gchar * command) {

    gchar * value = g_strstrip(g_strdup(command));

    if (strlen(value) > 0) {
        system(value);
    }

    g_free(value);

}

void run_custom_command(int spot, HotCorner * hotCorner) {

    switch (spot) {
        case UPPER_LEFT:
            run_command(hotCorner->upperLeftSavedCommand);
            break;
        case UPPER_RIGHT:
            run_command(hotCorner->upperRightSavedCommand);
            break;
        case LOWER_LEFT:
            run_command(hotCorner->lowerLeftSavedCommand);
            break;
        case LOWER_RIGHT:
            run_command(hotCorner->lowerRightSavedCommand);
            break;

    }

}

ActionCallback get_action_callback_from_index(int index, int * outSelected) {
    *outSelected = index;

    switch (index) {
        case 0:
            return NULL;
            break;
        case 1:
            return start_dashboard;
            break;
        case 2:
            return toggle_desktop;
            break;
        case 3:
            return start_screensaver;
            break;
        case 4:
            return turn_off_monitor;
            break;
        case 5:
            return run_custom_command;
    }

}

static void read_config_file(XfcePanelPlugin * plugin, HotCorner * hotCorner) {
    gchar  *file = xfce_panel_plugin_lookup_rc_file(plugin);

    if (file)
    {
        XfceRc *rc = xfce_rc_simple_open (file, TRUE);

        if (rc != NULL)
        {
            hotCorner->disableWhenFullScreen = xfce_rc_read_int_entry(rc, "DISABLE_WHEN_FULL_SCREEN", FALSE);
            hotCorner->upperLeftActionID = xfce_rc_read_int_entry(rc, "UPPER_LEFT_ACTION_ID", 0);
            hotCorner->upperRightActionID = xfce_rc_read_int_entry(rc, "UPPER_RIGHT_ACTION_ID", 0);
            hotCorner->lowerLeftActionID = xfce_rc_read_int_entry(rc, "LOWER_LEFT_ACTION_ID", 0);
            hotCorner->lowerRightActionID = xfce_rc_read_int_entry(rc, "LOWER_RIGHT_ACTION_ID", 0);

            hotCorner->upperLeftSavedCommand  = g_strdup(xfce_rc_read_entry (rc, "UPPER_LEFT_COMMAND", NULL));
            hotCorner->upperRightSavedCommand = g_strdup(xfce_rc_read_entry (rc, "UPPER_RIGHT_COMMAND", NULL));
            hotCorner->lowerLeftSavedCommand  = g_strdup(xfce_rc_read_entry (rc, "LOWER_LEFT_COMMAND", NULL));
            hotCorner->lowerRightSavedCommand = g_strdup(xfce_rc_read_entry (rc, "LOWER_RIGHT_COMMAND", NULL));

            hotCorner->upperLeftCallback = get_action_callback_from_index(hotCorner->upperLeftActionID, &(hotCorner->upperLeftActionID));
            hotCorner->upperRightCallback = get_action_callback_from_index(hotCorner->upperRightActionID, &(hotCorner->upperRightActionID));
            hotCorner->lowerLeftCallback = get_action_callback_from_index(hotCorner->lowerLeftActionID, &(hotCorner->lowerLeftActionID));
            hotCorner->lowerRightCallback = get_action_callback_from_index(hotCorner->lowerRightActionID, &(hotCorner->lowerRightActionID));

            xfce_rc_close (rc);
        }

        g_free (file);
    }

}

static void save_config_file(XfcePanelPlugin * plugin, HotCorner * hotCorner) {

    gchar  *file = xfce_panel_plugin_save_location (plugin, TRUE);

    hotCorner->upperLeftSavedCommand  = g_strdup(gtk_entry_get_text(GTK_ENTRY(hotCorner->upperLeftCommand)));
    hotCorner->upperRightSavedCommand = g_strdup(gtk_entry_get_text(GTK_ENTRY(hotCorner->upperRightCommand)));
    hotCorner->lowerLeftSavedCommand  = g_strdup(gtk_entry_get_text(GTK_ENTRY(hotCorner->lowerLeftCommand)));
    hotCorner->lowerRightSavedCommand = g_strdup(gtk_entry_get_text(GTK_ENTRY(hotCorner->lowerRightCommand)));

    if (file) {

        XfceRc * rc = xfce_rc_simple_open (file, FALSE);

        if (rc) {
            xfce_rc_write_int_entry (rc, "UPPER_LEFT_ACTION_ID", hotCorner->upperLeftActionID);
            xfce_rc_write_int_entry (rc, "UPPER_RIGHT_ACTION_ID", hotCorner->upperRightActionID);
            xfce_rc_write_int_entry (rc, "LOWER_LEFT_ACTION_ID", hotCorner->lowerLeftActionID);
            xfce_rc_write_int_entry (rc, "LOWER_RIGHT_ACTION_ID", hotCorner->lowerRightActionID);

            xfce_rc_write_int_entry (rc, "DISABLE_WHEN_FULL_SCREEN", hotCorner->disableWhenFullScreen);

            xfce_rc_write_entry (rc, "UPPER_LEFT_COMMAND",  gtk_entry_get_text(GTK_ENTRY(hotCorner->upperLeftCommand)));
            xfce_rc_write_entry (rc, "UPPER_RIGHT_COMMAND", gtk_entry_get_text(GTK_ENTRY(hotCorner->upperRightCommand)));
            xfce_rc_write_entry (rc, "LOWER_LEFT_COMMAND",  gtk_entry_get_text(GTK_ENTRY(hotCorner->lowerLeftCommand)));
            xfce_rc_write_entry (rc, "LOWER_RIGHT_COMMAND", gtk_entry_get_text(GTK_ENTRY(hotCorner->lowerRightCommand)));

            xfce_rc_close (rc);
        }

        g_free(file);

    }
}


static void free_data(XfcePanelPlugin *plugin, HotCorner * hotCorner) {
    if (hotCorner->timeout_id != 0) {
        g_source_remove (hotCorner->timeout_id);
    }

    gtk_widget_destroy(hotCorner->icon);
    g_free(hotCorner);
}

static gint check_hot_corner_action(HotCorner * hotCorner) {
    GdkScreen * screen;
    GdkWindow * window; 
    // GdkDisplay * display = gtk_widget_get_display(hotCorner->icon);
    gint x, y;
    // gdk_window_get_pointer(window, &x, &y, NULL);




    GdkSeat *seat;
    GdkDevice *pointer;
    screen = gtk_widget_get_screen(hotCorner->icon);
    window = gdk_screen_get_root_window(screen);
    seat = gdk_display_get_default_seat (gdk_display_get_default ());
    pointer = gdk_seat_get_pointer (seat);
    gdk_window_get_device_position (window, pointer, &x, &y, NULL);

    // if (hotCorner->disableWhenFullScreen) {
    //     WnckScreen * wnckScreen = wnck_screen_get_default();
    //     WnckWindow * activeWindow = wnck_screen_get_active_window(wnckScreen);
    //     if (wnck_window_is_fullscreen(activeWindow)) {
    //         return TRUE;
    //     }
    // }

    if (is_upper_left(hotCorner->monitorInfo, x, y) && hotCorner->upperLeftCallback != NULL) {
        hotCorner->ticket++;
        if (!hotCorner->isExecuted && hotCorner->ticket >= 5) {
            hotCorner->upperLeftCallback(UPPER_LEFT, hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else if (is_upper_right(hotCorner->monitorInfo, x, y) && hotCorner->upperRightCallback != NULL) {
        hotCorner->ticket++;
        if (!hotCorner->isExecuted && hotCorner->ticket >= 5) {
            hotCorner->upperRightCallback(UPPER_RIGHT, hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else if (is_lower_right(hotCorner->monitorInfo, x, y) && hotCorner->lowerRightCallback != NULL) {
        hotCorner->ticket++;
        if (!hotCorner->isExecuted && hotCorner->ticket >= 5) {
            hotCorner->lowerRightCallback(LOWER_RIGHT, hotCorner);
            hotCorner->isExecuted = TRUE;
        }

    } else if (is_lower_left(hotCorner->monitorInfo, x, y) && hotCorner->lowerLeftCallback != NULL) {
        hotCorner->ticket++;
        if (!hotCorner->isExecuted && hotCorner->ticket >= 5) {
            hotCorner->lowerLeftCallback(LOWER_LEFT, hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else {
        hotCorner->ticket = 0;
        hotCorner->isExecuted = FALSE;
    }
    return TRUE;
}

static void start_polling_mouse_position(HotCorner * hotCorner) {
    if (hotCorner->timeout_id == 0)
    {
        hotCorner->timeout_id = g_timeout_add (UPDATE_TIMEOUT, (GSourceFunc)check_hot_corner_action, hotCorner);
    }
}

static HotCorner * hotCorner_new(XfcePanelPlugin *plugin) {
    HotCorner * hotCorner = g_new0(HotCorner, 1);
    hotCorner->plugin = plugin;
    hotCorner->icon = xfce_panel_image_new_from_source ("xfce4-display");
    hotCorner->disableWhenFullScreen = TRUE;
    hotCorner->upperLeftCallback  = NULL;
    hotCorner->lowerLeftCallback  = NULL;
    hotCorner->upperRightCallback = NULL;
    hotCorner->lowerRightCallback = NULL;
    hotCorner->upperLeftCommand  = NULL;
    hotCorner->lowerLeftCommand  = NULL;
    hotCorner->upperRightCommand = NULL;
    hotCorner->lowerRightCommand = NULL;
    hotCorner->upperLeftActionID = 0;
    hotCorner->upperRightActionID = 0;
    hotCorner->lowerLeftActionID = 0;
    hotCorner->lowerRightActionID = 0;


    gtk_container_add(GTK_CONTAINER(hotCorner->plugin), hotCorner->icon);
    gtk_widget_show_all(hotCorner->icon);

    start_polling_mouse_position(hotCorner);


    return hotCorner;
}

static void set_monitor_size(HotCorner * hotCorner) {
      
    GdkDisplay   *display;
    GdkMonitor   *monitor;
    GdkRectangle  geometry;

    display = gdk_display_get_default();
    monitor = gdk_display_get_primary_monitor(display);
    gdk_monitor_get_geometry (monitor, &geometry);

    // // if (wnck_globals.has_init) return;
    // int   argc = 0;
    // char *argv[];
    // gtk_init();
    // // wnck_globals.has_init = TRUE;
    // WnckScreen *screen;
    // screen = wnck_screen_get_default();
    // wnck_screen_force_update(screen);
    


    // if (wnck_globals.has_init) return;
    
    // wnck_globals.has_init = TRUE;
    // wnck_globals.screen = wnck_screen_get_default();
    // wnck_screen_force_update(wnck_globals.screen);

    
    // GdkScreen * screen = gdk_screen_get_default();
    // GdkRectangle monitorInfo;
    // gint monitorID = gdk_screen_get_primary_monitor(screen);
    // gdk_screen_get_monitor_geometry (screen, monitorID, &monitorInfo);
    // hotCorner->monitorInfo = monitorInfo;

	// GdkRectangle result;
	// GdkWindow *gdkwindow =	gtk_widget_get_window(GTK_WIDGET(window));
	// GdkDisplay *display =	gdk_window_get_display(gdkwindow);
	// GdkMonitor *monitor =	gdk_display_get_monitor_at_window(display, gdkwindow);

	// gdk_monitor_get_geometry(monitor, &result);

	// return result;


// #if GTK_CHECK_VERSION(3, 22, 0)
    // GdkDisplay   *display;
    // GdkMonitor   *monitor;
    // GdkRectangle  geometry;

    // display = gdk_display_get_default();
    // monitor = gdk_display_get_primary_monitor(display);
    // gdk_monitor_get_geometry (monitor, &geometry);
    // hotCorner->monitorInfo = geometry;
    g_print ("Hello World1\n");
    // WnckScreen *screen;
    // GdkRectangle  geometry;
    // screen = wnck_screen_get_default();
    // wnck_screen_get_active_workspace(screen);

    // geometry.width  = wnck_screen_get_width(screen);
    // geometry.height  = wnck_screen_get_height(screen);
    // hotCorner->monitorInfo = geometry;
    // for (GList *windows = wnck_screen_get_windows(screen); windows != NULL; windows = windows->next) {        
    //     WnckWindow *window = WNCK_WINDOW(windows->data);
        
    //     if (wnck_window_get_state(window) & WNCK_WINDOW_STATE_SKIP_PAGER) continue;
    //     if (wnck_window_is_minimized(window)) continue;
    //     if (!wnck_window_is_on_workspace(window, wnck_screen_get_active_workspace(screen))) continue;       

    //     GdkRectangle window_geom;
    //     wnck_window_get_geometry(window, &(window_geom.x), &(window_geom.y), &(window_geom.width), &(window_geom.height));
    //     hotCorner->monitorInfo = window_geom;
    // }

    
    g_print ("Hello World2\n");

    // // display = gdk_display_get_default ();
    // display = gtk_widget_get_display (hotCorner->icon);
    // monitor = gdk_display_get_primary_monitor (display);
    // // gdk_monitor_get_geometry (monitor, &geometry);
    // gdk_monitor_get_workarea (monitor, &geometry);

	// get the available area of the screen
	// gdk_monitor_get_workarea(gdk_display_get_primary_monitor(
	// 	gdk_display_get_default()), &geometry);

    // printf ("dasfasdfasfasdfasdfasdf");
    // screen = gdk_display_get_default_screen (display_info->gdisplay);
    // int argc;
    // char *argv[];
    // gtk_init(&argc, &argv);                 // <---- added this

    // display = gdk_display_get_default();
    // monitor = gdk_display_get_primary_monitor(display);
    // gdk_monitor_get_geometry (monitor, &geometry);

    // hotCorner->monitorInfo = geometry;

    // gdk_monitor_get_geometry(m, &r);
    // display = gtk_widget_get_display (icon->priv->dock);
    // monitor = gdk_display_get_monitor_at_point (display, area.x, area.y);
    // gdk_monitor_get_geometry (monitor, &geometry);
    // GdkRectangle workarea = {0};
    // gdk_monitor_get_workarea(
    //     gdk_display_get_primary_monitor(gdk_display_get_default()),
    // &workarea);
    // hotCorner->monitorInfo = geometry;
// #else
//     GdkScreen * screen = gdk_screen_get_default();
//     GdkRectangle monitorInfo;
//     gint monitorID = gdk_screen_get_primary_monitor(screen);
//     gdk_screen_get_monitor_geometry (screen, monitorID, &monitorInfo);
//     hotCorner->monitorInfo = monitorInfo;
// #endif
}

ActionCallback get_action_callback(GtkComboBox * comboBox, int * outSelected) {
    gint selected = gtk_combo_box_get_active(comboBox);
    return get_action_callback_from_index(selected, outSelected);
}

static void set_new_entry_visibility(GtkComboBox * comboBox, GtkWidget * entry) {
    gint selected = gtk_combo_box_get_active(comboBox);

    if (selected != 5) {
        gtk_widget_set_no_show_all(entry, TRUE);
        gtk_widget_hide(entry);
    } else {
        gtk_widget_set_no_show_all(entry, FALSE);
        gtk_widget_show(entry);
    }

}

static void set_corner_command_entry(GtkWidget * entry, HotCorner * hotCorner) {

    const gchar * name = gtk_widget_get_name(GTK_WIDGET(entry));

    if (g_strcmp0(name, "UPPER_LEFT") == 0) {
        hotCorner->upperLeftCommand = entry;
        gtk_entry_set_text(GTK_ENTRY(entry), hotCorner->upperLeftSavedCommand);
    } else if (g_strcmp0(name, "UPPER_RIGHT") == 0) {
        hotCorner->upperRightCommand = entry;
        gtk_entry_set_text(GTK_ENTRY(entry), hotCorner->upperRightSavedCommand);
    } else if (g_strcmp0(name, "LOWER_LEFT") == 0) {
        hotCorner->lowerLeftCommand = entry;
        gtk_entry_set_text(GTK_ENTRY(entry), hotCorner->lowerLeftSavedCommand);
    } else if (g_strcmp0(name, "LOWER_RIGHT") == 0) {
        hotCorner->lowerRightCommand = entry;
        gtk_entry_set_text(GTK_ENTRY(entry), hotCorner->lowerRightSavedCommand);
    }

}

static void on_combo_box_changed(GtkComboBox * comboBox, HotCorner * hotCorner) {

    const gchar * name = gtk_widget_get_name(GTK_WIDGET(comboBox));
    if (g_strcmp0(name, "UPPER_LEFT") == 0) {
        ActionCallback callback = get_action_callback(comboBox, &(hotCorner->upperLeftActionID));
        hotCorner->upperLeftCallback = callback;
    } else if (g_strcmp0(name, "UPPER_RIGHT") == 0) {
        ActionCallback callback = get_action_callback(comboBox, &(hotCorner->upperRightActionID));
        hotCorner->upperRightCallback = callback;
    } else if (g_strcmp0(name, "LOWER_LEFT") == 0) {
        ActionCallback callback = get_action_callback(comboBox, &(hotCorner->lowerLeftActionID));
        hotCorner->lowerLeftCallback = callback;
    } else if (g_strcmp0(name, "LOWER_RIGHT") == 0) {
        ActionCallback callback = get_action_callback(comboBox, &(hotCorner->lowerRightActionID));
        hotCorner->lowerRightCallback = callback;
    }

}

// static GtkWidget * createComboBox(const gchar *name, HotCorner * hotCorner, gint actionID) {

//     // GtkWidget * vbox = gtk_vbox_new(FALSE, 5);
//     GtkWidget *vbox3;
//     GtkWidget *entry = gtk_entry_new();
//     GtkWidget *comboBox = gtk_combo_box_text_new();

//     vbox3 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);

//     gtk_widget_set_name(comboBox, name);
//     gtk_widget_set_name(entry, name);
//     gtk_entry_set_text (GTK_ENTRY (entry), "");


//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("-"));
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Xfdashboard"));
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Show Desktop"));
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Start Screensaver"));
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Turn Off Monitor"));
//     gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Custom Command"));

//     gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox), actionID);
    
//     gint selected = gtk_combo_box_get_active(GTK_COMBO_BOX(comboBox));
//     set_new_entry_visibility(GTK_COMBO_BOX(comboBox), entry);

//     gtk_box_pack_start(GTK_BOX(vbox3), comboBox, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(vbox3), entry, TRUE, FALSE, TRUE);

//     set_corner_command_entry(entry, hotCorner);

//     g_signal_connect (comboBox, "changed", G_CALLBACK(on_combo_box_changed), hotCorner);
//     g_signal_connect (comboBox, "changed", G_CALLBACK(set_new_entry_visibility), entry);

//     return vbox3;
// }


// static GtkWidget * create_layout(HotCorner * hotCorner) {

//     // GtkWidget * vbox = gtk_vbox_new(FALSE, 10);
//     GtkWidget *vbox;
//     // GtkWidget * row1 = gtk_hbox_new(FALSE, 10);
//     GtkWidget *row1;
//     // GtkWidget * row2 = gtk_hbox_new(FALSE, 10);
//     GtkWidget *row2;
//     // GtkWidget * row3 = gtk_hbox_new(FALSE, 10);
//     GtkWidget *row3;
    
//     vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
//     row1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
//     row2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
//     row3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);

//     GtkWidget * upperLeftCombo  = createComboBox("UPPER_LEFT", hotCorner, hotCorner->upperLeftActionID);
//     GtkWidget * upperRightCombo = createComboBox("UPPER_RIGHT", hotCorner, hotCorner->upperRightActionID);
//     GtkWidget * lowerLeftCombo  = createComboBox("LOWER_LEFT", hotCorner, hotCorner->lowerLeftActionID);
//     GtkWidget * lowerRightCombo = createComboBox("LOWER_RIGHT", hotCorner, hotCorner->lowerRightActionID);

//     GtkWidget * emptyLabel = gtk_label_new("");
//     GtkWidget * monitorImage = gtk_image_new_from_icon_name("xfce4-display", GTK_ICON_SIZE_BUTTON);

//     gtk_image_set_pixel_size(GTK_IMAGE(monitorImage), 256);

//     gtk_box_pack_start(GTK_BOX(row1), upperLeftCombo, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(row1), emptyLabel, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(row1), upperRightCombo, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(vbox), row1, FALSE, FALSE, FALSE);

//     gtk_box_pack_start(GTK_BOX(row2), emptyLabel, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(row2), monitorImage, TRUE, TRUE, TRUE);
//     gtk_box_pack_start(GTK_BOX(row2), emptyLabel, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(vbox), row2, FALSE, FALSE, FALSE);

//     gtk_box_pack_start(GTK_BOX(row3), lowerLeftCombo, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(row3), emptyLabel, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(row3), lowerRightCombo, TRUE, FALSE, TRUE);
//     gtk_box_pack_start(GTK_BOX(vbox), row3, FALSE, FALSE, FALSE);

//     return vbox;
// }

static void on_close_configure_window(GtkWidget * dialog, gint response, HotCorner * hotCorner) {
    xfce_panel_plugin_unblock_menu(hotCorner->plugin);
    save_config_file(hotCorner->plugin, hotCorner);
    gtk_widget_destroy(dialog);
}

static void setFullScreenDisableFlag(GtkToggleButton *toggleButton, HotCorner * hotCorner) {
    hotCorner->disableWhenFullScreen = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(toggleButton));
}


static void
time_out_about (XfcePanelPlugin *plugin)
{
  static const gchar *authors[] =
  {
    "Jannis Pohlmann <jannis@xfce.org>",
    "Florian Rivoal <frivoal@xfce.org>",
    NULL
  };

  gtk_show_about_dialog (GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (plugin))),
                         "authors", authors,
                         "comments", _("Xfce Panel plugin for taking a break from computer work every now and then."),
                         "destroy-with-parent", TRUE,
                         "logo-icon-name", "xfce4-time-out-plugin",
                         "program-name", PACKAGE_NAME,
                         "version", PACKAGE_VERSION,
                         "translator-credits", _("translator-credits"),
                         "license", XFCE_LICENSE_GPL,
                         "website", "http://goodies.xfce.org/projects/panel-plugins/xfce4-time-out-plugin",
                         NULL);
}


static void on_open_configure_window(XfcePanelPlugin *plugin, HotCorner *hotCorner) {
    
    GtkWidget *dialog;
    GtkWidget *framebox;
    GtkWidget *dlg = NULL, *header = NULL;
    GtkWidget *dialog_vbox;
    GtkWidget * disableInFullScreenCheckbox;
    GtkWidget *vbox;
    GtkWidget *row1;
    GtkWidget *row2;
    GtkWidget *row3;
    GtkWidget *upperLeftCombo;
    // GtkWidget *upperRightCombo;
    // GtkWidget *lowerLeftCombo;
    // GtkWidget *lowerRightCombo;
    GtkWidget *behaviourbin;

    // xfce_panel_plugin_block_menu (plugin);


    /* Block plugin context menu */
    xfce_panel_plugin_block_menu (plugin);

    /* Create properties dialog */
    #if LIBXFCE4UI_CHECK_VERSION (4,14,0)
    dialog = xfce_titled_dialog_new_with_mixed_buttons (_("HotCorner"),
        NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
        "window-close", _("_Close"), GTK_RESPONSE_OK,
        NULL);
    #else
    dialog = xfce_titled_dialog_new_with_buttons (_("HotCorner"),
        NULL, GTK_DIALOG_DESTROY_WITH_PARENT,
        "gtk-close", GTK_RESPONSE_OK,
        NULL);
    #endif

    /* Set dialog property */
    g_object_set_data (G_OBJECT (plugin), "dialog", dialog);

    // /* Be notified when the properties dialog is closed */
    g_signal_connect (dialog, "response", G_CALLBACK (on_close_configure_window), hotCorner);

    /* Basic dialog window setup */
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "xfce4-hotcorner-plugin");
    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);


    /* Create time settings section */
    framebox = xfce_gtk_frame_box_new (_("HotCorner settings"), &behaviourbin);
    gtk_container_set_border_width (GTK_CONTAINER (framebox), 6);
    gtk_box_pack_start (GTK_BOX (gtk_dialog_get_content_area (GTK_DIALOG (dialog))), framebox, TRUE, TRUE, 0);
    gtk_widget_show (framebox);

    /* Create behaviour box */
    vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 6);
    gtk_container_add (GTK_CONTAINER (behaviourbin), vbox);
    gtk_widget_show (vbox);


   // disableInFullScreenCheckbox = gtk_check_button_new_with_label(_("Disable when active window is full screen"));
    // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(disableInFullScreenCheckbox), hotCorner->disableWhenFullScreen);



    /* Create postpone check button */
    // checkbutton = gtk_check_button_new_with_label (_("Allow postpone"));
    // gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (checkbutton), time_out->allow_postpone);

    disableInFullScreenCheckbox = gtk_check_button_new_with_label(_("Disable when active window is full screen"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(disableInFullScreenCheckbox), hotCorner->disableWhenFullScreen);
    // g_signal_connect (checkbutton, "toggled", G_CALLBACK (time_out_allow_postpone_toggled), time_out);
    g_signal_connect (disableInFullScreenCheckbox, "toggled", G_CALLBACK(setFullScreenDisableFlag), hotCorner);

    gtk_container_add (GTK_CONTAINER (vbox), disableInFullScreenCheckbox);
    gtk_widget_show (disableInFullScreenCheckbox);




    // header = xfce_titled_dialog_new_with_buttons (
    //   _("HotCornerDani"),
    //   GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (hotCorner->plugin))),
    //   GTK_DIALOG_DESTROY_WITH_PARENT, _("Close"), GTK_RESPONSE_OK, NULL);
    
    // dlg = header;
    
    // gtk_window_set_icon_name (GTK_WINDOW (dlg), "xfce-hotcorner-plugin");

    // dialog_vbox = gtk_dialog_get_content_area(GTK_DIALOG(dlg));
    
    // vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 10);
    // gtk_box_pack_start (GTK_BOX (dlg), vbox, TRUE, TRUE, BORDER);



    // row1 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    // gtk_box_pack_start (GTK_BOX (vbox), row1, TRUE, TRUE, BORDER);

    // row2 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    // gtk_box_pack_start (GTK_BOX (vbox), row2, TRUE, TRUE, BORDER);
    
    // row3 = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 10);
    // gtk_box_pack_start (GTK_BOX (vbox), row3, TRUE, TRUE, BORDER);

    // // upperLeftCombo  = createComboBox("UPPER_LEFT", hotCorner, hotCorner->upperLeftActionID);
    // // upperRightCombo = createComboBox("UPPER_RIGHT", hotCorner, hotCorner->upperRightActionID);
    // // lowerLeftCombo  = createComboBox("LOWER_LEFT", hotCorner, hotCorner->lowerLeftActionID);
    // // lowerRightCombo = createComboBox("LOWER_RIGHT", hotCorner, hotCorner->lowerRightActionID);

    // // Ini funcion createComboBox
    // const gchar *name;
    // gint actionID;
    // name = "UPPER_LEFT";
    // actionID = hotCorner->upperLeftActionID;

    // GtkWidget *entry = gtk_entry_new();
    // GtkWidget *comboBox = gtk_combo_box_text_new();

    // upperLeftCombo = gtk_box_new (GTK_ORIENTATION_VERTICAL, 5);

    // gtk_widget_set_name(comboBox, name);
    // gtk_widget_set_name(entry, name);
    // gtk_entry_set_text (GTK_ENTRY (entry), "");


    // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("-"));
    // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Xfdashboard"));
    // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Show Desktop"));
    // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Start Screensaver"));
    // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Turn Off Monitor"));
    // gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox), _("Custom Command"));

    // gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox), actionID);
    
    // gint selected = gtk_combo_box_get_active(GTK_COMBO_BOX(comboBox));
    // set_new_entry_visibility(GTK_COMBO_BOX(comboBox), entry);

    // gtk_box_pack_start(GTK_BOX(upperLeftCombo), comboBox, TRUE, FALSE, TRUE);
    // gtk_box_pack_start(GTK_BOX(upperLeftCombo), entry, TRUE, FALSE, TRUE);

    // set_corner_command_entry(entry, hotCorner);

    // g_signal_connect (comboBox, "changed", G_CALLBACK(on_combo_box_changed), hotCorner);
    // g_signal_connect (comboBox, "changed", G_CALLBACK(set_new_entry_visibility), entry);
    // // fin funcion createComboBox



    // GtkWidget * emptyLabel = gtk_label_new("");
    // GtkWidget * monitorImage = gtk_image_new_from_icon_name("xfce4-display", GTK_ICON_SIZE_BUTTON);

    // gtk_image_set_pixel_size(GTK_IMAGE(monitorImage), 256);

    // gtk_box_pack_start(GTK_BOX(row1), upperLeftCombo, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(row1), emptyLabel, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(row1), upperRightCombo, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(vbox), row1, FALSE, FALSE, FALSE);

    // gtk_box_pack_start(GTK_BOX(row2), emptyLabel, TRUE, FALSE, TRUE);
    // gtk_box_pack_start(GTK_BOX(row2), monitorImage, TRUE, TRUE, TRUE);
    // gtk_box_pack_start(GTK_BOX(row2), emptyLabel, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(vbox), row2, FALSE, FALSE, FALSE);

    // // gtk_box_pack_start(GTK_BOX(row3), lowerLeftCombo, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(row3), emptyLabel, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(row3), lowerRightCombo, TRUE, FALSE, TRUE);
    // // gtk_box_pack_start(GTK_BOX(vbox), row3, FALSE, FALSE, FALSE);


    // // fin create layout

    // disableInFullScreenCheckbox = gtk_check_button_new_with_label(_("Disable when active window is full screen"));
    // gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(disableInFullScreenCheckbox), hotCorner->disableWhenFullScreen);

    // gtk_box_pack_start(GTK_BOX(vbox), vbox, TRUE, FALSE, TRUE);
    // gtk_box_pack_start(GTK_BOX(vbox), disableInFullScreenCheckbox, TRUE, FALSE, TRUE);

    // gtk_window_set_position (GTK_WINDOW (dlg), GTK_WIN_POS_CENTER);
    // gtk_window_set_icon_name (GTK_WINDOW (dlg), "xfce4-display");

    // g_signal_connect (dlg, "response", G_CALLBACK(on_close_configure_window), hotCorner);
    // g_signal_connect (disableInFullScreenCheckbox, "toggled", G_CALLBACK(setFullScreenDisableFlag), hotCorner);

    // gtk_widget_show_all(dlg);
    // gtk_widget_show_all (GTK_WIDGET (dlg));
    
    /* Show the entire dialog */
    gtk_widget_show (dialog);

}




static void on_screen_changed(GtkWidget * widget, GdkScreen * previous_screen, HotCorner * hotCorner) {
    set_monitor_size(hotCorner);
}

static void constructor(XfcePanelPlugin *plugin) {

    // HotCorner * hotCorner = hotCorner_new(plugin);
    HotCorner *hotCorner;
    /* create the plugin */
    hotCorner = hotCorner_new (plugin);
    xfce_textdomain(GETTEXT_PACKAGE, LOCALEDIR, "UTF-8");

    // set_monitor_size(hotCorner);

    xfce_panel_plugin_menu_show_configure (plugin);

    g_print ("proba10\n");
    WnckScreen *screenwnck = wnck_screen_get_default ();
    g_print ("proba20\n");

    /* Display the about menu item */
    // xfce_panel_plugin_menu_show_about (plugin);

    g_print ("proba30\n");
    GdkScreen * screen = gdk_screen_get_default();
    g_print ("proba40\n");
    g_signal_connect (plugin, "free-data", G_CALLBACK(free_data), hotCorner);
    g_signal_connect (plugin, "configure-plugin", G_CALLBACK(on_open_configure_window), hotCorner);
    // g_signal_connect (screen, "monitors-changed", G_CALLBACK(on_screen_changed), hotCorner);
    // g_signal_connect (plugin, "save", G_CALLBACK(save_config_file), hotCorner);
    g_signal_connect (G_OBJECT (plugin), "about", G_CALLBACK (time_out_about), NULL);


    // read_config_file(plugin, hotCorner);

    xfce_panel_plugin_set_expand (XFCE_PANEL_PLUGIN(plugin), FALSE);

}

XFCE_PANEL_PLUGIN_REGISTER(constructor);

