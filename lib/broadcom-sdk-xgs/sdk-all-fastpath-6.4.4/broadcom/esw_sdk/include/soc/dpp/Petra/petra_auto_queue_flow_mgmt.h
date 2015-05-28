/* $Id: petra_auto_queue_flow_mgmt.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_auto_queue_flow_mgmt.h
*
* MODULE PREFIX:  soc_petra_aqfm
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

#ifndef __SOC_PETRA_AQFM_SCHEME_INCLUDED__
/* { */
#define __SOC_PETRA_AQFM_SCHEME_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_auto_queue_flow_mgmt.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_AQFM_FIRST_DESTINATION_ID            0
#define SOC_PETRA_AQFM_FIRST_FLOW_ID                   32
#define SOC_PETRA_AQFM_FIRST_QUEUE_ID                  32
#define SOC_PETRA_AQFM_FIRST_PHYSICAL_SYSTEM_PORT_ID   0


#define SOC_PETRA_AQFM_PORT_AUTO_RELATIVE_ID_BIT       30
#define SOC_PETRA_AQFM_AGG_AUTO_RELATIVE_ID_BIT        29
#define SOC_PETRA_AQFM_CREDIT_SOURCE_IS_LOCAL_PORT \
          (SOC_SAND_BIT(SOC_PETRA_AQFM_PORT_AUTO_RELATIVE_ID_BIT) | SOC_PETRA_NOF_FAP_PORTS)

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
  /*
   *
   */
  uint32  entity_id;
  /*
   *
   */
  uint32   credit_source_subflow_0;
  /*
   *
   */
  uint32   credit_source_subflow_1;
  /*
   *
   */
  uint8  valid;
}SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO;

typedef struct
{
  SOC_PETRA_AQFM_AGG_TABLE_ITEM_INFO item[SOC_PETRA_AQFM_NOF_HIERARCY][SOC_PETRA_AQFM_NOF_INDEX];
}SOC_PETRA_AQFM_AGG_TABLE_INFO;

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
*     soc_petra_aqfm_system_info_save_unsafe
* TYPE:
*   PROC
* DATE:
*   May 12 2008
* FUNCTION:
*     This function saves the provided data in the SW database
*     for future use.
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_AQFM_SYSTEM_INFO    *p_info -
*     pointer to the struct to fill data.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_system_info_save_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  );

/*********************************************************************
* NAME:
*     soc_petra_aqfm_system_info_defaults_get_unsafe
* TYPE:
*   PROC
* DATE:
*   Nov 27 2007
* FUNCTION:
*     Each structure in the auto management system has a
*     function soc_petra_aqfm_XXX_defaults(). This function will:
*     1. Clean the structure; 2. Set default values to
*     parameters that have logical common values. 3. Set
*     invalid values to parameters that have to be configured
*     by user. The function
*     get the structure SOC_PETRA_AQFM_SYSTEM_INFO as parameter,
*     and fill it with some default values.
* INPUT:
*  SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO   *p_info -
*     pointer to the struct to fill data.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_aqfm_system_info_defaults_get_unsafe(
    SOC_SAND_OUT SOC_PETRA_AQFM_SYSTEM_INFO   *p_info
  );

uint32
  soc_petra_aqfm_port_actual2relative_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          auto_actual_id,
    SOC_SAND_OUT uint32          *auto_relative_id
  );

uint32
  soc_petra_aqfm_port_first_relative_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_OUT uint32          *auto_relative_id
  );

uint32
  soc_petra_aqfm_port_relative2actual_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          auto_relative_id,
    SOC_SAND_OUT uint32          *auto_actual_id
  );

uint32
  soc_petra_aqfm_nof_relative_port_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_OUT uint32          *nof_ports
  );

uint32
  soc_petra_aqfm_agg_relative_id_get(
    SOC_SAND_IN  uint32 agg_level,
    SOC_SAND_IN  uint32 agg_index,
    SOC_SAND_OUT uint32  *agg_relative_id
  );

uint32
  soc_petra_aqfm_queue_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32           destination_id,
    SOC_SAND_IN  uint32          class_id,
    SOC_SAND_OUT uint32           *auto_queue_id
  );

uint32
  soc_petra_aqfm_destination_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          destination_fap_id,
    SOC_SAND_IN  uint32          destination_port_id,
    SOC_SAND_OUT uint32           *auto_destination_id
  );

uint32
  soc_petra_aqfm_flow_id_get(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint32          source_fap_id,
    SOC_SAND_IN  uint32          port_id,
    SOC_SAND_IN  uint32          class_id,
    SOC_SAND_OUT uint32           *auto_flow_id
  );

uint32
  soc_petra_aqfm_system_physical_port_id_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32  dest_id,
    SOC_SAND_OUT uint32  *system_physical_port_id
  );

uint32
  soc_petra_aqfm_agg_base_ids_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32                port_id,
    SOC_SAND_OUT SOC_PETRA_AQFM_AGG_BASE_IDS *agg_base_ids
  );

uint32
  soc_petra_aqfm_credit_sources_get(
    SOC_SAND_IN  int        unit,
    SOC_SAND_IN  uint32        port_id,
    SOC_SAND_IN  SOC_PETRA_SCH_FLOW  *logical_flow,
    SOC_SAND_OUT SOC_PETRA_SCH_FLOW  *physical_flow
  );


/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_AQFM_SCHEME_INCLUDED__*/
#endif
