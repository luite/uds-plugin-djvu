/*
 * File Name: plugin_render_settings.h
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

#ifndef PLUGIN_RENDER_SETTINGS_H_
#define PLUGIN_RENDER_SETTINGS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Render Settings interface
 * Through IPluginRenderSetting, the caller can set/get the general rendering
 * attributes to/from plugin. Different with view settings, the rendering 
 * settings won't affect all of the rendered pages in a document, but only
 * affect the current page on displaying. IPluginRenderSettings only manages
 * the general settings fit for all of the document formats.
 */

typedef struct
{
  //TODO: Add functions of common render settings into this interface
} IPluginRenderSettings;

#ifdef __cplusplus
}
#endif 

#endif
