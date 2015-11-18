/* $Id$
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

#ifndef __SOC_TMC_API_STACK_INCLUDED__
/* { */
#define __SOC_TMC_API_STACK_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/TMC/tmc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*     The number of words needed to store the pruning bitmap. */
#define  SOC_TMC_STACK_PRUN_BMP_LEN (1)

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

typedef enum
{
  /*
   *  1 TM domain in system, i.e. stacking is not supported.
   */
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_1 = 1,
  /*
   *  Up to 8 TM-domains in system is supported
   */
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_8 = 8,
  /*
   *  Up to 16 TM-domains in system is supported. Note in this
   *  case LAG resolution can not be performed at egress.
   */
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_16 = 16
}SOC_TMC_STACK_MAX_NOF_TM_DOMAINS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Maximum number of TM-domains in system. Note if 16
   *  TM-domains are supported then LAG resolution can not be
   *  performed at egress.
   */
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS max_nof_tm_domains;
  /*
   *  The TM-domain this device belongs to. Range: 0 - 15. The
   *  actual range as configured by max_nof_tm_domains
   */
  uint32 my_tm_domain;

} SOC_TMC_STACK_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Peer TM-Domain, i.e. The TM domain this stacking port is
   *  connected to in the other side. Range: 0 - 15. The
   *  actual range as configured in
   *  tmd_stack_global_info_set().
   */
  uint32 peer_tm_domain;
  /*
   *  Bitmap includes which TM domains it's allowed for
   *  traffic leaving the current TM domain -using this
   *  stacking port- to enter and which not. Set bit k to 1, if
   *  the packet is not allowed to enter to TM-domain k (to be
   *  filtered). and set to zero otherwise. Generally, it's
   *  expected that peer_tm_domain is unset in 'prun_bmp'
   *  bitmap. Packet will be filtered if one of the following
   *  has occurred:- The packet already visited the peer TM
   *  domain 'peer_tm_domain' - Prior stacking port set
   *  peer_tm_domain in its 'prun_bmp' bitmap. Declaring not
   *  to enter this TM-domain.
   */
  uint32 prun_bmp[SOC_TMC_STACK_PRUN_BMP_LEN];

} SOC_TMC_STACK_PORT_DISTR_INFO;


/* Stacking egress programs for TM port profile. uses for Left/Right stacking route history prunning */
typedef enum
{
  /*
   *  Configure stack1 port profile
   */
  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK1 = 1,
  /*
   *  Configure stack1 port profile
   */
  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK2 = 2,
  /*
   *  Configure both stack port profile
   */
  SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK_ALL = 3
}SOC_TMC_STACK_EGR_PROG_TM_PORT_PROFILE_STACK;


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

void
  SOC_TMC_STACK_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STACK_GLBL_INFO *info
  );

void
  SOC_TMC_STACK_PORT_DISTR_INFO_clear(
    SOC_SAND_OUT SOC_TMC_STACK_PORT_DISTR_INFO *info
  );

#if SOC_TMC_DEBUG_IS_LVL1

const char*
  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS_to_string(
    SOC_SAND_IN  SOC_TMC_STACK_MAX_NOF_TM_DOMAINS enum_val
  );

void
  SOC_TMC_STACK_GLBL_INFO_print(
    SOC_SAND_IN  SOC_TMC_STACK_GLBL_INFO *info
  );

void
  SOC_TMC_STACK_PORT_DISTR_INFO_print(
    SOC_SAND_IN  SOC_TMC_STACK_PORT_DISTR_INFO *info
  );

#endif /* SOC_TMC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_TMC_API_STACK_INCLUDED__*/
#endif

