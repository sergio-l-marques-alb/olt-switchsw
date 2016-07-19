/* $Id: ppd_api_framework.c,v 1.16 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppd/src/soc_ppd_api_framework.c
*
* MODULE PREFIX:  soc_ppd_api_framework
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


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_PPD

#include <shared/bsl.h>
#include <soc/dpp/drv.h>

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dcmn/error.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/PPD/ppd_api_framework.h>

#include <soc/dpp/PPD/ppd_api_general.h>
#include <soc/dpp/PPD/ppd_api_llp_parse.h>
#include <soc/dpp/PPD/ppd_api_llp_sa_auth.h>
#include <soc/dpp/PPD/ppd_api_llp_vid_assign.h>
#include <soc/dpp/PPD/ppd_api_llp_filter.h>
#include <soc/dpp/PPD/ppd_api_llp_cos.h>
#include <soc/dpp/PPD/ppd_api_llp_trap.h>
#include <soc/dpp/PPD/ppd_api_llp_mirror.h>
#include <soc/dpp/PPD/ppd_api_mymac.h>
#include <soc/dpp/PPD/ppd_api_mpls_term.h>
#include <soc/dpp/PPD/ppd_api_vsi.h>
#include <soc/dpp/PPD/ppd_api_lif.h>
#include <soc/dpp/PPD/ppd_api_rif.h>
#include <soc/dpp/PPD/ppd_api_lif_ing_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_lif_cos.h>
#include <soc/dpp/PPD/ppd_api_lif_table.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_mact_mgmt.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv4.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ipv6.h>
#include <soc/dpp/PPD/ppd_api_frwrd_ilm.h>
#include <soc/dpp/PPD/ppd_api_frwrd_bmact.h>
#include <soc/dpp/PPD/ppd_api_frwrd_trill.h>
#include <soc/dpp/PPD/ppd_api_frwrd_extend_p2p.h>
#include <soc/dpp/PPD/ppd_api_frwrd_fec.h>
#include <soc/dpp/PPD/ppd_api_eg_filter.h>
#include <soc/dpp/PPD/ppd_api_eg_qos.h>
#include <soc/dpp/PPD/ppd_api_eg_encap.h>
#include <soc/dpp/PPD/ppd_api_eg_ac.h>
#include <soc/dpp/PPD/ppd_api_eg_vlan_edit.h>
#include <soc/dpp/PPD/ppd_api_eg_mirror.h>
#include <soc/dpp/PPD/ppd_api_port.h>
#include <soc/dpp/PPD/ppd_api_lag.h>
#include <soc/dpp/PPD/ppd_api_trap_mgmt.h>
#include <soc/dpp/PPD/ppd_api_metering.h>
#include <soc/dpp/PPD/ppd_api_fp.h>
#include <soc/dpp/PPD/ppd_api_diag.h>

#include <soc/dpp/PPD/ppd_api_oam.h>
#include <soc/dpp/PPD/ppd_api_l3_src_bind.h>
#include <soc/dpp/PPD/ppd_api_ptp.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */


#define SOC_PPD_PROCS_DESC_VALIDATE(module, err_num) \
  do { \
  if (SOC_PPD_PROC_DESC_BASE_##module##_LAST <= SOC_PPD_##module##_PROCEDURE_DESC_LAST) \
    { \
    SOC_SAND_SET_ERROR_CODE(SOC_SAND_ERRORS_FOUND_DUPLICATES_ERR, err_num, exit); \
    } \
  } while (0)

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

CONST STATIC
  SOC_PROCEDURE_DESC_ELEMENT
    Ppd_procedure_desc_element_framework[] =
{
  /*
   * Auto generated. Do not edit following section {
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_PROCEDURE_DESC_ADD),
  SOC_PROCEDURE_DESC_ELEMENT_DEF(SOC_PPD_FRAMEWORK_GET_PROCS_PTR),
  /*
   * } Auto generated. Do not edit previous section.
   */



  /*
   * Last element. Do no touch.
   */
  SOC_PROCEDURE_DESC_ELEMENT_DEF_LAST
};

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
  soc_ppd_procedure_desc_add(
  )
{
  uint32
    res;
  CONST SOC_PROCEDURE_DESC_ELEMENT
    *procedure_desc[100];
  int32
    nof_modules = 0;
  static uint8
    Ppd_proc_desc_initialized = FALSE;

  SOC_SAND_INIT_ERROR_DEFINITIONS_NO_DEVID(SOC_PPD_PROCEDURE_DESC_ADD);

  if (Ppd_proc_desc_initialized == TRUE)
  {
	  SOC_PPD_DO_NOTHING_AND_EXIT;
  }

  SOC_PPD_PROCS_DESC_VALIDATE(GENERAL, 10);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_PARSE, 20);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_SA_AUTH, 30);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_VID_ASSIGN, 40);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_FILTER, 50);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_COS, 60);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_TRAP, 70);
  SOC_PPD_PROCS_DESC_VALIDATE(LLP_MIRROR, 80);
  SOC_PPD_PROCS_DESC_VALIDATE(MYMAC, 90);
  SOC_PPD_PROCS_DESC_VALIDATE(MPLS_TERM, 100);
  SOC_PPD_PROCS_DESC_VALIDATE(VSI, 110);
  SOC_PPD_PROCS_DESC_VALIDATE(LIF, 120);
  SOC_PPD_PROCS_DESC_VALIDATE(RIF, 130);
  SOC_PPD_PROCS_DESC_VALIDATE(LIF_ING_VLAN_EDIT, 140);
  SOC_PPD_PROCS_DESC_VALIDATE(LIF_COS, 150);
  SOC_PPD_PROCS_DESC_VALIDATE(LIF_TABLE, 160);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_MACT, 170);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_MACT_MGMT, 180);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_IPV4, 190);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_IPV6, 200);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_ILM, 210);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_BMACT, 220);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_TRILL, 230);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_EXTEND_P2P, 240);
  SOC_PPD_PROCS_DESC_VALIDATE(FRWRD_FEC, 250);
  SOC_PPD_PROCS_DESC_VALIDATE(EG_FILTER, 260);
  SOC_PPD_PROCS_DESC_VALIDATE(EG_QOS, 270);
  SOC_PPD_PROCS_DESC_VALIDATE(EG_ENCAP, 280);
  SOC_PPD_PROCS_DESC_VALIDATE(EG_AC, 290);
  SOC_PPD_PROCS_DESC_VALIDATE(EG_VLAN_EDIT, 300);
  SOC_PPD_PROCS_DESC_VALIDATE(EG_MIRROR, 310);
  SOC_PPD_PROCS_DESC_VALIDATE(PORT, 320);
  SOC_PPD_PROCS_DESC_VALIDATE(LAG, 330);
  SOC_PPD_PROCS_DESC_VALIDATE(TRAP_MGMT, 340);
  SOC_PPD_PROCS_DESC_VALIDATE(METERING, 350);
  SOC_PPD_PROCS_DESC_VALIDATE(FP, 380);
  SOC_PPD_PROCS_DESC_VALIDATE(DIAG, 390);
  SOC_PPD_PROCS_DESC_VALIDATE(SRC_BIND, 410);
  SOC_PPD_PROCS_DESC_VALIDATE(OAM, 420);
  SOC_PPD_PROCS_DESC_VALIDATE(PTP, 430);

  procedure_desc[nof_modules++] = soc_ppd_general_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_parse_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_sa_auth_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_vid_assign_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_filter_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_cos_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_trap_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_llp_mirror_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_mymac_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_mpls_term_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_vsi_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_lif_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_rif_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_lif_ing_vlan_edit_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_lif_cos_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_lif_table_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_mact_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_mact_mgmt_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_ipv4_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_ipv6_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_ilm_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_bmact_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_trill_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_extend_p2p_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_frwrd_fec_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_eg_filter_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_eg_qos_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_eg_encap_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_eg_ac_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_eg_vlan_edit_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_eg_mirror_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_port_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_lag_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_trap_mgmt_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_metering_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_fp_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_diag_get_procs_ptr();

  procedure_desc[nof_modules++] = soc_ppd_oam_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_src_bind_get_procs_ptr();
  procedure_desc[nof_modules++] = soc_ppd_ptp_get_procs_ptr();

  res = soc_sand_add_proc_id_pools(procedure_desc, nof_modules) ;
  SOC_SAND_CHECK_FUNC_RESULT(res, 10, exit);

  Ppd_proc_desc_initialized = TRUE;

exit:
  SOC_SAND_EXIT_AND_SEND_ERROR( "error in soc_ppd_procedure_desc_add()",0,0);

}
/*********************************************************************
*     Get the pointer to the list of procedures of the
 *     soc_ppd_api_framework module.
 *     Details: in the H file. (search for prototype)
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  soc_ppd_framework_get_procs_ptr(
  )
{
  return Ppd_procedure_desc_element_framework;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

