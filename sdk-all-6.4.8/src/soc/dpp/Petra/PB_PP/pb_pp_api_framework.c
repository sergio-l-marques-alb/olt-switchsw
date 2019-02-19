/* $Id: pb_pp_api_framework.c,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/Soc_petra/SOC_PB_PP/src/soc_pb_pp_api_framework.c
*
* MODULE PREFIX:  soc_pb_pp
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

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>
#include <soc/dpp/SAND/Utils/sand_os_interface.h>
#include <soc/dpp/SAND/Management/sand_chip_descriptors.h>

#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_api_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_framework.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_general.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_parse.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_sa_auth.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_vid_assign.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_trap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_llp_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mymac.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mpls_term.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_vsi.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_rif.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_ing_vlan_edit.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_cos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lif_table.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_mact_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv4.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ipv6.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_ilm.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_bmact.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_trill.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_extend_p2p.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_frwrd_fec.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_filter.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_qos.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_encap.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_ac.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_vlan_edit.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_eg_mirror.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_port.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_trap_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_metering.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_fp.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_diag.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#include <soc/dpp/Petra/PB_TM/pb_pp_tbl_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_lem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_isem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_svem_access.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_init.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_mgmt.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_ssr.h>

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

#define SOC_PB_PP_ERRORS_DESC_VALIDATE(module, err_num) \
  do { \
    if (SOC_PB_PP_ERR_DESC_BASE_##module##_LAST <= SOC_PB_PP_##module##_ERR_LAST) \
    { \
      SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)

#define SOC_PB_PP_PROCS_DESC_VALIDATE(module, err_num) \
  do { \
  if (SOC_PB_PP_PROC_DESC_BASE_##module##_LAST <= SOC_PB_PP_##module##_PROCEDURE_DESC_LAST) \
    { \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_PROCS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)

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

static uint8 Soc_pb_pp_error_pool_initialized = FALSE;
static uint8 Soc_pb_pp_proc_desc_initialized  = FALSE;

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/*********************************************************************
*     Add the pool of SOC_SAND_FAP20V procedure descriptors to the
 *     all-system sorted pool.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_procedure_desc_add(void)
{
  uint32
    res;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc[100];
  int32
    nof_modules = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_PROCEDURE_DESC_ADD);

  if (Soc_pb_pp_proc_desc_initialized == TRUE)
  {
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  SOC_PB_PP_PROCS_DESC_VALIDATE(GENERAL, 10);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_PARSE, 20);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_SA_AUTH, 30);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_VID_ASSIGN, 40);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_FILTER, 50);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_COS, 60);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_TRAP, 70);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LLP_MIRROR, 80);
  SOC_PB_PP_PROCS_DESC_VALIDATE(MYMAC, 90);
  SOC_PB_PP_PROCS_DESC_VALIDATE(MPLS_TERM, 100);
  SOC_PB_PP_PROCS_DESC_VALIDATE(VSI, 110);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LIF, 120);
  SOC_PB_PP_PROCS_DESC_VALIDATE(RIF, 130);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LIF_ING_VLAN_EDIT, 140);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LIF_COS, 150);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LIF_TABLE, 160);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_MACT, 170);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_MACT_MGMT, 180);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_IPV4, 190);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_IPV6, 200);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_ILM, 210);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_BMACT, 220);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_TRILL, 230);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_EXTEND_P2P, 240);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FRWRD_FEC, 250);
  SOC_PB_PP_PROCS_DESC_VALIDATE(EG_FILTER, 260);
  SOC_PB_PP_PROCS_DESC_VALIDATE(EG_QOS, 280);
  SOC_PB_PP_PROCS_DESC_VALIDATE(EG_ENCAP, 290);
  SOC_PB_PP_PROCS_DESC_VALIDATE(EG_AC, 300);
  SOC_PB_PP_PROCS_DESC_VALIDATE(EG_VLAN_EDIT, 310);
  SOC_PB_PP_PROCS_DESC_VALIDATE(EG_MIRROR, 320);
  SOC_PB_PP_PROCS_DESC_VALIDATE(PORT, 330);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LAG, 340);
  SOC_PB_PP_PROCS_DESC_VALIDATE(TRAP_MGMT, 350);
  SOC_PB_PP_PROCS_DESC_VALIDATE(METERING, 360);
  SOC_PB_PP_PROCS_DESC_VALIDATE(FP, 380);
  SOC_PB_PP_PROCS_DESC_VALIDATE(DIAG, 390);
  SOC_PB_PP_PROCS_DESC_VALIDATE(LEM_ACCESS, 400);
  SOC_PB_PP_PROCS_DESC_VALIDATE(ISEM_ACCESS, 410);
  SOC_PB_PP_PROCS_DESC_VALIDATE(SVEM_ACCESS, 420);
  SOC_PB_PP_PROCS_DESC_VALIDATE(INIT, 430);
  SOC_PB_PP_PROCS_DESC_VALIDATE(MGMT, 440);
  SOC_PB_PP_PROCS_DESC_VALIDATE(SW_DB, 450);
  SOC_PB_PP_PROCS_DESC_VALIDATE(SSR, 460);

  procedure_desc[nof_modules++] = soc_pb_pp_general_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_parse_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_sa_auth_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_vid_assign_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_filter_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_cos_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_trap_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_llp_mirror_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_mymac_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_mpls_term_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_vsi_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_lif_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_rif_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_lif_ing_vlan_edit_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_lif_cos_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_lif_table_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_mact_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_mact_mgmt_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_ipv4_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_ipv6_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_ilm_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_bmact_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_trill_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_extend_p2p_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_frwrd_fec_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_eg_filter_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_eg_qos_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_eg_encap_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_eg_ac_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_eg_vlan_edit_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_eg_mirror_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_port_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_lag_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_trap_mgmt_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_metering_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_fp_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_diag_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_sw_db_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_lem_access_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_isem_access_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_svem_access_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_init_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_mgmt_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_pb_pp_ssr_get_procs_ptr();

  res = soc_sand_add_proc_id_pools(procedure_desc, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  Soc_pb_pp_proc_desc_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_procedure_desc_add()",0,0);

}
/*********************************************************************
*     Add the pool of error descriptors to the all-system
 *     sorted pool.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
uint32
  soc_pb_pp_errors_desc_add(void)
{
  uint32
    res;
  CONST SOC_ERROR_DESC_ELEMENT
    *error_desc[100];
  int32
    nof_modules = 0;

  SOC_SAND_INIT_ERROR_DEFINITIONS(SOC_PB_PP_ERRORS_DESC_ADD);

  if (Soc_pb_pp_error_pool_initialized == TRUE)
  {
    SOC_PB_PP_DO_NOTHING_AND_EXIT;
  }

  SOC_PB_PP_ERRORS_DESC_VALIDATE(GENERAL, 10);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_PARSE, 20);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_SA_AUTH, 30);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_VID_ASSIGN, 40);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_FILTER, 50);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_COS, 60);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_TRAP, 70);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LLP_MIRROR, 80);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(MYMAC, 90);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(MPLS_TERM, 100);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(VSI, 110);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LIF, 120);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(RIF, 130);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LIF_ING_VLAN_EDIT, 140);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LIF_COS, 150);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LIF_TABLE, 160);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_MACT, 170);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_MACT, 180);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_IPV4, 190);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_IPV6, 200);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_ILM, 210);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_BMACT, 220);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_TRILL, 230);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_EXTEND_P2P, 240);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FRWRD_FEC, 250);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(EG_FILTER, 260);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(EG_QOS, 280);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(EG_ENCAP, 290);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(EG_AC, 300);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(EG_VLAN_EDIT, 310);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(EG_MIRROR, 320);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(PORT, 330);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LAG, 340);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(TRAP_MGMT, 350);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(METERING, 360);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(FP, 380);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(DIAG, 390);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(LEM_ACCESS, 400);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(ISEM_ACCESS, 410);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(SVEM_ACCESS, 420);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(INIT, 430);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(MGMT, 440);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(SW_DB, 450);
  SOC_PB_PP_ERRORS_DESC_VALIDATE(SSR, 460);

  error_desc[nof_modules++] = soc_pb_pp_general_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_parse_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_sa_auth_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_vid_assign_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_filter_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_cos_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_trap_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_llp_mirror_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_mymac_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_mpls_term_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_vsi_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_lif_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_rif_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_lif_ing_vlan_edit_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_lif_cos_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_lif_table_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_mact_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_mact_mgmt_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_ipv4_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_ipv6_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_ilm_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_bmact_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_trill_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_extend_p2p_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_frwrd_fec_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_eg_filter_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_eg_qos_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_eg_encap_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_eg_ac_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_eg_vlan_edit_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_eg_mirror_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_port_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_lag_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_trap_mgmt_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_metering_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_fp_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_diag_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_sw_db_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_lem_access_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_isem_access_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_svem_access_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_init_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_mgmt_get_errs_ptr();
  error_desc[nof_modules++] = soc_pb_pp_ssr_get_errs_ptr();

  res = soc_sand_add_error_pools(error_desc, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  Soc_pb_pp_error_pool_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_pb_pp_errors_desc_add()",0,0);

}
#if SOC_PB_PP_DEBUG_IS_LVL1

#endif /* SOC_PB_PP_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

