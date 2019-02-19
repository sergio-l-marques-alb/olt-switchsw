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
 * TH counter module routines.
 *
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>

#include <soc/debug.h>
#include <soc/util.h>
#include <soc/mem.h>
#include <soc/tomahawk.h>

#ifdef BCM_TOMAHAWK_SUPPORT

int
soc_counter_tomahawk_get_child_dma(int unit, soc_reg_t id,
                                   soc_ctr_control_info_t ctrl_info,
                                   soc_counter_non_dma_t *child_dma)
{
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *parent_dma = NULL;
    int c_offset = -1;

    if ((id < NUM_SOC_REG) || (id >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }
    parent_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if ((!(child_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) || 
        (ctrl_info.instance == -1)) {
        child_dma = parent_dma;
        return SOC_E_NONE;
    }

    switch (ctrl_info.instance_type) {
        case SOC_CTR_INSTANCE_TYPE_POOL:
            c_offset = ctrl_info.instance;
            if ((c_offset < 0) || (c_offset >= child_dma->extra_ctr_ct)) {
                return SOC_E_PARAM;
            }

            switch (id) {
                case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE: 
                case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
                    if ((parent_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                        (parent_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                        */
                        child_dma = &parent_dma->extra_ctrs[(c_offset << 1)];
                    }
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        default:
            return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

int
soc_counter_tomahawk_generic_get_info(int unit,soc_ctr_control_info_t ctrl_info,
                                      soc_reg_t id, int *base_index,
                                      int *num_entries)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_control_t *soc = SOC_CONTROL(unit);
    soc_counter_non_dma_t *non_dma;
    soc_port_t port;
    soc_port_t phy_port = 0, mmu_port = 0;
    int pipe, c_offset, pipe_offset = 0;

    if ((id < NUM_SOC_REG) || (id >= SOC_COUNTER_NON_DMA_END)) {
        return SOC_E_PARAM;
    }

    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) &&
            !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* For Parent-Child model, return E_UNAVAIL if child non_dma is INVALID.
         */
        return SOC_E_UNAVAIL;
    }

    switch (ctrl_info.instance_type) {
        case SOC_CTR_INSTANCE_TYPE_PORT: 
            port = ctrl_info.instance;
            if (si->port_l2p_mapping[port] == -1) {
                *base_index = 0;
                *num_entries = 0;
                return SOC_E_PARAM;
            }
            pipe = si->port_pipe[port];

            switch (id) {
                case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
                case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
                    if (IS_CPU_PORT(unit, port)) {
                        *base_index = 0;
                    } else {
                        *base_index = pipe * 728 + 48 + (port % 34) * 20 + 10;
                    }
                    *num_entries = si->port_num_cosq[port];
                    break;

                case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
                case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
                    if (IS_CPU_PORT(unit, port)) {
                        *base_index = 0;
                    } else {
                        *base_index = pipe * 728 + 48 + (port % 34) * 20;
                    }
                    *num_entries = si->port_num_cosq[port];
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;

        case SOC_CTR_INSTANCE_TYPE_XPE_PORT:
            c_offset = (ctrl_info.instance & 0x3C00) >> 10;
            port =  (ctrl_info.instance & 0xFFC000) >> 14;
            phy_port = si->port_l2p_mapping[port];
            mmu_port = si->port_p2m_mapping[phy_port];
            pipe = si->port_pipe[port];

            switch (id) {
                case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
                case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
                    if (IS_CPU_PORT(unit, port)) {
                        *base_index = 330;
                        *num_entries = 48;
                    } else if (IS_LB_PORT(unit, port)) {
                        *base_index = 320;
                        *num_entries = 10;
                    } else {
                        *base_index = (mmu_port % 32) * 10;
                        *num_entries = 10;
                    }
                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         * c_offset for extra ctrs are in pairs of PKT/BYTE. 
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];

                        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                            return SOC_E_UNAVAIL;
                        }
                    }
                    pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
                    *base_index += pipe_offset;
                    break;

                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
                case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
                case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
                    if (IS_CPU_PORT(unit, port)) {
                        *base_index = 32;
                    } else if (IS_LB_PORT(unit, port)) {
                        *base_index = (pipe % 2) * 34 + 33;
                    } else {
                        *base_index = (pipe % 2) * 34 + (mmu_port % 32);
                    }
                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                        (non_dma->extra_ctrs != NULL)) {
                    /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                    */
                    non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
                    }
                    *num_entries = 1;
                    break;

         	  	case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
	  	        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
	  	 	        *base_index = mmu_port & 0x3f;
	  	 	        if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
	  	 	            (non_dma->extra_ctrs != NULL)) {
	  	 	            /* Refer extra_ctrs_init for PKT/BYTE control block placement.
	  	 	             * c_offset for extra ctrs are in pairs of PKT/BYTE.
	  	 	             */
	  	 	            non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
	  	 	 
	  	 	            if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
	  	 	                return SOC_E_UNAVAIL;
	  	 	            }
	  	 	        }
	  	 	        pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
	  	 	        *base_index += pipe_offset;
	  	 	        *num_entries = 1;
	  	 	        break;
                case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
                case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
                case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
                case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
                case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
                case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         * c_offset for extra ctrs are in pairs of PKT/BYTE. 
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];

                        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                            return SOC_E_UNAVAIL;
                        }
                    }
                    *base_index = port * _TH_MMU_NUM_PG; 
                    *num_entries = non_dma->entries_per_port;
                    break;
                case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
                case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
                case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
                case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
                    *num_entries = si->port_num_cosq[port];
                    if (IS_CPU_PORT(unit, port)) {
                        *base_index = 330;
                    } else if (IS_LB_PORT(unit, port)) {
                        *base_index = 320;
                    } else {
                        *base_index = (mmu_port % 32) * 10;
                    }
                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         * c_offset for extra ctrs are in pairs of PKT/BYTE. 
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];

                        if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                            return SOC_E_UNAVAIL;
                        }
                    }
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_PIPE:
            pipe = ctrl_info.instance;
            if ((pipe >=  NUM_PIPE(unit)) || (pipe < -1) ) {
                return SOC_E_PARAM;
            }
            switch (id) {
                case SOC_COUNTER_NON_DMA_EFP_PKT:
                case SOC_COUNTER_NON_DMA_EFP_BYTE:
                    *base_index = 0;
                    *num_entries = non_dma->num_entries / NUM_PIPE(unit);
                    if (pipe > 0) {
                        pipe_offset = pipe * (*num_entries);
                        *base_index += pipe_offset;
                    }
                    break;
                default:
                    return SOC_E_INTERNAL;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_POOL:
            c_offset = ctrl_info.instance;
            if ((c_offset < 0) || (c_offset >= non_dma->extra_ctr_ct)) {
                return SOC_E_PARAM;
            }

            switch (id) {
                case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE: 
                case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
                    *base_index = 0;

                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
                    }
                    *num_entries = non_dma->num_entries / NUM_PIPE(unit);
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        case SOC_CTR_INSTANCE_TYPE_POOL_PIPE:
            c_offset = (ctrl_info.instance & 0x3F0) >> 4;
            pipe = ctrl_info.instance & 0xF;
            if ((c_offset < 0) || (c_offset >= non_dma->extra_ctr_ct) ||
                    (pipe >=  NUM_PIPE(unit)) || (pipe < 0)) {
                return SOC_E_PARAM;
            }
            switch (id) {
                case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE: 
                case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
                case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
                case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:

                    if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                            (non_dma->extra_ctrs != NULL)) {
                        /* Refer extra_ctrs_init for PKT/BYTE control block placement.
                         */
                        non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
                    }
                    *num_entries = non_dma->num_entries / NUM_PIPE(unit);
                    *base_index = (*num_entries)*pipe;
                    break;
                default:
                    return SOC_E_PARAM;
            }
            break;
        default:
            return SOC_E_PARAM;
    }
    *base_index += non_dma->base_index;
    return SOC_E_NONE;
}

int
soc_counter_tomahawk_get_info(int unit, soc_port_t port, soc_reg_t id,
                               int *base_index, int *num_entries)
{
    soc_info_t *si;
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma;
    soc_port_t phy_port, mmu_port;
    int pipe, pipe_offset;
    uint8 c_offset = 0; /* child offset in Parent-child model */

    soc = SOC_CONTROL(unit);

    /* For TH non-dma, id is Encoded with offset.
     * Bit 31    - sign bit (used incase of -1)
     * Bit 30:24 - offset (7 bits)
     * Bit 23:0  - id
     */
    c_offset = (id >> 24) & 0x7f;

    if (c_offset) {
        id &= 0x80ffffff;
    }

    if (id >= NUM_SOC_REG) {
        if (id >= SOC_COUNTER_NON_DMA_END) {
            return SOC_E_PARAM;
        }
    }

    non_dma = &soc->counter_non_dma[id - SOC_COUNTER_NON_DMA_START];

    if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID) &&
        !(non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* For Parent-Child model, return E_UNAVAIL if child non_dma is INVALID.
         */
        return SOC_E_UNAVAIL;
    }

    si = &SOC_INFO(unit);
    if (si->port_l2p_mapping[port] == -1) {
        *base_index = 0;
        *num_entries = 0;
        return SOC_E_NONE;
    }

    phy_port = si->port_l2p_mapping[port];
    mmu_port = si->port_p2m_mapping[phy_port];
    pipe = si->port_pipe[port];

    switch (id) {
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 0;
        } else {
            *base_index = pipe * 728 + 48 + (port % 34) * 20 + 10;
        }
        *num_entries = si->port_num_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC:
    case SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 0;
        } else {
            *base_index = pipe * 728 + 48 + (port % 34) * 20;
        }
        *num_entries = si->port_num_cosq[port];
        break;
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
    case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 330;
            *num_entries = 48;
        } else if (IS_LB_PORT(unit, port)) {
            *base_index = 320;
            *num_entries = 10;
        } else {
            *base_index = (mmu_port % 32) * 10;
            *num_entries = 10;
        }
        if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
            (non_dma->extra_ctrs != NULL)) {
            /* Refer extra_ctrs_init for PKT/BYTE control block placement.
             * c_offset for extra ctrs are in pairs of PKT/BYTE. 
             */
            non_dma = &non_dma->extra_ctrs[(c_offset << 1)];

            if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                return SOC_E_UNAVAIL;
            }
        }
        pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
        *base_index += pipe_offset;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
    case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        *base_index = mmu_port & 0x3f;
        if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
            (non_dma->extra_ctrs != NULL)) {
            /* Refer extra_ctrs_init for PKT/BYTE control block placement.
             * c_offset for extra ctrs are in pairs of PKT/BYTE. 
             */
            non_dma = &non_dma->extra_ctrs[(c_offset << 1)];

            if (!(non_dma->flags & _SOC_COUNTER_NON_DMA_VALID)) {
                return SOC_E_UNAVAIL;
            }
        }
        pipe_offset =  (non_dma->num_entries / NUM_PIPE(unit)) * pipe;
        *base_index += pipe_offset;
        *num_entries = 1;
        break;
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
    case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
    case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
        if (IS_CPU_PORT(unit, port)) {
            *base_index = 32;
        } else if (IS_LB_PORT(unit, port)) {
            *base_index = (pipe % 2) * 32 + 33;
        } else {
            *base_index = (pipe % 2) * 34 + (mmu_port % 32);
        }
        if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
            (non_dma->extra_ctrs != NULL)) {
            /* Refer extra_ctrs_init for PKT/BYTE control block placement.
             */
            non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
        }
        *num_entries = 1;
        break;
    case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE: 
    case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
    case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
    case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
        *base_index = 0;
        
        if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
            (non_dma->extra_ctrs != NULL)) {
            /* Refer extra_ctrs_init for PKT/BYTE control block placement.
             */
            non_dma = &non_dma->extra_ctrs[(c_offset << 1)];
        }
        *num_entries = non_dma->num_entries / NUM_PIPE(unit);
        break;
    case SOC_COUNTER_NON_DMA_EFP_PKT:
    case SOC_COUNTER_NON_DMA_EFP_BYTE:
        *base_index = 0;
        *num_entries = non_dma->num_entries / NUM_PIPE(unit);
        break;
    default:
        return SOC_E_INTERNAL;
    }
    *base_index += non_dma->base_index;
    return SOC_E_NONE;
}

int
soc_counter_tomahawk_status_enable(int unit, int enable)
{
    soc_reg_t reg;
    uint32 rval;
    int reg_id, count;
    static const soc_reg_t update_regs[] = {
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r,  /* MEMORY_ID 1 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r,  /* MEMORY_ID 2 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r,  /* MEMORY_ID 3 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r,  /* MEMORY_ID 4 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_4r,  /* MEMORY_ID 5 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_5r,  /* MEMORY_ID 6 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_6r,  /* MEMORY_ID 7 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_7r,  /* MEMORY_ID 8 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_8r,  /* MEMORY_ID 9 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_9r,  /* MEMORY_ID 10 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_10r, /* MEMORY_ID 11 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_11r, /* MEMORY_ID 12 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_12r, /* MEMORY_ID 13 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_13r, /* MEMORY_ID 14 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_14r, /* MEMORY_ID 15 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_15r, /* MEMORY_ID 16 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_16r, /* MEMORY_ID 17 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_17r, /* MEMORY_ID 18 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_18r, /* MEMORY_ID 19 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_19r, /* MEMORY_ID 20 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r,  /* MEMORY_ID 21 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r,  /* MEMORY_ID 22 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r,  /* MEMORY_ID 23 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r,  /* MEMORY_ID 24 */
        EGR_PERQ_CNTR_UPDATE_CONTROLr,           /* MEMORY_ID 25 */
        EGR_EFP_CNTR_UPDATE_CONTROLr             /* MEMORY_ID 26 */
    };

    count = sizeof(update_regs) / sizeof(update_regs[0]);
    rval = 0;
    soc_reg_field_set(unit, update_regs[0], &rval, COUNTER_POOL_ENABLEf,
                      enable?1:0);
    for (reg_id = 0; reg_id < count; reg_id++) {
        reg = update_regs[reg_id];
        SOC_IF_ERROR_RETURN
            (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
    }
    return SOC_E_NONE;
}

STATIC int
soc_counter_tomahawk_eviction_init(int unit)
{
    soc_control_t *soc;
    soc_counter_evict_t *evict;
    int pipe, mem_id, offset=0;
    int counter_id;
    static const struct {
        int pkt_counter_id;
        int byte_counter_id;
        soc_mem_t mem;
    } evict_info[] = {
        { /* MEMORY_ID 0 is reserved */
            -1,
            -1,
            INVALIDm
        },
        { /* MEMORY_ID 1 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_0m
        },
        { /* MEMORY_ID 2 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_1m
        },
        { /* MEMORY_ID 3 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_2m
        },
        { /* MEMORY_ID 4 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_3m
        },
        { /* MEMORY_ID 5 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_4m
        },
        { /* MEMORY_ID 6 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_5m
        },
        { /* MEMORY_ID 7 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_6m
        },
        { /* MEMORY_ID 8 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_7m
        },
        { /* MEMORY_ID 9 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_8m
        },
        { /* MEMORY_ID 10 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_9m
        },
        { /* MEMORY_ID 11 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_10m
        },
        { /* MEMORY_ID 12 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_11m
        },
        { /* MEMORY_ID 13 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_12m
        },
        { /* MEMORY_ID 14 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_13m
        },
        { /* MEMORY_ID 15 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_14m
        },
        { /* MEMORY_ID 16 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_15m
        },
        { /* MEMORY_ID 17 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_16m
        },
        { /* MEMORY_ID 18 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_17m
        },
        { /* MEMORY_ID 19 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_18m
        },
        { /* MEMORY_ID 20 */
            SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
            ING_FLEX_CTR_COUNTER_TABLE_19m
        },
        { /* MEMORY_ID 21 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_0m
        },
        { /* MEMORY_ID 22 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_1m
        },
        { /* MEMORY_ID 23 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_2m
        },
        { /* MEMORY_ID 24 */
            SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
            EGR_FLEX_CTR_COUNTER_TABLE_3m
        },
        { /* MEMORY_ID 25 */
            SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT,
            SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE,
            EGR_PERQ_XMT_COUNTERSm
        },
        { /* MEMORY_ID 26 */
            SOC_COUNTER_NON_DMA_EFP_PKT,
            SOC_COUNTER_NON_DMA_EFP_BYTE,
            EFP_COUNTER_TABLEm
        }
    };

    soc = SOC_CONTROL(unit);

    if (soc->counter_evict == NULL) {
        /* memory_id 0 is reserved, allocate 1 extra entries */
        soc->counter_evict = sal_alloc(sizeof(soc_counter_evict_t) * 27,
                                       "Tomahawk counter_evict");
        if (soc->counter_evict == NULL) {
            return SOC_E_MEMORY;
        }
    }

    counter_id = -1;
    for (mem_id = 1; mem_id < sizeof(evict_info) / sizeof(evict_info[0]);
         mem_id++) {
        evict = &soc->counter_evict[mem_id];
        evict->pkt_counter_id = evict_info[mem_id].pkt_counter_id;
        evict->byte_counter_id = evict_info[mem_id].byte_counter_id;
        if (counter_id != evict->pkt_counter_id) {
            offset = 0;
            counter_id = evict->pkt_counter_id;
        }
        for (pipe = 0; pipe < 4; pipe++) {
            evict->offset[pipe] = offset;
            evict->mem[pipe] =
                SOC_MEM_UNIQUE_ACC(unit, evict_info[mem_id].mem)[pipe];
            offset += soc_mem_index_count(unit, evict->mem[pipe]);
        }
    }

    return SOC_E_NONE;
}

int
soc_counter_tomahawk_dma_flags_update(int unit, soc_counter_non_dma_t *ctr_dma,
                                      uint32 flags, int val)
{
    int rv = SOC_E_NONE;
    int count = 0, subset_ct = 0;

    if ((ctr_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
        (ctr_dma->extra_ctrs != NULL)) {
        subset_ct = ctr_dma->extra_ctr_ct << 1;
        ctr_dma = ctr_dma->extra_ctrs;
    } else {
        subset_ct = 1;
    }

    do {
        switch (flags) {
            case _SOC_COUNTER_NON_DMA_CLEAR_ON_READ:
                /* Update only for CLR_ON_READ, until necessity calls for other
                 * FLAGS */
                if (ctr_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT) {
                    if (val) {
                         ctr_dma->flags |= _SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
                    } else {
                         ctr_dma->flags &= ~_SOC_COUNTER_NON_DMA_CLEAR_ON_READ;
                    }
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_ALL:
                if (ctr_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT) {
                    ctr_dma->dma_rate_profile = (val) ?
                                       _SOC_COUNTER_DMA_RATE_PROFILE_MASK : 0;
                }
                break;
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_AUTO:
            case SOC_CTR_CTRL_CONFIG_DMA_RATE_PROFILE_WFS:
                return SOC_E_UNAVAIL;
            default:
                return SOC_E_PARAM;
        }
        count++;
        ctr_dma++;
    } while ((count < subset_ct) && ctr_dma);

    return rv;
}

void
soc_counter_tomahawk_eviction_flags_update(int unit, uint32 flags, int enable)
{
    soc_control_t *soc;
    int idx = 0;
    soc_counter_non_dma_t *non_dma = NULL;
    soc = SOC_CONTROL(unit);

    if (soc->counter_non_dma == NULL) {
        /* Counter_non_dma will be NULL after soc_counter_detach.
         * Also soc_counter_detach is called during bcm_detach and soc_detach.
         * Hence sanity check before accessing the pointer.
         */
        return;
    }


    for (idx = 0; idx < SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START;
         idx++) {
        non_dma = &soc->counter_non_dma[idx];

        if (!non_dma || !(non_dma->flags & _SOC_COUNTER_NON_DMA_CTR_EVICT)) {
            continue;
        }
        soc_counter_tomahawk_dma_flags_update(unit, non_dma, flags, enable);
    }
    return;
}

int
soc_counter_tomahawk_eviction_enable(int unit, int enable)
{
    soc_reg_t reg;
    uint32 rval;
    uint64 rval64;
    int pipe, id, count;

    /* Counter Eviction cannot use Memory ID 0. */
    static const soc_reg_t evict_regs[] = {
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_0r,  /* MEMORY_ID 1 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_1r,  /* MEMORY_ID 2 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_2r,  /* MEMORY_ID 3 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_3r,  /* MEMORY_ID 4 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_4r,  /* MEMORY_ID 5 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_5r,  /* MEMORY_ID 6 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_6r,  /* MEMORY_ID 7 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_7r,  /* MEMORY_ID 8 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_8r,  /* MEMORY_ID 9 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_9r,  /* MEMORY_ID 10 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_10r, /* MEMORY_ID 11 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_11r, /* MEMORY_ID 12 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_12r, /* MEMORY_ID 13 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_13r, /* MEMORY_ID 14 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_14r, /* MEMORY_ID 15 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_15r, /* MEMORY_ID 16 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_16r, /* MEMORY_ID 17 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_17r, /* MEMORY_ID 18 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_18r, /* MEMORY_ID 19 */
        ING_FLEX_CTR_EVICTION_CONTROL_POOL_19r, /* MEMORY_ID 20 */
        EGR_FLEX_CTR_EVICTION_CONTROL_POOL_0r,  /* MEMORY_ID 21 */
        EGR_FLEX_CTR_EVICTION_CONTROL_POOL_1r,  /* MEMORY_ID 22 */
        EGR_FLEX_CTR_EVICTION_CONTROL_POOL_2r,  /* MEMORY_ID 23 */
        EGR_FLEX_CTR_EVICTION_CONTROL_POOL_3r,  /* MEMORY_ID 24 */
        EGR_PERQ_EVICTION_CONTROLr              /* MEMORY_ID 25 */
        /* EFP_COUNTER_TABLE - Counter Eviction is disabled */
    };
    static const soc_reg_t seed_regs[] = {
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_0r,  /* MEMORY_ID 1 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_1r,  /* MEMORY_ID 2 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_2r,  /* MEMORY_ID 3 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_3r,  /* MEMORY_ID 4 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_4r,  /* MEMORY_ID 5 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_5r,  /* MEMORY_ID 6 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_6r,  /* MEMORY_ID 7 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_7r,  /* MEMORY_ID 8 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_8r,  /* MEMORY_ID 9 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_9r,  /* MEMORY_ID 10 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_10r, /* MEMORY_ID 11 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_11r, /* MEMORY_ID 12 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_12r, /* MEMORY_ID 13 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_13r, /* MEMORY_ID 14 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_14r, /* MEMORY_ID 15 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_15r, /* MEMORY_ID 16 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_16r, /* MEMORY_ID 17 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_17r, /* MEMORY_ID 18 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_18r, /* MEMORY_ID 19 */
        ING_FLEX_CTR_EVICTION_LFSR_SEED_POOL_19r, /* MEMORY_ID 20 */
        EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_0r,  /* MEMORY_ID 21 */
        EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_1r,  /* MEMORY_ID 22 */
        EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_2r,  /* MEMORY_ID 23 */
        EGR_FLEX_CTR_EVICTION_LFSR_SEED_POOL_3r,  /* MEMORY_ID 24 */
        EGR_PERQ_EVICTION_SEEDr                   /* MEMORY_ID 25 */
        /* EFP_COUNTER_TABLE - Counter Eviction is disabled */
    };
    static const soc_reg_t update_regs[] = {
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r,  /* MEMORY_ID 1 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r,  /* MEMORY_ID 2 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r,  /* MEMORY_ID 3 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r,  /* MEMORY_ID 4 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_4r,  /* MEMORY_ID 5 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_5r,  /* MEMORY_ID 6 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_6r,  /* MEMORY_ID 7 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_7r,  /* MEMORY_ID 8 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_8r,  /* MEMORY_ID 9 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_9r,  /* MEMORY_ID 10 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_10r, /* MEMORY_ID 11 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_11r, /* MEMORY_ID 12 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_12r, /* MEMORY_ID 13 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_13r, /* MEMORY_ID 14 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_14r, /* MEMORY_ID 15 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_15r, /* MEMORY_ID 16 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_16r, /* MEMORY_ID 17 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_17r, /* MEMORY_ID 18 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_18r, /* MEMORY_ID 19 */
        ING_FLEX_CTR_COUNTER_UPDATE_CONTROL_19r, /* MEMORY_ID 20 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_0r,  /* MEMORY_ID 21 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_1r,  /* MEMORY_ID 22 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_2r,  /* MEMORY_ID 23 */
        EGR_FLEX_CTR_COUNTER_UPDATE_CONTROL_3r,  /* MEMORY_ID 24 */
        EGR_PERQ_CNTR_UPDATE_CONTROLr            /* MEMORY_ID 25 */
        /* EFP_COUNTER_TABLE - Counter Eviction is disabled */
    };

    if (!(SOC_CONTROL(unit)->soc_flags & SOC_F_INITED)) {
        /* Soc is Not yet initialized. Do Nothing.
         */
        return SOC_E_NONE;
    }

    count = sizeof(evict_regs) / sizeof(evict_regs[0]);
    if (enable) {
        rval = 0;
        reg = CENTRAL_CTR_EVICTION_CONTROLr;
        soc_reg_field_set(unit, reg, &rval, NUM_CE_PER_PIPEf, count);
        soc_reg_field_set(unit, reg, &rval, FIFO_ENABLEf, 1);
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));

        rval = 0;
        soc_reg_field_set(unit, evict_regs[0], &rval, MODEf, 1);
        for (id = 0; id < count; id++) {
            soc_reg_field_set(unit, evict_regs[id], &rval, MEMORY_IDf, id + 1);
            for (pipe = 0; pipe < 4; pipe++) {
                reg = SOC_REG_UNIQUE_ACC(unit, evict_regs[id])[pipe];
                soc_reg_field_set(unit, reg, &rval, PIPE_IDf, pipe);
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
            }
        }

        COMPILER_64_ZERO(rval64);
        for (id = 0; id < count; id++) {
            for (pipe = 0; pipe < 4; pipe++) {
                reg = SOC_REG_UNIQUE_ACC(unit, seed_regs[id])[pipe];
                /* Arbitrarily pick a different seed for each table */
                soc_reg64_field32_set(unit, reg, &rval64, SEEDf,
                                      ((id + 1) << 16) + pipe);
                SOC_IF_ERROR_RETURN
                    (soc_reg64_set(unit, reg, REG_PORT_ANY, 0, rval64));
            }
        }

        rval = 0;
        soc_reg_field_set(unit, update_regs[0], &rval, COUNTER_POOL_ENABLEf,
                          1);
        soc_reg_field_set(unit, update_regs[0], &rval, CLR_ON_READf, 1);
        for (id = 0; id < count; id++) {
            reg = update_regs[id];
            SOC_IF_ERROR_RETURN
                (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }
    } else {
        for (pipe = 0; pipe < 4; pipe++) {
            for (id = 0; id < count; id++) {
                reg = SOC_REG_UNIQUE_ACC(unit, evict_regs[id])[pipe];
                SOC_IF_ERROR_RETURN
                    (soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
            }
        }

        /* Leave the seed regs as is */

        for (id = 0; id < count; id++) {
            rval = 0;
            /* Always have Counter enabled */
            soc_reg_field_set(unit, update_regs[0], &rval, COUNTER_POOL_ENABLEf,
                              1);
            reg = update_regs[id];
            SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, rval));
        }

        reg = CENTRAL_CTR_EVICTION_CONTROLr;
        SOC_IF_ERROR_RETURN(soc_reg32_set(unit, reg, REG_PORT_ANY, 0, 0));
    }

    return SOC_E_NONE;
}

STATIC int
soc_counter_th_extra_ctrs_init(int unit, soc_reg_t id,
                               soc_counter_non_dma_t *non_dma_parent,
                               soc_counter_non_dma_t *non_dma_extra,
                               uint32 extra_ctr_ct,
                               int *total_entries)
{
    int i, pipe, count = 0;
    soc_mem_t *base_mem_array = NULL, base_mem = INVALIDm;
    int base_mem_ct = 0;
    int parent_base_index = 0;
    uint8 mmu_mem = FALSE;
    int xpe;

    soc_mem_t ing_flex_mems[] = {
        ING_FLEX_CTR_COUNTER_TABLE_0m,   /* MEMORYID 1 */
        ING_FLEX_CTR_COUNTER_TABLE_1m,   /* MEMORYID 2 */
        ING_FLEX_CTR_COUNTER_TABLE_2m,   /* MEMORYID 3 */
        ING_FLEX_CTR_COUNTER_TABLE_3m,   /* MEMORYID 4 */
        ING_FLEX_CTR_COUNTER_TABLE_4m,   /* MEMORYID 5 */
        ING_FLEX_CTR_COUNTER_TABLE_5m,   /* MEMORYID 6 */
        ING_FLEX_CTR_COUNTER_TABLE_6m,   /* MEMORYID 7 */
        ING_FLEX_CTR_COUNTER_TABLE_7m,   /* MEMORYID 8 */
        ING_FLEX_CTR_COUNTER_TABLE_8m,   /* MEMORYID 9 */
        ING_FLEX_CTR_COUNTER_TABLE_9m,   /* MEMORYID 10 */
        ING_FLEX_CTR_COUNTER_TABLE_10m,  /* MEMORYID 11 */
        ING_FLEX_CTR_COUNTER_TABLE_11m,  /* MEMORYID 12 */
        ING_FLEX_CTR_COUNTER_TABLE_12m,  /* MEMORYID 13 */
        ING_FLEX_CTR_COUNTER_TABLE_13m,  /* MEMORYID 14 */
        ING_FLEX_CTR_COUNTER_TABLE_14m,  /* MEMORYID 15 */
        ING_FLEX_CTR_COUNTER_TABLE_15m,  /* MEMORYID 16 */
        ING_FLEX_CTR_COUNTER_TABLE_16m,  /* MEMORYID 17 */
        ING_FLEX_CTR_COUNTER_TABLE_17m,  /* MEMORYID 18 */
        ING_FLEX_CTR_COUNTER_TABLE_18m,  /* MEMORYID 19 */
        ING_FLEX_CTR_COUNTER_TABLE_19m   /* MEMORYID 20 */
    };

    soc_mem_t egr_flex_mems[] = {
        EGR_FLEX_CTR_COUNTER_TABLE_0m,   /* MEMORYID 21 */
        EGR_FLEX_CTR_COUNTER_TABLE_1m,   /* MEMORYID 22 */
        EGR_FLEX_CTR_COUNTER_TABLE_2m,   /* MEMORYID 23 */
        EGR_FLEX_CTR_COUNTER_TABLE_3m   /* MEMORYID 24 */
    };

    /* DO SANITY CHECK FOR INPUT PARAMs */
    if (!(non_dma_parent->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT)) {
        /* Only for Extra counters */
        return SOC_E_PARAM;
    }

    parent_base_index = non_dma_parent->base_index;
    *total_entries = 0;

    switch (id) {
        case SOC_COUNTER_NON_DMA_ING_FLEX_PKT:
        case SOC_COUNTER_NON_DMA_ING_FLEX_BYTE:
            base_mem_array = ing_flex_mems;
            count = sizeof(ing_flex_mems) / sizeof(ing_flex_mems[0]);
            break;
        case SOC_COUNTER_NON_DMA_EGR_FLEX_PKT:
        case SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE:
            base_mem_array = egr_flex_mems;
            count = sizeof(egr_flex_mems) / sizeof(egr_flex_mems[0]);
            break;
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC:
        case SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC:
        case SOC_COUNTER_NON_DMA_QUEUE_PEAK:
        case SOC_COUNTER_NON_DMA_QUEUE_CURRENT:
        case SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK:
        case SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT:
        case SOC_COUNTER_NON_DMA_PG_MIN_PEAK:
        case SOC_COUNTER_NON_DMA_PG_MIN_CURRENT:
        case SOC_COUNTER_NON_DMA_PG_SHARED_PEAK:
        case SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT:
        case SOC_COUNTER_NON_DMA_PG_HDRM_PEAK:
        case SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING:
        case SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
            mmu_mem = TRUE;
            count = NUM_XPE(unit);

            /* Get the mem attributes(Max idx) from the first child,
             * which could be used for other instances.
             * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
             */
            base_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma_parent->mem,
                                                   0, 0);
            if (base_mem != INVALIDm) {
                /* XPE 0/PIPE 0 combination is true for both IP/EP */
                base_mem_ct = soc_mem_index_max(unit, base_mem) + 1;
            }
            break;
        default:
            return SOC_E_INTERNAL;
    }

    if (extra_ctr_ct > count) {
        return SOC_E_INTERNAL;
    }

    for (i = 0; i < count; i++) {
        if (mmu_mem == FALSE) {
            non_dma_extra->mem = base_mem_array[i];
            base_mem_ct = soc_mem_index_count(unit, non_dma_extra->mem);

            /* Set Child attributes */
            non_dma_extra->base_index = parent_base_index + *total_entries;
            non_dma_extra->num_entries = NUM_PIPE(unit) * base_mem_ct;

            /* Inherit few info from the Parent */
            non_dma_extra->flags = (non_dma_parent->flags &
                                    ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                    _SOC_COUNTER_NON_DMA_SUBSET_CHILD;

            non_dma_extra->cname = sal_alloc(sal_strlen(non_dma_parent->cname) + 8,
                                             "Extra ctrs cname");

            if (non_dma_extra->cname == NULL) {
                return SOC_E_MEMORY;
            }
            sal_sprintf(non_dma_extra->cname, "%s_PL%d",
                        non_dma_parent->cname, i);
            
            non_dma_extra->field = non_dma_parent->field;
            non_dma_extra->reg = non_dma_parent->reg;

            for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                non_dma_extra->dma_mem[pipe] = 
                    SOC_MEM_UNIQUE_ACC(unit, non_dma_extra->mem)[pipe];

                non_dma_extra->dma_index_max[pipe] = 
                    soc_mem_index_max(unit, non_dma_extra->dma_mem[pipe]);
                non_dma_extra->dma_index_min[pipe] = 0;
                non_dma_extra->dma_buf[pipe] = non_dma_parent->dma_buf[pipe];
                *total_entries += (non_dma_extra->dma_index_max[pipe] -
                                   non_dma_extra->dma_index_min[pipe] + 1);
            }
            /* Allocation of Child Control blocks.
             * [0],..<even indices> - PKT_CTR non_dma ctrl block
             * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
             * Hence in below code, jump by 2, so all similar control blocks are
             * initilized at the same time.
             */
            non_dma_extra += 2;
            /* coverity[check_after_deref] */
            if (!non_dma_extra) {
                /* ERR: Cannot be NULL, until 'i reaches 'count' */
                /* This condition may not hit but intentional for defensive check */
                /* coverity[dead_error_line] */
                return SOC_E_INTERNAL;
            }
        } else { /* (mmu_mem == TRUE) */
            int entries_per_xpe = 0;
            int entries_per_pipe = 0;
            xpe = i;
            /* Inherit few info from the Parent */
            non_dma_extra->flags = (non_dma_parent->flags &
                                    ~_SOC_COUNTER_NON_DMA_SUBSET_PARENT) |
                                    _SOC_COUNTER_NON_DMA_SUBSET_CHILD;
            non_dma_extra->cname = non_dma_parent->cname;
            non_dma_extra->field = non_dma_parent->field;
            non_dma_extra->reg = non_dma_parent->reg;
            non_dma_extra->entries_per_port = non_dma_parent->entries_per_port;
            non_dma_extra->id = non_dma_parent->id;

            /* Set Child attributes */
            non_dma_extra->base_index = parent_base_index + *total_entries;
            if (non_dma_parent->mem != MMU_CTR_COLOR_DROP_MEMm) {
                for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
                    non_dma_extra->dma_mem[pipe] =
                        SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma_parent->mem,
                                                    xpe, pipe);
                    non_dma_extra->dma_index_min[pipe] = 0;

                    if (non_dma_extra->dma_mem[pipe] != INVALIDm) {
                        non_dma_extra->dma_index_max[pipe] = base_mem_ct - 1;
                        non_dma_extra->dma_buf[pipe] = non_dma_parent->dma_buf[pipe];
                    } else {
                        non_dma_extra->dma_index_max[pipe] = -1;
                    }

                    entries_per_pipe = (non_dma_extra->dma_index_max[pipe] -
                                       non_dma_extra->dma_index_min[pipe] + 1);
                    entries_per_xpe += entries_per_pipe;
                    *total_entries += entries_per_pipe;
                }
                non_dma_extra->num_entries = entries_per_xpe;
                non_dma_extra->mem = non_dma_parent->mem;
            } else {
                non_dma_extra->dma_mem[0] = 
                    SOC_MEM_UNIQUE_ACC(unit, non_dma_parent->mem)[xpe];
                non_dma_extra->dma_mem[1] = non_dma_extra->dma_mem[2] =
                non_dma_extra->dma_mem[3] = INVALIDm;
                if (non_dma_extra->dma_mem[0] != INVALIDm) {
                    non_dma_extra->dma_buf[0] = non_dma_parent->dma_buf[0];
                    switch(non_dma_extra->id) {
                        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED:
                            non_dma_extra->dma_index_min[0] = 0;
                            non_dma_extra->dma_index_max[0] = 67;
                            break;
                        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW:
                            non_dma_extra->dma_index_min[0] = 68;
                            non_dma_extra->dma_index_max[0]= 135;
                            break;              
                        case SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN:
                            non_dma_extra->dma_index_min[0] = 136;
                            non_dma_extra->dma_index_max[0] = 203;
                        break;
                        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED:
                            non_dma_extra->dma_index_min[0] = 204;
                            non_dma_extra->dma_index_max[0] = 271;
                            break;
                        case SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW:
                            non_dma_extra->dma_index_min[0] = 272;
                            non_dma_extra->dma_index_max[0] = 339;
                            break;
                        default:
                            return SOC_E_PARAM;
                    }
                    entries_per_xpe = 68;
                    *total_entries += entries_per_xpe;
                    non_dma_extra->mem = non_dma_parent->mem;
                    non_dma_extra->num_entries += entries_per_xpe;
                } else {
                        non_dma_extra->dma_index_min[0] = 0;
                        non_dma_extra->dma_index_max[0] = -1;
                }
            }


            /* Allocation of Child Control blocks.
             * [0],..<even indices> - PKT_CTR non_dma ctrl block
             * [1],..<odd indices> - BYTE_CTR non_dma ctrl block
             * Hence in below code, jump by 2, so all similar control blocks are
             * initilized at the same time.
             */
            non_dma_extra += 2;
            /* coverity[check_after_deref] */
            if (!non_dma_extra) {
                /* ERR: Cannot be NULL, until 'i reaches 'count' */
                /* coverity[dead_error_line] */
                return SOC_E_INTERNAL;
            }
        }

    }

    return SOC_E_NONE;
}

/* Temoporary function to disable few non_dma counters enabled during init.
 */
STATIC int
soc_counter_tomahawk_non_dma_post_init(int unit)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma = NULL;
    int id = 0, i, idx_max = 0;
    uint32 count = 0, subset_ct = 0; /* For Parent-Child Model */
    int disable_ids[] = {
        SOC_COUNTER_NON_DMA_QUEUE_PEAK,
        SOC_COUNTER_NON_DMA_QUEUE_CURRENT,
        SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK,
        SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT,
        SOC_COUNTER_NON_DMA_PG_SHARED_PEAK,
        SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT,
        SOC_COUNTER_NON_DMA_PG_HDRM_PEAK,
        SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT,
        SOC_COUNTER_NON_DMA_EFP_BYTE,
        SOC_COUNTER_NON_DMA_EFP_PKT,
        SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
        SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
        SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
        SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
        SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
        SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING 
    };

    soc = SOC_CONTROL(unit);

    idx_max = sizeof(disable_ids)/sizeof(int);

    for (i = 0; i < idx_max; i++) {
        for (id = 0; id < SOC_COUNTER_NON_DMA_END - SOC_COUNTER_NON_DMA_START;
             id++) {
            if (id != (disable_ids[i] - SOC_COUNTER_NON_DMA_START)) {
                continue;
            }

            non_dma = &soc->counter_non_dma[id];

            /* If there is subset of counters, use subset info for DMA */
            if ((non_dma->flags & _SOC_COUNTER_NON_DMA_SUBSET_PARENT) &&
                (non_dma->extra_ctrs != NULL)) {
                /* Do PACKET_CTRf and BYTE_CTRf sequentially. Hence 2x loop */
                subset_ct = non_dma->extra_ctr_ct << 1;
                non_dma->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
                non_dma = non_dma->extra_ctrs;
            } else {
                subset_ct = 1;
            }

            do {
                non_dma->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
                count++;
                non_dma++;
            } while ((count < subset_ct) && non_dma);
        }
    }

    return SOC_E_NONE;
}

/* Tomahawk: Counters are made of 2 types: 
 * 1. Traditional Non-StatDma
 * 2. Counter Eviction elligible counters. 
 *
 * When Counter Eviction is enabled, HW performs Clear on Read on the eviction
 * enabled counters when a Read is performed.
 *
 * When Counter Eviction is disabled, all counters will work as in TD2.
 */
int
soc_counter_tomahawk_non_dma_init(int unit, int nports, 
                                   int non_dma_start_index, 
                                   int *non_dma_entries)
{
    soc_control_t *soc;
    soc_counter_non_dma_t *non_dma0, *non_dma1, *non_dma2;
    int rv = SOC_E_NONE, pipe;
    int num_entries, alloc_size, entry_words, num_extra_ctr_entries;
    int pool_count = 0;
    soc_mem_t child_mem = INVALIDm;
    uint32 *buf;
    soc_counter_control_t *soc_ctr_ctrl = SOC_CTR_CTRL(unit);

    if (soc_ctr_ctrl == NULL) {
        return SOC_E_INIT;
    }

    soc = SOC_CONTROL(unit);
    *non_dma_entries = 0;

    /* Registration of Chip specific functions to the Global Counter 
     * control structure.
     */
    soc_ctr_ctrl->get_child_dma = soc_counter_tomahawk_get_child_dma;
    soc_ctr_ctrl->dma_config_update = soc_counter_tomahawk_dma_flags_update;

    /* ING_FLEX_CTR_COUNTER_TABLE_0 is the largest table to be DMA'ed */
    num_entries = soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    alloc_size = NUM_PIPE(unit) * num_entries * sizeof(uint32) * entry_words;

    buf = soc_cm_salloc(unit, alloc_size, "non_dma_counter");
    if (buf == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(buf, 0, alloc_size);
 
    /* Ctrl block for EGR_PERQ */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_ALLOC |
                      _SOC_COUNTER_NON_DMA_CTR_EVICT;

    /* Clear on Read Flag set if eviction is enabled */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* cpu port has max number of queues */
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    non_dma0->mem = EGR_PERQ_XMT_COUNTERSm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "PERQ_PKT";
    num_entries = soc_mem_index_count(unit, EGR_PERQ_XMT_COUNTERSm);
    entry_words = soc_mem_entry_words(unit, EGR_PERQ_XMT_COUNTERSm);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] = 
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;
    /* Clear on Read Flag set if eviction is enabled */
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma0->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "PERQ_BYTE";
    *non_dma_entries += non_dma1->num_entries;


    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_PKT_UC;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    /* Clear on Read Flag set if eviction is enabled */
    non_dma2->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma2->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10; /* 10 UC Qs Per port */
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_PKT";

    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_EGR_PERQ_XMT_BYTE_UC;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID;
    /* Clear on Read Flag set if eviction is enabled */
    non_dma2->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma2->dma_rate_profile |= _SOC_COUNTER_DMA_RATE_PROFILE0;

    non_dma2->pbmp = PBMP_PORT_ALL(unit);
    non_dma2->entries_per_port = 10; /* 10 UC Qs Per port */
    non_dma2->num_entries = 0;
    non_dma2->cname = "UC_PERQ_BYTE";

    /* Ctrl block for DROP_PKT_MC */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_CTR_MC_DROP_MEMm;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "PERQ_DROP_PKT";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_perq_drop_mc");


    /*
     * Below represents how Extra cntrs are arranged
     * for Per Queue(UC|MC) Drop Counters.
     * n = NUM_XPE
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | BYTE || BYTE |...| BYTE |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

 
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE;

    /* FLAGS - Notes
     * 1. Flag DO_DMA removed as BYTE descriptor need NOT do DMA.
     * DMA already performed in PACKET descriptor.
     *
     * 2. In Parent-child model based counters, BYTE counter Parent need
     * to contain DMA_VALID flag, long enough to setup the Children during
     * extra_ctrs_init. 
     * Post children INIT, byte counter parent's DMA_VALID flag NEED to be
     * set to FALSE, so during Counter thread it is ignored.
     */
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "PERQ_DROP_BYTE";
    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs) {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    *non_dma_entries += non_dma1->num_entries;
    /* End Ctrl block for DROP_PKT_MC */


    /* Ctrl block for DROP_PKT_UC */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_PORT_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 10; /* 10 UC queues per port */
    non_dma0->num_entries = soc_mem_index_count(unit, MMU_CTR_UC_DROP_MEMm);
    non_dma0->mem = MMU_CTR_UC_DROP_MEMm;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "PERQ_DROP_PKT_UC";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_perq_drop_uc");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_PKT_UC -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC;

    /* FLAGS - Notes
     * Refer notes init of ID - SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE
     */
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "PERQ_DROP_BYTE_UC";

    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE_UC -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    *non_dma_entries += non_dma1->num_entries;


    /* Ctrl block for DROP_PKT_MC */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_CTR_ING_DROP_MEMm;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = PKTCNTf;
    non_dma0->cname = "DROP_PKT_ING";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_drop");


    /*
     * Below represents how Extra cntrs are arranged
     * for Per Queue(UC|MC) Drop Counters.
     * n = NUM_XPE
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | BYTE || BYTE |...| BYTE |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_PKT_ING -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

 
    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING;

    /* FLAGS - Notes
     * Refer notes init of ID - SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE
     */
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTECNTf;
    non_dma1->cname = "DROP_BYTE_ING";
    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs) {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_BYTE_ING -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    *non_dma_entries += non_dma1->num_entries;
    /* End Ctrl block for DROP_PKT_MC */

    /* Ctrl block for wred red counter */

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID | 
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 1;
    non_dma0->mem = MMU_CTR_COLOR_DROP_MEMm;
    non_dma0->reg = INVALIDr;
    non_dma0->cname = "WRED_PKT_RED";
    non_dma0->field = PKTCNTf;
    
    child_mem = SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[0];
    num_entries = soc_mem_index_max(unit,child_mem) + 1;
    non_dma0->num_entries = num_entries * NUM_XPE(unit);
    
    non_dma0->dma_buf[0] = &buf[0];

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */
    
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_wred_pkt_red");
    
    /*
     * Below represents how Extra cntrs are arranged for wred red pkt, wred
     * yellow pkt, wred green pkt, drop pkt red, drop pkt yellow counters.
     *
     *  n = NUM_XPE
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | PKT  || PKT  |...| PKT  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | N/A  || N/A  |...| N/A  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_WRED_PKT_RED -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    /* Ctrl block for wred yellow counter */

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_YEL";
    
    non_dma1->dma_buf[0] = &buf[0];
    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_wred_pkt_yel");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_WRED_PKT_YELLOW -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Ctrl block for wred green counter */

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "WRED_PKT_GRE";
    non_dma1->dma_buf[0] = &buf[0];

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */
    
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_wred_pkt_gre");

    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_WRED_PKT_GREEN -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Ctrl block for Red Drop counter */

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_RED";
    non_dma1->dma_buf[0] = &buf[0];

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */
    
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_drop_pkt_red");
    
    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_PKT_RED -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Ctrl block for Yellow Drop counter */

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->cname = "DROP_PKT_YEL";
    non_dma1->dma_buf[0] = &buf[0];

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used. */
    
    non_dma1->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_drop_pkt_yel");
    
    if (non_dma1->extra_ctrs == NULL) {
        non_dma1->extra_ctr_ct = 0;
        non_dma1->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma1->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PORT_DROP_PKT_YELLOW -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma1->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma1->num_entries;

    /* Ctrl block for MC Queue Count */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_QUEUE_PEAK;

    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_THDM_DB_QUEUE_COUNTm;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "QUEUE_PEAK";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports Min Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_mc_q_ct");

    /*
     * Below represents how Extra cntrs are arranged 
     * for Q(MC) SHD counters.
     * n = NUM_XPE
     *
     *   [0]     [2]        [2n-2]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | SHD  || SHD  |...| SHD  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | N/A  || N/A  |...| N/A  |
     * | [0]  || [1]  |...| [n-1]|
     *  ------  ------     ------
     *   [1]     [3]        [2n-1]
     */

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_QUEUE_PEAK,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_QUEUE_PEAK -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

 
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_QUEUE_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "QUEUE_CUR";
    /* End Ctrl block for MC Queue Count */
 
    /* Ctrl block for UC Queue Count */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK;

    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 48; /* 48 for cpu port */
    non_dma0->mem = MMU_THDU_COUNTER_QUEUEm;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = SHARED_COUNTf;
    non_dma0->cname = "UC_QUEUE_PEAK";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports Min Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_uc_q_ct");

    /*
     * Below represents how Extra cntrs are arranged 
     * for Q(UC) SHD counters.
     * n = NUM_XPE
     *
     *   [0]     [2]        [2n]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | SHD  || SHD  |...| SHD  |
     * | [0]  || [1]  |...| [n]  |
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | N/A  || N/A  |...| N/A  |
     * | [0]  || [1]  |...| [n]  |
     *  ------  ------     ------
     *   [1]     [3]        [2n+1]
     */
 
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_UC_QUEUE_PEAK -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

 
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_UC_QUEUE_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "UC_QUEUE_CUR";
    /* End Ctrl block for MC Queue Count */

    /* Ctrl block for PG Counters */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG_MIN_PEAK;

    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT1m;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_MIN_COUNTf;
    non_dma0->cname = "PG_MIN_PEAK";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);
    /* Incl. buffer for SHD_CNTR. 2x alloc size
     * (Note: SDH_CNTR instead of BYTE CNTR present) */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_pg_ctrs");

    /*
     * Below represents how Extra cntrs are arranged 
     * for PG MIN and Shared(SHD) counters.
     * n = NUM_XPE
     *
     *   [0]     [2]        [2n]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | MIN  || MIN  |...| MIN  |
     * | [0]  || [1]  |...| [n]  |
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | SHD  || SHD  |...| SHD  |
     * | [0]  || [1]  |...| [n]  |
     *  ------  ------     ------
     *   [1]     [3]        [2n+1]
     */

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PG_MIN_PEAK,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PG_MIN_PEAK -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

 
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_MIN_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG_MIN_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "PG_MIN_CUR";

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_PG_SHARED_PEAK;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma1->cname = "PG_SHARED_PEAK";
    non_dma1->field = PG_SHARED_COUNTf;
    non_dma1->extra_ctr_ct = NUM_XPE(unit);

    /* Memory for extra ctrs for SHD_CNTRS are done along with MIN_CTRS */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PG_SHARED_PEAK,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PG_SHARED_PEAK -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    *non_dma_entries += non_dma1->num_entries;
 
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma1;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG_SHARED_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "PG_SHARED_CUR";
 
 

    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_PG_HDRM_PEAK;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_PEAK |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma0->pbmp = PBMP_ALL(unit);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 8;
    non_dma0->mem = THDI_PORT_PG_CNTRS_RT2m;

    /* Get the mem attributes(Max idx) from the first child,
     * which could be used for other instances.
     * Note: In TH, XPE 0/Pipe 0 instance valid for all memories(Ing/Egr).
     */
    child_mem = SOC_MEM_UNIQUE_ACC_XPE_PIPE(unit, non_dma0->mem, 0, 0);
    num_entries = soc_mem_index_max(unit, child_mem) + 1;
    entry_words = soc_mem_entry_words(unit, non_dma0->mem);
    non_dma0->num_entries = NUM_PIPE(unit) * num_entries;

    non_dma0->reg = INVALIDr;
    non_dma0->field = PG_HDRM_COUNTf;
    non_dma0->cname = "PG_HDRM_PEAK";

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = NUM_XPE(unit);

    /* Buffer allocated for 2x alloc size. Though 2nd half is NOT used.
     * This memory supports Min Count, but not implemented yet. So space
     * reserved.
     */
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_hdrm_ctrs");

    /*
     * Below represents how Extra cntrs are arranged 
     * for PG HDRM counters.
     * n = NUM_XPE
     *
     *   [0]     [2]        [2n]
     *  ------  ------     ------ 
     * |      ||      |...|      |
     * | HDRM || HDRM |...| HDRM |
     * | [0]  || [1]  |...| [n]  |
     *  ------  ------ ... ------ 
     * |      ||      |...|      |
     * | N/A  || N/A  |...| N/A  |
     * | [0]  || [1]  |...| [n]  |
     *  ------  ------     ------
     *   [1]     [3]        [2n+1]
     */
 
    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_PG_HDRM_PEAK,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_PG_HDRM_PEAK -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

 
    non_dma2 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma2 = *non_dma0;
    non_dma2->id = SOC_COUNTER_NON_DMA_PG_HDRM_CURRENT;
    non_dma2->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_CURRENT |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    non_dma2->cname = "PG_HDRM_CUR";
    /* End Ctrl block for PG Counters */


    /* Ctrl block for ING_FLEX */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_ING_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    /* Clear on Read Flag set if eviction is enabled */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    pool_count = 20;
    non_dma0->num_entries =
        pool_count * NUM_PIPE(unit) * 
                        soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    non_dma0->mem = ING_FLEX_CTR_COUNTER_TABLE_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "ING_FLEX_PKT";

    num_entries = soc_mem_index_count(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, ING_FLEX_CTR_COUNTER_TABLE_0m);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = pool_count; 
    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_flex");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit, SOC_COUNTER_NON_DMA_ING_FLEX_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_ING_FLEX_PKT -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
    *non_dma_entries += non_dma0->num_entries;

    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_ING_FLEX_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_ING_FLEX_BYTE;

    /* FLAGS - Notes
     * Refer notes init of ID - SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE
     */
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    /* Clear on Read Flag set if eviction is enabled */
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "ING_FLEX_BYTE";
    non_dma1->extra_ctr_ct = pool_count; /* Taken care by PKT_CTR */

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_ING_FLEX_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_ING_FLEX_BYTE -
                     SOC_COUNTER_NON_DMA_START));
        }
    }

    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    *non_dma_entries += non_dma1->num_entries;


    /* Ctrl block for EGR_FLEX */
    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_FLEX_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EGR_FLEX_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;

    /* Clear on Read Flag set if eviction is enabled */
    non_dma0->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    pool_count = 4;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        pool_count * NUM_PIPE(unit) *
            soc_mem_index_count(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    non_dma0->mem = EGR_FLEX_CTR_COUNTER_TABLE_0m;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "EGR_FLEX_PKT";

    num_entries = soc_mem_index_count(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    entry_words = soc_mem_entry_words(unit, EGR_FLEX_CTR_COUNTER_TABLE_0m);
    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
    }

    non_dma0->extra_ctr_ct = pool_count; 

    /* Incl. buffer for BYTE_CTR. 2x alloc size*/
    alloc_size = (non_dma0->extra_ctr_ct << 1) * sizeof(soc_counter_non_dma_t);
    non_dma0->extra_ctrs = sal_alloc(alloc_size, "non_dma_ctrs_ing_flex");

    if (non_dma0->extra_ctrs == NULL) {
        non_dma0->extra_ctr_ct = 0;
        non_dma0->flags &= ~_SOC_COUNTER_NON_DMA_VALID;
        return SOC_E_MEMORY;
    } else {
        sal_memset(non_dma0->extra_ctrs, 0, alloc_size);
        rv = soc_counter_th_extra_ctrs_init(unit, SOC_COUNTER_NON_DMA_EGR_FLEX_PKT,
                                            non_dma0,
                                            non_dma0->extra_ctrs,
                                            non_dma0->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_EGR_FLEX_PKT -
                     SOC_COUNTER_NON_DMA_START));
        }
        non_dma0->num_entries = num_extra_ctr_entries;
    }
 
    *non_dma_entries += non_dma0->num_entries;


    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE;

    /* FLAGS - Notes
     * Refer notes init of ID - SOC_COUNTER_NON_DMA_COSQ_DROP_BYTE
     */
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
    /* Clear on Read Flag set if eviction is enabled */
    non_dma1->flags |= _SOC_COUNTER_NON_DMA_CTR_EVICT;
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "EGR_FLEX_BYTE";
    non_dma1->extra_ctr_ct = pool_count; /* Taken care by PKT_CTR */

    /* Memory for extra ctrs for BYTE_CTRS are done along with PACKET_CTR */
    non_dma1->extra_ctrs = non_dma0->extra_ctrs + 1;

    if (non_dma0->extra_ctrs)
    {
        rv = soc_counter_th_extra_ctrs_init(unit,
                                            SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE,
                                            non_dma1,
                                            non_dma1->extra_ctrs,
                                            non_dma1->extra_ctr_ct,
                                            &num_extra_ctr_entries);
        if (rv != SOC_E_NONE) {
            LOG_CLI((BSL_META_U(unit,
                                "ERR: Extra ctrs Init FAILED for id %d\n"),
                     SOC_COUNTER_NON_DMA_EGR_FLEX_BYTE -
                     SOC_COUNTER_NON_DMA_START));
        }
    }
    non_dma1->flags = _SOC_COUNTER_NON_DMA_SUBSET_PARENT;
 
    *non_dma_entries += non_dma1->num_entries;


    non_dma0 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EFP_PKT -
                                     SOC_COUNTER_NON_DMA_START];
    non_dma0->id = SOC_COUNTER_NON_DMA_EFP_PKT;
    non_dma0->flags = _SOC_COUNTER_NON_DMA_VALID |
                      _SOC_COUNTER_NON_DMA_DO_DMA;

    /* EFP_COUNTER_TABLE - Counter Eviction is disabled */
    SOC_PBMP_CLEAR(non_dma0->pbmp);
    non_dma0->base_index = non_dma_start_index + *non_dma_entries;
    non_dma0->entries_per_port = 0;
    non_dma0->num_entries =
        NUM_PIPE(unit) * soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    non_dma0->mem = EFP_COUNTER_TABLEm;
    non_dma0->reg = INVALIDr;
    non_dma0->field = PACKET_COUNTERf;
    non_dma0->cname = "EFP_PKT";
 
    num_entries = soc_mem_index_count(unit, EFP_COUNTER_TABLEm);
    entry_words = soc_mem_entry_words(unit, EFP_COUNTER_TABLEm);

    for (pipe = 0; pipe < NUM_PIPE(unit); pipe++) {
        non_dma0->dma_mem[pipe] = 
            SOC_MEM_UNIQUE_ACC(unit, non_dma0->mem)[pipe];
        non_dma0->dma_buf[pipe] = &buf[num_entries * entry_words * pipe];
        non_dma0->dma_index_max[pipe] =
            soc_mem_index_max(unit, non_dma0->dma_mem[pipe]);
    }

    *non_dma_entries += non_dma0->num_entries;


    non_dma1 = &soc->counter_non_dma[SOC_COUNTER_NON_DMA_EFP_BYTE -
                                     SOC_COUNTER_NON_DMA_START];
    *non_dma1 = *non_dma0;
    non_dma1->id = SOC_COUNTER_NON_DMA_EFP_BYTE;
    non_dma1->flags = _SOC_COUNTER_NON_DMA_VALID;

    /* EFP_COUNTER_TABLE - Counter Eviction is disabled */
    non_dma1->base_index = non_dma_start_index + *non_dma_entries;
    non_dma1->field = BYTE_COUNTERf;
    non_dma1->cname = "EFP_BYTE";
    *non_dma_entries += non_dma1->num_entries;

    /* eviction table init */
    SOC_IF_ERROR_RETURN(soc_counter_tomahawk_eviction_init(unit));

    soc_counter_tomahawk_non_dma_post_init(unit);
    return SOC_E_NONE;
}

#endif /* BCM_TOMAHAWK_SUPPORT */


