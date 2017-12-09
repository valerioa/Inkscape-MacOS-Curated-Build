#ifndef __OSX_INTEGRATION_H__
#define __OSX_INTEGRATION_H__

/*
 * Main UI stuff
 *
 * Authors:
 *   Valerio Aimale <valerio@aimale.com>
 *
 * Copyright (C) 2013 authors
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <gtkmacintegration/gtkosxapplication.h>
#include "widgets/desktop-widget.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct _InkscapeOsxIntegration {
	GtkosxApplication *pGtkOsxApplication;
	GtkWidget *menubar;
	GtkWidget *about_item, *prefs_item;
	SPDesktopWidget *dtw_infocus;
	
} InkscapeOsxIntegration;


extern InkscapeOsxIntegration sInkscapeOsxIntegration;
extern GtkWidget *osx_integration_menuitems[4];

void OSX_set_menubar( SPDesktopWidget *menubar);
void   init_Mac_OSX_Integration();
void   init_Mac_OSX_Integration_complete();

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __OSX_INTEGRATION_H__ */
