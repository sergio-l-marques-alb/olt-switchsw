/* 
 * $Id:$
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
 * File:        port.c
 * Purpose:     TD2+ SOC Port driver.
 *
 *              Contains information and interfaces for the
 *              physical port in the device.
 */

#include <shared/bsl.h>
#include <soc/defs.h>

#if defined(BCM_TRIDENT2PLUS_SUPPORT)
#include <soc/drv.h>
#include <soc/error.h>
#include <soc/td2_td2p.h>
#include <soc/trident2.h>
#include <soc/esw/portctrl.h>


/*
 * Physical Port Information
 *
 *     Some information are defined in HW and others are defined
 *     through SOC properties during configuration time.
 */
typedef struct td2p_phy_port_s {
    /*
     * The following fields are defined in HW
     */
    soc_port_lane_info_t lane_info;    /* Lane information in PGW block */
    uint32               lanes_valid;  /* Lanes capabilities */
    int                  pipe;
    int                  sisters_ports[_TD2_PORTS_PER_XLP];
                                       /* Ports part of the XLPORT/XLMAC/TSC-4
                                          where the physical port resides */

    /*
     * The following fields are configurable throught SOC properties
     */
    int                  oversub;
    uint16               prio_mask;    /* Packet priority to priority
                                          group mapping mask */
} td2p_phy_port_t;

/*
 * Physical Device information
 */
typedef struct td2p_info_s {
    td2p_phy_port_t  phy_port[SOC_MAX_NUM_PORTS];
    int              phy_ports_max;   /* Max physical ports in device */
    uint32           speed_valid[SOC_PORT_RESOURCE_LANES_MAX+1];
                                      /* Port rate ability on serdes lane */
    int              speed_max;       /* Max speed on any port in device */
    int              ports_pipe_max;  /* Max logical ports per pipe */
    int              mmu_lossless;    /* MMU lossless */
} td2p_info_t;


/*
 * Contains information corresponding to each physical port.
 * This information is fixed in a device and is calculated
 * only during init time.
 */
static td2p_info_t                  *td2p_info[SOC_MAX_NUM_DEVICES];

#define TD2P_INFO(_u)               (td2p_info[(_u)])
#define TD2P_PHY_PORT(_u, _p)       (TD2P_INFO(_u)->phy_port[(_p)])
#define TD2P_PHY_PORT_LANE(_u, _p)  (TD2P_PHY_PORT(_u, _p).lane_info)


/* Lanes support */
#define TD2P_PHY_PORT_LANES_1      (1 << 0)
#define TD2P_PHY_PORT_LANES_2      (1 << 1)
#define TD2P_PHY_PORT_LANES_4      (1 << 2)
#define TD2P_PHY_PORT_LANES_10     (1 << 3)
#define TD2P_PHY_PORT_LANES_12     (1 << 4)

/* Port Rate support */
/* Ethernet */
#define TD2P_PHY_PORT_RATE_1       (1 << 0)
#define TD2P_PHY_PORT_RATE_2_5     (1 << 1)
#define TD2P_PHY_PORT_RATE_10      (1 << 2)
#define TD2P_PHY_PORT_RATE_20      (1 << 3)
#define TD2P_PHY_PORT_RATE_40      (1 << 4)
#define TD2P_PHY_PORT_RATE_100     (1 << 5)
/* HG */
#define TD2P_PHY_PORT_RATE_11      (1 << 6)
#define TD2P_PHY_PORT_RATE_21      (1 << 7)
#define TD2P_PHY_PORT_RATE_42      (1 << 8)
#define TD2P_PHY_PORT_RATE_106     (1 << 9)
#define TD2P_PHY_PORT_RATE_127     (1 << 10)


/* Default for the max port speed configured for device */
#define TD2P_FLEX_SPEED_MAX_DEFAULT    42000


/*
 * Define:
 *      TD2P_INFO_INIT_CHECK
 * Purpose:
 *      Causes a routine to return SOC_E_INIT if module is not yet initialized.
 */
#define TD2P_INFO_INIT_CHECK(_u) \
    if (td2p_info[_u] == NULL) { return SOC_E_INIT; }

/*
 * Define:
 *      TD2P_PHY_PORT_ADDRESSABLE
 * Purpose:
 *      Checks that physical port is addressable (within valid range).
 */
#define TD2P_PHY_PORT_ADDRESSABLE(_u, _phy_port)                  \
    (soc_td2p_phy_port_addressable((_u), (_phy_port)) == SOC_E_NONE)


/*
 * Function:
 *      _soc_td2p_phy_port_lane_info_init
 * Purpose:
 *      Initialize the physical port lane information in the
 *      SW device information data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_phy_port_lane_info_init(int unit)
{
    int phy_port;
    int phy_port_base;
    int i;
    int blk;
    int bindex;
    int pgw;
    int xlp;

    /* Set information to invalid */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        TD2P_PHY_PORT(unit, phy_port).pipe = -1;
        TD2P_PHY_PORT_LANE(unit, phy_port).pgw = -1;
        TD2P_PHY_PORT_LANE(unit, phy_port).xlp = -1;
        TD2P_PHY_PORT_LANE(unit, phy_port).port_index = -1;
    }

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {

        /* Check for end of port list */
        if (SOC_PORT_BLOCK(unit, phy_port) < 0 &&
            SOC_PORT_BINDEX(unit, phy_port) < 0) {
            break;
        }

        /* Set pipe information */
        TD2P_PHY_PORT(unit, phy_port).pipe =
            phy_port / TD2P_PHY_PORTS_PER_PIPE;

        /* Skip CPU and Loopback ports */
        if ((phy_port == TD2P_PHY_PORT_CPU) ||
            (phy_port == TD2P_PHY_PORT_LB)) {
            continue;
        }

        /* Set lane information */
        for (i = 0; i < SOC_DRIVER(unit)->port_num_blktype; i++) {
            /* Find PGW block */
            blk = SOC_PORT_IDX_BLOCK(unit, phy_port, i);
            bindex = SOC_PORT_IDX_BINDEX(unit, phy_port, i);

            if ((blk < 0) || (bindex < 0)) {
                continue;
            }

            /* Set PGW block information for lane */
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_PGW_CL) {
                /* Get PGW, XLP and Port Index values */
                TD2P_PHY_PORT_LANE(unit, phy_port).pgw =
                    SOC_BLOCK_NUMBER(unit, blk);
                TD2P_PHY_PORT_LANE(unit, phy_port).xlp =
                    bindex / TD2P_PORTS_PER_XLP;
                TD2P_PHY_PORT_LANE(unit, phy_port).port_index =
                    bindex % TD2P_PORTS_PER_XLP;
            }

            /* Set port lane capabilites */
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_XLPORT) {
                TD2P_PHY_PORT(unit, phy_port).lanes_valid =
                    TD2P_PHY_PORT_LANES_1;
                if (bindex == 0) {
                    TD2P_PHY_PORT(unit, phy_port).lanes_valid |=
                        TD2P_PHY_PORT_LANES_2 | TD2P_PHY_PORT_LANES_4;
                } else if (bindex == 2) {
                    TD2P_PHY_PORT(unit, phy_port).lanes_valid |=
                        TD2P_PHY_PORT_LANES_2;
                }
            }
            if (SOC_BLOCK_TYPE(unit, blk) == SOC_BLK_CPORT) {
                TD2P_PHY_PORT(unit, phy_port).lanes_valid |=
                    TD2P_PHY_PORT_LANES_10 | TD2P_PHY_PORT_LANES_12;
            }

        } /* for port_num_blktype */

    } /* for phy_port */

    /* Set total number of physical ports in device */
    TD2P_INFO(unit)->phy_ports_max = phy_port;

    /*
     * Fill sisters ports
     * This logic assumes a given physical layout in the device
     */
    for (phy_port = 0; phy_port < TD2P_INFO(unit)->phy_ports_max;) {

        if (TD2P_PHY_PORT_LANE(unit, phy_port).pgw == -1) {
            phy_port++;
            continue;
        }

        pgw = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
        xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;
        phy_port_base = phy_port;
        while ((TD2P_PHY_PORT_LANE(unit, phy_port).pgw == pgw) &&
               (TD2P_PHY_PORT_LANE(unit, phy_port).xlp == xlp)) {
            for (i = 0; i < _TD2_PORTS_PER_XLP; i++) {
                TD2P_PHY_PORT(unit, phy_port).sisters_ports[i] =
                    phy_port_base + i;
            }
            phy_port++;
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_oversub_init
 * Purpose:
 *      Initialize the oversub physical port property in
 *      the SW device information data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_oversub_init(int unit)
{
    int phy_port;
    int port;
    int oversub;
    soc_info_t *si = &SOC_INFO(unit);

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip ports not addressable */
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
            continue;
        }

        oversub = 0;

        /*
         * Check per physical port property first.
         * If property is not available on physical port,
         * use property on logical port.
         */
        /*
         * TBD once SOC property is approved.
         * if (property_physical) {
         *     ...
         * } else {
         *     ... logical property
         * }
         */

        /*
         * Set based on logical port property
         * This information is already populated in si->oversub_pbm
         */
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            if (SOC_PBMP_MEMBER(si->oversub_pbm, port)) {
                oversub = 1;
            }
        }

        TD2P_PHY_PORT(unit, phy_port).oversub = oversub;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_prio_mask_init
 * Purpose:
 *      Initialize the packet priority to priority group mapping mask
 *      for physical ports in the SW device information data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_prio_mask_init(int unit)
{
    int phy_port;
    int port;
    uint16 prio_mask;
    soc_info_t *si = &SOC_INFO(unit);

    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip ports not addressable */
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port)) {
            continue;
        }

        prio_mask = 0xffff;
        /*
         * Check per physical port property first.
         * If property is not available on physical port,
         * use property on logical port.
         */

        /*
         * TBD once SOC property is approved.
         * if (property_physical) {
         *     ...
         * } else {
         *     ... logical property
         * }
         */

        /* Set based on logical port property */
        port = si->port_p2l_mapping[phy_port];
        if (port != -1) {
            SOC_IF_ERROR_RETURN
                (soc_trident2_get_prio_map(unit, port, &prio_mask));
        }

        TD2P_PHY_PORT(unit, phy_port).prio_mask = prio_mask;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_speed_valid_init
 * Purpose:
 *      Initialize the SW device information data structure
 *      for the valid port rates according to the number of serdes lanes.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_speed_valid_init(int unit)
{
    int lanes;
    uint32 speed_valid;

    /* Set port rate based on number of serdes lanes */
    for (lanes = 0; lanes <= SOC_PORT_RESOURCE_LANES_MAX; lanes++) {

        if (lanes == 1) {
            speed_valid =
                TD2P_PHY_PORT_RATE_1 |
                TD2P_PHY_PORT_RATE_2_5 |
                TD2P_PHY_PORT_RATE_10 |
                TD2P_PHY_PORT_RATE_11 ;

        } else if (lanes == 2) {
            speed_valid =
                TD2P_PHY_PORT_RATE_10 |
                TD2P_PHY_PORT_RATE_20 |
                TD2P_PHY_PORT_RATE_21 ;

        } else if (lanes == 4) {
            speed_valid =
                TD2P_PHY_PORT_RATE_10 |
                TD2P_PHY_PORT_RATE_20 |
                TD2P_PHY_PORT_RATE_40 |
                TD2P_PHY_PORT_RATE_11 |
                TD2P_PHY_PORT_RATE_42 ;

        } else if (lanes == 10) {
            speed_valid =
                TD2P_PHY_PORT_RATE_100 |
                TD2P_PHY_PORT_RATE_106;

        } else if (lanes == 12) {
            speed_valid =
                TD2P_PHY_PORT_RATE_127;

        } else {
            speed_valid = 0;
        }

        TD2P_INFO(unit)->speed_valid[lanes] = speed_valid;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_ports_pipe_max_init
 * Purpose:
 *      Initialize the SW device information data structure
 *      for the maximum allowed speed in device and
 *      maximum number of logical ports allowed per pipe.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes SW data structure memory is valid.
 *      - Assumes SOC information is initialized.
 */
STATIC int
_soc_td2p_ports_pipe_max_init(int unit)
{
    int port;
    int speed_max;
    int ports_max;
    soc_info_t *si = &SOC_INFO(unit);

    /* Get speed from SOC property */
    speed_max = soc_property_get(unit, spn_PORT_FLEX_SPEED_MAX,
                                 TD2P_FLEX_SPEED_MAX_DEFAULT);

    /* Compare with port speed given in 'portmap' configuration */
    for (port = 0; port < SOC_MAX_NUM_PORTS; port++) {
        if (si->port_speed_max[port] > speed_max) {
            speed_max = si->port_speed_max[port];
        }
    }

    /*
     * Maximum number of ports depends on the max speed and
     * the device SKU (core bandwidth).
     */
    if (speed_max > 42000) {
        ports_max = 50;
    } else {
        soc_info_t *si = &SOC_INFO(unit);
        switch (si->bandwidth) {
        case 960000:
            ports_max = 52;
            break;
        case 720000:
            ports_max = 48;
            break;
        case 480000:
        default:
            ports_max = 42;
            break;
        }
    }

    TD2P_INFO(unit)->speed_max = speed_max;
    TD2P_INFO(unit)->ports_pipe_max = ports_max;

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_mmu_lossless_init
 * Purpose:
 *      Initialize the SW device information data structure
 *      for the MMU lossless.
 * Parameters:
 *      unit      - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes SW data structure memory is valid.
 */
STATIC int
_soc_td2p_mmu_lossless_init(int unit)
{
    TD2P_INFO(unit)->mmu_lossless =
        soc_property_get(unit, spn_MMU_LOSSLESS, 1);

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_phy_info_init
 * Purpose:
 *      Initialize the main SW device information data structure.
 *
 *      It populates the information of the device that are relevant
 *      to the physical port interface such as,
 *      PGW, XLP, port index, lane capabilities, speed, etc.
 *
 *      Information is derived from the HW port design specification
 *      and configuration properties on a physical port.
 *
 *      This routine does not modify any HW.  It only
 *      modifies the SW data structure.
 * Parameters:
 *      unit     - (IN) Unit number.
 * Returns:
 *      SOC_E_XXX
 * Notes:
 *      - Assumes SOC Control information has been initialized.
 *      - Must be called after the TD2P port configuration init routine
 *        soc_trident2_port_config_init().
 */
int
soc_td2p_phy_info_init(int unit)
{
    int phy_port;    /* Physical port */
    int lanes;       /* Serdes lane */

    /* Allocate memory for main SW data structure */
    if (td2p_info[unit] == NULL) {
        td2p_info[unit] = sal_alloc(sizeof(td2p_info_t), "td2p_info");
        if (td2p_info[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }

    /* Clear data structure */
    sal_memset(td2p_info[unit], 0, sizeof(td2p_info_t));

    /* Initialize device information */
    SOC_IF_ERROR_RETURN(_soc_td2p_phy_port_lane_info_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_oversub_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_prio_mask_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_speed_valid_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_ports_pipe_max_init(unit));

    SOC_IF_ERROR_RETURN(_soc_td2p_mmu_lossless_init(unit));

    /* Debug output */
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Physical ports max: %d  speed_max=%d "
                            "ports_per_pipe_max=%d mmu_lossless=%d\n"),
                 TD2P_INFO(unit)->phy_ports_max,
                 TD2P_INFO(unit)->speed_max,
                 TD2P_INFO(unit)->ports_pipe_max,
                 TD2P_INFO(unit)->mmu_lossless));

    for (phy_port = 0; phy_port < TD2P_INFO(unit)->phy_ports_max; phy_port++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Phy port=%d pgw=%d xlp=%d index=%d "
                                "valid=0x%x pipe=%d "
                                "oversub=%d prio_mask=0x%x\n"),
                     phy_port,
                     TD2P_PHY_PORT_LANE(unit, phy_port).pgw,
                     TD2P_PHY_PORT_LANE(unit, phy_port).xlp,
                     TD2P_PHY_PORT_LANE(unit, phy_port).port_index,
                     TD2P_PHY_PORT(unit, phy_port).lanes_valid,
                     TD2P_PHY_PORT(unit, phy_port).pipe,
                     TD2P_PHY_PORT(unit, phy_port).oversub,
                     TD2P_PHY_PORT(unit, phy_port).prio_mask));
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "    sisters_ports={%d,%d,%d,%d}\n"),
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[0],
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[1],
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[2],
                     TD2P_PHY_PORT(unit, phy_port).sisters_ports[3]));
    }

    for (lanes = 0; lanes <= SOC_PORT_RESOURCE_LANES_MAX; lanes++) {
        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Serdes lane=%d port_rate=0x%x\n"),
                     lanes, TD2P_INFO(unit)->speed_valid[lanes]));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_speed_valid
 * Purpose:
 *      Check that given speed is valid for the number of
 *      serdes lanes.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      lanes    - (IN) Number of PHY lanes.
 *      speed    - (IN) Port rate to check.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_speed_valid(int unit, int lanes, int speed)
{
    uint32 speed_mask = 0;

    if (speed == 1000) {
        speed_mask = TD2P_PHY_PORT_RATE_1;
    } else if (speed == 2500) {
        speed_mask = TD2P_PHY_PORT_RATE_2_5;
    } else if (speed == 10000) {
        speed_mask = TD2P_PHY_PORT_RATE_10;
    } else if (speed == 20000) {
        speed_mask = TD2P_PHY_PORT_RATE_20;
    } else if (speed == 40000) {
        speed_mask = TD2P_PHY_PORT_RATE_40;
    } else if (speed == 100000) {
        speed_mask = TD2P_PHY_PORT_RATE_100;
    } else if (speed == 11000) {
        speed_mask = TD2P_PHY_PORT_RATE_11;
    } else if (speed == 21000) {
        speed_mask = TD2P_PHY_PORT_RATE_21;
    } else if (speed == 42000) {
        speed_mask = TD2P_PHY_PORT_RATE_42;
    } else if (speed == 106000) {
        speed_mask = TD2P_PHY_PORT_RATE_106;
    } else if (speed == 127000) {
        speed_mask = TD2P_PHY_PORT_RATE_127;
    } else {
        return SOC_E_CONFIG;
    }

    if (!(TD2P_INFO(unit)->speed_valid[lanes] & speed_mask)) {
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_phy_port_lanes_valid
 * Purpose:
 *      Check that given lane setting is valid for physical port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Base physical port.
 *      lanes    - (IN) Number of PHY lanes.
 * Returns:
 *      SOC_E_XXX
 */
STATIC int
_soc_td2p_phy_port_lanes_valid(int unit, int phy_port, int lanes)
{
    uint32 lane_mask = 0;

    if (lanes == 1) {
        lane_mask = TD2P_PHY_PORT_LANES_1;
    } else if (lanes == 2) {
        lane_mask = TD2P_PHY_PORT_LANES_2;
    } else if (lanes == 4) {
        lane_mask = TD2P_PHY_PORT_LANES_4;
    } else if (lanes == 10) {
        lane_mask = TD2P_PHY_PORT_LANES_10;
    } else if (lanes == 12) {
        lane_mask = TD2P_PHY_PORT_LANES_12;
    } else {
        return SOC_E_CONFIG;
    }

    if (!(TD2P_PHY_PORT(unit, phy_port).lanes_valid & lane_mask)) {
        return SOC_E_CONFIG;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_lanes_valid
 * Purpose:
 *      Check that given lane setting is valid for logical port.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      port     - (IN) Logical port.
 *      lanes    - (IN) Number of PHY lanes.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_port_lanes_valid(int unit, soc_port_t port, int lanes)
{
    soc_info_t *si = &SOC_INFO(unit);
    int phy_port;

    TD2P_INFO_INIT_CHECK(unit);

    phy_port = si->port_l2p_mapping[port];
    if (phy_port == -1) {
        return SOC_E_PORT;
    }

    return _soc_td2p_phy_port_lanes_valid(unit, phy_port, lanes);
}


/*
 * Function:
 *      soc_td2p_port_resource_speed_max_get
 * Purpose:
 *      Get the maximum allowed speed in the device.
 *
 *      This information is used to determine the total number of
 *      logical  ports allowed in the system as well as the
 *      pre-allocation of MMU port numbers.
 * Parameters:
 *      unit  - (IN) Unit number.
 *      speed - (OUT) Maximum speed of any port in device.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
soc_td2p_port_resource_speed_max_get(int unit, int *speed)
{
    TD2P_INFO_INIT_CHECK(unit);

    *speed = TD2P_INFO(unit)->speed_max;

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_phy_port_addressable
 * Purpose:
 *      Check that given physical port number is addressable.
 * Parameters:
 *      unit      - (IN) Unit number.
 *      phy_port  - (IN) Physical port number.
 * Returns:
 *      SOC_E_NONE - If physical port number is addressable
 *      SOC_E_PORT - If physical port number is not addressable.
 * NOTE:
 *      This routine only checks that the physical port number is
 *      within the valid range.  It does NOT check whether the physical
 *      port has a port mapping.
 */
int
soc_td2p_phy_port_addressable(int unit, int phy_port)
{
    TD2P_INFO_INIT_CHECK(unit);

    if ((phy_port < 0) ||
        (phy_port >= TD2P_INFO(unit)->phy_ports_max)) {
        return SOC_E_PORT;
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_phy_port_pgw_info_get
 * Purpose:
 *      Provide the PGW information for given physical port.
 * Parameters:
 *      unit        - (IN) Unit number.
 *      phy_port    - (IN) Physical port number.
 *      pgw         - (OUT) PGW instance where port resides.
 *      xlp         - (OUT) XLP number within PGW
 *      port_index  - (OUT) Port index within XLP 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_td2p_phy_port_pgw_info_get(int unit, int phy_port,
                               int *pgw, int *xlp, int *port_index)
{
    TD2P_INFO_INIT_CHECK(unit);

    if (phy_port == -1) {
        return SOC_E_PORT;
    }

    *pgw = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
    *xlp = TD2P_PHY_PORT_LANE(unit, phy_port).xlp;
    *port_index = TD2P_PHY_PORT_LANE(unit, phy_port).port_index;

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_logic_ports_max_validate
 * Purpose:
 *      Check that number of logical ports do not exceed maximum allowed
 *      based on physical port mappings provided in bitmap.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      phy_pbmp - (IN) Bitmap of physical ports.
 *                      A bit set indicates physical port is mapped.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
STATIC int
_soc_td2p_logic_ports_max_validate(int unit, pbmp_t phy_pbmp)
{
    int phy_port;
    int ports_pipe_max;
    int x_pipe_count = 0;
    int y_pipe_count = 0;

    /* Get maximum number of logical ports allowed per pipeline */
    ports_pipe_max = TD2P_INFO(unit)->ports_pipe_max;

    /* Check number of logical ports do not exceed maximum allowed */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip CPU, Loopback */
        if ((phy_port == TD2P_PHY_PORT_CPU) ||
            (phy_port == TD2P_PHY_PORT_LB)) {
            continue;
        }

        /* Skip physical ports not mapped */
        if (!SOC_PBMP_MEMBER(phy_pbmp, phy_port)) {
            continue;
        }

        if (TD2P_PHY_PORT(unit, phy_port).pipe == TD2P_X_PIPE) {
            x_pipe_count++;
        } else {
            y_pipe_count++;
        }
    }

    if ((x_pipe_count > ports_pipe_max) ||
        (y_pipe_count > ports_pipe_max)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Number of logical ports exceeds "
                              "max allowed: x_pipe_count=%d "
                              "y_pipe_count=%d max_pipe_count=%d\n"),
                   x_pipe_count, y_pipe_count, ports_pipe_max));

        return SOC_E_RESOURCE;
    }

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Number of logical ports: "
                            "x_pipe_count=%d y_pipe_count=%d "
                            "max_pipe_count=%d\n"),
                 x_pipe_count, y_pipe_count, ports_pipe_max));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_mapping_validate
 * Purpose:
 *      Validate:
 *      - Port numbers are available and mappings are not replicated.
 *      - Number of logical port numbers do not exceed max allowed.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Resource is not NULL.
 *      - Assumes array order is correct
 *        ('delete' operations are first in array).
 */
STATIC int
_soc_td2p_port_mapping_validate(int unit, 
                                int nport,
                                soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    pbmp_t phy_pbmp;
    pbmp_t logic_pbmp;
    int phy_port;
    int i;
    int lane;

    /*
     * Determine bitmaps of used/mapped logical ports and
     * physical ports.
     *
     * A bit set indicates port is mapped.
     */
    SOC_PBMP_CLEAR(phy_pbmp);
    SOC_PBMP_CLEAR(logic_pbmp);

    /* Get current port assignment */
    for (phy_port = 0; phy_port < SOC_MAX_NUM_PORTS; phy_port++) {
        /* Skip not-addressable and invalid ports */
        if (!TD2P_PHY_PORT_ADDRESSABLE(unit, phy_port) ||
            (si->port_p2l_mapping[phy_port] == -1)) {
            continue;
        }

        SOC_PBMP_PORT_ADD(logic_pbmp, si->port_p2l_mapping[phy_port]);
        SOC_PBMP_PORT_ADD(phy_pbmp, phy_port);
    }

    /* First 'delete' mappings */
    for (i = 0, pr = &resource[0];
         (i < nport) && (pr->physical_port == -1);
         i++, pr++) {
        phy_port = si->port_l2p_mapping[pr->logical_port];
        if (phy_port == -1) {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Logical port %d is not currently mapped\n"),
                       pr->logical_port));
            return SOC_E_CONFIG;
        }
        SOC_PBMP_PORT_REMOVE(logic_pbmp, pr->logical_port);
        SOC_PBMP_PORT_REMOVE(phy_pbmp, phy_port);
    }

    /* Continue with 'add' mappings in rest of array */
    for (; i < nport; i++, pr++) {
        /* Check that port number is 'available' */
        if (SOC_PBMP_MEMBER(logic_pbmp, pr->logical_port) ||
            SOC_PBMP_MEMBER(phy_pbmp, pr->physical_port)) {
            return SOC_E_BUSY;
        }

        SOC_PBMP_PORT_ADD(logic_pbmp, pr->logical_port);
        SOC_PBMP_PORT_ADD(phy_pbmp, pr->physical_port);
    }


    /* Check maximum number of logical ports allowed */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_logic_ports_max_validate(unit, phy_pbmp));


    /*
     * Lanes availability
     *
     * Check that lanes needed by the physical port are not
     * not being used by any other port.
     *
     * Assume physical port numbers are consecutive in device
     * with respect of the lanes.
     */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip delete entries */
        if (pr->physical_port == -1) {
            continue;
        }

        for (lane = 1; lane < pr->num_lanes; lane++) {
            /* Check lane is not being used */
            if (SOC_PBMP_MEMBER(phy_pbmp, pr->physical_port + lane)) {
                LOG_ERROR(BSL_LS_SOC_PORT,
                          (BSL_META_U(unit,
                                      "Port=%d physical_port=%d, lane=%d "
                                      "is being used\n"),
                                      pr->logical_port, pr->physical_port,
                                      lane));
                return SOC_E_BUSY;
            }
        }
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_validate
 * Purpose:
 *      Validate that the given FlexPort configuration is valid.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
STATIC int
_soc_td2p_port_resource_validate(int unit, int nport,
                                 soc_port_resource_t *resource)
{
    int i;
    soc_port_resource_t *pr;

    /*
     * Check lanes and speed assignment
     */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip delete entries */
        if (pr->physical_port == -1) {
            continue;
        }

        /* Check lane assignment */
        SOC_IF_ERROR_RETURN
            (_soc_td2p_phy_port_lanes_valid(unit, pr->physical_port,
                                            pr->num_lanes));
        /* Check speed (port rate) */
        SOC_IF_ERROR_RETURN
            (_soc_td2p_speed_valid(unit, pr->num_lanes, pr->speed));
    }

    /* Check port mappings */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_port_mapping_validate(unit, nport, resource));

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_cleanup
 * Purpose:
 *      Cleanup give data structure (deallocate memory).
 * Parameters:
 *      unit           - (IN) Unit number.
 *      pre_resource   - (IN/OUT) Memory to deallocate.
 * Returns:
 *      None
 * Note:
 */
STATIC void
_soc_td2p_port_resource_data_cleanup(soc_port_resource_t **pre_resource)
{
    if (*pre_resource != NULL) {
        sal_free(*pre_resource);
        *pre_resource = NULL;
    }

    return;
}


/*
 * Function:
 *      _soc_td2p_port_resource_mode_update
 * Purpose:
 *      Update the port mode given port resource configurations.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      If some ports are not present (inactive), routine will
 *      try to figure out best match.
 *      Assumes SOC_INFO has been updated.
 */
STATIC int
_soc_td2p_port_resource_mode_update(int unit,
                                    int nport, soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit); 
    soc_port_resource_t *pr;
    int i;
    int j;
    int logic_port;
    int phy_port;
    int sister_port;
    int num_lanes[_TD2_PORTS_PER_XLP];
    int one_active_lane = 0;
    int two_active_lane = 0;
    int four_active_lane = 0;
    char *modes_str[] = {"Quad", "Tri_012", "Tri_023", "Dual", "Single"};

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /* Skip delete entries (assume these to be first in array) */
        if (pr->physical_port == -1) {
            continue;
        }

        /* Get number of active lanes on sister ports */
        phy_port = pr->physical_port;
        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
            sister_port = TD2P_PHY_PORT(unit, phy_port).sisters_ports[j];
            logic_port = si->port_p2l_mapping[sister_port];
            if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
                num_lanes[j] = 0;
            } else {
                num_lanes[j] = si->port_num_lanes[logic_port];
            }
        }


        /*
         * Quad:    Lane0, Lane1, Lane2, Lane3 are active (individually)
         * Dual:    Lane0, Lane2, are active (Lane0 and Lane2 are dual)
         * Single:  Lane0 is active, one physical port using all lanes, 4/10/12
         * Tri_012: Lane0, Lane1, Lane2 are active (Lane2 is dual)
         * Tri_023: Lane0, Lane2, Lane3 are active (Lane0 is dual)
         *
         * If any lane is -1, make best guess.
         */

        /* Count active lanes */
        for (j = 0; j < _TD2_PORTS_PER_XLP; j++) {
            if (num_lanes[j] == 1) {
                one_active_lane++;
            } else if (num_lanes[j] == 2) {
                two_active_lane++;
            } else if (num_lanes[j] >= 4) {
                /* No more checking is needed */
                four_active_lane++;
                break;
            }
        }

        /* Get mode based on active lanes */
        if (four_active_lane) {
            pr->mode = SOC_TD2_PORT_MODE_SINGLE;
        } else if (one_active_lane && !two_active_lane) {
            pr->mode = SOC_TD2_PORT_MODE_QUAD;
        } else if (!one_active_lane && two_active_lane) {
            pr->mode = SOC_TD2_PORT_MODE_DUAL;
        } else if (one_active_lane && two_active_lane) {
            if (num_lanes[0] == 1) {
                pr->mode = SOC_TD2_PORT_MODE_TRI_012;
            } else {
                pr->mode = SOC_TD2_PORT_MODE_TRI_023;
            }
        } else {
            LOG_ERROR(BSL_LS_SOC_PORT,
                      (BSL_META_U(unit,
                                  "Invalid number of lanes "
                                  "logical_port=%d physical_port=%d "
                                  "num_lanes=%d\n"),
                       pr->logical_port, pr->physical_port, pr->num_lanes));
            return SOC_E_INTERNAL;
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "Port mode is %s for "
                                "logical_port=%d physical_port=%d\n"),
                     modes_str[pr->mode],
                     pr->logical_port, pr->physical_port));
    }

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_output
 * Purpose:
 *      Debug output for tiven port resource array.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      header         - (IN) Header outout string.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 * Returns:
 *      SOC_E_XXX
 */
STATIC void
_soc_td2p_port_resource_output(int unit, char *header,
                               int nport, soc_port_resource_t *resource)
{
    int i;
    int lane;
    soc_port_resource_t *pr;
    char *modes_str[] = {"Quad", "T012", "T023", "Dual", "Sngl"};
    char *mode_str;

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "%s\n"), header));

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit,
                            "Logical Physical MMU Pipe  "
                            "Speed Lanes Mode Ovs PriMsk Flags       "
                            "PGW XLP PIDX\n")));

    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        /*
         * This arrays used to determine the 'string' assumes
         * certain values on the Port Modes defined in soc_td2_port_mode_e.
         */
        if ((pr->mode < 0) ||  (pr->mode > 4)) {
            mode_str = "----";
        } else {
            mode_str = modes_str[pr->mode];
        }

        LOG_VERBOSE(BSL_LS_SOC_PORT,
                    (BSL_META_U(unit,
                                "  %3d     %3d    %3d  %s   "
                                "%6d  %2d   %4s  %1d  0x%4.4x 0x%8.8x"),
                     pr->logical_port, pr->physical_port,
                     pr->mmu_port, ((pr->pipe == TD2P_X_PIPE) ? "X" : "Y"),
                     pr->speed, pr->num_lanes, mode_str,
                     pr->oversub, pr->prio_mask, pr->flags));
        if (pr->num_lanes == 0) {
            LOG_VERBOSE(BSL_LS_SOC_PORT,
                        (BSL_META_U(unit,
                                    "  %2d  %2d  %2d\n"),
                         -1, -1, -1));
        } else {
            for (lane = 0; lane < pr->num_lanes; lane++) {
                if (lane > 0) {
                    LOG_VERBOSE(BSL_LS_SOC_PORT,
                                (BSL_META_U(unit, "%65s"), " "));
                }
                LOG_VERBOSE(BSL_LS_SOC_PORT,
                            (BSL_META_U(unit,
                                        "  %2d  %2d  %2d\n"),
                         pr->lane_info[lane]->pgw,
                         pr->lane_info[lane]->xlp,
                         pr->lane_info[lane]->port_index));
            }
        }
    }

    return;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_output
 * Purpose:
 *      Debug output.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 *      pre_count      - (IN) Array size for pre-FlexPort array.
 *      pre_resource   - (IN) Pre-FlexPort configuration array.
 *      post_count     - (IN) Array size for post-FlexPort array.
 *      post_resource  - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 */
STATIC void
_soc_td2p_port_resource_data_output(int unit,
                                    int nport, soc_port_resource_t *resource,
                                    int pre_count,
                                    soc_port_resource_t *pre_resource,
                                    int post_count,
                                    soc_port_resource_t *post_resource)
{
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_td2p_port_resource_output(unit,
                                   "=== FlexPort Port Resource Data ===",
                                   nport, resource);
    
    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_td2p_port_resource_output(unit,
                                   "=== Pre-FlexPort Port Resource Data ===",
                                   pre_count, pre_resource);

    LOG_VERBOSE(BSL_LS_SOC_PORT,
                (BSL_META_U(unit, "\n")));
    _soc_td2p_port_resource_output(unit,
                                   "=== Post-FlexPort Port Resource Data ===",
                                   post_count, post_resource);

    return;
}


/*
 * Function:
 *      _soc_td2p_port_resource_data_init
 * Purpose:
 *      Allocate and get pre and post FlexPort Port Resource
 *      information for given FlexPort configuration.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN/OUT) Port Resource FlexPort configuration.
 *      pre_count      - (OUT) Array size for pre-FlexPort array.
 *      pre_resource   - (OUT) Pre-FlexPort configuration array.
 *      post_count     - (OUT) Array size for post-FlexPort array.
 *      post_resource  - (OUT) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes data is Port Resource has been validated.
 *      - Assumes array order is correct (deletes are first in array).
 *      - Assumes pointer parameters are not null.
 *      - Assumes the following fields have been filled already in input
 *        SOC Port Resource data structure:
 *          flags
 *          logical_port
 *          physical_port
 *          speed
 *          num_lanes 
 *
 *
 * The pre-FlexPort and post-FlexPort operation arrays are filled
 * out as follows:
 *
 * Pre-FlexPort array
 *     Contains current information on ports to be modified after
 *     the FlexPort operation.  This should include ports whose
 *     mappings are the same, deleted, or remapped.
 * Post-FlexPort array
 *     Contains information on new configuration for
 *     ports that are active (present) after the FlexPort operation.
 *     This should include ports whose mappings that are
 *     the same, remapped or new.
 *
 * Example
 *
 *     Logical_port --> Physical_port
 *               L1 --> -1 (current mapped to P1)
 *               L2 --> -1 (current mapped to P2)
 *               L3 --> -1 (current mapped to P3)
 *               L1 --> P1 (same mapping)
 *               L3 --> P5 (remapped)
 *               L9 --> P9 (new port)
 *
 *     Pre-FlexPort array
 *               L1 --> P1 (same mapping)
 *               L2 --> P2 (deleted mapping)
 *               L3 --> P3 (remapped)
 *
 *     Post-FlexPort array
 *               L1 --> P1 (same mapping)
 *               L3 --> P5 (remapped)
 *               L9 --> P9 (new mapping)
 */
STATIC int
_soc_td2p_port_resource_data_init(int unit,
                                  int nport, soc_port_resource_t *resource,
                                  int *pre_count,
                                  soc_port_resource_t **pre_resource,
                                  int *post_count,
                                  soc_port_resource_t **post_resource,
                                  soc_td2p_info_t *pre_soc_info)
{
    int i;
    int lane;
    int num_lanes;
    int phy_port;
    int delete_count = 0;
    soc_port_resource_t *pr;
    soc_port_resource_t *pre_pr;
    soc_info_t *si = &SOC_INFO(unit);

    /* Init parameters */
    *pre_count = 0;
    *pre_resource = NULL;
    *post_count = 0;
    *post_resource = NULL;


    /*************************************
     * Fill main FlexPort information
     */

    /* Validate and get MMU port assignments */
    SOC_IF_ERROR_RETURN
        (soc_td2p_mmu_flexport_map_validate(unit, nport, resource));

    /* Get rest of SOC FlexPort information */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        pr->mode = -1;

        /* Skip delete entries (assume these to be first in array) */
        if (pr->physical_port == -1) {
            delete_count++;
            continue;
        }

        phy_port = pr->physical_port;
        pr->pipe = TD2P_PHY_PORT(unit, phy_port).pipe;
        pr->oversub = TD2P_PHY_PORT(unit, phy_port).oversub;
        pr->prio_mask = TD2P_PHY_PORT(unit, phy_port).prio_mask;

        /*
         * Get Lanes information
         * Assume lanes are arranged consecutively with respect to
         * the physical port number.
         */
        for (lane = 0; lane < pr->num_lanes; lane++) {
            pr->lane_info[lane] = &TD2P_PHY_PORT_LANE(unit, phy_port + lane);
        }
    }

    /* Set count for pre and post FlexPort arrays */
    *pre_count = delete_count;
    *post_count = nport - delete_count;


    /********************************
     * Pre-FlexPort array
     */
    /* Allocate memory */
    if ((*pre_count) > 0) {
        *pre_resource = sal_alloc(sizeof(soc_port_resource_t) * (*pre_count),
                                  "pre_port_resource");
        if (*pre_resource == NULL) {
            return SOC_E_MEMORY;
        }
        sal_memset(*pre_resource, 0,
                   sizeof(soc_port_resource_t) * (*pre_count));
    }
    /* Fill information */
    for (i = 0, pre_pr = *pre_resource, pr = &resource[0];
         i < *pre_count;
         i++, pre_pr++, pr++) {

        /* Assume physical port is valid */
        phy_port = si->port_l2p_mapping[pr->logical_port];

        pre_pr->flags = pr->flags;
        pre_pr->logical_port = pr->logical_port;
        pre_pr->physical_port = phy_port;
        pre_pr->mmu_port = si->port_p2m_mapping[phy_port];
        pre_pr->pipe = TD2P_PHY_PORT(unit, phy_port).pipe;
        pre_pr->num_lanes = si->port_num_lanes[pr->logical_port];
        pre_pr->oversub = TD2P_PHY_PORT(unit, phy_port).oversub;
        pre_pr->prio_mask = TD2P_PHY_PORT(unit, phy_port).prio_mask;
        SOC_IF_ERROR_RETURN
            (soc_esw_portctrl_speed_get(unit, pre_pr->logical_port,
                                        &pre_pr->speed));

        /* Set inactive flag to indicate port is disabled */
        if (SOC_PBMP_MEMBER(si->all.disabled_bitmap, pr->logical_port)) {
            pre_pr->flags |= SOC_PORT_RESOURCE_INACTIVE;
            pre_pr->mode = -1;
        } else {
            SOC_IF_ERROR_RETURN
                (soc_portctrl_port_mode_get(unit, pr->logical_port,
                                            &(pre_pr->mode), &num_lanes));
        }

        /*
         * Get Lanes information
         * Assume lanes are arranged consecutively with respect to
         * the physical port number.
         */
        for (lane = 0; lane < pre_pr->num_lanes; lane++) {
            pre_pr->lane_info[lane] =
                &TD2P_PHY_PORT_LANE(unit, phy_port + lane);
        }
    }


    /********************************
     * Post-FlexPort array
     *
     * Just use the main FlexPort array since this has
     * all the new information that is needed for the 'post' array.
     */
    if ((*post_count) > 0) {
        *post_resource = &resource[(*pre_count)];
    }


    /********************************
     * Pre-FlexPort SOC info data
     *
     * This contains partial information of the current
     * SOC information (pre-FlexPort) needed during later
     * stages of the FlexPort reconfiguration sequence.
     */
    sal_memset(pre_soc_info, 0, sizeof(*pre_soc_info));
    for (i = 0; i < SOC_MAX_NUM_PORTS; i++) {
        pre_soc_info->port_l2p_mapping[i] = si->port_l2p_mapping[i];
        pre_soc_info->port_p2l_mapping[i] = si->port_p2l_mapping[i];
        pre_soc_info->port_p2m_mapping[i] = si->port_p2m_mapping[i];
        pre_soc_info->port_m2p_mapping[i] = si->port_m2p_mapping[i];
        pre_soc_info->port_group[i] = si->port_group[i];
        pre_soc_info->port_speed_max[i] = si->port_speed_max[i];
        pre_soc_info->port_num_lanes[i] = si->port_num_lanes[i];
    }
    SOC_PBMP_ASSIGN(pre_soc_info->xpipe_pbm, si->xpipe_pbm);
    SOC_PBMP_ASSIGN(pre_soc_info->ypipe_pbm, si->ypipe_pbm);
    SOC_PBMP_ASSIGN(pre_soc_info->oversub_pbm, si->oversub_pbm);

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_soc_info_update
 * Purpose:
 *      Update the SOC INFO SW data structure with the FlexPort
 *      information.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 */
int
_soc_td2p_soc_info_update(int unit, int nport,
                          soc_port_resource_t *resource)
{
    soc_info_t *si = &SOC_INFO(unit);
    soc_port_resource_t *pr;
    int i;
    int logic_port;
    int phy_port;
    int pre_phy_port;
    int pre_mmu_port;

    /* Update mapping */
    for (i = 0, pr = &resource[0]; i < nport; i++, pr++) {
        logic_port = pr->logical_port;
        phy_port = pr->physical_port;
        pre_phy_port = si->port_l2p_mapping[logic_port];
        pre_mmu_port = si->port_p2m_mapping[pre_phy_port];

        /* Logical to physical mapping update */

        /*
         * Clear information for mappings that are:
         *   - deleted
         *   - same (logical to physical mapping are the same)
         *   - remapped (different port mapping assignment)
         */
        if (phy_port == -1) {

            /*
             * If 'inactive' flag, just add port to disabled bitmap.
             * This is part of the legacy API behavior.
             */
            if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
                SOC_PBMP_PORT_ADD(si->all.disabled_bitmap, logic_port);
                continue;
            }

            /* Port Mapping related information */
            si->port_l2p_mapping[logic_port] = phy_port;
            if (pre_phy_port != -1) {
                si->port_p2l_mapping[pre_phy_port] = -1;
                si->port_p2m_mapping[pre_phy_port] = -1;
            }
            if (pre_mmu_port != -1) {
                si->port_m2p_mapping[pre_mmu_port] = -1;
            }

            /* Pipeline */
            if (pre_phy_port != -1) {
                if (TD2P_PHY_PORT(unit, pre_phy_port).pipe == TD2P_X_PIPE) {
                    SOC_PBMP_PORT_REMOVE(si->xpipe_pbm, logic_port);
                } else {
                    SOC_PBMP_PORT_REMOVE(si->ypipe_pbm, logic_port);
                }
            }

            /* Logical port related information */
            si->port_speed_max[logic_port] = -1;
            si->port_num_lanes[logic_port] = -1;
            si->port_group[logic_port] = -1;
            si->port_serdes[logic_port] = -1;
            SOC_PBMP_PORT_REMOVE(si->oversub_pbm, logic_port);

            continue;
        }

        /*
         * Set information for mappings that are:
         *   - same (logical to physical mapping are the same)
         *   - remapped (different port mapping assignment)
         *   - new
         */

        /* If 'inactive' flag is set, remove port from disabled bitmap */
        if (pr->flags & SOC_PORT_RESOURCE_I_MAP) {
            SOC_PBMP_PORT_REMOVE(si->all.disabled_bitmap, logic_port);
        }

        /* Port Mapping related information */
        si->port_l2p_mapping[logic_port] = phy_port;
        si->port_p2l_mapping[phy_port] = logic_port;
        si->port_p2m_mapping[phy_port] = pr->mmu_port;
        if (pr->mmu_port != -1) {
            si->port_m2p_mapping[pr->mmu_port] = phy_port;
        }

        /* Pipeline */
        if (TD2P_PHY_PORT(unit, phy_port).pipe == TD2P_X_PIPE) {
            SOC_PBMP_PORT_ADD(si->xpipe_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_ADD(si->ypipe_pbm, logic_port);
        }

        /* Port data information */
        si->port_speed_max[logic_port] = pr->speed;
        si->port_num_lanes[logic_port] = pr->num_lanes;
        si->port_group[logic_port] = TD2P_PHY_PORT_LANE(unit, phy_port).pgw;
        si->port_serdes[logic_port] = (phy_port - 1) / TD2P_PORTS_PER_XLP;
        if (pr->oversub) {
            SOC_PBMP_PORT_ADD(si->oversub_pbm, logic_port);
        } else {
            SOC_PBMP_PORT_REMOVE(si->oversub_pbm, logic_port);
        }

#if 0
        /* TBD: 100G information */
        si->port_100g_lane_config[logic_port] = lane_config;
        si->port_fallback_lane[logic_port]    = fallback_lane;
#endif
    }

    SOC_PBMP_ASSIGN(si->pipe_pbm[0], si->xpipe_pbm);
    SOC_PBMP_ASSIGN(si->pipe_pbm[1], si->ypipe_pbm);

    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_td2p_port_resource_execute
 * Purpose:
 *      Execute the FlexPort operation.
 *
 *      This function updates the main SOC_INFO SW data structure and
 *      make changes to HW.
 * 
 *      If any error occurs the operation, changes cannot be undone.
 * Parameters:
 *      unit           - (IN) Unit number.
 *      nport          - (IN) Number of elements in array resource.
 *      resource       - (IN) Port Resource FlexPort configuration.
 *      pre_count      - (IN) Array size for pre-FlexPort array.
 *      pre_resource   - (IN) Pre-FlexPort configuration array.
 *      post_count     - (IN) Array size for post-FlexPort array.
 *      post_resource  - (IN) Pre-FlexPort configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      - Assumes all data is Port Resource arrays has been validated.
 */
STATIC int
_soc_td2p_port_resource_execute(int unit,
                                int nport, soc_port_resource_t *resource,
                                int pre_count,
                                soc_port_resource_t *pre_resource,
                                int post_count,
                                soc_port_resource_t *post_resource,
                                soc_td2p_info_t *pre_soc_info)
{
    int delete_count;

    /*
     * Update MMU Port Mappings
     *
     * This needs to be done in two steps:
     *
     * 1) Clear MMU port mappings for ports to be deleted BEFORE SOC_INFO
     *    is updated.
     * 2) Set MMU port mappings for new ports AFTER SOC_INFO is updated.
     *
     * Reason:
     * The SOC register functions expect the argument 'port' to
     * be the logical port number, including those registers
     * who are indexed by the MMU port number.
     * The soc_reg_addr_get() figures out the correct index/address
     * by remapping (translates) the logical port to mmu port index based
     * on the current SOC INFO port mapping.
     *
     * Thus, we must update the MMU HW at the proper time so that
     * the SOC_INFO SW data structure contains the desired
     * port mappings.
     */

    /*
     * Calculate number of ports to delete (clear).
     * Assumes the 'delete' ports are always firs in 'resource' array.
     */
    for (delete_count = 0; delete_count < nport; delete_count++) {
        if (resource[delete_count].physical_port != -1) {
            break;
        }
    }
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_mmu_mapping_set(unit,
                                                delete_count, resource));

    /* Update SOC_INFO SW data structure */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_soc_info_update(unit, nport, resource));

    /*
     * At this point, the SOC_INFO data structure reflects
     * the new port information.
     */

    /*
     * Get ports mode (quad, dual, single, tri).
     * This must be done AFTER the SOC_INFO has been updated
     * since it needs all the new information to determine
     * the right ports mode.
     */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_port_resource_mode_update(unit,
                                             nport, resource));

    /* Debug output */
    _soc_td2p_port_resource_data_output(unit,
                                        nport, resource,
                                        pre_count, pre_resource,
                                        post_count, post_resource);

    /* Reconfigure Port Macro */
    SOC_IF_ERROR_RETURN
        (soc_esw_portctrl_port_resource_configure(unit,
                                                  nport, resource));

    /* Reconfigure Schedulers */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_tdm_set(unit,
                                        pre_count, pre_resource,
                                        post_count, post_resource,
                                        pre_soc_info,
                                        TD2P_INFO(unit)->mmu_lossless));
    
    /* Reconfigure PGW */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_pgw_set(unit,
                                        pre_count, pre_resource,
                                        post_count, post_resource,
                                        TD2P_INFO(unit)->mmu_lossless));

    /* Reconfigure IP */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_ip_set(unit,
                                       pre_count, pre_resource,
                                       post_count, post_resource,
                                       pre_soc_info));

    /* Reconfigure EP */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_ep_set(unit,
                                       pre_count, pre_resource,
                                       post_count, post_resource,
                                       pre_soc_info));

    /* Reconfigure MMU */
    SOC_IF_ERROR_RETURN
        (soc_td2p_port_resource_mmu_set(unit, post_count, post_resource));

    return SOC_E_NONE;
}


/*
 * Function:
 *      soc_td2p_port_resource_configure
 * Purpose:
 *      Configure device ports based on given FlexPort information.
 *
 *      This routine validates that the given FlexPort configuration
 *      is valid and if successful, executes the FlexPort operation.
 * Parameters:
 *      unit     - (IN) Unit number.
 *      nport    - (IN) Number of elements in array resource.
 *      resource - (IN) Port resource configuration array.
 * Returns:
 *      SOC_E_XXX
 * Note:
 *      Assumes:
 *      - Basic checks are already performed by caller:
 *          . Array is in order (deletes are first)
 *          . Logical and physical ports are addressable
 *      - Caller has lock.
 */
int
soc_td2p_port_resource_configure(int unit, int nport,
                                 soc_port_resource_t *resource)
{
    int rv;
    int pre_count;
    int post_count;
    soc_port_resource_t *pre_resource;
    soc_port_resource_t *post_resource;
    soc_td2p_info_t pre_soc_info;

    TD2P_INFO_INIT_CHECK(unit);

    /* Validate SOC Port Resource data */
    SOC_IF_ERROR_RETURN
        (_soc_td2p_port_resource_validate(unit, nport, resource));

    /* Get all Port Resource information needed for operation */
    rv = _soc_td2p_port_resource_data_init(unit, nport, resource,
                                           &pre_count, &pre_resource,
                                           &post_count, &post_resource,
                                           &pre_soc_info);
    if (SOC_FAILURE(rv)) {
        _soc_td2p_port_resource_data_cleanup(&pre_resource);
        return rv;
    }

    /*
     * Execute FlexPort operation
     *
     * At this point:
     *   - Validation is successfull
     *   - FlexPort information is ready
     *
     * Changes are done to the to the SOC_INFO SW data structure and HW.
     * If any error occurs the operation, changes cannot be undone.
     */
    rv = _soc_td2p_port_resource_execute(unit, nport, resource,
                                         pre_count, pre_resource,
                                         post_count, post_resource,
                                         &pre_soc_info);
    if (SOC_FAILURE(rv)) {
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Error executing FlexPort operation (%d)\n"),
                   rv));
        LOG_ERROR(BSL_LS_SOC_PORT,
                  (BSL_META_U(unit,
                              "Changes to device may have been partially "
                              "executed.  System may be unstable.\n")));
    }

    /* Deallocate memory */
    _soc_td2p_port_resource_data_cleanup(&pre_resource);

    return rv;
}


#endif /* BCM_TRIDENT2PLUS_SUPPORT */
