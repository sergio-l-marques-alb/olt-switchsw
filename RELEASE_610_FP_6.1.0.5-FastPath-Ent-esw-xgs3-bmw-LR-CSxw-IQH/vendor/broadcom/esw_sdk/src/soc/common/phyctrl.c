/*
 * $Id: phyctrl.c,v 1.1 2011/04/18 17:11:04 mruas Exp $
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
 * StrataSwitch PHY control API
 * All access to PHY drivers should call the following soc functions.
 */

#include <sal/core/libc.h>
#include <shared/alloc.h>
#include <sal/core/spl.h>

#include <soc/drv.h>
#include <soc/debug.h>
#include <soc/port_ability.h>
#include <soc/phy.h>
#include <soc/phyreg.h>
#include <soc/phyctrl.h>
#include <soc/phy/phyctrl.h>
#include <soc/phy/drv.h>
#ifdef BCM_SBX_SUPPORT
#include <soc/sbx/sbx_drv.h>
#endif
#ifdef INCLUDE_MACSEC
#include <soc/macsecphy.h>
#endif

/* PHY address to port re-mapping */
int *phy_rmap[SOC_MAX_NUM_DEVICES];

/* Per port data structure to manage PHY drivers */
soc_phy_info_t *phy_port_info[SOC_MAX_NUM_DEVICES];

/* Per PHY data structure to manage PHY drivers */
phy_ctrl_t **int_phy_ctrl[SOC_MAX_NUM_DEVICES];
phy_ctrl_t **ext_phy_ctrl[SOC_MAX_NUM_DEVICES];

#define SOC_PHYCTRL_INIT_CHECK(_ext_pc, _int_pc) \
    if ((NULL == (_ext_pc)) && (NULL == (_int_pc))) { \
        return SOC_E_INIT; \
    } 

#define SOC_NULL_PARAM_CHECK(_param) \
    if (NULL == (_param)) { \
        return SOC_E_PARAM; \
    }

#ifdef PHYCTRL_DEBUG_PRINT
#define PHYCTRL_DEBUG(_message) soc_cm_debug _message
#else
#define PHYCTRL_DEBUG(_message) 
#endif 

int
soc_phyctrl_software_deinit(int unit)
{
    int port;

    if (NULL != phy_port_info[unit]) {
        sal_free(phy_port_info[unit]);
        phy_port_info[unit] = NULL;
    }

    if (NULL != int_phy_ctrl[unit]) {
        PBMP_PORT_ITER(unit, port) {
            if (NULL != int_phy_ctrl[unit][port]) {
                sal_free(int_phy_ctrl[unit][port]);
                int_phy_ctrl[unit][port] = NULL;
            }
        }
        sal_free(int_phy_ctrl[unit]);
        int_phy_ctrl[unit] = NULL;
    }

    if (NULL != ext_phy_ctrl[unit]) {
        PBMP_PORT_ITER(unit, port) {
            if (NULL != ext_phy_ctrl[unit][port]) {
                sal_free(ext_phy_ctrl[unit][port]);
                ext_phy_ctrl[unit][port] = NULL;
            }
        }
        sal_free(ext_phy_ctrl[unit]);
        ext_phy_ctrl[unit] = NULL;
    }

    if (NULL != phy_rmap[unit]) {
        sal_free(phy_rmap[unit]);
        phy_rmap[unit] = NULL;
    }

    SOC_IF_ERROR_RETURN (soc_phy_deinit(unit));

    return (SOC_E_NONE);
}

int
soc_phyctrl_software_init(int unit)
{
    int port;

    if ((phy_port_info[unit] != NULL) ||
        (int_phy_ctrl[unit] != NULL) ||
        (ext_phy_ctrl[unit] != NULL) ||
        (phy_rmap[unit] != NULL)) {
        soc_phyctrl_software_deinit(unit);
    }

    phy_port_info[unit] = sal_alloc(sizeof(soc_phy_info_t) * 
                                    SOC_MAX_NUM_PORTS,
                                    "phy_port_info");
    if (phy_port_info[unit] == NULL) {
        return SOC_E_MEMORY;
    }
    sal_memset(phy_port_info[unit], 0, 
               sizeof(soc_phy_info_t) * SOC_MAX_NUM_PORTS);

    int_phy_ctrl[unit] = sal_alloc(sizeof(phy_ctrl_t *) *
                                   SOC_MAX_NUM_PORTS,
                                   "int_phy_ctrl");
    if (int_phy_ctrl[unit] == NULL) {
        soc_phyctrl_software_deinit(unit);
        return SOC_E_MEMORY;
    }
    sal_memset(int_phy_ctrl[unit], 0, 
               sizeof(phy_ctrl_t *) * SOC_MAX_NUM_PORTS);


    ext_phy_ctrl[unit] = sal_alloc(sizeof(phy_ctrl_t *) *
                                   SOC_MAX_NUM_PORTS,
                                   "int_phy_ctrl");
    if (ext_phy_ctrl[unit] == NULL) {
        soc_phyctrl_software_deinit(unit);
        return SOC_E_MEMORY;
    }
    sal_memset(ext_phy_ctrl[unit], 0, 
               sizeof(phy_ctrl_t *) * SOC_MAX_NUM_PORTS);

    phy_rmap[unit] = sal_alloc(sizeof(int) * EXT_PHY_ADDR_MAX,
                               "phy_rmap");
    if (phy_rmap[unit] == NULL) {
        soc_phyctrl_software_deinit(unit);
        return SOC_E_MEMORY;
    }
    sal_memset(phy_rmap[unit], -1, sizeof(int) * EXT_PHY_ADDR_MAX);

    /* Initialize PHY driver table and assign default PHY address */
    SOC_IF_ERROR_RETURN
        (soc_phy_init(unit));

    PBMP_PORT_ITER(unit, port) {
        PHY_ADDR_TO_PORT(unit, PHY_ADDR(unit, port)) = port;
        PHY_ADDR_TO_PORT(unit, PHY_ADDR_INT(unit, port)) = port;
    }

    PHYCTRL_DEBUG((DK_PHY | DK_VERBOSE, 
                 "soc_phyctrl_software_init Unit %d\n",
                      unit));

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_init_phy_id_map
 * Purpose:
 *      Set external PHY address.
 * Parameters:
 *      unit - SOC Unit #.
 *      map  - External PHY address map for all ports.
 *             Each address must encode both the MDIO Bus number and
 *             MDIO device number.
 * Returns:
 *      
 * Note:
 *     This function is deprecated and will be removed in future releases. 
 * Do not use this function in new development. 
 */
int
soc_phyctrl_init_phy_id_map(int unit, int map[])
{
    soc_port_t port;
   
    if (phy_rmap[unit] == NULL) {
        phy_rmap[unit] = sal_alloc(sizeof(int) * EXT_PHY_ADDR_MAX,
                                   "phy_rmap");
        if (phy_rmap[unit] == NULL) {
            return SOC_E_MEMORY;
        }
    }
   /*
    * Initialize the PHY port map for the unit to default
    */
    sal_memset(phy_rmap[unit], -1, sizeof(int) * EXT_PHY_ADDR_MAX);
   
    PBMP_PORT_ITER(unit, port) {
        /* The map address must contain external MDIO address and
         * external MDIO bus number encoded as follow.
         */
        PHY_ADDR_TO_PORT(unit, PHY_ADDR_INT(unit, port)) = port;
        PHY_ADDR_TO_PORT(unit, map[port])                = port;
        PHY_ADDR(unit, port)                             = map[port];

        PHYCTRL_DEBUG((DK_PHY | DK_VERBOSE, 
                     "phy_rmap: U=%d P= %d Map= 0x%0x Add=0x%0x Port=%d\n",
                      unit, port, map[port], PHY_ADDR(unit, port),
                      PHY_ADDR_TO_PORT(unit, port)));
    }

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_drv_name_get
 * Purpose:
 *      Get PHY driver name.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      name - Buffer for PHY driver name.
 *      len  - Length of buffer.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_drv_name_get(int unit, soc_port_t port, char *name, int len)
{
    static char       unknown_driver[] = "unknown driver";
    phy_driver_t      *pd;
    int               string_len;

    pd = NULL;
    if (NULL != EXT_PHY_SW_STATE(unit, port)) {
        pd = EXT_PHY_SW_STATE(unit, port)->pd;
    } else if (NULL != INT_PHY_SW_STATE(unit, port)) {
        pd = INT_PHY_SW_STATE(unit, port)->pd;
    }
    
    if (NULL == pd) {
        string_len = (int)sizeof(unknown_driver);
        if (string_len <= len) {
            sal_strcpy(name, unknown_driver);
        }
        return SOC_E_NOT_FOUND;
    }

    string_len = (int)sal_strlen(pd->drv_name);
    if (string_len > len) {
        return SOC_E_MEMORY;
    }

    sal_strcpy(name, pd->drv_name);

    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_probe
 * Purpose:
 *      Probe for internal and external PHYs attached to the port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_probe(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *ext_pd;
    phy_driver_t *int_pd;
    phy_ctrl_t   ext_pc;
    phy_ctrl_t   int_pc;
    phy_ctrl_t   *pc;

    rv = SOC_E_NONE;
    sal_memset(&ext_pc, 0, sizeof(phy_ctrl_t));
    sal_memset(&int_pc, 0, sizeof(phy_ctrl_t));

    /* The soc layer probe function always uses the default PHY addresses
     * instead of PHY address stored in phy_info from previous probe.
     * This make sure that the external PHY probe works correctly even
     * when the device is hot plugged.  
     */
    int_pc.unit      = unit;
    int_pc.port      = port;
    int_pc.speed_max = SOC_INFO(unit).port_speed_max[port];
    ext_pc.unit      = unit;
    ext_pc.port      = port;
    ext_pc.speed_max =  SOC_INFO(unit).port_speed_max[port];

#ifdef BCM_SBX_SUPPORT
    if (SOC_IS_SBX(unit)) {
        /* SBX verson of register access */
        int_pc.read  = soc_sbx_miim_read;
	int_pc.write = soc_sbx_miim_write; 
	if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
	    ext_pc.read  = soc_sbx_miimc45_read;
	    ext_pc.write = soc_sbx_miimc45_write;
	} else {
	    ext_pc.read  = soc_sbx_miim_read;
	    ext_pc.write = soc_sbx_miim_write; 
	}
    } else
#endif /* BCM_SBX_SUPPORT */
#ifdef BCM_ROBO_SUPPORT
     if (SOC_IS_ROBO(unit)) {
        /* ROBO version of register access */
        int_pc.read  = soc_robo_miim_int_read;
        int_pc.write = soc_robo_miim_int_write;
        
        ext_pc.read  = soc_robo_miim_read;
        ext_pc.write = soc_robo_miim_write;
    } else 
#endif /* BCM_ROBO_SUPPORT */
    {
#ifdef BCM_ESW_SUPPORT
        int_pc.read  = soc_esw_miim_read;
        int_pc.write = soc_esw_miim_write; 
        if (IS_XE_PORT(unit, port) || IS_HG_PORT(unit, port)) {
            ext_pc.read  = soc_esw_miimc45_read;
            ext_pc.write = soc_esw_miimc45_write;
        } else {
            ext_pc.read  = soc_esw_miim_read;
            ext_pc.write = soc_esw_miim_write; 
        }
#endif /* BCM_ESW_SUPPORT */
    }

    SOC_IF_ERROR_RETURN
        (soc_phy_probe(unit, port, &ext_pc, &int_pc));

    ext_pd = ext_pc.pd;
    int_pd = int_pc.pd;
    if (ext_pd == int_pd) {
        /* If external PHY driver and internal PHY driver are the same,
         * config setting must have override the PHY driver selection.
         * In this case just attach internal PHY driver. 
         * Internal driver is needed for all devices with internal
         * SerDes because MAC driver performs notify calls to internal
         * PHY driver. 
         */
        ext_pd = NULL;
    }

    if (NULL != ext_pd) {
        if (NULL == EXT_PHY_SW_STATE(unit, port)) {
            EXT_PHY_SW_STATE(unit, port) =
                sal_alloc (sizeof (phy_ctrl_t), ext_pd->drv_name);
            if (NULL == EXT_PHY_SW_STATE(unit, port)) {
                rv = SOC_E_MEMORY;
            }
        }
        if (SOC_SUCCESS(rv)) {
            pc = EXT_PHY_SW_STATE(unit, port);
            sal_memcpy(pc, &ext_pc, sizeof(phy_ctrl_t));
            rv = soc_phy_reset_register(unit, port, pc->pd->pd_reset, 
                                        NULL, TRUE);
            PHY_ADDR_TO_PORT(unit, PHY_ADDR(unit, port)) = port;
            soc_cm_debug(DK_PHY, 
                         "soc_phyctrl_probe external: u=%d p=%d %s\n",
                              unit, port, ext_pd->drv_name);
#ifdef INCLUDE_MACSEC
            /* 
             * Check if this is MACSEC capable PHY and perform MACSEC 
             * initialization 
             */
            if (SOC_SUCCESS(rv)) {
                rv = soc_macsecphy_init(unit, port, pc);
                if (!SOC_SUCCESS(rv)) {
                    soc_cm_debug(DK_ERR, "soc_phyctrl_probe: MACSEC init for"
                                         " u=%d p=%d FAILED ", unit, port);
                } else {
                    soc_cm_debug(DK_PHY, "soc_phyctrl_probe: MACSEC init for"
                                         " u=%d p=%d SUCCESS ", unit, port);
                }
            }
#endif  /* INCLUDE_MACSEC */
        } 
    } else {
        /* No external PHY detected. If there is allocated memory for
         * external PHY driver, free it.
         */ 
        if (NULL != EXT_PHY_SW_STATE(unit, port)) {
            sal_free(EXT_PHY_SW_STATE(unit, port));
            EXT_PHY_SW_STATE(unit, port) = NULL;
        }
    }

    if (SOC_SUCCESS(rv) && NULL != int_pd) {
        if (NULL == INT_PHY_SW_STATE(unit, port)) {
            INT_PHY_SW_STATE(unit, port) =
                sal_alloc (sizeof (phy_ctrl_t), int_pd->drv_name);
            if (NULL == INT_PHY_SW_STATE(unit, port)) {
                rv = SOC_E_MEMORY;
            }
        } else {
            phy_ctrl_t  *ppc = INT_PHY_SW_STATE(unit, port);
            if (ppc->driver_data) {
                /* If driver data allocated, must free it */
                sal_free(ppc->driver_data);
            }
        }
        if (SOC_SUCCESS(rv)) {
            pc = INT_PHY_SW_STATE(unit, port);
            sal_memcpy(pc, &int_pc, sizeof(phy_ctrl_t));
            PHY_ADDR_TO_PORT(unit, PHY_ADDR_INT(unit, port)) = port;
            if (NULL == ext_pd) {
                /* If there is no external PHY, the internal PHY 
                 * must be in fiber mode. 
                 */ 
                if (soc_property_port_get(unit, port,
                                              spn_SERDES_FIBER_PREF, 1)) {
                    PHY_FLAGS_SET(unit, port, PHY_FLAGS_FIBER);
                } else {
                    PHY_FLAGS_CLR(unit, port, PHY_FLAGS_FIBER);
                }
                rv = soc_phy_reset_register(unit, port, pc->pd->pd_reset, 
                                            NULL, TRUE);
            }

            soc_cm_debug(DK_PHY, 
                         "soc_phyctrl_probe internal: u=%d p=%d %s\n",
                              unit, port, int_pd->drv_name);
        }
    } else {
        /* No internal PHY detected. If there is allocated memory for
         * internal PHY driver, free it.
         */ 
        if (NULL != INT_PHY_SW_STATE(unit, port)) {
            phy_ctrl_t  *ppc = INT_PHY_SW_STATE(unit, port);
            if (ppc->driver_data) {
                /* If driver data allocated, must free it */
                sal_free(ppc->driver_data);
            }
            sal_free(ppc);
            ppc = NULL;
        }
    }



    if (SOC_SUCCESS(rv)) {
        /* Set SOC related restriction/configuration/flags first */ 
        PHY_FLAGS_CLR(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        if (soc_property_port_get(unit, port, spn_PHY_SGMII_AUTONEG, FALSE) &&
            soc_feature(unit, soc_feature_sgmii_autoneg)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        }
    } else {
        if (NULL != EXT_PHY_SW_STATE(unit, port)) {
            sal_free(EXT_PHY_SW_STATE(unit, port));
            EXT_PHY_SW_STATE(unit, port) = NULL;
        }
 
        if (NULL != INT_PHY_SW_STATE(unit, port)) {
            phy_ctrl_t  *ppc = INT_PHY_SW_STATE(unit, port);
            if (ppc->driver_data) {
                /* If driver data allocated, must free it */
                sal_free(ppc->driver_data);
            }
            sal_free(ppc);
            ppc = NULL;
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_init
 * Purpose:
 *      Initialize the PHY drivers attached to the port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_init(int unit, soc_port_t port)
{
    int         rv;
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;

    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

#ifdef BCM_SBX_SUPPORT
    if (SOC_IS_SBX(unit)) {
        /* Set Sandburst switch specific PHY flags */
    } else
#endif /* BCM_SBX_SUPPORT */
#ifdef BCM_ROBO_SUPPORT
     if (SOC_IS_ROBO(unit)) {
        /* Set ROBO switch specific PHY flags */
        if (SOC_IS_ROBODINO(unit)||SOC_IS_ROBO5395(unit) ||
            SOC_IS_ROBO5398(unit)||SOC_IS_ROBO5397(unit) ||
            SOC_IS_ROBO53115(unit)|| SOC_IS_ROBO53118(unit)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FORCED_COPPER);
        }
        if (soc_feature(unit, soc_feature_sgmii_autoneg)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        }
    } else 
#endif /* BCM_ROBO_SUPPORT */
    {
#ifdef BCM_ESW_SUPPORT
        if (SOC_IS_DRACO(unit)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_FORCED_SGMII);
        }
        if (SOC_IS_BRADLEY(unit)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SGMII_AUTONEG);
        }
        if (IS_LMD_ENABLED_PORT(unit, port)) {
            PHY_FLAGS_SET(unit, port, PHY_FLAGS_SINGLE_LANE);
        }
#endif /* BCM_ESW_SUPPORT */
    }

    rv = SOC_E_NONE;
    if (NULL != ext_pc) {
        rv = (PHY_INIT(ext_pc->pd, unit, port));
    } 

    if (NULL != int_pc) {
        rv = (PHY_INIT(int_pc->pd, unit, port));
    }

    PHY_FLAGS_SET(unit, port, PHY_FLAGS_INIT_DONE);

    PHYCTRL_DEBUG((DK_PHY | DK_VERBOSE, 
                 "soc_phyctrl_init: u=%d p=%d %s rv=%d\n",
                             unit, port, (ext_pc) ? "EXT" : "INT", rv));
    return rv;
}

int
soc_phyctrl_pd_get(int unit, soc_port_t port, phy_driver_t **pd)
{
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        *pd = ext_pc->pd;
    } else {
        *pd = int_pc->pd;
    }

    return SOC_E_NONE;
}

int
soc_phyctrl_passthru_pd_get(int unit, soc_port_t port, phy_driver_t **pd)
{
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (PHY_PASSTHRU_MODE(unit, port)) {
        if (NULL != int_pc) {
            *pd = int_pc->pd;
        } else {
            *pd = ext_pc->pd;
        }
    } else {
        if (NULL != ext_pc) {
            *pd = ext_pc->pd;
        } else {
            *pd = int_pc->pd;
        }
    }
 
    return SOC_E_NONE;
}

/*
 * Function:
 *      soc_phyctrl_reset
 * Purpose:
 *      Reset the PHY drivers attached to the port.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reset(int unit, soc_port_t port, void *user_arg)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_RESET(pd, unit, port));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN,
                      "soc_phyctrl_reset: u=%d p=%d rv=%d\n",
                       unit, port, rv));
    } 
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_drv_name
 * Purpose:
 *      Get pointer to driver name.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      name - driver name.
 * Returns:
 *      SOC_E_XXX
 */
char *
soc_phyctrl_drv_name(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        return pd->drv_name;
    }
    return NULL;
}


/*
 * Function:
 *      soc_phyctrl_link_get
 * Purpose:
 *      Read link status of the PHY driver.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      link - Link status
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_link_get(int unit, soc_port_t port, int *link)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(link);
    *link = FALSE;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINK_GET(pd, unit, port, link));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_link_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_enable_set
 * Purpose:
 *      Enable/Disable the PHY driver attached to the port. 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - enable/disable PHY driver
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_enable_set(int unit, soc_port_t port, int enable)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ENABLE_SET(pd, unit, port, enable));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, 
                      "soc_phyctrl_enable_set: u=%d p=%d e=%d rv=%d\n",
                       unit, port, enable, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_enable_get
 * Purpose:
 *      Get the enable/disable state of the PHY driver. 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - Current enable/disable state.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_enable_get(int unit, soc_port_t port, int *enable)
{
    phy_driver_t *pd;
    int           rv;

    SOC_NULL_PARAM_CHECK(enable);

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ENABLE_GET(pd, unit, port, enable));
    }
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_enable_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_duplex_set
 * Purpose:
 *      Set duplex of the PHY.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      duplex -  (1) Full duplex
 *                (0) Half duplex
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_duplex_set(int unit, soc_port_t port, int duplex)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_DUPLEX_SET(pd, unit, port, duplex));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, 
                      "soc_phyctrl_duplex_set: u=%d p=%d d=%d rv=%d\n",
                      unit, port, duplex, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_duplex_get
 * Purpose:
 *      Get current duplex setting of the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      duplex -  (1) Full duplex
 *                (0) Half duplex
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_duplex_get(int unit, soc_port_t port, int *duplex)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_DUPLEX_GET(pd, unit, port, duplex));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, 
                      "soc_phyctrl_duplex_get:  u=%d p=%d rv=%d\n",
                      unit, port, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_speed_set
 * Purpose:
 *      Set PHY speed.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      speed - new speed of the PHY
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_speed_set(int unit, soc_port_t port, int speed)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_SPEED_SET(pd, unit, port, speed));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN,
                      "soc_phyctrl_speed_set: u=%d p=%d s=%d rv=%d\n",
                      unit, port, speed, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_speed_get
 * Purpose:
 *      Read current PHY speed
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      speed - Current speed of PHY.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_speed_get(int unit, soc_port_t port, int *speed)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(speed);
    *speed = 0;

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_SPEED_GET(pd, unit, port, speed));
    }
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_speed_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_master_set
 * Purpose:
 *      Ser Master/Slave configuration of the PHY.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      master - (1) master mode.
 *               (0) slave mode. 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_master_set(int unit, soc_port_t port, int master)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MASTER_SET(pd, unit, port, master));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_PHY | DK_VERBOSE, 
                      "soc_phyctrl_master_set: u=%d p=%d m=%d rv=%d\n",
                      unit, port, master, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_master_get
 * Purpose:
 *      Read current Master/Slave setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      master - (1) master mode.
 *               (0) slave mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_master_get(int unit, soc_port_t port, int *master)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(master);

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MASTER_GET(pd, unit, port, master));
    }
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_master_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_auto_negotiate_set
 * Purpose:
 *      Enable/Disable autonegotiation of the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      an   - new autoneg setting 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_auto_negotiate_set(int unit, soc_port_t port, int an)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_AUTO_NEGOTIATE_SET(pd, unit, port, an));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN,
                     "soc_phyctrl_auto_negotiate_set: u=%d p=%d an=%d rv=%d\n", 
                     unit, port, an, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_auto_negotiate_get
 * Purpose:
 *      Get current auto neg setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      an   - current autoneg setting
 *      an_done - autoneg completed 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_auto_negotiate_get(int unit, soc_port_t port, int *an, int *an_done)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(an);
    SOC_NULL_PARAM_CHECK(an_done);

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_AUTO_NEGOTIATE_GET(pd, unit, port, an, an_done));
    }
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_auto_negotiate_get failed %d\n", 
                       rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_adv_local_set
 * Purpose:
 *      Configure local advertising setting.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - Advertising mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_adv_local_set(int unit, soc_port_t port, soc_port_mode_t mode)
{
    int                   rv;
    phy_driver_t         *pd;
    soc_port_ability_t    ability;

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);
  
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ADV_SET(pd, unit, port, mode));
        if (SOC_E_UNAVAIL == rv) {
            rv = soc_port_mode_to_ability(mode, &ability);
            if (SOC_SUCCESS(rv)) {
                rv = (PHY_ABILITY_ADVERT_SET(pd, unit, port, &ability));
            }
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_adv_local_set: u=%d p=%d "
                      "m=%x rv=%d\n", unit, port, mode, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_adv_local_get
 * Purpose:
 *      Get current local advertising setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - Current advertised mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_adv_local_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int                   rv;
    phy_driver_t         *pd;
    soc_port_ability_t    ability;

    SOC_NULL_PARAM_CHECK(mode);

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ADV_GET(pd, unit, port, mode));
        if (rv == SOC_E_UNAVAIL) {
            rv = (PHY_ABILITY_ADVERT_GET(pd, unit, port, &ability));
            if (SOC_SUCCESS(rv)) {
                rv = soc_port_ability_to_mode(&ability, mode);
            }
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_adv_local_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_adv_remote_get
 * Purpose:
 *      Get link partner advertised mode
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - Link partner advertised mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_adv_remote_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int                   rv;
    phy_driver_t         *pd;
    soc_port_ability_t    ability;

    SOC_NULL_PARAM_CHECK(mode);

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);
    if (SOC_SUCCESS(rv)) {
        rv = (PHY_ADV_REMOTE_GET(pd, unit, port, mode));

        if (rv == SOC_E_UNAVAIL) {
            rv = (PHY_ABILITY_REMOTE_GET(pd, unit, port, &ability));
            if (SOC_SUCCESS(rv)) {
                rv = soc_port_ability_to_mode(&ability, mode);
            }
        }
    }
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_adv_remote_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_loopback_set
 * Purpose:
 *      Enable/disable loopback mode.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_loopback_set(int unit, soc_port_t port, int enable)
{
    int           rv;
    phy_ctrl_t   *int_pc;
    phy_ctrl_t   *ext_pc;
    phy_driver_t *pd;

    rv     = SOC_E_NONE;
    int_pc = INT_PHY_SW_STATE(unit, port);
    ext_pc = EXT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    if (NULL != ext_pc) {
        pd = ext_pc->pd;
    } else {
        pd = int_pc->pd;
    }
    rv = (PHY_LOOPBACK_SET(pd, unit, port, enable));

    /* Wait until link up if internal PHY is put into loopback */
    if (SOC_SUCCESS(rv) && enable && (NULL != int_pc)) { 
        if ((PHY_PASSTHRU_MODE(unit, port)) || (NULL == ext_pc)) {
            int           link;
            soc_timeout_t to;

            /* Wait up to 5000 msec for link up */
            soc_timeout_init(&to, 5000000, 0);
            link = 0;
            /*
             * Needs more than one read to clear Latched Link down bits.
             */
            rv = (PHY_LINK_GET(int_pc->pd, unit, port, &link)); 
            do {
                rv = (PHY_LINK_GET(int_pc->pd, unit, port, &link)); 
                if (link || SOC_FAILURE(rv)) {
                    break;
                }
            } while (!soc_timeout_check(&to));
            if (!link) {
                soc_cm_debug(DK_WARN,
                             "soc_phyctrl_loopback_set: u=%d p=%d TIMEOUT\n",
                             unit, port);
                rv = SOC_E_TIMEOUT;
            }
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, 
                      "soc_phyctrl_loopback_set: u=%d p=%d l=%d rv=%d\n",
                      unit, port, enable, rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_loopback_get
 * Purpose:
 *      Get current loopback setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      enable - (1) Enable loopback mode.
 *               (0) Disable loopback mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_loopback_get(int unit, soc_port_t port, int *enable)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(enable);

    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LOOPBACK_GET(pd, unit, port, enable));
    }
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_loopback_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_interface_set
 * Purpose:
 *      Set the interface between MAC and PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      pif  - Interface type 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_interface_set(int unit, soc_port_t port, soc_port_if_t pif)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_INTERFACE_SET(pd, unit, port, pif));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN,
                      "soc_phyctrl_interface_set: u=%d p=%d i=%d rv=%d\n",
                      unit, port, pif, rv));
    }
    return rv; 
}

/*
 * Function:
 *      soc_phyctrl_interface_get
 * Purpose:
 *      Get current interface setting between MAC and PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      pif  - current interface setting
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_interface_get(int unit, soc_port_t port, soc_port_if_t *pif)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_INTERFACE_GET(pd, unit, port, pif));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_interface_get failed %d\n", rv));
    }
    return rv;
}

STATIC int
_soc_phy_ability_get(int unit, soc_port_t port, 
                         phy_driver_t *pd, soc_port_mode_t *mode)
{
    int                 rv;
    soc_port_ability_t  ability;

    rv = (PHY_ABILITY_GET(pd, unit, port, mode));

    if (SOC_E_UNAVAIL == rv) {
        rv = (PHY_ABILITY_LOCAL_GET(pd, unit, port, &ability));
        if (SOC_SUCCESS(rv)) {
            rv = soc_port_ability_to_mode(&ability, mode);
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "_soc_phy_ability_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_get
 * Purpose:
 *      Get PHY ability
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_get(int unit, soc_port_t port, soc_port_mode_t *mode)
{
    int              rv;
    soc_port_mode_t  mode_speed_int;
    soc_port_mode_t  mode_speed_ext;
    phy_ctrl_t      *int_pc;
    phy_ctrl_t      *ext_pc;

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);
    rv = SOC_E_NONE;

    mode_speed_int = mode_speed_ext = SOC_PM_SPEED_ALL;
    if (NULL != int_pc) {

        if (int_pc->speed_max > 16000) {
            soc_cm_debug(DK_ERR,
                    "soc_phyctrl_ability_get: Speed support above 16Gbps will"
                    "not work. Use soc_phyctrl_ability_local_get\n");
        }

        rv = _soc_phy_ability_get(unit, port, int_pc->pd, mode);
        mode_speed_int = *mode & SOC_PM_SPEED_ALL;
    }
    if (SOC_SUCCESS(rv) && NULL != ext_pc) {
        rv = _soc_phy_ability_get(unit, port, ext_pc->pd, mode);
        mode_speed_ext = *mode & SOC_PM_SPEED_ALL;
    }

    if (SOC_SUCCESS(rv)) {
        *mode &= ~(SOC_PM_SPEED_ALL);
        *mode |= (mode_speed_int & mode_speed_ext);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_ability_get failed %d\n", rv));
    }
    soc_cm_debug(DK_PHY, "soc_phyctrl_ability_get E=%08x I=%08x C=%08x\n",
                        mode_speed_ext, mode_speed_int,*mode);
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_linkup_evt
 * Purpose:
 *      Force link up event to PHY.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_linkup_evt(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINKUP_EVT(pd, unit, port));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_linkdn_evt
 * Purpose:
 *      Force link down event to PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_linkdn_evt(int unit, soc_port_t port)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINKDN_EVT(pd, unit, port));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_mdix_set
 * Purpose:
 *      Set new mdix setting 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mdix - new mdix mode
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_mdix_set(int unit, soc_port_t port, soc_port_mdix_t mdix)
{
    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MDIX_SET(pd, unit, port, mdix));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, 
                      "soc_phyctrl_mdix_set: u=%d p=%d m=%d rv=%d\n",
                      unit, port, mdix, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_mdix_get
 * Purpose:
 *      Get current mdix setting
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mdix - current mdix mode.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_mdix_get(int unit, soc_port_t port, soc_port_mdix_t *mdix)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(mdix);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MDIX_GET(pd, unit, port, mdix));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_mdix_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_mdix_status_get
 * Purpose:
 *      Current resolved mdix status.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      status - Current resolved mdix status.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_mdix_status_get(int unit, soc_port_t port,
                        soc_port_mdix_status_t *status)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(status);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MDIX_STATUS_GET(pd, unit, port, status));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_mdix_status_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_medium_config_set
 * Purpose:
 *      Set configuration of selected medium
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      medium - Selected medium
 *      cfg    - New configuration of the selected medium
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_medium_config_set(int unit, soc_port_t port, 
                          soc_port_medium_t medium,
                          soc_phy_config_t *cfg)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(cfg);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MEDIUM_CONFIG_SET(pd, unit, port, medium, cfg));
    }
 
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN,
                 "soc_phyctrl_medium_config_set: u=%d p=%d m=%d rv=%d\n",
                         unit, port, medium, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_medium_config_get
 * Purpose:
 *      Get current configuration of the selected medium
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      medium - Selected medium
 *      cfg    - Current configuration of the selected medium
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_medium_config_get(int unit, soc_port_t port, 
                          soc_port_medium_t medium,
                          soc_phy_config_t *cfg)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(cfg);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MEDIUM_CONFIG_GET(pd, unit, port, medium, cfg));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_medium_config_get failed %d\n", 
                       rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_medium_get
 * Purpose:
 *      Get active medium type
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      medium - active medium
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_medium_get(int unit, soc_port_t port, soc_port_medium_t *medium)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(medium);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_MEDIUM_GET(pd, unit, port, medium));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_medium_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_cable_diag
 * Purpose:
 *      Run cable diag on the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      status - Cable status
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_cable_diag(int unit, soc_port_t port, soc_port_cable_diag_t *status)
{
    int         rv;
    phy_ctrl_t *ext_pc;

    SOC_NULL_PARAM_CHECK(status);

    rv     = SOC_E_UNAVAIL;
    ext_pc = EXT_PHY_SW_STATE(unit, port);

    if (NULL != ext_pc) {
        rv = (PHY_CABLE_DIAG(ext_pc->pd, unit, port, status));

        if (SOC_FAILURE(rv)) {
            PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_cable_diag failed %d\n", rv));
        }
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_link_change
 * Purpose:
 *      Force link change on the PHY
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      link - Link status to change
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_link_change(int unit, soc_port_t port, int *link)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(link);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_LINK_CHANGE(pd, unit, port, link));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_link_change failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_control_set
 * Purpose:
 *      Set PHY specific configuration
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phy_ctrl - PHY control type to change
 *      value    - New setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_control_set(int unit, soc_port_t port, 
                    soc_phy_control_t phy_ctrl, uint32 value)
{

    int           rv;
    phy_driver_t *pd;

    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_CONTROL_SET(pd, unit, port, phy_ctrl, value));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN,
                 "soc_phyctrl_control_set: u=%d p=%d c=%d rv=%d\n",
                         unit, port, phy_ctrl, rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_control_get
 * Purpose:
 *      Get current setting of the PHY control
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      phy_ctrl - PHY control type to read 
 *      value    - Current setting for the PHY control
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_control_get(int unit, soc_port_t port, 
                    soc_phy_control_t phy_ctrl, uint32 *value)
{
    int           rv;
    phy_driver_t *pd;

    SOC_NULL_PARAM_CHECK(value);
    rv = soc_phyctrl_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = (PHY_CONTROL_GET(pd, unit, port, phy_ctrl, value));
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_control_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_reg_read
 * Purpose:
 *      Read PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data read
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reg_read(int unit, soc_port_t port, uint32 flags, 
                 uint32 addr, uint32 *data)
{
    int         rv;
    phy_ctrl_t *pc;

    SOC_NULL_PARAM_CHECK(data);

    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_REG_READ(pc->pd, unit, port, flags, addr, data);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_reg_read failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_reg_write
 * Purpose:
 *      Write to PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data to be written 
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reg_write(int unit, soc_port_t port, uint32 flags, 
                  uint32 addr, uint32 data)
{
    int         rv;
    phy_ctrl_t *pc;

    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_REG_WRITE(pc->pd, unit, port, flags, addr, data);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_reg_write failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_reg_modify
 * Purpose:
 *      Modify PHY register
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      addr  - PHY register address
 *      data  - data to be written
 *      mask  - bit mask of data to be modified
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_reg_modify(int unit, soc_port_t port, uint32 flags, 
                   uint32 addr, uint32 data, uint32 mask)
{
    int         rv;
    phy_ctrl_t *pc;

    rv = SOC_E_UNAVAIL;

    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }

    if (NULL != pc) {
        rv = PHY_REG_MODIFY(pc->pd, unit, port, flags, addr, data, mask);
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_reg_modify failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_advert_get
 * Purpose:
 *      Get local PHY advertised ability 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      ability - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_advert_get(int unit, soc_port_t port, 
                            soc_port_ability_t * ability)
{
    int              rv;
    phy_driver_t    *pd;
    soc_port_mode_t  mode;

    SOC_NULL_PARAM_CHECK(ability);
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = PHY_ABILITY_ADVERT_GET(pd, unit, port, ability);
    }

    if (SOC_E_UNAVAIL == rv) {
        rv = PHY_ADV_GET(pd, unit, port, &mode); 
        if (SOC_SUCCESS(rv)) {
            rv = soc_port_mode_to_ability(mode, ability);
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_ability_advert_get failed %d\n", rv));
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_advert_set
 * Purpose:
 *      Set local PHY advertised ability 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      ability - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_advert_set(int unit, soc_port_t port, 
                            soc_port_ability_t * ability)
{
    int              rv;
    phy_driver_t    *pd;
    soc_port_mode_t  mode;

    SOC_NULL_PARAM_CHECK(ability);
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = PHY_ABILITY_ADVERT_SET(pd, unit, port, ability);
    }

    if (SOC_E_UNAVAIL == rv) {
        rv = soc_port_ability_to_mode(ability, &mode);
        if (SOC_SUCCESS(rv)) {
            rv = PHY_ADV_SET(pd, unit, port, mode); 
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_ability_advert_set failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_remote_get
 * Purpose:
 *      Get remote PHY advertsied ability 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      ability - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_remote_get(int unit, soc_port_t port, 
                                   soc_port_ability_t * ability)
{
    int              rv;
    phy_driver_t    *pd;
    soc_port_mode_t  mode;

    SOC_NULL_PARAM_CHECK(ability);
    rv = soc_phyctrl_passthru_pd_get(unit, port, &pd);

    if (SOC_SUCCESS(rv)) {
        rv = PHY_ABILITY_REMOTE_GET(pd, unit, port, ability);
    }

    if (SOC_E_UNAVAIL == rv) {
        rv = PHY_ADV_REMOTE_GET(pd, unit, port, &mode); 
        if (SOC_SUCCESS(rv)) {
            rv = soc_port_mode_to_ability(mode, ability);
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_ability_remote_get failed %d\n", rv));
    }

    return rv;
}

STATIC int
_soc_phy_ability_local_get(int unit, soc_port_t port, 
                         phy_driver_t *pd, soc_port_ability_t *ability)
{
    int              rv;
    soc_port_mode_t  mode;

    rv = PHY_ABILITY_LOCAL_GET(pd, unit, port, ability);

    if (SOC_E_UNAVAIL == rv) {
        rv = PHY_ABILITY_GET(pd, unit, port, &mode); 
        if (SOC_SUCCESS(rv)) {
            rv = soc_port_mode_to_ability(mode, ability);
        }
    }

    return rv;
}

/*
 * Function:
 *      soc_phyctrl_ability_local_get
 * Purpose:
 *      Get PHY ability
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      mode - PHY ability
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_ability_local_get(int unit, soc_port_t port,
                             soc_port_ability_t *ability)
{
    int                 rv;
    soc_port_ability_t  ability_int;
    soc_port_ability_t  ability_ext;
    phy_ctrl_t         *int_pc;
    phy_ctrl_t         *ext_pc;

    SOC_NULL_PARAM_CHECK(ability);

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    ability_int.speed_half_duplex = ability_ext.speed_half_duplex = SOC_PA_SPEED_ALL;
    ability_int.speed_full_duplex = ability_ext.speed_full_duplex = SOC_PA_SPEED_ALL; 

    rv = SOC_E_NONE;
    if (NULL != int_pc) {
        rv = _soc_phy_ability_local_get(unit, port, int_pc->pd, ability);
        ability_int.speed_full_duplex = ability->speed_full_duplex;
        ability_int.speed_half_duplex = ability->speed_half_duplex;
    }
    if (SOC_SUCCESS(rv) && NULL != ext_pc) {
        rv = _soc_phy_ability_local_get(unit, port, ext_pc->pd, ability);
        ability_ext.speed_full_duplex = ability->speed_full_duplex;
        ability_ext.speed_half_duplex = ability->speed_half_duplex;
    }

    if (SOC_SUCCESS(rv)) {
        ability->speed_half_duplex = ability_int.speed_half_duplex & ability_ext.speed_half_duplex;
        ability->speed_full_duplex = ability_int.speed_full_duplex & ability_ext.speed_full_duplex;
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_ability_get failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_firmware_set
 * Purpose:
 *      Update the phy device's firmware 
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 *      flags - Flags
 *      offset - offset to the data stream
 *      addr  - PHY register address
 *      data  - data to be written
 *      mask  - bit mask of data to be modified
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_firmware_set(int unit, soc_port_t port, uint32 flags,
                   int offset, uint8 *array, int len)
{
    int         rv;
    phy_ctrl_t *pc;
                                                                                
    rv = SOC_E_UNAVAIL;
                                                                                
    if (flags & SOC_PHY_INTERNAL) {
        pc = INT_PHY_SW_STATE(unit, port);
    } else {
        pc = EXT_PHY_SW_STATE(unit, port);
    }
                                                                                
    if (NULL != pc) {
        rv = PHY_FIRMWARE_SET(pc->pd, unit, port, offset, array, len);
    }
                                                                                
    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_firmware_set failed %d\n", rv));
    }
    return rv;
}

/*
 * Function:
 *      soc_phyctrl_detach
 * Purpose:
 *      Remove PHY driver.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_detach(int unit, soc_port_t port)
{
    phy_driver_t  *phyd;
    phy_ctrl_t    *ext_pc;
 
    ext_pc = EXT_PHY_SW_STATE(unit, port);

    SOC_IF_ERROR_RETURN(soc_phy_nocxn(unit, &phyd));

    if (NULL == ext_pc) {
        /* This function is used for Hot Swapping external PHY driver.
         * Therefore, always attach no connection driver to external PHY. 
         */
        ext_pc = sal_alloc (sizeof (phy_ctrl_t), phyd->drv_name);
        if (NULL == ext_pc) {
            return SOC_E_MEMORY;
        }
        
        sal_memset(ext_pc, 0, sizeof(phy_ctrl_t));
        ext_pc->unit                 = unit;
        ext_pc->port                 = port;
        ext_pc->phy_id               = PHY_ADDR(unit, port);        
        EXT_PHY_SW_STATE(unit, port) = ext_pc;
    }

    ext_pc->pd   = phyd;

    /* Clear the PHY configuration flags after the external PHY is detached */
    PHY_FLAGS_CLR_ALL(unit, port);

    return SOC_E_NONE;
}

#ifdef PHYCTRL_DEBUG_PRINT
STATIC char * 
soc_phyctrl_event_string(soc_phy_event_t event)
{
     static char *phy_event[] = PHY_EVENT_STRING;

     assert((sizeof(phy_event) / sizeof(phy_event[0])) == phyEventCount);

     if (event >= phyEventCount) {
         return "Unknown Event";
     }

     return phy_event[event];
}
#endif /* PHYCTRL_DEBUG_PRINT */

/*
 * Function:
 *      soc_phyctrl_notify
 * Purpose:
 *      Notify events to internal PHY driver.
 * Parameters:
 *      unit - SOC Unit #.
 *      port - Port number.
 * Returns:
 *      SOC_E_XXX
 */
int
soc_phyctrl_notify(int unit, soc_port_t port,
                   soc_phy_event_t event, uint32 data)
{
    int         rv;
    phy_ctrl_t *int_pc;
    phy_ctrl_t *ext_pc;

    ext_pc = EXT_PHY_SW_STATE(unit, port);
    int_pc = INT_PHY_SW_STATE(unit, port);
    SOC_PHYCTRL_INIT_CHECK(ext_pc, int_pc);

    rv     = SOC_E_NONE;

    if ((NULL != ext_pc) && (NULL != int_pc)) {
        if (!PHY_SGMII_AUTONEG_MODE(unit, port)) {
            rv = (PHY_NOTIFY(int_pc->pd, unit, port, event, data));

            PHYCTRL_DEBUG((DK_PHY | DK_VERBOSE, 
                           "u=%d p=%d event=%s data=0x%08x\n",
                           unit, port, soc_phyctrl_event_string(event), data));
        }
    }

    if (SOC_FAILURE(rv)) {
        PHYCTRL_DEBUG((DK_WARN, "soc_phyctrl_notify failed %d\n", rv));
    }
 
    return rv;
}


STATIC void 
_soc_phyctrl_dump(phy_ctrl_t *pc)
{
    static char * if_string[] = {"NOCXN", "NULL",
                                 "MII", "GMII",
                                 "SGMII", "TBI",
                                 "XGMII", "RGMII",
                                 "RvMII", "1000X"};
                                      
    soc_cm_print("%s\n", pc->pd->drv_name);
    soc_cm_print("port         %d\n", pc->port);
    soc_cm_print("phy_id0      0x%04x\n", pc->phy_id0);
    soc_cm_print("phy_id1      0x%04x\n", pc->phy_id1);
    soc_cm_print("phy_model    0x%04x\n", pc->phy_model);
    soc_cm_print("phy_rev      0x%04x\n", pc->phy_rev);
    soc_cm_print("phy_oui      0x%04x\n", pc->phy_oui);
    soc_cm_print("phy_id       0x%02x\n", pc->phy_id);
    soc_cm_print("ledmode      0x%02x, 0x%02x, 0x%02x, 0x%02x\n", 
                               pc->ledmode[0], pc->ledmode[1],
                               pc->ledmode[2], pc->ledmode[3]);
    soc_cm_print("ledctrl      0x%04x\n", pc->ledctrl);
    soc_cm_print("ledselect    0x%04x\n", pc->ledselect);
    soc_cm_print("automedium   %s\n", pc->automedium ? "Y" : "N");
    soc_cm_print("tbi_capable  %s\n", pc->tbi_capable ? "Y" : "N");
    soc_cm_print("medium       %x\n", pc->medium);
    soc_cm_print("fiber_detect %d\n", pc->fiber_detect);
    soc_cm_print("halfout      %d\n", pc->halfout);
    soc_cm_print("interface    %s\n", if_string[pc->interface]);
}

STATIC void
_soc_phyinfo_dump(int unit, soc_port_t port) 
{
    soc_cm_print("phy_id0      0x%04x\n", PHY_ID0_REG(unit, port));
    soc_cm_print("phy_id1      0x%04x\n", PHY_ID1_REG(unit, port));
    soc_cm_print("phy_addr     0x%02x\n", PHY_ADDR(unit, port));
    soc_cm_print("phy_addr_int 0x%02x\n", PHY_ADDR_INT(unit, port));
    soc_cm_print("phy_name     %s\n", PHY_NAME(unit, port));
    soc_cm_print("phy_flags    %s%s%s%s%s%s\n",
                  PHY_COPPER_MODE(unit, port) ?  "COPPER\t" : "",
                  PHY_FIBER_MODE(unit, port) ?  "FIBER\t" : "",
                  PHY_PASSTHRU_MODE(unit, port) ?  "PASSTHRU\t" : "",
                  PHY_TBI_MODE(unit, port) ? "TBI\t" : "",
                  PHY_FIBER_100FX_MODE(unit, port) ? "100FX\t" : "",
                  PHY_SGMII_AUTONEG_MODE(unit, port) ?  "SGMII_AN\t" : "");
    soc_cm_print("phy_flags    %s%s%s%s%s%s\n", 
                  PHY_WAN_MODE(unit, port) ? "WAN\t" : "",
                  PHY_EXTERNAL_MODE(unit, port) ? "EXTERNAL\t" : "",
                  PHY_MEDIUM_CHANGED(unit, port) ?  "MEDIUM_CHANGED\t" : "",
                  PHY_SERDES_FIBER_MODE(unit, port) ?  "SERDES_FIBER\t" : "",
                  PHY_FORCED_SGMII_MODE(unit, port) ? "FORCED_SGMII\t" : "",
                  PHY_FORCED_COPPER_MODE(unit, port) ? "FORCED_COPPER\t" : "");
    soc_cm_print("phy_flags    %s%s\n", 
                  PHY_CLAUSE45_MODE(unit, port) ? "C45\t" : "",
                  PHY_DISABLED_MODE(unit, port) ? "DISABLED" : "");
    soc_cm_print("an_timeout   %d\n", PHY_AN_TIMEOUT(unit, port));
}

void
soc_phyctrl_port_dump(int unit, soc_port_t port)
{
    phy_ctrl_t *pc;

    if (NULL == phy_port_info[unit]) {
        soc_cm_print("----------------------\n");
        soc_cm_print("PHY SW not initialized\n");
        soc_cm_print("----------------------\n");
    } else {
        _soc_phyinfo_dump(unit, port);
        pc = INT_PHY_SW_STATE(unit, port);
        if (NULL != pc) {
            soc_cm_print("--------------------\n");
            soc_cm_print("Internal PHY Control\n");
            soc_cm_print("--------------------\n");
            _soc_phyctrl_dump(pc);
        }
    
        pc = EXT_PHY_SW_STATE(unit, port);
        if (NULL != pc) {
            soc_cm_print("--------------------\n");
            soc_cm_print("External PHY Control\n");
            soc_cm_print("--------------------\n");
            _soc_phyctrl_dump(pc);
        }
    }
} 
    
