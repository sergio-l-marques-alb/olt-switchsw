/*
 * $Id: dev.c 1.4 Broadcom SDK $
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
 */
#include <shared/types.h>
#include <soc/error.h>
#include <soc/drv_if.h>
#include <soc/robo/mcm/driver.h>
#include "robo_northstarplus.h"

/*
 * Save the EEE configuration before entering the MAC low power mode.
 * restore it when exiting the MAC low power mode
 */
static uint32   bcm53020_eee_enable_config[SOC_MAX_NUM_SWITCH_DEVICES];

/* the switch PBMP to indicated the ports within attached Switch-MACSEC */
static uint32   _soc_nsp_macsec_attach_pbmp[SOC_MAX_NUM_SWITCH_DEVICES];
static uint32   _soc_nsp_macsec_en_pbmp[SOC_MAX_NUM_SWITCH_DEVICES];

/*
 *  Function : drv_northstarplus_dev_prop_get
 *
 *  Purpose :
 *      Get the device property information
 *
 *  Parameters :
 *      unit        :   unit id
 *      prop_type   :   property type
 *      prop_val     :   property value of the property type
 *
 *  Return :
 *      SOC_E_NONE      :   success
 *      SOC_E_PARAM    :   parameter error
 *
 *  Note :
 *      This function is to get the device porperty information.
 *
 */
int 
drv_northstarplus_dev_prop_get(int unit, uint32 prop_type, uint32 *prop_val)
{
    uint32 reg_val = 0, temp = 0;
    
    switch (prop_type) {
        case DRV_DEV_PROP_MCAST_NUM:
            *prop_val = DRV_NORTHSTARPLUS_MCAST_GROUP_NUM;
            break;
        case DRV_DEV_PROP_AGE_TIMER_MAX_S:
            *prop_val = DRV_NORTHSTARPLUS_AGE_TIMER_MAX;
            break;
        case DRV_DEV_PROP_TRUNK_NUM:
            *prop_val = DRV_NORTHSTARPLUS_TRUNK_GROUP_NUM;
            break;
        case DRV_DEV_PROP_TRUNK_MAX_PORT_NUM:
            *prop_val = DRV_NORTHSTARPLUS_TRUNK_MAX_PORT_NUM;
            break;
        case DRV_DEV_PROP_COSQ_NUM:
            *prop_val = DRV_NORTHSTARPLUS_COS_QUEUE_NUM;
            break;
        case DRV_DEV_PROP_MSTP_NUM:
            *prop_val = DRV_NORTHSTARPLUS_MSTP_GROUP_NUM;
            break;
        case DRV_DEV_PROP_SEC_MAC_NUM_PER_PORT:
            *prop_val = DRV_NORTHSTARPLUS_SEC_MAC_NUM_PER_PORT;
            break;
        case DRV_DEV_PROP_COSQ_MAX_WEIGHT_VALUE:
            *prop_val = DRV_NORTHSTARPLUS_COS_QUEUE_MAX_WEIGHT_VALUE;
            break;
        case DRV_DEV_PROP_AUTH_PBMP:
            *prop_val = DRV_NORTHSTARPLUS_AUTH_SUPPORT_PBMP;
            break;
        case DRV_DEV_PROP_RATE_CONTROL_PBMP:
            *prop_val = DRV_NORTHSTARPLUS_RATE_CONTROL_SUPPORT_PBMP;
            break;
        case DRV_DEV_PROP_VLAN_ENTRY_NUM:
            *prop_val = DRV_NORTHSTARPLUS_VLAN_ENTRY_NUM;
            break;
        case DRV_DEV_PROP_BPDU_NUM:
            *prop_val = DRV_NORTHSTARPLUS_BPDU_NUM;
            break;
        case DRV_DEV_PROP_CFP_TCAM_SIZE:
            *prop_val = DRV_NORTHSTARPLUS_CFP_TCAM_SIZE;
            break;
        case DRV_DEV_PROP_CFP_UDFS_NUM:
            *prop_val = DRV_NORTHSTARPLUS_CFP_UDFS_NUM;
            break;
         case DRV_DEV_PROP_CFP_UDFS_OFFSET_MAX:
            *prop_val = DRV_NORTHSTARPLUS_CFP_UDFS_OFFSET_MAX;
            break;
        case DRV_DEV_PROP_AUTH_SEC_MODE:
            *prop_val = DRV_NORTHSTARPLUS_AUTH_SEC_MODE;
            break;            
        case DRV_DEV_PROP_AGE_HIT_VALUE:
            *prop_val = 0x1;
            break;
        case DRV_DEV_PROP_EEE_GLOBAL_CONG_THRESH:
            SOC_IF_ERROR_RETURN(
                REG_READ_EEE_GLB_CONG_THr(unit, &reg_val));
            soc_EEE_GLB_CONG_THr_field_get(unit, &reg_val, 
                GLB_CONG_THf, &temp);
            *prop_val = temp;
            break;
        case DRV_DEV_PROP_EEE_PIPELINE_TIMER:
            SOC_IF_ERROR_RETURN(
                REG_READ_EEE_PIPELINE_TIMERr(unit, &reg_val));
            soc_EEE_PIPELINE_TIMERr_field_get(unit, &reg_val, 
                PIPELINE_TIMERf, &temp);
            *prop_val = temp;
            break;
        case DRV_DEV_PROP_RESOURCE_ARBITER_REQ:
            SOC_IF_ERROR_RETURN(
                REG_READ_CPU_RESOURCE_ARBITERr(unit, &reg_val));
            soc_CPU_RESOURCE_ARBITERr_field_get(unit, &reg_val, 
                        EXT_CPU_REQf, &temp);
            if (temp) {
                *prop_val = TRUE;
            } else {
                *prop_val = FALSE;
            }
            break;
        case DRV_DEV_PROP_SUPPORTED_LED_FUNCTIONS:
            *prop_val = DRV_LED_FUNC_ALL_MASK & 
                    ~(DRV_LED_FUNC_SP_100_200 |  DRV_LED_FUNC_100_200_ACT | 
                    DRV_LED_FUNC_LNK_ACT_SP);
            break;
            
        case DRV_DEV_PROP_LOW_POWER_SUPPORT_PBMP:
            *prop_val = DRV_NORTHSTARPLUS_MAC_LOW_POWER_SUPPORT_PBMP;
            break;
        case DRV_DEV_PROP_INTERNAL_CPU_PORT_NUM:
            /* Port 8 connects to ARM processor */
            *prop_val = 8;
            break;
        case DRV_DEV_PROP_ADDITIONAL_SOC_PORT_NUM:
            /* Port 7 connects to SOC */
            *prop_val = 7;
            break;
        case DRV_DEV_PROP_IMP1_PORT_NUM:
            /* Port 5 can be used for IMP1 */
            *prop_val = 5;
            break;
        case DRV_DEV_PROP_PPPOE_SESSION_ETYPE:
            SOC_IF_ERROR_RETURN(
                REG_READ_PPPOE_SESSION_PARSE_ENr(unit, &reg_val));
            
            soc_PPPOE_SESSION_PARSE_ENr_field_get(unit, &reg_val, 
                PPPOE_SESSION_ETYPEf , &temp);
            *prop_val = temp;
            break;
        case DRV_DEV_PROP_SWITCHMACSEC_EN_PBMP:
            *prop_val = _soc_nsp_macsec_en_pbmp[unit];
            soc_cm_debug(DK_ERR, 
                    "%s,%d,DRV_DEV_PROP_SWITCHMACSEC_EN_PBMP=0x%x\n", 
                    FUNCTION_NAME(), __LINE__, *prop_val);
            break;
        case DRV_DEV_PROP_SWITCHMACSEC_ATTACH_PBMP:
            *prop_val = _soc_nsp_macsec_attach_pbmp[unit];
            soc_cm_debug(DK_ERR, 
                    "%s,%d,DRV_DEV_PROP_SWITCHMACSEC_ATTACH_PBMP=0x%x\n", 
                    FUNCTION_NAME(), __LINE__, *prop_val);
            break;
        default:
            return SOC_E_PARAM;
    }
    return SOC_E_NONE;
}

/*
 *  Function : drv_northstarplus_dev_prop_set
 *
 *  Purpose :
 *     Set the device property information
 *
 *  Parameters :
 *      unit        :   unit id
 *      prop_type   :   property type
 *      prop_val     :   property value of the property type
 *
 *  Return :
 *      SOC_E_UNAVAIL 
 *
 *  Note :
 *      This function is to set the device porperty information.
 *
 */
int 
drv_northstarplus_dev_prop_set(int unit, uint32 prop_type, uint32 prop_val)
{
    uint32 reg_val = 0, temp = 0;
    int rv = SOC_E_NONE;
    int i;
    
    switch (prop_type) {
        case DRV_DEV_PROP_EEE_GLOBAL_CONG_THRESH:
            if (prop_val > 512) {
                return SOC_E_CONFIG;
            }
            SOC_IF_ERROR_RETURN(
                REG_READ_EEE_GLB_CONG_THr(unit, &reg_val));
            temp = prop_val;
            soc_EEE_GLB_CONG_THr_field_set(unit, &reg_val, 
                GLB_CONG_THf, &temp);
            SOC_IF_ERROR_RETURN(
                REG_WRITE_EEE_GLB_CONG_THr(unit, &reg_val));
            break;
        case DRV_DEV_PROP_EEE_PIPELINE_TIMER:
            /* 16-bit length */
            if (prop_val > 65536) {
                return SOC_E_CONFIG;
            }
            SOC_IF_ERROR_RETURN(
                REG_READ_EEE_PIPELINE_TIMERr(unit, &reg_val));
            temp = prop_val;
            soc_EEE_PIPELINE_TIMERr_field_set(unit, &reg_val, 
                PIPELINE_TIMERf, &temp);
            SOC_IF_ERROR_RETURN(
                REG_WRITE_EEE_PIPELINE_TIMERr(unit, &reg_val));
            break;
        case DRV_DEV_PROP_RESOURCE_ARBITER_REQ:
            if (!soc_feature(unit, soc_feature_int_cpu_arbiter)) {
                return SOC_E_UNAVAIL;
            }
            SOC_IF_ERROR_RETURN(
                REG_READ_CPU_RESOURCE_ARBITERr(unit, &reg_val));
            sal_mutex_take(
                SOC_CONTROL(unit)->arbiter_mutex, sal_mutex_FOREVER);
            if (prop_val) {
                if (SOC_CONTROL(unit)->arbiter_lock_count == 0) {
                    /* Require an arbiter grant */
                    temp = 1;
                    soc_CPU_RESOURCE_ARBITERr_field_set(unit, &reg_val, 
                        EXT_CPU_REQf, &temp);
                    rv = REG_WRITE_CPU_RESOURCE_ARBITERr(unit, &reg_val);
                    if (SOC_FAILURE(rv)) {
                        sal_mutex_give(SOC_CONTROL(unit)->arbiter_mutex);
                        return rv;
                    }
 
                    /* wait for grant */
                    do {
                        rv = REG_READ_CPU_RESOURCE_ARBITERr(unit, &reg_val);
                        if (SOC_FAILURE(rv)) {
                            sal_mutex_give(SOC_CONTROL(unit)->arbiter_mutex);
                            return rv;
                        }
                        soc_CPU_RESOURCE_ARBITERr_field_get(unit, &reg_val, 
                            EXT_CPU_GNTf, &temp);
                    } while (temp == 0);
                }
                SOC_CONTROL(unit)->arbiter_lock_count++;
                
            } else {
                /* Release the arbiter grant */
                SOC_CONTROL(unit)->arbiter_lock_count--;
                if (SOC_CONTROL(unit)->arbiter_lock_count == 0) {
                    temp = 0;
                    soc_CPU_RESOURCE_ARBITERr_field_set(unit, &reg_val, 
                        EXT_CPU_REQf, &temp);
                    rv = REG_WRITE_CPU_RESOURCE_ARBITERr(unit, &reg_val);
                    if (SOC_FAILURE(rv)) {
                        sal_mutex_give(SOC_CONTROL(unit)->arbiter_mutex);
                        return rv;
                    }
                }
            }
            sal_mutex_give(SOC_CONTROL(unit)->arbiter_mutex);
            break;
        case DRV_DEV_PROP_LOW_POWER_ENABLE:
            if (prop_val) {
                /* Save the current EEE configuration */
                rv = REG_READ_EEE_EN_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                soc_EEE_EN_CTRLr_field_get(unit, &reg_val, EN_EEEf, &temp);
                bcm53020_eee_enable_config[unit] = temp;
                /* Disable EEE */
                reg_val = 0;
                rv = REG_WRITE_EEE_EN_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                /* Disable IMP port */
                rv = REG_READ_IMP_CTLr(unit, CMIC_PORT(unit), &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                temp = 1;
                soc_IMP_CTLr_field_set(unit, &reg_val, TX_DISf, &temp);
                soc_IMP_CTLr_field_set(unit, &reg_val, RX_DISf, &temp);
                rv = REG_WRITE_IMP_CTLr(unit, CMIC_PORT(unit), &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                /* 
                  * Slow down the system clock  
                  *     and shut down MAC clocks for port 5 and port 8 
                  */
                rv = REG_READ_LOW_POWER_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                temp = 0x1;
                soc_LOW_POWER_CTRLr_field_set(unit, &reg_val, 
                    EN_LOW_POWERf, &temp);
                soc_LOW_POWER_CTRLr_field_set(unit, &reg_val, 
                    SLEEP_P5f, &temp);
                soc_LOW_POWER_CTRLr_field_set(unit, &reg_val, 
                    SLEEP_P4f, &temp);
                soc_LOW_POWER_CTRLr_field_set(unit, &reg_val, 
                    SLEEP_SYSf, &temp);
                switch (prop_val) {
                    case 12500:
                        temp = 0;
                        break;
                    case 6250:
                        temp = 1;
                        break;
                    case 4170:
                        temp = 2;
                        break;
                    case 3125:
                        temp = 3;
                        break;
                    default:
                        return SOC_E_PARAM;
                }
                soc_LOW_POWER_CTRLr_field_set(unit, &reg_val, 
                    LOW_POWER_DIVIDERf, &temp);
                rv = REG_WRITE_LOW_POWER_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                /* 4. Show down PLL */
                rv = REG_READ_PLL_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                temp = 0x1f;
                soc_PLL_CTRLr_field_set(unit, &reg_val, 
                    PLL_LP_CTRLf, &temp);
                rv = REG_WRITE_PLL_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

            } else {
                /* 1. Bring back PLL */
                rv = REG_READ_PLL_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                temp = 0x1;
                soc_PLL_CTRLr_field_set(unit, &reg_val, 
                    PLL_ARESETf, &temp);
                soc_PLL_CTRLr_field_set(unit, &reg_val, 
                    PLL_DRESETf, &temp);
                temp = 0;
                soc_PLL_CTRLr_field_set(unit, &reg_val, 
                    PLL_LP_CTRLf, &temp);
                rv = REG_WRITE_PLL_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                
                reg_val = 0x0;
                rv = REG_WRITE_PLL_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }
                
                /* 2. Bring back system and MAC clocks */
                reg_val = 0;
                rv = REG_WRITE_LOW_POWER_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                /* Disable IMP port */
                rv = REG_READ_IMP_CTLr(unit, CMIC_PORT(unit), &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                temp = 0;
                soc_IMP_CTLr_field_set(unit, &reg_val, TX_DISf, &temp);
                soc_IMP_CTLr_field_set(unit, &reg_val, RX_DISf, &temp);
                rv = REG_WRITE_IMP_CTLr(unit, CMIC_PORT(unit), &reg_val);
                if (SOC_FAILURE(rv)) {
                    return rv;
                }

                /* Restore the configuration of EEE */
                reg_val = bcm53020_eee_enable_config[unit];
                /* if AN is disabled, disable the EEE as well */
                PBMP_ITER(PBMP_GE_ALL(unit), i) {
                    rv = DRV_PORT_GET(unit, i, DRV_PORT_PROP_AUTONEG, &temp);
                    if (SOC_FAILURE(rv)) {
                        INT_MCU_UNLOCK(unit);
                        return rv;
                    }
                    if (!temp) {
                        reg_val &= ~(0x1 << i);
                    } else {
                        reg_val |= (0x1 << i);
                    }
                }
                rv = REG_WRITE_EEE_EN_CTRLr(unit, &reg_val);
                if (SOC_FAILURE(rv)) {
                    INT_MCU_UNLOCK(unit);
                    return rv;
                }

            }
            break;
        case DRV_DEV_PROP_PPPOE_SESSION_ETYPE:
            SOC_IF_ERROR_RETURN(
                REG_READ_PPPOE_SESSION_PARSE_ENr(unit, &reg_val));
            temp = prop_val;
            soc_PPPOE_SESSION_PARSE_ENr_field_set(unit, &reg_val, 
                PPPOE_SESSION_ETYPEf , &temp);
            SOC_IF_ERROR_RETURN(
                REG_WRITE_PPPOE_SESSION_PARSE_ENr(unit, &reg_val));
            break;
        case DRV_DEV_PROP_SWITCHMACSEC_SW_INIT:
            soc_cm_debug(DK_ERR, "%s,%d,DRV_DEV_PROP_SWITCHMACSEC_SW_INIT..(en=%d)\n", 
                    FUNCTION_NAME(), __LINE__, prop_val);
            _soc_nsp_macsec_en_pbmp[unit] = 0x0;    /* default is bypass (not enabled) */
            _soc_nsp_macsec_attach_pbmp[unit] = 
                    DRV_NORTHSTARPLUS_MACSEC_PBMP;

#if defined(INCLUDE_MACSEC)
            /* request MACSEC default bypass is enabled or disabled 
             *
             *  Note : 
             *  - port 4 and port 5 in NSP Spec. support MACSEC feature.
             *  - device default bypass is enabled !
             */
            prop_val = (prop_val) ? TRUE : FALSE;
            SOC_IF_ERROR_RETURN(
                    soc_robo_macsec_bypass_set(unit, 4, prop_val));
            SOC_IF_ERROR_RETURN(
                    soc_robo_macsec_bypass_set(unit, 5, prop_val));
#endif  /* INCLUDE_MACSEC */
            break;
        case DRV_DEV_PROP_SWITCHMACSEC_EN_PBMP:
            _soc_nsp_macsec_en_pbmp[unit] = 
                    prop_val & DRV_NORTHSTARPLUS_MACSEC_PBMP;
            soc_cm_debug(DK_ERR, 
                    "%s,%d,DRV_DEV_PROP_SWITCHMACSEC_EN_PBMP=0x%x\n", 
                    FUNCTION_NAME(), __LINE__, _soc_nsp_macsec_en_pbmp[unit]);
            break;
        case DRV_DEV_PROP_SWITCHMACSEC_ATTACH_PBMP:
            _soc_nsp_macsec_attach_pbmp[unit] = 
                    prop_val & DRV_NORTHSTARPLUS_MACSEC_PBMP;
            soc_cm_debug(DK_ERR, 
                    "%s,%d,DRV_DEV_PROP_SWITCHMACSEC_ATTACH_PBMP=0x%x\n", 
                    FUNCTION_NAME(), __LINE__, _soc_nsp_macsec_en_pbmp[unit]);
            break;
        default:
            return SOC_E_UNAVAIL;
    }
    return SOC_E_NONE;
}

