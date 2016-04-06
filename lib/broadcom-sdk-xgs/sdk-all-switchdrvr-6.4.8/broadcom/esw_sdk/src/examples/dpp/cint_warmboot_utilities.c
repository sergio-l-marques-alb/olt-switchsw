/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Warmboot~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $Id: cint_warmboot_utilities.c,v 1.11 Broadcom SDK $
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
 * File: cint_warmboot_utilities.c
 * Purpose: Example of how to save and restore Software Databases
 *
 * Provides the following functionalities
 *
 * - Sync device state
 *   Application can sync the state. Useful for providing software upgrade functionality.
 *   Entire device state is synched.
 *   
 * - Setup device in a mode to sync relevant state only on configuration changes.
 *   This mode will provide the path to recovery from system crashes.
 *
 * - Release all driver resources (device continues to operate)
 *   Primarily useful for managed software upgrades.
 *   Could also be used by test scripts that do not cleanup the previous state
 *   (although this is not the primariy purpose of this API. It just aids in
 *   overcoming test limitation on cleanup).
 *
 * The following SOC propreties need to be setup warmboot/coldboot
 *  - stable_location=3
 *  - stable_filename=./warmboot_data
 *  - stable_size=1000000
 *  - stable_flags=0
 * 
 * The following environment variable needs to be setup for warmboot
 *  - setenv SOC_BOOT_FLAGS 0x200000
 * 
 * The following environment variable needs to be setup for coldboot
 *  - setenv SOC_BOOT_FLAGS 0x000000
 * 
 */


int
wb_sync_initiate(int unit)
{
    int                           rc = BCM_E_NONE;


    rc = bcm_switch_control_set(unit, bcmSwitchControlSync, 1);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_set-bcmSwitchControlSync, failed to sync, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

int
wb_immediate_sync_mode_set(int unit, int is_enable)
{
    int                           rc = BCM_E_NONE;


    if ( (is_enable != TRUE) && (is_enable != FALSE) )  {
        return(BCM_E_PARAM);
    }

    rc = bcm_switch_control_set(unit, bcmSwitchControlAutoSync, is_enable);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_set-bcmSwitchControlAutoSync, failed to setup sync mode, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

int
wb_immediate_sync_mode_get(int unit, int *is_enable)
{
    int                           rc = BCM_E_NONE;


    if (is_enable == NULL) {
        return(BCM_E_PARAM);
    }

    rc = bcm_switch_control_get(unit, bcmSwitchControlAutoSync, is_enable);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_switch_control_get-bcmSwitchControlAutoSync, failed to retreive sync mode, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

int
wb_deinit_init(int unit, int sync)
{
    int                           rc = BCM_E_NONE;

    if (sync) {
        bshell(unit, "tr 141 Warmboot=1");
    }
    else {

        bshell(unit, "tr 141 NoSync Warmboot=1");

    }
    return(rc);
}

/*
 * NOTE: The following routine is not complete as all symbols are not exported to CINT.
 *       It is best to execute the "EXIT" command from the Broadcom CLI.
 */
int
driver_graceful_resource_release(int unit)
{
    int                           rc = BCM_E_NONE;


    /* make sure unit is attached */
    rc = bcm_attach_check(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_attach_check failure, rc= %d.\n", rc);
        return(rc);
    }

/*
    rc = _bcm_shutdown(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in _bcm_shutdown, rc= %d.\n", rc);
        return(rc);
    }

    rc = soc_shutdown(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in _soc_shutdown, rc= %d.\n", rc);
        return(rc);
    }
*/

    /* detach device */
    /* Incomplete, added since the above symbols are not exposed to CINT */
    rc = bcm_detach(unit);
    if (rc != BCM_E_NONE) {
        printf("ERROR: in bcm_detach, rc= %d.\n", rc);
        return(rc);
    }

    return(rc);
}

