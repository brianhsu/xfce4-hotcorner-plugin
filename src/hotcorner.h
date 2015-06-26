#ifndef __HOTCORNER_PLUGIN__
#define __HOTCORNER_PLUGIN__

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>

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
