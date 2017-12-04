/*
 * Copyright (C) 2005-2007 Authors:
 *   Ted Gould <ted@gould.cx>
 *   Johan Engelen <johan@shouraizou.nl> *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#ifdef linux  // does the dollar sign need escaping when passed as string parameter?
# define ESCAPE_DOLLAR_COMMANDLINE
#endif

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <gtkmm/box.h>
#include <gtkmm/label.h>
#include <glibmm/i18n.h>
#include <glibmm/markup.h>
#include <glibmm/regex.h>

#include "xml/node.h"
#include "extension/extension.h"
#include "description.h"

namespace Inkscape {
namespace Extension {


/** \brief  Initialize the object, to do that, copy the data. */
ParamDescription::ParamDescription (const gchar * name,
                                    const gchar * guitext,
                                    const gchar * desc,
                                    const Parameter::_scope_t scope,
                                    bool gui_hidden,
                                    const gchar * gui_tip,
                                    Inkscape::Extension::Extension * ext,
                                    Inkscape::XML::Node * xml,
                                    AppearanceMode mode) :
    Parameter(name, guitext, desc, scope, gui_hidden, gui_tip, ext),
              _value(NULL), _mode(mode), _indent(0)
{
    // construct the text content by concatenating all (non-empty) text nodes,
    // removing all other nodes (e.g. comment nodes) and replacing <extension:br> elements with "<br/>"
    Glib::ustring value;
    Inkscape::XML::Node * cur_child = xml->firstChild();
    while (cur_child != NULL) {
        if (cur_child->type() == XML::TEXT_NODE && cur_child->content() != NULL) {
            value += cur_child->content();
        } else if (cur_child->type() == XML::ELEMENT_NODE && !g_strcmp0(cur_child->name(), "extension:br")) {
            value += "<br/>";
        }
        cur_child = cur_child->next();
    }

    // if there is no text content we can return immediately (the description will be invisible)
    if (value == Glib::ustring("")) {
        return;
    }

    const char * indent = xml->attribute("indent");
    if (indent != NULL) {
        _indent = atoi(indent) * 12;
    }

    // do replacements in the source string to account for the attribute xml:space="preserve"
    // (those should match replacements potentially performed by xgettext to allow for proper translation)
    if (g_strcmp0(xml->attribute("xml:space"), "preserve") == 0) {
        // xgettext copies the source string verbatim in this case, so no changes needed
    } else {
        // remove all whitespace from start/end of string and replace intermediate whitespace with a single space
        value = Glib::Regex::create("^\\s+|\\s+$")->replace_literal(value, 0, "", (Glib::RegexMatchFlags)0);
        value = Glib::Regex::create("\\s+")->replace_literal(value, 0, " ", (Glib::RegexMatchFlags)0);
    }

    // translate if underscored version (_param) was used
    if (g_str_has_prefix(xml->name(), "extension:_")) {
        const gchar * context = xml->attribute("msgctxt");
        if (context != NULL) {
            value = g_dpgettext2(NULL, context, value.c_str());
        } else {
            value = _(value.c_str());
        }
    }

    // finally replace all remaining <br/> with a real newline character
    value = Glib::Regex::create("<br/>")->replace_literal(value, 0, "\n", (Glib::RegexMatchFlags)0);

    _value = g_strdup(value.c_str());

    return;
}

/** \brief  Create a label for the description */
Gtk::Widget *
ParamDescription::get_widget (SPDocument * /*doc*/, Inkscape::XML::Node * /*node*/, sigc::signal<void> * /*changeSignal*/)
{
    if (_gui_hidden) {
        return NULL;
    }
    if (_value == NULL) {
        return NULL;
    }

    Glib::ustring newguitext = _value;

    Gtk::Label * label = Gtk::manage(new Gtk::Label());
    int padding = 12 + _indent;
    if (_mode == HEADER) {
        label->set_markup(Glib::ustring("<b>") + Glib::Markup::escape_text(newguitext) + Glib::ustring("</b>"));
        label->set_padding(0,5);
        padding = _indent;
    } else {
        label->set_text(newguitext);
    }
    label->set_alignment(Gtk::ALIGN_START);
    label->set_line_wrap();
    label->show();

    Gtk::HBox * hbox = Gtk::manage(new Gtk::HBox(false, 4));
    hbox->pack_start(*label, true, true, padding);
    hbox->show();

    return hbox;
}

}  /* namespace Extension */
}  /* namespace Inkscape */
