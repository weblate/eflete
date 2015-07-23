/*
 * Edje Theme Editor
 * Copyright (C) 2013-2014 Samsung Electronics.
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

#define EO_BETA_API
#define EFL_BETA_API_SUPPORT
#define EFL_EO_API_SUPPORT

#include "state_dialog.h"
#include "common_macro.h"
#include "string_common.h"
#include "main_window.h"

static Evas_Object *entry_name;
static Evas_Object *entry_value;
static Evas_Object *btn_add;
static Elm_Validator_Regexp *name_validator = NULL;
static Elm_Validator_Regexp *value_validator = NULL;

static const char *state_from;
static double value_from;
static Eina_Bool state_copy;

static void
_job_popup_close(void *data)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   elm_validator_regexp_free(name_validator);
   elm_validator_regexp_free(value_validator);
   evas_object_del(ap->popup);
   ap->popup = NULL;
   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
}

static void
_add_ok_clicked(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;

   assert(ap != NULL);

   Evas_Object *workspace = ap->workspace;
   Part *part = ui_states_list_part_get(ap->block.state_list);
   const char *str_name, *str_value;
   Eina_Stringshare *state;
   Eina_Bool result = false;

   if (elm_entry_is_empty(entry_name))
     {
        NOTIFY_WARNING(_("State name can not be empty!"))
        return;
     }

   str_name = elm_entry_entry_get(entry_name);
   str_value = elm_entry_entry_get(entry_value);
   if (edje_edit_state_exist(ap->project->current_style->obj, part->name,
                             str_name, atof(str_value)))
     {
        NOTIFY_WARNING(_("State is exist!"))
        return;
     }

   if (!state_copy)
     result = workspace_edit_object_part_state_add(workspace, part->name,
                                                   str_name, atof(str_value));
   else
     result =  workspace_edit_object_part_state_copy(workspace, part->name,
                                                     state_from, value_from,
                                                     str_name, atof(str_value));

   if (result)
     {
        state = eina_stringshare_printf("%s %.2f", str_name, atof(str_value));
        ui_states_list_state_add(ap->block.state_list, state);
        eina_stringshare_del(state);
        project_changed(true);
     }
   ecore_job_add(_job_popup_close, ap);
}

static void
_cancel_clicked(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   ecore_job_add(_job_popup_close, data);
}

static void
_on_dup_state_change(void *data,
                     Evas_Object *obj EINA_UNUSED,
                     void *ei)
{
   char **split = NULL;
   Part *part = (Part *) data;

   assert(part != NULL);

   Ewe_Combobox_Item *it = ei;
   const char *state = it->title;
   if (!it->index)
     {
        state_copy = false;
        return;
     }

   split = eina_str_split(state, " ", 2);
   eina_stringshare_del(part->curr_state);
   state_from = eina_stringshare_add(split[0]);
   value_from = atof(split[1]);
   free(split[0]);
   free(split);
   state_copy = true;
}

static void
_on_entry_changed(void *data __UNUSED__,
                  Evas_Object *obj __UNUSED__,
                  void *event_info __UNUSED__)
{
   if ((elm_validator_regexp_status_get(name_validator) != ELM_REG_NOERROR) ||
       (elm_validator_regexp_status_get(value_validator) != ELM_REG_NOERROR))
     elm_object_disabled_set(btn_add, true);
   else
     elm_object_disabled_set(btn_add, false);
}

Evas_Object *
state_dialog_state_add(App_Data *ap)
{
   Evas_Object *box, *item, *bt_no;
   Part *part = NULL;
   Eina_Stringshare *title = NULL;
   Eina_List *states = NULL, *l = NULL;
   const char *state_name = NULL;
   Evas_Object *combobox_dup_state;

   assert(ap != NULL);
   assert(ap->workspace != NULL);

   part = ui_states_list_part_get(ap->block.state_list);
   name_validator = elm_validator_regexp_new(NAME_REGEX, NULL);
   value_validator = elm_validator_regexp_new(STATE_VALUE_REGEX, NULL);

   assert(part != NULL);

   state_copy = false;

   ap->popup = elm_popup_add(ap->win_layout);
   title = eina_stringshare_printf(_("Add new state to part \"%s\""), part->name);
   elm_object_part_text_set(ap->popup, "title,text", title);
   elm_popup_orient_set(ap->popup, ELM_POPUP_ORIENT_CENTER);

   BOX_ADD(ap->popup, box, false, false);

   LAYOUT_PROP_ADD(box, _("Name:"), "property", "1swallow")
   ENTRY_ADD(item, entry_name, true);
   eo_do(entry_name, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, name_validator));
   evas_object_smart_callback_add(entry_name, "changed", _on_entry_changed, NULL);
   elm_object_part_text_set(entry_name, "guide", _("Type a new state name"));
   elm_object_part_content_set(item, "elm.swallow.content", entry_name);
   elm_box_pack_end(box, item);

   LAYOUT_PROP_ADD(box, _("Value:"), "property", "1swallow")
   ENTRY_ADD(item, entry_value, true);
   eo_do(entry_value, eo_event_callback_add(ELM_ENTRY_EVENT_VALIDATE, elm_validator_regexp_helper, value_validator));
   evas_object_smart_callback_add(entry_value, "changed", _on_entry_changed, NULL);
   /* need to manualy set not valid string for triggered validator */
   elm_entry_entry_set(entry_value, NULL);
   elm_object_part_text_set(entry_value, "guide", _("Type a state value (0.0 - 1.0)"));
   elm_object_part_content_set(item, "elm.swallow.content", entry_value);
   elm_box_pack_end(box, item);

   LAYOUT_PROP_ADD(box, _("Duplicate state:"), "property", "1swallow")
   EWE_COMBOBOX_ADD(item, combobox_dup_state)

   states = edje_edit_part_states_list_get(ap->project->current_style->obj, part->name);

   ewe_combobox_item_add(combobox_dup_state, _("None"));
   ewe_combobox_select_item_set(combobox_dup_state, 0);
   EINA_LIST_FOREACH(states, l, state_name)
     {
       ewe_combobox_item_add(combobox_dup_state, state_name);
     }
   elm_object_part_content_set(item, "elm.swallow.content", combobox_dup_state);
   evas_object_smart_callback_add(combobox_dup_state, "selected",
                                  _on_dup_state_change, part);
   edje_edit_string_list_free(states);

   elm_box_pack_end(box, item);
   elm_object_content_set(ap->popup, box);

   BUTTON_ADD(ap->popup, btn_add, _("Add"));
   evas_object_smart_callback_add (btn_add, "clicked", _add_ok_clicked, ap);
   elm_object_part_content_set(ap->popup, "button1", btn_add);
   elm_object_disabled_set(btn_add, true);

   BUTTON_ADD(ap->popup, bt_no, _("Cancel"));
   evas_object_smart_callback_add (bt_no, "clicked", _cancel_clicked, ap);
   elm_object_part_content_set(ap->popup, "button2", bt_no);

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, true);
   evas_object_show(ap->popup);
   elm_object_focus_set(entry_name, true);

   eina_stringshare_del(title);
   return ap->popup;
}

static void
_del_ok_clicked(void *data,
                Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   App_Data *ap = (App_Data *)data;
   Evas_Object *workspace;
   Part *part;
   Eina_Stringshare *state;

   assert(ap != NULL);

   workspace = ap->workspace;
   part = ui_states_list_part_get(ap->block.state_list);
   state = ui_states_list_selected_state_get(ap->block.state_list);

   char **arr = eina_str_split(state, " ", 2);

   TODO("need to check the program, some program can use the given state."
        "If given state used in programs - show dialog window with the question:"
        "'This state used in the program(s). Are you sure you want to delete"
        "%state name%' and delete the programs or some params from the program")
   if (workspace_edit_object_part_state_del(workspace, part->name, arr[0], atof(arr[1])))
     {
        ui_states_list_selected_state_del(ap->block.state_list);
        project_changed(true);
     }

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, false);
   free(arr[0]);
   free(arr);
   ecore_job_add(_job_popup_close, data);
}

Evas_Object *
state_dialog_state_del(App_Data *ap)
{
   Evas_Object *label, *bt_yes, *bt_no;
   Part *part;
   Eina_Stringshare *state, *title, *message;

   assert(ap != NULL);
   assert(ap->workspace != NULL);

   part = ui_states_list_part_get(ap->block.state_list);

   assert(part != NULL);

   state = ui_states_list_selected_state_get(ap->block.state_list);

   assert(state != NULL);

   if (!strcmp(state, "default 0.00"))
     {
        NOTIFY_WARNING(_("Can't delete the default state."));
        return NULL;
     }

   ap->popup = elm_popup_add(ap->win_layout);
   title = eina_stringshare_printf(_("Delete the state from part"));
   elm_object_part_text_set(ap->popup, "title,text", title);
   elm_popup_orient_set(ap->popup, ELM_POPUP_ORIENT_CENTER);

   message = eina_stringshare_printf(_("Do you want to delete the state \"%s\" "
                                       "from part \"%s\"."), state, part->name);
   LABEL_ADD(ap->popup, label, message)
   evas_object_size_hint_weight_set(label, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(label, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_label_line_wrap_set(label, ELM_WRAP_CHAR);
   elm_object_content_set(ap->popup, label);

   BUTTON_ADD(ap->popup, bt_yes, _("Delete"));
   evas_object_smart_callback_add (bt_yes, "clicked", _del_ok_clicked, ap);
   elm_object_part_content_set(ap->popup, "button1", bt_yes);

   BUTTON_ADD(ap->popup, bt_no, _("Cancel"));
   evas_object_smart_callback_add (bt_no, "clicked", _cancel_clicked, ap);
   elm_object_part_content_set(ap->popup, "button2", bt_no);

   ui_menu_items_list_disable_set(ap->menu, MENU_ITEMS_LIST_MAIN, true);

   evas_object_show(ap->popup);
   eina_stringshare_del(state);
   eina_stringshare_del(message);

   return ap->popup;
}
