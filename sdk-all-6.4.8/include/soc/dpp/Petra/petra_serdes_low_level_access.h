/* $Id: soc_petra_serdes_low_level_access.h,v 1.5 Broadcom SDK $
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


#ifndef __SOC_PETRA_SERDES_LOW_LEVEL_ACCESS_INCLUDED__
/* { */
#define __SOC_PETRA_SERDES_LOW_LEVEL_ACCESS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/Petra/petra_chip_regs.h>
#include <soc/dpp/Petra/petra_api_serdes_utils.h>
#include <soc/dpp/Petra/petra_api_reg_access.h>
/* } */

/*************
* DEFINES   *
*************/
/* { */
/* $Id: soc_petra_serdes_low_level_access.h,v 1.5 Broadcom SDK $
 *	If set, the EPB access is always validated,
 *  i.e. the EPB-OP register is read after being written.
 *  Failure of this test can be caused by the following:
 *   - SerDes access while IPU/CMU is still in-reset
 *   - Insufficient delay between two EPB-accessoperations
 *   - General problem with ECI access
 *  This test also insures that EPB accesses can not override
 *  each other in systems with fast ECI access,
 *  by adding a small delay of the read-operation
 */
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
void soc_petra_srd_brdcst_all_lanes_clear(void);

void soc_petra_srd_brdcst_all_lanes_set(
       SOC_SAND_IN  uint32 unit,
       SOC_SAND_IN  uint8 enable
     );

uint8 soc_petra_srd_brdcst_all_lanes_get(
    SOC_SAND_IN  uint32 unit
  );

uint32
  soc_petra_srd_lla_epb_write(
    SOC_SAND_IN  uint32          unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID star_id,
    SOC_SAND_IN  uint32          star_qrtt_id,
    SOC_SAND_IN  uint32          element,
    SOC_SAND_IN  uint32          channel,
    SOC_SAND_IN  uint32          reg_addr,
    SOC_SAND_IN  uint8            data
  );

uint32
  soc_petra_srd_lla_epb_read(
    SOC_SAND_IN  uint32          unit,
    SOC_SAND_IN  SOC_PETRA_SRD_STAR_ID star_id,
    SOC_SAND_IN  uint32          star_qrtt_id,
    SOC_SAND_IN  uint32          element,
    SOC_SAND_IN  uint32          channel,
    SOC_SAND_IN  uint32          reg_addr,
    SOC_SAND_OUT uint8            *data
  );

/*********************************************************************
* NAME:
*     soc_petra_srd_status_fld_poll_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Preform polling on the value of a given SerDes register.
*     According to the poll_info structure, the polling is
*     first performed as busy-wait, repeatedly reading the
*     register. Then, a polling is performed with the
*     requested timer delay between consecutive reads. The
*     'success' parameter returns TRUE if the expected value
*     is read, and FALSE if all the polling iterations do not
*     read the expected value (timeout).
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity -
*     the entity to access. Can be a SerDes lane, a CMU or an
*     IPU.
*  SOC_SAND_IN  uint32                 entity_ndx -
*     the entity index. If SerDes Lane - the SerDes lane
*     number. Range: 0-59. If SerDes CMU - the SerDes Quertet
*     index. Range: 0 - 14. If SerDes IPU - the SerDes Star
*     index (refer to SOC_PETRA_SRD_STAR_ID). Range: 0 - 3
*     (NIF-A/NIF-B/FABRIC-A/FABRIC-B).
*  SOC_SAND_IN  SOC_PETRA_REG_FIELD           *field -
*     The field to read. Contains access information:
*     element, offset, MSB, LSB.
*     The read value is compared to the expected value
*     on each polling iteration.
*  SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info -
*     The polling parameters (expected value, number of
*     iterations, delay).
*  SOC_SAND_OUT uint8                 *success -
*     TRUE if the expected value is read. FALSE if timeout
*     occurs, and the expected value is not read.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_srd_status_fld_poll_unsafe(
    SOC_SAND_IN  uint32                 unit,
    SOC_SAND_IN  SOC_PETRA_SRD_ENTITY_TYPE     entity,
    SOC_SAND_IN  uint32                 entity_ndx,
    SOC_SAND_IN  SOC_PETRA_SRD_REGS_FIELD      *field,
    SOC_SAND_IN  SOC_PETRA_POLL_INFO           *poll_info,
    SOC_SAND_OUT uint8                 *success
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SERDES_LOW_LEVEL_ACCESS_INCLUDED__*/
#endif
