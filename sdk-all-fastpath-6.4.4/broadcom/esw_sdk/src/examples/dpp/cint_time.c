/*~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~TIME~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*
 * $Id$
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
 * File: cint_time.c
 * Purpose: bcm_time_capture_get BCM API use example
 *
 * Before using examples in this CINT, one need to:
 * -----------------------------------------------:
 * 1) Enable TS_PLL clock by enabling following soc property:
 *      phy_1588_dpll_frequency_lock.BCM88650=1
 *
 * 2) Setup a broad sync (BS) system (requires dedication BS physical configuration). 
 *
 * 3) Initialize ARM ukernel firmware:
 *
 *      BS master configuration :
 *      ---------------------  
 *      mcsload 0 /firmware/BCM88650_A0_0.srec initmcs=1
 *      mcsload 1 /firmware/BCM88650_A0_1bs.srec
 *      bs config master bc=10000000 hb=4000
 *      bs debug on;sleep 5;bs debug off
 *      bs status
 *
 *      BS slave configuration :
 *      ---------------------  
 *      mcsload 0 /firmware/BCM88650_A0_0.srec initmcs=1
 *      mcsload 1 /firmware/BCM88650_A0_1bs.srec
 *      bs config bc=10000000 hb=4000
 *      bs debug on;sleep 5;bs debug off
 *      bs status
 *
 *
 * Provides the following functionalities:
 * - get ARAD BS time of day 
 * - Time interface traverse/delete/heartbeat_enable_get. 
 *
 *
 *
 */
/* 
 * how to run:
 *
 *
cint ../../../../src/examples/dpp/cint_time.c
cint
int rv;
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;
sleep 1
cint
rv = get_time(0, 0); 
exit;

cint
rv = interface_traverse(0); 
print rv;
rv = heartbeat_enable_get(0, 0); 
print rv;
rv = interface_delete(0, 0); 
print rv;
exit;

 */ 

/* 
   interface_id - the BS time interface id that is internally created when calling following BCM shell command :
      "bs config [master] bc=10000000 hb=4000" ("bs config" internally calls bcm_time_interface_add )
    
   unit - unit id of SLAVE device ! (in case 'unit' is master device, 'bcm_time_capture_get' initializes 'time' to zero)
*/


int get_time(int unit, int interface_id)
{
    int    rv = BCM_E_NONE;
    bcm_time_capture_t time = {0};
    bcm_time_interface_t interface;

    interface.id = interface_id;

    rv = bcm_time_capture_get(unit, interface.id, &time);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_capture_get rv = %d\n", rv);
        return rv;
    }


    printf("BS ToD:   %d.%09d \n", COMPILER_64_LO(time.received.seconds),   time.received.nanoseconds);

    return rv;
}

int traverse_cb(int unit, 
                bcm_time_interface_t *intf, 
                void *user_data)
{
    int *data= user_data;

    printf("traverse_cb: unit %d intf_id %d flags 0x%08x user_data %d\n", 
           unit, intf->id, intf->flags, *data);

}

int interface_traverse(int unit)
{
    int  rv = BCM_E_NONE;
    int user_data = 3;

    rv = bcm_time_interface_traverse(unit, traverse_cb, &user_data);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_interface_traverse rv = %d\n", rv);
        return rv;
    }

    return rv;
}


int heartbeat_enable_get(int unit, int interface_id)
{
    int  rv = BCM_E_NONE;
    int enable;

    rv = bcm_time_heartbeat_enable_get(unit, interface_id, &enable);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_heartbeat_enable_get rv = %d\n", rv);
        return rv;
    }

    printf("enable = %d\n", enable);

    return rv;
}


int interface_delete(int unit, int interface_id)
{
    int  rv = BCM_E_NONE;
    int enable;

    rv = bcm_time_interface_delete(unit, interface_id);
    if (rv != BCM_E_NONE) {
        printf("Error, bcm_time_interface_delete rv = %d\n", rv);
        return rv;
    }

    printf("interface %d deleted successfully\n", interface_id);

    return rv;
}
