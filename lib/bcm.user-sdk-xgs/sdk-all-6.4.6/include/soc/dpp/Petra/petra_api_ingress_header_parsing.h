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


#ifndef __SOC_PETRA_API_INGRESS_HEADER_PARSING_INCLUDED__
/* { */
#define __SOC_PETRA_API_INGRESS_HEADER_PARSING_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id$
 *  Statistics Tag field Maximal offset, in 4-bits units
 */
#define SOC_PETRA_IHP_STAG_OFFSET_4BITS_MAX 63

/*
 *  TM LAG hushing field Maximal offset, in 4-bits units
 */
#define SOC_PETRA_IHP_TMLAG_OFFSET_4BITS_MAX 63

/*
 *  The maximal offset, from Start Of Packet, of the out-most header
 */
#define SOC_PETRA_IHP_SOP2HEADER_OFFSET_BYTES_MAX 31

  /*
   *  Maximal number of bytes to remove from Start Of Packet after
   *  ingress parsing.
   */
#define SOC_PETRA_IHP_BYTES_FROM_SOP_TO_STRIP_MAX 31
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
   *  Start Of Packet
   */
  SOC_PETRA_IHP_PCKT_HEADER_ID_SOP=0,
  /*
   *  Header 0. For Ethernet Port: L2. For TM Port: ITMH.
   */
  SOC_PETRA_IHP_PCKT_HEADER_ID_0=1,
  /*
   *  Header 1. For Ethernet Port: L3. For TM Port: IS-ITMH
   *  (Ingress Shaping), if present
   */
  SOC_PETRA_IHP_PCKT_HEADER_ID_1=2,
  /*
   *  Header 2. For Ethernet Port: L4. For TM Port: Header
   *  following ITMH/IS-ITMH
   */
  SOC_PETRA_IHP_PCKT_HEADER_ID_2=3,
  /*
   *  Total number of Ingress Header Parsing headers.
   */
  SOC_PETRA_IHP_NOF_PCKT_HEADER_IDS=4
}SOC_PETRA_IHP_PCKT_HEADER_ID;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The packet header relative to which the offset is given.
   */
  SOC_PETRA_IHP_PCKT_HEADER_ID base_header;
  /*
   *  Offset of the statistics tag header data from the base
   *  header, in 4 bit (nobble) units. Range: 0 - 63.
   */
  uint32 offset_4bits;
  /*
   *  Size of statistics tag header data. Range: 0 - 7. Note.
   *  the actual size is limitied by 30-bit dedicated to the
   *  statistics tag fields. Other fields (Ingress FAP Port,
   *  TC, DP etc., refer to PORTS_STAT_TAG_FIELDS structure)
   *  can be enabled/disabled per-port.
   */
  uint32 length_4bits;
}SOC_PETRA_IHP_PCKT_STAG_HDR_DATA;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The packet header relative to which the offset is given.
   */
  SOC_PETRA_IHP_PCKT_HEADER_ID base_header;
  /*
   *  Offset of the statistics tag from the base header, in 4
   *  bit units. Range: 0 - 63.
   */
  uint32 offset_4bits;
  /*
   *  Range: 0 - 7.
   */
  uint32 length_4bits;
}SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The offset, from Start Of Packet, of the outmost header
   *  (header 0 - L2 for Ethernet Port, IS-ITMH/ITMH for
   *  TM). Units: Bytes. Range: 0 - 31.
   */
  uint32 sop2header_offset_bytes;
  /*
   *  Number of bytes to remove from Start Of Packet after
   *  ingress parsing. Units: Bytes. Range: 0 - 31.
   */
  uint32 strip_from_sop_bytes;
}SOC_PETRA_IHP_PORT_INFO;

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
*     soc_petra_ihp_port_set
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
  soc_petra_ihp_port_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_IN  SOC_PETRA_IHP_PORT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_port_get
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
  soc_petra_ihp_port_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_FAP_PORT_ID         port_ndx,
    SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO       *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_stag_hdr_data_set
* TYPE:
*   PROC
* FUNCTION:
*     Define the position and length of the Header Data field
*     in the statistics tag in the incoming packet header.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info -
*     Statistics Tag Header Data field configuration
* REMARKS:
*     The total size of the statistics tag is up to 30 bit.
*     Other fields of the statistics tag can be enabled
*     per/port - refer to ports_stat_tag_field_set API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_ihp_stag_hdr_data_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_stag_hdr_data_get
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
  soc_petra_ihp_stag_hdr_data_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_tmlag_hush_field_set
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
  soc_petra_ihp_tmlag_hush_field_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  );

/*********************************************************************
* NAME:
*     soc_petra_ihp_tmlag_hush_field_get
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
  soc_petra_ihp_tmlag_hush_field_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO  *info
  );

void
  soc_petra_PETRA_IHP_PCKT_STAG_HDR_DATA_clear(
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info
  );

void
  soc_petra_PETRA_IHP_PCKT_TMLAG_HUSH_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO *info
  );

void
  soc_petra_PETRA_IHP_PORT_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_IHP_PORT_INFO *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1


const char*
  soc_petra_PETRA_IHP_PCKT_HEADER_ID_to_string(
    SOC_SAND_IN SOC_PETRA_IHP_PCKT_HEADER_ID enum_val
  );



void
  soc_petra_PETRA_IHP_PCKT_STAG_HDR_DATA_print(
    SOC_SAND_IN SOC_PETRA_IHP_PCKT_STAG_HDR_DATA *info
  );



void
  soc_petra_PETRA_IHP_PCKT_TMLAG_HUSH_INFO_print(
    SOC_SAND_IN SOC_PETRA_IHP_PCKT_TMLAG_HUSH_INFO *info
  );



void
  soc_petra_PETRA_IHP_PORT_INFO_print(
    SOC_SAND_IN SOC_PETRA_IHP_PORT_INFO *info
  );


#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_INGRESS_HEADER_PARSING_INCLUDED__*/
#endif
