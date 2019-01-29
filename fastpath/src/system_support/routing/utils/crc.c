/********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2003-2007
 *
 * *********************************************************************
 *
 * @filename      crc.c
 *
 * @purpose
 *            Description:
 *   CRC generation routines.
 *   Routines in this file calculate CRC's as follows:
 *   CRC-32 for AAL-5
 *   CRC-10 for AAL-3/4 and OAM cells
 *   CRC-8 for ATM cell headers********
 *
 *   CRC-32 generation is by one of three methods:
 *      1. fast table look-up
 *         data is processed 16-bits at a time
 *         requires a 256K byte lookup table
 *      2. slow table look-up
 *         data is processed 8-bits at a time
 *         requires a 1K byte lookup table
 *      3. hardware generation
 *         currently, the only supported option is to
 *         use a free SCC channel in the MC68360
 *
 *   CRC-10 generation is by one of two methods:
 *      1. fast table look-up
 *         data is processed 16-bits at a time
 *         requires a 128K byte lookup table
 *      2. slow table look-up
 *         data is processed 8-bits at a time
 *         requires a 0.5K byte lookup table
 *
 *   CRC-8 generation is by one of two methods:
 *      1. fast table look-up
 *         data is processed 16-bits at a time
 *         requires a 64K byte lookup table
 *      2. slow table look-up
 *         data is processed 8-bits at a time
 *         requires a 256 byte lookup table
 *
 * @component       Routing Utils Component
 *
 * @comments
 *        External Routine:
 *   CRC32_Init         initialize CRC-32 lookup table
 *   CRC10_Init         initialize CRC-10 lookup table
 *   CRC8_Init          initialize CRC-8 lookup table
 *   CRC32_Calculate    calculate CRC-32 for a frame
 *   CRC10_Calculate    calculate CRC-10 for a frame
 *   CRC8_Calculate     calculate CRC-8 for a frame
 *
 * @create    01/01/1995
 *
 * @author    Jonathan Masel
 *            Igor Ternovsky
 *
 * @end
 *
 * ********************************************************************/


#ifdef FILEID_IN_MEMORY
static char __fileid[] = "$Header: lvl7dev\src\system_support\routing\utils\crc.c 1.1.3.1 2002/02/13 23:05:50EST ssaraswatula Development  $";
#endif



/* --- standard include files --- */

#include "std.h"


/* --- end of file crc.c --- */

