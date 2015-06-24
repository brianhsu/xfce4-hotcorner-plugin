#include <libxfce4panel/xfce-panel-plugin.h>
#include <stdio.h>

#define UPDATE_TIMEOUT 100

guint count;
typedef struct {
  guint timeout_id;
  XfcePanelPlugin * plugin;
  GtkWidget * label;
} HotCorner;

static void constructor(XfcePanelPlugin *plugin);
XFCE_PANEL_PLUGIN_REGISTER_INTERNAL(constructor);

static void free_data(XfcePanelPlugin *plugin, HotCorner * hot_corner) {
    if (hot_corner->timeout_id != 0) {
        g_source_remove (hot_corner->timeout_id);
    }

    gtk_widget_destroy(hot_corner->label);
    g_free(hot_corner);
}

static gint timer_cb(HotCorner * hot_corner) {
    count++;
    printf("count:%d\n", count);
    return TRUE;
}

static void start_monitor(HotCorner * hot_corner) {
    if (hot_corner->timeout_id == 0)
    {
        hot_corner->timeout_id = g_timeout_add (UPDATE_TIMEOUT, (GSourceFunc)timer_cb, hot_corner);
    }
}

static HotCorner * hot_corner_new(XfcePanelPlugin *plugin) {
    HotCorner * hot_corner = g_new0(HotCorner, 1);
    hot_corner->plugin = plugin;
    hot_corner->label = gtk_label_new("AA");
    gtk_container_add(GTK_CONTAINER(hot_corner->plugin), hot_corner->label);
    gtk_widget_show_all(hot_corner->label);
    start_monitor(hot_corner);
    return hot_corner;
}

static void constructor(XfcePanelPlugin *plugin) {
    HotCorner * hot_corner = hot_corner_new(plugin);
    g_signal_connect (plugin, "free-data", G_CALLBACK (free_data), hot_corner);
    xfce_panel_plugin_set_expand (XFCE_PANEL_PLUGIN(plugin), TRUE);
}

