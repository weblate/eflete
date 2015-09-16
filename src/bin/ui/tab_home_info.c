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

#include "tabs_private.h"
#include "tabs.h"

struct _Tab_Home_Info
{
   Evas_Object *layout;
   Evas_Object *btn_save;

   Evas_Object *label;
   Evas_Object *meta_version;
   Evas_Object *meta_authors;
   Evas_Object *meta_licenses;
   Evas_Object *meta_comment;
};
typedef struct _Tab_Home_Info Tab_Home_Info;
Tab_Home_Info tab_info;

#define PROJECT_INFO \
   "<linegap=6>" \
   "Project name: %s<br>" \
   "Path to project: %s<br>" \
   "Quantity of images: %d<br>" \
   "Quantity of sounds: %d<br>" \
   "Quantity of fonts: %d"

static void
_info_set(Eina_Bool disabled, const char *name, const char *path,
          int img_count, int snd_count, int fnd_count,
          const char *version, const char *authors,
          const char *licenses, const char *comment)
{
   Eina_Strbuf *buf;

   buf = eina_strbuf_new();
   elm_object_disabled_set(tab_info.layout, disabled);
   eina_strbuf_append_printf(buf, _(PROJECT_INFO), name, path,
                             img_count, snd_count, fnd_count);
   elm_object_text_set(tab_info.label, eina_strbuf_string_get(buf));
   elm_entry_entry_set(tab_info.meta_version, version);
   elm_entry_entry_set(tab_info.meta_authors, authors);
   elm_entry_entry_set(tab_info.meta_licenses, licenses);
   elm_entry_entry_set(tab_info.meta_comment, comment);

   eina_strbuf_free(buf);
}

static void
_show(void *data __UNUSED__,
      Evas *e __UNUSED__,
      Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   Eina_Stringshare *authors, *version, *license, *comment;

   if (!ap.project)
     _info_set(true, "", "", 0, 0, 0, "", "", "", "");
   else
     {
        pm_project_meta_data_get(ap.project, NULL, &authors, &version, &license, &comment);
        _info_set(false, ap.project->name, ap.project->pro_path,
                  eina_list_count(ap.project->images),
                  eina_list_count(ap.project->sounds) + eina_list_count(ap.project->tones),
                  eina_list_count(ap.project->fonts),
                  authors, version, license, comment);
     }
}

static void
_validate(void *data __UNUSED__,
          Evas_Object *obj __UNUSED__,
          void *event_info __UNUSED__)
{
   elm_object_disabled_set(tab_info.btn_save, false);
}

static void
_save(void *data __UNUSED__,
      Evas_Object *obj __UNUSED__,
      void *event_info __UNUSED__)
{
   assert(ap.project != NULL);
   pm_project_meta_data_set(ap.project,
                            ap.project->name,
                            elm_entry_entry_get(tab_info.meta_version),
                            elm_entry_entry_get(tab_info.meta_authors),
                            elm_entry_entry_get(tab_info.meta_licenses),
                            elm_entry_entry_get(tab_info.meta_comment));
   elm_object_disabled_set(tab_info.btn_save, true);
}


Evas_Object *
_tab_project_info_add(void)
{
   tab_info.layout = elm_layout_add(ap.win);
   /* use layout for import edc because it siuts us */
   elm_layout_theme_set(tab_info.layout, "layout", "tab_home", "import_edc");

   BUTTON_ADD(tab_info.layout, tab_info.btn_save, _("Save"))
   elm_object_part_content_set(tab_info.layout, "elm.swallow.btn_create", tab_info.btn_save);
   evas_object_smart_callback_add(tab_info.btn_save, "clicked", _save, NULL);
   elm_object_disabled_set(tab_info.btn_save, true);

   LABEL_ADD(tab_info.layout, tab_info.label, NULL)
   elm_object_part_content_set(tab_info.layout, "swallow.project_info", tab_info.label);

   /* label.meta_version */
   elm_object_part_text_set(tab_info.layout, "label.meta_version", _("Version of file:"));
   ENTRY_ADD(tab_info.layout, tab_info.meta_version, true)
   evas_object_smart_callback_add(tab_info.meta_version, "changed,user", _validate, NULL);
   elm_object_part_content_set(tab_info.layout, "swallow.meta_version", tab_info.meta_version);
   /* label.meta_authors */
   elm_object_part_text_set(tab_info.layout, "label.meta_authors", _("Authors:"));
   ENTRY_ADD(tab_info.layout, tab_info.meta_authors, false)
   evas_object_smart_callback_add(tab_info.meta_authors, "changed,user", _validate, NULL);
   elm_object_part_content_set(tab_info.layout, "swallow.meta_authors", tab_info.meta_authors);
   /* label.meta_licenses */
   elm_object_part_text_set(tab_info.layout, "label.meta_licenses", _("Licenses:"));
   ENTRY_ADD(tab_info.layout, tab_info.meta_licenses, false)
   evas_object_smart_callback_add(tab_info.meta_licenses, "changed,user", _validate, NULL);
   elm_object_part_content_set(tab_info.layout, "swallow.meta_licenses", tab_info.meta_licenses);
   /* label.meta_comment */
   elm_object_part_text_set(tab_info.layout, "label.meta_comment", _("Comment:"));
   ENTRY_ADD(tab_info.layout, tab_info.meta_comment, false)
   evas_object_smart_callback_add(tab_info.meta_comment, "changed,user", _validate, NULL);
   elm_object_part_content_set(tab_info.layout, "swallow.meta_comment", tab_info.meta_comment);

   evas_object_event_callback_add(tab_info.layout, EVAS_CALLBACK_SHOW, _show, NULL);

   return tab_info.layout;
}
