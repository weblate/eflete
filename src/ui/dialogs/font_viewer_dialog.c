#include "efl_tet.h"
#include "font_viewer_dialog.h"

static Elm_Gengrid_Item_Class *_itc_font = NULL;

static void
_on_button_ok_clicked_cb(void *data, Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   Evas_Object *inwin = (Evas_Object *)data;
   evas_object_del(inwin);
}

static void
_on_button_cancel_clicked_cb(void *data, Evas_Object *obj __UNUSED__,
                void *event_info __UNUSED__)
{
   Evas_Object *inwin = (Evas_Object *)data;
   evas_object_del(inwin);
}

static char *
_item_font_label_get(void *data,
                      Evas_Object *obj __UNUSED__,
                      const char *part __UNUSED__)
{
   return strdup((char *)data);
}


static void
_on_font_selected(void *data, Evas_Object *obj __UNUSED__, void *event_info)
{
   Elm_Object_Item *glit = (Elm_Object_Item *)event_info;
   Evas_Object *font_view = (Evas_Object *) data;
   char buf[256];
    Evas_Object *entry = elm_object_part_content_get(
      elm_object_part_content_get(elm_win_inwin_content_get(font_view),
         "swallow/panes"), "right");

   strcpy(buf,"DEFAULT='font=");
   strcat(buf, (char*) elm_object_item_data_get(glit));
   strcat(buf," font_size=14 align=center warp=word'");

   elm_entry_text_style_user_push(entry, buf);
   evas_object_size_hint_max_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);

}

Evas_Object *
font_viewer_add(Evas_Object *parent)
{
   Evas_Object *inwin;
   Evas_Object *button, *genlist, *layout, *panes, *entry;

   inwin = elm_win_inwin_add(parent);

   layout = elm_layout_add(inwin);
   evas_object_size_hint_weight_set(layout, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_layout_file_set(layout, TET_EDJ, "base/colorclass_viewer/default");
   elm_win_inwin_content_set(inwin, layout);
   evas_object_show(layout);

   panes = elm_panes_add(inwin);
   evas_object_size_hint_weight_set(panes, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(panes, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_panes_content_left_size_set(panes, 0.3);
   elm_object_part_content_set(layout, "swallow/panes", panes);
   evas_object_show(panes);

   entry = elm_entry_add(inwin);
   elm_entry_single_line_set(entry, EINA_TRUE);
   elm_entry_scrollable_set(entry, EINA_TRUE);
   elm_entry_scrollbar_policy_set(entry, ELM_SCROLLER_POLICY_OFF,
                                  ELM_SCROLLER_POLICY_OFF);
   evas_object_size_hint_weight_set(entry, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_size_hint_align_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   elm_object_text_set(entry,
      "Verbatim reports were quickly given by Jim Fox to his amazed audience");
   elm_object_part_content_set(panes, "right", entry);
   elm_entry_editable_set(entry, EINA_FALSE);
   elm_entry_text_style_user_push(entry, "DEFAULT='align=center'");
   evas_object_size_hint_max_set(entry, EVAS_HINT_FILL, EVAS_HINT_FILL);
   evas_object_show(entry);

   genlist = elm_genlist_add(inwin);
   evas_object_size_hint_weight_set(genlist, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_object_part_content_set(panes, "left", genlist);
   evas_object_show(genlist);
   evas_object_smart_callback_add(genlist, "selected", _on_font_selected, inwin);

   if (!_itc_font)
     {
        _itc_font = elm_genlist_item_class_new();
        _itc_font->item_style = "default";
        _itc_font->func.text_get = _item_font_label_get;
        _itc_font->func.content_get = NULL;
        _itc_font->func.state_get = NULL;
        _itc_font->func.del = NULL;
     }

   button = elm_button_add(inwin);
   elm_object_text_set(button, "Add");
   elm_object_part_content_set(layout, "button_add", button);
   evas_object_resize(button, 40, 20);
   evas_object_show(button);

   button = elm_button_add(inwin);
   elm_object_text_set(button, "Delete");
   elm_object_part_content_set(layout, "button_del", button);
   evas_object_resize(button, 40, 20);
   evas_object_show(button);

   button = elm_button_add(inwin);
   elm_object_text_set(button, "Ok");
   elm_object_part_content_set(layout, "button_ok", button);
   evas_object_smart_callback_add(button, "clicked", _on_button_ok_clicked_cb,
                                   inwin);
   evas_object_resize(button, 40, 20);
   evas_object_show(button);

   button = elm_button_add(inwin);
   elm_object_text_set(button, "Cancel");
   elm_object_part_content_set(layout, "button_cancel", button);
   evas_object_smart_callback_add(button, "clicked", _on_button_cancel_clicked_cb,
                                   inwin);
   evas_object_resize(button, 40, 20);
   evas_object_show(button);
   return inwin;
}

void
font_viewer_init(Evas_Object *font_view, Project *project)
{
   Evas_Object *genlist = elm_object_part_content_get(
      elm_object_part_content_get(elm_win_inwin_content_get(font_view),
         "swallow/panes"), "left");
   Widget *_widget = NULL;
   Style *_style = NULL;
   Group *_group = NULL;
   Part *_part = NULL;
   Part_State *_state = NULL;

   if (!project)
     {
        NOTIFY_ERROR (elm_object_parent_widget_get(font_view),
                      "EDJ/EDC file is not loaded");
        evas_object_del(font_view);
        return;
     }
   evas_object_show(font_view);
   EINA_INLIST_FOREACH(project->widgets, _widget)
     {
        EINA_INLIST_FOREACH(_widget->styles, _style)
          {
             EINA_INLIST_FOREACH(_style->groups, _group)
               {
                  EINA_INLIST_FOREACH(_group->parts, _part)
                    {
                      if (_part->type == EDJE_PART_TYPE_TEXT)
                         {
                             EINA_INLIST_FOREACH(_part->states, _state)
                               {
                                if (_state->text->font)
                                  {
                                     elm_genlist_item_append(genlist,
                                          _itc_font, _state->text->font, NULL,
                                          ELM_GENLIST_ITEM_NONE, NULL, NULL);
                                   }
                              }
                         }
                    }
               }
          }
     }

}

