/* $Id: ppc_api_frwrd_ilm.h,v 1.9 Broadcom SDK $
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
* FILENAME:       DuneDriver/ppc/include/soc_ppc_api_frwrd_ilm.h
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

#ifndef __SOC_PPC_API_FRWRD_ILM_INCLUDED__
/* { */
#define __SOC_PPC_API_FRWRD_ILM_INCLUDED__

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

/*     SOC_PPC_FRWRD_ILM_KEY_XXX flags */
#define  SOC_PPC_FRWRD_ILM_KEY_COUPLING  (0x1)

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
   *  If FALSE, then the key of the ILM includes the local
   *  port the packet came from. Otherwise it is masked.
   */
  uint8 mask_port;
  /*
   *  If FALSE, then the key of the ILM includes the incoming
   *  router interface the packet came from. Otherwise it is
   *  masked. Always masked in T20E
   */
  uint8 mask_inrif;

} SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  For labels in this range, the ILM key includes the EXP
   *  value. In order to provide QoS treatment according to
   *  the EXP bits in the MPLS header.
   */
  SOC_SAND_U32_RANGE labels_range;
  /*
   *  For labels in the range: map the EXP (from the MPLS
   *  header) to internal values. For labels out of the range,
   *  internal value is masked and set to 0. exp_map_tbl[x] =
   *  y; maps EXP x to internal value y. Note that the mapped
   *  values should be smaller than 8.
   */
  SOC_SAND_PP_MPLS_EXP exp_map_tbl[SOC_SAND_PP_NOF_BITS_IN_EXP];

} SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Incoming label.
   */
  SOC_SAND_PP_MPLS_LABEL in_label;
  /*
   *  Incoming label second.
   *  Used for Coupling.
   *  Second label after forwarding label.
   *  Invalid for Soc_petra-B.
   */
  SOC_SAND_PP_MPLS_LABEL in_label_second;
  /*
   *  Internal EXP. Relevant only for labels in the ELSP
   *  range. In this case, this is the value after mapping the
   *  header EXP with 'exp_map_tbl table'. Otherwise, this is
   *  ignored (set to zero)
   */
  SOC_SAND_PP_MPLS_EXP mapped_exp;
  /*
   *  The local port the packet enters from. Note: If by the
   *  global setting soc_ppd_frwrd_ilm_glbl_info_set, the port is
   *  masked, then this value has to be zero.
   */
  SOC_PPC_PORT in_local_port;
  /*
   *  The incoming interface the packet associated with. Note:
   *  If by the global setting soc_ppd_frwrd_ilm_glbl_info_set,
   *  the in-RIF is masked, then this value has to be
   *  zero. Always masked in T20E.
   */
  SOC_PPC_RIF_ID inrif;
  /* 
   *  FRWRD ILM Key flags
   *  See SOC_PPC_FRWRD_ILM_KEY_XXX
   *  Invalid for Soc_petra-B.
   */
  uint32 flags;

} SOC_PPC_FRWRD_ILM_KEY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  How to build the key to be used for the forwarding
   *  lookup.
   */
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO key_info;
  /*
   *  Defines range of the labels as an E-LSP
   *  (Exp-Inferred-LSP) label that the QoS treatment for MPLS
   *  packet derived from the EXP bits in the MPLS header
   */
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO elsp_info;

  /*
   * Enable short pipe mode for MPLS tunnel.
   */
  uint8 short_pipe_enable;

} SOC_PPC_FRWRD_ILM_GLBL_INFO;


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
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_KEY_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_KEY *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_clear(
    SOC_SAND_OUT SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  );

#if SOC_PPC_DEBUG_IS_LVL1

void
  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_KEY_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_ELSP_INFO *info
  );

void
  SOC_PPC_FRWRD_ILM_KEY_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_KEY *info
  );

void
  SOC_PPC_FRWRD_ILM_GLBL_INFO_print(
    SOC_SAND_IN  SOC_PPC_FRWRD_ILM_GLBL_INFO *info
  );

#endif /* SOC_PPC_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PPC_API_FRWRD_ILM_INCLUDED__*/
#endif

