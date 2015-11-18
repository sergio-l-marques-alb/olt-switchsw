/* $Id: petra_ingress_header_parsing.h,v 1.6 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_ingress_header_parsing.h
*
* MODULE PREFIX:  soc_petra_ihp
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


#ifndef __SOC_PETRA_INGRESS_HEADER_PARSING_INCLUDED__
/* { */
#define __SOC_PETRA_INGRESS_HEADER_PARSING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_ingress_header_parsing.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_api_ingress_packet_queuing.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define SOC_PETRA_IGNR_KEY_PROG_SEL_STAG    0
#define SOC_PETRA_IGNR_KEY_PROG_SEL_TM_LAG  3
#define SOC_PETRA_IHP_PTC_ENTRY_NOF_CMDS  4
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
   *  Ingress shaping range
   */
  SOC_PETRA_ITM_INGRESS_SHAPE_Q_RANGE ingr_shape_range;

  /*
   *  Explicit queue ID packet Base queue number.
   */
  SOC_PETRA_IPQ_EXPLICIT_MAPPING_MODE_INFO base_q_num;
}SOC_PETRA_IHP_INGR_SHAPE_INFO;

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
*     soc_petra_ingress_header_parsing_init
* FUNCTION:
*     Initialization of the Soc_petra blocks configured in this module.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Called as part of the initialization sequence.
*********************************************************************/
uint32
  soc_petra_ingress_header_parsing_init(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_prog_n00_load_unsafe
* TYPE:
*   PROC
* DATE:
*   Apr  7 2008
* FUNCTION:
*     Loads the default program (program number 00) for
*     ingress Packet Header processing. This program supports
*     the following functionalities:1. Ethernet and TM header
*     parsing (including ingress shaping header if exist).2.
*     Statistics tag parsing3. LAG resolution based on TM
*     header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     This API may only be called before init_phase2.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_prog_n00_load_unsafe(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_port_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set ingress port configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info -
*     Ingress Port type and configuration.
* REMARKS:
*     When configuring fat pipe port,
*     SOC_PETRA_NIF_FAT_PIPE_RESEQ_HDR_SIZE_BYTES must be added
*     for the fat pipe resequencing header. For normal
*     operation, it must be added both as bytes-to-skip and as
*     bytes-to-remove. For Debug configuration, it may be
*     added as bytes-to-skip - this enables inspecting the
*     header at the egress.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_port_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_port_verify
* TYPE:
*   PROC
* FUNCTION:
*     Set ingress port configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info -
*     Ingress Port type and configuration.
* REMARKS:
*     When configuring fat pipe port,
*     SOC_PETRA_NIF_FAT_PIPE_RESEQ_HDR_SIZE_BYTES must be added
*     for the fat pipe resequencing header. For normal
*     operation, it must be added both as bytes-to-skip and as
*     bytes-to-remove. For Debug configuration, it may be
*     added as bytes-to-skip - this enables inspecting the
*     header at the egress.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_ihp_port_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_port_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set ingress port configuration
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx -
*     Local port index. Port Index. Range: 0 - 79.
*  SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO       *info -
*     Ingress Port type and configuration.
* REMARKS:
*     When configuring fat pipe port,
*     SOC_PETRA_NIF_FAT_PIPE_RESEQ_HDR_SIZE_BYTES must be added
*     for the fat pipe resequencing header. For normal
*     operation, it must be added both as bytes-to-skip and as
*     bytes-to-remove. For Debug configuration, it may be
*     added as bytes-to-skip - this enables inspecting the
*     header at the egress.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_port_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_prog_ptc_cmd_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Set a single PTC command for Programmable Port
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN uint32 cmd_offset - command offset
    SOC_SAND_IN uint32 cmd_val - command value
    SOC_SAND_IN uint32 cmd_len - command length
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_prog_ptc_cmd_set_unsafe(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 cmd_offset,
    SOC_SAND_IN uint32 cmd_val,
    SOC_SAND_IN uint32 cmd_len
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_stag_hdr_data_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the Header Data field
*     in the statistics tag in the incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info -
*     Statistics Tag Header Data field configuration
* REMARKS:
*     The total size of the statistics tag is up to 30 bit.
*     Other fields of the statistics tag can be enabled
*     per/port - refer to ports_stat_tag_field_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_stag_hdr_data_verify
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the Header Data field
*     in the statistics tag in the incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info -
*     Statistics Tag Header Data field configuration
* REMARKS:
*     The total size of the statistics tag is up to 30 bit.
*     Other fields of the statistics tag can be enabled
*     per/port - refer to ports_stat_tag_field_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_ihp_stag_hdr_data_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_stag_hdr_data_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the Header Data field
*     in the statistics tag in the incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info -
*     Statistics Tag Header Data field configuration
* REMARKS:
*     The total size of the statistics tag is up to 30 bit.
*     Other fields of the statistics tag can be enabled
*     per/port - refer to ports_stat_tag_field_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_tmlag_hush_field_set_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info -
*     TM LAG Hushing field configuration
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_set_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_tmlag_hush_field_verify
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info -
*     TM LAG Hushing field configuration
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_verify(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_tmlag_hush_field_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the TM-LAG Hushing
*     field in incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info -
*     TM LAG Hushing field configuration
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_tmlag_hush_field_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_INGRESS_HEADER_PARSING_INCLUDED__*/
#endif
