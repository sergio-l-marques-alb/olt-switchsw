/* $Id: soc_pb_egr_prog_editor.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PB_EGR_PROG_EDITOR_INCLUDED__
/* { */
#define __SOC_PB_EGR_PROG_EDITOR_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/Petra/PB_TM/pb_api_framework.h>

#include <soc/dpp/Petra/PB_TM/pb_api_stack.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define SOC_PB_EGR_PROG_EDITOR_CE_TBL_SIZE   (16)

/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* $Id: soc_pb_egr_prog_editor.h,v 1.5 Broadcom SDK $
 * Get the OTMH extension profile
 */
#define SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD(is_src, is_dest, cud_type) \
  (SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV + (cud_type) + ((is_dest) * 3) + ((is_src) * 6))

#define SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_CUD_PRESENT_GET(tm_port_profile, is_tm_cud_to_add) 																					\
    if ((tm_port_profile - SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) % SOC_PETRA_PORTS_FTMH_NOF_EXT_OUTLIFS == 0)          \
    {                                                                                                                               \
      /* Never add CUD */                                                                                                           \
      is_tm_cud_to_add = FALSE;                                                                                                     \
    }                                                                                                                               \
    else if ((tm_port_profile - SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) % SOC_PETRA_PORTS_FTMH_NOF_EXT_OUTLIFS == 1)     \
    {                                                                                                                               \
      /* Add CUD if Multicast */                                                                                                    \
      is_tm_cud_to_add = system_mc;                                                                                                 \
    }                                                                                                                               \
    else                                                                                                                            \
    {                                                                                                                               \
      is_tm_cud_to_add = TRUE;                                                                                                      \
    }

#define SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_GET(tm_port_profile, is_src)  \
  is_src = SOC_SAND_NUM2BOOL(((tm_port_profile - SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) / 6) % 2)

#define SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_DEST_GET(tm_port_profile, is_dest)  \
  is_dest = SOC_SAND_NUM2BOOL(((tm_port_profile - SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV) / 3) % 2)

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   *  CPU without editing maybe need to add the stamping of the cud
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU,
  /*
   *  Raw without editing remove all system headers including PPH
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_RAW,
  /*
   * Stacking Port With/Without CUD ext
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK1,
  /*
   *  fap20b rcy only increase the packet in two bytes
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_FAP20,
  /*
   *	 TDM  mode: removing FTMH
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM1,
  /*
   * TDM  mode: remove standard FTMH and generating external header
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TDM2,
  /*
   *Remove system headers and old Network header and Add new Network Header
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_ETH,
  /*
   *  CPU without editing but add the stamping of the cud
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_CPU_CUD,
  /*
   *  No Source, No Destination, Never CUD.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV,
  /*
   *  No source, No Destination, CUD if Multicast.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC,
  /*
   *	No Source, No Destination, CUD always.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW,
  /*
   *  No Source, Destination, Never CUD.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_NEV,
  /*
   *  No source, Destination, CUD if Multicast.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_MC,
  /*
   *	No Source, Destination, CUD always.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_ALW,
  /*
   *  Source, No Destination, Never CUD.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV,
  /*
   *  Source, No Destination, CUD if Multicast.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC,
  /*
   *	Source, No Destination, CUD always.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW,
  /*
   *  Source, Destination, Never CUD.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_NEV,
  /*
   *  Source, Destination, CUD if Multicast.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_MC,
  /*
   *	Source, Destination, CUD always.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_ALW,
  /*
   *  No Source, No Destination, Never CUD, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_NEV_WITH_PPH,
  /*
   *  No source, No Destination, CUD if Multicast, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_MC_WITH_PPH,
  /*
   *	No Source, No Destination, CUD always, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_NO_CUD_ALW_WITH_PPH,
  /*
   *  No Source, Destination, Never CUD, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_NEV_WITH_PPH,
  /*
   *  No source, Destination, CUD if Multicast, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_MC_WITH_PPH,
  /*
   *	No Source, Destination, CUD always, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_NO_DEST_CUD_ALW_WITH_PPH,
  /*
   *  Source, No Destination, Never CUD, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_NEV_WITH_PPH,
  /*
   *  Source, No Destination, CUD if Multicast, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_MC_WITH_PPH,
  /*
   *	Source, No Destination, CUD always, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_NO_CUD_ALW_WITH_PPH,
  /*
   *  Source, Destination, Never CUD, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_NEV_WITH_PPH,
  /*
   *  Source, Destination, CUD if Multicast, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_MC_WITH_PPH,
  /*
   *	Source, Destination, CUD always, Keep PPH.
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_TM_SRC_DEST_CUD_ALW_WITH_PPH,
  /*
   * Stacking Port With/Without CUD ext
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_STACK2,
  /*
   * Injected: FTMH (6B) + Out-LIF Extension (2B) + PPH
   */
  SOC_PB_EGR_PROG_TM_PORT_PROFILE_INJECTED,
  /*
   *  Number of types in SOC_PB_EGR_PROG_TM_PORT_PROFILE
   */
  SOC_PB_EGR_NOF_PROG_TM_PORT_PROFILES
}SOC_PB_EGR_PROG_TM_PORT_PROFILE;

typedef struct
{
  uint32 lfem_program;
  uint32 bitcount;
  uint32 niblle_field_offset;
  uint32 header_offset_select;
  uint32 source_select;
  uint32 valid;
} SOC_PB_EGR_PROG_EDITOR_CE_INSTRUCTION;


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
  soc_pb_egr_prog_editor_port_profile_to_hw_add(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_IN  SOC_PB_EGR_PROG_TM_PORT_PROFILE  eg_profile,
    SOC_SAND_IN  uint32                    internal_eg_profile_ndx,
    SOC_SAND_IN  uint8                    first_appear
  );

uint32
  soc_pb_egr_prog_editor_profile_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  uint32                    port_ndx,
    SOC_SAND_OUT SOC_PB_EGR_PROG_TM_PORT_PROFILE  *eg_profile,
    SOC_SAND_OUT uint32                     *internal_profile
  );

uint32
  soc_pb_egr_prog_editor_unsafe(
    SOC_SAND_IN  int                                 unit
  );

uint32
  soc_pb_egr_prog_editor_stack_prune_bitmap_set(
    SOC_SAND_IN int unit,
    SOC_SAND_IN uint32 local_stack_port_ndx,
    SOC_SAND_IN uint32  bitmap[SOC_PB_STACK_PRUN_BMP_LEN]
  );

uint32
  soc_pb_egr_prog_editor_stack_prune_bitmap_get(
    SOC_SAND_IN  int unit,
    SOC_SAND_IN  uint32 stack_profile,
    SOC_SAND_OUT uint32  bitmap[SOC_PB_STACK_PRUN_BMP_LEN]
  );

#if SOC_PB_DEBUG_IS_LVL1

#endif /* SOC_PB_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_EGR_PROG_EDITOR_INCLUDED__*/
#endif

