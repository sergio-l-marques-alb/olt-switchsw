/*
 *
 * Product: EmWeb
 * Release: R6_2_0
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION OF VIRATA CORPORATION
 * THE EMWEB SOFTWARE ARCHITECTURE IS PATENTED (US PATENT #5,973,696)
 * AND OTHER US AND INTERNATIONAL PATENTS PENDING.
 * 'EMWEB' AND 'EMSTACK' ARE TRADEMARKS OF VIRATA CORPORATION
 * 
 * Notice to Users of this Software Product:
 * 
 * This software product of Virata Corporation ("Virata"), 5 Clock Tower
 * Place, Suite 400, Maynard, MA  01754 (e-mail: info@virata.com) in 
 * source and object code format embodies valuable intellectual property 
 * including trade secrets, copyrights and patents which are the exclusive
 * proprietary property of Virata. Access, use, reproduction, modification
 * disclsoure and distribution are expressly prohibited unless authorized
 * in writing by Virata.  Under no circumstances may you copy this
 * software or distribute it to anyone else by any means whatsoever except in
 * strict accordance with a license agreement between Virata and your
 * company.  This software is also protected under patent, trademark and 
 * copyright laws of the United States and foreign countries, as well as 
 * under the license agreement, against any unauthorized copying, modification,
 * or distribution.
 * Please see your supervisor or manager regarding any questions that you may
 * have about your right to use this software product.  Thank you for your
 * cooperation.
 * 
 * Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001 Virata Corporation
 * All Rights Reserved
 * 
 * Virata Corporation
 * 5 Clock Tower Place
 * Suite 400
 * Maynard, MA  01754
 * 
 * Voice: +1 (978) 461-0888
 * Fax:   +1 (978) 461-2080
 * 
 * http://www.emweb.com/
 *   support@agranat.com
 *     sales@agranat.com
 *
 */

#ifndef _EWS_STRING_H_
#define _EWS_STRING_H_

#include "ew_common.h"
#include "ews_net.h"

/******************************************************************************
 *
 * BUFFER STRING LIBRARY
 *
 * The EmWeb/Server design attempts to make as few assumptions as possible
 * about the buffer structures used to represent network data. As the HTTP
 * protocol is string intensive, it is difficult to guarantee that strings
 * of interest are necessarily contiguous on buffer boundaries. Indeed, there
 * are no fixed limits on how big many strings may become.  For efficient use
 * of available memory, it is best that the EmWeb/Server avoid copying strings
 * from network buffers into private memory as much as possible.
 *
 * This library defines datastructures and functions intended to facilitate
 * the manipulation of strings within chains of network buffers.
 *
 *****************************************************************************/

/*
 * EwsString
 * This structure represents a string within a chain of network buffers.
 *
 * **NOTE: offset must NEVER be > ewaNetBufferLengthGet(EwsString.first)**
 *
 *         However, it may be = to it (and thus the character referred to is
 *         in the next buffer, or "off the end of the string").
 *
 *         Also, first may be NULL, but only if length = 0 and offset = 0.
 */
typedef struct EwsString_s
  {
    EwaNetBuffer        first;          /* first buffer containing string */
    uintf               offset;         /* offset into first to string start */
    uintf               length;         /* length of string in bytes */
  } EwsString;

#define EWS_STRING_EOF ((uintf) -1)

typedef struct EwsString_s *EwsStringP;

/*
 * ewsStringInit
 * Initialize an EwsString structure
 *
 * estring      - pointer to EwsString to initialize
 * buffer       - initial buffer, or EWA_NET_BUFFER_NULL
 *
 * No return value
 */
extern void ewsStringInit(EwsStringP estring, EwaNetBuffer buffer);

/*
 * ewsStringAttachBuffer
 * Attach a buffer to the string if not currently attached, and return buffer
 * or EWA_NET_BUFFER_NULL if no buffer attached and no resources available.
 *
 * estring      - pointer to EwsString to attach buffer
 *
 * Returns attached buffer or EWA_NET_BUFFER_NULL.
 */
extern EwaNetBuffer ewsStringAttachBuffer ( EwsString *estring );

/*
 * ewsStringCopyIn
 * Copy a contiguous null-terminated string to the end of a buffer chain.
 *
 * estring      - pointer to EwsString with estring->first initialized to the
 *                last buffer on the chain and estring->offset initialized to
 *                the first unwritten byte in the buffer (whose length
 *                indicates the end of the buffer).  (If estring->first is
 *                EWA_NET_BUFFER_NULL, then a new buffer will be allocated).
 * src          - pointer to contiguous null-terminated character string.
 *
 * Returns the last buffer on the chain, or NULL if buffers could not be
 * allocated.  On return, the estring is updated with estring.first pointing to
 * thelast buffer on the chain and estring.offset indicating the first
 * unwritten byte in the buffer.
 */
extern EwaNetBuffer ewsStringCopyIn ( EwsStringP estring, const char * src );

/*
 * ewsStringNCopyIn
 * Copy a contiguous fixed-length string to the end of a buffer chain.
 *
 * estring      - pointer to EwsString with estring->first initialized to the
 *                last buffer on the chain and estring->offset initialized to
 *                the first unwritten byte in the buffer (whose length
 *                indicates the end of the buffer).  (If estring->first is
 *                EWA_NET_BUFFER_NULL, then a new buffer will be allocated).
 * src          - pointer to contiguous null-terminated character string.
 * length       - length of string
 *
 * Returns the last buffer on the chain, or NULL if buffers could not be
 * allocated.  On return, the estring is updated with estring.first pointing to
 * thelast buffer on the chain and estring.offset indicating the first
 * unwritten byte in the buffer.
 */
extern EwaNetBuffer ewsStringNCopyIn
  ( EwsStringP estring, const uint8 * src, uintf length );

/*
 * ewsStringCopyOut
 * Copy a string from a chain of network buffers into a contiguous
 * null-terminated character string.
 *
 * dst          - pointer to contiguous memory for a null-terminated string
 * estring      - pointer to an EwsString describing the string to be copied.
 * maxbytes     - maximum number of bytes to copy, not including '\0'.
 *
 * No return value.
 */
extern void ewsStringCopyOut
  ( char * dst, EwsStringP estring, uintf maxbytes );

/*
 * ewsStringSearch
 * Searches buffer chain for specified characters.
 *
 * estring      - input/output: describes start of string, adjusts length.
 * next         - input/output: describes start of search, move to end.
 * match_list   - null-terminated string of characters to search for.
 * incl_excl    - include or exclude flag.  If ewsStringSearchInclude,
 *                then search terminates when character matching match_list
 *                is found.  If ewsStringSearchExclude, then search terminates
 *                when character not matching match_list is found.
 * chunklenp    - input/output: bytes remaining in chunk, or NULL.  If not
 *                NULL, decremented by the number of bytes compared (regardless
 *                of whether or not match found) -same as increase in estring
 *                length.
 *
 * Returns matching character, or EWS_STRING_EOF if no match was found before
 * running out of buffers in the chain.
 *
 * With or without a match, estring->length is incremented by the number of
 * bytes scanned in this pass, estring->first/->offset are never changed.
 * With or without a match, next->first and next->offset are updated to point
 * to the character following the match or EOF.  This allows efficient searches
 * as buffers are received by not restarting the search from the beginning each
 * time.  When the search terminates, estring will be a valid string from the
 * beginning to the matching character or EOF.
 */
typedef enum EwsStringSearchType_e
  {
    ewsStringSearchInclude,
    ewsStringSearchExclude
  } EwsStringSearchType;

extern uintf ewsStringSearch( EwsStringP estring
                             ,EwsStringP next
                             ,const char *match_list
                             ,EwsStringSearchType incl_exl
                             ,uint32 *chunklenp
                             );

/*
 * ewsStringCompare
 * -> Beware that this is a prefix test on the E-string argument. <-
 * Compares a contiguous null-terminated string with a string from a chain of
 * network buffers.  Returns TRUE if the E-string begins with the C-string.
 *
 * estring      - input/output: pointer to EwsString describing string in
 *                buffer chain.  estring->length must be >= strlen(string).
 *                On match, estring->offset/->first is advanced past the
 *                matching string with its length decreased by the skipped
 *                string length.  If no match, estring is not modified.
 * string       - pointer to contiguous null-terminated string
 * case_type    - ewsStringCompareCaseSensitive/Insensitive
 * chunklenp    - input/output: pointer to remaining bytes in chunk, or NULL.
 *                If match succeeds and chunklenp != NULL, *chunklenp is
 *                decreased by the length of the matched string.
 *
 * Returns TRUE if strings compare and updates estring to end of match.
 * Otherwise, returns FALSE.
 */
typedef enum EwsStringCompareCase_e
  {
    ewsStringCompareCaseSensitive,
    ewsStringCompareCaseInsensitive
  } EwsStringCompareCase;

extern boolean ewsStringCompare
  ( EwsStringP estring
   ,const char *string
   ,EwsStringCompareCase case_type
   ,uint32 *chunklenp );


#ifdef EMWEB_EWS_STRING_COMPARE
/*
 * ewsEwsStringCompare
 *
 * Compares two strings from chains of network buffers.
 *
 * string1      - input: pointer to EwsString describing string in
 *                buffer chain.
 * string2      - input: pointer to EwsString describing another string in
 *                buffer chain.
 * case_type    - ewsStringCompareCaseSensitive/Insensitive
 *
 * Returns TRUE if strings compare.
 * Otherwise, returns FALSE.
 */
boolean
ewsEwsStringCompare
  ( EwsString string1
   ,EwsString string2
   ,EwsStringCompareCase case_type );

#endif /* EMWEB_EWS_STRING_COMPARE */

/*
 * ewsStringGetChar
 * This function returns a single character, or EWS_STRING_EOF, and advances
 * the string pointer
 *
 * estring      - input/output: pointer to EwsString
 *
 * Returns the first character in estring and advances estring->offset/->first
 * to point to the next character, estring->length is decremented.
 * Or, returns EWS_STRING_EOF if the end of the buffer chain has been reached.
 * DOES NOT return EWS_STRING_EOF just because estring->length is 0!
 *
 * Note: estring->length is blindly decremented, and some code relies on that,
 * so don't check for a valid length here!
 * (But this means that estring->length need not be valid, which is useful
 * if estring is a pointer rather than a string descriptor.)
 */
extern uintf ewsStringGetChar ( EwsStringP estring );

/*
 * ewsStringGetInteger
 * Read characters from string in given base until end of string or invalid
 * character returned.
 *
 * estring      - pointer to EwsString
 * base         - base of string (2-16)
 * valuep       - output: value of string
 *
 * Returns TRUE on success, FALSE on parse error.  Advances estring to
 * character after value.
 */
extern boolean ewsStringGetInteger
  ( EwsStringP estring, uintf base, uint32 *valuep );

/*
 * ewsStringRewriteChar
 * This function rewrites a single character to the string and advances the
 * string pointer
 *
 * estring      - input/output: first/offset indicates where to rewrite byte
 * byte         - byte value to rewrite
 *
 * No return value
 */
extern void ewsStringRewriteChar ( EwsStringP estring, uint8 byte );

/*
 * ewsStringUnescape
 * Translate EwsString contents converting '%' HEX HEX sequences into
 * corresponding single-octet values.  Update length accordingly.
 *
 * estring      - pointer to EwsString
 * plus2space   - ewsStringUnescapePlus2Space or ewsStringUnescapeNoPlus2Space
 *
 * No return value
 */
typedef enum
  {
    ewsStringUnescapePlus2Space,
    ewsStringUnescapeNoPlus2Space
  } EwsStringUnescape_e;

extern void ewsStringUnescape
  ( EwsStringP estring, EwsStringUnescape_e plus2space);


#ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
/*
 * ewsStringSubstringSearch
 *
 * Search a chain of data for a substring (the substring is a null terminated
 * C string).  Return TRUE when found, else FALSE.
 *
 * estring - input/output describes start of string.  This string grows as the
 *           search skips characters that don't match.  If the match is found,
 *           estring will represent the portion of the original string that
 *           comes BEFORE the match substring.  If no match, estring will
 *           encompass the entire chain of data (up to chunklenp, if used).
 * end     - input/output  on the first call, this parameter should be set to
 *           estring.  When a match is found, end is set to the string segment
 *           that matched.
 * string  - input the string to match
 * case_type - input ewsStringCompareCaseSensitive/Insensitive
 * chunklenp - input/output remaining bytes in chunk, updated on return by
 *             subtracting the length of both estring and end.  Else NULL
 *
 * Returns TRUE if strings compare, else FALSE.  This routine is meant to be
 * reentrant - on return of FALSE, the parameters estring and end are updated
 * to record where the search ended.  As new data is received on the network,
 * this routine can be re-called with the same parameters.  The search
 * continues from where it was left off.   Only on the first call should
 * estring and end be initialized to the same value: a string of zero length
 * starting at the first byte to be seached.
 *
 * example:
 *
 * say estring points to
 * AAAAAAAAAAAAAAAAAAAAAAAABBBBAAAAAAA
 * and we're searching for substring BBBB
 *
 * set estring length to 0
 * set end = estring
 * assume chunklen = 35
 *
 * After call:
 *
 * estring (length = 24)   end (length = 4)
 * |                       |
 * V                       V
 * AAAAAAAAAAAAAAAAAAAAAAAABBBBAAAAAAA
 *
 * and chunklen -= estring.length + end.length
 */
extern boolean ewsStringSubstringSearch
  ( EwsStringP estring, EwsStringP end, const char *string,
    EwsStringCompareCase case_type, uint32 *chunklenp );

#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


#if   defined( EW_CONFIG_FILE_METHODS ) \
   || defined( EW_CONFIG_OPTION_FIELDTYPE_FILE ) \
   || defined( EW_CONFIG_OPTION_TELNET)\
   || defined (EMWEB_XMLP)
/*
 * ewsStringTrim
 *
 * Skips over a number of characters in a string, up to string->length
 * chars can be skipped.  Useful for trimming characters from the
 * front of a string, or skipping over a string entirely.
 *
 *  estring - input/output: string to be trimmed.
 *  count   - input: the number of characters to skip
 *
 * On return, estring->first/->offset are advanced
 *          MIN( estring->length, count )
 * characters.  Returns the number of characters estring was advanced.
 * estring->length is decremented by count.  If count == estring->length,
 * then estring is set to point to the character beyond the string estring
 * represented.
 */
extern uintf ewsStringTrim( EwsStringP estring, uintf count );
#endif

#if defined(EW_CONFIG_OPTION_FIELDTYPE_FILE) || defined(EW_CONFIG_OPTION_PRIMARY)
/*
 * ewsStringWrite
 *
 * Writes the contents of estring to the given output file.  Advance
 * the input string by the number of characters written successfully.
 *
 *  estring - input/output: string to be written out.  It is advanced
 *              by the number of bytes successfully written.
 *  handle  - output: file or secondary handle to write to.
 *  type    - ewsStringWriteFile or ewsStringWriteSecondary
 *  context - current context, used if file write suspends.
 *
 * Returns the number of bytes written, else <0 on error.  If suspend
 * functionality is supported, then context->schedulingState must
 * be checked for ewsContextSuspended on return.
 */
#ifdef EW_CONFIG_OPTION_PRIMARY
typedef enum
  {
    ewsStringWriteFile,
    ewsStringWriteSecondary
  } EwsStringWriteType;
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE || EW_CONFIG_OPTION_PRIMARY */

extern sintf
ewsStringWrite( EwsContext context
               ,void * handle
#               ifdef EW_CONFIG_OPTION_PRIMARY
               ,EwsStringWriteType type
#               endif /* EW_CONFIG_OPTION_PRIMARY */
               ,EwsString *estring );
#endif /* EW_CONFIG_OPTION_FILE */

/*
 * ewsStringIsContiguous
 *
 * estring      - input string
 *
 * Returns TRUE if estring is contained within a single buffer (with room for
 * one extra byte for a '\0', otherwise FALSE is returned.
 */
#define ewsStringIsContiguous(es) \
          (((es)->offset + (es)->length) < ewaNetBufferLengthGet((es)->first))

/*
 * ewsStringExpand
 * Increment the string length by the amount specified
 *
 * estring      - input string
 * bytes        - number of bytes to add to length
 *
 * No return value
 */
#define ewsStringExpand(es, bytes) ((es)->length += bytes)

/*
 * ewsStringData
 *
 * estring      - input string
 *
 * Returns pointer to first character of data in string
 */
#define ewsStringData(es) (ewaNetBufferDataGet((es)->first) + (es)->offset)

/*
 * ewsStringLength
 *
 * estring      - input string
 *
 * Returns length of data in string
 */
#define ewsStringLength(es) ((es)->length)

/*
 * ewsStringLengthSet
 * set string length to the amount specified
 *
 * estring      - input string
 * bytes        - number of bytes to set length to
 *
 * No return value
 */
#define ewsStringLengthSet(es, bytes) ((es)->length = bytes)

/*
 * ewsStringAvailable
 *
 * estring      - input string
 *
 * Returns available data bytes after offset in current buffer.
 */
#define ewsStringAvailable(es) \
          (((es)->first == EWA_NET_BUFFER_NULL)? 0 : \
           (ewaNetBufferLengthGet((es)->first) - (es)->offset))

/*
 * ewsStringBuffer
 *
 * estring      - input string
 *
 * Returns first buffer in string
 */
#define ewsStringBuffer(es) ((es)->first)

#ifdef EMWEB_HAVE_STRCHR
# ifndef EMWEB_STRCHR
#   define EMWEB_STRCHR(s,c) strchr(s,c)
# endif
#else
# define EMWEB_STRCHR(s,c) ews_strchr(s,c)
  extern char* ews_strchr( const char* s, char c ); /* in ews_str.c */
#endif /* EMWEB_HAVE_STRCHR */

#ifdef EMWEB_HAVE_STRSTR
# ifndef EMWEB_STRSTR
#   define EMWEB_STRSTR(s1, s2)          strstr(s1, s2)
# endif
#else
# define EMWEB_STRSTR(s1, s2)   ews_strstr(s1,s2)
  extern char *ews_strstr(const char *s1, const char *s2); /* in ews_str.c */
#endif /* EMWEB_HAVE_STRSTR */

/*
 * ewsStringTrimOptions
 * Remove leading spaces and tabs or trailing spaces from the passed string,
 * based on conditions.
 *
 *  s - input/output: string to be trimmed.
 *  trim_lead  - input: flag to remove leading whitespace.
 *  trim_trail  - input: flag to remove trailing whitespace.
 *
 * Return trimmed string.
 */
extern char* ewsStringTrimOptions
(char *s, boolean trim_lead, boolean trim_trail);

/*
 * ewsStringTrimLeading
 * Remove leading whitespace from the passed string.
 *
 *  s - input/output: string to be trimmed.
 *
 * Return trimmed string.
 */
extern char* ewsStringTrimLeading (char *s);

/*
 * ewsStringTrimTrailing
 * Remove trailing whitespace from the passed string.
 *
 *  s - input/output: string to be trimmed.
 *
 * Return trimmed string.
 */
extern char* ewsStringTrimTrailing (char *s);

/*
 * Copy a string into a user-specified buffer.  The string which is copied is
 * either the buffer data specified by an EwsString, or, of that's empty, a
 * default string.
 *
 * Parameters:
 *
 *   s -
 *         A portion of a buffer chain which defines a string.  If this
 *         parameter's "length" field is zero, then the string pointed to by
 *         ifEmpty will be used instead.
 *
 *   ifEmpty -
 *         If the EwsString is empty, then the string pointed to by this
 *         parameter will be placed into the output buffer.
 *
 *   pBuf -
 *         Pointer to a buffer into which one of the two strings will be
 *         copied.
 *
 *   maxlen -
 *         The length of the buffer.  maxlen-1 characters will be copied into
 *         the buffer and the buffer will be null terminated.
 *
 * Returns:
 *         The start of the user-specified buffer (i.e. the passed-in value of
 *         pBuf)
 */
char *
ewsStringContiguous(EwsString s,
                    const char * ifEmpty,
                    char * pBuf,
                    int maxlen);


/* This function may be called from the debugger */
char *
ewsStringPrintable(EwsString s);

#endif /* _EWS_STRING_H_ */
