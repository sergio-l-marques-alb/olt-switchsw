/* $Id: soc_petra_chip_tbls.h,v 1.7 Broadcom SDK $
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


#ifndef __SOC_PETRA_CHIP_TBLS_INCLUDED__
/* { */
#define __SOC_PETRA_CHIP_TBLS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_framework.h>
#ifdef LINK_PA_LIBRARIES
#include <soc/dpp/Petra/petra_a_chip_tbls.h>
#endif
#ifdef LINK_PB_LIBRARIES
#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>
#endif


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_chip_tbls.h,v 1.7 Broadcom SDK $
 * In the system red mechanism, we divide the queues to 16 ranges,
 * according to the thresholds. and each ranges receives a value of
 * 4 bit (0-15), this represents the queue in the system red test.
 */
#define SOC_PETRA_SYS_RED_NOF_Q_RNGS_THS      (SOC_PETRA_SYS_RED_NOF_Q_RNGS-1)

/* block index and mask definitions { */
#define SOC_PETRA_OLP_MASK           (SOC_PETRA_OLP_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IRE_MASK           (SOC_PETRA_IRE_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IDR_MASK           (SOC_PETRA_IDR_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IRR_MASK           (SOC_PETRA_IRR_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IHP_MASK           (SOC_PETRA_IHP_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_QDR_MASK           (SOC_PETRA_QDR_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IPS_MASK           (SOC_PETRA_IPS_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IPT_MASK           (SOC_PETRA_IPT_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_DPI_A_MASK         (SOC_PETRA_DPI_A_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_DPI_B_MASK         (SOC_PETRA_DPI_B_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_DPI_C_MASK         (SOC_PETRA_DPI_C_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_DPI_D_MASK         (SOC_PETRA_DPI_D_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_DPI_E_MASK         (SOC_PETRA_DPI_E_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_DPI_F_MASK         (SOC_PETRA_DPI_F_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_RTP_MASK           (SOC_PETRA_RTP_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_EGQ_MASK           (SOC_PETRA_EGQ_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_SCH_MASK           (SOC_PETRA_SCH_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_CFC_MASK           (SOC_PETRA_CFC_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_EPNI_MASK          (SOC_PETRA_EPNI_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PETRA_IQM_MASK           (SOC_PETRA_IQM_ID << SOC_SAND_MODULE_SHIFT)
/* block index and mask definitions } */

#define SOC_PETRA_NOF_DPI_IDS 6

/*
 *  The offset between two consecutive MAL-s (SCM table).
 */
#define SOC_PETRA_EGQ_NIF_SCM_TBL_OFFSET              0x00010000


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*
 *	Invalid table in case of access to a non allowed Soc_petra device
 */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0xFFFFFFFF */

  /* Invalid device called                                         */
  SOC_PETRA_TBL_FIELD invalid_device;

} __ATTRIBUTE_PACKED__ SOC_PETRA_INVALID_DEVICE_TBL;

typedef struct
{
#ifdef LINK_PA_LIBRARIES
  SOC_PA_TBLS *a_tables;
#endif
#ifdef LINK_PB_LIBRARIES
  SOC_PB_TBLS *b_tables;
#endif
#if !(defined(LINK_PA_LIBRARIES) && defined(LINK_PB_LIBRARIES))
  SOC_PETRA_INVALID_DEVICE_TBL *invalid_device_tbl;
#endif
} __ATTRIBUTE_PACKED__ SOC_PETRA_TBLS;


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

/*****************************************************
*NAME
* soc_petra_tbls_get
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT  SOC_PETRA_TBLS  **soc_petra_tbls - pointer to soc_petra
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pa_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_petra_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_tbls_get(
    SOC_SAND_OUT  SOC_PETRA_TBLS  **soc_petra_tbls
  );

/* Same functionality as soc_pa_tbls_get, but no error-checking */
SOC_PETRA_TBLS*
  soc_petra_tbls(void);

/*****************************************************
*NAME
*  soc_pa_tbls_init
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Dynamically allocates and initializes Soc_petra tables database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized tables will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_tbls_init(void);

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_CHIP_TBLS_INCLUDED__*/
#endif
