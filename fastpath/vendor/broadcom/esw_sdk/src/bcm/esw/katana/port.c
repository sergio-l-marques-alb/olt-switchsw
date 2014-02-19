/*
 * $Id: port.c 1.1.2.8 Broadcom SDK $
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

#define S3MII_WAR_MAX_RETRIES   25
#define S3MII_WAR_BUF_SIZE      1736

int _kt_s3mii_war(int unit, uint32 *tx_pkt)
{
    soc_pbmp_t  fe_pbmp;
    soc_port_t  p;
    bcm_pkt_t   pkt_info;
    int         retries, count;
    int         passed, rv = 0;

    /* Applicable only for FE ports. */
    SOC_PBMP_ASSIGN(fe_pbmp, PBMP_FE_ALL(unit));
    if (SOC_PBMP_IS_NULL(fe_pbmp)) {
        return BCM_E_NONE;
    }

    pkt_info.unit = unit;
    pkt_info._pkt_data.data = (uint8 *) tx_pkt;
    pkt_info.pkt_data = &pkt_info._pkt_data;
    pkt_info.blk_count = 1;
    pkt_info._pkt_data.len = 1518;
    pkt_info.call_back = NULL;
    pkt_info.flags = 0x00230020;

    passed = 0;
    for(retries = 0; retries < S3MII_WAR_MAX_RETRIES; retries++) {

        /* Setup */
        PBMP_ITER(fe_pbmp, p) {
            if ((rv = bcm_esw_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_PHY)) < 0) {
                soc_cm_debug(DK_ERR, "Port %d LoopBack Set Failed\n", p);
            }
        }

        /* Send Packets */
        PBMP_ITER(fe_pbmp, p) {
            for (count = 0; count < 500; count++) {
                BCM_PBMP_CLEAR(pkt_info.tx_pbmp);
                BCM_PBMP_PORT_ADD(pkt_info.tx_pbmp, p);
                rv = bcm_esw_tx(unit, &pkt_info, NULL);
                if (rv < 0) {
                    soc_cm_debug(DK_ERR, "Tx Error %d\n", rv);
                    return BCM_E_FAIL;
                }
                sal_udelay(SAL_BOOT_SIMULATION ? 100000 : 100);
            }
        }

        /* Verify Counters */
        sal_udelay(SAL_BOOT_SIMULATION ? 100000 : 100);
        rv = SOC_E_NONE;
        PBMP_ITER(fe_pbmp, p) {
            uint32 gtpkt, grfcs, grpkt;
            
            SOC_IF_ERROR_RETURN(READ_GTPKTr(unit, p, &gtpkt));
            SOC_IF_ERROR_RETURN(READ_GRFCSr(unit, p, &grfcs));
            SOC_IF_ERROR_RETURN(READ_GRPKTr(unit, p, &grpkt));
            if ((gtpkt != 500) || (grpkt != 500) || (grfcs)) {
                soc_cm_debug(DK_VERBOSE, "Port %d GTPKT = %d, GRPKT = %d, GRFCS = %d\n", p, gtpkt, grpkt, grfcs);
                rv = BCM_E_FAIL;
                break;
            }
        }

        /* Re-init and retest  */
        if (rv < 0) {
            SOC_IF_ERROR_RETURN(soc_reset_init(unit));
            SOC_IF_ERROR_RETURN(soc_misc_init(unit));
            SOC_IF_ERROR_RETURN(soc_mmu_init(unit));
            SOC_IF_ERROR_RETURN(_bcm_esw_reinit_ka_war(unit));
        } else {
            soc_cm_debug(DK_VERBOSE, "S3MII WAR completed in %d Iterations\n", retries + 1);
            passed = 1;
            break; /* All clear */
        }
    }
    /* Cleanup */
    PBMP_ITER(fe_pbmp, p) {
        if ((rv = bcm_esw_port_loopback_set(unit, p, BCM_PORT_LOOPBACK_NONE)) < 0) {
            soc_cm_debug(DK_ERR, "Port %d LoopBack reset Failed\n", p);
        }
    }
    return(passed ? BCM_E_NONE : BCM_E_FAIL);
}

int kt_s3mii_war(int unit)
{
    uint32 *tx_pkt;
    uint32 *buff;
    int rv,i;
    sal_usecs_t stime = sal_time_usecs();

    if (soc_property_get(unit, spn_S3MII_WAR_ENABLE, 1) == 0) {
        return BCM_E_NONE;
    }

    /* Applicable only for FE ports. */
    if (SOC_PBMP_IS_NULL(PBMP_FE_ALL(unit))) {
        return BCM_E_NONE;
    }

    tx_pkt = soc_cm_salloc(unit, S3MII_WAR_BUF_SIZE, "tx_pkt");
    if (tx_pkt == NULL) {
        return BCM_E_MEMORY;
    }
    buff = tx_pkt;
    for(i=0; i<16; i+=4) {
        *buff = 0;
        buff++;
    }
    for(i=0; i<1518; i+=4) {
        *buff = 0xaa55aa55;
        buff++;
    }
/*    sal_memset(tx_pkt, 0, S3MII_WAR_BUF_SIZE); */
    soc_cm_sflush(unit, tx_pkt, S3MII_WAR_BUF_SIZE);

    rv = _kt_s3mii_war(unit, tx_pkt);

    if (rv < 0) {
        soc_cm_debug(DK_ERR, "S3MII WAR Failed\n");
    }
    soc_cm_sfree(unit, tx_pkt);
    soc_cm_debug(DK_VERBOSE, "S3MII WAR: took %d usec\n",
                 SAL_USECS_SUB(sal_time_usecs(), stime));
    return rv;
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

    /* if burst size less than recommended, set the recommended burst size */
    if (rate_exp < 10) {
        if (burst_exp < 5) {
            burst_exp = 5;
            burst_mantissa = 0;
        }
    } else {
        if (burst_exp < (rate_exp - 4)) {
            burst_exp = rate_exp - 4;
               burst_mantissa = (rate_mantissa >> 3);
        }
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
                            uint32 kbits_sec, uint32 kbits_burst)
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
                            uint32 *kbits_sec, uint32 *kbits_burst)
{
  return _bcm_kt_port_rate_egress_get(unit, port, kbits_sec, kbits_burst); 
}
#endif /* BCM_KATANA_SUPPORT */
