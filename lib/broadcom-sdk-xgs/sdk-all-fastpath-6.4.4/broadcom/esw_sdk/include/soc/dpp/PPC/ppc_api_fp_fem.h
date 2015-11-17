/* $Id: soc_ppc_api_fp_fem.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PPC_FP_FEM_INCLUDED__
/* { */
#define __SOC_PPC_FP_FEM_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>
#include <soc/dpp/PPC/ppc_api_fp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If True, this FEM is for a Direct Extraction entry.
   */
  uint8 is_for_entry;
  /*
   *  Database strength of this FEM
   */
  uint32 db_strength;
  /*
   *  Database-ID of this FEM.
   */
  uint32 db_id;
  /*
   *  Entry strength of this FEM. Relevant only if
   *  'is_for_entry' is True.
   */
  uint32 entry_strength;
  /*
   *  Entry-ID of this FEM (relevant only if 'is_for_entry' is
   *  True).
   */
  uint32 entry_id;
  /*
   *  Required action type. Needed to know the minimal FEM
   *  size.
   */
  SOC_PPC_FP_ACTION_TYPE action_type[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];
  /*
   * If TRUE, then Base-Offset is positive
   */
  uint8 is_base_positive[SOC_PPC_FP_NOF_ACTIONS_PER_DB_MAX];

} SOC_PPC_FP_FEM_ENTRY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If True, then the cycle-id is fixed (e.g., for TCAM
   *  Databases)
   */
  uint8 is_cycle_fixed;
  /*
   *  Cycle-ID required for this FEM
   */
  uint8 cycle_id;

} SOC_PPC_FP_FEM_CYCLE;


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
  SOC_PPC_FP_FEM_ENTRY_clear(
    SOC_SAND_OUT SOC_PPC_FP_FEM_ENTRY *info
  );

void
  SOC_PPC_FP_FEM_CYCLE_clear(
    SOC_SAND_OUT SOC_PPC_FP_FEM_CYCLE *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FP_FEM_ENTRY_print(
    SOC_SAND_IN  SOC_PPC_FP_FEM_ENTRY *info
  );

void
  SOC_PPC_FP_FEM_CYCLE_print(
    SOC_SAND_IN  SOC_PPC_FP_FEM_CYCLE *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_FP_FEM_INCLUDED__*/
#endif

