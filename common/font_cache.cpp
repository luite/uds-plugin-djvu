/*
 * File Name: font_cache.cpp
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

#include <math.h>

#include "log.h"
#include "font_cache.h"

FontCache::FontCache(void)
{
}

FontCache::FontCache(const FontCache& r)
{
}

FontCache::~FontCache(void)
{
    clear();
}

void FontCache::remove_smallest()
{
    // it's necessary to maintain at least one kind of glyph
    if (codes.size() <= 1)
    {
        return;
    }

    // get smallest one
    UINT32 count = (UINT32)-1;
    CodeIter smallest = codes.begin();
    for(CodeIter ci = codes.begin(); ci != codes.end(); ++ci)
    {
        if (count > ci->second)
        {
            smallest = ci;
            count = ci->second;
        }
    }
    UINT32 code = smallest->first;
    codes.erase(smallest);

    // release them
    FontIter it = cache.find(code);
    if (it != cache.end())
    {
        for(GlyphIter t = it->second.begin(); t != it->second.end(); ++t)
        {
            FT_Done_Glyph(t->second);
        }
        it->second.clear();
        cache.erase(it);
    }
}

void FontCache::clear()
{
    for(FontIter it = cache.begin(); it != cache.end(); ++it)
    {
        for(GlyphIter t = it->second.begin(); t != it->second.end(); ++t)
        {
            FT_Done_Glyph(t->second);
        }
        it->second.clear();
    }
    cache.clear();

}

FT_BitmapGlyph FontCache::get_glyph(FT_Face face, const unsigned int hash_code, int ch)
{
    FT_UInt index = ch; 
    
    // check hash code at first
    CodeIter ci = codes.find(hash_code);
    if (ci == codes.end())
    {
        codes[hash_code] = 0;
    }

    // search 
    FontIter iter = cache.find(hash_code);
    if (iter != cache.end())
    {
        GlyphIter t = iter->second.find(index);
        if (t != iter->second.end())
        {
            return reinterpret_cast<FT_BitmapGlyph>(t->second);
        }
    }
    ++codes[hash_code];

    // load glyph 
    if (FT_Load_Glyph(face, index, FT_LOAD_DEFAULT))
    {
        ERRORPRINTF("Unable to load glyph!");
        return 0;
    }    

    // get glyph
    FT_Glyph glyph;
    if (FT_Get_Glyph(face->glyph, &glyph))
    {
        ERRORPRINTF("Unable to copy glyph!");
        return 0;
    }
    
    // convert to bitmap
    if (glyph->format != FT_GLYPH_FORMAT_BITMAP)
    {
        if (FT_Glyph_To_Bitmap(&glyph, FT_RENDER_MODE_NORMAL, 0, 1))
        {
            ERRORPRINTF("Unable to render glyph!");
            return 0;
        }
    }

    // cache it
    cache[hash_code][index] = glyph;

    return (FT_BitmapGlyph)glyph;
}
