/*
 * File Name: ipc.c
 */

/*
 * This file is part of hello-world.
 *
 * hello-world is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * hello-world is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * Copyright (C) 2009 iRex Technologies B.V.
 * All rights reserved.
 */

//----------------------------------------------------------------------------
// Include Files
//----------------------------------------------------------------------------

#include "config.h"

// system include files, between < >
#include <stdio.h>
#include <stdlib.h>
#include <gdk/gdkx.h>
#include <sys/types.h>
#include <unistd.h>

// ereader include files, between < >
#include <liberipc/eripc.h>
#include <liberipc/eripc_support.h>

// local include files, between " "
#include "common/log.h"
// #include "i18n.h"
#include "ipc.h"
// #include "main.h"
#include "menu.h"


//----------------------------------------------------------------------------
// Type Declarations
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// Constants
//----------------------------------------------------------------------------

// IPC application
// TODO: set DBUS_APPL_NAME to your application name, use lower-case and digits only
//       or set to PACKAGENAME when this is lower-case and digits only
#define DBUS_APPL_NAME                  "helloworld"            // lower-case and digits only
#define DBUS_SERVICE                     "com.irexnet." DBUS_APPL_NAME
#define DBUS_PATH                       "/com/irexnet/" DBUS_APPL_NAME
#define DBUS_INTERFACE                   "com.irexnet." DBUS_APPL_NAME

// IPC system control
#define DBUS_SERVICE_SYSTEM_CONTROL     "com.irexnet.sysd"

// IPC popup menu
#define DBUS_SERVICE_POPUP_MENU         "com.irexnet.popupmenu"


//----------------------------------------------------------------------------
// Static Variables
//----------------------------------------------------------------------------

static eripc_client_context_t *eripcClient = NULL;

//============================================================================
// Local Function Definitions
//============================================================================

static void on_menu_item         ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_mounted           ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_file_open         ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_file_close        ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_window_activated  ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_window_deactivated( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_prepare_unmount   ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );
                                 
static void on_unmounted         ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );

static void on_prepare_standby   ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );

static void on_changed_locale    ( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );

static void on_changed_orientation(eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data );

static void on_page_change               (eripc_context_t *context,
                                          const eripc_event_info_t *info,
                                          void *user_data);

static void on_sys_changed_pageturn_inverted (eripc_context_t *context,
                                          const eripc_event_info_t *info,
                                          void *user_data);


// Exported DBUS API list
static eripc_callback_function_t service_functions[] = {
    // message handlers (method calls to this service)
    { on_menu_item,           "menuItemActivated",      NULL                        },
    { on_mounted,             "sysVolumeMounted",       NULL                        },
    { on_file_open,           "openFile",               NULL                        },
    { on_file_close,          "closeFile",              NULL                        },
    { on_window_activated,    "activatedWindow",        NULL                        },
    { on_window_deactivated,  "deactivatedWindow",      NULL                        },
    // signal handlers (broadcasted from given service)
    { on_mounted,             "sysVolumeMounted",       DBUS_SERVICE_SYSTEM_CONTROL },
    { on_prepare_unmount,     "sysPrepareUnmount",      DBUS_SERVICE_SYSTEM_CONTROL },
    { on_unmounted,           "sysVolumeUnmounted",     DBUS_SERVICE_SYSTEM_CONTROL },
    { on_prepare_standby,     "sysPrepareStandby",      DBUS_SERVICE_SYSTEM_CONTROL },
    { on_changed_locale,      "sysChangedLocale",       DBUS_SERVICE_SYSTEM_CONTROL },
    { on_changed_orientation, "sysChangedOrientation",  DBUS_SERVICE_SYSTEM_CONTROL },
    { on_page_change,         "pageChange",             NULL},
    { on_sys_changed_pageturn_inverted, "sysChangedPageturnInverted", DBUS_SERVICE_SYSTEM_CONTROL},
    { NULL }
};


//============================================================================
// Functions Implementation
//============================================================================


//----------------------------------------------------------------------------
// Generic
//----------------------------------------------------------------------------

// initialise
void ipc_set_services (void)
{
    eripcClient = eripc_client_context_new(
                    DBUS_APPL_NAME, 
                    "1.0",
                    DBUS_SERVICE, 
                    DBUS_PATH,
                    DBUS_INTERFACE,
                    service_functions);
}


// un-initialise
void ipc_unset_services (void)
{
    eripc_client_context_free(eripcClient, service_functions);
}


//----------------------------------------------------------------------------
// System control
//----------------------------------------------------------------------------

// report "application started"
void ipc_sys_startup_complete ( void )
{
//    const int xid = GDK_WINDOW_XID(g_main_window->window);
//    eripc_sysd_startup_complete( eripcClient, getpid(), TRUE, xid);
}


// start task (application)
gint ipc_sys_start_task ( const gchar  *cmd_line,
                          const gchar  *work_dir,
                          const gchar  *label,
                          const gchar  *thumbnail_path,
                          gchar        **err_message )
{
    LOGPRINTF("entry: cmd_line [%s] work_dir [%s] label [%s] thumbnail_path [%s]",
                       cmd_line,     work_dir,     label,     thumbnail_path       );
    g_assert( cmd_line && *cmd_line );
    return eripc_sysd_start_task( eripcClient, 
                                  cmd_line, 
                                  work_dir, 
                                  label, 
                                  thumbnail_path, 
                                  err_message);
}


// stop task (application)
gboolean ipc_sys_stop_task ( const gchar  *cmd_line )
{
    LOGPRINTF("entry: cmd_line [%s]", cmd_line );
    g_assert( cmd_line && *cmd_line );
    return eripc_sysd_stop_task( eripcClient, cmd_line );
}


//----------------------------------------------------------------------------
// Popup menu
//----------------------------------------------------------------------------

// add a menu set
gboolean ipc_menu_add_menu( const char *name,
                            const char *group1,
                            const char *group2,
                            const char *group3 )
{
    return eripc_menu_add_menu(eripcClient, name, group1, group2, group3, "");
}


// add a menu group
gboolean ipc_menu_add_group( const char *name,
                             const char *parent, 
                             const char *image )
{
    return eripc_menu_add_group(eripcClient, name, parent, image);
}


// add a menu item
gboolean ipc_menu_add_item( const char *name,
                            const char *parent, 
                            const char *image  )
{
    return eripc_menu_add_item(eripcClient, name, parent, image);
}


// set text for a menu set
gboolean ipc_menu_set_menu_label ( const char *name,
                                   const char *label )
{
    return eripc_menu_set_menu_label(eripcClient, name, label);
}


// set text for a menu group
gboolean ipc_menu_set_group_label ( const char *name,
                                    const char *label )
{
    return eripc_menu_set_group_label(eripcClient, name, label);
}


// set text for an menu item
gboolean ipc_menu_set_item_label ( const char *name,
                                   const char *parent, 
                                   const char *label )
{
    return eripc_menu_set_item_label(eripcClient, name, parent, label);
}


// show the given menu set
gboolean ipc_menu_show_menu( const char *name )
{
    return eripc_menu_show_menu(eripcClient, name);
}


// remove the given menu set
gboolean ipc_remove_menu( const char *name )
{
    return eripc_menu_remove_menu(eripcClient, name);
}


// set the state of a menu group
gboolean ipc_menu_set_group_state( const char *name,
                                   const char *state )
{
    return eripc_menu_set_group_state(eripcClient, name, state);
}


// set the state of a menu item
gboolean ipc_menu_set_item_state( const char *name,
                                  const char *parent,
                                  const char *state  )
{
    return eripc_menu_set_item_state(eripcClient, name, parent, state);
}


// set busy indication
gboolean ipc_sys_busy( gboolean look_busy )
{
    if (look_busy)
        return eripc_sysd_set_busy(eripcClient, "delaydialog", NULL);
    else
        return eripc_sysd_reset_busy(eripcClient);
}


//============================================================================
// Local Function Implementation
//============================================================================

//----------------------------------------------------------------------------
// Signal/message handlers
//----------------------------------------------------------------------------

/* @brief Called when a menu items is activated in Popup menu
 *
 * Application (callee) should handle the item depending on the current state.
 */
static void on_menu_item ( eripc_context_t          *context,
                           const eripc_event_info_t *info,
                           void                     *user_data )
{
    LOGPRINTF("entry");
    const eripc_arg_t *arg_array = info->args;

    if ((arg_array[0].type == ERIPC_TYPE_STRING) && 
        (arg_array[1].type == ERIPC_TYPE_STRING) && 
        (arg_array[2].type == ERIPC_TYPE_STRING) && 
        (arg_array[3].type == ERIPC_TYPE_STRING))
    {
        const char        *item      = arg_array[0].value.s;
        const char        *group     = arg_array[1].value.s;
        const char        *menu      = arg_array[2].value.s;
        const char        *state     = arg_array[3].value.s;
        
        if (item && group && menu && state)
        {
//            menu_on_item_activated( item, group, menu, state );
        }
    }
}


/* @brief Called after a window was activated (set to the foreground)
 *
 * Application (callee) should set its context for the given window and set the 
 * Popupmenu menu context.
 */
static void on_window_activated( eripc_context_t          *context,
                                 const eripc_event_info_t *info,
                                 void                     *user_data )
{
    LOGPRINTF("entry");
//    gchar             *msg        = NULL;
    gboolean          result      = FALSE; 
    const eripc_arg_t *arg_array  = info->args;

    if (arg_array[0].type == ERIPC_TYPE_INT)
    {
        // TODO: Replace implementation

//        gint window = arg_array[0].value.i;
/*        
        menu_show();

        msg = g_strdup_printf("Window activated: %d", window);
        gtk_label_set_text( GTK_LABEL(g_action), msg);
        LOGPRINTF("%s", msg);
        g_free(msg);
        */
        result = TRUE;
    }

    // return result to caller
    eripc_reply_bool(context, info->message_id, result);
}


/* @brief Called after a window was deactivated (set to the background)
 *
 * Application (callee) may adapt its context and free resources.
 */  
static void on_window_deactivated( eripc_context_t          *context,
                                   const eripc_event_info_t *info,
                                   void                     *user_data )
{
    LOGPRINTF("entry");
//    gchar             *msg        = NULL;
    gboolean          result      = FALSE; 
    const eripc_arg_t *arg_array  = info->args;

    if (arg_array[0].type == ERIPC_TYPE_INT)
    {
        // TODO: Replace implementation

        gint window = arg_array[0].value.i;
/*        
        msg = g_strdup_printf("Window deactivated: %d", window);
        gtk_label_set_text( GTK_LABEL(g_action), msg);
        LOGPRINTF("%s", msg);
        g_free(msg);
        */
        result = TRUE;
    }
    
    // return result to caller
    eripc_reply_bool(context, info->message_id, result);

    // and close application
    // main_quit();
}


/* @brief Called when a file, document or url is to be opened by the application
 *
 * Application (callee) should create and realise, or reuse an existing window 
 * for the given file and return the X window id in the method reply. The X window 
 * can be obtained using GDK_WINDOW_XID(widget->window). When the file is already 
 * opened by the callee, it may just return its X window id. This call implies that
 * the window is activated (set to the foreground) so callee should also set its 
 * context for the given window and set the Popupmenu menu context. 
 * System Daemon adds a task to Task Manager of Popupmenu, or replaces the task 
 * when an existing window is returned.
 */
static void on_file_open ( eripc_context_t          *context,
                           const eripc_event_info_t *info,
                           void                     *user_data )
{
    LOGPRINTF("entry");
    gchar             *msg        = NULL;
    gchar             *error_msg  = NULL;
    gint              my_xid      = -1; 

    const eripc_arg_t *arg_array  = info->args;
    
    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char *file = arg_array[0].value.s;
        if (file)
        {
            // TODO: Replace implementation
/*
            msg = g_strdup_printf("File to open: %s", file);
            gtk_label_set_text( GTK_LABEL(g_action), msg);
            LOGPRINTF("%s", msg);
            g_free(msg);
            
            // TODO: Replace implementation

            gboolean success = TRUE;
            
            if (success) 
            {
                my_xid = GDK_WINDOW_XID(g_main_window->window);
                menu_show();
            }
            else
            {
                error_msg = g_strdup_printf(_("Error opening '%s'"), file);
            }
*/
        }
    }
    
    // return result to caller
    eripc_reply_varargs(context, info->message_id, 
                        ERIPC_TYPE_INT, my_xid,
                        ERIPC_TYPE_STRING, error_msg,
                        ERIPC_TYPE_INVALID);
    g_free(error_msg);
}


/* @brief Called when a file, document or url is to be closed by the application
 *
 * Application (callee) should close the file and may destroy its window and free 
 * other resources. System Daemon removes the task from the Task Manager of Popupmenu.
 */
static void on_file_close ( eripc_context_t          *context,
                            const eripc_event_info_t *info,
                            void                     *user_data )
{
    LOGPRINTF("entry");
    gchar             *msg        = NULL;
    gboolean          result      = FALSE; 
    const eripc_arg_t *arg_array  = info->args;
    
    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char *file = arg_array[0].value.s;
        if (file)
        {
            // TODO: Replace implementation
/*
            msg = g_strdup_printf("File to close: %s", file);
            gtk_label_set_text( GTK_LABEL(g_action), msg);
            LOGPRINTF("%s", msg);
            g_free(msg);
*/
            result = TRUE;
        }
    }
    
    // return result to caller
    eripc_reply_bool(context, info->message_id, result);
}


/* @brief Called just after a volume is mounted
 *
 * Application may use this to add/open the new volume.
 */
static void on_mounted ( eripc_context_t          *context,
                         const eripc_event_info_t *info,
                         void                     *user_data )
{
    LOGPRINTF("entry");
    gchar             *msg        = NULL;
    const eripc_arg_t *arg_array  = info->args;

    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char *mountpoint = arg_array[0].value.s;
        if (mountpoint)
        {
            // TODO: Replace implementation
/*            
            g_free(g_mountpoint);
            g_mountpoint = g_strdup(mountpoint);
            
            msg = g_strdup_printf("Device mounted: %s", mountpoint);
            gtk_label_set_text( GTK_LABEL(g_volume), msg);
            LOGPRINTF("%s", msg);
            g_free(msg);
*/
        }
    }
}


/* @brief Called just before unmounting the volume
 *
 * Application must close all its open files on the given volume. Failing to 
 * this signal may result in unsaved data or currupt files.
 */
static void on_prepare_unmount ( eripc_context_t          *context,
                                 const eripc_event_info_t *info,
                                 void                     *user_data )
{
    LOGPRINTF("entry");
//    gchar             *msg        = NULL;
    const eripc_arg_t *arg_array  = info->args;

    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char *mountpoint = arg_array[0].value.s;
        if (mountpoint)
        {
            // TODO: Replace implementation
/*
            msg = g_strdup_printf("Device to unmount: %s", mountpoint);
            gtk_label_set_text( GTK_LABEL(g_volume), msg);
            LOGPRINTF("%s", msg);
            g_free(msg);
*/
        }
    }
}


/* @brief Called just after unmounting the volume
 *
 * Typically an application should have responded to a prior sysPrepareUnmount 
 * signal, but when a device with volumes was removed unexpectedly it may need 
 * to act on this signal.
 */  
static void on_unmounted ( eripc_context_t          *context,
                           const eripc_event_info_t *info,
                           void                     *user_data )
{
    LOGPRINTF("entry");
 //   gchar             *msg        = NULL;
    const eripc_arg_t *arg_array  = info->args;

    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char  *mountpoint = arg_array[0].value.s;
        if (mountpoint)
        {
            // TODO: Replace implementation
/*            
            if (g_mountpoint && (strcmp(mountpoint, g_mountpoint) == 0))
            {
                g_free(g_mountpoint);
                g_mountpoint = NULL;
            }
            
            msg = g_strdup_printf("Device unmounted: %s", mountpoint);
            gtk_label_set_text( GTK_LABEL(g_volume), msg);
            LOGPRINTF("%s", msg);
            g_free(msg);
*/
        }
    }
}


/* @brief Called just before the system enters standby mode
 * 
 * Application must commit changes (flush) of all its open files. Failing to 
 * handle this signal may result in unsaved data or currupt files.
 */
static void on_prepare_standby ( eripc_context_t          *context,
                                 const eripc_event_info_t *info,
                                 void                     *user_data )
{
    LOGPRINTF("entry");

    // TODO: Add implementation here
}


/* @brief Called when the system's locale has changed
 *
 * Application should load language dependent screen texts and probably set new 
 * labels for its menu items; to activate a new locale application should call:
 *             g_setenv("LANG", new_locale, TRUE);
 *             setlocale(LC_ALL, "");
 */
static void on_changed_locale ( eripc_context_t          *context,
                                const eripc_event_info_t *info,
                                void                     *user_data )
{
    LOGPRINTF("entry");
    const eripc_arg_t *arg_array = info->args;

    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char *locale = arg_array[0].value.s;
        if (locale)
        {
            const char *old_locale = g_getenv("LANG");
            if (!old_locale || (strcmp(old_locale, locale) != 0))
            {
                // main_set_locale(locale);
            }
        }
    }
}


/* @brief Called when the display's orientation has changed
 *
 * Application may need to adapt its screen size, coordinates and/or origin.
 * Possible values: "portrait", "landscape_clockwise", "landscape_anticlockwise"
 */
static void on_changed_orientation ( eripc_context_t          *context,
                                     const eripc_event_info_t *info,
                                     void                     *user_data )
{
    LOGPRINTF("entry");
 //   gchar             *msg        = NULL;
    const eripc_arg_t *arg_array = info->args;
    
    if (arg_array[0].type == ERIPC_TYPE_STRING)
    {
        const char *orientation = arg_array[0].value.s;
        if (orientation)
        {
            // TODO: Replace implementation
/*            
            msg = g_strdup_printf("Orientation changed: %s", orientation);
            gtk_label_set_text( GTK_LABEL(g_action), msg);
            LOGPRINTF("%s", msg);
            g_free(msg);
*/
        }
    }
}

//---------------------
// new ipc callbacks
//---------------------


static void on_sys_changed_pageturn_inverted(  eripc_context_t *context, 
                                            const eripc_event_info_t *info, 
                                            void *user_data)
{
    LOGPRINTF("entry");

 //   gchar             *msg        = NULL;
    const eripc_arg_t *arg_array = info->args;

    if (arg_array[0].type == ERIPC_TYPE_BOOL)
    {
        gboolean is_inverted = arg_array[0].value.b;

        // TODO: Replace implementation
/*        
		msg = g_strdup_printf("page turning setting from from flipbar change, is_inverted = %d", is_inverted);
        gtk_label_set_text( GTK_LABEL(g_action), msg);
        LOGPRINTF("%s", msg);
        g_free(msg);
*/
    }

}


static void on_page_change( eripc_context_t *context, 
                            const eripc_event_info_t *info, 
                            void *user_data)
{
    LOGPRINTF("entry");

//    gchar             *msg        = NULL;
    const eripc_arg_t *arg_array = info->args;

    g_return_if_fail(arg_array[0].type == ERIPC_TYPE_STRING);

    const gchar* direction = arg_array[0].value.s;

    if (direction)
    {
        // TODO: Replace implementation
/*
        msg = g_strdup_printf("page change from menubar, direction:  %s", direction);
        gtk_label_set_text( GTK_LABEL(g_action), msg);
        LOGPRINTF("%s", msg);
        g_free(msg);
*/
    }

}



//--------------------------------------------------
// Queries over ipc
//-------------------------------------------------

// the following functions queries sysd about the
// custom setting of the flipbar.

// Function: ipc_sys_is_pageturn_inverted()
//
// Return value: False -> pageturning is not inverted i.e. left to right gives next page
//               True  -> pageturning is     inverted i.e. left to right gives previous page
// Intended use: during application initialisation and only useful in portrait reading mode

gboolean ipc_sys_is_pageturn_inverted()
{
    gboolean result = FALSE;

    eripc_error_t retval;
    eripc_event_info_t* info = NULL;

    LOGPRINTF("entry");

    retval = eripc_send_varargs_and_wait(eripcClient->context,
            &info,
            ERIPC_BUS_SESSION,
            DBUS_SERVICE_SYSTEM_CONTROL,
            "sysGetPageturnInverted",
            ERIPC_TYPE_INVALID);

    if (retval != ERIPC_ERROR_SUCCESS)
    {
        ERRORPRINTF("Error launching eripc handler");
    }
    else if (info == NULL || info->args == NULL)
    {
        ERRORPRINTF("sysd returns OK but no reply structure");
    }
    else
    {
        const eripc_arg_t *arg_array = info->args;

        if (arg_array[0].type == ERIPC_TYPE_BOOL)
        {
            result = (gboolean) arg_array[0].value.b;
        }
        else 
        {
            result = FALSE ;
        }
    }

    eripc_event_info_free(eripcClient->context, info);
    return result; 
}



// Function: ipc_sys_is_in_portrait_mode()
//
// Return value: False -> the device is in landscape mode i.e. flipbar up should be interpreted as previous page.
//               True  ->  the device is in portrait mode i.e. flipbar interpretation should depend on sys_is_pageturn_inverted result.
// Intended use: during application initialisation

gboolean ipc_sys_is_in_portrait_mode(void)
{

    gboolean result = TRUE;

    eripc_error_t retval;
    eripc_event_info_t* info = NULL;

    LOGPRINTF("entry");

    retval = eripc_send_varargs_and_wait(eripcClient->context,
            &info,
            ERIPC_BUS_SESSION,
            DBUS_SERVICE_SYSTEM_CONTROL,
            "sysGetOrientation",
            ERIPC_TYPE_INVALID);

    if (retval != ERIPC_ERROR_SUCCESS)
    {
        ERRORPRINTF("Error launching eripc handler");
    }
    else if (info == NULL || info->args == NULL)
    {
        ERRORPRINTF("sysd returns OK but no reply structure");
    }
    else
    {
        const eripc_arg_t *arg_array = info->args;

        if (arg_array[0].type == ERIPC_TYPE_STRING)
        {
            if ( strcmp("portrait", arg_array[0].value.s) == 0 )
            {
                result = TRUE;
            }
            else
            {
                result = FALSE;
            }
        }
        else
        {
            result = FALSE ;
        }
    }
    eripc_event_info_free(eripcClient->context, info);
    return result;
}


