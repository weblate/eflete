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

#include "test_shortcuts.h"

/**
 * @addtogroup shortcuts_test
 * @{
 * @addtogroup shortcuts_main_add
 * @{
 * Shortcuts
 * <TABLE>
 * @}
 */

/**
 * @addtogroup shortcuts_main_add
 * @{
 * <tr>
 * <td>shortcuts_main_add</td>
 * <td>shortcuts_main_add_test_p</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 initialize and get application data
 * @step 3 load "default" profile
 * @step 4 get loaded "default" profile
 * @step 5 load "default" profile's shortcuts
 *
 * @procedure
 * @step 1 call shortcuts_main_add
 * </td>
 * <td>(App_Data *)app</td>
 * <td>EINA_TRUE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(shortcuts_main_add_test_p)
{
   App_Data *ap;

   elm_init(0,0);
   app_init();
   ap = app_data_get();
   profile_load("default");

   shortcuts_profile_load(ap, profile_get());
   ck_assert_msg(shortcuts_main_add(ap), "Cant set main shortcut's callback function.");

   shortcuts_shutdown();
   app_shutdown();
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup shortcuts_main_add
 * @{
 * <tr>
 * <td>shortcuts_main_add</td>
 * <td>shortcuts_main_add_test_n1</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 * @step 2 initialize and get application data
 * @step 3 load "default" profile
 * @step 4 get loaded "default" profile
 * @step 5 load "default" profile's shortcuts
 *
 * @procedure
 * @step 1 call shortcuts_main_add
 * @step 2 call shortcuts_main_add
 * </td>
 * <td>(App_Data *)app</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(shortcuts_main_add_test_n1)
{
   App_Data *ap;

   elm_init(0,0);
   app_init();
   ap = app_data_get();
   profile_load("default");

   shortcuts_profile_load(ap, profile_get());
   ck_assert_msg(shortcuts_main_add(ap), "Cant set main shortcut's callback function.");
   ck_assert_msg(!shortcuts_main_add(ap), "Main shortcut's callback function was set again.");

   shortcuts_shutdown();
   app_shutdown();
   elm_shutdown();
}
END_TEST

/**
 * @addtogroup shortcuts_main_add
 * @{
 * <tr>
 * <td>shortcuts_main_add</td>
 * <td>shortcuts_main_add_test_n2</td>
 * <td>
 * @precondition
 * @step 1 initialize elementary library
 *
 * @procedure
 * @step 1 call shortcuts_main_add
 * </td>
 * <td>NULL</td>
 * <td>EINA_FALSE</td>
 * <td>_REAL_RESULT_</td>
 * <td>_PASSED_</td>
 * </tr>
 * @}
 */
EFLETE_TEST(shortcuts_main_add_test_n2)
{
   elm_init(0,0);

   ck_assert_msg(!shortcuts_main_add(NULL), "Main shortcut's callback function was set again.");

   elm_shutdown();
}
END_TEST

/**
 * @addtogroup shortcuts_main_add
 * @{
 * </TABLE>
 * @}
 * @}
 */
