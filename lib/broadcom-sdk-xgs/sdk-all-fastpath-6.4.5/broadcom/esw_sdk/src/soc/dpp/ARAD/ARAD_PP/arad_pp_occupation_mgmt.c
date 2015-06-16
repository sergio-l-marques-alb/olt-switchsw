/* $Id: arad_pp_occupation_mgmt.c,v 1.72 Broadcom SDK $
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
*/

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_MANAGEMENT

/*************
 * INCLUDES  *
 *************/
/* { */

#include <shared/bsl.h>
#include <shared/swstate/sw_state_access.h>
#include <shared/shr_occupation.h>
#include <soc/dcmn/error.h>
#include <soc/dpp/ARAD/ARAD_PP/arad_pp_occupation_mgmt.h>

/* } */
/*************
 * MACROS    *
 *************/
/* { */

#define OCCUPATION_ACCESS_CALLBACKS(unit)  sw_state_access[unit].dpp.soc.arad.pp.occupation

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

/* returns the SW-state-access callbacks of the given occupation type */
soc_error_t
arad_pp_occ_mgmt_access_callbacks_get(
   int unit,
   SOC_OCC_MGMT_TYPE occ_type,
   int(**load)(int, int, int*),
   int(**save)(int, int, int)
  )
{
    SOCDNX_INIT_FUNC_DEFS;

    switch (occ_type)
    {
    case SOC_OCC_MGMT_TYPE_INLIF:
        *load = OCCUPATION_ACCESS_CALLBACKS(unit).inlif_profile.get;
        *save = OCCUPATION_ACCESS_CALLBACKS(unit).inlif_profile.set;
        break;
    case SOC_OCC_MGMT_TYPE_OUTLIF:
        *load = OCCUPATION_ACCESS_CALLBACKS(unit).outlif_profile.get;
        *save = OCCUPATION_ACCESS_CALLBACKS(unit).outlif_profile.set;
        break;
    case SOC_OCC_MGMT_TYPE_RIF:
        *load = OCCUPATION_ACCESS_CALLBACKS(unit).rif_profile.get;
        *save = OCCUPATION_ACCESS_CALLBACKS(unit).rif_profile.set;
        break;

    /* Add more when new occupation types are introduced */

    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOCDNX_MSG("Error, given occupation type is not supported\n")));
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* Creates and saves to SW-state the INLIF occupation map */
soc_error_t
arad_pp_occ_mgmt_init_inlif_occupation (int unit)
{
    soc_error_t             res;
    shr_occ_item_t          item;
    shr_occ_map_args_t      args;
    shr_occ_map_t           map;
    SHR_BITDCL              full_mask = 0xf; /* Inlif occupation consists of 4 bits */

    /* sw state access callbacks for saving and loading the occupation map */
    int (*load)(int,int,int*);
    int (*save)(int,int,int);

    SOCDNX_INIT_FUNC_DEFS;

    /* fill in sw state access callbacks */
    res = arad_pp_occ_mgmt_access_callbacks_get(unit, SOC_OCC_MGMT_TYPE_INLIF, &load, &save);
    SOCDNX_IF_ERR_EXIT(res);

    /* Initialize occupation map */
    shr_occ_map_args_init(unit, &args, SOC_OCC_MGMT_INLIF_SIZE, "INLIF", load, save);
    shr_occ_map_create(unit, &map, args, SOC_OCC_MGMT_FLAGS_NONE);

    /* Adding applications to occupation map */

    /* SAME INTERFACE */
    if (soc_property_get(unit, spn_BCM886XX_LOGICAL_INTERFACE_BRIDGE_FILTER_ENABLE, 0)) {
        /* For same interface we need one bit of the inlif occupation map, should be tranfered to the egress */
        SHR_BITDCL mask = 0x3; /* Only two lsb occupation map bits are transferred to egress */
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE, &mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* DSCP MARKING */
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_QOS_L3_L2_MARKING, 0)) {
        /* For DSCP marking we need two bits of the inlif occupation map, should be tranfered to the egress */
        SHR_BITDCL mask = 0x3; /* Only two lsb occupation map bits are transferred to egress */
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING, &mask, 2);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* STRICT URPF */
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_L3_INGRESS_URPF_ENABLE, 0)) {
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_INLIF_APP_STRICT_URPF, &full_mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* OAM */
    if (SOC_IS_ARADPLUS(unit) && soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0)) {
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_INLIF_APP_OAM, &full_mask, soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE, 0));
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* SPLIT HORIZON ORIENTATION */
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_INLIF_APP_ORIENTATION, &full_mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* Do not add more applications below */

    /* 
     * Solve constraints implied by applications, and save result to sw state. 
     * If successful, applications can be accessed using arad_pp_occ_mgmt_app_get/set.
     */
    res = shr_occ_map_solve(unit, &map, SOC_OCC_MGMT_FLAGS_NONE);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* Creates and saves to SW-state the OUTLIF occupation map */
soc_error_t
arad_pp_occ_mgmt_init_outlif_occupation (int unit)
{
    soc_error_t             res;
    shr_occ_item_t          item;
    shr_occ_map_args_t      args;
    shr_occ_map_t           map;
    SHR_BITDCL              full_mask = 0x3f; /* Outlif occupation map consists of 6 bits */
    SHR_BITDCL              mask;

    /* sw state access callbacks for saving and loading the occupation map */
    int (*load)(int,int,int*);
    int (*save)(int,int,int);

    SOCDNX_INIT_FUNC_DEFS;

    /* fill in sw state access callbacks */
    res = arad_pp_occ_mgmt_access_callbacks_get(unit, SOC_OCC_MGMT_TYPE_OUTLIF, &load, &save);
    SOCDNX_IF_ERR_EXIT(res);
    /* Initialize occupation map */
    shr_occ_map_args_init(unit, &args, SOC_OCC_MGMT_OUTLIF_SIZE(unit), "OUTLIF", load, save);
    shr_occ_map_create(unit, &map, args, SOC_OCC_MGMT_FLAGS_NONE);

    /* Adding applications to occupation map */

    /* SPLIT HORIZON ORIENTATION */
    if (soc_property_get(unit, spn_SPLIT_HORIZON_FORWARDING_GROUPS_MODE, 1)) {
        SHR_BITDCL mask = 0x7; /* only 3 LSB allowed */
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, &mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
        mask = 0x38;    /* only 3 MSB allowed */
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION, &mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* OAM PCP */
    if (SOC_INFO(unit).oamp_port[0] == ARAD_OAMP_PORT_ID) {
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_OUTLIF_APP_OAM_PCP, &full_mask, 1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* OAM DEFAULT MEP */
    if (soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE_EGRESS, 0)) {
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_OUTLIF_APP_OAM_DEFAULT_MEP, &full_mask,
                          soc_property_get(unit, spn_BCM886XX_OAM_DEFAULT_PROFILE_EGRESS, 0));
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    /* EVPN */
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_EVPN_ENABLE, 0)) {
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_OUTLIF_APP_EVPN, &full_mask,1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }

    
    if (SOC_IS_JERICHO(unit) && soc_property_get(unit, spn_ROO_EXTENSION_LABEL_ENCAPSULATION, 0)) {
        /* Extension label must use a known bit of the outlif profile for the PRGE program that creates it.
        If this value is changed, the relevant programs also must be modified! */
        mask = 1;
        shr_occ_item_init(unit, &item, SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL, &mask,1);
        res = shr_occ_map_add_item(unit, &map, item, SOC_OCC_MGMT_FLAGS_NONE);
        SOCDNX_IF_ERR_EXIT(res);
    }
    /* Do not add more applications below */

    /* 
     * Solve constraints implied by applications, and save result to sw state. 
     * If successful, applications can be accessed using arad_pp_occ_mgmt_app_get/set.
     */
    res = shr_occ_map_solve(unit, &map, SOC_OCC_MGMT_FLAGS_NONE);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/*
 * Function returns application relevant mask in given occupation type. 
 * That is, which bits are occupied by the application. 
 * 
 * @param (in) unit 
 * @param (in) occ_type - inlif/outlif/rif/etc.
 * @param (in) application_type - application that is using occupation. 
 *             use SOC_OCC_MGMT_APP_USER if you want to get the user part.
 * @param (out) mask - the mask of the application in the occupation map.
 *  
 * @return soc_error_t
 */
soc_error_t arad_pp_occ_mgmt_get_app_mask(
   int                                                              unit,
   SOC_OCC_MGMT_TYPE                                                occ_type,
   int                                                              application_type,
   SHR_BITDCL                                                       *mask
   ){
    soc_error_t res;
    int(*load)(int, int, int*);
    int(*save)(int, int, int);

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_occ_mgmt_access_callbacks_get(unit, occ_type, &load, &save);
    SOCDNX_IF_ERR_EXIT(res);

    res = shr_occ_mgmt_item_mask_get(unit, load, application_type, mask);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * Gets application's value from the full_occupation. 
 * Full occupation is a value read from HW, containing all data of the shared resource. 
 * An application may occupy non consecutive bits. The returned value will only be affected 
 * by the application-relevant bits. 
 *  
 * @param (in)  unit 
 * @param (in)  occ_type - inlif/outlif/rif/etc.
 * @param (in)  application_type - application that is using occupation. 
 *              use SOC_OCC_MGMT_APP_USER if you want to get the user part.
 * @param (in)  full_occupation - shared resource value as read from HW.
 * @param (out) val - application's value in full_occupation
 */
soc_error_t
arad_pp_occ_mgmt_app_get(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   SHR_BITDCL                  *full_occupation,
   uint32                      *val
   ) {
    soc_error_t res;
    uint32 read_opcode = 0;
    int(*load)(int, int, int*);
    int(*save)(int, int, int);

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_occ_mgmt_access_callbacks_get(unit, occ_type, &load, &save);
    SOCDNX_IF_ERR_EXIT(res);

    res = shr_occ_mgmt_item_access(unit, load, save, application_type, val, read_opcode, full_occupation);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * Sets application's value into full_occupation. 
 * Full occupation is a value read from HW, containing all data of the shared resource. 
 * An application may occupy non consecutive bits. The input value will only affect 
 * the application-relevant bits. 
 * Note: make sure your value can be contained by the applications' number of bits. 
 *  
 * @param (in)  unit 
 * @param (in)  occ_type - inlif/outlif/rif/etc.
 * @param (in)  application_type - application that is using occupation. 
 *              use SOC_OCC_MGMT_APP_USER if you want to get the user part.
 * @param (in)  val - application's value to be set in full_occupation 
 * @param (out) full_occupation - shared resource value as read from HW. 
 */
soc_error_t
arad_pp_occ_mgmt_app_set(
   int                         unit,
   SOC_OCC_MGMT_TYPE           occ_type,
   int                         application_type,
   uint32                      val,
   SHR_BITDCL                  *full_occupation
   ) {
    soc_error_t res;
    uint32 write_opcode = 1;
    int(*load)(int, int, int*);
    int(*save)(int, int, int);

    SOCDNX_INIT_FUNC_DEFS;

    res = arad_pp_occ_mgmt_access_callbacks_get(unit, occ_type, &load, &save);
    SOCDNX_IF_ERR_EXIT(res);

    res = shr_occ_mgmt_item_access(unit, load, save, application_type, &val, write_opcode, full_occupation);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* Initializes all of the occupation maps */
soc_error_t
arad_pp_occ_mgmt_init (int unit)
{
    soc_error_t res;
    SOCDNX_INIT_FUNC_DEFS;

    /* init SW state DB */
    res = sw_state_access[unit].dpp.soc.arad.pp.occupation.alloc(unit);
    SOCDNX_IF_ERR_EXIT(res);

    /* init occupation maps */

    res = arad_pp_occ_mgmt_init_inlif_occupation(unit);
    SOCDNX_IF_ERR_EXIT(res);

    if (SOC_IS_JERICHO(unit)) {
        res = arad_pp_occ_mgmt_init_outlif_occupation(unit);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

soc_error_t
arad_pp_occ_mgmt_deinit (int unit)
{
    soc_error_t res;
    SOCDNX_INIT_FUNC_DEFS;

    /* deinit SW state DB */
    res = sw_state_access[unit].dpp.soc.arad.pp.occupation.free(unit);
    SOCDNX_IF_ERR_EXIT(res);

exit:
    SOCDNX_FUNC_RETURN;
}

/* 
 * please help me find a suitable name for this function :) 
 *  
 * Returns arguments required for TCAM initialization 
 *  
 * @param (in)  unit 
 * @param (in)  occ_type: 
 *                  inlif/outlif/rif/etc.
 * @param (in)  application_type: 
 *                  application that is using occupation. 
 *                  use SOC_OCC_MGMT_APP_USER if you want to get the user part.
 * @param (in)  value: 
 *                  the value of the application in the occupation map. 
 *  
 * @param (out) full_occupation: 
 *                  the full state of the hardware that TCAM should expect for the given
 *                  input occupation type, application type, and value.
 * @param (out) mask: 
 *                  signals which bits of the full occupation are being used by the given
 *                  application type. 1 = used by app, 0 = not used by app.
 * @param (out) mask_flipped: 
 *                  for convenience, this is the negation ('flip') of mask.
 *  
 * NOTE: Out params may be passed in as NULL, in which case they'll be ignored.
 */
soc_error_t
arad_pp_occ_mgmt_tcam_args_get (
   int                  unit, 
   SOC_OCC_MGMT_TYPE    occ_type,
   int                  application_type,
   uint32               value,
   SHR_BITDCL           *full_occupation,
   SHR_BITDCL           *mask,
   SHR_BITDCL           *mask_flipped)
{
    soc_error_t res;
    int size;
    int(*load)(int, int, int*);
    int(*save)(int, int, int);
    SHR_BITDCL* _mask = NULL;

    SOCDNX_INIT_FUNC_DEFS;

    /* get occupation map's size */
    res = arad_pp_occ_mgmt_access_callbacks_get(unit,occ_type,&load,&save);
    SOCDNX_IF_ERR_EXIT(res);
    res = (*load)(unit,0,&size);
    SOCDNX_IF_ERR_EXIT(res);

    /* get the app's mask relative to the map */
    SOCDNX_ALLOC(_mask, uint32, _SHR_BITDCLSIZE(size),"mask");
    sal_memset(_mask, 0, SHR_BITALLOCSIZE(size));
    res = arad_pp_occ_mgmt_get_app_mask(unit,occ_type,application_type,_mask);
    SOCDNX_IF_ERR_EXIT(res);

    /* fill in mask output param */
    if (mask != NULL) {
        SHR_BITCOPY_RANGE(mask, 0, _mask, 0, size);
    }

    /* fill in mask_flipped output param */
    if (mask_flipped != NULL) {
        SHR_BITNEGATE_RANGE(mask,0,size,mask_flipped);
    }

    /* fill in full_occupation output param */
    if (full_occupation != NULL) {
        SHR_BITCLR_RANGE(full_occupation,0,SHR_BITALLOCSIZE(size));
        res = arad_pp_occ_mgmt_app_set(unit,occ_type,application_type,value,full_occupation);
        SOCDNX_IF_ERR_EXIT(res);
    }

exit:
    SOCDNX_FREE(_mask);
    SOCDNX_FUNC_RETURN;
}

/* } */
/****************
 * DIAG helpers *
 ****************/
/* { */

char*
arad_pp_occ_mgmt_get_profile_name(SOC_OCC_MGMT_TYPE occ_type) {
    switch (occ_type) {
    case SOC_OCC_MGMT_TYPE_INLIF: 
        return "InLIF Profile";
    case SOC_OCC_MGMT_TYPE_OUTLIF: 
        return "OutLIF Profile";
    case SOC_OCC_MGMT_TYPE_RIF: 
        return "RIF Profile";
    default:
        return "";
    }
}

char*
arad_pp_occ_mgmt_get_app_name(SOC_OCC_MGMT_TYPE occ_type, int app) {

    if (app == SOC_OCC_MGMT_APP_USER) {
        return"Unreserved";
    }

    switch (occ_type) {

    /* INLIF PROFILE */
    case SOC_OCC_MGMT_TYPE_INLIF: 
        switch (app) {
        case SOC_OCC_MGMT_INLIF_APP_SIMPLE_SAME_INTERFACE:
            return "EVB / MPLS MP (Same interface filtering)";
        case SOC_OCC_MGMT_INLIF_APP_OAM:
           return "OAM default lif occupation map";
        case SOC_OCC_MGMT_INLIF_APP_TAGGED_MODE:
           return "PWE tagged mode";
        case SOC_OCC_MGMT_INLIF_APP_SIMPLE_DSCP_MARKING:
           return "DSCP/EXP marking profile when bridging";
        case SOC_OCC_MGMT_INLIF_APP_STRICT_URPF:
           return "URPF";
        case SOC_OCC_MGMT_INLIF_APP_ORIENTATION:
           return "Split Horizon Orientation";
        default:
            return "Invalid";
        }

    /* OUTLIF PROFILE */
    case SOC_OCC_MGMT_TYPE_OUTLIF: 
        switch (app) {
        case SOC_OCC_MGMT_OUTLIF_APP_OAM_PCP:
           return "OAM PCP lif occupation map";
        case SOC_OCC_MGMT_OUTLIF_APP_OAM_DEFAULT_MEP:
           return "OAM default lif occupation map";
        case SOC_OCC_MGMT_OUTLIF_APP_TAGGED_MODE:
           return "PWE tagged mode";
        case SOC_OCC_MGMT_OUTLIF_APP_ORIENTATION:
           return "Split Horizon Orientation";
        case SOC_OCC_MGMT_OUTLIF_APP_EVPN:
           return "EVPN outlif occupation map";
        case SOC_OCC_MGMT_OUTLIF_APP_MPLS_ENCAPSULATE_EXTENDED_LABEL:
           return "Mpls encapsulate extended label outlif occupation map";
        default:
            return "Invalid";
        }

    /* RIF PROFILE */
    case SOC_OCC_MGMT_TYPE_RIF: 
        return "RIF_PROFILE";

    default:
        return "Invalid";
    }
}

soc_error_t
arad_pp_occ_mgmt_diag_info_get(int unit, SOC_OCC_MGMT_TYPE occ_type, char* apps[])
{
    uint32 res;
	int i;
    int occupation[256];

    int(*load)(int, int, int*);
    int(*save)(int, int, int);

	SOCDNX_INIT_FUNC_DEFS;

    apps[0] = arad_pp_occ_mgmt_get_profile_name(occ_type);

    res = arad_pp_occ_mgmt_access_callbacks_get(unit,occ_type,&load,&save);
    SOCDNX_IF_ERR_EXIT(res);

    res = shr_occ_mgmt_solution_load(unit,load,occupation);
    SOCDNX_IF_ERR_EXIT(res);

    for (i = 1; i <= occupation[0]; i++ ) {
        apps[i] = arad_pp_occ_mgmt_get_app_name(occ_type, occupation[i]);
    }

exit:
    SOCDNX_FUNC_RETURN;
}

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

