/*
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
 * EmWeb Decompression
 *
 */

#ifdef HAVE_CONFIG_H
#  include "emweb_config.h"
#endif

#include "ew_config.h"
#include "ew_cmp_i.h"
#include "ews_sys.h"
#include "ew_common.h"

/*
 * EmWeb/Decompress
 *
 * The EmWeb compression technique was designed with the following objectives:
 *
 *   1. Minimum run-time memory requirement
 *   2. Maximum compression of data
 *   3. Avoid algorithms patented by others (e.g. LZ77, LZ78, etc.)
 *
 * We examined some of the patented algorithms.  LZ77 was ruled out as
 * requiring too much run-time memory (a 32K sliding window into the
 * uncompressed data is required.  Since we don't hold on to the uncompressed
 * data as it is transmitted to the network, we would have to keep a copy
 * of data in a 32K buffer).  LZ78 requires some run-time memory to build
 * a dictionary, but the dictionary size is managable (3*2^N bytes, where N
 * is between 9 and 16 -- 11-12 being optimal for most HTML files).  However,
 * it was ruled out because of the patented algorithm (Unisys requires a
 * royalty based on percentage of the final product (as built by our customers)
 * price.
 *
 * It turns out that compression ratios slightly better than LZ78 (data
 * reduction of approximately 40%) can be achieved by constructing a dictionary
 * of common strings observed throughout the archive, and referencing this
 * dictionary from individual documents.
 *
 * The compression algorithm runs over the input data in two passes.  The first
 * pass is used to build the dictionary, and the second pass uses the
 * dictionary to compress the individual documents.
 *
 * The dictionary is represented as follows:
 *
 *  {
 *     EwCompressDictionaryHeader       header;
 *     uint8                            string_offset_lo [ index_size ];
 *     uint8                            string_offset_hi [ index_size ];
 *     uint8                            string_length    [ index_size ];
 *     uint8                            dictionary       [ dictionary_size ];
 *  }
 *
 * The dictionary is a contiguous array of characters representing repeated
 * strings found in the archive.  The 16-bit offset (split into hi and lo
 * components for endian-independence) index into the dictionary to represent
 * the start of a string.  The 8-bit length indicates the size of the string
 * in bytes.  The string offset and length arrays themselves are indexed by
 * (codeword_value - 256) where codewords 0-255 are reserved to indicate
 * literal characters.
 */

/******************************************************************************
 * DECOMPRESSION API
 *
 * ewDecompressBegin            - initialize decompression context
 * ewDecompressRead             - read uncompressed data
 * ewDecompressInput            - (application-provided)
 * ewDecompressSaveContext      - save string pointer and remaining count
 * ewDecompressRestoreContext   - restore string pointer and remaining count
 *
 * The application would begin decompression by passing a dictionary pointer
 * and handle to ewDecompressBegin in order to allocate a decompression
 * context.  Then, the application can read uncompressed data into memory
 * by invoking ewDecompressRead zero or more times.  In turn, this function
 * invokes ewDecompressInput to request compressed data one byte at a time
 * from the application.
 *
 * On completion, the application simply releases the context with ewaFree().
 */

/*
 * ewDecompressBegin
 * Allocate resources for new decompression and return context
 *
 * dictionary   - pointer to string dictionary header
 * handle       - application-specific handle
 *
 * Returns pointer to decompression context, or NULL if no resources available
 */

EwDecompressContext
ewDecompressBegin ( const uint8 * dictionary, void * handle )
{
  const EwCompressDictionaryHeader *header =
    (const EwCompressDictionaryHeader *) dictionary;
  EwDecompressContext context;

    /*
     * Allocate and initialize context.
     */
    context = (EwDecompressContext) ewaAlloc(sizeof(*context));
    if (context != (EwDecompressContext) NULL)
      {
        context->dictionary = (const EwCompressDictionaryHeader *) dictionary;
        context->index_size = EW_BYTES_TO_UINT16(header->index_size);
        context->code_size = header->code_size;
        context->string_offset_lo = (const uint8 *) &header[1];
        context->string_offset_hi =
          &context->string_offset_lo[context->index_size];
        context->string_length =
          &context->string_offset_hi[context->index_size];
        context->string_data = &context->string_length[context->index_size];
        context->code = 0;
        context->bits = 0;
        context->remaining = 0;
        context->handle = handle;
      }
    return context;
}

/*
 * ewDecompressRead
 * Read uncompressed data from decompression context into a memory buffer
 *
 * context      - decompression context
 * datap        - pointer to data buffer, if NULL, then "length" bytes
 *                of uncompressed data is "skipped"
 * bytes        - length of data buffer
 *
 * Returns number of bytes actually decompressed
 */
uintf
ewDecompressRead ( EwDecompressContext context, uint8 *datap, uint32 length )
{
  uintf code = 0;
  uintf bytes = 0;
  sintf byte;

    /*
     * While the application's request to decompress data has not yet been
     * fulfilled
     */
    while (length > 0)
      {
        /*
         * If decompressing a matching string, read from dictionary
         */
        if (context->remaining > 0)
          {
            if (datap != NULL)
              {
                *datap++ = *context->stringp;
              }
            context->stringp++;
            context->remaining--;
            length--;
            bytes++;
            continue;
          }

        /*
         * Read the next byte from the compressed data stream and check for EOF
         */
        byte = ewDecompressInput ( context->handle );
        if (byte < 0)
          {
            return bytes;
          }

        /*
         * Shift byte into codeword buffer.  If a codeword has not been
         * completed, continue
         */
        context->code |= byte << context->bits;
        context->bits += 8;
        if (context->bits < context->code_size)
          {
            continue;
          }

        /*
         * Extract a codeword from the bit buffer
         */
        code = context->code & ((1 << context->code_size) - 1);
        context->code >>= context->code_size;
        context->bits -= context->code_size;

        /*
         * If code is literal, then output byte and continue
         */
        if (code < 256)
          {
            if (datap != NULL)
              {
                *datap++ = code;
              }
            length--;
            bytes++;
            continue;
          }

        /*
         * Otherwise, lookup string
         */
        context->remaining = context->string_length[code - 256];
        context->stringp = &context->string_data
          [ ((uintf)context->string_offset_hi[code - 256] << 8) |
            (context->string_offset_lo[code - 256]) ];
      }

    return bytes;
}

#if defined(EW_CONFIG_OPTION_FORM_REPEAT) \
 || defined(EW_CONFIG_OPTION_ITERATE)
/*
 * ewDecompressSaveContext
 * Save string pointer and remaining count
 *
 * context      - decompression context
 * save         - stores snapshot of current context.
 *
 * return - void
 */
void
ewDecompressSaveContext ( EwDecompressContext context,
                          EwDecompressSaveState save )
{
  save->stringp = context->stringp;
  save->remaining = context->remaining;
  save->index_size = context->index_size;
  save->code_size = context->code_size;
  save->code = context->code;
  save->bits = context->bits;
}

/*
 * ewDecompressRestoreContext
 * Restore string pointer and remaining count
 *
 * context      - decompression context
 * restore      - old state that is to be restored to context
 *
 * return - none,
 */
void
ewDecompressRestoreContext ( EwDecompressContext context
                            ,EwDecompressSaveState restore)
{
  context->stringp = restore->stringp;
  context->remaining = restore->remaining;
  context->index_size = restore->index_size;
  context->code_size = restore->code_size;
  context->code = restore->code;
  context->bits = restore->bits;
}
#endif /* EW_CONFIG_OPTION_FORM_REPEAT/ITERATE */
