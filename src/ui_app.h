#ifndef __UIAPP_H
#define __UIAPP_H

#include <gtk/gtk.h>

#define UI_APP_TYPE (ui_app_get_type ())
G_DECLARE_FINAL_TYPE (UiApp, ui_app, UI, APP, GtkApplication)

UiApp     *ui_app_new         (void);


#endif /* __UIAPP_H */

