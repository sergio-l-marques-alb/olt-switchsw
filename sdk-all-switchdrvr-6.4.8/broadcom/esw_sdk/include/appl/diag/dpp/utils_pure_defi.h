/* $Id: utils_pure_defi.h,v 1.5 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
*/
#ifndef UTILS_PURE_DEFI_INCLUDED
/* { */
#define UTILS_PURE_DEFI_INCLUDED

#include <appl/diag/dpp/bits_bytes_macros.h>

/*
 * Definitions related to Thermometer chip (DS1721, I2C)
 * {
 */
#define THERMOMETER_MEZANINE_ARR_LOCATION     1
#define THERMOMETER_FABCRIC_ARR_LOCATION      2
#define THERMOMETER_LINECARD_1_ARR_LOCATION   THERMOMETER_FABCRIC_ARR_LOCATION
#define THERMOMETER_LINECARD_2_ARR_LOCATION   3
#define THERMOMETER_LINECARD_3_ARR_LOCATION   8
#define THERMOMETER_FRONTEND_3_ARR_LOCATION   4
#define THERMOMETER_DB_1_ARR_LOCATION         5
#define THERMOMETER_FRONTEND_4_ARR_LOCATION   6
#define THERMOMETER_FRONTEND_5_ARR_LOCATION   7

/*
 */
/*
 */
/*
 * GFA first thermometer.
 */
/*
 * GFA second thermometer.
 */
/*
 * GFA DB thermometer.
 */
/*
 * PTG/FTG first thermometer.
 */
/*
 * TEVB board thermometer.
 */
/*
 * }
 */

/*
 * Definitions related to Thermometer chip (DS1721, I2C)
 * {
 */

/*
 * TEVB FPGA thermometer.
 */

/*
* Soc_petra MAX1617A thermometer.
*/

/*
* SOC_SAND_FE600 thermometer.
*/

/*
 * }
 */

/*
 * Definitions related to OBJECT:
 *   Real Time Trace Utility
 * {
 */
/*
 * Maximal size of text descriptor of each event, including
 * terminating null.
 */
#define TRACE_EVENT_TEXT_SIZE        45
/*
 * Log-base-2 of maximal number of trace_event elements in the system. This
 * is the size of the trace fifo.
 * By default, set to 8
 */
#define LOG_NUM_TRACE_FIFO_ELEMENTS  8
/*
 * Maximal number of trace_event elements in the system. This
 * is the size of the trace fifo. Must be a power of 2.
 */
#define NUM_TRACE_FIFO_ELEMENTS      BIT(LOG_NUM_TRACE_FIFO_ELEMENTS)
/*
 * Mask of significant bits representing num of elements in fifo.
 */
/*
 * Number of unsigned long parameters that can be stored
 * for the formatting string in 'trace_event_text'
 */
# define NUM_TRACE_FIFO_PARAMETERS   4
typedef struct
{
    /*
     * Flag indicating whether this entry is valid.
     */
  unsigned int  valid ;
    /*
     * Time of this trace event. Time is from startup, in microseconds.
     */
  unsigned long time_microseconds ;
    /*
     * Internal identifier of event
     */
  int           trace_event_id ;
    /*
     * Null terminated text description of event.
     * This is a formatting string ('printf' style) into
     * which trace_params[i] are inserted.
     */
  char           trace_event_text[TRACE_EVENT_TEXT_SIZE] ;
  unsigned long  trace_params[NUM_TRACE_FIFO_PARAMETERS] ;
} TRACE_EVENT ;
typedef struct
{
    /*
     * Flag. Indicating fifo is locked and may not be loaded with
     * new data until unlocked.
     */
  unsigned int  fifo_locked ;
    /*
     * Flag. Indicating an attempt was made to load fifo while it
     * was locked. (Actually this is a counter of the number of
     * attempts. We assume 'unsigned int' will not overflow, itself)
     */
  unsigned int  load_while_locked ;
    /*
     * Flag. Indicating fifo is full and has overflown (some information
     * has been deleted).  (Actually this is a counter of the
     * number of overflow cases, We assume 'unsigned int' will not
     * overflow, itself)
     */
  unsigned int  overflow ;
    /*
     * Number of valid trace events in fifo.
     */
  unsigned int  num_elements ;
    /*
     * Index of NEXT element to 'put' into fifo.
     */
  unsigned int  put_index ;
    /*
     * Index of NEXT element to 'get' from fifo.
     */
  unsigned int  get_index ;
  TRACE_EVENT   trace_events[NUM_TRACE_FIFO_ELEMENTS] ;
} TRACE_FIFO ;
/*
 * }
 */
/* } */
#endif
