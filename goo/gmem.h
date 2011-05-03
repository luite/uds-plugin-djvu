/*
 * gmem.h
 *
 * Memory routines with out-of-memory checking.
 *
 * Copyright 1996-2003 Glyph & Cog, LLC
 */

#ifndef GMEM_H
#define GMEM_H

#include <stdio.h>
#include <poppler/poppler-config.h>

#if USE_EXCEPTIONS

class GMemException {
public:
  GMemException() {}
  ~GMemException() {}
};

#define GMEM_EXCEP throw(GMemException)

#else // USE_EXCEPTIONS

#define GMEM_EXCEP

#endif // USE_EXCEPTIONS

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Same as malloc, but prints error message and exits if malloc()
 * returns NULL.
 */
extern void *gmalloc(size_t size) GMEM_EXCEP;

/*
 * Same as realloc, but prints error message and exits if realloc()
 * returns NULL.  If <p> is NULL, calls malloc instead of realloc().
 */
extern void *grealloc(void *p, size_t size) GMEM_EXCEP;

/*
 * These are similar to gmalloc and grealloc, but take an object count
 * and size.  The result is similar to allocating nObjs * objSize
 * bytes, but there is an additional error check that the total size
 * doesn't overflow an int.
 * The gmallocn_checkoverflow variant returns NULL instead of exiting
 * the application if a overflow is detected
 */
extern void *gmallocn(int nObjs, int objSize) GMEM_EXCEP;
extern void *gmallocn_checkoverflow(int nObjs, int objSize) GMEM_EXCEP;
extern void *greallocn(void *p, int nObjs, int objSize) GMEM_EXCEP;

/*
 * Same as free, but checks for and ignores NULL pointers.
 */
extern void gfree(void *p);

#ifdef DEBUG_MEM
/*
 * Report on unfreed memory.
 */
extern void gMemReport(FILE *f);
#else
#define gMemReport(f)
#endif

/*
 * Allocate memory and copy a string into it.
 */
extern char *copyString(char *s);

/*
 * Allocate memory and copy a limited-length string to it.
 */
extern char *gstrndup(const char *s, size_t n);

#ifdef __cplusplus
}
#endif

#endif
