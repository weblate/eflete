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

#ifndef RESOURCE_MANAGER_SIGNALS_H
#define RESOURCE_MANAGER_SIGNALS_H

typedef enum {
   ATTRIBUTE_NONE,

   ATTRIBUTE_GROUP_NAME,
   ATTRIBUTE_GROUP_MIN_W,
   ATTRIBUTE_GROUP_MIN_H,
   ATTRIBUTE_GROUP_MAX_W,
   ATTRIBUTE_GROUP_MAX_H,
   ATTRIBUTE_GROUP_SCRIPT,
   ATTRIBUTE_GROUP_DATA_NAME,
   ATTRIBUTE_GROUP_DATA_VALUE,
   ATTRIBUTE_STATE_NAME,
   ATTRIBUTE_STATE_MIN_W,
   ATTRIBUTE_STATE_MIN_H,
   ATTRIBUTE_STATE_MAX_W,
   ATTRIBUTE_STATE_MAX_H,
   ATTRIBUTE_STATE_ALIGN_X,
   ATTRIBUTE_STATE_ALIGN_Y,
   ATTRIBUTE_STATE_REL1_RELATIVE_X,
   ATTRIBUTE_STATE_REL1_RELATIVE_Y,
   ATTRIBUTE_STATE_REL2_RELATIVE_X,
   ATTRIBUTE_STATE_REL2_RELATIVE_Y,
   ATTRIBUTE_STATE_REL1_OFFSET_X,
   ATTRIBUTE_STATE_REL1_OFFSET_Y,
   ATTRIBUTE_STATE_REL2_OFFSET_X,
   ATTRIBUTE_STATE_REL2_OFFSET_Y,
   ATTRIBUTE_STATE_ASPECT_MIN,
   ATTRIBUTE_STATE_ASPECT_MAX,
   ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_X,
   ATTRIBUTE_STATE_FILL_ORIGIN_RELATIVE_Y,
   ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_X,
   ATTRIBUTE_STATE_FILL_ORIGIN_OFFSET_Y,
   ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_X,
   ATTRIBUTE_STATE_FILL_SIZE_RELATIVE_Y,
   ATTRIBUTE_STATE_FILL_SIZE_OFFSET_X,
   ATTRIBUTE_STATE_FILL_SIZE_OFFSET_Y,
   ATTRIBUTE_STATE_TEXT_ALIGN_X,
   ATTRIBUTE_STATE_TEXT_ALIGN_Y,
   ATTRIBUTE_STATE_TEXT_ELIPSIS,
   ATTRIBUTE_STATE_TEXT_ELIPSIS_ENABLE /* virtual attribute */,
   ATTRIBUTE_STATE_TEXT_SIZE,
   ATTRIBUTE_STATE_TEXT_FIT_X,
   ATTRIBUTE_STATE_TEXT_FIT_Y,
   ATTRIBUTE_STATE_TEXT_MAX_X,
   ATTRIBUTE_STATE_TEXT_MAX_Y,
   ATTRIBUTE_STATE_TEXT_MIN_X,
   ATTRIBUTE_STATE_TEXT_MIN_Y,
   ATTRIBUTE_STATE_FIXED_H,
   ATTRIBUTE_STATE_FIXED_W,
   ATTRIBUTE_STATE_FILL_SMOOTH,
   ATTRIBUTE_STATE_VISIBLE,
   ATTRIBUTE_STATE_VECTOR,
   ATTRIBUTE_STATE_IMAGE,
   ATTRIBUTE_STATE_IMAGE_TWEEN,
   ATTRIBUTE_STATE_COLOR_CLASS,
   ATTRIBUTE_STATE_REL1_TO_X,
   ATTRIBUTE_STATE_REL1_TO_Y,
   ATTRIBUTE_STATE_REL2_TO_X,
   ATTRIBUTE_STATE_REL2_TO_Y,
   ATTRIBUTE_STATE_TEXT_SOURCE,
   ATTRIBUTE_STATE_TEXT_TEXT_SOURCE,
   ATTRIBUTE_STATE_TEXT,
   ATTRIBUTE_STATE_FONT,
   ATTRIBUTE_STATE_TEXT_STYLE,
   ATTRIBUTE_STATE_COLOR,
   ATTRIBUTE_STATE_OUTLINE_COLOR,
   ATTRIBUTE_STATE_SHADOW_COLOR,
   ATTRIBUTE_STATE_IMAGE_BORDER_TOP,
   ATTRIBUTE_STATE_IMAGE_BORDER_BOTTOM,
   ATTRIBUTE_STATE_IMAGE_BORDER_LEFT,
   ATTRIBUTE_STATE_IMAGE_BORDER_RIGHT,
   ATTRIBUTE_STATE_IMAGE_BORDER_FILL,
   ATTRIBUTE_STATE_ASPECT_PREF,
   ATTRIBUTE_PART_TEXT_EFFECT,
   ATTRIBUTE_PART_TEXT_SHADOW_DIRECTION,
   ATTRIBUTE_PART_IGNORE_FLAGS,
   ATTRIBUTE_PART_MOUSE_EVENTS,
   ATTRIBUTE_PART_REPEAT_EVENTS,
   ATTRIBUTE_PART_SCALE,
   ATTRIBUTE_PART_DRAG_COUNT_X,
   ATTRIBUTE_PART_DRAG_COUNT_Y,
   ATTRIBUTE_PART_DRAG_X,
   ATTRIBUTE_PART_DRAG_Y,
   ATTRIBUTE_PART_DRAG_STEP_X,
   ATTRIBUTE_PART_DRAG_STEP_Y,
   ATTRIBUTE_PART_CLIP_TO,
   ATTRIBUTE_PART_DRAG_CONFINE,
   ATTRIBUTE_PART_DRAG_THRESHOLD,
   ATTRIBUTE_PART_DRAG_EVENT,
   ATTRIBUTE_PART_NAME,
   ATTRIBUTE_PART_TYPE,
   ATTRIBUTE_PART_GROUP_SOURCE,
   ATTRIBUTE_PART_TEXTBLOCK_SELECTION_UNDER,
   ATTRIBUTE_PART_TEXTBLOCK_SELECTION_OVER,
   ATTRIBUTE_PART_TEXTBLOCK_CURSOR_UNDER,
   ATTRIBUTE_PART_TEXTBLOCK_CURSOR_OVER,
   ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_UNDER,
   ATTRIBUTE_PART_TEXTBLOCK_ANCHORS_OVER,
   ATTRIBUTE_PART_ITEM_ASPECT_MODE,
   ATTRIBUTE_PART_ITEM_ALIGN_X,
   ATTRIBUTE_PART_ITEM_ALIGN_Y,
   ATTRIBUTE_PART_ITEM_WEIGHT_X,
   ATTRIBUTE_PART_ITEM_WEIGHT_Y,
   ATTRIBUTE_PART_ITEM_ASPECT_H,
   ATTRIBUTE_PART_ITEM_ASPECT_W,
   ATTRIBUTE_PART_ITEM_MAX_H,
   ATTRIBUTE_PART_ITEM_MAX_W,
   ATTRIBUTE_PART_ITEM_MIN_H,
   ATTRIBUTE_PART_ITEM_MIN_W,
   ATTRIBUTE_PART_ITEM_NAME,
   ATTRIBUTE_PART_ITEM_PREFER_H,
   ATTRIBUTE_PART_ITEM_PREFER_W,
   ATTRIBUTE_PART_ITEM_SPREAD_H,
   ATTRIBUTE_PART_ITEM_SPREAD_W,
   ATTRIBUTE_PART_ITEM_SPAN_COL,
   ATTRIBUTE_PART_ITEM_SPAN_ROW,
   ATTRIBUTE_PART_ITEM_POSITION_COL,
   ATTRIBUTE_PART_ITEM_POSITION_ROW,
   ATTRIBUTE_PART_ITEM_SOURCE,
   ATTRIBUTE_PART_ITEM_PADDING_TOP,
   ATTRIBUTE_PART_ITEM_PADDING_BOTTOM,
   ATTRIBUTE_PART_ITEM_PADDING_LEFT,
   ATTRIBUTE_PART_ITEM_PADDING_RIGHT,
   ATTRIBUTE_STATE_CONTAINER_ALIGN_X,
   ATTRIBUTE_STATE_CONTAINER_ALIGN_Y,
   ATTRIBUTE_STATE_CONTAINER_MIN_H,
   ATTRIBUTE_STATE_CONTAINER_MIN_V,
   ATTRIBUTE_STATE_TABLE_HOMOGENEOUS,
   ATTRIBUTE_STATE_CONTAINER_PADING_X,
   ATTRIBUTE_STATE_CONTAINER_PADING_Y,
   ATTRIBUTE_STATE_MINMUL_H,
   ATTRIBUTE_STATE_MINMUL_W,
   ATTRIBUTE_PART_MULTILINE,
   ATTRIBUTE_STATE_PROXY_SOURCE,
   ATTRIBUTE_PART_SELECT_MODE,
   ATTRIBUTE_PART_ENTRY_MODE,
   ATTRIBUTE_PART_POINTER_MODE,
   ATTRIBUTE_PART_CURSOR_MODE,
   ATTRIBUTE_STATE_FILL_TYPE,
   ATTRIBUTE_STATE_MAP_ON,
   ATTRIBUTE_STATE_MAP_PERSPECTIVE_ON,
   ATTRIBUTE_STATE_MAP_PERSPECTIVE,
   ATTRIBUTE_STATE_MAP_LIGHT,
   ATTRIBUTE_STATE_MAP_SMOOTH,
   ATTRIBUTE_STATE_MAP_ALPHA,
   ATTRIBUTE_STATE_MAP_BACKFACE_CULL,
   ATTRIBUTE_STATE_MAP_PERSPECTIVE_FOCAL,
   ATTRIBUTE_STATE_MAP_PERSPECTIVE_ZPLANE,
   ATTRIBUTE_STATE_MAP_ROTATION_CENTER,
   ATTRIBUTE_STATE_MAP_ROTATION_X,
   ATTRIBUTE_STATE_MAP_ROTATION_Y,
   ATTRIBUTE_STATE_MAP_ROTATION_Z,
   ATTRIBUTE_STATE_MAP_POINT_COLOR_1,
   ATTRIBUTE_STATE_MAP_POINT_COLOR_2,
   ATTRIBUTE_STATE_MAP_POINT_COLOR_3,
   ATTRIBUTE_STATE_MAP_POINT_COLOR_4,
   ATTRIBUTE_PROGRAM_TRANSITION_TYPE,
   ATTRIBUTE_PROGRAM_TRANSITION_FROM_CURRENT,
   ATTRIBUTE_PROGRAM_ACTION,
   ATTRIBUTE_PROGRAM_CHANNEL,
   ATTRIBUTE_PROGRAM_TONE_DURATION,
   ATTRIBUTE_PROGRAM_IN_FROM,
   ATTRIBUTE_PROGRAM_IN_RANGE,
   ATTRIBUTE_PROGRAM_TRANSITION_TIME,
   ATTRIBUTE_PROGRAM_SAMPLE_SPEED,
   ATTRIBUTE_PROGRAM_VALUE,
   ATTRIBUTE_PROGRAM_TRANSITION_FACTOR,
   ATTRIBUTE_PROGRAM_TRANSITION_GRADIENT,
   ATTRIBUTE_PROGRAM_TRANSITION_DECAY,
   ATTRIBUTE_PROGRAM_TRANSITION_BOUNCES,
   ATTRIBUTE_PROGRAM_TRANSITION_SWINGS,
   ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X1,
   ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y1,
   ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_X2,
   ATTRIBUTE_PROGRAM_TRANSITION_BEZIER_Y2,
   ATTRIBUTE_PROGRAM_FILTER_PART,
   ATTRIBUTE_PROGRAM_FILTER_STATE,
   ATTRIBUTE_PROGRAM_API_NAME,
   ATTRIBUTE_PROGRAM_API_DESCRIPTION,
   ATTRIBUTE_PROGRAM_SAMPLE_NAME,
   ATTRIBUTE_PROGRAM_TONE_NAME,
   ATTRIBUTE_PROGRAM_SIGNAL,
   ATTRIBUTE_PROGRAM_SOURCE,
   ATTRIBUTE_PROGRAM_STATE,
   ATTRIBUTE_PROGRAM_EMIT_SIGNAL,
   ATTRIBUTE_PROGRAM_EMIT_SOURCE,
   ATTRIBUTE_PROGRAM_DRAG_VALUE_X,
   ATTRIBUTE_PROGRAM_DRAG_VALUE_Y,
   ATTRIBUTE_PROGRAM_NAME,
   ATTRIBUTE_PROGRAM_TARGET,
   ATTRIBUTE_PROGRAM_AFTER,
   ATTRIBUTE_PROGRAM_SCRIPT,

   ATTRIBUTE_LAST
} Attribute;

typedef enum {
   ATTRIBUTE_RESOURCES_NONE,

   ATTRIBUTE_RESOURCES_COLORCLASS_DESCRIPTION,
   ATTRIBUTE_RESOURCES_COLORCLASS_COLORS,
   ATTRIBUTE_RESOURCES_STYLE_TAG_ADDED,
   ATTRIBUTE_RESOURCES_STYLE_TAG_DELETED,

   ATTRIBUTE_RESOURCES_LAST
} Attribute_Resource;

#endif /* RESOURCE_MANAGER_SIGNALS_H */
