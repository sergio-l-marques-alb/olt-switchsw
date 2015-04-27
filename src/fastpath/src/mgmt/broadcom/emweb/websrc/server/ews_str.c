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
 * EmWeb/Server Buffer String Library
 *
 */
#include "ew_types.h"
#include "ew_config.h"

#ifndef EMWEB_NO_SERVER
#include "ews.h"
#include "ews_sys.h"
#include "ews_dist.h"
#include "ews_file.h"
#include "ews_gway.h"
#include "ews_form.h"
#include "ews_tah.h"
#endif

#include "ews_str.h"
#include "ew_lib.h"

extern char *	strncpy (char *__s1, const char *__s2, unsigned int __n);  /*LVL7_P0006*/

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
 * ewsStringInit
 * Initialize an EwsString structure
 *
 * estring      - pointer to EwsString to initialize
 * buffer       - initial buffer, or EWA_NET_BUFFER_NULL
 *
 * No return value
 */
void
ewsStringInit(EwsStringP estring, EwaNetBuffer buffer)
{
    estring->first = buffer;
    estring->offset = 0;
    estring->length = 0;
}

/*
 * ewsStringAttachBuffer
 *
 * Attach a buffer to a EwsString.  It is assumed that estring is in
 * the LAST buffer of a chain (NO NEXT BUFFER), or that estring is not
 * associated with a buffer (estring->first == EWA_NET_BUFFER_NULL).
 * If estring does not have a buffer, one is allocated, attached to
 * estring (offset & length are zeroed).  This buffer is then returned.
 * If estring already has a buffer, but offset indicates no more room,
 * (length is ignored), then a new buffer is attached to the old, and
 * estring is advanced to point to the first byte of this new buffer, which
 * is returned.
 * If estring has a buffer and offset is not yet at the end, estring
 * is not modified, and the address of the buffer is returned.
 * EWA_NET_BUFFER_NULL is returned if no buffer attached and no resources
 * available.
 *
 * estring      - input/output: pointer to EwsString to attach buffer.
 *                fields first, offset, and length may be altered
 *
 * Returns attached buffer or EWA_NET_BUFFER_NULL.
 */
EwaNetBuffer
ewsStringAttachBuffer ( EwsString *estring )
{
  EwaNetBuffer buffer;

    /*
     * If no buffer attached, attempt to allocate one
     */
    buffer = estring->first;
    if (buffer == EWA_NET_BUFFER_NULL)
      {
        estring->first = buffer = ewaNetBufferAlloc();
        estring->offset = 0;
        estring->length = 0;

#       ifdef EMWEB_SANITY
        if (buffer != EWA_NET_BUFFER_NULL &&
            ewaNetBufferLengthGet(buffer) == 0)
          {
            EMWEB_ERROR(("ewsStringAttachBuffer: porting error - buffer length"
                         " must be initialized to buffer size\n"));
          }
#       endif /* EMWEB_SANITY */

      }

    /*
     * Otherwise, if we are at the end of the current buffer, attempt to
     * add an additional one
     */

    else if (ewaNetBufferLengthGet(buffer) == estring->offset)
      {
        buffer = ewaNetBufferAlloc();
        if (buffer != EWA_NET_BUFFER_NULL)
          {
#           ifdef EMWEB_SANITY

            if (ewaNetBufferNextGet(estring->first) != EWA_NET_BUFFER_NULL)
              {
                EMWEB_ERROR(("ewsStringAttachBuffer: Not attaching to END"
                             " of buffer chain.\n"));
                return EWA_NET_BUFFER_NULL;
              }
#           endif /* EMWEB_SANITY */
            ewaNetBufferNextSet(estring->first, buffer);
            estring->first = buffer;
            estring->offset = 0;
          }
      }
#   ifdef EMWEB_SANITY
    else if (estring->offset > ewaNetBufferLengthGet(estring->first))
      {
        EMWEB_ERROR(("ewsStringAttachBuffer: Corrupt offset.\n"));
        return EWA_NET_BUFFER_NULL;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Return attached buffer, if any
     */
    return buffer;
}

/*
 * Special unified context for keeping track of the buffers into which
 * we are writing output.
 * In a language that supported nested procedures, these would be
 * local to ewsStringCopyIn, but C doesn't support that.
 */
typedef struct
{
  EwaNetBuffer buffer;        /* current buffer */
  uintf        offset;        /* current offset into buffer */
  uintf        available;     /* current available bytes to end of buffer */
  uint8*       dst;           /* current destination character pointer */
  EwsStringP   estring_p;     /* the string we are copying into */
  uintf        string_length; /* current length of estring_p */
} CopyCtxt_s, *CopyCtxt;

/*
 * Write a character to the buffer.
 * This routine assumes that TEXTAREA quoting has already been done.
 * Returns 0 if an attempt to allocate a buffer failed; 1 otherwise.
 */
static boolean
ewsCopyExpandedChar ( CopyCtxt ctxt, uint8 c )
{
  /*
   * If there are no more bytes left in the current buffer, allocate
   * a new one, append to chain, and reset state.  If we can't get a
   * buffer, then return 0.
   */
  while ( ctxt->available == 0)
    {
      ctxt->buffer = ewaNetBufferAlloc();
#     ifdef EW_CONFIG_OPTION_SCHED
      if (ctxt->buffer == EWA_NET_BUFFER_NULL)
        {
          return 0;
        }
#     endif /* EW_CONFIG_OPTION_SCHED */
      if (ctxt->estring_p->first != EWA_NET_BUFFER_NULL)
        {
          ewaNetBufferNextSet(ctxt->estring_p->first, ctxt->buffer);
        }
      ctxt->estring_p->first = ctxt->buffer;
      ctxt->offset = 0;
      ctxt->available = ewaNetBufferLengthGet(ctxt->buffer);
      ctxt->dst = ewaNetBufferDataGet(ctxt->buffer);
    }

  *ctxt->dst++ = c;
  ctxt->available--;
  ctxt->offset++;
  ctxt->string_length++;

  return 1;
}

/*
 * Write a character to the buffer.
 * This routine takes into account TEXTAREA quoting and expands the character
 * accordingly.
 * Returns 0 if an attempt to allocate a buffer failed; 1 otherwise.
 */
static boolean
ewsCopyRawChar ( CopyCtxt ctxt, uint8 c, boolean quoted )
{
# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  if (quoted)
    {
      /* Represent the escape character as TEXTAREA_ESCAPE_CHAR 'a'. */
      if (c == TEXTAREA_ESCAPE_CHAR)
        {
          return (   ewsCopyExpandedChar( ctxt, TEXTAREA_ESCAPE_CHAR)
                  && ewsCopyExpandedChar( ctxt, 'a')
                  );
        }

      /* Represent CR as TEXTAREA_ESCAPE_CHAR 'b'. */
      if (c == '\r')
        {
          return (   ewsCopyExpandedChar( ctxt, TEXTAREA_ESCAPE_CHAR )
                  && ewsCopyExpandedChar( ctxt, 'b' )
                  );
        }

      /* Represent LF (a/k/a NL) as TEXTAREA_ESCAPE_CHAR 'c'. */
      if (c == '\n')
        {
          return (   ewsCopyExpandedChar( ctxt, TEXTAREA_ESCAPE_CHAR )
                  && ewsCopyExpandedChar( ctxt, 'c')
                  );
        }
      /* Represent '&' as TEXTAREA_ESCAPE_CHAR 'd'. */
      if (c == '&')
        {
          return (   ewsCopyExpandedChar( ctxt, TEXTAREA_ESCAPE_CHAR)
                  && ewsCopyExpandedChar( ctxt, 'd')
                  );
        }

      /* Represent '<' as TEXTAREA_ESCAPE_CHAR 'e'. */
      if (c == '<')
        {
          return (   ewsCopyExpandedChar( ctxt, TEXTAREA_ESCAPE_CHAR)
                  && ewsCopyExpandedChar( ctxt, 'e')
                  );
        }
    }
# else
  EW_UNUSED(quoted);
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

  /* Represent all other characters as themselves */
  return ewsCopyExpandedChar( ctxt, c);
}

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
 * Returns the last buffer on the chain, or EWA_NET_BUFFER_NULL if buffers
 * could not be allocated.  On return, the estring is updated with
 * estring.first pointing to the last buffer on the chain and estring.offset
 * indicating the first unwritten byte in the buffer.  estring.length is
 * incremented by the # of characters copied in.
 *
 * Note that if the input buffer chain has no members and the input string
 * has no characters, ewsStringCopyIn will return EWA_NET_BUFFER_NULL even
 * if no error is found.
 *
 * If output quoting is selected by the context flag
 * EWS_CONTEXT_FLAGS_OUTPUT_QUOTE (that is, the content we are serving is
 * inside a 'TEXTAREA hack' for transporting semi-arbitrary characters),
 * apply the following transformations:
 *      replace '&' with '&amp;'
 *      replace '<' with '&lt;'
 * These transformations are so that the 'value' of the TEXTAREA is
 * the string of characters which is in the EWS archive member.
 */
EwaNetBuffer
ewsStringCopyIn ( EwsStringP estring, const char * src )
{
  static CopyCtxt_s ctxt; /* this is static to avoid a gcc optimizer bug */

  /* QUOTED is true if quoting is in effect. */
# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  boolean quoted =
    (ews_state->context_current != NULL) &&
    ((ews_state->context_current->flags &
      EWS_CONTEXT_FLAGS_OUTPUT_QUOTE) != 0);
  #define QUOTED quoted
# else
  #define QUOTED 0
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

    /*
     * Initialize state.
     */
    ctxt.estring_p = estring;
    ctxt.buffer = estring->first;
    ctxt.offset = estring->offset;
    ctxt.string_length = estring->length;
    if (ctxt.buffer == EWA_NET_BUFFER_NULL)
      {
        ctxt.available = 0;
        ctxt.dst = NULL;     /* avoid ANSI C compiler warning -- not needed */
      }
    else
      {
        ctxt.available = ewaNetBufferLengthGet(ctxt.buffer) - ctxt.offset;
        ctxt.dst = ewaNetBufferDataGet(ctxt.buffer) + ctxt.offset;
#       ifdef EMWEB_SANITY
        if (ctxt.offset > ewaNetBufferLengthGet(ctxt.buffer))
          {
            EMWEB_ERROR(("ewsStringCopyIn: Corrupt offset.\n"));
            return EWA_NET_BUFFER_NULL;
          }
#       endif /* EMWEB_SANITY */
      }

    /*
     * Copy data until NULL termination found in source string
     */
    while (*src != '\0')
      {
#ifdef EW_CONFIG_OPTION_SCHED
        if (ewsCopyRawChar( &ctxt, *src++, QUOTED) == 0)
          {
            goto allocate_failed;
          }
#else
        ewsCopyRawChar( &ctxt, *src++, QUOTED);
#endif /* EW_CONFIG_OPTION_SCHED */
      }

    /*
     * Update estring and return
     */
    estring->offset = ctxt.offset;
    estring->length = ctxt.string_length;
    return ctxt.buffer;

# ifdef EW_CONFIG_OPTION_SCHED
allocate_failed:
  EMWEB_WARN(("ewsStringCopyIn: no net buffers available.\n"));
  /*
   * Note: we may be in this function without current context,
   * (for example if we are processing ewsDistSecondaryData()
   * call) so Abort (or Finish) action makes no sense under
   * these circumstances.
   */
  if (ews_state->context_current != NULL)
    {
#     ifdef EW_CONFIG_OPTION_PRIMARY
      /*
       * Special case:
       * If we are dealing with the response from a secondary server...
       */
      if (ews_state->context_current->is_secondary_context)
        {
          ewsDistFinish(ews_state->context_current->secondary,
                        EWS_STATUS_ABORTED);
        }
      else
#     endif /* EW_CONFIG_OPTION_PRIMARY */
        ewsNetHTTPAbort(ews_state->context_current);
    }
  estring->offset = ctxt.offset;
  estring->length = ctxt.string_length;
  return EWA_NET_BUFFER_NULL;   /* failure */
# endif /* EW_CONFIG_OPTION_SCHED */
}

/*
 * ewsStringNCopyIn
 * Copy a contiguous fixed-length string to the end of a buffer chain.
 *
 * estring      - pointer to EwsString with estring->first initialized to the
 *                last buffer on the chain and estring->offset initialized to
 *                the first unwritten byte in the buffer (whose length
 *                indicates the end of the buffer).  (If estring->first is
 *                EWA_NET_BUFFER_NULL, then a new buffer will be allocated).
 * src          - pointer to contiguous fixed-length character string.
 * length       - length of bytes in string.
 *
 * Returns the last buffer on the chain, or EWA_NET_BUFFER_NULL if buffers
 * could not be allocated.  On return, the estring is updated with
 * estring.first pointing to the last buffer on the chain and estring.offset
 * indicating the first unwritten byte in the buffer.  estring.length is
 * incremented by the # of characters copied in.
 *
 * If output quoting is selected by the context flag
 * EWS_CONTEXT_FLAGS_OUTPUT_QUOTE (that is, the content we are serving is
 * inside a 'TEXTAREA hack' for transporting semi-arbitrary characters),
 * apply the following transformations:
 *      replace '&' with '&amp;'
 *      replace '<' with '&lt;'
 *      replace LF with CR-LF if it is not preceeded by CR already
 * The first two transformations are so that the 'value' of the TEXTAREA is
 * the string of characters which is in the EWS archive member.
 * The last transformation gets around a bug in Netscape that inserts a CR-LF
 * in the value of a TEXTAREA if there are around 500 characters not containing
 * a CR-LF.
 */
EwaNetBuffer
ewsStringNCopyIn ( EwsStringP estring, const uint8 * src, uintf length )
{
  static CopyCtxt_s ctxt; /* this is static to avoid a gcc optimizer bug */

  /* QUOTED is true if quoting is in effect. */
# ifdef EW_CONFIG_OPTION_CONVERT_TEXTAREA
  boolean quoted =
    (ews_state->context_current != NULL) &&
    ((ews_state->context_current->flags &
      EWS_CONTEXT_FLAGS_OUTPUT_QUOTE) != 0);
  #define QUOTED quoted
# else
  #define QUOTED 0
# endif /* EW_CONFIG_OPTION_CONVERT_TEXTAREA */

    /*
     * Initialize state.
     */
    ctxt.estring_p = estring;
    ctxt.buffer = estring->first;
    ctxt.offset = estring->offset;
    ctxt.string_length = estring->length;
    if (ctxt.buffer == EWA_NET_BUFFER_NULL)
      {
        ctxt.available = 0;
        ctxt.dst = NULL;     /* avoid ANSI C compiler warning -- not needed */
      }
    else
      {
        ctxt.available = ewaNetBufferLengthGet(ctxt.buffer) - ctxt.offset;
        ctxt.dst = ewaNetBufferDataGet(ctxt.buffer) + ctxt.offset;
#       ifdef EMWEB_SANITY
        if (ctxt.offset > ewaNetBufferLengthGet(ctxt.buffer))
          {
            EMWEB_ERROR(("ewsStringNCopyIn: Corrupt offset.\n"));
            return EWA_NET_BUFFER_NULL;
          }
#       endif /* EMWEB_SANITY */
      }

    /*
     * Copy data until NULL termination found in source string
     */
    while (length != 0)
      {
        length--;
#ifdef EW_CONFIG_OPTION_SCHED
        if (ewsCopyRawChar( &ctxt, *src++, QUOTED) == 0)
          {
            goto allocate_failed;
          }
#else
        ewsCopyRawChar( &ctxt, *src++, QUOTED);
#endif /* EW_CONFIG_OPTION_SCHED */
      }

    /*
     * Update estring and return
     */
    estring->offset = ctxt.offset;
    estring->length = ctxt.string_length;
    return ctxt.buffer;

# ifdef EW_CONFIG_OPTION_SCHED
allocate_failed:
  EMWEB_WARN(("ewsStringNCopyIn: no net buffers available.\n"));
  /*
   * Note: we may be in this function without current context,
   * (for example if we are processing ewsDistSecondaryData()
   * call) so Abort (or Finish) action makes no sense under
   * these circumstances.
   */
  if (ews_state->context_current != NULL)
    {
#     ifdef EW_CONFIG_OPTION_PRIMARY
      /*
       * Special case:
       * If we are dealing with the response from a secondary server...
       */
      if (ews_state->context_current->is_secondary_context)
        {
          ewsDistFinish(ews_state->context_current->secondary,
                        EWS_STATUS_ABORTED);
        }
      else
#     endif /* EW_CONFIG_OPTION_PRIMARY */
        ewsNetHTTPAbort(ews_state->context_current);
    }
  estring->offset = ctxt.offset;
  estring->length = length;
  return EWA_NET_BUFFER_NULL;     /* failure */
# endif /* EW_CONFIG_OPTION_SCHED */
}

/*
 * ewsStringCopyOut
 * Copy a string from a chain of network buffers into a contiguous
 * null-terminated character string.
 *
 * dst          - pointer to contiguous memory for a null-terminated string
 * estring      - pointer to an EwsString describing the string to be copied.
 * maxbytes     - maximum number of bytes to copy, not including '\0' byte
 *
 * No return value.  On return, MIN(estring.length,maxbytes) bytes are
 * copied to dst + 1 for '\0'.
 */
void
ewsStringCopyOut ( char * dst, EwsStringP estring, uintf maxbytes )
{
  EwaNetBuffer  buffer;         /* current buffer */
  uint8         *src;           /* current source character pointer */
  uintf         bytes;          /* bytes remaining in string */
  uintf         remaining;      /* bytes remaining in current buffer */

  /* estring->first == NULL is a special case */
  if (EWA_NET_BUFFER_NULL == estring->first)
      {
      *dst++ = '\0';
        return;
      }

# ifdef EMWEB_SANITY
    if (estring->offset > ewaNetBufferLengthGet(estring->first))
      {
        EMWEB_ERROR(("ewsStringCopyOut: Corrupt offset.\n"));
        return;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Initialize state
     */
    buffer = estring->first;
    bytes = estring->length > maxbytes ? maxbytes : estring->length;
    remaining = ewaNetBufferLengthGet(buffer) - estring->offset;
    src = ewaNetBufferDataGet(buffer) + estring->offset;

    /*
     * While there are bytes to copy
     */
    while (bytes != 0)
      {
        /*
         * If no more bytes in this buffer, go to next buffer
         */
        while (remaining == 0)
          {
            buffer = ewaNetBufferNextGet(buffer);

#           ifdef EMWEB_SANITY
            if (buffer == EWA_NET_BUFFER_NULL)
              {
                EMWEB_ERROR(("ewsStringCopyOut: buffer corruption\n"));
                return;
              }
#           endif /* EMWEB_SANITY */

            src = ewaNetBufferDataGet(buffer);
            remaining = ewaNetBufferLengthGet(buffer);
          }

        /*
         * Copy bytes
         */
        if (remaining > bytes)
          {
            remaining = bytes;
          }
        EMWEB_MEMCPY(dst, src, remaining);
        bytes -= remaining;
        dst += remaining;
        src += remaining;
        remaining = 0;
      }

    *dst++ = '\0';
}

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
 * to the match character or EOF.  This allows efficient searches
 * as buffers are received by not restarting the search from the beginning each
 * time.  When the search terminates, estring will be a valid string from the
 * beginning to the matching character or EOF.
 *
 * CAVEAT: if the found match is the first character in the estring,
 *         estring->length does not get incremented.
 */
uintf
ewsStringSearch ( EwsStringP estring, EwsStringP next, const char *match_list,
  EwsStringSearchType incl_excl, uint32 *chunklenp )
{
  uint8         *cp;            /* character pointer */
  uintf         remaining;      /* bytes remaining in this buffer */
  EwaNetBuffer  buffer;         /* buffer pointer */
  uint8         *match;         /* pointer into match list */

#   ifdef EMWEB_SANITY
    if (next->first == EWA_NET_BUFFER_NULL)
      {
        EMWEB_ERROR(("ewsStringSearch: no buffer\n"));
        return EWS_STRING_EOF;
      }
    if (next->offset > ewaNetBufferLengthGet(next->first))
      {
        EMWEB_ERROR(("ewsStringSearch: Corrupt offset.\n"));
        return EWS_STRING_EOF;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Initialize state
     */
    cp = ewaNetBufferDataGet(next->first) + next->offset;
    remaining = ewaNetBufferLengthGet(next->first) - next->offset;

    /*
     * Search for matching characters
     */
    for (;;)
      {

        /*
         * If no more bytes in this buffer, advance to the next.  If end of
         * chain is reached, return EOF.
         */
        if (chunklenp != NULL && *chunklenp == 0) return EWS_STRING_EOF;
        while (remaining == 0)
          {
            buffer = ewaNetBufferNextGet(next->first);

            if (buffer == EWA_NET_BUFFER_NULL)
              {
                return EWS_STRING_EOF;
              }
            cp = ewaNetBufferDataGet(buffer);
            remaining = ewaNetBufferLengthGet(buffer);
            next->first = buffer;
            next->offset = 0;
          }

        /*
         * Check for matching character
         */
        if (incl_excl == ewsStringSearchInclude)
          {
            for (match = (uint8 *) match_list; *match != '\0'; match++)
              {
                if (*cp == *match)
                  {
                    return *cp;
                  }
               }
            }
          else
            {
              for (match = (uint8 *) match_list; *match != '\0'; match++)
                {
                  if (*cp == *match)
                    {
                      break;
                    }
                }
              if (*cp != *match)
                {
                  return *cp;
                }
            }

        /*
         * Continue search
         */
        cp++;
        remaining--;
        estring->length++;
        next->offset++;
        next->length--;
        if (chunklenp != NULL) (*chunklenp)--;
      }
}

/*
 * ewsStringCompare
 * Compares a contiguous null-terminated string with a string from a chain of
 * network buffers.
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
boolean
ewsStringCompare
  ( EwsStringP estring          /* CAUTION: gets modified! */
   ,const char *string
   ,EwsStringCompareCase case_type
   ,uint32 *chunklenp )
{
  EwaNetBuffer  buffer;         /* current buffer */
  uintf         remaining;      /* bytes remaining in current buffer */
  uint8         *cp;            /* pointer to character data in buffer */
  uintf         offset;         /* offset into buffer */
  uintf         length = 0;     /* length of string */
  uint8         c1, c2;         /* characters to compare, adjusted for case */

  /* verify good buffer - must be done outside of sanity check, as
   * this may happen in the real world
   */
  if ((buffer = estring->first) == EWA_NET_BUFFER_NULL)
    return FALSE;

#   ifdef EMWEB_SANITY
    if (string == NULL)
      {
        EMWEB_ERROR(("ewsStringCompare: no string\n"));
        return FALSE;
      }
    if (estring->offset > ewaNetBufferLengthGet(estring->first))
      {
        EMWEB_ERROR(("ewsStringCompare: Corrupt offset.\n"));
        return FALSE;
      }
#   endif /* EMWEB_SANITY */

    /*
     * Initialize state
     */
    offset = estring->offset;
    cp = ewaNetBufferDataGet(buffer) + offset;
    remaining = ewaNetBufferLengthGet(buffer) - offset;

    /*
     * While not at end of null-terminated contiguous string
     */
    while (*string != '\0')
      {
        /*
         * If no more bytes remaining in this buffer, advance to the next.
         * If no more buffers, we failed to match the string
         */
        if (chunklenp != NULL && *chunklenp == length) return FALSE;
        while (remaining == 0)
          {
            buffer = ewaNetBufferNextGet(buffer);
            if (buffer == EWA_NET_BUFFER_NULL)
              {
                return FALSE;
              }
            cp = ewaNetBufferDataGet(buffer);
            remaining = ewaNetBufferLengthGet(buffer);
            offset = 0;
          }

        /*
         * Get characters for comparison and adjust case for sensitivity
         */
        c1 = *cp;
        c2 = *string;
        if (case_type == ewsStringCompareCaseInsensitive)
          {
            if (c1 >= 'A' && c1 <= 'Z')
              {
                c1 += ('a' - 'A');
              }
            if (c2 >= 'A' && c2 <= 'Z')
              {
                c2 += ('a' - 'A');
              }
          }

        /*
         * If characters don't match, then we failed to match strings
         */
        if (c1 != c2)
          {
            return FALSE;
          }

        /*
         * Advance to next character and continue matching
         */
        cp++;
        string++;
        offset++;
        length++;
        remaining--;
      }

    /*
     * Match succeeded.  Update estring to point to character following
     * match and remove length of matched string.
     */
    estring->first = buffer;
    estring->offset = offset;
    estring->length -= length;
    if (chunklenp != NULL) *chunklenp -= length;
    return TRUE;
}

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
   ,EwsStringCompareCase case_type )
{
  char            c1 = 0;
  char            c2 = 0;

  /*
   * This implementation opts for efficiency in terms of space rather than
   * speed.  The calls to ewsStringGetChar() are somewhat slow, but this is
   * more space-efficient than manually manipulating the buffers.
   */

    do
      {
        /* Compare lengths */
        if (string1.length == 0 ||
            string2.length == 0 ||
            string1.length != string2.length)
          {
            /* Lengths don't match */
            break;
          }

        /* Get a character from each string */
        c1 = ewsStringGetChar(&string1);
        c2 = ewsStringGetChar(&string2);

        /* Convert to lower case */
        if (case_type == ewsStringCompareCaseInsensitive)
          {
            if (c1 >= 'A' && c1 <= 'Z')
              {
                c1 += ('a' - 'A');
              }
            if (c2 >= 'A' && c2 <= 'Z')
              {
                c2 += ('a' - 'A');
              }
          }
      } while (c1 == c2);

    /* Did we match for the entire length of both strings? */
    if (c1 == c2 && string1.length == string2.length)
      {
        return TRUE;
      }

    return FALSE;
}
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
 *
 * Note: estring->length is blindly decremented, and some code relies on that,
 * so don't check for a valid length here!
 */
uintf
ewsStringGetChar ( EwsStringP estring )
{
  EwaNetBuffer  buffer = estring->first;
  uint8         data;

  /* buffer == NULL is valid but needs to be handled specially. */
    if (buffer == EWA_NET_BUFFER_NULL)
      {
 
        return EWS_STRING_EOF;
      }

# ifdef EMWEB_SANITY
  if (estring->offset > ewaNetBufferLengthGet(buffer))
      {
        EMWEB_ERROR(("ewsStringGetChar: Corrupt offset.\n"));
        return EWS_STRING_EOF;
      }
#   endif /* EMWEB_SANITY */

    /*
     * If end of current buffer, advance to next buffer
     */
    while (estring->offset == ewaNetBufferLengthGet(buffer))
      {
        buffer = ewaNetBufferNextGet(buffer);
        if (buffer == EWA_NET_BUFFER_NULL)
          {
            return EWS_STRING_EOF;
          }
        estring->first = buffer;
        estring->offset = 0;
      }

    /*
     * Read data, advance offset, and return
     */
    data = *((uint8 *)ewaNetBufferDataGet(buffer) + estring->offset);
    estring->offset++;
    estring->length--;
    return (uintf) data;
}

/*
 * ewsStringGetInteger
 * Read characters from string in given base until end of string or invalid
 * character returned.
 *
 * estring      - pointer to EwsString
 * base         - base of string (2-16)
 * valuep       - output: value of string
 *
 * Returns TRUE on success, FALSE on parse error.  Advances estring->first
 * and estring->buffer to the character after value, decrementing
 * estring->length by the number of characters parsed.
 */
boolean
ewsStringGetInteger ( EwsStringP estring, uintf base, uint32 *valuep )
{
  EwaNetBuffer  buffer = estring->first;
  uintf         offset = estring->offset;
  uint32        value = 0, newValue; /* MUST be unsigned for compares to work! */
  boolean       minus = FALSE;
  uint8         byte;
  uintf         count = 0;
  uintf         good = 0;

#   ifdef EMWEB_SANITY
    if (buffer == EWA_NET_BUFFER_NULL)
      {
        return FALSE;
      }
    if (estring->offset > ewaNetBufferLengthGet(estring->first))
      {
        EMWEB_ERROR(("ewsStringGetInteger: Corrupt offset.\n"));
        return (boolean)EWS_STRING_EOF;
      }
#   endif /* EMWEB_SANITY */

    while (count != estring->length)
      {
        /*
         * If end of current buffer, advance to next buffer,
         * skip zero length buffers
         */
        while (offset == ewaNetBufferLengthGet(buffer))
          {
            buffer = ewaNetBufferNextGet(buffer);
            if (buffer == EWA_NET_BUFFER_NULL)
              {
                return FALSE;
              }
            offset = 0;
          }

        /*
         * Read next byte
         */
        byte = *(ewaNetBufferDataGet(buffer) + offset);

        /*
         * If first byte is minus sign, remember and continue
         */
        if (count == 0 && byte == '-')
          {
            minus = TRUE;
            offset++;
            count++;
            continue;
          }

        /*
         * Convert character to digit and check within base range
         */
        if (byte >= '0' && byte <= '9')
          {
            byte -= '0';
          }
        else if (byte >= 'A' && byte <= 'F')
          {
            byte -= 'A' - 10;
          }
        else if (byte >= 'a' && byte <= 'f')
          {
            byte -= 'a' - 10;
          }
        else
          {
            break;
          }
        if (byte >= base)
          {
            break;
          }

        /* Begin LVL7 Defect 5901 */

        /* Make sure the addition of this new byte will not cause overflow */

        /* 
         * 1) (value * base) must be less than or equal to MAXUINT. Therefore, MAXUINT/base
         *  must be greater than or equal to value. 
         * 2) (value*base)+byte must be less than or equal to MAXUINT.  Therefore, 
         * (MAXUINT-byte) must be greater than or equal to (value*base)
         */

#define MAXUINT 4294967295U        

        if ( ( MAXUINT/base < value ) ||      
             ( MAXUINT-byte < (value * base) ) )
        {
            good = 0;     
            break;
        }

        /* End LVL7 Defect 5901 */

        /*
         * Update value, and check for overflow.
         */
        newValue = value * base + byte;
                
        if (newValue < value)
          {
            good = 0;       /* bad - overflow */
            break;
          }
        value = newValue;

        offset++;
        count++;
        good++;
      }
    if (good == 0)
      {
        return FALSE;
      }

    if (minus)
      {
        /*
         * negative 32 bit numbers range
         * from -2,147,483,648 .. 2,147,483,647.
         * So if we attempt to convert 2,147,483,649 or
         * higher to a negative, then the results will not be defined.
         * Note that 2147483648 is 0x80000000H.  The
         * below non-obvious shifting is required to prevent warnings
         * from compilers who's int type is 16 bits....
         * ...trust me....
         */
        if (value > (((uint32)0x8000)<<16))  /* 2147483648 */
          {
            /*
             * another note:  This routine is used from many places throughout
             * the server.  In most cases, the caller is expecting unsigned
             * integer.  But in some (like the DECIMAL form input field converstion
             * code), it is expecting a signed number.
             * When the caller of this routine expects a *signed* integer, then
             * it is up to them to ensure that the user did not enter a
             * positive value > 2,147,483,647.  This routine does not
             * know if the numerical target is signed or unsigned (unless
             * a "-" is seen).  So this routine can only verify that the
             * max negative number has not been exceeded.  It is up to the
             * caller to verify that the max signed postive has not been
             * exceeded (see ewFieldFromDecimal in ews_form.c)
             */
            return FALSE;
          }
        *valuep = -(int32)value;
      }
    else
      {
        *valuep = value;
      }
    estring->offset = offset;
    estring->first = buffer;
    estring->length -= count;
    return TRUE;
}

/*
 * ewsStringRewriteChar
 * This function rewrites a single character to the string and advances the
 * the estring by one char (decrementing estring->length).
 *
 * estring      - input/output: first/offset indicate where to rewrite byte
 *
 * byte         - byte to rewrite
 *
 * No return value
 */
void
ewsStringRewriteChar ( EwsStringP estring, uint8 byte )
{
  EwaNetBuffer  buffer;         /* current buffer */

#   ifdef EMWEB_SANITY
    if (estring->offset > ewaNetBufferLengthGet(estring->first))
      {
        EMWEB_ERROR(("ewsStringRewriteChar: Corrupt offset.\n"));
        return;
      }
#   endif /* EMWEB_SANITY */

    while (estring->offset == ewaNetBufferLengthGet(estring->first))
      {
        buffer = ewaNetBufferNextGet(estring->first);

#       ifdef EMWEB_SANITY
        if (buffer == EWA_NET_BUFFER_NULL)
          {
            EMWEB_ERROR(("ewsStringRewriteChar: overwrite\n"));
            return;
          }
#       endif /* EMWEB_SANITY */

        estring->first = buffer;
        estring->offset = 0;
      }
    *(ewaNetBufferDataGet(estring->first) + estring->offset) = byte;
    estring->offset++;
    estring->length++;
}

/*
 * ewsStringUnescape
 * Translate EwsString contents converting '%' HEX HEX sequences into
 * corresponding single-octet values.  Updates estring->length accordingly.
 *
 * estring      - pointer to EwsString
 * plus         - ewsStringUnescapePlus2Space or ewsStringUnescapeNoPlus2Space
 *
 * No return value
 */
void
ewsStringUnescape ( EwsStringP estring, EwsStringUnescape_e plus2space )
{
  EwsString     old_estring;    /* old estring */
  EwsString     new_estring;    /* new (overwritten) estring */
  uintf         old_length;     /* original estring length */
  uintf         new_length;     /* new estring length */
  uint8         old_byte;       /* original byte value */
  uint8         new_byte;       /* new byte value */


      /*
       * Initialize state
       */
      old_estring = new_estring = *estring;
      old_length = estring->length;
      new_length = 0;

      while (old_length > 0)
        {
          old_byte = ewsStringGetChar(&old_estring);
          if (old_byte != '%')
            {
              if (old_byte == '+' && plus2space == ewsStringUnescapePlus2Space)
                {
                  old_byte = ' ';
                }
              ewsStringRewriteChar(&new_estring, old_byte);
              new_length++;
              old_length--;
            }
          else
            {
              if (old_length < 3)
                {
                  break;
                }
              old_byte = ewsStringGetChar(&old_estring);
              if (old_byte >= '0' && old_byte <= '9')
                {
                  new_byte = (old_byte - '0') << 4;
                }
              else if (old_byte >= 'A' && old_byte <= 'F')
                {
                  new_byte = ((old_byte - 'A') + 0x0a) << 4;
                }
              else
                {
                  new_byte = ((old_byte - 'a') + 0x0a) << 4;
                }
              old_byte = ewsStringGetChar(&old_estring);
              if (old_byte >= '0' && old_byte <= '9')
                {
                  new_byte |= (old_byte - '0');
                }
              else if (old_byte >= 'A' && old_byte <= 'F')
                {
                  new_byte |= (old_byte - 'A') + 0x0a;
                }
              else
                {
                  new_byte |= (old_byte - 'a') + 0x0a;
                }
              ewsStringRewriteChar(&new_estring, new_byte);
              new_length++;
              old_length -= 3;
            }
        }
    estring->length = new_length;
}



#if   defined( EW_CONFIG_FILE_METHODS ) \
   || defined( EW_CONFIG_OPTION_FIELDTYPE_FILE ) \
   || defined( EW_CONFIG_OPTION_TELNET) \
   || defined( EMWEB_XMLP )
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
 *
 * NOTE: If count is greater than available bytes in received buffer chain,
 * the string pointer is not advanced, and zero is returned.
 */
uintf
ewsStringTrim( EwsStringP estring, uintf count )
{
  uintf     bufBytes;
  uintf     bytesTrimmed;
  EwsString save;

# ifdef EMWEB_SANITY
  if (estring->first == EWA_NET_BUFFER_NULL)
    {
      EMWEB_ERROR(("ewsStringTrim: No string buffer.\n"));
      return 0;
    }
  if (estring->offset > ewaNetBufferLengthGet(estring->first))
    {
      EMWEB_ERROR(("ewsStringTrim: Corrupt offset.\n"));
      return 0;
    }
# endif /* EMWEB_SANITY */

  bytesTrimmed = count = ((count > estring->length)
                          ? estring->length
                          : count);

  save = *estring;
  while (count > (bufBytes = ewsStringAvailable(estring)))
    {
      count -= bufBytes;

      estring->first = ewaNetBufferNextGet( estring->first );
      estring->offset = 0;
      if (estring->first == EWA_NET_BUFFER_NULL)
        {
          *estring = save;
          return 0;
        }
    }

  estring->offset += count;
  estring->length -= bytesTrimmed;

  return( bytesTrimmed );
}
#endif


#if defined( EW_CONFIG_OPTION_FIELDTYPE_FILE )
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
boolean ewsStringSubstringSearch( EwsStringP estring
                                  ,EwsStringP end
                                  ,const char *string
                                  ,EwsStringCompareCase case_type
                                  ,uint32 *chunklenp )
{
  uintf stringLen;
  char      c1;

  EMWEB_STRLEN( stringLen, string );
  if (stringLen == 0)
    return TRUE;      /* Because the null string is contained in any string. */

  c1 = *string;

  if (case_type == ewsStringCompareCaseInsensitive)
    if ('A' <= c1 && c1 <= 'Z')
      c1 += 'a' - 'A';

  /*
   * spin until we find match or run out of data
   */

  while (chunklenp == NULL || *chunklenp >= stringLen)
    {
      char      c2;

      /* make sure we're dealing with a valid offset to data */

      while (end->offset == ewaNetBufferLengthGet( end->first ))
        {
          EwaNetBuffer  temp;

          if ((temp = ewaNetBufferNextGet( end->first ))
              == EWA_NET_BUFFER_NULL)
            {
              return FALSE;
            }
          end->first = temp;
          end->offset = 0;
        }

      c2 = *(ewaNetBufferDataGet( end->first ) + end->offset);

      if (case_type == ewsStringCompareCaseInsensitive)
        if ('A' <= c2 && c2 <= 'Z')
          c2 += 'a' - 'A';

      /*
       * if the first character of the search string matches
       * the current character, then begin a match of the
       * entire string
       */

      if (c1 == c2)
        {
          char          c3;
          const char    *ptr;
          EwaNetBuffer  buffer = end->first;
          uintf         offset = end->offset + 1;

          ptr = string + 1;

          while ((c3 = *ptr) != '\0')
            {
              while (offset == ewaNetBufferLengthGet( buffer ))
                {
                  if ((buffer = ewaNetBufferNextGet( buffer ))
                      == EWA_NET_BUFFER_NULL)
                    {
                      return FALSE;
                    }
                  offset = 0;
                }

              c2 = *(ewaNetBufferDataGet( buffer ) + offset++);

              if (case_type == ewsStringCompareCaseInsensitive)
                {
                if ('A' <= c2 && c2 <= 'Z')
                  c2 += 'a' - 'A';
                if ('A' <= c3 && c3 <= 'Z')
                  c3 += 'a' - 'A';
                }

              if (c3 != c2)     /* failure */
                break;

              ptr++;
            }

          if (*ptr == '\0')         /* match found! */
            {
              end->length = stringLen;
              if (chunklenp != NULL) *chunklenp -= stringLen;
              return TRUE;
            }
        }       /* if c1 == c2 */

      if (chunklenp != NULL) *chunklenp -= 1;
      estring->length++;
      end->offset++;
    }       /* end while not found and data available */

  return FALSE;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE */


#if defined(EW_CONFIG_OPTION_FIELDTYPE_FILE) || defined(EW_CONFIG_OPTION_PRIMARY)
/*
 * ewsStringWrite
 *
 * Writes the contents of estring to the given output file.  Advance
 * the input string by the number of characters written successfully.
 *
 *  estring - input/output: string to be written out.  It is advanced
 *              by the number of bytes successfully written.
 *  handle  - output: file handle to write to.
 *  type    - type of file (ewsStringWriteFile or ewsStringWriteSecondary)
 *  context - current context, used if file write suspends.
 *
 * Returns the number of bytes written, else <0 on error.  If suspend
 * functionality is supported, then context->schedulingState must
 * be checked for ewsContextSuspended on return.
 */
sintf
ewsStringWrite( EwsContext context
               ,void * handle
#               ifdef EW_CONFIG_OPTION_PRIMARY
               ,EwsStringWriteType type
#               endif /* EW_CONFIG_OPTION_PRIMARY */
               ,EwsString *estring )
{
  sintf bytes = 0;
  sintf status = 0;
  uintf length;

# ifdef EMWEB_SANITY
  if (estring->offset > ewaNetBufferLengthGet(estring->first))
    {
      EMWEB_ERROR(("ewsStringWrite: Corrupt offset.\n"));
      return -1;
    }
# endif /* EMWEB_SANITY */

  while (estring->length)           /* while bytes to write */
    {
      /* verify offset into buffer that has data */

      while (estring->offset == ewaNetBufferLengthGet( estring->first ))
        {
          if ((estring->first = ewaNetBufferNextGet( estring->first ))
              == EWA_NET_BUFFER_NULL)
            return bytes;

          estring->offset = 0;
        }

      /* send a chunk of bytes to the file */

      if ((length = ewsStringAvailable( estring )) > estring->length)
        length = estring->length;

#     ifdef EW_CONFIG_OPTION_PRIMARY
      switch(type)
        {
          case ewsStringWriteFile:
#     endif /* EW_CONFIG_OPTION_PRIMARY */

#           ifdef EW_CONFIG_OPTION_FIELDTYPE_FILE
#           ifdef EW_CONFIG_OPTION_FILE_SYSTEM_LIST
            if (context->inputFile->fileSystem == EWS_FILE_SYSTEM_HANDLE_NULL)
              {
                return status;        /* unrecoverable error */
              }
            if ((context->inputFile->fileSystem->fs.fileWrite) == NULL)
              {
                EMWEB_ERROR(("ewsStringWrite: NULL fs fileWrite function\n"));
                return status;        /* unrecoverable error */
              }
            if ((status = (*((context->inputFile->fileSystem->fs).fileWrite))(
#           else /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
            if ((status = ewaFileWrite(
#           endif /* EW_CONFIG_OPTION_FILE_SYSTEM_LIST */
                                        context
                                       ,(EwaFileHandle) handle
                                       ,(const uint8 *)ewsStringData( estring )
                                       ,length ))
                < 0)
              {
                return status;        /* unrecoverable error */
              }
#           endif /* EW_CONFIG_OPTION_FILE */

#     ifdef EW_CONFIG_OPTION_PRIMARY
            break;

          case ewsStringWriteSecondary:
            if ((status = ewaSecondaryData((EwaSecondaryHandle) handle
                                           ,context
                                           ,ewsStringData( estring )
                                           ,length ))
                < 0)
              {
                return status; /* unrecoverable error */
              }

        }
#     endif /* EW_CONFIG_OPTION_PRIMARY */

      bytes += status;
      estring->offset += status;
      estring->length -= status;

      /*
       * The user can suspend (and return status > 0)
       * or abort.
       */


      if (   context->abort_flags & EWS_CONTEXT_FLAGS_ABORT
#         ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
          || context->schedulingState == ewsContextSuspended
#         endif  /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
         )
        {
          return bytes;             /* done for now */
        }

      /* ensure not resuming */

#     ifdef EW_CONFIG_OPTION_SCHED_SUSP_RES
      context->schedulingState = ewsContextScheduled;
#     endif  /* EW_CONFIG_OPTION_SCHED_SUSP_RES */
    }

  return bytes;
}
#endif /* EW_CONFIG_OPTION_FIELDTYPE_FILE || PRIMARY */

#ifndef EMWEB_HAVE_STRCHR
char* ews_strchr( const char* s, char c )
{
  char* found;

  for ( found = NULL;
        found == NULL && *s != '\0';
        s++
       )
    {
      if ( *s == c )
        {
          found = (char *) s;
        }
    }

  return found;
}
#endif /* ! EMWEB_HAVE_STRCHR */

#ifndef EMWEB_HAVE_STRSTR
char *ews_strstr(const char *s1, const char *s2)
{
  const char *p = NULL;
  const char *s2_start = s2;

  while( *s1 != '\0' && *s2 != '\0' )
    {
      if(*s1 == *s2)   /* match */
        {
          if(p == NULL)   /* first match - record start position */
            {
              p = s1;
            }
          s1++;
          s2++;
        }
      else   /* no match */
        {
          if (p != NULL)   /* had previous match */
            {
              s1 = p + 1;    /* start s1 at char after previous match */
              s2 = s2_start;   /* start s2 at beginning */
            }
          else   /* no previous match */
            {
              s1++;
            }
          p = NULL;   /* forget any previous match */
        }
    }

  if(*s2 == '\0' && p != '\0')
    {
      return (char *) p;
    }
  else
    {
      return NULL;
    }
}
#endif /* ! EMWEB_HAVE_STRSTR */

/*
 * Remove leading spaces and tabs or trailing spaces from the passed string,
 * based on conditions.
 */
char* ewsStringTrimOptions (char *s, boolean trim_lead, boolean trim_trail)
{
  char *sp;
  char *tp;
  uintf len;

  sp = s;
  EMWEB_STRLEN (len, sp);
  tp = sp + len - 1;

  if (trim_trail)
    {
      while (*tp == ' ')
        {
          tp--;
        }
      *++tp = '\0';
    }
  if (trim_lead)
    {
      while (*sp == ' ' || *sp == '\t')
        {
          sp++;
        }
      EMWEB_STRLEN (len, sp);
      EMWEB_MEMCPY (s, sp, len + 1);
    }
  return (s);
}

/*
 * Remove leading whitespace from the passed string.
 */
char* ewsStringTrimLeading (char *s)
{
  return (ewsStringTrimOptions ((char *)s, 1, 0));
}

/*
 * Remove trailing whitespace from the passed string.
 */
char* ewsStringTrimTrailing (char *s)
{
  return (ewsStringTrimOptions ((char *)s, 0, 1));
}

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
                    int maxlen)
{
    /* If the EwsString's length is zero... */
    if (s.length == 0)
    {
        /* ... then use the default string instead. */
        EMWEB_STRNCPY(pBuf, ifEmpty, maxlen - 1);
    }
    else
    {
        /* We have a valid EwsString.  Copy it into the buffer. */
        ewsStringCopyOut(pBuf, &s, maxlen - 1);
    }

    /* Null-terminate the string (in case we used the maxlen) */
    pBuf[maxlen - 1] = '\0';

    /* Give 'em what they came for */
    return pBuf;
}

#ifdef STR_DEBUG
/* This function may be called from the debugger */
char *
ewsStringPrintable(EwsString s)
{
    static char     buf[16768];

    return ewsStringContiguous(s, "*** EMPTY ***", buf, sizeof(buf));
}
#endif
