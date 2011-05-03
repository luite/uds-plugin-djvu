/*
 * File Name: plugin_event.h
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

#ifndef PLUGIN_EVENT_H_
#define PLUGIN_EVENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Type definition of plugin events.
 */
typedef enum
{
    EVENT_PAGINATE_START,             /**< Pagination starting Event */
    EVENT_PAGINATE_END,               /**< Pagination ending Event */
    EVENT_RENDERING_END,              /**< Rendering ending Event */
    EVENT_MARKER_READY,               /**< Marker(tree) ready Event */
    EVENT_MARKER_OUT_OF_DATE,         /**< Marker(tree) Out-Of-Date Event */
    EVENT_SEARCH_END,                 /**< Search ending Event */
    EVENT_SEARCH_ABORTED,             /**< Search aborted Event */
    EVENT_PRERENDERING_START,         /**< Pre rendering started Event */
    EVENT_PRERENDERING_END            /**< Pre rendering ended Event */
} PluginEvent;

/**
 * @brief Define render status here.
 */
typedef enum
{
    RENDER_PENDING = -1,                /**< Job pending      */
    RENDER_DONE,                        /**< Render finished. */
    RENDER_OUT_OF_MEMORY,               /**< Out of memory.   */
    RENDER_INVALID_PAGE,                /**< Invalid page.    */
    RENDER_FAIL,                        /**< Render fails.    */
    RENDER_BEYOND_LAST_PAGE,            /**< Requested page is after the document's end */
    RENDER_BEYOND_FIRST_PAGE            /**< Requested page is before the document's start */
} PluginRenderStatus;

/**
 * @brief Pagination event, could be triggered when start and finished.
 */
typedef struct
{
    int current;    /**< The number of the latest paginated page */
    int total;      /**< The number of total pages that should be paginated */
} EventParmsPaginate_t;

/**
 * @brief Render event.
 */
typedef struct 
{
    unsigned long       rid;        /**< The reference ID of the rendered result */
    PluginRenderStatus  status;     /**< The status of render result. */
    IPluginUnknown*     result;     /**< The IPluginUnknown pointer of render result object */
                                    /**< depending on the render status:                    */
                                    /**<   RENDER_DONE              rendered page           */
                                    /**<   RENDER_BEYOND_LAST_PAGE  last page in document   */
                                    /**<   RENDER_BEYOND_FIRST_PAGE first page in document  */
                                    /**<   other                    NULL pointer            */
} EventRenderEnd_t;

/**
 * @brief Marker(tree) generation done event.
 */
typedef struct 
{
    IPluginUnknown* result; /**< The IPluginUnknown pointer of the collection 
                                 which contains the requested marker(tree) */
} EventMarkerReady_t;

/**
 * @brief Search finishing event.
 */
typedef struct 
{
    unsigned long   search_id;  /**< ID of the search request*/
    IPluginUnknown* result;     /**< The IPluginUnknown pointer of the collection 
                                     which contains the requested search results. */
} EventSearchEnd_t;


/**
 * @brief Put all above events into together.
 */
typedef union 
{
    EventParmsPaginate_t    paginate;       /**< Parameter of pagination  */
    EventRenderEnd_t        render_end;     /**< Parameter of rendering done */
    EventMarkerReady_t      marker_ready;   /**< Parameter of marker ready */
    EventSearchEnd_t        search_end;     /**< Parameter of search done */
} PluginEventAttrs;

/** 
 * @brief Define the callback function prototype.
 * @param thiz_sender The IPluginUnknown pointer of the object which sends 
 * the event.
 * @param handler_id Because UDS can register same event with same function 
    pointer twice, we use handler_id to distinguish them.
 * @param plugin_event The event which is sent by thiz_sender.
 * @param user_data User specified data.
 * @param plugin_data The data coming from plugin library. It would be used 
 * as parameters when broadcasting the event.
 */
typedef void (* EventFunc)( IPluginUnknown          *thiz_sender,
                            unsigned long           handler_id,
                            const PluginEvent       plugin_event,
                            void                    *user_data,
                            const PluginEventAttrs  *plugin_data );

/**
 * @brief Universal Document Shell Plugin Document Event Broadcaster interface.
 * Through IPluginEventBroadcaster, listeners can receive notification from 
 * plugin.
 */
typedef struct 
{
    /**
     * @brief Register call back function for specified event.
     * @param thiz The object identifier which implements the 
     * IPluginEventBroadcaster interface.
     * @param event The event on which the listener needs to register.
     * @param callback Callback function defined by caller.
     * @param user_data User specified data, plugin puts this variable 
     * to callback function as a parameter.
     * @param handler_id The identifier ID of this registration.
     * @return TODO. Add return code here.
     */
    PluginStatus (* add_event_receiver)( IPluginUnknown     *thiz,
                                         const PluginEvent  plugin_event,
                                         EventFunc          callback,
                                         void               *user_data,
                                         unsigned long      *handler_id);

    /**
     * @brief Remove a listener by specified ID.
     * @param thiz The object identifier which implements the 
     * IPluginEventBroadcaster interface.
     * @param handler_id The unique identifier of the registration, it is 
     * always generated by plugin while caller registers a callback 
     * function for specified event.
     * @return TODO. Add return code here.
     */
    PluginStatus (* remove_event_receiver)( IPluginUnknown  *thiz,
                                            unsigned long   handler_id );

} IPluginEventBroadcaster;

#ifdef __cplusplus
}
#endif 

#endif

