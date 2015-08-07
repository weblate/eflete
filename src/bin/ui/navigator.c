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

#include "navigator.h"

typedef struct
{
   Evas_Object *genlist;
   Elm_Genlist_Item_Class *itc_group;
   Elm_Genlist_Item_Class *itc_folder;
} Navigator;

static Navigator navigator;

static char *
_group_item_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   const char *pos;
   Group *group = data;

   pos = strrchr(group->name, '/');
   if (pos) return strdup(pos + 1);
   else return strdup(group->name);
}

static char *
_folder_item_label_get(void *data,
                       Evas_Object *obj __UNUSED__,
                       const char *part __UNUSED__)
{
   const char *pos;
   char buf[BUFF_MAX];
   int symbols;
   Eina_Stringshare *prefix = data;

   symbols = strlen(prefix) - 1;
   strncpy(buf, prefix, symbols);
   buf[symbols] = '\0';
   pos = strrchr(buf, '/');
   if (pos) return strdup(pos + 1);
   else return strdup(buf);
}

static Evas_Object *
_folder_item_icon_get(void *data __UNUSED__,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   Evas_Object *icon = NULL;

   return icon;
}

static Evas_Object *
_group_item_icon_get(void *data __UNUSED__,
                     Evas_Object *obj __UNUSED__,
                     const char *part __UNUSED__)
{
   Evas_Object *icon = NULL;

   return icon;
}

static void
_folder_item_del(void *data,
                 Evas_Object *obj __UNUSED__)
{
   Eina_Stringshare *prefix = data;
   eina_stringshare_del(prefix);
}

static Eina_Stringshare *
_get_prefix(const char *group_name, int level, int *symbols)
{
   const char *pos;
   char prefix[BUFF_MAX];
   int i;

   assert(group_name != NULL);
   assert(level >= 0);

   pos = group_name - 1;
   for (i = 0; (pos) && (i <= level); i++)
     {
        pos = strchr(pos + 1, '/');
     }
   if (!pos) return NULL;
   *symbols = pos - group_name + 1;
   strncpy(prefix, group_name, *symbols);
   prefix[*symbols] = '\0';

   return eina_stringshare_add(prefix);
}

static void
_tree_items_get(const char *prefix,
                Eina_List **folders_out,
                Eina_List **groups_out)
{
   int cmp;
   int level = 0;
   const char *pos;
   App_Data *ap = app_data_get();
   Group *group, *group_next;
   size_t prefix_len;
   Eina_Stringshare *group_prefix;
   int group_prefix_len;
   Eina_List *l, *lnext;

   assert(prefix != NULL);
   assert(folders_out != NULL);
   assert(groups_out != NULL);

   prefix_len = strlen(prefix);

   pos = prefix - 1;
   while ((pos = strchr(pos + 1, '/')))
     level++;

   EINA_LIST_FOREACH(ap->project->groups, l, group)
     {
        cmp = strncmp(group->name, prefix, prefix_len);
        /* skipping all groups with different prefix */
        if (cmp < 0)
          {
             continue;
          }
        if (cmp > 0)
          {
             if (strlen(group->name) < prefix_len)
               continue;
             else
               break; /* there is no sense to check all next groups because list is sorted */
          }

        group_prefix = _get_prefix(group->name, level, &group_prefix_len);
        if (group_prefix)
          {
             *folders_out= eina_list_append(*folders_out, group_prefix);

             /* skipping other groups from the same "folder" */
             lnext = l;
             while ((lnext = eina_list_next(lnext)))
               {
                  group_next = eina_list_data_get(lnext);
                  if ((group_next) && (!strncmp(group_next->name, group_prefix, group_prefix_len)))
                    l = lnext;
                  else
                    break;
               }
          }
        else
          {
             *groups_out = eina_list_append(*groups_out, group);
          }
     }
}

static void
_expand_request_cb(void *data __UNUSED__,
                   Evas_Object *o __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_TRUE);
}

static void
_contract_request_cb(void *data __UNUSED__,
                     Evas_Object *o __UNUSED__,
                     void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_expanded_set(glit, EINA_FALSE);
}

static void
_expanded_cb(void *data __UNUSED__,
             Evas_Object *o __UNUSED__,
             void *event_info)
{
   Group *group;
   Eina_List *folders = NULL, *groups = NULL;
   Elm_Object_Item *glit = event_info;
   Eina_Stringshare *prefix = elm_object_item_data_get(glit);

   _tree_items_get(prefix, &folders, &groups);
   EINA_LIST_FREE(folders, prefix)
     {
        elm_genlist_item_append(navigator.genlist,
                                navigator.itc_folder,
                                prefix,
                                glit,
                                ELM_GENLIST_ITEM_TREE,
                                NULL,
                                NULL);
     }
   EINA_LIST_FREE(groups, group)
     {
        elm_genlist_item_append(navigator.genlist,
                                navigator.itc_group,
                                group,
                                glit,
                                ELM_GENLIST_ITEM_NONE,
                                NULL,
                                NULL);
     }
}

static void
_contracted_cb(void *data __UNUSED__,
               Evas_Object *o __UNUSED__,
               void *event_info)
{
   Elm_Object_Item *glit = event_info;
   elm_genlist_item_subitems_clear(glit);
}

static void
_on_clicked_double(void *data __UNUSED__,
                   Evas_Object *obj __UNUSED__,
                   void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;

   if (elm_genlist_item_type_get(glit) == ELM_GENLIST_ITEM_TREE)
     {
        elm_genlist_item_expanded_set(glit, !elm_genlist_item_expanded_get(glit));
     }
   else
     {
        TODO("Add group selection signal here");
     }
}

Evas_Object *
navigator_add(void)
{
   Eina_List *folders = NULL, *groups = NULL;
   Eina_Stringshare *prefix;
   Group *group;
   App_Data *ap = app_data_get();

   assert(ap != NULL);
   assert(ap->win != NULL);

   navigator.itc_folder = elm_genlist_item_class_new();
   navigator.itc_folder->item_style = "default";
   navigator.itc_folder->func.text_get = _folder_item_label_get;
   navigator.itc_folder->func.content_get = _folder_item_icon_get;
   navigator.itc_folder->func.state_get = NULL;
   navigator.itc_folder->func.del = _folder_item_del;

   navigator.itc_group = elm_genlist_item_class_new();
   navigator.itc_group->item_style = "default";
   navigator.itc_group->func.text_get = _group_item_label_get;
   navigator.itc_group->func.content_get = _group_item_icon_get;
   navigator.itc_group->func.state_get = NULL;
   navigator.itc_group->func.del = NULL;

   navigator.genlist = elm_genlist_add(ap->win);

   evas_object_size_hint_align_set(navigator.genlist, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_size_hint_weight_set(navigator.genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);

   _tree_items_get("", &folders, &groups);

   EINA_LIST_FREE(folders, prefix)
     {
        elm_genlist_item_append(navigator.genlist,
                                navigator.itc_folder,
                                prefix,
                                NULL,
                                ELM_GENLIST_ITEM_TREE,
                                NULL,
                                NULL);
     }
   EINA_LIST_FREE(groups, group)
     {
        elm_genlist_item_append(navigator.genlist,
                                navigator.itc_group,
                                group,
                                NULL,
                                ELM_GENLIST_ITEM_NONE,
                                NULL,
                                NULL);
     }

   evas_object_smart_callback_add(navigator.genlist, "clicked,double", _on_clicked_double, NULL);
   evas_object_smart_callback_add(navigator.genlist, "expand,request", _expand_request_cb, NULL);
   evas_object_smart_callback_add(navigator.genlist, "contract,request", _contract_request_cb, NULL);
   evas_object_smart_callback_add(navigator.genlist, "expanded", _expanded_cb, NULL);
   evas_object_smart_callback_add(navigator.genlist, "contracted", _contracted_cb, NULL);

   TODO("Add deletion callback and free resources");

   return navigator.genlist;
}
