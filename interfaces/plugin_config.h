/*
 * File Name: plugin_config.h
 */

/*
 * This file is part of uds-plugin-common.
 *
 * uds-plugin-common is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-common is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Copyright (C) 2008 iRex Technologies B.V.
 * All rights reserved.
 */

#ifndef PLUGIN_CONFIG_H_
#define PLUGIN_CONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif

    // TODO. remove unnecessary defines. 
    // change name from config to constants.
#define PLUGIN_MAX_ERROR_MSG_LENGTH     1024    /**< The max error message length. Whenever you change it you must increase it. */
#define PLUGIN_MAX_PATH                 1025

/**
 * Zoom factor.
 */
#define PLUGIN_ZOOM_INVALID               0.0f
#define PLUGIN_ZOOM_TO_PAGE              -1.0f
#define PLUGIN_ZOOM_TO_WIDTH             -2.0f
#define PLUGIN_ZOOM_TO_CROP              -3.0f
#define PLUGIN_ZOOM_TO_CROP_BY_PAGE     -10.0f
#define PLUGIN_ZOOM_TO_CROP_BY_WIDTH    -11.0f
#define PLUGIN_ZOOM_DEFAULT             PLUGIN_ZOOM_TO_PAGE
#define PLUGIN_ZOOM_MAX                 200.0f
#define PLUGIN_ZOOM_MIN                  75.0f


/**
 * Font size in points.
 */
#define PLUGIN_FONT_SIZE_INVALID         0
#define PLUGIN_FONT_SIZE_DEFAULT        16

/**
 * Page number.
 */
#define PLUGIN_MAX_ANCHOR_LENGTH        1024
#define PLUGIN_DEFAULT_START_PAGE       1
#define PLUGIN_DEFAULT_PAGE_NUMBER      1
#define PLUGIN_DEFAULT_PAGE_COUNT       1
#define PLUGIN_PAGE_NUMBER_PENDING      -1  /**< The total page number is still unknown. */
#define PLUGIN_INVALID_PAGE_NUMBER      -2

/**
 * Encoding.
 */
#define PLUGIN_ENCODING_MAX_LENGTH      256

#ifdef __cplusplus
}
#endif 

#endif

