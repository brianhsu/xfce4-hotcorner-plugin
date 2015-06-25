#ifndef __HOTCORNER_PLUGIN__
#define __HOTCORNER_PLUGIN__

#include <libxfce4panel/xfce-panel-plugin.h>
#include <libxfce4ui/libxfce4ui.h>

#define UPDATE_TIMEOUT 100

typedef struct HotCorner HotCorner;

struct HotCorner {

  XfcePanelPlugin * plugin;

  guint timeout_id;
  gboolean isExecuted;

  GtkWidget * icon;

  GdkRectangle monitorInfo;
  void (* upperLeftCallback)(HotCorner * hotCorner);
  void (* lowerLeftCallback)(HotCorner * hotCorner);
  void (* upperRightCallback)(HotCorner * hotCorner);
  void (* lowerRightCallback)(HotCorner * hotCorner);

};

#endif
