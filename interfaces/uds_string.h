/*
 * File Name: uds_string.h
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

#ifndef UDS_STRING_H_
#define UDS_STRING_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Universal Document Shell String object. 
 * The UDS string is implemented by UDS and is usually used by plugin.
 */
typedef struct _UDSString
{
    // Define a virtual desctuctor to avoid memory leaks in derived classes.
    // Note: UDSString is intended as a plain C struct,
    //       but uds and/or plugins use it as a C++ base class.
    //       This may cause memory leaks because the derived class destructor
    //       is not called when deleting the base type.
    virtual ~_UDSString() {}

    /**
     * @brief Assign the the source string to the UDS string object.
     * @param thiz The UDS string object.
     * @param src The source string.
     * @return This function returns the UDS string object equal to thiz.
     */
    struct _UDSString* (* assign)( struct _UDSString    *thiz,
                                   const char           *src );

    /**
     * @brief Get the string buffer inside the UDS string object, always used
     * by plugin to retrieve the native string.
     * @param thiz The UDS string object.
     * @return string buffer inside the UDS string object.
     */
    const char* (* get_buffer)( const struct _UDSString    *thiz );

    /**
     * @brief Get the length of the source string, always used by coping the UDS
     * string to the internal string in Plugin
     * @param thiz The UDS string object
     */
    unsigned int (* size)( const struct _UDSString   *thiz);

} UDSString;


#ifdef __cplusplus
}
#endif 

#endif

