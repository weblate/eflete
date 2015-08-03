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

#include "test_ui_workspace.h"
#include "main_window.h"
#include "test_common.h"

/**
 * @addtogroup ui_workspace_test
 * @{
 * @addtogroup workspace_zoom_factor_get
 * @{
 * ui_workspace
 * <TABLE>
 * @}
 */

/**
 * @addtogroup workspace_zoom_factor_get
 * @{
 * <tr>
 * <td>workspace_zoom_factor_get</td>
 * <td>workspace_zoom_factor_get_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 load extenstion theme from EFLETE_THEME file
 * @step 3 create parent window
 * @step 4 Mmap edj file.
 * @step 5 create workspace object
 * @step 6 create style
 * @step 7 load data into created style from edj file
 * @step 8 set loaded object into workspace
 * @step 9 Set zoom factor equal 1.5
 *
 * @procedure
 * @step 1 call workspace_zoom_factor_get
 * @step 2 check returned value
 * </td>
 * <td>(Evas_Object *) workspace</td>
 * <td>Returned (double) 1.5</td>
 * </tr>
 * @}
 */
EFLETE_TEST (workspace_zoom_factor_get_test_p)
{
   App_Data *app = NULL;
   Style *style = NULL;

   elm_init(0, 0);
   app_init();

   app = app_data_get();
   ui_main_window_add(app);
   app->project = setup("workspace_zoom_factor_get_test_p");
   wm_widgets_list_objects_load(app->project->widgets,
                                evas_object_evas_get(app->win), app->project->mmap_file);
   blocks_show(app);
   style = wm_style_object_find(app->project->widgets, "elm/radio/base/def");
   ui_style_clicked(app, style);
   workspace_zoom_factor_set(app->workspace, 1.5);

   ck_assert_msg(workspace_zoom_factor_get(app->workspace) == 1.5, "Failed get zoom factor");

   pm_project_close(app->project);
   app->project = NULL;
   ui_main_window_del(app);
   app_shutdown();
   teardown("./workspace_zoom_factor_get_test_p");
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup workspace_zoom_factor_get
 * @{
 * </TABLE>
 * @}
 * @}
 */
