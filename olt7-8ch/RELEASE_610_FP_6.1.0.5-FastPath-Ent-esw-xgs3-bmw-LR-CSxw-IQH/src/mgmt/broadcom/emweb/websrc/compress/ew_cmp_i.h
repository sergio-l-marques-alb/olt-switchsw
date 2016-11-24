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
 * EmWeb Compression
 *
 */
#ifndef _EW_CMP_I_H
#define _EW_CMP_I_H

#include "ew_cmp.h"

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

/*
 * EwCompressDictionaryHeader
 *
 * Like the archive data headers, this header is endian-independent and packed
 * by defining 16-bit fields as arrays of two bytes.
 */
typedef struct EwCompressDictionaryHeader_s
  {
    uint8       version;                        /* compression version */
#   define      EW_COMPRESS_DICTIONARY_VERSION  1

    uint8       code_size;      /* bits/codeword, 9-16 */
    uint8       index_size[2];  /* size of dictionary index (# strings) */
    uint8       dict_size[2];   /* size of dictionary, bytes */
    uint8       reserved[2];    /* pad structure to eight bytes */
  } EwCompressDictionaryHeader, * EwCompressDictionaryHeaderP;

/*
 * Compression algorithm constants
 *
 * EW_COMPRESS_BUFFER_SIZE - We use a 64K buffer to read data into during
 * the dictionary construction process.  Once the full 64K buffer has been
 * filled (or end of data reached), the buffer is scanned for matching
 * strings.  We then pack the buffer down by moving all repeating strings
 * towards the beginning.  Thus, we end up with a partial dictionary in the
 * beginning of the buffer, and can then continue reading and processing
 * input data at the end of the buffer.  The maximum dictionary size is 64K,
 * and no more input data will be handled once the dictionary is full.
 *
 * Each string is hashed according to the first two characters (all dictionary
 * strings are at least two characters in length!) using a simple hash (two's
 * [... something is missing here ...]
 * that corresponds to the offset in the buffer.  The OffsetNode represents
 * the set of all matching strings (of different lengths) that begin at the
 * offset indicated.
 *
 * EW_COMPRESS_MAX_MATCH - the largest matching string is 256 bytes.
 *
 * EW_COMPRESS_MAX_SEARCH - when looking for matching strings, we limit the
 * search of the string buffer hashing function to this many tries.  This
 * speeds up compression of files that contain long strings of repeating
 * characters that could result in very long hashing chains.
 *
 * EW_COMPRESS_BUFFER_HASH_SIZE - Hash table of 2-character strings in buffer
 *
 * EW_COMPRESS_OFFSET_HASH_SIZE - OffsetNodes are hashed according to offset
 * in the buffer for fast lookup.
 */
#define EW_COMPRESS_BUFFER_SIZE         65536   /* string buffer size */
#define EW_COMPRESS_MIN_MATCH           2       /* min string match length */
#define EW_COMPRESS_MAX_MATCH           255     /* max string match length */
#define EW_COMPRESS_MAX_SEARCH          256     /* max strings to compare */
#define EW_COMPRESS_BUFFER_HASH_SIZE    4096    /* offset node hash size */
#define EW_COMPRESS_OFFSET_HASH_SIZE    4096    /* offset node hash size */

#define EW_COMPRESS_BUFFER_HASH(cp)     \
          (((((uint8*)cp)[0] << 4)^((uint8*)cp)[1]) % \
            EW_COMPRESS_BUFFER_HASH_SIZE)
#define EW_COMPRESS_BUFFER_HASH_NULL    (EW_COMPRESS_BUFFER_SIZE - 1)
#define EW_COMPRESS_OFFSET_HASH(x)      \
          ((x) % EW_COMPRESS_OFFSET_HASH_SIZE)

/*
 * EwCompressOffsetNode
 *
 * This structure is allocated each time a matching string is found, and
 * corresponds to the offset within the buffer that begins the string.  This
 * structure represents one or more matching strings of different lengths
 * beginning at the same offset.  A bit-table is used to mark the observed
 * lengths, and we keep track of the max_length and number of lengths
 * maintained.
 */
typedef struct EwCompressOffsetNode_s
  {
    struct EwCompressOffsetNode_s * next;       /* next in hash */
    uint16      offset;                         /* offset into buffer */
    uint8       max_max_length;                 /* upper bound on max length */
    uint8       max_length;                     /* max matching length */
    uint8       length_count;                   /* number of lengths */
    uint16      first_code;                     /* first code assigned */
    uint16      last_code;                      /* last code + 1 assigned */
    uint32      weight_bytes;                   /* bytes matched */
    uint32      weight_refs;                    /* references */
    uint8       length_table[(EW_COMPRESS_MAX_MATCH >> 3) + 1];
  } EwCompressOffsetNode, * EwCompressOffsetNodeP;

/*
 * EwCompressContext
 *
 * Context for compression state
 */
typedef struct EwCompressContext_s
  {
    /*
     * Buffer and hash tables
     */
    uint8                       buffer [ EW_COMPRESS_BUFFER_SIZE ];

#ifndef EW_NO_COMPRESS /* if not building the compiler */
    /* Pointer to the next location in the buffer where starts a string with
     * the same hash as this location. */
    uint16                      buffer_hash_next [ EW_COMPRESS_BUFFER_SIZE ];
    /* For each hash value, pointer to the first location in the buffer where
     * starts a string with the hash value.  I.e., the head of the chain
     * of locations with this hash value.  The chain always has the largest
     * location numbers at the head and the smallest at the tail. */
    uint16                      buffer_hash [ EW_COMPRESS_BUFFER_HASH_SIZE ];
#endif /*  EW_NO_COMPRESS  */

    /* The heads of the chains of offset nodes whose offsets hash to the
     * same value. */
    EwCompressOffsetNodeP       offset_hash [ EW_COMPRESS_OFFSET_HASH_SIZE ];

    Endian      endian;                 /* endian of archive */
    uintf       buffer_data_in;         /* new data to scan goes here */
    uintf       buffer_data_out;        /* scanning begins here */
    uintf       string_count;           /* number of codewords generated */

    EwCompressDictionaryHeaderP dictionary;     /* dictionary (allocated) */
    uint8       *string_offset_lo;              /* LSB dictionary offset */
    uint8       *string_offset_hi;              /* MSB dictionary offset */
    uint8       *string_length;                 /* dictionary string len */
    uint8       *string_data;                   /* dictionary data */
    void        *handle;                        /* current app. handle */

    uintf       byte;                   /* outgoing byte buffer */
    uintf       bits;                   /* bits in outgoing byte */

  } EwCompressContext_t;


typedef struct EwDecompressContext_s
  {
    const EwCompressDictionaryHeader *dictionary;     /* dictionary pointer */
    const uint8 *string_offset_lo;      /* LSB dictionary offset */
    const uint8 *string_offset_hi;      /* MSB dictionary offset */
    const uint8 *string_length;         /* dictionary string len */
    const uint8 *string_data;           /* dictionary data */
    void        *handle;                /* application handle */
    uintf       index_size;             /* number strings */
    uintf       code_size;              /* bits per codeword */
    uintf       code;                   /* current codeword */
    uintf       bits;                   /* bits in codeword */
    const uint8 *stringp;               /* pointer into string */
    uintf       remaining;              /* bytes remaining in string */
  } EwDecompressContext_t;

#endif /* _EW_CMP_I_H */
