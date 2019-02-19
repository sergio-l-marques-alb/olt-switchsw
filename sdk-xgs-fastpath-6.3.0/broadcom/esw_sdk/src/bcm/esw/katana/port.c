/*
 * $Id: port.c 1.11 Broadcom SDK $
 * 
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
 */


#include <soc/defs.h>
#include <soc/debug.h>
#include <soc/drv.h>

#include <sal/core/time.h>

#include <bcm/port.h>
#include <bcm/tx.h>
#include <bcm/error.h>

#include <soc/katana.h>
#include <bcm_int/esw/katana.h>

extern int _bcm_esw_reinit_ka_war(int unit);
extern int bcm_esw_port_loopback_set(int unit, bcm_port_t port, int loopback);
extern int bcm_esw_tx(int unit, bcm_pkt_t *pkt, void *cookie);

#if defined(BCM_KATANA_SUPPORT)

/*
 * Function:
 *      bcm_kt_port_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (IN)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(IN)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_port_rate_egress_set(int unit, bcm_port_t port,
                            uint32 kbits_sec, uint32 kbits_burst)
{
    int rv = BCM_E_NONE; 
    uint32 rate_exp, rate_mantissa;
    uint32 burst_exp, burst_mantissa;
    uint32 cycle_sel;
    lls_port_shaper_config_c_entry_t port_entry;
    lls_port_shaper_config_c_entry_t port_null_entry;
    lls_port_shaper_bucket_c_entry_t port_bucket_entry;

    soc_field_t rate_exp_f =  C_MAX_REF_RATE_EXPf;
    soc_field_t rate_mant_f = C_MAX_REF_RATE_MANTf;
    soc_field_t burst_exp_f = C_MAX_THLD_EXPf;
    soc_field_t burst_mant_f = C_MAX_THLD_MANTf; 
    soc_field_t cycle_sel_f = C_MAX_CYCLE_SELf;
   
    /* compute exp and mantissa and program the registers */
    rv = soc_katana_get_shaper_rate_info(unit, kbits_sec,
                                         &rate_mantissa, &rate_exp);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_get_shaper_burst_info(unit, kbits_burst,
                                          &burst_mantissa, &burst_exp, 0);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    if (rate_exp < 10) {
        burst_exp = 5;
        burst_mantissa = 0;
    } else {
        burst_exp = rate_exp - 4;
        burst_mantissa = (rate_mantissa >> 3);
    }

    if (rate_exp > 4) {
        cycle_sel = 0;
    } else if (rate_exp > 1) {    
        cycle_sel = 5 - rate_exp;
    } else {
        cycle_sel = 4;
    }

    SOC_IF_ERROR_RETURN
       (soc_mem_read(unit, LLS_PORT_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                     port, &port_entry));
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        rate_exp_f, rate_exp);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        rate_mant_f, rate_mantissa);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        burst_exp_f, burst_exp);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        burst_mant_f, burst_mantissa);
    soc_mem_field32_set(unit, LLS_PORT_SHAPER_CONFIG_Cm, &port_entry,
                        cycle_sel_f, cycle_sel);    
    if (soc_feature(unit, soc_feature_dynamic_shaper_update)) {
        sal_memset(&port_null_entry, 0, sizeof(lls_port_shaper_config_c_entry_t));
        sal_memset(&port_bucket_entry, 0, sizeof(lls_port_shaper_bucket_c_entry_t));
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                           MEM_BLOCK_ALL, port, &port_null_entry));
        soc_mem_field32_set(unit, LLS_PORT_SHAPER_BUCKET_Cm, &port_bucket_entry,
                            NOT_ACTIVE_IN_LLSf, 1);   
        SOC_IF_ERROR_RETURN
            (soc_mem_write(unit, LLS_PORT_SHAPER_BUCKET_Cm,
                           MEM_BLOCK_ALL, port, &port_bucket_entry));   
    }    
    SOC_IF_ERROR_RETURN
       (soc_mem_write(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                      MEM_BLOCK_ALL, port, &port_entry));

    return BCM_E_NONE; 
}

/*
 * Function:
 *      bcm_kt_port_rate_egress_get
 * Purpose:
 *      Get egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (OUT)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(OUT)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
_bcm_kt_port_rate_egress_get(int unit, bcm_port_t port,
                            uint32 *kbits_sec, uint32 *kbits_burst)
{
    int rv = BCM_E_NONE;
    uint32 rate_exp, rate_mantissa;
    uint32 burst_exp, burst_mantissa;
    lls_port_shaper_config_c_entry_t port_entry;

    soc_field_t rate_exp_f =  C_MAX_REF_RATE_EXPf;
    soc_field_t rate_mant_f = C_MAX_REF_RATE_MANTf;
    soc_field_t burst_exp_f = C_MAX_THLD_EXPf;
    soc_field_t burst_mant_f = C_MAX_THLD_MANTf;

    /* Input parameters check. */
    if (!kbits_sec || !kbits_burst) {
        return (BCM_E_PARAM);
    }

    SOC_IF_ERROR_RETURN
       (soc_mem_read(unit, LLS_PORT_SHAPER_CONFIG_Cm, MEM_BLOCK_ALL,
                     port, &port_entry));
    rate_exp = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm, 
                                   &port_entry,  rate_exp_f);
    rate_mantissa = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                                        &port_entry, rate_mant_f);
    burst_exp = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                                    &port_entry, burst_exp_f);
    burst_mantissa = soc_mem_field32_get(unit, LLS_PORT_SHAPER_CONFIG_Cm,
                                         &port_entry, burst_mant_f);

    /* convert exp and mantissa to bps */
    rv = soc_katana_compute_shaper_rate(unit, rate_mantissa, rate_exp,
                                        kbits_sec);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }

    rv = soc_katana_compute_shaper_burst(unit, burst_mantissa, burst_exp,
                                         kbits_burst);
    if (BCM_FAILURE(rv)) {
        return(rv);
    }


    return BCM_E_NONE;

}

/*
 * Function:
 *      bcm_kt_port_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (IN)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(IN)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_port_rate_egress_set(int unit, bcm_port_t port,
                            uint32 kbits_sec, uint32 kbits_burst, uint32 mode)
{
  return _bcm_kt_port_rate_egress_set(unit, port, kbits_sec, kbits_burst);
}

/*
 * Function:
 *      bcm_kt_port_rate_egress_get
 * Purpose:
 *      Get egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit       - (IN)SOC unit number
 *      port       - (IN)Port number
 *      kbits_sec  - (OUT)Rate in kilobits (1000 bits) per second.
 *                       Rate of 0 disables rate limiting.
 *      kbits_burst -(OUT)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_port_rate_egress_get(int unit, bcm_port_t port,
                            uint32 *kbits_sec, uint32 *kbits_burst, uint32 *mode)
{
  return _bcm_kt_port_rate_egress_get(unit, port, kbits_sec, kbits_burst);
}

/*
 * Function:
 *      bcm_kt_port_pps_rate_egress_set
 * Purpose:
 *      Set egress rate limiting parameters for the Katana chip.
 * Parameters:
 *      unit        - (IN)SOC unit number
 *      port       - (IN)Port number
 *      pps        - (IN)Rate in packets per second
 *                       Rate of 0 disables rate limiting.
 *      burst      -(IN)Maximum burst size in kilobits (1000 bits).
 * Returns:
 *      BCM_E_XXX
 */
int
bcm_kt_port_pps_rate_egress_set(int unit, bcm_port_t port,
                                uint32 pps, uint32 burst)
{
    lls_port_config_entry_t port_cfg;
    int packet_mode;

    SOC_IF_ERROR_RETURN
        (READ_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &port_cfg));
    packet_mode = soc_mem_field32_get(unit, LLS_PORT_CONFIGm, &port_cfg, 
                                      PACKET_MODE_WRR_ACCOUNTING_ENABLEf);
    if (packet_mode == 0) {
        soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, 
                            PACKET_MODE_WRR_ACCOUNTING_ENABLEf, 1);
        soc_mem_field32_set(unit, LLS_PORT_CONFIGm, &port_cfg, 
                            PACKET_MODE_SHAPER_ACCOUNTING_ENABLEf, 1);
        SOC_IF_ERROR_RETURN
            (WRITE_LLS_PORT_CONFIGm(unit, MEM_BLOCK_ALL, port, &port_cfg));
    }    
        
    return _bcm_kt_port_rate_egress_set(unit, port, pps, burst);    
}
#endif /* BCM_KATANA_SUPPORT */
