/* Edje Theme Editor
* Copyright (C) 2013 Samsung Electronics.
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
* along with this program; If not, see .
*/

#include "project_manager.h"
#include "alloc.h"

static const char *dst_path;

static void
_on_copy_done_cb(void *data,
                 Eio_File *handler __UNUSED__)
{
   char *file_name = (char *)data;
   DBG("Copy file '%s' is finished!", file_name);
   ecore_main_loop_quit();
}

static void
_on_copy_done_save_as_cb(void *data,
                 Eio_File *handler __UNUSED__)
{
   Project *project = (Project *)data;
   if (project->edj)
     free(project->edj);
   project->edj = strdup(dst_path);
   DBG("Copy file '%s' is finished!", dst_path);
   dst_path = NULL;
   ecore_main_loop_quit();
}

static void
_on_copy_error_cb(void *data,
                  Eio_File *handler __UNUSED__,
                  int error)
{
   char *file_name = (char *)data;
   ERR("Copy file '%s' is failed. Something wrong has happend: %s\n",
       file_name, strerror(error));
   ecore_main_loop_quit();
}

static void
_on_unlink_done_cb(void *data,
                   Eio_File *handler __UNUSED__)
{
   Project *project = (Project *)data;
   if (project->edc) free(project->edc);
   if (project->edj) free(project->edj);
   if (project->image_directory) free(project->image_directory);
   if (project->font_directory) free(project->font_directory);
   if (project->sound_directory) free(project->sound_directory);
   if (project->compiler) compiler_free(project->compiler);
   if (project->decompiler) decompiler_free(project->decompiler);
   INFO ("Closed project: %s", project->name);
   free(project->name);
   wm_widget_list_free(project->widgets);
   DBG ("Project data is released.");
   ecore_main_loop_quit();
}

static void
_on_unlink_error_cb(void *data,
                    Eio_File *handler __UNUSED__,
                    int error)
{
   char *file_name = (char *)data;
   ERR("Unlink file '%s' is failed. Something wrong has happend: %s\n",
       file_name, strerror(error));
   ecore_main_loop_quit();
}

Eina_Bool
pm_free(Project *project)
{
   if (!project) return EINA_FALSE;

   eio_file_unlink(project->swapfile, _on_unlink_done_cb,
                   _on_unlink_error_cb, project);
   ecore_main_loop_begin();

   if (project->swapfile) free(project->swapfile);
   free(project);
   return EINA_TRUE;
}

static Project *
pm_project_add(const char *name,
               const char *path,
               const char *id, /* image directory */
               const char *fd, /* font directory */
               const char *sd  /* sound directory */)
{
   Project *pro;
   char *tmp = NULL;

   pro = mem_calloc(1, sizeof(Project));
   pro->name = strdup(name);
   DBG ("Project name: '%s'", pro->name);

   /* set path to edc */
   pro->edc = strdup(path);

   /* FIXME: A substirng '.edj' can meet in the middle of the path */
   tmp = strstr(pro->edc, ".edj");
   if (tmp != NULL)
     strncpy(tmp, ".edc", 4);
   tmp = NULL;

   /* set path to edj */
   pro->edj = strdup(path);

   /* FIXME: A substring '.edc' can meet in the middle of the path */
   tmp = strstr(pro->edj, ".edc");
   if (tmp != NULL)
     strncpy(tmp, ".edj", 4);

   tmp = NULL;
   DBG ("Path to edc-file: '%s'", pro->edc);
   DBG ("Path to edj-file: '%s'", pro->edj);

   /* set path to swap file */
   pro->swapfile = mem_malloc((strlen(pro->edj) + 6) * sizeof(char));
   strcpy(pro->swapfile, pro->edj);
   strncat(pro->swapfile, ".swap", 5);
   DBG ("Path to swap file: '%s'", pro->swapfile);

   /* set path to image directory */
   pro->image_directory = id ? strdup(id) : NULL;
   DBG ("Path to image direcotory: '%s'", pro->image_directory);

   /* set path to font directory */
   pro->font_directory = fd ? strdup(fd) : NULL;
   DBG("Path to font direcotory: '%s'", pro->font_directory);

   /* set default path to sound directory */
   pro->sound_directory = sd ? strdup(sd) : NULL;
   DBG ("Path to sound direcotory: '%s'", pro->sound_directory);

   pro->compiler = NULL;
   pro->decompiler = NULL;

   return pro;
}

void
pm_save_project_edc(Project *project)
{
   char **split;
   const char *save_path;
   int size;
   char *save_dir;

   if (!project)
     {
        WARN("Project is missing. Please open one.");
        return;
     }

   /* compile project and create swapfile */
   project->decompiler = decompile(project->edj, NULL);

   if (project->decompiler)
     {
        split = eina_str_split(ecore_file_file_get(project->edj), ".", 2);

        save_path = ecore_file_dir_get(project->edj);
        size = strlen(save_path) + BUFF_MAX;
        save_dir = mem_malloc(size * sizeof(char));
        sprintf(save_dir, "%s/DECOMPILED", save_path);


        if (!ecore_file_mkpath(save_dir))
          {
             NOTIFY_WARNING("Could not create dir with decompiled project. <br>"
                          "Please check directory permissions or move edj file into another directory.");
             free(save_dir);
             free(split[0]);
             free(split);
             return;
          }

        eio_dir_move(split[0], save_dir, NULL, NULL,
                     _on_copy_done_cb, _on_copy_error_cb, NULL);
        ecore_main_loop_begin();

        free(save_dir);
        free(split[0]);
        free(split);
     }
}

Project *
pm_open_project_edc(const char *name,
                    const char *path,
                    const char *image_directory,
                    const char *font_directory,
                    const char *sound_directory)
{
   Project *project = NULL;

   if (!path) return NULL;

   project = pm_project_add(name,
                            path,
                            image_directory,
                            font_directory,
                            sound_directory);

   /* compile project and create swapfile */
   project->compiler = compile(project->edc,
                               project->edj,
                               project->image_directory,
                               project->font_directory,
                               project->sound_directory);
   if (project->compiler)
     {
        eio_file_move(project->edj, project->swapfile, NULL,
                      _on_copy_done_cb, _on_copy_error_cb, project->swapfile);
        ecore_main_loop_begin();
     }

   project->widgets = wm_widget_list_new(project->swapfile);

   return project;
}

Project *
pm_open_project_edj(const char *name,
                    const char *path)
{
   Project *project;

   if (!path)
     {
        ERR("Can not open project path is NULL!");
        return NULL;
     }

   INFO("Open project! Path to project: '%s'.", path);
   project = pm_project_add(name, path, NULL, NULL, NULL);
   eio_file_copy(project->edj, project->swapfile, NULL,
                 _on_copy_done_cb, _on_copy_error_cb, project->swapfile);
   ecore_main_loop_begin();
   project->widgets = wm_widget_list_new(project->swapfile);
   INFO("Project '%s' is open!", project->name);

   return project;
}

Eina_Bool
pm_save_project_edj(Project *project)
{
   if (!project) return EINA_FALSE;
   eio_file_copy(project->swapfile, project->edj, NULL,
                 _on_copy_done_cb, _on_copy_error_cb, project->swapfile);
   ecore_main_loop_begin();
   return EINA_TRUE;
}

Eina_Bool
pm_save_as_project_edj(Project *project, const char *path)
{
   if (!project) return EINA_FALSE;
   if (!path) return EINA_FALSE;

   dst_path = path;
   eio_file_copy(project->swapfile, path, NULL,
                 _on_copy_done_save_as_cb, _on_copy_error_cb, project);
   ecore_main_loop_begin();
   return EINA_TRUE;
}

Eina_Bool
pm_save_project_to_swap(Project *project)
{
   Evas_Object *edje_object;
   GET_OBJ(project, edje_object)
   if (!edje_object) return EINA_FALSE;
   else edje_edit_save_all(edje_object);
   return EINA_TRUE;
}
