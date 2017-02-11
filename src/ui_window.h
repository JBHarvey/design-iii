#ifndef __UIWINDOW_H
#define __UIWINDOW_H

#include <gtk/gtk.h>
#include "ui_app.h"


#define UI_WINDOW_TYPE (ui_window_get_type ())
G_DECLARE_FINAL_TYPE (UiWindow, ui_window, UI, WINDOW, GtkApplicationWindow)


UiWindow    *ui_window_new              (UiApp *app);
void        ui_window_open              (UiWindow *window,
                                        GFile *file);


#endif /* __UIWINDOW_H */

