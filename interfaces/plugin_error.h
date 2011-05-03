/*
 * File Name: plugin_error.h
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

#ifndef PLUGIN_ERROR_H_
#define PLUGIN_ERROR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"

/** 
 * @brief Definition of error code for specific functions
 */

/**
 * @brief PluginStatus Function return code definition
 */
#define GENERIC_STATUS_START            0
#define RENDER_SETTINGS_STATUS_START    30

/// @brief Plugin status
/// Please keep these entries as generic as possible, plugin specific names should be avoided.
/// https://tracker.irexnet.com/view.php?id=2913 describes future improvements for the error
/// handling.
/// Please note when adding entries, to also update ViewCtrl::get_error_message
/// @see ViewCtrl::get_error_message
typedef enum
{
    // Generic
    PLUGIN_OK                   = GENERIC_STATUS_START,
    PLUGIN_FAIL,
    PLUGIN_OUT_OF_BUFFER,
    PLUGIN_NOT_SUPPORTED,
    PLUGIN_ERROR_OPEN_FILE,
    PLUGIN_OUT_OF_MEMORY,
    PLUGIN_UNSUPPORTED_ENCODING,
    PLUGIN_ANCHOR_NOT_AVAILABLE,
    
    // Licensing problem
    PLUGIN_LIC_ERROR_OPENING,   // Document not licensed correctly or belongs to other user. Reported during opening of document.
    PLUGIN_LIC_NOT_ACTIVATED_ADOBE,   // The user has not been activated yet. Reported during opening of document.
    PLUGIN_LIC_EXPIRED,         // License for this document has expired
    PLUGIN_NO_LOAN_INFO_FOUND,  // No loan information found. 
    
    PLUGIN_DOCUMENT_CORRUPT,

    PLUGIN_PDF_PASSWORD_NEEDED,			      // (Adobe PDF) password needed
    PLUGIN_EPUB_USERNAMEPASSWORD_NEEDED,      // EPUB Username/password combination needed
    PLUGIN_BN_EPUB_CREDITCARDINFO_NEEDED,     // B&N EPUB DRM creditcard info needed
    PLUGIN_BN_PDB_CREDITCARDINFO_NEEDED,      // B&N legacy PDB DRM creditcard info needed

    PLUGIN_CREDENTIALS_AUTHORIZATION_FAILED,	// Credential authorization failed
    PLUGIN_CREDENTIALS_AUTHORIZATION_PENDING,	// Credentials could not be verified, UDS has to reopen the document
    
    PLUGIN_CORRUPT_ACTIVATION_FILE,           // (Adobe DRM) corrupt activation file, maybe tampering
       
    // Render settings
    PLUGIN_MAX_ZOOM_ALREADY     = RENDER_SETTINGS_STATUS_START,
    PLUGIN_MIN_ZOOM_ALREADY,
} PluginStatus;

#ifdef __cplusplus
}
#endif 

#endif

