/*
 * File Name: utils.cpp
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

#include <cassert>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <iconv.h>
// #include <sys/sysinfo.h>

#include "utils.h"
#include "log.h"

int get_free_memory()
{
    /*
    struct sysinfo info;
    
    if (0 == sysinfo(&info))
    {
        return info.freeram;
    }
    */
    return 0;
}

// borrow from glib
// TODO test
unsigned int make_hash_code(const char *buffer, unsigned int len)
{
    assert(buffer);
    assert(len != 0);

    const char *p = buffer;
    unsigned int h = 0;

    while (len--)
    {
        h = (h << 5) - h + *p;
        p++;
    }

    return h;
}

const char* utf8_strichr(const char *str, int len, gunichar c)
{
    if (g_unichar_isalpha(c))
    {
        // Only alphabetic character is case related.
        c = g_unichar_toupper(c);
        for (const char* p=str; (len == -1 && *p != 0) || (p-str<len); p = g_utf8_next_char(p))
        {
            gunichar i = g_utf8_get_char(p);
            if (i == c || i-c == 32)
            {
                return p;
            }
        }
        return NULL;
    }
    else
    {
        return g_utf8_strchr(str, len, c);
    }
}

const char* utf8_strirchr(const char *str, int len, gunichar c)
{
    if (g_unichar_isalpha(c))
    {
        // Only alphabetic character is case related.
        c = g_unichar_toupper(c);
        const char *p = str + len;
        while (true)
        {
            // Try previous character.
            p = g_utf8_find_prev_char(str, p);
            if (p == NULL)
            {
                return NULL;
            }
            
            gunichar i = g_utf8_get_char(p);
            if (i == c || i-c == 32)
            {
                return p;
            }
        }
    }
    else
    {
        return g_utf8_strrchr(str, len, c);
    }
}

const char* utf8_strstr(const char *str, const char *sub, bool case_sensitive)
{
    if (case_sensitive)
    {
        return strstr(str, sub);
    }
    else
    {
        // Ignore case.
        gunichar first_char = g_utf8_get_char(sub);
        size_t sub_len = strlen(sub);

        for (const char *p=str; *p != 0;)
        {
            const char *find = utf8_strichr(p, -1, first_char);
            if (find != NULL)
            {
#ifdef WIN32
                if (_strnicmp(find, sub, sub_len) == 0)
#else
                if (strncasecmp(find, sub, sub_len) == 0)
#endif
                {
                    return find;
                }
                else
                {
                    p = g_utf8_next_char(find);
                }
            }
            else
            {
                return NULL;
            }
        }

        return NULL;
    }
}

const char* utf8_strrstr(const char *str, int len, const char *sub, bool case_sensitive)
{
    gunichar first_char = g_utf8_get_char(sub);
    size_t sub_len = strlen(sub);

    while (len > 0)
    {
        const char *p = NULL;
        if (case_sensitive)
        {
            p = g_utf8_strrchr(str, len, first_char);
        }
        else
        {
            // Ignore case.
            p = utf8_strirchr(str, len, first_char);
        }

        if (p == NULL)
        {
            return NULL;
        }

        // The first char of sub string is found.
        int ret;
        if (case_sensitive)
        {
            ret = strncmp(p, sub, sub_len);
        }
        else
        {
#ifdef WIN32
            ret = _strnicmp(p, sub, sub_len);
#else
            ret = strncasecmp(p, sub, sub_len);
#endif
        }
        
        if (ret == 0)
        {
            // Pattern found
            return p;
        }
        len = static_cast<int>(p - str);
    }

    return NULL;
}

bool is_whole_word(const char *str, const char *sub, size_t sub_len)
{
    const char *p = g_utf8_find_prev_char(str, sub);
    const char *n = sub + sub_len;

    gunichar prev_char = g_utf8_get_char(p);
    gunichar next_char = g_utf8_get_char(n);

    if (g_unichar_isalpha(prev_char) || g_unichar_isdigit(prev_char) ||
        g_unichar_isalpha(next_char) || g_unichar_isdigit(next_char) ||
        prev_char == '_' || next_char == '_')
    {
        return false;
    }

    return true;
}

bool get_dir_path(const char * filename, char * dir, int dir_len)
{
    bool has_dir = false;

    if (filename && dir)
    {
       char * ptr = g_path_get_dirname(filename);
       if (ptr)
       {
           has_dir = true;
           
           g_snprintf(dir, dir_len, "%s", ptr);
           g_free(ptr);
       }
    }

    LOGPRINTF("%s %s %d", filename, dir, has_dir);

    return has_dir;
}

bool get_file_name(const char * filename, char *name, int name_len)
{
    bool has_name = false;

    if (filename && name)
    {
        if (filename[strlen(filename)-1] != G_DIR_SEPARATOR)
        {
            char * ptr = g_path_get_basename(filename);
            if (ptr)
            {
                if (strcmp(ptr, G_DIR_SEPARATOR_S)
                   && strcmp(ptr, "."))
                {
                    has_name = true;
                    g_snprintf(name, name_len, "%s", ptr);
                }
                g_free(ptr);
            }
        }
    }

    LOGPRINTF("%s %s %d", filename, name, has_name);

    return has_name;
}

bool get_ext_name(const char * filename, char * ext, int ext_len)
{
    bool has_ext = false;
  
    if (filename && ext)
    {
        char name[MAX_PATH_LEN];
        
        if (get_file_name(filename, name, MAX_PATH_LEN))
        {
            char * ptr = g_strrstr(name, ".");
            if (ptr)
            {
                has_ext = true;

                ptr++;
                g_snprintf(ext, ext_len, "%s", ptr);
            }
        }
    }

    LOGPRINTF("%s %s %d", filename, ext, has_ext);

    return has_ext;
}

static iconv_t conv = (iconv_t) -1;
bool ucs2utf8(char* ucs_str, size_t input_len, char* utf_str, size_t output_len)
{
    if (conv == (iconv_t) -1)
    {
        conv = iconv_open("UTF-8", "Unicode");
    }

    assert(conv != (iconv_t) -1);

    char* in_buf  = ucs_str;
    char* out_buf = utf_str;
    size_t in_bytes_left  = input_len;
    size_t out_bytes_left = output_len;

#ifdef WIN32
    if (-1 == iconv(conv, const_cast<const char **>(&in_buf), &in_bytes_left, &out_buf, &out_bytes_left))
#else
    if ((size_t)-1 == iconv(conv, &in_buf, &in_bytes_left, &out_buf, &out_bytes_left))
#endif
    {
        return false;
    }

    if (in_bytes_left != 0)
    {
        // Either we encountered an error, or the output buffer is too small.
        return false;
    }

    // Add '\0' terminator.
    *out_buf = 0;
    return true;
}

