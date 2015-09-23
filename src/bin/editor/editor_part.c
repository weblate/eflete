/*
 * Edje Theme Editor
 * Copyright (C) 2013-2015 Samsung Electronics.
 *
 * This file is part of Edje Theme Editor.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; If not, see www.gnu.org/licenses/lgpl.html.
 */

#include "editor.h"
#include "editor_macro.h"

Eina_Bool
editor_part_effect_set(Evas_Object *edit_object, Change *change, Eina_Bool merge,
                       const char *part_name, Edje_Text_Effect new_val)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_PART_EFFECT;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   if (change)
     {
        Edje_Text_Effect old_value = edje_edit_part_effect_get(edit_object, part_name);
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_EDJETEXTEFFECT;
        diff->redo.function = editor_part_effect_set;
        diff->redo.args.type_sete.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_sete.ete2 = new_val;
        diff->undo.type = FUNCTION_TYPE_STRING_EDJETEXTEFFECT;
        diff->undo.function = editor_part_effect_set;
        diff->undo.args.type_sete.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_sete.ete2 = old_value;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (!edje_edit_part_effect_set(edit_object, part_name, new_val))
     return false;
   _editor_project_changed();
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

Eina_Bool
editor_part_ignore_flags_set(Evas_Object *edit_object, Change *change, Eina_Bool merge,
                             const char *part_name, Evas_Event_Flags new_val)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_PART_IGNORE_FLAGS;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   if (change)
     {
        Evas_Event_Flags old_value = edje_edit_part_ignore_flags_get(edit_object, part_name);
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_EVASEVENTFLAGS;
        diff->redo.function = editor_part_ignore_flags_set;
        diff->redo.args.type_seef.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_seef.eef2 = new_val;
        diff->undo.type = FUNCTION_TYPE_STRING_EVASEVENTFLAGS;
        diff->undo.function = editor_part_ignore_flags_set;
        diff->undo.args.type_seef.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_seef.eef2 = old_value;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (!edje_edit_part_ignore_flags_set(edit_object, part_name, new_val))
     return false;
   _editor_project_changed();
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

EDITOR_PART_BOOL(mouse_events, ATTRIBUTE_PART_MOUSE_EVENTS)
EDITOR_PART_BOOL(repeat_events, ATTRIBUTE_PART_REPEAT_EVENTS)
EDITOR_PART_BOOL(scale, ATTRIBUTE_PART_SCALE)

EDITOR_PART_INT(drag_count_x, ATTRIBUTE_PART_DRAG_COUNT_X)
EDITOR_PART_INT(drag_count_y, ATTRIBUTE_PART_DRAG_COUNT_Y)
EDITOR_PART_INT(drag_x, ATTRIBUTE_PART_DRAG_X)
EDITOR_PART_INT(drag_y, ATTRIBUTE_PART_DRAG_Y)
EDITOR_PART_INT(drag_step_x, ATTRIBUTE_PART_DRAG_STEP_X)
EDITOR_PART_INT(drag_step_y, ATTRIBUTE_PART_DRAG_STEP_Y)

EDITOR_STRING_STRING(part_clip_to, ATTRIBUTE_PART_CLIP_TO)
EDITOR_STRING_STRING(part_drag_confine, ATTRIBUTE_PART_DRAG_CONFINE)
EDITOR_STRING_STRING(part_drag_threshold, ATTRIBUTE_PART_DRAG_THRESHOLD)
EDITOR_STRING_STRING(part_drag_event, ATTRIBUTE_PART_DRAG_EVENT)
EDITOR_STRING_STRING(part_source, ATTRIBUTE_PART_SOURCE)

Eina_Bool
editor_part_name_set(Evas_Object *edit_object, Change *change, Eina_Bool merge,
                     const char *name, const char *new_val)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_PART_NAME;
   assert(edit_object != NULL);
   assert(name != NULL);
   assert(new_val != NULL);
   if (change)
     {
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING;
        diff->redo.function = editor_part_name_set;
        diff->redo.args.type_ssds.s1 = eina_stringshare_add(name);
        diff->redo.args.type_ssds.s2 = eina_stringshare_add(new_val);
        diff->undo.type = FUNCTION_TYPE_STRING_STRING;
        diff->undo.function = editor_part_name_set;
        diff->undo.args.type_ssds.s1 = eina_stringshare_add(new_val);
        diff->undo.args.type_ssds.s2 = eina_stringshare_add(name);
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (!edje_edit_part_name_set(edit_object, name, new_val))
     return false;
   _editor_project_changed();
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

Eina_Bool
editor_part_item_aspect_mode_set(Evas_Object *edit_object, Change *change, Eina_Bool merge,
                                 const char *part_name, const char *item_name, Edje_Aspect_Control new_val)
{
   Diff *diff;
   Attribute attribute = ATTRIBUTE_PART_ITEM_ASPECT_MODE;
   assert(edit_object != NULL);
   assert(part_name != NULL);
   assert(item_name != NULL);
   if (change)
     {
        Edje_Aspect_Control old_value = edje_edit_part_ignore_flags_get(edit_object, part_name);
        diff = mem_calloc(1, sizeof(Diff));
        diff->redo.type = FUNCTION_TYPE_STRING_STRING_EDJEASPECTCONTROL;
        diff->redo.function = editor_part_item_aspect_mode_set;
        diff->redo.args.type_sseac.s1 = eina_stringshare_add(part_name);
        diff->redo.args.type_sseac.s2 = eina_stringshare_add(item_name);
        diff->redo.args.type_sseac.eac3 = new_val;
        diff->undo.type = FUNCTION_TYPE_STRING_STRING_EDJEASPECTCONTROL;
        diff->undo.function = editor_part_item_aspect_mode_set;
        diff->undo.args.type_sseac.s1 = eina_stringshare_add(part_name);
        diff->undo.args.type_sseac.s2 = eina_stringshare_add(item_name);
        diff->undo.args.type_sseac.eac3 = old_value;
        if (merge)
          change_diff_merge_add(change, diff);
        else
          change_diff_add(change, diff);
     }
   if (!edje_edit_part_item_aspect_mode_set(edit_object, part_name, item_name, new_val))
     return false;
   _editor_project_changed();
   evas_object_smart_callback_call(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, &attribute);
   return true;
}

EDITOR_PART_ITEM_DOUBLE(align_x, ATTRIBUTE_PART_ITEM_ALIGN_X);
EDITOR_PART_ITEM_DOUBLE(align_y, ATTRIBUTE_PART_ITEM_ALIGN_Y);
EDITOR_PART_ITEM_DOUBLE(weight_x, ATTRIBUTE_PART_ITEM_WEIGHT_X);
EDITOR_PART_ITEM_DOUBLE(weight_y, ATTRIBUTE_PART_ITEM_WEIGHT_Y);

EDITOR_PART_ITEM_INT(aspect_h, ATTRIBUTE_PART_ITEM_ASPECT_H);
EDITOR_PART_ITEM_INT(aspect_w, ATTRIBUTE_PART_ITEM_ASPECT_W);
EDITOR_PART_ITEM_INT(max_h, ATTRIBUTE_PART_ITEM_MAX_H);
EDITOR_PART_ITEM_INT(max_w, ATTRIBUTE_PART_ITEM_MAX_W);
EDITOR_PART_ITEM_INT(min_h, ATTRIBUTE_PART_ITEM_MIN_H);
EDITOR_PART_ITEM_INT(min_w, ATTRIBUTE_PART_ITEM_MIN_W);
EDITOR_PART_ITEM_INT(prefer_h, ATTRIBUTE_PART_ITEM_PREFER_H);
EDITOR_PART_ITEM_INT(prefer_w, ATTRIBUTE_PART_ITEM_PREFER_W);
EDITOR_PART_ITEM_INT(spread_h, ATTRIBUTE_PART_ITEM_SPREAD_H);
EDITOR_PART_ITEM_INT(spread_w, ATTRIBUTE_PART_ITEM_SPREAD_W);

EDITOR_PART_ITEM_USHORT(span_col, ATTRIBUTE_PART_ITEM_SPAN_COL);
EDITOR_PART_ITEM_USHORT(span_row, ATTRIBUTE_PART_ITEM_SPAN_ROW);
EDITOR_PART_ITEM_USHORT(position_col, ATTRIBUTE_PART_ITEM_POSITION_COL);
EDITOR_PART_ITEM_USHORT(position_row, ATTRIBUTE_PART_ITEM_POSITION_ROW);
