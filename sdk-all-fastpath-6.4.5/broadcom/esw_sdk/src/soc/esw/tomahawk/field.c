/*
 * $Id: $
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
 *
 * SOC Routines related to Field module.
 *
 */

#include <soc/debug.h>
#include <soc/tomahawk.h>
#include <bcm_int/esw/tomahawk.h>

#if defined(BCM_TOMAHAWK_SUPPORT)


/*
 * Function:
 *      soc_th_field_mem_mode_get
 * Purpose:
 *      Retrieve Field Processor Group operational mode
 *      for the given field memory.
 * Parameters:
 *      unit    - (IN)  BCM Device number.
 *      mem     - (IN)  Field TCAM Memory.
 *      mode    - (OUT) Reference to SER memory mode.
 *
 * Returns:
 *      SOC_E_NONE   - Operation successful.
 *      SOC_E_PARAM  - Invalid parameter.
 *      SOC_E_INIT   - BCM unit not initialized.
 *
 * Notes:
 *      Valid "mem" input parameter values supported by
 *      this routine:
 *          "VFP_TCAMm, EFP_TCAMm, IFP_TCAMm,"
 *          "IFP_TCAM_WIDEm, IFP_LOGICAL_TABLE_TCAMm"
 *          "EXACT_MATCH_LOGICAL_TABLE_SELECTm."
 */
int
soc_th_field_mem_mode_get(
    int unit,
    soc_mem_t mem,
    int *mode)
{
   bcm_field_qualify_t          stage;      /* Field Stage Qualifier Id */
   bcm_field_group_oper_mode_t  oper_mode;  /* Field Operation Mode */
   int                          rv;         /* Return Value */

   /* Validate input parameters */
   if (mode == NULL) {
      return SOC_E_PARAM;
   }

   switch (mem) {
     /* Field lookup TCAMs and SRAMs */
     case VFP_TCAMm:
     case VFP_POLICY_TABLEm:
          stage = bcmFieldQualifyStageLookup;
          break;

     /* Field Egress TCAMs and SRAMs */
     case EFP_TCAMm:
     case EFP_POLICY_TABLEm:
     case EFP_COUNTER_TABLEm:
     case EFP_METER_TABLEm:
          stage = bcmFieldQualifyStageEgress;
          break;

     /* Field Ingress TCAMs */
     case IFP_LOGICAL_TABLE_SELECTm:
     case IFP_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
     case IFP_TCAMm:
     case IFP_TCAM_WIDEm:
     /* Field Ingress SRAMs */
     case FP_UDF_OFFSETm:
     case IFP_LOGICAL_TABLE_SELECT_DATA_ONLYm:
     case IFP_LOGICAL_TABLE_ACTION_PRIORITYm:
     case IFP_KEY_GEN_PROGRAM_PROFILEm:
     case IFP_KEY_GEN_PROGRAM_PROFILE2m:
     case IFP_POLICY_TABLEm:
     case IFP_METER_TABLEm:
          stage = bcmFieldQualifyStageIngress;
          break;

#if 0  
     /* Exact Match TCAMs */ 
     case EXACT_MATCH_LOGICAL_TABLE_SELECTm:
     case EXACT_MATCH_LOGICAL_TABLE_SELECT_TCAM_ONLYm:
     /* Exact Match SRAMs */ 
     case EXACT_MATCH_LOGICAL_TABLE_SELECT_DATA_ONLYm:
     case EXACT_MATCH_KEY_GEN_PROGRAM_PROFILEm:
     case EXACT_MATCH_ACTION_PROFILEm:
     case EXACT_MATCH_QOS_ACTIONS_PROFILEm:
     case EXACT_MATCH_DEFAULT_POLICYm:
     case EXACT_MATCH_KEY_GEN_MASKm:
     case EXACT_MATCH_2m:
     case EXACT_MATCH_4m:
     case EXACT_MATCH_2_ENTRY_ONLYm:
     case EXACT_MATCH_4_ENTRY_ONLYm:
          stage = bcmFieldQualifyStageExactMatch;
          break;
#endif /* 0 */

     /* Field Ingress Compression TCAMs and SRAMs */
     case SRC_COMPRESSIONm:
     case SRC_COMPRESSION_TCAM_ONLYm:
     case SRC_COMPRESSION_DATA_ONLYm:
     case DST_COMPRESSIONm:
     case DST_COMPRESSION_TCAM_ONLYm:
     case DST_COMPRESSION_DATA_ONLYm:
          stage = bcmFieldQualifyStageClass;
          break;
     case TTL_FNm:
     case TOS_FNm:
     case TCP_FNm:
     case IP_PROTO_MAPm:
          stage = bcmFieldQualifyStageClassExactMatch;
          break;

     case FP_UDF_TCAMm:
          /*
           *  Currently Field Module is not supporting per-pipe mode
           *  for this memory. Hence, returning it as Global mode.
           */
          *mode = _SOC_SER_MEM_MODE_GLOBAL;
          return BCM_E_NONE; 
     default:
          return SOC_E_UNAVAIL;
          break;
   }

   rv = _bcm_field_th_group_oper_mode_get(unit, stage, &oper_mode);
   if (SOC_FAILURE(rv)) {
       LOG_ERROR(BSL_LS_BCM_FP,
                 (BSL_META_U(unit,
                             "Failed to get Group Operation Mode[%d] for "
                             "Stage Qualifier[%d]/TCAM Memory[%d].\n"),
                             rv, stage, mem));
       return rv;
   }

   *mode = (oper_mode == bcmFieldGroupOperModeGlobal) ?
               _SOC_SER_MEM_MODE_GLOBAL:_SOC_SER_MEM_MODE_PIPE_UNIQUE;

   return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */
