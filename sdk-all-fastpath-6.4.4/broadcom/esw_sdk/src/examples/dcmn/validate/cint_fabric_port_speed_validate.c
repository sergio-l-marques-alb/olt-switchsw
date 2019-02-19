/*  
 * $Id: cint_fabric_port_speed_validate.c,v 1.3 Broadcom SDK $
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
 * this cint validates that port speed shown by "phy measure" is as expected (with error percentage, must be in [0,100])
 */

int fabric_port_speed_validate(int unit,int port, float expected_rate, int error_percentage) 
{
    int rc;
    uint32 actual_rate_int, actual_remainder_int;
    float error_value, bound_high, bound_low, rate_received, remainder_float;

    if (error_percentage < 0 || error_percentage > 100)
    {
        printf("Error in cint_fabric_port_speed_validate, error percentage must be in the interval [0,100]\n");
        return -1;
    }
    error_value = expected_rate * (error_percentage/100.0);
    bound_high = expected_rate + error_value;
    bound_low  = expected_rate - error_value;

    /* Get actual value of speed */
    rc = diag_dcmn_phy_measure_port(unit, port, 0, &actual_rate_int, &actual_remainder_int);
    if (rc != 0)
    {
        printf("Error in cint_fabric_port_speed_validate, diag_dcmn_phy_measure_port failed");
        return -1;
    }
    /* Conver actual_rate, actual_remainder to a float with the value actual_rate_int.actual_remainder_int */
    remainder_float = 0;
    while (actual_remainder_int)
    {
        remainder_float /= 10;
        remainder_float += (actual_remainder_int%10)/10.0;
        actual_remainder_int /= 10;
    }
    rate_received = actual_rate_int + remainder_float;
    rate_received *= 1000; 

    if (rate_received > bound_high || rate_received < bound_low)
    {
        printf("Error in cint_fabric_port_speed_validate, rate recieved is %f , while expected rate is %f, and error percentage is %d", rate_received, expected_rate, error_percentage);
        return -1;
    }

    return 0;
}




