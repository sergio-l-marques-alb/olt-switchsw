/*
 * $Id: esw_trunk.c,v 1.1 2011/04/18 17:11:01 mruas Exp $
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
 * File:    trunk.c
 * Purpose: BCM level APIs for trunking (a.k.a. Port Aggregation)
 */

#include <sal/types.h>

#include <soc/mem.h>
#include <soc/debug.h>
#include <soc/drv.h>
#if defined(BCM_BRADLEY_SUPPORT)
#include <soc/bradley.h>
#endif /* BCM_BRADLEY_SUPPORT */

#include <bcm/error.h>
#include <bcm/mirror.h>
#include <bcm/trunk.h>

#include <bcm_int/common/lock.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw/trunk.h>
#include <bcm_int/esw/port.h>
#include <bcm_int/esw/mirror.h>
#include <bcm_int/esw/firebolt.h>

#include <bcm_int/esw_dispatch.h>

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
#include <appl/diag/system.h>
#include <appl/diag/parse.h>
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

/*
 * Accesses to the trunk hardware tables and software state
 * are made atomic using the TTR memory table lock.  The TTR lock
 * protects ALL related hardware tables, even if the particular chip
 * doesn't have a TTR table.
 */

#define TRUNK_LOCK(unit)	_bcm_esw_trunk_lock(unit)
#define TRUNK_UNLOCK(unit)	_bcm_esw_trunk_unlock(unit)

typedef struct trunk_cntl_s {
    int                ngroups_fp;  /* number of (front panel) trunk groups */
    int                nports_fp;   /* max ports per trunk group */
    int                ngroups_hg;  /* number of fabric trunk groups */
    int                nports_hg;   /* max ports per fabric trunk group */
    trunk_private_t   *t_info;
    sal_mutex_t        lock;        /* Trunk module lock. */
} trunk_cntl_t;

/*
 * One trunk control entry for each SOC device containing trunk book keeping
 * info for that device.
 */

static trunk_cntl_t bcm_trunk_control[BCM_MAX_NUM_UNITS];

#define TRUNK_CNTL(unit)	bcm_trunk_control[unit]
#define TRUNK_INFO(unit, tid)	bcm_trunk_control[unit].t_info[tid]

/*
 * Cause a routine to return BCM_E_INIT if trunking subsystem is not
 * initialized.
 */

#define TRUNK_INIT(unit)	                    \
	if ( (TRUNK_CNTL(unit).ngroups_fp <= 0) && \
	     (TRUNK_CNTL(unit).ngroups_hg <= 0) ) { return BCM_E_INIT; }

#define TRUNK_NUM_GROUPS(unit) \
        (TRUNK_CNTL(unit).ngroups_fp + TRUNK_CNTL(unit).ngroups_hg)

/*
 * Make sure TID is within valid range.
 */

#define TRUNK_CHECK(unit, tid) \
	if ((tid) < 0 || (tid) >= TRUNK_NUM_GROUPS(unit)) \
           { return BCM_E_PARAM; }

#define TRUNK_FP_TID(unit, tid) \
    ( ((tid) >= 0) && ((tid) < TRUNK_CNTL(unit).ngroups_fp) )

#define TRUNK_FABRIC_TID(unit, tid)             \
    ( ((tid) >= TRUNK_CNTL(unit).ngroups_fp) && \
      ((tid) < TRUNK_NUM_GROUPS(unit)) )

extern int _bcm_switch_module_type_get(int unit, bcm_module_t mod, 
                                       uint32 *mod_type);

/*
 * Function:
 *	_bcm_esw_trunk_gport_construct
 * Purpose:
 *	Converts ports and modules given in t_data structure into gports
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      fabric_trunk - trunk is a fabric trunk
 *      count - number of failover gports in list
 *      port_list - (IN) list of port numbers
 *      modid_list - (IN) list of module ids
 *      port_array - (OUT) list of gports to return
 *
 * Note:
 *      port_list and port_array may be the same list.  This updates
 *      the list in place.
 */
STATIC int
_bcm_esw_trunk_gport_construct(int unit, int fabric_trunk, int count,
                               bcm_port_t *port_list,
                               bcm_module_t *modid_list,
                               bcm_gport_t *port_array)
{
    bcm_gport_t         gport;
    int                 i, mod_is_local;
    _bcm_gport_dest_t   dest;

    _bcm_gport_dest_t_init(&dest);
    
    for (i = 0; i < count; i++) {
        gport = 0;
        /* Stacking ports should be encoded as devport */
        if (fabric_trunk) {
            /* Error checking during set functions should guarantee that
             * stack ports iff fabric trunks */
            dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_modid_is_local(unit, modid_list[i],
                                         &mod_is_local));
            if (mod_is_local && IS_ST_PORT(unit, port_list[i])) {
                dest.gport_type = _SHR_GPORT_TYPE_DEVPORT;
            } else {
                dest.gport_type = _SHR_GPORT_TYPE_MODPORT;
                dest.modid = modid_list[i];
            }
        }
        dest.port = port_list[i];
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_construct(unit, &dest, &gport));
        port_array[i] = gport;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_gport_array_resolve
 * Purpose:
 *	Converts gports list into ports and modules
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      fabric_trunk - trunk is a fabric trunk
 *      count - number of failover gports in list
 *      port_array - (IN) list of gports
 *      port_list - (OUT) list of port numbers to return
 *      modid_list - (OUT) list of module ids to return
 *
 * Note:
 *      port_list and port_array may be the same list.  This updates
 *      the list in place.
 */
STATIC int
_bcm_esw_trunk_gport_array_resolve(int unit, int fabric_trunk, int count,
                                   bcm_gport_t *port_array,
                                   bcm_port_t *port_list,
                                   bcm_module_t *modid_list)
                             
{
    bcm_port_t      port;
    bcm_module_t    modid, local_modid;
    bcm_trunk_t     tgid;
    int             i, id, rv;

    if (port_array != port_list) {
        /* For failover port lists */
        rv = bcm_esw_stk_my_modid_get(unit, &local_modid);
        if (BCM_FAILURE(rv) && !SOC_IS_XGS_FABRIC(unit) ) {
            return (rv);
        }
    }

    for (i = 0; i < count; i++) {
        if (BCM_GPORT_IS_SET(port_array[i])) {
            if (fabric_trunk) {
                /* Must be local */
                BCM_IF_ERROR_RETURN
                    (bcm_esw_port_local_get(unit, port_array[i], &port));
                port_list[i] = port;
                modid_list[i] = -1;
            } else {
                BCM_IF_ERROR_RETURN
                    (_bcm_esw_gport_resolve(unit, port_array[i],
                                            &modid, &port, &tgid, &id));
                if ((-1 != tgid) || (-1 != id)) {
                    return BCM_E_PARAM;
                }
                port_list[i] = port;
                modid_list[i] = modid;
            }
        } else {
            PORT_DUALMODID_VALID(unit, port_array[i]);
            if (port_array != port_list) {
                port_list[i] = port_array[i];
                modid_list[i] = local_modid;
            }
        }
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *	_bcm_esw_trunk_gport_resolve
 * Purpose:
 *	Converts gports given in t_data structure into ports and modules
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      t_data -   Information on the trunk group.
 */
int
_bcm_esw_trunk_gport_resolve(int unit, bcm_trunk_t tid,
                             bcm_trunk_add_info_t *t_data)
{
    return _bcm_esw_trunk_gport_array_resolve(unit,
                                              TRUNK_FABRIC_TID(unit, tid),
                                              t_data->num_ports, t_data->tp,
                                              t_data->tp, t_data->tm);
}

/*
 * Function:
 *	bcm_trunk_init
 * Purpose:
 *	Initializes the trunk tables to empty (no trunks configured)
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_init(int unit)
{
    trunk_private_t	*t_info;
    bcm_trunk_t		tid;

    TRUNK_CNTL(unit).ngroups_fp = 0;
    TRUNK_CNTL(unit).nports_fp = 0;
    TRUNK_CNTL(unit).ngroups_hg = 0;
    TRUNK_CNTL(unit).nports_hg = 0;

    /* Create protection mutex. */
    if (NULL == TRUNK_CNTL(unit).lock) {
        TRUNK_CNTL(unit).lock = sal_mutex_create("Trunk module mutex");
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {
        TRUNK_CNTL(unit).ngroups_fp = soc_mem_index_count(unit, TRUNK_GROUPm);
        TRUNK_CNTL(unit).nports_fp = BCM_SWITCH_TRUNK_MAX_PORTCNT;

	if (!SOC_WARM_BOOT(unit)) {
	    for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
		if (soc_mem_write(unit, TRUNK_GROUPm, MEM_BLOCK_ALL, tid,
				  soc_mem_entry_null(unit,
						     TRUNK_GROUPm)) < 0) {
		    return BCM_E_INTERNAL;
		}

		if (soc_mem_write(unit, TRUNK_BITMAPm, MEM_BLOCK_ALL, tid,
				  soc_mem_entry_null(unit,
						     TRUNK_BITMAPm)) < 0) {
		    return BCM_E_INTERNAL;
		}

                if (soc_feature(unit, soc_feature_trunk_egress)) {
                    if (soc_mem_write(unit, TRUNK_EGR_MASKm, MEM_BLOCK_ALL, tid,
                        soc_mem_entry_null(unit,
                                           TRUNK_EGR_MASKm)) < 0) {
                        return BCM_E_INTERNAL;
                    }
                }
	    }
	}
#if defined(BCM_XGS3_SWITCH_SUPPORT)
        if (SOC_IS_XGS3_SWITCH(unit)) {
            uint64 config64, oconfig64;
            soc_reg_t reg, hg_trunk_reg, hg_trunk_bmap;
            uint32 addr, hg_trunk_reg_addr = 0;
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            uint32 hg_trunk_bmap_addr;
#endif
            int rv;
#if defined(BCM_EASYRIDER_SUPPORT)
            uint32 config, oconfig;
#endif /* BCM_EASYRIDER_SUPPORT */
#if defined(BCM_TRIUMPH2_SUPPORT)
            hg_trunk_reg = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                            SOC_IS_VALKYRIE2(unit)) ? HIGIG_TRUNK_CONTROL_64r : 
                                                      HIGIG_TRUNK_CONTROLr;
            hg_trunk_bmap = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                             SOC_IS_VALKYRIE2(unit)) ? HG_TRUNK_BITMAP_64r : 
                                                       HG_TRUNK_BITMAPr;
#else /* BCM_TRIUMPH2_SUPPORT */
            hg_trunk_reg = HIGIG_TRUNK_CONTROLr;
            hg_trunk_bmap = HG_TRUNK_BITMAPr;
#endif /* BCM_TRIUMPH2_SUPPORT */

            if (SOC_WARM_BOOT(unit)) {
                if (SOC_IS_FBX(unit)) {
#if defined(BCM_TRX_SUPPORT)
                    reg = SOC_IS_TRX(unit) ? ING_CONFIG_64r : ING_CONFIGr;
#else /* BCM_TRX_SUPPORT */
                    reg = ING_CONFIGr;
#endif /* BCM_TRX_SUPPORT */
                    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                    SOC_IF_ERROR_RETURN(soc_reg_read(unit, reg, addr,
                                                     &config64));

                    if (!soc_reg64_field32_get(unit, reg, config64,
                                               TRUNKS128f)) {
                        TRUNK_CNTL(unit).ngroups_fp = 32;
                    }
		    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) {
			TRUNK_CNTL(unit).ngroups_hg = 2;
			TRUNK_CNTL(unit).nports_hg = 4;
		    }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
		    if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
			TRUNK_CNTL(unit).ngroups_hg =
			    SOC_REG_NUMELS(unit, hg_trunk_bmap);
#if defined(BCM_SCORPION_SUPPORT)
                        if (SOC_IS_SC_CQ(unit)) {
                            TRUNK_CNTL(unit).nports_hg =
                                BCM_TRUNK_MAX_PORTCNT;
                        } else
#endif /* BCM_SCORPION_SUPPORT */
                        {
                            TRUNK_CNTL(unit).nports_hg =
                                BCM_SWITCH_TRUNK_MAX_PORTCNT;
                        }
		    }
#endif /* BCM_BRADLEY_SUPPORT || BCM_TRX_SUPPORT */
		    if (SOC_IS_XGS3_FABRIC(unit)) {
                        TRUNK_CNTL(unit).ngroups_fp = 0;
                        TRUNK_CNTL(unit).nports_fp = 0;
		    }
                } 
#if defined(BCM_EASYRIDER_SUPPORT)
                else if (SOC_IS_EASYRIDER(unit)) {
                    SOC_IF_ERROR_RETURN(READ_SEER_CONFIGr(unit, &config));
                    if (!soc_reg_field_get(unit, SEER_CONFIGr,
                                           config, TRUNKS128f)) {
                        TRUNK_CNTL(unit).ngroups_fp = 32;
                    }
                }
#endif /* BCM_EASYRIDER_SUPPORT */
            } else {
                int trunk128;

                trunk128 = soc_property_get(unit, spn_TRUNK_EXTEND, 0);

                /* For Raptor TRUNKS128 field should alwyas be programmed to 1 */
                if (SOC_IS_RAPTOR(unit) || SOC_IS_RAVEN(unit)) {
                    trunk128 = 1;
                }

                SOC_IF_ERROR_RETURN(soc_mem_clear(unit, SOURCE_TRUNK_MAP_TABLEm,
                                                  MEM_BLOCK_ALL, 0));

                if (SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) { 

                    if (soc_feature(unit, soc_feature_hg_trunking)) {
                        uint64 val, hg_cntl_entry;
                        hg_trunk_reg_addr = soc_reg_addr(unit, hg_trunk_reg, 
                                                         REG_PORT_ANY, 0);   
                        SOC_IF_ERROR_RETURN(soc_reg_read(unit, hg_trunk_reg, 
                                                         hg_trunk_reg_addr,
                                                         &hg_cntl_entry));
                        val = soc_reg64_field_get(unit, hg_trunk_reg,
                                                  hg_cntl_entry, ACTIVE_PORT_BITMAPf);
                        COMPILER_64_ZERO(hg_cntl_entry);
                        soc_reg64_field_set(unit, hg_trunk_reg, &hg_cntl_entry,
                                            ACTIVE_PORT_BITMAPf, val);
                        SOC_IF_ERROR_RETURN(soc_reg_write(unit, hg_trunk_reg, 
                                                          hg_trunk_reg_addr,
                                                          hg_cntl_entry));
                    }
                    SOC_IF_ERROR_RETURN
                        (soc_mem_clear(unit, NONUCAST_TRUNK_BLOCK_MASKm,
                                       MEM_BLOCK_ALL, 0));
#if defined(BCM_TRX_SUPPORT)
                    reg = SOC_IS_TRX(unit) ? ING_CONFIG_64r : ING_CONFIGr;
#else /* BCM_TRX_SUPPORT */
                    reg = ING_CONFIGr;
#endif /* BCM_TRX_SUPPORT */
                    addr = soc_reg_addr(unit, reg, REG_PORT_ANY, 0);
                    SOC_IF_ERROR_RETURN(soc_reg_read(unit, reg, addr,
                                                     &config64));
                    oconfig64 = config64;
                    if (SOC_IS_TRX(unit)) {
                        soc_reg64_field32_set(unit, reg, &config64, TRUNKS128f, 1);
                    } else {
                        soc_reg64_field32_set(unit, reg, &config64, TRUNKS128f,
                                          trunk128 ? 1 : 0);
                    }
                    if (COMPILER_64_NE(config64, oconfig64)) {
                        SOC_IF_ERROR_RETURN(soc_reg_write(unit, reg, addr,
                                                          config64));
                    }

                    if ((SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit)) 
                        && soc_feature(unit, soc_feature_hg_trunking)) {
                        TRUNK_CNTL(unit).ngroups_hg = 2;
                        TRUNK_CNTL(unit).nports_hg = 4;
                        SOC_IF_ERROR_RETURN(WRITE_HIGIG_TRUNK_GROUPr(unit, 0));
                    }
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                    if (SOC_IS_HB_GW(unit) || SOC_IS_TRX(unit)) {
                        uint64      val64;

                        TRUNK_CNTL(unit).ngroups_hg =
                            SOC_REG_NUMELS(unit, hg_trunk_bmap);
#if defined(BCM_SCORPION_SUPPORT)
                        if (SOC_IS_SC_CQ(unit)) {
                            TRUNK_CNTL(unit).nports_hg =
                                BCM_TRUNK_MAX_PORTCNT;
                        } else
#endif /* BCM_SCORPION_SUPPORT */
                        {
                            TRUNK_CNTL(unit).nports_hg =
                                BCM_SWITCH_TRUNK_MAX_PORTCNT;
                        }
                        COMPILER_64_ZERO(val64);
                        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                            hg_trunk_bmap_addr = soc_reg_addr(unit, hg_trunk_bmap, 
                                                              REG_PORT_ANY, tid);
                            SOC_IF_ERROR_RETURN(soc_reg_write(unit, hg_trunk_bmap, hg_trunk_bmap_addr, val64));
                            SOC_IF_ERROR_RETURN
                                (WRITE_HG_TRUNK_GROUPr(unit, tid, val64));
                        }
                    }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
                }
#if defined(BCM_EASYRIDER_SUPPORT)
                else if (SOC_IS_EASYRIDER(unit)) {
                    int i, num_reg;
                    uint32 trk_blk;

                    num_reg = SOC_REG_NUMELS(unit, NONUCAST_TRUNK_BLOCK_MASKr);
                    trk_blk = 0;
                    for (i = 0; i < num_reg; i++) {
			SOC_IF_ERROR_RETURN
			    (WRITE_NONUCAST_TRUNK_BLOCK_MASKr(unit, i, trk_blk));
                    }

                    SOC_IF_ERROR_RETURN
                        (READ_SEER_CONFIGr(unit, &config));
                    oconfig = config;
                    soc_reg_field_set(unit, SEER_CONFIGr, &config,
                                      TRUNKS128f, (trunk128) ? 1 : 0);
                    if (config != oconfig) {
                        SOC_IF_ERROR_RETURN
                            (WRITE_SEER_CONFIGr(unit, config));
                    }
                }
#endif /* BCM_EASYRIDER_SUPPORT */

                if (!trunk128) {
                    if(SOC_IS_HAWKEYE(unit)){
                        TRUNK_CNTL(unit).ngroups_fp = 8;
                    } else {
                        TRUNK_CNTL(unit).ngroups_fp = 32;
                    }
                }
		if (SOC_IS_XGS3_FABRIC(unit)) {
		    TRUNK_CNTL(unit).ngroups_fp = 0;
		    TRUNK_CNTL(unit).nports_fp = 0;
		}
            }
            BCM_IF_ERROR_RETURN
               (_bcm_xgs3_trunk_swfailover_init(unit));
            rv = _bcm_xgs3_trunk_mod_port_map_init(unit);
            if (BCM_FAILURE(rv)) {
                _bcm_xgs3_trunk_swfailover_detach(unit);
                return rv;
            }

#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            if (soc_feature(unit, soc_feature_hg_trunk_failover) ||
                soc_feature(unit, soc_feature_port_lag_failover)) {
                _bcm_xgs3_trunk_hwfailover_init(unit);
            }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
        }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
	if (SOC_IS_DRACO15(unit)) {
	    bcm_port_t		port;
	    egr_mask_entry_t 	egr_mask_entry;
	    int 		index, index_min, index_max;
	    uint32         	configreg;

	    /*
	     * Set number of trunk groups based on TRUNKS128 mode.
	     */
	    if (SOC_WARM_BOOT(unit)) {
		PBMP_PORT_ITER(unit, port) {
		    SOC_IF_ERROR_RETURN(READ_CONFIGr(unit, port, &configreg));
		    break;
		}

		if (!soc_reg_field_get(unit, CONFIGr,
				       configreg, TRUNKS128f)) {
		    TRUNK_CNTL(unit).ngroups_fp = 32;
		}
	    } else {
		if (soc_property_get(unit, spn_TRUNK_EXTEND, 0)) {
		    uint32         configreg;

		    PBMP_PORT_ITER(unit, port) {
			SOC_IF_ERROR_RETURN
			    (READ_CONFIGr(unit, port, &configreg));
			soc_reg_field_set(unit, CONFIGr,
					  &configreg, TRUNKS128f, 1);
			SOC_IF_ERROR_RETURN
			    (WRITE_CONFIGr(unit, port, configreg));
		    }
		} else {
		    TRUNK_CNTL(unit).ngroups_fp = 32;
		}

                index_min = soc_mem_index_min(unit, EGR_MASKm);
                index_max = soc_mem_index_max(unit, EGR_MASKm);
		/*
		 * Initialize TGIDs in EGR_MASK table
		 */
		for (index = index_max; index >= index_min; index--) {
		    SOC_IF_ERROR_RETURN
			(soc_mem_read(unit, EGR_MASKm, MEM_BLOCK_ANY,
				      index, &egr_mask_entry));

		    if (soc_EGR_MASKm_field32_get(unit,
						  &egr_mask_entry, Tf)) {
			/*
			 * This modid/port entry was a trunk port, so
			 * reinit EGRESS_MASKf field for non-trunk use.
			 */
			soc_EGR_MASKm_field32_set(unit, &egr_mask_entry,
						  EGRESS_MASKf, 0);
		    }

		    soc_EGR_MASKm_field32_set(unit, &egr_mask_entry, Tf, 0);
		    soc_EGR_MASKm_field32_set(unit, &egr_mask_entry, TGIDf, 0);

		    SOC_IF_ERROR_RETURN
			(soc_mem_write(unit, EGR_MASKm, MEM_BLOCK_ALL,
				       index, &egr_mask_entry));
		}

		if (soc_feature(unit, soc_feature_ipmc_lookup)) {
		    mmu_memories2_egr_trunk_map_entry_t etm_entry;

		    /*
		     * Initialize SRC_TRUNK_MAPm and EGR_TRUNK_MAPm
		     */

		    SOC_IF_ERROR_RETURN
			(soc_mem_clear(unit,
				       MMU_MEMORIES2_SRC_TRUNK_MAPm,
				       MEM_BLOCK_ALL, 0));

		    PBMP_E_ITER(unit, port) {
			SOC_IF_ERROR_RETURN
			    (READ_MMU_MEMORIES2_EGR_TRUNK_MAPm(unit,
							       MEM_BLOCK_ALL,
							       port,
							       &etm_entry));
			soc_MMU_MEMORIES2_EGR_TRUNK_MAPm_field32_set
			    (unit, &etm_entry, TGIDf, 0);
			soc_MMU_MEMORIES2_EGR_TRUNK_MAPm_field32_set
			    (unit, &etm_entry, TRUNKf, 0);

			SOC_IF_ERROR_RETURN
			    (WRITE_MMU_MEMORIES2_EGR_TRUNK_MAPm(unit,
								MEM_BLOCK_ALL,
								port,
								&etm_entry));
		    }
		}
	    }
	}
    }
#endif	/* BCM_XGS_SWITCH_SUPPORT */

#ifdef	BCM_XGS12_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
	bcm_port_t	port;

        TRUNK_CNTL(unit).ngroups_hg = SOC_REG_NUMELS(unit, ING_HGTRUNKr);
        TRUNK_CNTL(unit).nports_hg = (SOC_IS_HERCULES1(unit)) ?
	    BCM_XGS1_FABRIC_TRUNK_MAX_PORTCNT :
	    BCM_XGS23_FABRIC_TRUNK_MAX_PORTCNT;

	if (!SOC_WARM_BOOT(unit)) {
	    for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
		PBMP_PORT_ITER(unit, port) {
		    SOC_IF_ERROR_RETURN
			(WRITE_ING_HGTRUNKr(unit, port, tid, 0));
		}
	    }
	}
    }
#endif	/* BCM_XGS12_FABRIC_SUPPORT */

    if (TRUNK_CNTL(unit).t_info != NULL) {
        sal_free(TRUNK_CNTL(unit).t_info);
        TRUNK_CNTL(unit).t_info = NULL;

    }

    if (TRUNK_NUM_GROUPS(unit) > 0) {
        TRUNK_CNTL(unit).t_info = 
            sal_alloc(TRUNK_NUM_GROUPS(unit) * sizeof(trunk_private_t),
                      "trunk_priv");
        if (NULL == TRUNK_CNTL(unit).t_info) {
#if defined(BCM_XGS3_SWITCH_SUPPORT)
            if (SOC_IS_XGS3_SWITCH(unit)) {
                _bcm_xgs3_trunk_swfailover_detach(unit);
                _bcm_xgs3_trunk_mod_port_map_deinit(unit);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
                _bcm_xgs3_trunk_hwfailover_detach(unit);
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
            }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
            return (BCM_E_MEMORY);
        }

	t_info = TRUNK_CNTL(unit).t_info;

	if (!SOC_WARM_BOOT(unit)) {
	    for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
		t_info->tid = BCM_TRUNK_INVALID;
		t_info->in_use = FALSE;
		t_info->dlf_index_spec = -1;
		t_info->dlf_index_used = -1;
		t_info->mc_index_spec = -1;
		t_info->mc_index_used = -1;
		t_info->ipmc_index_spec = -1;
		t_info->ipmc_index_used = -1;
		t_info++;
	    }
	}
    }

#if defined(BCM_XGS_FABRIC_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    if (SOC_WARM_BOOT(unit)) {
        return(_bcm_trunk_reinit(unit));
    }
#endif

    return BCM_E_NONE;
}


/*
 * Function:
 *	bcm_trunk_detach
 * Purpose:
 *	Cleans up the trunk tables.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_detach(int unit)
{
    /* Return if trunk was not initialized. */
    if (NULL == TRUNK_CNTL(unit).t_info) {
        return (BCM_E_NONE);
    }

    sal_free(TRUNK_CNTL(unit).t_info);
    /* Destroy protection mutex. */
    if (NULL != TRUNK_CNTL(unit).lock) {
        sal_mutex_destroy(TRUNK_CNTL(unit).lock);
        TRUNK_CNTL(unit).lock = NULL;
    }
    sal_memset(&TRUNK_CNTL(unit), 0, sizeof(trunk_cntl_t));
#if defined(BCM_XGS3_SWITCH_SUPPORT)
    if (SOC_IS_XGS3_SWITCH(unit)) {
        _bcm_xgs3_trunk_swfailover_detach(unit);
        _bcm_xgs3_trunk_mod_port_map_deinit(unit);
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
        _bcm_xgs3_trunk_hwfailover_detach(unit);
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_trunk_create
 * Purpose:
 *      Create the software data structure for a trunk ID.
 *      This function does not update any hardware tables,
 *      must call bcm_trunk_set() to finish trunk setting.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - (Out), The trunk ID.
 * Returns:
 *      BCM_E_NONE     - Success.
 *      BCM_E_BADID - run out of TIDs
 * Note:
 *      For devices that support both front panel and fabric
 *      trunking, this routine creates a front panel TID only.
 *      To create a fabric trunk on such devices,
 *      use bcm_trunk_create_id().
 */

int
bcm_esw_trunk_create(int unit, bcm_trunk_t *tid)
{
    trunk_private_t *t_info;
    int rv, i;
    bcm_trunk_t max_tid;

    TRUNK_INIT(unit);

    TRUNK_LOCK(unit);

    rv = BCM_E_PARAM;

    t_info = TRUNK_CNTL(unit).t_info;

    if (TRUNK_CNTL(unit).ngroups_fp == 0) {
        max_tid = TRUNK_CNTL(unit).ngroups_hg - 1;
    }
    else {
        max_tid = TRUNK_CNTL(unit).ngroups_fp - 1;
    }

    for (i = 0; i <= max_tid; i++) {
        if (t_info->tid == BCM_TRUNK_INVALID) {
            t_info->tid = i;
            t_info->in_use = FALSE;
            t_info->psc = BCM_TRUNK_PSC_DEFAULT;
            *tid = i;
            rv = BCM_E_NONE;
            break;
        }
        t_info++;
    }

    TRUNK_UNLOCK(unit);

    return rv;
}


/*
 * Function:
 *	_bcm_trunk_id_validate
 * Purpose:
 *  Service routine to validate validity of trunk id.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID.
 * Returns:
 *      BCM_E_NONE    - Success.
 *      BCM_E_INIT    - trunking software not initialized
 *      BCM_E_BADID   - TID out of range
 */
int
_bcm_trunk_id_validate(int unit, bcm_trunk_t tid)
{
    TRUNK_CHECK(unit, tid);
    return (BCM_E_NONE);
}

/*
 * Function:
 *	bcm_trunk_create_id
 * Purpose:
 *      Create the software data structure for the specified trunk ID.
 *      This function does not update any hardware tables,
 *      must call bcm_trunk_set() to finish trunk setting.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID.
 * Returns:
 *      BCM_E_NONE       - Success.
 *      BCM_E_INIT       - trunking software not initialized
 *      BCM_E_EXISTS - TID already used
 *      BCM_E_BADID   - TID out of range
 */

int
bcm_esw_trunk_create_id(int unit, bcm_trunk_t tid)
{
    trunk_private_t *t_info;
    int rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    TRUNK_LOCK(unit);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        t_info->tid = tid;
        t_info->psc = BCM_TRUNK_PSC_DEFAULT;
        t_info->in_use = FALSE;
        rv = BCM_E_NONE;
    } else {
	rv = BCM_E_EXISTS;
    }

    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_psc_set
 * Purpose:
 *      Set the trunk selection criteria.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      psc - Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_psc_set(int unit, bcm_trunk_t tid, int psc)
{
    trunk_private_t		*t_info;
    bcm_trunk_add_info_t	add_info;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    if (psc <= 0) {
	psc = BCM_TRUNK_PSC_DEFAULT;
    }

    if (t_info->psc == psc) {
        return BCM_E_NONE;
    }

    if (!t_info->in_use) {
	t_info->psc = psc;
	return BCM_E_NONE;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, &add_info));
    add_info.psc = psc;
    BCM_IF_ERROR_RETURN(bcm_esw_trunk_set(unit, tid, &add_info));
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_trunk_psc_get
 * Purpose:
 *      Get the trunk selection criteria.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be used.
 *      psc - (OUT) Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_psc_get(int unit, bcm_trunk_t tid, int *psc)
{
    trunk_private_t *t_info;
    int rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
	*psc = 0;
        rv = BCM_E_NOT_FOUND;
    } else {
	*psc = t_info->psc;
	rv = BCM_E_NONE;
    }

    return rv;
}

/*
 * Function:
 *	bcm_trunk_chip_info_get
 * Purpose:
 *      Get the trunk information.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      t_info - (OUT) Identify the trunk selection criteria.
 * Returns:
 *      BCM_E_NONE              Success.
 * Notes:
 *      None.
 */

int
bcm_esw_trunk_chip_info_get(int unit, bcm_trunk_chip_info_t *ta_info)
{
    TRUNK_INIT(unit);

    ta_info->trunk_group_count = TRUNK_CNTL(unit).ngroups_fp;

    if (TRUNK_CNTL(unit).ngroups_fp > 0) {
        ta_info->trunk_id_min = 0;
	ta_info->trunk_id_max = TRUNK_CNTL(unit).ngroups_fp - 1;
    }
    else {
        ta_info->trunk_id_min = -1;
	ta_info->trunk_id_max = -1;
    }
    ta_info->trunk_ports_max = TRUNK_CNTL(unit).nports_fp;

    if (TRUNK_CNTL(unit).ngroups_hg > 0) {
        ta_info->trunk_fabric_id_min = TRUNK_CNTL(unit).ngroups_fp;
	ta_info->trunk_fabric_id_max =
	  TRUNK_CNTL(unit).ngroups_fp + TRUNK_CNTL(unit).ngroups_hg - 1;
    }
    else {
        ta_info->trunk_fabric_id_min = -1;
	ta_info->trunk_fabric_id_max = -1;
    }
    ta_info->trunk_fabric_ports_max = TRUNK_CNTL(unit).nports_hg;

    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_trunk_set
 * Purpose:
 *      Adds ports to a trunk group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - The trunk ID to be affected.
 *      t_add_info - Information on the trunk group.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes:
 *      The existing ports in the trunk group will be replaced with new ones.
 */

int
bcm_esw_trunk_set(int unit, bcm_trunk_t tid, bcm_trunk_add_info_t *add_info)
{
    trunk_private_t *t_info;
    int rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }
    /* Check number of ports in trunk group */
    if ( add_info->num_ports < 1 ||
	 (TRUNK_FP_TID(unit, tid) &&
	  (add_info->num_ports > TRUNK_CNTL(unit).nports_fp)) ||
	 (TRUNK_FABRIC_TID(unit, tid) &&
	  (add_info->num_ports > TRUNK_CNTL(unit).nports_hg)) ) {
        return (BCM_E_PARAM);
    }

    BCM_IF_ERROR_RETURN(
        _bcm_esw_trunk_gport_resolve(unit, tid, add_info));

    TRUNK_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_trunk_set(unit, tid, add_info, t_info);
    if (rv >= 0) {
        t_info->psc = add_info->psc;
        t_info->in_use = TRUE;
    }

    TRUNK_UNLOCK(unit);

#ifdef BCM_XGS12_SWITCH_SUPPORT
#ifdef INCLUDE_L3
    /* reconcile L3 programming with VLAN tables' state */
    if (SOC_IS_XGS12_SWITCH(unit) && soc_feature(unit, soc_feature_l3)) {
        _bcm_xgs_l3_untagged_update(unit, BCM_VLAN_INVALID, tid, -1);
    }
#endif  /* INCLUDE_L3 */
#endif  /* BCM_XGS12_SWITCH_SUPPORT */

#if defined(BCM_XGS3_SWITCH_SUPPORT)
    /* reconcile Mirror to Port programming with trunk member ports. */
    if (SOC_IS_XGS3_SWITCH(unit) && BCM_SUCCESS(rv)) {
        rv = _bcm_xgs3_mirror_trunk_update(unit, tid);
    }
#endif /* BCM_XGS3_SWITCH_SUPPORT */

    return (rv);
}

/*
 * Function:
 *	bcm_trunk_destroy
 * Purpose:
 *	Removes a trunk group.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.
 * Returns:
 *      BCM_E_NONE     Success.
 *      BCM_E_XXX
 */

int
bcm_esw_trunk_destroy(int unit, bcm_trunk_t tid)
{
    trunk_private_t *t_info;
    int rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    if (!t_info->in_use) {
        t_info->tid = BCM_TRUNK_INVALID;
        return (BCM_E_NONE);
    }

    TRUNK_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_trunk_destroy(unit, tid, t_info);
    t_info->tid = BCM_TRUNK_INVALID;
    t_info->in_use = FALSE;
    t_info->psc = BCM_TRUNK_PSC_DEFAULT;
    t_info->dlf_index_spec = -1;
    t_info->dlf_index_used = -1;
    t_info->dlf_port_used = 0;
    t_info->mc_index_spec = -1;
    t_info->mc_index_used = -1;
    t_info->mc_port_used = 0;
    t_info->ipmc_index_spec = -1;
    t_info->ipmc_index_used = -1;
    t_info->ipmc_port_used = 0;
    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_get
 * Purpose:
 *      Return a port information of given trunk ID.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk ID.
 *      pbmp - (Out), Place to store bitmap of returned ports.
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_XXX
 * Notes:
 *      The bitmap returned always includes the port itself.
 */

int
bcm_esw_trunk_get(int unit, bcm_trunk_t tid, bcm_trunk_add_info_t *t_data)
{
    trunk_private_t *t_info;
    int             rv, isGport = 0;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    bcm_trunk_add_info_t_init(t_data);

    TRUNK_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_trunk_get(unit, tid, t_data, t_info);
    TRUNK_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(
        bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
    if (isGport) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_gport_construct(unit, TRUNK_FABRIC_TID(unit, tid),
                                           t_data->num_ports,
                                           t_data->tp, t_data->tm,
                                           t_data->tp));
    }

    return rv;
}

/*
 * Function:
 *	bcm_trunk_mcast_join
 * Purpose:
 *	Add the trunk group to existing MAC multicast entry.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.
 *      vid - Vlan ID.
 *      mac - MAC address.
 * Returns:
 *	BCM_E_XXX
 * Notes:
 *      Applications have to remove the MAC multicast entry and re-add in with
 *      new port bitmap to remove the trunk group from MAC multicast entry.
 */

int
bcm_esw_trunk_mcast_join(int unit, bcm_trunk_t tid, bcm_vlan_t vid, mac_addr_t mac)
{
    trunk_private_t *t_info;
    int	rv;

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    if (!t_info->in_use) {
        return (BCM_E_NONE);
    }

    TRUNK_LOCK(unit);
    rv = mbcm_driver[unit]->mbcm_trunk_mcast_join(unit, tid, vid, mac, t_info);
    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_bitmap_expand
 * Purpose:
 *      Given a port bitmap, if any of the ports are in a trunk,
 *      add all of the ports of that trunk to the bitmap.
 * Parameters:
 *      unit - StrataSwitch PCI device unit number (driver internal).
 *      pbmp_ptr - Input/output port bitmap
 * Returns:
 *      BCM_E_NONE              Success.
 *      BCM_E_INTERNAL          Chip access failure.
 * Notes:
 *      Port bitmaps in the vtable and ptable must contain only one of
 *      the trunk ports when multiple ports are aggregated in a trunk.
 *      This is so DLF flooding, etc. goes out only one trunk port.
 */

int
bcm_esw_trunk_bitmap_expand(int unit, pbmp_t *pbmp_ptr)
{
    int		    rv = BCM_E_NONE;
    bcm_trunk_t     tid;
    pbmp_t          tports, tpbm;

    TRUNK_LOCK(unit);

#ifdef	BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS_SWITCH(unit)) {
        pbmp_t    hg_pbmp, nonhg_pbmp;
        int       hg_trunk = FALSE;
        soc_reg_t hg_trunk_bmap;
#ifdef BCM_TRIUMPH2_SUPPORT
        hg_trunk_bmap = (SOC_IS_TRIUMPH2(unit) || SOC_IS_APOLLO(unit) || 
                         SOC_IS_VALKYRIE2(unit)) ? HG_TRUNK_BITMAP_64r : 
                                                   HG_TRUNK_BITMAPr;
#else
        hg_trunk_bmap = HG_TRUNK_BITMAPr;
#endif
        SOC_PBMP_ASSIGN(hg_pbmp, PBMP_ST_ALL(unit));
        SOC_PBMP_NEGATE(nonhg_pbmp, hg_pbmp);
        SOC_PBMP_AND(hg_pbmp, *pbmp_ptr);
        SOC_PBMP_AND(nonhg_pbmp, *pbmp_ptr);
        if (SOC_PBMP_NOT_NULL(hg_pbmp) && SOC_PBMP_NOT_NULL(nonhg_pbmp)) {
            TRUNK_UNLOCK(unit);
            return BCM_E_PARAM;
        } else if (SOC_PBMP_NOT_NULL(hg_pbmp)) {
            hg_trunk = TRUE;
        }

        if (hg_trunk && SOC_IS_FBX(unit)) {
            uint32     val;

            if ((SOC_IS_FB_FX_HX(unit) || SOC_IS_RAVEN(unit))  && 
                soc_feature(unit, soc_feature_hg_trunking)) {
                rv = READ_HIGIG_TRUNK_CONTROLr(unit, &val);
                if (rv >= 0) {
                    SOC_PBMP_CLEAR(tports);
                    SOC_PBMP_WORD_SET(tports, 0, soc_reg_field_get(unit,
                                                 HIGIG_TRUNK_CONTROLr, val,
                                                 HIGIG_TRUNK_BITMAP0f)<<24);
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }

                    SOC_PBMP_CLEAR(tports);
                    SOC_PBMP_WORD_SET(tports, 0, soc_reg_field_get(unit,
                                                 HIGIG_TRUNK_CONTROLr, val,
                                                 HIGIG_TRUNK_BITMAP1f)<<24);
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
                }
            } 
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
            else if (SOC_REG_IS_VALID(unit, hg_trunk_bmap)) {
                uint64 val64;
                uint32    hg_trunk_bmap_addr;
                for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
                    uint64    hg_trunk_bmap_f;
                    hg_trunk_bmap_addr = soc_reg_addr(unit, hg_trunk_bmap, 
                                                      REG_PORT_ANY, tid);
                    rv = soc_reg_read(unit, hg_trunk_bmap, hg_trunk_bmap_addr, &val64);
                    if (rv < 0) {
                        break;
                    }
                    SOC_PBMP_CLEAR(tports);
                    hg_trunk_bmap_f = soc_reg64_field_get(unit,
                                                          hg_trunk_bmap,    
                                                          val64,
                                                          HIGIG_TRUNK_BITMAPf);
                    SOC_PBMP_WORD_SET(tports,
                                      0,
                                      COMPILER_64_LO(hg_trunk_bmap_f));
                    SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
                    SOC_PBMP_AND(tpbm, tports);
                    if (SOC_PBMP_NOT_NULL(tpbm)) {
                        SOC_PBMP_OR(*pbmp_ptr, tports);
                    }
               }
            }
#endif /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
        } else {
	    trunk_bitmap_entry_t trunk_bitmap_entry;

            for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_fp; tid++) {
	        rv = READ_TRUNK_BITMAPm(unit, MEM_BLOCK_ANY, tid,
	    			        &trunk_bitmap_entry);
	        if (rv < 0) {
		    break;
	        }

	        SOC_PBMP_CLEAR(tports);
                if (SOC_IS_TUCANA(unit)) {
                    SOC_PBMP_WORD_SET(tports, 0,
		    	    soc_TRUNK_BITMAPm_field32_get(unit,
			    			          &trunk_bitmap_entry,
						          TRUNK_BITMAP_M0f));
                    SOC_PBMP_WORD_SET(tports, 1,
			    soc_TRUNK_BITMAPm_field32_get(unit,
						          &trunk_bitmap_entry,
						          TRUNK_BITMAP_M1f));
                } else {
#if defined(BCM_TRIUMPH_SUPPORT) || defined(BCM_RAPTOR_SUPPORT)
                    if (soc_feature(unit, soc_feature_register_hi) || 
                        SOC_IS_TR_VL(unit)) {
                        SOC_PBMP_WORD_SET(tports, 0, 
                            soc_TRUNK_BITMAPm_field32_get(unit,
                                      &trunk_bitmap_entry,
                                      TRUNK_BITMAP_LOf));
                        if(!SOC_IS_ENDURO(unit))
                            SOC_PBMP_WORD_SET(tports, 1, 
                                soc_TRUNK_BITMAPm_field32_get(unit,
                                          &trunk_bitmap_entry,
                                          TRUNK_BITMAP_HIf));
                    } else 
#endif /* BCM_TRIUMPH_SUPPORT || BCM_RAPTOR_SUPPORT */
                    {
                        SOC_PBMP_WORD_SET(tports, 0, 
                            soc_TRUNK_BITMAPm_field32_get(unit,
                                      &trunk_bitmap_entry,
                                      TRUNK_BITMAPf));
                    }
                }
	        SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
	        SOC_PBMP_AND(tpbm, tports);
                if (SOC_PBMP_NOT_NULL(tpbm)) {
                    SOC_PBMP_OR(*pbmp_ptr, tports);
                }
            }
        }
    }
#endif	/* BCM_XGS_SWITCH_SUPPORT */

#ifdef	BCM_XGS_FABRIC_SUPPORT
    if (SOC_IS_XGS12_FABRIC(unit)) {
        uint32     val;
        bcm_port_t    port;

        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
             PBMP_HG_ITER(unit, port) {
                 rv = READ_ING_HGTRUNKr(unit, port, tid, &val);
                 break;
             }
             if (rv < 0) {
                 break;
             }
            SOC_PBMP_CLEAR(tports);
            SOC_PBMP_WORD_SET(tports, 0, soc_reg_field_get(unit,
                              ING_HGTRUNKr, val, BMAPf));
            SOC_PBMP_ASSIGN(tpbm, *pbmp_ptr);
            SOC_PBMP_AND(tpbm, tports);
            if (SOC_PBMP_NOT_NULL(tpbm)) {
                SOC_PBMP_OR(*pbmp_ptr, tports);
            }
        }
    }
#endif	/* BCM_XGS_FABRIC_SUPPORT */

    TRUNK_UNLOCK(unit);

    return rv;
}

/*
 * Function:
 *	bcm_trunk_egress_set
 * Description:
 *	Set switching only to indicated ports from given trunk.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.  Negative trunk id means set all trunks.
 *	pbmp - bitmap of ports to allow egress.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_trunk_egress_set(int unit, bcm_trunk_t tid, bcm_pbmp_t pbmp)
{
    trunk_egr_mask_entry_t    tem_entry;
    bcm_trunk_t               tid_min, tid_max;

    TRUNK_INIT(unit);
    if (soc_feature(unit,soc_feature_trunk_egress)) {
        if (tid >= TRUNK_CNTL(unit).ngroups_fp) {
            return BCM_E_BADID;
        }

        if (!SOC_IS_XGS_SWITCH(unit)) {
            if (BCM_PBMP_EQ(pbmp, PBMP_ALL(unit))) {
                return BCM_E_NONE;
            } else {
                return BCM_E_UNAVAIL;
            }
        }

        sal_memset(&tem_entry, 0, sizeof(trunk_egr_mask_entry_t));

        BCM_PBMP_NEGATE(pbmp, pbmp);
        BCM_PBMP_AND(pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(pbmp, PBMP_LB(unit));
        if (SOC_IS_TUCANA(unit)) {
            soc_TRUNK_EGR_MASKm_field32_set(unit, &tem_entry,
                                            TRUNK_EGRESS_MASK_M0f,
                                            SOC_PBMP_WORD_GET(pbmp, 0));
            soc_TRUNK_EGR_MASKm_field32_set(unit, &tem_entry,
                                            TRUNK_EGRESS_MASK_M1f,
                                            SOC_PBMP_WORD_GET(pbmp, 1));
        } else {
            soc_mem_pbmp_field_set(unit, TRUNK_EGR_MASKm, &tem_entry,
                                   TRUNK_EGRESS_MASKf, &pbmp);
        }

        if (tid < 0) {
            tid_min = 0;
            tid_max = TRUNK_CNTL(unit).ngroups_fp - 1;
        } else if (TRUNK_INFO(unit, tid).tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        } else {
            tid_min = tid_max = tid;
        }

        for (tid = tid_min; tid <= tid_max; tid++) {
            if (TRUNK_INFO(unit, tid).tid == BCM_TRUNK_INVALID) {
                continue;
            }
            BCM_IF_ERROR_RETURN
                (WRITE_TRUNK_EGR_MASKm(unit, MEM_BLOCK_ALL, tid, &tem_entry));

        	if (SOC_IS_DRACO15(unit)) {
        	    /*
        	     * The EGR_MASKm table must also hold a copy of the pbmp.
        	     *
        	     * When TGID is directly supplied (SL stack ports), the
        	     * mask from the TRUNK_EGRESS_MASKm table is used.
        	     *
        	     * When modid/port information is supplied (by TRUNKS128
        	     * HiGig headers, and local ports), the mask from the
        	     * EGR_MASKm table is used.
        	     *
        	     * Extract info from TRUNK_GROUPm to get the modid/port
        	     * information to index the EGR_MASKm table.
        	     *
        	     * EGR_MASK T/TGID fields were previously initialized
        	     * by bcm_trunk_set().
        	     */
    
        	    bcm_trunk_add_info_t t_data;
        	    egr_mask_entry_t     egr_mask_entry;
        	    int                  rv, size, em_index, isGport;
    
        	    BCM_IF_ERROR_RETURN
        	      (bcm_esw_trunk_get(unit, tid, &t_data));

                BCM_IF_ERROR_RETURN
                    (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
                if (isGport) {
                    BCM_IF_ERROR_RETURN(
                        _bcm_esw_trunk_gport_resolve(unit, tid, &t_data));
                }

                /* num_ports = 7..0 for trunk sizes 8..1 */
                size = t_data.num_ports-1; 
    
        	    rv = BCM_E_NONE;
        	    soc_mem_lock(unit, EGR_MASKm);
        	    for (; size>=0; size--) {
        	        em_index = t_data.tm[size]*32 + t_data.tp[size];
        		rv = soc_mem_read(unit, EGR_MASKm, MEM_BLOCK_ANY,
        				  em_index, &egr_mask_entry);
        		if (!BCM_SUCCESS(rv)) {
        		    break;
        		}
        		soc_EGR_MASKm_field32_set(unit, &egr_mask_entry,
        					  EGRESS_MASKf,
        					  SOC_PBMP_WORD_GET(pbmp, 0));
        		rv = soc_mem_write(unit, EGR_MASKm, MEM_BLOCK_ALL,
        				  em_index, &egr_mask_entry);
        		if (!BCM_SUCCESS(rv)) {
        		    break;
        		}
        	    }
        	    soc_mem_unlock(unit, EGR_MASKm);
        	    if (!BCM_SUCCESS(rv)) {
        	        return rv;
        	    }
        	}
        }
    } else {
        return BCM_E_UNAVAIL;
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *	bcm_trunk_egress_get
 * Description:
 *	Retrieve bitmap of ports for which switching is enabled for trunk.
 * Parameters:
 *	unit - StrataSwitch PCI device unit number (driver internal).
 *      tid - Trunk Id.  Negative trunk id means choose any trunk.
 *	pbmp - (OUT) bitmap of ports where egress allowed.
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_trunk_egress_get(int unit, bcm_trunk_t tid, bcm_pbmp_t *pbmp)
{
    trunk_egr_mask_entry_t    tem_entry;
    bcm_pbmp_t                tem_pbmp;

    TRUNK_INIT(unit);
    if (soc_feature(unit,soc_feature_trunk_egress)) {
        if (tid >= TRUNK_CNTL(unit).ngroups_fp) {
            return BCM_E_BADID;
        }

        if (!SOC_IS_XGS_SWITCH(unit)) {
            BCM_PBMP_ASSIGN(*pbmp, PBMP_ALL(unit));
            return BCM_E_NONE;
        }

        if (tid < 0) {
    	tid = 0;
        }

        if (TRUNK_INFO(unit, tid).tid == BCM_TRUNK_INVALID) {
            return BCM_E_NOT_FOUND;
        }

        BCM_PBMP_CLEAR(tem_pbmp);

        BCM_IF_ERROR_RETURN
            (READ_TRUNK_EGR_MASKm(unit, MEM_BLOCK_ANY, tid, &tem_entry));

        if (SOC_IS_TUCANA(unit)) {
    	SOC_PBMP_WORD_SET(tem_pbmp, 0,
    			  soc_TRUNK_EGR_MASKm_field32_get(unit, &tem_entry,
    						  TRUNK_EGRESS_MASK_M0f));
    	SOC_PBMP_WORD_SET(tem_pbmp, 1,
    			  soc_TRUNK_EGR_MASKm_field32_get(unit, &tem_entry,
    						  TRUNK_EGRESS_MASK_M1f));
        } else {
            soc_mem_pbmp_field_get(unit, TRUNK_EGR_MASKm, &tem_entry,
                                   TRUNK_EGRESS_MASKf, &tem_pbmp);
        }
        BCM_PBMP_ASSIGN(*pbmp, PBMP_ALL(unit));
        BCM_PBMP_REMOVE(*pbmp, PBMP_LB(unit));
        BCM_PBMP_XOR(*pbmp, tem_pbmp);
    } else{
        return BCM_E_UNAVAIL;
    }

    return BCM_E_NONE;
}

#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
#define _TRUNK_OVERRIDE_CHECK(_u, _p, _tid) \
        TRUNK_INIT(_u); \
        if (!SOC_IS_HERCULES15(_u) && (!SOC_IS_FBX(_u) || \
        !soc_feature(_u, soc_feature_hg_trunk_override)))  \
           { return (BCM_E_UNAVAIL); } \
        if (!TRUNK_FABRIC_TID(_u, _tid)) \
           { return (BCM_E_PARAM); } \
        if ((_p >= 0) && !IS_PORT(_u, _p)) \
           { return (BCM_E_PARAM); }
#define _UCAST_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_UCm) || \
                _id > soc_mem_index_max(_u, MEM_UCm))) \
            { return (BCM_E_PARAM); } \
         } else { \
            if ((_id < soc_mem_index_min(_u, MODPORT_MAPm) || \
                _id > soc_mem_index_max(_u, MODPORT_MAPm))) \
             { return (BCM_E_PARAM); }}
#define _MCAST_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_MCm) || \
                _id > soc_mem_index_max(_u, MEM_MCm))) \
            { return (BCM_E_PARAM); } \
        } else { \
            if ((_id < soc_mem_index_min(_u, L2MCm) || \
                _id > soc_mem_index_max(_u, L2MCm))) \
             { return (BCM_E_PARAM); }}
#define _IPMC_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_IPMCm) || \
                _id > soc_mem_index_max(_u, MEM_IPMCm))) \
            { return (BCM_E_PARAM); } \
        } else { \
	    if ((_id < soc_mem_index_min(_u, L3_IPMCm) || \
                _id > soc_mem_index_max(_u, L3_IPMCm))) \
            { return (BCM_E_PARAM); }}
#define _VLAN_RANGE_CHECK(_u, _id) \
        if (SOC_IS_HERCULES15(_u)) { \
            if ((_id < soc_mem_index_min(_u, MEM_VIDm) || \
                _id > soc_mem_index_max(_u, MEM_VIDm))) \
            { return (BCM_E_PARAM); } \
        } else { \
            if ((_id < soc_mem_index_min(_u, VLAN_TABm) || \
                _id > soc_mem_index_max(_u, VLAN_TABm))) \
             { return (BCM_E_PARAM); }}
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */

/*
 * Function:
 *      bcm_trunk_override_ucast_set
 * Description:
 *      Set the trunk override over UC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      modid  - Module id.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ucast_set(int unit, bcm_port_t port,
                             bcm_trunk_t tid, int modid, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _UCAST_RANGE_CHECK(unit, modid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_uc_entry_t      uc;
        soc_field_t tf[] = {TRUNK0_OVER_UCf,TRUNK1_OVER_UCf,
                            TRUNK2_OVER_UCf,TRUNK3_OVER_UCf};
        int bk=-1, blk;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_UCm);
        SOC_MEM_BLOCK_ITER(unit, MEM_UCm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_UCm(unit, blk, modid, &uc);
            if (rv >= 0) {
                soc_MEM_UCm_field32_set(unit, &uc,
                                        tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_UCm(unit, blk, modid, &uc);
            }
        }
        soc_mem_unlock(unit, MEM_UCm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT


    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        modport_map_entry_t uc;
        uint32 val, hgo_bit;
        int i, min, max, idx, modid_count;

	modid_count = SOC_MODID_MAX(unit) + 1;
        min = soc_mem_index_min(unit, MODPORT_MAPm) / modid_count;
        max = soc_mem_index_max(unit, MODPORT_MAPm) / modid_count;
        if (SOC_IS_HBX(unit)) {
            if (port >= min && port <= max) {
                min = max = port;
            }
        }

        soc_mem_lock(unit, MODPORT_MAPm);
        for (i = min; i <= max; i++) {
            idx = i * modid_count + modid;
            rv = READ_MODPORT_MAPm(unit, MEM_BLOCK_ANY, idx, &uc);
            if (rv >= 0) {
                val = soc_MODPORT_MAPm_field32_get(unit, &uc,
                                                   HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                if (enable) {
                    val |= hgo_bit;
                } else {
                    val &= ~hgo_bit;
                }
                soc_MODPORT_MAPm_field32_set(unit, &uc,
                                             HIGIG_TRUNK_OVERRIDEf, val);
                rv = WRITE_MODPORT_MAPm(unit, MEM_BLOCK_ALL, idx, &uc);
            }
        }
        soc_mem_unlock(unit, MODPORT_MAPm);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_ucast_get
 * Description:
 *      Get the trunk override over UC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      modid  - Module id.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ucast_get(int unit, bcm_port_t port,
                             bcm_trunk_t tid, int modid, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }
    
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _UCAST_RANGE_CHECK(unit, modid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_uc_entry_t      uc;
        soc_field_t tf[] = {TRUNK0_OVER_UCf,TRUNK1_OVER_UCf,
                            TRUNK2_OVER_UCf,TRUNK3_OVER_UCf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_UCm(unit, blk, modid, &uc);
        if (rv >= 0) {
            *enable = soc_MEM_UCm_field32_get(unit, &uc, tf[tid]);
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT


    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        modport_map_entry_t uc;
        uint32 val, hgo_bit;
        int idx;

        if (SOC_IS_FB_FX_HX(unit) || SOC_IS_TR_VL(unit)) {
            idx = modid;
        } else {
            idx = (port * (SOC_MODID_MAX(unit) + 1)) + modid;
        }

        *enable = 0;

        rv = READ_MODPORT_MAPm(unit, MEM_BLOCK_ANY, idx, &uc);
        if (rv >= 0) {
            val = soc_MODPORT_MAPm_field32_get(unit, &uc,
                                               HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            *enable = (val & hgo_bit) ? 1 : 0;
        }
     }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_mcast_set
 * Description:
 *      Set the trunk override over MC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      idx    - MC index carried in HiGig header.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_mcast_set(int unit, bcm_port_t port,
                             bcm_trunk_t tid, int idx, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_module_t    modid;
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _MCAST_RANGE_CHECK(unit, idx);

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_mc_entry_t      mc;
        soc_field_t tf[] = {TRUNK0_OVER_MCf,TRUNK1_OVER_MCf,
                            TRUNK2_OVER_MCf,TRUNK3_OVER_MCf};
        int  bk=-1, blk;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_MCm);
        SOC_MEM_BLOCK_ITER(unit, MEM_MCm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_MCm(unit, blk, idx, &mc);
            if (rv >= 0) {
                soc_MEM_MCm_field32_set(unit, &mc,
                                        tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_MCm(unit, blk, idx, &mc);
            }
        }
        soc_mem_unlock(unit, MEM_MCm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT


    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        l2mc_entry_t mc;
        uint32 val, hgo_bit;

        soc_mem_lock(unit, L2MCm);
        rv = READ_L2MCm(unit, MEM_BLOCK_ANY, idx, &mc);
        if (rv >= 0) {
            val = soc_L2MCm_field32_get(unit, &mc,
                                        HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            if (enable) {
                val |= hgo_bit;
            } else {
                val &= ~hgo_bit;
            }
            soc_L2MCm_field32_set(unit, &mc,
                                  HIGIG_TRUNK_OVERRIDEf, val);
            rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, idx, &mc);
        }
        soc_mem_unlock(unit, L2MCm);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_mcast_get
 * Description:
 *      Get the trunk override over MC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      idx    - MC index carried in HiGig header.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_mcast_get(int unit, bcm_port_t port,
                             bcm_trunk_t tid, int idx, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (BCM_GPORT_IS_SET(port)) {
        bcm_module_t    modid;
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _MCAST_RANGE_CHECK(unit, idx);

#ifdef BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_mc_entry_t      mc;
        soc_field_t tf[] = {TRUNK0_OVER_MCf,TRUNK1_OVER_MCf,
                            TRUNK2_OVER_MCf,TRUNK3_OVER_MCf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_MCm(unit, blk, idx, &mc);
        if (rv >= 0) {
            *enable = soc_MEM_MCm_field32_get(unit, &mc, tf[tid]);
        }
    }
#endif /* SOC_IS_HERCULES15 */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override))) { 
        l2mc_entry_t mc;
        uint32 val, hgo_bit;

        *enable = 0;

        rv = READ_L2MCm(unit, MEM_BLOCK_ANY, idx, &mc);
        if (rv >= 0) {
            val = soc_L2MCm_field32_get(unit, &mc,
                                        HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            *enable = (val & hgo_bit) ? 1 : 0;
        }
     }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_ipmc_set
 * Description:
 *      Set the trunk override over IPMC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      idx    - IPMC index carried in HiGig header.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ipmc_set(int unit, bcm_port_t port,
                            bcm_trunk_t tid, int idx, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        return BCM_E_UNAVAIL;
    }

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_module_t    modid;
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }
    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _IPMC_RANGE_CHECK(unit, idx);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_ipmc_entry_t      ipmc;
        soc_field_t tf[] = {TRUNK0_OVER_IPMCf,TRUNK1_OVER_IPMCf,
                            TRUNK2_OVER_IPMCf,TRUNK3_OVER_IPMCf};
        int bk=-1, blk;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_IPMCm);
        SOC_MEM_BLOCK_ITER(unit, MEM_IPMCm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_IPMCm(unit, blk, idx, &ipmc);
            if (rv >= 0) {
                soc_MEM_IPMCm_field32_set(unit, &ipmc,
                                          tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_IPMCm(unit, blk, idx, &ipmc);
            }
        }
        soc_mem_unlock(unit, MEM_IPMCm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT
    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        ipmc_entry_t ipmc;
        uint32 val, hgo_bit;

        soc_mem_lock(unit, L3_IPMCm);
        rv = READ_L3_IPMCm(unit, MEM_BLOCK_ANY, idx, &ipmc);
        if (rv >= 0) {
            val = soc_L3_IPMCm_field32_get(unit, &ipmc,
                                           HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            if (enable) {
                val |= hgo_bit;
            } else {
                val &= ~hgo_bit;
            }
            soc_L3_IPMCm_field32_set(unit, &ipmc,
                                     HIGIG_TRUNK_OVERRIDEf, val);
            rv = WRITE_L3_IPMCm(unit, MEM_BLOCK_ALL, idx, &ipmc);
        }
        soc_mem_unlock(unit, L3_IPMCm);

#if defined(BCM_BRADLEY_SUPPORT)
        if (SOC_IS_HBX(unit)) {
            l2mc_entry_t mc;
            uint32 val, hgo_bit;
            int	mc_base, mc_size, mc_index;

            SOC_IF_ERROR_RETURN
                (soc_hbx_ipmc_size_get(unit, &mc_base, &mc_size));
            if (idx < 0 || idx > mc_size) {
                return BCM_E_PARAM;
            }
            mc_index = idx + mc_base;

            soc_mem_lock(unit, L2MCm);
            rv = READ_L2MCm(unit, MEM_BLOCK_ANY, mc_index, &mc);
            if (rv >= 0) {
                val = soc_L2MCm_field32_get(unit, &mc,
                                            HIGIG_TRUNK_OVERRIDEf);
                hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
                if (enable) {
                    val |= hgo_bit;
                } else {
                    val &= ~hgo_bit;
                }
                soc_L2MCm_field32_set(unit, &mc,
                                      HIGIG_TRUNK_OVERRIDEf, val);
                rv = WRITE_L2MCm(unit, MEM_BLOCK_ALL, mc_index, &mc);
            }
            soc_mem_unlock(unit, L2MCm);

            
        }
#endif /* BCM_BRADLEY_SUPPORT */
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_ipmc_get
 * Description:
 *      Get the trunk override over IPMC.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      idx    - IPMC index carried in HiGig header.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_ipmc_get(int unit, bcm_port_t port,
                            bcm_trunk_t tid, int idx, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (!soc_feature(unit, soc_feature_ip_mcast)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        bcm_module_t    modid;
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _IPMC_RANGE_CHECK(unit, idx);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_ipmc_entry_t      ipmc;
        soc_field_t tf[] = {TRUNK0_OVER_IPMCf,TRUNK1_OVER_IPMCf,
                            TRUNK2_OVER_IPMCf,TRUNK3_OVER_IPMCf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_IPMCm(unit, blk, idx, &ipmc);
        if (rv >= 0) {
            *enable = soc_MEM_IPMCm_field32_get(unit, &ipmc, tf[tid]);
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        ipmc_entry_t ipmc;
        uint32 val, hgo_bit;

        *enable = 0;

        rv = READ_L3_IPMCm(unit, MEM_BLOCK_ANY, idx, &ipmc);
        if (rv >= 0) {
            val = soc_L3_IPMCm_field32_get(unit, &ipmc,
                                           HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            *enable = (val & hgo_bit) ? 1 : 0;
        }
     }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT  || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_vlan_set
 * Description:
 *      Set the trunk override over VLAN.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number, -1 to all ports.
 *      tid    - Trunk id.
 *      vid    - VLAN id.
 *      enable - TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_vlan_set(int unit, bcm_port_t port,
                            bcm_trunk_t tid, bcm_vlan_t vid, int enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_module_t    modid;
            bcm_trunk_t     tgid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port)) {
                return BCM_E_PORT;
            }
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _VLAN_RANGE_CHECK(unit, vid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_vid_entry_t      ve;
        soc_field_t tf[] = {TRUNK0_OVER_VIDf,TRUNK1_OVER_VIDf,
                            TRUNK2_OVER_VIDf,TRUNK3_OVER_VIDf};
        int blk, bk = -1;

        if (port >= 0) {
            bk = SOC_PORT_BLOCK(unit, port);
        }

        soc_mem_lock(unit, MEM_VIDm);
        SOC_MEM_BLOCK_ITER(unit, MEM_VIDm, blk) {
            if ((port >= 0) && (bk != blk)) {
                continue;
            }
            rv = READ_MEM_VIDm(unit, blk, vid, &ve);
            if (rv >= 0) {
                soc_MEM_VIDm_field32_set(unit, &ve,
                                         tf[tid], enable ? 1: 0);
                rv = WRITE_MEM_VIDm(unit, blk, vid, &ve);
            }
        }
        soc_mem_unlock(unit, MEM_VIDm);
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) { 
        vlan_tab_entry_t ve;
        uint32 val, hgo_bit;

        soc_mem_lock(unit, VLAN_TABm);
        rv = READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &ve);
        if (rv >= 0) {
            val = soc_VLAN_TABm_field32_get(unit, &ve,
                                            HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            if (enable) {
                val |= hgo_bit;
            } else {
                val &= ~hgo_bit;
            }
            soc_VLAN_TABm_field32_set(unit, &ve,
                                      HIGIG_TRUNK_OVERRIDEf, val);
            rv = WRITE_VLAN_TABm(unit, MEM_BLOCK_ALL, vid, &ve);
        }
        soc_mem_unlock(unit, VLAN_TABm);
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_override_vlan_get
 * Description:
 *      Get the trunk override over VLAN.
 * Parameters:
 *      unit   - StrataSwitch PCI device unit number (driver internal).
 *      port   - Port number.
 *      tid    - Trunk id.
 *      vid    - VLAN id.
 *      enable - (OUT) TRUE if enabled, FALSE if disabled.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_override_vlan_get(int unit, bcm_port_t port,
                            bcm_trunk_t tid, bcm_vlan_t vid, int *enable)
{
#if defined(BCM_HERCULES15_SUPPORT) || defined(BCM_FIREBOLT_SUPPORT)
    int rv=BCM_E_NONE;

    if (BCM_GPORT_IS_SET(port)) {
        bcm_module_t    modid;
        bcm_trunk_t     tgid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port)) {
            return BCM_E_PORT;
        }
    }

    _TRUNK_OVERRIDE_CHECK(unit, port, tid);
    _VLAN_RANGE_CHECK(unit, vid);

#ifdef  BCM_HERCULES15_SUPPORT
    if (SOC_IS_HERCULES15(unit)) {
        mem_vid_entry_t      ve;
        soc_field_t tf[] = {TRUNK0_OVER_VIDf,TRUNK1_OVER_VIDf,
                            TRUNK2_OVER_VIDf,TRUNK3_OVER_VIDf};
        int blk;

        *enable = 0;

        blk = SOC_PORT_BLOCK(unit, port);
        rv = READ_MEM_VIDm(unit, blk, vid, &ve);
        if (rv >= 0) {
            *enable = soc_MEM_VIDm_field32_get(unit, &ve, tf[tid]);
        }
    }
#endif /* BCM_HERCULES15_SUPPORT */

#ifdef BCM_FIREBOLT_SUPPORT

    if ((SOC_IS_FBX(unit) || SOC_IS_RAVEN(unit)) && 
        (soc_feature(unit,soc_feature_hg_trunk_override)) ) {         
        vlan_tab_entry_t ve;
        uint32 val, hgo_bit;

        *enable = 0;

        rv = READ_VLAN_TABm(unit, MEM_BLOCK_ANY, vid, &ve);
        if (rv >= 0) {
            val = soc_VLAN_TABm_field32_get(unit, &ve,
                                            HIGIG_TRUNK_OVERRIDEf);
            hgo_bit = 1 << (tid - TRUNK_CNTL(unit).ngroups_fp);
            *enable = (val & hgo_bit) ? 1 : 0;
        }
     }
#endif /* BCM_FIREBOLT_SUPPORT */

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT || BCM_FIREBOLT_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_pool_set
 * Description:
 *      Set trunk pool table that contains the egress port number
 *      indexed by the hash value.
 * Parameters:
 *      unit    - StrataSwitch PCI device unit number (driver internal).
 *      port    - Port number, -1 to all ports.
 *      tid     - Trunk id.
 *      size    - Trunk pool size.
 *      weights - Weights for each port, all 0 means weighted fair.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_pool_set(int unit, bcm_port_t port, bcm_trunk_t tid,
                   int size, const int weights[BCM_TRUNK_MAX_PORTCNT])
{
#ifdef  BCM_HERCULES15_SUPPORT
    uint32 val;
    int idx_min, idx_max, weighted, p, idx;
    mem_trunk_port_pool_entry_t trk_pool;
    trunk_private_t *t_info;
    bcm_trunk_add_info_t add_info;
    soc_field_t pf[] = {PORT_NO_0f, PORT_NO_1f, PORT_NO_2f, PORT_NO_3f};
    int token[BCM_TRUNK_MAX_PORTCNT], index;
    int i, reset=0, blk, rv=BCM_E_NONE, lp=0, isGport;

    if (!SOC_IS_HERCULES15(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (port >= 0) {
        if (BCM_GPORT_IS_SET(port)) {
            bcm_trunk_t     tgid;
            bcm_module_t    modid;
            int             id;

            BCM_IF_ERROR_RETURN(
                _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
            if ((-1 != id) || (-1 != tgid)){
                return BCM_E_PARAM;
            }
        } else {
            if (!SOC_PORT_VALID(unit, port) || !IS_HG_PORT(unit, port) ) {
                return BCM_E_PORT;
            }
        }
    }

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    idx_min = soc_mem_index_min(unit, MEM_TRUNK_PORT_POOLm);
    idx_max = soc_mem_index_max(unit, MEM_TRUNK_PORT_POOLm);
    if ((size < idx_min) || (size > idx_max)) {
        return BCM_E_PARAM;
    }
    if (size == 0) {
        index = size = idx_max;
    } else {
        index = size - 1;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, &add_info));
    if (add_info.num_ports <= 0) {
        return BCM_E_NOT_FOUND;
    }

    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
    if (isGport) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_gport_resolve(unit, tid, &add_info));
    }

    for (i=0; i < add_info.num_ports; i++) {
         lp |= weights[add_info.tp[i]-1];
         if ((lp & ~0x3f) != 0) {
             return BCM_E_PARAM;
         }
    }
    weighted = (lp) ? 1 : 0;

    soc_mem_lock(unit, MEM_TRUNK_PORT_POOLm);
    PBMP_PORT_ITER(unit, p) {
        if ((port >= 0) && (p != port)) {
            continue;
        }

        if ((rv = READ_ING_HGTRUNKr(unit, p, tid, &val)) >= 0) {
            soc_reg_field_set(unit, ING_HGTRUNKr, &val,
                              TRUNK_POOL_SIZEf, size);
            rv = WRITE_ING_HGTRUNKr(unit, p, tid, val);
        }
        if (rv < 0) {
            break;
        }

        if (weighted) {
            for (i = 0; i < add_info.num_ports; i++) {
                 token[add_info.tp[i]-1] = weights[add_info.tp[i]-1];
            }
        }

        idx = index;
        blk = SOC_PORT_BLOCK(unit, p);

        while (idx >= 0) {
            if (weighted) {
                for (i = 0; i < add_info.num_ports; i++) {
                    if (token[add_info.tp[i]-1]) {
                        reset = 0;
                        break;
                    }
                    reset = 1;
                }
                if (reset) {
                    for (i = 0; i < add_info.num_ports; i++) {
                        token[add_info.tp[i]-1] = weights[add_info.tp[i]-1];
                    }
                }
            }
            for (i = 0; i < add_info.num_ports; i++) {
                if (idx < 0) break;
                sal_memset(&trk_pool, 0, sizeof(trk_pool));
                if (weighted) {
                    if (token[add_info.tp[i]-1]) {
                        if ((rv = READ_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx, &trk_pool)) >= 0) {
                            soc_MEM_TRUNK_PORT_POOLm_field32_set(unit, &trk_pool, pf[tid], add_info.tp[i]);
                            rv = WRITE_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx,  &trk_pool);
                        }
                        token[add_info.tp[i]-1]--;
                        idx--;
                    } else {
                      continue;
                    }
                } else {
                    if ((rv = READ_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx, &trk_pool)) >= 0) {
                        soc_MEM_TRUNK_PORT_POOLm_field32_set(unit, &trk_pool, pf[tid], add_info.tp[i]);
                        rv = WRITE_MEM_TRUNK_PORT_POOLm(unit, blk, index-idx,  &trk_pool);
                    }
                    idx--;
                }
            }
        }
    }
    soc_mem_unlock(unit, MEM_TRUNK_PORT_POOLm);

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT */
}

/*
 * Function:
 *      bcm_trunk_pool_get
 * Description:
 *      Get trunk pool table that contains the egress port number
 *      indexed by the hash value.
 * Parameters:
 *      unit    - StrataSwitch PCI device unit number (driver internal).
 *      port    - Port number.
 *      tid     - Trunk id.
 *      size    - (OUT) Trunk pool size.
 *      weights - (OUT) Weights (total count) for each port.
 * Returns:
 *      BCM_E_xxxx
 */

int
bcm_esw_trunk_pool_get(int unit, bcm_port_t port, bcm_trunk_t tid,
                   int *size, int weights[BCM_TRUNK_MAX_PORTCNT])
{
#ifdef  BCM_HERCULES15_SUPPORT
    uint32 val;
    mem_trunk_port_pool_entry_t trk_pool;
    trunk_private_t *t_info;
    bcm_trunk_add_info_t add_info;
    soc_field_t pf[] = {PORT_NO_0f, PORT_NO_1f, PORT_NO_2f, PORT_NO_3f};
    int i,pool, idx, blk, pool_port, isGport, rv=BCM_E_NONE;

    if (!SOC_IS_HERCULES15(unit)) {
        return BCM_E_UNAVAIL;
    }

    if (BCM_GPORT_IS_SET(port)) {
        bcm_trunk_t     tgid;
        bcm_module_t    modid;
        int             id;

        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &modid, &port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PARAM;
        }
    } else {
        if (!SOC_PORT_VALID(unit, port) || !IS_HG_PORT(unit, port) ) {
            return BCM_E_PORT;
        }
    }

    TRUNK_INIT(unit);
    TRUNK_CHECK(unit, tid);
    t_info = &TRUNK_INFO(unit, tid);
    if (t_info->tid == BCM_TRUNK_INVALID) {
        return BCM_E_NOT_FOUND;
    }

    *size = 0;
    for (idx=0; idx < BCM_TRUNK_MAX_PORTCNT; idx++) {
         weights[idx] = 0;
    }

    SOC_IF_ERROR_RETURN(READ_ING_HGTRUNKr(unit, port, tid, &val));
    *size = soc_reg_field_get(unit, ING_HGTRUNKr, val, TRUNK_POOL_SIZEf);
    if (*size == 0) { /* HW meaning, we never program 0 above in _set */
        *size = 1;
    }

    BCM_IF_ERROR_RETURN(bcm_esw_trunk_get(unit, tid, &add_info));

    BCM_IF_ERROR_RETURN
        (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
    if (isGport) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_trunk_gport_resolve(unit, tid, &add_info));
    }

    blk = SOC_PORT_BLOCK(unit, port);
    pool = *size;
    for (idx = 0; idx < pool; idx++) {
         if ((rv = READ_MEM_TRUNK_PORT_POOLm(unit, blk, idx, &trk_pool)) >= 0) {
              pool_port = soc_MEM_TRUNK_PORT_POOLm_field32_get(unit, &trk_pool, pf[tid]);
              for (i=0; i < add_info.num_ports; i++) {
                   if (pool_port == add_info.tp[i]) {
                       weights[add_info.tp[i]-1]++;
                       break;
                   }
              }
         }
    }

    return rv;
#else
    return BCM_E_UNAVAIL;
#endif  /* BCM_HERCULES15_SUPPORT */
}

int _bcm_nuc_tpbm_get(int unit,
                      int num_ports,
                      bcm_module_t tm[BCM_TRUNK_MAX_PORTCNT],
                      uint32 *nuc_tpbm)
{
    int i, mod = -1;
    uint32 mod_type;
    int all_equal = 1;
    uint32 xgs12_tpbm = 0;
    uint32 xgs3_tpbm = 0;
    uint32 unknown_tpbm = 0;

    *nuc_tpbm = 0x1;
    for (i = 0; i < num_ports; i++) {
        if (i == 0) {
            mod = tm[i];
        } else if (mod != tm[i]) {
            all_equal = 0;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_switch_module_type_get(unit, tm[i], &mod_type));
        switch(mod_type) {
            case BCM_SWITCH_MODULE_XGS1   :
            case BCM_SWITCH_MODULE_XGS2   : xgs12_tpbm |= (1 << i); break;
            case BCM_SWITCH_MODULE_XGS3   : xgs3_tpbm |= (1 << i); break;
            case BCM_SWITCH_MODULE_UNKNOWN:
            default                       : unknown_tpbm |= (1 << i); break;
        }
    }
    if (all_equal) {
        *nuc_tpbm = ((0x1 << num_ports) - 1);
    } else if (unknown_tpbm || xgs12_tpbm) {
        *nuc_tpbm = 0x1;
    } else if (xgs3_tpbm) {
        *nuc_tpbm = xgs3_tpbm;
    }

    return(BCM_E_NONE);
}

/*
 * Function:
 *      bcm_trunk_find
 * Description:
 *      Get trunk id that contains the given system port
 * Parameters:
 *      unit    - StrataSwitch PCI device unit number (driver internal)
 *      modid   - Module ID
 *      port    - Port number
 *      tid     - (OUT) Trunk id
 * Returns:
 *      BCM_E_xxxx
 */
int
bcm_esw_trunk_find(int unit, bcm_module_t modid, bcm_port_t port,
                   bcm_trunk_t *tid)
{
    int          rv = BCM_E_NONE;
    bcm_module_t hw_mod;
    bcm_port_t   hw_port;
    int          tgid, id;

    TRUNK_INIT(unit);

    if (tid == NULL) {
        return BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SET(port)) {
        BCM_IF_ERROR_RETURN(
            _bcm_esw_gport_resolve(unit, port, &hw_mod, &hw_port, &tgid, &id));
        if ((-1 != id) || (-1 != tgid)){
            return BCM_E_PORT;
        }
    } else {
        PORT_DUALMODID_VALID(unit, port);
    BCM_IF_ERROR_RETURN
        (bcm_esw_stk_modmap_map(unit, BCM_STK_MODMAP_SET,
                                modid, port,
                                &hw_mod, &hw_port));

    if (!SOC_MODID_ADDRESSABLE(unit, hw_mod)) {
        return BCM_E_BADID;
    }

    if (!SOC_PORT_ADDRESSABLE(unit, hw_port)) {
        return BCM_E_PORT;
    }
    }

    if (SOC_IS_XGS3_SWITCH(unit)) {
  	return(_bcm_xgs3_trunk_get_port_property(unit, 
                                                 hw_mod, 
                                                 hw_port, 
                                                 tid)); 
    } else {
        int idx = 0, isGport;
        bcm_trunk_add_info_t t_data;

        for (idx = 0; idx < TRUNK_NUM_GROUPS(unit); idx++) {
             if (TRUNK_INFO(unit, idx).tid == BCM_TRUNK_INVALID) {
                continue;
             }
             
             sal_memset(&t_data, 0, sizeof(bcm_trunk_add_info_t));
             rv = bcm_esw_trunk_get(unit, idx, &t_data);

             BCM_IF_ERROR_RETURN
                 (bcm_esw_switch_control_get(unit, bcmSwitchUseGport, &isGport));
             if (isGport) {
                 BCM_IF_ERROR_RETURN(
                     _bcm_esw_trunk_gport_resolve(unit, idx, &t_data));
             }

             if (BCM_SUCCESS(rv)) {
                 int i = 0;

                 for (i = 0; i < t_data.num_ports; i++) {
		   if (t_data.tm[i] == modid && t_data.tp[i] == port) {
                       *tid = idx;
                       return BCM_E_NONE;
                   }
                 }
             }
        }

        return BCM_E_NOT_FOUND;
    }
}

/*
 * Function:
 *      bcm_esw_trunk_failover_set
 * Purpose:
 *      Assign the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to specify failover port list.
 *      psc - (IN) Port selection criteria for failover port list.
 *      flags - (IN) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      count - (IN) Number of ports in failover port list.
 *      fail_to_array - (IN) Failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_trunk_failover_set(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                           int psc, uint32 flags, int count, 
                           bcm_gport_t *fail_to_array)
{
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    trunk_private_t *t_info;
    int rv, id, fabric_trunk;
    bcm_port_t port, ftp[BCM_TRUNK_MAX_PORTCNT];
    bcm_module_t modid, ftm[BCM_TRUNK_MAX_PORTCNT];
    bcm_trunk_t tgid;

    TRUNK_INIT(unit);

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    fabric_trunk = TRUNK_FABRIC_TID(unit, tid);

    if (fabric_trunk) {
        if (!soc_feature(unit, soc_feature_hg_trunk_failover)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(failport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, failport, &port));
        } else {
            port = failport;
        }
        modid = -1;
    } else {
        if (!soc_feature(unit, soc_feature_port_lag_failover)) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, failport, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)) {
            return BCM_E_PARAM;
        }
    }

    if (psc <= 0) {
	psc = BCM_TRUNK_PSC_DEFAULT;
    }

    /* Failover options:
     * 1) Specify plan with a flag
     * 2) Specify plan with count > 0 and fail_to_array listing ports
     * 3) Disable failover with count == 0
     */
    if ((flags == 0) && ((count < 0) ||
                         ((count > 0) && (fail_to_array == NULL)))) {
        return BCM_E_PARAM;
    }

    if (count > 0) {
        if ((TRUNK_FP_TID(unit, tid) &&
             (count > TRUNK_CNTL(unit).nports_fp)) ||
            (TRUNK_FABRIC_TID(unit, tid) &&
             (count > TRUNK_CNTL(unit).nports_hg)) ) {
            return (BCM_E_PARAM);
        } else {
            BCM_IF_ERROR_RETURN
                (_bcm_esw_trunk_gport_array_resolve(unit, fabric_trunk,
                                                    count, fail_to_array,
                                                    ftp, ftm));
        }
    }

    /* Offset of trunk type */
    tgid = tid - (fabric_trunk ? TRUNK_CNTL(unit).ngroups_fp : 0);

    TRUNK_LOCK(unit);
    rv = _bcm_xgs3_trunk_hwfailover_set(unit, tgid, fabric_trunk,
                                        port, modid, psc,
                                        flags, count, ftp, ftm);
    TRUNK_UNLOCK(unit);

    return rv;
#else /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    return BCM_E_UNAVAIL; 
#endif
}

/*
 * Function:
 *      bcm_esw_trunk_failover_get
 * Purpose:
 *      Retrieve the failover port list for a specific trunk port.
 * Parameters:
 *      unit - (IN) Unit number.
 *      tid - (IN) Trunk id.
 *      failport - (IN) Port in trunk for which to retrieve failover port list.
 *      psc - (OUT) Port selection criteria for failover port list.
 *      flags - (OUT) BCM_TRUNK_FLAG_FAILOVER_xxx.
 *      array_size - (IN) Maximum number of ports in provided failover port list.
 *      fail_to_array - (OUT) Failover port list.
 *      array_count - (OUT) Number of ports in returned failover port list.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
bcm_esw_trunk_failover_get(int unit, bcm_trunk_t tid, bcm_gport_t failport, 
                           int *psc, uint32 *flags, int array_size, 
                           bcm_gport_t *fail_to_array, int *array_count)
{
#if defined(BCM_TRX_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    trunk_private_t *t_info;
    int rv, id, fabric_trunk;
    bcm_port_t port, ftp[BCM_TRUNK_MAX_PORTCNT];
    bcm_module_t modid, ftm[BCM_TRUNK_MAX_PORTCNT];
    bcm_trunk_t tgid;

    TRUNK_INIT(unit);

    if (!soc_feature(unit, soc_feature_hg_trunk_failover)) {
        return BCM_E_UNAVAIL;
    }

    TRUNK_CHECK(unit, tid);

    t_info = &TRUNK_INFO(unit, tid);

    if (t_info->tid == BCM_TRUNK_INVALID) {
        return (BCM_E_NOT_FOUND);
    }

    fabric_trunk = TRUNK_FABRIC_TID(unit, tid);

    if (fabric_trunk) {
        if (!soc_feature(unit, soc_feature_hg_trunk_failover)) {
            return BCM_E_UNAVAIL;
        }
        if (BCM_GPORT_IS_SET(failport)) {
            BCM_IF_ERROR_RETURN
                (bcm_esw_port_local_get(unit, failport, &port));
        } else {
            port= failport;
        }
        modid = -1;
    } else {
        if (!soc_feature(unit, soc_feature_port_lag_failover)) {
            return BCM_E_UNAVAIL;
        }
        BCM_IF_ERROR_RETURN
            (_bcm_esw_gport_resolve(unit, failport, &modid, &port, &tgid, &id));
        if ((-1 != tgid) || (-1 != id)) {
            return BCM_E_PARAM;
        }
    }

    if ((psc == NULL) || (flags == NULL) || (array_size < 0) ||
        (fail_to_array == NULL) || (array_count == NULL)) {
        return BCM_E_PARAM;
    }

    /* Offset of trunk type */
    tgid = tid - (fabric_trunk ? TRUNK_CNTL(unit).ngroups_fp : 0);

    TRUNK_LOCK(unit);
    rv = _bcm_xgs3_trunk_hwfailover_get(unit, tgid, fabric_trunk,
                                        port, modid, psc, flags,
                                        array_size, ftp, ftm, array_count);
    TRUNK_UNLOCK(unit);

    BCM_IF_ERROR_RETURN(rv);

    return _bcm_esw_trunk_gport_construct(unit, fabric_trunk, *array_count,
                                          ftp, ftm, fail_to_array);
#else /* BCM_TRX_SUPPORT || BCM_BRADLEY_SUPPORT */
    return BCM_E_UNAVAIL; 
#endif
}



#ifdef BCM_WARM_BOOT_SUPPORT

#define TRUNK_MIN_FABRIC_TID(unit) \
    (TRUNK_CNTL(unit).ngroups_hg ? TRUNK_CNTL(unit).ngroups_fp : -1)
#define TRUNK_MAX_FABRIC_TID(unit) \
    ( TRUNK_CNTL(unit).ngroups_hg ? \
     (TRUNK_CNTL(unit).ngroups_fp + TRUNK_CNTL(unit).ngroups_hg - 1): -1)

/*
 * _bcm_trunk_lag_reinit
 *
 * Restores sw state for front pannel trunks
 * Note: Application must register modid-port mapping function
 *       using bcm_stk_modmap_register before calling this
 */
STATIC int
_bcm_trunk_lag_reinit(int unit)
{
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_EASYRIDER_SUPPORT)
    if (SOC_IS_FB_FX_HX(unit) || SOC_IS_EASYRIDER(unit)) {
        return (_xgs3_trunk_reinit(unit, TRUNK_CNTL(unit).ngroups_fp,
                                   TRUNK_CNTL(unit).t_info));
    }
#endif /* BCM_FIREBOLT_SUPPORT || BCM_EASYRIDER_SUPPORT */
  	 
    return BCM_E_NONE;
}

/*
 * _bcm_trunk_fabric_reinit
 *
 * Restores sw state for fabric trunks
 */
STATIC int
_bcm_trunk_fabric_reinit(int unit)
{
#if defined(BCM_FIREBOLT_SUPPORT) || defined(BCM_BRADLEY_SUPPORT)
    if ((SOC_IS_FB_FX_HX(unit) && !SOC_IS_RAPTOR(unit)) || SOC_IS_HBX(unit)) {
        int min_tid = TRUNK_MIN_FABRIC_TID(unit);

        return (_xgs3_trunk_fabric_reinit(unit, 
                                          TRUNK_MIN_FABRIC_TID(unit),
                                          TRUNK_MAX_FABRIC_TID(unit),
                                          &TRUNK_INFO(unit,min_tid)));
    }
#endif /* BCM_FIREBOLT_SUPPORT */

    return BCM_E_NONE;
}

/*
 * _bcm_trunk_reinit
 *
 * Recovers trunk state from hardware (Reload)
 */
STATIC int
_bcm_trunk_reinit(int unit)
{
    bcm_trunk_t tid;
    trunk_private_t *t_info;
 	 
    /*
     * Trunk state initialization
     */
    t_info = TRUNK_CNTL(unit).t_info;
  
    for (tid = 0; tid < TRUNK_NUM_GROUPS(unit); tid++) {
        t_info->tid = BCM_TRUNK_INVALID;
        t_info->in_use = FALSE;
        t_info->dlf_index_spec = -1;
        t_info->dlf_index_used = -1;
        t_info->mc_index_spec = -1;
        t_info->mc_index_used = -1;
        t_info->ipmc_index_spec = -1;
        t_info->ipmc_index_used = -1;
        t_info++;
    }
  	     
    if (SOC_IS_XGS12_FABRIC(unit)) {
#ifdef BCM_XGS12_FABRIC_SUPPORT
        bcm_pbmp_t tports;
        bcm_port_t port;
        bcm_trunk_add_info_t *t_data, td;        
        uint32 val;
  	         
        for (tid = 0; tid < TRUNK_CNTL(unit).ngroups_hg; tid++) {
            PBMP_HG_ITER(unit, port) {
                SOC_IF_ERROR_RETURN(READ_ING_HGTRUNKr(unit, port, tid, &val));
                break;
            }
  	             
            SOC_PBMP_CLEAR(tports);
            SOC_PBMP_WORD_SET(tports, 0,
                              soc_reg_field_get(unit, ING_HGTRUNKr, val, BMAPf));
  	             
            sal_memset(&td, 0, sizeof(bcm_trunk_add_info_t));
            t_data = &td;
  	             
            t_data->num_ports = 0;
            PBMP_ITER(tports, port) {
                t_data->tm[t_data->num_ports] = 0; 
                t_data->tp[t_data->num_ports] = port;
                t_data->num_ports += 1;
            }        
  	             
            /*
             * Nothing to update for this TID
             */
            if (t_data->num_ports == 0) {
                continue;
            }
  	             
            switch(soc_reg_field_get(unit, ING_HGTRUNKr, val, ALGORITHMf)) {
            case 0:
                t_data->psc = BCM_TRUNK_PSC_SRCDSTMAC;
  	                 
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPMACSAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPMACSA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPMACDAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPMACDA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPTYPEf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPTYPE;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPVIDf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPVID;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPSAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPSA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_IPDAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_IPDA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_L4SSf)) {
                    t_data->psc |= BCM_TRUNK_PSC_L4SS;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_L4DSf)) {
                    t_data->psc |= BCM_TRUNK_PSC_L4DS;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_MACDAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_MACDA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_MACSAf)) {
                    t_data->psc |= BCM_TRUNK_PSC_MACSA;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_TYPEf)) {
                    t_data->psc |= BCM_TRUNK_PSC_TYPE;
                }
                if (soc_reg_field_get(unit, ING_HGTRUNKr, val, EN_VIDf)) {
                    t_data->psc |= BCM_TRUNK_PSC_VID;
                }
                break;
            case 1:
                t_data->psc = BCM_TRUNK_PSC_REDUNDANT;
                break;
            default:
                t_data->psc = -1;
                break;
            }
  	             
            t_data->dlf_index = -1;
            t_data->mc_index = -1;
            t_data->ipmc_index = -1;
  	             
            /*
             * Fill in t_info struct for this tid
             */
            t_info = &TRUNK_INFO(unit, tid);
  	             
            /*
             * Check number of ports in trunk group
             */
            if (t_data->num_ports < 1 ||
                (TRUNK_FP_TID(unit, tid) &&
                 (t_data->num_ports > TRUNK_CNTL(unit).nports_fp)) ||
                (TRUNK_FABRIC_TID(unit, tid) &&
                 (t_data->num_ports > TRUNK_CNTL(unit).nports_hg))) {
                return (BCM_E_PARAM);
            }
  	             
            TRUNK_LOCK(unit);
            t_info->tid = tid;
            t_info->psc = t_data->psc;
            t_info->in_use = TRUE;
            TRUNK_UNLOCK(unit);
        }
#endif /* BCM_XGS12_FABRIC_SUPPORT */
    } else if (SOC_IS_XGS3_FABRIC(unit)) {
#ifdef BCM_XGS3_FABRIC_SUPPORT
        /*
         * recover software state for fabric trunks
         */
        BCM_IF_ERROR_RETURN(_bcm_trunk_fabric_reinit(unit));
#endif /* BCM_XGS3_FABRIC_SUPPORT */
    }

#ifdef BCM_XGS_SWITCH_SUPPORT
    if (SOC_IS_XGS3_SWITCH(unit)) {
        if (SOC_IS_FB_FX_HX(unit) || SOC_IS_HBX(unit)) { 
            /*
             * recover software state for fabric trunks
             */
            BCM_IF_ERROR_RETURN(_bcm_trunk_fabric_reinit(unit));
  	 
            /*
             * recover software state for ether trunks
             */
            BCM_IF_ERROR_RETURN(_bcm_trunk_lag_reinit(unit));
        } else if (SOC_IS_EASYRIDER(unit)) {
            /*
             * recover software state for ether trunks
             */
            BCM_IF_ERROR_RETURN(_bcm_trunk_lag_reinit(unit));		
        }
    }
#endif
  	     
    return BCM_E_NONE;
}
#endif /* BCM_WARM_BOOT_SUPPORT */
  	 
#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
/*
 * Function:
 *     _bcm_trunk_sw_dump
 * Purpose:
 *     Displays trunk information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
_bcm_trunk_sw_dump(int unit)
{
    trunk_cntl_t    *t_cntl;
    trunk_private_t *t_info;
    int             i;

    t_cntl = &TRUNK_CNTL(unit);
    soc_cm_print("\nSW Information TRUNK - Unit %d\n", unit);
    soc_cm_print("  Front  panel trunk groups    : %d\n", t_cntl->ngroups_fp);
    soc_cm_print("  Front  panel trunk max ports : %d\n", t_cntl->nports_fp);
    soc_cm_print("  Fabric panel trunk groups    : %d\n", t_cntl->ngroups_hg);
    soc_cm_print("  Fabric panel trunk max ports : %d\n", t_cntl->nports_hg);

    for (i = 0; i < TRUNK_NUM_GROUPS(unit); i++) {
        t_info = &TRUNK_INFO(unit, i);
        soc_cm_print("  Trunk %d\n", i);
        soc_cm_print("      ID              : %d\n", t_info->tid);
        soc_cm_print("      in use          : %d\n", t_info->in_use);
        soc_cm_print("      psc             : 0x%x\n", t_info->psc);
        soc_cm_print("      dlf index spec  : %d\n", t_info->dlf_index_spec);
        soc_cm_print("      dlf index used  : %d\n", t_info->dlf_index_used);
        soc_cm_print("      dlf port used   : %d\n", t_info->dlf_port_used);
        soc_cm_print("      mc index spec   : %d\n", t_info->mc_index_spec);
        soc_cm_print("      mc index used   : %d\n", t_info->mc_index_used);
        soc_cm_print("      mc port used    : %d\n", t_info->mc_port_used);
        soc_cm_print("      ipmc index spec : %d\n", t_info->ipmc_index_spec);
        soc_cm_print("      ipmc index used : %d\n", t_info->ipmc_index_used);
        soc_cm_print("      ipmc port used  : %d\n", t_info->ipmc_port_used);
    }

    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */



/*
 * Function:
 *      _bcm_esw_trunk_lock
 * Purpose:
 *     Lock Trunk module - if module was not initialized NOOP
 *    
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_trunk_lock(int unit)
{
    if (NULL != TRUNK_CNTL(unit).lock) {
        return sal_mutex_take(TRUNK_CNTL(unit).lock, 
                              sal_mutex_FOREVER);
    }
    return (BCM_E_NONE);
}
/*
 * Function:
 *      _bcm_esw_trunk_unlock
 * Purpose:
 *     Unlock Trunk module - if module was not initialized NOOP
 *    
 * Parameters:
 *      unit - (IN) Unit number.
 * Returns:
 *      BCM_E_XXX
 * Notes:
 */
int 
_bcm_esw_trunk_unlock(int unit)
{
    if (NULL != TRUNK_CNTL(unit).lock) {
        return sal_mutex_give(TRUNK_CNTL(unit).lock);
    }
    return (BCM_E_NONE);
}
