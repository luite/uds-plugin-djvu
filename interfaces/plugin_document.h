/*
 * File Name: plugin_document.h
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

#ifndef PLUGIN_DOCUMENT_H_
#define PLUGIN_DOCUMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "plugin_type.h"
#include "plugin_unknown.h"

/**
 * @brief Universal Document Shell Plugin Document interface.
 * Through IPluginDocument, caller can open and close a specified document,
 * estimate whether an interface pointer delegates an opened document object,
 * and create a new view object.
 */
typedef struct 
{

    /**
     * @brief Open specified document. 
     * @param thiz IPluginUnknown pointer of document object.
     * @param path The path of the document.
     * @return TODO. Add return code here.
     */
    PluginStatus (* open )( IPluginUnknown    *thiz, 
                            const UDSString   *path );
                            

    /**
     * @brief Supply extra credentials for opening the document
     *
     * This should be called after opening the document, because in most cases only then it is possible
     * to determine if a document needs extra credentials (eg. name/password combination) for opening.
     * After adding the credentials commit_credentials() should be called to notify the plugin that
     * the credentials are complete. 
     *
     * Please note: Always call either commit_credentials() or reset_credentials() to ensure credential
     * information is removed from memory.
     *
     * @param thiz IPluginUnknown pointer of document object.
     * @param string name Name of the credentials (eg. "password" or "name"), depending on plugin implementation
     * @param string value Value of the credential depending on plugin implementation
     * @return PLUGIN_OK Supplied credential name/value is correct
     * @return PLUGIN_FAIL Supplied credential name/value is not correct, programming error
     * @return PLUGIN_NOT_SUPPORTED Plugin does not support credentials
     */
    PluginStatus (* add_credentials )( IPluginUnknown    *thiz, 
                                       const UDSString   *name,
                                       const UDSString   *value );


    /**
     * @brief Notifies plugin to process the credential information
     *
     *
     * This is called by UDS to notify that the credential information is complete.
     * The plugin now has to store the credential information and verify it if possible.
     * If the credential information cannot be verified without reopening the document,
     * the plugin should return PLUGIN_CREDENTIAL_AUTHORIZATION_PENDING. UDS then will
     * reopen the document.
     *
     * Please note: If the commit fails or if the authorization failed, reset_credentials has to
     * be called!
     *
     * @return PLUGIN_OK Credential information is verified and OK
     * @return PLUGIN_CREDENTIAL_AUTHORIZATION_FAILED Credential information is verified but does not match
     * @return PLUGIN_CREDENTIAL_AUTHORIZATION_PENDING Credential could not be verified, UDS has to reopen the document
     * @return PLUGIN_FAIL Credential information supplied with add_credentials() is not complete
     * @return PLUGIN_NOT_SUPPORTED Plugin does not support credentials
     */
    PluginStatus (* commit_credentials )( IPluginUnknown    *thiz );
    
    /**
     * @brief Notifies plugin to remove all credential information
     *
     * This function will be called from UDS in these situations:
     * <li> If for some reason commit_credentials() was not called and the credential procedure was
     *       aborted. This ensures that credential information() is removed from memory.
     * <li> If commit_credentials() was called but returned an error
     * <li> If commit_credentials() was called but the authorization failed (either reported directly
     *      as PLUGIN_CREDENTIAL_AUTHORIZATION_FAILED from commit_credentials() or detected later
     *      when reopening the document)
     *
     * @return PLUGIN_OK Credential information is reset
     * @return PLUGIN_FAIL Credential information could not be reset
     * @return PLUGIN_NOT_SUPPORTED Plugin does not support resetting of credentials
     */
    PluginStatus (* reset_credentials )( IPluginUnknown    *thiz );

    /**
     * @brief Check whether the current document object has been opened or not.
     * @param thiz The IPluginUnknown pointer of document object.
     * @return Returns PLUGIN_TRUE if the document has been successfully
     * opened, otherwise returns PLUGIN_FALSE.
     */
    PluginBool (* is_open )( IPluginUnknown    *thiz );

    /**
     * @brief Close document. This document object won't be destroyed
     * immediately after calling this function. The object can be used to
     * open another document.
     * @param thiz The IPluginUnknown pointer of document object to be closed.
     * @return TODO. Add return code here.
     */
    PluginStatus (* close )( IPluginUnknown     *thiz );

    /**
     * @brief Create view object.
     * @param thiz IPluginUnknown pinter of document object.
     * @return Returns the IPluginUnknown pointer of new created view object. 
     * If the view object is failed to be created, this function returns NULL.
     */
    IPluginUnknown * (* create_view )( IPluginUnknown   *thiz );

} IPluginDocument;

#ifdef __cplusplus
}
#endif 

#endif

