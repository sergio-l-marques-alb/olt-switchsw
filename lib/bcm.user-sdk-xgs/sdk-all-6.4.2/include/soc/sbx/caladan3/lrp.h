/*
 * $Id: lrp.h,v 1.19 Broadcom SDK $
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
 * lrp.h : LRP defines
 *
 *-----------------------------------------------------------------------------*/
#ifndef _SBX_CALADN3_LRP_H_
#define _SBX_CALADN3_LRP_H_

#define SOC_SBX_CALADAN3_LR_NUM_OF_STREAMS      (12)
#define SOC_SBX_CALADAN3_LR_NUM_OF_INSTRS       (1536)
#define SOC_SBX_CALADAN3_LR_NUM_OF_PE           (64)
#define SOC_SBX_CALADAN3_LR_INST_MEM_ECC_BYTES  (2)
#define SOC_SBX_CALADAN3_LR_INST_MEM_INST_BYTES (12)
#define SOC_SBX_CALADAN3_LR_INST_NUM_BANKS      (2)

#define SOC_SBX_CALADAN3_LR_CONTEXTS_MAX         8
#define SOC_SBX_CALADAN3_LR_CONTEXTS_RSVD        3
#define SOC_SBX_CALADAN3_LR_CONTEXTS_USER_MAX    \
             (SOC_SBX_CALADAN3_LR_CONTEXTS_MAX - SOC_SBX_CALADAN3_LR_CONTEXTS_RSVD)

#define SOC_SBX_CALADAN3_LR_EPOCH_LENGTH_MIN     426
#define SOC_SBX_CALADAN3_LR_EPOCH_LENGTH_MAX              \
		SOC_SBX_CALADAN3_LR_NUM_OF_INSTRS
#define SOC_SBX_CALADAN3_LR_FRAMES_PER_CONTEXT_MAX 128

#define SOC_SBX_CALADAN3_DUPLEX_MODE            1
#define SOC_SBX_CALADAN3_SIMPLEX_MODE           0

#define SOC_SBX_CALADAN3_LR_STREAM_ONLINE(lrp, stream) \
                 ((lrp)->streams_online | (1 << (stream))

#define SOC_SBX_CALADAN3_LR_IS_STREAM_ONLINE(lrp, stream) \
                 ((lrp)->streams_online & (1 << (stream)) ? 1 : 0)


/* LRP */
typedef struct soc_sbx_caladan3_lrp_s {
    int  epoch_length;
    int  num_context;
    int  detected_context[SOC_SBX_CALADAN3_LR_INST_NUM_BANKS];
    int  frames_per_context;
    int  num_active_pe;
    int  bank_select;
    int   debug_load;
    int   debug_len;
    uint8 bypass;
    uint8 duplex;
    uint8 loader_enable;
    uint32 streams_online;
    int   init_done;
    int   ucode_done;
    int   ucode_reload;
    uint32 oam_num_endpoints;
    lra_inst_b0_mem0_entry_t *s01;
} soc_sbx_caladan3_lrp_t;


extern int soc_sbx_caladan3_lr_driver_init(int unit);

int soc_sbx_caladan3_lr_iwrite(void* handle, uint32 snum, 
                               uint32 inum, uint8 *inst);

int soc_sbx_caladan3_lr_iread(void *handle, uint32 snum, 
                              uint32 inum, uint8 *inst);

extern int soc_sbx_caladan3_lr_host_bubble(int unit,
                                  int stream,
                                  int task,
                                  uint32 id);
int
soc_sbx_caladan3_lr_bubble_enable(int unit, uint32 enable, uint32 size_in_bytes);

extern int soc_sbx_caladan3_lr_shared_register_iaccess(int unit,
                                  int operation,
                                  uint32 address,
                                  uint32 *data);

extern int soc_sbx_lrp_setup_tmu_program(int unit, 
				  int table_index,
				  uint32 program, 
                                  int update,
				  uint8 key0valid,
                                  uint8 key1valid );
int
soc_sbx_caladan3_lr_isr_enable(int unit);
int
soc_sbx_caladan3_lrb_isr_enable(int unit);


#endif /* _SBX_CALADN3_LRP_H_ */
