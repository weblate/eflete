/**
 * Edje Theme Editor
 * Copyright (C) 2013-2014 Samsung Electronics.
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

#include "test_save_file_dialog.h"

/**
 * @addtogroup save_file_dialog_test
 * @{
 * @addtogroup save_as_edc_file
 * @{
 * save_file_dialog
 * <TABLE>
 * @}
 */

/**
 * @addtogroup save_as_edc_file
 * @{
 * <tr>
 * <td>save_as_edc_file</td>
 * <td>save_as_edc_file_test_n1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 add theme extension "eflete theme".
 * @step 3 initialize application with app_init() function
 * @step 4 create application data
 * @step 5 create main window
 *
 * @procedure
 * @step 1 Call function save_as_edc_file(app_data).
 * @step 2 Check returned value.
 * </td>
 * <td>App_Data *app_data</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (save_as_edc_file_test_n1)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   App_Data *app_data;
   Eina_Bool result = EINA_FALSE;

   app_init();
   app_data = app_create();
   ui_main_window_add(app_data);

   result = save_as_edc_file(app_data);
   ck_assert_msg(result == EINA_FALSE, "'Save as edc' dialog was created");

   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup save_as_edc_file
 * @{
 * <tr>
 * <td>save_as_edc_file</td>
 * <td>save_as_edc_file_test_n2</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 add theme extension "eflete theme".
 * @step 3 initialize application with app_init() function
 * @step 4 create application data
 * @step 5 open new project and create widget list
 *
 * @procedure
 * @step 1 Call function save_as_edc_file(app_data).
 * @step 2 Check returned value.
 * </td>
 * <td>App_Data *app_data</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (save_as_edc_file_test_n2)
{
   elm_init(0, 0);
   elm_theme_extension_add(NULL, EFLETE_THEME);
   App_Data *app_data;
   Eina_Bool result = EINA_FALSE;
   const char *path = "./edj_build/save_as_edc_file.edj";

   app_init();
   app_data = app_create();
   ui_edj_load_done(app_data, path);

   result = save_as_edc_file(app_data);
   ck_assert_msg(result == EINA_FALSE, "'Save as edc' dialog was created");

   elm_theme_extension_del(NULL, EFLETE_THEME);
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup save_as_edc_file
 * @{
 * <tr>
 * <td>save_as_edc_file</td>
 * <td>save_as_edc_file_test_n3</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 Call function save_as_edc_file(NULL).
 * @step 2 Check returned value.
 * </td>
 * <td>NULL</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST (save_as_edc_file_test_n3)
{
   elm_init(0, 0);
   Eina_Bool result = EINA_FALSE;

   result = save_as_edc_file(NULL);
   ck_assert_msg(result == EINA_FALSE, "'Save as edc' dialog was created");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup save_as_edc_file
 * @{
 * </TABLE>
 * @}
 * @}
 */
