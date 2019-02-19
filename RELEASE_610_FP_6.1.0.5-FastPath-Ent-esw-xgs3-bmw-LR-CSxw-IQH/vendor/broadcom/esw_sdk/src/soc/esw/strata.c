/*
 * $Id: strata.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 * File:        strata.c
 * Purpose:
 * Requires:
 */

#include <soc/strata.h>
#include <soc/drv.h>
#include <soc/counter.h>
#include <soc/ll.h>
#include <soc/error.h>
#include <soc/debug.h>

#ifdef BCM_STRATA_SUPPORT

/*
 * Strata chip driver functions.  Common across strata devices for now.
 * These may get broken out by chip in the future, but not needed yet.
 */
soc_functions_t soc_strata_drv_funs = {
    soc_strata_misc_init,
    soc_strata_mmu_init,
    soc_strata_age_timer_get,
    soc_strata_age_timer_max_get,
    soc_strata_age_timer_set
};

int
soc_strata_misc_init(int unit)
{
    uint32	epc_rate_n_ip;
    uint32	config;
    uint16	dev_id;
    uint64	reg64;
    int		port;

    soc_cm_get_id(unit, &dev_id, NULL);

    /*
     * Broadcast rate limit control
     */

    epc_rate_n_ip = 0;
    soc_reg_field_set(unit, EPC_RATE_N_IPr, &epc_rate_n_ip, DLFBC_ENf, 1);
    soc_reg_field_set(unit, EPC_RATE_N_IPr, &epc_rate_n_ip, LIMITf, 0x1000);
    if (dev_id == BCM5645_DEVICE_ID) {
	/* IPX_SIZE bits - part of GNATS 2442 workaround on BCM5645_B0 */
	soc_reg_field_set(unit, EPC_RATE_N_IPr,
			  &epc_rate_n_ip, IPX_SIZEf, 0x1f);
    }
    PBMP_E_ITER(unit, port) {
	SOC_IF_ERROR_RETURN(WRITE_EPC_RATE_N_IPr(unit, port, epc_rate_n_ip));
    }

    /*
     * Chip CONFIG register
     */

    SOC_IF_ERROR_RETURN(soc_reg_read_any_block(unit, CONFIGr, &config));
    soc_reg_field_set(unit, CONFIGr, &config, L3_ENAf,
		      soc_property_get(unit, spn_L3_ENABLE, 1));
    SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, CONFIGr, config));

#ifdef BCM_STRATA2_SUPPORT
    /*
     * Chip CONFIG2 register
     */

    if (soc_feature(unit, soc_feature_ip_mcast)) {
	uint32          config2;

	SOC_IF_ERROR_RETURN(soc_reg_read_any_block(unit, CONFIG2r, &config2));
	soc_reg_field_set(unit, CONFIG2r, &config2, IPMC_DISABLEf,
			  !soc_property_get(unit, spn_IPMC_ENABLE, 1));
	SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, CONFIG2r, config2));
    }

    /* gig port i/o voltage control: BCM5615 and later */
    if (SOC_CHIP_GROUP(unit) == SOC_CHIP_BCM5615) {
	uint32		config2;

	PBMP_GE_ITER(unit, port) {
	    SOC_IF_ERROR_RETURN(READ_CONFIG2r(unit, port, &config2));
	    soc_reg_field_set(unit, CONFIG2r, &config2, IOV_SELf,
			      soc_property_port_get(unit, port, spn_GIG_IOV,
						    0));
	    SOC_IF_ERROR_RETURN(WRITE_CONFIG2r(unit, port, config2));
	}
    }
#endif	/* BCM_STRATA2_SUPPORT */

    /*
     * Egress Enable
     */

    if (NUM_FE_PORT(unit) > 0) {
	COMPILER_64_ZERO(reg64);
	soc_reg64_field32_set(unit, EGR_ENABLEr, &reg64, PRT_ENABLEf, 0xff);
	SOC_IF_ERROR_RETURN
	    (soc_reg64_write_all_blocks(unit, EGR_ENABLEr, reg64));
    }

    COMPILER_64_ZERO(reg64);
    soc_reg64_field32_set(unit, GEGR_ENABLEr, &reg64, PRT_ENABLEf, 1);
    PBMP_GE_ITER(unit, port) {
	SOC_IF_ERROR_RETURN(WRITE_GEGR_ENABLEr(unit, port, reg64));
    }

    SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, EPC_HOLr, 0x0));

    SOC_IF_ERROR_RETURN
	(soc_reg_write_all_blocks(unit, EPC_LINKr,
				  SOC_PBMP_WORD_GET(PBMP_CMIC(unit), 0)));
    SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, EGR_BKP_DISr, 0x0));
    SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, EPC_PFM0r, 0x0));
    SOC_IF_ERROR_RETURN(soc_reg_write_all_blocks(unit, EPC_PFM1r, 0x0));
    SOC_IF_ERROR_RETURN
	(soc_reg_write_all_blocks(unit, EGR_MAXTIMEr, 0xffffffff));

    return SOC_E_NONE;
}

int
soc_strata_mmu_init(int unit)
{
    uint32	hbm_fe, hbm_ge, hbm_cpu;
    uint32	ibp_fe, ibp_ge, ibp_cpu;
    uint32	lwm, lwmsum, hwm;
    uint32	sched;
    soc_cos_t	cos;
    int		port;

    if (SOC_CHIP_GROUP(unit) == SOC_CHIP_BCM5680) {
	hbm_fe = 0;
	hbm_ge = soc_property_get(unit, spn_MMU_HBM_GE, 0x300);
	hbm_cpu = soc_property_get(unit, spn_MMU_HBM_CPU, 0x300);

	ibp_fe = 0;
	ibp_ge = soc_property_get(unit, spn_MMU_IBP_GE, 0x200);
	ibp_cpu = soc_property_get(unit, spn_MMU_IBP_CPU, 0x200);

	lwm = 0x100;
	hwm = 0x1fff;
    } else {
	hbm_fe = soc_property_get(unit, spn_MMU_HBM_FE, 0x1c00);
	hbm_ge = soc_property_get(unit, spn_MMU_HBM_GE, 0xf000);
	hbm_cpu = soc_property_get(unit, spn_MMU_HBM_CPU, 0x1c00);

	ibp_fe = soc_property_get(unit, spn_MMU_IBP_FE, 0x2100);
	ibp_ge = soc_property_get(unit, spn_MMU_IBP_GE, 0xe000);
	ibp_cpu = soc_property_get(unit, spn_MMU_IBP_CPU, 0x2000);

	if (soc_property_get(unit, spn_MMU_SDRAM_ENABLE, 1)) {
	    lwm = 0x4c;
	    hwm = 0x7e;
	} else {
	    lwm = 0xfff;
	    hwm = 0xfff;
            
	}
    }
    lwmsum = 0;

    sched = 0;
    soc_reg_field_set(unit, SCHEDULERCONTROLr, &sched,
		      PACKETPOINTERPOOLSIZEf, 1);

    PBMP_FE_ITER(unit, port) {
	SOC_IF_ERROR_RETURN(WRITE_LOWWATERMARKERr(unit, port, lwm));
	lwmsum += lwm;
	SOC_IF_ERROR_RETURN(WRITE_HIGHWATERMARKERr(unit, port, hwm));
	SOC_IF_ERROR_RETURN(WRITE_HOLBLOCKINGMARKERr(unit, port, hbm_fe));
	SOC_IF_ERROR_RETURN(WRITE_INGRESSBACKPRESSUREr(unit, port, ibp_fe));
	SOC_IF_ERROR_RETURN(WRITE_SCHEDULERCONTROLr(unit, port, sched));
    }

    PBMP_GE_ITER(unit, port) {
	SOC_IF_ERROR_RETURN(WRITE_LOWWATERMARKERr(unit, port, lwm));
	lwmsum += lwm;
	SOC_IF_ERROR_RETURN(WRITE_HIGHWATERMARKERr(unit, port, hwm));
	SOC_IF_ERROR_RETURN(WRITE_HOLBLOCKINGMARKERr(unit, port, hbm_ge));
	SOC_IF_ERROR_RETURN(WRITE_INGRESSBACKPRESSUREr(unit, port, ibp_ge));
	SOC_IF_ERROR_RETURN(WRITE_SCHEDULERCONTROLr(unit, port, sched));
    }

    port = CMIC_PORT(unit);
    SOC_IF_ERROR_RETURN(WRITE_LOWWATERMARKERr(unit, port, lwm));
    lwmsum += lwm;
    SOC_IF_ERROR_RETURN(WRITE_HIGHWATERMARKERr(unit, port, hwm));
    SOC_IF_ERROR_RETURN(WRITE_HOLBLOCKINGMARKERr(unit, port, hbm_cpu));
    SOC_IF_ERROR_RETURN(WRITE_INGRESSBACKPRESSUREr(unit, port, ibp_cpu));
    SOC_IF_ERROR_RETURN(WRITE_SCHEDULERCONTROLr(unit, port, sched));

    SOC_IF_ERROR_RETURN(WRITE_SUM_LWMSr(unit, lwmsum));

    for (cos = 0; cos < NUM_COS(unit); cos++) {
	SOC_IF_ERROR_RETURN(WRITE_FLOWCONTROLr(unit, cos, 0));
    }

    return SOC_E_NONE;
}

int
soc_strata_age_timer_get(int unit, int *age_seconds, int *enabled)
{
    uint32 value;

    SOC_IF_ERROR_RETURN
	(soc_reg_read_any_block(unit, AGE_TIMERr, &value));

    *age_seconds = soc_reg_field_get(unit, AGE_TIMERr, value, AGE_VALf);
    *enabled = soc_reg_field_get(unit, AGE_TIMERr, value, AGE_ENAf);

    return SOC_E_NONE;
}

int
soc_strata_age_timer_max_get(int unit, int *max_seconds)
{
    *max_seconds = soc_reg_field_get(unit, AGE_TIMERr, 0xffffffff, AGE_VALf);

    return SOC_E_NONE;
}

int
soc_strata_age_timer_set(int unit, int age_seconds, int enable)
{
    uint32 value;

    value = 0;
    soc_reg_field_set(unit, AGE_TIMERr, &value, AGE_ENAf, enable);
    soc_reg_field_set(unit, AGE_TIMERr, &value, AGE_VALf, age_seconds);
    return soc_reg_write_all_blocks(unit, AGE_TIMERr, value);
}

#endif	/* BCM_STRATA_SUPPORT */
