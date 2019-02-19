/* 
 * $Id:$
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
 * File:        portctrl.h
 * Purpose:     SDK SOC Port Control Glue Layer
 */

#ifndef   _SOC_ESW_PORTCTRL_H_
#define   _SOC_ESW_PORTCTRL_H_

#include <soc/feature.h>
#include <soc/property.h>
#include <soc/phyctrl.h>
#include <soc/error.h>


#define SOC_USE_PORTCTRL(_unit)                 \
    (soc_feature(_unit, soc_feature_portmod))

/*
 * The following definitions allow for easy enhancements and adjustments
 * in the Port Control module in case the PortMod interface changes.
 */

/*
 * Define:
 *      PORTMOD_SUCCESS
 *      PORTMOD_FAILURE
 *      PORTMOD_IF_ERROR_RETURN
 * Purpose:
 *      Checks PortMod and PhyMod library error return code.
 *      PORTMOD_IF_ERROR_RETURN() causes routine to return on error.
 * Note:
 *      Currently, the PortMod functions return error codes are the same as
 *      the SHARED error codes, which are used by the BCM and SOC layers.
 *      Phymod functions error codes are also the same as the
 *      SHARED error codes.
 */
#define PORTMOD_SUCCESS(_rv)           _SHR_E_SUCCESS(_rv)
#define PORTMOD_FAILURE(_rv)           _SHR_E_FAILURE(_rv)
#define PORTMOD_IF_ERROR_RETURN(_op)   _SHR_E_IF_ERROR_RETURN(_op)

/* Port type used in PortMod functions */
typedef int portctrl_pport_t;


extern int
soc_portctrl_software_deinit(int unit);

extern int
soc_portctrl_led_chain_config(int unit, int port, int value);

extern char *
soc_portctrl_phy_name_get(int unit, int port);

extern int
soc_portctrl_port_mode_set(int unit, int port, int mode);
extern int
soc_portctrl_port_mode_get(int unit, int port, int *mode, int *lanes);

extern int
soc_portctrl_phy_control_set(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 value);
extern int
soc_portctrl_phy_control_get(int unit, soc_port_t port, int phyn,
                             int phy_lane, int sys_side,
                             soc_phy_control_t phy_ctrl, uint32 *value);

extern int
soc_esw_portctrl_encap_get(int unit, soc_port_t port, int *mode);

extern int
soc_esw_portctrl_port_to_phyaddr(int unit, soc_port_t port, uint8* phy_addr);
#endif /* _SOC_ESW_PORTCTRL_H_ */

