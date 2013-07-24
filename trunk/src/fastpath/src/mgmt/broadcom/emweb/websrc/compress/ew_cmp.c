/*
 * Product:  EmWeb
 * Release:  R6_2_0
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
 * EmWeb Compression
 *
 */

#include <stdlib.h>

#include "ewc_config.h"
#include "ew_cmp_i.h"

/*
 * EmWeb/Compress
 *
 * The EmWeb compression technique was designed with the following objectives:
 *
 *   1. Minimum run-time memory requirement
 *   2. Maximum compression of data
 *   3. Avoid algorithms patented by others (e.g. LZ77, LZ78, etc.)
 *
 * We examined some of the patented algorithms.  LZ77 was ruled out as
 * requiring too much run-time memory.  (A 32K sliding window into the
 * uncompressed data is required.  Since we don't hold on to the uncompressed
 * data as it is transmitted to the network, we would have to keep a copy
 * of data in a 32K buffer).  LZ78 requires some run-time memory to build
 * a dictionary, but the dictionary size is managable (3*2^N bytes, where N
 * is between 9 and 16 -- 11-12 being optimal for most HTML files).  However,
 * it was ruled out because of the patented algorithm.  (Unisys requires a
 * royalty based on percentage of the final product (as built by our customers)
 * price.)
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
 * COMPRESSION API
 *
 * ewCompressDictionaryOpen     - initialize compression dictionary
 * ewCompressDictionaryWrite    - pass data to dictionary constructor (pass 1)
 * ewCompressDictionaryCreate   - create dictionary from state
 * ewCompressDictionaryClose    - close constructed dictionary
 * ewCompressBegin              - begin compression of a new file
 * ewCompressWrite              - pass data to compressor (pass 2)
 * ewCompressEnd                - complete compression of a file
 * ewCompressOutput             - (application-provided)
 *
 * The application would first invoke ewCompressDictionaryOpen() to start a
 * new archive.  Then, invoke ewCompressDictionaryWrite() zero or more times
 * to pass all archive data to be compressed through the dictionary constructor
 * in a first pass.  Then, invoke ewCompressDictionaryCreate() to create a
 * dictionary from the first pass data stream.
 *
 * Next, for each file to be compressed, the application would first invoke
 * ewCompressBegin to pass an application-specific handle to the compressor.
 * Then, it would call ewCompressWrite zero or more times to pass the file
 * contents through the compressor.  Finally, it would call ewCompressEnd
 * to complete compression of the file.  The application-provided
 * ewCompressOutput function is invoked for each byte of compressed data
 * output by the compressor.
 *
 * Finally, the application invokes ewCompressDictionaryClose to release
 * resources assigned to the dictionary.
 */

/*
 * Forward declarations for internal functions
 */
void ewCompressProcess( EwCompressContext context );
uintf ewCompressMatch ( EwCompressContext context );
void ewCompress ( EwCompressContext context );
void ewCompressGenerateCode ( EwCompressContext context, uintf code );

/*
 * ewCompressDictionaryOpen
 * Start a new compression dictionary
 *
 * endian       - ewBigEndian or ewLittleEndian format for output
 *
 * Returns a new compression context, or NULL if resources not available.
 */
EwCompressContext
ewCompressDictionaryOpen ( Endian endian )
{
  EwCompressContext context;
  uintf i;

    /*
     * Allocate and initialize context
     */
    context = (EwCompressContext) ewaAlloc(sizeof(*context));
    if (context != (EwCompressContext) NULL)
      {
        for (i = 0; i < EW_COMPRESS_BUFFER_SIZE; i++)
          {
            context->buffer_hash_next[i] = EW_COMPRESS_BUFFER_HASH_NULL;
          }
        for (i = 0; i < EW_COMPRESS_BUFFER_HASH_SIZE; i++)
          {
            context->buffer_hash[i] = EW_COMPRESS_BUFFER_HASH_NULL;
          }
        for (i = 0; i < EW_COMPRESS_OFFSET_HASH_SIZE; i++)
          {
            context->offset_hash[i] = (EwCompressOffsetNodeP) NULL;
          }
        context->endian = endian;
        context->buffer_data_in = 0;
        context->buffer_data_out = 0;
        context->string_count = 0;
        context->dictionary = (EwCompressDictionaryHeaderP) NULL;
      }
    return context;
}

/*
 * ewCompressDictionaryWrite
 * Pass data through dictionary constructor (pass 1)
 *
 * context      - compression context
 * datap        - pointer to data to be scanned for dictionary construction
 * bytes        - length of data
 *
 * No return value
 */
void
ewCompressDictionaryWrite
  ( EwCompressContext context, const uint8 *datap, uint32 bytes )
{
  uintf available = EW_COMPRESS_BUFFER_SIZE - context->buffer_data_in;

    /*
     * If buffer is full, attempt to pack dictionary strings to make room
     * for more data.
     */
    if (available == 0)
      {
        ewCompressProcess(context);
        available = EW_COMPRESS_BUFFER_SIZE - context->buffer_data_in;
      }

    /*
     * While the dictionary is not full and there are bytes to run through it
     */
    while (available != 0 && bytes > 0)
      {
        /*
         * Copy available data from application into buffer and update state
         */
        if (available > bytes)
          {
            available = bytes;
          }
        memcpy(&context->buffer[context->buffer_data_in], datap, available);
        bytes -= available;
        datap += available;
        context->buffer_data_in += available;

        /*
         * If there is still application data to handle, process the
         * buffer.  This may cause the buffer to be packed pushing matching
         * strings discovered so far to the beginning of the buffer and making
         * room for additional input data.
         */
        if (bytes > 0)
          {
            ewCompressProcess(context);
            available = EW_COMPRESS_BUFFER_SIZE - context->buffer_data_in;
          }
      }
}

/*
 * ewCompressDictionaryCreate
 * Complete dictionary construction process.  The final dictionary is
 * created, and a pointer and length to the dictionary is returned.  The
 * dictionary includes the header, index, and data formatted as described
 * above.
 *
 * context      - compression context
 * datapp       - Output: pointer to raw dictionary database
 * bytesp       - Output: size of raw dictionary database
 *
 * No return value
 */
void
ewCompressDictionaryCreate
  ( EwCompressContext context, uint8 **datapp, uint32 *bytesp )
{
  EwCompressDictionaryHeaderP header;
  EwCompressOffsetNodeP onp;
  uintf i, j, k;
  uintf code = 0;
  uintf mask;

    /*
     * Process any data that was copied into the buffer but not yet scanned
     * for new repeating strings.
     */
    ewCompressProcess(context);

    /*
     * Allocate a dictionary
     */
    context->dictionary = (EwCompressDictionaryHeaderP)
      ewaAlloc( sizeof(*header) +
                context->buffer_data_in +
                3 * context->string_count );
    if (context->dictionary == NULL)
      {
        fprintf(stderr, "ewCompressDictionaryCreate: out of memory\n");
        exit(1);
      }
    context->string_offset_lo = (uint8 *) &context->dictionary[1];
    context->string_offset_hi =
      &context->string_offset_lo[context->string_count];
    context->string_length = &context->string_offset_hi[context->string_count];
    context->string_data = &context->string_length[context->string_count];

    /*
     * Copy dictionary data
     */
    memcpy(context->string_data, context->buffer, context->buffer_data_in);

    /*
     * For each OffsetNode
     */
    for (i = 0; i < EW_COMPRESS_OFFSET_HASH_SIZE; i++)
      {
        for (onp = context->offset_hash[i]; onp != NULL; onp = onp->next)
          {

            /*
             * For each string length
             */
            onp->first_code = code;
            for (j = 0; j < (EW_COMPRESS_MAX_MATCH >> 3)+1; j++)
              {
                if (onp->length_table[j] == 0)
                  {
                    continue;
                  }
                for (mask = 1, k =0 ; k < 8; mask = mask << 1, k++)
                  {

                    /*
                     * If a string is defined of a particular length at this
                     * offset into the dictionary, allocate a codeword to
                     * represent the substring
                     */
                    if ((onp->length_table[j] & mask) != 0)
                      {
                        context->string_offset_lo[code] = onp->offset & 0xff;
                        context->string_offset_hi[code] = (onp->offset>>8)&0xff;
                        context->string_length[code] = 1 + (j << 3) + k;
                        code++;
                      }
                  }
              }
            onp->last_code = code;
          }
      }

    /*
     * Calculate codeword size
     */
    code += 256;
    for (i = 8; i <= 16; i++)
      {
        if ((((uintf)1) << i) >= code)
          {
            break;
          }
      }

    /*
     * Write dictionary header
     */
    context->dictionary->code_size = i;
    context->dictionary->version = EW_COMPRESS_DICTIONARY_VERSION;

    EWC_SET_UINT16( context->endian
                   ,context->dictionary->index_size
                   ,context->string_count
                   );
    EWC_SET_UINT16( context->endian
                   ,context->dictionary->reserved
                   ,0
                   );
    EWC_SET_UINT16( context->endian
                   ,context->dictionary->dict_size
                   ,context->buffer_data_in
                   );

    /*
     * Return pointer and length
     */
    *datapp = (uint8 *) context->dictionary;
    *bytesp = sizeof(*context->dictionary) +
              context->buffer_data_in +
              3 * context->string_count;
}

/*
 * ewCompressDictionaryClose
 * Release dictionary resources
 *
 * context      - compression context
 *
 * No return value
 */
void
ewCompressDictionaryClose ( EwCompressContext context )
{
  uintf i;
  EwCompressOffsetNodeP onp, onp2;

    /*
     * Release offset nodes by scanning hash table
     */
    for (i = 0; i < EW_COMPRESS_OFFSET_HASH_SIZE; i++)
      {
        onp = context->offset_hash[i];
        while (onp != (EwCompressOffsetNodeP) NULL)
          {
            onp2 = onp;
            onp = onp->next;
            ewaFree(onp2);
          }
      }

    /*
     * Free the dictionary if it was constructed
     */
    if (context->dictionary != NULL)
      {
        ewaFree(context->dictionary);
      }

    /*
     * Free the context
     */
    ewaFree(context);
}

/*
 * ewCompressBegin
 * Begin compression of a file (during second pass after opening, first pass
 * data writing, and closing of the dictionary).
 *
 * context      - compression context
 * handle       - application-defined handle corresponding to the file being
 *                written.
 *
 * No return value
 */
void
ewCompressBegin ( EwCompressContext context, void * handle )
{
    /*
     * Reset state for new file
     */
    context->buffer_data_in = context->buffer_data_out = 0;
    context->handle = handle;
    context->byte = 0;
    context->bits = 0;
}

/*
 * ewCompressWrite
 * Pass data through compressor (pass 2).  This function may call the
 * application-provided function ewCompressOutput() to pass the compressed
 * data stream back to the application.
 *
 * context      - compression context
 * datap        - pointer to data to be compressed
 * bytes        - length of data
 *
 * No return value
 */
void
ewCompressWrite ( EwCompressContext context, const uint8 *datap, uint32 bytes )
{
  uintf available = EW_COMPRESS_BUFFER_SIZE - context->buffer_data_in;

    /*
     * If buffer is full, process current contents and reset state to empty
     */
    if (available == 0)
      {
        ewCompress(context);
        context->buffer_data_in = 0;
        context->buffer_data_out = 0;
        available = EW_COMPRESS_BUFFER_SIZE;
      }

    /*
     * While the dictionary is not full and there are bytes to run through it
     */
    while (available != 0 && bytes > 0)
      {
        /*
         * Copy available data from application into buffer and update state
         */
        if (available > bytes)
          {
            available = bytes;
          }
        memcpy(&context->buffer[context->buffer_data_in], datap, available);
        bytes -= available;
        datap += available;
        context->buffer_data_in += available;

        /*
         * If there is still application data to handle, process the
         * buffer.
         */
        if (bytes > 0)
          {
            ewCompress(context);
            available = EW_COMPRESS_BUFFER_SIZE;
            context->buffer_data_in = 0;
            context->buffer_data_out = 0;
          }
      }
}

/*
 * ewCompressEnd
 * Complete compression of file.  This function may call the application-
 * provided function ewCompressOutput() to pass the remaining compressed
 * data stream back to the application.
 *
 * context      - compression context
 *
 * No return value
 */
void
ewCompressEnd ( EwCompressContext context )
{
    ewCompress(context);
    if (context->bits != 0)
      {
        ewCompressOutput(context->handle, (uint8)context->byte);
      }
}

/*
 * ewCompressProcess (internal)
 * Search for matching strings in the newly buffered data, and pack the buffer
 * down by moving matching strings (ultimately appearing in dictionary) down
 * to the beginning of the buffer.
 *
 * context      - compression context
 *
 * No return value
 */
void
ewCompressProcess( EwCompressContext context )
{
  uintf i, j;
  uintf next_offset = 0;
  uintf last_match = 0;
  uintf delta = 0;
  uintf ohash;
  uintf bhash;
  EwCompressOffsetNodeP onp, prev_onp;
  EwCompressOffsetNodeP new_hash[EW_COMPRESS_OFFSET_HASH_SIZE];
  uint32 weight;
  uintf bits;

    /*
     * Scan new data in buffer looking for matching strings.
     */
    while (context->buffer_data_out < context->buffer_data_in)
      {
        /* Find the best match available from this location and increment
         * buffer_data_out beyond it.  (If ewCompressMatch finds no match,
         * it returns 1, which steps buffer_data_out to the next character.
         */
        context->buffer_data_out += ewCompressMatch(context);
      }

    /*
     * Calculate current estimation of codeword size from number of matching
     * strings
     */
    for (bits = 9; bits < 16; bits++)
      {
        if ((((uintf)1) << bits) >= context->string_count + 256)
          {
            break;
          }
      }

    /*
     * Pack matching strings to beginning of buffer.  We scan the buffer for
     * matching strings skipping unmatched strings, and move matching strings
     * down to be contiguous with previously matching strings.
     */
    for (i = 0; i < context->buffer_data_in; i++)
      {
        /*
         * Look for offset node in hash table corresponding with buffer offset
         */
        ohash = EW_COMPRESS_OFFSET_HASH(i);
        prev_onp = NULL;
        for (onp = context->offset_hash[ohash];
             onp != NULL;
             prev_onp = onp, onp = onp->next)
          {
            if (onp->offset == i)
              {
                break;
              }
          }
        if (onp == (EwCompressOffsetNodeP) NULL)
          {
            continue;
          }

        /*
         * Calculate weight of including strings contained at this offset
         * in the dictionary, in bits
         */
        weight = (i >= next_offset)? onp->max_length * 8 : 0;   /* string */
        weight += 24 * onp->length_count;       /* 24 bits overhead/code */

        /*
         * If weight is not high enough, discard node
         */
        if (onp->weight_refs * bits + onp->weight_bytes * 8 < weight)
          {
            if (prev_onp == NULL)
              {
                context->offset_hash[ohash] = onp->next;
              }
            else
              {
                prev_onp->next = onp->next;
              }
            context->string_count -= onp->length_count;
            ewaFree(onp);
            continue;
          }

        /*
         * If we aren't yet at an offset to look for non-contiguous strings,
         * update the offset of this onode to reflect that it is a substring
         * of a previously matched string.
         */
        if (i < next_offset)
          {
            onp->offset -= delta;
            continue;
          }

        /*
         * Move longest matching string at offset down to end of last match.
         * Since this may overlap, it is safer to do this byte-by-byte rather
         * than depend upon memcpy doing the right thing!
         */
        {
          uint8 *cp1 = &context->buffer[last_match];
          uint8 *cp2 = &context->buffer[i];

            j = onp->max_length;
            while (j-- > 0)
              {
                *cp1++ = *cp2++;
              }
        }

        /*
         * Update offsets for intermediate onodes
         */
        delta = onp->offset - last_match;

        /*
         * Update last_match and offset index
         */
        onp->offset -= delta;
        last_match += onp->max_length;
        next_offset = i + onp->max_length;
      }

    /*
     * Offsets 0..(last_match-1) now contain packed strings.  The offset and
     * buffer hash tables are completely incorrect and need to be rewritten.
     * First, we clear the buffer hash table and the offset hash table.
     */
    for (i = 0; i < EW_COMPRESS_BUFFER_HASH_SIZE; i++)
      {
        context->buffer_hash[i] = EW_COMPRESS_BUFFER_HASH_NULL;
      }
    for (i = 0; i < EW_COMPRESS_BUFFER_SIZE; i++)
      {
        context->buffer_hash_next[i] = EW_COMPRESS_BUFFER_HASH_NULL;
      }
    for (i = 0; i < EW_COMPRESS_OFFSET_HASH_SIZE; i++)
      {
        new_hash[i] = (EwCompressOffsetNodeP) NULL;
      }

    /*
     * Now we rewrite the offset hash table.  This is tricky as the offset
     * nodes are all singly linked to the current table.  We need to create a
     * new table and copy it back.  We also update the buffer hash.
     */
    for (i = 0; i < EW_COMPRESS_OFFSET_HASH_SIZE; i++)
      {
        while ((onp = context->offset_hash[i]) != (EwCompressOffsetNodeP) NULL)
          {
            bhash = EW_COMPRESS_BUFFER_HASH(&context->buffer[onp->offset]);
            context->buffer_hash_next[onp->offset]=context->buffer_hash[bhash];
            context->buffer_hash[bhash] = onp->offset;
            context->offset_hash[i] = onp->next;
            ohash = EW_COMPRESS_OFFSET_HASH(onp->offset);
            onp->next = new_hash[ohash];
            new_hash[ohash] = onp;
          }
      }
    memcpy(context->offset_hash, new_hash, sizeof(context->offset_hash));

    /*
     * Finally, we update the context for any new data that might be processed.
     */
    context->buffer_data_in = context->buffer_data_out = last_match;
}

/*
 * ewCompressMatch (internal)
 * Search for matching strings that match the string beginning at
 * buffer_data_out, not exceeding EW_COMPRESS_MAX_MATCH nor buffer_data_in.
 * If a match is found, update (and possibly create) offset node.
 *
 * context      - compression context
 *
 * Returns size of match, or 1 if no match was found.
 */
uintf
ewCompressMatch ( EwCompressContext context )
{
  uintf hash;
  uintf match_offset;
  uintf search_count = 0;
  uint8 *cp;
  uint8 *matchp;
  uintf match_length;
  uintf max_match = 0;
  uintf max_match_offset = 0;
  EwCompressOffsetNodeP onp;
  uintf i;
  uintf mask;

    /*
     * Lookup current string in hash table.  If not found, add to hash table
     * and return.
     */
  /* What if there is only one byte at buffer_data_out? */
    hash = EW_COMPRESS_BUFFER_HASH(&context->buffer[context->buffer_data_out]);
    match_offset = context->buffer_hash[hash];
    if (match_offset == EW_COMPRESS_BUFFER_HASH_NULL)
      {
        context->buffer_hash[hash] = context->buffer_data_out;
        return 1;
      }

    /*
     * Scan hash chain (up to EW_COMPRESS_MAX_SEARCH for speed improvement)
     * looking for best match (in length of matching characters).
     */
    while (search_count++ != EW_COMPRESS_MAX_SEARCH &&
           match_offset != EW_COMPRESS_BUFFER_HASH_NULL)
      {
        /*
         * See how long this match is.  (Limit the matching string to
         * be before buffer_data_out, so the matching string does not
         * overlap the string we are currently processing.)
         */
        cp = &context->buffer[context->buffer_data_out];
        matchp = &context->buffer[match_offset];
        match_length = 0;
        while (*cp == *matchp &&
               match_length < EW_COMPRESS_MAX_MATCH &&
               match_length + context->buffer_data_out <
                 context->buffer_data_in &&
               match_length + match_offset < context->buffer_data_out)
          {
            cp++;
            matchp++;
            match_length++;
          }

        /*
         * If this is the best match so far, save it.  Since we are traversing
         * the hash chain from largest offset to smallest offset, we want to
         * match the smaller of equal matches to keep the dictionary from
         * repeating itself.  So, we compare for >=, not >.
         */
        if (match_length >= max_match)
          {
            max_match = match_length;
            max_match_offset = match_offset;

            /*
             * Stop if it doesn't get better than this!
             */
            if (match_length == EW_COMPRESS_MAX_MATCH ||
                match_length + context->buffer_data_out ==
                  context->buffer_data_in)
              {
                break;
              }
          }

        /*
         * Get next match
         */
        match_offset = context->buffer_hash_next[match_offset];
      }

    /*
     * If a good-enough match was not found, add this as a potential location
     * to be matched by later data and return.
     */
    if (max_match < EW_COMPRESS_MIN_MATCH)
      {
        /*
         * Add current location to the hash table.
         */
        match_offset = context->buffer_hash[hash];
        context->buffer_hash_next[context->buffer_data_out] = match_offset;
        context->buffer_hash[hash] = context->buffer_data_out;
        return 1;
      }

    /*
     * We have selected the best match.  Now we need to update (or possibly
     * create) an offset node corresponding to the offset of the matching
     * string.
     */
    hash = EW_COMPRESS_OFFSET_HASH(max_match_offset);
    onp = context->offset_hash[hash];
    while (onp != (EwCompressOffsetNodeP) NULL &&
           onp->offset != max_match_offset)
      {
        onp = onp->next;
      }

    /*
     * If there isn't already a node for the offset of the matching string,
     * create one now.
     */
    if (onp == (EwCompressOffsetNodeP) NULL)
      {
        onp = (EwCompressOffsetNodeP) ewaAlloc(sizeof(EwCompressOffsetNode));
        if (onp == NULL)
          {
            fprintf(stderr, "ewCompressMatch: no resources\n");
            return max_match;
          }
        /* Add to the correct chain in context->offset_hash. */
        onp->next = context->offset_hash[hash];
        context->offset_hash[hash] = onp;
        onp->offset = max_match_offset;
        /* Initialize to record that no matches have been found to this
         * location.  The current match will be recorded below. */
        onp->max_length = 0;
        onp->max_max_length = EW_COMPRESS_MAX_MATCH;
        onp->length_count = 0;
        onp->weight_bytes = 0;
        onp->weight_refs = 0;
        for (i = 0; i < (EW_COMPRESS_MAX_MATCH >> 3)+1; i++)
          {
            onp->length_table[i] = 0;
          }
      }

    /*
     * Enforce per-offset clamp on maximum string length to avoid repeating
     * strings in the dictionary (see below).
     */
    if (max_match <= onp->max_max_length)
      {
        /*
         * Get index and mask into length_table for match length.  If length
         * not already seen, then add new length.
         */
        i = (max_match - 1) >> 3;
        mask = 1 << ((max_match - 1) & 7);
        if ((onp->length_table[i] & mask) == 0)
          {
            onp->length_table[i] |= mask;
            /* Increment the total number of offset/length pairs that are
             * recorded in the dictionary. */
            onp->length_count++;
            context->string_count++;
            if (onp->max_length < max_match)
              {
                onp->max_length = max_match;
              }
           }
       }
     else
       {
         max_match = onp->max_max_length;
       }

    /*
     * If this matching string repeats (i.e. the matching string ends
     * where the current comparing string begins), then we want to limit
     * the offset node from expanding the dictionary (to avoid redundancy
     * in the dictionary).  The max_max_length is a clamp value.
     */
    if (max_match + max_match_offset == context->buffer_data_out)
      {
        onp->max_max_length = max_match;
      }

    /*
     * Update weight parameters.  These are used later to see if the strings
     * in this node are used frequently enough to justify their inclusion in
     * the dictionary
     */
    onp->weight_bytes += max_match;
    onp->weight_refs++;

    /*
     * Return size of match.
     */
    return max_match;
}

/*
 * ewCompress (internal)
 * This function compresses a buffer full of input data
 *
 * context      - compression context
 *
 * No return value
 */
void
ewCompress ( EwCompressContext context )
{
  uintf hash;
  uintf match_offset;
  uintf match_length;
  EwCompressOffsetNodeP onp;
  uintf max_match_code = 0;
  uintf max_match;
  uint8 *cp;
  uint8 *matchp;
  uintf i;

    /*
     * While input data to compress
     */
    while (context->buffer_data_out != context->buffer_data_in)
      {

        /*
         * Lookup current string in hash table.  If not found, emit literal and
         * continue.
         */
        hash =
          EW_COMPRESS_BUFFER_HASH(&context->buffer[context->buffer_data_out]);
        match_offset = context->buffer_hash[hash];
        if (match_offset == EW_COMPRESS_BUFFER_HASH_NULL)
          {
            ewCompressGenerateCode
              (context, context->buffer[context->buffer_data_out++]);
            continue;
          }

        /*
         * Scan hash chain looking for best match (in length of matching
         * characters).
         */
        max_match = 0;
        while (match_offset != EW_COMPRESS_BUFFER_HASH_NULL)
          {
            /*
             * Get offset node.  If no node, then continue search
             */
            hash = EW_COMPRESS_OFFSET_HASH(match_offset);
            for (onp = context->offset_hash[hash];
                 onp != NULL && onp->offset != match_offset;
                 onp = onp->next)
               ;
            if (onp == NULL)
              {
                match_offset = context->buffer_hash_next[match_offset];
                continue;
              }

            /*
             * See how good this match is
             */
            cp = &context->buffer[context->buffer_data_out];
            matchp = &context->string_data[match_offset];
            match_length = 0;
            while (*cp == *matchp &&
                   match_length < onp->max_length &&
                   match_length + context->buffer_data_out <
                     context->buffer_data_in)
              {
                cp++;
                matchp++;
                match_length++;
              }

            /*
             * If match isn't big enough, continue
             */
            if (match_length < EW_COMPRESS_MIN_MATCH)
              {
                match_offset = context->buffer_hash_next[match_offset];
                continue;
              }

            /*
             * Get the best codeword at this offset
             */
            for (i = onp->last_code - 1;
                 i != (uintf)(onp->first_code - 1);
                 i--)
              {
                if (match_length >= context->string_length[i])
                  {
                    break;
                  }
              }

            /*
             * If no codeword found that represents substring of match,
             * keep searching
             */
            if (i < onp->first_code || i >= onp->last_code)
              {
                match_offset = context->buffer_hash_next[match_offset];
                continue;
              }

            /*
             * If this is the best match so far, save it.
             */
            if (context->string_length[i] > max_match)
              {
                max_match = context->string_length[i];
                max_match_code = i;

                /*
                 * Stop if it doesn't get better than this!
                 */
                if (max_match == EW_COMPRESS_MAX_MATCH ||
                    max_match + context->buffer_data_out ==
                      context->buffer_data_in)
                  {
                    break;
                  }
              }

            /*
             * Get next match
             */
            match_offset = context->buffer_hash_next[match_offset];
          }

        /*
         * If no match found, generate literal
         */
        if (max_match == 0)
          {
            ewCompressGenerateCode
              (context, context->buffer[context->buffer_data_out++]);
            continue;
          }

        /*
         * Generate matching codeword and skip matching string in input data
         */
        ewCompressGenerateCode(context, 256 + max_match_code);
        context->buffer_data_out += max_match;
      }
}

/*
 * ewCompressGenerateCode (internal)
 * Generate output bytes representing codewords passed through this interface.
 *
 * context      - compression context
 * code         - codeword to emit
 *
 * No return value
 */
void
ewCompressGenerateCode ( EwCompressContext context, uintf code )
{
  uintf bits = context->dictionary->code_size;

    /*
     * Add lower bits to codeword to bit buffer and transmit full byte
     */
    context->byte |= code << context->bits;
    ewCompressOutput(context->handle, (uint8)context->byte);

    /*
     * Update bits remaining in codeword and codeword value
     */
    code >>= 8 - context->bits;
    bits -= 8 - context->bits;

    /*
     * If there are 8 or more bits remaining in the code word, send the second
     * byte and update bits and remaining value
     */
    if (bits >= 8)
      {
        ewCompressOutput(context->handle, (uint8) (code & 0xff));
        code >>= 8;
        bits -= 8;
      }

    /*
     * Save remaining bits for next time
     */
    context->bits = bits;
    context->byte = code;
}
