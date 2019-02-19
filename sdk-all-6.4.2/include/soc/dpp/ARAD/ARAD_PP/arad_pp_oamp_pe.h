/* $Id: arad_pp_oamp_pe.h,v 1.2 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifndef __ARAD_PP_OAMP_PE_INCLUDED__
/* { */
#define __ARAD_PP_OAMP_PE_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

typedef enum
{
    ARAD_PP_OAMP_PE_PROGS_DEFAULT, /*Default null program. Always loaded, must be first.*/
    ARAD_PP_OAMP_PE_PROGS_OAMP_SERVER , 
    ARAD_PP_OAMP_PE_PROGS_1DM , /*gerenerating 1DM from DMM*/
    ARAD_PP_OAMP_PE_PROGS_ETH_TLV , /*WA for Eth-TLV bug. This one is AKA  Port/interface status TLV bug fix*/
    ARAD_PP_OAMP_PE_PROGS_ETH_TLV_ON_SERVER,  /*WA for Eth-TLV bug on server MEP.  AKA Port/interface status TLV bug fix for OAMP server  */
    ARAD_PP_OAMP_PE_PROGS_6374_LM , /*WA for 6374 LM generation bug*/
    ARAD_PP_OAMP_PE_PROGS_6374_DM ,/*WA for 6374 DM generation bug. */
    ARAD_PP_OAMP_PE_PROGS_PUSH_RA , /*WA for RA bug. Not implemented at this stage.*/
    ARAD_PP_OAMP_PE_PROGS_POP_CW , /*WA for CW bug. Not implemented at this stage.*/
    ARAD_PP_OAMP_PE_PROGS_PUSH_RA_AND_POP_CW , /*WA for both bugs, RA and CW. Not implemented at this stage.*/
    ARAD_PP_OAMP_PE_PROGS_BFD_ECHO, /* Arad+ only*/
    ARAD_PP_OAMP_PE_PROGS_BFD_UDP_CHECKSUM, /* Arad+ only */
    /* nof progs - always last */
    ARAD_PP_OAMP_PE_PROGS_NOF_PROGS
} ARAD_PP_OAMP_PE_PROGRAMS;


/* } */
/*************
 * MACROS    *
 *************/
/* { */


/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */


/* } */
/*************
 * GLOBALS   *
 *************/
/* { */



/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/**
* NAME:
 *   arad_pp_oamp_pe_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Initialize the OAMP Programable Editor.
 * INPUT:
 *   SOC_SAND_IN  int                   unit -
 * REMARKS:
 * None.
 * RETURNS:
 *   OK or ERROR indication.
**/
uint32
  arad_pp_oamp_pe_unsafe(
    SOC_SAND_IN  int                                 unit
  );



/**
* NAME:
 *   arad_pp_oamp_pe_profile_get
 * TYPE:
 *   PROC
 * FUNCTION:
*    Returns a program pointer (first instruction of program in
*  OAMP PE Program table)
*  INPUT:
*    SOC_SAND_IN  int                   unit -
*   SOC_SAND_IN   ARAD_PP_OAMP_PE_PROGRAMS     program_id -
*  program to set
*  SOC_SAND_OUT  int *program_profile
*     Result goes there. Program profile (0-7) if found,
*  -1 otherwise
*  REMARKS:
 * None for now.
 * RETURNS:
 *   Nothing.
**/
void
  arad_pp_oamp_pe_program_profile_get(
    SOC_SAND_IN   int                                 unit,
    SOC_SAND_IN   ARAD_PP_OAMP_PE_PROGRAMS     program_id,
    SOC_SAND_OUT  uint32                                 *program_profile
  );


#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_OAMP_PE_INCLUDED__*/
#endif




