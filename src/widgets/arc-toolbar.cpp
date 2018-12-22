/**
 * @file
 * Arc aux toolbar
 */
/* Authors:
 *   MenTaLguY <mental@rydia.net>
 *   Lauris Kaplinski <lauris@kaplinski.com>
 *   bulia byak <buliabyak@users.sf.net>
 *   Frank Felfe <innerspace@iname.com>
 *   John Cliff <simarilius@yahoo.com>
 *   David Turner <novalis@gnu.org>
 *   Josh Andler <scislac@scislac.com>
 *   Jon A. Cruz <jon@joncruz.org>
 *   Maximilian Albert <maximilian.albert@gmail.com>
 *   Tavmjong Bah <tavmjong@free.fr>
 *   Abhishek Sharma
 *   Kris De Gussem <Kris.DeGussem@gmail.com>
 *
 * Copyright (C) 2004 David Turner
 * Copyright (C) 2003 MenTaLguY
 * Copyright (C) 1999-2011 authors
 * Copyright (C) 2001-2002 Ximian, Inc.
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <glibmm/i18n.h>
#include <2geom/transforms.h>

#include "arc-toolbar.h"

#include "desktop.h"
#include "document.h"
#include "document-undo.h"
#include "widgets/ege-adjustment-action.h"
#include "widgets/ege-output-action.h"
#include "widgets/ege-select-one-action.h"
#include "widgets/ink-action.h"
#include "mod360.h"
#include "preferences.h"
#include "selection.h"
#include "sp-ellipse.h"
#include "toolbox.h"
#include "ui/icon-names.h"
#include "ui/uxmanager.h"
#include "ui/widget/unit-tracker.h"
#include "ui/tools/arc-tool.h"
#include "verbs.h"
#include "widgets/spinbutton-events.h"
#include "widgets/widget-sizes.h"
#include "xml/node-event-vector.h"
#include "xml/repr.h"

using Inkscape::UI::Widget::UnitTracker;
using Inkscape::UI::UXManager;
using Inkscape::DocumentUndo;
using Inkscape::UI::ToolboxFactory;
using Inkscape::UI::PrefPusher;
using Inkscape::Util::Quantity;
using Inkscape::Util::unit_table;

//########################
//##    Circle / Arc    ##
//########################

static void sp_arctb_sensitivize( GObject *tbl, double v1, double v2 )
{
    GtkAction *ocb = GTK_ACTION( g_object_get_data( tbl, "open_action" ) );
    GtkAction *make_whole = GTK_ACTION( g_object_get_data( tbl, "make_whole" ) );

    if (v1 == 0 && v2 == 0) {
        if (g_object_get_data( tbl, "single" )) { // only for a single selected ellipse (for now)
            gtk_action_set_sensitive( ocb, FALSE );
            gtk_action_set_sensitive( make_whole, FALSE );
        }
    } else {
        gtk_action_set_sensitive( ocb, TRUE );
        gtk_action_set_sensitive( make_whole, TRUE );
    }
}

static void sp_arctb_value_changed(GtkAdjustment *adj, GObject *tbl, gchar const *value_name)
{
    // Per SVG spec "a [radius] value of zero disables rendering of the element".
    // However our implementation does not allow a setting of zero in the UI (not even in the XML editor)
    // and ugly things happen if it's forced here, so better leave the properties untouched.
    if (!gtk_adjustment_get_value(adj)) {
        return;
    }

    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( tbl, "desktop" ));

    UnitTracker* tracker = reinterpret_cast<UnitTracker*>(g_object_get_data( tbl, "tracker" ));
    Unit const *unit = tracker->getActiveUnit();
    g_return_if_fail(unit != NULL);

    SPDocument* document = desktop->getDocument();
    Geom::Scale scale = document->getDocumentScale();

    if (DocumentUndo::getUndoSensitive(document)) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setDouble(Glib::ustring("/tools/shapes/arc/") + value_name,
            Quantity::convert(gtk_adjustment_get_value(adj), unit, "px"));
    }

    // quit if run by the attr_changed listener
    if (g_object_get_data( tbl, "freeze" ) || tracker->isUpdating()) {
        return;
    }

    // in turn, prevent listener from responding
    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(TRUE));

    bool modmade = false;
    Inkscape::Selection *selection = desktop->getSelection();
    std::vector<SPItem*> itemlist=selection->itemList();
    for(std::vector<SPItem*>::const_iterator i=itemlist.begin();i!=itemlist.end();++i){
        SPItem *item = *i;
        if (SP_IS_GENERICELLIPSE(item)) {

            SPGenericEllipse *ge = SP_GENERICELLIPSE(item);

            if (!strcmp(value_name, "rx")) {
                ge->setVisibleRx(Quantity::convert(gtk_adjustment_get_value(adj), unit, "px"));
            } else {
                ge->setVisibleRy(Quantity::convert(gtk_adjustment_get_value(adj), unit, "px"));
            }

            ge->normalize();
            (SP_OBJECT(ge))->updateRepr();
            (SP_OBJECT(ge))->requestDisplayUpdate(SP_OBJECT_MODIFIED_FLAG);

            modmade = true;
        }
    }

    if (modmade) {
        DocumentUndo::done(desktop->getDocument(), SP_VERB_CONTEXT_ARC,
                           _("Change arc"));
    }

    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(FALSE) );
}

static void sp_arctb_rx_value_changed(GtkAdjustment *adj, GObject *tbl)
{
    sp_arctb_value_changed(adj, tbl, "rx");
}

static void sp_arctb_ry_value_changed(GtkAdjustment *adj, GObject *tbl)
{
    sp_arctb_value_changed(adj, tbl, "ry");
}

static void
sp_arctb_startend_value_changed(GtkAdjustment *adj, GObject *tbl, gchar const *value_name, gchar const *other_name)
{
    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( tbl, "desktop" ));

    if (DocumentUndo::getUndoSensitive(desktop->getDocument())) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setDouble(Glib::ustring("/tools/shapes/arc/") + value_name, gtk_adjustment_get_value(adj));
    }

    // quit if run by the attr_changed listener
    if (g_object_get_data( tbl, "freeze" )) {
        return;
    }

    // in turn, prevent listener from responding
    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(TRUE) );

    gchar* namespaced_name = g_strconcat("sodipodi:", value_name, NULL);

    bool modmade = false;
    std::vector<SPItem*> itemlist=desktop->getSelection()->itemList();
    for(std::vector<SPItem*>::const_iterator i=itemlist.begin();i!=itemlist.end();++i){
        SPItem *item = *i;
        if (SP_IS_GENERICELLIPSE(item)) {

            SPGenericEllipse *ge = SP_GENERICELLIPSE(item);

            if (!strcmp(value_name, "start")) {
                ge->start = (gtk_adjustment_get_value(adj) * M_PI)/ 180;
            } else {
                ge->end = (gtk_adjustment_get_value(adj) * M_PI)/ 180;
            }

            ge->normalize();
            (SP_OBJECT(ge))->updateRepr();
            (SP_OBJECT(ge))->requestDisplayUpdate(SP_OBJECT_MODIFIED_FLAG);

            modmade = true;
        }
    }

    g_free(namespaced_name);

    GtkAdjustment *other = GTK_ADJUSTMENT( g_object_get_data( tbl, other_name ) );

    sp_arctb_sensitivize( tbl, gtk_adjustment_get_value(adj), gtk_adjustment_get_value(other) );

    if (modmade) {
        DocumentUndo::maybeDone(desktop->getDocument(), value_name, SP_VERB_CONTEXT_ARC,
                                _("Arc: Change start/end"));
    }

    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(FALSE) );
}


static void sp_arctb_start_value_changed(GtkAdjustment *adj,  GObject *tbl)
{
    sp_arctb_startend_value_changed(adj,  tbl, "start", "end");
}

static void sp_arctb_end_value_changed(GtkAdjustment *adj, GObject *tbl)
{
    sp_arctb_startend_value_changed(adj,  tbl, "end", "start");
}


static void sp_arctb_open_state_changed( EgeSelectOneAction *act, GObject *tbl )
{
    SPDesktop *desktop = static_cast<SPDesktop *>(g_object_get_data( tbl, "desktop" ));
    if (DocumentUndo::getUndoSensitive(desktop->getDocument())) {
        Inkscape::Preferences *prefs = Inkscape::Preferences::get();
        prefs->setBool("/tools/shapes/arc/open", ege_select_one_action_get_active(act) != 0);
    }

    // quit if run by the attr_changed listener
    if (g_object_get_data( tbl, "freeze" )) {
        return;
    }

    // in turn, prevent listener from responding
    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(TRUE) );

    bool modmade = false;

    if ( ege_select_one_action_get_active(act) != 0 ) {
    	std::vector<SPItem*> itemlist=desktop->getSelection()->itemList();
        for(std::vector<SPItem*>::const_iterator i=itemlist.begin();i!=itemlist.end();++i){
            SPItem *item = *i;
            if (SP_IS_GENERICELLIPSE(item)) {
                Inkscape::XML::Node *repr = item->getRepr();
                repr->setAttribute("sodipodi:open", "true");
                item->updateRepr();
                modmade = true;
            }
        }
    } else {
    	std::vector<SPItem*> itemlist=desktop->getSelection()->itemList();
        for(std::vector<SPItem*>::const_iterator i=itemlist.begin();i!=itemlist.end();++i){
            SPItem *item = *i;
            if (SP_IS_GENERICELLIPSE(item)) {
                Inkscape::XML::Node *repr = item->getRepr();
                repr->setAttribute("sodipodi:open", NULL);
                item->updateRepr();
                modmade = true;
            }
        }
    }

    if (modmade) {
        DocumentUndo::done(desktop->getDocument(), SP_VERB_CONTEXT_ARC,
                           _("Arc: Change open/closed"));
    }

    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(FALSE) );
}

static void sp_arctb_defaults(GtkWidget *, GObject *obj)
{
    GtkAdjustment *adj;

    adj = GTK_ADJUSTMENT( g_object_get_data(obj, "start") );
    gtk_adjustment_set_value(adj, 0.0);
    gtk_adjustment_value_changed(adj);

    adj = GTK_ADJUSTMENT( g_object_get_data(obj, "end") );
    gtk_adjustment_set_value(adj, 0.0);
    gtk_adjustment_value_changed(adj);

    spinbutton_defocus(GTK_WIDGET(obj));
}

static void arc_tb_event_attr_changed(Inkscape::XML::Node *repr, gchar const * /*name*/,
                                      gchar const * /*old_value*/, gchar const * /*new_value*/,
                                      bool /*is_interactive*/, gpointer data)
{
    GObject *tbl = G_OBJECT(data);

    // quit if run by the _changed callbacks
    if (g_object_get_data( tbl, "freeze" )) {
        return;
    }

    // in turn, prevent callbacks from responding
    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(TRUE) );

    gpointer item = g_object_get_data( tbl, "item" );
    if (item && SP_IS_GENERICELLIPSE(item)) {
        SPGenericEllipse *ge = SP_GENERICELLIPSE(item);

        UnitTracker* tracker = reinterpret_cast<UnitTracker*>( g_object_get_data( tbl, "tracker" ) );
        Unit const *unit = tracker->getActiveUnit();
        g_return_if_fail(unit != NULL);

        GtkAdjustment *adj;
        adj = GTK_ADJUSTMENT( g_object_get_data(tbl, "rx") );
        gdouble rx = ge->getVisibleRx();
        gtk_adjustment_set_value(adj, Quantity::convert(rx, "px", unit));
        gtk_adjustment_value_changed(adj);

        adj = GTK_ADJUSTMENT( g_object_get_data(tbl, "ry") );
        gdouble ry = ge->getVisibleRy();
        gtk_adjustment_set_value(adj, Quantity::convert(ry, "px", unit));
        gtk_adjustment_value_changed(adj);
    }

    gdouble start = 0.;
    gdouble end = 0.;
    sp_repr_get_double(repr, "sodipodi:start", &start);
    sp_repr_get_double(repr, "sodipodi:end", &end);

    GtkAdjustment *adj1,*adj2;
    adj1 = GTK_ADJUSTMENT( g_object_get_data( tbl, "start" ) );
    gtk_adjustment_set_value(adj1, mod360((start * 180)/M_PI));
    adj2 = GTK_ADJUSTMENT( g_object_get_data( tbl, "end" ) );
    gtk_adjustment_set_value(adj2, mod360((end * 180)/M_PI));

    sp_arctb_sensitivize( tbl, gtk_adjustment_get_value(adj1), gtk_adjustment_get_value(adj2) );

    char const *openstr = NULL;
    openstr = repr->attribute("sodipodi:open");
    EgeSelectOneAction *ocb = EGE_SELECT_ONE_ACTION( g_object_get_data( tbl, "open_action" ) );

    if (openstr) {
        ege_select_one_action_set_active( ocb, 1 );
    } else {
        ege_select_one_action_set_active( ocb, 0 );
    }

    g_object_set_data( tbl, "freeze", GINT_TO_POINTER(FALSE) );
}

static Inkscape::XML::NodeEventVector arc_tb_repr_events = {
    NULL, /* child_added */
    NULL, /* child_removed */
    arc_tb_event_attr_changed,
    NULL, /* content_changed */
    NULL  /* order_changed */
};


static void sp_arc_toolbox_selection_changed(Inkscape::Selection *selection, GObject *tbl)
{
    int n_selected = 0;
    Inkscape::XML::Node *repr = NULL;
    SPItem *item = NULL;

    if ( g_object_get_data( tbl, "repr" ) ) {
        g_object_set_data( tbl, "item", NULL );
    }
    purge_repr_listener( tbl, tbl );

    std::vector<SPItem*> itemlist=selection->itemList();
    for(std::vector<SPItem*>::const_iterator i=itemlist.begin();i!=itemlist.end();++i){
        if (SP_IS_GENERICELLIPSE(*i)) {
            n_selected++;
            item = *i;
            repr = item->getRepr();
        }
    }

    EgeOutputAction* act = EGE_OUTPUT_ACTION( g_object_get_data( tbl, "mode_action" ) );

    g_object_set_data( tbl, "single", GINT_TO_POINTER(FALSE) );
    if (n_selected == 0) {
        g_object_set( G_OBJECT(act), "label", _("<b>New:</b>"), NULL );
    } else if (n_selected == 1) {
        g_object_set_data( tbl, "single", GINT_TO_POINTER(TRUE) );
        g_object_set( G_OBJECT(act), "label", _("<b>Change:</b>"), NULL );

        GtkAction* rx = GTK_ACTION( g_object_get_data( tbl, "rx_action" ) );
        gtk_action_set_sensitive(rx, TRUE);
        GtkAction* ry = GTK_ACTION( g_object_get_data( tbl, "ry_action" ) );
        gtk_action_set_sensitive(ry, TRUE);

        if (repr) {
            g_object_set_data( tbl, "repr", repr );
            g_object_set_data( tbl, "item", item );
            Inkscape::GC::anchor(repr);
            sp_repr_add_listener(repr, &arc_tb_repr_events, tbl);
            sp_repr_synthesize_events(repr, &arc_tb_repr_events, tbl);
        }
    } else {
        // FIXME: implement averaging of all parameters for multiple selected
        //gtk_label_set_markup(GTK_LABEL(l), _("<b>Average:</b>"));
        g_object_set( G_OBJECT(act), "label", _("<b>Change:</b>"), NULL );
        sp_arctb_sensitivize( tbl, 1, 0 );
    }
}

static void arc_toolbox_check_ec(SPDesktop* dt, Inkscape::UI::Tools::ToolBase* ec, GObject* holder);

void sp_arc_toolbox_prep(SPDesktop *desktop, GtkActionGroup* mainActions, GObject* holder)
{
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();

    EgeAdjustmentAction* eact = 0;
    Inkscape::IconSize secondarySize = ToolboxFactory::prefToSize("/toolbox/secondary", 1);

    UnitTracker* tracker = new UnitTracker(Inkscape::Util::UNIT_TYPE_LINEAR);
    tracker->setActiveUnit(unit_table.getUnit("px"));
    g_object_set_data( holder, "tracker", tracker );

    {
        EgeOutputAction* act = ege_output_action_new( "ArcStateAction", _("<b>New:</b>"), "", 0 );
        ege_output_action_set_use_markup( act, TRUE );
        gtk_action_group_add_action( mainActions, GTK_ACTION( act ) );
        g_object_set_data( holder, "mode_action", act );
    }

    /* Radius X */
    {
        gchar const* labels[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        gdouble values[] = {1, 2, 3, 5, 10, 20, 50, 100, 200, 500};
        eact = create_adjustment_action( "ArcRadiusXAction",
                                         _("Horizontal radius"), _("Rx:"), _("Horizontal radius of the circle, ellipse, or arc"),
                                         "/tools/shapes/arc/rx", 0,
                                         GTK_WIDGET(desktop->canvas), holder, TRUE, "altx-arc",
                                         0, 1e6, SPIN_STEP, SPIN_PAGE_STEP,
                                         labels, values, G_N_ELEMENTS(labels),
                                         sp_arctb_rx_value_changed, tracker);
        tracker->addAdjustment( ege_adjustment_action_get_adjustment(eact) );
        g_object_set_data( holder, "rx_action", eact );
        gtk_action_set_sensitive( GTK_ACTION(eact), FALSE );
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
    }

    /* Radius Y */
    {
        gchar const* labels[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        gdouble values[] = {1, 2, 3, 5, 10, 20, 50, 100, 200, 500};
        eact = create_adjustment_action( "ArcRadiusYAction",
                                         _("Vertical radius"), _("Ry:"), _("Vertical radius of the circle, ellipse, or arc"),
                                         "/tools/shapes/arc/ry", 0,
                                         GTK_WIDGET(desktop->canvas), holder, FALSE, NULL,
                                         0, 1e6, SPIN_STEP, SPIN_PAGE_STEP,
                                         labels, values, G_N_ELEMENTS(labels),
                                         sp_arctb_ry_value_changed, tracker);
        tracker->addAdjustment( ege_adjustment_action_get_adjustment(eact) );
        g_object_set_data( holder, "ry_action", eact );
        gtk_action_set_sensitive( GTK_ACTION(eact), FALSE );
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
    }

    // add the units menu
    {
        GtkAction* act = tracker->createAction( "ArcUnitsAction", _("Units"), ("") );
        gtk_action_group_add_action( mainActions, act );
    }

    /* Start */
    {
        eact = create_adjustment_action( "ArcStartAction",
                                         _("Start"), _("Start:"),
                                         _("The angle (in degrees) from the horizontal to the arc's start point"),
                                         "/tools/shapes/arc/start", 0.0,
                                         GTK_WIDGET(desktop->canvas), holder, TRUE, "altx-arc",
                                         -360.0, 360.0, 1.0, 10.0,
                                         0, 0, 0,
                                         sp_arctb_start_value_changed);
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
    }

    /* End */
    {
        eact = create_adjustment_action( "ArcEndAction",
                                         _("End"), _("End:"),
                                         _("The angle (in degrees) from the horizontal to the arc's end point"),
                                         "/tools/shapes/arc/end", 0.0,
                                         GTK_WIDGET(desktop->canvas), holder, FALSE, NULL,
                                         -360.0, 360.0, 1.0, 10.0,
                                         0, 0, 0,
                                         sp_arctb_end_value_changed);
        gtk_action_group_add_action( mainActions, GTK_ACTION(eact) );
    }

    /* Segments / Pie checkbox */
    {
        GtkListStore* model = gtk_list_store_new( 3, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING );

        GtkTreeIter iter;
        gtk_list_store_append( model, &iter );
        gtk_list_store_set( model, &iter,
                            0, _("Closed arc"),
                            1, _("Switch to segment (closed shape with two radii)"),
                            2, INKSCAPE_ICON("draw-ellipse-segment"),
                            -1 );

        gtk_list_store_append( model, &iter );
        gtk_list_store_set( model, &iter,
                            0, _("Open Arc"),
                            1, _("Switch to arc (unclosed shape)"),
                            2, INKSCAPE_ICON("draw-ellipse-arc"),
                            -1 );

        EgeSelectOneAction* act = ege_select_one_action_new( "ArcOpenAction", (""), (""), NULL, GTK_TREE_MODEL(model) );
        gtk_action_group_add_action( mainActions, GTK_ACTION(act) );
        g_object_set_data( holder, "open_action", act );

        ege_select_one_action_set_appearance( act, "full" );
        ege_select_one_action_set_radio_action_type( act, INK_RADIO_ACTION_TYPE );
        g_object_set( G_OBJECT(act), "icon-property", "iconId", NULL );
        ege_select_one_action_set_icon_column( act, 2 );
        ege_select_one_action_set_icon_size( act, secondarySize );
        ege_select_one_action_set_tooltip_column( act, 1  );

        bool isClosed = !prefs->getBool("/tools/shapes/arc/open", false);
        ege_select_one_action_set_active( act, isClosed ? 0 : 1 );
        g_signal_connect_after( G_OBJECT(act), "changed", G_CALLBACK(sp_arctb_open_state_changed), holder );
    }

    /* Make Whole */
    {
        InkAction* inky = ink_action_new( "ArcResetAction",
                                          _("Make whole"),
                                          _("Make the shape a whole ellipse, not arc or segment"),
                                          INKSCAPE_ICON("draw-ellipse-whole"),
                                          secondarySize );
        g_signal_connect_after( G_OBJECT(inky), "activate", G_CALLBACK(sp_arctb_defaults), holder );
        gtk_action_group_add_action( mainActions, GTK_ACTION(inky) );
        gtk_action_set_sensitive( GTK_ACTION(inky), TRUE );
        g_object_set_data( holder, "make_whole", inky );
    }

    g_object_set_data( G_OBJECT(holder), "single", GINT_TO_POINTER(TRUE) );
    // sensitivize make whole and open checkbox
    {
        GtkAdjustment *adj1 = GTK_ADJUSTMENT( g_object_get_data( holder, "start" ) );
        GtkAdjustment *adj2 = GTK_ADJUSTMENT( g_object_get_data( holder, "end" ) );
        sp_arctb_sensitivize( holder, gtk_adjustment_get_value(adj1), gtk_adjustment_get_value(adj2) );
    }

    desktop->connectEventContextChanged(sigc::bind(sigc::ptr_fun(arc_toolbox_check_ec), holder));
    g_signal_connect( holder, "destroy", G_CALLBACK(purge_repr_listener), holder );
}

static void arc_toolbox_check_ec(SPDesktop* desktop, Inkscape::UI::Tools::ToolBase* ec, GObject* holder)
{
    static sigc::connection changed;

    if (SP_IS_ARC_CONTEXT(ec)) {
        changed = desktop->getSelection()->connectChanged(sigc::bind(sigc::ptr_fun(sp_arc_toolbox_selection_changed), holder));
        sp_arc_toolbox_selection_changed(desktop->getSelection(), holder);
    } else {
        if (changed)
            changed.disconnect();
    }
}

/*
  Local Variables:
  mode:c++
  c-file-style:"stroustrup"
  c-file-offsets:((innamespace . 0)(inline-open . 0)(case-label . +))
  indent-tabs-mode:nil
  fill-column:99
  End:
*/
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4:fileencoding=utf-8:textwidth=99 :
