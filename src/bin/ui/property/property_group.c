/*
 * Edje Theme Editor
 * Copyright (C) 2013-2016 Samsung Electronics.
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

#include "property.h"
#include "property_private.h"
#include "group_manager.h"
#include "history.h"
#include "change.h"
#include "project_manager.h"

struct _Property_Group_Data {
   Group *group;
   Part *part;
   Eina_Stringshare *program;

   /* data needed to correctly handle changes that will be passed to history module */
   struct {
      const char *format;
      Change *change;
      struct {
         int int_val1;
         Eina_Stringshare *str_val1;
      } old, new;
   } history;

   struct {
        struct {
             Property_Attribute title;
             Property_Attribute name;
             Property_Attribute min;
             Property_Attribute max;
        } group;
        struct {
             Property_Attribute title;
             Property_Attribute name;
             Property_Attribute type;
             Property_Attribute scale;
             Property_Attribute mouse_events;
             Property_Attribute repeat_events;
             Property_Attribute clip_to;
             Property_Attribute ignore_flags;
             struct {
                  Property_Attribute title;
                  Property_Attribute enable;
                  Property_Attribute step;
                  Property_Attribute count;
                  Property_Attribute confine;
                  Property_Attribute threshold;
                  Property_Attribute events;
             } dragable;
             struct {
                  Property_Attribute title;
                  Property_Attribute source;
             } type_group;
        } part;
        struct {
             Property_Attribute title;
             Property_Attribute name;
             Property_Attribute visible;
        } state;
        Property_Attribute item;
        Property_Attribute program;
   } items;
};
typedef struct _Property_Group_Data Property_Group_Data;
static Property_Group_Data group_pd;

struct _Property_Group_Update_Info {
   Property_Attribute *pa;
   Property_Action *action;
};
typedef struct _Property_Group_Update_Info Property_Group_Update_Info;

/* array to find item by Attribute */
static Property_Group_Update_Info attribute_map[ATTRIBUTE_LAST];

/* mapping enums to strings */
static const char *ignore_flags_strings[] = { STR_NONE,
                                              "On hold",
                                              NULL};
/* defines for args */
#define EDIT_OBJ group_pd.group->edit_object
#define PART_ARGS group_pd.part->name
#define STATE_ARGS PART_ARGS, group_pd.part->current_state->parsed_name, group_pd.part->current_state->parsed_val
#define CHANGE_MERGE group_pd.history.change, true
#define CHANGE_NO_MERGE group_pd.history.change, false

/* global callbacks */
static void
_on_part_selected(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   group_pd.part = event_info;

   assert(group_pd.part != NULL);

   DBG("selected part \"%s\"", PART_ARGS);
   property_item_del(&group_pd.items.program);

   property_item_add(&group_pd.items.part.title, NULL);
   property_item_add(&group_pd.items.state.title, NULL);
   if (group_pd.part->current_item_name)
     property_item_add(&group_pd.items.item, NULL);
   else
     property_item_del(&group_pd.items.item);

   /* type-specific in part block */
   if (group_pd.items.part.title.expanded)
     {
        property_item_del(&group_pd.items.part.type_group.title);
        switch (group_pd.part->type)
          {
           case EDJE_PART_TYPE_GROUP:
              property_item_add(&group_pd.items.part.type_group.title, group_pd.items.part.title.glit);
              break;
           default:
              break;
          }
     }

   property_item_update_recursively(&group_pd.items.part.title);
   property_item_update_recursively(&group_pd.items.state.title);
}

static void
_on_group_navigator_unselected(void *data __UNUSED__,
                               Evas_Object *obj __UNUSED__,
                               void *event_info __UNUSED__)
{
   DBG("unselected_cb\n");
   group_pd.part = NULL;
   group_pd.program = NULL;
   property_item_del(&group_pd.items.part.title);
   property_item_del(&group_pd.items.state.title);
   property_item_del(&group_pd.items.item);
   property_item_del(&group_pd.items.program);
}

static void
_on_part_state_selected(void *data __UNUSED__,
                        Evas_Object *obj __UNUSED__,
                        void *event_info)
{
   Part *part = event_info;

   assert(group_pd.part == part);

   DBG("selected state \"%s\"", group_pd.part->current_state->name);
   property_item_update_recursively(&group_pd.items.state.title);
}

static void
_on_program_selected(void *data __UNUSED__,
                     Evas_Object *obj __UNUSED__,
                     void *event_info)
{
   group_pd.program = event_info;

   assert(group_pd.program != NULL);

   DBG("selected program \"%s\"", group_pd.program);
   property_item_del(&group_pd.items.part.title);
   property_item_del(&group_pd.items.state.title);
   property_item_del(&group_pd.items.item);

   property_item_add(&group_pd.items.program, NULL);
}

static void
_on_group_changed(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info)
{
   group_pd.group = event_info;

   assert(group_pd.group != NULL);

   DBG("group changed to \"%s\"", group_pd.group->name);
   group_pd.part = group_pd.group->current_part;
   if (group_pd.part)
     {
        _on_part_selected(NULL, NULL, group_pd.part);
     }
   else
     {
        property_item_del(&group_pd.items.part.title);
        property_item_del(&group_pd.items.state.title);
     }
   group_pd.program = group_pd.group->current_program;
   property_item_update_recursively(&group_pd.items.group.title);
}

static void
_on_editor_attribute_changed(void *data __UNUSED__,
                             Evas_Object *obj __UNUSED__,
                             void *event_info)
{
   Attribute *attribute = event_info;
   Property_Attribute *pa = attribute_map[*attribute].pa;
   Property_Action *action = attribute_map[*attribute].action;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->update_cb != NULL);

   if (pa->realized)
     action->update_cb(pa, action);
}

/* local callbacks */
static Eina_List *
_subitems_get(Property_Attribute *pa)
{
   Eina_List *items = NULL;

   assert(pa != NULL);

   if (pa == &group_pd.items.group.title)
     {
         items = eina_list_append(items, &group_pd.items.group.name);
         items = eina_list_append(items, &group_pd.items.group.min);
         items = eina_list_append(items, &group_pd.items.group.max);
     }
   else if (pa == &group_pd.items.part.title)
     {
         items = eina_list_append(items, &group_pd.items.part.name);
         items = eina_list_append(items, &group_pd.items.part.type);
         items = eina_list_append(items, &group_pd.items.part.scale);
         items = eina_list_append(items, &group_pd.items.part.mouse_events);
         items = eina_list_append(items, &group_pd.items.part.repeat_events);
         items = eina_list_append(items, &group_pd.items.part.clip_to);
         items = eina_list_append(items, &group_pd.items.part.ignore_flags);
         items = eina_list_append(items, &group_pd.items.part.dragable.title);
         switch (group_pd.part->type)
           {
            case EDJE_PART_TYPE_GROUP:
               items = eina_list_append(items, &group_pd.items.part.type_group.title);
               break;
            default:
               break;
           }
     }
   else if (pa == &group_pd.items.state.title)
     {
         items = eina_list_append(items, &group_pd.items.state.name);
         items = eina_list_append(items, &group_pd.items.state.visible);
     }
   else if (pa == &group_pd.items.part.dragable.title)
     {
         items = eina_list_append(items, &group_pd.items.part.dragable.enable);
         items = eina_list_append(items, &group_pd.items.part.dragable.step);
         items = eina_list_append(items, &group_pd.items.part.dragable.count);
         items = eina_list_append(items, &group_pd.items.part.dragable.confine);
         items = eina_list_append(items, &group_pd.items.part.dragable.threshold);
         items = eina_list_append(items, &group_pd.items.part.dragable.events);
     }
   else if (pa == &group_pd.items.part.type_group.title)
     {
         items = eina_list_append(items, &group_pd.items.part.type_group.source);
     }
   else
     {
        CRIT("items callback not found for %s", pa->name);
        abort();
     }
   return items;
}

static void
_fill_combobox_with_enum(Evas_Object *control, const char **array)
{
   int i = 0;

   assert(control != NULL);
   assert(array != NULL);

   while (array[i] != NULL)
     {
        ewe_combobox_item_add(control, array[i]);
        ++i;
     }
}

static void
_init_cb(Property_Attribute *pa, Property_Action *action)
{
   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
      case ATTRIBUTE_STATE_NAME:
         elm_object_disabled_set(action->control, true);
         break;
      case ATTRIBUTE_GROUP_MIN_W:
      case ATTRIBUTE_GROUP_MIN_H:
      case ATTRIBUTE_GROUP_MAX_W:
      case ATTRIBUTE_GROUP_MAX_H:
      case ATTRIBUTE_PART_NAME:
      case ATTRIBUTE_PART_TYPE:
      case ATTRIBUTE_PART_SCALE:
      case ATTRIBUTE_PART_MOUSE_EVENTS:
      case ATTRIBUTE_PART_REPEAT_EVENTS:
      case ATTRIBUTE_PART_CLIP_TO:
      case ATTRIBUTE_PART_DRAG_STEP_X:
      case ATTRIBUTE_PART_DRAG_STEP_Y:
      case ATTRIBUTE_PART_DRAG_COUNT_X:
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
      case ATTRIBUTE_PART_DRAG_CONFINE:
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
      case ATTRIBUTE_PART_DRAG_EVENT:
      case ATTRIBUTE_PART_GROUP_SOURCE:
      case ATTRIBUTE_STATE_VISIBLE:
         break;
      case ATTRIBUTE_PART_DRAG_X:
      case ATTRIBUTE_PART_DRAG_Y:
         elm_spinner_min_max_set(action->control, -1, 1);
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         _fill_combobox_with_enum(action->control, ignore_flags_strings);
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("init callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

static void
_groups_combobox_fill(Evas_Object *combo, const char *selected)
{
   Eina_List *l;
   Group *group;

   assert(combo != NULL);

   if (selected)
     ewe_combobox_text_set(combo, selected);
   else
     ewe_combobox_text_set(combo, STR_NONE);
   ewe_combobox_item_add(combo, STR_NONE);
   EINA_LIST_FOREACH(ap.project->groups, l, group)
     {
        if (group != group_pd.group)
          ewe_combobox_item_add(combo, group->name);
     }
}

static void
_parts_combobox_fill(Evas_Object *combo, const char *selected, Edje_Part_Type allowed_type)
{
   Eina_List *l;
   Part *part;

   assert(combo != NULL);

   if (selected)
     ewe_combobox_text_set(combo, selected);
   else
     ewe_combobox_text_set(combo, STR_NONE);
   ewe_combobox_item_add(combo, STR_NONE);

   if (allowed_type)
     {
        EINA_LIST_FOREACH(group_pd.group->parts, l, part)
          {
             if ((part->type == allowed_type) && (part != group_pd.part))
               ewe_combobox_item_add(combo, part->name);
          }
     }
   else
     {
        EINA_LIST_FOREACH(group_pd.group->parts, l, part)
          {
             if (part != group_pd.part)
               ewe_combobox_item_add(combo, part->name);
          }
     }
}

static void
_update_cb(Property_Attribute *pa, Property_Action *action)
{
   int int_val1;
   Eina_Bool bool_val1;
   Eina_Stringshare *str_val1;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
         property_entry_set(action->control, group_pd.group->name);
         break;
      case ATTRIBUTE_STATE_NAME:
         property_entry_set(action->control, group_pd.part->current_state->name);
         break;
      case ATTRIBUTE_GROUP_MIN_W:
         int_val1 = edje_edit_group_min_w_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_GROUP_MIN_H:
         int_val1 = edje_edit_group_min_h_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_GROUP_MAX_W:
         int_val1 = edje_edit_group_max_w_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_GROUP_MAX_H:
         int_val1 = edje_edit_group_max_h_get(EDIT_OBJ);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_NAME:
         property_entry_set(action->control, PART_ARGS);
         break;
      case ATTRIBUTE_PART_TYPE:
         elm_layout_text_set(action->control, NULL, gm_part_type_text_get(group_pd.part->type));
         break;
      case ATTRIBUTE_PART_SCALE:
         bool_val1 = edje_edit_part_scale_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         bool_val1 = edje_edit_part_mouse_events_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         bool_val1 = edje_edit_part_repeat_events_get(EDIT_OBJ, PART_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      case ATTRIBUTE_PART_CLIP_TO:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_clip_to_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, EDJE_PART_TYPE_RECTANGLE);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         ewe_combobox_select_item_set(action->control,
           (int) edje_edit_part_ignore_flags_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_DRAG_X:
         int_val1 = edje_edit_part_drag_x_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_Y:
         int_val1 = edje_edit_part_drag_y_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_X:
         int_val1 = edje_edit_part_drag_step_x_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_Y:
         int_val1 = edje_edit_part_drag_step_y_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_X:
         int_val1 = edje_edit_part_drag_count_x_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         int_val1 = edje_edit_part_drag_count_y_get(EDIT_OBJ, PART_ARGS);
         elm_spinner_value_set(action->control, int_val1);
         break;
      case ATTRIBUTE_PART_DRAG_CONFINE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_drag_confine_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_drag_threshold_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_DRAG_EVENT:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_drag_event_get(EDIT_OBJ, PART_ARGS);
         _parts_combobox_fill(action->control, str_val1, 0);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_PART_GROUP_SOURCE:
         ewe_combobox_items_list_free(action->control, true);
         str_val1 = edje_edit_part_source_get(EDIT_OBJ, PART_ARGS);
         _groups_combobox_fill(action->control, str_val1);
         edje_edit_string_free(str_val1);
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         bool_val1 = edje_edit_state_visible_get(EDIT_OBJ, STATE_ARGS);
         elm_check_state_set(action->control, bool_val1);
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("update callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

static void
_start_cb(Property_Attribute *pa, Property_Action *action)
{
   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);
   assert(group_pd.history.change == NULL);

   group_pd.history.change = change_add(NULL);

   /* setting new and old to current value. this will be checked on stop to ignore
      empty changes (i.e. from 0 to 0) */
#define VAL(NAME) \
 group_pd.history.new.NAME = group_pd.history.old.NAME
#define STR_VAL(NAME, VAL) \
 group_pd.history.new.NAME = VAL; \
 group_pd.history.old.NAME = eina_stringshare_add(group_pd.history.new.NAME);

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
         group_pd.history.format = _("group name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(group_pd.group->name));
         break;
      case ATTRIBUTE_STATE_NAME:
         group_pd.history.format = _("state name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(group_pd.part->current_state->name));
         break;
      case ATTRIBUTE_GROUP_MIN_W:
         group_pd.history.format = _("group.min_w changed from %d to %d");
         VAL(int_val1) = edje_edit_group_min_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MIN_H:
         group_pd.history.format = _("group.min_h changed from %d to %d");
         VAL(int_val1) = edje_edit_group_min_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_W:
         group_pd.history.format = _("group.max_w changed from %d to %d");
         VAL(int_val1) = edje_edit_group_max_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_H:
         group_pd.history.format = _("group.max_h changed from %d to %d");
         VAL(int_val1) = edje_edit_group_max_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_PART_NAME:
         group_pd.history.format = _("part name changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(PART_ARGS));
         break;
      case ATTRIBUTE_PART_TYPE:
         /* part type can't be changed */
         break;
      case ATTRIBUTE_PART_SCALE:
         group_pd.history.format = _("part scale %s");
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         group_pd.history.format = _("mouse events %s");
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         group_pd.history.format = _("repeat events %s");
         break;
      case ATTRIBUTE_PART_CLIP_TO:
         group_pd.history.format = _("clip to changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_clip_to_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         group_pd.history.format = _("ignore_flags changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, eina_stringshare_add(
            ignore_flags_strings[edje_edit_part_ignore_flags_get(EDIT_OBJ, PART_ARGS)]));
         break;
      case ATTRIBUTE_PART_DRAG_X:
         group_pd.history.format = _("dragable enable x changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_Y:
         group_pd.history.format = _("dragable enable y changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_X:
         group_pd.history.format = _("dragable step x changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_step_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_Y:
         group_pd.history.format = _("dragable step y changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_step_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_X:
         group_pd.history.format = _("dragable count x changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_count_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         group_pd.history.format = _("dragable count y changed from %d to %d");
         VAL(int_val1) = edje_edit_part_drag_count_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_CONFINE:
         group_pd.history.format = _("dragable confine changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_drag_confine_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
         group_pd.history.format = _("dragable threshold changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_drag_threshold_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_DRAG_EVENT:
         group_pd.history.format = _("dragable event changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_drag_event_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_PART_GROUP_SOURCE:
         group_pd.history.format = _("source changed from \"%s\" to \"%s\"");
         STR_VAL(str_val1, edje_edit_part_source_get(EDIT_OBJ, PART_ARGS));
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         group_pd.history.format = _("state visible %s");
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("start callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
#undef VAL
}

static void
_change_cb(Property_Attribute *pa, Property_Action *action)
{
   double double_val1 = 0.0;
   Eina_Stringshare *str_val1 = NULL;
   Eina_Bool bool_val1 = false;;
   Ewe_Combobox_Item *cb_item = NULL;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);
   assert(group_pd.history.change != NULL);

   switch (action->control_type)
     {
      case PROPERTY_CONTROL_SPINNER:
         double_val1 = elm_spinner_value_get(action->control);
         break;
      case PROPERTY_CONTROL_ENTRY:
         str_val1 = property_entry_get(action->control);
         break;
      case PROPERTY_CONTROL_CHECK:
         bool_val1 = elm_check_state_get(action->control);
         break;
      case PROPERTY_CONTROL_COMBOBOX:
         TODO("change this after migrating to elm_combobox");
         cb_item = ewe_combobox_select_item_get(action->control);
      default:
         break;
     }

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
         TODO("implement group rename");
         break;
      case ATTRIBUTE_STATE_NAME:
         TODO("implement state rename");
         break;
      case ATTRIBUTE_GROUP_MIN_W:
         editor_group_min_w_set(EDIT_OBJ, CHANGE_MERGE, double_val1);
         group_pd.history.new.int_val1 = edje_edit_group_min_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MIN_H:
         editor_group_min_h_set(EDIT_OBJ, CHANGE_MERGE, double_val1);
         group_pd.history.new.int_val1 = edje_edit_group_min_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_W:
         editor_group_max_w_set(EDIT_OBJ, CHANGE_MERGE, double_val1);
         group_pd.history.new.int_val1 = edje_edit_group_max_w_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_GROUP_MAX_H:
         editor_group_max_h_set(EDIT_OBJ, CHANGE_MERGE, double_val1);
         group_pd.history.new.int_val1 = edje_edit_group_max_h_get(EDIT_OBJ);
         break;
      case ATTRIBUTE_PART_NAME:
         editor_part_name_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_TYPE:
         /* part type can't be changed */
         break;
      case ATTRIBUTE_PART_SCALE:
         editor_part_scale_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1);
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         editor_part_mouse_events_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1);
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         editor_part_repeat_events_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, bool_val1);
         break;
      case ATTRIBUTE_PART_CLIP_TO:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         editor_part_clip_to_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_IGNORE_FLAGS:
         str_val1 = eina_stringshare_add(cb_item->title);
         editor_part_ignore_flags_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, cb_item->index);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_DRAG_X:
         editor_part_drag_x_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1);
         group_pd.history.new.int_val1 = edje_edit_part_drag_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_Y:
         editor_part_drag_y_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1);
         group_pd.history.new.int_val1 = edje_edit_part_drag_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_X:
         editor_part_drag_step_x_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1);
         group_pd.history.new.int_val1 = edje_edit_part_drag_step_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_STEP_Y:
         editor_part_drag_step_y_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1);
         group_pd.history.new.int_val1 = edje_edit_part_drag_step_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_X:
         editor_part_drag_count_x_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1);
         group_pd.history.new.int_val1 = edje_edit_part_drag_count_x_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         editor_part_drag_count_y_set(EDIT_OBJ, CHANGE_MERGE, PART_ARGS, double_val1);
         group_pd.history.new.int_val1 = edje_edit_part_drag_count_y_get(EDIT_OBJ, PART_ARGS);
         break;
      case ATTRIBUTE_PART_DRAG_CONFINE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         editor_part_drag_confine_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         editor_part_drag_threshold_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_DRAG_EVENT:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         editor_part_drag_event_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_PART_GROUP_SOURCE:
         str_val1 = (cb_item->index != 0) ? eina_stringshare_add(cb_item->title) : NULL;
         editor_part_group_source_set(EDIT_OBJ, CHANGE_NO_MERGE, PART_ARGS, str_val1);
         eina_stringshare_del(group_pd.history.new.str_val1);
         group_pd.history.new.str_val1 = str_val1;
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         editor_state_visible_set(EDIT_OBJ, CHANGE_NO_MERGE, STATE_ARGS, bool_val1);
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("change callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

static void
_stop_cb(Property_Attribute *pa, Property_Action *action)
{
   Eina_Bool bool_val1;
   Eina_Stringshare *msg = NULL;

   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);
   assert(group_pd.history.change != NULL);

#define CHECK_VAL(NAME) \
 if (group_pd.history.new.NAME == group_pd.history.old.NAME) \
   { \
     change_free(group_pd.history.change); \
     goto clean; \
   }

   switch (action->type.attribute)
     {
      case ATTRIBUTE_GROUP_NAME:
      case ATTRIBUTE_PART_NAME:
      case ATTRIBUTE_STATE_NAME:
         CHECK_VAL(str_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.str_val1,
                                       group_pd.history.new.str_val1);
         break;
      case ATTRIBUTE_PART_CLIP_TO:
      case ATTRIBUTE_PART_IGNORE_FLAGS:
      case ATTRIBUTE_PART_DRAG_CONFINE:
      case ATTRIBUTE_PART_DRAG_THRESHOLD:
      case ATTRIBUTE_PART_DRAG_EVENT:
      case ATTRIBUTE_PART_GROUP_SOURCE:
         CHECK_VAL(str_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (group_pd.history.old.str_val1) ? group_pd.history.old.str_val1 : STR_NONE,
                                       (group_pd.history.new.str_val1) ? group_pd.history.new.str_val1 : STR_NONE);
         break;
      case ATTRIBUTE_GROUP_MIN_W:
      case ATTRIBUTE_GROUP_MIN_H:
      case ATTRIBUTE_GROUP_MAX_W:
      case ATTRIBUTE_GROUP_MAX_H:
      case ATTRIBUTE_PART_DRAG_X:
      case ATTRIBUTE_PART_DRAG_Y:
      case ATTRIBUTE_PART_DRAG_STEP_X:
      case ATTRIBUTE_PART_DRAG_STEP_Y:
      case ATTRIBUTE_PART_DRAG_COUNT_X:
      case ATTRIBUTE_PART_DRAG_COUNT_Y:
         CHECK_VAL(int_val1);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       group_pd.history.old.int_val1,
                                       group_pd.history.new.int_val1);
         break;
      case ATTRIBUTE_PART_TYPE:
         /* part type can't be changed */
         break;
      case ATTRIBUTE_PART_SCALE:
         bool_val1 = edje_edit_part_scale_get(EDIT_OBJ, PART_ARGS);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (bool_val1) ? _("turned on") : _("turned off"));
         break;
      case ATTRIBUTE_PART_MOUSE_EVENTS:
         bool_val1 = edje_edit_part_mouse_events_get(EDIT_OBJ, PART_ARGS);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (bool_val1) ? _("turned on") : _("turned off"));
         break;
      case ATTRIBUTE_PART_REPEAT_EVENTS:
         bool_val1 = edje_edit_part_repeat_events_get(EDIT_OBJ, PART_ARGS);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (bool_val1) ? _("turned on") : _("turned off"));
         break;
      case ATTRIBUTE_STATE_VISIBLE:
         bool_val1 = edje_edit_state_visible_get(EDIT_OBJ, STATE_ARGS);
         msg = eina_stringshare_printf(group_pd.history.format,
                                       (bool_val1) ? _("turned on") : _("turned off"));
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("stop callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
   change_description_set(group_pd.history.change, msg);
   history_change_add(group_pd.group->history, group_pd.history.change);
   eina_stringshare_del(msg);
#undef CHECK_VAL
clean:
   eina_stringshare_del(group_pd.history.new.str_val1);
   eina_stringshare_del(group_pd.history.old.str_val1);
   group_pd.history.change = NULL;
}

/* blocks */
static inline void
_action_internal(Property_Action *action, const char *name, const char *units,
                 Property_Control control_type, Attribute attribute)
{
   /* check that we haven't inited this action already */
   assert(action->update_cb == NULL);

   action->name = name;
   action->units = units;
   action->control_type = control_type;
   action->type.attribute = attribute;
   action->update_cb = _update_cb;
   action->init_cb = _init_cb;
   action->start_cb = _start_cb;
   action->stop_cb = _stop_cb;
   action->change_cb = _change_cb;
}

static inline void
_action1(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type, Attribute attribute)
{
   _action_internal(&pa->action1, name, units, control_type, attribute);

   assert(attribute_map[attribute].pa == NULL);
   assert(attribute_map[attribute].action == NULL);

   attribute_map[attribute].pa = pa;
   attribute_map[attribute].action = &pa->action1;
}

static inline void
_action2(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type, Attribute attribute)
{
   _action_internal(&pa->action2, name, units, control_type, attribute);

   assert(attribute_map[attribute].pa == NULL);
   assert(attribute_map[attribute].action == NULL);

   attribute_map[attribute].pa = pa;
   attribute_map[attribute].action = &pa->action2;
}

static void
_init_group_block()
{
   group_pd.items.group.title.name = "group";
   group_pd.items.group.title.expandable = true;
   group_pd.items.group.title.expanded = true;
   group_pd.items.group.title.expand_cb = _subitems_get;

   group_pd.items.group.name.name = "name";
   _action1(&group_pd.items.group.name, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_GROUP_NAME);

   group_pd.items.group.min.name = "min";
   _action1(&group_pd.items.group.min, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MIN_W);
   _action2(&group_pd.items.group.min, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MIN_H);

   group_pd.items.group.max.name = "max";
   _action1(&group_pd.items.group.max, "w", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MAX_W);
   _action2(&group_pd.items.group.max, "h", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_GROUP_MAX_H);
}

static void
_init_part_dragable_block()
{
   group_pd.items.part.dragable.title.name = "dragable";
   group_pd.items.part.dragable.title.expandable = true;
   group_pd.items.part.dragable.title.expanded = false;
   group_pd.items.part.dragable.title.expand_cb = _subitems_get;

   group_pd.items.part.dragable.enable.name = "enable";
   _action1(&group_pd.items.part.dragable.enable, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_X);
   _action2(&group_pd.items.part.dragable.enable, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_Y);

   group_pd.items.part.dragable.step.name = "step";
   _action1(&group_pd.items.part.dragable.step, "x", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_STEP_X);
   _action2(&group_pd.items.part.dragable.step, "y", "px", PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_STEP_Y);

   group_pd.items.part.dragable.count.name = "count";
   _action1(&group_pd.items.part.dragable.count, "x", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_COUNT_X);
   _action2(&group_pd.items.part.dragable.count, "y", NULL, PROPERTY_CONTROL_SPINNER, ATTRIBUTE_PART_DRAG_COUNT_Y);

   group_pd.items.part.dragable.confine.name = "confine";
   _action1(&group_pd.items.part.dragable.confine, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_DRAG_CONFINE);

   group_pd.items.part.dragable.threshold.name = "threshold";
   _action1(&group_pd.items.part.dragable.threshold, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_DRAG_THRESHOLD);

   group_pd.items.part.dragable.events.name = "events";
   _action1(&group_pd.items.part.dragable.events, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_DRAG_EVENT);
}

static void
_init_part_group_specific_block()
{
   group_pd.items.part.type_group.title.name = "type specific (GROUP)";
   group_pd.items.part.type_group.title.expandable = true;
   group_pd.items.part.type_group.title.expanded = true;
   group_pd.items.part.type_group.title.expand_cb = _subitems_get;

   group_pd.items.part.type_group.source.name = "source";
   _action1(&group_pd.items.part.type_group.source, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_GROUP_SOURCE);
}

static void
_init_part_block()
{
   group_pd.items.part.title.name = "part";
   group_pd.items.part.title.expandable = true;
   group_pd.items.part.title.expanded = true;
   group_pd.items.part.title.expand_cb = _subitems_get;

   group_pd.items.part.name.name = "name";
   _action1(&group_pd.items.part.name, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_PART_NAME);

   group_pd.items.part.type.name = "type";
   _action1(&group_pd.items.part.type, NULL, NULL, PROPERTY_CONTROL_LABEL, ATTRIBUTE_PART_TYPE);

   group_pd.items.part.scale.name = "scale";
   _action1(&group_pd.items.part.scale, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_SCALE);

   group_pd.items.part.mouse_events.name = "mouse events";
   _action1(&group_pd.items.part.mouse_events, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_MOUSE_EVENTS);

   group_pd.items.part.repeat_events.name = "repeat events";
   _action1(&group_pd.items.part.repeat_events, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_PART_REPEAT_EVENTS);

   group_pd.items.part.clip_to.name = "clip to";
   _action1(&group_pd.items.part.clip_to, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_CLIP_TO);

   group_pd.items.part.ignore_flags.name = "ignore flags";
   _action1(&group_pd.items.part.ignore_flags, NULL, NULL, PROPERTY_CONTROL_COMBOBOX, ATTRIBUTE_PART_IGNORE_FLAGS);

   _init_part_dragable_block();
}

static void
_init_state_block()
{
   group_pd.items.state.title.name = "state";
   group_pd.items.state.title.expandable = true;
   group_pd.items.state.title.expanded = true;
   group_pd.items.state.title.expand_cb = _subitems_get;

   group_pd.items.state.name.name = "name";
   _action1(&group_pd.items.state.name, NULL, NULL, PROPERTY_CONTROL_ENTRY, ATTRIBUTE_STATE_NAME);

   group_pd.items.state.visible.name = "visible";
   _action1(&group_pd.items.state.visible, NULL, NULL, PROPERTY_CONTROL_CHECK, ATTRIBUTE_STATE_VISIBLE);
}

/* public */
void
property_group_init()
{
   _init_group_block();
   _init_part_block();
   _init_part_group_specific_block();
   _init_state_block();

   group_pd.items.item.name = "item";
   group_pd.items.item.expandable = true;

   group_pd.items.program.name = "program";
   group_pd.items.program.expandable = true;

   /* register global callbacks */
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_CHANGED, _on_group_changed, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_PART_SELECTED, _on_part_selected, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_GROUP_NAVIGATOR_UNSELECTED, _on_group_navigator_unselected, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_PART_STATE_SELECTED, _on_part_state_selected, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_PROGRAM_SELECTED, _on_program_selected, NULL);
   evas_object_smart_callback_add(ap.win, SIGNAL_EDITOR_ATTRIBUTE_CHANGED, _on_editor_attribute_changed, NULL);
}

Eina_List *
property_group_items_get()
{
   Eina_List *items = NULL;

   assert(group_pd.group != NULL);

   items = eina_list_append(items, &group_pd.items.group.title);
   if (group_pd.part)
     {
        items = eina_list_append(items, &group_pd.items.part.title);
        items = eina_list_append(items, &group_pd.items.state.title);
        if (group_pd.part->current_item_name)
          items = eina_list_append(items, &group_pd.items.item);
     }
   else if (group_pd.program)
     items = eina_list_append(items, &group_pd.items.program);

   return items;
}
