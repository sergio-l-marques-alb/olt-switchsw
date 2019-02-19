/* $Id: sand_trace.h,v 1.4 Broadcom SDK $
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
/******************************************************************
*
* FILENAME:       soc_sand_trace.h
*
* AUTHOR:         Dune (S.Z.)
*
* FILE DESCRIPTION:
*   Time trace utility.
*   soc_sand_trace_init() -- Initialize the tracing table.
*   soc_sand_trace_end()  -- Currently do nothing.
*   soc_sand_trace_clear() -- Clear the tracing table.
*   soc_sand_trace_print() -- Print trace table.
*                         Prints the time difference between 2 consecutive entries.
*   soc_sand_trace_add_entry() -- Add trace entry to the table.
*                             Add time stamp, except user given data.
* REMARKS:
*   The tracing table is filled continually -- Cyclic.
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
*******************************************************************/


#ifndef __SOC_SAND_TRACE_H_INCLUDED__
/* { */
#define __SOC_SAND_TRACE_H_INCLUDED__

#ifdef  __cplusplus
extern "C" {
#endif

#include <soc/dpp/SAND/Utils/sand_framework.h>

/*****************************************************
*NAME
*  soc_sand_trace_init
*TYPE:
*  PROC
*DATE:
*  15-Jan-03
*FUNCTION:
*  Initialize the trace mechanism.
*INPUT:
*  SOC_SAND_DIRECT:
*    void -
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_RET -
*      SOC_SAND_OK, SOC_SAND_ERR.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
SOC_SAND_RET
  soc_sand_trace_init(
    void
  );

/*****************************************************
*NAME
*  soc_sand_trace_end
*TYPE:
*  PROC
*DATE:
*  15-Jan-03
*FUNCTION:
*  End the trace mechanism.
*INPUT:
*  SOC_SAND_DIRECT:
*    void -
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_RET -
*      SOC_SAND_OK, SOC_SAND_ERR.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
SOC_SAND_RET
  soc_sand_trace_end(
    void
  );

/*****************************************************
*NAME
*  soc_sand_trace_clear
*TYPE:
*  PROC
*DATE:
*  15-Jan-03
*FUNCTION:
*  Clear the trace mechanism.
*INPUT:
*  SOC_SAND_DIRECT:
*    void -
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_RET -
*      SOC_SAND_OK, SOC_SAND_ERR.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
SOC_SAND_RET
  soc_sand_trace_clear(
    void
  );

/*****************************************************
*NAME
*  soc_sand_trace_add_entry
*TYPE:
*  PROC
*DATE:
*  15-Jan-03
*FUNCTION:
*  Add trace entry to the table.
*  Add time stamp, except user given data.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_IN uint32    identifier -
*         Identifier to be add to trace line.
*         Has no meaning to the tracing process
*    SOC_SAND_IN char*            str
*         String to be add to trace line.
*         Has no meaning to the tracing process
*         XXXXX pointer to const string (not on stack...)  XXXXX
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_RET -
*      SOC_SAND_OK, SOC_SAND_ERR.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  None.
*SEE ALSO:
*****************************************************/
SOC_SAND_RET
  soc_sand_trace_add_entry(
    SOC_SAND_IN uint32  identifier,
    SOC_SAND_IN char* const   str    /*pointer to const string*/
  );


/*****************************************************
*NAME
*  soc_sand_trace_print
*TYPE:
*  PROC
*DATE:
*  16-Jan-03
*FUNCTION:
*  Print trace table.
*  Prints the time difference between 2 consecutive entries.
*INPUT:
*  SOC_SAND_DIRECT:
*    void -
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_RET -
*      SOC_SAND_OK, SOC_SAND_ERR.
*  SOC_SAND_INDIRECT:
*    NON
*REMARKS:
*  Time difference is in micro-seconds 10^(-6).
*SEE ALSO:
*****************************************************/
SOC_SAND_RET
  soc_sand_trace_print(
    void
  );

#ifdef  __cplusplus
}
#endif

/* } __SOC_SAND_TRACE_H_INCLUDED__*/
#endif
