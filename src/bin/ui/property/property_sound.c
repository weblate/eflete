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
#include "main_window.h"
#include "project_manager.h"

struct _Property_Sound_Data {
   Sound_Data *snd;
   External_Resource *tone, *sample;
   Property_Attribute items[PROPERTY_SOUND_ITEM_LAST];
};
typedef struct _Property_Sound_Data Property_Sound_Data;
static Property_Sound_Data sound_pd;

/* mapping enums to strings (THIS WILL BE USED LATER KEEP IT IN HERE) */
/*static const char *edje_sound_compression[] = { N_("RAW"),
                                                N_("COMP"),
                                                N_("LOSSY"),
                                                N_("AS_IS"),
                                                NULL};
*/

struct _Property_Sound_Update_Info {
   Property_Attribute *pa;
   Property_Action *action;
};
typedef struct _Property_Sound_Update_Info Property_Sound_Update_Info;

/* array to find item by Property_Item */
static Property_Sound_Update_Info attribute_map[PROPERTY_SOUND_ITEM_LAST] __UNUSED__;

static void
_update_cb(Property_Attribute *pa, Property_Action *action)
{
   assert(pa != NULL);
   assert(action != NULL);
   assert(action->control != NULL);

   switch (pa->type.sound_item)
     {
      case PROPERTY_SOUND_ITEM_FILE_NAME:
         if (!sound_pd.snd)
           property_entry_set(action->control, "-");
         else
           property_entry_set(action->control, sound_pd.snd->name);
         break;
      case PROPERTY_SOUND_ITEM_DURATION:
         break;
      case PROPERTY_SOUND_ITEM_TYPE:
         break;
      case PROPERTY_SOUND_ITEM_SIZE:
         break;
      case PROPERTY_SOUND_ITEM_COMPRESSION_TYPE:
         break;
      case PROPERTY_SOUND_ITEM_COMPRESSION_QUALITY:
         break;
      default:
         TODO("remove default case after all attributes will be added");
         CRIT("update callback not found for %s (%s)", pa->name, action->name ? action->name : "unnamed");
         abort();
         break;
     }
}

/* blocks */
static inline void
_action_internal(Property_Action *action, const char *name, const char *units,
                 Property_Control control_type)
{
   /* check that we haven't inited this action already */
   assert(action->update_cb == NULL);

   action->name = name;
   action->units = units;
   action->control_type = control_type;
   action->update_cb = _update_cb;
//   action->init_cb = _init_cb;
//   action->start_cb = _start_cb;
//   action->stop_cb = _stop_cb;
//   action->change_cb = _change_cb;
}

static inline void
_action1(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type)
{
   _action_internal(&pa->action1, name, units, control_type);

   assert(attribute_map[control_type].pa == NULL);
   assert(attribute_map[control_type].action == NULL);

   attribute_map[control_type].pa = pa;
   attribute_map[control_type].action = &pa->action1;
}

static inline void
_action2(Property_Attribute *pa, const char *name, const char *units,
         Property_Control control_type)
{
   _action_internal(&pa->action2, name, units, control_type);

   assert(attribute_map[control_type].pa == NULL);
   assert(attribute_map[control_type].action == NULL);

   attribute_map[control_type].pa = pa;
   attribute_map[control_type].action = &pa->action2;
}


static Eina_List *
_subitems_get(Property_Attribute *pa)
{
   Eina_List *items = NULL;

   assert(pa != NULL);
#define APPEND(TYPE) items = eina_list_append(items, &sound_pd.items[TYPE]);
   switch (pa->type.sound_item)
     {
      case PROPERTY_SOUND_ITEM_PREVIEW_TITLE:
         APPEND(PROPERTY_SOUND_ITEM_PLAYER);
         break;
      case PROPERTY_SOUND_ITEM_INFO_TITLE:
         APPEND(PROPERTY_SOUND_ITEM_FILE_NAME);
         APPEND(PROPERTY_SOUND_ITEM_DURATION);
         APPEND(PROPERTY_SOUND_ITEM_TYPE);
         APPEND(PROPERTY_SOUND_ITEM_SIZE);
         APPEND(PROPERTY_SOUND_ITEM_COMPRESSION_TYPE);
         APPEND(PROPERTY_SOUND_ITEM_COMPRESSION_QUALITY);
         break;
      default:
         CRIT("items callback not found for %s", pa->name);
         abort();
     }

   return items;
#undef APPEND
}

static void
_init_items()
{
   Property_Sound_Item it;
#define IT sound_pd.items[it]
   for (it = 0 /* first element of enum */; it < PROPERTY_SOUND_ITEM_LAST; it++)
     {
        IT.type.sound_item = it;
        switch(it)
          {
             /* group block */
           case PROPERTY_SOUND_ITEM_PREVIEW_TITLE:
              IT.name = "preview";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_SOUND_ITEM_PLAYER:
              break;

              /* part block */
           case PROPERTY_SOUND_ITEM_INFO_TITLE:
              IT.name = "info";
              IT.expandable = true;
              IT.expanded = true;
              IT.expand_cb = _subitems_get;
              break;
           case PROPERTY_SOUND_ITEM_FILE_NAME:
              IT.name = "name";
              _action1(&IT, NULL, NULL, PROPERTY_CONTROL_ENTRY);
              break;
           case PROPERTY_SOUND_ITEM_DURATION:
              break;
           case PROPERTY_SOUND_ITEM_TYPE:
              break;
           case PROPERTY_SOUND_ITEM_SIZE:
              break;
           case PROPERTY_SOUND_ITEM_COMPRESSION_TYPE:
              break;
           case PROPERTY_SOUND_ITEM_COMPRESSION_QUALITY:
              break;

           case PROPERTY_SOUND_ITEM_LAST:
              break;
          }
     }
#undef IT
}

static void
_on_grid_clicked(void *data,
                     Evas_Object *obj __UNUSED__,
                     void *event_info)
{
   Property_Data *pd = data;
   sound_pd.snd = (Sound_Data *)event_info;

   if (sound_pd.snd && sound_pd.snd->type == SOUND_TYPE_SAMPLE)
     {
        sound_pd.sample = (External_Resource *)sound_pd.snd->resource;
        sound_pd.tone = NULL;
     }
   else if (sound_pd.snd && sound_pd.snd->type == SOUND_TYPE_TONE)
     {
        sound_pd.sample = NULL;
        sound_pd.tone = (External_Resource *)sound_pd.snd->resource;
     }
   else
     {
        sound_pd.sample = NULL;
        sound_pd.tone = NULL;
     }

   assert(pd != NULL);
   property_item_update_recursively(&sound_pd.items[PROPERTY_SOUND_ITEM_INFO_TITLE]);
}

/* public */
void
property_sound_manager_init(Property_Data *pd)
{
   assert(pd != NULL);

   _init_items();

   /* register global callbacks */
   evas_object_smart_callback_add(ap.win, SIGNAL_SOUND_SELECT, _on_grid_clicked, pd);
   evas_object_smart_callback_add(ap.win, SIGNAL_SOUND_UNSELECTED, _on_grid_clicked, pd);
}

Eina_List *
property_sound_manager_items_get()
{
   Eina_List *items = NULL;

   items = eina_list_append(items, &sound_pd.items[PROPERTY_SOUND_ITEM_PREVIEW_TITLE]);
   items = eina_list_append(items, &sound_pd.items[PROPERTY_SOUND_ITEM_INFO_TITLE]);

   return items;
}
