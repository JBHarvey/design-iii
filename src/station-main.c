#include <gtk/gtk.h>

#include "ui_app.h"

int main(int argc, char *argv[])
{
    return g_application_run(G_APPLICATION(ui_app_new()), argc, argv);
}
