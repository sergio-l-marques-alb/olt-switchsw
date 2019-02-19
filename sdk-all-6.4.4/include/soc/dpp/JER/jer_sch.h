/*
 * $Id: $
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
 *
 * File: jer_sch.h
 */

#ifndef __JER_SCH_INCLUDED__

#define __JER_SCH_INCLUDED__

int
  soc_jer_sch_init(
    SOC_SAND_IN   int                    unit
    );
/*********************************************************************
* NAME:
*     jer_sch_device_rate_entry_core_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function gets an entry in the device rate table.
*     Each entry containss a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver reads from
*     the following tables:
*     Device Rate Memory (SCH_SHARED_DEVICE_RATE_SHARED_DRM)
* INPUT:
*  SOC_SAND_IN  uint32                 rci_level_ndx -
*     RCI bucket level. Range: 0 - 7
*  SOC_SAND_IN  uint32                 nof_active_links_ndx -
*     Number of current active links. range of input: 0 - 36
*     standing for 0-36 active links.
*  SOC_SAND_OUT uint32                 *rate -
*     The credit generation rate, in Mega-Bit-Sec. If 0 - no
*     credits are generated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  jer_sch_device_rate_entry_core_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_OUT uint32              *rate
   ) ;
/*********************************************************************
* NAME:
*     jer_sch_device_rate_entry_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function is for multi-core devices only.
*     This function sets an entry in the device rate table.
*     Each entry sets a credit generation rate, for a given
*     pair of fabric congestion (presented by rci_level) and
*     the number of active fabric links. The driver writes to
*     the following tables:
*     Device Rate Memory (SCH_SHARED_DEVICE_RATE_SHARED_DRM)
* INPUT:
*  SOC_SAND_IN  uint32                 rci_level_ndx -
*     RCI bucket level. Range: 0 - 7
*  SOC_SAND_IN  uint32                 nof_active_links_ndx -
*     Number of current active links. range of input: 0 - 36
*     standing for 0-36 active links.
*  SOC_SAND_IN  uint32                  rate -
*     The credit generation rate, in Mega-Bit-Sec. If 0 - no
*     credits are generated.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
* See also:
*   arad_sch_device_rate_entry_set_unsafe()
*********************************************************************/
uint32
  jer_sch_device_rate_entry_core_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 core,
    SOC_SAND_IN  uint32              rci_level_ndx,
    SOC_SAND_IN  uint32              nof_active_links_ndx,
    SOC_SAND_IN  uint32              rate
  ) ;

int 
soc_jer_sch_e2e_interface_allocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int 
soc_jer_sch_e2e_interface_deallocate(
    SOC_SAND_IN  int             unit,
    SOC_SAND_IN  soc_port_t      port
    );

int 
  soc_jer_sch_cal_max_size_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_OUT  uint32*                max_cal_size
   );

int
  soc_jer_sch_cal_tbl_set(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set /*A (0) or B (1)*/,
    SOC_SAND_IN   uint32                 slots_count,
                  uint32*                slots
  );

int
  soc_jer_sch_cal_tbl_get(
    SOC_SAND_IN   int                    unit,
    SOC_SAND_IN   int                    core_id,
    SOC_SAND_IN   uint32                 sch_offset,
    SOC_SAND_IN   uint32                 sch_to_set /*A (0) or B (1)*/,
    SOC_SAND_IN   uint32                 slots_count,
    SOC_SAND_OUT  uint32*                slots
  );

int soc_jer_sch_prio_propagation_enable_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  int enable
    );

int soc_jer_sch_prio_propagation_enable_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_OUT int *enable
    );

int soc_jer_sch_prio_propagation_port_set(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_IN  int is_high_prio
   );

int soc_jer_sch_prio_propagation_port_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  soc_port_t port,
    SOC_SAND_IN  int cosq,
    SOC_SAND_OUT int *is_high_prio
   );

uint32
  jer_sch_slow_max_rates_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_IN  int                 slow_rate_val
  );

uint32
  jer_sch_slow_max_rates_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  int                 slow_rate_type,
    SOC_SAND_OUT int                 *slow_rate_val
  );

uint32
jer_sch_slow_max_rates_per_level_set(
   SOC_SAND_IN int unit, 
   SOC_SAND_IN int core, 
   SOC_SAND_IN int level , 
   SOC_SAND_IN int slow_rate_type, 
   SOC_SAND_IN int slow_rate_val);

uint32
jer_sch_slow_max_rates_per_level_get(
   SOC_SAND_IN  int   unit,
   SOC_SAND_IN  int   core,
   SOC_SAND_IN  int   level,
   SOC_SAND_IN  int   slow_rate_type,
   SOC_SAND_OUT int*  slow_rate_val);

#endif /*__JER_SCH_INCLUDED__*/

