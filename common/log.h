/*
 * File Name: log.h
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

#ifndef PLUGIN_TEXT_LOG_H
#define PLUGIN_TEXT_LOG_H

#include <stdio.h>

#define LOGGING_ON       0 
#define WARNING_ON       1
#define ERROR_ON         1
#define DUMP_ON          1
#define TRACE_ON         0
#define MEM_ON           1
#define MEM_LEAK_ON      1
#define LIST_ON          1
#define PROFILE_ON       1
#define SCB_ON           0
#define TOOLBAR_ON       0 
#define PLUGIN_ON        1

#if (LOGGING_ON)
#define LOGPRINTF(x, ...) fprintf(stderr, "(LOG)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__ , ##__VA_ARGS__)
#else
#define LOGPRINTF(x, ...) do {} while (0)
#endif

#if (PLUGIN_ON)
#define PLUGINPRINTF(x, ...) fprintf(stderr, "(LOG)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__ , ##__VA_ARGS__)
#else
#define PLUGINPRINTF(x, ...) do {} while (0)
#endif

#if (WARNING_ON)
#define WARNPRINTF(x, ...) fprintf(stderr, "(WARNING)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__ , ##__VA_ARGS__)
#else
#define WARNPRINTF(x, ...) do {} while (0)
#endif

#if (ERROR_ON)
#define ERRORPRINTF(x, ...) fprintf(stderr, "(ERROR)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define ERRORPRINTF(x, ...) do {} while (0)
#endif

#if (DUMP_ON)
#define DUMP(x, ...) fprintf(stderr,  x , ##__VA_ARGS__)
#else
#define DUMP(x, ...) do {} while (0)
#endif

#if (TRACE_ON)
#define TRACE(x, ...) fprintf(stderr,  x , ##__VA_ARGS__)
#else
#define TRACE(x, ...) do {} while (0)
#endif

#if (LIST_ON)
#define LISTPRINTF(x, ...) fprintf(stderr,  x , ##__VA_ARGS__)
#else
#define LISTPRINTF(x, ...) do {} while (0)
#endif

#if (MEM_ON)
#define MEMPRINTF(x, ...) fprintf(stderr, "(MEM)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__ , ##__VA_ARGS__)
#else
#define MEMPRINTF(x, ...) do {} while(0)
#endif

#if (SCB_ON)
#define SCBPRINTF(x, ...) fprintf(stderr, "(SCRIBBLE)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__ , ##__VA_ARGS__)
#else
#define SCBPRINTF(x, ...) do {} while (0)
#endif

#if (TOOLBAR_ON)
#define TBPRINTF(x, ...) fprintf(stderr, "(TB)" __FILE__ ":%d,%s() " x "\n", __LINE__, __FUNCTION__ , ##__VA_ARGS__)
#else
#define TBPRINTF(x, ...) do {} while (0)
#endif

#endif // PLUGIN_TEXT_LOG_H
