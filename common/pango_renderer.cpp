/*
 * File Name: pango_renderer.cpp
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

#include "font_cache.h"
#include "pango_renderer.h"

void render_single_line(unsigned char   *bmp,
                        PangoLayoutLine *pango_line,
                        unsigned int    font_hash_code,
                        int             x,
                        int             y,
                        const int       width)
{
    // Draw run list .
    int glyph_pos_x = 0, glyph_pos_y = 0;
    GSList *runs_list = pango_line->runs;

    FontCache& font_cache = FontCache::instance();

    // draw each pango run
    while (runs_list)
    {
        // Get glyph data.
        PangoLayoutRun *run = (PangoLayoutRun *)runs_list->data;
        PangoGlyphString *glyphs = run->glyphs;
        FT_Face ft_face = pango_fc_font_lock_face(PANGO_FC_FONT(run->item->analysis.font));

        // draw each glyph
        for (int i=0; i<glyphs->num_glyphs; i++)
        {
            PangoGlyphGeometry geometry = glyphs->glyphs[i].geometry;
            glyph_pos_x = x + geometry.x_offset;
            glyph_pos_y = y + geometry.y_offset;
            x += geometry.width; 

            if (glyphs->glyphs[i].glyph == PANGO_GLYPH_EMPTY)
            {
                continue;
            }

            // load glyph bitmap and copy
            FT_BitmapGlyph data = font_cache.get_glyph(ft_face,
                                                       font_hash_code,
                                                       glyphs->glyphs[i].glyph);
            if (data)
            {
                render_single_glyph(bmp,
                                    PANGO_PIXELS(glyph_pos_x) + data->left,
                                    PANGO_PIXELS(glyph_pos_y) - data->top,
                                    width,
                                    data);
            }
        }

        pango_fc_font_unlock_face(PANGO_FC_FONT(run->item->analysis.font));
        runs_list = runs_list->next;
    }
}

void render_single_glyph(unsigned char        *bmp,
                         int                  x,
                         int                  y,
                         const int            width,
                         const FT_BitmapGlyph glyph)
{
    unsigned char *src = glyph->bitmap.buffer;
    unsigned char *dst = bmp + y * width + x;

    for(int i = 0; i < glyph->bitmap.rows; i++)
    {
        unsigned char *p = dst;
        for (int j = 0; j < glyph->bitmap.pitch; j++)
        {
            *p++ ^= *src++;
        }
        dst += width;
    }
}
