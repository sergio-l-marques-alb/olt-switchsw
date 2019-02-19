/* $Id: petra_cell.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/soc_petra/include/soc_petra_cell.h
*
* MODULE PREFIX:  soc_petra_cell
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

#ifndef __SOC_PETRA_CELL_INCLUDED__
/* { */
#define __SOC_PETRA_CELL_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/SAND_FM/sand_cell.h>
#include <soc/dpp/Petra/petra_api_cell.h>
#include <soc/dpp/Petra/petra_framework.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_SR_DATA_CELL_PAYLOAD_LENGTH_FE1600     (256)

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
   * The first field is the destination id or the multicast id
   */
  uint16   block_id;                              /* 6 bits                      */
  uint8    cell_data[SOC_SAND_DATA_CELL_PAYLOAD_IN_BYTES]; /* 256 bits                */

} SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600;

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


uint32
  soc_petra_transaction_with_fe1600(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST         *sr_link_list,
    SOC_SAND_IN  soc_reg_t                       reg,
    SOC_SAND_IN  soc_mem_t                       mem,
    SOC_SAND_IN  int32                        port_or_copyno,
    SOC_SAND_IN  int32                        index,
    SOC_SAND_IN  uint32                        *data_in,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint8                       is_write,
    SOC_SAND_IN  uint8                       is_inband,
    SOC_SAND_IN  uint8                       is_memory,
    SOC_SAND_OUT uint32                        *data_out
  );

/*********************************************************************
* NAME:
*     soc_petra_cell_init
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
  soc_petra_cell_init(
    SOC_SAND_IN  int                 unit
  );
 /*****************************************************
*NAME:
*   soc_petra_sr_send_cell
*DATE:
*   01/DEC/2008
*FUNCTION:
* sends a cell and acts on the trigger.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN  int                      unit
*                                           - the device id
*   SOC_SAND_IN  SOC_SAND_DATA_CELL                 *data_cell_sent,
*                                           - the data cell sent
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_sr_send_cell(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32       is_fe1600,
    SOC_SAND_IN  SOC_SAND_DATA_CELL  *data_cell_sent,
    SOC_SAND_IN  SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  );
/*****************************************************
*NAME:
*   soc_petra_sr_send_and_wait_ack
*DATE:
*   01/DEC/2008
*FUNCTION:
* sends a cell and waits for an acknowledgment.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN  int                       unit
*                                            - the device id
*   SOC_SAND_IN  SOC_SAND_DATA_CELL                 *data_cell_sent,
*                                           - the data cell sent
*   SOC_SAND_OUT SOC_SAND_DATA_CELL                 *data_cell,
*                                           - the data cell received
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_sr_send_and_wait_ack(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_SAND_DATA_CELL                   *data_cell_sent,
    SOC_SAND_IN  uint8                        is_fe1600,
    SOC_SAND_OUT SOC_SAND_DATA_CELL                   *data_cell_rcv,
    SOC_SAND_OUT SOC_PETRA_CELL_ADDED_DATA_FOR_FE1600 *added_data
  );
/*****************************************************
*NAME:
*   soc_petra_read_from_fe600_unsafe
*DATE:
*   01/DEC/2008
*FUNCTION:
* Scheduling of read direct command generated by the cpu
* via a source-routed cell.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN  int                        unit
*                                            - the sent device id
*   SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST  *sr_link_list
*                                            - the sent four link list
*   SOC_SAND_IN  uint32                         size
*                                            - the sent size
*   SOC_SAND_IN  uint32                         offset
*                                            - the sent offset
*   SOC_SAND_OUT uint32                         *data_out
*                                            - the received data
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  );
/*****************************************************
*NAME:
*   soc_petra_write_to_fe600_unsafe
*DATE:
*   01/DEC/2008
*FUNCTION:
* Scheduling of write direct command generated by the cpu
* via a source-routed cell.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN int                        unit
*                                            - the sent device id
*   SOC_SAND_IN SOC_PETRA_SR_CELL_LINK_LIST  *sr_link_list
*                                            - the sent four link list
*   SOC_SAND_IN uint32                         size
*                                            - the sent size
*   SOC_SAND_IN uint32                         offset
*                                            - the sent offset
*   SOC_SAND_IN uint32                         *data_in
*                                            - the sent data
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  );
/*****************************************************
*NAME:
*   soc_petra_indirect_read_from_fe600_unsafe
*DATE:
*   01/DEC/2008
*FUNCTION:
* Scheduling of read indirect command generated by the cpu
* via a source-routed cell.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN int                        unit
*                                            - the sent device id
*   SOC_SAND_IN SOC_PETRA_SR_CELL_LINK_LIST  *sr_link_list
*                                            - the sent four link list
*   SOC_SAND_IN uint32                         size
*                                            - the sent size
*   SOC_SAND_IN uint32                         offset
*                                            - the sent offset
*   SOC_SAND_OUT uint32                        *data_out
*                                            - the received data
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_indirect_read_from_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_OUT uint32                        *data_out
  );
/*****************************************************
*NAME:
*   soc_petra_indirect_write_to_fe600_unsafe
*DATE:
*   01/DEC/2008
*FUNCTION:
* Scheduling of read direct command generated by the cpu
* via a source-routed cell.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN int                        unit
*                                            - the sent device id
*   SOC_SAND_IN SOC_PETRA_SR_CELL_LINK_LIST  *sr_link_list
*                                            - the sent four link list
*   SOC_SAND_IN uint32                         size
*                                            - the sent size
*   SOC_SAND_IN uint32                         offset
*                                            - the sent offset
*   SOC_SAND_IN uint32                         *data_in
*                                            - the sent data
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/
uint32
  soc_petra_indirect_write_to_fe600_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        offset,
    SOC_SAND_IN  uint32                        *data_in
  );

/*****************************************************
*NAME:
*   soc_petra_cpu2cpu_write_unsafe
*DATE:
*   01/DEC/2008
*FUNCTION:
* Interaction with a CPU of SOC_SAND_FE600.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN int                        unit
*                                            - the sent device id
*   SOC_SAND_IN SOC_PETRA_SR_CELL_LINK_LIST  *sr_link_list
*                                            - the sent four link list
*   SOC_SAND_IN uint32                         size
*                                            - the sent size
*   SOC_SAND_IN uint32                         *data_in
*                                            - the sent data
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/


uint32
  soc_petra_cpu2cpu_write_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_IN  SOC_PETRA_SR_CELL_LINK_LIST          *sr_link_list,
    SOC_SAND_IN  uint32                        size,
    SOC_SAND_IN  uint32                        *data_in
  );

/*****************************************************
*NAME:
*   soc_petra_cpu2cpu_read_unsafe
*DATE:
*   01/DEC/2008
*FUNCTION:
* Interaction with a CPU of SOC_SAND_FE600.
*INPUT:
*  SOC_SAND_DIRECT:
*   SOC_SAND_IN int                        unit
*                                            - the sent device id
*   SOC_SAND_OUT uint32                        *data_out
*                                            - the received data
*  SOC_SAND_INDIRECT:
*OUTPUT:
*  SOC_SAND_DIRECT:
*    Non-Zero in case of an error.
*  SOC_SAND_INDIRECT:
*REMARKS:
*SEE ALSO:
*****************************************************/


uint32
  soc_petra_cpu2cpu_read_unsafe(
    SOC_SAND_IN  int                       unit,
    SOC_SAND_OUT uint32                        *data_out
  );

/*********************************************************************
* NAME:
 *   soc_petra_cell_mc_tbl_write_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure an entry of the SOC_SAND_FE600 multicast tables via
 *   inband indirect write cells.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 mc_id_ndx -
 *     Id of the multicast group (and also the entry index in
 *     the multicast table). Range: 0 - 16K-1.
 *   SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info -
 *     Settings of the inband cell for the table entry
 *     configuration.
 *   SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data -
 *     Data to write in the multicast table entry.
 * REMARKS:
 *   This function is designed to get optimized time
 *   performance - minimal number of logic and access
 *   operations - when configuring many entries in the
 *   multicast table in comparison to
 *   soc_petra_indirect_write_to_fe600().
 *   A global variable for a typical inband cell is set at the Driver
 *   initialization and is updated for each call to this API.
* RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_cell_mc_tbl_write_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  );

uint32
  soc_petra_cell_mc_tbl_write_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  );

/*********************************************************************
* NAME:
 *   soc_petra_cell_mc_tbl_read_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get an entry of the SOC_SAND_FE600 multicast tables via an inband
 *   indirect read cell.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 mc_id_ndx -
 *     Id of the multicast group (and also the entry index in
 *     the multicast table). Range: 0 - 16K-1.
 *   SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info -
 *     Settings of the inband cell for the table entry
 *     configuration.
 *   SOC_SAND_OUT SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data -
 *     Data written in the multicast table entry.
 * REMARKS:
 *   This function is designed to get optimized time
 *   performance - minimal number of logic and access
 *   operations - in comparison to
 *   soc_petra_indirect_read_from_fe600().
 *   A global variable for a typical inband cell is set at the Driver
 *   initialization and is updated for each call to this API.
* RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_cell_mc_tbl_read_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info,
    SOC_SAND_OUT SOC_PETRA_CELL_MC_TBL_DATA                    *tbl_data
  );

uint32
  soc_petra_cell_mc_tbl_read_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 mc_id_ndx,
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO                    *info
  );



uint32
  soc_petra_PETRA_CELL_MC_TBL_DATA_verify(
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_DATA *info
  );

uint32
  soc_petra_PETRA_CELL_MC_TBL_INFO_verify(
    SOC_SAND_IN  SOC_PETRA_CELL_MC_TBL_INFO *info
  );

#if SOC_PETRA_DEBUG

#endif /* SOC_PETRA_DEBUG */
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_CELL_INCLUDED__*/
#endif
