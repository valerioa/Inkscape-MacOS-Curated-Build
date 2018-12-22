/*
 * Copyright (C) Johan Engelen 2012 <j.b.c.engelen@alumnus.utwente.nl>
 *
 * Released under GNU GPL, read the file 'COPYING' for more information
 */

#include <glibmm/i18n.h>

#include "live_effects/lpe-clone-original.h"
#include "live_effects/lpe-spiro.h"
#include "live_effects/lpe-bspline.h"
#include "live_effects/lpeobject.h"
#include "live_effects/lpeobject-reference.h"
#include "live_effects/effect.h"
#include "sp-shape.h"
#include "sp-text.h"
#include "display/curve.h"

namespace Inkscape {
namespace LivePathEffect {

LPECloneOriginal::LPECloneOriginal(LivePathEffectObject *lpeobject) :
    Effect(lpeobject),
    linked_path(_("Linked path:"), _("Path from which to take the original path data"), "linkedpath", &wr, this)
{
    registerParameter(&linked_path);
}

LPECloneOriginal::~LPECloneOriginal()
{

}

void LPECloneOriginal::doEffect (SPCurve * curve)
{
    if ( linked_path.linksToPath() ) {
        SPCurve *curve_out = NULL;
        SPItem *linked_obj = linked_path.getObject();
        if (SP_IS_SHAPE(linked_obj)) {
            SPLPEItem * lpe_item = SP_LPE_ITEM(linked_obj);
            if (lpe_item && lpe_item->hasPathEffect()){
                curve_out = SP_SHAPE(linked_obj)->getCurveBeforeLPE();
                PathEffectList lpelist = lpe_item->getEffectList();
                PathEffectList::iterator i;
                for (i = lpelist.begin(); i != lpelist.end(); ++i) {
                    LivePathEffectObject *lpeobj = (*i)->lpeobject;
                    if (lpeobj) {
                        Inkscape::LivePathEffect::Effect *lpe = lpeobj->get_lpe();
                        if (dynamic_cast<Inkscape::LivePathEffect::LPEBSpline *>(lpe)) {
                            LivePathEffect::sp_bspline_do_effect(curve_out, 0);
                        } else if (dynamic_cast<Inkscape::LivePathEffect::LPESpiro *>(lpe)) {
                            LivePathEffect::sp_spiro_do_effect(curve_out);
                        }
                    }
                }
            } else {
                curve_out = SP_SHAPE(linked_obj)->getCurve();
            }
        }
        if (SP_IS_TEXT(linked_obj)) {
            curve_out = SP_TEXT(linked_obj)->getNormalizedBpath();
        }

        if (curve_out == NULL) {
            // curve invalid, set empty pathvector
            curve_out->set_pathvector(Geom::PathVector());
        }
        curve->set_pathvector(curve_out->get_pathvector());
    }
}

} // namespace LivePathEffect
} /* namespace Inkscape */

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
