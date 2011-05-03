/*
 * File Name: pango_renderer.h
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

#ifndef PANGO_RENDERER_H
#define PANGO_RENDERER_H

#include <pango/pango.h>
#include <pango/pangoft2.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define PANGO_GLYPH_EMPTY       ((PangoGlyph)0x0FFFFFFF)

/// @brief Render 1 single pango line at (x,y).
void render_single_line(unsigned char   *bmp,
                        PangoLayoutLine *pango_line,
                        unsigned int    font_hash_code,
                        int             x,
                        int             y,
                        const int       width);

/// @brief Render 1 single glyph at (x,y).
void render_single_glyph(unsigned char        *bmp,
                         int                  x,
                         int                  y,
                         const int            width,
                         const FT_BitmapGlyph glyph);

#endif
