 /* 
 * $Id: bfd.c,v 1.6 Broadcom SDK $
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
 * File:       subport.c
 * Purpose:    Subport CoE related functions.
 *
 */

#include <soc/defs.h>
#include <sal/core/libc.h>
#include <shared/bsl.h>

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/subport.h>

#include <bcm_int/esw/subport.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/triumph.h>
#include <bcm_int/esw/triumph2.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/trident2plus.h>

/*
 * Function Vector
 */
static bcm_esw_subport_drv_t bcm_td2plus_subport_drv = {

#if defined(BCM_TRIUMPH2_SUPPORT) && defined(INCLUDE_L3)
	/* subport_init                */ bcm_tr2_subport_init,
	/* subport_group_create        */ bcm_tr2_subport_group_create,
	/* subport_group_get           */ bcm_tr2_subport_group_get,
	/* subport_group_traverse      */ NULL,
	/* subport_group_destroy       */ bcm_tr2_subport_group_destroy,
	/* subport_linkphy_config_set  */ NULL,
	/* subport_linkphy_config_get  */ NULL,
	/* subport_port_add            */ bcm_tr2_subport_port_add,
	/* subport_port_get            */ bcm_tr2_subport_port_get,
	/* subport_port_traverse       */ bcm_tr2_subport_port_traverse,
	/* subport_port_stat_set       */ NULL,
	/* subport_port_stat_get       */ NULL,
	/* subport_port_delete         */ bcm_tr2_subport_port_delete,
	/* subport_cleanup             */ bcm_tr2_subport_cleanup,
#else 
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL,
#endif

	/* coe_init                */ bcm_td2plus_subport_coe_init,
	/* coe_group_create        */ bcmi_xgs5_subport_coe_group_create,
	/* coe_group_get           */ bcmi_xgs5_subport_coe_group_get,
	/* coe_group_traverse      */ bcmi_xgs5_subport_coe_group_traverse,
	/* coe_group_destroy       */ bcmi_xgs5_subport_coe_group_destroy,
	/* coe_linkphy_config_set  */ NULL,
	/* coe_linkphy_config_get  */ NULL,
	/* coe_port_add            */ bcmi_xgs5_subport_coe_port_add,
	/* coe_port_get            */ bcmi_xgs5_subport_coe_port_get,
	/* coe_port_traverse       */ bcmi_xgs5_subport_coe_port_traverse,
	/* coe_port_stat_set       */ NULL,
	/* coe_port_stat_get       */ NULL,
	/* coe_port_delete         */ bcmi_xgs5_subport_coe_port_delete,
	/* coe_cleanup             */ bcmi_xgs5_subport_coe_cleanup
};

/*
 * Device Specific HW Tables
 */

/* ING Port Table */
static bcmi_xgs5_subport_coe_ing_port_table_t bcmi_td2p_subport_coe_ing_port_table = {
    /* mem */ 		PORT_TABm,
	/* port_type */ PORT_TYPEf
};

/* EGR Port Table */
static bcmi_xgs5_subport_coe_egr_port_table_t  bcmi_td2p_subport_coe_egr_port_table = {
    /* mem */ 		EGR_PORTm,
    /* port_type */ PORT_TYPEf
};

/* Subport Tag SGPP Memory Table */
static bcmi_xgs5_subport_coe_subport_tag_sgpp_table_t bcmi_td2p_subport_coe_subport_tag_sgpp_table = {
    /* mem */ 	                     SUBPORT_TAG_SGPP_MAPm,
    /* valid */                      VALIDf,
    /* subport_tag */                SUBPORT_TAGf,
    /* subport_tag_mask */	         SUBPORT_TAG_MASKf,
    /* subport_tag_namespace */	     SUBPORT_TAG_NAMESPACEf,
    /* subport_tag_namespace_mask */ SUBPORT_TAG_NAMESPACE_MASKf,
    /* src_modid */                  SRC_MODIDf,
    /* src_port */	                 SRC_PORTf,
    /* phb_enable */                 PHB_ENABLEf,
    /* int_pri */	                 INT_PRIf,
    /* cng */	                     CNGf
};

/* Modport map subport Memory Table */
bcmi_xgs5_subport_coe_modport_map_subport_table_t bcmi_td2p_subport_coe_modport_map_subport_table = {
    /* mem */        MODPORT_MAP_SUBPORTm,
    /* dest */	     DESTf,
    /* is_trunk */	 ISTRUNKf,
    /* enable */	 ENABLEf,
};

/* Egress subport tag dot1p Table */
static bcmi_xgs5_subport_coe_egr_subport_tag_dot1p_table_t bcmi_td2p_subport_coe_egr_subport_tag_dot1p_table = {
    /* mem */                   EGR_SUBPORT_TAG_DOT1P_MAPm,
    /* subport_tag_priority */  SUBPORT_TAG_PRIORITYf,
    /* subport_tag_color */     SUBPORT_TAG_COLORf
};

/* HW Definitions */
static bcmi_xgs5_subport_coe_hw_defs_t    bcmi_td2p_subport_coe_hw_defs;

int bcm_td2plus_subport_coe_init(int unit)
{
    /* HW Definition Tables */
    sal_memset(&bcmi_td2p_subport_coe_hw_defs, 0, sizeof(bcmi_td2p_subport_coe_hw_defs));

    bcmi_td2p_subport_coe_hw_defs.igr_port = 
        &bcmi_td2p_subport_coe_ing_port_table;
    bcmi_td2p_subport_coe_hw_defs.egr_port   = 
        &bcmi_td2p_subport_coe_egr_port_table;
    bcmi_td2p_subport_coe_hw_defs.subport_tag_sgpp = 
        &bcmi_td2p_subport_coe_subport_tag_sgpp_table;
    bcmi_td2p_subport_coe_hw_defs.modport_map_subport = 
        &bcmi_td2p_subport_coe_modport_map_subport_table;
    bcmi_td2p_subport_coe_hw_defs.egr_subport_tag_dot1p = 
        &bcmi_td2p_subport_coe_egr_subport_tag_dot1p_table;

    /* Initialize Common XGS5 CoE module */
    BCM_IF_ERROR_RETURN
        (bcmi_xgs5_subport_init(unit, &bcm_td2plus_subport_drv, &bcmi_td2p_subport_coe_hw_defs));
    return 0;
}



