/*
 * File Name: plugin_font.h
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

#ifndef PLUGIN_FONT_H_
#define PLUGIN_FONT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Font interface.
 * Through IPluginFont, caller can get/set font size and font family.
 */
typedef struct
{
    /**
     * @brief Get the font size in point.
     * @param thiz IPluginUnknown pointer to the object which implments
     * IPluginFont.
     * @return This function returns the font size in point.
     */
    int (* get_font_size)( IPluginUnknown   *thiz );

    /**
     * @brief Set the font size in point.
     * @param thiz IPluginUnknown pointer to the object which implments 
     * IPluginFont.
     * @param font_size The font size to be set.
     * @return TODO. Add return code here.
     */
    PluginStatus (* set_font_size)( IPluginUnknown      *thiz, 
                                    const unsigned int  font_size );

    
    /**
     * @brief Get the font family.
     * @param thiz IPluginUnknown pointer to the object which implments 
     * IPluginFont.
     * @param font_family The output string of the font_family.
     * @return TODO. Add return code here.
     */
    PluginStatus (* get_font_family)( IPluginUnknown    *thiz, 
                                      UDSString         *font_family );

    /**
     * @brief Set the font family.
     * @param thiz IPluginUnknown pointer to the object which implments 
     * IPluginFont.
     * @param font_family The input string of the font_family which is going
     * to be set.
     * @return TODO. Add return code here.
     */
    PluginStatus (* set_font_family)( IPluginUnknown    *thiz, 
                                      const UDSString   *font_family );

} IPluginFont;



#ifdef __cplusplus
}
#endif 

#endif

