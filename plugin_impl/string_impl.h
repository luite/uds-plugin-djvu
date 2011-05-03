/*
 * File Name: string_impl.h
 */

/*
 * This file is part of uds-plugin-pdf.
 *
 * uds-plugin-pdf is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * uds-plugin-pdf is distributed in the hope that it will be useful,
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

#ifndef STRING_IMPL_H_
#define STRING_IMPL_H_

#include <string>
#include "plugin_inc.h"
#include "string_impl.h"

namespace pdf
{

class StringImpl : public _UDSString
{
public:
    StringImpl();
    explicit StringImpl(const char *str);
    explicit StringImpl(const std::string& str);
    explicit StringImpl(const StringImpl &src);
    ~StringImpl();

public:
    StringImpl& operator = (const StringImpl &right);
    bool operator == (const StringImpl &right);
    std::string& stl_string() { return impl; }

private:
    /// Implement all interfaces defined by UDSString
    static _UDSString * assign_impl(
        _UDSString * thiz, 
        const char * src);

    /// Implement the get_buffer.
    static const char* get_buffer_impl(
        const _UDSString *thiz);

    /// Size in bytes.
    static unsigned int size_impl(
        const _UDSString   *thiz);

private:
    /// Helper functions
    static StringImpl * down_cast(_UDSString * ptr);
    static const StringImpl * down_cast(const _UDSString * ptr);

private:
    std::string impl;
};

};  // namespace pdf


#endif

