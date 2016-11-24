/*
 * $Id: hash.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
 * $Copyright: Copyright 2009 Broadcom Corporation.
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
 * Hash table calculation routines
 */

#include <assert.h>

#include <sal/types.h>
#include <sal/core/thread.h>

#include <soc/util.h>
#include <soc/hash.h>
#include <soc/mem.h>
#include <soc/drv.h>
#include <soc/debug.h>

#ifdef BCM_XGS_SWITCH_SUPPORT
uint32
soc_crc32b(uint8 *data, int data_nbits)
{
    uint32 rv;
    rv = _shr_crc32b(0, data, data_nbits);
    rv = _shr_bit_rev_by_byte_word32(rv);
    return rv;
}

uint16
soc_crc16b(uint8 *data, int data_nbits)
{
    uint16 rv;
    rv = _shr_crc16b(0, data, data_nbits);
    rv = _shr_bit_rev16(rv);
    return rv;
}

/*
 * Implement the crc32 routines so that the bit ordering matches Draco
 */

uint32
soc_draco_crc32(uint8 *data, int data_size)
{
    uint32 rv;
    rv = _shr_crc32(0, data, data_size);
    rv = _shr_bit_rev_by_byte_word32(rv);
    return rv;
}

uint16
soc_draco_crc16(uint8 *data, int data_size)
{
    uint16 rv;
    rv = _shr_crc16(0, data, data_size);
    rv = _shr_bit_rev16(rv);
    return rv;
}

/* Compute the "CRC32" of the 76-bit keys used by 5695. */
/* The 5695 CRC32 hashing algorithm is different from   */
/* that implemented in the 5690.                        */

uint32 
soc_draco15_key76_crc32(uint8 *key, int unused_data_size)
{
    uint32 crc, nibble;
    int nibnum;
    static uint32 crc32_d15_table[16] = {
        0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac, 
        0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c, 
        0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c, 
        0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
    }; 

    /* This routine processes 76 bits. Ignore datasize parameter. */
    COMPILER_REFERENCE(unused_data_size);

    crc = 0;
    for (nibnum = 0; nibnum < 19; nibnum++) {
        nibble = ((uint32)key[nibnum / 2] << ((nibnum & 1) ? 24 : 28)) 
            & 0xf0000000;
        crc = (crc >> 4) ^ nibble ^ crc32_d15_table[crc & 0x0000000f];
    }
    return _shr_bit_rev32(crc);
}

/* Compute the "CRC16" of the 76-bit keys used by 5695. */
/* The 5695 CRC16 hashing algorithm is different from   */
/* that implemented in the 5690.                        */

uint16 
soc_draco15_key76_crc16(uint8 *key, int unused_data_size)
{
    uint16 crc, nibble;
    int nibnum;
    static uint16 crc16_d15_table[16] = {
        0x0000, 0xcc01, 0xd801, 0x1400, 0xf001, 0x3c00, 0x2800, 0xe401, 
        0xa001, 0x6c00, 0x7800, 0xb401, 0x5000, 0x9c01, 0x8801, 0x4400 
    };

    /* This routine processes 76 bits. Ignore datasize parameter. */
    COMPILER_REFERENCE(unused_data_size);

    crc = 0;
    for (nibnum = 0; nibnum < 19; nibnum++) {
        nibble = ((uint16)key[nibnum / 2] << ((nibnum & 1) ? 8 : 12))
            & 0xf000;
        crc = (crc >> 4) ^ nibble ^ crc16_d15_table[crc & 0x000f];
    }
    return _shr_bit_rev16(crc);
}

/*
 * And now some routines to deal with details
 */

void
soc_draco_l2x_base_entry_to_key(int unit, l2x_entry_t *entry, uint8 *key)
{
    sal_mac_addr_t mac;
    int vid;

    soc_L2Xm_mac_addr_get(unit, entry, MAC_ADDRf, mac);

    vid = soc_L2Xm_field32_get(unit, entry, VLAN_IDf);

    soc_draco_l2x_param_to_key(mac, vid, key);
}

void
soc_draco_l2x_param_to_key(sal_mac_addr_t mac, int vid, uint8 *key)
{
    int ix;

    key[0] = 0;
    for (ix = 0; ix < 6; ix++) {
        key[ix + 1] = (mac[5 - ix] >> 4) & 0x0f;
        key[ix + 0] |= (mac[5 - ix] << 4) & 0xf0;
    }

    key[6] |= (vid << 4) & 0xf0;
    key[7] = (vid >> 4) & 0xff;
}

uint32
soc_draco_l2_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x == 0) {
        uint32  mask;
        int     bits;

        mask = soc_mem_index_max(unit, L2X_VALIDm);
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l2x = mask;
        SOC_CONTROL(unit)->hash_bits_l2x = bits;
    }

    switch (hash_sel) {
    case XGS_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case XGS_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case XGS_HASH_LSB:
        rv = ((uint32)key[0] >> 4) | ((uint32)key[1] << 4);
        break;

    case XGS_HASH_ZERO:
        rv = 0;
        break;

    case XGS_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case XGS_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_draco_l2_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x;
}

void
soc_draco_l3x_base_entry_to_key(int unit, l3x_entry_t *entry,
                                int key_src_ip0, uint8 *key)
{
    ip_addr_t ip, src_ip;
    uint16 vid;

    ip = soc_L3Xm_field32_get(unit, entry, IP_ADDRf);
    src_ip = key_src_ip0 ? 0 : soc_L3Xm_field32_get(unit, entry, SRC_IP_ADDRf);
    vid = 0;

    /*
     * (S,G,V) key is either (DIP + SIP + VID) or (DIP + SIP + 0),
     * depending on IPMC_DO_VLANf field. Regardless, always 76-bit key.
     */
    
    if (soc_feature(unit, soc_feature_l3_sgv)) { /* (S,G,V) */
        /* We need to be be l3 sync'd here.  Likely */
        if (SOC_CONTROL(unit)->hash_key_config & L3X_IPMC_VLAN) {
                vid = soc_L3Xm_field32_get(unit, entry, VLAN_IDf);
        }        
    }

    soc_draco_l3x_param_to_key(ip, src_ip, vid, key);
}

void
soc_draco_l3x_param_to_key(ip_addr_t ip, ip_addr_t src_ip,
                           uint16 vid, uint8 *key)
{
    int ix;

    for (ix = 0; ix < 4; ix++) {
        key[ix] = (ip >> (8*ix)) & 0xff;
        key[ix + 4] = (src_ip >> (8*ix)) & 0xff;
    }

    key[8] = vid & 0xff;
    key[9] = (vid >> 8) & 0x0f;
}

uint32
soc_draco_l3_hash(int unit, int hash_sel, int ipmc, uint8 *key)
{
    uint32 rv;
    int bytes;
    uint8 *eff_key = key;
    uint8 local_key[XGS_HASH_KEY_SIZE];

    uint16 (*selected_crc16_routine)(uint8 *, int) = &soc_draco_crc16;
    uint32 (*selected_crc32_routine)(uint8 *, int) = &soc_draco_crc32;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l3x == 0) {
        uint32  mask;
        int     bits;

        mask = soc_mem_index_max(unit, L3X_VALIDm);
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l3x = mask;
        SOC_CONTROL(unit)->hash_bits_l3x = bits;
    }

    if (ipmc) {
        if (soc_feature(unit, soc_feature_l3_sgv_aisb_hash)) {
            /* Draco1.5 IPMC (DIP+SIP+VID) */
            /* For IPMC, the 5695 hashes a 76 bit key. In addition, */
            /* the hashes are not the same as Draco CRC16 or CRC32  */
            selected_crc16_routine = &soc_draco15_key76_crc16;
            selected_crc32_routine = &soc_draco15_key76_crc32;
            bytes = 10; /* 76 bits, actually. */
        } else if (soc_feature(unit, soc_feature_l3_sgv)) {
            int ix;

            /* Now we have to shift up the key to properly align for CRC's */
            for (ix = 9; ix > 0; ix--) {
                local_key[ix] = (key[ix]  << 4) & 0xf0;
                local_key[ix] |= (key[ix - 1] >> 4) & 0x0f;
            }

            local_key[0] = (key[0] << 4) & 0xf0;
            bytes = 10; /* 76 bits, actually. */
            eff_key = local_key;
        }
        else {
            bytes = 8; /* IPMC (DIP+SIP) */
        }
    } else {
        bytes = 4; /* Unicast IP */
    }

    switch (hash_sel) {
    case XGS_HASH_CRC16_UPPER:
        rv = selected_crc16_routine(eff_key, bytes);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l3x;
        return rv & SOC_CONTROL(unit)->hash_mask_l3x;

    case XGS_HASH_CRC16_LOWER:
        rv = selected_crc16_routine(eff_key, bytes);
        return rv & SOC_CONTROL(unit)->hash_mask_l3x;

    case XGS_HASH_LSB:
        rv = ((uint32)eff_key[2] << 16) | 
            ((uint32)eff_key[1] << 8) | eff_key[0];
        return rv & SOC_CONTROL(unit)->hash_mask_l3x;

    case XGS_HASH_ZERO:
        return 0;

    case XGS_HASH_CRC32_UPPER:
        rv = selected_crc32_routine(eff_key, bytes);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l3x;
        return rv & SOC_CONTROL(unit)->hash_mask_l3x;

    case XGS_HASH_CRC32_LOWER:
        rv = selected_crc32_routine(eff_key, bytes);
        return rv & SOC_CONTROL(unit)->hash_mask_l3x;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_draco_l3_hash: invalid hash_sel %d\n",
                     hash_sel);
        return 0;
    }
}

/*
 * ecmp_count is zero based, i.e. it is 0 when there is one ECMP path
 */
uint32
soc_draco_l3_ecmp_hash(int unit, ip_addr_t dip, ip_addr_t sip, int ecmp_count,
                         int ecmp_hash_sel, int l3_hash_sel)
{
    uint8 key[XGS_HASH_KEY_SIZE];
    uint8 local_key[XGS_HASH_KEY_SIZE];
    uint8 *eff_key = key;
    int i, bytes, rv;

    uint16 (*selected_crc16_routine)(uint8 *, int) = &soc_draco_crc16;
    uint32 (*selected_crc32_routine)(uint8 *, int) = &soc_draco_crc32;

    /*
     * HASH_CONTROL.ECMP_HASH_SELECT = 0, use DIP + SIP (default case)
     * HASH_CONTROL.ECMP_HASH_SELECT = 1, then use SIP only
     */
    if (ecmp_hash_sel == 0) {
        bytes = 10;   /* 76 bits actually */

        /*
         * key[9] [8] ([7] [6] [5] [4]) ([3] [2] [1] [0])
         *     0   0      SIP in HEX       DIP in HEX
         */
        for (i = 0; i < 4; i++) {
            key[i]   = (dip >> (8*i)) & 0xff;  /* key[0-31]=DIP */
            key[i+4] = (sip >> (8*i)) & 0xff;  /* key[32-63]=SIP */
        }
        key[8] = key[9] = 0;                   /* key[75:64] = 0 */

        if (soc_feature(unit, soc_feature_l3_sgv_aisb_hash)) { /* Draco15 A0/A1 */
            selected_crc16_routine = &soc_draco15_key76_crc16;
            selected_crc32_routine = &soc_draco15_key76_crc32;
        } else if (soc_feature(unit, soc_feature_l3_sgv)) {  /* Draco15 B0 */
            /* Shift the key to properly align for CRC's */
            for (i = 9; i > 0; i--) {
                local_key[i]  = (key[i]  << 4) & 0xf0;
                local_key[i] |= (key[i - 1] >> 4) & 0x0f;
            }
            local_key[0] = (key[0] << 4) & 0xf0;
            eff_key = local_key;
        }
    } else {
        bytes = 4;    /* 32 bits */
        for (i = 0; i < 4; i++) {
            key[i] = (sip >> (8*i)) & 0xff;   /* key[0-31]=SIP */
        }
        for (i = 5; i < XGS_HASH_KEY_SIZE; i++) {
            key[i] = 0;
        }
    }

    switch (l3_hash_sel) {
    case XGS_HASH_CRC16_UPPER:
        rv = selected_crc16_routine(eff_key, bytes);
        rv >>= 6;   /* use the upper 10 bits */
        break;

    case XGS_HASH_CRC16_LOWER:
        rv = selected_crc16_routine(eff_key, bytes);
        break;

    case XGS_HASH_LSB:
        rv = ((uint32)key[2] << 16) | ((uint32)key[1] << 8) | key[0];
        break;

    case XGS_HASH_ZERO:
        rv = 0;
        break;

    case XGS_HASH_CRC32_UPPER:
        rv = selected_crc32_routine(eff_key, bytes);
        rv >>= 22;   /* use the upper 10 bits */
        break;

    case XGS_HASH_CRC32_LOWER:
        rv = selected_crc32_routine(eff_key, bytes);
        break;

    default:
        soc_cm_debug(DK_ERR,
             "soc_draco_l3_ecmp_hash: invalid l3_hash_sel %d\n", l3_hash_sel);
        return 0;
    }

    /* The ECMP offset is the LS 5 bits of CRC result mod (ecmp_count + 1) */
    return ((rv & 0x1F) % (ecmp_count + 1));
}

/*
 * ecmp_count is zero based, i.e. it is 0 when there is one ECMP path
 */
uint32
soc_lynx_l3_ecmp_hash(int unit, ip_addr_t dip, ip_addr_t sip, int ecmp_count)
{
    uint8 key[XGS_HASH_KEY_SIZE];
    uint8 local_key[XGS_HASH_KEY_SIZE];
    uint8 *eff_key = key;
    int i, bytes, rv;

    /* Initialize ecmp key. */
    soc_draco_l3x_param_to_key(dip, sip, 0, key);

    /* 5674 device use 76 bit key shifted by 4 bits. */
    if (soc_feature(unit, soc_feature_l3_sgv)) {  /* BCM 5674 */
        /* Shift the key 4 bits align for CRC's */
        for (i = 9; i > 0; i--) {
            local_key[i]  = (key[i] << 4) & 0xf0;
            local_key[i] |= (key[i - 1] >> 4) & 0x0f;
        }
        local_key[0] = (key[0] << 4) & 0xf0;
        eff_key = local_key;
        bytes = 10;   /* 76 bits actually */
    } else /* BCM5673 crc32(sip,dip) */{
        bytes = 8;   
    }
    rv = soc_draco_crc32(eff_key, bytes);

    /* The ECMP offset is the LS 5 bits of CRC result mod (ecmp_count + 1) */
    return (((rv & 0xFF) % (ecmp_count + 1)) << 3);
}


uint32
soc_tucana_l3_ecmp_hash(int unit, ip_addr_t dip, ip_addr_t sip, int
                        ecmp_count)
{
    uint8 key[XGS_HASH_KEY_SIZE];
    uint32 rv;

    /* Initialize ecmp key. */
    /*
     * For Tucana, ecmp_key[63:0] = {SIP, DIP}
     * key([7] [6] [5] [4]) ([3] [2] [1] [0])
     *        SIP in HEX       DIP in HEX
     */
    soc_draco_l3x_param_to_key(dip, sip, 0, key);

    /* Tucana always uses CRC16 for ECMP hash calculation */
    rv = soc_draco_crc16(key, 8);

    /*
     * From Tucana Theory of Operations :
     *  - ecmp_count is zero based, i.e. it is 0 when
     there is one ECMP path,
     *  - ECMP_OFFSET = (((CRC result) & 0xFF) % (ECMP_COUNT+1)) << 3
     *  - FINAL_L3_TABLE_INDEX = LPM->L3_TABLE_INDEX
     ECMP_OFFSET
     */
    return (((rv & 0xFF) % (ecmp_count + 1)) << 3);
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
/*
 * Get the XGS3 ECMP hash result based on HW
 */
uint32 
soc_xgs3_l3_ecmp_hash(int unit, soc_xgs3_ecmp_hash_t *data)
{
    uint8    key[SOC_MAX_MEM_WORDS];
    uint8    use_l4_port = 0;
    uint8    use_dip;
    uint8    hash_sel;
    uint32   crc_val;
    uint32   regval;
    int      index;
    int      idx; 
    uint8    udf;

    if (NULL == data) {
        return (SOC_E_PARAM);
    }

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &regval));
    hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                 regval, ECMP_HASH_SELf);

    /* If hash is disabled return 0. */
    if (FB_HASH_ZERO == hash_sel) {
        return 0;
    }

    /* If hash is based on LSB of sip. */
    if (FB_HASH_LSB == hash_sel) {
        if(data->v6) {
            crc_val = data->sip6[0] & 0x1f;
        }  else {
            crc_val = data->sip & 0x1f;
        }
        return ((crc_val & 0x1F) % (data->ecmp_count + 1));
    }

    use_dip = soc_reg_field_get(unit, HASH_CONTROLr,
                                 regval, ECMP_HASH_USE_DIPf);

    udf = soc_reg_field_get(unit, HASH_CONTROLr,
                            regval, ECMP_HASH_UDFf);

    if (SOC_REG_FIELD_VALID(unit, HASH_CONTROLr, USE_TCP_UDP_PORTSf)) {
        use_l4_port = soc_reg_field_get(unit, HASH_CONTROLr,
                                        regval, USE_TCP_UDP_PORTSf);
    } 
#if defined(BCM_EASYRIDER_SUPPORT)    
    else if (SOC_REG_FIELD_VALID(unit, HASH_CONTROLr,
                                   ECMP_HASH_NO_TCP_UDP_PORTSf)){
        use_l4_port = !(soc_reg_field_get(unit, HASH_CONTROLr,
                                        regval, ECMP_HASH_NO_TCP_UDP_PORTSf));
    }
#endif /* BCM_EASYRIDER_SUPPORT */

    /* Initialize key structure. */
    sal_memset(key, 0, SOC_MAX_MEM_WORDS * sizeof (uint8));

    /*KEY FORMAT IS UDF[12] DST_PORT[10-11] SRC_PORT[8-9] DIP[4-7] SIP[0-3]*/
    if (data->v6) {
        /* IP[32] = IP[0-31] ^ IP[32-63] ^ IP[64-95] ^ IP[96-127] */
        for (idx = 0; idx < 4; idx++) {
            for (index = 3 - idx; index < 16; index += 4) {
                key[idx] ^= data->sip6[index]  & 0xff;
                if (use_dip) {
                    key[idx + 4] ^= data->dip6[index] & 0xff;
                }
            }
        }
    } else {
        for (idx = 0; idx < 4; idx++) {
            key[idx] = (data->sip >> (8*idx)) & 0xff;
            if (use_dip) {
                key[idx + 4] = (data->dip >> (8*idx)) & 0xff;
            }
        }
    }

    if (use_l4_port) {
        for (idx = 0; idx < 2; idx++) {
            key[idx + 8] = (data->l4_src_port >> (8*idx)) & 0xff;
            key[idx + 10] = (data->l4_dst_port >> (8*idx)) & 0xff;
        }
    }

    key[12] = udf & 0xff;
   
    /* XGS3 always uses CRC32 for ECMP hash calculation */
    crc_val = soc_draco_crc32(key, 13);

    if (FB_HASH_CRC32_UPPER == hash_sel) {
        return ((crc_val >> 27) % (data->ecmp_count + 1));
    } else if (FB_HASH_CRC32_LOWER == hash_sel) {
        return ((crc_val & 0x1F) % (data->ecmp_count + 1));
    }
    /* Hopefully never reached. */
    return (SOC_E_INTERNAL);
}
#endif /* BCM_XGS3_SWITCH_SUPPORT */

uint32
soc_draco_trunk_hash(sal_mac_addr_t da, sal_mac_addr_t sa, int tgsize)
{
    uint8 key[12];
    int ix;
    uint32 crc32;

    if (tgsize <= 0 || tgsize > 8) {
        return 0;
    }

    for (ix = 0; ix < 6; ix++) {
        key[ix + 0] = da[5 - ix];
    }

    for (ix = 0; ix < 6; ix++) {
        key[ix + 6] = sa[5 - ix];
    }

    crc32 = soc_draco_crc32(key, 12);

    return (crc32 & 0xffff) % tgsize;
}

int
soc_draco_hash_set(int unit, int hash_sel)
{
    uint32              hash_control, ohash;

    assert(hash_sel >= 0 && hash_sel <= 5);

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));
    ohash = hash_control;
    soc_reg_field_set(unit, HASH_CONTROLr, &hash_control,
                      HASH_SELECTf, hash_sel);
    if (hash_control != ohash) {
        SOC_IF_ERROR_RETURN(WRITE_HASH_CONTROLr(unit, hash_control));
    }

    return SOC_E_NONE;
}

int
soc_draco_hash_get(int unit, int *hash_sel)
{
    uint32              hash_control;

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &hash_control));

    *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr, hash_control,
                                  HASH_SELECTf);

    if (*hash_sel > 5) {
        *hash_sel = 5;
    }

    return SOC_E_NONE;
}

/* This next functions is a mess.  But that's what the HW is doing. */
uint32
soc_lynx_dmux_entry(uint32 *data)
{
    uint32 rv, local_data;
    uint8 key[16];
    int ix;

    /* Is this really endian safe? */
    for (ix = 0; ix < 3; ix++) {
        local_data = _shr_swap32(data[ix]);
        memcpy(&(key[4*ix]), &local_data, 4);
    }
    local_data = 0;
    memcpy(&(key[12]), &local_data, 4);

    rv = _shr_crc32(0, key, 16);
    rv = _shr_bit_rev_by_byte_word32(rv);

    return rv & SOC_LYNX_DMUX_MASK;
}

#ifdef BCM_XGS3_SWITCH_SUPPORT
#define GEN_KEY2(k, ks, k1)                              \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff

#define GEN_KEY3(k, ks, k1)                              \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff;                        \
    k[ks + 2] = (k1 >> 12) & 0xff;                       \
    k[ks + 3] = ((k1 >> 20) & 0x0f)

#define GEN_KEY4(k, ks, k1)                              \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff;                        \
    k[ks + 2] = (k1 >> 12) & 0xff;                       \
    k[ks + 3] = (k1 >> 20) & 0xff;                       \
    k[ks + 4] = ((k1 >> 28) & 0x0f)
#endif /* BCM_XGS3_SWITCH_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

uint32
soc_fb_l2_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x == 0) {
        uint32  mask, mask_bits;
        int     bits;

        /* Get the effective table max for the hash mask */
        mask = soc_mem_index_max(unit, L2_HITDA_ONLYm);
        /* Need the maximum table size for the shift bits */
        mask_bits = SOC_MEM_INFO(unit, L2_HITDA_ONLYm).index_max;
        bits = 0;
        rv = 1;
        while (rv && (mask_bits & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l2x = mask;
        SOC_CONTROL(unit)->hash_bits_l2x = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        if (SOC_IS_HAWKEYE(unit)) {
            rv >>= 16 - (SOC_CONTROL(unit)->hash_bits_l2x + 1);
        } else {
            rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l2x;
        }
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        if (SOC_IS_HAWKEYE(unit)) {
            rv >>= 32 - (SOC_CONTROL(unit)->hash_bits_l2x + 1);
        } else {
            rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x;
        }
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_fb_l2_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x;
}

uint32
soc_fb_l3_hash(int unit, int hash_sel, int key_nbits, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l3x == 0) {
        uint32  mask, mask_bits;
        int     bits;

        /* Get the effective table max for the hash mask */
        mask = soc_mem_index_max(unit, L3_ENTRY_HIT_ONLYm);
        /* Need the maximum table size for the shift bits */
        mask_bits = SOC_MEM_INFO(unit, L3_ENTRY_HIT_ONLYm).index_max;
        bits = 0;
        rv = 1;
        while (rv && (mask_bits & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l3x = mask;
        SOC_CONTROL(unit)->hash_bits_l3x = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l3x;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        rv = ((uint32)key[0]) | ((uint32)key[1] << 8) | ((uint32)key[2] << 16);
        /* See _soc_fb_l3x_realign_key() to understand the shift values below */
        if ((key_nbits == 80)  || (key_nbits == 264)) {
                rv >>= 4;
        } else if ((key_nbits == 40)  || (key_nbits == 136)) {
                rv >>= 2;
        } else if ((key_nbits == 88)  || (key_nbits == 272)) {
                rv >>= 6;
        } else if ((key_nbits == 96)  || (key_nbits == 280)) {
#if defined (BCM_TRIUMPH_SUPPORT)
            if(SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_MULTICASTm, L3_IIFf)) {
                rv >>= (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                        SOC_IS_VALKYRIE2(unit) || SOC_IS_ENDURO(unit)) ? 5 : 6;
            } else 
#endif /* BCM_TRX_SUPPORT */
            {
                rv >>= 7;
            }
        } else if ((key_nbits == 48)  || (key_nbits == 144)) {
#if defined (BCM_ENDURO_SUPPORT)
           if(SOC_IS_ENDURO(unit)) {
               rv >>= 2;
           } else
#endif /* BCM_ENDURO_SUPPORT */
           {
                rv >>= 3;
            }
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l3x;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_fb_l3_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l3x;
}

uint32
soc_fb_vlan_mac_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits;

        /* 4 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 2;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_vlan_mac = mask;
        SOC_CONTROL(unit)->hash_bits_vlan_mac = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 6);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 6);
        break;

    case FB_HASH_LSB:
        /* Extract more than required 8 bits. Masked below anyway */
        rv = (((uint32)key[0]) | ((uint32)key[1] << 8));
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 6);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 6);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_fb_vlan_mac_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

static int
_soc_fb_l3x_realign_key(uint8 *key, int bits)
{
    int byte, offset, shift;

    offset = bits % 8;
    if (offset == 0) {
        return bits;        
    }

    shift = 8 - offset;
    byte = bits / 8;
    while (byte) {
        key[byte] = ((key[byte] << shift) | (key[byte - 1] >> offset)) & 0xff;
        byte--;
    }

    key[0] = (key[0] << shift) & 0xff;

    return bits + shift;
}

static int
_soc_fb_l3x_ip4ucast_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    ip_addr_t   ip;
    int i, bits;
#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    uint32 vrf;
    int vrf_bits = 0;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    uint32 key_type;
#endif /* BCM_TRX_SUPPORT */

    ip = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, e, IP_ADDRf);
    for (i = 0; i < 4; i++) {
        key[i] = (ip >> (8 * i)) & 0xff;
    }
    bits = (4 * 8);

#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_UNICASTm, VRF_IDf)) {
        vrf_bits = SOC_HASH_VRF_BITS(unit);
        vrf = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, e, VRF_IDf) &
            SOC_HASH_VRF_MASK(unit);

        key[4] = vrf & 0xff;
        if (vrf_bits > 8) {
            key[5] = (vrf >> 8) & 0xff;
        }
        bits += vrf_bits;

#if defined(BCM_TRX_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_UNICASTm, KEY_TYPEf)) {
            key_type = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, e, KEY_TYPEf);
            key[5] |= (key_type << (vrf_bits - 8));
            bits += 2;
        }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            bits +=1;   /*in Enduro, KEY_TYPE is 3 bits */
        } else
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            for (i = 6; i < 35; i++) {
                key[i] = 0;
            }
            bits = 275;
        }
#endif
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
    bits = _soc_fb_l3x_realign_key(key, bits);
    return bits;
}

static int
_soc_fb_l3x_ip4mcast_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    ip_addr_t   sip;
    ip_addr_t   gip;
    vlan_id_t   vid;
    int i, bits;
#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    uint32 vrf;
    int vrf_bits;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT) 
    uint32 key_type;
#endif /* BCM_TRX_SUPPORT */

    sip = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, SOURCE_IP_ADDRf);
    gip = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, GROUP_IP_ADDRf);

    key[0] = 0;
    for (i = 0; i < 4; i++) {
        key[i] = (gip >> (8 * i)) & 0xff;
        key[i + 4] = (sip >> (8 * i)) & 0xff;
    }
    bits = (8 * 8);

#if defined(BCM_TRIUMPH_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_MULTICASTm, L3_IIFf)) {
        vid = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, L3_IIFf);
        key[8] = vid & 0xff;
        key[9] = (vid >> 8) & 0x1f;
        bits += 13;

        vrf_bits = SOC_HASH_VRF_BITS(unit);;
        vrf = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, VRF_IDf) &
            SOC_HASH_VRF_MASK(unit);
        key[9] |= (vrf << 5) & 0xe0;
        key[10] = (vrf >> 3) & 0xff;
        bits += vrf_bits;
        key_type = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, KEY_TYPE_0f);
        key[11] = key_type;
        bits += 2;
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            bits +=1;   /*in Enduro, KEY_TYPE is 3 bits */
        } else
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            for (i = 12; i < 35; i++) {
                key[i] = 0;
            }
            bits = 275;
        }
#endif
    } else 
#endif /* BCM_TRIUMPH_SUPPORT */ 
    {
        vid = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, VLAN_IDf);
        key[8] = vid & 0xff;
        key[9] = (vid >> 8) & 0x0f;
        bits += 12;
#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_SCORPION_SUPPORT)    
        if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_MULTICASTm, VRF_IDf)) {
            vrf_bits = SOC_HASH_VRF_BITS(unit);
            vrf = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, VRF_IDf) &
                SOC_HASH_VRF_MASK(unit);

            key[9] |= (vrf << 4) & 0xf0;
            if (vrf_bits > 4) {
                key[10] = (vrf >> 4) & 0x7f;
            }
            bits += vrf_bits;
        }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */
#if defined(BCM_SCORPION_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_MULTICASTm, KEY_TYPEf)) {
            key_type = soc_L3_ENTRY_IPV4_MULTICASTm_field32_get(unit, e, KEY_TYPEf);
            key[10] |= (key_type << 0x7) & 0x80;
            key[11] = (key_type >> 0x1) & 0x1;
            bits += 2;
        }
#endif /* BCM_SCORPION_SUPPORT */
    }
    return _soc_fb_l3x_realign_key(key, bits);
}

static int
_soc_fb_l3x_ip6ucast_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32      ipu64[SOC_MAX_MEM_FIELD_WORDS];
    uint32      ipl64[SOC_MAX_MEM_FIELD_WORDS];
    int         i, bits;
#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    uint32      vrf;
    int         vrf_bits = 0;
    soc_field_t vrf_field = INVALIDf;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
#if defined(BCM_TRX_SUPPORT)
    int         key_type; 
#endif /* BCM_TRX_SUPPORT  */

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_UNICASTm, VRF_IDf)) {
        vrf_field = VRF_IDf;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)  || \
    defined(BCM_SCORPION_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_UNICASTm, VRF_ID_0f)) {
        vrf_field = VRF_ID_0f;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */

    soc_mem_field_get(unit, L3_ENTRY_IPV6_UNICASTm, e, IP_ADDR_UPR_64f, ipu64);
    soc_mem_field_get(unit, L3_ENTRY_IPV6_UNICASTm, e, IP_ADDR_LWR_64f, ipl64);

    for (i = 0; i < 4; i++) {
        key[i +  0] = (ipl64[0] >> (8 * i)) & 0xff;
        key[i +  4] = (ipl64[1] >> (8 * i)) & 0xff;
        key[i +  8] = (ipu64[0] >> (8 * i)) & 0xff;
        key[i + 12] = (ipu64[1] >> (8 * i)) & 0xff;
    }
    bits = (16 * 8);

#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    if (INVALIDf != vrf_field) {
        vrf_bits = SOC_HASH_VRF_BITS(unit);
        vrf = soc_L3_ENTRY_IPV6_UNICASTm_field32_get(unit, e, vrf_field) &
            SOC_HASH_VRF_MASK(unit);

        key[16] = vrf & 0xff;
        if (vrf_bits > 8) {
            key[17] = (vrf >> 8) & 0xff;
        }
        bits += vrf_bits;
#if defined(BCM_TRX_SUPPORT)
        if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_UNICASTm, KEY_TYPE_0f)) {
            key_type = soc_L3_ENTRY_IPV6_UNICASTm_field32_get(unit, e, KEY_TYPE_0f);
            key[17] |= (key_type << (vrf_bits - 8));
            bits += 2;
        }
#endif /* BCM_TRX_SUPPORT */
#ifdef BCM_TRIUMPH2_SUPPORT
        if (SOC_IS_ENDURO(unit)) {
            bits +=1;   /*in Enduro, KEY_TYPE is 3 bits */
        } else
        if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
            SOC_IS_VALKYRIE2(unit)) {
            for (i = 18; i < 35; i++) {
                key[i] = 0;
            }
            bits = 275;
        }
#endif
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
    bits = _soc_fb_l3x_realign_key(key, bits);

    return bits;
}

static int
_soc_fb_l3x_ip6mcast_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    vlan_id_t   vid;
    uint32      sipu64[SOC_MAX_MEM_FIELD_WORDS];
    uint32      sipl64[SOC_MAX_MEM_FIELD_WORDS];
    uint32      gipu56[SOC_MAX_MEM_FIELD_WORDS];
    uint32      gipl64[SOC_MAX_MEM_FIELD_WORDS];
    int i, bits;
    int         vlan_bits;
#if defined(BCM_TRX_SUPPORT)
    uint32      key_type;
#endif /* BCM_TRIUMPH_SUPPORT */
#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    uint32      vrf;
    int         vrf_bits;
    soc_field_t vrf_field = INVALIDf;
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_MULTICASTm, VRF_IDf)) {
        vrf_field = VRF_IDf;
    }
#endif /* BCM_TRIUMPH_SUPPORT */

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT)  || \
    defined(BCM_SCORPION_SUPPORT)
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_MULTICASTm, VRF_ID_0f)) {
        vrf_field = VRF_ID_0f;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_SCORPION_SUPPORT */


    soc_mem_field_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                      e, SOURCE_IP_ADDR_UPR_64f, sipu64);
    soc_mem_field_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                      e, SOURCE_IP_ADDR_LWR_64f, sipl64);
    soc_mem_field_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                      e, GROUP_IP_ADDR_UPR_56f, gipu56);
    soc_mem_field_get(unit, L3_ENTRY_IPV6_MULTICASTm,
                      e, GROUP_IP_ADDR_LWR_64f, gipl64);

    for (i = 0; i < 4; i++) {
        key[i +  0] = (gipl64[0] >> (8 * i)) & 0xff;
        key[i +  4] = (gipl64[1] >> (8 * i)) & 0xff;
        key[i +  8] = (gipu56[0] >> (8 * i)) & 0xff;
        if (i != 3) {
            key[i + 12] = (gipu56[1] >> (8 * i)) & 0xff;
        }
        key[i + 15] = (sipl64[0] >> (8 * i)) & 0xff;
        key[i + 19] = (sipl64[1] >> (8 * i)) & 0xff;
        key[i + 23] = (sipu64[0] >> (8 * i)) & 0xff;
        key[i + 27] = (sipu64[1] >> (8 * i)) & 0xff;
    }

    bits = (31 * 8);

#ifdef BCM_TRIUMPH_SUPPORT
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_MULTICASTm, L3_IIFf)) {
        vid = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, e, L3_IIFf);
        vlan_bits = 0xd;
    } else
#endif /* BCM_TRIUMPH_SUPPORT */
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_MULTICASTm, VLAN_IDf)) {
        vid = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, e, VLAN_IDf);
        vlan_bits = 0xc;
    } else if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_MULTICASTm, VLAN_ID_0f)) {
        vid = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, e, VLAN_ID_0f);
        vlan_bits = 0xc;
    }  else {
        return (SOC_E_INTERNAL);
    }
    key[31] = vid & 0xff;
    key[32] = (vid >> 8) & ((0xc == vlan_bits) ? 0x0f : 0x1f);
    bits += vlan_bits;


#if defined (BCM_FIREBOLT2_SUPPORT) || defined(BCM_RAVEN_SUPPORT) || \
    defined (BCM_TRX_SUPPORT)    
    if (INVALIDf != vrf_field) {
        vrf_bits = SOC_HASH_VRF_BITS(unit);
        vrf = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, e, vrf_field) &
            SOC_HASH_VRF_MASK(unit);

        if (0xc == vlan_bits) {
            key[32] |= (vrf << 4) & 0xf0;
            if (vrf_bits > 4) {
                key[33] = (vrf >> 4) & 0x7f;
            }
        } else {
            key[32] |= (vrf << 5) & 0xe0;
            key[33] = (vrf >> 3) & 0xff;
        }
        bits += vrf_bits;
    }
#endif /* BCM_FIREBOLT2_SUPPORT || BCM_RAVEN_SUPPORT || BCM_TRX_SUPPORT */
#if defined (BCM_TRX_SUPPORT) 
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV6_MULTICASTm, KEY_TYPE_0f)) {
        key_type = soc_L3_ENTRY_IPV6_MULTICASTm_field32_get(unit, e, KEY_TYPE_0f);
        if (0xc == vlan_bits) {
            key[33] |= (key_type << 0x7) & 0x80;
            key[34] = (key_type >> 0x1) & 0x1;
        } else {
            key[34] = key_type;
        }
        bits += 2;
    }
#ifdef BCM_TRIUMPH2_SUPPORT
    if (SOC_IS_ENDURO(unit)) {
        bits += 1;  /* In Enduro, KEY_TYPE is 3 bits */
    } else
    if (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
        SOC_IS_VALKYRIE2(unit)) {
        bits = 275;
    }
#endif
#endif /* BCM_TRX_SUPPORT */
    return _soc_fb_l3x_realign_key(key, bits);
}

int _soc_fb_l3x_lmep_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    uint32 sglp;
    uint32 vlan;

    sglp = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, entry, LMEP__SGLPf);
    vlan = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, entry, LMEP__VIDf);

    key[0] = sglp & 0xFF;
    key[1] = (sglp >> 8) | ((vlan << 6) & 0xFF);
    key[2] = (vlan >> 2) & 0xFF;
    key[3] = ((vlan >> 10) & 0xFF) | (TR_L3_HASH_KEY_TYPE_LMEP << 2);

    sal_memset(key + 4, 0, 31);

    return _soc_fb_l3x_realign_key(key, 275);
#else
    return SOC_E_UNAVAIL;
#endif
}

int _soc_fb_l3x_rmep_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    uint32 sglp;
    uint32 vlan;
    uint32 mepid;
    uint32 mdl;

    sglp = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, entry, RMEP__SGLPf);
    vlan = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, entry, RMEP__VIDf);
    mepid = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, entry, RMEP__MEPIDf);
    mdl = soc_L3_ENTRY_IPV4_UNICASTm_field32_get(unit, entry, RMEP__MDLf);

    key[0] = sglp & 0xFF;
    key[1] = (sglp >> 8) | ((vlan << 6) & 0xFF);
    key[2] = (vlan >> 2) & 0xFF;
    key[3] = ((vlan >> 10) & 0xFF) | (mdl << 2) | ((mepid << 5) & 0xFF);
    key[4] = ((mepid >> 3) & 0xFF);
    key[5] = ((mepid >> 11) & 0xFF) | (TR_L3_HASH_KEY_TYPE_RMEP << 5);

    sal_memset(key + 6, 0, 29);

    return _soc_fb_l3x_realign_key(key, 275);
#else
    return SOC_E_UNAVAIL;
#endif
}

int
soc_fb_l3x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    if (SOC_MEM_FIELD_VALID(unit, L3_ENTRY_IPV4_UNICASTm, KEY_TYPEf))
    {
        switch (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry,
            KEY_TYPEf))
        {
            case TR_L3_HASH_KEY_TYPE_V4UC:
                return _soc_fb_l3x_ip4ucast_entry_to_key(unit, entry, key);
            case TR_L3_HASH_KEY_TYPE_V4MC:
                return _soc_fb_l3x_ip4mcast_entry_to_key(unit, entry, key);
            case TR_L3_HASH_KEY_TYPE_V6UC:
                return _soc_fb_l3x_ip6ucast_entry_to_key(unit, entry, key);
            case TR_L3_HASH_KEY_TYPE_V6MC:
                return _soc_fb_l3x_ip6mcast_entry_to_key(unit, entry, key);
            case TR_L3_HASH_KEY_TYPE_LMEP:
                return _soc_fb_l3x_lmep_entry_to_key(unit, entry, key);
            case TR_L3_HASH_KEY_TYPE_RMEP:
                return _soc_fb_l3x_rmep_entry_to_key(unit, entry, key);
            default:
                return (SOC_E_PARAM);
        }
    }
    else
    {
        if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, V6f)) {
            if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, IPMCf)) {
                return _soc_fb_l3x_ip6mcast_entry_to_key(unit, entry, key);
            } else {
                return _soc_fb_l3x_ip6ucast_entry_to_key(unit, entry, key);
            }
        } else {
            if (soc_mem_field32_get(unit, L3_ENTRY_IPV4_UNICASTm, entry, IPMCf)) {
                return _soc_fb_l3x_ip4mcast_entry_to_key(unit, entry, key);
            } else {
                return _soc_fb_l3x_ip4ucast_entry_to_key(unit, entry, key);
            }
        }
    }
}

uint32
soc_fb_l3x2_entry_hash(int unit, uint32 *entry)
{
    int             hash_sel;
    uint32          index;
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          tmp_hs;

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
    hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                 tmp_hs, L3_HASH_SELECTf);
    key_nbits = soc_fb_l3x_base_entry_to_key(unit, entry, key);
    index = soc_fb_l3_hash(unit, hash_sel, key_nbits, key);

    return index;
}

#if defined(BCM_FIREBOLT2_SUPPORT) || defined(BCM_TRIUMPH_SUPPORT) \
 || defined(BCM_RAVEN_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
int
soc_fb_l2x_entry_bank_hash_sel_get(int unit, int bank, int *hash_sel)
{
    uint32          tmp_hs;

    *hash_sel = -1;
    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_L2_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            *hash_sel = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    }

    if (*hash_sel == -1) {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                      tmp_hs, L2_AND_VLAN_MAC_HASH_SELECTf);
    }

    return SOC_E_NONE;
}

uint32
soc_fb_l2x_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    soc_draco_l2x_base_entry_to_key(unit, (l2x_entry_t *)entry, key);
    index = soc_fb_l2_hash(unit, hash_sel, key);

    return index;
}

int
soc_fb_l3x_entry_bank_hash_sel_get(int unit, int bank, int *hash_sel)
{
    uint32          tmp_hs;

    *hash_sel = -1;
    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_L3_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            *hash_sel = soc_reg_field_get(unit, L3_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    }

    if (*hash_sel == -1) {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                     tmp_hs, L3_HASH_SELECTf);
    }

    return SOC_E_NONE;
}

uint32
soc_fb_l3x_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_fb_l3x_base_entry_to_key(unit, entry, key);
    index = soc_fb_l3_hash(unit, hash_sel, key_nbits, key);

    return index;
}

int
soc_fb_l3x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int        hash_sel;

    SOC_IF_ERROR_RETURN
        (soc_fb_l3x_entry_bank_hash_sel_get(unit, bank, &hash_sel));

    return soc_fb_l3x_entry_hash(unit, hash_sel, entry);
}

#endif /* BCM_FIREBOLT2_SUPPORT || BCM_TRIUMPH_SUPPORT || BCM_RAVEN_SUPPORT */

int
soc_fb_rv_vlanmac_hash_sel_get(int unit, int dual, int *hash_sel)
{
    uint32          tmp_hs;

    *hash_sel = -1;
#if defined(BCM_RAVEN_SUPPORT)
    if (dual > 0 && SOC_REG_IS_VALID(unit, VLAN_MAC_AUX_HASH_CONTROLr)) {
        SOC_IF_ERROR_RETURN(READ_VLAN_MAC_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, VLAN_MAC_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            *hash_sel = soc_reg_field_get(unit, VLAN_MAC_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    }
#endif /* BCM_RAVEN_SUPPORT */

    if (*hash_sel == -1) {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        *hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                      tmp_hs, L2_AND_VLAN_MAC_HASH_SELECTf);
    }

    return SOC_E_NONE;
}

#endif /* BCM_FIREBOLT_SUPPORT */

#ifdef BCM_EASYRIDER_SUPPORT

uint32
soc_er_l2_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x == 0) {
        uint32  mask;
        int     max;

        max = soc_mem_index_max(unit, L2_ENTRY_INTERNALm);
        if (max < 0) {
            SOC_CONTROL(unit)->hash_mask_l2x = ER_HASH_L2_NO_TABLE;
            SOC_CONTROL(unit)->hash_bits_l2x = -1;
        } else {
            /* 8 Entries per bucket */
            mask = max >> 3;
            rv = 1;
            while (rv && (mask & rv)) {
                rv <<= 1;
            }
            SOC_CONTROL(unit)->hash_mask_l2x = mask;
            SOC_CONTROL(unit)->hash_bits_l2x = ER_HASH_L2_INT_BITS;
        }
    }

    if (SOC_CONTROL(unit)->hash_bits_l2x < 0) {
        return ER_HASH_L2_NO_TABLE;
    }

    /* ER & FB use the same hash encodings */
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_er_l2_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x;
}

uint32
soc_er_l2ext_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x_ext == 0) {
        uint32  mask;
        int     bits, max;

        max = soc_mem_index_max(unit, L2_ENTRY_EXTERNALm);
        if (max < 0) {
            SOC_CONTROL(unit)->hash_mask_l2x_ext = ER_HASH_L2_NO_TABLE;
            SOC_CONTROL(unit)->hash_bits_l2x_ext = -1;
        } else {
            /* 4 Entries per bucket */
            mask = max >> 2;
            bits = 0;
            rv = 1;
            while (rv && (mask & rv)) {
                bits += 1;
                rv <<= 1;
            }
            SOC_CONTROL(unit)->hash_mask_l2x_ext = mask;
            SOC_CONTROL(unit)->hash_bits_l2x_ext = bits;
        }
    }

    if (SOC_CONTROL(unit)->hash_bits_l2x_ext < 0) {
        return ER_HASH_L2_NO_TABLE;
    }

    /* ER does not use CRC16 for the external table */
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc32(key, 8) >> ER_HASH_L2_EXT_CRC16_UPPER_SHIFT;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc32(key, 8) >> ER_HASH_L2_EXT_CRC16_LOWER_SHIFT;
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) |
            ((uint32)key[2] << 12) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x_ext;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_er_l2ext_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x_ext;
}

uint32
soc_er_l3v4_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l3x == 0) {
        uint32  mask;
        int     bits, max;

        max = soc_mem_index_max(unit, L3_ENTRY_V4m);
        if (max < 0) {
            SOC_CONTROL(unit)->hash_mask_l3x = ER_HASH_L2_NO_TABLE;
            SOC_CONTROL(unit)->hash_bits_l3x = -1;
        } else {
            /* 8 Entries per bucket */
            mask = max >> 3;
            bits = 0;
            rv = 1;
            while (rv && (mask & rv)) {
                bits += 1;
                rv <<= 1;
            }
            SOC_CONTROL(unit)->hash_mask_l3x = mask;
            SOC_CONTROL(unit)->hash_bits_l3x = 12;
        }
    }

    if (SOC_CONTROL(unit)->hash_bits_l3x < 0) {
        return ER_HASH_L2_NO_TABLE;
    }

    /* ER & FB use the same hash encodings */
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l3x;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l3x;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_er_l3v4_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l3x;
}

uint32
soc_er_l3v6_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l3v6 == 0) {
        uint32  mask;
        int     bits, max;

        max = soc_mem_index_max(unit, L3_ENTRY_V6m);
        if (max < 0) {
            SOC_CONTROL(unit)->hash_mask_l3v6 = ER_HASH_L2_NO_TABLE;
            SOC_CONTROL(unit)->hash_bits_l3v6 = -1;
        } else {
            /* 4 Entries per bucket */
            mask = max >> 2;
            bits = 0;
            rv = 1;
            while (rv && (mask & rv)) {
                bits += 1;
                rv <<= 1;
            }
            SOC_CONTROL(unit)->hash_mask_l3v6 = mask;
            SOC_CONTROL(unit)->hash_bits_l3v6 = 12;
        }
    }

    if (SOC_CONTROL(unit)->hash_bits_l3v6 < 0) {
        return ER_HASH_L2_NO_TABLE;
    }

    /* ER & FB use the same hash encodings */
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 16);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l3v6;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 16);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0]) | ((uint32)key[1] << 8));
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 16);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l3v6;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 16);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_er_l3v6_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l3v6;
}

int
soc_er_l3x_hash(int unit, uint32 *entry, int l3v6)
{
    uint8           key[ER_L3V6_HASH_KEY_SIZE];
    uint32          hash_sel;
    uint32          regval;
    int             index;

    SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &regval));

    hash_sel= soc_reg_field_get(unit, HASH_CONTROLr, regval, L3_HASH_SELECTf);

    if (l3v6) {
        soc_er_l3v6_base_entry_to_key(unit, (uint32 *)entry, key);
        index = soc_er_l3v6_hash(unit, hash_sel, key);
    } else {
        soc_er_l3v4_base_entry_to_key(unit, (uint32 *)entry, key);
        index = soc_er_l3v4_hash(unit, hash_sel, key);
    }
    return (index);
}

uint32
soc_er_vlan_mac_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits, max;

        /* 8 Entries per bucket */
        max = soc_mem_index_max(unit, VLAN_MAC_ENTRYm) >> 3;
        if (max < 0) {
            SOC_CONTROL(unit)->hash_mask_vlan_mac = ER_HASH_MVL_NO_TABLE;
            SOC_CONTROL(unit)->hash_bits_vlan_mac = -1;
        } else {
            mask = max;
            bits = 0;
            rv = 1;
            while (rv && (mask & rv)) {
                bits += 1;
                rv <<= 1;
            }
            SOC_CONTROL(unit)->hash_mask_vlan_mac = mask;
            SOC_CONTROL(unit)->hash_bits_vlan_mac = bits;
        }
    }

    /* ER & FB use the same hash encodings */
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_er_vlan_mac_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

uint32
soc_er_my_station_hash(int unit, int hash_sel, uint8 *key)
{
    uint32 rv = 0;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_my_station == 0) {
        uint32  mask;
        int     bits, max;

        /* 8 Entries per bucket */
        max = soc_mem_index_max(unit, MY_STATIONm) >> 3;
        if (max < 0) {
            SOC_CONTROL(unit)->hash_mask_my_station = ER_HASH_MVL_NO_TABLE;
            SOC_CONTROL(unit)->hash_bits_my_station = -1;
        } else {
            mask = max;
            bits = 0;
            rv = 1;
            while (rv && (mask & rv)) {
                bits += 1;
                rv <<= 1;
            }
            SOC_CONTROL(unit)->hash_mask_my_station = mask;
            SOC_CONTROL(unit)->hash_bits_my_station = bits;
        }
    }

    if (SOC_CONTROL(unit)->hash_bits_my_station == -1) {
        return ER_HASH_MVL_NO_TABLE;
    }

    /* ER & FB use the same hash encodings */
    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_draco_crc16(key, 8);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_my_station;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_draco_crc16(key, 8);
        break;

    case FB_HASH_LSB:
        rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_draco_crc32(key, 8);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_my_station;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_draco_crc32(key, 8);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_er_my_station_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_my_station;
}

#define GEN_ER_KEY2(k, ks, k1)                           \
    k[ks + 0] |= (k1 << 4) & 0xf0;                       \
    k[ks + 1] = (k1 >> 4) & 0xff;                        \
    k[ks + 2] = (k1 >> 12) & 0x0f  

void
soc_er_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 mac[SOC_MAX_MEM_FIELD_WORDS];
    uint32 vid = 0;

    soc_mem_field_get(unit, L2_ENTRY_INTERNALm, entry, MAC_ADDRf, mac);
    vid = soc_L2_ENTRY_INTERNALm_field32_get(unit, entry, VLAN_IDf);

    key[0] = 0;
    GEN_KEY4(key, 0, mac[0]);
    GEN_ER_KEY2(key, 4, mac[1]);
    GEN_KEY2(key, 6, vid);
}

void
soc_er_l3v4_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 ip;
    uint32 vrf = 0;

    ip = soc_L3_ENTRY_V4m_field32_get(unit, entry, IP_ADDRf);
    vrf = soc_L3_ENTRY_V4m_field32_get(unit, entry, VRFf);

    key[0] = 0;
    GEN_KEY4(key, 0, ip);
    GEN_KEY2(key, 4, vrf);
    GEN_ER_KEY2(key, 5, 0);
}

#define GEN_ER_KEY4(k, ks, k1)                             \
    k[ks + 0] = (k1) & 0xff;                               \
    k[ks + 1] = ((k1) >> 8) & 0xff;                        \
    k[ks + 2] = ((k1) >> 16) & 0xff;                       \
    k[ks + 3] = ((k1) >> 24) & 0xff

void
soc_er_l3v6_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 ip6[SOC_MAX_MEM_FIELD_WORDS];

    soc_mem_field_get(unit, L3_ENTRY_V6m, entry, IP_ADDRf, ip6);

    GEN_ER_KEY4(key, 0, ip6[0]);
    GEN_ER_KEY4(key, 4, ip6[1]);
    GEN_ER_KEY4(key, 8, ip6[2]);
    GEN_ER_KEY4(key, 12, ip6[3]);
}


#endif /* BCM_EASYRIDER_SUPPORT */

#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_SCORPION_SUPPORT)
uint32
soc_tr_l2x_hash(int unit, int hash_sel, int key_nbits, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_l2x == 0) {
        uint32  mask;
        int     bits;

        mask = soc_mem_index_max(unit, L2_HITDA_ONLYm);
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_l2x = mask;
        SOC_CONTROL(unit)->hash_bits_l2x = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        if (key_nbits == 64) {
            /* Key type 0, use mac address */
            rv = ((uint32)key[2]) | ((uint32)key[3] << 8);
        } else if (key_nbits == 16) {
            /* Key type 1, use ovid */
            rv = (((uint32)key[0] >> 4) | ((uint32)key[1] << 4) );
        } else {
            /* Key type 2, use ovid */
            rv = ((uint32)key[1]) | ((uint32)key[2] << 8);
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_l2x;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr_l2_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_l2x;
}

int
soc_tr_l2x_param_to_key(int unit, sal_mac_addr_t mac, int vid, uint8 *key)
{
    int ix, key_type = 0;

    /* key[0] has 2 bits of padding of byte alignment */
    key[0] = ((vid << 4) & 0xf0) | ((key_type << 2) & 0x0c);
    key[1] = (vid >> 4) & 0xff;

    for (ix = 0; ix < 6; ix++) {
        key[ix + 2] = (mac[5 - ix]) & 0xff;
    }

    return (64); /* mac (48) + ovid (12) + key_type (2) + padding (2) */
}

static int
_soc_tr_l2x_bridge_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    sal_mac_addr_t mac;
    int vid;

    /* Key type 0x0 (BRIDGE - Bridging) Key = {OVID, MAC} */
    soc_L2Xm_mac_addr_get(unit, e, MAC_ADDRf, mac);
    vid = soc_L2Xm_field32_get(unit, e, VLAN_IDf);

    return soc_tr_l2x_param_to_key(unit, mac, vid, key);
}

static int
_soc_tr_l2x_scc_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    int ovid, key_type = 1;

    /* Key type 0x1 (SINGLE_CROSS_CONNECT) Key = {OVID} */
    ovid = soc_L2Xm_field32_get(unit, e, OVIDf);

    /* key[0] has 2 bits of padding of byte alignment */
    key[0] = ((ovid << 4) & 0xf0) | ((key_type << 2) & 0x0c);
    key[1] = (ovid >> 4) & 0xff;

    return (16); /* ovid (12) + key_type (2) + padding (2) */
}

static int
_soc_tr_l2x_dcc_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    int ivid, ovid, key_type = 2;

    /* Key type 0x2 (DOUBLE_CROSS_CONNECT) Key = {OVID, IVOD} */
    ovid = soc_L2Xm_field32_get(unit, e, OVIDf);
    ivid = soc_L2Xm_field32_get(unit, e, IVIDf);

    /* key[0] has 6 bits of padding of byte alignment */
    key[0] = (key_type << 6) & 0xc0;
    key[1] = ovid & 0xff;
    key[2] = ((ovid >> 8) & 0x0f) | ((ivid << 4) & 0xf0);
    key[3] = (ivid >> 4) & 0xff;

    return (32); /* ivid (12) + ovid (12) + key_type (2) + padding (6) */
}

static int
_soc_tr_l2x_vfi_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    sal_mac_addr_t mac;
    int ix, key_type = 3;
    int vfi;

    /* Key type 0x3 (VFI - MPLS) Key = {VFI, MAC} */
    soc_L2Xm_mac_addr_get(unit, e, MAC_ADDRf, mac);
    vfi = soc_L2Xm_field32_get(unit, e, VFIf);

    /* key[0] has 2 bits of padding of byte alignment */
    key[0] = ((vfi << 4) & 0xf0) | ((key_type << 2) & 0x0c);
    key[1] = (vfi >> 4) & 0xff;

    for (ix = 0; ix < 6; ix++) {
        key[ix + 2] = (mac[5 - ix]) & 0xff;
    }

    return (64); /* mac (48) + vfi (12) + key_type (2) + padding (2) */
}

int
soc_tr_l2x_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    int key_type, rv = 0;

    key_type = soc_mem_field32_get(unit, L2Xm, entry, KEY_TYPEf);

    switch (key_type) {
        case TR_L2_HASH_KEY_TYPE_BRIDGE:
            rv = _soc_tr_l2x_bridge_entry_to_key(unit, entry, key);
            break;
        case TR_L2_HASH_KEY_TYPE_SINGLE_CROSS_CONNECT:
            rv = _soc_tr_l2x_scc_entry_to_key(unit, entry, key);
            break;
        case TR_L2_HASH_KEY_TYPE_DOUBLE_CROSS_CONNECT:
            rv = _soc_tr_l2x_dcc_entry_to_key(unit, entry, key);
            break;
        case TR_L2_HASH_KEY_TYPE_VFI:
            rv = _soc_tr_l2x_vfi_entry_to_key(unit, entry, key);
            break;
        default:
            break;
    }
    return rv;
}

uint32
soc_tr_l2x_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_l2x_base_entry_to_key(unit, entry, key);
    index = soc_tr_l2x_hash(unit, hash_sel, key_nbits, key);

    return index;
}

uint32
soc_tr_l2x_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int             hash_sel = 0;
    uint32          tmp_hs;

    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_L2_AUX_HASH_CONTROLr(unit, &tmp_hs));
        if (soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                              tmp_hs, ENABLEf)) {
            hash_sel = soc_reg_field_get(unit, L2_AUX_HASH_CONTROLr,
                                         tmp_hs, HASH_SELECTf);
        }
    } else {
        SOC_IF_ERROR_RETURN(READ_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, HASH_CONTROLr,
                                     tmp_hs, L2_AND_VLAN_MAC_HASH_SELECTf);
    }
    return soc_tr_l2x_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, 
                       uint32 *base_entry, uint8 *key)
{
    uint32 rv = 0;
    int key_type;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_vlan_mac = mask;
        SOC_CONTROL(unit)->hash_bits_vlan_mac = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        key_type = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, KEY_TYPEf);
        switch (key_type) {
            case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OTAG:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_ITAG:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, ITAGf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                {
                    sal_mac_addr_t  mac;
                    soc_VLAN_MACm_mac_addr_get(unit, base_entry,
                                               MAC_ADDRf, mac);
                    rv = (((uint32)mac[4]) << 11) | (((uint32)mac[5]) << 3) |
                        soc_mem_field32_get(unit, VLAN_MACm,
                                            base_entry, KEY_TYPEf);;
                }
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OVID:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_IVID:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, IVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, GLPf);
                break;
            default:
                rv = 0;
                break;
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr_vlan_xlate_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

static int
_soc_tr_vlan_mac_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 key_type=3;
    sal_mac_addr_t  mac;
    int             i, bits;

    /* Key type 0x3 (VLAN_MAC key) = MAC, KEY_TYPE (3) */
    soc_VLAN_MACm_mac_addr_get(unit, e, MAC_ADDRf, mac);

    key[0] = (key_type << 5) & 0xe0;
    for (i = 0; i < 6; i++) {
        key[i+1] = mac[5 - i];
    }
    bits = 56; /* 48 + 3 + 5-bits padding */

    return bits;
}

static int
_soc_tr_vlan_xlate_dtag_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=0;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x0 (IVID_OVID key) = IVID, OVID, GLP, KEY_TYPE (0) */

    ivid = soc_VLAN_XLATEm_field32_get(unit, e, IVIDf);
    ovid = soc_VLAN_XLATEm_field32_get(unit, e, OVIDf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 7;
    key[1] = ((key_type >> 1) & 0x3) | ((glp << 2) & 0xfc);
    key[2] = (glp >> 6) & 0xff;
    key[3] = ovid & 0xff;
    key[4] = ((ovid >> 8) & 0x0f) | ((ivid << 4) & 0xf0);
    key[5] = (ivid >> 4) & 0xff;

    bits = 48; /* 12 + 12 + 14 + 3 + 7-bit padding */

    return bits;
}

static int
_soc_tr_vlan_xlate_otag_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 otag, glp, key_type=1;
    int bits;

    /* Key type 0x1 (OTAG key) = OTAG, GLP, KEY_TYPE (1) */

    otag = soc_VLAN_XLATEm_field32_get(unit, e, OTAGf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 7;
    key[1] = ((key_type >> 1) & 0x3) | ((glp << 2) & 0xfc);
    key[2] = (glp >> 6) & 0xff;
    key[3] = otag & 0xff;
    key[4] = (otag >> 8) & 0xff;
    key[5] = 0;

    bits = 48; /* same as key-type 0 */

    return bits;
}

static int
_soc_tr_vlan_xlate_itag_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 itag, glp, key_type=2;
    int bits;

    /* Key type 0x2 (ITAG key) = ITAG, GLP, KEY_TYPE (2) */

    itag = soc_VLAN_XLATEm_field32_get(unit, e, ITAGf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);
    
    key[0] = key_type << 7;
    key[1] = ((key_type >> 1) & 0x3) | ((glp << 2) & 0xfc);
    key[2] = (glp >> 6) & 0xff;
    key[3] = itag & 0xff;
    key[4] = (itag >> 8) & 0xff;
    key[5] = 0;

    bits = 48; /* same as key-type 0 */

    return bits;
}

static int
_soc_tr_vlan_xlate_ovid_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=4;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x4 (OVID key) = IVID (0), OVID, GLP, KEY_TYPE (4) */

    ivid = 0;
    ovid = soc_VLAN_XLATEm_field32_get(unit, e, OVIDf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 7;
    key[1] = ((key_type >> 1) & 0x3) | ((glp << 2) & 0xfc);
    key[2] = (glp >> 6) & 0xff;
    key[3] = ovid & 0xff;
    key[4] = ((ovid >> 8) & 0x0f) | ((ivid << 4) & 0xf0);
    key[5] = (ivid >> 4) & 0xff;

    bits = 48; /* 12 + 12 + 14 + 3 + 7-bit padding */

    return bits;
}

static int
_soc_tr_vlan_xlate_ivid_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=5;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x5 (IVID key) = IVID, OVID (0), GLP, KEY_TYPE (5) */

    ivid = soc_VLAN_XLATEm_field32_get(unit, e, IVIDf);
    ovid = 0;
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 7;
    key[1] = ((key_type >> 1) & 0x3) | ((glp << 2) & 0xfc);
    key[2] = (glp >> 6) & 0xff;
    key[3] = ovid & 0xff;
    key[4] = ((ovid >> 8) & 0x0f) | ((ivid << 4) & 0xf0);
    key[5] = (ivid >> 4) & 0xff;

    bits = 48; /* 12 + 12 + 14 + 3 + 7-bit padding */
    return bits;
}

static int
_soc_tr_vlan_xlate_pri_cfi_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=6;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x6 (PRI_CFI key) = 8b (0) PRI, CFI, OVID (0), GLP, KEY_TYPE (6) */

    ivid = soc_VLAN_XLATEm_field32_get(unit, e, IVIDf);
    ivid &= 0xf;
    ovid = 0;
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 7;
    key[1] = ((key_type >> 1) & 0x3) | ((glp << 2) & 0xfc);
    key[2] = (glp >> 6) & 0xff;
    key[3] = ovid & 0xff;
    key[4] = ((ovid >> 8) & 0x0f) | ((ivid << 4) & 0xf0);
    key[5] = (ivid >> 4) & 0xff;

    bits = 48; /* 12 + 12 + 14 + 3 + 7-bit padding */
    return bits;
}

int
soc_tr_vlan_xlate_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    int key_type, rv = 0;
    
    key_type = soc_mem_field32_get(unit, VLAN_XLATEm, entry, KEY_TYPEf);
    switch (key_type) {
        case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
            rv = _soc_tr_vlan_xlate_dtag_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_OTAG:
            rv = _soc_tr_vlan_xlate_otag_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_ITAG:
            rv = _soc_tr_vlan_xlate_itag_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            rv = _soc_tr_vlan_mac_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_OVID:
            rv = _soc_tr_vlan_xlate_ovid_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_IVID:
            rv = _soc_tr_vlan_xlate_ivid_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            rv = _soc_tr_vlan_xlate_pri_cfi_entry_to_key(unit, entry, key);
            break;
    }
    return rv;
}

uint32
soc_tr_vlan_xlate_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_vlan_xlate_base_entry_to_key(unit, entry, key);
    index = soc_tr_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    uint32  tmp_hs;
    int     hash_sel;

    if (bank > 0) { 
        SOC_IF_ERROR_RETURN(READ_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Bf);
    } else {
        SOC_IF_ERROR_RETURN(READ_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Af);
    }

    return soc_tr_vlan_xlate_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr_egr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, EGR_VLAN_XLATEm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate = mask;
        SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        rv = ((uint32)key[0]) | ((uint32)key[1] << 8);
        rv >>= 2;
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr_vlan_xlate_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate;
}

int
soc_tr_egr_vlan_xlate_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32    pgid;
    vlan_id_t ovid, ivid;
    int bits;

    /* key = {IVID, PORT_GROUP_ID, OVID} */

    ivid = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, IVIDf);
    ovid = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, OVIDf);
    pgid = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, PORT_GROUP_IDf);

    key[0] = (ovid << 2) & 0xfc;
    key[1] = ((ovid >> 6) & 0x3f) | ((pgid << 6) & 0xc0);
    key[2] = ((pgid >> 2) & 0x0f) | ((ivid << 4) & 0xf0);
    key[3] = ivid >> 4;
    key[4] = 0;
    key[5] = 0;

    bits = 32; /* 12 + 6 + 12 + 2-bits padding */

    return bits;
}

uint32
soc_tr_egr_vlan_xlate_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_egr_vlan_xlate_base_entry_to_key(unit, entry, key);
    index = soc_tr_egr_vlan_xlate_hash(unit, hash_sel, key_nbits, key);

    return index;
}

uint32
soc_tr_egr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int     hash_sel;
    uint32  tmp_hs;

    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));

        hash_sel = soc_reg_field_get(unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Bf);
    } else {
        SOC_IF_ERROR_RETURN(READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));

        hash_sel = soc_reg_field_get(unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Af);
    }

    return soc_tr_egr_vlan_xlate_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr_mpls_hash(int unit, int hash_sel, int key_nbits, uint32 *base_entry, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_mpls == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, MPLS_ENTRYm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_mpls = mask;
        SOC_CONTROL(unit)->hash_bits_mpls = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry, MPLS_LABELf);
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr_mpls_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_mpls;
}

int
soc_tr_mpls_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 port, module, trunk_bit, label;
    int bits;

    port = soc_MPLS_ENTRYm_field32_get(unit, entry, PORT_NUMf);
    module = soc_MPLS_ENTRYm_field32_get(unit, entry, MODULE_IDf);
    trunk_bit = soc_MPLS_ENTRYm_field32_get(unit, entry, Tf);
    label = soc_MPLS_ENTRYm_field32_get(unit, entry, MPLS_LABELf);

    key[0] = port << 6;
    key[1] = ((port >> 2) & 0xf) | ((module << 4) & 0xf0);
    key[2] = ((module >> 4) & 0x7) | (trunk_bit << 3) | ((label << 4) & 0xf0);
    key[3] = (label >> 4) & 0xff;
    key[4] = (label >> 12) & 0xff;

    bits = 40; /* 20 + 1 + 7 + 6 + 6-bit padding */
    return bits;
}

uint32
soc_tr_mpls_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr_mpls_base_entry_to_key(unit, entry, key);
    index = soc_tr_mpls_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr_mpls_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    uint32  tmp_hs;
    int     hash_sel;

    if (bank > 0) { 
        SOC_IF_ERROR_RETURN(READ_MPLS_ENTRY_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, MPLS_ENTRY_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Bf);
    } else {
        SOC_IF_ERROR_RETURN(READ_MPLS_ENTRY_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, MPLS_ENTRY_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Af);
    }

    return soc_tr_mpls_entry_hash(unit, hash_sel, entry);
}
#endif /* BCM_TRIUMPH_SUPPORT || BCM_SCORPION_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
static int
_soc_tr2_vlan_xlate_dtag_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=0;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x0 (IVID_OVID key) = IVID, OVID, GLP, KEY_TYPE (0) */

    ivid = soc_VLAN_XLATEm_field32_get(unit, e, IVIDf);
    ovid = soc_VLAN_XLATEm_field32_get(unit, e, OVIDf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 5;
    key[1] = (glp  & 0xff);
    key[2] = ((glp >> 8) & 0x3f) | ((ovid << 6) & 0xc0);
    key[3] = (ovid >> 2) & 0xff;
    key[4] = ((ovid >> 10) & 0x3) | ((ivid << 2) & 0xfc);
    key[5] = (ivid >> 6) & 0x3f;
    key[6] = 0;

    bits = 56; /* 10 + 12 + 12 + 14 + 3 + 5-bit padding */

    return bits;
}

static int
_soc_tr2_vlan_xlate_otag_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 otag, glp, key_type=1;
    int bits;

    /* Key type 0x1 (OTAG key) = OTAG, GLP, KEY_TYPE (1) */

    otag = soc_VLAN_XLATEm_field32_get(unit, e, OTAGf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 5;
    key[1] = glp & 0xff;
    key[2] = ((glp >> 8) & 0x3f) | ((otag << 6) & 0xc0);
    key[3] = (otag >> 2) & 0xff;
    key[4] = (otag >> 10) & 0x3f;
    key[5] = 0;
    key[6] = 0;

    bits = 56; /* same as key-type 0 */

    return bits;
}

static int
_soc_tr2_vlan_xlate_itag_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 itag, glp, key_type=2;
    int bits;

    itag = soc_VLAN_XLATEm_field32_get(unit, e, ITAGf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 5;
    key[1] = glp & 0xff;
    key[2] = ((glp >> 8) & 0x3f) | ((itag << 6) & 0xc0);
    key[3] = (itag >> 2) & 0xff;
    key[4] = (itag >> 10) & 0x3f;
    key[5] = 0;
    key[6] = 0;

    bits = 56; /* same as key-type 0 */

    return bits;
}

static int
_soc_tr2_vlan_xlate_ovid_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=4;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x4 (OVID key) = IVID (0), OVID, GLP, KEY_TYPE (4) */

    ivid = 0;
    ovid = soc_VLAN_XLATEm_field32_get(unit, e, OVIDf);
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 5;
    key[1] = (glp & 0xff);
    key[2] = ((glp >> 8) & 0x3f) | ((ovid << 6) & 0xc0);
    key[3] = (ovid >> 2) & 0xff;
    key[4] = ((ovid >> 10) & 0x3) | ((ivid << 2) & 0xfc);
    key[5] = (ivid >> 6) & 0x3f;
    key[6] = 0;

    bits = 56; /* 10 + 12 + 12 + 14 + 3 + 5-bit padding */
    return bits;
}

static int
_soc_tr2_vlan_xlate_ivid_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=5;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x5 (IVID key) = IVID, OVID (0), GLP, KEY_TYPE (5) */

    ivid = soc_VLAN_XLATEm_field32_get(unit, e, IVIDf);
    ovid = 0;
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 5;
    key[1] = (glp & 0xff);
    key[2] = ((glp >> 8) & 0x3f) | ((ovid << 6) & 0xc0);
    key[3] = (ovid >> 2) & 0xff;
    key[4] = ((ovid >> 10) & 0x3) | ((ivid << 2) & 0xfc);
    key[5] = (ivid >> 6) & 0x3f;
    key[6] = 0;

    bits = 56; /* 10 + 12 + 12 + 14 + 3 + 5-bit padding */
    return bits;
}

static int
_soc_tr2_vlan_xlate_pri_cfi_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 glp, key_type=6;
    vlan_id_t ovid, ivid;
    int bits;

    /* Key type 0x6 (PRI_CFI key) = 8b (0) PRI, CFI, OVID (0), GLP, KEY_TYPE (6) */

    ivid = soc_VLAN_XLATEm_field32_get(unit, e, IVIDf);
    ivid &= 0xf;
    ovid = 0;
    glp = soc_VLAN_XLATEm_field32_get(unit, e, GLPf);

    key[0] = key_type << 5;
    key[1] = (glp & 0xff);
    key[2] = ((glp >> 8) & 0x3f) | ((ovid << 6) | 0xc0);
    key[3] = ovid & 0xff;
    key[4] = ((ovid >> 10) & 0x3) | ((ivid << 2) & 0xfc);
    key[5] = (ivid >> 6) & 0x3f;
    key[6] = 0;

    bits = 56; /* 10 + 12 + 12 + 14 + 3 + 5-bit padding */
    return bits;
}

static int
_soc_tr2_vlan_mac_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32 key_type=3;
    sal_mac_addr_t  mac;
    int             i, bits;

    /* Key type 0x3 (VLAN_MAC key) = MAC, KEY_TYPE (3) */
    soc_VLAN_MACm_mac_addr_get(unit, e, MAC_ADDRf, mac);

    key[0] = (key_type << 5) & 0xe0;
    for (i = 0; i < 6; i++) {
        key[i+1] = mac[5 - i];
    }
    bits = 56; /* 48 + 3 + 5-bits padding */

    return bits;
}

static int
_soc_tr2_vlan_xlate_hpae_entry_to_key(int unit, uint32 *e, uint8 *key)
{
    uint32    key_type=7;
    ip_addr_t sip;
    int       bits;

    /* Key type 0x7 (MAC_IP_BIND key) = IP, KEY_TYPE (7) */
    soc_VLAN_MACm_field_get(unit, e, MAC_IP_BIND__SIPf, &sip);

    key[0] = (key_type << 5) & 0xe0;
    key[1] = sip & 0xff;
    key[2] = (sip >> 8) & 0xff;
    key[3] = (sip >> 16) & 0xff;
    key[4] = (sip >> 24) & 0xff;
    key[5] = 0;
    key[6] = 0;
    bits = 56; /* 16 + 32 + 3 + 5-bits padding */

    return bits;
}

int
soc_tr2_vlan_xlate_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    int key_type, rv = 0;
    
    key_type = soc_mem_field32_get(unit, VLAN_XLATEm, entry, KEY_TYPEf);
    switch (key_type) {
        case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
            rv = _soc_tr2_vlan_xlate_dtag_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_OTAG:
            rv = _soc_tr2_vlan_xlate_otag_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_ITAG:
            rv = _soc_tr2_vlan_xlate_itag_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
            rv = _soc_tr2_vlan_mac_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_OVID:
            rv = _soc_tr2_vlan_xlate_ovid_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_IVID:
            rv = _soc_tr2_vlan_xlate_ivid_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
            rv = _soc_tr2_vlan_xlate_pri_cfi_entry_to_key(unit, entry, key);
            break;
        case TR_VLXLT_HASH_KEY_TYPE_HPAE:
            rv = _soc_tr2_vlan_xlate_hpae_entry_to_key(unit, entry, key);
    }
    return rv;
}

uint32
soc_tr2_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, 
                       uint32 *base_entry, uint8 *key)
{
    uint32 rv = 0;
    int key_type;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_vlan_mac == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, VLAN_MACm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_vlan_mac = mask;
        SOC_CONTROL(unit)->hash_bits_vlan_mac = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        key_type = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, KEY_TYPEf);
        switch (key_type) {
            case TR_VLXLT_HASH_KEY_TYPE_IVID_OVID:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OTAG:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OTAGf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_ITAG:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, ITAGf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_VLAN_MAC:
                {
                    sal_mac_addr_t  mac;
                    soc_VLAN_MACm_mac_addr_get(unit, base_entry,
                                               MAC_ADDRf, mac);
                    rv = (((uint32)mac[4]) << 8) | ((uint32)mac[5]);
                }
                break;
            case TR_VLXLT_HASH_KEY_TYPE_OVID:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, OVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_IVID:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, IVIDf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_PRI_CFI:
                rv = soc_mem_field32_get(unit, VLAN_XLATEm, base_entry, GLPf);
                break;
            case TR_VLXLT_HASH_KEY_TYPE_HPAE:
                rv = soc_mem_field32_get(unit, VLAN_MACm, base_entry, MAC_IP_BIND__SIPf);
                break;
            default:
                rv = 0;
                break;
        }
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_vlan_mac;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr_vlan_xlate_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_vlan_mac;
}

uint32
soc_tr2_vlan_xlate_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr2_vlan_xlate_base_entry_to_key(unit, entry, key);
    index = soc_tr2_vlan_xlate_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr2_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    uint32  tmp_hs;
    int     hash_sel;

    if (bank > 0) { 
        SOC_IF_ERROR_RETURN(READ_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Bf);
    } else {
        SOC_IF_ERROR_RETURN(READ_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Af);
    }

    return soc_tr2_vlan_xlate_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr2_mpls_hash(int unit, int hash_sel, int key_nbits, uint32 *base_entry, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_mpls == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, MPLS_ENTRYm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_mpls = mask;
        SOC_CONTROL(unit)->hash_bits_mpls = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        rv = soc_mem_field32_get(unit, MPLS_ENTRYm, base_entry, MPLS_LABELf);
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_mpls;
        break;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr2_mpls_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_mpls;
}

static int
_soc_tr2_mpls_legacy_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 port, module, trunk_bit, label, key_type;
    int bits;

    key_type = soc_MPLS_ENTRYm_field32_get(unit, entry, KEY_TYPEf);
    port = soc_MPLS_ENTRYm_field32_get(unit, entry, PORT_NUMf);
    module = soc_MPLS_ENTRYm_field32_get(unit, entry, MODULE_IDf);
    trunk_bit = soc_MPLS_ENTRYm_field32_get(unit, entry, Tf);
    label = soc_MPLS_ENTRYm_field32_get(unit, entry, MPLS_LABELf);

    key[0] = ((port << 4) & 0xf0) | (key_type << 1);
    key[1] = ((port >> 4) & 0x3) | ((module << 2) & 0xfc);
    key[2] = ((module >> 6) & 0x1) | (trunk_bit << 1) | ((label << 2) & 0xfc);
    key[3] = (label >> 6) & 0xff;
    key[4] = (label >> 14) & 0x3f;
    key[5] = 0;
    key[6] = 0;
    key[7] = 0;

    bits = 64; /* 26 + 20 + 1 + 7 + 6 + 3 + 1-bit padding */
    return bits;
}

static int
_soc_tr2_mpls_mim_nvp_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 key_type, bvid;
    int bits;
    sal_mac_addr_t bmacsa;

    key_type = soc_MPLS_ENTRYm_field32_get(unit, entry, KEY_TYPEf);
    soc_mem_mac_addr_get(unit, MPLS_ENTRYm, entry, MIM_NVP__BMACSAf, bmacsa);
    bvid = soc_MPLS_ENTRYm_field32_get(unit, entry, MIM_NVP__BVIDf);

    key[0] = ((bvid << 4) & 0xf0) | (key_type << 1);
    key[1] = (bvid >> 4) & 0xff;
    key[2] = bmacsa[5];
    key[3] = bmacsa[4];
    key[4] = bmacsa[3];
    key[5] = bmacsa[2];
    key[6] = bmacsa[1];
    key[7] = bmacsa[0];

    bits = 64; /* 48 + 12 + 3 + 1-bit padding */
    return bits;
}

static int
_soc_tr2_mpls_mim_isid_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 key_type, isid;
    int bits;

    key_type = soc_MPLS_ENTRYm_field32_get(unit, entry, KEY_TYPEf);
    isid = soc_MPLS_ENTRYm_field32_get(unit, entry, MIM_ISID__ISIDf);

    key[0] = ((isid << 4) & 0xf0) | (key_type << 1);
    key[1] = (isid >> 4) & 0xff;
    key[2] = (isid >> 12) & 0xff;
    key[3] = (isid >> 20) & 0xf;
    key[4] = 0;
    key[5] = 0;
    key[6] = 0;
    key[7] = 0;

    bits = 64; /* 36 + 24 + 3 + 1-bit padding */
    return bits;
}

static int
_soc_tr2_mpls_mim_isid_svp_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 key_type, isid, svp;
    int bits;

    key_type = soc_MPLS_ENTRYm_field32_get(unit, entry, KEY_TYPEf);
    isid = soc_MPLS_ENTRYm_field32_get(unit, entry, MIM_ISID__ISIDf);
    svp = soc_MPLS_ENTRYm_field32_get(unit, entry, MIM_ISID__SVPf);

    key[0] = ((isid << 4) & 0xf0) | (key_type << 1);
    key[1] = (isid >> 4) & 0xff;
    key[2] = (isid >> 12) & 0xff;
    key[3] = ((svp << 4) & 0xf0) | ((isid >> 20) & 0xf);
    key[4] = (svp >> 4) & 0xff;
    key[5] = (svp >> 12) & 0x1;
    key[6] = 0;
    key[7] = 0;

    bits = 64; /* 23 + 13 + 24 + 3 + 1-bit padding */
    return bits;
}

static int
_soc_tr2_mpls_wlan_mac_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32 key_type;
    int bits;
    sal_mac_addr_t mac;

    key_type = soc_MPLS_ENTRYm_field32_get(unit, entry, KEY_TYPEf);
    soc_mem_mac_addr_get(unit, MPLS_ENTRYm, entry, WLAN_MAC__MAC_ADDRf, mac);

    key[0] = ((mac[5] << 4) & 0xf0) | (key_type << 1);
    key[1] = ((mac[5] >> 4) & 0xf) | ((mac[4] << 4) & 0xf0);
    key[2] = ((mac[4] >> 4) & 0xf) | ((mac[3] << 4) & 0xf0);
    key[3] = ((mac[3] >> 4) & 0xf) | ((mac[2] << 4) & 0xf0);
    key[4] = ((mac[2] >> 4) & 0xf) | ((mac[1] << 4) & 0xf0);
    key[5] = ((mac[1] >> 4) & 0xf) | ((mac[0] << 4) & 0xf0);
    key[6] = (mac[0] >> 4) & 0xf;
    key[7] = 0;

    bits = 64; /* 12 + 48 + 3 + 1-bit padding */
    return bits;

}

int
soc_tr2_mpls_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    int key_type, rv = 0;

    key_type = soc_mem_field32_get(unit, MPLS_ENTRYm, entry, KEY_TYPEf);

    switch (key_type) {
        case 0:
            rv = _soc_tr2_mpls_legacy_entry_to_key(unit, entry, key);
            break;
        case 1:
            rv = _soc_tr2_mpls_mim_nvp_entry_to_key(unit, entry, key);
            break;
        case 2:
            rv = _soc_tr2_mpls_mim_isid_entry_to_key(unit, entry, key);
            break;
        case 3:
            rv = _soc_tr2_mpls_mim_isid_svp_entry_to_key(unit, entry, key);
            break;
        case 4:
            rv = _soc_tr2_mpls_wlan_mac_entry_to_key(unit, entry, key);
            break;
        default:
            break;
    }
    return rv;
}

uint32
soc_tr2_mpls_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr2_mpls_base_entry_to_key(unit, entry, key);
    index = soc_tr2_mpls_hash(unit, hash_sel, key_nbits, entry, key);

    return index;
}

uint32
soc_tr2_mpls_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    uint32  tmp_hs;
    int     hash_sel;

    if (bank > 0) { 
        SOC_IF_ERROR_RETURN(READ_MPLS_ENTRY_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, MPLS_ENTRY_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Bf);
    } else {
        SOC_IF_ERROR_RETURN(READ_MPLS_ENTRY_HASH_CONTROLr(unit, &tmp_hs));
        hash_sel = soc_reg_field_get(unit, MPLS_ENTRY_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Af);
    }

    return soc_tr2_mpls_entry_hash(unit, hash_sel, entry);
}

uint32
soc_tr2_egr_vlan_xlate_hash(int unit, int hash_sel, int key_nbits, uint8 *key)
{
    uint32 rv;

    /*
     * Cache bucket mask and shift amount for upper crc
     */
    if (SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate == 0) {
        uint32  mask;
        int     bits;

        /* 8 Entries per bucket */
        mask = soc_mem_index_max(unit, EGR_VLAN_XLATEm) >> 3;
        bits = 0;
        rv = 1;
        while (rv && (mask & rv)) {
            bits += 1;
            rv <<= 1;
        }
        SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate = mask;
        SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate = bits;
    }

    switch (hash_sel) {
    case FB_HASH_CRC16_UPPER:
        rv = soc_crc16b(key, key_nbits);
        rv >>= 16 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;

    case FB_HASH_CRC16_LOWER:
        rv = soc_crc16b(key, key_nbits);
        break;

    case FB_HASH_LSB:
        rv = ((uint32)key[0]) | ((uint32)(key[1] & 0x3f) << 8);
        rv >>= 3;
        break;

    case FB_HASH_ZERO:
        rv = 0;
        break;

    case FB_HASH_CRC32_UPPER:
        rv = soc_crc32b(key, key_nbits);
        rv >>= 32 - SOC_CONTROL(unit)->hash_bits_egr_vlan_xlate;
        break;
        break;

    case FB_HASH_CRC32_LOWER:
        rv = soc_crc32b(key, key_nbits);
        break;

    default:
        soc_cm_debug(DK_ERR,
                     "soc_tr2_vlan_xlate_hash: invalid hash_sel %d\n",
                     hash_sel);
        rv = 0;
        break;
    }

    return rv & SOC_CONTROL(unit)->hash_mask_egr_vlan_xlate;
}

int
_soc_tr2_egr_vlan_xlate_xlate_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32    dvp;
    vlan_id_t ovid, ivid;
    int bits, key_type;

    key_type = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry, ENTRY_TYPEf);

    /* key = {IVID, DVP, OVID} */

    ivid = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, IVIDf);
    ovid = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, OVIDf);
    dvp = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, DVPf);

    key[0] = ((ovid << 3) & 0xf8) | key_type;
    key[1] = ((ovid >> 5) & 0x7f) | ((dvp << 7) & 0x80);
    key[2] = ((dvp >> 1) & 0xff);
    key[3] = ((dvp >> 9) & 0xf) | ((ivid << 4) & 0xf0);
    key[4] = (ivid >> 4) & 0xff;
    key[5] = 0;
    key[6] = 0;

    bits = 56; /* 16 + 12 + 13 + 12 + 3 */

    return bits;
}

int
_soc_tr2_egr_vlan_xlate_mim_isid_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32    keyfield;
    int bits, key_type;

    key_type = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry, ENTRY_TYPEf);

    /* key = {KEY} */

    keyfield = soc_EGR_VLAN_XLATEm_field32_get(unit, entry, MIM_ISID__KEYf);

    key[0] = ((keyfield << 3) & 0xf8) | key_type;
    key[1] = (keyfield >> 5) & 0xff;
    key[2] = (keyfield >> 13) & 0xff;
    key[3] = (keyfield >> 21) & 0xf;
    key[4] = 0;
    key[5] = 0;
    key[6] = 0;

    bits = 56; /* 28 + 25 + 3 */

    return bits;

}

int
_soc_tr2_egr_vlan_xlate_wlan_svp_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    uint32    keyfield[2];
    int bits, key_type;

    key_type = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry, ENTRY_TYPEf);

    /* key = {KEY} */

    soc_EGR_VLAN_XLATEm_field_get(unit, entry, WLAN_SVP__KEYf, keyfield);

    key[0] = ((keyfield[0] << 3) & 0xf8) | key_type;
    key[1] = (keyfield[0] >> 5) & 0xff;
    key[2] = (keyfield[0] >> 13) & 0xff;
    key[3] = (keyfield[0] >> 21) & 0xff;
    key[4] = ((keyfield[0] >> 29) & 0x7) | ((keyfield[1] << 3) & 0xf8);
    key[5] = (keyfield[1] >> 5) & 0xff;
    key[6] = (keyfield[1] >> 13) & 0xff;

    bits = 56; /* 53 + 3 */

    return bits;

}

int
soc_tr2_egr_vlan_xlate_base_entry_to_key(int unit, uint32 *entry, uint8 *key)
{
    int entry_type, rv = 0;

    entry_type = soc_mem_field32_get(unit, EGR_VLAN_XLATEm, entry, ENTRY_TYPEf);

    switch (entry_type) {
        case 0:
        case 1:
        case 2:
            rv = _soc_tr2_egr_vlan_xlate_xlate_entry_to_key(unit, entry, key);
            break;
        case 3:
        case 4:
            rv = _soc_tr2_egr_vlan_xlate_mim_isid_entry_to_key(unit, entry, key);
            break;
        case 5:
        case 6:
        case 7:
            rv = _soc_tr2_egr_vlan_xlate_wlan_svp_entry_to_key(unit, entry, key);
        default:
            break;
    }
    return rv;
}

uint32
soc_tr2_egr_vlan_xlate_entry_hash(int unit, int hash_sel, uint32 *entry)
{
    int             key_nbits;
    uint8           key[SOC_MAX_MEM_WORDS * 4];
    uint32          index;

    key_nbits = soc_tr2_egr_vlan_xlate_base_entry_to_key(unit, entry, key);
    index = soc_tr2_egr_vlan_xlate_hash(unit, hash_sel, key_nbits, key);

    return index;
}

uint32
soc_tr2_egr_vlan_xlate_bank_entry_hash(int unit, int bank, uint32 *entry)
{
    int     hash_sel;
    uint32  tmp_hs;

    if (bank > 0) {
        SOC_IF_ERROR_RETURN(READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));

        hash_sel = soc_reg_field_get(unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Bf);
    } else {
        SOC_IF_ERROR_RETURN(READ_EGR_VLAN_XLATE_HASH_CONTROLr(unit, &tmp_hs));

        hash_sel = soc_reg_field_get(unit, EGR_VLAN_XLATE_HASH_CONTROLr,
                                     tmp_hs, HASH_SELECT_Af);
    }

    return soc_tr2_egr_vlan_xlate_entry_hash(unit, hash_sel, entry);

}
#endif /* BCM_TRIUMPH2_SUPPORT */
#endif /* BCM_XGS_SWITCH_SUPPORT */
