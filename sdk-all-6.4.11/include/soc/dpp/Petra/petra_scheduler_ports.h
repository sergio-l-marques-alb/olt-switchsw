/* $Id: petra_scheduler_ports.h,v 1.6 Broadcom SDK $
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
* FILENAME:       soc_petra_scheduler_ports.h
*
* MODULE PREFIX:  soc_petra_scheduler_ports
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/


#ifndef __SOC_PETRA_SCHEDULER_PORTS_H_INCLUDED__
/* { */
#define __SOC_PETRA_SCHEDULER_PORTS_H_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
/* } */

/*************
 * DEFINES   *
 *************/
/* { */

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

  /*********************************************************************
* NAME:
*     soc_petra_sch_port_sched_verify
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_port_sched_set
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx -
*     Port index (0-63).
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info -
*     Scheduler port configuration info.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_port_sched_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_port_sched_set_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     Sets the scheduler-port state (enable/disable), and its
*     HR mode of operation (single or dual). The driver writes
*     to the following tables: Scheduler Enable Memory (SEM),
*     HR-Scheduler-Configuration (SHC), Flow Group Memory
*     (FGM)
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx -
*     Port index (0-63).
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info -
*     Scheduler port configuration info.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_sch_port_sched_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_INFO      *port_info
  );

/*********************************************************************
* NAME:
*     soc_petra_sch_port_sched_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Oct 18 2007
* FUNCTION:
*     See soc_petra_sch_port_sched_set_unsafe
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of device to access.
*  SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx -
*     Port index (0-63).
*  SOC_SAND_OUT  SOC_PETRA_SCH_PORT_INFO      *port_info -
*     Scheduler port configuration info.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/


uint32
  soc_petra_sch_port_sched_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID        port_ndx,
    SOC_SAND_OUT SOC_PETRA_SCH_PORT_INFO      *port_info
  );

/*****************************************************
* NAME
*   soc_petra_sch_port_hp_class_conf_set_unsafe
* TYPE:
*   PROC
* DATE:
*   31/10/2007
* FUNCTION:
*     See p21v_sch_port_hp_class_conf_set_set
* INPUT:
*   SOC_SAND_IN  int                      unit -
*     Identifier of device to access.
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info -
*    A set of available configurations for port low flow control
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_verify(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );

/*****************************************************
* NAME
*   soc_petra_sch_port_hp_class_conf_set_unsafe
* TYPE:
*   PROC
* DATE:
*   31/10/2007
* FUNCTION:
*   Sets the group of available configurations for high priority
*   hr class settings.
*   Out of 5 possible configurations, 4 are available at any time.
* INPUT:
*   SOC_SAND_IN  int                      unit -
*     Identifier of device to access.
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info -
*    A set of available configurations for port low flow control
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_set_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );

/*****************************************************
* NAME
*   soc_petra_sch_port_hp_class_conf_set_unsafe
* TYPE:
*   PROC
* DATE:
*   31/10/2007
* FUNCTION:
*     See p21v_sch_port_hp_class_conf_set_set
* INPUT:
*   SOC_SAND_IN  int                      unit -
*     Identifier of device to access.
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info -
*    A set of available configurations for port low flow control
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_port_hp_class_conf_get_unsafe(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_OUT  SOC_PETRA_SCH_PORT_HP_CLASS_INFO  *hp_class_info
  );

/*****************************************************
* NAME
*   soc_petra_sch_hr_to_port_assign_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Assign HR scheduling element to port.
*   This will direct port credits to the HR.
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx -
*     The index of the port to set.
*     Range: 0 - 79
*   SOC_SAND_IN  uint8           is_port_hr -
*     If TRUE, the HR will be assigned to the port.
*     Otherwise - unasigned.
*     HR that is not assigned to port can be used as
*     HR scheduler.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_hr_to_port_assign_set(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx,
    SOC_SAND_IN  uint8           is_port_hr
  );

/*****************************************************
* NAME
*   soc_petra_sch_hr_to_port_assign_set
* TYPE:
*   PROC
* DATE:
*   26/12/2007
* FUNCTION:
*   Check if an HR scheduling element is assigned to port.
*   This will direct port credits to the HR.
* INPUT:
*   SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx -
*     The index of the port to set.
*     Range: 0 - 79
*   SOC_SAND_OUT  uint8           is_port_hr -
*     If TRUE, the HR is assigned to the port.
*     Otherwise - unasigned.
*     HR that is not assigned to port can be used as
*     HR scheduler.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*    None.
*****************************************************/
uint32
  soc_petra_sch_hr_to_port_assign_get(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  SOC_PETRA_SCH_PORT_ID  port_ndx,
    SOC_SAND_OUT uint8           *is_port_hr
  );

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PETRA_SCHEDULER_PORTS_H_INCLUDED__*/
#endif
