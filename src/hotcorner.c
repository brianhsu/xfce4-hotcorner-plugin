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

#include <stdio.h>
#include "hotcorner.h"
#include "util.h"

#define UPPER_LEFT  0
#define UPPER_RIGHT 1
#define LOWER_LEFT  2
#define LOWER_RIGHT 3

struct command_text_callback_map_entry {
    const char *entry_text;
    ActionCallback entry_cb;
};

void run_custom_command(int spot, HotCorner * hotCorner);

struct command_text_callback_map_entry command_text_callback_map[] = {
    {"-", NULL},
    {"Xfdashboard", start_dashboard},
    {"Start Screensaver", start_screensaver},
    {"Turn Off Monitor", turn_off_monitor},
    {"Custom Command", run_custom_command},
};

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

    if (index >= ARRAY_SIZEOF(command_text_callback_map)) {
        return NULL;
    }

    return command_text_callback_map[index].entry_cb;
}

static void read_config_file(XfcePanelPlugin * plugin, HotCorner * hotCorner) {
    gchar  *file = xfce_panel_plugin_lookup_rc_file(plugin);

    if (file)
    {
        XfceRc *rc = xfce_rc_simple_open (file, TRUE);

        if (rc != NULL)
        {
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
    GdkScreen * screen = gtk_widget_get_screen(hotCorner->icon);
    GdkWindow * window = gdk_screen_get_root_window(screen);
    gint x, y;
    gdk_window_get_pointer(window, &x, &y, NULL);

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
    hotCorner->icon = gtk_image_new_from_icon_name("video-display", GTK_ICON_SIZE_BUTTON);
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
    GdkScreen * screen = gdk_screen_get_default();
    GdkRectangle monitorInfo;
    gint monitorID = gdk_screen_get_primary_monitor(screen);
    gdk_screen_get_monitor_geometry (screen, monitorID, &monitorInfo);
    hotCorner->monitorInfo = monitorInfo;
}

ActionCallback get_action_callback(GtkComboBox * comboBox, int * outSelected) {
    gint selected = gtk_combo_box_get_active(comboBox);
    return get_action_callback_from_index(selected, outSelected);
}

static void set_new_entry_visibility(GtkComboBox * comboBox, GtkWidget * entry) {
    gint selected = gtk_combo_box_get_active(comboBox);

    if (command_text_callback_map[selected].entry_cb != run_custom_command) {
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

static GtkWidget * createComboBox(const gchar *name, HotCorner * hotCorner,
                                  gint actionID) {
    GtkWidget * vbox = gtk_vbox_new(FALSE,
                                    ARRAY_SIZEOF(command_text_callback_map));
    GtkWidget * entry = gtk_entry_new();
    GtkWidget * comboBox = gtk_combo_box_text_new();
    size_t i;

    gtk_widget_set_name(comboBox, name);
    gtk_widget_set_name(entry, name);

    for (i = 0; i < ARRAY_SIZEOF(command_text_callback_map); i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(comboBox),
                                       command_text_callback_map[i].entry_text);
    }

    gtk_combo_box_set_active(GTK_COMBO_BOX(comboBox), actionID);
    
    set_new_entry_visibility(GTK_COMBO_BOX(comboBox), entry);

    gtk_box_pack_start(GTK_BOX(vbox), comboBox, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), entry, TRUE, FALSE, TRUE);

    set_corner_command_entry(entry, hotCorner);

    g_signal_connect (comboBox, "changed", G_CALLBACK(on_combo_box_changed), hotCorner);
    g_signal_connect (comboBox, "changed", G_CALLBACK(set_new_entry_visibility), entry);

    return vbox;
}


static GtkWidget * create_layout(HotCorner * hotCorner) {

    GtkWidget * vbox = gtk_vbox_new(FALSE, 10);
    GtkWidget * row1 = gtk_hbox_new(FALSE, 10);
    GtkWidget * row2 = gtk_hbox_new(FALSE, 10);
    GtkWidget * row3 = gtk_hbox_new(FALSE, 10);

    GtkWidget * upperLeftCombo  = createComboBox("UPPER_LEFT", hotCorner, hotCorner->upperLeftActionID);
    GtkWidget * upperRightCombo = createComboBox("UPPER_RIGHT", hotCorner, hotCorner->upperRightActionID);
    GtkWidget * lowerLeftCombo  = createComboBox("LOWER_LEFT", hotCorner, hotCorner->lowerLeftActionID);
    GtkWidget * lowerRightCombo = createComboBox("LOWER_RIGHT", hotCorner, hotCorner->lowerRightActionID);

    GtkWidget * emptyLabel = gtk_label_new("");
    GtkWidget * monitorImage = gtk_image_new_from_icon_name("video-display", GTK_ICON_SIZE_BUTTON);

    gtk_image_set_pixel_size(GTK_IMAGE(monitorImage), 256);

    gtk_box_pack_start(GTK_BOX(row1), upperLeftCombo, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row1), emptyLabel, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row1), upperRightCombo, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), row1, FALSE, FALSE, FALSE);

    gtk_box_pack_start(GTK_BOX(row2), emptyLabel, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row2), monitorImage, TRUE, TRUE, TRUE);
    gtk_box_pack_start(GTK_BOX(row2), emptyLabel, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), row2, FALSE, FALSE, FALSE);

    gtk_box_pack_start(GTK_BOX(row3), lowerLeftCombo, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row3), emptyLabel, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row3), lowerRightCombo, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), row3, FALSE, FALSE, FALSE);

    return vbox;
}

static void on_close_configure_window(GtkWidget * dialog, gint response, HotCorner * hotCorner) {
    xfce_panel_plugin_unblock_menu(hotCorner->plugin);
    save_config_file(hotCorner->plugin, hotCorner);
    gtk_widget_destroy(dialog);
}



static void on_open_configure_window(XfcePanelPlugin * plugin, HotCorner * hotCorner) {

    xfce_panel_plugin_block_menu (plugin);
    GtkWidget * dialog = xfce_titled_dialog_new_with_buttons (
        _("HotCorner"),
        GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (hotCorner->plugin))),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_STOCK_CLOSE, GTK_RESPONSE_OK, NULL
    );

    GtkWidget * vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget * content = create_layout(hotCorner);

    gtk_box_pack_start(GTK_BOX(vbox), content, TRUE, FALSE, TRUE);

    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "video-display");

    g_signal_connect (dialog, "response", G_CALLBACK(on_close_configure_window), hotCorner);

    gtk_widget_show_all(dialog);

}


static void on_screen_changed(GtkWidget * widget, GdkScreen * previous_screen, HotCorner * hotCorner) {
    set_monitor_size(hotCorner);
}

static void constructor(XfcePanelPlugin *plugin) {
    HotCorner * hotCorner = hotCorner_new(plugin);
    xfce_textdomain(GETTEXT_PACKAGE, LOCALEDIR, "UTF-8");
    set_monitor_size(hotCorner);
    xfce_panel_plugin_menu_show_configure (plugin);


    GdkScreen * screen = gdk_screen_get_default();
    g_signal_connect (plugin, "free-data", G_CALLBACK(free_data), hotCorner);
    g_signal_connect (plugin, "configure-plugin", G_CALLBACK(on_open_configure_window), hotCorner);
    g_signal_connect (screen, "monitors-changed", G_CALLBACK(on_screen_changed), hotCorner);
    g_signal_connect (plugin, "save", G_CALLBACK(save_config_file), hotCorner);


    read_config_file(plugin, hotCorner);

    xfce_panel_plugin_set_expand (XFCE_PANEL_PLUGIN(plugin), FALSE);

}

XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(constructor);

