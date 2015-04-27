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
 */
#ifndef _EW_CMP_H
#define _EW_CMP_H

#include "ew_common.h"

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

typedef struct EwCompressContext_s *EwCompressContext;
typedef struct EwDecompressContext_s *EwDecompressContext;
typedef struct EwDecompressSaveState_s *EwDecompressSaveState;


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
 * ewCompressDictionaryOpen
 * Start a new compression dictionary
 *
 * endian       - endian of output archive (ewBigEndian, or
 *                ewLittleEndian)
 *
 * Returns a new compression context, or NULL if resources not available.
 */
extern EwCompressContext ewCompressDictionaryOpen ( Endian endian );

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
extern void ewCompressDictionaryWrite
  ( EwCompressContext context, const uint8 *datap, uint32 bytes );

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
extern void ewCompressDictionaryCreate
  ( EwCompressContext context, uint8 **datapp, uint32 *bytesp );

/*
 * ewCompressDictionaryClose
 * Release dictionary resources
 *
 * context      - compression context
 *
 * No return value
 */
extern void ewCompressDictionaryClose ( EwCompressContext context );

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
extern void ewCompressBegin ( EwCompressContext context, void * handle );

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
extern void ewCompressWrite
  ( EwCompressContext context, const uint8 *datap, uint32 bytes );

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
extern void ewCompressEnd ( EwCompressContext context );

/*
 * ewCompressOutput (application-provided)
 * Pass a byte of compressed data to the application
 *
 * handle       - application-specific handle from ewCompressOpen()
 * byte         - byte of compressed data
 *
 * No return value
 */
extern void ewCompressOutput ( void * handle, uint8 byte );

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
extern EwDecompressContext ewDecompressBegin
  ( const uint8 * dictionary, void * handle );

/*
 * ewDecompressRead
 * Read uncompressed data from decompression context into a memory buffer
 *
 * context      - decompression context
 * datap        - pointer to data buffer
 * bytes        - length of data buffer
 *
 * Returns number of bytes decompressed
 */
extern uintf ewDecompressRead
  ( EwDecompressContext context, uint8 *datap, uint32 length );

/*
 * ewDecompressInput
 * Return the next byte of compressed data
 *
 * handle       - application-specific handle
 *
 * Returns next byte of compressed data, or <0 if EOF reached
 */
extern sintf ewDecompressInput ( void * handle );


/*
 * This structure is used to "store" a point in the decompressed
 * data stream for looping
 */

typedef struct EwDecompressSaveState_s
{
  const uint8 *stringp;          /* saved pointer into string */
  uintf       remaining;         /* saved bytes remaining in string */
  uintf       index_size;        /* number strings */
  uintf       code_size;         /* bits per codeword */
  uintf       code;              /* current codeword */
  uintf       bits;              /* bits in codeword */
} EwDecompressSaveState_t;

/*
 * ewDecompressSaveContext
 * Save string pointer and remaining count
 *
 * context      - decompression context
 *
 * return - none
 */
extern void
ewDecompressSaveContext ( EwDecompressContext context
                         ,EwDecompressSaveState save );

/*
 * ewDecompressRestoreContext
 * Restore string pointer and remaining count
 *
 * context      - decompression context
 *
 * return - none
 */
void
ewDecompressRestoreContext ( EwDecompressContext context
                            ,EwDecompressSaveState restore );


#endif /* _EW_CMP_H */
