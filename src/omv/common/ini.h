/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright (C) 2009-2020, Ben Hoyt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 * Read an INI file into easy-to-access name/value pairs.
 * For more info, see: https://github.com/benhoyt/inih
 */

#ifndef __INI_H__
#define __INI_H__

int ini_atoi(const char *string);
bool ini_is_true(const char *value);

/* Make this header file easier to include in C++ code */
#ifdef __cplusplus
extern "C" {
#endif

#include "file_utils.h"

/* Nonzero if ini_handler callback should accept lineno parameter. */
#ifndef INI_HANDLER_LINENO
#define INI_HANDLER_LINENO    0
#endif

/* Typedef for prototype of handler function. */
#if INI_HANDLER_LINENO
typedef int (*ini_handler) (void *user, const char *section,
                            const char *name, const char *value,
                            int lineno);
#else
typedef int (*ini_handler) (void *user, const char *section,
                            const char *name, const char *value);
#endif

/* Typedef for prototype of fgets-style reader function. */
typedef char * (*ini_reader) (char *str, int num, void *stream);

/* Parse given INI-style file. May have [section]s, name=value pairs
   (whitespace stripped), and comments starting with ';' (semicolon). Section
   is "" if name=value pair parsed before any section heading. name:value
   pairs are also supported as a concession to Python's configparser.

   For each name=value pair parsed, call handler function with given user
   pointer as well as section, name, and value (data only valid for duration
   of handler call). Handler should return nonzero on success, zero on error.

   Returns 0 on success, line number of first error on parse error (doesn't
   stop on first error), -1 on file open error, or -2 on memory allocation
   error (only when INI_USE_STACK is zero).
 */
int ini_parse(FATFS *fs, const char *filename, ini_handler handler, void *user);

/* Same as ini_parse(), but takes a FIL* instead of filename. This doesn't
   close the file when it's finished -- the caller must do that. */
int ini_parse_file(FIL *file, ini_handler handler, void *user);

/* Same as ini_parse(), but takes an ini_reader function pointer instead of
   filename. Used for implementing custom or string-based I/O (see also
   ini_parse_string). */
int ini_parse_stream(ini_reader reader, void *stream, ini_handler handler,
                     void *user);

/* Same as ini_parse(), but takes a zero-terminated string with the INI data
   instead of a file. Useful for parsing INI data from a network socket or
   already in memory. */
int ini_parse_string(const char *string, ini_handler handler, void *user);

/* Nonzero to allow multi-line value parsing, in the style of Python's
   configparser. If allowed, ini_parse() will call the handler with the same
   name for each subsequent line parsed. */
#ifndef INI_ALLOW_MULTILINE
#define INI_ALLOW_MULTILINE            1
#endif

/* Nonzero to allow a UTF-8 BOM sequence (0xEF 0xBB 0xBF) at the start of
   the file. See http://code.google.com/p/inih/issues/detail?id=21 */
#ifndef INI_ALLOW_BOM
#define INI_ALLOW_BOM                  1
#endif

/* Nonzero to allow inline comments (with valid inline comment characters
   specified by INI_INLINE_COMMENT_PREFIXES). Set to 0 to turn off and match
   Python 3.2+ configparser behaviour. */
#ifndef INI_ALLOW_INLINE_COMMENTS
#define INI_ALLOW_INLINE_COMMENTS      1
#endif
#ifndef INI_INLINE_COMMENT_PREFIXES
#define INI_INLINE_COMMENT_PREFIXES    ";"
#endif

/* Nonzero to use stack for line buffer, zero to use heap (malloc/free). */
#ifndef INI_USE_STACK
#define INI_USE_STACK                  1
#endif

/* Maximum line length for any line in INI file (stack or heap). Note that
   this must be 3 more than the longest line (due to '\r', '\n', and '\0'). */
#ifndef INI_MAX_LINE
#define INI_MAX_LINE                   200
#endif

/* Nonzero to allow heap line buffer to grow via realloc(), zero for a
   fixed-size buffer of INI_MAX_LINE bytes. Only applies if INI_USE_STACK is
   zero. */
#ifndef INI_ALLOW_REALLOC
#define INI_ALLOW_REALLOC              0
#endif

/* Initial size in bytes for heap line buffer. Only applies if INI_USE_STACK
   is zero. */
#ifndef INI_INITIAL_ALLOC
#define INI_INITIAL_ALLOC              200
#endif

/* Stop parsing on first error (default is to keep parsing). */
#ifndef INI_STOP_ON_FIRST_ERROR
#define INI_STOP_ON_FIRST_ERROR        0
#endif

#ifdef __cplusplus
}
#endif

#endif /* __INI_H__ */
