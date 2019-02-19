/*
 * $Id: instr.c,v 1.26 Broadcom SDK $
 *
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
 *
 * Tomahawk Instrumentation API
 */

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <soc/loopback.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/switch.h>
#include <bcm_int/esw/instrumentation.h>
#include <bcm/l2.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/stack.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/firebolt.h>
#include <shared/bsl.h>
#include <shared/types.h>
#include <shared/bitop.h>
/* when visibility packet is sent
   following datastructure will be set */
STATIC _bcm_switch_pkt_trace_port_info_t pkt_trace_port_info[BCM_MAX_NUM_UNITS];
STATIC uint8 cpu_pkt_profile_id[BCM_MAX_NUM_UNITS];

#define TH_MAX_PKT_TRACE_CPU_PROFILE_INDEX 0x7

static uint32 ivp_raw_data[4];     /* packet trace ivp process result data */
static uint32 isw1_raw_data[8];    /* packet trace isw1 process result data */
static uint32 isw2_raw_data[16];   /* packet trace isw2 process result data */

/*
 * Function:
 *      bcm_th_pkt_trace_hw_reset
 * Purpose:
 *      clean PTR_RESULTS_BUFFER_IVP/ISW1/ISW2 registers
 *      call this functio before sending a 
 *      visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_hw_reset(int unit)
{
    uint32                          result_index;
    ptr_results_buffer_ivp_entry_t  ivp_entry;
    ptr_results_buffer_isw1_entry_t isw1_entry;
    ptr_results_buffer_isw2_entry_t isw2_entry;

    memset(&ivp_entry,  0x0, sizeof(ptr_results_buffer_ivp_entry_t));
    memset(&isw1_entry, 0x0, sizeof(ptr_results_buffer_isw1_entry_t));
    memset(&isw2_entry, 0x0, sizeof(ptr_results_buffer_isw2_entry_t));
     
    for (result_index = 0; result_index < TH_PTR_RESULTS_IVP_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_IVPm(unit, MEM_BLOCK_ALL, result_index, &ivp_entry);
    }

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW1_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_ISW1m(unit, MEM_BLOCK_ALL, result_index,&isw1_entry);
    }

    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW2_MAX_INDEX; result_index++) {  
        WRITE_PTR_RESULTS_BUFFER_ISW2m(unit, MEM_BLOCK_ALL, result_index,&isw2_entry);
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_port_set 
 * Purpose:
 *      set the source port of 
 *      visiblity packet
 *      this must be called before visibilty packet 
 *      is sent to set the pipe to read the resuls 
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_src_port_set(int unit, uint32 logical_src_port) 
{
    soc_info_t  *si;
    si = &SOC_INFO(unit);

    pkt_trace_port_info[unit].pkt_trace_src_logical_port = logical_src_port;
    pkt_trace_port_info[unit].pkt_trace_src_pipe =  
                                      si->port_pipe[logical_src_port];

    return 0;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_port_get 
 * Purpose:
 *      get the source port of 
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_src_port_get(int unit) 
{
    return pkt_trace_port_info[unit].pkt_trace_src_logical_port;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_src_pipe_get 
 * Purpose:
 *      get the ingress pipe of
 *      visiblity packet
 * Parameters:
 *      IN :  unit
 *      IN : logical_src_port
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_src_pipe_get(int unit) 
{
    return pkt_trace_port_info[unit].pkt_trace_src_pipe;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_stp_state_get
 * Purpose:
 *      change bit mapping of stp state 
 *      and assign to sw stp state 
 * Parameters:
 *      unit - (IN) Unit number.
 *      entry - (IN) BCM field entry id.
 *      stp_state (INOUT) stp state output.
 *      hw_stp_date (IN). hw stp state bits
 :* Returns:
 *      BCM_E_XXX
 * Notes:
 */
int
_bcm_th_pkt_trace_stp_state_get(int unit,
                                uint32 *stp_state,
                                uint32  hw_stp_data)
{
    /* Input parameters check. */
    if ((NULL == stp_state)) {
        return (BCM_E_PARAM);
    }

    switch (hw_stp_data) {
      case 0:
          *stp_state = BCM_STG_STP_DISABLE;
          break;
      case 1:
          *stp_state = BCM_STG_STP_BLOCK;
          break;
      case 2:
          *stp_state = BCM_STG_STP_LEARN;
          break;
      case 3:
          *stp_state = BCM_STG_STP_FORWARD;
          break;
      default:
          return (BCM_E_INTERNAL);
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *      _bcm_th_pkt_trace_drop_reason_abstract
 * Purpose:
 *      abstract visibility packet drop reason,
 * Parameters:
 *      unit - (IN) Unit number.
 *      drop_reason - (OUT) packet drop reason.
 * Returns:
 *      BCM_E_XXX
 */
static
int _bcm_th_pkt_trace_drop_reason_abstract(int unit,
                                           uint32* drop_reason)
{
    typedef struct drop_reason_map_s {
        int32  reason;
        int32  offset;
    } drop_reason_map_t;

    static drop_reason_map_t drop_reason_map[] = {
        {bcmSwitchPktTraceDropReasonMplsLabelLookupMiss,          48},
        {bcmSwitchPktTraceDropReasonMplsInvalidAction,            49},
        {bcmSwitchPktTraceDropReasonMplsInvalidPayload,           50},
        {bcmSwitchPktTraceDropReasonMplsTtlCheckFail,             51},
        {bcmSwitchPktTraceDropReasonMplsInvalidControlWord,       52},
        {bcmSwitchPktTraceDropReasonL2greSipLookupMiss,           54},
        {bcmSwitchPktTraceDropReasonL2greVpnLookupMiss,           55},
        {bcmSwitchPktTraceDropReasonL2greTunnelError,             56},
        {bcmSwitchPktTraceDropReasonVxlanSipLookupMiss,           57},
        {bcmSwitchPktTraceDropReasonVxlanVnidLookupMiss,          58},
        {bcmSwitchPktTraceDropReasonVxlanTunnelError,             59},
        {bcmSwitchPktTraceDropReasonVlanNotValid,                 27},
        {bcmSwitchPktTraceDropReasonIngressPortNotInVlanMember,   28},
        {bcmSwitchPktTraceDropReasonTpidMismatch,                 29},
        {bcmSwitchPktTraceDropReasonIpv4ProtocolError,            31},
        {bcmSwitchPktTraceDropReasonHigigLoopback,                32},
        {bcmSwitchPktTraceDropReasonHigigMirrorOnly,              33},
        {bcmSwitchPktTraceDropReasonHigigUnknownHeader,           34},
        {bcmSwitchPktTraceDropReasonHigigUnknownOpcode,           35},
        {bcmSwitchPktTraceDropReasonLagFailLoopback,              37},
        {bcmSwitchPktTraceDropReasonL2SrcEqualL2Dst,              38},
        {bcmSwitchPktTraceDropReasonIpv6ProtocolError,            39},
        {bcmSwitchPktTraceDropReasonNivVntagPresent,              43},
        {bcmSwitchPktTraceDropReasonNivVntagNotPresent,           44},
        {bcmSwitchPktTraceDropReasonNivVntagFormat,               45},
        {bcmSwitchPktTraceDropReasonTrillErrorFrame,              46},
        {bcmSwitchPktTraceDropReasonBpdu,                         65},
        {bcmSwitchPktTraceDropReasonBadUdpChecksum,               89},
        {bcmSwitchPktTraceDropReasonTunnlDecapEcnError,           107},
        {bcmSwitchPktTraceDropReasonIpv4HeaderError,              129},
        {bcmSwitchPktTraceDropReasonIpv6HeaderError,              131},
        {bcmSwitchPktTraceDropReasonParityError,                  143},
        {bcmSwitchPktTraceDropReasonRpfCheckFail,                 144},
        {-1,                                                      -1},
    };
    uint32 drop_reason_tmp[5] = {0,0,0,0,0};
    uint32 index;

    if(drop_reason == NULL) {
        return BCM_E_PARAM;
    }

    SHR_BITCOPY_RANGE(&drop_reason_tmp[0], 27, &(isw1_raw_data[6]), 27, 5);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[1], 0, &(isw1_raw_data[7]), 0, 32);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[2], 0, &(isw2_raw_data[12]), 0, 32);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[3], 0, &(isw2_raw_data[13]), 0, 12);
    SHR_BITCOPY_RANGE(&drop_reason_tmp[4], 0, &(isw2_raw_data[14]), 0, 25);

    SHR_BITSET(drop_reason, bcmSwitchPktTraceNoDrop);
    for(index = 0; ; index++) {
        if((drop_reason_map[index].offset == -1) || (drop_reason_map[index].reason == -1)) {
            break;
        }
        if(drop_reason_map[index].offset >= (32*5)) {
            continue;
        }
        if((drop_reason_map[index].reason <= bcmSwitchPktTraceDropReasonInternal) ||
           (drop_reason_map[index].reason >= bcmSwitchPktTraceDropReasonCount)) {
            continue;
        }
        if(SHR_BITGET(drop_reason_tmp, drop_reason_map[index].offset)) {
            SHR_BITSET(drop_reason, drop_reason_map[index].reason);
            SHR_BITCLR(drop_reason, bcmSwitchPktTraceNoDrop);
            SHR_BITCLR(drop_reason_tmp, drop_reason_map[index].offset);
        }
    }

    if(drop_reason_tmp[0] || drop_reason_tmp[1] || drop_reason_tmp[2] || drop_reason_tmp[3] || drop_reason_tmp[4]) {
        SHR_BITSET(drop_reason, bcmSwitchPktTraceDropReasonInternal);
        SHR_BITCLR(drop_reason, bcmSwitchPktTraceNoDrop);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_info_abstract
 * Purpose: 
 *      abstract visibility packet process data from PTR_RESULTS_BUFFER_IVP,
 *      ISW1, and ISW2 stored in bcm_switch_pkt_trace_info_s.
 * Parameters:
 *      IN :  unit
 *      IN:   pkt_trace_info
 * Returns:
 *      BCM_E_XXX
 */
static
int _bcm_th_pkt_trace_info_abstract(int unit,
                                  bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
#define STP_BIT_POS 4
#define STP_BIT_NUM 2
    /* pkt lookup and resolution vars*/
    uint32  my_station_index[1] = {0}, my_station_hit[1] = {0},
            lookup_status[1] = {0}, pkt_rsl_vector[1] = {0},
            ing_stp_state[1] = {0};

#ifdef INCLUDE_L3
    /* hashing information vars */
    uint32  ecmp_rsl_done1[1] = {0}, ecmp_rsl_done2[1] = {0}, 
            ecmp_hash_val1[1] = {0}, ecmp_hash_val2[1] = {0},
            ecmp_group1[1]    = {0}, ecmp_group2[1]    = {0}, 
            ecmp_offset1[1]   = {0}, ecmp_offset2[1]   = {0};
    uint32  ecmp_index, ecmp_ptr;
    int     ecmp_id;
    ecmp_count_entry_t      ecmpc_entry;
    ecmp_entry_t            ecmp_entry;
#endif
    uint32  lag_rsl_done[1]   = {0}, lag_hash_val[1]   = {0},
            lag_id[1]         = {0},  
            hg_rsl_done[1]    = {0}, hg_trunk_id[1]    = {0}, 
            hg_port[1]        = {0}, hg_hash_val[1]    = {0},
            hg_offset[1]      = {0}, lag_offset[1]     = {0};
    /* hashing cal vars */
    int         tgid,  mod_is_local;
    uint32      trunk_base, trunk_group_size;
    uint32      trunk_index, trunk_member_table_index;
    bcm_module_t        mod_id;
    bcm_port_t          port_id;
    bcm_gport_t         dst_member_port;
    _bcm_gport_dest_t   dest;

    trunk_member_entry_t    trunk_member_entry;
    trunk_group_entry_t     tg_entry;
    hg_trunk_member_entry_t hg_trunk_member_entry;
    hg_trunk_group_entry_t  hg_tg_entry;

    bcm_trunk_chip_info_t chip_info;

    /* retrieve my station info */
    SHR_BITCOPY_RANGE(my_station_index, 0, &(ivp_raw_data[0]), 17, 10);
    SHR_BITCOPY_RANGE(my_station_hit, 0, &(ivp_raw_data[0]), 27, 1);

    /* retrieve lookup status */
    SHR_BITCOPY_RANGE(lookup_status, 0, &(isw1_raw_data[6]), 7, 20);

    if (my_station_hit[0]) {
        SHR_BITSET(lookup_status, bcmSwitchPktTraceLookupMystationHit);
    }

    /* fill ingress stp enum*/
    SHR_BITCOPY_RANGE(ing_stp_state, 0, lookup_status, STP_BIT_POS, STP_BIT_NUM);
    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_stp_state_get(unit, 
                              &(pkt_trace_info->pkt_trace_stp_state),
                                ing_stp_state[0]));

    /* clear stp bits from lookup status */
    SHR_BITCLR(lookup_status, STP_BIT_POS);
    SHR_BITCLR(lookup_status, STP_BIT_POS + STP_BIT_NUM - 1);

    /* fill lookup status enum*/
    SHR_BITCOPY_RANGE(
        (pkt_trace_info->pkt_trace_lookup_status.pkt_trace_status_bitmap), 0, 
         lookup_status, 0, bcmSwitchPktTraceLookupCount+1);
  
    /* fill packet resolution vector */
    SHR_BITCOPY_RANGE(pkt_rsl_vector, 0, &(isw2_raw_data[0]), 22, 6);
    pkt_trace_info->pkt_trace_resolution = pkt_rsl_vector[0];

#ifdef INCLUDE_L3
    /*retrieve hashing information*/
    SHR_BITCOPY_RANGE(ecmp_offset2 , 0, &(isw2_raw_data[2]), 4, 10);
    SHR_BITCOPY_RANGE(ecmp_group2 , 0, &(isw2_raw_data[2]), 14 , 11);
    SHR_BITCOPY_RANGE(ecmp_rsl_done2 , 0, &(isw2_raw_data[2]), 25, 1);

    /* ecmp_hash_val1 = isw2.dw1[41:26] */
    SHR_BITCOPY_RANGE(ecmp_hash_val1 , 0, &(isw2_raw_data[2]), 26 , 6);
    SHR_BITCOPY_RANGE(ecmp_hash_val1 , 6, &(isw2_raw_data[3]), 0, 10);
    SHR_BITCOPY_RANGE(ecmp_offset1, 0, &(isw2_raw_data[3]), 10, 10);
    SHR_BITCOPY_RANGE(ecmp_group1 , 0, &(isw2_raw_data[3]), 20, 11);
    SHR_BITCOPY_RANGE(ecmp_rsl_done1 , 0, &(isw2_raw_data[3]), 31, 1);
    SHR_BITCOPY_RANGE(ecmp_hash_val2 , 0, &(isw2_raw_data[5]), 16, 16);
#endif

    /* isw2.dw2[31:0] */
    SHR_BITCOPY_RANGE(lag_offset, 0, &(isw2_raw_data[4]), 1, 10);
    SHR_BITCOPY_RANGE(lag_hash_val , 0, &(isw2_raw_data[4]), 11, 10);
    SHR_BITCOPY_RANGE(lag_id , 0, &(isw2_raw_data[4]) ,21, 10);
    SHR_BITCOPY_RANGE(lag_rsl_done , 0, &(isw2_raw_data[4]), 31, 1);

    /* isw2.dw3[21:0] */
    SHR_BITCOPY_RANGE(hg_offset , 0, &(isw2_raw_data[6]), 0, 5);
    SHR_BITCOPY_RANGE(hg_port , 0, &(isw2_raw_data[6]), 6, 8);
    SHR_BITCOPY_RANGE(hg_trunk_id , 0, &(isw2_raw_data[6]), 14, 7);
    SHR_BITCOPY_RANGE(hg_rsl_done , 0, &(isw2_raw_data[6]),21 , 1);

    /* isw2.dw4[63:54] */
    SHR_BITCOPY_RANGE(hg_hash_val , 0, &(isw2_raw_data[8]), 22, 10);

#ifdef INCLUDE_L3
    if (ecmp_rsl_done1[0]) {
        ecmp_id = ecmp_group1[0];

       SOC_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_id, &ecmpc_entry));

        ecmp_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                  BASE_PTRf);
        ecmp_index = (ecmp_ptr + ecmp_offset1[0]) & 0x3fff;
    
        SOC_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group   = ecmp_id; 
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_group  += BCM_XGS3_MPATH_EGRESS_IDX_MIN;
        
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress  = 
                    soc_L3_ECMPm_field32_get(unit, &ecmp_entry, NEXT_HOP_INDEXf); 
        pkt_trace_info->pkt_trace_hash_info.ecmp_1_egress += BCM_XGS3_EGRESS_IDX_MIN;

        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_ECMP_1;
    }

    if (ecmp_rsl_done2[0]) {
        ecmp_id = ecmp_group2[0];

       SOC_IF_ERROR_RETURN
            (READ_L3_ECMP_COUNTm(unit, MEM_BLOCK_ANY, ecmp_id, &ecmpc_entry));

        ecmp_ptr = soc_L3_ECMP_COUNTm_field32_get(unit, &ecmpc_entry,
                                                  BASE_PTRf);
        ecmp_index = (ecmp_ptr + ecmp_offset2[0]) & 0x3fff;
    
        SOC_IF_ERROR_RETURN
            (READ_L3_ECMPm(unit, MEM_BLOCK_ANY, ecmp_index, &ecmp_entry));

        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group   = ecmp_id; 
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_group  += BCM_XGS3_MPATH_EGRESS_IDX_MIN;
        
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress   = 
                    soc_L3_ECMPm_field32_get(unit, &ecmp_entry, NEXT_HOP_INDEXf); 
        pkt_trace_info->pkt_trace_hash_info.ecmp_2_egress  += BCM_XGS3_EGRESS_IDX_MIN;
        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_ECMP_2;
    }

#endif        

    if (lag_rsl_done[0]) {
        tgid = lag_id[0]; 

        BCM_IF_ERROR_RETURN(READ_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tgid, &tg_entry));

        trunk_group_size    = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry, TG_SIZEf);
        trunk_base          = soc_TRUNK_GROUPm_field32_get(unit, &tg_entry, BASE_PTRf);
        trunk_index         = lag_hash_val[0] % (trunk_group_size + 1);
        trunk_member_table_index = (trunk_base + trunk_index) & 0x7ff;

        BCM_IF_ERROR_RETURN(READ_TRUNK_MEMBERm(unit, MEM_BLOCK_ANY, 
                                 trunk_member_table_index, &trunk_member_entry));
        mod_id  = soc_TRUNK_MEMBERm_field32_get(unit, &trunk_member_entry,
                                                MODULE_IDf);
        port_id = soc_TRUNK_MEMBERm_field32_get(unit, &trunk_member_entry,
                                                PORT_NUMf);
       
        BCM_IF_ERROR_RETURN
            (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                            &(dest.modid), &(dest.port)));        
        dest.gport_type = _SHR_GPORT_TYPE_MODPORT;

        BCM_IF_ERROR_RETURN
            (_bcm_esw_modid_is_local(unit, dest.modid,
                                     &mod_is_local));
        if (mod_is_local) {
            if (IS_ST_PORT(unit, dest.port)) {
                dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            }
        }
        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &dst_member_port));

        pkt_trace_info->pkt_trace_hash_info.trunk        = tgid; 
        pkt_trace_info->pkt_trace_hash_info.trunk_member = dst_member_port; 
        pkt_trace_info->pkt_trace_hash_info.flags += BCM_SWITCH_PKT_TRACE_TRUNK;

    } /* end lag resolution */

    if (hg_rsl_done[0]) {
        tgid = hg_trunk_id[0]; 

        BCM_IF_ERROR_RETURN(READ_HG_TRUNK_GROUPm(unit, MEM_BLOCK_ANY, tgid, &hg_tg_entry));

        trunk_group_size    = soc_HG_TRUNK_GROUPm_field32_get(unit, &hg_tg_entry, TG_SIZEf);
        trunk_base          = soc_HG_TRUNK_GROUPm_field32_get(unit, &hg_tg_entry, BASE_PTRf);
        trunk_index         = hg_hash_val[0] % (trunk_group_size + 1);
        trunk_member_table_index = (trunk_base + trunk_index) & 0xff;

        BCM_IF_ERROR_RETURN(READ_HG_TRUNK_MEMBERm(unit, MEM_BLOCK_ANY,
                            trunk_member_table_index, &hg_trunk_member_entry));
        port_id = soc_HG_TRUNK_MEMBERm_field32_get(unit, 
                            &hg_trunk_member_entry, PORT_NUMf);

        if (BCM_FAILURE(bcm_esw_stk_my_modid_get(unit, &mod_id))) {
            mod_id = 0;
        }
    
        BCM_IF_ERROR_RETURN
                (_bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_GET, mod_id, port_id,
                                             &(dest.modid), &(dest.port)));
        dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;

        BCM_IF_ERROR_RETURN(_bcm_esw_gport_construct(unit, &dest, &dst_member_port));

        BCM_IF_ERROR_RETURN(bcm_esw_trunk_chip_info_get(unit, &chip_info));
        pkt_trace_info->pkt_trace_hash_info.fabric_trunk = 
                            tgid + chip_info.trunk_fabric_id_min;
        pkt_trace_info->pkt_trace_hash_info.fabric_trunk_member = 
                            dst_member_port;
        pkt_trace_info->pkt_trace_hash_info.flags += 
                            BCM_SWITCH_PKT_TRACE_FABRIC_TRUNK;

    } /* end lag resolution */

    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_drop_reason_abstract(unit,
                        pkt_trace_info->pkt_trace_drop_reason));
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_info_get
 * Purpose:
 *      read results ptr buffer for
 *      visibility packet process and
 *      store into pkt_trace_info
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_info_get(int unit,
                              bcm_switch_pkt_trace_info_t *pkt_trace_info)
{
    ptr_results_buffer_ivp_entry_t        ivp_entry;
    ptr_results_buffer_isw1_entry_t       isw1_entry; 
    ptr_results_buffer_isw2_entry_t       isw2_entry;

    uint8    result_index = 0, prev_index = 0, pipe = 0, trace_result_avail = 0;
    uint32   *dw_first, *dw_second;

    soc_mem_t mem;
    pipe =  pkt_trace_port_info[unit].pkt_trace_src_pipe;
    pkt_trace_info->dest_pipe_num = pipe;

    for (result_index = 0; result_index < TH_PTR_RESULTS_IVP_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IVPm) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_IVPm)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &ivp_entry)); 

        dw_first = (uint32*)&ivp_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(ivp_raw_data[result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(ivp_raw_data[(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

        if ((*dw_first != 0) || (*dw_second != 0)) {
            trace_result_avail = 1;
        }  

        sal_memcpy(&(pkt_trace_info->raw_data[result_index*8]), 
                   (uint8*)&ivp_entry, (sizeof(ivp_entry)));

    }/*end of ivp_parsing*/
    prev_index += result_index;
 
    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW1_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW1m) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW1m)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &isw1_entry)); 

        dw_first = (uint32*)&isw1_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(isw1_raw_data[result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(isw1_raw_data[(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

        if ((*dw_first != 0) || (*dw_second != 0)) {
            trace_result_avail = 1;
        }
 
        sal_memcpy(&(pkt_trace_info->raw_data[(prev_index+result_index)*8]), 
                   (uint8*)&isw1_entry, (sizeof(isw1_entry)));
   
    }/*end of isw1_parsing*/
    prev_index += result_index;
 
    for (result_index = 0; result_index < TH_PTR_RESULTS_ISW2_MAX_INDEX; result_index++) {

        if (SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW2m) == NULL) {
            return BCM_E_INTERNAL;
        }
        mem = SOC_MEM_UNIQUE_ACC(unit, PTR_RESULTS_BUFFER_ISW2m)[pipe];
        if (mem == INVALIDm) {
            return BCM_E_INTERNAL;
        }

        SOC_IF_ERROR_RETURN(soc_mem_read(unit, mem, MEM_BLOCK_ANY, result_index, &isw2_entry)); 

        dw_first = (uint32*)&isw2_entry;
        dw_second = dw_first+1; 

        sal_memcpy(&(isw2_raw_data[result_index *2]),
                   dw_first, sizeof(uint32));
        sal_memcpy(&(isw2_raw_data[(result_index *2) + 1]),
                   dw_second, sizeof(uint32));

        if ((*dw_first != 0) || (*dw_second != 0)) {
            trace_result_avail = 1;
        }

 
        sal_memcpy(&(pkt_trace_info->raw_data[(prev_index+result_index)*8]), 
                   (uint8*)&isw2_entry, (sizeof(isw2_entry)));
   
    }/*end of isw2_parsing*/
  
    if (trace_result_avail == 0) {
        return BCM_E_BUSY;
    }

    pkt_trace_info->raw_data_length = (prev_index+result_index)*8;
    /* abstraction */
    BCM_IF_ERROR_RETURN(_bcm_th_pkt_trace_info_abstract(unit, pkt_trace_info));
 
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_cpu_profile_init 
 * Purpose:
 *      pre-configure CPU_PKT_PROFILE_1,2 Register according to
 *      index : drop | ifp  | no learn
 *       0       0      0      0          
 *       1       0      0      1
 *       2       0      1      0
 *       3       0      1      1
 *       4       1      0      0
 *       5       1      0      1
 *       6       1      1      0
 *       7       1      1      1
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_cpu_profile_init(int unit) {
    /* pkt_profile_1 contains control of IFP and Learning */
    uint32  pkt_profile_1_w = 0xCCAA;
    /* pkt_profile_2 contains control of forwarding */
    uint32  pkt_profile_2_w = 0xF0;

    if (SOC_REG_IS_VALID(unit, CPU_PKT_PROFILE_1r)) {
        WRITE_CPU_PKT_PROFILE_1r(unit, pkt_profile_1_w);
    }

    if (SOC_REG_IS_VALID(unit, CPU_PKT_PROFILE_2r)) {
        WRITE_CPU_PKT_PROFILE_2r(unit, pkt_profile_2_w);
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_cpu_profile_set
 * Purpose:
 *      given visibility packet behavior flags
 *      setting the profile id for the next visiblity packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */
int _bcm_th_pkt_trace_cpu_profile_set(int unit,
                                     uint32 flags)
{
    uint32 negate_flags[1] = {0}, param_flags[1] = {0};
    uint32 max_profile_index[1] = {0}, profile_id[1] = {0};
    param_flags[0] = flags;
    max_profile_index[0] =  TH_MAX_PKT_TRACE_CPU_PROFILE_INDEX;
    SHR_BITNEGATE_RANGE(param_flags, 0, 3, negate_flags); 

    SHR_BITAND_RANGE(negate_flags, max_profile_index, 0, 3,
                     profile_id);    

    cpu_pkt_profile_id[unit] = profile_id[0];
          

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_cpu_profile_get
 * Purpose:
 *      retrieve cpu profile id to be used for
 *      the next visibilty packet
 * Parameters:
 *      IN :  unit
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_cpu_profile_get(int unit, uint32* profile_id)
{
    if (profile_id == NULL) {
        return BCM_E_INTERNAL;
    }
    *profile_id = cpu_pkt_profile_id[unit];
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_th_pkt_trace_int_lbport_set
 * Purpose:
 *       When SDK performs cpu masquerade, make sure
 *       the following 6 fields are consistent between
 *       lport_table and port_table.
 *       V6L3_ENABLEf, V4L3_ENABLEf, IEEE_802_1AS_ENABLEf,
 *       V4IPMC_ENABLEf, L2GRE_TERMINATION_ALLOWEDf,
 *       VXLAN_TERMINATION_ALLOWEDf.
 *
 * Parameters:
 *      unit    - (IN)  bcm device.
 *      port    - (IN)  port number.
 *      enable   - (IN)  1: enable the sw workaround;
 *                       0: disable the sw workaround.
 * Returns:
 *      BCM_E_XXX
 */

int _bcm_th_pkt_trace_int_lbport_set(int unit, uint8 port, int enable)
{
    int pipe_num, i;
    int loopback_port;
    uint32 values[6];
    static int old_values[6] = {0};
    soc_field_t  fields[6] = {
        V6L3_ENABLEf, V4L3_ENABLEf, IEEE_802_1AS_ENABLEf,
        V4IPMC_ENABLEf, L2GRE_TERMINATION_ALLOWEDf, VXLAN_TERMINATION_ALLOWEDf,
    };
    SOC_IF_ERROR_RETURN(
        soc_port_pipe_get(unit, port, &pipe_num));
    loopback_port = soc_loopback_lbport_num_get(unit, pipe_num);
    if (loopback_port == -1) {
        return BCM_E_PARAM;
    }
    if (enable) {
        BCM_IF_ERROR_RETURN(
            bcm_esw_port_lport_fields_get(unit, port,
                                          LPORT_PROFILE_LPORT_TAB, 6,
                                          fields, values));
        for (i = 0; i < 6; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_get(unit, loopback_port,
                                      fields[i], &old_values[i]));
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, loopback_port, 0,
                                      fields[i], values[i]));
        }
    } else {
        for (i = 0; i < 6; i++) {
            BCM_IF_ERROR_RETURN(
                _bcm_esw_port_tab_set(unit, loopback_port, 0,
                                      fields[i], old_values[i]));
        }
    }
    return BCM_E_NONE;
}
