/** @file
 * @brief Integration with Mac OS X Desktop, Launcher and UI in general
 */
/* Authors:
 *   Valerio Aimale <valerio@aimale.com>
 *
 * Copyright (C) 2013 authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */
#ifdef __APPLE__

#include <gtkmacintegration/gtkosxapplication.h>
#include "osx-integration.h"

#include <ui/interface.h>
#include <file.h>

#ifdef __cplusplus
// extern "C" {
#endif /* __cplusplus */

static gboolean
osx_quartz_quit (GtkosxApplication *app, gpointer data);
static gboolean
osx_quartz_block_termination (GtkosxApplication *app, gpointer data);

InkscapeOsxIntegration sInkscapeOsxIntegration = {0};

GtkWidget *osx_integration_menuitems[4];

static gboolean
osx_quartz_block_termination (GtkosxApplication *app, gpointer data)
{
	return !sp_ui_close_all();
}


static gboolean
osx_quartz_quit (GtkosxApplication *app, gpointer data)
{
	sp_ui_close_all();
	g_print("Got here");
	gtk_main_quit();
	return FALSE;
}

static gboolean
osx_open_file (GtkosxApplication *app, gchar *path ,gpointer data)
{
	Glib::ustring ustr = Glib::ustring(path);
	sp_file_open(ustr, NULL, true, false);
	return true;
}

	
void   init_Mac_OSX_Integration() {
	sInkscapeOsxIntegration.pGtkOsxApplication  = (GtkosxApplication*) g_object_new(GTKOSX_TYPE_APPLICATION, NULL);
	//gtkosx_application_set_use_quartz_accelerators(sInkscapeOsxIntegration.pGtkOsxApplication, FALSE);
}

void   init_Mac_OSX_Integration_complete() {
	gtkosx_application_ready(sInkscapeOsxIntegration.pGtkOsxApplication);
	g_signal_connect(sInkscapeOsxIntegration.pGtkOsxApplication, "NSApplicationBlockTermination",
	                 G_CALLBACK(osx_quartz_block_termination), NULL);
	g_signal_connect(sInkscapeOsxIntegration.pGtkOsxApplication, "NSApplicationWillTerminate",
	                 G_CALLBACK(osx_quartz_block_termination), NULL);
	g_signal_connect(sInkscapeOsxIntegration.pGtkOsxApplication, "NSApplicationOpenFile",
	                 G_CALLBACK(osx_open_file), NULL);
	gtk_accel_map_save("/tmp/inkscape.map");
}

void OSX_set_menubar(SPDesktopWidget *dtw) {
        gtkosx_application_set_menu_bar(sInkscapeOsxIntegration.pGtkOsxApplication, GTK_MENU_SHELL (dtw->menubar));

        if (dtw->about_item) {
            GtkWidget *sep = gtk_separator_menu_item_new();
            gtkosx_application_insert_app_menu_item  (sInkscapeOsxIntegration.pGtkOsxApplication,
                                                      dtw->about_item,
                                                      0);
            g_object_ref(sep);
            gtkosx_application_insert_app_menu_item  (sInkscapeOsxIntegration.pGtkOsxApplication, sep, 1);
        }
        if (dtw->prefs_item) {
            gtkosx_application_insert_app_menu_item  (sInkscapeOsxIntegration.pGtkOsxApplication,
                                                      dtw->prefs_item,
                                                      2);
        }

}

#ifdef __cplusplus
//}
#endif /* __cplusplus */

#endif /* __APPLE__ */
