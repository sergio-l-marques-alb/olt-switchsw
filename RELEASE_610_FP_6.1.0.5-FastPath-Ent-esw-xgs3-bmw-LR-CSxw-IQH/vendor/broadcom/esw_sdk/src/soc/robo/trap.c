/*
 * $Id: trap.c,v 1.1 2011/04/18 17:11:08 mruas Exp $
 *
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
 */
 #include <soc/drv.h>
 #include <soc/debug.h>

/*
 *  Function : drv_trap_set
 *
 *  Purpose :
 *      Set the trap frame type to CPU.
 *
 *  Parameters :
 *      unit        :   unit id
 *      bmp   :   port bitmap.
 *      trap_mask   :   the mask of trap type.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_trap_set(int unit, soc_pbmp_t bmp, uint32 trap_mask)
{
    int     rv = SOC_E_NONE;
    uint32  reg_addr, reg_value, temp;
    int     reg_len;
    soc_port_t port = 0;

    soc_cm_debug(DK_VERBOSE, 
    "drv_trap_set: unit = %d, trap mask= %x\n",
        unit, trap_mask);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, GMNGCFGr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, GMNGCFGr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    if (trap_mask & DRV_SWITCH_TRAP_IGMP) {    
        temp = 1;
    } else {
        temp = 0;
        if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
            if (trap_mask & DRV_SWITCH_TRAP_MLD) {
                temp = 1;
            }
        }
    }

    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
        /*
         * temp =  1b'01: IGMP forward to IMP.
         *         1b'11: IGMP forward to original pbmp and IMP.
         */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, GMNGCFGr, &reg_value, IGMP_IP_CHKf, &temp));
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
         /* temp =  1b'01: IGMP/MLD forward to IMP.
                  1b'11: IGMP/MLD forward to original pbmp and IMP. */
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, GMNGCFGr, &reg_value, IGMP_MLD_CHKf, &temp));
    } else {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, GMNGCFGr, &reg_value, IGMPIP_SNOP_ENf, &temp));
    }

    if (trap_mask & DRV_SWITCH_TRAP_BPDU1) {    
        temp = 1;
    } else {
        temp = 0;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, GMNGCFGr, &reg_value, RXBPDU_ENf, &temp));    
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    
    if (trap_mask & DRV_SWITCH_TRAP_8021X) {
        temp = 1;
    } else {
        temp = 0;
    }
    if(SOC_IS_ROBO5324(unit)||SOC_IS_ROBO5348(unit) || 
        SOC_IS_ROBO5347(unit)) {
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, SPECIAL_MNGTr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, SPECIAL_MNGTr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, SPECIAL_MNGTr, &reg_value, EN_SEC_DROP2MIIf, &temp));
        if ((rv = (DRV_SERVICES(unit)->reg_write)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
    } else if(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
        PBMP_ITER(bmp, port) {
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, PORT_SEC_CONr);
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, PORT_SEC_CONr, port, 0);
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }

            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, PORT_SEC_CONr, &reg_value, SA_VIO_OPTf, &temp));

            if ((rv = (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }
        }
    } else if(SOC_IS_ROBODINO(unit)|| SOC_IS_ROBO5398(unit)||
                SOC_IS_ROBO5397(unit)){
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    
    if (trap_mask & DRV_SWITCH_TRAP_IPMC) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    if (trap_mask & DRV_SWITCH_TRAP_GARP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    if (trap_mask & DRV_SWITCH_TRAP_ARP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    } 
    if (trap_mask & DRV_SWITCH_TRAP_8023AD) {
        rv = SOC_E_UNAVAIL;
        return rv;
    } 
    if (trap_mask & DRV_SWITCH_TRAP_ICMP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    if (trap_mask & DRV_SWITCH_TRAP_BPDU2) {
        rv = SOC_E_UNAVAIL;
        return rv;
    } 
    if (trap_mask & DRV_SWITCH_TRAP_RARP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    } 
    if (trap_mask & DRV_SWITCH_TRAP_8023AD_DIS) {
        rv = SOC_E_UNAVAIL;
        return rv;
    } 
    if (trap_mask & DRV_SWITCH_TRAP_BGMP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    } 
    if (trap_mask & DRV_SWITCH_TRAP_LLDP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }

    /* Enable IP_Multicast */
    if (trap_mask & DRV_SWITCH_TRAP_IGMP) {
        if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit) ||
            SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            /* No IP Multicast for 5348 / 5347 / 53242*/
        } else {
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, NEW_CONTROLr);
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, NEW_CONTROLr, 0, 0);
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }
            temp = 1;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, NEW_CONTROLr, &reg_value, IP_MULTICASTf, &temp));
            if ((rv = (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }
        }
    }
    /* Broadcast packet */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MII_PCTLr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MII_PCTLr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    if (trap_mask & DRV_SWITCH_TRAP_BCST) {
        temp = 1;
    } else {
        temp = 0;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
        (unit, MII_PCTLr, &reg_value, MIRX_BC_ENf, &temp));
    if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    
    return rv;
}

/*
 *  Function : drv_trap_get
 *
 *  Purpose :
 *      Get the trap frame type to CPU.
 *
 *  Parameters :
 *      unit        :   unit id
 *      port   :   port id.
 *      trap_mask   :   the mask of trap type.
 *      mac     :   mac address.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_trap_get(int unit, soc_port_t port, uint32 *trap_mask)
{
    int     rv = SOC_E_NONE;
    uint32  reg_addr, reg_value, temp;
    int     reg_len;

    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, GMNGCFGr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, GMNGCFGr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    if (SOC_IS_ROBO5348(unit) || SOC_IS_ROBO5347(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, GMNGCFGr, &reg_value, IGMP_IP_CHKf, &temp));
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, GMNGCFGr, &reg_value, IGMP_MLD_CHKf, &temp));
    } else {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, GMNGCFGr, &reg_value, IGMPIP_SNOP_ENf, &temp));
    }
    if (temp) {
        *trap_mask |= DRV_SWITCH_TRAP_IGMP;
        if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            *trap_mask |= DRV_SWITCH_TRAP_MLD;
        }
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, GMNGCFGr, &reg_value, RXBPDU_ENf, &temp));
    if (temp) {
        *trap_mask |= DRV_SWITCH_TRAP_BPDU1;
    }
    if(SOC_IS_ROBO5324(unit)||SOC_IS_ROBO5348(unit)||
        SOC_IS_ROBO5347(unit)) {
        reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, SPECIAL_MNGTr);
        reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, SPECIAL_MNGTr, 0, 0);
        if ((rv = (DRV_SERVICES(unit)->reg_read)
            (unit, reg_addr, &reg_value, reg_len)) < 0) {
            return rv;
        }
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, SPECIAL_MNGTr, &reg_value, EN_SEC_DROP2MIIf, &temp));
        if (temp) {
            *trap_mask |= DRV_SWITCH_TRAP_8021X;
        }
    } else if(SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)){
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, PORT_SEC_CONr);
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, PORT_SEC_CONr, port, 0);
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }

            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, PORT_SEC_CONr, &reg_value, SA_VIO_OPTf, &temp));
            if (temp) {
                *trap_mask |= DRV_SWITCH_TRAP_8021X;
            }
    }

    /* Broadcast packet */
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, MII_PCTLr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, MII_PCTLr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
        (unit, MII_PCTLr, &reg_value, MIRX_BC_ENf, &temp));
    if (temp) {
        *trap_mask |= DRV_SWITCH_TRAP_BCST;
    }

    soc_cm_debug(DK_VERBOSE, 
    "drv_trap_get: unit = %d, trap mask= %x\n",
        unit, *trap_mask);
    return rv;
}

/*
 *  Function : drv_snoop_set
 *
 *  Purpose :
 *      Set the Snoop type.
 *
 *  Parameters :
 *      unit        :   unit id
 *      snoop_mask   :   the mask of snoop type.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_snoop_set(int unit, uint32 snoop_mask)
{
    int     rv = SOC_E_NONE;
    uint32  reg_addr, reg_value, temp;
    int     reg_len;

    soc_cm_debug(DK_VERBOSE, 
    "drv_snoop_set: unit = %d, snoop mask= %x\n",
        unit, snoop_mask);
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, GMNGCFGr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, GMNGCFGr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    if (snoop_mask & DRV_SNOOP_IGMP) {    
        temp = 1;
        if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            temp = 3;
        }
    } else {
        temp = 0;
    }

    if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, GMNGCFGr, &reg_value, IGMP_IP_CHKf, &temp));
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, GMNGCFGr, &reg_value, IGMP_MLD_CHKf, &temp));
    } else {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
            (unit, GMNGCFGr, &reg_value, IGMPIP_SNOP_ENf, &temp));
    }
     if ((rv = (DRV_SERVICES(unit)->reg_write)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    /* enable IP_Multicast */
    if (snoop_mask & DRV_SNOOP_IGMP) {
        if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit) ||
            SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
            /* No IP Multicast for 5348 / 5347 / 53242*/
        } else {
            reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, NEW_CONTROLr);
            reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, NEW_CONTROLr, 0, 0);
            if ((rv = (DRV_SERVICES(unit)->reg_read)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }
            temp = 1;
            SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_set)
                (unit, NEW_CONTROLr, &reg_value, IP_MULTICASTf, &temp));
            if ((rv = (DRV_SERVICES(unit)->reg_write)
                (unit, reg_addr, &reg_value, reg_len)) < 0) {
                return rv;
            }
        }
    }
    if (snoop_mask & DRV_SNOOP_ARP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    if (snoop_mask & DRV_SNOOP_RARP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    if (snoop_mask & DRV_SNOOP_ICMP) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
    if (snoop_mask & DRV_SNOOP_ICMPV6) {
        rv = SOC_E_UNAVAIL;
        return rv;
    }
     
    return rv; 
     
}

/*
 *  Function : drv_snoop_get
 *
 *  Purpose :
 *      Get the Snoop type.
 *
 *  Parameters :
 *      unit        :   unit id
 *      snoop_mask   :   the mask of snoop type.
 *
 *  Return :
 *      SOC_E_XXX
 *
 *  Note :
 *      
 *
 */
int 
drv_snoop_get(int unit, uint32 *snoop_mask)
{
    int     rv = SOC_E_NONE;
    uint32  reg_addr, reg_value, temp;
    int     reg_len;
    
    reg_len = (DRV_SERVICES(unit)->reg_length_get)(unit, GMNGCFGr);
    reg_addr = (DRV_SERVICES(unit)->reg_addr)(unit, GMNGCFGr, 0, 0);
    if ((rv = (DRV_SERVICES(unit)->reg_read)
        (unit, reg_addr, &reg_value, reg_len)) < 0) {
        return rv;
    }
    if (SOC_IS_ROBO5348(unit)||SOC_IS_ROBO5347(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, GMNGCFGr, &reg_value, IGMP_IP_CHKf, &temp));
    } else if (SOC_IS_ROBO53242(unit)||SOC_IS_ROBO53262(unit)) {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, GMNGCFGr, &reg_value, IGMP_MLD_CHKf, &temp));
    } else {
        SOC_IF_ERROR_RETURN((DRV_SERVICES(unit)->reg_field_get)
            (unit, GMNGCFGr, &reg_value, IGMPIP_SNOP_ENf, &temp));
    }
    if (temp) {
       *snoop_mask = DRV_SNOOP_IGMP;
    }
    soc_cm_debug(DK_VERBOSE, 
    "drv_snoop_get: unit = %d, snoop mask= %x\n",
        unit, *snoop_mask);
    return rv; 
}
