/* $Id: pb_diagnostics.c,v 1.10 Broadcom SDK $
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Utils/sand_64cnt.h>
#include <soc/dpp/SAND/Utils/sand_bitstream.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_mem_access.h>
#include <soc/dpp/SAND/SAND_FM/sand_trigger.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/SAND/Utils/sand_integer_arithmetic.h>
#include <soc/dpp/SAND/Utils/sand_framework.h>

#include <soc/dpp/Petra/PB_TM/pb_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>
#include <soc/dpp/Petra/PB_TM/pb_api_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_diagnostics.h>
#include <soc/dpp/Petra/PB_TM/pb_ports.h>
#include <soc/dpp/Petra/PB_TM/pb_nif.h>
#include <soc/dpp/Petra/PB_TM/pb_tbl_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_reg_access.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/petra_general.h>
#include <soc/dpp/Petra/petra_chip_tbls.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_ports.h>
#include <soc/dpp/Petra/petra_mgmt.h>
#include <soc/dpp/Petra/petra_header_parsing_utils.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE                 (0x1000)
#define SOC_PB_DIAG_TBLS_DUMP_MAX_WORD_SIZE                     (10)



#define SOC_PB_DIAGNOSTICS_TM_PORT_MAX                             (79)
#define SOC_PB_DIAGNOSTICS_PP_PORT_MAX                             (63)

#define SOC_PB_PP_DIAG_MAX_NOF_DUMP_TABLES 800

/* $Id: pb_diagnostics.c,v 1.10 Broadcom SDK $
 * Whether to print the name of the table.
 * this option to enable saving place in the object file.
 * (the address and block name of the table always will be printed)
 */
#define SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT 1


#define SOC_PB_DIAG_LB_CPU_TM_PROFILE     0

#define SOC_PB_DIAG_BLK_NOF_BITS (768)

#define SOC_PB_DIAG_TBLS_DUMP_MAX_WORD_SIZE                     (10)

#define SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE                 (0x1000)

#define SOC_PB_DIAG_LAST_PACKET_INFO_clear                      SOC_TMC_DIAG_LAST_PACKET_INFO_clear

#define SOC_PB_PORT_IS_LAG_BIT                                  (12)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(width_bits)                           \
  SOC_SAND_DIV_ROUND_UP(width_bits, SOC_SAND_NOF_BITS_IN_UINT32) * sizeof(uint32)


#define SOC_PB_DIAG_FLD_READ(prm_fld,prm_blk, prm_addr_msb, prm_addr_lsb,prm_fld_msb,prm_fld_lsb,prm_err_num)  \
  (prm_fld)->addr.base = (prm_addr_msb << 16) + prm_addr_lsb;  \
  (prm_fld)->msb = prm_fld_msb;  \
  (prm_fld)->lsb= prm_fld_lsb;  \
  res = soc_pb_diag_dbg_val_get_unsafe(      \
  unit,      \
  prm_blk,      \
  prm_fld,      \
  regs_val      \
  );              \
  SOC_SAND_CHECK_FUNC_RESULT(res, prm_err_num, exit);

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

typedef struct{
  uint32      base;
  uint32      size;
  uint32     wrd_sz;
  uint32     mod_id;
  const char    *name;
}SOC_PB_TBL_PRINT_INFO;




/* } */

/*************
 * GLOBALS   *
 *************/
/* { */
static uint8 Soc_pb_diag_blk_msb_max[] = {9,14,0,0};
static uint8 Soc_pb_diag_blk_lsb_max[] = {6,10,3,8};
static uint8 Soc_pb_diag_blk_id[] = {SOC_PB_IHP_ID,SOC_PB_IHB_ID,SOC_PB_EGQ_ID,SOC_PB_EPNI_ID};
static
  SOC_PROCEDURE_DESC_ELEMENT
    Soc_pb_procedure_desc_element_diagnostics[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_DIAG_LAST_PACKET_INFO_GET),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_DIAG_LAST_PACKET_INFO_GET_PRINT),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_DIAG_LAST_PACKET_INFO_GET_UNSAFE),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_DIAG_LAST_PACKET_INFO_GET_VERIFY),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_DIAGNOSTICS_GET_PROCS_PTR),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PB_DIAGNOSTICS_GET_ERRS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

static
  SOC_ERROR_DESC_ELEMENT
    Soc_pb_error_desc_element_diagnostics[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  {
    SOC_PB_DIAGNOSTICS_TM_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_DIAGNOSTICS_TM_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'tm_port' is out of range. \n\r "
    "The range is: 0 - 79.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  {
    SOC_PB_DIAGNOSTICS_PP_PORT_OUT_OF_RANGE_ERR,
    "SOC_PB_DIAGNOSTICS_PP_PORT_OUT_OF_RANGE_ERR",
    "The parameter 'pp_port' is out of range. \n\r "
    "The range is: 0 - 63.\n\r ",
    SOC_SAND_SVR_ERR,
    FALSE
  },
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_ERR_DESC_ELEMENT_DEF_LAST
};

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

STATIC
  uint32
    soc_pb_diag_lpm_port_state_save(
      SOC_SAND_IN  int              unit,
      SOC_SAND_IN  uint32               port_indx,
      SOC_SAND_IN  uint32               pp_port_ndx,
      SOC_SAND_IN  SOC_PETRA_INTERFACE_ID     nif_ndx
    )
{
  SOC_PB_SW_DB_LBG_PORT
    port_stat;
  SOC_PB_SW_DB_LBG_PP_PORT_TRAP
    profile_stat;
  uint32
    trap_ndx,
    res;
  SOC_PB_PP_IHP_PINFO_LLR_TBL_DATA
    pinfo_llr_tbl;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info;
  uint32
    pp_port_out;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PETRA_DIAG_LPM_PORT_STATE_SAVE);

 /*
  * Get the hardware status and save it.
  */
  res = soc_pb_port_header_type_get_unsafe(
          unit,
          port_indx,
          &(port_stat.headers[SOC_PETRA_PORT_DIRECTION_INCOMING]),
          &(port_stat.headers[SOC_PETRA_PORT_DIRECTION_OUTGOING])
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	Get the PP-Port of this Port
   */
  res = soc_pb_port_to_pp_port_map_get_unsafe(
          unit,
          port_indx,
          &(port_stat.pp_port),
          &pp_port_out
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  /*
   *	Get the Port-TM-Profile trap definition
   */
  res = soc_pb_port_forwarding_header_configuration_get_unsafe(
          unit,
          port_stat.pp_port,
          &trap_ndx,
          &(profile_stat.data),
          &(profile_stat.snoop)
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

  res = soc_pb_pp_ihp_pinfo_llr_tbl_get_unsafe(
          unit,
          pp_port_ndx,
          &pinfo_llr_tbl
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 25, exit);

  profile_stat.trap_code = pinfo_llr_tbl.default_cpu_trap_code;

  if (nif_ndx != SOC_PETRA_IF_ID_CPU)
  {
    res = soc_pb_nif_loopback_get_unsafe(
            unit,
            nif_ndx,
            &(port_stat.is_nif_lpbck_on)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
  }
  else
  {
    port_stat.is_nif_lpbck_on = TRUE;
  }

  port_stat.saved_valid = TRUE;
  profile_stat.saved_valid = TRUE;

  res = soc_pb_sw_db_lbg_port_stat_save(
          unit,
          port_indx,
          &port_stat
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

  res = soc_pb_sw_db_lbg_profile_trap_stat_save(
          unit,
          pp_port_ndx,
          &profile_stat
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

  /*
   *	Get the PP-Port parameters
   */
  SOC_PB_PORT_PP_PORT_INFO_clear(&pp_port_info);
  res = soc_pb_port_pp_port_get_unsafe(
          unit,
          pp_port_ndx,
          &pp_port_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);

  /*
   *	Save this profile parameters
   */
  res = soc_pb_sw_db_lbg_profile_info_stat_save(
          unit,
          pp_port_ndx,
          &pp_port_info
       );
  SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_petra_diag_lpm_port_state_save()",0,0);
}

uint32
  soc_pb_diag_lbg_raw_port_forwarding_set_unsafe(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                        port_ndx,
    SOC_SAND_IN  uint32                        pp_port,
    SOC_SAND_IN  SOC_PB_PORTS_FORWARDING_HEADER_INFO *info
  )
{
  uint32
    res;
  SOC_PB_PORT_PP_PORT_INFO
    pp_port_info;
  SOC_SAND_SUCCESS_FAILURE
    success;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LBG_RAW_PORT_FORWARDING_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(info);

  /*
   *  1. Define the TM-Port as Raw
   */
  res = soc_petra_port_header_type_set_unsafe(
          unit,
          port_ndx,
          SOC_PETRA_PORT_DIRECTION_BOTH,
          SOC_PETRA_PORT_HEADER_TYPE_RAW
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  /*
   *	2. Set the PP-Port defined as Raw
   */
  SOC_PB_PORT_PP_PORT_INFO_clear(&pp_port_info);
  pp_port_info.fc_type = 0; /* No influence */
  pp_port_info.header_type = SOC_PETRA_PORT_HEADER_TYPE_RAW;
  res = soc_pb_port_pp_port_set_unsafe(
          unit,
          pp_port,
          &pp_port_info,
          &success
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  /*
   *	Exit if not success
   */
  if (success != SOC_SAND_SUCCESS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_DIAG_LBG_TM_PROFILE_OUT_OF_RSRC_ERR, 40, exit);
  }

  /*
   *	3. Map the TM port to its profile
   */
  res = soc_pb_port_to_pp_port_map_set_unsafe(
          unit,
          port_ndx,
          SOC_PETRA_PORT_DIRECTION_INCOMING,
          pp_port
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

  /*
   *	4. Set the forwarding header for this Raw port (profile)
   */
  res = soc_pb_port_forwarding_header_set_unsafe(
          unit,
          pp_port,
          info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);


exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_lbg_raw_port_forwarding_set_unsafe()",0,0);
}



uint32
  soc_pb_diag_lbg_conf_set_unsafe(
    SOC_SAND_IN  int                          unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_LBG_INFO                *lbg_info
  )
{
  uint32
    port_indx;
  uint32
    pp_port_ndx,
    next_local_port_id,
    cur_local_port_id,
    sys_port_id,
    sys_fap_id_self;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    port2if_in,
    port2if_out;
  uint32
    res;
  SOC_PB_PORTS_FORWARDING_HEADER_INFO
    forwarding_header_info;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LBG_CONF_SET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(lbg_info);

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_in);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_out);

  /*
   *	1. Check the number of ports is inferior to the
   *     number of PP-Port
   */
  if (lbg_info->path.nof_ports >= SOC_PB_PORT_NOF_PP_PORTS)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_DIAG_LBG_PATH_NOF_PORTS_OUT_OF_RANGE_ERR, 10, exit);
  }

  /*
   *	2. Save the actual configuration per Port and PP-Port
   */
  res = soc_pb_diag_lpm_port_state_save(
          unit,
          SOC_PETRA_DIAG_LBG_CPU_PORT_ID,
          SOC_PB_DIAG_LB_CPU_TM_PROFILE,
          SOC_PETRA_IF_ID_CPU
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

 /*
  * Set CPU port to raw and to send to the first port in the list.
  */
  res = soc_petra_mgmt_system_fap_id_get_unsafe(
        unit,
        &sys_fap_id_self
      );
  SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

  res = soc_petra_local_to_sys_phys_port_map_get_unsafe(
          unit,
          sys_fap_id_self,
          lbg_info->path.ports[0],
          &sys_port_id
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

  SOC_PB_PORTS_FORWARDING_HEADER_INFO_clear(&forwarding_header_info);
  /* Handle also a LAG destination */
  if (SOC_SAND_GET_BIT(sys_port_id, SOC_PB_PORT_IS_LAG_BIT) == 0x1)
  {
    forwarding_header_info.destination.type = SOC_PETRA_DEST_TYPE_LAG;
    forwarding_header_info.destination.id = SOC_SAND_GET_BITS_RANGE(sys_port_id, SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_MSB, SOC_PETRA_HPU_OTMH_SRC_PORT_LAG_ID_LSB);
  }
  else
  {
    forwarding_header_info.destination.type = SOC_PETRA_DEST_TYPE_SYS_PHY_PORT;
    forwarding_header_info.destination.id = sys_port_id;
  }
  forwarding_header_info.counter.processor_id = 0; /* No influence */

  res = soc_pb_diag_lbg_raw_port_forwarding_set_unsafe(
          unit,
          SOC_PETRA_DIAG_LBG_CPU_PORT_ID,
          SOC_PB_DIAG_LB_CPU_TM_PROFILE,
          &forwarding_header_info
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

 /*
  * Build snake that includes all the given ports.
  */
  for (port_indx = 0; port_indx < lbg_info->path.nof_ports; ++port_indx)
  {
   /*
    * If the previous status of the ports is not already saved, save it.
    * Take the PP-Port = port_indx + 1
    */
    cur_local_port_id = lbg_info->path.ports[port_indx];
    pp_port_ndx = port_indx + 1;

    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            cur_local_port_id,
            &port2if_in,
            &port2if_out
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    res = soc_pb_diag_lpm_port_state_save(
            unit,
            cur_local_port_id,
            pp_port_ndx,
            port2if_in.if_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 70, exit);

   /*
    * If this is not last port make it forward to the next port in the list
    */
    if (port_indx < lbg_info->path.nof_ports - 1)
    {
      next_local_port_id = lbg_info->path.ports[port_indx + 1];
    }
   /*
    * otherwise make it forward back to the first port in the list (make loop).
    */
    else
    {
      next_local_port_id = lbg_info->path.ports[0];
    }

    res = soc_petra_local_to_sys_phys_port_map_get_unsafe(
            unit,
            sys_fap_id_self,
            next_local_port_id,
            &sys_port_id
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 80, exit);

    forwarding_header_info.destination.id = sys_port_id;

    res = soc_pb_diag_lbg_raw_port_forwarding_set_unsafe(
            unit,
            cur_local_port_id,
            pp_port_ndx,
            &forwarding_header_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 90, exit);


    res = soc_pb_nif_loopback_set_unsafe(
            unit,
            port2if_in.if_id,
            TRUE
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 100, exit);
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_lbg_conf_set_unsafe()",0,0);
}

STATIC
  uint32
    soc_pb_diag_lpm_port_state_load_unsafe(
      SOC_SAND_IN  int                         unit,
      SOC_SAND_IN  uint32                          port_indx
    )
{
  uint8
    saved;
  SOC_PB_SW_DB_LBG_PORT
    port_stat;
  uint32
    res;
  SOC_PETRA_PORT2IF_MAPPING_INFO
    port2if_in,
    port2if_out;
  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL_DATA
    tm_port_pp_port_config_tbl;
  uint32
    pp_port;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LPM_PORT_STATE_LOAD_UNSAFE);

  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_in);
  soc_petra_PETRA_PORT2IF_MAPPING_INFO_clear(&port2if_out);

 /*
  * Consider only saved configurations (modified since).
  */
  saved = soc_pb_sw_db_lbg_port_save_valid_get(unit, port_indx);

  if (saved == TRUE)
  {
    res = soc_pb_sw_db_lbg_port_stat_load(
            unit,
            port_indx,
            &port_stat
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_petra_port_to_interface_map_get_unsafe(
            unit,
            port_indx,
            &port2if_in,
            &port2if_out
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 72, exit);

    if (port2if_in.if_id != SOC_PETRA_IF_ID_CPU)
    {
      res = soc_pb_nif_loopback_set_unsafe(
              unit,
              port2if_in.if_id,
              port_stat.is_nif_lpbck_on
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 74, exit);
    }

    /*
     *	Set back the mapping from port to PP-Port
     */
    if (port_stat.pp_port < SOC_PB_PORT_NOF_PP_PORTS)
    {
      pp_port = port_stat.pp_port;
      res = soc_pb_port_to_pp_port_map_set_unsafe(
              unit,
              port_indx,
              SOC_PETRA_PORT_DIRECTION_INCOMING,
              pp_port
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);
    }
    else
    {
      res = SOC_SAND_OK; sal_memset(&tm_port_pp_port_config_tbl, 0x0, sizeof(tm_port_pp_port_config_tbl));
      SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

      res = soc_pb_ihp_tm_port_pp_port_config_tbl_set_unsafe(
              unit,
              port_indx,
              &tm_port_pp_port_config_tbl
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);
    }

   /*
    *  Set back the hardware status
    *  (Header type after the TM to TM-Profile mapping).
    */
    res = soc_petra_port_header_type_set_unsafe(
            unit,
            port_indx,
            SOC_PETRA_PORT_DIRECTION_INCOMING,
            port_stat.headers[SOC_PETRA_PORT_DIRECTION_INCOMING]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

    res = soc_petra_port_header_type_set_unsafe(
            unit,
            port_indx,
            SOC_PETRA_PORT_DIRECTION_OUTGOING,
            port_stat.headers[SOC_PETRA_PORT_DIRECTION_OUTGOING]
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    soc_pb_sw_db_lbg_port_save_valid_set(unit, port_indx, FALSE);

  } /* Previous configuration saved */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_lpm_port_state_load_unsafe()", port_indx, 0);
}


STATIC
  uint32
    soc_pb_diag_lpm_profile_load_unsafe(
      SOC_SAND_IN  int                         unit,
      SOC_SAND_IN  uint32                          pp_port_ndx
    )
{
  uint8
    saved;
  uint32
    trap_ndx,
    res;
  SOC_PB_PORT_PP_PORT_INFO
    profile_info;
  SOC_SAND_SUCCESS_FAILURE
    success;
  SOC_PB_SW_DB_LBG_PP_PORT_TRAP
    profile_trap;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LPM_PROFILE_LOAD_UNSAFE);

 /*
  * Consider only saved configurations (modified since).
  */
  saved = soc_pb_sw_db_lbg_profile_save_valid_get(unit, pp_port_ndx);

  if (saved == TRUE)
  {
    /*
     *	1. Set back the Profile parameters
     */
    SOC_PB_PORT_PP_PORT_INFO_clear(&profile_info);
    res = soc_pb_sw_db_lbg_profile_info_stat_load(
            unit,
            pp_port_ndx,
            &profile_info
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

    res = soc_pb_port_pp_port_set_unsafe(
            unit,
            pp_port_ndx,
            &profile_info,
            &success
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

    /*
     *	Exit if not success
     */
    if (success != SOC_SAND_SUCCESS)
    {
      SOC_SAND_SET_ERROR_CODE(SOC_PB_DIAG_LBG_TM_PROFILE_OUT_OF_RSRC_ERR, 40, exit);
    }

    /*
     *	2. Set back the trap parameters
     */
    res = soc_pb_port_forwarding_header_configuration_get_unsafe(
            unit,
            pp_port_ndx,
            &trap_ndx,
            &(profile_trap.data),
            &(profile_trap.snoop)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

    res = soc_pb_sw_db_lbg_profile_trap_stat_load(
            unit,
            pp_port_ndx,
            &profile_trap
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 50, exit);

    /*
     *	Set to the Hardware
     */
    res = soc_pb_port_forwarding_header_configuration_set_unsafe(
            unit,
            pp_port_ndx,
            profile_trap.trap_code,
            &(profile_trap.data),
            &(profile_trap.snoop)
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 60, exit);

    soc_pb_sw_db_lbg_profile_save_valid_set(unit, pp_port_ndx, FALSE);

  } /* Previous configuration saved */

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_lpm_profile_load_unsafe()", pp_port_ndx, 0);
}


uint32
  soc_pb_diag_lbg_close_unsafe(
    SOC_SAND_IN  int                          unit
  )
{
  uint32
    pp_port_ndx,
    port_indx;
  uint32
    res;
  SOC_PETRA_REGS
    *regs;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LBG_CLOSE_UNSAFE);

  regs = soc_petra_regs();

  /*
   *	Set back the Port and Port profile configuration
   */
  for (pp_port_ndx = 0; pp_port_ndx < SOC_PB_PORT_NOF_PP_PORTS; ++pp_port_ndx)
  {
    res = soc_pb_diag_lpm_profile_load_unsafe(
            unit,
            pp_port_ndx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }

  for (port_indx = 0; port_indx < SOC_PETRA_NOF_FAP_PORTS; ++port_indx)
  {
    res = soc_pb_diag_lpm_port_state_load_unsafe(
            unit,
            port_indx
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);
  }

  /*
   *	Discard all remaining packets
   */
  SOC_PETRA_FLD_SET(regs->iqm.iqm_enablers_reg.dscrd_all_pkt, 0x1, 30, exit);
  sal_msleep(100);
  SOC_PETRA_FLD_SET(regs->iqm.iqm_enablers_reg.dscrd_all_pkt, 0x0, 32, exit);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_lbg_close_unsafe()",0,0);
}


/*********************************************************************
 *     read diagnostic value
 *********************************************************************/


uint32
  soc_pb_diag_dbg_val_get_unsafe(
    SOC_SAND_IN  int              unit,
    SOC_SAND_IN  uint32               blk,
    SOC_SAND_IN  SOC_PB_DIAG_REG_FIELD      *fld,
    SOC_SAND_OUT uint32               reg_val[SOC_PB_DIAG_DBG_VAL_LEN]
  )
{
  uint32
  tbl_entry_indx,
  reg_indx,
  entry_start,
  entry_end;
  SOC_PETRA_REGS
    *regs;
  SOC_PB_PP_REGS
    *pp_regs;
  SOC_PB_EGQ_ERPP_DEBUG_TBL_DATA
    erpp_debug_tbl_data;
  SOC_PB_PP_EPNI_ETPP_DEBUG_TBL_DATA
    epni_etpp_debug_tbl_data;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_DBG_VAL_GET_UNSAFE);
  SOC_PETRA_CLEAR(reg_val,uint32,SOC_PB_DIAG_DBG_VAL_LEN);

  regs = soc_petra_regs();
  pp_regs = soc_pb_pp_regs();

  if (blk == SOC_PB_IHP_ID)
  {
    SOC_PB_FLD_SET(regs->ihp.ihp_debug_reg.dbg_addr,fld->addr.base,10,exit);
    res = soc_petra_read_reg_buffer_unsafe(
            unit,
            SOC_PB_REG_DB_ACC_REF(regs->ihp.ihp_debug_res_reg.addr),
            SOC_TMC_DEFAULT_INSTANCE,
            SOC_PB_DIAG_DBG_VAL_LEN,
            reg_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (blk == SOC_PB_IHB_ID)
  {
    SOC_PB_FLD_SET(regs->ihb.ihb_debug_reg.dbg_addr,fld->addr.base,20,exit);
    res = soc_petra_read_reg_buffer_unsafe(
            unit,
            SOC_PB_REG_DB_ACC_REF(regs->ihb.ihb_debug_res_reg.addr),
            SOC_TMC_DEFAULT_INSTANCE,
            SOC_PB_DIAG_DBG_VAL_LEN,
            reg_val
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);
  }
  else if (blk == SOC_PB_EGQ_ID)
  {
    SOC_PB_PP_FLD_SET(pp_regs->egq.erpp_debug_configuration_reg.debug_select,fld->addr.base,30,exit);
    res = soc_pb_egq_erpp_debug_tbl_get_unsafe(
            unit,
            0,
            &erpp_debug_tbl_data
          );
    SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
    SOC_PETRA_COPY(reg_val,erpp_debug_tbl_data.erpp_debug,uint32,8);
  }
  else if (blk == SOC_PB_EPNI_ID)
  {
    SOC_PB_PP_FLD_SET(pp_regs->epni.etpp_debug_configuration_reg.debug_select,fld->addr.base,40,exit);

    entry_start = fld->lsb /32;
    entry_end = fld->msb /32;
    reg_indx = 0;
    for (tbl_entry_indx=entry_start; tbl_entry_indx<=entry_end; ++tbl_entry_indx)
    {
      res = soc_pb_pp_epni_etpp_debug_tbl_get_unsafe(
              unit,
              tbl_entry_indx,
              &epni_etpp_debug_tbl_data
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);
      reg_val[reg_indx++] = epni_etpp_debug_tbl_data.etpp_debug;
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_dbg_val_get_unsafe()", 0, 0);
}



uint32
  soc_pb_diag_tbls_dump_tables_get(
    SOC_SAND_OUT SOC_PB_TBL_PRINT_INFO   *indirect_print,
    SOC_SAND_IN  uint8                block_id,
    SOC_SAND_IN  uint32               table_offset,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode,
    SOC_SAND_OUT uint32              *nof_tbls
  )
{
  uint32
    res = SOC_SAND_OK;
  SOC_PB_TBLS
    *tables;
  SOC_PB_PP_TBLS
    *pp_tables;
  uint32
    counter = 0;
  uint32
    tbl_ndx;
  uint32
    indx;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_TBLS_DUMP_TABLES_GET);

  SOC_SAND_CHECK_NULL_INPUT(indirect_print);
  SOC_SAND_CHECK_NULL_INPUT(nof_tbls);

  res = soc_pb_tbls_get(
          &tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  res = soc_pb_pp_tbls_get(
          &pp_tables
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 15, exit);

  if (block_id == SOC_PB_OLP_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Pge Mem */
    if (table_offset == tables->olp.pge_mem_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->olp.pge_mem_tbl.addr.base;
      indirect_print[counter].size   = tables->olp.pge_mem_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->olp.pge_mem_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_OLP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "OLP.pge_mem_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PB_IRE_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Nif Ctxt Map */
    if (table_offset == tables->ire.nif_ctxt_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ire.nif_ctxt_map_tbl.addr.base;
      indirect_print[counter].size   = tables->ire.nif_ctxt_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ire.nif_ctxt_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRE_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRE.nif_ctxt_map_tbl";
#endif
      counter++;
    }
    /* Nif Port To Ctxt Bit Map */
    if (table_offset == tables->ire.nif_port_to_ctxt_bit_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ire.nif_port_to_ctxt_bit_map_tbl.addr.base;
      indirect_print[counter].size   = tables->ire.nif_port_to_ctxt_bit_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ire.nif_port_to_ctxt_bit_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRE_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRE.nif_port_to_ctxt_bit_map_tbl";
#endif
      counter++;
    }
    /* Rcy Ctxt Map */
    if (table_offset == tables->ire.rcy_ctxt_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ire.rcy_ctxt_map_tbl.addr.base;
      indirect_print[counter].size   = tables->ire.rcy_ctxt_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ire.rcy_ctxt_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRE_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRE.rcy_ctxt_map_tbl";
#endif
      counter++;
    }
    /* Tdm Config */
    if (table_offset == tables->ire.tdm_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ire.tdm_config_tbl.addr.base;
      indirect_print[counter].size   = tables->ire.tdm_config_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ire.tdm_config_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRE_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRE.tdm_config_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PB_IDR_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Context Mru */
    if (table_offset == tables->idr.context_mru_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->idr.context_mru_tbl.addr.base;
      indirect_print[counter].size   = tables->idr.context_mru_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.context_mru_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IDR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IDR.context_mru_tbl";
#endif
      counter++;
    }
    /* Ethernet Meter Profiles */
    if (table_offset == tables->idr.ethernet_meter_profiles_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->idr.ethernet_meter_profiles_tbl.addr.base;
      indirect_print[counter].size   = tables->idr.ethernet_meter_profiles_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.ethernet_meter_profiles_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IDR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IDR.ethernet_meter_profiles_tbl";
#endif
      counter++;
    }
    /* Ethernet Meter Config */
    if (table_offset == tables->idr.ethernet_meter_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->idr.ethernet_meter_config_tbl.addr.base;
      indirect_print[counter].size   = tables->idr.ethernet_meter_config_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.ethernet_meter_config_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IDR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IDR.ethernet_meter_config_tbl";
#endif
      counter++;
    }
    /* Global Meter Profiles */
    if (table_offset == tables->idr.global_meter_profiles_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->idr.global_meter_profiles_tbl.addr.base;
      indirect_print[counter].size   = tables->idr.global_meter_profiles_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.global_meter_profiles_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IDR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IDR.global_meter_profiles_tbl";
#endif
      counter++;
    }
    /* Global Meter Status */
    if (table_offset == tables->idr.global_meter_status_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->idr.global_meter_status_tbl.addr.base;
      indirect_print[counter].size   = tables->idr.global_meter_status_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.global_meter_status_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IDR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IDR.global_meter_status_tbl";
#endif
      counter++;
    }

    /* check the table_dump mode and get the values accordingly */
    if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL )
    {
        if (table_offset == tables->idr.pcb_link_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->idr.pcb_link_tbl.addr.base;
          indirect_print[counter].size   = tables->idr.pcb_link_tbl.addr.size;
          indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.pcb_link_tbl.addr.width_bits);
          indirect_print[counter].mod_id = SOC_PB_IDR_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name = "IDR.pcb_link_tbl";
    #endif
          counter++;
        }
      

        for (indx = 0 ; indx < 6 ; indx ++)
        {
           if (table_offset == tables->idr.dbuff_pointer_cache[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
           {
              indirect_print[counter].base   = tables->idr.dbuff_pointer_cache[indx].addr.base;
              indirect_print[counter].size   = tables->idr.dbuff_pointer_cache[indx].addr.size;
              indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->idr.dbuff_pointer_cache[indx].addr.width_bits);
              indirect_print[counter].mod_id = SOC_PB_IDR_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
           indirect_print[counter].name = "IDR.dbuff_pointer_cache";
    #endif
              counter++;
          }
        }
  
   }
  }
  if (block_id == SOC_PB_IRR_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Is Ingress Replication DB */
    if (table_offset == tables->irr.is_ingress_replication_db_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.is_ingress_replication_db_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.is_ingress_replication_db_tbl.addr.size;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.is_ingress_replication_db_tbl.addr.width_bits);
      indirect_print[counter].mod_id = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.is_ingress_replication_db_tbl";
#endif
      counter++;
    }
    /* Ingress Replication Multicast DB */
    if (table_offset == tables->irr.ingress_replication_multicast_db_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.ingress_replication_multicast_db_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.ingress_replication_multicast_db_tbl.addr.size;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.ingress_replication_multicast_db_tbl.addr.width_bits);
      indirect_print[counter].mod_id = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "IRR.ingress_replication_multicast_db_tbl";
#endif
      counter++;
    }
    /* Snoop Mirror Table0 */
    if (table_offset == tables->irr.snoop_mirror_table0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.snoop_mirror_table0_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.snoop_mirror_table0_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.snoop_mirror_table0_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.snoop_mirror_table0_tbl";
#endif
      counter++;
    }
    /* Snoop Mirror Table1 */
    if (table_offset == tables->irr.snoop_mirror_table1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.snoop_mirror_table1_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.snoop_mirror_table1_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.snoop_mirror_table1_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.snoop_mirror_table1_tbl";
#endif
      counter++;
    }
    /* Free Pcb Memory */
    if (table_offset == tables->irr.free_pcb_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.free_pcb_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.free_pcb_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.free_pcb_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.free_pcb_memory_tbl";
#endif
      counter++;
    }
    /* Pcb Link Table */
    if (table_offset == tables->irr.pcb_link_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.pcb_link_table_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.pcb_link_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.pcb_link_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.pcb_link_table_tbl";
#endif
      counter++;
    }
    /* Is Free Pcb Memory */
    if (table_offset == tables->irr.is_free_pcb_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.is_free_pcb_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.is_free_pcb_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.is_free_pcb_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.is_free_pcb_memory_tbl";
#endif
      counter++;
    }
    /* Is Pcb Link Table */
    if (table_offset == tables->irr.is_pcb_link_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.is_pcb_link_table_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.is_pcb_link_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.is_pcb_link_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.is_pcb_link_table_tbl";
#endif
      counter++;
    }
    /* Rpf Memory */
    if (table_offset == tables->irr.rpf_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.rpf_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.rpf_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.rpf_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.rpf_memory_tbl";
#endif
      counter++;
    }
    /* Mcr Memory */
    if (table_offset == tables->irr.mcr_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.mcr_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.mcr_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.mcr_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.mcr_memory_tbl";
#endif
      counter++;
    }
    /* Isf Memory */
    if (table_offset == tables->irr.isf_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.isf_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.isf_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.isf_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.isf_memory_tbl";
#endif
      counter++;
    }
    /* Destination Table */
    if (table_offset == tables->irr.destination_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.destination_table_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.destination_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.destination_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.destination_table_tbl";
#endif
      counter++;
    }
    /* Lag To Lag Range */
    if (table_offset == tables->irr.lag_to_lag_range_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.lag_to_lag_range_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.lag_to_lag_range_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.lag_to_lag_range_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.lag_to_lag_range_tbl";
#endif
      counter++;
    }
    /* Lag Mapping */
    if (table_offset == tables->irr.lag_mapping_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.lag_mapping_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.lag_mapping_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.lag_mapping_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.lag_mapping_tbl";
#endif
      counter++;
    }
    /* Lag Next Member */
    if (table_offset == tables->irr.lag_next_member_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.lag_next_member_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.lag_next_member_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.lag_next_member_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.lag_next_member_tbl";
#endif
      counter++;
    }
    /* Smooth Division */
    if (table_offset == tables->irr.smooth_division_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.smooth_division_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.smooth_division_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.smooth_division_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.smooth_division_tbl";
#endif
      counter++;
    }
    /* Traffic Class Mapping */
    if (table_offset == tables->irr.traffic_class_mapping_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->irr.traffic_class_mapping_tbl.addr.base;
      indirect_print[counter].size   = tables->irr.traffic_class_mapping_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->irr.traffic_class_mapping_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IRR_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IRR.traffic_class_mapping_tbl";
#endif
      counter++;
    }
 
  }
  if (block_id == SOC_PB_IHP_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Port Mine Table Lag Port */
    if (table_offset == tables->ihp.port_mine_table_lag_port_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihp.port_mine_table_lag_port_tbl.addr.base;
      indirect_print[counter].size   = tables->ihp.port_mine_table_lag_port_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.port_mine_table_lag_port_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHP.port_mine_table_lag_port_tbl";
#endif
      counter++;
    }
    /* Tm Port Pp Port Config */
    if (table_offset == tables->ihp.tm_port_pp_port_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihp.tm_port_pp_port_config_tbl.addr.base;
      indirect_print[counter].size   = tables->ihp.tm_port_pp_port_config_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.tm_port_pp_port_config_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHP.tm_port_pp_port_config_tbl";
#endif
      counter++;
    }
    /* Tm Port Sys Port Config */
    if (table_offset == tables->ihp.tm_port_sys_port_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihp.tm_port_sys_port_config_tbl.addr.base;
      indirect_print[counter].size   = tables->ihp.tm_port_sys_port_config_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.tm_port_sys_port_config_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHP.tm_port_sys_port_config_tbl";
#endif
      counter++;
    }
    /* Pp Port Values */
    if (table_offset == tables->ihp.pp_port_values_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihp.pp_port_values_tbl.addr.base;
      indirect_print[counter].size   = tables->ihp.pp_port_values_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.pp_port_values_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHP.pp_port_values_tbl";
#endif
      counter++;
    }

 /* check here the dump mode and print the values accordingly */
    if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL )
    {
         
        if (table_offset == tables->ihp.isem_keyt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_keyt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_keyt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_keyt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_keyt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_keyt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_keyt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_keyt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_keyt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_keyt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_keyt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_keyt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_keyt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_keyt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_keyt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_pldt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_pldt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_pldt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_pldt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_pldt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_pldt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_pldt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_pldt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_pldt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_pldt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_pldt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_pldt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_pldt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_pldt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_pldt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_mgmt_mem_a_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_mgmt_mem_a_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_mgmt_mem_a_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_mgmt_mem_a_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_mgmt_mem_a_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_mgmt_mem_b_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_mgmt_mem_b_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_mgmt_mem_b_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_mgmt_mem_b_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_mgmt_mem_b_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_em_keyt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_em_keyt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_em_keyt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_em_keyt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_em_keyt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_em_keyt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_em_keyt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_em_keyt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_em_keyt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.isem_em_keyt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.isem_em_keyt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.isem_em_keyt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.isem_em_keyt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.isem_em_keyt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHB.isem_em_keyt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->ihp.em_pldt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihp.em_pldt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->ihp.em_pldt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihp.em_pldt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHP.em_pldt_h0_tbl";
    #endif
          counter++;
        }

	    /* Port Mine Table Physical Port */
	    if (table_offset == pp_tables->ihp.port_mine_table_physical_port_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.port_mine_table_physical_port_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.port_mine_table_physical_port_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.port_mine_table_physical_port_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.port_mine_table_physical_port_tbl";
	#endif
	      counter++;
	    }
	    /* Isem Management Request */
	    if (table_offset == pp_tables->ihp.isem_management_request_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.isem_management_request_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.isem_management_request_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.isem_management_request_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.isem_management_request_tbl";
	#endif
	      counter++;
	    }
	    /* Pinfo Llr */
	    if (table_offset == pp_tables->ihp.pinfo_llr_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pinfo_llr_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pinfo_llr_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pinfo_llr_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pinfo_llr_tbl";
	#endif
	      counter++;
	    }
	    /* Llr Llvp */
	    if (table_offset == pp_tables->ihp.llr_llvp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.llr_llvp_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.llr_llvp_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.llr_llvp_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.llr_llvp_tbl";
	#endif
	      counter++;
	    }
	    /* Ll Mirror Profile */
	    if (table_offset == pp_tables->ihp.ll_mirror_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.ll_mirror_profile_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.ll_mirror_profile_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.ll_mirror_profile_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.ll_mirror_profile_tbl";
	#endif
	      counter++;
	    }
	    /* Subnet Classify */
	    if (table_offset == pp_tables->ihp.subnet_classify_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.subnet_classify_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.subnet_classify_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.subnet_classify_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.subnet_classify_tbl";
	#endif
	      counter++;
	    }
	    /* Port Protocol */
	    if (table_offset == pp_tables->ihp.port_protocol_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.port_protocol_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.port_protocol_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.port_protocol_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.port_protocol_tbl";
	#endif
	      counter++;
	    }
	    /* Tos 2 Cos */
	    if (table_offset == pp_tables->ihp.tos_2_cos_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.tos_2_cos_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.tos_2_cos_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.tos_2_cos_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.tos_2_cos_tbl";
	#endif
	      counter++;
	    }
	    /* Reserved Mc */
	    if (table_offset == pp_tables->ihp.reserved_mc_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.reserved_mc_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.reserved_mc_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.reserved_mc_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.reserved_mc_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format1 */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format1_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format1_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format1_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format1_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format1 Asd Tp2p */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format1_asd_tp2p_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format1_asd_tp2p_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format1 Asd Pbp */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format1_asd_pbp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format1_asd_pbp_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format1_asd_pbp_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format1_asd_pbp_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format1_asd_pbp_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format1 Asd Ilm */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format1_asd_ilm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format1_asd_ilm_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format1_asd_ilm_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format1_asd_ilm_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format1_asd_ilm_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format1 Asd Sp2p */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format1_asd_sp2p_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format1_asd_sp2p_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format1 Asd Ethernet */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format1_asd_ethernet_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format1_asd_ethernet_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format2 */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format2_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format2_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format2_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format2_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format2_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format2 Trill */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format2_trill_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format2_trill_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format2_trill_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format2_trill_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format2_trill_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Pldt Format2 Auth */
	    if (table_offset == pp_tables->ihp.large_em_pldt_format2_auth_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_pldt_format2_auth_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_pldt_format2_auth_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_pldt_format2_auth_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_pldt_format2_auth_tbl";
	#endif
	      counter++;
	    }
	    /* Flush Db */
	    if (table_offset == pp_tables->ihp.flush_db_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.flush_db_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.flush_db_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.flush_db_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.flush_db_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Fid Counter Db */
	    if (table_offset == pp_tables->ihp.large_em_fid_counter_db_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_fid_counter_db_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_fid_counter_db_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_fid_counter_db_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_fid_counter_db_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Fid Counter Profile Db */
	    if (table_offset == pp_tables->ihp.large_em_fid_counter_profile_db_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_fid_counter_profile_db_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_fid_counter_profile_db_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_fid_counter_profile_db_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_fid_counter_profile_db_tbl";
	#endif
	      counter++;
	    }
	    /* Large Em Aging Configuration Table */
	    if (table_offset == pp_tables->ihp.large_em_aging_configuration_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.large_em_aging_configuration_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.large_em_aging_configuration_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.large_em_aging_configuration_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.large_em_aging_configuration_table_tbl";
	#endif
	      counter++;
	    }
	    /* Tm Port Pp Context Config */
	    if (table_offset == pp_tables->ihp.tm_port_pp_context_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.tm_port_pp_context_config_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.tm_port_pp_context_config_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.tm_port_pp_context_config_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.tm_port_pp_context_config_tbl";
	#endif
	      counter++;
	    }
	    /* Pp Port Fem Bit Select Table */
	    if (table_offset == pp_tables->ihp.pp_port_fem_bit_select_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pp_port_fem_bit_select_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pp_port_fem_bit_select_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pp_port_fem_bit_select_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pp_port_fem_bit_select_table_tbl";
	#endif
	      counter++;
	    }
	    /* Pp Port Fem Map Index Table */
	    if (table_offset == pp_tables->ihp.pp_port_fem_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pp_port_fem_map_index_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pp_port_fem_map_index_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pp_port_fem_map_index_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pp_port_fem_map_index_table_tbl";
	#endif
	      counter++;
	    }
	    /* Pp Port Fem Field Select Map */
	    if (table_offset == pp_tables->ihp.pp_port_fem_field_select_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pp_port_fem_field_select_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pp_port_fem_field_select_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pp_port_fem_field_select_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pp_port_fem_field_select_map_tbl";
	#endif
	      counter++;
	    }
	    /* Src System Port Fem Bit Select Table */
	    if (table_offset == pp_tables->ihp.src_system_port_fem_bit_select_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.src_system_port_fem_bit_select_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.src_system_port_fem_bit_select_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.src_system_port_fem_bit_select_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.src_system_port_fem_bit_select_table_tbl";
	#endif
	      counter++;
	    }
	    /* Src System Port Fem Map Index Table */
	    if (table_offset == pp_tables->ihp.src_system_port_fem_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.src_system_port_fem_map_index_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.src_system_port_fem_map_index_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.src_system_port_fem_map_index_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.src_system_port_fem_map_index_table_tbl";
	#endif
	      counter++;
	    }
	    /* Src System Port Fem Field Select Map */
	    if (table_offset == pp_tables->ihp.src_system_port_fem_field_select_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.src_system_port_fem_field_select_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.src_system_port_fem_field_select_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.src_system_port_fem_field_select_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.src_system_port_fem_field_select_map_tbl";
	#endif
	      counter++;
	    }
	    /* Pp Context Fem Bit Select Table */
	    if (table_offset == pp_tables->ihp.pp_context_fem_bit_select_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pp_context_fem_bit_select_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pp_context_fem_bit_select_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pp_context_fem_bit_select_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pp_context_fem_bit_select_table_tbl";
	#endif
	      counter++;
	    }
	    /* Pp Context Fem Map Index Table */
	    if (table_offset == pp_tables->ihp.pp_context_fem_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pp_context_fem_map_index_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pp_context_fem_map_index_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pp_context_fem_map_index_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pp_context_fem_map_index_table_tbl";
	#endif
	      counter++;
	    }
	    /* Pp Context Fem Field Select Map */
	    if (table_offset == pp_tables->ihp.pp_context_fem_field_select_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pp_context_fem_field_select_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pp_context_fem_field_select_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pp_context_fem_field_select_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pp_context_fem_field_select_map_tbl";
	#endif
	      counter++;
	    }
	    /* Parser Program1 */
	    if (table_offset == pp_tables->ihp.parser_program1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.parser_program1_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.parser_program1_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.parser_program1_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.parser_program1_tbl";
	#endif
	      counter++;
	    }
	    /* Parser Program2 */
	    if (table_offset == pp_tables->ihp.parser_program2_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.parser_program2_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.parser_program2_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.parser_program2_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.parser_program2_tbl";
	#endif
	      counter++;
	    }
	    /* Packet Format Table */
	    if (table_offset == pp_tables->ihp.packet_format_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.packet_format_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.packet_format_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.packet_format_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.packet_format_table_tbl";
	#endif
	      counter++;
	    }
	    /* Parser Custom Macro Parameters */
	    if (table_offset == pp_tables->ihp.parser_custom_macro_parameters_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.parser_custom_macro_parameters_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.parser_custom_macro_parameters_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.parser_custom_macro_parameters_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.parser_custom_macro_parameters_tbl";
	#endif
	      counter++;
	    }
	    /* Parser Eth Protocols */
	    if (table_offset == pp_tables->ihp.parser_eth_protocols_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.parser_eth_protocols_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.parser_eth_protocols_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.parser_eth_protocols_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.parser_eth_protocols_tbl";
	#endif
	      counter++;
	    }
	    /* Parser Ip Protocols */
	    if (table_offset == pp_tables->ihp.parser_ip_protocols_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.parser_ip_protocols_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.parser_ip_protocols_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.parser_ip_protocols_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.parser_ip_protocols_tbl";
	#endif
	      counter++;
	    }
	    /* Parser Custom Macro Protocols */
	    if (table_offset == pp_tables->ihp.parser_custom_macro_protocols_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.parser_custom_macro_protocols_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.parser_custom_macro_protocols_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.parser_custom_macro_protocols_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.parser_custom_macro_protocols_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi Isid */
	    if (table_offset == pp_tables->ihp.vsi_isid_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vsi_isid_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vsi_isid_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vsi_isid_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vsi_isid_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi My Mac */
	    if (table_offset == pp_tables->ihp.vsi_my_mac_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vsi_my_mac_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vsi_my_mac_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vsi_my_mac_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vsi_my_mac_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi Topology Id */
	    if (table_offset == pp_tables->ihp.vsi_topology_id_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vsi_topology_id_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vsi_topology_id_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vsi_topology_id_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vsi_topology_id_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi Fid Class */
	    if (table_offset == pp_tables->ihp.vsi_fid_class_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vsi_fid_class_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vsi_fid_class_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vsi_fid_class_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vsi_fid_class_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi Da Not Found */
	    if (table_offset == pp_tables->ihp.vsi_da_not_found_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vsi_da_not_found_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vsi_da_not_found_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vsi_da_not_found_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vsi_da_not_found_tbl";
	#endif
	      counter++;
	    }
	    /* Bvd Topology Id */
	    if (table_offset == pp_tables->ihp.bvd_topology_id_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.bvd_topology_id_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.bvd_topology_id_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.bvd_topology_id_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.bvd_topology_id_tbl";
	#endif
	      counter++;
	    }
	    /* Bvd Fid Class */
	    if (table_offset == pp_tables->ihp.bvd_fid_class_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.bvd_fid_class_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.bvd_fid_class_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.bvd_fid_class_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.bvd_fid_class_tbl";
	#endif
	      counter++;
	    }
	    /* Bvd Da Not Found */
	    if (table_offset == pp_tables->ihp.bvd_da_not_found_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.bvd_da_not_found_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.bvd_da_not_found_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.bvd_da_not_found_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.bvd_da_not_found_tbl";
	#endif
	      counter++;
	    }
	    /* Fid Class 2 Fid */
	    if (table_offset == pp_tables->ihp.fid_class_2_fid_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.fid_class_2_fid_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.fid_class_2_fid_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.fid_class_2_fid_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.fid_class_2_fid_tbl";
	#endif
	      counter++;
	    }
	    /* Vlan Range Compression Table */
	    if (table_offset == pp_tables->ihp.vlan_range_compression_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vlan_range_compression_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vlan_range_compression_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vlan_range_compression_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vlan_range_compression_table_tbl";
	#endif
	      counter++;
	    }
	    /* Vtt In Pp Port Vlan Config */
	    if (table_offset == pp_tables->ihp.vtt_in_pp_port_vlan_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vtt_in_pp_port_vlan_config_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vtt_in_pp_port_vlan_config_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vtt_in_pp_port_vlan_config_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vtt_in_pp_port_vlan_config_tbl";
	#endif
	      counter++;
	    }
	    /* Designated Vlan Table */
	    if (table_offset == pp_tables->ihp.designated_vlan_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.designated_vlan_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.designated_vlan_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.designated_vlan_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.designated_vlan_table_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi Port Membership */
	    if (table_offset == pp_tables->ihp.vsi_port_membership_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vsi_port_membership_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vsi_port_membership_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vsi_port_membership_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vsi_port_membership_tbl";
	#endif
	      counter++;
	    }
	    /* Vtt In Pp Port Config */
	    if (table_offset == pp_tables->ihp.vtt_in_pp_port_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vtt_in_pp_port_config_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vtt_in_pp_port_config_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vtt_in_pp_port_config_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vtt_in_pp_port_config_tbl";
	#endif
	      counter++;
	    }
	    /* Sem Result Table */
	    if (table_offset == pp_tables->ihp.sem_result_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.sem_result_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.sem_result_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.sem_result_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.sem_result_table_tbl";
	#endif
	      counter++;
	    }
	    /* Sem Opcode Ip Offsets */
	    if (table_offset == pp_tables->ihp.sem_opcode_ip_offsets_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.sem_opcode_ip_offsets_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.sem_opcode_ip_offsets_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.sem_opcode_ip_offsets_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.sem_opcode_ip_offsets_tbl";
	#endif
	      counter++;
	    }
	    /* Sem Opcode Tc Dp Offsets */
	    if (table_offset == pp_tables->ihp.sem_opcode_tc_dp_offsets_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.sem_opcode_tc_dp_offsets_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.sem_opcode_tc_dp_offsets_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.sem_opcode_tc_dp_offsets_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.sem_opcode_tc_dp_offsets_tbl";
	#endif
	      counter++;
	    }
	    /* Sem Opcode Pcp Dei Offsets */
	    if (table_offset == pp_tables->ihp.sem_opcode_pcp_dei_offsets_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.sem_opcode_pcp_dei_offsets_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.sem_opcode_pcp_dei_offsets_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.sem_opcode_pcp_dei_offsets_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.sem_opcode_pcp_dei_offsets_tbl";
	#endif
	      counter++;
	    }
	    /* Stp Table */
	    if (table_offset == pp_tables->ihp.stp_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.stp_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.stp_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.stp_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.stp_table_tbl";
	#endif
	      counter++;
	    }
	    /* Vrid My Mac Map */
	    if (table_offset == pp_tables->ihp.vrid_my_mac_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vrid_my_mac_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vrid_my_mac_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vrid_my_mac_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vrid_my_mac_map_tbl";
	#endif
	      counter++;
	    }
	    /* Mpls Label Range Encountered */
	    if (table_offset == pp_tables->ihp.mpls_label_range_encountered_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.mpls_label_range_encountered_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.mpls_label_range_encountered_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.mpls_label_range_encountered_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.mpls_label_range_encountered_tbl";
	#endif
	      counter++;
	    }
	    /* Mpls Tunnel Termination Valid */
	    if (table_offset == pp_tables->ihp.mpls_tunnel_termination_valid_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.mpls_tunnel_termination_valid_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.mpls_tunnel_termination_valid_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.mpls_tunnel_termination_valid_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.mpls_tunnel_termination_valid_tbl";
	#endif
	      counter++;
	    }
	    /* Ip Over Mpls Exp Mapping */
	    if (table_offset == pp_tables->ihp.ip_over_mpls_exp_mapping_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.ip_over_mpls_exp_mapping_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.ip_over_mpls_exp_mapping_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.ip_over_mpls_exp_mapping_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.ip_over_mpls_exp_mapping_tbl";
	#endif
	      counter++;
	    }
	    /* Vtt Llvp */
	    if (table_offset == pp_tables->ihp.vtt_llvp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vtt_llvp_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vtt_llvp_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vtt_llvp_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vtt_llvp_tbl";
	#endif
	      counter++;
	    }
	    /* Llvp Prog Sel */
	    if (table_offset == pp_tables->ihp.llvp_prog_sel_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.llvp_prog_sel_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.llvp_prog_sel_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.llvp_prog_sel_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.llvp_prog_sel_tbl";
	#endif
	      counter++;
	    }
	    /* Vtt1st Key Construction0 */
	    if (table_offset == pp_tables->ihp.vtt1st_key_construction0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vtt1st_key_construction0_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vtt1st_key_construction0_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vtt1st_key_construction0_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vtt1st_key_construction0_tbl";
	#endif
	      counter++;
	    }
	    /* Vtt1st Key Construction1 */
	    if (table_offset == pp_tables->ihp.vtt1st_key_construction1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vtt1st_key_construction1_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vtt1st_key_construction1_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vtt1st_key_construction1_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vtt1st_key_construction1_tbl";
	#endif
	      counter++;
	    }
	    /* Vtt2nd Key Construction */
	    if (table_offset == pp_tables->ihp.vtt2nd_key_construction_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vtt2nd_key_construction_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vtt2nd_key_construction_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vtt2nd_key_construction_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vtt2nd_key_construction_tbl";
	#endif
	      counter++;
	    }
	    /* Ingress Vlan Edit Command Table */
	    if (table_offset == pp_tables->ihp.ingress_vlan_edit_command_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.ingress_vlan_edit_command_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.ingress_vlan_edit_command_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.ingress_vlan_edit_command_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.ingress_vlan_edit_command_table_tbl";
	#endif
	      counter++;
	    }
	    /* Vlan Edit Pcp Dei Map */
	    if (table_offset == pp_tables->ihp.vlan_edit_pcp_dei_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.vlan_edit_pcp_dei_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.vlan_edit_pcp_dei_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.vlan_edit_pcp_dei_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.vlan_edit_pcp_dei_map_tbl";
	#endif
	      counter++;
	    }
	    /* Pbb Cfm Table */
	    if (table_offset == pp_tables->ihp.pbb_cfm_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.pbb_cfm_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.pbb_cfm_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.pbb_cfm_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.pbb_cfm_table_tbl";
	#endif
	      counter++;
	    }
	    /* Sem Result Accessed */
	    if (table_offset == pp_tables->ihp.sem_result_accessed_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.sem_result_accessed_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.sem_result_accessed_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.sem_result_accessed_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.sem_result_accessed_tbl";
	#endif
	      counter++;
	    }
	    /* In Rif Config Table */
	    if (table_offset == pp_tables->ihp.in_rif_config_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.in_rif_config_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.in_rif_config_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.in_rif_config_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.in_rif_config_table_tbl";
	#endif
	      counter++;
	    }
	    /* Tc Dp Map Table */
	    if (table_offset == pp_tables->ihp.tc_dp_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->ihp.tc_dp_map_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->ihp.tc_dp_map_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihp.tc_dp_map_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_IHP_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "IHP.tc_dp_map_table_tbl";
	#endif
	      counter++;
	    }
 }

 }
 if (block_id == SOC_PB_IHB_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Pinfo Lbp */
    if (table_offset == tables->ihb.pinfo_lbp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.pinfo_lbp_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.pinfo_lbp_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.pinfo_lbp_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.pinfo_lbp_tbl";
#endif
      counter++;
    }
    /* Pinfo PMF */
    if (table_offset == tables->ihb.pinfo_pmf_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.pinfo_pmf_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.pinfo_pmf_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.pinfo_pmf_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.pinfo_pmf_tbl";
#endif
      counter++;
    }
    /* Packet Format Code Profile */
    if (table_offset == tables->ihb.packet_format_code_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.packet_format_code_profile_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.packet_format_code_profile_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.packet_format_code_profile_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.packet_format_code_profile_tbl";
#endif
      counter++;
    }
    /* Src Dest Port For L3 ACL Key */
    if (table_offset == tables->ihb.src_dest_port_for_l3_acl_key_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.src_dest_port_for_l3_acl_key_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.src_dest_port_for_l3_acl_key_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.src_dest_port_for_l3_acl_key_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.src_dest_port_for_l3_acl_key_tbl";
#endif
      counter++;
    }
    /* Direct 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.direct_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.direct_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* Direct 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.direct_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.direct_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem0_4b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem1_4b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem2_14b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 1st Pass Offset Table */
    if (table_offset == tables->ihb.fem2_14b_1st_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_1st_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_1st_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_1st_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_1st_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem3_14b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 1st Pass Offset Table */
    if (table_offset == tables->ihb.fem3_14b_1st_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_1st_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_1st_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_1st_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_1st_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem4_14b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 1st Pass Offset Table */
    if (table_offset == tables->ihb.fem4_14b_1st_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_1st_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_1st_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_1st_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_1st_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem5_17b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 1st Pass Offset Table */
    if (table_offset == tables->ihb.fem5_17b_1st_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_1st_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_1st_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_1st_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_1st_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem6_17b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 1st Pass Offset Table */
    if (table_offset == tables->ihb.fem6_17b_1st_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_1st_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_1st_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_1st_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_1st_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 1st Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 1st Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_1st_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_1st_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 1st Pass Map Index Table */
    if (table_offset == tables->ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_1st_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_1st_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 1st Pass Map Table */
    if (table_offset == tables->ihb.fem7_17b_1st_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_1st_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_1st_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_1st_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_1st_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 1st Pass Offset Table */
    if (table_offset == tables->ihb.fem7_17b_1st_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_1st_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_1st_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_1st_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_1st_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM0 4b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem0_4b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem0_4b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem0_4b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem0_4b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem0_4b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM1 4b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem1_4b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem1_4b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem1_4b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem1_4b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem1_4b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem2_14b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM2 14b 2nd Pass Offset Table */
    if (table_offset == tables->ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem2_14b_2nd_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem2_14b_2nd_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem3_14b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM3 14b 2nd Pass Offset Table */
    if (table_offset == tables->ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem3_14b_2nd_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem3_14b_2nd_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem4_14b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM4 14b 2nd Pass Offset Table */
    if (table_offset == tables->ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem4_14b_2nd_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem4_14b_2nd_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem5_17b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM5 17b 2nd Pass Offset Table */
    if (table_offset == tables->ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem5_17b_2nd_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem5_17b_2nd_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem6_17b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM6 17b 2nd Pass Offset Table */
    if (table_offset == tables->ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem6_17b_2nd_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem6_17b_2nd_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 2nd Pass Key Profile Resolved Data */
    if (table_offset == tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 2nd Pass Program Resolved Data */
    if (table_offset == tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_2nd_pass_program_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_2nd_pass_program_resolved_data_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 2nd Pass Map Index Table */
    if (table_offset == tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_2nd_pass_map_index_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_2nd_pass_map_index_table_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 2nd Pass Map Table */
    if (table_offset == tables->ihb.fem7_17b_2nd_pass_map_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_2nd_pass_map_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_2nd_pass_map_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_2nd_pass_map_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_2nd_pass_map_table_tbl";
#endif
      counter++;
    }
    /* FEM7 17b 2nd Pass Offset Table */
    if (table_offset == tables->ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.fem7_17b_2nd_pass_offset_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fem7_17b_2nd_pass_offset_table_tbl";
#endif
      counter++;
    }
    /* Pinfo Fer */
    if (table_offset == pp_tables->ihb.pinfo_fer_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.pinfo_fer_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.pinfo_fer_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.pinfo_fer_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.pinfo_fer_tbl";
#endif
      counter++;
    }
    /* Ecmp Group Size */
    if (table_offset == pp_tables->ihb.ecmp_group_size_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.ecmp_group_size_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.ecmp_group_size_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.ecmp_group_size_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.ecmp_group_size_tbl";
#endif
      counter++;
    }
    /* Lb Pfc Profile */
    if (table_offset == pp_tables->ihb.lb_pfc_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lb_pfc_profile_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lb_pfc_profile_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lb_pfc_profile_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lb_pfc_profile_tbl";
#endif
      counter++;
    }
    /* Lb Vector Program Map */
    if (table_offset == pp_tables->ihb.lb_vector_program_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lb_vector_program_map_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lb_vector_program_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lb_vector_program_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lb_vector_program_map_tbl";
#endif
      counter++;
    }
    /* Fec Super Entry */
    if (table_offset == pp_tables->ihb.fec_super_entry_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_super_entry_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_super_entry_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_super_entry_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_super_entry_tbl";
#endif
      counter++;
    }
    /* Fec Entry[0..1] */
    if (table_offset == pp_tables->ihb.fec_entry_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_entry_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_entry_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_entry_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_entry_tbl";
#endif
      counter++;
    }
    /* Fec Entry General */
    if (table_offset == pp_tables->ihb.fec_entry_general_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_entry_general_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_entry_general_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_entry_general_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_entry_general_tbl";
#endif
      counter++;
    }
    /* Fec Entry Eth Or Trill */
    if (table_offset == pp_tables->ihb.fec_entry_eth_or_trill_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_entry_eth_or_trill_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_entry_eth_or_trill_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_entry_eth_or_trill_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_entry_eth_or_trill_tbl";
#endif
      counter++;
    }
    /* Fec Entry Ip */
    if (table_offset == pp_tables->ihb.fec_entry_ip_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_entry_ip_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_entry_ip_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_entry_ip_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_entry_ip_tbl";
#endif
      counter++;
    }
    /* Fec Entry Default */
    if (table_offset == pp_tables->ihb.fec_entry_default_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_entry_default_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_entry_default_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_entry_default_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_entry_default_tbl";
#endif
      counter++;
    }
    /* Fec Entry Accessed[0..1] */
    if (table_offset == pp_tables->ihb.fec_entry_accessed_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fec_entry_accessed_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fec_entry_accessed_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fec_entry_accessed_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fec_entry_accessed_tbl";
#endif
      counter++;
    }
    /* Path Select */
    if (table_offset == pp_tables->ihb.path_select_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.path_select_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.path_select_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.path_select_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.path_select_tbl";
#endif
      counter++;
    }
    /* Destination Status */
    if (table_offset == pp_tables->ihb.destination_status_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.destination_status_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.destination_status_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.destination_status_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.destination_status_tbl";
#endif
      counter++;
    }
    /* Fwd Act Profile */
    if (table_offset == pp_tables->ihb.fwd_act_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.fwd_act_profile_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.fwd_act_profile_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.fwd_act_profile_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.fwd_act_profile_tbl";
#endif
      counter++;
    }
    /* Snoop Action */
    if (table_offset == pp_tables->ihb.snoop_action_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.snoop_action_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.snoop_action_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.snoop_action_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.snoop_action_tbl";
#endif
      counter++;
    }
    /* Pinfo Flp */
    if (table_offset == pp_tables->ihb.pinfo_flp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.pinfo_flp_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.pinfo_flp_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.pinfo_flp_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.pinfo_flp_tbl";
#endif
      counter++;
    }
    /* Program Translation Map */
    if (table_offset == pp_tables->ihb.program_translation_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_translation_map_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_translation_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_translation_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_translation_map_tbl";
#endif
      counter++;
    }
    /* Flp Key Program Map */
    if (table_offset == pp_tables->ihb.flp_key_program_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.flp_key_program_map_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.flp_key_program_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.flp_key_program_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.flp_key_program_map_tbl";
#endif
      counter++;
    }
    /* Unknown Da Action Profiles */
    if (table_offset == pp_tables->ihb.unknown_da_action_profiles_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.unknown_da_action_profiles_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.unknown_da_action_profiles_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.unknown_da_action_profiles_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.unknown_da_action_profiles_tbl";
#endif
      counter++;
    }
    /* Vrf Config */
    if (table_offset == pp_tables->ihb.vrf_config_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.vrf_config_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.vrf_config_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.vrf_config_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.vrf_config_tbl";
#endif
      counter++;
    }
    /* Snp Act Profile */
    if (table_offset == pp_tables->ihb.snp_act_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.snp_act_profile_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.snp_act_profile_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.snp_act_profile_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.snp_act_profile_tbl";
#endif
      counter++;
    }
    /* Mrr Act Profile */
    if (table_offset == pp_tables->ihb.mrr_act_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.mrr_act_profile_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.mrr_act_profile_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.mrr_act_profile_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.mrr_act_profile_tbl";
#endif
      counter++;
    }

    /* Lpm1 */
    if (table_offset == pp_tables->ihb.lpm_tbl[0].addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lpm_tbl[0].addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lpm_tbl[0].addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lpm_tbl[0].addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lpm1_tbl";
#endif
      counter++;
    }
    /* Lpm2 */
    if (table_offset == pp_tables->ihb.lpm_tbl[1].addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lpm_tbl[1].addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lpm_tbl[1].addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lpm_tbl[1].addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lpm2_tbl";
#endif
      counter++;
    }
    /* Lpm3 */
    if (table_offset == pp_tables->ihb.lpm_tbl[2].addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lpm_tbl[2].addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lpm_tbl[2].addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lpm_tbl[2].addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lpm3_tbl";
#endif
      counter++;
    }
    /* Lpm4 */
    if (table_offset == pp_tables->ihb.lpm_tbl[3].addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lpm_tbl[3].addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lpm_tbl[3].addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lpm_tbl[3].addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lpm4_tbl";
#endif
      counter++;
    }
    /* Lpm5 */
    if (table_offset == pp_tables->ihb.lpm_tbl[4].addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lpm_tbl[4].addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lpm_tbl[4].addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lpm_tbl[4].addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lpm5_tbl";
#endif
      counter++;
    }
    /* Lpm6 */
    if (table_offset == pp_tables->ihb.lpm_tbl[5].addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.lpm_tbl[5].addr.base;
      indirect_print[counter].size   = pp_tables->ihb.lpm_tbl[5].addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.lpm_tbl[5].addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.lpm6_tbl";
#endif
      counter++;
    }
    /* Pinfo PMF Key Gen Var */
    if (table_offset == pp_tables->ihb.pinfo_pmf_key_gen_var_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.pinfo_pmf_key_gen_var_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.pinfo_pmf_key_gen_var_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.pinfo_pmf_key_gen_var_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.pinfo_pmf_key_gen_var_tbl";
#endif
      counter++;
    }
    /* Program Selection Map0 */
    if (table_offset == pp_tables->ihb.program_selection_map0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map0_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map0_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map0_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map0_tbl";
#endif
      counter++;
    }
    /* Program Selection Map1 */
    if (table_offset == pp_tables->ihb.program_selection_map1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map1_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map1_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map1_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map1_tbl";
#endif
      counter++;
    }
    /* Program Selection Map2 */
    if (table_offset == pp_tables->ihb.program_selection_map2_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map2_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map2_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map2_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map2_tbl";
#endif
      counter++;
    }
    /* Program Selection Map3 */
    if (table_offset == pp_tables->ihb.program_selection_map3_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map3_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map3_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map3_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map3_tbl";
#endif
      counter++;
    }
    /* Program Selection Map4 */
    if (table_offset == pp_tables->ihb.program_selection_map4_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map4_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map4_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map4_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map4_tbl";
#endif
      counter++;
    }
    /* Program Selection Map5 */
    if (table_offset == pp_tables->ihb.program_selection_map5_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map5_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map5_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map5_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map5_tbl";
#endif
      counter++;
    }
    /* Program Selection Map6 */
    if (table_offset == pp_tables->ihb.program_selection_map6_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map6_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map6_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map6_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map6_tbl";
#endif
      counter++;
    }
    /* Program Selection Map7 */
    if (table_offset == pp_tables->ihb.program_selection_map7_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_selection_map7_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_selection_map7_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_selection_map7_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_selection_map7_tbl";
#endif
      counter++;
    }
    /* Program Vars Table */
    if (table_offset == pp_tables->ihb.program_vars_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.program_vars_table_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.program_vars_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.program_vars_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.program_vars_table_tbl";
#endif
      counter++;
    }

    for(indx = 0 ; indx < SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS; ++indx)
    {
      /* Key A Program Instruction Table[0..3] */
      if (table_offset == pp_tables->ihb.key_a_program_instruction_table_tbl[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = pp_tables->ihb.key_a_program_instruction_table_tbl[indx].addr.base;
        indirect_print[counter].size   = pp_tables->ihb.key_a_program_instruction_table_tbl[indx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.key_a_program_instruction_table_tbl[indx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "IHB.key_a_program_instruction_table_tbl";
  #endif
        counter++;
      }
    }
    for(indx = 0 ; indx < SOC_PB_PP_IHB_PROGRAM_INSTRUCTION_TABLE_NOF_TBLS; ++indx)
    {

      /* Key B Program Instruction Table[0..3] */
      if (table_offset == pp_tables->ihb.key_b_program_instruction_table_tbl[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = pp_tables->ihb.key_b_program_instruction_table_tbl[indx].addr.base;
        indirect_print[counter].size   = pp_tables->ihb.key_b_program_instruction_table_tbl[indx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.key_b_program_instruction_table_tbl[indx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "IHB.key_b_program_instruction_table_tbl";
  #endif
        counter++;
      }
    }
    /* TCAM 1st Pass Key Profile Resolved Data */
    if (table_offset == pp_tables->ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.tcam_1st_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.tcam_1st_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* TCAM 2nd Pass Key Profile Resolved Data */
    if (table_offset == pp_tables->ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.tcam_2nd_pass_key_profile_resolved_data_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.tcam_2nd_pass_key_profile_resolved_data_tbl";
#endif
      counter++;
    }
    /* Direct Action Table */
    if (table_offset == pp_tables->ihb.direct_action_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.direct_action_table_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.direct_action_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.direct_action_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.direct_action_table_tbl";
#endif
      counter++;
    }
    /* Tcam Bank */
    if (table_offset == pp_tables->ihb.tcam_bank_out_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->ihb.tcam_bank_out_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->ihb.tcam_bank_out_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.tcam_bank_out_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IHB.tcam_bank_tbl";
#endif
      counter++;
    }
    /* Tcam Action[0..3] */
    for(indx = 0 ; indx < SOC_PB_TCAM_NOF_BANKS; ++indx)
    {
      if (table_offset == pp_tables->ihb.tcam_action_tbl[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = pp_tables->ihb.tcam_action_tbl[indx].addr.base;
        indirect_print[counter].size   = pp_tables->ihb.tcam_action_tbl[indx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->ihb.tcam_action_tbl[indx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "IHB.tcam_action_tbl";
  #endif
        counter++;
      }
    }
    if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL)
    {
      for(indx = 0; indx < 10; ++indx)
      {
        /* elk_fifo_data Offset Table */
        if (table_offset == tables->ihb.elk_fifo_data[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->ihb.elk_fifo_data[indx].addr.base;
          indirect_print[counter].size   = tables->ihb.elk_fifo_data[indx].addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ihb.elk_fifo_data[indx].addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_IHB_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "IHB.tcam_action_tbl";
  #endif
          counter++;
        }

      }

    }
  }
  if (block_id == SOC_PB_IQM_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* BDB Link List */
    if (table_offset == tables->iqm.bdb_link_list_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.bdb_link_list_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.bdb_link_list_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.bdb_link_list_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.bdb_link_list_tbl";
#endif
      counter++;
    }
    /* Packet Queue Descriptor (Dynamic) */
    if (table_offset == tables->iqm.dynamic_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.dynamic_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.dynamic_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.dynamic_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.dynamic_tbl";
#endif
      counter++;
    }
    /* Packet Queue Descriptor (Static) */
    if (table_offset == tables->iqm.static_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.static_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.static_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.static_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.static_tbl";
#endif
      counter++;
    }
    /* Packet Queue Tail Pointer */
    if (table_offset == tables->iqm.packet_queue_tail_pointer_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.packet_queue_tail_pointer_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.packet_queue_tail_pointer_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.packet_queue_tail_pointer_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.packet_queue_tail_pointer_tbl";
#endif
      counter++;
    }
    /* Packet Queue Red Weight table */
    if (table_offset == tables->iqm.packet_queue_red_weight_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.packet_queue_red_weight_table_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.packet_queue_red_weight_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.packet_queue_red_weight_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.packet_queue_red_weight_table_tbl";
#endif
      counter++;
    }
    /* Credit Discount table */
    if (table_offset == tables->iqm.credit_discount_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.credit_discount_table_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.credit_discount_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.credit_discount_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.credit_discount_table_tbl";
#endif
      counter++;
    }
    /* Full User Count Memory */
    if (table_offset == tables->iqm.full_user_count_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.full_user_count_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.full_user_count_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.full_user_count_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.full_user_count_memory_tbl";
#endif
      counter++;
    }
    /* Mini-Multicast User Count Memory */
    if (table_offset == tables->iqm.mini_multicast_user_count_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.mini_multicast_user_count_memory_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.mini_multicast_user_count_memory_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.mini_multicast_user_count_memory_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.mini_multicast_user_count_memory_tbl";
#endif
      counter++;
    }
    /* Packet Queue Red parameters table */
    if (table_offset == tables->iqm.packet_queue_red_parameters_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.packet_queue_red_parameters_table_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.packet_queue_red_parameters_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.packet_queue_red_parameters_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.packet_queue_red_parameters_table_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group A */
    if (table_offset == tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_descriptor_rate_class_group_a_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_descriptor_rate_class_group_a_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group B */
    if (table_offset == tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_descriptor_rate_class_group_b_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_descriptor_rate_class_group_b_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group C */
    if (table_offset == tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_descriptor_rate_class_group_c_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_descriptor_rate_class_group_c_tbl";
#endif
      counter++;
    }
    /* VSQ Descriptor Rate Class - group D */
    if (table_offset == tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_descriptor_rate_class_group_d_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_descriptor_rate_class_group_d_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group A */
    if (table_offset == tables->iqm.vsq_qsize_memory_group_a_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_qsize_memory_group_a_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_qsize_memory_group_a_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_qsize_memory_group_a_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_qsize_memory_group_a_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group B */
    if (table_offset == tables->iqm.vsq_qsize_memory_group_b_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_qsize_memory_group_b_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_qsize_memory_group_b_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_qsize_memory_group_b_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_qsize_memory_group_b_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group C */
    if (table_offset == tables->iqm.vsq_qsize_memory_group_c_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_qsize_memory_group_c_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_qsize_memory_group_c_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_qsize_memory_group_c_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_qsize_memory_group_c_tbl";
#endif
      counter++;
    }
    /* VSQ Qsize memory - group D */
    if (table_offset == tables->iqm.vsq_qsize_memory_group_d_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_qsize_memory_group_d_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_qsize_memory_group_d_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_qsize_memory_group_d_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_qsize_memory_group_d_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group A */
    if (table_offset == tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_average_qsize_memory_group_a_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_average_qsize_memory_group_a_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group B */
    if (table_offset == tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_average_qsize_memory_group_b_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_average_qsize_memory_group_b_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group C */
    if (table_offset == tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_average_qsize_memory_group_c_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_average_qsize_memory_group_c_tbl";
#endif
      counter++;
    }
    /* VSQ Average Qsize memory - group D */
    if (table_offset == tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_average_qsize_memory_group_d_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.vsq_average_qsize_memory_group_d_tbl";
#endif
      counter++;
    }
    /* VSQ Flow-Control Parameters table - group A */
    for (tbl_ndx = 0; tbl_ndx < SOC_PETRA_NOF_VSQ_GROUPS; ++tbl_ndx)
    {
      if (table_offset == tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_ndx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_ndx].addr.base;
        indirect_print[counter].size   = tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_ndx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_flow_control_parameters_table_group_tbl[tbl_ndx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "IQM.vsq_flow_control_parameters_table_group_tbl[tbl_ndx]";
  #endif
        counter++;
      }
      /* VSQ Queue Parameters table - group A */
      if (table_offset == tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_ndx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_ndx].addr.base;
        indirect_print[counter].size   = tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_ndx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.vsq_queue_parameters_table_group_tbl[tbl_ndx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "IQM.vsq_queue_parameters_table_group_tbl[tbl_ndx]";
  #endif
        counter++;
      }
    }

    /* System Red parameters table */
    if (table_offset == tables->iqm.system_red_parameters_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.system_red_parameters_table_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.system_red_parameters_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.system_red_parameters_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.system_red_parameters_table_tbl";
#endif
      counter++;
    }
    /* System Red drop probability values */
    if (table_offset == tables->iqm.system_red_drop_probability_values_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.system_red_drop_probability_values_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.system_red_drop_probability_values_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.system_red_drop_probability_values_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.system_red_drop_probability_values_tbl";
#endif
      counter++;
    }
    /* Source Qsize range thresholds (System Red) */
    if (table_offset == tables->iqm.system_red_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.system_red_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.system_red_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.system_red_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.system_red_tbl";
#endif
      counter++;
    }
    /* Congestion Point (CNM) descriptor - Static */
    if (table_offset == tables->iqm.cnm_descriptor_static_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.cnm_descriptor_static_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.cnm_descriptor_static_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.cnm_descriptor_static_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.cnm_descriptor_static_tbl";
#endif
      counter++;
    }
    /* Congestion Point (CNM) descriptor - Dynamic */
    if (table_offset == tables->iqm.cnm_descriptor_dynamic_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.cnm_descriptor_dynamic_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.cnm_descriptor_dynamic_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.cnm_descriptor_dynamic_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.cnm_descriptor_dynamic_tbl";
#endif
      counter++;
    }
    /* Congestion Point (CNM) parameters table */
    if (table_offset == tables->iqm.cnm_parameters_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.cnm_parameters_table_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.cnm_parameters_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.cnm_parameters_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.cnm_parameters_table_tbl";
#endif
      counter++;
    }
    /* Meter-processor result resolve table - Static */
    if (table_offset == tables->iqm.meter_processor_result_resolve_table_static_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->iqm.meter_processor_result_resolve_table_static_tbl.addr.base;
      indirect_print[counter].size   = tables->iqm.meter_processor_result_resolve_table_static_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->iqm.meter_processor_result_resolve_table_static_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IQM_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IQM.meter_processor_result_resolve_table_static_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PB_IPS_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* System Physical Port Lookup Table */
    if (table_offset == tables->ips.system_physical_port_lookup_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.system_physical_port_lookup_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.system_physical_port_lookup_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.system_physical_port_lookup_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.system_physical_port_lookup_table_tbl";
#endif
      counter++;
    }
    /* Destination Device And Port Lookup Table */
    if (table_offset == tables->ips.destination_device_and_port_lookup_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.destination_device_and_port_lookup_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.destination_device_and_port_lookup_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.destination_device_and_port_lookup_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.destination_device_and_port_lookup_table_tbl";
#endif
      counter++;
    }
    /* Flow Id Lookup Table */
    if (table_offset == tables->ips.flow_id_lookup_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.flow_id_lookup_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.flow_id_lookup_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.flow_id_lookup_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.flow_id_lookup_table_tbl";
#endif
      counter++;
    }
    /* Queue Type Lookup Table */
    if (table_offset == tables->ips.queue_type_lookup_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.queue_type_lookup_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.queue_type_lookup_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.queue_type_lookup_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.queue_type_lookup_table_tbl";
#endif
      counter++;
    }
    /* Queue Priority Map Select */
    if (table_offset == tables->ips.queue_priority_map_select_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.queue_priority_map_select_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.queue_priority_map_select_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.queue_priority_map_select_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.queue_priority_map_select_tbl";
#endif
      counter++;
    }
    /* Queue Priority Maps Table */
    if (table_offset == tables->ips.queue_priority_maps_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.queue_priority_maps_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.queue_priority_maps_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.queue_priority_maps_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.queue_priority_maps_table_tbl";
#endif
      counter++;
    }
    /* Queue Size-Based Thresholds Table */
    if (table_offset == tables->ips.queue_size_based_thresholds_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.queue_size_based_thresholds_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.queue_size_based_thresholds_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.queue_size_based_thresholds_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.queue_size_based_thresholds_table_tbl";
#endif
      counter++;
    }
    /* Credit Balance Based Thresholds Table */
    if (table_offset == tables->ips.credit_balance_based_thresholds_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.credit_balance_based_thresholds_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.credit_balance_based_thresholds_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.credit_balance_based_thresholds_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.credit_balance_based_thresholds_table_tbl";
#endif
      counter++;
    }
    /* Empty Queue Credit Balance Table */
    if (table_offset == tables->ips.empty_queue_credit_balance_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.empty_queue_credit_balance_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.empty_queue_credit_balance_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.empty_queue_credit_balance_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.empty_queue_credit_balance_table_tbl";
#endif
      counter++;
    }
    /* Credit Watchdog Thresholds Table */
    if (table_offset == tables->ips.credit_watchdog_thresholds_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.credit_watchdog_thresholds_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.credit_watchdog_thresholds_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.credit_watchdog_thresholds_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.credit_watchdog_thresholds_table_tbl";
#endif
      counter++;
    }
    /* Queue Descriptor Table */
    if (table_offset == tables->ips.queue_descriptor_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.queue_descriptor_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.queue_descriptor_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.queue_descriptor_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.queue_descriptor_table_tbl";
#endif
      counter++;
    }
    /* Queue Size Table */
    if (table_offset == tables->ips.queue_size_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.queue_size_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.queue_size_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.queue_size_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.queue_size_table_tbl";
#endif
      counter++;
    }
    /* System Red Max Queue Size Table */
    if (table_offset == tables->ips.system_red_max_queue_size_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.system_red_max_queue_size_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.system_red_max_queue_size_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.system_red_max_queue_size_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.system_red_max_queue_size_table_tbl";
#endif
      counter++;
    }
/* System Red Max Queue Size Table */
    if (table_offset == tables->ips.fms_msg_mem_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.fms_msg_mem_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.fms_msg_mem_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.fms_msg_mem_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.fms_msg_mem_table_tbl";
#endif
      counter++;
    }
/* fms_timestamp_table_tbl */
    if (table_offset == tables->ips.fms_timestamp_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.fms_timestamp_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.fms_timestamp_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.fms_timestamp_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.fms_timestamp_table_tbl";
#endif
      counter++;
    }
/*enq_block_qnum_table_tbl */
    if (table_offset == tables->ips.enq_block_qnum_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ips.enq_block_qnum_table_tbl.addr.base;
      indirect_print[counter].size   = tables->ips.enq_block_qnum_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ips.enq_block_qnum_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPS_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPS.enq_block_qnum_table_tbl";
#endif
      counter++;
    }
    
  }
  if (block_id == SOC_PB_IPT_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* BDQ */
    if (table_offset == tables->ipt.bdq_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.bdq_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.bdq_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.bdq_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.bdq_tbl";
#endif
      counter++;
    }
    /* PCQ */
    if (table_offset == tables->ipt.pcq_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.pcq_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.pcq_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.pcq_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.pcq_tbl";
#endif
      counter++;
    }
    /* Sop MMU */
    if (table_offset == tables->ipt.sop_mmu_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.sop_mmu_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.sop_mmu_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.sop_mmu_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.sop_mmu_tbl";
#endif
      counter++;
    }
    /* Mop MMU */
    if (table_offset == tables->ipt.mop_mmu_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.mop_mmu_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.mop_mmu_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.mop_mmu_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.mop_mmu_tbl";
#endif
      counter++;
    }
    /* FDTCTL */
    if (table_offset == tables->ipt.fdtctl_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.fdtctl_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.fdtctl_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.fdtctl_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.fdtctl_tbl";
#endif
      counter++;
    }
    /* FDTDATA */
    if (table_offset == tables->ipt.fdtdata_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.fdtdata_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.fdtdata_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.fdtdata_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.fdtdata_tbl";
#endif
      counter++;
    }
    /* EGQCTL */
    if (table_offset == tables->ipt.egqctl_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.egqctl_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.egqctl_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.egqctl_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.egqctl_tbl";
#endif
      counter++;
    }
    /* EGQDATA */
    if (table_offset == tables->ipt.egqdata_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.egqdata_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.egqdata_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.egqdata_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.egqdata_tbl";
#endif
      counter++;
    }
    /* Egq Txq Wr Addr */
    if (table_offset == tables->ipt.egq_txq_wr_addr_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.egq_txq_wr_addr_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.egq_txq_wr_addr_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.egq_txq_wr_addr_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.egq_txq_wr_addr_tbl";
#endif
      counter++;
    }
    /* Egq Txq Rd Addr */
    if (table_offset == tables->ipt.egq_txq_rd_addr_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.egq_txq_rd_addr_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.egq_txq_rd_addr_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.egq_txq_rd_addr_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.egq_txq_rd_addr_tbl";
#endif
      counter++;
    }
    /* Fdt Txq Wr Addr */
    if (table_offset == tables->ipt.fdt_txq_wr_addr_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.fdt_txq_wr_addr_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.fdt_txq_wr_addr_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.fdt_txq_wr_addr_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.fdt_txq_wr_addr_tbl";
#endif
      counter++;
    }
    /* Fdt Txq Rd Addr */
    if (table_offset == tables->ipt.fdt_txq_rd_addr_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.fdt_txq_rd_addr_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.fdt_txq_rd_addr_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.fdt_txq_rd_addr_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.fdt_txq_rd_addr_tbl";
#endif
      counter++;
    }
    /* Gci Backoff Mask */
    if (table_offset == tables->ipt.gci_backoff_mask_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->ipt.gci_backoff_mask_tbl.addr.base;
      indirect_print[counter].size   = tables->ipt.gci_backoff_mask_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->ipt.gci_backoff_mask_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_IPT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "IPT.gci_backoff_mask_tbl";
#endif
      counter++;
    }
  }
  if ((block_id >= SOC_PB_DPI_A_ID) && (block_id <= SOC_PB_DPI_F_ID))
  {
    /* DLL_RAM */
    if (table_offset == tables->dpi.dll_ram_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->dpi.dll_ram_tbl.addr.base;
      indirect_print[counter].size   = tables->dpi.dll_ram_tbl.addr.size;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->dpi.dll_ram_tbl.addr.width_bits);
      indirect_print[counter].mod_id = block_id;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "DPI.dll_ram_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PB_FDT_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Ipt Contro L Fifo */
    if (table_offset == tables->fdt.ipt_contro_l_fifo_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->fdt.ipt_contro_l_fifo_tbl.addr.base;
      indirect_print[counter].size   = tables->fdt.ipt_contro_l_fifo_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->fdt.ipt_contro_l_fifo_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_FDT_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "FDT.ipt_contro_l_fifo_tbl";
#endif
      counter++;
    }
  }
  if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL)
  {
    if (block_id == SOC_PB_NBI_ID || block_id == SOC_PB_NOF_MODULES)
    {

      for (indx = 0; indx < 16; ++indx)
      {
        /* mlf_rx_tbl[indx] */
        if (table_offset == tables->nbi.mlf_rx_tbl[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->nbi.mlf_rx_tbl[indx].addr.base;
          indirect_print[counter].size   = tables->nbi.mlf_rx_tbl[indx].addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->nbi.mlf_rx_tbl[indx].addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_NBI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "NBI.mlf_rx_tbl[indx]";
    #endif
          counter++;
        }
      }

      for (indx = 0; indx < 16; ++indx)
      {
        /* mlf_tx_tbl[indx] */
        if (table_offset == tables->nbi.mlf_tx_tbl[indx].addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->nbi.mlf_tx_tbl[indx].addr.base;
          indirect_print[counter].size   = tables->nbi.mlf_tx_tbl[indx].addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->nbi.mlf_tx_tbl[indx].addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_NBI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "NBI.mlf_tx_tbl[indx]";
    #endif
          counter++;
        }
      }
      /* mlf_rx_eob */
      if (table_offset == tables->nbi.mlf_rx_eob.addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = tables->nbi.mlf_rx_eob.addr.base;
        indirect_print[counter].size   = tables->nbi.mlf_rx_eob.addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->nbi.mlf_rx_eob.addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_NBI_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "NBI.mlf_rx_eob";
  #endif
        counter++;
      }

      /* mlf_tx_eob */
      if (table_offset == tables->nbi.mlf_tx_eob.addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = tables->nbi.mlf_tx_eob.addr.base;
        indirect_print[counter].size   = tables->nbi.mlf_tx_eob.addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->nbi.mlf_tx_eob.addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_NBI_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        indirect_print[counter].name   = "NBI.mlf_tx_eob";
  #endif
        counter++;
      }
    }
  }
  if (block_id == SOC_PB_RTP_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Unicast Distribution Memory for data cells */
    if (table_offset == tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.base;
      indirect_print[counter].size   = tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->rtp.unicast_distribution_memory_for_data_cells_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_RTP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "RTP.unicast_distribution_memory_for_data_cells_tbl";
#endif
      counter++;
    }
    /* Unicast Distribution Memory for control cells */
    if (table_offset == tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.base;
      indirect_print[counter].size   = tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->rtp.unicast_distribution_memory_for_control_cells_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_RTP_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "RTP.unicast_distribution_memory_for_control_cells_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PB_EGQ_ID || block_id == SOC_PB_NOF_MODULES)
  {
    for (tbl_ndx = 0; tbl_ndx < SOC_PB_NOF_MAC_LANES; tbl_ndx++)
    {
      /* Egress Shaper Nif Mal Credit Configuration (Nif Mal0 Scm) */
      if ((table_offset == tables->egq.nif_scm_tbl.addr.base + tbl_ndx * SOC_PETRA_EGQ_NIF_SCM_TBL_OFFSET) || (table_offset == SOC_SAND_U32_MAX))
      {
        indirect_print[counter].base   = tables->egq.nif_scm_tbl.addr.base + tbl_ndx * SOC_PETRA_EGQ_NIF_SCM_TBL_OFFSET;
        indirect_print[counter].size   = tables->egq.nif_scm_tbl.addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.nif_scm_tbl.addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        switch(tbl_ndx) {
        case 0:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[0]";
        	break;
        case 1:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[1]";
        	break;
        case 2:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[2]";
        	break;
        case 3:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[3]";
        	break;
        case 4:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[4]";
        	break;
        case 5:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[5]";
        	break;
        case 6:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[6]";
        	break;
        case 7:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[7]";
        	break;
        case 8:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[8]";
        	break;
        case 9:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[9]";
        	break;
        case 10:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[10]";
        	break;
        case 11:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[11]";
        	break;
        case 12:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[12]";
        	break;
        case 13:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[13]";
        	break;
        case 14:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[14]";
        	break;
        case 15:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl[15]";
        	break;
        default:
          indirect_print[counter].name   = "EGQ.nif_scm_tbl";
        }
        
  #endif
        counter++;
      }
    }
    /* Egress Shaper Recycling Ports Credit Configuration (Rcy Scm) */
    if (table_offset == tables->egq.rcy_scm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.rcy_scm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.rcy_scm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.rcy_scm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.rcy_scm_tbl";
#endif
      counter++;
    }
    /* Egress Shaper CPUPorts Credit Configuration(Cpu Scm) */
    if (table_offset == tables->egq.cpu_scm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.cpu_scm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.cpu_scm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.cpu_scm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.cpu_scm_tbl";
#endif
      counter++;
    }
    /* Egress Shaper Calendar Selector (CCM) */
    if (table_offset == tables->egq.ccm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ccm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ccm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ccm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ccm_tbl";
#endif
      counter++;
    }
    /* Per Port Max Credit Memory (PMC) */
    if (table_offset == tables->egq.pmc_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pmc_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pmc_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pmc_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pmc_tbl";
#endif
      counter++;
    }
    /* Egress Shaper Per Port Credit Balance Memory (CBM) */
    if (table_offset == tables->egq.cbm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.cbm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.cbm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.cbm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.cbm_tbl";
#endif
      counter++;
    }
    /* Free Buffers Allocation Bitmap Memory (FBM) */
    if (table_offset == tables->egq.fbm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.fbm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.fbm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.fbm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.fbm_tbl";
#endif
      counter++;
    }
    /* Free Descriptors Allocation Bitmap Memory (FDM) */
    if (table_offset == tables->egq.fdm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.fdm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.fdm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.fdm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.fdm_tbl";
#endif
      counter++;
    }
    /* QM */
    if (table_offset == tables->egq.qm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.qm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.qm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.qm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.qm_tbl";
#endif
      counter++;
    }
    /* QSM */
    if (table_offset == tables->egq.qsm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.qsm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.qsm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.qsm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.qsm_tbl";
#endif
      counter++;
    }
    /* DCM */
    if (table_offset == tables->egq.dcm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.dcm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.dcm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.dcm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.dcm_tbl";
#endif
      counter++;
    }
    /* Ofp Dequeue Wfq Configuration Memory (DWM) */
    if (table_offset == tables->egq.dwm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.dwm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.dwm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.dwm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.dwm_tbl";
#endif
      counter++;
    }
    /* Ofp Dequeue Wfq Configuration Memory (DWM) */
    if (table_offset == tables->egq.dwm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.dwm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.dwm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.dwm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.dwm_tbl";
#endif
      counter++;
    }
    /* RRDM */
    if (table_offset == tables->egq.rrdm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.rrdm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.rrdm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.rrdm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.rrdm_tbl";
#endif
      counter++;
    }
    /* RPDM */
    if (table_offset == tables->egq.rpdm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.rpdm_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.rpdm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.rpdm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.rpdm_tbl";
#endif
      counter++;
    }
    /* Port Configuration Table (PCT) */
    if (table_offset == tables->egq.pct_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pct_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pct_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pct_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pct_tbl";
#endif
      counter++;
    }
    /* Vlan Table Configuration Memory(Vlan Table) */
    if (table_offset == tables->egq.vlan_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.vlan_table_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.vlan_table_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.vlan_table_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.vlan_table_tbl";
#endif
      counter++;
    }
    /* Tc Dp Map */
    if (table_offset == tables->egq.tc_dp_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.tc_dp_map_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.tc_dp_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.tc_dp_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.tc_dp_map_tbl";
#endif
      counter++;
    }
    /* Cfc Flow Control */
    if (table_offset == tables->egq.cfc_flow_control_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.cfc_flow_control_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.cfc_flow_control_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.cfc_flow_control_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.cfc_flow_control_tbl";
#endif
      counter++;
    }
    /* Nifa Flow Control */
    if (table_offset == tables->egq.nifa_flow_control_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.nifa_flow_control_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.nifa_flow_control_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.nifa_flow_control_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.nifa_flow_control_tbl";
#endif
      counter++;
    }
    /* Nifb Flow Control */
    if (table_offset == tables->egq.nifb_flow_control_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.nifb_flow_control_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.nifb_flow_control_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.nifb_flow_control_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.nifb_flow_control_tbl";
#endif
      counter++;
    }
    /* Cpu Last Header */
    if (table_offset == tables->egq.cpu_last_header_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.cpu_last_header_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.cpu_last_header_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.cpu_last_header_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.cpu_last_header_tbl";
#endif
      counter++;
    }
    /* Ipt Last Header */
    if (table_offset == tables->egq.ipt_last_header_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ipt_last_header_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ipt_last_header_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ipt_last_header_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ipt_last_header_tbl";
#endif
      counter++;
    }
    /* Fdr Last Header */
    if (table_offset == tables->egq.fdr_last_header_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.fdr_last_header_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.fdr_last_header_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.fdr_last_header_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.fdr_last_header_tbl";
#endif
      counter++;
    }
    /* Cpu Packet Counter */
    if (table_offset == tables->egq.cpu_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.cpu_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.cpu_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.cpu_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.cpu_packet_counter_tbl";
#endif
      counter++;
    }
    /* Ipt Packet Counter */
    if (table_offset == tables->egq.ipt_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ipt_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ipt_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ipt_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ipt_packet_counter_tbl";
#endif
      counter++;
    }
    /* Fdr Packet Counter */
    if (table_offset == tables->egq.fdr_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.fdr_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.fdr_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.fdr_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.fdr_packet_counter_tbl";
#endif
      counter++;
    }
    /* Rqp Packet Counter */
    if (table_offset == tables->egq.rqp_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.rqp_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.rqp_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.rqp_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.rqp_packet_counter_tbl";
#endif
      counter++;
    }
    /* Rqp Discard Packet Counter */
    if (table_offset == tables->egq.rqp_discard_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.rqp_discard_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.rqp_discard_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.rqp_discard_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.rqp_discard_packet_counter_tbl";
#endif
      counter++;
    }
    /* Ehp Unicast Packet Counter */
    if (table_offset == tables->egq.ehp_unicast_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ehp_unicast_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ehp_unicast_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ehp_unicast_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ehp_unicast_packet_counter_tbl";
#endif
      counter++;
    }
    /* Ehp Multicast High Packet Counter */
    if (table_offset == tables->egq.ehp_multicast_high_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ehp_multicast_high_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ehp_multicast_high_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ehp_multicast_high_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ehp_multicast_high_packet_counter_tbl";
#endif
      counter++;
    }
    /* Ehp Multicast Low Packet Counter */
    if (table_offset == tables->egq.ehp_multicast_low_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ehp_multicast_low_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ehp_multicast_low_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ehp_multicast_low_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ehp_multicast_low_packet_counter_tbl";
#endif
      counter++;
    }
    /* Ehp Discard Packet Counter */
    if (table_offset == tables->egq.ehp_discard_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.ehp_discard_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.ehp_discard_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.ehp_discard_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.ehp_discard_packet_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Unicast High Packet Counter */
    if (table_offset == tables->egq.pqp_unicast_high_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_unicast_high_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_unicast_high_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_unicast_high_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_unicast_high_packet_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Unicast Low Packet Counter */
    if (table_offset == tables->egq.pqp_unicast_low_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_unicast_low_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_unicast_low_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_unicast_low_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_unicast_low_packet_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Multicast High Packet Counter */
    if (table_offset == tables->egq.pqp_multicast_high_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_multicast_high_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_multicast_high_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_multicast_high_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_multicast_high_packet_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Multicast Low Packet Counter */
    if (table_offset == tables->egq.pqp_multicast_low_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_multicast_low_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_multicast_low_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_multicast_low_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_multicast_low_packet_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Unicast High Bytes Counter */
    if (table_offset == tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_unicast_high_bytes_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_unicast_high_bytes_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Unicast Low Bytes Counter */
    if (table_offset == tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_unicast_low_bytes_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_unicast_low_bytes_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Multicast High Bytes Counter */
    if (table_offset == tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_multicast_high_bytes_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_multicast_high_bytes_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Multicast Low Bytes Counter */
    if (table_offset == tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_multicast_low_bytes_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_multicast_low_bytes_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Discard Unicast Packet Counter */
    if (table_offset == tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_discard_unicast_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_discard_unicast_packet_counter_tbl";
#endif
      counter++;
    }
    /* Pqp Discard Multicast Packet Counter */
    if (table_offset == tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.pqp_discard_multicast_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.pqp_discard_multicast_packet_counter_tbl";
#endif
      counter++;
    }
    /* Fqp Packet Counter */
    if (table_offset == tables->egq.fqp_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.fqp_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.fqp_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.fqp_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.fqp_packet_counter_tbl";
#endif
      counter++;
    }
    /* Fqp Nif Port Mux */
    if (table_offset == tables->egq.fqp_nif_port_mux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.fqp_nif_port_mux_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.fqp_nif_port_mux_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.fqp_nif_port_mux_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.fqp_nif_port_mux_tbl";
#endif
      counter++;
    }
    /* Key Profile Map Index */
    if (table_offset == tables->egq.key_profile_map_index_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.key_profile_map_index_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.key_profile_map_index_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.key_profile_map_index_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.key_profile_map_index_tbl";
#endif
      counter++;
    }
    /* TCAM Key Resolution Profile */
    if (table_offset == tables->egq.tcam_key_resolution_profile_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.tcam_key_resolution_profile_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.tcam_key_resolution_profile_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.tcam_key_resolution_profile_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.tcam_key_resolution_profile_tbl";
#endif
      counter++;
    }
    /* Parser Last Sys Record */
    if (table_offset == tables->egq.parser_last_sys_record_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.parser_last_sys_record_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.parser_last_sys_record_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.parser_last_sys_record_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.parser_last_sys_record_tbl";
#endif
      counter++;
    }
    /* Parser Last Nwk Record1 */
    if (table_offset == tables->egq.parser_last_nwk_record1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.parser_last_nwk_record1_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.parser_last_nwk_record1_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.parser_last_nwk_record1_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.parser_last_nwk_record1_tbl";
#endif
      counter++;
    }
    /* Parser Last Nwk Record2 */
    if (table_offset == tables->egq.parser_last_nwk_record2_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.parser_last_nwk_record2_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.parser_last_nwk_record2_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.parser_last_nwk_record2_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.parser_last_nwk_record2_tbl";
#endif
      counter++;
    }
    /* Erpp Debug */
    if (table_offset == tables->egq.erpp_debug_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->egq.erpp_debug_tbl.addr.base;
      indirect_print[counter].size   = tables->egq.erpp_debug_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.erpp_debug_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EGQ.erpp_debug_tbl";
#endif
      counter++;
    }

  /* check here the dump_mode and print the values accordingly */
     if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL)
     {
        if (table_offset == tables->egq.svem_keyt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_keyt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_keyt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_keyt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_keyt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_keyt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_keyt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_keyt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_keyt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_keyt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_keyt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_keyt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_keyt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_keyt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_keyt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_pldt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_pldt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_pldt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_pldt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_pldt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_pldt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_pldt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_pldt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_pldt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_pldt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_pldt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_pldt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_pldt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_pldt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_pldt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_mgmt_mem_a_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_mgmt_mem_a_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_mgmt_mem_a_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_mgmt_mem_a_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_mgmt_mem_a_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->egq.svem_mgmt_mem_b_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->egq.svem_mgmt_mem_b_tbl.addr.base;
          indirect_print[counter].size   = tables->egq.svem_mgmt_mem_b_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->egq.svem_mgmt_mem_b_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EGQ.svem_mgmt_mem_b_tbl";
    #endif
          counter++;
        }
	    /* Per Port Configuration Table(PPCT) */
	    if (table_offset == pp_tables->egq.ppct_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.ppct_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.ppct_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.ppct_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.ppct_tbl";
	#endif
	      counter++;
	    }
	    /* packet processing Per Port Configuration Table(PP PPCT) */
	    if (table_offset == pp_tables->egq.pp_ppct_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.pp_ppct_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.pp_ppct_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.pp_ppct_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.pp_ppct_tbl";
	#endif
	      counter++;
	    }
	    /* Ingress Vlan Edit Command Map */
	    if (table_offset == pp_tables->egq.ingress_vlan_edit_command_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.ingress_vlan_edit_command_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.ingress_vlan_edit_command_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.ingress_vlan_edit_command_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.ingress_vlan_edit_command_map_tbl";
	#endif
	      counter++;
	    }
	    /* Vsi Membership Memory(Vsi Membership) */
	    if (table_offset == pp_tables->egq.vsi_membership_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.vsi_membership_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.vsi_membership_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.vsi_membership_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.vsi_membership_tbl";
	#endif
	      counter++;
	    }
	    /* Ttl Scope Memory(Ttl Scope) */
	    if (table_offset == pp_tables->egq.ttl_scope_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.ttl_scope_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.ttl_scope_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.ttl_scope_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.ttl_scope_tbl";
	#endif
	      counter++;
	    }
	    /* Auxiliary Data Memory(Aux Table) */
	    if (table_offset == pp_tables->egq.aux_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.aux_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.aux_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.aux_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.aux_table_tbl";
	#endif
	      counter++;
	    }
	    /* Eep Orientation Memory(Eep Orientation) */
	    if (table_offset == pp_tables->egq.eep_orientation_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.eep_orientation_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.eep_orientation_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.eep_orientation_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.eep_orientation_tbl";
	#endif
	      counter++;
	    }
	    /* Cfm Trap */
	    if (table_offset == pp_tables->egq.cfm_trap_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.cfm_trap_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.cfm_trap_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.cfm_trap_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.cfm_trap_tbl";
	#endif
	      counter++;
	    }
	    /* Action Profile Table */
	    if (table_offset == pp_tables->egq.action_profile_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->egq.action_profile_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->egq.action_profile_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->egq.action_profile_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EGQ_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EGQ.action_profile_table_tbl";
	#endif
	      counter++;
	    }
    }

  }
  if (block_id == SOC_PB_EPNI_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Epe Packet Counter */
    if (table_offset == tables->epni.epe_packet_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->epni.epe_packet_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->epni.epe_packet_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.epe_packet_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EPNI.epe_packet_counter_tbl";
#endif
      counter++;
    }
    /* Epe Bytes Counter */
    if (table_offset == tables->epni.epe_bytes_counter_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->epni.epe_bytes_counter_tbl.addr.base;
      indirect_print[counter].size   = tables->epni.epe_bytes_counter_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.epe_bytes_counter_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EPNI.epe_bytes_counter_tbl";
#endif
      counter++;
    }
    /* Copy Engine0/1/2 Program */
    for (tbl_ndx = 0; tbl_ndx < SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_TBLS; ++tbl_ndx)
    {
      if (table_offset == tables->epni.copy_engine_program_tbl[tbl_ndx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = tables->epni.copy_engine_program_tbl[tbl_ndx].addr.base;
        indirect_print[counter].size   = tables->epni.copy_engine_program_tbl[tbl_ndx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.copy_engine_program_tbl[tbl_ndx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        switch (tbl_ndx)
        {
        case 0:
          indirect_print[counter].name   = "EPNI.copy_engine0_program_tbl";
          break;
        case 1:
          indirect_print[counter].name   = "EPNI.copy_engine1_program_tbl";
          break;
        case 2:
          indirect_print[counter].name   = "EPNI.copy_engine2_program_tbl";
          break;
        }
  #endif
        counter++;
      }
    }

    /* Lfem0/1/2 Field Select Map */
    for (tbl_ndx = 0; tbl_ndx < SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS; ++tbl_ndx)
    {
      if (table_offset == tables->epni.lfem_field_select_map_tbl[tbl_ndx].addr.base || table_offset == SOC_SAND_U32_MAX)
      {
        indirect_print[counter].base   = tables->epni.lfem_field_select_map_tbl[tbl_ndx].addr.base;
        indirect_print[counter].size   = tables->epni.lfem_field_select_map_tbl[tbl_ndx].addr.size;
        indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.lfem_field_select_map_tbl[tbl_ndx].addr.width_bits);
        indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
  #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
        switch (tbl_ndx)
        {
        case 0:
          indirect_print[counter].name   = "EPNI.lfem0_field_select_map_tbl";
          break;
        case 1:
          indirect_print[counter].name   = "EPNI.lfem1_field_select_map_tbl";
          break;
        case 2:
          indirect_print[counter].name   = "EPNI.lfem2_field_select_map_tbl";
          break;
        }
  #endif
        counter++;
      }
    }

  /* check the dump_mode value and get the relvant tables accordingly */

  if (dump_mode == SOC_PETRA_DIAG_TBLS_DUMP_MODE_FULL)
     {
        if (table_offset == tables->epni.esem_keyt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_keyt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_keyt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_keyt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_keyt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_keyt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_keyt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_keyt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_keyt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_keyt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_keyt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_keyt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_keyt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_keyt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_keyt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_pldt_h0_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_pldt_h0_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_pldt_h0_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_pldt_h0_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_pldt_h0_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_pldt_h1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_pldt_h1_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_pldt_h1_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_pldt_h1_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_pldt_h1_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_pldt_aux_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_pldt_aux_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_pldt_aux_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_pldt_aux_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_pldt_aux_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_mgmt_mem_a_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_mgmt_mem_a_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_mgmt_mem_a_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_mgmt_mem_a_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_mgmt_mem_a_tbl";
    #endif
          counter++;
        }

        if (table_offset == tables->epni.esem_mgmt_mem_b_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
        {
          indirect_print[counter].base   = tables->epni.esem_mgmt_mem_b_tbl.addr.base;
          indirect_print[counter].size   = tables->epni.esem_mgmt_mem_b_tbl.addr.size;
          indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->epni.esem_mgmt_mem_b_tbl.addr.width_bits);
          indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
    #ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
          indirect_print[counter].name   = "EPNI.esem_mgmt_mem_b_tbl";
    #endif
          counter++;
        }
	    /* encap db1 */
	    if (table_offset == pp_tables->epni.egress_encapsulation_bank1_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.egress_encapsulation_bank1_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.egress_encapsulation_bank1_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.egress_encapsulation_bank1_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.encapsulation_DB2";
	#endif
	      counter++;
	    }
	    /* encap db2 */
	    if (table_offset == pp_tables->epni.egress_encapsulation_bank2_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.egress_encapsulation_bank2_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.egress_encapsulation_bank2_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.egress_encapsulation_bank2_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.encapsulation_DB2";
	#endif
	      counter++;
	    }
	    /* Tx Tag Table */
	    if (table_offset == pp_tables->epni.tx_tag_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.tx_tag_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.tx_tag_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.tx_tag_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.tx_tag_table_tbl";
	#endif
	      counter++;
	    }
	    /* Spanning Tree Protocol State Memory (STP) */
	    if (table_offset == pp_tables->epni.stp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.stp_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.stp_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.stp_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.stp_tbl";
	#endif
	      counter++;
	    }
	    /* Small Em Result Memory */
	    if (table_offset == pp_tables->epni.small_em_result_memory_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.small_em_result_memory_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.small_em_result_memory_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.small_em_result_memory_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.small_em_result_memory_tbl";
	#endif
	      counter++;
	    }
	    /* Pcp Dei Table */
	    if (table_offset == pp_tables->epni.pcp_dei_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.pcp_dei_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.pcp_dei_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.pcp_dei_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.pcp_dei_table_tbl";
	#endif
	      counter++;
	    }
	    /* PP PCT */
	    if (table_offset == pp_tables->epni.pp_pct_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.pp_pct_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.pp_pct_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.pp_pct_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.pp_pct_tbl";
	#endif
	      counter++;
	    }
	    /* Llvp Table */
	    if (table_offset == pp_tables->epni.llvp_table_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.llvp_table_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.llvp_table_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.llvp_table_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.llvp_table_tbl";
	#endif
	      counter++;
	    }
	    /* Egress Edit Cmd */
	    if (table_offset == pp_tables->epni.egress_edit_cmd_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.egress_edit_cmd_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.egress_edit_cmd_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.egress_edit_cmd_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.egress_edit_cmd_tbl";
	#endif
	      counter++;
	    }
	    /* Program Vars */
	    if (table_offset == pp_tables->epni.program_vars_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.program_vars_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.program_vars_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.program_vars_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.program_vars_tbl";
	#endif
	      counter++;
	    }
	    /* Ingress Vlan Edit Command Map */
	    if (table_offset == pp_tables->epni.ingress_vlan_edit_command_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
	    {
	      indirect_print[counter].base   = pp_tables->epni.ingress_vlan_edit_command_map_tbl.addr.base;
	      indirect_print[counter].size   = pp_tables->epni.ingress_vlan_edit_command_map_tbl.addr.size;
	      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.ingress_vlan_edit_command_map_tbl.addr.width_bits);
	      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
	#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
	      indirect_print[counter].name   = "EPNI.ingress_vlan_edit_command_map_tbl";
	#endif
	      counter++;
	    }
	    /* Ip Exp Map */
    if (table_offset == pp_tables->epni.ip_exp_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->epni.ip_exp_map_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->epni.ip_exp_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.ip_exp_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EPNI.ip_exp_map_tbl";
#endif
      counter++;
    }
    /* Etpp Debug */
    if (table_offset == pp_tables->epni.etpp_debug_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = pp_tables->epni.etpp_debug_tbl.addr.base;
      indirect_print[counter].size   = pp_tables->epni.etpp_debug_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(pp_tables->epni.etpp_debug_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_EPNI_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "EPNI.etpp_debug_tbl";
#endif
      counter++;
    }
    }

   
  }
  if (block_id == SOC_PB_CFC_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* RCL2 OFP */
    if (table_offset == tables->cfc.rcl2_ofp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.rcl2_ofp_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.rcl2_ofp_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.rcl2_ofp_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.rcl2_ofp_tbl";
#endif
      counter++;
    }
    /* NIFCLSB2 OFP */
    if (table_offset == tables->cfc.nifclsb2_ofp_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.nifclsb2_ofp_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.nifclsb2_ofp_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.nifclsb2_ofp_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.nifclsb2_ofp_tbl";
#endif
      counter++;
    }
    /* CALRXA */
    if (table_offset == tables->cfc.calrxa_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.calrxa_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.calrxa_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.calrxa_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.calrxa_tbl";
#endif
      counter++;
    }
    /* CALRXB */
    if (table_offset == tables->cfc.calrxb_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.calrxb_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.calrxb_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.calrxb_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.calrxb_tbl";
#endif
      counter++;
    }
    /* Oob0 Sch Map */
    if (table_offset == tables->cfc.oob0_sch_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.oob0_sch_map_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.oob0_sch_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.oob0_sch_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.oob0_sch_map_tbl";
#endif
      counter++;
    }
    /* Oob1 Sch Map */
    if (table_offset == tables->cfc.oob1_sch_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.oob1_sch_map_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.oob1_sch_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.oob1_sch_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.oob1_sch_map_tbl";
#endif
      counter++;
    }
    /* CALTX */
    if (table_offset == tables->cfc.caltx_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.caltx_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.caltx_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.caltx_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.caltx_tbl";
#endif
      counter++;
    }
    /* 
     * ILKN CALRX cannot always be read under traffic if ILKN FC is active (HW limitation)
     */
    /* Ilkn0 Sch Map */
    if (table_offset == tables->cfc.ilkn0_sch_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.ilkn0_sch_map_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.ilkn0_sch_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.ilkn0_sch_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.ilkn0_sch_map_tbl";
#endif
      counter++;
    }
    /* Ilkn1 Sch Map */
    if (table_offset == tables->cfc.ilkn1_sch_map_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.ilkn1_sch_map_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.ilkn1_sch_map_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.ilkn1_sch_map_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.ilkn1_sch_map_tbl";
#endif
      counter++;
    }
    /* ILKN0 CALTX */
    if (table_offset == tables->cfc.ilkn0_caltx_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.ilkn0_caltx_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.ilkn0_caltx_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.ilkn0_caltx_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.ilkn0_caltx_tbl";
#endif
      counter++;
    }
    /* ILKN1 CALTX */
    if (table_offset == tables->cfc.ilkn1_caltx_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->cfc.ilkn1_caltx_tbl.addr.base;
      indirect_print[counter].size   = tables->cfc.ilkn1_caltx_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->cfc.ilkn1_caltx_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_CFC_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "CFC.ilkn1_caltx_tbl";
#endif
      counter++;
    }
  }
  if (block_id == SOC_PB_SCH_ID || block_id == SOC_PB_NOF_MODULES)
  {
    /* Scheduler Credit Generation Calendar (CAL) */
    if (table_offset == tables->sch.cal_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.cal_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.cal_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.cal_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.cal_tbl";
#endif
      counter++;
    }
    /* Device Rate Memory (DRM) */
    if (table_offset == tables->sch.drm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.drm_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.drm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.drm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.drm_tbl";
#endif
      counter++;
    }
    /* Dual Shaper Memory (DSM) */
    if (table_offset == tables->sch.dsm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.dsm_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.dsm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.dsm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.dsm_tbl";
#endif
      counter++;
    }
    /* Flow Descriptor Memory Static (FDMS) */
    if (table_offset == tables->sch.fdms_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.fdms_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.fdms_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.fdms_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.fdms_tbl";
#endif
      counter++;
    }
    /* Shaper Descriptor Memory Static (SHDS) */
    if (table_offset == tables->sch.shds_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.shds_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.shds_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.shds_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.shds_tbl";
#endif
      counter++;
    }
    /* Scheduler Enable Memory (SEM) */
    if (table_offset == tables->sch.sem_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.sem_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.sem_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.sem_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.sem_tbl";
#endif
      counter++;
    }
    /* Flow Sub-Flow (FSF) */
    if (table_offset == tables->sch.fsf_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.fsf_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.fsf_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.fsf_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.fsf_tbl";
#endif
      counter++;
    }
    /* Flow Group Memory (FGM) */
    if (table_offset == tables->sch.fgm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.fgm_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.fgm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.fgm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.fgm_tbl";
#endif
      counter++;
    }
    /* HR-Scheduler-Configuration (SHC) */
    if (table_offset == tables->sch.shc_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.shc_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.shc_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.shc_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.shc_tbl";
#endif
      counter++;
    }
    /* CL-Schedulers Configuration (SCC) */
    if (table_offset == tables->sch.scc_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.scc_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.scc_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.scc_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.scc_tbl";
#endif
      counter++;
    }
    /* CL-Schedulers Type (SCT) */
    if (table_offset == tables->sch.sct_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.sct_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.sct_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.sct_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.sct_tbl";
#endif
      counter++;
    }
    /* Flow to Queue Mapping (FQM) */
    if (table_offset == tables->sch.fqm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.fqm_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.fqm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.fqm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.fqm_tbl";
#endif
      counter++;
    }
    /* Flow to FIP Mapping (FFM) */
    if (table_offset == tables->sch.ffm_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.ffm_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.ffm_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.ffm_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.ffm_tbl";
#endif
      counter++;
    }


    /* Token Memory Controller (TMC) */
    if (table_offset == tables->sch.soc_tmctbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.soc_tmctbl.addr.base;
      indirect_print[counter].size   = tables->sch.soc_tmctbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.soc_tmctbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.soc_tmctbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40110000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40110000;
      indirect_print[counter].size   = 0x0e00;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fim_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40120000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40120000;
      indirect_print[counter].size   = 0x3800;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(8);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fsm_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40130000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40130000;
      indirect_print[counter].size   = 0xe000;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fdmd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40140000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40140000;
      indirect_print[counter].size   = 0xe000;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.shdd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40150000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40150000;
      indirect_print[counter].size   = 0x0100;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(32);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dfq_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40160000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40160000;
      indirect_print[counter].size   = 0x0100;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(78);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dhd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40170000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40170000;
      indirect_print[counter].size   = 0x2000;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(71);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dcd_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40180000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40180000;
      indirect_print[counter].size   = 0x0054;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(32);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sflh_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x40190000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40190000;
      indirect_print[counter].size   = 0x0054;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(32);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sflt_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x401a0000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x401a0000;
      indirect_print[counter].size   = 0x0300;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sad_tbl";
#endif
      counter++;
    }
    /* Internal */
    if (table_offset == 0x401b0000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x401b0000;
      indirect_print[counter].size   = 0x0e00;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.sim_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x401c0000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x401c0000;
      indirect_print[counter].size   = 0x0054;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(4);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.dpn_tbl";
#endif
      counter++;
    }
    /* Port Queue Size (PQS) */
    if (table_offset == (tables->sch.pqs_tbl.addr.base) || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = (tables->sch.pqs_tbl.addr.base);
      indirect_print[counter].size   = (tables->sch.pqs_tbl.addr.size);
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES((tables->sch.pqs_tbl.addr.width_bits));
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.pqs_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40300000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40300000;
      indirect_print[counter].size   = 0x0400;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flhhr_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40310000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40310000;
      indirect_print[counter].size   = 0x0400;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flthr_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40320000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40320000;
      indirect_print[counter].size   = 0x2000;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flhcl_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40330000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40330000;
      indirect_print[counter].size   = 0x2000;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(64);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fltcl_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40340000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40340000;
      indirect_print[counter].size   = 0x1f00;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.flhfq_tbl";
#endif
      counter++;
    }
    if (table_offset == 0x40350000 || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = 0x40350000;
      indirect_print[counter].size   = 0x1f00;
      indirect_print[counter].wrd_sz = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(16);
      indirect_print[counter].mod_id = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name = "SCH.fltfq_tbl";
#endif
      counter++;
    }


    /* Port Queue Size (PQS) */
    if (table_offset == tables->sch.pqs_tbl.addr.base || table_offset == SOC_SAND_U32_MAX)
    {
      indirect_print[counter].base   = tables->sch.pqs_tbl.addr.base;
      indirect_print[counter].size   = tables->sch.pqs_tbl.addr.size;
      indirect_print[counter].wrd_sz   = SOC_PB_WIDTH_BITS_TO_WIDTH_BYTES(tables->sch.pqs_tbl.addr.width_bits);
      indirect_print[counter].mod_id   = SOC_PB_SCH_ID;
#ifdef SOC_PB_DIAG_DUMP_TBLS_NAME_PRINT
      indirect_print[counter].name   = "SCH.pqs_tbl";
#endif
      counter++;
    }
  }

  *nof_tbls = counter;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_tbls_dump_tables_get()",0,0);
}


/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_diag_sample_enable_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint8                                 enable
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    tmp;
  SOC_PB_PP_REGS
    *pp_regs;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_SAMPLE_ENABLE_SET_UNSAFE);

  tmp = SOC_SAND_BOOL2NUM(enable);
  regs = soc_petra_regs();
  pp_regs = soc_pb_pp_regs();

  /* ihp enable set*/
  SOC_PB_FLD_SET(regs->ihp.ihp_debug_reg.enable,tmp,10,exit);
  /* ihb enable set*/
  SOC_PB_FLD_SET(regs->ihb.ihb_debug_reg.enable,tmp,20,exit);
  /* epni enable set*/
  SOC_PB_PP_FLD_SET(pp_regs->epni.etpp_debug_configuration_reg.debug_wr,tmp,20,exit);
  
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_sample_enable_set_unsafe()", 0, 0);
}

/*********************************************************************
*     Enable/disable diagnostic APIs.affects only APIs with
 *     type: need_sample
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_diag_sample_enable_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint8                                 *enable
  )
{
  SOC_PETRA_REGS
    *regs;
  uint32
    tmp;
  uint32
    res = SOC_SAND_OK;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_SAMPLE_ENABLE_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(enable);

  regs = soc_petra_regs();
  /* ihp enable get*/
  SOC_PB_FLD_GET(regs->ihp.ihp_debug_reg.enable,tmp,10,exit);
  *enable = SOC_SAND_NUM2BOOL(tmp);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_sample_enable_get_unsafe()", 0, 0);
}


#if SOC_PB_DEBUG

void
  soc_pb_diag_sig_val_print(
    SOC_SAND_IN  int          unit,
    SOC_SAND_IN  uint8            block_id,
    SOC_SAND_IN  SOC_PB_DIAG_REG_FIELD  *fld,
    SOC_SAND_IN  uint32           reg_val[SOC_PB_DIAG_DBG_VAL_LEN],
    SOC_SAND_IN  uint32           flags
  )
{
  uint32
    idx,
    nof_vals;

  nof_vals = (block_id == SOC_PB_EPNI_ID)?24:8;

  soc_sand_os_printf("0x%08x:", fld->addr.base);
  for (idx = 0; idx < nof_vals; ++idx)
  {
    if ( idx > 0 && (idx % 5) == 0)
    {
      soc_sand_os_printf("\n\r%11s","");
    }
    soc_sand_os_printf(" 0x%08x", reg_val[idx]);
  }
  soc_sand_os_printf("\n\r");
}

uint32
  soc_pb_diag_signals_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint32   flags
  )
{
  uint32
    reg_val[SOC_PB_DIAG_DBG_VAL_LEN];
  uint32
    i,j,k;
  SOC_PB_DIAG_REG_FIELD
    fld;
  uint32
    regs_val[SOC_PB_DIAG_DBG_VAL_LEN];
  uint8
    samp_enable;
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_SIGNALS_DUMP_UNSAFE);
  
  SOC_PETRA_CLEAR(reg_val,uint32,SOC_PB_DIAG_DBG_VAL_LEN);

 /*
  * check if sampling is enabled/ if not retutn error
  */
  res = soc_pb_diag_sample_enable_get_unsafe(unit,&samp_enable);
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  if (!samp_enable)
  {
    SOC_SAND_SET_ERROR_CODE(SOC_PB_DIAG_SAMPLE_NOT_ENABLED_ERR, 5, exit);
  }
    
  for (i = 0; i < sizeof(Soc_pb_diag_blk_id)/sizeof(uint8); ++i)
  {
    for (j = 0; j <= Soc_pb_diag_blk_msb_max[i]; ++j)
    {
      for (k = 0; k <= Soc_pb_diag_blk_lsb_max[i]; ++k)
      {
        SOC_PB_DIAG_FLD_READ(&fld,Soc_pb_diag_blk_id[i],j,k,SOC_PB_DIAG_BLK_NOF_BITS-1,0,15);

        soc_pb_diag_sig_val_print(unit,Soc_pb_diag_blk_id[i],&fld,regs_val,flags);
      }
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_signals_dump_unsafe()",0,0);
}


uint32
  soc_pb_diag_tbls_dump_print(
    SOC_SAND_IN  int           unit,
    SOC_SAND_IN  uint32            *address_array,
    SOC_SAND_IN  uint32            size_bytes,
    SOC_SAND_IN  uint32           word_size_bytes,
    SOC_SAND_IN  uint32            print_options_bm,
    SOC_SAND_IN  uint32            address_start
  )
{
  SOC_SAND_RET
    ret = SOC_SAND_OK;
  uint32
    word_i,
    entry_i,
    bit_i,
    same_as_prev = FALSE;
  uint32
    nof_entries = size_bytes,
    address,
    zero_word[SOC_PB_DIAG_TBLS_DUMP_MAX_WORD_SIZE];
  const uint32
    *data_next,
    *data_curr;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  ret = SOC_SAND_OK; sal_memset(zero_word, 0x0, SOC_PB_DIAG_TBLS_DUMP_MAX_WORD_SIZE * sizeof(uint32));
  SOC_SAND_CHECK_FUNC_RESULT(ret, 10, exit);

  for(entry_i = 0; entry_i < nof_entries; ++entry_i)
  {
    address = entry_i + address_start;
    data_curr = address_array + (entry_i + 0) * (word_size_bytes >> 2);
    data_next = address_array + (entry_i + 1) * (word_size_bytes >> 2);


    if(print_options_bm & SOC_SAND_PRINT_RANGES)
    {
      if (
          (sal_memcmp(data_curr, data_next, word_size_bytes) != 0) ||
          (entry_i + 1 == nof_entries)
         )
      {
        same_as_prev = FALSE;
      }
      else
      {
        if(!same_as_prev)
        {
          same_as_prev = TRUE;
          if (
              (sal_memcmp(data_curr, zero_word, word_size_bytes) != 0) ||
              (!(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS))
             )
          {
            soc_sand_os_printf("  From 0x%04X To ", address);
          }
        }
        if(entry_i != (nof_entries - 1))
        {
          continue;
        }
      }
    }
    if (
        (sal_memcmp(data_curr, zero_word, word_size_bytes) != 0) ||
        (!(print_options_bm & SOC_SAND_DONT_PRINT_ZEROS))
       )
    {
      soc_sand_os_printf("  0x%04x: ", address);
      for (word_i = 0; word_i < (word_size_bytes / sizeof(uint32)); ++word_i)
      {
        soc_sand_os_printf("0x%08x ", data_curr[word_i]);

        if(print_options_bm & SOC_SAND_PRINT_BITS)
        {
          soc_sand_os_printf(" Bits:");

          for(bit_i = 0; bit_i < SOC_SAND_NOF_BITS_IN_UINT32; ++bit_i)
          {
            if(data_curr[word_i] & SOC_SAND_BIT(bit_i))
            {
              soc_sand_os_printf("%d,", bit_i);
            }
          }
        }
      }
      soc_sand_os_printf("\n\r");
    }
  }

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_tbls_dump_print()",0,0);
}

uint32
  soc_pb_diag_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  uint8    block_id,
    SOC_SAND_IN  uint32   tbl_offset,
    SOC_SAND_IN  uint8  print_zero
  )
{
  uint32
    res = SOC_SAND_OK;
  uint32
    ind,
    print_options_bm = 0,
    tmp_size,
    max_size_bytes = 0,
    counter;
  int32
    curr_tbl_size;
  uint32
    granularity,
    block_num,
    temp_size;
  uint32
    *data = NULL;
  SOC_PB_TBL_PRINT_INFO
    *indirect_print = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_TBLS_DUMP);

  SOC_PETRA_ALLOC(indirect_print, SOC_PB_TBL_PRINT_INFO, SOC_PB_PP_DIAG_MAX_NOF_DUMP_TABLES);

  res = soc_pb_diag_tbls_dump_tables_get(
          indirect_print,
          block_id,
          tbl_offset,
          SOC_PETRA_DIAG_TBLS_DUMP_MODE_SELECTIVE,
          &counter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  print_options_bm |= SOC_SAND_PRINT_RANGES;

  if(!print_zero)
  {
    print_options_bm |= SOC_SAND_DONT_PRINT_ZEROS;
  }

  for (ind = 0; ind < counter; ++ind)
  {
    tmp_size = indirect_print[ind].size * indirect_print[ind].wrd_sz;
    max_size_bytes = tmp_size > max_size_bytes ? tmp_size : max_size_bytes;
  }

  max_size_bytes = (max_size_bytes > SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE ? SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE : max_size_bytes);

  SOC_PETRA_ALLOC(data, uint32, max_size_bytes);

  for (ind = 0; ind < counter; ++ind)
  {
    if (indirect_print[ind].name)
    {
      soc_sand_os_printf("%2d. Table: %s Addr: 0x%08X \n\r", ind, indirect_print[ind].name,indirect_print[ind].base);
    }
    else
    {
      /*print block and address*/
      soc_sand_os_printf("%2d. Block: %s Addr: 0x%08X\n\r", ind, SOC_PB_DIAG_DUMP_TBLS_BLOCK_ID_to_string(indirect_print[ind].mod_id),indirect_print[ind].base);
    }

    curr_tbl_size = indirect_print[ind].size;
    granularity = (SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE / indirect_print[ind].wrd_sz);


    block_num = 0;
    while (curr_tbl_size > 0)
    {
      temp_size = (uint32)(curr_tbl_size > (int32)granularity ? granularity : curr_tbl_size);

      res = soc_sand_tbl_read_unsafe(
              unit,
              data,
              indirect_print[ind].base,
              temp_size * indirect_print[ind].wrd_sz,
              indirect_print[ind].mod_id,
              indirect_print[ind].wrd_sz
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_pb_diag_tbls_dump_print(
              unit,
              data,
              temp_size,
              indirect_print[ind].wrd_sz,
              print_options_bm,
              block_num * granularity
            );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      curr_tbl_size -= granularity;
      indirect_print[ind].base += granularity;
      ++block_num;
    }
  }

exit:
  SOC_PETRA_FREE(indirect_print);
  SOC_PETRA_FREE(data);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_tbls_dump()",0,0);
}

uint32
  soc_pb_diag_dev_tbls_dump_unsafe(
    SOC_SAND_IN  int  unit,
    SOC_SAND_IN  SOC_PETRA_DIAG_TBLS_DUMP_MODE dump_mode
  )
{

  uint8
    block_id = SOC_PETRA_DIAG_TBLS_DUMP_ALL_BLOCKS;
  uint32
    tbl_offset = SOC_PETRA_DIAG_TBLS_DUMP_ALL_TBLS;
  uint8
    print_zero = TRUE;
  uint32
    res = SOC_SAND_OK;
  uint32
    ind,
    print_options_bm = 0,
    tmp_size,
    max_size_bytes = 0,
    counter;
  int32
    curr_tbl_size;
   uint32
    granularity,
      block_num,
      temp_size;
  

  uint32
    *data = NULL;
  SOC_PB_TBL_PRINT_INFO
    *indirect_print = NULL;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_DEV_TBLS_DUMP);

  SOC_PETRA_ALLOC(indirect_print, SOC_PB_TBL_PRINT_INFO, SOC_PB_PP_DIAG_MAX_NOF_DUMP_TABLES);

  res = soc_pb_diag_tbls_dump_tables_get(
          indirect_print,
          block_id,
          tbl_offset,
          dump_mode,
          &counter
        );
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  print_options_bm |= SOC_SAND_PRINT_RANGES;

  if(!print_zero)
  {
    /* Petra b code. Almost not in use. Ignore coverity defects */
    /* coverity[dead_error_line] */
    print_options_bm |= SOC_SAND_DONT_PRINT_ZEROS;
  }

  for (ind = 0; ind < counter; ++ind)
  {
    tmp_size = indirect_print[ind].size * indirect_print[ind].wrd_sz;
    max_size_bytes = tmp_size > max_size_bytes ? tmp_size : max_size_bytes;
  }

  max_size_bytes = (max_size_bytes > SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE ? SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE : max_size_bytes);

  SOC_PETRA_ALLOC(data, uint32, max_size_bytes);

  for (ind = 0; ind < counter; ++ind)
  {
    soc_sand_os_printf("%2d. Block: %s Addr: 0x%08X\n\r", ind, SOC_PB_DIAG_DUMP_TBLS_BLOCK_ID_to_string(indirect_print[ind].mod_id),indirect_print[ind].base);
    if (indirect_print[ind].name)
    {
        soc_sand_os_printf("%2d. Table: %s Addr: 0x%08X \n\r", ind, indirect_print[ind].name,indirect_print[ind].base);
    }
    else
    {
      /*print block and address*/
      soc_sand_os_printf("%2d. Block: %s Addr: 0x%08X\n\r", ind, SOC_PB_DIAG_DUMP_TBLS_BLOCK_ID_to_string(indirect_print[ind].mod_id),indirect_print[ind].base);
    }


    curr_tbl_size = indirect_print[ind].size;
    granularity = (SOC_PB_DIAG_TBLS_DUMP_MAX_DUMP_TBL_SIZE / indirect_print[ind].wrd_sz);


    block_num = 0;
    while (curr_tbl_size > 0)
    {
      temp_size = (uint32)(curr_tbl_size > (int32)granularity ? granularity : curr_tbl_size);

      res = soc_sand_tbl_read_unsafe(
        unit,
        data,
        indirect_print[ind].base,
        temp_size * indirect_print[ind].wrd_sz,
        indirect_print[ind].mod_id,
        indirect_print[ind].wrd_sz
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 20, exit);

      res = soc_pb_diag_tbls_dump_print(
        unit,
        data,
        temp_size,
        indirect_print[ind].wrd_sz,
        print_options_bm,
        block_num * granularity
        );
      SOC_SAND_CHECK_FUNC_RESULT(res, 30, exit);

      curr_tbl_size -= granularity;
      indirect_print[ind].base += granularity;
      ++block_num;
    }
  }

exit:
  SOC_PETRA_FREE(indirect_print);
  SOC_PETRA_FREE(data);

  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_tbls_dump()",0,0);
}


const char*
  SOC_PB_DIAG_DUMP_TBLS_BLOCK_ID_to_string(
    SOC_SAND_IN uint32 block_id
  )
{
  const char* str = NULL;
  switch(block_id)
  {
  case SOC_PB_OLP_ID:
    str = "OLP";
    break;
  case SOC_PB_IRE_ID:
    str = "IRE";
    break;
  case SOC_PB_IDR_ID:
    str = "IDR";
    break;
  case SOC_PB_IRR_ID:
    str = "IRR";
    break;
  case SOC_PB_IHP_ID:
    str = "IHP";
    break;
  case SOC_PB_QDR_ID:
    str = "QDR";
    break;
  case SOC_PB_IPS_ID:
    str = "IPS";
    break;
  case SOC_PB_IPT_ID:
    str = "IPT";
    break;
  case SOC_PB_DPI_A_ID:
    str = "DPIA";
    break;
  case SOC_PB_DPI_B_ID:
    str = "DPIB";
    break;
  case SOC_PB_DPI_C_ID:
    str = "DPIC";
    break;
  case SOC_PB_DPI_D_ID:
    str = "DPID";
    break;
  case SOC_PB_DPI_E_ID:
    str = "DPIE";
    break;
  case SOC_PB_DPI_F_ID:
    str = "DPIF";
    break;
  case SOC_PB_RTP_ID:
    str = "RTP";
    break;
  case SOC_PB_EGQ_ID:
    str = "EGQ";
    break;
  case SOC_PB_SCH_ID:
    str = "SCH";
    break;
  case SOC_PB_CFC_ID:
    str = "CFC";
    break;
  case SOC_PB_EPNI_ID:
    str = "EPN";
    break;
  case SOC_PB_IQM_ID:
    str = "IQM";
    break;
  case SOC_PB_MMU_ID:
    str = "MMU";
    break;
  case SOC_PB_IHB_ID:
    str = "IHB";
    break;
  case SOC_PB_FDT_ID:
    str = "FDT";
    break;
  case SOC_PB_NBI_ID:
    str = "NBI";
    break;

  default:
    str = "Unknown";
  }
  return str;
}

#endif /* SOC_PB_DEBUG */

/* } */

/*********************************************************************
*     Returns diagnostics information regarding the last
 *     packet: the incoming TM port and the corresponding PP
 *     port, port header processing type, packet headers and
 *     payload (first 128 Bytes). In case of TM port, the ITMH,
 *     which is part of that buffer, is parsed.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_diag_last_packet_info_get_unsafe(
    SOC_SAND_IN  int                unit,
    SOC_SAND_OUT SOC_PB_DIAG_LAST_PACKET_INFO *last_packet
  )
{
  uint32
    port_ndx,
    small_buff_ndx,
    port_context,
    reg_ndx,
  
    fld_val2,
    fld_val3,
    fld_val4,
    res = SOC_SAND_OK;
  SOC_PB_REGS
    *soc_pb_regs_l;
  SOC_PB_PP_REGS
    *soc_pb_pp_regs_l;
  uint8
    buffer_temp [4];
  SOC_PETRA_PORT_HEADER_TYPE  header_type_incoming;
 
  SOC_PETRA_PORTS_ITMH        last_incoming_header;


  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LAST_PACKET_INFO_GET_UNSAFE);

  SOC_SAND_CHECK_NULL_INPUT(last_packet);

  SOC_PB_DIAG_LAST_PACKET_INFO_clear(last_packet);

  soc_pb_regs_l = soc_pb_regs();

  soc_pb_pp_regs_l = soc_pb_pp_regs();



 for (reg_ndx = 0; reg_ndx < SOC_PB_NOF_DEBUG_HEADER_REGS; reg_ndx++)
 {
   SOC_PB_PP_REG_GET(soc_pb_regs_l->ihp.debug_header_reg[reg_ndx], fld_val2, 10 ,exit);
   soc_sand_U32_to_U8(&fld_val2, 4,buffer_temp);
   for (small_buff_ndx = 0; small_buff_ndx < 4; small_buff_ndx++)
   {
    last_packet->buffer[reg_ndx*4+small_buff_ndx] = buffer_temp[small_buff_ndx];
     
   }
 }

 SOC_PB_PP_FLD_GET(soc_pb_pp_regs_l->ihp.last_generated_values_reg.last_pp_port,fld_val3 ,30 ,exit);
 last_packet->pp_port = fld_val3;


 SOC_PB_PP_FLD_GET(soc_pb_pp_regs_l->ihp.last_generated_values_reg.last_pp_context,fld_val2 ,30 ,exit);
 port_context = fld_val2;

  SOC_PB_PP_FLD_GET(soc_pb_regs_l->ihp.debug_port_type_reg.last_rcvd_tm_port,fld_val4 ,20 ,exit);
  last_packet->tm_port = fld_val4;

  port_ndx = fld_val4;


 res = soc_pb_port_parse_header_type_unsafe(
         unit,
         port_ndx,
         port_context,
         &header_type_incoming
       );

 SOC_SAND_CHECK_FUNC_RESULT(res, 40, exit);

 last_packet->port_header_type = header_type_incoming;

 if (header_type_incoming == SOC_PETRA_PORT_HEADER_TYPE_TM)
 {
   res = soc_petra_nif_diag_last_packet_get(
           unit,
           &last_incoming_header
         );
   SOC_SAND_CHECK_FUNC_RESULT(res, 110, exit);

   last_packet->itmh = last_incoming_header;
 }

  


   

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_last_packet_info_get_unsafe()", 0, 0);
}

uint32
  soc_pb_diag_last_packet_info_get_verify(
    SOC_SAND_IN  int                unit
  )
{
  uint32
    res = SOC_SAND_OK;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_DIAG_LAST_PACKET_INFO_GET_VERIFY);


  SOC_SAND_TODO_IMPLEMENT_WARNING;
  SOC_PB_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in soc_pb_diag_last_packet_info_get_verify()", 0, 0);
}

/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_pb_api_diagnostics module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_PROCEDURE_DESC_ELEMENT*
  soc_pb_diagnostics_get_procs_ptr(void)
{
  return Soc_pb_procedure_desc_element_diagnostics;
}
/*********************************************************************
*     Get the pointer to the list of errors of the
 *     soc_pb_api_diagnostics module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
SOC_ERROR_DESC_ELEMENT*
  soc_pb_diagnostics_get_errs_ptr(void)
{
  return Soc_pb_error_desc_element_diagnostics;
}
uint32
  SOC_PB_DIAG_LAST_PACKET_INFO_verify(
    SOC_SAND_IN  SOC_PB_DIAG_LAST_PACKET_INFO *info
  )
{
  uint32
    ind;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);
  SOC_SAND_CHECK_NULL_INPUT(info);

  SOC_SAND_ERR_IF_ABOVE_MAX(info->tm_port, SOC_PB_DIAGNOSTICS_TM_PORT_MAX, SOC_PB_DIAGNOSTICS_TM_PORT_OUT_OF_RANGE_ERR, 10, exit);
  SOC_SAND_ERR_IF_ABOVE_MAX(info->pp_port, SOC_PB_DIAGNOSTICS_PP_PORT_MAX, SOC_PB_DIAGNOSTICS_PP_PORT_OUT_OF_RANGE_ERR, 11, exit);
  for (ind = 0; ind < SOC_PB_DIAG_LAST_PCKT_SNAPSHOT_LEN_BYTES; ++ind)
  {
  }

  SOC_SAND_MAGIC_NUM_VERIFY(info);

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR("error in SOC_PB_DIAG_LAST_PACKET_INFO_verify()",0,0);
}

uint32
    soc_pb_diag_simple_mbist_sms_type_get(
      SOC_SAND_IN int                     unit,
      SOC_SAND_IN uint32                     sms_ndx,
      SOC_SAND_OUT SOC_PETRA_DIAG_MBIST_SMS_TYPE    *sms_tp
    )
{
  SOC_PETRA_DIAG_MBIST_SMS_TYPE
    sms_type;
  
  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  if (
      ((sms_ndx >= 1) && (sms_ndx <= 6))
      || (sms_ndx == 9)
      || (sms_ndx == 11)
      || (sms_ndx == 13)
      || (sms_ndx == 16)
      || (sms_ndx == 18)
      || (sms_ndx == 20)
      || (sms_ndx == 21)
      || (sms_ndx == 24)
      || (sms_ndx == 27)
      || ((sms_ndx >= 30) && (sms_ndx <= 32))
      || (sms_ndx == 34)
      || (sms_ndx == 36)
      || (sms_ndx == 39)
      || (sms_ndx == 42)
      || (sms_ndx == 43)
    )
  {
    sms_type = SOC_PETRA_DIAG_MBIST_SMS_TYPE_RF;
  }
  else if (
          (sms_ndx == 7)
          || (sms_ndx == 12)
          || (sms_ndx == 14)
          || (sms_ndx == 22)
          || (sms_ndx == 25)
          || (sms_ndx == 35)
          || (sms_ndx == 37)
          || (sms_ndx == 40)
    )
  {
    sms_type = SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_D;
  }
  else
  {
    sms_type = SOC_PETRA_DIAG_MBIST_SMS_TYPE_512_S;
  }

  *sms_tp = sms_type;

  SOC_PETRA_DO_NOTHING_AND_EXIT;
exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_simple_mbist_sms_type_get()",sms_ndx,0);
}


uint32
  soc_pb_diag_simple_mbist_sms_sel_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *sel_sz
  )
{
  uint32
    sel_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (sms_ndx)
  {
  case 1  : sel_size = 27   ;break;
  case 2  : sel_size = 27   ;break;
  case 3  : sel_size = 27   ;break;
  case 4  : sel_size = 27   ;break;
  case 5  : sel_size = 27   ;break;
  case 6  : sel_size = 27   ;break;
  case 7  : sel_size = 42   ;break;
  case 8  : sel_size = 2888 ;break;
  case 9  : sel_size = 85   ;break;
  case 10 : sel_size = 352  ;break;
  case 11 : sel_size = 62   ;break;
  case 12 : sel_size = 2412 ;break;
  case 13 : sel_size = 60   ;break;
  case 14 : sel_size = 42   ;break;
  case 15 : sel_size = 1542 ;break;
  case 16 : sel_size = 53   ;break;
  case 17 : sel_size = 2312 ;break;
  case 18 : sel_size = 57   ;break;
  case 19 : sel_size = 67   ;break;
  case 20 : sel_size = 41   ;break;
  case 21 : sel_size = 30   ;break;
  case 22 : sel_size = 904  ;break;
  case 23 : sel_size = 954  ;break;
  case 24 : sel_size = 77   ;break;
  case 25 : sel_size = 1213 ;break;
  case 26 : sel_size = 1043 ;break;
  case 27 : sel_size = 58   ;break;
  case 28 : sel_size = 52   ;break;
  case 29 : sel_size = 52   ;break;
  case 30 : sel_size = 44   ;break;
  case 31 : sel_size = 44   ;break;
  case 32 : sel_size = 182  ;break;
  case 33 : sel_size = 70   ;break;
  case 34 : sel_size = 29   ;break;
  case 35 : sel_size = 79   ;break;
  case 36 : sel_size = 228  ;break;
  case 37 : sel_size = 756  ;break;
  case 38 : sel_size = 348  ;break;
  case 39 : sel_size = 38   ;break;
  case 40 : sel_size = 265  ;break;
  case 41 : sel_size = 2219 ;break;
  case 42 : sel_size = 37   ;break;
    
 default:
    sel_size = 0;
  }

  *sel_sz = sel_size;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_simple_mbist_sms_sel_size_get()",sms_ndx,0);
}

uint32
  soc_pb_diag_simple_mbist_sms_addr_size_get(
    SOC_SAND_IN  int                     unit,
    SOC_SAND_IN  uint32                     sms_ndx,
    SOC_SAND_OUT uint32                      *addr_sz
  )
{
  uint32
    addr_size;

  SOC_SAND_INIT_ERROR_DEFINITIONS(0);

  switch (sms_ndx)
  {
  case 1  : addr_size = 7   ; break;
  case 2  : addr_size = 7   ; break;
  case 3  : addr_size = 7   ; break;
  case 4  : addr_size = 7   ; break;
  case 5  : addr_size = 7   ; break;
  case 6  : addr_size = 7   ; break;
  case 7  : addr_size = 13  ; break;
  case 8  : addr_size = 13  ; break;
  case 9  : addr_size = 10  ; break;
  case 10 : addr_size = 14  ; break;
  case 11 : addr_size = 7   ; break;
  case 12 : addr_size = 11  ; break;
  case 13 : addr_size = 8   ; break;
  case 14 : addr_size = 11  ; break;
  case 15 : addr_size = 14  ; break;
  case 16 : addr_size = 9   ; break;
  case 17 : addr_size = 14  ; break;
  case 18 : addr_size = 9   ; break;
  case 19 : addr_size = 10  ; break;
  case 20 : addr_size = 9   ; break;
  case 21 : addr_size = 6   ; break;
  case 22 : addr_size = 13  ; break;
  case 23 : addr_size = 12  ; break;
  case 24 : addr_size = 9   ; break;
  case 25 : addr_size = 13  ; break;
  case 26 : addr_size = 13  ; break;
  case 27 : addr_size = 9   ; break;
  case 28 : addr_size = 13  ; break;
  case 29 : addr_size = 13  ; break;
  case 30 : addr_size = 8   ; break;
  case 31 : addr_size = 8   ; break;
  case 32 : addr_size = 10  ; break;
  case 33 : addr_size = 13  ; break;
  case 34 : addr_size = 8   ; break;
  case 35 : addr_size = 10  ; break;
  case 36 : addr_size = 7   ; break;
  case 37 : addr_size = 13  ; break;
  case 38 : addr_size = 13  ; break;
  case 39 : addr_size = 9   ; break;
  case 40 : addr_size = 12  ; break;
  case 41 : addr_size = 13  ; break;
  case 42 : addr_size = 8   ; break;

  default:
    addr_size = 0;
  }

  *addr_sz = addr_size;
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_diag_simple_mbist_sms_addr_size_get()",sms_ndx,0);
}


#include <soc/dpp/SAND/Utils/sand_footer.h>


