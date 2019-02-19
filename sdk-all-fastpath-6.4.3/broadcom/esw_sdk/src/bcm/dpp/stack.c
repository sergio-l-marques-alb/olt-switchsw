
/*
 * $Id: stack.c,v 1.46 Broadcom SDK $
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
 * Soc_petra-B Stack Control Implementation
 */

#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_STK

#include <shared/bsl.h>

#include <bcm/error.h>

#include <bcm_int/common/debug.h>
#include <bcm_int/api_xlate_port.h>
#include <bcm_int/petra_dispatch.h>

#include <bcm_int/dpp/utils.h>
#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/cosq.h>
#include <bcm_int/dpp/fabric.h>
#include <bcm_int/dpp/stack.h>
#include <bcm_int/dpp/wb_db_stack.h>
#include <bcm_int/dpp/switch.h>
#include <bcm_int/dpp/gport_mgmt.h>

#include <soc/drv.h>

#include <soc/dpp/port_sw_db.h>

#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/dpp/TMC/tmc_api_general.h>

#include <soc/dpp/PPD/ppd_api_eg_encap.h>


#include <soc/dpp/mbcm.h>

#include <soc/dpp/TMC/tmc_api_ingress_packet_queuing.h>

#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/port_sw_db.h>
#endif

#define _DPP_STACK__SYSPORT_ERP_GET(unit, modid, _sysport_erp)                    \
    _rv = _bcm_dpp_wb_stack__sysport_erp_get(unit, &_sysport_erp, modid); \
    if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: get _sysport_erp failed, unit %d"),FUNCTION_NAME(), unit)); \
    }
   
#define _DPP_STACK__SYSPORT_ERP_SET(unit, modid, _sysport_erp)   \
      _rv = _bcm_dpp_wb_stack__sysport_erp_set(unit, _sysport_erp, modid); \
      if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set _sysport_erp failed, unit %d"),FUNCTION_NAME(), unit)); \
      }


#define _DPP_STACK__MODID_TO_DOMAIN_GET(unit, domain)   \
        _bcm_dpp_wb_stack__modid_to_domain_get(unit, domain);
#define _DPP_STACK__MODID_TO_DOMAIN_SET(unit, domain, modid, val)   \
        _rv = _bcm_dpp_wb_stack__modid_to_domain_set(unit, domain, modid, val); \
        if (_rv != BCM_E_NONE) { \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set _modid_to_domain failed, unit %d"),FUNCTION_NAME(), unit)); \
      }

 
#define _DPP_STACK__STK_TRUNK_DOMAINS_GET(unit, stk_trunk)   \
        _bcm_dpp_wb_stack__stk_trunk_to_domains_get(unit, stk_trunk);        
#define _DPP_STACK__STK_TRUNK_DOMAINS_SET(unit, stk_trunk, domain, val);   \
        _rv = _bcm_dpp_wb_stack__stk_trunk_to_domain_set(unit, stk_trunk, domain, val); \
        if (_rv != BCM_E_NONE) {  \
        BCMDNX_ERR_EXIT_MSG(_rv, (_BSL_BCM_MSG("%s: set _domain_to_stk_trunk failed, unit %d"),FUNCTION_NAME(), unit)); \
      }
     
  
    
int
bcm_petra_stk_modid_get(
    int unit, 
    int *modid)
{  
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, modid));
exit:
    BCMDNX_FUNC_RETURN;
}   

int
bcm_petra_stk_my_modid_get(
    int unit, 
    int *my_modid)
{
    unsigned int soc_sand_my_modid = 0;
    int          rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    rv = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_mgmt_system_fap_id_get,(unit, &soc_sand_my_modid)));
    BCMDNX_IF_ERR_EXIT(rv);
    *my_modid = (int)soc_sand_my_modid;

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stk_modid_set(int unit, int modid)
{
    int          rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    if (modid < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_BADID, (_BSL_BCM_MSG("modid is invalid")));
    }

    rv = soc_dpp_petra_stk_modid_set(unit, modid);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stk_modid_config_get(
    int unit, 
    bcm_stk_modid_config_t *modid)
{  
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(modid);

    BCMDNX_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &(modid->modid)));
exit:
    BCMDNX_FUNC_RETURN;
}
  
int
bcm_petra_stk_modid_config_set(int unit, bcm_stk_modid_config_t* modid)
{
    int          rv = BCM_E_NONE;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);

    BCMDNX_NULL_CHECK(modid);

    if (modid->modid < 0) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_BADID, (_BSL_BCM_MSG("modid is invalid")));
    }

    rv = soc_dpp_petra_stk_modid_config_set(unit, modid->modid);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stk_modid_count(int unit, int *num_modid) 
{
    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (num_modid == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Uninitialized parameter num_modid")));
    }

    *num_modid = 1;
    BCM_EXIT; 
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stk_my_modid_set(int unit, int modid)
{
    BCMDNX_INIT_FUNC_DEFS;
    BCMDNX_IF_ERR_EXIT(bcm_petra_stk_modid_set(unit, modid));
exit:
    BCMDNX_FUNC_RETURN;
}


int
_bcm_petra_erp_sys_port_map_set(int unit,unsigned int soc_sand_dev_id, unsigned int sysport_id, int modid) {
    int soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;
    if (SOC_SAND_OK != soc_sand_take_chip_descriptor_mutex(soc_sand_dev_id)) {
        LOG_ERROR(BSL_LS_BCM_STK,
                  (BSL_META_U(unit,
                              "Error while taking soc_sand chip descriptor mutex")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error while tacking the chip mutex.")));
    }

    soc_sand_rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sys_virtual_port_to_local_port_map_set_unsafe,
        (unit, sysport_id, modid, SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID)));

    if (SOC_SAND_OK != soc_sand_give_chip_descriptor_mutex(soc_sand_dev_id)) {
        LOG_ERROR(BSL_LS_BCM_STK,
                  (BSL_META_U(unit,
                              "Error while giving soc_sand chip descriptor mutex")));
        BCMDNX_ERR_EXIT_MSG(BCM_E_INTERNAL, (_BSL_BCM_MSG("Internal error while giving the chip mutex.")));
    }   
    
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Map a System-Port to a Gport */
int
bcm_petra_stk_sysport_gport_set( int unit, bcm_gport_t sysport, bcm_port_t gport)
{
    unsigned int soc_sand_dev_id;
    unsigned int sysport_id;
    int          rv = BCM_E_NONE;
    int          dest_port = 0;
    int          modid;
    int          my_modid;
    uint32       tm_port;

    BCMDNX_INIT_FUNC_DEFS;

   _BCM_DPP_SWITCH_API_START(unit);

    BCM_DPP_UNIT_CHECK(unit);

    soc_sand_dev_id = (unit);

    /* Assume local until gport decode is complete */
    rv = bcm_petra_stk_my_modid_get(unit, &modid);

    if (rv) {
        BCMDNX_ERR_EXIT_MSG(rv, (_BSL_BCM_MSG("my modid invalid bcm_petra_stk_my_modid_get() rv=%s"), bcm_errmsg(rv)));
    }

    if (BCM_GPORT_IS_LOCAL(gport)) {

        dest_port = BCM_GPORT_LOCAL_GET(gport);

    } else if (BCM_GPORT_IS_MODPORT(gport)) {

        dest_port = BCM_GPORT_MODPORT_PORT_GET(gport);
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);

    } else if (BCM_COSQ_GPORT_IS_FMQ_CLASS(gport) || BCM_COSQ_GPORT_IS_ISQ_ROOT(gport)) { /* FMQ / ISQ special treatment */
        rv = bcm_petra_cosq_resource_gport_port_get(unit, gport, &dest_port);
        BCMDNX_IF_ERR_EXIT(rv);
    
    } else if (SOC_PORT_VALID(unit,gport)) {

        dest_port = gport;

    } else {
        LOG_ERROR(BSL_LS_BCM_STK,
                  (BSL_META_U(unit,
                              "unit(%d): Invalid gport type input to bcm_petra_stk_sysport_gport_set()\n"), unit));
        rv = BCM_E_PARAM;
    }

    if (BCM_GPORT_IS_SYSTEM_PORT(sysport)) {
        sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);
    } else {
        
        sysport_id = sysport;
    }

    /* ERP port required special attention, since it is not a physical port, and should
       be only mapped in the IPS (mapped to 0xff) */
    if (dest_port == BCM_DPP_PORT_INTERNAL_ERP(0)) {
        rv = _bcm_petra_erp_sys_port_map_set(unit,soc_sand_dev_id, sysport_id, modid);
        BCM_SAND_IF_ERR_EXIT(rv);

        /* Save system port for get API */
        _DPP_STACK__SYSPORT_ERP_SET(unit, modid, sysport_id);

    } else {
        BCM_SAND_IF_ERR_EXIT(bcm_petra_stk_my_modid_get(unit, &my_modid)); 
        tm_port = dest_port;
        
        rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sys_phys_to_local_port_map_set,
            (unit, sysport_id, modid, tm_port)));
        BCM_SAND_IF_ERR_EXIT(rv);
    }

    BCM_EXIT;
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/* Given a System-Port get the Gport */
int bcm_petra_stk_sysport_gport_get(int unit, bcm_gport_t sysport, bcm_gport_t *gport)
{
    unsigned int sysport_id;
    int          rv = BCM_E_NONE;
    int          modid;
    bcm_port_t   port;
    uint32    fap_id = 0;
    uint32    fap_port_id = 0;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (gport == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Uninitialized parameter gport")));
    }
 
    if (BCM_GPORT_IS_MODPORT(sysport)) {
        *gport = sysport;
        BCM_EXIT; 
    } else if(BCM_GPORT_IS_SYSTEM_PORT(sysport)){

        sysport_id = BCM_GPORT_SYSTEM_PORT_ID_GET(sysport);

        
        rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_sys_phys_to_local_port_map_get, 
            (unit, sysport_id, &fap_id, &fap_port_id)));
                                              
                                              
                                              
        BCM_SAND_IF_ERR_EXIT(rv);
        modid = BCM_SAND_FAP_ID_TO_BCM_MODID(fap_id);
        port = BCM_SAND_FAP_PORT_ID_TO_BCM_PORT(fap_port_id);
        
    } else if (BCM_GPORT_IS_LOCAL(sysport)) {
        rv = bcm_petra_stk_my_modid_get(unit, &modid);
        BCMDNX_IF_ERR_EXIT(rv);

        port = BCM_GPORT_LOCAL_GET(sysport);
    
    } else if (SOC_PORT_VALID(unit, sysport)) {
    
        rv = bcm_petra_stk_my_modid_get(unit, &modid);
        BCMDNX_IF_ERR_EXIT(rv);

        port = sysport;

    } else {
        LOG_ERROR(BSL_LS_BCM_STK,
                  (BSL_META_U(unit,
                              "gport type unsupported (0x%08x)\n"), sysport));
        BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);    
    }
    /* ERP port required special attention, since it is not a physical port, and should
       be only mapped in the IPS (mapped to 0xff) */
    if (port == SOC_TMC_FAP_EGRESS_REPLICATION_IPS_PORT_ID) {
        port = BCM_DPP_PORT_INTERNAL_ERP(0);
    }

    BCM_GPORT_MODPORT_SET(*gport, modid, port); 
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}


/* Given a Gport get the System-Port */
int 
bcm_petra_stk_gport_sysport_get(int unit, bcm_gport_t gport, bcm_gport_t *sysport)
{
    unsigned int sysport_id;
    int          rv = BCM_E_NONE;
    int          modid, core;
    bcm_port_t   port;
    uint32       tm_port;
    int          sysport_erp;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
    if (sysport == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Uninitialized parameter sysport")));
    }

    if (BCM_GPORT_IS_SYSTEM_PORT(gport)) {
        *sysport = gport;
        BCM_EXIT; 

    } else if (BCM_GPORT_IS_MODPORT(gport)) {
        modid = BCM_GPORT_MODPORT_MODID_GET(gport);
        port = BCM_GPORT_MODPORT_PORT_GET(gport);
    }
    else if (BCM_GPORT_IS_LOCAL(gport)) {
        rv = bcm_petra_stk_my_modid_get(unit, &modid);
        BCMDNX_IF_ERR_EXIT(rv);

        port = BCM_GPORT_LOCAL_GET(gport);
    
    } else if (SOC_PORT_VALID(unit, gport)) {
        rv = bcm_petra_stk_my_modid_get(unit, &modid);
        BCMDNX_IF_ERR_EXIT(rv);

        port = gport;

    } else {
        
        LOG_ERROR(BSL_LS_BCM_STK,
                  (BSL_META_U(unit,
                              "gport type unsupported (0x%08x)\n"), gport));
        BCM_ERR_EXIT_NO_MSG(BCM_E_PARAM);
    }
    if (port == BCM_DPP_PORT_INTERNAL_ERP(0)) {

        _DPP_STACK__SYSPORT_ERP_GET(unit, modid, sysport_erp);
        sysport_id = sysport_erp;

    } else {

        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_tm_port_get, (unit, port, &tm_port, &core)));

        rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_local_to_sys_phys_port_map_get, (unit, modid, tm_port, &sysport_id)));
    }
    BCM_SAND_IF_ERR_EXIT(rv);

    /* The MBCM_DPP_DRIVER_CALL initializing the required varible */ 
    /* coverity[uninit_use:FALSE] */
    BCM_GPORT_SYSTEM_PORT_ID_SET(*sysport, sysport_id); 
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

int
bcm_petra_stk_init(int unit)
{
#ifdef BCM_WARM_BOOT_SUPPORT
    int rv = BCM_E_NONE;
#endif /* BCM_WARM_BOOT_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   _BCM_DPP_SWITCH_API_START(unit);

#ifdef BCM_WARM_BOOT_SUPPORT
    rv = _bcm_dpp_wb_stack_state_init(unit);
    if (rv != BCM_E_NONE) {
            
        LOG_ERROR(BSL_LS_BCM_STK,
                  (BSL_META_U(unit,
                              "unit %d, failed warmboot init, error 0x%x\n"), unit, rv));
            
        BCMDNX_IF_ERR_EXIT(rv);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */
      

    BCM_EXIT;
exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

/*
 * Function:    _bcm_petra_stk_detach
 * Purpose:     Shuts down the stk module.
 * Parameters:  unit - Device unit number.
 * Returns:     BCM_E_NONE              Success.
 *              BCM_E_XXX
 */
int
_bcm_petra_stk_detach(int unit)
{
    BCMDNX_INIT_FUNC_DEFS;

#ifdef BCM_WARM_BOOT_SUPPORT    
    BCMDNX_IF_ERR_EXIT(_bcm_dpp_wb_stack_state_deinit(unit));
#endif /*BCM_WARM_BOOT_SUPPORT*/

    goto exit;
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_module_enable(
    int unit, 
    bcm_module_t modid, 
    int nports, 
    int enable)
{
    int rv;
    bcm_module_t my_modid;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   _BCM_DPP_SWITCH_API_START(unit);

    rv = bcm_petra_stk_modid_get(unit, &my_modid);
    BCMDNX_IF_ERR_EXIT(rv);

    if (modid != my_modid) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Invalid parameter modid")));
    }

    /* Calcualte MC Reserved Thresholds */
    if(enable)
    {
      rv = _bcm_cosq_before_traffic_validation(unit);
      BCMDNX_IF_ERR_EXIT(rv);
    }

    rv = soc_dpp_petra_stk_module_enable(unit, enable);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_modid_domain_add(
    int unit, 
    int modid, 
    int tm_domain)
{

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   _BCM_DPP_SWITCH_API_START(unit);
    
    /* Check params */
    if ((modid >= _DPP_STACK_MAX_DEVICES) || (tm_domain >= _DPP_STACK_MAX_TM_DOMAIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Params out of range")));
    }
    
    _DPP_STACK__MODID_TO_DOMAIN_SET(unit, tm_domain, modid, 1);

exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_modid_domain_delete(
    int unit, 
    int modid, 
    int tm_domain)
{

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   _BCM_DPP_SWITCH_API_START(unit);

    /* Check params */
    if ((modid >= _DPP_STACK_MAX_DEVICES) || (tm_domain >= _DPP_STACK_MAX_TM_DOMAIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: Params out of range")));
    }

     _DPP_STACK__MODID_TO_DOMAIN_SET(unit, tm_domain, modid, 0);

exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_modid_domain_get(
    int unit, 
    int tm_domain,
    int mod_max,
    int *mod_array,
    int *mod_count)
{

    uint32 
        modid,
        mod_count_tmp = 0x0,
        bit_val = 0x0,
        *modid_bm = 0;

    BCMDNX_INIT_FUNC_DEFS;

    /* Check params */
    if (tm_domain >= _DPP_STACK_MAX_TM_DOMAIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: Params out of range")));
    }

    /* Input parameters check. */
    if ((mod_array == NULL) || (mod_count== NULL) || (mod_max == 0x0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: NULL inputs")));
    }

    modid_bm = _DPP_STACK__MODID_TO_DOMAIN_GET(unit, tm_domain);
    if(modid_bm == NULL ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("ERROR: modid domain data is uninitialized")));
    } 

    for (modid = 0; modid < _DPP_STACK_MAX_DEVICES; modid++) {
        bit_val = SHR_BITGET(modid_bm, modid);

        if (bit_val != 0x0) {
            mod_array[mod_count_tmp] = modid;
            mod_count_tmp++;
        }
        bit_val = 0x0;

        if (mod_count_tmp == mod_max) {
            break;
        }
    }

    *mod_count = mod_count_tmp;

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_domain_stk_trunk_add(
    int unit, 
    int domain, 
    bcm_trunk_t stk_trunk)
{

    int rv;
    bcm_trunk_t stk_tid;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   _BCM_DPP_SWITCH_API_START(unit);

    stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk);

    /* Check params */
    if ((stk_tid >= _DPP_STACK_MAX_TM_DOMAIN) || (domain >= _DPP_STACK_MAX_TM_DOMAIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: Params out of range")));
    }

     _DPP_STACK__STK_TRUNK_DOMAINS_SET(unit, stk_tid, domain, 1);

    /* update Stack fec resolve table */
    rv = __bcm_petra_stk_trunk_domian_fec_map_update(unit, domain/*peer domain*/);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_domain_stk_trunk_delete(
    int unit, 
    int domain, 
    bcm_trunk_t stk_trunk)
{
    int rv;
    bcm_trunk_t stk_tid;

    BCMDNX_INIT_FUNC_DEFS;
    BCM_DPP_UNIT_CHECK(unit);
   _BCM_DPP_SWITCH_API_START(unit);

    stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk);

    /* Check params */
    if ((stk_tid >= _DPP_STACK_MAX_TM_DOMAIN) || (domain >= _DPP_STACK_MAX_TM_DOMAIN)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: Params out of range")));
    }

    _DPP_STACK__STK_TRUNK_DOMAINS_SET(unit, stk_tid, domain, 0);

    /* update Stack fec resolve table */
    rv = __bcm_petra_stk_trunk_domian_fec_map_update(unit, domain);
    BCMDNX_IF_ERR_EXIT(rv);

exit:
   _BCM_DPP_SWITCH_API_END(unit);
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_domain_stk_trunk_get(
    int unit, 
    bcm_trunk_t stk_trunk,
    int domain_max,
    int *domain_array,
    int *domain_count)
{

    uint32 
        domain,
        domain_count_tmp = 0x0,
        bit_val = 0x0,
        *domain_bm;
    bcm_trunk_t stk_tid;

    BCMDNX_INIT_FUNC_DEFS;

    stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk);    

    /* Check params */
    if (stk_tid >= _DPP_STACK_MAX_TM_DOMAIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: Params out of range")));
    }

    /* Input parameters check. */
    if ((domain_array == NULL) || (domain_count== NULL) || (domain_max == 0x0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: NULL inputs")));
    }

    domain_bm = _DPP_STACK__STK_TRUNK_DOMAINS_GET(unit, stk_tid);
    if(domain_bm == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("ERROR: stk trunk domain data is uninitialized")));
    } 

    for (domain = 0; domain < _DPP_STACK_MAX_TM_DOMAIN; domain++) {
    
        bit_val = SHR_BITGET(domain_bm, domain);

        if (bit_val != 0x0) {
            domain_array[domain_count_tmp] = domain;
            domain_count_tmp++;
        }
        bit_val = 0x0;

        if (domain_count_tmp == domain_max) {
            break;
        }
    }

    *domain_count = domain_count_tmp;

exit:
    BCMDNX_FUNC_RETURN;
}

int 
_bcm_petra_stk_trunk_domian_get(
    int unit, 
    int domain,
    int stk_trunk_max,
    bcm_trunk_t *stk_trunk_array,
    int *stk_trunk_count)
{

    uint32 
        stk_tid_ndx = 0,
        stk_tid_count_tmp = 0x0,
        bit_val = 0x0,
        *domain_bm;
    bcm_trunk_t stk_tid;

    BCMDNX_INIT_FUNC_DEFS;

    /* Check params */
    if (domain >= _DPP_STACK_MAX_TM_DOMAIN) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: Params out of range")));
    }

    /* Input parameters check. */
    if ((stk_trunk_array == NULL) || (stk_trunk_count== NULL) || (stk_trunk_max == 0x0)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("ERROR: NULL inputs")));
    }
    
    for (stk_tid_ndx = 0; stk_tid_ndx < _DPP_STACK_MAX_TM_DOMAIN; stk_tid_ndx++) {
    
        domain_bm = _DPP_STACK__STK_TRUNK_DOMAINS_GET(unit, stk_tid_ndx);
        if(domain_bm == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("ERROR: domain stk trunk data is uninitialized")));
        } 
    
        bit_val = SHR_BITGET(domain_bm, domain);

        if (bit_val != 0x0) {
             BCM_TRUNK_STACKING_TID_SET(stk_tid, stk_tid_ndx);
            stk_trunk_array[stk_tid_count_tmp] = stk_tid;
            stk_tid_count_tmp++;
        }
        bit_val = 0x0;

        if (stk_tid_count_tmp == stk_trunk_max) {
            break;
        }
    }

    *stk_trunk_count = stk_tid_count_tmp;

exit:
    BCMDNX_FUNC_RETURN;
}

int 
__bcm_petra_stk_trunk_domian_fec_map_update(
    int unit, 
    int domain)
{
    int rv, index, entry;
    uint32 soc_sand_rc;
    bcm_trunk_t stk_tid;
    bcm_trunk_t stk_trunk_array[_DPP_STACK_MAX_TM_DOMAIN];
    int stk_trunk_count;

    BCMDNX_INIT_FUNC_DEFS;

    sal_memset(stk_trunk_array, 0x0, sizeof(bcm_trunk_t) * _DPP_STACK_MAX_TM_DOMAIN);

    rv = _bcm_petra_stk_trunk_domian_get(unit, domain, _DPP_STACK_MAX_TM_DOMAIN, stk_trunk_array, &stk_trunk_count);
    BCMDNX_IF_ERR_EXIT(rv);


    if(stk_trunk_count > 4 ) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE, (_BSL_BCM_MSG("%s: try to add more than 4 stackin trunk to the same domain"),
                           FUNCTION_NAME()));
    }
    
    /* if no stk trunk defined revert it to init values */
    if (stk_trunk_count == 0x0) {
        stk_trunk_array[0] = domain;
        stk_trunk_count = 1;
    }

    index = 0x0;
    for (entry = 0; entry < SOC_TMC_IPQ_STACK_LAG_STACK_FEC_RESOLVE_ENTRY_MAX; entry++) {

        stk_tid = BCM_TRUNK_STACKING_TID_GET(stk_trunk_array[index]);

        /* Write entries to Stack fec resolve table  */
        soc_sand_rc = (MBCM_DPP_DRIVER_CALL(unit,mbcm_dpp_ipq_stack_fec_map_stack_lag_set,(unit, domain, entry, stk_tid)));
        if (SOC_SAND_FAILURE(soc_sand_rc)) {
            BCMDNX_ERR_EXIT_MSG(translate_sand_success_failure(soc_sand_rc), (_BSL_BCM_MSG("Failed setting stack_fec_map_stack_lag. domain=%d, entry=%d, stk_tid=%d."), domain, entry, stk_tid));
        }

        index = (index + 1) % stk_trunk_count;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

/* 
 *  Stacknig utility functions.
 */ 
int bcm_petra_stk_modid_to_domain_find(
    int unit, 
    uint32 local_modid, 
    uint32 sysport_modid, 
    uint32 *is_same_domain)
{
    int
        domain,
        local_tmd = -1,
        sysport_tmd = -1;
    uint32
        bit_local_modid_val,
        bit_sysport_modid_val,
        *modid_bm;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    if (is_same_domain == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL argument")));
    }

    *is_same_domain = 0x0;

    for (domain = 0; domain < _DPP_STACK_MAX_TM_DOMAIN; domain++) {
    
        modid_bm = _DPP_STACK__MODID_TO_DOMAIN_GET(unit, domain);
        if(modid_bm == NULL) {
            BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("ERROR: modid domain data is uninitialized")));
        } 
    
        bit_local_modid_val = SHR_BITGET(modid_bm, local_modid);
        bit_sysport_modid_val = SHR_BITGET(modid_bm, sysport_modid);

        if (bit_local_modid_val != 0x0) {
            local_tmd = domain;
        }
        if (bit_sysport_modid_val != 0x0) {
            sysport_tmd = domain;
        }
    }

    if ((local_tmd == -1) || (sysport_tmd == -1) || (local_tmd == sysport_tmd)) {
        *is_same_domain = 0x1;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_stk_domain_modid_get(
    int unit, 
    uint32 modid, 
    int *domain)
{
    int
        tmp_domain;
    uint32
        bit_modid_val,
        *modid_bm;

    BCMDNX_INIT_FUNC_DEFS;

    BCM_DPP_UNIT_CHECK(unit);
    if (domain == NULL) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("NULL argument")));
    }

    *domain = _DPP_STACK_INVALID_DOMAIN;

    /* find in which tm domain the gport is */
    for (tmp_domain = 0; tmp_domain < _DPP_STACK_MAX_TM_DOMAIN; tmp_domain++) {
    
        modid_bm = _DPP_STACK__MODID_TO_DOMAIN_GET(unit, tmp_domain);
        if(modid_bm == NULL) {
             BCMDNX_ERR_EXIT_MSG(BCM_E_MEMORY, (_BSL_BCM_MSG("ERROR: modid domain data is uninitialized")));
        }

        bit_modid_val = SHR_BITGET(modid_bm, modid);
        if ((bit_modid_val != 0x0)) {
            *domain = tmp_domain;
            break;
        }
    }

    if (*domain == _DPP_STACK_INVALID_DOMAIN) {
        LOG_VERBOSE(BSL_LS_BCM_COMMON,
                    (BSL_META_U(unit,
                                "Failed to find TM-domain for modid=%d"),
                     modid));
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}

int bcm_petra_stk_system_gport_map_create(
    int unit, 
    bcm_stk_system_gport_map_t *sys_gport_map)
{

  SOC_PPD_EG_ENCAP_DATA_INFO data_info;
  SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
  SOC_PPD_EG_ENCAP_ENTRY_TYPE entry_type;
  uint32 
    soc_sand_rv,
    in_cud = sys_gport_map->dest_gport,
    out_cud = sys_gport_map->system_gport,
    second_cud,
    entry_index,
    nof_entries,
    port_idx,
    next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];  
  uint8
    two_hop_scheduling = 0,
    is_even_entry;
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF olif_ext_en;

  BCMDNX_INIT_FUNC_DEFS;
  
  SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_info);
  unit = (unit);

#ifdef BCM_ARAD_SUPPORT
  if((SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_stk_system_gport_map_create supported only in tm mode")));
  }
#endif /* BCM_ARAD_SUPPORT */

  for (port_idx = 0; port_idx < ARAD_NOF_FAP_PORTS; port_idx++) {
      BCMDNX_IF_ERR_EXIT(soc_port_sw_db_olif_ext_en_get(unit, port_idx, &olif_ext_en));
      if (olif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG) {
          two_hop_scheduling = 1;
      }
  }
  if(((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) == FALSE && !two_hop_scheduling) &&
     SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge == FALSE) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_stk_system_gport_map_create supported only when MC CUD is mapped")));
  }
  if (!SOC_IS_ARAD_B1_AND_BELOW(unit))
  {
      /* 
       *  Verify that:
       *  in_cud holds no more than 17 bits
       *  out_cud holds no more than 21 bits
       */
      if (0x1ffff < in_cud) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 17 bits value"), in_cud));
      }
      if (0x1fffff < out_cud) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("out_cud(%d) should not hold more than 21 bits value"), out_cud));
      }

      /*
       * CUD21[18:0] comes from EPNI_EEDB_BANKm array index: CUD17[16..13]] memory index: CUD[12..2], 
       * Each entry contains four 19 bit values selectable by CUD17[1..0].
       *  
       * CUD21[20:19] comes from EPNI_DSCP_REMARKm index CUD17[16..4], 
       * Each entry contains sixteen two bit values selectable using CUD17[3..0].
       * 
       */
      /*use a SOC layer function accessing EPNI_EEDB_BANKm the for tm mode*/
      soc_sand_rv = soc_ppd_eg_encap_data_local_to_global_cud_set(unit,in_cud,out_cud);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  } else {
      if (two_hop_scheduling) {
          /* 
           *  Verify that:
           *  in_cud holds no more than 16 bits
           *  out_cud holds no more than 28 bits
           */
          if (0xffff < in_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 16 bits value"), in_cud));
          }
          if (0xfffffff < out_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("out_cud(%d) should not hold more than 28 bits value"), out_cud));
          }

          /* Each DB entry will hold 2 pairs of Flow-ID + Out-LIF (both encoded in out_cud) */
          /* So, entry X will hold CUDs X and X+1 */
          is_even_entry = ((in_cud % 2 == 0) ? 1 : 0);

          /* Always access with an even entry */
          entry_index = (is_even_entry ? in_cud : in_cud - 1);

          /* Check if it the entry exist */
          soc_sand_rv = soc_ppd_eg_encap_entry_type_get(unit, entry_index, &entry_type);
          if(entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA)
          {
            /* Entry exist */
            /* We want to update only the requested CUD */
            /* So we will read the other CUD of the same entry */
            /* And then we will write the entire entry */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0, 
                                                     encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

            if(is_even_entry) {
              /* Save the odd CUD */
              second_cud = SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(unit, 1, &encap_entry_info[0].entry_val.data_info);
            } else {
              /* Save the even CUD */
              second_cud = SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(unit, 0, &encap_entry_info[0].entry_val.data_info);
            }
          }
          else
          {
            /* Entry does NOT exist */
            second_cud = 0;
          }

          if(is_even_entry) {
            /* Update the even CUD */
            SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_SET(unit, out_cud, second_cud, &data_info);
          } else {
            /* Update the odd CUD */
            SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_SET(unit, second_cud, out_cud, &data_info);
          }

          /* Update the DB entry */
          soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, entry_index, &data_info, FALSE, 0);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

      }
      else {
          /* 
           *  Verify that:
           *  in_cud holds no more than 16 bits
           *  out_cud holds no more than 24 bits
           */
          if (0xffff < in_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 16 bits value"), in_cud));
          }
          if (0xffffff < out_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("out_cud(%d) should not hold more than 24 bits value"), out_cud));
          }

          /* Each DB entry will hold 2 CUDs       */
          /* So, entry X will hold CUDs X and X+1 */
          is_even_entry = ((in_cud % 2 == 0) ? 1 : 0);

          /* Always access with an even entry */
          entry_index = (is_even_entry ? in_cud : in_cud - 1);

          /* Check if it the entry exist */
          soc_sand_rv = soc_ppd_eg_encap_entry_type_get(unit, entry_index, &entry_type);
          if(entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA)
          {
            /* Entry exist */
            /* We want to update only the requested CUD */
            /* So we will read the other CUD of the same entry */
            /* And then we will write the entire entry */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0, 
                                                     encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

            if(is_even_entry) {
              /* Save the odd CUD */
              second_cud = SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(unit, 1, &encap_entry_info[0].entry_val.data_info);
            } else {
              /* Save the even CUD */
              second_cud = SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(unit, 0, &encap_entry_info[0].entry_val.data_info);
            }
          }
          else
          {
            /* Entry does NOT exist */
            second_cud = 0;
          }

          if(is_even_entry) {
            /* Update the even CUD */
            SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_SET(unit, out_cud, second_cud, &data_info);
          } else {
            /* Update the odd CUD */
            SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_SET(unit, second_cud, out_cud, &data_info);
          }

          /* Update the DB entry */
          soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, entry_index, &data_info, FALSE, 0);
          BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
      }
  }

exit:
    BCMDNX_FUNC_RETURN;
}

/* Get mapping from local gport id to system gport id */
int bcm_petra_stk_system_gport_map_get(
    int unit, 
    bcm_stk_system_gport_map_t *sys_gport_map)
{

  SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
  SOC_PPD_EG_ENCAP_ENTRY_TYPE entry_type;
  uint32 
    soc_sand_rv,
    in_cud = sys_gport_map->dest_gport,
    out_cud,
    entry_index,
    nof_entries,
    port_idx,
    next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
  uint8
    two_hop_scheduling = 0,
    is_even_entry;
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF olif_ext_en;

  BCMDNX_INIT_FUNC_DEFS;
  
  unit = (unit);

  for (port_idx = 0; port_idx < ARAD_NOF_FAP_PORTS; port_idx++) {
      BCMDNX_IF_ERR_EXIT(soc_port_sw_db_olif_ext_en_get(unit, port_idx, &olif_ext_en));
      if (olif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG) {
          two_hop_scheduling = 1;
      }
  }    
  if(((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) == FALSE && !two_hop_scheduling) && 
     SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge == FALSE) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_stk_system_gport_map_create supported only when MC CUD is mapped")));
  }
#ifdef BCM_ARAD_SUPPORT
  if((SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_stk_system_gport_map_create supported only in tm mode")));
  }
  if (!SOC_IS_ARAD_B1_AND_BELOW(unit))
  {
      /* 
       *  Verify that:
       *  in_cud holds no more than 17 bits
       */
      if (0x1ffff < in_cud) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 17 bits value"), in_cud));
      }
      /*
       * CUD21[18:0] comes from EPNI_EEDB_BANKm array index: CUD17[16..13]] memory index: CUD[12..2], 
       * Each entry contains four 19 bit values selectable by CUD17[1..0].
       *  
       * CUD21[20:19] comes from EPNI_DSCP_REMARKm index CUD17[16..4], 
       * Each entry contains sixteen two bit values selectable using CUD17[3..0].
       * 
       */
      /*use a SOC layer function accessing EPNI_EEDB_BANKm and EPNI_DSCP_REMARKm*/
      soc_sand_rv = soc_ppd_eg_encap_data_local_to_global_cud_get(unit,in_cud,&out_cud);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  } else 
#endif /* BCM_ARAD_SUPPORT */
  {
      if (two_hop_scheduling) {
          /* 
           *  Verify that:
           *  in_cud holds no more than 16 bits
           */
          if (0xffff < in_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 16 bits value"), in_cud));
          }

          /* Each DB entry will hold 2 pairs of Flow-ID + Out-LIF (both encoded in out_cud) */
          /* So, entry X will hold CUDs X and X+1 */
          is_even_entry = ((in_cud % 2 == 0) ? 1 : 0);

          /* Always access with an even entry */
          entry_index = (is_even_entry ? in_cud : in_cud - 1);

          /* Check if it the entry exist */
          soc_sand_rv = soc_ppd_eg_encap_entry_type_get(unit, entry_index, &entry_type);
          if(entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA)
          {
            /* Entry exist */
            /* We want to get only the requested CUD */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0, 
                                                     encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

            if(is_even_entry) {
              /* Read the even CUD */
              out_cud = SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(unit, 0, &encap_entry_info[0].entry_val.data_info);
            } else {
              /* Read the odd CUD */
              out_cud = SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(unit, 1, &encap_entry_info[0].entry_val.data_info);
            }  
          }
          else
          {
            /* Entry does NOT exist */
            out_cud = 0;
          }

      }
      else {
          /* 
           *  Verify that:
           *  in_cud holds no more than 16 bits
           */
          if (0xffff < in_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 16 bits value"), in_cud));
          }
          /* Each DB entry will hold 2 CUDs       */
          /* So, entry X will hold CUDs X and X+1 */
          is_even_entry = ((in_cud % 2 == 0) ? 1 : 0);

          /* Always access with an even entry */
          entry_index = (is_even_entry ? in_cud : in_cud - 1);

          /* Check if the entry exist */
          soc_sand_rv = soc_ppd_eg_encap_entry_type_get(unit, entry_index, &entry_type);
          if(entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA)
          {
            /* Entry exist */
            /* We want to get only the requested CUD */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0, 
                                                     encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

            if(is_even_entry) {
              /* Read the even CUD */
              out_cud = SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(unit, 0, &encap_entry_info[0].entry_val.data_info);
            } else {
              /* Read the odd CUD */
              out_cud = SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(unit, 1, &encap_entry_info[0].entry_val.data_info);
            }  
          }
          else
          {
            /* Entry does NOT exist */
            out_cud = 0;
          }
      }
  }
  /* Return the mapping of the requested CUD */
  sys_gport_map->system_gport = out_cud;

exit:
  BCMDNX_FUNC_RETURN;
}

/* Destroy mapping from local gport id to system gport id */
int bcm_petra_stk_system_gport_map_destroy(
    int unit, 
    bcm_gport_t system_gport)
{

  SOC_PPD_EG_ENCAP_DATA_INFO data_info;
  SOC_PPD_EG_ENCAP_ENTRY_INFO encap_entry_info[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
  SOC_PPD_EG_ENCAP_ENTRY_TYPE entry_type;
  uint32 
    soc_sand_rv,
    in_cud = system_gport,
    cud1, cud2,
    entry_index,
    nof_entries,
    port_idx,
    next_eep[SOC_PPD_NOF_EG_ENCAP_EEP_TYPES];
  uint8
    two_hop_scheduling = 0,
    is_even_entry;
  SOC_TMC_PORTS_FTMH_EXT_OUTLIF olif_ext_en;

  BCMDNX_INIT_FUNC_DEFS;
  
  SOC_PPD_EG_ENCAP_DATA_INFO_clear(&data_info);
#ifdef BCM_ARAD_SUPPORT
  if((SOC_DPP_CONFIG(unit)->arad->init.pp_enable == TRUE)) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_stk_system_gport_map_create supported only in tm mode")));
  }
#endif /* BCM_ARAD_SUPPORT */
  for (port_idx = 0; port_idx < ARAD_NOF_FAP_PORTS; port_idx++) {
      BCMDNX_IF_ERR_EXIT(soc_port_sw_db_olif_ext_en_get(unit, port_idx, &olif_ext_en));
      if (olif_ext_en == SOC_TMC_PORTS_FTMH_EXT_OUTLIF_DOUBLE_TAG) {
          two_hop_scheduling = 1;
      }
  }
  if(((SOC_DPP_CONFIG(unit)->tm.mc_mode & DPP_MC_CUD_EXTENSION_MODE) == FALSE && !two_hop_scheduling) && 
     SOC_DPP_CONFIG(unit)->tm.ftmh_outlif_enlarge == FALSE) {
      BCMDNX_ERR_EXIT_MSG(BCM_E_CONFIG, (_BSL_BCM_MSG("bcm_petra_stk_system_gport_map_create supported only when MC CUD is mapped")));
  }
  if (!SOC_IS_ARAD_B1_AND_BELOW(unit))
  {
      /* 
       *  Verify that:
       *  in_cud holds no more than 17 bits
       */
      if (0x1ffff < in_cud) {
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 17 bits value"), in_cud));
      }
      /*
       * CUD21[18:0] comes from EPNI_EEDB_BANKm array index: CUD17[16..13]] memory index: CUD[12..2], 
       * Each entry contains four 19 bit values selectable by CUD17[1..0].
       *  
       * CUD21[20:19] comes from EPNI_DSCP_REMARKm index CUD17[16..4], 
       * Each entry contains sixteen two bit values selectable using CUD17[3..0].
       * 
       */
      /*use a SOC layer function accessing EPNI_EEDB_BANKm the for tm mode*/
      /*map CUD to itself*/
      soc_sand_rv = soc_ppd_eg_encap_data_local_to_global_cud_set(unit,in_cud,in_cud);
      BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

  } else {
      if (two_hop_scheduling) {
          /* 
           *  Verify that:
           *  in_cud holds no more than 16 bits
           */
          if (0xffff < in_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 16 bits value"), in_cud));
          }

          /* Each DB entry will hold 2 pairs of Flow-ID + Out-LIF (both encoded in out_cud) */
          /* So, entry X will hold CUDs X and X+1 */
          is_even_entry = ((in_cud % 2 == 0) ? 1 : 0);

          /* Always access with an even entry */
          entry_index = (is_even_entry ? in_cud : in_cud - 1);

          /* Check if it the entry exist */
          soc_sand_rv = soc_ppd_eg_encap_entry_type_get(unit, entry_index, &entry_type);
          if(entry_type == SOC_PPD_EG_ENCAP_ENTRY_TYPE_DATA)
          {
              /* Entry exist */
              /* We want to update only the requested CUD */
              /* So we will read the other CUD of the same entry */
              /* And then we will write the entire entry */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0, 
                                                     encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

            /* Read both CUDs */
            cud1 = SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(unit, 0, &encap_entry_info[0].entry_val.data_info);
            cud2 = SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_GET(unit, 1, &encap_entry_info[0].entry_val.data_info);

            if(is_even_entry) {
              /* Delete the even CUD */
              SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_SET(unit, 0, cud2, &data_info);
            } else {
              /* Delete the odd CUD */
              SOC_PPD_EG_ENCAP_DATA_TWO_HOP_SCH_INFO_SET(unit, cud1, 0, &data_info);
            }

            /* Update the DB entry */
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, entry_index, &data_info, FALSE, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
      }
      else {
          /* 
           *  Verify that:
           *  in_cud holds no more than 16 bits
           */
          if (0xffff < in_cud) {
              BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("in_cud(%d) should not hold more than 16 bits value"), in_cud));
          }
          /* Each DB entry will hold 2 CUDs       */
          /* So, entry X will hold CUDs X and X+1 */
          is_even_entry = ((in_cud % 2 == 0) ? 1 : 0);

          /* Always access with an even entry */
          entry_index = (is_even_entry ? in_cud : in_cud - 1);

          /* Check if the entry exist */
          soc_sand_rv = soc_ppd_eg_encap_entry_type_get(unit, entry_index, &entry_type);
          if(entry_type == SOC_PPC_EG_ENCAP_ENTRY_TYPE_DATA)
          {
            /* Entry exist */
            /* We want to update only the requested CUD */
            /* So we will read the other CUD of the same entry */
            /* And then we will write the entire entry */
            soc_sand_rv = soc_ppd_eg_encap_entry_get(unit, SOC_PPC_EG_ENCAP_EEP_TYPE_DATA, entry_index, 0, 
                                                     encap_entry_info, next_eep, &nof_entries);
            SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

            /* Read both CUDs */
            cud1 = SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(unit, 0, &encap_entry_info[0].entry_val.data_info);
            cud2 = SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_GET(unit, 1, &encap_entry_info[0].entry_val.data_info);
            
            if(is_even_entry) {
              /* Delete the even CUD */
              SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_SET(unit, 0, cud2, &data_info);
            } else {
              /* Delete the odd CUD */
              SOC_PPD_EG_ENCAP_DATA_CUD_EXTEND_SET(unit, cud1, 0, &data_info);
            }

            /* Update the DB entry */
            soc_sand_rv = soc_ppd_eg_encap_data_lif_entry_add(unit, entry_index, &data_info, FALSE, 0);
            BCM_SAND_IF_ERR_EXIT(soc_sand_rv);
          }
      }
  }

exit:
  BCMDNX_FUNC_RETURN;
}

/* Set remote mapping between local port and remote port, remote modid */
int bcm_petra_stk_modport_remote_map_set(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    int remote_modid, 
    bcm_port_t remote_port)
{
#ifdef BCM_ARAD_SUPPORT
    bcm_error_t rv;
    uint32 port_sw_db_flags = 0;
    bcm_port_t local_port;
    SOC_TMC_PORTS_APPLICATION_MAPPING_INFO info;   
    _bcm_dpp_gport_info_t gport_info;
#endif /* BCM_ARAD_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;  

    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_stk_modport_remote_map_set not supported ")));
    }    

#ifdef BCM_ARAD_SUPPORT    
    if (SOC_IS_ARAD(unit)) {
        /* 
         * Valid port is XGS MAC Extender
         */
        SOC_TMC_PORTS_APPLICATION_MAPPING_INFO_clear(&info);

        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {

            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit,local_port, &port_sw_db_flags));

            if (!(port_sw_db_flags & SOC_PORT_FLAGS_XGS_MAC_EXT)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port %d. Only XGS MAC extender port can be supported using bcm_petra_stk_modport_remote_map_set"),local_port));
            }
            /* 
             * Map XGS modid, XGS port to local port
             */
            info.type = SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER;
            info.value.xgs_mac_ext.hg_modid = remote_modid & 0xFF;
            info.value.xgs_mac_ext.hg_port  = remote_port  & 0xFF;
            info.value.xgs_mac_ext.pp_port  = local_port;
            rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_set, (unit, local_port, &info)));
            BCM_SAND_IF_ERR_EXIT(rv);        
        }
    }
#endif /* BCM_ARAD_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}


/* Get remote mapping between local port and remote port, remote modid */
int bcm_petra_stk_modport_remote_map_get(
    int unit, 
    bcm_gport_t port, 
    uint32 flags, 
    int *remote_modid, 
    bcm_port_t *remote_port)
{    
#ifdef BCM_ARAD_SUPPORT
    bcm_error_t rv;
    uint32 port_sw_db_flags = 0;
    bcm_port_t local_port;
    SOC_TMC_PORTS_APPLICATION_MAPPING_INFO info;   
    _bcm_dpp_gport_info_t gport_info;
#endif /* BCM_ARAD_SUPPORT */

    BCMDNX_INIT_FUNC_DEFS;  

    if (SOC_IS_PETRAB(unit)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_UNAVAIL, (_BSL_BCM_MSG("bcm_petra_stk_modport_remote_map_set not supported ")));
    }    

#ifdef BCM_ARAD_SUPPORT    
    if (SOC_IS_ARAD(unit)) {
        /* 
         * Valid port is XGS MAC Extender
         */
        SOC_TMC_PORTS_APPLICATION_MAPPING_INFO_clear(&info);

        rv = _bcm_dpp_gport_to_phy_port(unit, port, 0, &gport_info); 
        BCMDNX_IF_ERR_EXIT(rv);

        BCM_PBMP_ITER(gport_info.pbmp_local_ports, local_port) {

            BCMDNX_IF_ERR_EXIT(soc_port_sw_db_flags_get(unit,local_port, &port_sw_db_flags));

            if (!(port_sw_db_flags & SOC_PORT_FLAGS_XGS_MAC_EXT)) {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PORT, (_BSL_BCM_MSG("Port %d. Only XGS MAC extender port can be supported using bcm_petra_stk_modport_remote_map_set"),local_port));
            }
            /* 
             * Map XGS modid, XGS port from local port
             */
            info.type = SOC_TMC_PORTS_APPLICATION_MAPPING_TYPE_XGS_MAC_EXTENDER;
            
            rv = (MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_ports_application_mapping_info_get, (unit, local_port, &info)));
            BCM_SAND_IF_ERR_EXIT(rv);        

            *remote_port   = info.value.xgs_mac_ext.hg_port;
            *remote_modid  = info.value.xgs_mac_ext.hg_modid;
        }
    }
#endif /* BCM_ARAD_SUPPORT */

exit:
    BCMDNX_FUNC_RETURN;
}


int 
bcm_petra_stk_module_control_get(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int *arg)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(arg);

    if (!BCM_FABRIC_MODID_IS_VALID(unit, module)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("module is out of range")));
    }

    switch (control) {
    case bcmStkModuleAllReachableIgnore:
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_get, (unit, module, arg)));
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_SOCDNX_MSG("Invalid control %d"),control));
        break;
    } 
       
exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_module_control_set(
    int unit, 
    uint32 flags, 
    bcm_module_t module, 
    bcm_stk_module_control_t control, 
    int arg)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!BCM_FABRIC_MODID_IS_VALID(unit, module)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("module is out of range")));
    }

    switch (control) {
    case bcmStkModuleAllReachableIgnore:
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_all_reachable_ignore_id_set, (unit, module, arg)));
        break;
    default:
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_SOCDNX_MSG("Invalid control %d"),control));
        break;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_module_max_get(
    int unit, 
    uint32 flags, 
    bcm_module_t *max_module)
{
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(max_module);

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE){
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_max_all_reachable_get, (unit, max_module)));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_max_get, (unit, max_module)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}

int 
bcm_petra_stk_module_max_set(
    int unit, 
    uint32 flags, 
    bcm_module_t max_module)
{
    BCMDNX_INIT_FUNC_DEFS;

    if (!BCM_FABRIC_MODID_IS_VALID(unit, max_module)) {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("max_module is out of range")));
    }
    BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_devide_by_32_verify, (unit, max_module)));

    if (flags & BCM_STK_MODULE_MAX_ALL_REACHABLE){
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_max_all_reachable_set, (unit, max_module)));
    } else {
        BCMDNX_IF_ERR_EXIT(MBCM_DPP_DRIVER_CALL(unit, mbcm_dpp_fabric_stack_module_max_set, (unit, max_module)));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
int
_bcm_dpp_stk_sw_dump(int unit)
{
    uint32 i;
    int sysport_erp;

    BCMDNX_INIT_FUNC_DEFS;

    /* Make sure the stack module is initialized */
    BCM_DPP_UNIT_CHECK(unit);


    LOG_CLI((BSL_META_U(unit,
                        "\nSTACK:")));
    LOG_CLI((BSL_META_U(unit,
                        "\n------")));


    LOG_CLI((BSL_META_U(unit,
                        "\n\n_sysport_erp:\n")));

    for(i = 0; i < _DPP_STACK_MAX_DEVICES; i++) {

        _DPP_STACK__SYSPORT_ERP_GET(unit, i, sysport_erp);
        
        if(0 != sysport_erp) {
            LOG_CLI((BSL_META_U(unit,
                                "  (%d) %13d\n"),
                     i, 
                     sysport_erp));
        }
        
    }

    

    LOG_CLI((BSL_META_U(unit,
                        "\n")));


exit:
    BCMDNX_FUNC_RETURN;
}



#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */
 	  	 

