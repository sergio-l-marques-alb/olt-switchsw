/* $Id: ppc_api_frwrd_extend_p2p.h,v 1.6 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_frwrd_extend_p2p.h
*
* MODULE PREFIX:  soc_ppc_frwrd
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

#ifndef __SOC_PPC_API_FRWRD_EXTEND_P2P_INCLUDED__
/* { */
#define __SOC_PPC_API_FRWRD_EXTEND_P2P_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/PPC/ppc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

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
   *  System VSI to be used for all P2P services. In Soc_petra,
   *  the local VSI is also needed, and it is derived from the
   *  mapping system VSI to local VSI.
   */
  SOC_PPC_SYS_VSI_ID system_vsi;

} SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  This will enable the P2P in the device. There is also
   *  per out-port/in-port enable for the P2P service. See
   *  soc_ppd_port_info_set().
   *  Soc_petra-B: hast to be true.
   */
  uint8 enable;
  /*
   *  Global information for P2P services coming from the
   *  access side.
   */
  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO ac_info;
  /*
   *  Global information for P2P services coming from the Core
   *  Side.
   */
  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO pwe_info;

} SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Forwarding Decision
   */
  SOC_PPC_FRWRD_DECISION_INFO forward_decision;
  /*
   *  Has Control-word. Relevant only for PWE P2P (when the
   *  key is VC label) used by the PWE termination to
   *  determine number of bytes to remove from the header.
   */
  uint8 has_cw;
  /*
   *  Selects one of the TPID profiles defined by
   *  soc_ppd_frwrd_extend_p2p_glbl_info_set Used to perform the
   *  egress parsing of the Link Layer header.
   */
  uint32 tpid_profile;

} SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Incoming label.
   *  for Soc_petra-B: only in_label is relevant other fields are ignored and should be set to 0
   *  by calling SOC_PPD_FRWRD_EXTEND_PWE_KEY_clear()
   */
  SOC_SAND_PP_MPLS_LABEL in_label;
  /*
   *  Internal EXP. Relevant only for labels in the ELSP
   *  range. In this case, this is the value after mapping the
   *  header EXP with 'exp_map_tbl table'. Otherwise this is
   *  ignored (set to zero).
   */
  SOC_SAND_PP_MPLS_EXP internal_exp;
  /*
   *  The local port the packet enters from. Note: If by the
   *  global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is
   *  masked, then this value has to be zero.
   */
  SOC_PPC_PORT in_local_port;
  /*
   *  The incoming interface the packet associated with. Note:
   *  If by the global setting soc_ppd_frwrd_ilm_glbl_info_set,
   *  the in-RIF is masked, then this value has to be zero.
   */
  SOC_PPC_RIF_ID inrif_port;

} SOC_PPC_FRWRD_EXTEND_PWE_KEY;


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

void
  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO *info
  );

void
  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO *info
  );

void
  SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO *info
  );

void
  SOC_PPC_FRWRD_EXTEND_PWE_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_EXTEND_PWE_KEY *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_LIF_INFO *info
  );

void
  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_EXTEND_P2P_GLBL_INFO *info
  );

void
  SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_EXTEND_P2P_FRWRD_INFO *info
  );

void
  SOC_PPC_FRWRD_EXTEND_PWE_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_EXTEND_PWE_KEY *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_FRWRD_EXTEND_P2P_INCLUDED__*/
#endif

