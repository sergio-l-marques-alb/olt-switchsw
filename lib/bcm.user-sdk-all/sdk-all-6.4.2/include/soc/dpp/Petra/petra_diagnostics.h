/* $Id: soc_petra_diagnostics.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PETRA_DIAGNOSTICS_INCLUDED__
/* { */
#define __SOC_PETRA_DIAGNOSTICS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/Petra/petra_api_framework.h>
#include <soc/dpp/Petra/petra_api_diagnostics.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
#include <soc/dpp/Petra/petra_api_ingress_packet_queuing.h>
#include <soc/dpp/Petra/petra_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_framework.h>
#include <soc/dpp/Petra/petra_general.h>

#include <soc/dpp/SAND/SAND_FM/sand_user_callback.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PETRA_DIAG_DRAM_NOF_BYTES_IN_MBYTES      (1024*1024)

/* $Id: soc_petra_diagnostics.h,v 1.5 Broadcom SDK $
 *  The CPU port used for LBG
 */
#define SOC_PETRA_DIAG_LBG_CPU_PORT_ID      0


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
  SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF = 0,
  SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D = 1,
  SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_S = 2,
  SOC_PETRA_DIAG_MBIST_SMS_NOF_TYPES = 3
}SOC_PETRA_DIAG_MBIST_SMS_TYPE;

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
*     soc_petra_diag_tbls_dump_unsafe
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
  soc_petra_diag_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8    block_id,
    SOC_SAND_IN  uint32   tbl_offset,
    SOC_SAND_IN  uint8  print_zero
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dev_tables_dump_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     dump tables; Dumps all device tables ;
*Dumps only part of device tables. These are the tables that are relevant for most debug cases.
* This dump is not traffic effecting (can be called under traffic).
*
* INPUT:
*  SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
*
* REMARKS:
*
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dev_tables_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
  );

uint32
  soc_petra_diag_ipt_rate_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_SAND_64CNT               *rate
  );

uint32
  soc_petra_diag_iddr_set_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_IN  uint32     *data,
    SOC_SAND_IN  uint32     size
  );

uint32
  soc_petra_diag_iddr_get_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32    dram_ndx,
    SOC_SAND_IN  uint32    bank_ndx,
    SOC_SAND_IN  uint32     dram_offset,
    SOC_SAND_OUT uint32     *data,
    SOC_SAND_IN  uint32     size
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_init
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
  soc_petra_diag_init(
    SOC_SAND_IN  int                 unit
  );


/*********************************************************************
* NAME:
*     soc_petra_nif_diag_last_packet_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Read the last packet header entered the NIF and returns
*     the relevant fields.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *last_incoming_header -
*     Fields of the last ITMH header
* RETURNS:
*     OK or ERROR indication.
* REMARKS:
 *   1. If the packet is processed with ingress shaping, then
 *   the returned ITMH corresponds to the one of the ingress
 *   shaping. 2. For Soc_petra-A, not valid for Raw and Ethernet
 *   ports.
*********************************************************************/

uint32
  soc_petra_nif_diag_last_packet_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_PORTS_ITMH          *last_incoming_header
  );

/************************************************************************/
/* BIST APIs                                                            */
/************************************************************************/
uint32
  soc_petra_diag_dram_presettings_set(
    SOC_SAND_IN  int         unit,
    SOC_SAND_IN  uint32          dram_ndx,
    SOC_SAND_IN  uint32          nof_reads,
    SOC_SAND_IN  uint32          nof_writes
  );

uint32
  soc_petra_diag_dram_access(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                         dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO      *info,
    SOC_SAND_IN  uint32                         size_in_bytes,
    SOC_SAND_IN  uint32                         *buffer,
    SOC_SAND_IN  uint8                        is_read_and_write_performed
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_write_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Write a pattern of 256 bits long into the DRAM via the
*     BIST mechanism.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  dram_ndx -
*     DRAM index.
*  SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info -
*     Information for writing in the DRAM.
*  SOC_SAND_IN  uint32                  *buffer -
*     The pattern to write.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dram_write_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info,
    SOC_SAND_IN  uint32                  *buffer
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_read_and_compare_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Indicates if a pattern is written in the specified
*     addresses by reading and comparing the written pattern
*     with the expected data via the BIST mechanism.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  dram_ndx -
*     DRAM index.
*  SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info -
*     Information for reading with a comparison in the DRAM.
*  SOC_SAND_IN  uint32                  *buff_expected -
*     Expected pattern. The actual value is compared to this
*     pattern.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status -
*     Indicates if errors have been found and their
*     characteristics.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dram_read_and_compare_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_ACCESS_INFO *info,
    SOC_SAND_IN  uint32                  *buff_expected,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_READ_COMPARE_STATUS *status
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_qdr_BIST_test_start_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Run BIST test with the given parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *test_info -
*     Information for running the BIST test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_start_unsafe(
  SOC_SAND_IN  int                 unit,
  SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO *test_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_qdr_BIST_test_result_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Stop BIST test and return the result.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO *res_info -
*     Result of the test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_qdr_BIST_test_result_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_BIST_TEST_RES_INFO  *res_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_BIST_test_start_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Run DRAM BIST test with the given parameters.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  dram_ndx -
*     Dram index.
*  SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info -
*     Information for running the BIST test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test. - The accessed addresses are the ones with a
*     value between test_info.start_addr and
*     test_info.end_addr.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_start_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_BIST_test_result_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Stop DRAM BIST test and return the result.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 mem_ndx -
*     Memory index.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *res_info -
*     Result of the test.
* REMARKS:
*     - All traffic and indirect access must stop before run
*     BIST test. - if the test start when infinite mode then
*     this operation will stop the BIST and return the result.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_BIST_test_result_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                          dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_BIST_TEST_RES_INFO *res_info
  );

uint32
  soc_petra_diag_qdr_BIST_test_start_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_BIST_TEST_RUN_INFO  *test_info
  );

uint32
  soc_petra_diag_dram_BIST_test_start_verify(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  uint32                           dram_ndx,
    SOC_SAND_IN  SOC_PETRA_DIAG_DRAM_BIST_TEST_RUN_INFO *test_info
  );


/*********************************************************************
* NAME:
*     soc_petra_diag_dll_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     This function is used as a diagnostics tool that gives
*     indications about the dll status.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 dram_ndx -
*     DRAM index. Range: 0 - 5.
*  SOC_SAND_IN  uint32                 dll_ndx -
*     DLL index. Range: 0 - 3.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *dll_stt_info -
*     Different indications about the dll.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dll_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_IN  uint32                 dll_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DLL_STATUS_INFO *dll_stt_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_dram_status_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*     Get DRAM diagnostic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 dram_ndx -
*     DRAM index. Range: 0 - 5.
*  SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *dram_status -
*     Different indications about the DRAM.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_diag_dram_status_get_unsafe(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_STATUS_INFO *dram_status
  );

/*********************************************************************
* NAME:
*   soc_petra_diag_dram_diagnostic_get_unsafe
* TYPE:
*   PROC
* FUNCTION:
*   Get diagnostic for the DRAM interface training sequence.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                 dram_ndx -
*     DRAM index. Range: 0 - 5.
*   SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO                  *error_info -
*     Different possible errors in the DRAM interface.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_diagnostic_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_OUT SOC_PETRA_DIAG_DRAM_ERR_INFO                  *error_info
  );

/* Retrieves roundtrip error count */
uint32
  soc_petra_diag_dram_roundtrip_status_check_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx,
    SOC_SAND_OUT uint32                                  *err_count
  );

/*********************************************************************
* NAME:
*   soc_petra_diag_dram_diagnostic_get_verify
* TYPE:
*   PROC
* FUNCTION:
*   Get diagnostic for the DRAM interface training sequence.
* INPUT:
*   SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*   SOC_SAND_IN  uint32                                 dram_ndx -
*     DRAM index. Range: 0 - 5.
* REMARKS:
*   None.
* RETURNS:
*   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_dram_diagnostic_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                 dram_ndx
  );

/*********************************************************************
* NAME:
 *   soc_petra_diag_qdr_window_validity_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   A diagnostic tool that determines the QDR window
 *   validity by using a series of BIST commands.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern -
 *     Selects the pattern to use in the BIST. Both a
 *     user-defined and an automatic random pattern are
 *     supported.
 *   SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO          *window_info -
 *     Parameters characterizing the QDR window validity.
 * REMARKS:
 *   1. The expected ratio is between 50% and 70%.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_qdr_window_validity_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern,
    SOC_SAND_OUT SOC_PETRA_DIAG_QDR_WINDOW_INFO          *window_info
  );

uint32
  soc_petra_diag_qdr_window_validity_get_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_PATTERN              *pattern
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_conf_set_unsafe
* FUNCTION:
*     - save previous relevant configuration of Soc_petra.
*     - configure the Soc_petra to perform LBG test
*       -Raw port snake configuration.
*
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     - All traffic must stop before run LBG test
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_conf_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
  );


/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_traffic_send_unsafe
* FUNCTION:
*  send traffic using CPU.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* REMARKS:
*     - All traffic must stop before run LBG test
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_traffic_send_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_TRAFFIC_INFO        *traffic_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_result_get_unsafe
* FUNCTION:
*  Get the counters and traffic rate. resulted from the last call
*  of soc_petra_diag_lbg_traffic_send.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_result_get_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_OUT  SOC_PETRA_DIAG_LBG_RESULT_INFO        *res_info
  );

/*********************************************************************
* NAME:
*     soc_petra_diag_lbg_close_unsafe
* FUNCTION:
*  Close the LBG test and configure the configuration
*  as were before the call to soc_petra_diag_lbg_conf_set.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_lbg_close_unsafe(
    SOC_SAND_IN  int                          unit
  );

/*
 *  Run MBIST on a specified SMS.
 *  To run on all memories, use SOC_PETRA_DIAG_SMS_ALL as the sms value.
 *  SMS index memory to write the pattern in. 
 *  sms is in range: 1 -  29 (SOC_PETRA_DIAG_SMS_ALL).
 */
uint32
  soc_petra_diag_mbist_run_unsafe(
    SOC_SAND_IN  int    unit,
    SOC_SAND_IN  uint32     sms,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT   *sms_result[SOC_PETRA_DIAG_SMS_MAX]
  );

/*********************************************************************
* NAME:
 *   soc_petra_diag_soft_error_test_start_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Start the soft-error test analyzing the memory validity.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info -
 *     Parameters of the test: pattern format and SMS to be
 *     used for the tests.
 * REMARKS:
 *   The test overrides all the memory values. The device
 *   must be reset after performing this test.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_start_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  uint8                           is_mbist_only
  );

uint32
  soc_petra_diag_soft_error_test_start_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info
  );


uint32
  soc_petra_diag_simple_mbist_sms_sel_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *sel_sz
  );

/*********************************************************************
* NAME:
 *   soc_petra_diag_soft_error_test_result_get_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Start the soft-error test analyzing the memory validity.
 * INPUT:
 *   SOC_SAND_IN  int                           unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info -
 *     Parameters of the test: pattern format and SMS to be
 *     used for the tests.
 *   SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type -
 *     Count type of the errors.
 *   SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT          **sms_result -
 *     Pointers of SMS results. The user must allocate the
 *     pointers before calling the API.
 *   SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT          *glbl_result -
 *     Global error result when added on all the analyzed
 *     SMSes.
 * REMARKS:
 *   1. The test must be previously started via the
 *   soc_petra_diag_soft_error_test_start API.2. The number of
 *   SMS results pointers to be allocated is equal to the
 *   number of SMS to analyze. The SMS result is (maximally)
 *   an array of
 *   [SOC_PETRA_DIAG_NOF_SMS][SOC_PETRA_DIAG_NOF_SONE_PER_SMS_MAX]3.
 *   The first SONE result per SMS result is the global SMS
 *   result (the only relevant in case of 'FAST' count type).
 *   The other SMS results are the per SONE results (1 - up
 *   to 8K)
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_diag_soft_error_test_result_get_unsafe(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type,
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_SMS_RESULT          *sms_result[SOC_PETRA_DIAG_SMS_MAX][SOC_PETRA_DIAG_NOF_SONE_SAVED_PER_SMS_MAX],
    SOC_SAND_OUT SOC_PETRA_DIAG_SOFT_ERR_RESULT          *glbl_result
  );

uint32
  soc_petra_diag_soft_error_test_result_get_verify(
    SOC_SAND_IN  int                           unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO            *info,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_COUNT_TYPE          count_type
  );

uint32
  soc_petra_PETRA_DIAG_SOFT_ERR_INFO_verify(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_INFO *info
  );

uint32
  soc_petra_PETRA_DIAG_SOFT_ERR_RESULT_verify(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_ERR_RESULT *info
  );

uint32
  soc_petra_PETRA_DIAG_SOFT_SMS_RESULT_verify(
    SOC_SAND_IN  SOC_PETRA_DIAG_SOFT_SMS_RESULT *info
  );
#if SOC_PETRA_DEBUG

uint32
  soc_petra_egq_resources_print_unsafe(
    SOC_SAND_IN  int unit
  );

const char*
  soc_petra_PETRA_DIAG_DUMP_TBLS_BLOCK_ID_to_string(
    SOC_SAND_IN uint32 block_id
  );


#endif /* SOC_PETRA_DEBUG */

uint32
  soc_petra_PETRA_DIAG_QDR_WINDOW_INFO_verify(
    SOC_SAND_IN  SOC_PETRA_DIAG_QDR_WINDOW_INFO *info
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_DIAGNOSTICS_INCLUDED__*/
#endif

