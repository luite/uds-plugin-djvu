/*
 * File Name: font_cache.h
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

#ifndef FONT_CACHE_H
#define FONT_CACHE_H

#include <stdio.h>
#include <list>

#ifdef _WIN32
#include <unordered_map>
#else
#include <tr1/unordered_map>
#endif

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

/// @brief Cached font glyphs.
/// The class is designed to contain multiple font glyphs. For every font glyph, 
/// The caller should provide a hash code.
class FontCache
{
public:
    ~FontCache();
    static FontCache& instance()
    {
        static FontCache _instance;
        return _instance;
    }

private:
    /// @brief Constructors and destructors
    FontCache();
    FontCache(const FontCache&);

public:
    void remove_smallest();
    void clear();
    FT_BitmapGlyph get_glyph(FT_Face face, const unsigned int hash_code, int ch);

private:
    typedef unsigned int UINT32;
    typedef std::tr1::unordered_map<FT_UInt, FT_Glyph> GlyphTable;
    typedef GlyphTable::iterator        GlyphIter;

    typedef std::tr1::unordered_map<UINT32, GlyphTable> FontTable;
    typedef FontTable::iterator          FontIter;
    FontTable cache;

    typedef std::tr1::unordered_map<UINT32, UINT32> Code;
    typedef Code::iterator           CodeIter;
    Code codes;
};

#endif // FONT_CACHE_H
