/**
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


#include "test_history.h"
#include "main_window.h"
#include "test_common.h"

/**
 * @addtogroup history_test
 * @{
 * @addtogroup history_undo
 * @{
 * history
 * <TABLE>
 * @}
 */

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p1</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  min height param of "bg" part.
 * @step 10 Set new value [10] for min height param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of min height param of part "bg" with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p1)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p1");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_h_set(style->obj, "bg", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_min_h_set,
                    "Min h", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_INT value type.");
   check_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p1");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p2</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag x param of "bg" part.
 * @step 10 Set new value [1] for drag x param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of drag x param of part "bg" with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p2)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value = 99;
   int new_value = 1;
   int check_value = 55;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p2");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_part_drag_x_get(style->obj, "bg");
   edje_edit_part_drag_x_set(style->obj, "bg", new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_x_set,
                    "Drag x", "bg", NULL, 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_INT value type, setted without state");
   check_value = edje_edit_part_drag_x_get(style->obj, "bg");
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p2");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p3</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag y param of "bg" part.
 * @step 10 Set new value [1] for drag y param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Save current value of  min weight param of "bg" part.
 * @step 13 Set new value [10] for min weight param of "bg" part.
 * @step 14 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of min weight param of part "bg" with the
 *         value, that was saved at step 12 of precondition.
 * @step 4 Compare current value of drag y param of part "bg" with the
 *         value, that was setted at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p3)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value_drag_y = 11;
   int new_value_drag_y = 1;
   int check_value_drag_y = 55;
   int old_value_min_w = -1;
   int new_value_min_w = 10;
   int check_value_min_w = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p3");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_drag_y = edje_edit_part_drag_y_get(style->obj, "bg");
   edje_edit_part_drag_y_set(style->obj, "bg", new_value_drag_y);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_drag_y, new_value_drag_y,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_y_set,
                    "Drag y", "bg", NULL, 0.0);
   old_value_min_w = edje_edit_state_min_w_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_w_set(style->obj, "bg", "default", 0.0, new_value_min_w);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_min_w, new_value_min_w,
                    "elm/radio/base/def", (void *)edje_edit_state_min_w_set,
                    "Min h", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_INT value type, setted without state");
   check_value_min_w = edje_edit_state_min_w_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value_min_w == old_value_min_w, "Cancel last action doesn't change value");
   check_value_drag_y = edje_edit_part_drag_y_get(style->obj, "bg");
   ck_assert_msg(check_value_drag_y == new_value_drag_y, "Canceled all actions");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p3");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p4</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag x param of "bg" part.
 * @step 10 Set new value [1] for drag x param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 * @step 12 Save current value of  min height param of "bg" part.
 * @step 13 Set new value [10] for min height param of "bg" part.
 * @step 14 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of min height param of part "bg" with the
 *         value, that was saved at step 12 of precondition.
 * @step 4 Compare current value of drag x param of part "bg" with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 2 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p4)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value_drag_x = 99;
   int new_value_drag_x = 1;
   int check_value_drag_x = 55;
   int old_value_min_h = -1;
   int new_value_min_h = 10;
   int check_value_min_h = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p4");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_drag_x = edje_edit_part_drag_x_get(style->obj, "bg");
   edje_edit_part_drag_x_set(style->obj, "bg", new_value_drag_x);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_drag_x, new_value_drag_x,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_x_set,
                    "Drag x", "bg", NULL, 0.0);
   old_value_min_h = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_h_set(style->obj, "bg", "default", 0.0, new_value_min_h);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value_min_h, new_value_min_h,
                    "elm/radio/base/def", (void *)edje_edit_state_min_h_set,
                    "Min h", "bg", "default", 0.0);

   result = history_undo(style->obj, 2);
   ck_assert_msg(result, "Failed to undo diff with VAL_INT value type, setted without state");
   check_value_min_h = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value_min_h == old_value_min_h, "Cancel last action doesn't change value");
   check_value_drag_x = edje_edit_part_drag_x_get(style->obj, "bg");
   ck_assert_msg(check_value_drag_x == old_value_drag_x, "Canceled not all actions");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p4");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p5</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 7 Save current value of  aspect max param of "bg" part.
 * @step 8 Set new value [0.4] for aspect param of "bg" part.
 * @step 9 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of aspect max param of part "bg" with the
 *         value, that was saved at step 7 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p5)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   double old_value = -1;
   double new_value = 0.4;
   double check_value = -5;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p5");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_state_aspect_max_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_aspect_max_set(style->obj, "bg", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_DOUBLE, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_aspect_max_set,
                    "Min h", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_DOUBLE value type.");
   check_value = edje_edit_state_aspect_max_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p5");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p6</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of clip to param of "bg" part.
 * @step 10 Set new value [events] for clip to param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of clip to param of part "bg" with the
 *         value, that was saved at step 8 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p6)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_Stringshare *old_value = NULL;
   Eina_Stringshare *new_value =  NULL;
   Eina_Stringshare *check_value = NULL;
   const char *tmp;

   elm_init(0, 0);
   app_init();

   new_value = eina_stringshare_add("events");
   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p6");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   tmp = edje_edit_part_clip_to_get(style->obj, "bg");
   old_value = eina_stringshare_add(tmp);
   edje_edit_part_clip_to_set(style->obj, "bg", new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_STRING, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_clip_to_set,
                    "clip to", "bg", NULL, 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_STRING value type.");
   tmp = edje_edit_part_clip_to_get(style->obj, "bg");
   check_value = eina_stringshare_add(tmp);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   eina_stringshare_del(new_value);
   eina_stringshare_del(old_value);
   eina_stringshare_del(check_value);
   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p6");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p7</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of rel1 to x param of "radio" part.
 * @step 10 Set new value [events] for rel1 to x param of "radio" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of rel1 to x param of part "radio" with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p7)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_Stringshare *old_value = NULL;
   Eina_Stringshare *new_value = NULL;
   Eina_Stringshare *check_value = NULL;
   const char *tmp;

   elm_init(0, 0);
   app_init();

   new_value = eina_stringshare_add("events");
   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p7");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   tmp = edje_edit_state_rel1_to_x_get(style->obj, "radio", "default", 0.0);
   old_value = eina_stringshare_add(tmp);
   edje_edit_state_rel1_to_x_set(style->obj, "radio", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_STRING, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_rel1_to_x_set,
                    "clip to", "radio", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_STRING value type.");
   tmp = edje_edit_state_rel1_to_x_get(style->obj, "radio", "default", 0.0);
   check_value = eina_stringshare_add(tmp);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   eina_stringshare_del(new_value);
   eina_stringshare_del(old_value);
   eina_stringshare_del(check_value);
   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p7");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p8</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of color param of "bg" part.
 * @step 10 Set new value [80;90;100;120] for color param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of color param of part "bg" with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p8)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int oldr, oldg, oldb, olda;
   int newr = 80, newg = 90, newb = 100, newa = 120;
   int checkr, checkg, checkb, checka;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p8");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_color_get(style->obj, "radio", "default", 0.0, &oldr, &oldg,
                             &oldb, &olda);
   edje_edit_state_color_set(style->obj, "radio", "default", 0.0, newr, newg,
                             newb, newa);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_FOUR, oldr, oldg, oldb, olda,
                    newr, newg, newb, newa, "elm/radio/base/def",
                    (void *)edje_edit_state_color_set,
                    "clip to", "radio", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_FOUR value type.");
   edje_edit_state_color_get(style->obj, "radio", "default", 0.0, &checkr, &checkg,
                             &checkb, &checka);
   ck_assert_msg(((checkr == oldr) && (checkg == oldg) && (checkb == oldb) &&
                  (checka == olda )), "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p8");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p9</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  max height param of group.
 * @step 10 Set new value [10] for max height param of group.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of max height param of group with the
 *         value, that was saved at step 9 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p9)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p9");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   old_value = edje_edit_group_max_h_get(style->obj);
   edje_edit_group_max_h_set(style->obj, new_value);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_GROUP, old_value, new_value,
                    old_value, new_value, (void *)edje_edit_group_min_h_set,
                    "elm/radio/base/def", (void *)edje_edit_group_max_h_set,
                    "Group height", NULL, NULL, 0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_GROUP value type.");
   check_value = edje_edit_group_max_h_get(style->obj);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");
   check_value = edje_edit_group_min_h_get(style->obj);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p9");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p10</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Set new name [new] for part "bg".
 * @step 10 Set new name [new] for Part in style.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check is exist part "bg" in group.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p10)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Part *part = NULL;
   Eina_Bool result = EINA_FALSE;
   char *old_value = "bg";
   char *new_value = "new";

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p10");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   part = wm_part_by_name_find(style, eina_stringshare_add(old_value));
   edje_edit_part_name_set(style->obj, old_value, new_value);
   part->name = eina_stringshare_add(new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_RENAME, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_name_set,
                    "Rename", new_value, NULL, 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_RENAME value type.");
   result = edje_edit_part_exist(style->obj, old_value);
   ck_assert_msg(result, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p10");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p11</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag x param of "bg" part.
 * @step 10 Set new value [1] for drag x param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for style object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of max height param of group with the
 *         value, that was saved at step 7 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p11)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p11");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_part_drag_x_get(style->obj, "bg");
   edje_edit_part_drag_x_set(style->obj, "bg", new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_part_drag_x_set,
                    "Drag x", "bg", NULL, 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_INT value type.");
   check_value = edje_edit_part_drag_x_get(style->obj, "bg");
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./history_undo_test_p11");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p12</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  drag x param of "bg" part.
 * @step 10 Set new value [1] for drag x param of "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for style object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of max height param of group with the
 *         value, that was saved at step 7 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p12)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p12");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_min_h_set(style->obj, "bg", "default", 0.0, new_value);
   history_diff_add(style->obj, PROPERTY, MODIFY, VAL_INT, old_value, new_value,
                    "elm/radio/base/def", (void *)edje_edit_state_min_h_set,
                    "Min h", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_INT value type.");
   check_value = edje_edit_state_min_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p12");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p13</td>
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value of  max height param of group.
 * @step 10 Set new value [10] for max height param of group.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for style object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of max height param of group with the
 *         value, that was saved at step 7 of precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p13)
{
   App_Data *app = NULL;
   Eina_Bool result = EINA_FALSE;
   Style *style = NULL;
   int old_value = -1;
   int new_value = 10;
   int check_value = -15;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p13");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value = edje_edit_group_max_h_get(style->obj);
   edje_edit_group_max_h_set(style->obj, new_value);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_GROUP, old_value, new_value,
                    old_value, new_value, (void *)edje_edit_group_min_h_set,
                    "elm/radio/base/def", (void *)edje_edit_group_max_h_set,
                    "Group height", NULL, NULL, 0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with VAL_GROUP value type.");
   check_value = edje_edit_group_max_h_get(style->obj);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");
   check_value = edje_edit_group_min_h_get(style->obj);
   ck_assert_msg(check_value == old_value, "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;

   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p13");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p14</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Add new  tween image ["radio.png"] to the "bg" part.
 * @step 10 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check count of entries in list of tween images. [should be 0])
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p14)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   const char *name = "radio.png";
   Eina_List *tween_list = NULL;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p14");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_tween_add(style->obj, "bg", "default", 0.0, name);
   history_diff_add(style->obj, PROPERTY, ADD, VAL_STRING, name,
                    (void *)edje_edit_state_tween_del, "elm/radio/base/def",
                    (void *)edje_edit_state_tween_add,
                    "tween add", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with ADD action and VAL_STRING value type.");
   tween_list = edje_edit_state_tweens_list_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(eina_list_count(tween_list) == 0,
                 "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p14");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p15</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Add new  tween image ["radio.png"] to the "bg" part.
 * @step 10 dele tween image, that was added at step 9 from "bg" part.
 * @step 11 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check count of entries in list of tween images. [should be 1])
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p15)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   const char *name = "radio.png";
   Eina_List *tween_list = NULL;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p15");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_tween_add(style->obj, "bg", "default", 0.0, name);
   edje_edit_state_tween_del(style->obj, "bg", "default", 0.0, name);
   history_diff_add(style->obj, PROPERTY, DEL, VAL_STRING, name,
                    (void *)edje_edit_state_tween_add, "elm/radio/base/def",
                    (void *)edje_edit_state_tween_del,
                    "tween add", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with DEL action and VAL_STRING value type.");
   tween_list = edje_edit_state_tweens_list_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(eina_list_count(tween_list) == 1,
                 "Canceled action doesn't change value");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p15");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p16</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value for the max height param of "bg" part
 * @step 10 Save current value for the max weight param of "bg" part
 * @step 11 Set new value [10] for max height param of "bg" part.
 * @step 12 Set new value [99] for max weight param of "bg" part.
 * @step 13 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of the max height param with stored value on
 *         step 9 of precondition.
 * @step 4 Compare current value of the max weight param with stored value on
 *         step 10 of the precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p16)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   int old_value_1 = -10, old_value_2 = -10;
   int new_value_1 = 10, new_value_2 = 99;
   int check_value = -999;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p16");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_1 = edje_edit_state_max_h_get(style->obj, "bg", "default", 0.0);
   old_value_2 = edje_edit_state_max_w_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_max_h_set(style->obj, "bg", "default", 0.0, new_value_1);
   edje_edit_state_max_w_set(style->obj, "bg", "default", 0.0, new_value_2);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_INT, old_value_1, new_value_1,
                    old_value_2, new_value_2, (void *)edje_edit_state_max_h_set,
                    "elm/radio/base/def", (void *)edje_edit_state_max_w_set,
                    "max size", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with CONTAINER action and VAL_INT value type.");
   check_value = edje_edit_state_max_h_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value_1, "Max height didn't canceled");
   check_value = edje_edit_state_max_w_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value_2, "Max weight didn't canceled");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p16");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p17</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Save current value for the align x param of "bg" part
 * @step 10 Save current value for the align y param of "bg" part
 * @step 11 Set new value [0.7] for align x param of "bg" part.
 * @step 12 Set new value [0.3] for align y param of "bg" part.
 * @step 13 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Compare current value of the align x param with stored value on
 *         step 9 of precondition.
 * @step 4 Compare current value of the align y param with stored value on
 *         step 10 of the precondition.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p17)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   double old_value_1 = -1.0, old_value_2 = -1.0;
   double new_value_1 = 0.7, new_value_2 = 0.3;
   double check_value = -1.0;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p17");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   old_value_1 = edje_edit_state_align_x_get(style->obj, "bg", "default", 0.0);
   old_value_2 = edje_edit_state_align_y_get(style->obj, "bg", "default", 0.0);
   edje_edit_state_align_x_set(style->obj, "bg", "default", 0.0, new_value_1);
   edje_edit_state_align_y_set(style->obj, "bg", "default", 0.0, new_value_2);
   history_diff_add(style->obj, PROPERTY, CONTAINER, VAL_DOUBLE, old_value_1, new_value_1,
                    old_value_2, new_value_2, (void *)edje_edit_state_align_x_set,
                    "elm/radio/base/def", (void *)edje_edit_state_align_y_set,
                    "align", "bg", "default", 0.0);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with CONTAINER action and VAL_DOUBLE value type.");
   check_value = edje_edit_state_align_x_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value_1, "Align x didn't canceled");
   check_value = edje_edit_state_align_y_get(style->obj, "bg", "default", 0.0);
   ck_assert_msg(check_value == old_value_2, "Align y didn't canceled");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p17");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p18</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Add new state with name "new_state" and value 0.1 to the part "bg".
 * @step 10 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that state "new_state" with value 0.1 for part "bg" does not exists.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p18)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p18");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   edje_edit_state_add(style->obj, "bg", "new_state", 0.1);
   history_diff_add(style->obj, STATE_TARGET, ADD, "elm/radio/base/def",
                    "bg", "new_state", 0.1, "create state");

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with target type STATE_TARGET"
                 " with ADD action.");
   result = edje_edit_state_exist(style->obj, "bg", "new_state", 0.1);
   ck_assert_msg(!result, "Adding new state didn't canceled'");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p18");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p19</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Delete state with name "test_state" and value 0.2 from the part "elm.text".
 * @step 10 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that state "test_state" with value 0.2 for part "elm.text" is exists.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p19)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p19");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   history_diff_add(style->obj, STATE_TARGET, DEL, "elm/radio/base/def",
                    "elm.text", "test_state", 0.2, "delete state");
   edje_edit_state_del(style->obj, "elm.text", "test_state", 0.2);

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with target type STATE_TARGET"
                 " with DEL action.");
   result = edje_edit_state_exist(style->obj, "elm.text", "test_state", 0.2);
   ck_assert_msg(result, "Deleting state didn't canceled'");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p19");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p20</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Store list of states from part "elm.text".
 * @step 10 Store diff with using history_diff_add function.
 * @step 11 Delete part with name "elm.text".
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that part "elm.text" exist.
 * @step 4 Check, that all states of part "elm.text" restored.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p20)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_List *states = NULL, *l = NULL, *check_states = NULL;
   Eina_Stringshare *state = NULL;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p20");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   states = edje_edit_part_states_list_get(style->obj, "elm.text");
   history_diff_add(style->obj, PART_TARGET, DEL, "elm.text");
   edje_edit_part_del(style->obj, "elm.text");

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with target type PART_TARGET"
                 " with DEL action.");
   result = edje_edit_part_exist(style->obj, "elm.text");
   ck_assert_msg(result, "Deleting part didn't canceled");
   check_states = edje_edit_part_states_list_get(style->obj, "elm.text");
   result = true;
   EINA_LIST_FOREACH(states, l, state)
     {
        if (!eina_list_data_find_list(check_states, state))
          result = false;
     }
   ck_assert_msg(result, "Not all states are restored");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p20");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p21</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 ADD part with name "part_add" with type RECT.
 * @step 10 Store diff with using history_diff_add function.
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that part "part_add" not exist.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p21)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p21");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   workspace_edit_object_part_add(app->workspace, "part_add", EDJE_PART_TYPE_RECTANGLE, NULL);
   history_diff_add(style->obj, PART_TARGET, ADD, "part_add");

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with target type PART_TARGET"
                 " with ADD action.");
   result = edje_edit_part_exist(style->obj, "part_add");
   ck_assert_msg(!result, "Adding part didn't canceled");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p21");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * <tr>
 * <td>history_undo</td>
 * <td>history_undo_test_p22</td>:
 * <td>
 * @precondition
 * @step 1 Initialize elementary library.
 * @step 2 Initialize Application Data structure.
 * @step 3 Initialize main window.
 * @step 4 Open project.
 * @step 5 Fill widget inlist with data.
 * @step 6 Find style that represent the group "elm/radio/base/def"
 * @step 7 Load style into project.
 * @step 8 Register in history style object, that finded at step 6, as module.
 * @step 9 Store part name, that below part "radio".
 * @step 10 Store diff with using history_diff_add function.
 * @step 11 Restack below part with name "radio".
 *
 * @procedure
 * @step 1 Call history_undo for object from step 6 of precondition.
 * @step 2 Check returned value.
 * @step 3 Check that part "radio" is below part, name of that was stored at step 9.
 * </td>
 * <td>(Evas_Object *) style->obj, (int) 1 </td>
 * <td>All checks passed</td>
 * </tr>
 * @}
 */
EFLETE_TEST(history_undo_test_p22)
{
   App_Data *app = NULL;
   Style *style = NULL;
   Eina_Bool result = EINA_FALSE;
   Eina_Stringshare *below = NULL, *check = NULL;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("history_undo_test_p22");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win),
                                app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   history_module_add(style->obj);
   history_diff_add(style->obj, PART_TARGET, RESTACK, "radio");
   below = eina_stringshare_add(edje_edit_part_below_get(style->obj, "radio"));
   edje_edit_part_restack_below(style->obj, "part_add");

   result = history_undo(style->obj, 1);
   ck_assert_msg(result, "Failed to undo diff with target type PART_TARGET"
                 " with RESTACK action.");
   check = eina_stringshare_add(edje_edit_part_below_get(style->obj, "radio"));
   ck_assert_msg(check == below, "Restack part didn't canceled");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("history_undo_test_p22");
   elm_shutdown();}
END_TEST

/**
 * @addtogroup history_undo
 * @{
 * </TABLE>
 * @}
 * @}
 */
