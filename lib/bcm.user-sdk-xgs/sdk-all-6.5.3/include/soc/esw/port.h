/* 
 * $Id:$
 * $Copyright: Copyright 2016 Broadcom Corporation.
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
 * File:        port.h
 * Purpose:     SOC Port definitions.
 */

#ifndef   _SOC_ESW_PORT_H_
#define   _SOC_ESW_PORT_H_


/*
 * Internal flags
 *
 *   SOC_PORT_RESOURCE_I_MAP      Indicates legacy FlexPort API is used.
 *                                In this case, when flexing to a fewer
 *                                number of ports (4x10 -> 1x40), the 'old'
 *                                ports become inactive rather than
 *                                being detached and destroyed (which is
 *                                what happens with the new multi_set API).
 *                                To be used only by legacy API.
 *
 *   SOC_PORT_RESOURCE_INACTIVE   Indicates if port is inactive or active.
 *                                To be used only by legacy API.
 *
 *   SOC_PORT_RESOURCE_ATTACH     The port needs to go through the attach
 *                                sequence.
 *
 *   SOC_PORT_RESOURCE_DETACH     The port needs to go through the detach
 *                                sequence.
 *
 *   SOC_PORT_RESOURCE_NEW        A new logical port, which previously
 *                                didn't exist, is added as a result of
 *                                a FlexPort operation.
 *
 *   SOC_PORT_RESOURCE_DESTROY    The logical port is deleted and
 *                                no longer exists in the system after
 *                                FlexPort.
 *
 *   SOC_PORT_RESOURCE_REMAP      The logical port existed before FlexPort
 *                                but is mapped to a different physical port
 *                                after FlexPort.
 *   SOC_PORT_RESOURCE_SPEED      Only speed is changed. Logical-physical mapping,
 *                                lanes, encap remain the same
 */
#define SOC_PORT_RESOURCE_I_MAP      (1 << 31)
#define SOC_PORT_RESOURCE_INACTIVE   (1 << 30)
#define SOC_PORT_RESOURCE_ATTACH     (1 << 29)
#define SOC_PORT_RESOURCE_DETACH     (1 << 28)
#define SOC_PORT_RESOURCE_NEW        (1 << 27)
#define SOC_PORT_RESOURCE_DESTROY    (1 << 26)
#define SOC_PORT_RESOURCE_REMAP      (1 << 25)
#define SOC_PORT_RESOURCE_SPEED      (1 << 24)


/* Maximum number of lanes that a port can have */
#define  SOC_PORT_RESOURCE_LANES_MAX    12

/*
 * Lane Information
 *
 * Contains information on a given lane for a port.
 *
 * A port that uses 'x' number of lanes will have 'x' soc_port_lane_info_t
 * elements, one for each of the lanes that the port will use.
 */
typedef struct soc_port_lane_info_s {
    int pgw;           /* PGW instance where lane resides */
    int xlp;           /* XLP number within PGW */
    int tsc;           /*  TSC number (Same as XLP) */
    int port_index;    /* Index within XLP */
} soc_port_lane_info_t;


/*
 * Port Resource
 *
 * Contains port resource configuration used for FlexPort operations.
 *
 */
typedef struct soc_port_resource_s {
    uint32 flags;
    int logical_port;       /* Logical port associated to physical port */
    int physical_port;      /* Physical port associated logical port */
    int mmu_port;           /* MMU port associated to logical port */
    int pipe;               /* Pipe number of the port */
    int speed;              /* Initial speed after FlexPort operation */
    int mode;               /* Port mode: single, dual, quad, tri012, ... */
    int num_lanes;          /* Number of PHY lanes */
    soc_port_lane_info_t *lane_info[SOC_PORT_RESOURCE_LANES_MAX];
                            /* Lane information array */
    soc_encap_mode_t encap; /* Encapsulation mode for port */
    int oversub;            /* Indicates if port has oversub enabled */
    uint16 prio_mask;       /* Packet priority to priority group mapping mask */
} soc_port_resource_t;


#endif /* _SOC_ESW_PORT_H_ */

