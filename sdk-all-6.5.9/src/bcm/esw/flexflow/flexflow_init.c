
#include <sal/core/libc.h>
#if defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/scache.h>
#include <bcm/error.h>
#include <bcm/module.h>
#include <bcm_int/esw/trx.h>
#include <bcm_int/esw/xgs5.h>
#include <bcm_int/esw/flow.h>
#include <bcm_int/esw/switch.h>
#include <soc/esw/flow_db_core.h>
#include <bcm_int/esw/virtual.h>

_bcm_flow_bookkeeping_t   *_bcm_flow_bk_info[BCM_MAX_NUM_UNITS] = { 0 };

#define VIRTUAL_INFO(_unit_)    (&_bcm_virtual_bk_info[_unit_])
#define L3_INFO(_unit_)         (&_bcm_l3_bk_info[_unit_])

/*
 * Function:
 *      _bcm_td3_flow_free_resource
 * Purpose:
 *      Free all allocated software resources 
 * Parameters:
 *      unit - SOC unit number
 * Returns:
 *      Nothing
 */

STATIC void
_bcm_td3_flow_free_resource(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info = FLOW_INFO(unit);

    /* If software tables were not allocated we are done. */ 
    if (NULL == FLOW_INFO(unit)) {
        return;
    }

    /* Destroy EGR_DVP_ATTRIBUTE usage bitmap  */
    if (flow_info->dvp_attr_bitmap) {
        sal_free(flow_info->dvp_attr_bitmap);
        flow_info->dvp_attr_bitmap = NULL;
    }

    /* Destroy soft init tunnel table   */
    if (flow_info->init_tunnel) {
        sal_free(flow_info->init_tunnel);
        flow_info->init_tunnel = NULL;
    }

    /* Destroy iif_ref_cnt table   */
    if (flow_info->iif_ref_cnt) {
        sal_free(flow_info->iif_ref_cnt);
        flow_info->iif_ref_cnt = NULL;
    }

    /* Destroy vp_ref_cnt table   */
    if (flow_info->vp_ref_cnt) {
        sal_free(flow_info->vp_ref_cnt);
        flow_info->vp_ref_cnt = NULL;
    }

/*
    if (flow_info->flow_tunnel_term) {
        sal_free(flow_info->flow_tunnel_term);
        flow_info->flow_tunnel_term = NULL;
    }
*/

    if (flow_info->flow_vpn_vlan) {
        sal_free(flow_info->flow_vpn_vlan);
        flow_info->flow_vpn_vlan = NULL;
    }

    /* Free module data. */
    sal_free(FLOW_INFO(unit));
    FLOW_INFO(unit) = NULL;
}
/*
 * Function:
 *      bcm_td3_flow_allocate_bk
 * Purpose:
 *      Initialize FLOW software book-kepping
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
bcm_td3_flow_allocate_bk(int unit)
{
    /* Allocate/Init unit software tables. */
    if (NULL == FLOW_INFO(unit)) {
        BCM_TD3_FLOW_ALLOC(FLOW_INFO(unit), sizeof(_bcm_flow_bookkeeping_t),
                          "flow_bk_module_data");
        if (NULL == FLOW_INFO(unit)) {
            return (BCM_E_MEMORY);
        } else {
            FLOW_INFO(unit)->initialized = FALSE;
        }
    }
    return BCM_E_NONE;
}

#ifdef BCM_WARM_BOOT_SUPPORT

#define BCM_WB_VERSION_1_0                SOC_SCACHE_VERSION(1, 0)
#define BCM_WB_DEFAULT_VERSION            BCM_WB_VERSION_1_0

STATIC int _bcm_flow_wb_alloc(int unit);

/*
 * Function:
 *      _bcm_flow_wb_recover
 *
 * Purpose:
 *      Recover VXLAN module info for Level 2 Warm Boot from persisitent memory
 *
 * Warm Boot Version Map:
 *      see _bcm_esw_flow_sync definition
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_wb_recover(int unit)
{
    int sz = 0, rv = BCM_E_NONE;
    int num_dvp = 0;
    int num_iif;
    int num_vp;
    int stable_size;
    uint16 recovered_ver = 0;
    uint8 *flow_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_flow_bookkeeping_t *flow_info;
    int num_soft_tnl = 0;
    int i;

    flow_info = FLOW_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }
    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOW, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &flow_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, &recovered_ver);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    } else if (rv == BCM_E_NOT_FOUND) {
        return _bcm_flow_wb_alloc(unit);
    }

    if (flow_scache_ptr != NULL) {
        num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
        num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);
        sz = sizeof(uint16);  /* idx of _bcm_flow_init_tunnel_entry_t */

        /* recover init_tunnel state */
        for (i = 0; i < num_soft_tnl; i++) {
            sal_memcpy(&flow_info->init_tunnel[i].idx, flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }

        /* recover the dvp_attr_bitmap */
        sz = SHR_BITALLOCSIZE(num_dvp);
        sal_memcpy(flow_info->dvp_attr_bitmap,flow_scache_ptr,sz);
        flow_scache_ptr += sz;

        num_iif = soc_mem_index_count(unit, L3_IIFm);
        sz =  sizeof(uint16);  /* iif_ref_cnt */
        for (i = 0; i < num_iif; i++) {
            sal_memcpy(&flow_info->iif_ref_cnt[i], flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }

        num_vp = soc_mem_index_count(unit, SOURCE_VPm);
        sz =  sizeof(uint16);  /* vp_ref_cnt */
        for (i = 0; i < num_vp; i++) {
            sal_memcpy(&flow_info->vp_ref_cnt[i], flow_scache_ptr, sz);
            flow_scache_ptr += sz;
        }
#if 0
        /* Reallocate additional scache size if current scache requirement
         * is more than the one recovered.
         */ 
        if (additional_scache_size > 0) {
            rv = soc_scache_realloc(unit,scache_handle,additional_scache_size);
            if(BCM_FAILURE(rv)) {
               return rv;
            }
        }
#endif
    }
    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_wb_alloc
 *
 * Purpose:
 *      Alloc persisitent memory for Level 2 Warm Boot scache.
 *
 * Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */

STATIC int
_bcm_flow_wb_alloc(int unit)
{
    int alloc_sz = 0, rv = BCM_E_NONE;
    soc_scache_handle_t scache_handle;
    int num_soft_tnl = 0, num_dvp = 0;
    uint8 *flow_scache_ptr = NULL;
    int stable_size;
    int num_iif;
    int num_vp;

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    /* init tunnel entry table size */
    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);
    /* num_soft_tnl * idx of _bcm_flow_init_tunnel_entry_t */
    alloc_sz = num_soft_tnl * sizeof(uint16); 

    /* dvp_attr_bitmap size */
    alloc_sz += SHR_BITALLOCSIZE(num_dvp);

    /* iif_ref_cnt table size */
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    /* num_iif * iif_ref_cnt */
    alloc_sz += num_iif * sizeof(uint16);

    /* vp_ref_cnt table size */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    /* num_vp * vp_ref_cnt */
    alloc_sz += num_vp * sizeof(uint16);

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOW, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, TRUE,
                                 alloc_sz, (uint8**)&flow_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_NOT_FOUND)) {
        return rv;
    }

    return BCM_E_NONE;
}

/*
 * Function:
 *      _bcm_flow_reinit
 * Purpose:
 *      Warm boot recovery for the VXLAN software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_bcm_flow_reinit(int unit)
{
    int rv = BCM_E_NONE;

    /* Tunnel initiator hash and ref-count recovery */
    rv = _bcm_flow_tunnel_initiator_reinit(unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }

    /* Recover L2 scache */
    rv = _bcm_flow_wb_recover(unit);
    return rv;
}

/*
 * Function:
 *      _bcm_esw_flow_sync
 *
 * Purpose:
 *      Record flow module persistent info for Level 2 Warm Boot
 *
 * Warm Boot Version Map:
 *  WB_VERSION_1_0
* Parameters:
 *      unit - (IN) Device Unit Number.
 *
 * Returns:
 *      BCM_E_XXX
 */

int
_bcm_esw_flow_sync(int unit)
{
    int sz = 0, rv = BCM_E_NONE;
    int num_soft_tnl = 0, num_dvp = 0;
    int stable_size;
    uint8 *flow_scache_ptr = NULL;
    soc_scache_handle_t scache_handle;
    _bcm_flow_bookkeeping_t *flow_info;
    int num_iif;
    int num_vp;
    int i;

    if (!soc_feature(unit, soc_feature_flex_flow)) {
        return BCM_E_UNAVAIL;
    }

    /* Parameter validation checks */
    if (!SOC_UNIT_VALID(unit)) {
        return BCM_E_UNIT;
    }
    BCM_IF_ERROR_RETURN(bcmi_esw_flow_check_init(unit));

    flow_info = FLOW_INFO(unit);

    SOC_IF_ERROR_RETURN(soc_stable_size_get(unit, &stable_size));

    /* Requires extended scache support level-2 warmboot */
    if ((stable_size == 0) || (SOC_WARM_BOOT_SCACHE_IS_LIMITED(unit))) {
        return BCM_E_NONE;
    }

    SOC_SCACHE_HANDLE_SET(scache_handle, unit, BCM_MODULE_FLOW, 0);
    rv = _bcm_esw_scache_ptr_get(unit, scache_handle, FALSE,
                                 0, &flow_scache_ptr,
                                 BCM_WB_DEFAULT_VERSION, NULL);
    if (BCM_FAILURE(rv) && (rv != BCM_E_INTERNAL)) {
        return rv;
    }

    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);

    /* save init_tunnel entry table */
    sz = sizeof(uint16); /* idx of _bcm_flow_init_tunnel_entry_t */
    for (i = 0; i < num_soft_tnl; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->init_tunnel[i].idx, sz);
        flow_scache_ptr += sz;
    }

    /* save the dvp_attr_bitmap */
    sz = SHR_BITALLOCSIZE(num_dvp);
    sal_memcpy(flow_scache_ptr, flow_info->dvp_attr_bitmap, sz);
    flow_scache_ptr += sz;

    /* save iif_ref_cnt table */
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    sz = sizeof(uint16); /* iif_ref_cnt */ 
    for (i = 0; i < num_iif; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->iif_ref_cnt[i], sz);
        flow_scache_ptr += sz;
    }

    /* save vp_ref_cnt table */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    sz = sizeof(uint16); /* vp_ref_cnt */ 
    for (i = 0; i < num_vp; i++) {
        sal_memcpy(flow_scache_ptr,&flow_info->vp_ref_cnt[i], sz);
        flow_scache_ptr += sz;
    }

    return rv;
}

#endif /* BCM_WARM_BOOT_SUPPORT */

/*
 * Function:
 *      bcmi_esw_flow_init
 * Purpose:
 *      Initialize the FLOW software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */
int
bcmi_esw_flow_init(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;
    int num_dvp; 
    int rv = BCM_E_NONE;
    int num_vfi = 0;
    int num_tnl;
    int num_iif;
    int num_vp;
    uint64 rval;

    if (!L3_INFO(unit)->l3_initialized) {
        LOG_ERROR(BSL_LS_BCM_FLOW,
                  (BSL_META_U(unit,
                              "L3 module must be initialized prior to FLOW Init\n")));
        return BCM_E_CONFIG;
    }

    if (BCM_FAILURE(soc_flow_db_status_get(unit))) {
        return BCM_E_INIT;
    }
    /* Allocate BK Info */
    BCM_IF_ERROR_RETURN(bcm_td3_flow_allocate_bk(unit));
    flow_info = FLOW_INFO(unit);

    /*
     * allocate resources
     */
    if (flow_info->initialized) {
         BCM_IF_ERROR_RETURN(bcmi_esw_flow_cleanup(unit));
         BCM_IF_ERROR_RETURN(bcm_td3_flow_allocate_bk(unit));
         flow_info = FLOW_INFO(unit);
    }

    /* Create EGR_DVP_ATTRIBUTEm usage bitmap */
    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);

    flow_info->dvp_attr_bitmap =
        sal_alloc(SHR_BITALLOCSIZE(num_dvp), "dvp_attr_bitmap");
    if (flow_info->dvp_attr_bitmap == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->dvp_attr_bitmap, 0, SHR_BITALLOCSIZE(num_dvp));

    /* Create soft tunnel index table  */
    /* L2 tunnel associated with dvp, dvp represents the soft tunnnel index */
    num_tnl = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);

    /* L3 tunnel associated with egr_intf, soft tunnel index: 
     * max l2 tunnel + EGR_L3_INTFm index. 
     */
    num_tnl += soc_mem_index_count(unit, EGR_L3_INTFm);
    flow_info->init_tunnel =
        sal_alloc(sizeof(_bcm_flow_init_tunnel_entry_t) * num_tnl, 
                  "flow_init_tunnel");
    if (flow_info->init_tunnel == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->init_tunnel, 0, 
               sizeof(_bcm_flow_init_tunnel_entry_t) * num_tnl);

    /* Create iif_ref_cnt table */
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    flow_info->iif_ref_cnt =
        sal_alloc(sizeof(uint16) * num_iif,
                  "flow_match iif_ref_cnt");
    if (flow_info->iif_ref_cnt == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->iif_ref_cnt, 0,
               sizeof(uint16) * num_iif);

    /* Create vp_ref_cnt table */
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    flow_info->vp_ref_cnt =
        sal_alloc(sizeof(uint16) * num_vp,
                  "flow_match vp_ref_cnt");
    if (flow_info->vp_ref_cnt == NULL) {
        _bcm_td3_flow_free_resource(unit);
        return BCM_E_MEMORY;
    }
    sal_memset(flow_info->vp_ref_cnt, 0,
               sizeof(uint16) * num_vp);

    rv = READ_EGR_SEQUENCE_NUMBER_CTRLr(unit, &rval);
    if (SOC_FAILURE(rv)) {
         _bcm_td3_flow_free_resource(unit);
         return rv;
    }
    flow_info->frag_base_inx = soc_reg64_field32_get(unit, 
             EGR_SEQUENCE_NUMBER_CTRLr, rval, EGR_IP_TUNNEL_OFFSET_BASEf);

    /* Create FLOW protection mutex. */
    flow_info->flow_mutex = sal_mutex_create("flow_mutex");
    if (!flow_info->flow_mutex) {
         _bcm_td3_flow_free_resource(unit);
         return BCM_E_MEMORY;
    }

#if 0
    if (NULL == flow_info->flow_tunnel_term) {
        flow_info->flow_tunnel_term =
            sal_alloc(sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp, "flow tunnel term store");
        if (flow_info->flow_tunnel_term == NULL) {
            _bcm_td3_flow_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(flow_info->flow_tunnel_term, 0, 
                sizeof(_bcm_vxlan_tunnel_endpoint_t) * num_vp);
    }

#endif

    num_vfi = soc_mem_index_count(unit, VFIm);
    if (NULL == flow_info->flow_vpn_vlan) {
        flow_info->flow_vpn_vlan =
            sal_alloc(sizeof(bcm_vlan_t) * num_vfi, "flow vpn vlan store");
        if (flow_info->flow_vpn_vlan == NULL) {
            _bcm_td3_flow_free_resource(unit);
            return BCM_E_MEMORY;
        }
        sal_memset(flow_info->flow_vpn_vlan, 0, sizeof(bcm_vlan_t) * num_vfi);
    }

#ifdef BCM_WARM_BOOT_SUPPORT
    if (SOC_WARM_BOOT(unit)) {
        rv = _bcm_flow_reinit(unit);
        if (BCM_FAILURE(rv)) {
            _bcm_td3_flow_free_resource(unit);
        }
    } else {
        rv = _bcm_flow_wb_alloc(unit);
    }
#endif /* BCM_WARM_BOOT_SUPPORT */

    /* Mark the state as initialized */
    flow_info->initialized = TRUE;

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_check_init
 * Purpose:
 *      Check if FLOW is initialized
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_esw_flow_check_init(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    flow_info = FLOW_INFO(unit);

    if ((flow_info == NULL) || (flow_info->initialized == FALSE)) {
         return BCM_E_INIT;
    } else {
         return BCM_E_NONE;
    }
}

/*
 * Function:
 *      bcmi_esw_flow_cleanup
 * Purpose:
 *      DeInit  FLOW software module
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

int
bcmi_esw_flow_cleanup(int unit)
{
    _bcm_flow_bookkeeping_t *flow_info;
    int rv = BCM_E_UNAVAIL;

    if ((unit < 0) || (unit >= BCM_MAX_NUM_UNITS)) {
         return BCM_E_UNIT;
    }

    flow_info = FLOW_INFO(unit);

    if (FALSE == flow_info->initialized) {
        return (BCM_E_NONE);
    } 

    rv = bcmi_esw_flow_lock (unit);
    if (BCM_FAILURE(rv)) {
        return rv;
    }



    /* Mark the state as uninitialized */
    flow_info->initialized = FALSE;

    sal_mutex_give(flow_info->flow_mutex);

    /* Destroy protection mutex. */
    sal_mutex_destroy(flow_info->flow_mutex );

    /* Free software resources */
    (void) _bcm_td3_flow_free_resource(unit);

    return rv;
}

/*
 * Function:
 *      bcmi_esw_flow_lock
 * Purpose:
 *      Take FLOW Lock Sempahore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */

 int 
 bcmi_esw_flow_lock(int unit)
{
   int rv=BCM_E_NONE;

   rv = bcmi_esw_flow_check_init(unit);
   
   if ( rv == BCM_E_NONE ) {
           sal_mutex_take(FLOW_INFO((unit))->flow_mutex, sal_mutex_FOREVER);
   }
   return rv; 
}

/*
 * Function:
 *      bcmi_esw_flow_unlock
 * Purpose:
 *      Release  FLOW Lock Semaphore
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXX
 */


void
bcmi_esw_flow_unlock(int unit)
{
   int rv=BCM_E_NONE;

   rv = bcmi_esw_flow_check_init(unit);
    if ( rv == BCM_E_NONE ) {
         sal_mutex_give(FLOW_INFO((unit))->flow_mutex);
    }
}

#ifdef BCM_WARM_BOOT_SUPPORT_SW_DUMP
STATIC
void _bcmi_esw_flow_sw_dump(int unit)
{
    int num_dvp = 0;
    int num_iif = 0;
    int num_vp  = 0;
    int num_vfi = 0;
    int num_soft_tnl = 0;
    _bcm_flow_bookkeeping_t *flow_info = NULL;
    int i = 0;
    uint16 flag = FALSE;

    flow_info = FLOW_INFO(unit);

    num_dvp = soc_mem_index_count(unit, EGR_DVP_ATTRIBUTEm);
    num_soft_tnl = num_dvp + soc_mem_index_count(unit, EGR_L3_INTFm);
    num_vp = soc_mem_index_count(unit, SOURCE_VPm);
    num_iif = soc_mem_index_count(unit, L3_IIFm);
    num_vfi = soc_mem_index_count(unit, VFIm);

    for (i = 0, flag = FALSE; i < num_vfi; i++) {
        if (VIRTUAL_INFO(unit)->flow_vfi_bitmap) {
            if (SHR_BITGET(VIRTUAL_INFO(unit)->flow_vfi_bitmap, i)) {
                if (flag == TRUE) {
                    LOG_CLI((BSL_META_U(unit," , %d"), i));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                      "%-32s : %d \n"),"Flow VFI", i));
                }
                flag = TRUE;
            }
        }
    }

    for (i = 0, flag = FALSE; i < num_vp; i++) {
        if (VIRTUAL_INFO(unit)->flow_vp_bitmap) {
            if (SHR_BITGET(VIRTUAL_INFO(unit)->flow_vp_bitmap, i)) {
                if (flag == TRUE) {
                    LOG_CLI((BSL_META_U(unit," , %d"), i));
                } else {
                    LOG_CLI((BSL_META_U(unit,
                         "\n%-32s : %d"), "Flow VP", i));
                }
                flag = TRUE;
            }
        }
    }

    for (i = 0, flag = FALSE; i < num_soft_tnl; i++) {
        if(flow_info->init_tunnel[i].idx) {
            if (flag == TRUE) {
                LOG_CLI((BSL_META_U(unit," , %d"), i));
            } else {
                LOG_CLI((BSL_META_U(unit,
                    "\n%-32s : %d"),
                    "init_tunnel soft index",
                    num_soft_tnl));
            }
            flag = TRUE;
        }
    }

    LOG_CLI((BSL_META_U(unit,
             "\n%-32s : 0x%x"),
             "DVP Attribute bitmap",*flow_info->dvp_attr_bitmap));

    for (i = 0; i < num_iif;  i++) {
        if (flow_info->iif_ref_cnt[i]) {
            LOG_CLI((BSL_META_U(unit,
                 "\n %-10s : %d, %-14s : %d"),
                 "IIF index", i, "IIF use count", flow_info->iif_ref_cnt[i]));
        }
    }

    for (i = 0; i < num_vp; i++) {
        if (flow_info->vp_ref_cnt[i]) {
            LOG_CLI((BSL_META_U(unit,
                 "\n%-10s : %d,  %-14s : %d"),
                 "VP index", i, "VP use count", flow_info->vp_ref_cnt[i]));
        }
    }

    LOG_CLI((BSL_META_U(unit,"\n")));
    return;
}
/*
 * Function:
 *     bcmi_esw_flow_sw_dump
 * Purpose:
 *     Displays flow information maintained by software.
 * Parameters:
 *     unit - Device unit number
 * Returns:
 *     None
 */
void
bcmi_esw_flow_sw_dump(int unit)
{
    if (soc_feature(unit, soc_feature_flex_flow)) {
        _bcmi_esw_flow_sw_dump(unit);
    }
    return;
}
#endif /* BCM_WARM_BOOT_SUPPORT_SW_DUMP */

#endif
