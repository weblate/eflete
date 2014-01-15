/* Edje Theme Editor
* Copyright (C) 2013 Samsung Electronics.
*
* This file is part of Edje Theme Editor.
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2, or (at your option)
* any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; If not, see www.gnu.org/licenses/gpl-2.0.html.
*/

#include <ui_workspace.h>
#include "ctxpopup.h"
#include "highlight.h"
#include "groupedit.h"
#include "eflete.h"

/**
 * @typedef Ws_Smart_Data
 *
 * @brief The struct _Ws_Smart_Data uses, as smart data for Workspace object.
 * All UI elements of workspace stored here, only provate API of a workspace
 * module can modify pointers and values in this struct.
 *
 * @ingroup Workspace
 */
struct _Ws_Smart_Data
{
   Evas_Object_Smart_Clipped_Data base;    /**< Needed for correct create and \
                                             managment of Smart Object. \
                                             In future clip rulers, for \
                                             increase efficiency of EFLETE */
   Evas_Object *clipper;         /**< Needed for processing common events,\
                                   like mouse move or mouse click. */
   Evas_Object *background;      /**< A backround image, \
                                   which swallowed into scroller.*/
   Evas_Object *ruler_hor;       /**< A ruler object, which created and \
                                   managed with ui_ruler API. Horizontal.*/
   Evas_Object *ruler_ver;       /**< A ruler object, which created and \
                                   managed with ui_ruler API. Vertical.*/
   Evas_Object *scroller;        /**< A scroler with 'eflete/workspace' style. \
                                   Implement scrollable interface.*/
   Evas_Object *groupedit;       /**< A groupedit smart object, \
                                   needed for view and edit style.*/
   Evas_Object *obj;             /**< A elementary layout object, \
                                   which markup with workspace.edc file.*/
   Evas_Object *button_separate; /**< A button object, which switch (on/off)\
                                   separate mode of groupedit.*/
   Eina_List *guides;            /**< A guides list. Not implemented yet*/
   struct {
        double factor;           /**< zoom factor, coefficient for view \
                                   groupedit primitives with zoom.*/
        Evas_Object *control;    /**< ui element, which provide change zoom \
                                   factor. Markup in zoom_control.edc*/
   } zoom;
   struct {
        Evas_Object *view;        /**< A elementary layout with special \
                                    markup stored in legend.edc. TDD*/
        Eina_Bool visible;        /**< Boolean flag for legend view visibly. */
   } legend;
   Group *group;

   struct {
        Evas_Object *highlight; /**< A highlight object */
        Evas_Object *space_hl; /**< A object area highlight*/
        Part *part; /**< Contain part name and it's state. need for callbacks of highlight. */
   } highlight;
   DEPRECATED Eina_Bool separated; /**< Will remove, when groupedit \
                                     will be smart */
};
typedef struct _Ws_Smart_Data Ws_Smart_Data;

#define _evas_smart_ws "Evas_Smart_Ws"

#define WS_DATA_GET(o, ptr) \
   Ws_Smart_Data *ptr = evas_object_smart_data_get(o);

#define WS_DATA_GET_OR_RETURN_VAL(o, ptr, val)      \
   WS_DATA_GET(o, ptr);                             \
   if (!ptr)                                        \
    {                                               \
       ERR("No workspace data for object %p (%s)!", \
           o, evas_object_type_get(o));             \
       return val;                                  \
    }

static const Evas_Smart_Cb_Description _smart_callbacks[] = {{NULL, NULL}};

EVAS_SMART_SUBCLASS_NEW(_evas_smart_ws, _workspace,
                        Evas_Smart_Class, Evas_Smart_Class,
                        evas_object_smart_clipped_class_get, _smart_callbacks);

static void
_ws_ruler_hide_cb(void *data,
               Evas_Object *obj,
               void *event_info)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, RETURN_VOID)
   Eina_Bool visible = false;
   Evas_Object *ruler = (Evas_Object *)data;
   char *data_info = (char *)event_info;

   if (!strcmp(data_info, "hor"))
     {
        if (ui_ruler_visible_get(ruler))
          {
             elm_layout_signal_emit(sd->scroller, "ruler,hide,hor", "");
             ui_ruler_hide(ruler);
          }
        else
          {
             ui_ruler_show(ruler);
             elm_layout_signal_emit(sd->scroller, "ruler,show,hor", "");
          }
     }

   if (!strcmp(data_info, "ver"))
     {
        if (ui_ruler_visible_get(ruler))
          {
             elm_layout_signal_emit(sd->scroller, "ruler,hide,ver", "");
             ui_ruler_hide(ruler);
          }
        else
          {
             ui_ruler_show(ruler);
             elm_layout_signal_emit(sd->scroller, "ruler,show,ver", "");
          }
     }

   if (!strcmp(data_info, "abs"))
     {
        visible = ui_ruler_scale_absolute_visible_get(sd->ruler_hor);
        ui_ruler_scale_absolute_visible_set(sd->ruler_hor, !visible);
        ui_ruler_scale_absolute_visible_set(sd->ruler_ver, !visible);
     }

   if (!strcmp(data_info, "rel"))
     {
        visible = ui_ruler_scale_relative_visible_get(sd->ruler_hor);
        ui_ruler_scale_relative_visible_set(sd->ruler_hor, !visible);
        ui_ruler_scale_relative_visible_set(sd->ruler_ver, !visible);
     }
   free(data_info);
}

/*
static void
_ws_smart_ruler_move_cb(void *data __UNUSED__,
               Evas_Object *obj,
               void *event_info __UNUSED__)
{
   Evas_Object *target = (Evas_Object *)event_info;
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, RETURN_VOID)

   int x, y, w, h;
   int xt, yt;
   int cross_size;

   evas_object_geometry_get(sd->ruler_hor, &cross_size, NULL, NULL, NULL);
   evas_object_geometry_get(target, &x, &y, &w, &h);
   evas_object_geometry_get(sd->background, &xt, &yt, NULL, NULL);
   cross_size -= xt;
   ui_ruler_scale_absolute_position_zero_set(sd->ruler_hor, x - xt - cross_size);
   ui_ruler_scale_absolute_position_zero_set(sd->ruler_ver, y - yt - cross_size);
   ui_ruler_scale_relative_position_set(sd->ruler_hor, x - xt - cross_size,
                                        x + w - xt - cross_size);
   ui_ruler_scale_relative_position_set(sd->ruler_ver, y - yt - cross_size,
                                        y + h - yt - cross_size);
   ui_ruler_redraw(sd->ruler_hor);
   ui_ruler_redraw(sd->ruler_ver);
}
*/

static void
_ws_smart_mouse_click_cb(void *data ,
                   Evas *e __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Evas_Event_Mouse_Down *ev = event_info;
   Evas_Object *ws = (Evas_Object *)data;

   if (ev->button == 3) ui_popup_add(ws);
}

static void
_ws_smart_mouse_move_cb(void *data, Evas *e,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   int x, y;
   Evas_Object *ws = (Evas_Object *)data;
   WS_DATA_GET_OR_RETURN_VAL(ws, sd, RETURN_VOID)
   evas_pointer_output_xy_get(e, &x, &y);
   ui_ruler_pointer_pos_set(sd->ruler_hor, x);
   ui_ruler_pointer_pos_set(sd->ruler_ver, y);
}

static void
_separate_smart_on_click(void *data,
                   Evas_Object *obj __UNUSED__,
                   void *event_info __UNUSED__)
{
   const char *name = NULL;
   Evas_Object *o = (Evas_Object *)data;
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID)

   Eina_Bool sep = groupedit_edit_object_parts_separeted_is(sd->groupedit);
   if (sd->highlight.part)
     {
        name = sd->highlight.part->name;
        highlight_object_unfollow(sd->highlight.highlight);
        highlight_object_unfollow(sd->highlight.space_hl);
        evas_object_hide(sd->highlight.space_hl);
        evas_object_hide(sd->highlight.highlight);
        sd->highlight.part = NULL;
     }
   groupedit_edit_object_parts_separeted(sd->groupedit, !sep, name);

}
static void
_sc_smart_resize_cb(void *data ,
                    Evas *e __UNUSED__,
                    Evas_Object *obj __UNUSED__,
                    void *event_info __UNUSED__)
{
   Evas_Object *o = (Evas_Object *)data;
   //Evas_Object *box = NULL;
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID)

   //if (!sd->groupedit) return;
   //box = ui_groupspace_box_get(sd->groupedit);
   //if (!box) return;
   //evas_object_smart_calculate(box);
}

void
_sc_smart_move_cb(void *data,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   int bg_x, bg_y, gs_x, gs_y, gs_w, gs_h;
   int cross_size;
   Evas_Object *o = (Evas_Object *)data;
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID)


   if (!sd->group) return;
   evas_object_geometry_get(sd->ruler_hor, &cross_size, NULL, NULL, NULL);
   evas_object_geometry_get(sd->background, &bg_x, &bg_y, NULL, NULL);
   evas_object_geometry_get(sd->group->obj, &gs_x, &gs_y, &gs_w, &gs_h);
   cross_size -= bg_x;
   ui_ruler_scale_absolute_position_zero_set(sd->ruler_hor, gs_x - bg_x - cross_size);
   ui_ruler_scale_absolute_position_zero_set(sd->ruler_ver, gs_y - bg_y - cross_size);
   ui_ruler_scale_relative_position_set(sd->ruler_hor, gs_x - bg_x - cross_size,
                                        gs_x + gs_w - bg_x - cross_size);
   ui_ruler_scale_relative_position_set(sd->ruler_ver, gs_y - bg_y - cross_size,
                                        gs_y + gs_h - bg_y - cross_size);
   ui_ruler_redraw(sd->ruler_hor);
   ui_ruler_redraw(sd->ruler_ver);
   ws_object_highlight_move(o);
}

static Eina_Bool
_background_load(Evas_Object *image, const char *path)
{
   Evas_Load_Error err;
   int w, h;
   evas_object_image_file_set(image, path, NULL);
   err = evas_object_image_load_error_get(image);
   if (err != EVAS_LOAD_ERROR_NONE)
     {
        ERR("Could not load image [%s]. Error is \"%s\"", path,
            evas_load_error_str(err));
        /* Load default background*/
        evas_object_image_file_set(image, TET_IMG_PATH"bg_demo.png", NULL);
        return false;
     }
   evas_object_image_size_get(image, &w, &h);
   evas_object_image_filled_set(image, false);
   evas_object_image_fill_set(image, 0, 0, w, h);
   return true;
}

static Eina_Bool
_zoom_factor_update(Evas_Object *obj, double factor)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   //if (!sd->groupedit) return false;
   sd->zoom.factor = factor;
   /*
    * TODO:
    * Create method, which update current zoom in groupedit and workspace
    */
   return true;
}

Eina_Bool
ws_zoom_factor_set(Evas_Object *obj, double factor)
{
   if ((factor < 0.01) || (factor > 20 )) return false;
   return _zoom_factor_update(obj, factor);
}

double
ws_zoom_factor_get(Evas_Object *obj)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, 0);
   return sd->zoom.factor;
}

Eina_Bool
ws_background_image_set(Evas_Object *obj, const char *path)
{
   if (!path) return false;
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)

   if (!sd->background) return false;
   if (!_background_load(sd->background, path)) return false;
   return true;
}

Evas_Object *
ws_groupspace_get(Evas_Object *obj)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, NULL)
   //return sd->groupedit;
      return NULL;
}

/*
Eina_Bool
ws_groupspace_set(Evas_Object *obj, Evas_Object *gs)
{
   if (!gs) return false;
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   sd->groupedit = gs;
   elm_object_content_set(sd->scroller, sd->groupedit);
   evas_object_smart_callback_add(obj, "ruler,move",  _ws_smart_ruler_move_cb,
                                  NULL);
   return true;
}
*/

Eina_Bool
ws_legend_visible_set(Evas_Object *obj)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   if (!sd->legend.visible)
    elm_layout_signal_emit(obj, "legend,hide", "");
   else
    elm_layout_signal_emit(obj, "legend,show", "");
   sd->legend.visible = !sd->legend.visible;
   return true;
}

static void
_ws_mouse_move_cb(void *data, Evas *e,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   int x, y;
   Evas_Object *ws_obj = (Evas_Object *)data;
   WS_DATA_GET_OR_RETURN_VAL(ws_obj, sd, RETURN_VOID)
   evas_pointer_output_xy_get (e, &x, &y);
   ui_ruler_pointer_pos_set(sd->ruler_hor, x);
   ui_ruler_pointer_pos_set(sd->ruler_ver, y);
}

static void
_on_resize(void *data,
            Evas_Object * obj __UNUSED__,
            void *ei)
{
   Highlight_Events *events = (Highlight_Events *)ei;
   Evas_Object *ws_obj = (Evas_Object *)data;
   WS_DATA_GET_OR_RETURN_VAL(ws_obj, sd, RETURN_VOID)

   Part *part = sd->highlight.part;
   if ((!sd->group) && (!part)) return;
   edje_edit_state_max_w_set(sd->group->obj, part->name,
                             part->curr_state, part->curr_state_value,
                             events->w);
   edje_edit_state_max_h_set(sd->group->obj, part->name,
                             part->curr_state, part->curr_state_value,
                             events->h);
   workspace_edit_object_recalc(ws_obj);
   evas_object_smart_callback_call(ws_obj, "part,changed", part);
}

Eina_Bool
workspace_highlight_set(Evas_Object *obj, Part *part)
{
   Evas_Object *object;
   if ((!obj) || (!part)) return false;
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)

   groupedit_part_object_area_set(sd->groupedit, part->name);

   object = groupedit_edit_object_part_draw_get(sd->groupedit, part->name);
   highlight_object_follow(sd->highlight.highlight, object);

   object = groupedit_part_object_area_get(sd->groupedit);
   highlight_object_follow(sd->highlight.space_hl, object);

   sd->highlight.part = part;

   evas_object_hide(sd->highlight.space_hl);
   evas_object_show(sd->highlight.highlight);

   evas_object_event_callback_add(sd->highlight.highlight,
                                  EVAS_CALLBACK_MOUSE_MOVE,
                                  _ws_mouse_move_cb, obj);
   evas_object_smart_callback_add(sd->highlight.highlight, "hl,resize",
                                  _on_resize, obj);

   return true;
}

Eina_Bool
workspace_highlight_unset(Evas_Object *obj)
{
   if (!obj) return false;
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   highlight_object_unfollow(sd->highlight.highlight);
   highlight_object_unfollow(sd->highlight.space_hl);
   sd->highlight.part = NULL;
   evas_object_hide(sd->highlight.space_hl);
   evas_object_hide(sd->highlight.highlight);

   evas_object_event_callback_del(sd->highlight.highlight,
                                  EVAS_CALLBACK_MOUSE_MOVE,
                                  _ws_mouse_move_cb);
   evas_object_smart_callback_del(sd->highlight.highlight, "hl,resize",
                                  _on_resize);
   return true;
}

/*-------------DEPRECATED functions-----------------------*/

DEPRECATED Eina_Bool
ws_object_highlight_move(Evas_Object *obj __UNUSED__)
{
   /*
   int x, y, w, h;

   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   if(!sd->highlight.part) return false;

   evas_object_geometry_get(sd->highlight.part->obj, &x, &y, &w, &h);
   evas_object_move(sd->highlight.highlight, x, y);
   evas_object_resize(sd->highlight.highlight, w, h);

   Group *group = ui_groupspace_group_get(sd->groupspace);
   ui_groupspace_part_space_geometry_get(group, sd->highlight.part,
                                         &x, &y, &w, &h);
   evas_object_resize(sd->highlight.space_hl, w, h);
   evas_object_move(sd->highlight.space_hl, x, y);
   return true;
   */
   return true;
}

DEPRECATED Eina_Bool
ws_object_highlight_handler_move(Evas_Object *obj __UNUSED__)
{
   /*
   int x, y, w, h;

   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   if(!sd->highlight.part) return false;

   evas_object_geometry_get(sd->highlight.part->obj, &x, &y, &w, &h);
   evas_object_move(sd->highlight.highlight, x, y);
   evas_object_resize(sd->highlight.highlight, w, h);


   Group *group = ui_groupspace_group_get(sd->groupspace);
   ui_groupspace_part_space_geometry_get(group, sd->highlight.part, &x, &y, &w, &h);
   evas_object_resize(sd->highlight.space_hl, w, h);
   evas_object_move(sd->highlight.space_hl, x, y);
   return true;
   */
   return true;
}

DEPRECATED Eina_Bool
ws_object_highlight_hide(Evas_Object *obj __UNUSED__)
{
   /*
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   if (!sd->highlight.part) return false;

   evas_object_hide(sd->highlight.highlight);
   evas_object_hide(sd->highlight.space_hl);
   return true;
   */
   return true;
}

DEPRECATED Eina_Bool
ws_object_highlight_del(Evas_Object *obj __UNUSED__)
{
   /*
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   if (!sd->highlight.part) return false;

   evas_object_del(sd->highlight.highlight);
   evas_object_del(sd->highlight.space_hl);
   sd->highlight.highlight = NULL;
   sd->highlight.space_hl = NULL;
   sd->highlight.part = NULL;

   return true;
   */
   return true;
}

DEPRECATED Evas_Object *
ws_space_hl_get(Evas_Object *obj __UNUSED__)
{
   /*
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, NULL)
   return sd->highlight.space_hl;
   */
   return NULL;
}

DEPRECATED Evas_Object *
ws_highlight_get(Evas_Object *obj __UNUSED__)
{
   /*
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, NULL)
   return sd->highlight.highlight;
   */
   return NULL;
}

DEPRECATED Eina_Bool
ws_separated_mode_get(Evas_Object *obj __UNUSED__)
{
   /*
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)
   return sd->separated;
   */
   return false;
}

DEPRECATED Eina_Bool
ws_separated_mode_set(Evas_Object *obj __UNUSED__, Eina_Bool separated __UNUSED__)
{
   /*
   Evas_Object *icon = NULL;
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false)

   icon = elm_object_part_content_get(sd->button_separate, NULL);
   sd->separated = separated;
   if (!separated)
     {
        evas_object_color_set(icon, 58, 110, 155, 255);
        ws_object_highlight_del(obj);
    }
   else
     {
        evas_object_color_set(icon, 255, 255, 255, 255);
        ws_object_highlight_set(obj, sd->highlight.part);
        ws_object_highlight_move(obj);
     }
   elm_object_part_content_set(sd->button_separate, NULL, icon);

   return true;
   */
   return true;
}

/*-------------End of DEPRECATED functions------------------*/

static void
_workspace_smart_add(Evas_Object *o)
{
   /* Allocate memory for workspace smart data*/
   EVAS_SMART_DATA_ALLOC(o, Ws_Smart_Data)

   _workspace_parent_sc->add(o);
}

static Eina_Bool
_workspace_child_create(Evas_Object *o, Evas_Object *parent)
{
   WS_DATA_GET_OR_RETURN_VAL(o, priv, false);

   Evas *e = evas_object_evas_get(o);
   Evas_Object *icon = NULL;

   /* Load main markup layout into smart_data->obj*/
   priv->obj = elm_layout_add(parent);
   elm_layout_file_set(priv->obj, TET_EDJ, "elm/workspace/base/smart");
   evas_object_smart_member_add(priv->obj, o);

   /* Here create evas image, whitch will be background for workspace*/
   priv->background = evas_object_image_filled_add(e);
   _background_load(priv->background, TET_IMG_PATH"bg_demo.png");
   evas_object_smart_member_add(priv->background, o);

   /* Clipper needed for check mouse events*/
   priv->clipper = evas_object_rectangle_add(e);
   evas_object_color_set(priv->clipper, 0, 0, 0, 0);
   evas_object_smart_member_add(priv->clipper, o);
   evas_object_event_callback_add(priv->clipper, EVAS_CALLBACK_MOUSE_MOVE,
                                  _ws_smart_mouse_move_cb, o);
   evas_object_event_callback_add(priv->clipper, EVAS_CALLBACK_MOUSE_DOWN,
                                  _ws_smart_mouse_click_cb, o);

   /* button for switch mode of view: separated or normal*/
   priv->button_separate = elm_button_add(priv->obj);
   elm_object_style_set(priv->button_separate, "eflete/simple");
   icon = elm_icon_add(priv->button_separate);
   elm_image_file_set(icon, TET_IMG_PATH"icon-separate.png", NULL);
   elm_image_no_scale_set(icon, true);
   elm_object_part_content_set(priv->button_separate, NULL, icon);
   evas_object_smart_callback_add(priv->button_separate, "clicked",
                                  _separate_smart_on_click, o);
   evas_object_smart_member_add(priv->button_separate, o);

   /* create empty groupspace object */
   priv->groupedit = NULL;
   //evas_object_smart_member_add(priv->groupedit, o);
   //evas_object_smart_callback_add(o, "ruler,move",  _ws_smart_ruler_move_cb,
   //                               NULL);

   /* using scroller in workspace, with special style*/
   priv->scroller = elm_scroller_add(priv->obj);
   elm_object_style_set(priv->scroller, "eflete/workspace");
   elm_scroller_policy_set(priv->scroller, ELM_SCROLLER_POLICY_ON,
                           ELM_SCROLLER_POLICY_ON);
   elm_scroller_content_min_limit(priv->scroller, false, false);
   elm_object_part_content_set(priv->obj, "groupspace", priv->scroller);

   evas_object_smart_callback_add(priv->scroller, "scroll", _sc_smart_move_cb, o);
   evas_object_smart_callback_add(priv->scroller, "scroll,drag,stop",
                                  _sc_smart_move_cb, o);
   evas_object_event_callback_add(priv->scroller, EVAS_CALLBACK_RESIZE,
                                  _sc_smart_resize_cb, o);
   evas_object_smart_member_add(priv->scroller, o);


   /* Add members of workspace into scroller markup field''s*/
   elm_object_part_content_set(priv->scroller, "elm.swallow.background",
                               priv->background);
   elm_object_part_content_set(priv->scroller, "elm.swallow.overlay",
                               priv->clipper);
   elm_object_part_content_set(priv->scroller, "cross.swallow",
                               priv->button_separate);
   //elm_object_content_set(priv->scroller, priv->groupedit);

   /* create rulers, using ui_ruler.h API*/
   priv->ruler_hor = ui_ruler_add(priv->scroller);
   elm_object_part_content_set(priv->scroller, "ruler.swallow.hor", priv->ruler_hor);
   evas_object_smart_callback_add(o, "ruler,hide,hor",
                                  _ws_ruler_hide_cb, priv->ruler_hor);
   evas_object_smart_member_add(priv->ruler_hor, o);

   priv->ruler_ver = ui_ruler_add(priv->scroller);
   ui_ruler_orient_set(priv->ruler_ver, VERTICAL);
   elm_object_part_content_set(priv->scroller, "ruler.swallow.ver", priv->ruler_ver);
   evas_object_smart_callback_add(o, "ruler,hide,ver",
                                  _ws_ruler_hide_cb, priv->ruler_ver);
   evas_object_smart_member_add(priv->ruler_ver, o);


   /* Simple initialize variavbles of smart data */
   priv->zoom.factor = 1.0;
   priv->zoom.control = NULL;

   priv->legend.view = NULL;
   priv->legend.visible = false;

   /*TODO: DEPRECATED <- delete */
   //priv->separated = true;

   priv->group = NULL;
   priv->guides = NULL;

   /* Create highlights for object and relative space */
   /*TODO: remake scroller and layout with rulers etc.
           because highlight work wrong because of that */
   priv->highlight.space_hl = highlight_add(priv->scroller);
   highlight_bg_color_set(priv->highlight.space_hl, OBG_AREA_BG_COLOR);
   highlight_border_color_set(priv->highlight.space_hl, OBG_AREA_COLOR);
   highlight_handler_disabled_set(priv->highlight.space_hl, true);
   evas_object_smart_member_add(priv->highlight.space_hl, o);

   priv->highlight.highlight = highlight_add(priv->scroller);
   highlight_bg_color_set(priv->highlight.highlight, HIGHLIGHT_BG_COLOR);
   highlight_handler_color_set(priv->highlight.highlight, HIGHLIGHT_COLOR);
   highlight_border_color_set(priv->highlight.highlight, HIGHLIGHT_COLOR);
   evas_object_smart_member_add(priv->highlight.highlight, o);

   return true;
}


static void
_workspace_smart_del(Evas_Object *o)
{
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID);

   evas_object_smart_member_del(sd->ruler_hor);
   evas_object_smart_member_del(sd->ruler_ver);
   evas_object_smart_member_del(sd->obj);
   evas_object_smart_member_del(sd->background);
   evas_object_smart_member_del(sd->button_separate);
   evas_object_smart_member_del(sd->scroller);
   evas_object_smart_member_del(sd->clipper);
   evas_object_smart_member_del(sd->highlight.highlight);
   evas_object_smart_member_del(sd->highlight.space_hl);
   if (sd->groupedit)
     evas_object_smart_member_del(sd->groupedit);

   _workspace_parent_sc->del(o);
}

static void
_workspace_smart_show(Evas_Object *o)
{
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID)

   evas_object_show(sd->button_separate);
   evas_object_show(sd->scroller);
   evas_object_show(sd->clipper);

   if (sd->groupedit)
     evas_object_show(sd->groupedit);
   if (sd->ruler_hor)
     ui_ruler_show(sd->ruler_hor);
   if (sd->ruler_ver)
     ui_ruler_show(sd->ruler_ver);

   evas_object_show(sd->background);
   _workspace_parent_sc->show(o);
}

static void
_workspace_smart_hide(Evas_Object *o)
{
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID);

   if (sd->groupedit) evas_object_hide(sd->groupedit);
   if (sd->ruler_hor) ui_ruler_hide(sd->ruler_hor);
   if (sd->ruler_ver) ui_ruler_hide(sd->ruler_ver);

   evas_object_hide(sd->button_separate);
   evas_object_hide(sd->scroller);
   evas_object_hide(sd->clipper);

   _workspace_parent_sc->hide(o);
}

static void
_workspace_smart_resize(Evas_Object *o,
                            Evas_Coord w,
                            Evas_Coord h)
{
   Evas_Coord ox, oy, ow, oh;
   WS_DATA_GET_OR_RETURN_VAL(o, sd, RETURN_VOID)

   evas_object_geometry_get(o, &ox, &oy, &ow, &oh);
   if ((ow == w) && (oh == h)) return;
   evas_object_resize(sd->obj, w, h);

   evas_object_smart_changed(o);
}


static void
_workspace_smart_set_user(Evas_Smart_Class *sc)
{
   //evas_object_smart_clipped_smart_set(sc);
   sc->add = _workspace_smart_add;
   sc->del = _workspace_smart_del;
   sc->show = _workspace_smart_show;
   sc->hide = _workspace_smart_hide;

   sc->resize = _workspace_smart_resize;
   sc->calculate = NULL;
}


Evas_Object *
workspace_add(Evas_Object *parent)
{
   Evas *e = NULL;
   Evas_Object *obj = NULL;

   if (!parent) return NULL;

   e = evas_object_evas_get(parent);
   obj = evas_object_smart_add(e, _workspace_smart_class_new());
   if (_workspace_child_create(obj, parent)) return obj;
   else
     {
        evas_object_del(obj);
        return NULL;
     }
}

static void
_on_separete_close(void *data,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Evas_Object *follow;
   Ws_Smart_Data *sd = (Ws_Smart_Data *)data;
   const char *part_name = (const char *)event_info;

   if (!part_name) return;

   groupedit_part_object_area_set(sd->groupedit, part_name);

   follow = groupedit_edit_object_part_draw_get(sd->groupedit, part_name);
   highlight_object_follow(sd->highlight.highlight, follow);
   follow = groupedit_part_object_area_get(sd->groupedit);
   highlight_object_follow(sd->highlight.space_hl, follow);

   evas_object_hide(sd->highlight.space_hl);
   evas_object_show(sd->highlight.highlight);
}

Eina_Bool
workspace_edit_object_set(Evas_Object *obj, Group *group, const char *file)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);

   if ((!group) || (!file)) return false;
   if (!sd->groupedit) sd->groupedit = groupedit_add(sd->scroller);
   else groupedit_edit_object_unset(sd->groupedit);
   sd->group = group;
   groupedit_handler_size_set(sd->groupedit, 8, 8, 8, 8);
   groupedit_edit_object_set(sd->groupedit, group->obj, file);
   evas_object_smart_callback_add(sd->groupedit, "parts,separete,close",
                                  _on_separete_close, sd);
   groupedit_bg_set(sd->groupedit, sd->background);
   elm_object_content_set(sd->scroller, sd->groupedit);
   evas_object_show(sd->groupedit);

   return true;
}

void
workspace_edit_object_unset(Evas_Object *obj)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, RETURN_VOID);

   sd->group = NULL;
   if (sd->groupedit)
     {
        groupedit_edit_object_unset(sd->groupedit);
        elm_object_content_unset(sd->scroller);
        evas_object_del(sd->groupedit);
        sd->groupedit = NULL;
     }
}

Group *
workspace_edit_object_get(Evas_Object *obj)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, NULL);

   return sd->group;
}

void
workspace_edit_object_recalc(Evas_Object *obj)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, RETURN_VOID);
   groupedit_edit_object_recalc_all(sd->groupedit);
}

Eina_Bool
workspace_edit_object_part_add(Evas_Object *obj, const char *part,
                               Edje_Part_Type type, const char *data)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   if (!part) return false;

   return groupedit_edit_object_part_add(sd->groupedit, part, type, data);
}

Eina_Bool
workspace_edit_object_part_del(Evas_Object *obj, const char *part)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   if (!part) return false;

   return groupedit_edit_object_part_del(sd->groupedit, part);
}

Eina_Bool
workspace_edit_object_part_above(Evas_Object *obj, const char *part)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   if (!part) return false;

   return groupedit_edit_object_part_above(sd->groupedit, part);
}

Eina_Bool
workspace_edit_object_part_below(Evas_Object *obj, const char *part)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   if (!part) return false;

   return groupedit_edit_object_part_below(sd->groupedit, part);
}

Eina_Bool
workspace_edit_object_part_state_set(Evas_Object *obj, Part *part)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);

   if (!part) return false;
   return groupedit_edit_object_part_state_set(sd->groupedit, part->name,
                                               part->curr_state,
                                               part->curr_state_value);
}

Eina_Bool
workspace_edit_object_part_state_add(Evas_Object *obj, const char *part,
                                     const char *state, double value)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   if ((!part) || (!state)) return false;

   return groupedit_edit_object_part_state_add(sd->groupedit, part, state, value);
}

Eina_Bool
workspace_edit_object_part_state_del(Evas_Object *obj, const char *part,
                                     const char *state, double value)
{
   WS_DATA_GET_OR_RETURN_VAL(obj, sd, false);
   if ((!part) || (!state)) return false;

   return groupedit_edit_object_part_state_del(sd->groupedit, part, state, value);
}
