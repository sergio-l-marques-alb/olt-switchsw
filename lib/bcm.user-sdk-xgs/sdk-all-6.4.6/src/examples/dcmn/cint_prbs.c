
/*
 * $Id: cint_prbs.c,v 1.1.2.2 Broadcom SDK $
 *
 * $Copyright: Copyright 2015 Broadcom Corporation.
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
 * Running PRBS
 */

/*
 * Run PRBS over PHY/MAC loopback  
 *  
 */
int run_lb_prbs(int unit, int port, int is_mac)
{
    int rv, lb_orig, prbs_status, prbs_mode_val;
    bcm_port_config_t config;
    
    bshell(unit, "counter off");

    
    bcm_port_config_get(unit, &config);
    if (BCM_PBMP_MEMBER(config.sfi, port)){
        /*Get default prbs mode*/
        rv = bcm_port_control_get(unit, port, bcmPortControlPrbsMode, &prbs_mode_val);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port, rv);
            return rv;
        }
    }

    /*Get previous loopback*/
    rv = bcm_port_loopback_get(unit, port, &lb_orig);
    if(rv != 0) {
        printf("Error, in bcm_port_loopback_get, rv=%d, \n", rv);
        return rv;
    }

    /*Enable PHY loopback*/
    rv = bcm_port_loopback_set(unit, port, BCM_PORT_LOOPBACK_PHY);
    if(rv != 0) {
        printf("Error, in bcm_port_loopback_set 1, rv=%d, \n", rv);
        return rv;
    }

    if (BCM_PBMP_MEMBER(config.sfi, port)){
        /*Set MAC/PHY PRBS*/
        if (is_mac) {
            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsMode, 1);
        } else {
            rv = bcm_port_control_set(unit, port, bcmPortControlPrbsMode, 0);
        }
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port, rv);
            return rv;
        }
    }

    /*Start PRBS*/
    rv = bcm_port_control_set(unit, port, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, rv=%d, \n", rv);
        return rv;
    }

    /*relevant only for phy loopback*/
    if (!is_mac) {
        rv = bcm_port_control_set(unit, port, bcmPortControlPrbsTxInvertData, 1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsTxInvertData, rv=%d, \n", rv);
            return rv;
        }
    }

    rv = bcm_port_control_set(unit, port, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, rv=%d, \n", rv);
        return rv;
    }
    
    /*Clear prbs status*/
    rv = bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus, rv=%d, \n", rv);
        return rv;
    }

    bshell(unit, "sleep");

    /*Check status*/
    rv = bcm_port_control_get(unit, port, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus, rv=%d, \n", rv);
        return rv;
    }

    /*Stop PRBS*/
    rv = bcm_port_control_set(unit, port, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable (stop prbs), rv=%d, \n", rv);
        return rv;
    }

    rv = bcm_port_control_set(unit, port, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable (stop prbs), rv=%d, \n", rv);
        return rv;
    }

   /*relevant only for phy loopback*/
    if (!is_mac) {
        rv = bcm_port_control_set(unit, port, bcmPortControlPrbsTxInvertData, 0);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsTxInvertData (stop prbs), rv=%d, \n", rv);
            return rv;
        }
    }

    /*Return loopback to original state*/
    rv = bcm_port_loopback_set(unit, port, lb_orig);
    if(rv != 0) {
        printf("Error, in bcm_port_loopback_set 2, rv=%d, \n", rv);
        return rv;
    }

    
    if (BCM_PBMP_MEMBER(config.sfi, port)){
        /*Restore default prbs mode*/
        rv = bcm_port_control_get(unit, port, bcmPortControlPrbsMode, &prbs_mode_val);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port, rv);
            return rv;
        }
    }

    printf("PRBS status: %d\n",prbs_status);

    if(prbs_status == 0 || prbs_status == -2) {
        printf("run_lb_prbs: PASS\n");
    }

    return 0;
};

/*
 * Run PHY PRBS over external loopback
 */
int run_phy_ext_lb_prbs(int unit, int port1, int port2, bcm_port_prbs_t poly)
{
    int prbs_status, rv, prbs_mode_val_1, prbs_mode_val_2;
    bcm_port_config_t config;
    
    bcm_port_config_get(unit, &config);
    if (BCM_PBMP_MEMBER(config.sfi, port1)){
        /*Get default prbs mode*/
        rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsMode, &prbs_mode_val_1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
        /*Set PHY PRBS*/
        rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsMode, 0);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
    }
    
    if (BCM_PBMP_MEMBER(config.sfi, port2)){
        rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsMode, &prbs_mode_val_2);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
        rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsMode, 0);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port2, rv);
            return rv;
        }
    }

    /*Set Polynom*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsPolynomial, poly);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsPolynomial, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsPolynomial, poly);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsPolynomial, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    /*Start PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxInvertData, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxInvertData, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxInvertData, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxInvertData, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }

    /*Clear status*/
    rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus - clear status, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus - clear status, port %d, rv=%d, \n", port2, rv);
        return rv;
    }

    bshell(unit, "sleep");

    /*CHECK status*/
    rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    printf("PRBS status port %d: %d\n", port1, prbs_status);

    rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    printf("PRBS status port %d: %d\n", port2, prbs_status);

    /*Stop PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }


    if (BCM_PBMP_MEMBER(config.sfi, port1)){
        /*Restore default prbs mode*/
        rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsMode, &prbs_mode_val_1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
    }

    if (BCM_PBMP_MEMBER(config.sfi, port2)){
        rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsMode, &prbs_mode_val_2);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
    }


    if(prbs_status == 0 || prbs_status == -2) {
        printf("run_phy_ext_lb_prbs: PASS\n");
    }


    return 0;

}

/*
 * Run MAC PRBS over external loopback
 */
int run_mac_ext_lb_prbs(int unit, int port1, int port2)
{
    int prbs_status, rv, prbs_mode_val_1, prbs_mode_val_2;
    bcm_port_config_t config;
    
    bcm_port_config_get(unit, &config);
    if (BCM_PBMP_MEMBER(config.sfi, port1)){
        /*get default prbs mode values*/
        rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsMode, &prbs_mode_val_1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
        /*Set MAC PRBS*/
        rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsMode, 1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
    }

    if (BCM_PBMP_MEMBER(config.sfi, port2)){
        rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsMode, &prbs_mode_val_2);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port2, rv);
            return rv;
        }
        rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsMode, 1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port2, rv);
            return rv;
        }
    }

    /*Start PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }

    /*Clear status*/
    rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus - clear status, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus - clear status, port %d, rv=%d, \n", port2, rv);
        return rv;
    }

    bshell(unit, "sleep");

    /*CHECK status*/
    rv = bcm_port_control_get(unit, port1, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    printf("PRBS status port %d: %d\n", port1, prbs_status);

    rv = bcm_port_control_get(unit, port2, bcmPortControlPrbsRxStatus, &prbs_status);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxStatus, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    printf("PRBS status port %d: %d\n", port2, prbs_status);

    /*Stop PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }


    if (BCM_PBMP_MEMBER(config.sfi, port1)){
        /*restore default prbs mode values*/
        rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsMode, prbs_mode_val_1);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
            return rv;
        }
    }

    if (BCM_PBMP_MEMBER(config.sfi, port2)){
        rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsMode, prbs_mode_val_2);
        if(rv != 0) {
            printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port2, rv);
            return rv;
        }
    }

    if(prbs_status == 0 || prbs_status == -2) {
        printf("run_mac_ext_lb_prbs: PASS\n");
    }


    return 0;

}


/*
 * Start PHY PRBS over external loopback without stopping
 */
int start_phy_ext_lb_prbs(int unit, int port1, int port2, bcm_port_prbs_t poly)
{
    int prbs_status, rv, prbs_mode_val_1, prbs_mode_val_2;
    
    /*Set PHY PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsMode, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsMode, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port2, rv);
        return rv;
    }

    /*Set Polynom*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsPolynomial, poly);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsPolynomial, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsPolynomial, poly);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsPolynomial, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    /*Start PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
   

    return 0;    
}

/*
 * Start MAC PRBS over external loopback without stopping
 */
int start_mac_ext_lb_prbs(int unit, int port1, int port2, bcm_port_prbs_t poly)
{
    int prbs_status, rv, prbs_mode_val_1, prbs_mode_val_2;
    

    /*Stop PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 0);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }


    /*Set MAC PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsMode, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port1, rv);
        return rv;
    }

    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsMode, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsMode, port %d, rv=%d, \n", port2, rv);
        return rv;
    }

    /*Start PRBS*/
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsRxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsRxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port1, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port1, rv);
        return rv;
    }
    rv = bcm_port_control_set(unit, port2, bcmPortControlPrbsTxEnable, 1);
    if(rv != 0) {
        printf("Error, in bcmPortControlPrbsTxEnable, port %d, rv=%d, \n", port2, rv);
        return rv;
    }
   

    return 0;    
}



