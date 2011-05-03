/*
 * File Name: string_impl.cpp
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

#include <cassert>
#include "string_impl.h"

namespace pdf
{

StringImpl::StringImpl()
{
    assign = assign_impl;
    get_buffer = get_buffer_impl;
    size = size_impl;
}

StringImpl::StringImpl(const char *str)
: impl(str)
{
    assign = assign_impl;
    get_buffer = get_buffer_impl;
    size = size_impl;
}

StringImpl::StringImpl(const std::string& str)
: impl(str)
{
    assign = assign_impl;
    get_buffer = get_buffer_impl;
    size = size_impl;
}

StringImpl::StringImpl(const StringImpl &src)
: impl(src.impl)
{
    assign = assign_impl;
    get_buffer = get_buffer_impl;
    size = size_impl;
}

StringImpl::~StringImpl()
{
}

StringImpl& StringImpl::operator = (const StringImpl &right)
{
    if (this != &right)
    {
        impl = right.impl;
    }
    return *this;
}

bool StringImpl::operator == (const StringImpl &right)
{
    return (impl == right.impl);
}

StringImpl * StringImpl::down_cast(_UDSString * ptr)
{
    return static_cast<StringImpl *>(ptr);
}

const StringImpl * StringImpl::down_cast(const _UDSString * ptr)
{
    return static_cast<const StringImpl *>(ptr);
}

_UDSString * StringImpl::assign_impl(_UDSString * thiz, 
                                     const char * src)
{
    if (thiz == 0)
    {
        return 0;
    }

    StringImpl * object = down_cast(thiz);
    object->impl = src;
    return thiz;
}

const char* StringImpl::get_buffer_impl(const _UDSString *thiz)
{
    if (thiz == 0)
    {
        return 0;
    }

    const StringImpl * object = down_cast(thiz);
    return object->impl.c_str();
}

unsigned int StringImpl::size_impl(const _UDSString   *thiz)
{
    if (thiz == 0)
    {
        return 0;
    }

    const StringImpl * object = down_cast(thiz);
    return static_cast<unsigned int>(object->impl.size());
}

};  // namespace pdf


