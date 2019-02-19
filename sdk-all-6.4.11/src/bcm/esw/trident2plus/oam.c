/* $Id: oam.c,v 1.0.0 Broadcom SDK $
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
 * File:
 *  oam.c
 *
 * Purpose:
 *  OAM implementation for trident2p family of devices.
 */

#include <shared/bsl.h>

#include <sal/core/libc.h>
#include <soc/defs.h>
#include <soc/drv.h>
#include <soc/scache.h>
#include <soc/profile_mem.h>
#include <soc/hash.h>

#include <bcm/oam.h>
#include <bcm/field.h>

#include <bcm_int/esw/oam.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/switch.h>
#include <bcm_int/esw/virtual.h>
#include <bcm_int/esw_dispatch.h>

#include <soc/shared/olp_pkt.h>
#include <soc/shared/olp_pack.h>

#if defined(INCLUDE_CCM)
#include <bcm_int/esw/fp_oam.h>
#endif

#define _BCM_TD2P_OLP_HDR_TYPE_RX    1

/* OLP_HDR_SUBTYPE filled with one of the following values */
/* Down mep subtypes */
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM             0x01
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM              0x02
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM              0x03
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC        0x04
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_CCM                 0x05
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_LM                  0x06
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_DM                  0x07
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_OTHER_OC            0x08
#define _BCM_TD2P_OLP_HDR_SUBTYPE_BFD                     0x09
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM           0x0A
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DM            0x0B
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM_DM        0x0C
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM           0x0D
#define _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM_DM        0x0E
#define _BCM_TD2P_OLP_HDR_SUBTYPE_SAT                     0x0F
#define _BCM_TD2P_OLP_HDR_SUBTYPE_OTH_ACH                 0x10

/* Up mep subtypes */
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP             0x11
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM_UP              0x12
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM_UP              0x13
#define _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC_UP        0x14
#define _BCM_TD2P_OLP_HDR_SUBTYPE_SAT_UP                     0x15

typedef struct _bcm_td2p_olp_hdr_type_mapping_s {
    bcm_field_olp_header_type_t     field_olp_hdr_type;
    uint8                           subtype;
} _bcm_td2p_olp_hdr_type_mapping_t;

STATIC _bcm_td2p_olp_hdr_type_mapping_t td2p_olp_hdr_type_mapping[] = {

    /* Field OLP header type to Subtype mapping table
     * Note: Do not change the order, as EGR_OLP_HEADER_TYPE_MAPPING table is
     *       programmed in the same order.
     */

/* BFD */
{bcmFieldOlpHeaderTypeBfdOam, _BCM_TD2P_OLP_HDR_SUBTYPE_BFD},

/* ETH OAM Down MEP*/
{bcmFieldOlpHeaderTypeEthOamCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM},
{bcmFieldOlpHeaderTypeEthOamLm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM},
{bcmFieldOlpHeaderTypeEthOamDm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM},
{bcmFieldOlpHeaderTypeEthOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC},

/* BHH */
{bcmFieldOlpHeaderTypeBhhOamCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_CCM},
{bcmFieldOlpHeaderTypeBhhOamLm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_LM},
{bcmFieldOlpHeaderTypeBhhOamDm, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_DM},
{bcmFieldOlpHeaderTypeBhhOamOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_BHH_OTHER_OC},

/* MPLS LM/DM */
{bcmFieldOlpHeaderTypeRfc6374Dlm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM},
{bcmFieldOlpHeaderTypeRfc6374Dm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DM},
{bcmFieldOlpHeaderTypeRfc6374DlmPlusDm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_DLM_DM},
{bcmFieldOlpHeaderTypeRfc6374Ilm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM},
{bcmFieldOlpHeaderTypeRfc6374IlmPlusDm, _BCM_TD2P_OLP_HDR_SUBTYPE_MPLS_LMDM_ILM_DM},

/* Down SAT */
{bcmFieldOlpHeaderTypeSat, _BCM_TD2P_OLP_HDR_SUBTYPE_SAT},

/* Other ACH */
{bcmFieldOlpHeaderTypeOtherAch, _BCM_TD2P_OLP_HDR_SUBTYPE_OTH_ACH},

/* ETH-OAM Up MEP */
{bcmFieldOlpHeaderTypeEthOamUpMepCcm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_CCM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepLm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_LM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepDm, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_DM_UP},
{bcmFieldOlpHeaderTypeEthOamUpMepOthers, _BCM_TD2P_OLP_HDR_SUBTYPE_ETH_OAM_OTHER_OC_UP},

/* Up SAT*/
{bcmFieldOlpHeaderTypeUpSat, _BCM_TD2P_OLP_HDR_SUBTYPE_SAT_UP},

};

static uint8 td2p_olp_hdr_type_count = sizeof(td2p_olp_hdr_type_mapping) /
                                           sizeof(td2p_olp_hdr_type_mapping[0]);

/* Magic port used for remote OLP over HG communication */
#define _BCM_OAM_TD2P_OLP_MAGIC_PORT 0x7F

/*
 * Function:
 *    _bcm_td2p_oam_olp_header_type_mapping_set
 * Purpose:
 *     Miscellaneous OAM configurations:
 *         1. Enable IFP lookup on the CPU port.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_olp_header_type_mapping_set(int unit)
{
    int                                 index           = 0;
    int                                 mem_index_count = 0;
    egr_olp_header_type_mapping_entry_t entry;

    mem_index_count = soc_mem_index_count(unit, EGR_OLP_HEADER_TYPE_MAPPINGm);

    if (td2p_olp_hdr_type_count > mem_index_count) {
        return BCM_E_RESOURCE;
    }

    for (index = 0; index < td2p_olp_hdr_type_count; index++) {
        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(unit,
                                                     &entry,
                                                     HDR_TYPEf,
                                                     _BCM_TD2P_OLP_HDR_TYPE_RX);

        soc_EGR_OLP_HEADER_TYPE_MAPPINGm_field32_set(
                                            unit,
                                            &entry,
                                            HDR_SUBTYPEf,
                                            td2p_olp_hdr_type_mapping[index].subtype);

        SOC_IF_ERROR_RETURN(
                     WRITE_EGR_OLP_HEADER_TYPE_MAPPINGm(unit,  MEM_BLOCK_ALL,
                                                        index, &entry));
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *     _bcm_td2p_oam_hg_olp_enable
 * Purpose:
 *    Enable OLP on HG ports by default
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_hg_olp_enable(int unit)
{
    bcm_pbmp_t                     ports;
    bcm_port_t                     port;
    iarb_ing_port_table_entry_t    entry;

    BCM_PBMP_ASSIGN(ports, PBMP_PORT_ALL(unit));

    PBMP_ITER(ports, port) {
        if (IS_HG_PORT(unit, port)) {
            SOC_IF_ERROR_RETURN(soc_mem_read(unit, IARB_ING_PORT_TABLEm,
                                             MEM_BLOCK_ANY, port, &entry));

            soc_IARB_ING_PORT_TABLEm_field32_set(unit, &entry, OLP_ENABLEf, 1);

            SOC_IF_ERROR_RETURN(soc_mem_write(unit, IARB_ING_PORT_TABLEm,
                                              MEM_BLOCK_ALL, port, &entry));
        }
    }
    return (BCM_E_NONE);
}

/*
 * Function:
 *     _bcm_td2p_oam_olp_magic_port_set
 * Purpose:
 *     Set Magic port used for remote OLP over HG communication
 * Parameters:
 *     unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_td2p_oam_olp_magic_port_set(int unit)
{
    int    modid;

    /* Configure modid and the magic port */
    BCM_IF_ERROR_RETURN(bcm_esw_stk_my_modid_get(unit, &modid));

    SOC_IF_ERROR_RETURN(
          soc_reg_field32_modify(unit, IARB_OLP_CONFIG_1r,
                                 REG_PORT_ANY, MY_MODIDf, modid));

    SOC_IF_ERROR_RETURN(soc_reg_field32_modify(unit,
                                               IARB_OLP_CONFIG_1r,
                                               REG_PORT_ANY,
                                               MY_PORT_NUMf,
                                               _BCM_OAM_TD2P_OLP_MAGIC_PORT));

    return (BCM_E_NONE);
}

/* * * * * * * * * * * * * * * * * * * *
 *            OAM BCM APIs             *
 * * * * * * * * * * * * * * * * * * * */


/*
 * Function: _bcm_td2p_oam_olp_fp_hw_index_get
 *
 * Purpose:
 *     Get OLP_HDR_TYPE_COMPRESSED corresponding to subtype
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_fp_hw_index_get(int unit,
                                  bcm_field_olp_header_type_t olp_hdr_type,
                                  int *hwindex)
{
    int index;

    for (index = 0; index < td2p_olp_hdr_type_count; index++) {
        if (td2p_olp_hdr_type_mapping[index].field_olp_hdr_type == olp_hdr_type) {
            *hwindex = index;
            return BCM_E_NONE;
        }
    }

    return BCM_E_NOT_FOUND;
}

/*
 * Function: _bcm_td2p_oam_olp_hw_index_olp_type_get
 *
 * Purpose:
 *     Get subtype corresponding to OLP_HDR_TYPE_COMPRESSED
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
_bcm_td2p_oam_olp_hw_index_olp_type_get(int unit,
                                        int hwindex,
                                        bcm_field_olp_header_type_t *olp_hdr_type)
{
    if (hwindex >= td2p_olp_hdr_type_count) {
        return BCM_E_PARAM;
    }

    *olp_hdr_type = td2p_olp_hdr_type_mapping[hwindex].field_olp_hdr_type;

    return BCM_E_NONE;
}


#if defined(INCLUDE_CCM)
/*******************************************************************************/
/* Common supporting functions used by Embedded Apps Start */
/*******************************************************************************/

/* External functions definition */
extern void _bcm_esw_stat_get_counter_id_info(
                                         int                   unit,
                                         uint32                stat_counter_id,
                                         bcm_stat_group_mode_t *group,
                                         bcm_stat_object_t     *object,
                                         uint32                *mode,
                                         uint32                *pool_number,
                                         uint32                *base_idx);

extern int _bcm_olp_l2_hdr_get(int unit, int glp, soc_olp_l2_hdr_t *l2hdr);
extern int soc_uc_in_reset(int unit, int uC);
/*
 *Macro:
 *     _BCM_OAM_LOCK
 * Purpose:
 *     Lock take the OAM control mutex
 * Parameters:
 *     control - Pointer to OAM control structure.
 */
#define _BCM_OAM_LOCK(control) \
    sal_mutex_take((control)->oc_lock, sal_mutex_FOREVER)


/*
 * Macro:
 *     _BCM_OAM_UNLOCK
 * Purpose:
 *     Lock take the OAM control mutex
 * Parameters:
 *     control - Pointer to OAM control structure.
 */
#define _BCM_OAM_UNLOCK(control) \
    sal_mutex_give((control)->oc_lock);


/*
 *Macro:
 *     _BCM_OAM_IS_INIT
 * Purpose:
 *     Check if module is initialized
 * Parameters:
 *     unit - soc unit number.
 */
#define _BCM_OAM_IS_INIT(unit)                                         \
    do {                                                               \
        if (_fp_oam_control[unit] == NULL) {                           \
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Module " \
                "not initialized\n")));                                \
            return (BCM_E_INIT);                                       \
        }                                                              \
    } while (0)

/*
 * Macro:
 *     _BCM_OAM_ALLOC
 * Purpose:
 *      Generic memory allocation routine.
 * Parameters:
 *    _ptr_     - Pointer to allocated memory.
 *    _ptype_   - Pointer type.
 *    _size_    - Size of heap memory to be allocated.
 *    _descr_   - Information about this memory allocation.
 */
#define _BCM_OAM_ALLOC(_ptr_,_ptype_,_size_,_descr_)             \
    do {                                                         \
        if (NULL == (_ptr_)) {                                   \
           (_ptr_) = (_ptype_ *) sal_alloc((_size_), (_descr_)); \
        }                                                        \
        if((_ptr_) != NULL) {                                    \
            sal_memset((_ptr_), 0, (_size_));                    \
        }  else {                                                \
            LOG_ERROR(BSL_LS_BCM_OAM, \
                      (BSL_META("OAM Error: Allocation failure %s\n"), \
                       (_descr_)));                              \
        }                                                        \
    } while (0)

/*
 * Macro:
 *     _BCM_OAM_KEY_PACK
 * Purpose:
 *     Pack the hash table look up key fields.
 * Parameters:
 *     _dest_ - Hash key buffer.
 *     _src_  - Hash key field to be packed.
 *     _size_ - Hash key field size in bytes.
 */
#define _BCM_OAM_KEY_PACK(_dest_,_src_,_size_)            \
    do {                                          \
        sal_memcpy((_dest_), (_src_), (_size_));  \
        (_dest_) += (_size_);                     \
    } while (0)

 /* Device OAM control structure */
_bcm_fp_oam_control_t *_fp_oam_control[SOC_MAX_NUM_DEVICES];


/*
 * Function:
 *     _bcm_fp_oam_control_get
 * Purpose:
 *     Lookup a OAM control config from a bcm device id.
 * Parameters:
 *     unit -  (IN)BCM unit number.
 *     oc   -  (OUT) OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_control_get(int unit, _bcm_fp_oam_control_t **oc)
{
    if (NULL == oc) {
        return (BCM_E_PARAM);
    }

    /* Ensure oam module is initialized. */
    _BCM_OAM_IS_INIT(unit);

    *oc = _fp_oam_control[unit];

    return (BCM_E_NONE);
}


/*
 * Function:
 *      _bcm_fp_oam_msg_send_receive
 * Purpose:
 *      Sends given control message to the uController.
 *      Receives and verifies expected reply.
 *      Performs DMA operation if required.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      msg_class   - (IN) Message Class
 *      s_subclass  - (IN) Message subclass.
 *      s_len       - (IN) Value for 'len' field in message struct.
 *                         Length of buffer to flush if DMA send is required.
 *      s_data      - (IN) Value for 'data' field in message struct.
 *                         Ignored if message requires a DMA send/receive
 *                         operation.
 *      r_subclass  - (IN) Expected reply message subclass.
 *      r_len       - (OUT) Returns value in 'len' reply message field.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 *     - The uc_msg 'len' and 'data' fields of mos_msg_data_t
 *       can take any arbitrary data.
 */
STATIC int
_bcm_fp_oam_msg_send_receive(int unit, uint8 msg_class, uint8 s_subclass,
                                    uint16 s_len, uint32 s_data,
                                    uint8 r_subclass, uint16 *r_len)
{
    int rv;
    _bcm_fp_oam_control_t *oc;
    mos_msg_data_t send, reply;
    uint8 *dma_buffer = NULL;
    int dma_buffer_len = 0, uc_num = 0;
    uint32 uc_rv;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    sal_memset(&send, 0, sizeof(send));
    sal_memset(&reply, 0, sizeof(reply));
    send.s.mclass = msg_class;
    send.s.subclass = s_subclass;
    send.s.len = bcm_htons(s_len);

    /* Set 'data' to DMA buffer address if a DMA operation is
     * required for send or receive. */
#if defined(INCLUDE_CCM) 
    if (msg_class == MOS_MSG_CLASS_CCM) {
        dma_buffer = oc->dma_buffer;
        dma_buffer_len = oc->dma_buffer_len;
        uc_num = oc->ccm_uc_num;
    } 
#endif

    if(dma_buffer == NULL){
        return (BCM_E_INTERNAL);
    }

    if (MOS_MSG_DMA_MSG(s_subclass) ||
        MOS_MSG_DMA_MSG(r_subclass)) {
        send.s.data = bcm_htonl(soc_cm_l2p(unit, dma_buffer));
    } else {
        send.s.data = bcm_htonl(s_data);
    }

    /* Flush DMA memory */
    if (MOS_MSG_DMA_MSG(s_subclass)) {
        soc_cm_sflush(unit, dma_buffer, s_len);
    }

    /* Invalidate DMA memory to read */
    if (MOS_MSG_DMA_MSG(r_subclass)) {
        soc_cm_sinval(unit, dma_buffer, dma_buffer_len);
    }

    rv = soc_cmic_uc_msg_send_receive(unit, uc_num, &send, &reply,
                                      _UC_MSG_TIMEOUT_USECS);

    /* Check reply class, subclass */
    if (rv == SOC_E_TIMEOUT) {
        return (BCM_E_TIMEOUT);
    } else if (rv != SOC_E_NONE) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "UC Message Send/Rcv Failed %s\n"), bcm_errmsg(rv)));
        return (BCM_E_INTERNAL);
    }

    /* Convert uController error code to BCM */
    uc_rv = bcm_ntohl(reply.s.data);
    switch(uc_rv) {
    case SHR_UC_E_NONE:
        rv = BCM_E_NONE;
        break;
    case SHR_UC_E_INTERNAL:
        rv = BCM_E_INTERNAL;
        break;
    case SHR_UC_E_MEMORY:
        rv = BCM_E_MEMORY;
        break;
    case SHR_UC_E_PARAM:
        rv = BCM_E_PARAM;
        break;
    case SHR_UC_E_RESOURCE:
        rv = BCM_E_RESOURCE;
        break;
    case SHR_UC_E_EXISTS:
        rv = BCM_E_EXISTS;
        break;
    case SHR_UC_E_NOT_FOUND:
        rv = BCM_E_NOT_FOUND;
        break;
    case SHR_UC_E_INIT:
        rv = BCM_E_INIT;
        break;
    case SHR_UC_E_UNAVAIL:
        rv = BCM_E_UNAVAIL;
        break;
    default:
        rv = BCM_E_INTERNAL;
        break;
    }

    *r_len = bcm_ntohs(reply.s.len);

    /*Check reply class and subclass*/
    if((rv == SOC_E_NONE) && ((reply.s.mclass != msg_class) ||
        (reply.s.subclass != r_subclass)))
    {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
          "UC Err or Rcv Sclass not same as Expected %s\n"), bcm_errmsg(rv)));
        return BCM_E_INTERNAL;
    }

    return (rv);
}


/*
 * Function:
 *     _bcm_fp_oam_endpoint_gport_resolve
 * Purpose:
 *     Resolve an endpoint GPORT value to SGLP and DGLP value.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     ep_info_p  - (IN/OUT) Pointer to endpoint information.
 *     src_glp    - (IN/OUT) Pointer to source generic logical port value.
 *     dst_glp    - (IN/OUT) Pointer to destination generic logical port value.
 *     trunk_id   - (IN/OUT) Pointer to trunk id
 *     svp        - (IN/OUT) Pointer to VP value
 *     is_vp_valid - (OUT)   VP is valid or not
 *     trunk_member - (OUT)  trunk member on which EP present 
 *
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_endpoint_gport_resolve(int unit, bcm_oam_endpoint_info_t *ep_info_p,
        int *src_glp, int *dst_glp, bcm_trunk_t *trunk_id, uint32 *svp, 
        int *is_vp_valid, bcm_trunk_member_t *trunk_member)
{
    bcm_module_t        module_id;              /* Module ID */
    bcm_port_t          port_id;                /* Port ID */
    int                 local_id;               /* Hardware ID */

    bcm_trunk_info_t    trunk_info;             /* Trunk information.  */
    bcm_trunk_member_t  *member_array = NULL;   /* Trunk member array */
    int                 member_count = 0;       /* Trunk Member count */

    int                 tx_enabled = 0;         /* CCM Tx enabled */
    int                 rv = 0;                 /* Return Value */
    int                is_local = 0;

    /* Get Trunk ID or (Modid + Port) value from Gport */
    BCM_IF_ERROR_RETURN
        (_bcm_esw_gport_resolve(unit, ep_info_p->gport, &module_id,
                                &port_id, trunk_id, &local_id));

    /* Set CCM endpoint Tx status only for local endpoints. */
    if (!(ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE)) {
        tx_enabled =
            (ep_info_p->ccm_period !=
             BCM_OAM_ENDPOINT_CCM_PERIOD_DISABLED) ? 1 : 0;
    }

    if (SOC_GPORT_IS_MPLS_PORT(ep_info_p->gport)) {
        *svp = local_id;
        if(BCM_TRUNK_INVALID == (*trunk_id)){
            rv = _bcm_esw_modid_is_local(unit, module_id, &is_local);
            if(BCM_SUCCESS(rv) && (is_local)) { 
                _bcm_esw_glp_construct(unit, *trunk_id, module_id, port_id,
                                       dst_glp);
                *src_glp = *dst_glp;
                *is_vp_valid = 1;
            }
        }
    }

    /*
     * If Gport is Trunk type, _bcm_esw_gport_resolve()
     * sets trunk_id. Using Trunk ID, get Dst Modid and Port value.
     */
    if (BCM_GPORT_IS_TRUNK(ep_info_p->gport)) {

        if (BCM_TRUNK_INVALID == *trunk_id)  {
            /* Has to be a valid Trunk. */
            return (BCM_E_PARAM);
        }
    }

    if (BCM_TRUNK_INVALID != *trunk_id)  {
        /* Construct Hw SGLP value. */
        _bcm_esw_glp_construct(unit, *trunk_id, module_id, port_id, src_glp);


        /*
         * CCM Tx is enabled on a trunk member port.
         * trunk_index value is required to derive the Modid and Port info.
         */
        if (1 == tx_enabled) {

            if (_BCM_OAM_INVALID_INDEX == ep_info_p->trunk_index) {
            /* Invalid Trunk member index passed. */
            return (BCM_E_PORT);
            }

            /* Get count of ports in this trunk. */
            BCM_IF_ERROR_RETURN
                (bcm_esw_trunk_get(unit, *trunk_id, NULL, 0, NULL, &member_count));
            if (0 == member_count) {
                /* No members have been added to the trunk group yet */
                return BCM_E_PARAM;
            }

            _BCM_OAM_ALLOC(member_array, bcm_trunk_member_t,
                           sizeof(bcm_trunk_member_t) * member_count, "Trunk info");
            if (NULL == member_array) {
                return (BCM_E_MEMORY);
            }

            /* Get Trunk Info for the Trunk ID. */
            rv = bcm_esw_trunk_get(unit, *trunk_id, &trunk_info, member_count,
                                   member_array, &member_count);
            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return (rv);
            }

            /* Check if the input trunk_index is valid. */
            if (ep_info_p->trunk_index >= member_count) {
                sal_free(member_array);
                return BCM_E_PARAM;
            }

            /* Get the Modid and Port value using Trunk Index value. */
            rv = _bcm_esw_gport_resolve
                    (unit, member_array[ep_info_p->trunk_index].gport,
                     &module_id, &port_id, trunk_id, &local_id);
            if (BCM_FAILURE(rv)) {
                sal_free(member_array);
                return (rv);
            }

            sal_free(member_array);

            /* Construct Hw DGLP value. */
            _bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID, module_id, port_id,
                                   dst_glp);
            *trunk_member = member_array[ep_info_p->trunk_index];

        } else {
                *dst_glp = *src_glp;
        }

    }

    /*
     * Application can resolve the trunk and pass the desginated
     * port as Gport value. Check if the Gport belongs to a trunk.
     */
    if ((BCM_TRUNK_INVALID == (*trunk_id))
        && (BCM_GPORT_IS_MODPORT(ep_info_p->gport)
        || BCM_GPORT_IS_LOCAL(ep_info_p->gport))) {

        /* When Gport is ModPort or Port type, _bcm_esw_gport_resolve()
         * returns Modid and Port value. Use these values to make the DGLP
         * value.
         */
        _bcm_esw_glp_construct(unit, BCM_TRUNK_INVALID, module_id, port_id,
                               dst_glp);

        /* Use the Modid, Port value and determine if the port
         * belongs to a Trunk.
         */
        rv = bcm_esw_trunk_find(unit, module_id, port_id, trunk_id);
        if (BCM_SUCCESS(rv)) {
            /*
             * Port is member of a valid trunk.
             * Now create the SGLP value from Trunk ID.
             */
            _bcm_esw_glp_construct(unit, *trunk_id, module_id, port_id, src_glp);
        } else {
            /* Port not a member of trunk. DGLP and SGLP are the same. */
            *src_glp = *dst_glp;
        }
    }

    return BCM_E_NONE;
}


/*
 * Function:
 *     _bcm_fp_oam_olp_l2_header_pack
 * Purpose:
 *     Pack OLP L2 Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack OLP Header
 *     l2         - (IN)     Pointer to OLP L2 Header fields structure.
 * Retruns:
 *     Pointer to end of packet buffer
 */
STATIC uint8 *
_bcm_fp_oam_olp_l2_header_pack(uint8 *buffer, soc_olp_l2_hdr_t *l2)
{

    int     i;

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < 6; i++) {
        _SHR_PACK_U8(buffer, l2->src_mac[i]);
    }

    /* Vlan tpid */
    _SHR_PACK_U16(buffer, l2->tpid);

    /* Vlan Id */
    _SHR_PACK_U16(buffer, l2->vlan);

    /* Ethertype */
    _SHR_PACK_U16(buffer, l2->etherType);

    return (buffer);
}


/*
 * Function:
 *     _bcm_fp_oam_olp_l2_header_unpack
 * Purpose:
 *     Unpack OLP L2 Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack OLP Header
 *     l2         - (IN)     Pointer to OLP L2 Header fields structure.
 * Retruns:
 *     Pointer to end of packet buffer
 */
STATIC uint8 *
_bcm_fp_oam_olp_l2_header_unpack(uint8 *buffer, soc_olp_l2_hdr_t *l2)
{

    int     i;

    for (i = 0; i < 6; i++) {
        _SHR_UNPACK_U8(buffer, l2->dst_mac[i]);
    }

    for (i = 0; i < 6; i++) {
        _SHR_UNPACK_U8(buffer, l2->src_mac[i]);
    }

    /* Vlan tpid */
    _SHR_UNPACK_U16(buffer, l2->tpid);

    /* Vlan Id */
    _SHR_UNPACK_U16(buffer, l2->vlan);

    /* Ethertype */
    _SHR_UNPACK_U16(buffer, l2->etherType);

    return (buffer);
}

/*******************************************************************************/
/* CCM Embedded App supporting functions Start */
/*******************************************************************************/
#define _BCM_OAM_GLP_MODULE_ID_GET(_glp_) (0xFF & ((_glp_) >> 7))
#define _BCM_OAM_GLP_PORT_GET(_glp_)      (0x7F & (_glp_))

/*
 * Macro:
 *     _BCM_OAM_EP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Endpoint ID value.
 * Parameters:
 *     _ep_ - Endpoint ID value.
 */
#define _BCM_OAM_EP_INDEX_VALIDATE(_ep_)                       \
    do {                                                       \
        if ((_ep_) < 0 || (_ep_) >= oc->mep_count) {            \
            LOG_ERROR(BSL_LS_BCM_OAM,                          \
                      (BSL_META("OAM(unit %d) Error: Invalid Endpoint ID" \
                       " = %d.\n"), unit, _ep_));              \
            return (BCM_E_PARAM);                              \
        }                                                      \
    } while (0);

/*
 * Macro:
 *     _BCM_FP_OAM_GROUP_INDEX_VALIDATE
 * Purpose:
 *     Validate OAM Group ID value.
 * Parameters:
 *     _group_ - Group ID value.
 */
#define _BCM_FP_OAM_GROUP_INDEX_VALIDATE(_group_)                   \
    do {                                                            \
        if ((_group_) < 0 || (_group_) >= oc->group_count) {        \
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META("OAM Error: "       \
              "Invalid Group ID = %d.\n"), _group_));               \
            return (BCM_E_PARAM);                                   \
        }                                                           \
    } while (0);


/*
 * Function:
 *     _bcm_fp_oam_group_set_msg_send
 * Purpose:
 *     Send Group Create/Update Message to UKERNEL
 * Parameters:
 *     unit          - (IN) BCM device number
 *     ep_data       - (IN) Pointer to endpoint information.
 *     msg_flags     - (IN) Msg control flags
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_fp_oam_group_set_msg_send(int unit, _bcm_fp_oam_group_data_t *grp_p,
                              uint32 msg_flags, uint16 *r_msg_len)
{
    shr_ccm_msg_ctrl_group_data_t group_msg;
    _bcm_fp_oam_control_t *oc = NULL;
    uint8 *dma_buffer1 = NULL;  /* DMA buffer */
    uint8 *dma_buffer2 = NULL;  /* DMA buffer ptr after pack */
    uint16 s_msg_len = 0;       /* Send Message length */
    int rv = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    dma_buffer1 = oc->dma_buffer;

    /* Validate input parameter. */
    if (NULL == grp_p) {
        return (BCM_E_INTERNAL);
    }

    /* Clear meo_msg structure */
    sal_memset(&group_msg, 0, sizeof(group_msg));
    group_msg.flags = msg_flags;

    /* Fill Mep data and Send MEP Create message to UKENREL */
    sal_memcpy(&(group_msg.group_data), &(grp_p->group_data),
               sizeof(ccm_group_data_t));

    /* Pack Mep data to buffer */
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_group_set_pack(dma_buffer1, &group_msg);
    s_msg_len = dma_buffer2 - dma_buffer1;
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_SET, s_msg_len, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_SET_REPLY, r_msg_len);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group Set Msg returned error %s.\n"), bcm_errmsg(rv)));
    }
    return rv;
}


/*
 * Function:
 *     bcm_fp_oam_ccm_group_create
 * Purpose:
 *     Create or replace an OAM group object
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group information.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bcm_fp_oam_group_data_t *oam_group = NULL; /* OAM Group being created */
    uint32 msg_flags = 0;                       /* Group Msg flags */
    uint16 r_len = 0;                           /* Received Msg length */
    int rv = 0;                                 /* return value */

    /* Validate input parameter. */
    if (NULL == group_info) {
        return (BCM_E_PARAM);
    }

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate group id. */
    if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        _BCM_FP_OAM_GROUP_INDEX_VALIDATE(group_info->id);
    }

    /*
     * If MA group create is called with replace flag bit set.
     *  - Check and return error if a group does not exist with the ID.
     */
    if (group_info->flags & BCM_OAM_GROUP_REPLACE) {
        if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {

            /* Search the list with the MA Group ID value. */
            rv = shr_idxres_list_elem_state(oc->group_pool, group_info->id);
            if (BCM_E_EXISTS != rv) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                          "OAM Error: Group does not exist.\n")));
                return (BCM_E_PARAM);
            }
        } else {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                      "OAM Error: Replace command needs a valid Group ID.\n")));
            return (BCM_E_PARAM);
        }
    } else if (group_info->flags & BCM_OAM_GROUP_WITH_ID) {
        /*
         * If MA group create is called with ID flag bit set.
         *  - Check and return error if the ID is already in use.
         */
        rv = shr_idxres_list_reserve(oc->group_pool, group_info->id,
                                     group_info->id);
        if (BCM_FAILURE(rv)) {
            return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
        }
    } else {
        /* Reserve the next available group index. */
         rv = shr_idxres_list_alloc(oc->group_pool,
                                    (shr_idxres_element_t *) &group_info->id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                      "OAM Error: Group allocation (GID=%d) %s\n"),
                      group_info->id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    /* Get this group memory to store group information. */
    oam_group = &(oc->group_data[group_info->id]);

    if (!(group_info->flags & BCM_OAM_GROUP_REPLACE)) {

        /* Store the group name. */
        sal_memcpy(oam_group->group_data.name, group_info->name,
                   BCM_OAM_GROUP_NAME_LENGTH);

        /* Store Lowest Alarm Priority */
        oam_group->group_data.lowest_alarm_priority = group_info->lowest_alarm_priority;

        /* Initialize RMEP head to NULL.*/
        sal_memset(oam_group->group_data.rmep_bitmap, 0,
                                    sizeof(oam_group->group_data.rmep_bitmap));

        /* Check if software RDI flag bit needs to be set in hardware. */
        oam_group->group_data.flags |=
            ((group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) ?
              _FP_OAM_GROUP_SW_RDI_FLAG : 0);

        /* Set LMEP as not configured */
        oam_group->group_data.lmep_id = _FP_OAM_INVALID_LMEP_ID;

        oam_group->group_data.group_id = group_info->id;

        /* Send Group Create message to uKernel */
        rv = _bcm_fp_oam_group_set_msg_send(unit, oam_group, msg_flags, &r_len);
         if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Group create Msg failed, id:%d - %s"), group_info->id,
                bcm_errmsg(rv)));
        } else {
            oam_group->in_use = 1;
        }

    } else {
        /* Only lowest alarm prio and S/w RDI bit can be replaced */
        if ((oam_group->group_data.flags & _FP_OAM_GROUP_SW_RDI_FLAG) !=
            (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX)) {

            if (group_info->flags & BCM_OAM_GROUP_REMOTE_DEFECT_TX) {
                oam_group->group_data.flags |= _FP_OAM_GROUP_SW_RDI_FLAG;
            } else {
                oam_group->group_data.flags &= ~(_FP_OAM_GROUP_SW_RDI_FLAG);
            }
            msg_flags |= _GRP_REPLACE;
            msg_flags |= _GRP_REPLACE_SW_RDI;
        }

        if (oam_group->group_data.lowest_alarm_priority !=
            group_info->lowest_alarm_priority) {

            oam_group->group_data.lowest_alarm_priority =
                group_info->lowest_alarm_priority;
            msg_flags |= _GRP_REPLACE;
            msg_flags |= _GRP_REPLACE_LOWEST_ALARM_PRIO;
        }

        /* Send Group Create message to uKernel */
        rv = _bcm_fp_oam_group_set_msg_send(unit, oam_group, msg_flags, &r_len);
         if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Group create Msg failed, id:%d - %s"), group_info->id,
                bcm_errmsg(rv)));
        }
    }

    return rv;
}

/*
 * Function:
 *     bcm_fp_oam_group_get
 * Purpose:
 *     Delete an OAM group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_group_get(int unit, bcm_oam_group_info_t *group_info) {

    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bcm_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    uint16 reply_len;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;
    shr_ccm_msg_ctrl_group_data_t group_msg;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate Group Index */
    _BCM_FP_OAM_GROUP_INDEX_VALIDATE(group_info->id);

    /* Get Handle to Local DB */
    group_p = &(oc->group_data[group_info->id]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET_FAULTS_ONLY, group_info->id, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET_REPLY, &reply_len);

    /* UnPack control message data into DMA buffer */
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_group_get_unpack(dma_buffer1, &group_msg);

    if (reply_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Group get Msg lenght check failed, id:%d.\n"), group_info->id));
        return (rv);
    }
    group_info->faults = group_msg.group_data.faults;
    group_info->persistent_faults = group_msg.group_data.persistent_faults;

    sal_memcpy(group_info->name, group_p->group_data.name, _FP_OAM_MAID_LEN);
    group_info->lowest_alarm_priority =
        group_p->group_data.lowest_alarm_priority;

    if (group_p->group_data.flags & _FP_OAM_GROUP_SW_RDI_FLAG) {
        group_info->flags |= BCM_OAM_GROUP_REMOTE_DEFECT_TX;
    }

    return rv;
}


/*
 * Function:
 *     bcm_fp_oam_ccm_group_destroy
 * Purpose:
 *     Delete an OAM group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_group_destroy(int unit, bcm_oam_group_t group_id)
{
    _bcm_fp_oam_control_t *oc = NULL;           /* OAM control structure */
    _bcm_fp_oam_group_data_t *group_p;          /* Handle to S/w Group Data */
    int rv = 0;                                 /* Return Value */
    uint16 r_len;                               /* Msg reply lenght */
    int i = 0;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate Group Index */
    _BCM_FP_OAM_GROUP_INDEX_VALIDATE(group_id);

    /* Get Handle to Local DB */
    group_p = &(oc->group_data[group_id]);
    if (!(group_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_DELETE, group_id, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_DELETE_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Group Del Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
        return rv;
    }

    /* Clean up Local db, including LMEPs and RMEPs */
    if (group_p->group_data.lmep_id != _FP_OAM_INVALID_LMEP_ID) {
        sal_memset(&(oc->mep_data[group_p->group_data.lmep_id]), 0,
                   sizeof(_bcm_fp_oam_mep_data_t));

        /* Free the LMEP index */
        rv = shr_idxres_list_free(oc->mep_pool, group_p->group_data.lmep_id);
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "LMEP index Free failed EP:%d %s.\n"),
                      group_p->group_data.lmep_id, bcm_errmsg(rv)));
            return (rv);
        }
    }

    for (i=0; i<_CCM_MEP_COUNT; i++) {
        if (_GET_BIT_FLD(group_p->group_data.rmep_bitmap, i) ) {
            sal_memset(&(oc->mep_data[i]), 0, sizeof(_bcm_fp_oam_mep_data_t));

            /* Free the RMEP index */
            rv = shr_idxres_list_free(oc->mep_pool, i);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                          "RMEP index Free failed EP:%d %s.\n"), i,
                          bcm_errmsg(rv)));
                return (rv);
            }
        }
    }

    /* Free the Group index */
    rv = shr_idxres_list_free(oc->group_pool, group_id);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group index Free failed EP:%d %s.\n"), group_id,
                  bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(group_p, 0, sizeof(_bcm_fp_oam_group_data_t));
    group_p->group_data.lmep_id = _FP_OAM_INVALID_LMEP_ID;

    return rv;
}


/*
 * Function:
 *     _bcm_fp_oam_ep_hash_key_construct
 * Purpose:
 *     Construct hash table key for a given endpoint information.
 * Parameters:
 *     unit    - (IN) BCM device number
 *     oc      - (IN) Pointer to OAM control structure.
 *     ep_info - (IN) Pointer to endpoint information structure.
 *     key     - (IN/OUT) Pointer to hash key buffer.
 * Retruns:
 *     None
 */
STATIC void
_bcm_fp_oam_ep_hash_key_construct(int unit, _bcm_fp_oam_control_t *oc,
        bcm_oam_endpoint_info_t *ep_info, _bcm_fp_oam_hash_key_t *key)
{
    uint8   *loc = *key;
    uint8   direction = 0;
    uint8   isLocal = 1;

    sal_memset(key, 0, sizeof(_bcm_fp_oam_hash_key_t));

    if (NULL != ep_info) {

        if (ep_info->flags & BCM_OAM_ENDPOINT_UP_FACING) {
            direction = 1;
        }
        if (ep_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            isLocal = 0;
        }
        _BCM_OAM_KEY_PACK(loc, &direction, sizeof(direction));
        _BCM_OAM_KEY_PACK(loc, &isLocal, sizeof(isLocal));
        _BCM_OAM_KEY_PACK(loc, &ep_info->group, sizeof(ep_info->group));
        _BCM_OAM_KEY_PACK(loc, &ep_info->name, sizeof(ep_info->name));
        _BCM_OAM_KEY_PACK(loc, &ep_info->gport, sizeof(ep_info->gport));
        _BCM_OAM_KEY_PACK(loc, &ep_info->level, sizeof(ep_info->level));
        _BCM_OAM_KEY_PACK(loc, &ep_info->vlan, sizeof(ep_info->vlan));
        _BCM_OAM_KEY_PACK(loc, &ep_info->inner_vlan,
                          sizeof(ep_info->inner_vlan));
    }

    /* End address should not exceed size of _bcm_oam_hash_key_t. */
    assert ((int) (loc - *key) <= sizeof(_bcm_oam_hash_key_t));
}


/*
 * Function:
 *     _bcm_fp_oam_endpoint_params_validate
 * Purpose:
 *     Validate an endpoint parameters.
 * Parameters:
 *     unit      - (IN) BCM device number
 *     oc        - (IN) Pointer to OAM control structure.
 *     hash_key  - (IN) Pointer to endpoint hash key value.
 *     ep_info_p - (IN) Pointer to endpoint information.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_endpoint_params_validate(int unit, _bcm_fp_oam_control_t *oc,
        _bcm_fp_oam_hash_key_t *hash_key,  bcm_oam_endpoint_info_t *ep_info_p)
{
    int rv = 0;
    _bcm_fp_oam_group_data_t *group_p = NULL;

    LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Info: "
              "_bcm_fp_oam_endpoint_params_validate.\n")));

    /* Endpoint must be 802.1ag/Ethernet OAM type. */
    if ((bcmOAMEndpointTypeEthernet != ep_info_p->type))
    {
        /* Other OAM types are not supported, return error. */
        return BCM_E_UNAVAIL;
    }

    /* Supported MDL level is 0 - 7. */
    if ((ep_info_p->level < 0) || (ep_info_p->level > _FP_OAM_MAX_MDL)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
          "EP Level should be in the range(0-%d).\n"), _FP_OAM_MAX_MDL ));
        return (BCM_E_PARAM);
     }

    /* For replace operation, endpoint ID is required. */
    if ((ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)
        && !(ep_info_p->flags & BCM_OAM_ENDPOINT_WITH_ID)) {

        return (BCM_E_PARAM);
    }

    /* If it is a Port + CVlan based MEP or Section Port + CVlan
     * based MEP and the CVlan (inner_vlan) is 0, return E_PARAM
     */
    if ((0 == ep_info_p->inner_vlan) &&
        (ep_info_p->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN)) {
        return (BCM_E_PARAM);
    }

    /* If it is a Port + S + CVlan based MEP or Section Port + S + CVlan
     * based MEP and one of SVlan(vlan) or CVlan (inner_vlan) is 0,
     * return E_PARAM
     */
    if (((0 == ep_info_p->inner_vlan) || (0 == ep_info_p->vlan)) &&
        ((ep_info_p->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN))) {
        return (BCM_E_PARAM);
    }

    /* Validate endpoint index value. */
    if (ep_info_p->flags & BCM_OAM_ENDPOINT_WITH_ID) {
        _BCM_OAM_EP_INDEX_VALIDATE(ep_info_p->id);
    }

    /* Validate endpoint group id. */
    _BCM_FP_OAM_GROUP_INDEX_VALIDATE(ep_info_p->group);

    rv = shr_idxres_list_elem_state(oc->group_pool, ep_info_p->group);
    if (BCM_E_EXISTS != rv) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group (GID:%d) does not exist.\n"), ep_info_p->group));
        return (BCM_E_PARAM);
    }

    /* Only One LMEP should be configured per group */
    group_p = &(oc->group_data[ep_info_p->group]);
    if (!(ep_info_p->flags & BCM_OAM_ENDPOINT_REMOTE)) {

        if ((group_p->group_data.lmep_id != _FP_OAM_INVALID_LMEP_ID) &&
             !(ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "LMEP already existst for the group\n")));
            return (rv);
        }
    }

#if 0
    
    rv = shr_htb_find(oc->ma_mep_htbl, *hash_key,
                      (shr_htb_data_t *)&h_stored_data, 0);

    if (BCM_SUCCESS(rv) && !(ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: %s\n"),
                  bcm_errmsg(BCM_E_EXISTS)));

        /* Endpoint must not be in use except for Replace operation. */
        return (BCM_E_EXISTS);
    } else {
        /* Cannot replace any Key params for an endpoint */
        if ((ep_info_p->flags & BCM_OAM_ENDPOINT_REPLACE) && !(BCM_SUCCESS(rv))) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Endpoint "
                "ID=%d, invalid params for replace operation or Endpoint "
                "doesn't exists\n"), ep_info_p->id));
        }
    }
#endif
    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_fp_oam_get_ctr_info_from_stat_id
 * Purpose:
 *     Get OLP Formatted Counter Id from Stat Id
 * Parameters:
 *     unit          - (IN) BCM device number
 *     stat_id       - (IN) Stat group Id.
 *     offset        - (IN) Offset from Base
 *     counter_id    - (OUT) OLP Formatted counter value
 * Returns:
 *      void
 */
STATIC void
_bcm_fp_oam_get_ctr_info_from_stat_id(int unit, uint32 stat_id, uint8 offset,
                                                           uint32 *counter_id)
{
    bcm_stat_group_mode_t stat_group;
    bcm_stat_object_t stat_object;
    uint32 stat_mode;
    uint32 stat_pool_number;
    uint32 stat_base_idx;

    _bcm_esw_stat_get_counter_id_info(unit, stat_id,
      &stat_group, &stat_object, &stat_mode, &stat_pool_number, &stat_base_idx);

    *counter_id = ( ((stat_base_idx & _FP_OAM_OLP_CTR_WIDTH) + offset) |
                    ((stat_pool_number & _FP_OAM_OLP_CTR_POOL_WIDTH) <<
                     _FP_OAM_OLP_CTR_WIDTH) );

}


/*
 * Function:
 *     _bcm_fp_oam_olp_header_pack
 * Purpose:
 *     Pack OLP Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack OLP Header
 *     mep_data   - (IN)     Pointer to MEP data.
 * Retruns:
 *     BCM_E_XX
 */
STATIC int
_bcm_fp_oam_olp_header_pack(int unit, uint8 *buffer, ccm_mep_data_t *mep_data)
{

    soc_olp_l2_hdr_t olp_l2_hdr;
    soc_olp_tx_hdr_t olp_oam_header;
    int rv = BCM_E_NONE;
    uint32 counter_id;

    /* Get OLP L2 Header for CPU port */
    rv = _bcm_olp_l2_hdr_get(unit, 0, &olp_l2_hdr);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "OLP Header get failed.\n")));
        return (rv);
    }

    /* OLP OAM Header */
    sal_memset(&olp_oam_header, 0, sizeof(olp_oam_header));
    /* Set Port number */
    SOC_OLP_TX_PORT(&olp_oam_header) =
        _BCM_OAM_GLP_PORT_GET(mep_data->dglp);
    SOC_OLP_TX_MODID(&olp_oam_header) =
        _BCM_OAM_GLP_MODULE_ID_GET(mep_data->dglp);

    if (0 < mep_data->num_ctrs) {
        _bcm_fp_oam_get_ctr_info_from_stat_id(unit, mep_data->flex_stat_id[0],
            mep_data->counter_offset[0], &counter_id);
        SOC_OLP_TX_CTR1_ID(&olp_oam_header) = counter_id;
        SOC_OLP_TX_CTR1_LOCATION(&olp_oam_header) = _FP_OAM_OLP_CTR_LOC_EP;
        SOC_OLP_TX_CTR1_ACTION(&olp_oam_header) = _FP_OAM_OLP_CTR_ACTN_INC;
    }
    if (1 < mep_data->num_ctrs) {
        _bcm_fp_oam_get_ctr_info_from_stat_id(unit, mep_data->flex_stat_id[1],
            mep_data->counter_offset[1], &counter_id);
        SOC_OLP_TX_CTR2_ID_SET(&olp_oam_header, counter_id);
        SOC_OLP_TX_CTR2_LOCATION(&olp_oam_header) = _FP_OAM_OLP_CTR_LOC_EP;
        SOC_OLP_TX_CTR2_ACTION(&olp_oam_header) = _FP_OAM_OLP_CTR_ACTN_INC;
    }

    /* Now that we have both L2 and OAM OLP Headers, pack them */
    buffer = _bcm_fp_oam_olp_l2_header_pack(buffer, &olp_l2_hdr);
    buffer = shr_olp_tx_header_pack(buffer, &olp_oam_header);

    return (rv);
}


/*
 * Function:
 *     _bcm_fp_oam_l2_encap_pack
 * Purpose:
 *     Pack OLP Header into buffer.
 * Parameters:
 *     buffer     - (IN/OUT) Buffer to Pack L2 Encap
 *     mep_data   - (IN)     Pointer to MEP data.
 * Retruns:
 *     BCM_E_XX
 */
STATIC uint8*
_bcm_fp_oam_l2_encap_pack(int unit, uint8 *buffer, ccm_mep_data_t *mep_data)
{
    uint32 vlan_tag = 0;
    uint16 cfm_ether_type = _CCM_ETHER_TYPE;
    int i;
    /* Pack DA */
    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buffer, mep_data->da[i]);
    }
    /* Pack SA */
    for (i=0; i<6; i++) {
        _SHR_PACK_U8(buffer, mep_data->sa[i]);
    }
    /* Pack Outer vlan */
    if (mep_data->vlan) {
        vlan_tag = 0;
        vlan_tag = (mep_data->tpid << 16) |
                   ((mep_data->pkt_pri & 0x7) << 13) |
                   (mep_data->vlan & 0x0FFF);
        _SHR_PACK_U32(buffer, vlan_tag);
    }
    /* Pack Inner vlan */
    if (mep_data->inner_vlan) {
        vlan_tag = 0;
        vlan_tag = (mep_data->inner_tpid << 16) |
                   ((mep_data->inner_vlan_pri & 0x7) << 13) |
                   (mep_data->inner_vlan & 0x0FFF);
        _SHR_PACK_U32(buffer, vlan_tag);
    }
    /* Pack EtherType */
    _SHR_PACK_U16(buffer, cfm_ether_type);
    return buffer;
}


/*
 * Function:
 *     _bcm_fp_oam_mep_set_msg_send
 * Purpose:
 *     Send Mep Create/Update Message to UKERNEL
 * Parameters:
 *     unit          - (IN) BCM device number
 *     ep_data       - (IN) Pointer to endpoint information.
 *     uint32        - (IN) Msg Control flags
 * Returns:
 *      BCM_E_XXX
 */
static int
_bcm_fp_oam_mep_set_msg_send(int unit, _bcm_fp_oam_mep_data_t *ep_data,
                            uint32 msg_flags, uint16 *r_msg_len)
{
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    _bcm_fp_oam_control_t *oc = NULL;
    uint8 *dma_buffer1 = NULL;  /* DMA buffer */
    uint8 *dma_buffer2 = NULL;  /* DMA buffer ptr after pack */
    uint16 s_msg_len = 0;       /* Send Message length */
    int rv = 0;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    dma_buffer1 = oc->dma_buffer;

    /* Validate input parameter. */
    if (NULL == ep_data) {
        return (BCM_E_INTERNAL);
    }

    /* Clear mep_msg structure */
    sal_memset(&mep_msg, 0, sizeof(mep_msg));
    mep_msg.flags = msg_flags;

    if (msg_flags & _MEP_L2_ENCAP_RECREATE) {
        dma_buffer1 = _bcm_fp_oam_l2_encap_pack(unit, mep_msg.L2_encap,
                                                &(ep_data->mep_data));
        mep_msg.l2_encap_len = dma_buffer1 - mep_msg.L2_encap;
    }

    if (msg_flags & _MEP_OLP_RECREATE) {
        rv = _bcm_fp_oam_olp_header_pack(unit, mep_msg.OLP_Tx_Hdr, 
                                         &(ep_data->mep_data));
        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "OLP Header Pack failed.\n")));
            return (rv);
        }
    }

    /* Fill Mep data and Send MEP Create message to UKENREL */
    sal_memcpy(&(mep_msg.mep_data), &(ep_data->mep_data),
               sizeof(ccm_mep_data_t));

    /* Pack Mep data to buffer */
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_mep_set_pack(dma_buffer1, &mep_msg);
    s_msg_len = dma_buffer2 - dma_buffer1;
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_SET, s_msg_len, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_SET_REPLY, r_msg_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "MEP Create Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
    }
    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_ccm_endpoint_create
 * Purpose:
 *     Create or replace a CCM OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_ccm_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    _bcm_fp_oam_control_t *oc = NULL;
    _bcm_fp_oam_hash_key_t hash_key;
    _bcm_fp_oam_mep_data_t *ep_data;
    uint32 mep_index;
    int src_glp;
    int dst_glp;
    bcm_trunk_t trunk_id;
    ccm_group_data_t *grp_data_ptr = NULL;
    uint32  msg_flags = 0, dummy_vp;
    uint16  r_len = 0; /* Msg response lenght */
    int rv = 0;
    int is_vp_valid = 0;
    bcm_trunk_member_t   trunk_member;
    int i;

    /* Validate input parameter. */
    if (NULL == endpoint_info) {
        return (BCM_E_PARAM);
    }
    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Calculate the hash key for given enpoint input parameters. */
    _bcm_fp_oam_ep_hash_key_construct(unit, oc, endpoint_info, &hash_key);

    /* Validate endpoint input parameters. */
    rv = _bcm_fp_oam_endpoint_params_validate(unit, oc, &hash_key,
                                              endpoint_info);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Parameter "
            "Validation failed %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    rv = _bcm_fp_oam_endpoint_gport_resolve(unit, endpoint_info, &src_glp,
                                            &dst_glp, &trunk_id, &dummy_vp, 
                                            &is_vp_valid, &trunk_member);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: Gport Resolve "
            "Failed %s.\n"), bcm_errmsg(rv)));
        return (rv);
    }

    /* Now that EP passed all the validation checks, create the endpoint */
    if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REPLACE)) {

        if (endpoint_info->flags & BCM_OAM_ENDPOINT_WITH_ID) {
            /* Reserve the Id passed by the user */
            mep_index = endpoint_info->id;
            rv = shr_idxres_list_reserve(oc->mep_pool, mep_index, mep_index);

        } else {
            /* Allocate the next Id from the pool */
            rv = shr_idxres_list_alloc(oc->mep_pool,
                    (shr_idxres_element_t *)&mep_index);
            endpoint_info->id = mep_index;
        }

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "MEP index alloc failed EP:%d %s.\n"), endpoint_info->id,
                      bcm_errmsg(rv)));
            return (rv);
        }       

        /* Shift to specific data element in the array */
        ep_data = &(oc->mep_data[mep_index]);

        /* If it is a Port + S + CVlan based MEP */
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_OUTER_AND_INNER_VLAN) {
            ep_data->mep_data.type = _FP_OAM_MEP_TYPE_S_C_VLAN;

        } else {

            /* If it is a Port + CVlan based MEP */
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_MATCH_INNER_VLAN) {
                ep_data->mep_data.type = _FP_OAM_MEP_TYPE_CVLAN;

            } else {

                /* If it is a Port + SVlan based MEP */
                if (endpoint_info->vlan) {
                    ep_data->mep_data.type = _FP_OAM_MEP_TYPE_SVLAN;

                } else {
                    /* If it is a Port based MEP */
                    ep_data->mep_data.type = _FP_OAM_MEP_TYPE_PORT;
                }
            }
        }

        ep_data->mep_data.ep_id = mep_index;
        ep_data->mep_data.level = endpoint_info->level;
        ep_data->mep_data.period = endpoint_info->ccm_period;
        ep_data->mep_data.name = endpoint_info->name;

        ep_data->mep_data.gport = endpoint_info->gport;
        ep_data->mep_data.dglp = dst_glp;
        ep_data->mep_data.sglp = src_glp;
        ep_data->mep_data.trunk_index = endpoint_info->trunk_index;

        ep_data->mep_data.group_id = endpoint_info->group;

        ep_data->mep_data.vlan = endpoint_info->vlan;
        ep_data->mep_data.pkt_pri = endpoint_info->pkt_pri;
        ep_data->mep_data.tpid = endpoint_info->outer_tpid;

        ep_data->mep_data.inner_vlan = endpoint_info->inner_vlan;
        ep_data->mep_data.inner_vlan_pri = endpoint_info->inner_pkt_pri;
        ep_data->mep_data.inner_tpid = endpoint_info->inner_tpid;

        sal_memcpy(ep_data->mep_data.da,  endpoint_info->dst_mac_address,
                   sizeof(bcm_mac_t));
        sal_memcpy(ep_data->mep_data.sa,  endpoint_info->src_mac_address,
                   sizeof(bcm_mac_t));

        grp_data_ptr = &(oc->group_data[endpoint_info->group].group_data);
        if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {
                ep_data->mep_data.flags |= _FP_OAM_INTERFACE_STATUS_TX;
                ep_data->mep_data.intf_state = endpoint_info->interface_state;
            }

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
                ep_data->mep_data.flags |= _FP_OAM_PORT_STATUS_TX;
                ep_data->mep_data.port_state = endpoint_info->port_state;
            }

            

            if (endpoint_info->ccm_tx_update_lm_counter_size != 0) {
                ep_data->mep_data.num_ctrs =
                    endpoint_info->ccm_tx_update_lm_counter_size;
                for (i=0; i<ep_data->mep_data.num_ctrs; i++) {
                    ep_data->mep_data.flex_stat_id[i] =
                            endpoint_info->ccm_tx_update_lm_counter_base_id[i];
                    ep_data->mep_data.counter_offset[i] =
                            endpoint_info->ccm_tx_update_lm_counter_offset[i];
                }
            }

            /* Set LMEP ID in group */
            grp_data_ptr->lmep_id = mep_index;
            msg_flags |= _MEP_OLP_RECREATE;
        } else {
            ep_data->mep_data.flags |= _FP_OAM_REMOTE_MEP;
            /* RMEP ID in the group */
            _SET_BIT_FLD(grp_data_ptr->rmep_bitmap, mep_index);
        }

        msg_flags |= (_MEP_L2_ENCAP_RECREATE);
        /* Send MEP Create Message to UKERNEL */
        rv = _bcm_fp_oam_mep_set_msg_send(unit, ep_data, msg_flags, &r_len);

        if (BCM_FAILURE(rv)) {

            /* Free the allocated MEP index */
            shr_idxres_list_free(oc->mep_pool, mep_index);

            /* Clear LMEP ID in Group */
            if (!(endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE)) {
                grp_data_ptr->lmep_id = _FP_OAM_INVALID_LMEP_ID;
            }
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "MEP Creation failed EP:%d %s.\n"), endpoint_info->id,
                  bcm_errmsg(rv)));
            return (rv);

        } else {
            ep_data->in_use = 1;
        }

    } else {
        /* Update case */
        ep_data = &(oc->mep_data[endpoint_info->id]);

        /* For Remote MEP, can't replace anything */
        if (endpoint_info->flags & BCM_OAM_ENDPOINT_REMOTE) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "Cannot replace any parameter in RMEP.\n")));
            return BCM_E_PARAM;
        }

        /* For local MEP, can replace only Intf status, Port status, trunk index
           counter indices */
        if (((ep_data->mep_data.flags & _FP_OAM_INTERFACE_STATUS_TX) !=
            (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX)) ||
            (ep_data->mep_data.intf_state != endpoint_info->interface_state)) {

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_INTERFACE_STATE_TX) {

                ep_data->mep_data.flags |= _FP_OAM_INTERFACE_STATUS_TX;
                ep_data->mep_data.intf_state = endpoint_info->interface_state;
            } else {
                ep_data->mep_data.flags &= ~(_FP_OAM_INTERFACE_STATUS_TX);
                ep_data->mep_data.intf_state = 0;
            }
            /* Something valid was found to be replaced */
            msg_flags |= _MEP_REPLACE;
            msg_flags |= _REPLACE_INTF_STATUS;

        }

        if (((ep_data->mep_data.flags & _FP_OAM_PORT_STATUS_TX) !=
            (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX)) ||
            (ep_data->mep_data.port_state != endpoint_info->port_state)) {

            if (endpoint_info->flags & BCM_OAM_ENDPOINT_PORT_STATE_TX) {
                ep_data->mep_data.flags |= _FP_OAM_PORT_STATUS_TX;
                ep_data->mep_data.port_state = endpoint_info->port_state;
            } else {
                ep_data->mep_data.flags &= ~(_FP_OAM_PORT_STATUS_TX);
                ep_data->mep_data.port_state = 0;
            }
            /* Something valid was found to be replaced */
            msg_flags |= _MEP_REPLACE;
            msg_flags |= _REPLACE_PORT_STATUS;
        }

        if (ep_data->mep_data.trunk_index != endpoint_info->trunk_index) {
            ep_data->mep_data.trunk_index = endpoint_info->trunk_index;
            ep_data->mep_data.dglp = dst_glp;

            msg_flags |= (_MEP_REPLACE | _MEP_OLP_RECREATE);
            msg_flags |= _REPLACE_TRUNK_INDEX;
        }

        if (endpoint_info->flags2 & BCM_OAM_ENDPOINT_FLAGS2_UPDATE_COUNTER_ACTION) {
            if (endpoint_info->ccm_tx_update_lm_counter_size != 0) {
                ep_data->mep_data.num_ctrs =
                    endpoint_info->ccm_tx_update_lm_counter_size;
                for (i=0; i<ep_data->mep_data.num_ctrs; i++) {
                    ep_data->mep_data.flex_stat_id[i] =
                            endpoint_info->ccm_tx_update_lm_counter_base_id[i];
                    ep_data->mep_data.counter_offset[i] =
                            endpoint_info->ccm_tx_update_lm_counter_offset[i];
                }
            }
            msg_flags |= (_MEP_REPLACE | _MEP_OLP_RECREATE);
            msg_flags |= _REPLACE_CTRS;
        }

        if (!(msg_flags & _MEP_REPLACE)) {
            /* Nothing valid was found to be updated */
            rv = BCM_E_PARAM;
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Nothing valid to update:%d %s.\n"), endpoint_info->id,
                bcm_errmsg(rv)));
            return (rv);
        }

        /* Send MEP Update message to UKERNEL */
        rv = _bcm_fp_oam_mep_set_msg_send(unit, ep_data, msg_flags, &r_len);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "MEP Update failed EP:%d %s.\n"), endpoint_info->id,
                  bcm_errmsg(rv)));
            return (rv);
        }
    }

    return rv;

}


/*
 * Function:
 *     bcm_fp_oam_ccm_endpoint_get
 * Purpose:
 *     Create or replace an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_ccm_endpoint_get(int unit, bcm_oam_endpoint_info_t *ep_info_p) {

    _bcm_fp_oam_control_t *oc = NULL;
    _bcm_fp_oam_mep_data_t *ep_data;
    uint16  r_len = 0; /* Msg response lenght */
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    int rv = 0;

    /* Validate input parameter. */
    if (NULL == ep_info_p) {
        return (BCM_E_PARAM);
    }
    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate EP Index */
    _BCM_OAM_EP_INDEX_VALIDATE(ep_info_p->id);

    /* Get pointer to S/w MEP data */
    ep_data = &(oc->mep_data[ep_info_p->id]);

    if (!(ep_data->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_GET_FAULTS_ONLY, ep_info_p->id, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_GET_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "MEP Update failed EP:%d %s.\n"), ep_info_p->id,
              bcm_errmsg(rv)));
        return (rv);
    }
    sal_memset(&mep_msg, 0, sizeof(shr_ccm_msg_ctrl_mep_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_mep_get_unpack(dma_buffer1, &mep_msg);

    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Mep get Msg lenght check failed, id:%d.\n"), ep_info_p->id));
        return (rv);
    }
    ep_info_p->faults = mep_msg.mep_data.faults;
    ep_info_p->persistent_faults = mep_msg.mep_data.persistent_faults;

    /* Fill rest of data from local db */
    ep_info_p->type = bcmOAMEndpointTypeEthernet;
    ep_info_p->group = ep_data->mep_data.group_id;
    ep_info_p->ccm_period = ep_data->mep_data.period;
    ep_info_p->gport = ep_data->mep_data.gport;
    ep_info_p->trunk_index = ep_data->mep_data.trunk_index;
    ep_info_p->name = ep_data->mep_data.name;

    ep_info_p->vlan = ep_data->mep_data.vlan;
    ep_info_p->pkt_pri = ep_data->mep_data.pkt_pri;
    ep_info_p->outer_tpid = ep_data->mep_data.tpid;

    ep_info_p->inner_vlan = ep_data->mep_data.inner_vlan;
    ep_info_p->inner_pkt_pri = ep_data->mep_data.inner_vlan_pri;
    ep_info_p->inner_tpid = ep_data->mep_data.inner_tpid;

    if (!(ep_data->mep_data.flags & _FP_OAM_REMOTE_MEP)) {

        if (ep_data->mep_data.flags & _FP_OAM_INTERFACE_STATUS_TX) {
            ep_info_p->flags |= BCM_OAM_ENDPOINT_INTERFACE_STATE_TX;
            ep_info_p->interface_state = ep_data->mep_data.intf_state;
        }

        if (ep_data->mep_data.flags & _FP_OAM_PORT_STATUS_TX) {
            ep_info_p->flags |= BCM_OAM_ENDPOINT_PORT_STATE_TX;
            ep_info_p->port_state = ep_data->mep_data.port_state;
        }

        ep_info_p->lm_counter_base_id = ep_data->mep_data.flex_stat_id[0];
        sal_memcpy(ep_info_p->src_mac_address, ep_data->mep_data.sa,
                   sizeof(bcm_mac_t));
        sal_memcpy(ep_info_p->dst_mac_address, ep_data->mep_data.da,
                   sizeof(bcm_mac_t));
    } else {
        /* Set remote flag */
        ep_info_p->flags |= BCM_OAM_ENDPOINT_REMOTE;
        ep_info_p->interface_state = mep_msg.mep_data.intf_state;
        ep_info_p->port_state = mep_msg.mep_data.port_state;
        sal_memcpy(ep_info_p->src_mac_address, mep_msg.mep_data.sa, 
                   sizeof(bcm_mac_t));
        sal_memcpy(ep_info_p->dst_mac_address, mep_msg.mep_data.da, 
                   sizeof(bcm_mac_t));
    }

    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_ccm_group_destroy
 * Purpose:
 *     Delete an OAM group object. All OAM endpoints associated with the
 *     group will also be destroyed.
 * Parameters:
 *     unit       - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to an OAM group Id.
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_ccm_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    _bcm_fp_oam_control_t *oc = NULL;      /* OAM control structure */
    _bcm_fp_oam_mep_data_t *ep_p;          /* Handle to S/w Group Data */
    int rv = 0;                            /* Return Value */
    uint16 r_len;                          /* Msg reply lenght */
    _bcm_fp_oam_group_data_t *grp_p;       /* Handle to S/w Group Data */

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Validate EP Index */
    _BCM_OAM_EP_INDEX_VALIDATE(endpoint);

    /* Get Handle to Local DB */
    ep_p = &(oc->mep_data[endpoint]);
    if (!(ep_p->in_use)) {
        return BCM_E_NOT_FOUND;
    }

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_DELETE, endpoint, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_DELETE_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Mep Del Msg returned error %s.\n"), bcm_errmsg(rv)));
        rv = BCM_E_INTERNAL;
        return rv;
    }

    /* Clean up Group's LMEP/RMEP reference */
    grp_p = &(oc->group_data[ep_p->mep_data.group_id]);
    if (ep_p->mep_data.flags &  _FP_OAM_REMOTE_MEP) {
        _CLR_BIT_FLD(grp_p->group_data.rmep_bitmap, endpoint);
    } else {
        grp_p->group_data.lmep_id = _FP_OAM_INVALID_LMEP_ID;
    }

    /* Free the EP index */
    rv = shr_idxres_list_free(oc->mep_pool, endpoint);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "MEP index Free failed EP:%d %s.\n"), endpoint,
                  bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(ep_p, 0, sizeof(_bcm_fp_oam_mep_data_t));

    return rv;
}


/*
 * Function:
 *      _bcm_fp_oam_ccm_event_mask_set
 * Purpose:
 *      Set the CCM Events mask.
 * Parameters:
 *      unit        - (IN) Unit number.
 * Returns:
 *      BCM_E_NONE Operation completed successfully
 *      BCM_E_XXX  Operation failed
 * Notes:
 */
STATIC int
_bcm_fp_oam_ccm_event_mask_set(int unit)
{
    _bcm_fp_oam_control_t *oc;
    _bcm_oam_event_handler_t *event_handler_p;
    uint32 event_mask = 0;
    uint16 reply_len;
    int rv = BCM_E_NONE;

    /* Lock already taken by the calling routine. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    /* Get event mask from all callbacks */
    for (event_handler_p = oc->event_handler_list_p;
         event_handler_p != NULL;
         event_handler_p = event_handler_p->next_p) {

        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMxcon)) {
            event_mask |= G_XCON;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMError)) {
            event_mask |= G_CCM_ERROR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupRemote)) {
            event_mask |= G_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMTimeout)) {
            event_mask |= G_CCM_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupMACStatus)) {
            event_mask |= G_CCM_MAC;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMxconClear)) {
            event_mask |= G_XCON_CLR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMErrorClear)) {
            event_mask |= G_CCM_ERROR_CLR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupRemoteClear)) {
            event_mask |= G_RDI_CLR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupCCMTimeoutClear)) {
            event_mask |= G_CCM_TIME_IN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventGroupMACStatusClear)) {
            event_mask |= G_CCM_MAC_CLR;
        }

        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointPortDown)) {
            event_mask |= E_PORT_DOWN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointPortUp)) {
            event_mask |= E_PORT_UP;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceDown)) {
            event_mask |= E_INTF_DOWN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceUp)) {
            event_mask |= E_INTF_UP;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceTesting)) {
            event_mask |= E_INTF_TEST;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceUnkonwn)) {
            event_mask |= E_INTF_UK;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceDormant)) {
            event_mask |= E_INTF_DOR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceDormantToUp)) {
            event_mask |= E_INTF_DOR;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceNotPresent)) {
            event_mask |= E_INTF_NP;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointInterfaceLLDown)) {
            event_mask |= E_INTF_LL;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointCCMTimeout)) {
            event_mask |= E_CCM_TIMEOUT;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointCCMTimein)) {
            event_mask |= E_CCM_TIME_IN;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointRemote)) {
            event_mask |= E_CCM_RDI;
        }
        if (SHR_BITGET(event_handler_p->event_types.w,
                       bcmOAMEventEndpointRemoteUp)) {
            event_mask |= E_CCM_RDI_CLR;
        }
    }

    /* Update CCM event mask in uKernel */
    if (event_mask != oc->event_mask) {
        /* Send CCM Event Mask message to uC */
        rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
                 MOS_MSG_SUBCLASS_CCM_EVENT_MASK_SET, 0, event_mask,
                 MOS_MSG_SUBCLASS_CCM_EVENT_MASK_SET_REPLY,
                 &reply_len);

        if (BCM_FAILURE(rv)) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                "Event Mask set Msg returned error %s.\n"), bcm_errmsg(rv)));
            rv = BCM_E_INTERNAL;
        }
    }

    oc->event_mask = event_mask;

    return (rv);
}


/*
 * Function:
 *      _bcm_fp_oam_ccm_callback_thread
 * Purpose:
 *      Thread to listen for event messages from uController.
 * Parameters:
 *      param - Pointer to CCM info structure.
 * Returns:
 *      None
 */
STATIC void
_bcm_fp_oam_ccm_callback_thread(void *param)
{
    int rv;
    _bcm_fp_oam_control_t *oc = (_bcm_fp_oam_control_t *)param;
    bcm_oam_event_types_t events;
    bcm_oam_event_type_t event_type;
    mos_msg_data_t event_msg;
    int data_index = 0;
    uint32 event_mask;
    _bcm_oam_event_handler_t *event_handler_p;
    int ep_id = BCM_OAM_ENDPOINT_INVALID;
    int group_id = BCM_OAM_GROUP_INVALID;
    char thread_name[SAL_THREAD_NAME_MAX_LEN];

    thread_name[0] = 0;
    sal_thread_name(oc->event_thread_id, thread_name, sizeof (thread_name));

    while (1) {
        /* Wait on notifications from uController */
        rv = soc_cmic_uc_msg_receive(oc->unit, oc->ccm_uc_num,
                                     MOS_MSG_CLASS_CCM_EVENT, &event_msg,
                                     sal_sem_FOREVER);
        if (BCM_FAILURE(rv)) {
            break;  /*  Thread exit */
        }

        /* Get data from event message */
        data_index = (int)bcm_ntohs(event_msg.s.len);
        event_mask = bcm_ntohl(event_msg.s.data);

        /* One event Msg can either carry one or more Events related
           Group or Endpoint, Check whether Group event or EP event */

         /* Validate EP Index */
         if ( (data_index < 0) || (data_index >= oc->mep_count) ||
             !(oc->mep_data[data_index].in_use) ) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META("OAM Error: "
                "CCM Event with Invalid EP ID = %d.\n"), data_index));
            continue;
         }
         ep_id = data_index;
         group_id = oc->mep_data[data_index].mep_data.group_id;

        /* Set events */
        sal_memset(&events, 0, sizeof(events));
        if (event_mask & G_EVENT_MASK) {

            /* Group Events */
            if (event_mask & G_XCON) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMxcon - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMxcon] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMxcon);
                }
            }
            if (event_mask & G_CCM_ERROR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMError - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMError] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMError);
                }
            }

            if (event_mask & G_RDI) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupRemote] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupRemote);
                }
            }

            if (event_mask & G_CCM_TIMEOUT) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMTimeout - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMTimeout] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMTimeout);
                }
            }

            if (event_mask & G_CCM_MAC) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupMACStatus - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupMACStatus] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupMACStatus);
                }
            }

            /* Group Clear Events */
            if (event_mask & G_XCON_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMxconClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMxconClear);
                }
            }

            if (event_mask & G_CCM_ERROR_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMTimeout - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMErrorClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMErrorClear);
                }
            }

            if (event_mask & G_RDI_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupRemoteClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupRemoteClear);
                }
            }

            if (event_mask & G_CCM_TIME_IN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupCCMTimeoutClear - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupCCMTimeoutClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupCCMTimeoutClear);
                }
            }

            if (event_mask & G_CCM_MAC_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventGroupMACStatusClear - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventGroupMACStatusClear] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventGroupMACStatusClear);
                }
            }
        }
        if ( event_mask & E_EVENT_MASK) {

            /* Endpoint Events */
            if (event_mask & E_PORT_DOWN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointPortDown - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointPortDown] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointPortDown);
                }
            }

            if (event_mask & E_PORT_UP) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointPortUp - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointPortUp] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointPortUp);
                }
            }

            if (event_mask & E_INTF_DOWN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceDown - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceDown] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceDown);
                }
            }

            if (event_mask & E_INTF_UP) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceUp - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceUp] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceUp);
                }
            }

            if (event_mask & E_INTF_TEST) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceTesting - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceTesting] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceTesting);
                }
            }

            if (event_mask & E_INTF_UK) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceUnkonwn - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceUnkonwn] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceUnkonwn);
                }
            }

            if (event_mask & E_INTF_DOR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceDormant - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceDormant] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceDormant);
                }
            }

            if (event_mask & E_INTF_NP) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceNotPresent - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceNotPresent] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceNotPresent);
                }
            }

            if (event_mask & E_INTF_LL) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointInterfaceLLDown - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointInterfaceLLDown] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointInterfaceLLDown);
                }
            }

            if (event_mask & E_CCM_TIMEOUT) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointCCMTimeout - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointCCMTimeout] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointCCMTimeout);
                }
            }

            if (event_mask & E_CCM_TIME_IN) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointCCMTimein - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointCCMTimein] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointCCMTimein);
                }
            }

            if (event_mask & E_CCM_RDI) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointRemote - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointRemote] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointRemote);
                }
            }

            if (event_mask & E_CCM_RDI_CLR) {
                LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit,
                    "bcmOAMEventEndpointRemoteUp - %d\n"), data_index));

                if (oc->event_handler_cnt[bcmOAMEventEndpointRemoteUp] > 0) {
                    SHR_BITSET(events.w, bcmOAMEventEndpointRemoteUp);
                }
            }
        }

        /* Loop over registered callbacks,
         * If any match the events field, then invoke
         */
        for (event_handler_p = oc->event_handler_list_p; event_handler_p != NULL;
             event_handler_p = event_handler_p->next_p) {

            for (event_type = bcmOAMEventEndpointPortDown;
                 event_type < bcmOAMEventCount; ++event_type) {

                if (SHR_BITGET(events.w, event_type)) {
                    if (SHR_BITGET(event_handler_p->event_types.w, event_type)) {
                        event_handler_p->cb(oc->unit,
                        0,
                        event_type,
                        group_id, /* Group index */
                        ep_id, /* Endpoint index */
                        event_handler_p->user_data);
                    }
                }
            }
        }
    }

    oc->event_thread_id = NULL;
    LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(oc->unit, "Thread Exit:%s\n"),
               thread_name));
    sal_thread_exit(0);
}


/*
 * Function:
 *      _bcm_fp_oam_cmm_group_regen
 * Purpose:
 *      Reconstruct CCM Group S/w State from UKENREL on WB
 *      Assumes Lock
 * Parameters:
 *      unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_cmm_group_regen(int unit, int grp_id) {

    int rv;
    _bcm_fp_oam_control_t *oc;  /* Pointer to OAM control structure */
    uint16 r_len;
    shr_ccm_msg_ctrl_group_data_t grp_msg;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET, grp_id, 0,
            MOS_MSG_SUBCLASS_CCM_GROUP_GET_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "Group Get failed Id:%d %s.\n"), grp_id, bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(&grp_msg, 0, sizeof(shr_ccm_msg_ctrl_group_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_group_get_unpack(dma_buffer1, &grp_msg);

    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Group get Msg lenght check failed, Id:%d.\n"), grp_id));
        return (rv);
    }

    sal_memcpy(&(oc->group_data[grp_id].group_data) , &(grp_msg.group_data), 
               sizeof(ccm_group_data_t));

    rv = shr_idxres_list_reserve(oc->group_pool, grp_id, grp_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Group index reservation failed, id:%d.\n"), grp_id));
        return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
    }
    oc->group_data[grp_id].in_use = 1;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_fp_oam_cmm_mep_regen
 * Purpose:
 *      Reconstruct CCM MEP S/w State from UKENREL on WB
 *      Assumes Lock
 * Parameters:
 *      unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_cmm_mep_regen(int unit, int mep_id) {

    int rv;
    _bcm_fp_oam_control_t *oc;  /* Pointer to OAM control structure */
    uint16 r_len;
    shr_ccm_msg_ctrl_mep_data_t mep_msg;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));

    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
            MOS_MSG_SUBCLASS_CCM_MEP_GET, mep_id, 0,
            MOS_MSG_SUBCLASS_CCM_MEP_GET_REPLY, &r_len);

    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "MEP Get failed EP:%d %s.\n"), mep_id, bcm_errmsg(rv)));
        return (rv);
    }

    sal_memset(&mep_msg, 0, sizeof(shr_ccm_msg_ctrl_mep_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_mep_get_unpack(dma_buffer1, &mep_msg);

    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "Mep get Msg lenght check failed, id:%d.\n"), mep_id));
        return (rv);
    }

    sal_memcpy(&(oc->mep_data[mep_id].mep_data) , &(mep_msg.mep_data), 
               sizeof(ccm_mep_data_t));

    rv = shr_idxres_list_reserve(oc->mep_pool, mep_id, mep_id);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                  "Mep index reservation failed, id:%d.\n"), mep_id));
        return ((rv == BCM_E_RESOURCE) ? (BCM_E_EXISTS) : rv);
    }
    oc->mep_data[mep_id].in_use = 1;

    return BCM_E_NONE;
}


/*
 * Function:
 *      _bcm_fp_oam_ccm_reinit
 * Purpose:
 *      Reconstruct CCM S/w State from UKENREL on WB
 * Parameters:
 *      unit - (IN) BCM device number
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_ccm_reinit(int unit) {
    _bcm_fp_oam_control_t *oc;  /* Pointer to OAM control structure */
    int rv = BCM_E_NONE;        /* Operation return status */
    uint16 r_len = 0;
    shr_ccm_msg_ctrl_app_data_t app_data_msg;
    uint8 *dma_buffer1 = NULL;
    uint8 *dma_buffer2 = NULL;
    int i, j;
    ccm_group_data_t *group_ptr = NULL;

    /* Get OAM control structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    /* Recover group Bitmap from UKENREL */
    rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
        MOS_MSG_SUBCLASS_CCM_GET_APP_DATA, 0, 0,
        MOS_MSG_SUBCLASS_CCM_GET_APP_DATA_REPLY, &r_len);
    if (BCM_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
              "App data get failed %s\n"), bcm_errmsg(rv)));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    sal_memset(&app_data_msg, 0, sizeof(shr_ccm_msg_ctrl_app_data_t));
    dma_buffer1 = oc->dma_buffer;
    dma_buffer2 = shr_ccm_msg_ctrl_app_data_unpack(dma_buffer1, &app_data_msg);
    if (r_len != dma_buffer2 - dma_buffer1) {
        rv = BCM_E_INTERNAL;
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
            "App Data get Length mismatch\n")));
        _BCM_OAM_UNLOCK(oc);
        return (rv);
    }

    /* Recover Group Data from UKENREL */
    for (i=0; i<oc->group_count; i++) {
        if (_GET_BIT_FLD(app_data_msg.group_bitmap, i)) {
            rv = _bcm_fp_oam_cmm_group_regen(unit, i);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                      "Group Regen failed, Id:%d, %s\n"), i, bcm_errmsg(rv)));
                _BCM_OAM_UNLOCK(oc);
                return (rv);
            }

            /* Now that we have reconstrcuted the group, Reconstrcut its 
               LMEP & RMEPs */
            group_ptr = &(oc->group_data[i].group_data);
            if (group_ptr->lmep_id != _FP_OAM_INVALID_LMEP_ID) {

                /* Get LMEP */
                rv = _bcm_fp_oam_cmm_mep_regen(unit, group_ptr->lmep_id);
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                          "LMEP Regen failed, Id:%d, %s\n"), group_ptr->lmep_id,
                          bcm_errmsg(rv)));
                    _BCM_OAM_UNLOCK(oc);
                    return (rv);
                }

                /* Get RMEPs */
                for (j=0; j<oc->mep_count; j++) {
                    if (_GET_BIT_FLD(group_ptr->rmep_bitmap, j)) {
                        rv = _bcm_fp_oam_cmm_mep_regen(unit, j);
                        if (BCM_FAILURE(rv)) {
                            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, 
                                "OAM Error: RMEP Regen failed Id:%d %s\n"), j,
                                bcm_errmsg(rv)));
                            _BCM_OAM_UNLOCK(oc);
                            return (rv);
                        }
                    }
                }
            }
        }
    }

    _BCM_OAM_UNLOCK(oc);
    return BCM_E_NONE;
}

/*******************************************************************************/
/* CCM Embedded App supporting functions End */
/*******************************************************************************/
/*
 * Function:
 *     bcm_fp_oam_event_register
 * Purpose:
 *     Register a callback for handling OAM events
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM events for which the specified
 *                        callback should be called.
 *     cb          - (IN) A pointer to the callback function to call for
 *                        the specified OAM events
 *     user_data   - (IN) Pointer to user data to supply in the callback
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_event_register(int unit, bcm_oam_event_types_t event_types,
                        bcm_oam_event_cb cb, void *user_data)
{
    _bcm_fp_oam_control_t    *oc;
    _bcm_oam_event_handler_t *event_h_p;
    _bcm_oam_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t     e_type;
    uint32                   event_bmp;
    int                      rv = 0;
    int                      update_event_mask = 0;

    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Check if an event is set for register in the events bitmap. */
    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, event_bmp);
    if (0 == event_bmp) {
        /* No events specified. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "No events specified for register.\n"), unit));
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {

        _BCM_OAM_ALLOC(event_h_p, _bcm_oam_event_handler_t,
             sizeof(_bcm_oam_event_handler_t), "OAM event handler");

        if (NULL == event_h_p) {
            LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
                "Event handler alloc failed - %s.\n"), unit,
                bcm_errmsg(BCM_E_MEMORY)));
            return (BCM_E_MEMORY);
        }

        event_h_p->next_p = NULL;
        event_h_p->cb = cb;
        event_h_p->user_data = user_data;

        SHR_BITCLR_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount);
        if (prev_p != NULL) {
            prev_p->next_p = event_h_p;
        } else {
            oc->event_handler_list_p = event_h_p;
        }
    }

    for (e_type = 0; e_type < bcmOAMEventCount; ++e_type) {
        if (SHR_BITGET(event_types.w, e_type)) {
            if ((e_type == bcmOAMEventEndpointPortDown) ||
                (e_type == bcmOAMEventEndpointPortUp) ||
                (e_type == bcmOAMEventEndpointInterfaceDown) ||
                (e_type == bcmOAMEventEndpointInterfaceUp) ||
                (e_type == bcmOAMEventEndpointInterfaceTesting) ||
                (e_type == bcmOAMEventEndpointInterfaceUnkonwn) ||
                (e_type == bcmOAMEventEndpointInterfaceDormant) ||
                (e_type == bcmOAMEventEndpointInterfaceNotPresent) ||
                (e_type == bcmOAMEventEndpointInterfaceLLDown) ||
                (e_type == bcmOAMEventGroupCCMxcon) ||
                (e_type == bcmOAMEventGroupCCMError) ||
                (e_type == bcmOAMEventGroupRemote) ||
                (e_type == bcmOAMEventGroupCCMTimeout) ||
                (e_type == bcmOAMEventGroupMACStatus) ||
                (e_type == bcmOAMEventGroupCCMxconClear) ||
                (e_type == bcmOAMEventGroupCCMErrorClear) ||
                (e_type == bcmOAMEventGroupRemoteClear) ||
                (e_type == bcmOAMEventGroupCCMTimeoutClear) ||
                (e_type == bcmOAMEventGroupMACStatusClear) ||
                (e_type == bcmOAMEventEndpointCCMTimeout) ||
                (e_type == bcmOAMEventEndpointCCMTimein) ||
                (e_type == bcmOAMEventEndpointRemote) ||
                (e_type == bcmOAMEventEndpointRemoteUp)) {

                SHR_BITSET(event_h_p->event_types.w, e_type);
                oc->event_handler_cnt[e_type] += 1;
                update_event_mask = 1;
                continue;
            }
        }
    }

    if (update_event_mask) {
        /* Update CCM Events mask */
        rv = _bcm_fp_oam_ccm_event_mask_set(unit);
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_event_unregister
 * Purpose:
 *     Unregister event and its callback from the event handler list
 * Parameters:
 *     unit        - (IN) BCM device number
 *     event_types - (IN) The set of OAM events for which the specified
 *                        callback should not be called.
 *     cb          - (IN) A pointer to the callback function to unregister
 *                        from the specified OAM events
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_event_unregister(int unit, bcm_oam_event_types_t event_types,
                        bcm_oam_event_cb cb)
{
    _bcm_fp_oam_control_t    *oc;
    _bcm_oam_event_handler_t *event_h_p;
    _bcm_oam_event_handler_t *prev_p = NULL;
    bcm_oam_event_type_t     e_type;
    uint32                   event_bmp;
    int                      rv = 0;
    int                      update_event_mask = 0;

    /* Validate event callback input parameter. */
    if (NULL == cb) {
        return (BCM_E_PARAM);
    }

    /* Check if an event is set for unregister in the events bitmap. */
    SHR_BITTEST_RANGE(event_types.w, 0, bcmOAMEventCount, event_bmp);
    if (0 == event_bmp) {
        /* No events specified. */
        LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM(unit %d) Error: "
            "No events specified for unregister.\n"), unit));
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

    for (event_h_p = oc->event_handler_list_p; event_h_p != NULL;
         event_h_p = event_h_p->next_p) {
        if (event_h_p->cb == cb) {
            break;
        }
        prev_p = event_h_p;
    }

    if (NULL == event_h_p) {
        return (BCM_E_NOT_FOUND);
    }

    for (e_type = 0; e_type < bcmOAMEventCount; ++e_type) {
        if (SHR_BITGET(event_types.w, e_type)) {
                /* CCM events are generated by the uKernel */
            if ((e_type == bcmOAMEventEndpointPortDown) ||
                (e_type == bcmOAMEventEndpointPortUp) ||
                (e_type == bcmOAMEventEndpointInterfaceDown) ||
                (e_type == bcmOAMEventEndpointInterfaceUp) ||
                (e_type == bcmOAMEventEndpointInterfaceTestingToUp) ||
                (e_type == bcmOAMEventEndpointInterfaceUnknownToUp) ||
                (e_type == bcmOAMEventEndpointInterfaceDormantToUp) ||
                (e_type == bcmOAMEventEndpointInterfaceNotPresentToUp) ||
                (e_type == bcmOAMEventEndpointInterfaceLLDownToUp) ||
                (e_type == bcmOAMEventEndpointInterfaceTesting) ||
                (e_type == bcmOAMEventEndpointInterfaceUnkonwn) ||
                (e_type == bcmOAMEventEndpointInterfaceDormant) ||
                (e_type == bcmOAMEventEndpointInterfaceNotPresent) ||
                (e_type == bcmOAMEventEndpointInterfaceLLDown) ||
                (e_type == bcmOAMEventGroupCCMxcon) ||
                (e_type == bcmOAMEventGroupCCMError) ||
                (e_type == bcmOAMEventGroupRemote) ||
                (e_type == bcmOAMEventGroupCCMTimeout) ||
                (e_type == bcmOAMEventEndpointCCMTimeout) ||
                (e_type == bcmOAMEventEndpointCCMTimein) ||
                (e_type == bcmOAMEventEndpointRemote) ||
                (e_type == bcmOAMEventEndpointRemoteUp)) {

                SHR_BITCLR(event_h_p->event_types.w, e_type);
                oc->event_handler_cnt[e_type] -= 1;
                update_event_mask = 1;
                continue;
            }
        }
    }
    SHR_BITTEST_RANGE(event_h_p->event_types.w, 0, bcmOAMEventCount, event_bmp);

    if (0 == event_bmp) {

        if (NULL != prev_p) {

            prev_p->next_p = event_h_p->next_p;

        } else {

            oc->event_handler_list_p = event_h_p->next_p;

        }
        sal_free(event_h_p);
    }

    if (update_event_mask) {
        /* Update CCM Events mask */
        rv = _bcm_fp_oam_ccm_event_mask_set(unit);
    }

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}


/*
 * Function:
 *     _bcm_fp_oam_resource_count_init
 * Purpose:
 *     Retrieves and initializes endpoint count information for this device.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to device OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_resource_count_init(int unit, _bcm_fp_oam_control_t *oc)
{
    /* Input parameter check. */
    if (NULL == oc) {
        return (BCM_E_PARAM);
    }
    /*
     * Get endpoint firmware table index count values and
     * initialize device OAM control structure members variables.
     */
    oc->mep_count = soc_property_get(unit, spn_OAM_CCM_MAX_MEPS, 256);
    oc->group_count = soc_property_get(unit, spn_OAM_CCM_MAX_GROUPS, 256);

    return (BCM_E_NONE);
}


/*
 * Function:
 *     _bcm_fp_oam_control_free
 * Purpose:
 *     Free OAM control structure resources allocated by this unit.
 * Parameters:
 *     unit -  (IN) BCM unit number.
 *     oc   -  (IN) Pointer to OAM control structure.
 * Retruns:
 *     BCM_E_XXX
 */
STATIC int
_bcm_fp_oam_control_free(int unit, _bcm_fp_oam_control_t *oc)
{
    _fp_oam_control[unit] = NULL;

    if (NULL == oc) {
        /* Module already un-initialized */
        return (BCM_E_NONE);
    }

    /* Free protection mutex */
    if (NULL != oc->oc_lock) {
        sal_mutex_destroy(oc->oc_lock);
        oc->oc_lock = NULL;
    }

    /* Destory group indices list */
    if (NULL != oc->group_pool) {
       shr_idxres_list_destroy(oc->group_pool);
       oc->group_pool = NULL;
    }

    /* Free group memory. */
    if (NULL != oc->group_data) {
        sal_free(oc->group_data);
        oc->group_data = NULL;
    }

    /* Destroy LMEP indices list */
    if (NULL != oc->mep_pool) {
        shr_idxres_list_destroy(oc->mep_pool);
        oc->mep_pool = NULL;
    }

    /* Free LMEP memory. */
    if (NULL != oc->mep_data) {
        sal_free(oc->mep_data);
    }
#if 0
    /* Free hash data storage memory */
    if (NULL != oc->oam_hash_data) {
        sal_free(oc->oam_hash_data);
    }
#endif

    /* Free OAM control structure memory. */
    sal_free(oc);
    oc = NULL;

    return (BCM_E_NONE);

}


/*
 * Function:
 *     bcm_fp_oam_detach
 * Purpose:
 *     Shut down OAM subsystem
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_detach(int unit) {
    int rv = 0;
    _bcm_fp_oam_control_t    *oc = NULL;
    uint16 reply_len;

    /* Get OAM Control Structure. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined(INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm)) {
        if (oc->ccm_ukernel_ready) {
            /* Event Handler thread exit signal */
            sal_usecs_t timeout = sal_time_usecs() + 5000000;
            while (NULL !=  oc->event_thread_id) {
                soc_cmic_uc_msg_receive_cancel(unit, oc->ccm_uc_num,
                                               MOS_MSG_CLASS_CCM_EVENT);

                if (sal_time_usecs() < timeout) {
                    /*give some time to already running CCM callback thread
                     * to schedule and exit */
                    sal_usleep(10000);
                } else {
                    /*timeout*/
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                              "CCM event thread did not exit.\n")));
                    _BCM_OAM_UNLOCK(oc);
                    return BCM_E_INTERNAL;
                }
            }

            if (!SOC_WARM_BOOT(unit)) {
                /*
                 * Send CCM Uninit message to uC
                 * Ignore error since that may indicate uKernel was reloaded.
                 */
                rv = _bcm_fp_oam_msg_send_receive(unit,
                        MOS_MSG_CLASS_CCM, 
                        MOS_MSG_SUBCLASS_CCM_UNINIT,
                        0, 0,
                        MOS_MSG_SUBCLASS_CCM_UNINIT_REPLY,
                        &reply_len);
                if (BCM_SUCCESS(rv) && (reply_len != 0)) {
                    if (NULL != oc->oc_lock) {
                        _BCM_OAM_UNLOCK(oc);
                    }
                    return BCM_E_INTERNAL;
                } else {
                    if (rv == BCM_E_TIMEOUT) {
                        LOG_DEBUG(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM: "
                            "Uninit timedout - %s\n"), bcm_errmsg(rv)));
                        rv = BCM_E_NONE;
                    }
                }
            }
        }

    }
#endif /* INCLUDE_CCM */

    _BCM_OAM_UNLOCK(oc);
    _bcm_fp_oam_control_free(unit, oc);
    return rv;
}


/*
 * Function:
 *     bcm_fp_oam_endpoint_create
 * Purpose:
 *     Create or replace an OAM endpoint object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     endpoint_info - (IN/OUT) Pointer to endpoint information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_create(int unit, bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) &&
        (endpoint_info->type  == bcmOAMEndpointTypeEthernet)) {
        rv = bcm_fp_oam_ccm_endpoint_create(unit, endpoint_info);
    }
#endif

    _BCM_OAM_UNLOCK(oc);
    return rv;
}


/*
 * Function:
 *     bcm_fp_oam_endpoint_get
 * Purpose:
 *     Fetches an OAM endpoint object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     endpoint - (IN) Endpoint ID to get.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_get(int unit, bcm_oam_endpoint_t endpoint,
                        bcm_oam_endpoint_info_t *endpoint_info)
{
    int rv = BCM_E_UNAVAIL; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((endpoint >= 0) &&  (endpoint < oc->mep_count))) {
        endpoint_info->id = endpoint;
        rv = bcm_fp_oam_ccm_endpoint_get(unit, endpoint_info);     
    }
#endif

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_endpoint_destroy
 * Purpose:
 *     Destroy an OAM endpoint object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     endpoint - (IN) Endpoint ID to destroy.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_endpoint_destroy(int unit, bcm_oam_endpoint_t endpoint)
{
    int rv = BCM_E_UNAVAIL; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((endpoint >= 0) &&  (endpoint < oc->mep_count))) {
        rv = bcm_fp_oam_ccm_endpoint_destroy(unit, endpoint);
    }
#endif

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_group_create
 * Purpose:
 *     Create or replace an OAM group object
 * Parameters:
 *     unit          - (IN) BCM device number
 *     group_info - (IN/OUT) Pointer to group information buffer.
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_fp_oam_group_create(int unit, bcm_oam_group_info_t *group_info)
{
    int rv = BCM_E_UNAVAIL;
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm)) {
        rv = bcm_fp_oam_ccm_group_create(unit, group_info);
    }
#endif

    _BCM_OAM_UNLOCK(oc);
    return rv;
}


/*
 * Function:
 *     bcm_fp_oam_endpoint_get
 * Purpose:
 *     Fetches an OAM group object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group - (IN) Group ID to get.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_group_get(int unit,  bcm_oam_group_t group,
                         bcm_oam_group_info_t *group_info)
{
    int rv = BCM_E_UNAVAIL; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((group >= 0) &&  (group < oc->group_count))) {
        group_info->id = group;
        rv = bcm_fp_oam_ccm_group_get(unit, group_info);     
    }
#endif

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}


/*
 * Function:
 *     bcm_fp_oam_group_destroy
 * Purpose:
 *     Destroy an OAM group object
 * Parameters:
 *     unit     - (IN) BCM device number
 *     group    - (IN) Group ID to destroy.
 * result =s:
 *     BCM_E_XXX
 */
int
bcm_fp_oam_group_destroy(int unit, bcm_oam_group_t group)
{
    int rv = BCM_E_UNAVAIL; /* Operation return status */
    _bcm_fp_oam_control_t *oc = NULL;

    /* Get OAM control structure handle. */
    BCM_IF_ERROR_RETURN(_bcm_fp_oam_control_get(unit, &oc));
    _BCM_OAM_LOCK(oc);

#if defined (INCLUDE_CCM)
    if (soc_feature(unit, soc_feature_uc_ccm) && 
        ((group >= 0) &&  (group < oc->group_count))) {
        rv = bcm_fp_oam_ccm_group_destroy(unit, group);
    }
#endif

    _BCM_OAM_UNLOCK(oc);
    return (rv);
}
/*******************************************************************************/
/* CCM Embedded App supporting functions End */
/*******************************************************************************/
#endif

/*
 * Function: bcm_td2p_oam_init
 *
 * Purpose:
 *     Initialize OAM module.
 * Parameters:
 *     unit - (IN) BCM device number
 * Returns:
 *     BCM_E_UNIT    - Invalid BCM unit number.
 *     BCM_E_UNAVAIL - OAM not support on this device.
 *     BCM_E_MEMORY  - Allocation failure
 *     CM_E_XXX     - Error code from bcm_XX_oam_init()
 *     BCM_E_NONE    - Success
 */
int
bcm_td2p_oam_init(int unit)
{
    int rv;             /* Operation return value. */
#if defined (INCLUDE_CCM)
    _bcm_fp_oam_control_t *oc = NULL;  /* OAM control structure. */
    shr_ccm_msg_ctrl_init_t ccm_init_msg;
    uint8 *buffer_p1 = NULL;
    uint8 *buffer_p2 = NULL;
    uint16 s_len = 0;
    uint16 r_len = 0;
    uint32 size = 0;    /* Size of memory allocation. */

    if (soc_feature(unit, soc_feature_uc_ccm)) {

        /* Detach first if the module has been previously initialized. */
        if (NULL != _fp_oam_control[unit]) {
            _fp_oam_control[unit]->init = FALSE;
            rv = bcm_fp_oam_detach(unit);
            if (BCM_FAILURE(rv)) {
                LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                                "Module deinit - %s.\n"), bcm_errmsg(rv)));
                return (rv);
            }
        }


        /* Allocate OAM control memory for this unit. */
        _BCM_OAM_ALLOC(oc, _bcm_fp_oam_control_t,
                sizeof(_bcm_fp_oam_control_t), "OAM control");
        if (NULL == oc) {
            return (BCM_E_MEMORY);
        }

        /* Create protection mutex. */
        oc->oc_lock = sal_mutex_create("oam_control.lock");
        if (NULL == oc->oc_lock) {
            _bcm_fp_oam_control_free(unit, oc);
            return (BCM_E_MEMORY);
        }

        /* Set up the unit OAM control structure. */
        _fp_oam_control[unit] = oc;

        /* Get number of endpoints and groups supported by this unit. */
        rv = _bcm_fp_oam_resource_count_init(unit, oc);
        if (BCM_FAILURE(rv)) {
            _bcm_fp_oam_control_free(unit, oc);
            return (rv);
        }
        oc->unit = unit;

        /* if uKernel is not ready silently return */
        /* Core #0 is used for CCM in TD2P */
        if (soc_uc_in_reset(unit, _CCM_UC_NUM)) {

            LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM Warn:"
                      " uKernel Not Ready, CCM not started.\n If CCM"
                      " app is not not needed, compile SDK by excluding CCM"
                      " from FEATURE_LIST.\n")));

        } else {

            /* Initialize uController side
             * Start CCM application in BTE (Broadcom Task Engine) uController.
             * _CCM_UC_NUM runs the MPLS_LM_DM Application.
             */
            rv = soc_cmic_uc_appl_init(unit, _CCM_UC_NUM, MOS_MSG_CLASS_CCM,
                    _UC_MSG_TIMEOUT_USECS, _CCM_SDK_VERSION,
                    _CCM_UC_MIN_VERSION, NULL, NULL);

            if (!(BCM_FAILURE(rv))) {

                /* Create Group list.*/
                rv = shr_idxres_list_create(&oc->group_pool, 0, 
                                        oc->group_count - 1,
                                        0, oc->group_count - 1, "group pool");
                if (BCM_FAILURE(rv)) {
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }
                
                /* Allocate group memory */
                size = sizeof(_bcm_fp_oam_group_data_t) * oc->group_count;
                _BCM_OAM_ALLOC(oc->group_data, _bcm_fp_oam_group_data_t, size,
                        "Group Info");
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM Error: "
                        "Group data Alloca failed - %s.\n"), bcm_errmsg(rv)));
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }

                /* Create Mep list. */
                rv = shr_idxres_list_create(&oc->mep_pool, 0, oc->mep_count - 1,
                                            0, oc->mep_count - 1, "mep pool");
                if (BCM_FAILURE(rv)) {
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }

                /* Allocate group memory */
                size = sizeof(_bcm_fp_oam_mep_data_t) * oc->mep_count;
                _BCM_OAM_ALLOC(oc->mep_data, _bcm_fp_oam_mep_data_t, size,
                        "MEP Info");
                if (BCM_FAILURE(rv)) {
                    LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "CCM Error: "
                        "MEP data Alloca failed - %s.\n"), bcm_errmsg(rv)));
                    _bcm_fp_oam_control_free(unit, oc);
                    return (rv);
                }            

                /* Allocate DMA Memory */
                oc->dma_buffer_len = sizeof(shr_ccm_msg_ctrl_t);
                oc->dma_buffer = soc_cm_salloc(unit, oc->dma_buffer_len,
                                               "CCM DMA buffer");
                if (!oc->dma_buffer) {
                    _bcm_fp_oam_control_free(unit, oc);
                    return (BCM_E_MEMORY);
                }

                oc->ccm_ukernel_ready = 1;
                oc->ccm_uc_num = _CCM_UC_NUM;

                /* Send Initialize CCM App on UKERNEL */
                /* Set control message data */
                sal_memset(&ccm_init_msg, 0, sizeof(ccm_init_msg));
                ccm_init_msg.max_groups = oc->group_count;
                ccm_init_msg.max_meps = oc->mep_count;
                ccm_init_msg.rx_channel = _CCM_RX_CHANNEL_ID;

                /* Pack control message data into DMA buffer */
                buffer_p1 = oc->dma_buffer;
                buffer_p2 = shr_ccm_msg_ctrl_init_pack(buffer_p1, &ccm_init_msg);
                s_len = buffer_p2 - buffer_p1;

                /* Send CCM Init message to uC */
                rv = _bcm_fp_oam_msg_send_receive(unit, MOS_MSG_CLASS_CCM,
                       MOS_MSG_SUBCLASS_CCM_INIT, s_len, 0,
                       MOS_MSG_SUBCLASS_CCM_INIT_REPLY, &r_len);

                if (BCM_FAILURE(rv) || (r_len != 0)) {
                   /* Could not start MPLS_LM_DM appl */
                   LOG_ERROR(BSL_LS_BCM_OAM, (BSL_META_U(unit, "OAM Error: "
                       "CCM Session alloc failed\n")));
                   BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                   return rv;
                }

                /* Start event message callback thread */
                if (oc->event_thread_id == NULL) {
                    oc->event_thread_id =  sal_thread_create("bcmCCM",
                        SAL_THREAD_STKSZ, _FP_OAM_CCM_EVENT_THREAD_PRIO,
                        _bcm_fp_oam_ccm_callback_thread, (void*)oc);
                    if (oc->event_thread_id == SAL_THREAD_ERROR) {
                         oc->event_thread_id = NULL;
                         BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                        return (BCM_E_MEMORY);
                    }
                }

                /* If we have Warmrebooted, recover S/w state from UKENREL */
#ifdef BCM_WARM_BOOT_SUPPORT
                if (SOC_WARM_BOOT(unit)) {
                    rv = _bcm_fp_oam_ccm_reinit(unit);
                    if (BCM_FAILURE(rv)) {
                        BCM_IF_ERROR_RETURN(bcm_fp_oam_detach(unit));
                        return (rv);
                    }
                }
#endif
            } else {
                /* Could not start CCM appl */
                LOG_WARN(BSL_LS_BCM_OAM, (BSL_META_U(unit,
                         "OAM WARN: CCM Application not available\n")));
                /* Most likely core 0 has a different Application image */
            }
        }
    } /* soc_feature_ccm */
#endif /* INCLUDE_CCM */

#if defined(BCM_WARM_BOOT_SUPPORT)
    if (!SOC_WARM_BOOT(unit)) 
#endif    
    {
        /* Set default olp header type mapping */
        rv = _bcm_td2p_oam_olp_header_type_mapping_set(unit); 
        if (BCM_FAILURE(rv)) {
            return (rv);
        }

        /* Enable OLP on HG ports */
        BCM_IF_ERROR_RETURN(_bcm_td2p_oam_hg_olp_enable(unit));

        /* Set Magic port used for remote OLP over HG communication */
        BCM_IF_ERROR_RETURN(_bcm_td2p_oam_olp_magic_port_set(unit));
    }
    return (BCM_E_NONE);
}
