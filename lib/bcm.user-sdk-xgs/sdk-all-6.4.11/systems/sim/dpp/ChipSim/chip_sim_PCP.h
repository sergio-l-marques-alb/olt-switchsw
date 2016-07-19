/* $Id: chip_sim_PCP.h,v 1.3 Broadcom SDK $
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


#ifndef __CHIP_SIM_PCP_H_INCLUDED__
/* { */ 
#define __CHIP_SIM_PCP_H_INCLUDED__


/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h> 

#include "chip_sim.h"
#include "chip_sim_counter.h"
#include "chip_sim_interrupts.h"
#include "chip_sim_indirect.h"
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* table index definitions { */
#define PCP_ECI_QDR_TBL_ID                       0
#define PCP_ECI_RLD1_TBL_ID                      1
#define PCP_ECI_RLD2_TBL_ID                      2
#define PCP_ELK_FID_COUNTER_PROFILE_DB_TBL_ID    3
#define PCP_ELK_FID_COUNTER_DB_TBL_ID            4
#define PCP_ELK_SYS_PORT_IS_MINE_TBL_ID          5
#define PCP_ELK_AGING_CFG_TABLE_TBL_ID           6
#define PCP_ELK_FLUSH_DB_TBL_ID                  7
#define PCP_OAM_PR2_TCDP_TBL_ID                  8
#define PCP_OAM_MEP_DB_TBL_ID                    9
#define PCP_OAM_RMEP_DB_TBL_ID                   10
#define PCP_OAM_RMEP_HASH_0_DB_TBL_ID            11
#define PCP_OAM_RMEP_HASH_1_DB_TBL_ID            12
#define PCP_OAM_LMDB_CMN_TBL_ID                  13
#define PCP_OAM_LMDB_TX_TBL_ID                   14
#define PCP_OAM_DMDB_TX_TBL_ID                    15
#define PCP_OAM_DMDB_RX_TBL_ID                    16
#define PCP_STS_ING_COUNTERS_TBL_ID              17
#define PCP_STS_EGR_COUNTERS_TBL_ID              18
#define PCP_TBL_ID_LAST                                    19
/* table index definitions } */
/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
extern
  CHIP_SIM_INDIRECT_BLOCK
    Pcp_indirect_blocks[];
extern
  CHIP_SIM_COUNTER
    Pcp_counters[];
extern
  CHIP_SIM_INTERRUPT
    Pcp_interrupts[];
/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */
/*****************************************************
*NAME
* pcp_indirect_init
*TYPE:
*  PROC
*FUNCTION:
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *pcp_tbls.
*REMARKS:
*    None.
*SEE ALSO:
*****************************************************/
void
  pcp_indirect_init(
  );
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __CHIP_SIM_PCP_INCLUDED__*/
#endif
