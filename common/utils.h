/*
 * File Name: utils.h
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

#ifndef PLUGIN_UTILS_H
#define PLUGIN_UTILS_H

#include <glib.h>

#define MAX_PATH_LEN 1024

/// @brief Get free memory.
int get_free_memory();

/// @brief Generate hash code given by a buffe with fixed length.
unsigned int make_hash_code(const char *buffer, unsigned int len);

/// @brief Finds the leftmost occurrence of the given unicode character in a
///  UTF-8 encoded string, while limiting the search to len bytes. Ignore case.
const char* utf8_strichr(const char *str, int len, gunichar c);

/// @brief Finds the rightmost occurrence of the given unicode character in a
///  UTF-8 encoded string, while limiting the search to len bytes. Ignore case.
const char* utf8_strirchr(const char *str, int len, gunichar c);

/// @brief Finds the leftmost occurrence of substring in a string.
const char* utf8_strstr(const char *str, const char *sub, bool case_sensitive);

/// @brief Finds the rightmost occurrence of substring in a string.
const char* utf8_strrstr(const char *str, int len, const char *sub, bool case_sensitive);

/// @brief Check if the sub string is a whole word.
/// @param str The UTF-8 encoded string.
/// @param sub The sub string to be checked.
/// @param sub_len The length of the sub string.
/// @return true if sub is a whole word.
bool is_whole_word(const char *str, const char *sub, size_t sub_len);


/// @brief Get the dir path of 'filename'.
/// @param filename  e.g. /data/wa/sample/rome.jpg 
/// @param dir       dir path e.g. /data/wa/sample
/// @param dir_len   the max len of 'dir' 
/// @return          true has dir, false no dir or error
bool get_dir_path(const char * filename, char * dir, int dir_len);

/// @brief Get the name of 'filename'.
/// @param filename  e.g. /data/wa/sample/rome.jpg 
/// @param name      name e.g. rome.jpg
/// @param name_len  the max len of 'name' 
/// @return          true has name, false no name or error
bool get_file_name(const char * filename, char *name, int name_len);

/// @brief Get the extension name from a filename.
/// @param filename  e.g. /data/wa/sample/rome.jpg, or rome.jpg
/// @param ext       extension name e.g. jpg
/// @param ext_len   the max len of 'ext' 
/// @return          true has extension, false no extension or error
bool get_ext_name(const char * filename, char * ext, int ext_len);

/// @brief Convert the unicode string to utf-8 string.
/// @param ucs_str The input unicode string.
/// @param input_len The length of input buffer.
/// @param utf_str The output utf-8 string buffer.
/// @param output_len The buffer length of output buffer.
/// @return true if conversion succeed, otherwise false is returned.
bool ucs2utf8(char* ucs_str, size_t input_len, char* utf_str, size_t output_len);

#endif // PLUGIN_UTILS_H

