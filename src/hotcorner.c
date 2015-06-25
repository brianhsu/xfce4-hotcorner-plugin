#include <stdio.h>
#include "hotcorner.h"
#include "util.h"

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
    GdkDisplay * display = gtk_widget_get_display(hotCorner->icon);
    gint x, y;
    gdk_window_get_pointer(window, &x, &y, NULL);

    if (is_upper_left(hotCorner->monitorInfo, x, y) && hotCorner->upperLeftCallback != NULL) {
        if (!hotCorner->isExecuted) {
            hotCorner->upperLeftCallback(hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else if (is_upper_right(hotCorner->monitorInfo, x, y) && hotCorner->upperRightCallback != NULL) {
        if (!hotCorner->isExecuted) {
            hotCorner->upperRightCallback(hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else if (is_lower_right(hotCorner->monitorInfo, x, y) && hotCorner->lowerRightCallback != NULL) {
        if (!hotCorner->isExecuted) {
            hotCorner->lowerRightCallback(hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else if (is_lower_left(hotCorner->monitorInfo, x, y) && hotCorner->lowerLeftCallback != NULL) {
        if (!hotCorner->isExecuted) {
            hotCorner->lowerLeftCallback(hotCorner);
            hotCorner->isExecuted = TRUE;
        }
    } else {
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
    hotCorner->upperLeftCallback = start_dashboard;
    hotCorner->lowerLeftCallback = toggle_desktop;
    hotCorner->upperRightCallback = start_screensaver;
    hotCorner->lowerRightCallback = turn_off_monitor;

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


static GtkWidget * create_layout() {
    GtkWidget * vbox = gtk_vbox_new(FALSE, 10);
    GtkWidget * row1 = gtk_hbox_new(FALSE, 10);
    GtkWidget * row2 = gtk_hbox_new(FALSE, 10);
    GtkWidget * row3 = gtk_hbox_new(FALSE, 10);

    GtkWidget * label1 = gtk_label_new("UpperLeft");
    GtkWidget * label2 = gtk_label_new("");
    GtkWidget * label3 = gtk_label_new("UpperRight");
    GtkWidget * label4 = gtk_label_new("");
    GtkWidget * label5 = gtk_label_new("");
    GtkWidget * label6 = gtk_label_new("");
    GtkWidget * label7 = gtk_label_new("DownLeft");
    GtkWidget * label8 = gtk_label_new("");
    GtkWidget * label9 = gtk_label_new("DownRight");
    GtkWidget * monitorImage = gtk_image_new_from_icon_name("xfce4-display", GTK_ICON_SIZE_BUTTON);

    gtk_image_set_pixel_size(GTK_IMAGE(monitorImage), 256);

    gtk_box_pack_start(GTK_BOX(row1), label1, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row1), label2, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row1), label3, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), row1, FALSE, FALSE, FALSE);

    gtk_box_pack_start(GTK_BOX(row2), label4, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row2), monitorImage, TRUE, TRUE, TRUE);
    gtk_box_pack_start(GTK_BOX(row2), label6, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), row2, FALSE, FALSE, FALSE);

    gtk_box_pack_start(GTK_BOX(row3), label7, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row3), label8, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(row3), label9, TRUE, FALSE, TRUE);
    gtk_box_pack_start(GTK_BOX(vbox), row3, FALSE, FALSE, FALSE);

    return vbox;
}

static void on_close_configure_window(GtkWidget * dialog, gint response, HotCorner * hotCorner) {
    xfce_panel_plugin_unblock_menu(hotCorner->plugin);
    gtk_widget_destroy(dialog);
}

static void on_open_configure_window(XfcePanelPlugin * plugin, HotCorner * hotCorner) {
    xfce_panel_plugin_block_menu (plugin);
    GtkWidget * dialog = xfce_titled_dialog_new_with_buttons (
        "HotCorner",
        GTK_WINDOW (gtk_widget_get_toplevel (GTK_WIDGET (hotCorner->plugin))),
        GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_STOCK_CLOSE, GTK_RESPONSE_OK, NULL
    );

    GtkWidget * vbox = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget * content = create_layout();

    gtk_box_pack_start(GTK_BOX(vbox), content, TRUE, FALSE, TRUE);

    gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
    gtk_window_set_icon_name (GTK_WINDOW (dialog), "xfce4-display");

    g_signal_connect (dialog, "response", G_CALLBACK(on_close_configure_window), hotCorner);

    gtk_widget_show_all(dialog);

}

static void on_screen_changed(GtkWidget * widget, GdkScreen * previous_screen, HotCorner * hotCorner) {
    set_monitor_size(hotCorner);
}

static void constructor(XfcePanelPlugin *plugin) {
    HotCorner * hotCorner = hotCorner_new(plugin);

    set_monitor_size(hotCorner);
    xfce_panel_plugin_menu_show_configure (plugin);

    GdkScreen * screen = gdk_screen_get_default();
    g_signal_connect (plugin, "free-data", G_CALLBACK(free_data), hotCorner);
    g_signal_connect (plugin, "configure-plugin", G_CALLBACK(on_open_configure_window), hotCorner);
    g_signal_connect (screen, "monitors-changed", G_CALLBACK(on_screen_changed), hotCorner);


    xfce_panel_plugin_set_expand (XFCE_PANEL_PLUGIN(plugin), TRUE);
}

XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(constructor);

