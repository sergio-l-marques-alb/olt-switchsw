/* $Id: pb_diagnostics.h,v 1.7 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_TM/include/soc_pb_diagnostics.h
*
* MODULE PREFIX:  soc_pb_diagnostics
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

#ifndef __SOC_PB_DIAGNOSTICS_INCLUDED__
/* { */
#define __SOC_PB_DIAGNOSTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_diagnostics.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */
/* length of debug value */
#define SOC_PB_DIAG_DBG_VAL_LEN        (24)


#define SOC_PB_ERR_DESC_BASE_DIAGNOSTICS_FIRST   (0)

#define SOC_PB_PROC_DESC_BASE_DIAGNOSTICS_FIRST  (0)

#define SOC_PB_DIAG_LAST_PACKET_INFO             SOC_TMC_DIAG_LAST_PACKET_INFO

#define SOC_PB_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES SOC_TMC_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES_PB
 
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
   *  Register address of the register this field belongs to.
   */
  SOC_PETRA_REG_ADDR addr;
  /*
   *  Field Most Significant Bit in the register.
   */
  uint32 msb;
  /*
   *  Field Least Significant Bit in the register.
   */
  uint32 lsb;

} SOC_PB_DIAG_REG_FIELD;
typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_DIAG_LAST_PACKET_INFO_GET = SOC_PB_PROC_DESC_BASE_DIAGNOSTICS_FIRST,
  SOC_PB_DIAG_LAST_PACKET_INFO_GET_PRINT,
  SOC_PB_DIAG_LAST_PACKET_INFO_GET_UNSAFE,
  SOC_PB_DIAG_LAST_PACKET_INFO_GET_VERIFY,
  SOC_PB_DIAGNOSTICS_GET_PROCS_PTR,
  SOC_PB_DIAGNOSTICS_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PB_DIAGNOSTICS_PROCEDURE_DESC_LAST
} SOC_PB_DIAGNOSTICS_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PB_DIAGNOSTICS_TM_PORT_OUT_OF_RANGE_ERR = SOC_PB_ERR_DESC_BASE_DIAGNOSTICS_FIRST,
  SOC_PB_DIAGNOSTICS_PP_PORT_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PB_DIAGNOSTICS_ERR_LAST
} SOC_PB_DIAGNOSTICS_ERR;


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
 *   soc_pb_diag_last_packet_info_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Returns diagnostics information regarding the last
 *   packet: the incoming TM port and the corresponding PP
 *   port, port header processing type, packet headers and
 *   payload (first 128 Bytes). In case of TM port, the ITMH,
 *   which is part of that buffer, is parsed.
 * INPUT:
 *   SOC_SAND_IN  int                unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT SOC_PB_DIAG_LAST_PACKET_INFO *last_packet -
 *     Fields of the last packet.
 * REMARKS:
 *   1. If the packet is processed with ingress shaping, then
 *   the returned ITMH corresponds to the one of the ingress
 *   shaping2. This API does not retrieve PP-related
 *   information and does not parse PP-headers, e.g. Ethernet
 *   header in the case of Ethernet port. For this reason,
 *   this API targets mainly TM ports diagnostics. For PP
 *   diagnostics, use the PPD API 'received_packet_info_get'
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_diag_last_packet_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_DIAG_LAST_PACKET_INFO *last_packet
  );

uint32
  soc_pb_diag_last_packet_info_get_verify(
    SOC_SAND_IN  int                unit
  );


uint32
  soc_pb_diag_lbg_conf_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
  );

uint32
  soc_pb_diag_lbg_close_unsafe(
    SOC_SAND_IN  int                          unit
  );

/*********************************************************************
* NAME:
 *   soc_pb_diag_sample_enable_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Enable/disable diagnostic APIs.affects only APIs with
 *   type: need_sample
 * INPUT:
 *   SOC_SAND_IN  int                               unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint8                               enable -
 *     TRUE: diag APIs are enabled, FALSE diag APIs are
 *     disabled.
 * REMARKS:
 *   - when enabled will affect device power consuming
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_IN  uint8                               enable
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "soc_pb_diag_sample_enable_get_unsafe" API.
 *     Refer to "soc_pb_diag_sample_enable_get_unsafe" API for details.
*********************************************************************/
uint32
  soc_pb_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                               unit,
    SOC_SAND_OUT uint8                               *enable
  );


#if SOC_PB_DEBUG

/*********************************************************************
* NAME:
*     soc_pb_diag_tbls_dump_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     dump tables; one table; tables of one block, all tables
* INPUT:
*  SOC_SAND_IN  uint8    block_id -
*    the block ID that include the table/tables to print
*    Set SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS to dump all blocks.
*  SOC_SAND_IN  uint32   tbl_base_addr -
*    the base address of the table to dump.
*    Set to SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS to dump all tables in a
*    block blocks
* REMARKS:
*   1. block_id = SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS AND
*      tbl_base_addr = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS
*      Will dump all tables in all blocks.
*   2. block_id = "block_i" AND
*      tbl_base_addr = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS
*      Will dump all tables in the block "block_i".
*   3. block_id = "block_i" OR block_id = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS AND
*      tbl_base_addr = "table_base_address_val"
*      Will dump the table with the given base address.
*      i.e. to print one table: set tbl_base_addr to be the base address
*      of the table. and block_id to be the correct block id or SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_diag_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8    block_id,
    SOC_SAND_IN  uint32   tbl_offset,
    SOC_SAND_IN  uint8  print_zero
  );

/*********************************************************************
* NAME:
*     soc_pb_diag_dev_tbls_dump_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   print the tables according to the dump mode.
*
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
* REMARKS:
*   1.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_diag_dev_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
  );




/*********************************************************************
* NAME:
*     soc_pb_diag_signals_dump_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     dump signals from the device for last packet
* INPUT:
*  SOC_SAND_IN  uint32   flags -
* REMARKS:
*  has to call to soc_pb_diag_sample_enable_set_unsafe() with enable = true.
*  before calling this API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_pb_diag_signals_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   flags
  );

const char*
  SOC_PB_DIAG_DUMP_TBLS_BLOCK_ID_to_string(
    SOC_SAND_IN uint32 block_id
  );


#endif /* SOC_PB_DEBUG */

/*********************************************************************
* NAME:
 *   soc_pb_diagnostics_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   soc_pb_api_diagnostics module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_diagnostics_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   soc_pb_diagnostics_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   soc_pb_api_diagnostics module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_diagnostics_get_errs_ptr(void);

uint32
  SOC_PB_DIAG_LAST_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_DIAG_LAST_PACKET_INFO *info
  );


uint32
    soc_pb_diag_simple_mbist_sms_type_get(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx,
      SOC_SAND_OUT SOC_PETRA_DIAG_MBIST_SMS_TYPE    *sms_tp
    );

uint32
  soc_pb_diag_simple_mbist_sms_sel_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *sel_sz
  );

uint32
  soc_pb_diag_simple_mbist_sms_addr_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *addr_sz
  );
/* } */



#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_DIAGNOSTICS_INCLUDED__*/
#endif

