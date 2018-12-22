/**
 * @file-update
 * Operations to bump files from the pre-0.92 era into the 0.92+ era
 * (90dpi vs 96dpi, line height problems, and related bugs)
 */
/* Authors:
 * Tavmjong Bah
 * Marc Jeanmougin
 * su_v
 */

#include <gtkmm.h>

#include "extension/effect.h"
#include "extension/db.h"
#include "extension/input.h"
#include "extension/output.h"
#include "extension/system.h"

#include "inkscape.h"
#include "file.h"
#include "preferences.h"
#include "sp-root.h"
#include "sp-text.h"
#include "sp-tspan.h"
#include "sp-flowdiv.h"
#include "sp-flowtext.h"
#include "sp-object.h"
#include "sp-item.h"
#include "style.h"
#include "sp-defs.h"
#include "dir-util.h"
#include "document.h"
#include <string>
#include <clocale>
#include "text-editing.h"
#include "document-undo.h"
#include "display/canvas-grid.h"
#include "sp-guide.h"
#include "selection-chemistry.h"
#include "persp3d.h"
#include "proj_pt.h"
#include "ui/shape-editor.h"
#include "io/sys.h"
#include "print.h"
#include "ui/interface.h"
#include "desktop.h"
#include "message.h"
#include "message-stack.h"
#include "verbs.h"
#include "sp-namedview.h"

using Inkscape::DocumentUndo;
using namespace std;

int sp_file_convert_dpi_method_commandline = -1; // Unset

bool is_line(SPObject *i)
{
    if (!(i->getAttribute("sodipodi:role")))
        return false;
    return !strcmp(i->getAttribute("sodipodi:role"), "line");
}


void fix_blank_line(SPObject *o)
{
    if (SP_IS_TEXT(o))
        ((SPText *)o)->rebuildLayout();
    else if (SP_IS_FLOWTEXT(o))
        ((SPFlowtext *)o)->rebuildLayout();

    SPIFontSize fontsize = o->style->font_size;
    SPILengthOrNormal lineheight = o->style->line_height;
    vector<SPObject *> cl = o->childList(false);
    bool beginning = true;
    for (vector<SPObject *>::const_iterator ci = cl.begin(); ci != cl.end(); ++ci) {
        SPObject *i = *ci;
        if ((SP_IS_TSPAN(i) && is_line(i)) || SP_IS_FLOWPARA(i) || SP_IS_FLOWDIV(i)) {
            if (sp_text_get_length((SPItem *)i) <= 1) { // empty line
                Inkscape::Text::Layout::iterator pos = te_get_layout((SPItem*)(o))->charIndexToIterator(
                        ((SP_IS_FLOWPARA(i) || SP_IS_FLOWDIV(i))?0:((ci==cl.begin())?0:1)) + sp_text_get_length_upto(o,i) );
                sp_te_insert((SPItem *)o, pos, "\u00a0"); //"\u00a0"
                gchar *l = g_strdup_printf("%f", lineheight.value);
                gchar *f = g_strdup_printf("%f", fontsize.value);
                i->style->line_height.readIfUnset(l);
                if (!beginning)
                    i->style->font_size.read(f);
                else
                    i->style->font_size.readIfUnset(f);
                g_free(l);
                g_free(f);
            } else {
                beginning = false;
                fontsize = i->style->font_size;
                lineheight = o->style->line_height;
            }
        }
    }
}

void fix_line_spacing(SPObject *o)
{
    SPILengthOrNormal lineheight = o->style->line_height;
    bool inner = false;
    vector<SPObject *> cl = o->childList(false);
    for (vector<SPObject *>::const_iterator ci = cl.begin(); ci != cl.end(); ++ci) {
        SPObject *i = *ci;
        if ((SP_IS_TSPAN(i) && is_line(i)) || SP_IS_FLOWPARA(i) || SP_IS_FLOWDIV(i)) {
            // if no line-height attribute, set it
            gchar *l = g_strdup_printf("%f", lineheight.value);
            i->style->line_height.readIfUnset(l);
            g_free(l);
        }
        inner = true;
    }
    if (inner) {
        if (SP_IS_TEXT(o)) {
            o->style->line_height.read("0.00%");
        } else {
            o->style->line_height.read("0.01%");
        }
    }
}

void fix_font_name(SPObject *o)
{
    vector<SPObject *> cl = o->childList(false);
    for (vector<SPObject *>::const_iterator ci = cl.begin(); ci != cl.end(); ++ci)
        fix_font_name(*ci);
    string prev = o->style->font_family.value ? o->style->font_family.value : o->style->font_family.value_default;
    if (prev == "Sans")
        o->style->font_family.read("sans-serif");
    else if (prev == "Serif")
        o->style->font_family.read("serif");
    else if (prev == "Monospace")
        o->style->font_family.read("monospace");
}


void fix_font_size(SPObject *o)
{
    SPIFontSize fontsize = o->style->font_size;
    if (!fontsize.set)
        return;
    bool inner = false;
    vector<SPObject *> cl = o->childList(false);
    for (vector<SPObject *>::const_iterator ci = cl.begin(); ci != cl.end(); ++ci) {
        SPObject *i = *ci;
        fix_font_size(i);
        if ((SP_IS_TSPAN(i) && is_line(i)) || SP_IS_FLOWPARA(i) || SP_IS_FLOWDIV(i)) {
            inner = true;
            gchar *s = g_strdup_printf("%f", fontsize.value);
            if (fontsize.set)
                i->style->font_size.readIfUnset(s);
            g_free(s);
        }
    }
    if (inner && (SP_IS_TEXT(o) || SP_IS_FLOWTEXT(o)))
        o->style->font_size.clear();
}



// helper function
void sp_file_text_run_recursive(void (*f)(SPObject *), SPObject *o)
{
    if (SP_IS_TEXT(o) || SP_IS_FLOWTEXT(o))
        f(o);
    else {
        vector<SPObject *> cl = o->childList(false);
        for (vector<SPObject *>::const_iterator ci = cl.begin(); ci != cl.end(); ++ci)
            sp_file_text_run_recursive(f, *ci);
    }
}

void fix_update(SPObject *o) { 
    o->style->write();
    o->updateRepr();
}

void sp_file_convert_text_baseline_spacing(SPDocument *doc)
{
    setlocale(LC_NUMERIC,"C");
    sp_file_text_run_recursive(fix_blank_line, doc->getRoot());
    sp_file_text_run_recursive(fix_line_spacing, doc->getRoot());
    sp_file_text_run_recursive(fix_font_size, doc->getRoot());
    sp_file_text_run_recursive(fix_update, doc->getRoot());
    setlocale(LC_NUMERIC,"");
}

void sp_file_convert_font_name(SPDocument *doc)
{
    sp_file_text_run_recursive(fix_font_name, doc->getRoot());
    sp_file_text_run_recursive(fix_update, doc->getRoot());
}

// Quick and dirty internal backup function
bool sp_file_save_backup( Glib::ustring uri ) {
    Glib::ustring out = uri;
    out.insert(out.find(".svg"),"_backup");

    FILE *filein  = Inkscape::IO::fopen_utf8name(uri.c_str(), "rb");
    if (!filein) {
        std::cerr << "sp_file_save_backup: failed to open: " << uri << std::endl;
        return false;
    }

    FILE *fileout = Inkscape::IO::fopen_utf8name(out.c_str(), "wb");
    if (!fileout) {
        std::cerr << "sp_file_save_backup: failed to open: " << out << std::endl;
        fclose(filein);
        return false;
    }

    int ch;
    while ((ch = fgetc(filein)) != EOF) {
        fputc(ch, fileout);
    }
    fflush(fileout);

    bool return_value = true;
    if (ferror(fileout)) {
        std::cerr << "sp_file_save_backup: error when writing to: " << out << std::endl;
        return_value = false;
    }

    fclose(filein);
    fclose(fileout);

    return return_value;
}

void sp_file_convert_dpi(SPDocument *doc)
{
    Inkscape::Preferences *prefs = Inkscape::Preferences::get();
    Glib::ustring uri = doc->getURI();
    SPRoot *root = doc->getRoot();

    // See if we need to offer the user a fix for the 90->96 px per inch change.
    // std::cout << "SPFileOpen:" << std::endl;
    // std::cout << "  Version: " << sp_version_to_string(root->version.inkscape) << std::endl;
    // std::cout << "  SVG file from old Inkscape version detected: "                          
    //           << sp_version_to_string(root->version.inkscape) << std::endl;                 
    static const double ratio = 90.0/96.0;

    bool need_fix_viewbox = false;
    bool need_fix_units   = false;
    bool need_fix_guides  = false;
    bool need_fix_grid_mm = false;
    bool need_fix_box3d   = false;
    bool did_scaling      = false;

    // Check if potentially need viewbox or unit fix                                           
    switch (root->width.unit) {
        case SP_CSS_UNIT_PC:
        case SP_CSS_UNIT_PT:
        case SP_CSS_UNIT_MM:
        case SP_CSS_UNIT_CM:
        case SP_CSS_UNIT_IN:
            need_fix_viewbox = true;
            break;
        case SP_CSS_UNIT_NONE:
        case SP_CSS_UNIT_PX:
            need_fix_units = true;
            break;
        case SP_CSS_UNIT_EM:
        case SP_CSS_UNIT_EX:
        case SP_CSS_UNIT_PERCENT:
            // OK                                                                              
            break;
        default:
            std::cerr << "sp_file_open: Unhandled width unit!" << std::endl;
    }

    switch (root->height.unit) {
        case SP_CSS_UNIT_PC:
        case SP_CSS_UNIT_PT:
        case SP_CSS_UNIT_MM:
        case SP_CSS_UNIT_CM:
        case SP_CSS_UNIT_IN:
            need_fix_viewbox = true;
            break;
        case SP_CSS_UNIT_NONE:
        case SP_CSS_UNIT_PX:
            need_fix_units = true;
            break;
        case SP_CSS_UNIT_EM:
        case SP_CSS_UNIT_EX:
        case SP_CSS_UNIT_PERCENT:
            // OK                                                                              
            break;
        default:
            std::cerr << "sp_file_open: Unhandled height unit!" << std::endl;
    }

    if (need_fix_units && need_fix_viewbox) {
        std::cerr << "Different units in document size !" << std::endl;
        if (root->viewBox_set)
            need_fix_viewbox = false;
        else
            need_fix_units = false;
    }

    // std::cout << "Absolute SVG units in root? " << (need_fix_viewbox?"true":"false") << std::endl;                 
    // std::cout << "User units in root? "         << (need_fix_units  ?"true":"false") << std::endl;                 

    if ((!root->viewBox_set && need_fix_viewbox) || need_fix_units) {
        int response = FILE_DPI_UNCHANGED; // default

        /******** UI *******/
        bool backup = prefs->getBool("/options/dpifixbackup", true);
        if (INKSCAPE.use_gui() && sp_file_convert_dpi_method_commandline == -1) {
            Gtk::Dialog scale_dialog(_("Convert legacy Inkscape file"));
            scale_dialog.set_transient_for( *(INKSCAPE.active_desktop()->getToplevel()) );
            scale_dialog.set_border_width(10);
            scale_dialog.set_resizable(false);
            Gtk::Label explanation;
            explanation.set_markup(Glib::ustring("<b>") + doc->getName() + "</b>\n" +
                                   _("was created in an older version of Inkscape (90 DPI) and we need "
                                     "to make it compatible with newer versions (96 DPI). Tell us about this file:\n"));
            explanation.set_line_wrap(true);
            explanation.set_size_request(600,-1);
            Gtk::RadioButton::Group c1, c2;

            Gtk::Label choice1_label;
            choice1_label.set_markup(
                _("This file contains digital artwork for screen display. <b>(Choose if unsure.)</b>"));
            Gtk::RadioButton choice1(c1);
            choice1.add(choice1_label);
            Gtk::RadioButton choice2(c1, _("This file is intended for physical output, such as paper or 3D prints."));
            Gtk::Label choice2_1_label;
            choice2_1_label.set_markup(_("The appearance of elements such as clips, masks, filters, and clones\n"
                                         "is most important. <b>(Choose if unsure.)</b>"));
            Gtk::RadioButton choice2_1(c2);
            choice2_1.add(choice2_1_label);
            Gtk::RadioButton choice2_2(c2, _("The accuracy of the physical unit size and position values of objects\n"
                                             "in the file is most important. (Experimental.)"));
            Gtk::CheckButton backup_button(_("Create a backup file in same directory."));
            Gtk::Expander moreinfo(_("More details..."));
            Gtk::Label moreinfo_text;
            moreinfo_text.set_markup(
                _("<small>We've updated Inkscape to follow the CSS standard of 96 DPI for better browser "
                  "compatibility; we used to use 90 DPI. "
                  "Digital artwork for screen display will be converted to 96 DPI without scaling and should be "
                  "unaffected. "
                  "Artwork drawn at 90 DPI for a specific physical size will be too small if converted to 96 DPI "
                  "without any scaling. There are two scaling methods:\n\n"
                  "<b>Scaling the whole document:</b> The least error-prone method, this preserves the appearance of "
                  "the artwork, including filters and the position of masks, etc. "
                  "The scale of the artwork relative to the document size may not be accurate.\n\n"
                  "<b>Scaling individual elements in the artwork:</b> This method is less reliable and can result in "
                  "a changed appearance, "
                  "but is better for physical output that relies on accurate sizes and positions (for example, for "
                  "3D printing.)\n\n"
                  "More information about this change are available in the <a "
                  "href='https://inkscape.org/en/learn/faq#dpi_change'>Inkscape FAQ</a>"
                  "</small>"));
            moreinfo_text.set_line_wrap(true);
            moreinfo_text.set_size_request(554,-1);
#if WITH_GTKMM_3_0
            Gtk::Box b;
#else
            Gtk::VBox b;
#endif
            b.set_border_width(0);
            
            b.pack_start(choice2_1, false, false, 4);
            b.pack_start(choice2_2, false, false, 4);
            choice2_1.show();
            choice2_2.show();

            Gtk::Alignment balign(0, 0, 0, 0);
            balign.set_padding (0, 0, 30, 0);
            balign.show();
            balign.add(b);
#if WITH_GTKMM_3_0
            Gtk::Box *content = scale_dialog.get_content_area();
#else
            Gtk::VBox *content = scale_dialog.get_vbox();
#endif
            content->pack_start(explanation, false, false, 5);
            content->pack_start(choice1, false, false, 5);
            content->pack_start(choice2, false, false, 5);
            content->pack_start(balign, false, false, 5);
            content->pack_start(backup_button, false, false, 5);
            Gtk::Button *ok_button = scale_dialog.add_button(_("OK"), GTK_RESPONSE_ACCEPT);
            backup_button.set_active(backup);
            content->pack_start(moreinfo, false, false, 5);
            moreinfo.add(moreinfo_text);
            scale_dialog.show_all_children();
            b.hide();
#if WITH_GTKMM_3_0
            choice1.signal_clicked().connect(sigc::mem_fun(b, &Gtk::Box::hide));
            choice2.signal_clicked().connect(sigc::mem_fun(b, &Gtk::Box::show));
#else
            choice1.signal_clicked().connect(sigc::mem_fun(b, &Gtk::VBox::hide));
            choice2.signal_clicked().connect(sigc::mem_fun(b, &Gtk::VBox::show));
#endif

            response = prefs->getInt("/options/dpiupdatemethod", FILE_DPI_UNCHANGED);
            if ( response != FILE_DPI_UNCHANGED ) {
                choice2.set_active();
                b.show();
                if ( response == FILE_DPI_DOCUMENT_SCALED)
                    choice2_2.set_active();
            }
            ok_button->grab_focus();

            int status = scale_dialog.run();
            if ( status == GTK_RESPONSE_ACCEPT ) {
                backup = backup_button.get_active();
                prefs->setBool("/options/dpifixbackup", backup);
                response = choice1.get_active() ? FILE_DPI_UNCHANGED : choice2_1.get_active() ? FILE_DPI_VIEWBOX_SCALED : FILE_DPI_DOCUMENT_SCALED;
                prefs->setInt("/options/dpiupdatemethod", response);
            } else if (sp_file_convert_dpi_method_commandline != -1) {
                response = sp_file_convert_dpi_method_commandline;
            } else {
                response = FILE_DPI_UNCHANGED;
            }
        } else { // GUI with explicit option
            response = FILE_DPI_UNCHANGED;
        }

        if (backup && (response != FILE_DPI_UNCHANGED)) {
            sp_file_save_backup(uri);
        }

        if (!(response == FILE_DPI_UNCHANGED && need_fix_units)) {
            need_fix_guides = true; // Only fix guides if drawing scaled
            need_fix_box3d = true;
        }

        if (response == FILE_DPI_VIEWBOX_SCALED) {
            double ratio_viewbox = need_fix_units ? 1.0 : ratio;

            doc->setViewBox(Geom::Rect::from_xywh(0, 0, doc->getWidth().value("px") * ratio_viewbox,
                                                  doc->getHeight().value("px") * ratio_viewbox));
            Inkscape::Util::Quantity width = // maybe set it to mm ?
                Inkscape::Util::Quantity(doc->getWidth().value("px") / ratio, "px");
            Inkscape::Util::Quantity height = Inkscape::Util::Quantity(doc->getHeight().value("px") / ratio, "px");
            if (need_fix_units)
                doc->setWidthAndHeight(width, height, false);

            } else if (response == FILE_DPI_DOCUMENT_SCALED) {

            Inkscape::Util::Quantity width = // maybe set it to mm ?
                Inkscape::Util::Quantity(doc->getWidth().value("px") / ratio, "px");
            Inkscape::Util::Quantity height = Inkscape::Util::Quantity(doc->getHeight().value("px") / ratio, "px");
            if (need_fix_units)
                doc->setWidthAndHeight(width, height, false);

            if (!root->viewBox_set) {
                // Save preferences
                bool transform_stroke = prefs->getBool("/options/transform/stroke", true);
                bool transform_rectcorners = prefs->getBool("/options/transform/rectcorners", true);
                bool transform_pattern = prefs->getBool("/options/transform/pattern", true);
                bool transform_gradient = prefs->getBool("/options/transform/gradient", true);

                prefs->setBool("/options/transform/stroke", true);
                prefs->setBool("/options/transform/rectcorners", true);
                prefs->setBool("/options/transform/pattern", true);
                prefs->setBool("/options/transform/gradient", true);

                Inkscape::UI::ShapeEditor::blockSetItem(true);
                doc->getRoot()->scaleChildItemsRec(Geom::Scale(1 / ratio), Geom::Point(0, 0), false);
                Inkscape::UI::ShapeEditor::blockSetItem(false);

                // Restore preferences
                prefs->setBool("/options/transform/stroke", transform_stroke);
                prefs->setBool("/options/transform/rectcorners", transform_rectcorners);
                prefs->setBool("/options/transform/pattern", transform_pattern);
                prefs->setBool("/options/transform/gradient", transform_gradient);

                did_scaling = true;
            }
        } else { // FILE_DPI_UNCHANGED
            if (need_fix_units)
                need_fix_grid_mm = true;
        }
    }

    // Fix guides and grids and perspective
    for (SPObject *child = root->firstChild(); child; child = child->getNext()) {
        SPNamedView *nv = dynamic_cast<SPNamedView *>(child);
        if (nv) {
            if (need_fix_guides) {
                // std::cout << "Fixing guides" << std::endl;
                for (SPObject *child2 = nv->firstChild(); child2; child2 = child2->getNext()) {
                    SPGuide *gd = dynamic_cast<SPGuide *>(child2);
                    if (gd) {
                        gd->moveto(gd->getPoint() / ratio, true);
                    }
                }
            }

            for (std::vector<Inkscape::CanvasGrid *>::const_iterator it = nv->grids.begin(); it != nv->grids.end();
                 ++it) {
                Inkscape::CanvasXYGrid *xy = dynamic_cast<Inkscape::CanvasXYGrid *>(*it);
                if (xy) {
                    // std::cout << "A grid: " << xy->getSVGName() << std::endl;
                    // std::cout << "  Origin: " << xy->origin
                    //           << "  Spacing: " << xy->spacing << std::endl;
                    // std::cout << (xy->isLegacy()?"  Legacy":"  Not Legacy") << std::endl;
                    Geom::Scale scale = doc->getDocumentScale();
                    if (xy->isLegacy()) {
                        if (xy->isPixel()) {
                            if (need_fix_grid_mm) {
                                xy->Scale(Geom::Scale(1, 1)); // See note below
                            } else {
                                scale *= Geom::Scale(ratio, ratio);
                                xy->Scale(scale.inverse()); /* *** */
                            }
                        } else {
                            if (need_fix_grid_mm) {
                                xy->Scale(Geom::Scale(ratio, ratio));
                            } else {
                                xy->Scale(scale.inverse()); /* *** */
                            }
                        }
                    } else {
                        if (need_fix_guides) {
                            if (did_scaling) {
                                xy->Scale(Geom::Scale(ratio, ratio).inverse());
                            } else {
                                // HACK: Scaling the document does not seem to cause
                                // grids defined in document units to be updated.
                                // This forces an update.
                                xy->Scale(Geom::Scale(1, 1));
                            }
                        }
                    }
                }
            }
        } // If SPNamedView

        SPDefs *defs = dynamic_cast<SPDefs *>(child);
        if (defs && need_fix_box3d) {
            for (SPObject *child = defs->firstChild(); child; child = child->getNext()) {
                Persp3D *persp3d = dynamic_cast<Persp3D *>(child);
                if (persp3d) {
                    std::vector<Glib::ustring> tokens;

                    const gchar *vp_x = persp3d->getAttribute("inkscape:vp_x");
                    const gchar *vp_y = persp3d->getAttribute("inkscape:vp_y");
                    const gchar *vp_z = persp3d->getAttribute("inkscape:vp_z");
                    const gchar *vp_o = persp3d->getAttribute("inkscape:persp3d-origin");
                    // std::cout << "Found Persp3d: "
                    //           << " vp_x: " << vp_x
                    //           << " vp_y: " << vp_y
                    //           << " vp_z: " << vp_z << std::endl;
                    Proj::Pt2 pt_x(vp_x);
                    Proj::Pt2 pt_y(vp_y);
                    Proj::Pt2 pt_z(vp_z);
                    Proj::Pt2 pt_o(vp_o);
                    pt_x = pt_x * (1.0 / ratio);
                    pt_y = pt_y * (1.0 / ratio);
                    pt_z = pt_z * (1.0 / ratio);
                    pt_o = pt_o * (1.0 / ratio);
                    persp3d->setAttribute("inkscape:vp_x", pt_x.coord_string());
                    persp3d->setAttribute("inkscape:vp_y", pt_y.coord_string());
                    persp3d->setAttribute("inkscape:vp_z", pt_z.coord_string());
                    persp3d->setAttribute("inkscape:persp3d-origin", pt_o.coord_string());
                }
            }
        }
    } // Look for SPNamedView and SPDefs loop

    // desktop->getDocument()->ensureUpToDate();  // Does not update box3d!
    DocumentUndo::done(doc, SP_VERB_NONE, _("Update Document"));
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
// vim: filetype=cpp:expandtab:shiftwidth=4:tabstop=8:softtabstop=4 :
