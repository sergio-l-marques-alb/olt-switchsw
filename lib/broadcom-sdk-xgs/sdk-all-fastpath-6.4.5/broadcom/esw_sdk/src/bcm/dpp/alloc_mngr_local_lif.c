/*
 * $Id: alloc_mngr.c,v 1.312 Broadcom SDK $
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
 * File:        alloc_mngr_local_lif.c
 * Purpose:     Local Lif allocation manager for SOC_SAND chips.
 *
 */

#ifdef _ERR_MSG_MODULE_NAME
#error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_BCM_COMMON

#include <shared/bsl.h>

/*#include <soc/error.h>*/
#include <bcm_int/common/debug.h>
#include <soc/debug.h>
#include <bcm/error.h>
#include <bcm/types.h>
#include <bcm/module.h>

#include <bcm_int/dpp/error.h>
#include <bcm_int/dpp/alloc_mngr_shr.h>
#include <bcm_int/dpp/alloc_mngr_lif.h>
#include <bcm_int/dpp/alloc_mngr_local_lif.h>
#include <bcm_int/dpp/alloc_mngr_glif.h>
#include <bcm_int/dpp/alloc_mngr_utils.h>
#include <bcm_int/dpp/alloc_mngr.h>
#include <soc/dpp/dpp_config_defs.h>

#ifdef BCM_WARM_BOOT_SUPPORT
#include <bcm_int/dpp/wb_db_alloc.h>
#endif

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_frwrd_trill.h>


STATIC uint32
_bcm_dpp_am_local_inlif_internal_alloc(int unit, uint8 bank_id, int flags, int *local_lif);

STATIC uint32
_bcm_dpp_am_local_inlif_internal_dealloc(int unit, int flags, int local_lif);

STATIC uint32
_bcm_dpp_am_local_outlif_internal_dealloc(int unit, int local_lif);

STATIC uint32
_bcm_dpp_am_local_inlif_is_pack_empty(int unit,uint8 bank_id, uint8 flags, int elem, uint8 *is_pack_empty);

STATIC uint32
_bcm_dpp_am_local_inlif_app_type_to_bitmap_access_phase(int unit, bcm_dpp_am_ingress_lif_app_t app_type, uint32 *bitmap_phase_number);


STATIC uint32
_bcm_dpp_am_local_outlif_bank_alloc(int unit, int flags, bcm_dpp_am_egress_encap_alloc_info_t *app_alloc_info, int *bank_id , uint8 *bank_found  );

STATIC uint32 
_bcm_dpp_am_local_outlif_internal_alloc(int unit, uint8 bank_id,  bcm_dpp_am_egress_encap_alloc_info_t *app_alloc_info,  int flags, int *local_lif);

STATIC uint32
  _bcm_dpp_am_local_outlif_extension_lif_id_get(int unit, int base_lif,  int *ext_lif);


STATIC uint8 _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[SOC_PPD_NOF_EG_ENCAP_ACCESS_PHASE_TYPES] = {
                    /*SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE      */        BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_1,
                    /*SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO*/              BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2,
                    /*SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE*/            BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_3,
                    /*SOC_PPD_EG_ENCAP_ACCESS_PHASE_FOUR*/             BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_4
                                                                        }; 

STATIC uint8 _bcm_dpp_am_local_outlif_bank_phase_to_access_phase( int bank_phase)
{
    switch (bank_phase)
    {
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC:
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_1:
            return SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2:
            return SOC_PPD_EG_ENCAP_ACCESS_PHASE_TWO;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_3:
            return SOC_PPD_EG_ENCAP_ACCESS_PHASE_THREE;
        case BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_4:
            return SOC_PPD_EG_ENCAP_ACCESS_PHASE_FOUR;
        default:
            return SOC_PPD_EG_ENCAP_ACCESS_PHASE_ONE;
    }
}


/*
 * LOCAL LIF DEFINES AND MACROS
 */
/* Validate Ingress LIF */
#define _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(init) \
    do {                                                                \
        if (!((unit) >= 0 && ((unit) < (BCM_MAX_NUM_UNITS)))) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not valid\n" ), (unit))); \
        } \
        if (!SOC_IS_JERICHO(unit) || !SOC_DPP_PP_ENABLE(unit)) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_UNIT,(_BSL_BCM_MSG("unit %d is not supported device for ingress local lif alloc\n" ), (unit))); \
        } \
        BCMDNX_IF_ERR_EXIT(SOC_DPP_WB_ENGINE_GET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_INIT, &init )); \
        if (init == FALSE) { \
          BCMDNX_ERR_EXIT_MSG(BCM_E_INIT,(_BSL_BCM_MSG("unit %d is not initialize ingress local lif alloc\n" ), (unit))); \
        } \
    } while (0);

#define _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK (_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_ENTRIES_PER_BANK )
#define _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK  (_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK / 3 )

#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_ADJUSTMENT(object_off, lif_type) ( ( lif_type == BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ?  object_off * 3 :  object_off * 2 ) 
#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_LOCAL_LIF_ID(object_off, bank_id, lif_type) (_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK * bank_id +  _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_ADJUSTMENT(object_off, lif_type)) 

#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id) (lif_id /_BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK)
#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id) (lif_id % _BCM_DPP_AM_LOCAL_LIF_NOF_INLIF_HALF_ENTRIES_PER_BANK)
#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_BASE(lif_id, lif_type) ( ( lif_type == BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id) * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK ) : ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id) * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK ) )

#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(object_id, lif_type) ( ( lif_type == BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? (object_id % _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK ) : (object_id % _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK ) )
#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(lif_id, lif_type) ( ( lif_type == BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_BASE(lif_id, lif_type) + (_BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id) / 3) ) : ( _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_BASE(lif_id, lif_type) + (_BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id) / 2 ) ) ) 
#define _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_OBJECT_ID(object_off, bank_id, lif_type) ( ( lif_type == BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ?  ( bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK + object_off ) :  ( bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK + object_off ) )

#define _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_INTERNAL_ID(is_wide,base_lif_id, ext_lif_id) ( (is_wide) ? ((base_lif_id < ext_lif_id ) ? ( 2 * base_lif_id) :  ( 2 * base_lif_id - 1)) : ( 2 * base_lif_id) )

#define _BCM_DPP_AM_INGRESS_LIF_CONFLICT_FOUND(phase_bmp)  \
        ( ( ( SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_tt_lookup_res_0)) && (SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_tt_lookup_res_1)) ) || \
          ( ( SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_vt_lookup_res_0)) && (SHR_BITGET(&phase_bmp,_dpp_am_ingress_lif_phase_vt_lookup_res_1)) ) )

#define _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_TO_EXTENSION(app_type) ( (app_type == bcm_dpp_am_egress_encap_app_linker_layer ) ? TRUE : FALSE) 

#define _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY(pool_id,app_type) \
       ( ( pool_id == dpp_am_res_eg_out_rif ) || \
         ( app_type == bcm_dpp_am_egress_encap_app_out_ac)   || \
         ( pool_id ==  dpp_am_res_eg_data_trill_invalid_entry) )

#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_PHASE(bank_phase, lif_phase) (_bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[lif_phase] == bank_phase)
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_TYPE(bank_type, lif_type) (lif_type  == bank_type)
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXTENSION(bank_extension, lif_extension) (lif_extension == bank_extension)

#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK(eedb_bank_info, lif_phase, lif_type, lif_extension) \
                                                    ( (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_PHASE(eedb_bank_info.phase,lif_phase ) ) && \
                                                      (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_TYPE(eedb_bank_info.type,lif_type ) ) && \
                                                      (_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXTENSION(eedb_bank_info.is_extended,lif_extension ) ) )      

/*check that bank attributes are forced by user. if forced - check that bank attributes fits lif allocation. if not - return true*/
#define _BCM_DPP_AM_EGRESS_LOCAL_LIF_NEW_BANK_FORCE_VALIDATE(eedb_bank_info, lif_phase, lif_type, lif_extension) \
           (  ( (eedb_bank_info.force_flags &  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE ) && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_PHASE(eedb_bank_info.phase,lif_phase) )  ) || \
              ( (eedb_bank_info.force_flags &  BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE )  && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_TYPE(eedb_bank_info.type,lif_type) )  ) || \
              ( (eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_EXTENSION)  && !(_BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK_EXTENSION(eedb_bank_info.is_extended,lif_extension ) )  ) )


#define _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id, offset)      (bank_id * _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK  + offset)

#define _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_id)          ( lif_id /_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK)
#define _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_id)         (lif_id % _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK)

#define _BCM_DPP_AM_EGRESS_TOP_BANK_TO_EEDB_BANK(top_bank_id)                (_BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_REGULAR_BANKS * 2 + top_bank_id / 2 )
#define _BCM_DPP_AM_EGRESS_TOP_BANK_TO_ELEMENT_OFFSET_IN_BANK(top_bank_id)   (_BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK /2 * ( top_bank_id % 2 )  )


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_init
 * Purpose:
 *      Initalize ingress lif allocation 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_init(int unit)
{
    int rv;
    uint8 is_init = TRUE;
    /* Add global flag of initalization*/
    BCMDNX_INIT_FUNC_DEFS;

    /* Allocate the local inlif resources. */
    BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_INLIF_SW_DB));

    /*set the local inlif intialization flag to TRUE*/
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_INIT, &is_init );
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}



/*
 * Function:
 *     _bcm_dpp_am_local_inlif_alloc
 * Purpose:
 *      Allocate new ingress local LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     flags            - (IN) BCM_DPP_AM_IN_LIF_FLAG_* to indicate the lif type (common/wide).
 *     lif_info         - (IN) Lif info struct
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_alloc(int unit, int flags, bcm_dpp_am_local_inlif_info_t *lif_info)
{
    int rv , bank_id_iter;
    uint8 is_init;
    uint8 is_wide_lif = ( lif_info->local_lif_flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    int local_inlif_id ;
    uint32 bitmap_access_phase , tmp_phase_bmp;
    uint8 bank_found = FALSE;
    uint8 bank_id = 0;
    uint8 checked_bank_bmp = 0;
    bcm_dpp_am_local_inlif_application_type_info_t app_type_info;
    bcm_dpp_am_local_inlif_table_bank_info_t       bank_info;         

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(is_init);

    rv = _bcm_dpp_am_local_inlif_app_type_to_bitmap_access_phase(unit, lif_info->app_type, &bitmap_access_phase);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_APP_TYPE_INFO, &app_type_info, lif_info->app_type);
    BCMDNX_IF_ERR_EXIT(rv);

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS ; bank_id_iter++) 
    {
        /* This application is already allocated inLif in this bank*/
        if ( app_type_info.banks_bmp & ( 1 << bank_id_iter) )
        {
            rv = _bcm_dpp_am_local_inlif_internal_alloc(unit,bank_id_iter,lif_info->local_lif_flags,&local_inlif_id); /*bank_id_iter = bank to allocate LIF */
            if (rv == BCM_E_RESOURCE) 
            {
                checked_bank_bmp |= ( 1 << bank_id_iter);
                continue;
            }
            else if (rv == BCM_E_NONE) {
                bank_found = TRUE;
                bank_id = bank_id_iter;
                break;
            }
            else  
            {
                BCM_EXIT;
            }
        }
    }
 
    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_INGRESS_LIF_NOF_BANKS && !bank_found  ; bank_id_iter++) 
    {
        if ( checked_bank_bmp & ( 1 << bank_id_iter) )
        {
            continue; /*no need to check this bank - it is full */
        }

        /*get the bank info*/
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv);

        tmp_phase_bmp = bank_info.phase_bmp | bitmap_access_phase; 
        /*check for application conflicts according to bank phase */
        if (_BCM_DPP_AM_INGRESS_LIF_CONFLICT_FOUND( tmp_phase_bmp ))            
        {
            continue;
        }  
  
         /* If no conflict found - this bank can be used*/  
        rv = _bcm_dpp_am_local_inlif_internal_alloc(unit,bank_id_iter,is_wide_lif,&local_inlif_id);
        if (rv == BCM_E_RESOURCE) 
        {
            continue;
        }
        else if (rv == BCM_E_NONE) {
            bank_found = TRUE;
            bank_id = bank_id_iter;
            break;
        }
        else  
        {
            BCM_EXIT;
        }

    }
    if ( !bank_found ) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("No more free inlifs." )));
    }
    /* else: Allocation succeeded */
     lif_info->base_lif_id = BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_BASE_LIF_ID(local_inlif_id);
     lif_info->ext_lif_id  = ( is_wide_lif ) ? BCM_DPP_AM_LOCAL_INLIF_HANDLE_TO_ADDITIONAL_LIF_ID(local_inlif_id) : _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED ;
     lif_info->valid = TRUE;

     rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
     BCMDNX_IF_ERR_EXIT(rv);

     bank_info.phase_bmp |= bitmap_access_phase; /* Set application in bank applications bitmap*/

     /*set the inlif bank info into SW database*/
     rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
     BCMDNX_IF_ERR_EXIT(rv); 

     app_type_info.banks_bmp |= ( 1 << bank_id ); /* Set bank in bank applications bitmap*/

     /*set the app-type  info into SW database*/
     rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_APP_TYPE_INFO, &app_type_info, lif_info->app_type);
     BCMDNX_IF_ERR_EXIT(rv); 


     /*set the local inlif info into SW database*/
     rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_DB, lif_info, lif_info->base_lif_id);
     BCMDNX_IF_ERR_EXIT(rv);

    /*map local inlif to global inlif*/
    rv= _bcm_dpp_global_lif_mapping_create(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, lif_info->glif, lif_info->base_lif_id);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_dealloc
 * Purpose:
 *      De-Allocate new ingress local LIF
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     lif_index        - (IN) local inlif handle id
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_dealloc(int unit, int lif_index)
{
    int rv; 
    uint8 flags ; 
    uint8 is_init , is_wide_lif;
    bcm_dpp_am_local_inlif_info_t inlif_info;
    int internal_inlif_id;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(is_init);

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_DB, &inlif_info, lif_index);
    BCMDNX_IF_ERR_EXIT(rv);
    flags =  inlif_info.local_lif_flags ; 

    is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE;

    /* covert base lif to handle id*/
    internal_inlif_id =  _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_INTERNAL_ID(is_wide_lif,inlif_info.base_lif_id,inlif_info.ext_lif_id);

    rv = _bcm_dpp_am_local_inlif_internal_dealloc(unit,flags,internal_inlif_id); 
    BCMDNX_IF_ERR_EXIT(rv);

    /*destroy mapping of local inlif to global inlif*/
    rv= _bcm_dpp_global_lif_mapping_remove(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL | _BCM_DPP_GLOBAL_LIF_MAPPING_INGRESS, 0,  lif_index);
    BCMDNX_IF_ERR_EXIT(rv);

    BCM_EXIT;

exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_internal_alloc
 * Purpose:
 *     Allocate either a wide/ common lif according to lif type. calculate the LIF id, and update the corresponding pool id.
 *     See the algorithm above.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     bank_id      - (IN) Ingress LIF bank id (0-3).
 *     flags        - (IN) BCM_DPP_AM_IN_LIF_FLAG_WIDE, BCM_DPP_AM_IN_LIF_FLAG_COMMON or BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID.
 *     local_lif   - (INOUT) Will be filled with the local lif's id. If WITH_ID alloc flag is provided, Id must be provided.
 * Returns: 
 *     BCM_E_NONE       - If allocation is successful. 
 *     BCM_E_PARAM      - If WITH_ID flag is set and given global lif is illegal.
 *     BCM_E_RESOURCE   - If lif banks are full.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_local_inlif_internal_alloc(int unit, uint8 bank_id, int flags, int *local_lif){
    uint32 rv;

    int elem, alloc_elem_id ,elem_in_bank, elem_to_alloc ;
    uint8 is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    bcm_dpp_am_local_inlif_table_bank_info_t       bank_info;
    uint32 alloc_flags;
    BCMDNX_INIT_FUNC_DEFS;

    BCMDNX_NULL_CHECK(local_lif);

    rv = bcm_dpp_am_local_inlif_alloc(unit,bank_id,flags,&elem);
    BCMDNX_IF_ERR_EXIT(rv);

    /* sucees in allocation*/

    /*compute new LIF id*/
    elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(elem, is_wide_lif);
    *local_lif = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK_TO_LOCAL_LIF_ID(elem_in_bank, bank_id, is_wide_lif);

    /*update corresponding LIF pool*/
    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
    BCMDNX_IF_ERR_EXIT(rv);

    if (is_wide_lif)
    {
        /*check if this is first entry allocated in 6-pack*/
        if (!bank_info.packs_info[elem_in_bank/2].is_used) /* wide lif : 2 wide lifs per 6-pack */ 
        {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK + (elem_in_bank /2  * 3)  ;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id <= elem_to_alloc + 2 ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_alloc(unit,bank_id,alloc_flags,&alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info.packs_info[elem_in_bank/2].is_used  = TRUE;  
            bank_info.packs_info[elem_in_bank/2].lif_type =  BCM_DPP_AM_IN_LIF_FLAG_WIDE;
            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
            BCMDNX_IF_ERR_EXIT(rv);
        }
    } else {
        /*check if this is first entry allocated in 6-pack*/
        if (!bank_info.packs_info[elem_in_bank/3].is_used) /* common lif : 3 common lifs per 6-pack */ 
        {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK + (elem_in_bank /3  * 2 )  ;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id <= elem_to_alloc + 1 ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_alloc(unit,bank_id,alloc_flags,&alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info.packs_info[elem_in_bank/3].is_used  = TRUE;   
            bank_info.packs_info[elem_in_bank/3].lif_type =  BCM_DPP_AM_IN_LIF_FLAG_COMMON;
            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

    }

exit:
    BCMDNX_FUNC_RETURN;

}

/*checks weather a six-pack is empty or not*/
STATIC uint32
_bcm_dpp_am_local_inlif_is_pack_empty(int unit,uint8 bank_id, uint8 flags, int elem, uint8 *is_pack_empty)
{

    uint32 rv1, rv2;
    int elem_to_check, elem_in_bank;
    uint8 is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    BCMDNX_INIT_FUNC_DEFS;

    elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(elem, is_wide_lif);

    if ( is_wide_lif )
    {
        elem_to_check = (elem_in_bank % 2 == 0) ? (elem + 1) : (elem -1) ;
        rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem_to_check);
        if (rv1 != BCM_E_EXISTS) {
            if (rv1 == BCM_E_NOT_FOUND  )
            {
                *is_pack_empty = TRUE;
            }
            else
            {
                BCMDNX_IF_ERR_EXIT(rv1);
            }
        }
    }
    else 
    {
        switch (elem_in_bank % 3) 
        {
        case 0 : 
           rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+1);
           rv2 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+2);

           if ( rv1 != BCM_E_EXISTS &&  rv2 != BCM_E_EXISTS )
           {
                *is_pack_empty = TRUE;
           }

           break;
        case 1:
           rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-1);
           rv2 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem+1);

           if ( rv1 != BCM_E_EXISTS &&  rv2 != BCM_E_EXISTS )
           {
                *is_pack_empty = TRUE;
           }

           break;
         case 2:
           rv1 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-1);
           rv2 = bcm_dpp_am_local_inlif_is_alloc(unit, bank_id, flags, elem-2);

           if ( rv1 != BCM_E_EXISTS &&  rv2 != BCM_E_EXISTS )
           {
                *is_pack_empty = TRUE;
           }

           break;
        }
    }
exit:
    BCMDNX_FUNC_RETURN;

}


/*
 * Function:
 *     _bcm_dpp_am_local_inlif_internal_dealloc
 * Purpose:
 *     Allocate either a wide/ common lif according to lif type. calculate the LIF id, and update the corresponding pool id.
 *     See the algorithm above.
 *  
 * Parameters:
 *     unit         - (IN) Device number
 *     bank_id      - (IN) Ingress LIF bank id (0-3).
 *     flags        - (IN) BCM_DPP_AM_IN_LIF_FLAG_WIDE, BCM_DPP_AM_IN_LIF_FLAG_COMMON or BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID.
 *     local_lif   - (INOUT) Will be filled with the local lif's id. If WITH_ID alloc flag is provided, Id must be provided.
 * Returns: 
 *     BCM_E_NONE       - If allocation is successful. 
 *     BCM_E_PARAM      - If WITH_ID flag is set and given global lif is illegal.
 *     BCM_E_RESOURCE   - If lif banks are full.
 *     BCM_E_*          - Otherwise.
 */
STATIC uint32
_bcm_dpp_am_local_inlif_internal_dealloc(int unit, int flags, int local_lif){

    uint32 rv;
    uint8 bank_id;
    int elem, alloc_elem_id, elem_in_bank, elem_to_alloc ;

    bcm_dpp_am_local_inlif_table_bank_info_t       bank_info;
    uint32 alloc_flags;
    uint8 is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE ; 
    uint8 is_six_pack_empty = FALSE ;

    BCMDNX_INIT_FUNC_DEFS;


    elem = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(local_lif,flags);
    bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(local_lif);

    rv = bcm_dpp_am_local_inlif_dealloc(unit,bank_id,flags,elem);
    BCMDNX_IF_ERR_EXIT(rv);

    elem_in_bank = _BCM_DPP_AM_INGRESS_LIF_OBJECT_OFFSET_IN_BANK(elem, is_wide_lif);

    /*check whether there are another entries in that 6-pack: if not update 6-pack as empty*/
    rv = _bcm_dpp_am_local_inlif_is_pack_empty(unit,bank_id,flags,elem,&is_six_pack_empty);
    BCMDNX_IF_ERR_EXIT(rv);

    /*update corresponding LIF pool*/
    if (is_six_pack_empty) 
    {
        /*update corresponding LIF pool*/
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
        BCMDNX_IF_ERR_EXIT(rv);
        if (is_wide_lif)
        {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_COMMON | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
             elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_COMMON_LIFS_IN_BANK + (elem_in_bank /2  * 3)  ;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id <= elem_to_alloc + 2 ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_dealloc(unit,bank_id,alloc_flags,alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info.packs_info[elem_in_bank/2].is_used  = FALSE;  
            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
            BCMDNX_IF_ERR_EXIT(rv);

        } else {
            alloc_flags = BCM_DPP_AM_IN_LIF_FLAG_WIDE | BCM_DPP_AM_IN_LIF_FLAG_ALLOC_WITH_ID;
            elem_to_alloc = bank_id * _BCM_DPP_AM_INGRESS_LIF_NOF_WIDE_LIFS_IN_BANK + (elem_in_bank /3  * 2 )  ;
            for ( alloc_elem_id = elem_to_alloc ; alloc_elem_id <= elem_to_alloc + 1 ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_inlif_dealloc(unit,bank_id,alloc_flags,alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
            bank_info.packs_info[elem_in_bank/3].is_used  = FALSE;  
            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_BANK_INFO, &bank_info, bank_id);
            BCMDNX_IF_ERR_EXIT(rv); 
        }
    }

exit:
    BCMDNX_FUNC_RETURN;

}

/*
 * Function:
 *     _bcm_dpp_am_local_inlif_is_alloc
 * Purpose:
 *      Checks whether ingress local LIF is allocated 
 *  
 * Parameters:
 *     unit             - (IN) Device number
 *     lif_index        - (IN) local inlif handle id
 * Returns: 
 *     BCM_E_NONE       - If the initiation was succesful.
 *     BCM_E_*          - Otherwise.
 */
uint32
_bcm_dpp_am_local_inlif_is_alloc(int unit, int lif_index)
{
    int rv; 
    uint8 bank_id;
    uint8 is_init, is_wide_lif;
    int elem;
    uint32 flags ; 
    int internal_inlif_id;
    bcm_dpp_am_local_inlif_info_t inlif_info;

    BCMDNX_INIT_FUNC_DEFS;

    _BCM_DPP_AM_INGRESS_LOCAL_LIF_VALID_CHECK(is_init);

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_DB, &inlif_info, lif_index);
    BCMDNX_IF_ERR_EXIT(rv);
    flags =  inlif_info.local_lif_flags ; 

    is_wide_lif = ( flags & BCM_DPP_AM_IN_LIF_FLAG_WIDE ) ? TRUE : FALSE;

    /* covert base lif to handle id*/
    internal_inlif_id =  _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_INTERNAL_ID(is_wide_lif,inlif_info.base_lif_id,inlif_info.ext_lif_id);
    
    elem = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_OBJECT_ID(internal_inlif_id,flags);
    bank_id = _BCM_DPP_AM_INGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(internal_inlif_id);

    rv = bcm_dpp_am_local_inlif_is_alloc(unit,bank_id,flags,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}

/* Convert application type to access phase number */
STATIC uint32
_bcm_dpp_am_local_inlif_app_type_to_bitmap_access_phase(int unit, bcm_dpp_am_ingress_lif_app_t app_type, uint32 *bitmap_phase_number)
{    
    BCMDNX_INIT_FUNC_DEFS;

    *bitmap_phase_number = 0;

     switch (app_type) {
        case bcm_dpp_am_ingress_lif_app_mpls_term:
        /* In case of Unindexed supported up to two terminations */
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
        _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break;
      /* MPLS Indexed Support */
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1:    
            if (SOC_DPP_CONFIG(unit)->pp.tunnel_termination_in_tt_only) {
              _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_tt_lookup_res_0);  /* TT only lookups, tt lookup res 0 is MPLS_1 */         
            } else{
              _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number, _dpp_am_ingress_lif_phase_vt_lookup_res_1);          
            }
            break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_2:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);          
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_3:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
          break;        
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_2:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_indexed_1_3:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
          break;
        case bcm_dpp_am_ingress_lif_app_ingress_ac:
        case bcm_dpp_am_ingress_lif_app_vpn_ac:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_0);
          /* In case trill is enabled, make sure the ingress nick lookup does conflict with VD-vlan-vlan lookup */
          if (SOC_DPP_CONFIG(unit)->trill.mode) {
              _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);
          }
          break;
        case bcm_dpp_am_ingress_lif_app_ingress_isid:
        case bcm_dpp_am_ingress_lif_app_ip_term:
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);
          break;
        /* FRR */
        case bcm_dpp_am_ingress_lif_app_mpls_frr_term:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);          
        break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_explicit_null:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_vt_lookup_res_1);          
        break;
        case bcm_dpp_am_ingress_lif_app_trill_nick:
          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_1);
          break;
        case bcm_dpp_am_ingress_lif_app_mpls_term_mldp:          
          _BCM_DPP_AM_INGRESS_LIF_BITMAP_PHASE_SET(bitmap_phase_number,_dpp_am_ingress_lif_phase_tt_lookup_res_0);          
        break;
        default:
          BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM, (_BSL_BCM_MSG("Not supported app type %d"), app_type));
        }
    BCM_EXIT;
exit:
    BCMDNX_FUNC_RETURN;
}

/* Egress - Local Outlif allocation*/

/* Initalize egress lif allocation */
uint32
_bcm_dpp_am_local_outlif_init(int unit)
{
    int rv , bank_id_iter, rif_max_id, rif_id_iter;
    uint8 is_init = TRUE;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;
    arad_pp_eg_encap_eedb_top_bank_info_t top_bank_info;
    bcm_dpp_am_local_out_lif_info_t rif_info;

    /* Add global flag of initalization*/
    BCMDNX_INIT_FUNC_DEFS;

    /* Allocate the local outlif resources. */
    BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_JER_LOCAL_OUTLIF_SW_DB));



    /* Allocate the local EEDB resources. */
    BCMDNX_IF_ERR_EXIT(soc_dpp_wb_engine_init_buffer(unit, SOC_DPP_WB_ENGINE_BUFFER_JER_EEDB_BANKS_SW_DB));

    rif_max_id = SOC_DPP_CONFIG(unit)->l3.nof_rifs;

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS ; bank_id_iter++) 
    {
        eedb_bank_info.valid = FALSE ; 
        eedb_bank_info.is_extended = FALSE;
        eedb_bank_info.force_flags = 0;

        eedb_bank_info.phase =         ( bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_PHASE(rif_max_id) ) ? BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_2 : BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC ;
        eedb_bank_info.force_flags |=  ( bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_PHASE(rif_max_id) ) ? BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE : 0;

        if ( ( bank_id_iter % 2 ) ==0 ) {
            rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id_iter/2,eedb_bank_info.phase);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        eedb_bank_info.type =   ( bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id) ) ? BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT : BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED;
        eedb_bank_info.force_flags |=  ( bank_id_iter < ARAD_PP_EG_ENCAP_NOF_RIF_BANKS_TYPE(rif_max_id) ) ? BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE : 0;

        eedb_bank_info.nof_free_entries = _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK;
        
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS ; bank_id_iter++) 
    {
 
        top_bank_info.extension_bank_owner = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED ; 

        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO, &top_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

    /* 
     * In case Routing is enabled the rif banks must be reserved.
     * mapping out-rif to VSI and remark-profile.
     * Also Bank 0 is used in order to use entry 0 as a NULL entry.
     * In that case Bank 0 must be not allocated for Out-AC.
     */
    sal_memset(&rif_info,0,sizeof(bcm_dpp_am_local_out_lif_info_t));
    rif_info.app_alloc_info.pool_id = dpp_am_res_eg_out_rif;
    rif_info.local_lif_flags = BCM_DPP_AM_OUT_LIF_FLAG_DIRECT;

    for (rif_id_iter = 0 ; rif_id_iter < rif_max_id ; rif_id_iter++) {
        rif_info.base_lif_id = rif_id_iter;
        rif_info.glif = rif_id_iter;

        rv = _bcm_dpp_am_local_outlif_alloc(unit, BCM_DPP_AM_FLAG_ALLOC_WITH_ID, &rif_info);
        BCMDNX_IF_ERR_EXIT(rv);
    }

    /*set the local outlif intialization flag to TRUE*/
    rv = SOC_DPP_WB_ENGINE_SET_VAR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_INIT, &is_init );
    BCMDNX_IF_ERR_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_local_outlif_alloc(int unit, int flags, bcm_dpp_am_local_out_lif_info_t *lif_info)
{

    int rv ,second_half_id;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info, eedb_bank_info2;
    SOC_PPD_EG_ENCAP_ACCESS_PHASE phase_access;   
    uint8 ext_bank_found = FALSE;
    int ext_bank_id;
    uint8 type;
    int local_outlif_id = 0;
    int bank_id = 0;
    uint8 bank_found = FALSE, lif_extension = FALSE;
    BCMDNX_INIT_FUNC_DEFS;

    lif_extension = ( _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_TO_EXTENSION(lif_info->app_alloc_info.application_type) && (lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_WIDE ) );

    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
        bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_info->base_lif_id);

        type = ( lif_info->local_lif_flags & BCM_DPP_AM_OUT_LIF_FLAG_DIRECT ) ? BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT : BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ;

        rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, &(lif_info->app_alloc_info), &phase_access);
        BCMDNX_IF_ERR_EXIT(rv);

        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
        BCMDNX_IF_ERR_EXIT(rv); 

        if (!eedb_bank_info.valid) /*bank is not allocated yet - allocate if possible*/
        {
            /*force validation - if bank attribures are forced and lif allocated attributes doesn't fit bank attributes, this bank cannot be used*/
            if (_BCM_DPP_AM_EGRESS_LOCAL_LIF_NEW_BANK_FORCE_VALIDATE(eedb_bank_info, phase_access, type, lif_extension))
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Outlif entry doesn't match bank type" )));
            }

            second_half_id = ( bank_id % 2) ? bank_id - 1 : bank_id + 1 ; 
            /*set also the phase of other half bank*/
            rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info2, second_half_id);
            BCMDNX_IF_ERR_EXIT(rv); 

            if  (  ( (_bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access] != eedb_bank_info.phase) && (eedb_bank_info.phase!= BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC) ) || 
                   ( eedb_bank_info2.valid && (lif_extension != eedb_bank_info.is_extended) ) ) 
            {
                BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Outlif entry doesn't match bank type" )));
            }

            if (lif_extension && !eedb_bank_info.is_extended) /*Need to allocate new extension bank*/
            {
                    rv = _bcm_dpp_am_local_outlif_extension_bank_alloc(unit,(bank_id/2), &ext_bank_id, &ext_bank_found );
                    BCMDNX_IF_ERR_EXIT(rv); 

                    if (ext_bank_found) 
                    {
                        eedb_bank_info.extension_bank = ext_bank_id;
                        eedb_bank_info.is_extended = lif_extension;
                        eedb_bank_info2.extension_bank = ext_bank_id;
                        eedb_bank_info2.is_extended = lif_extension;

                        /*set extension in HW*/
                        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,(bank_id/2), TRUE ,(uint32)ext_bank_id ));
                        BCMDNX_IF_ERR_EXIT(rv); 
                    }
            }

            eedb_bank_info.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];
            eedb_bank_info2.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];

            /* Set HW */
            rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id/2,eedb_bank_info.phase);
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info2, second_half_id);
            BCMDNX_IF_ERR_EXIT(rv); 

            eedb_bank_info.type = type;
            eedb_bank_info.valid = TRUE;

            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id,( type == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ) ? TRUE : FALSE));
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
            BCMDNX_IF_ERR_EXIT(rv); 

            bank_found = TRUE;
        }
        if ( _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK(eedb_bank_info, phase_access, type, lif_extension) && eedb_bank_info.nof_free_entries > 0 ) 
        {
            bank_found = TRUE;/*This bank can be used.*/
        }
        else
        {
             BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Requested EEDB bank is full." )));
        }

        local_outlif_id = lif_info->base_lif_id;
    }
    else
    {
        rv = _bcm_dpp_am_local_outlif_bank_alloc(unit, lif_info->local_lif_flags, &(lif_info->app_alloc_info), &bank_id, &bank_found);
        BCMDNX_IF_ERR_EXIT(rv); 
    }

    if ( !bank_found ) 
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("No free eedb entry." )));
    }
    /* else: Allocation succeeded */
    {
        /*allocate a lif in this bank*/
        rv = _bcm_dpp_am_local_outlif_internal_alloc(unit,bank_id,&(lif_info->app_alloc_info), flags,&local_outlif_id);
        BCMDNX_IF_ERR_EXIT(rv); 

        lif_info->base_lif_id = local_outlif_id;

        if (lif_extension)
        {
            rv = _bcm_dpp_am_local_outlif_extension_lif_id_get(unit,local_outlif_id,&(lif_info->ext_lif_id));
            BCMDNX_IF_ERR_EXIT(rv); 
        }

        /*set the local outlif info into SW database*/
        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_DB, lif_info, local_outlif_id);
        BCMDNX_IF_ERR_EXIT(rv);

        /*map local inlif to global inlif*/
        rv= _bcm_dpp_global_lif_mapping_create(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, lif_info->glif, local_outlif_id);
        BCMDNX_IF_ERR_EXIT(rv);
    }


exit:
    BCMDNX_FUNC_RETURN;

}

uint32
_bcm_dpp_am_local_outlif_dealloc(int unit,  int lif_index)
{
    int rv; 

    BCMDNX_INIT_FUNC_DEFS;
 
    rv = _bcm_dpp_am_local_outlif_internal_dealloc(unit, lif_index); 
    BCMDNX_IF_ERR_EXIT(rv);

    /*destroy mapping of local outlif to global inlif*/
    rv= _bcm_dpp_global_lif_mapping_remove(unit,  _BCM_DPP_GLOBAL_LIF_MAPPING_LOCAL_TO_GLOBAL | _BCM_DPP_GLOBAL_LIF_MAPPING_EGRESS, 0, lif_index);
    BCMDNX_IF_ERR_EXIT(rv) ;
exit:
    BCMDNX_FUNC_RETURN;
}

STATIC uint32
_bcm_dpp_am_local_outlif_internal_dealloc(int unit, int local_lif)
{
    int rv ;
    int bank_id,  elem , flags, count , second_half_id;
    bcm_dpp_am_local_out_lif_info_t outlif_info;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info, eedb_bank_info2;
    BCMDNX_INIT_FUNC_DEFS;

    elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(local_lif);
    bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(local_lif);

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_DB, &outlif_info , local_lif );
    BCMDNX_IF_ERR_EXIT(rv);
    flags =  outlif_info.local_lif_flags ; 

    /*number of entries to deallocate*/
    count = ( _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY(outlif_info.app_alloc_info.pool_id, outlif_info.app_alloc_info.application_type) ) ? 1 : 2;

    rv = bcm_dpp_am_local_outlif_dealloc(unit,bank_id,flags,count, elem);
    BCMDNX_IF_ERR_EXIT(rv);

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
    BCMDNX_IF_ERR_EXIT(rv); 

    eedb_bank_info.nof_free_entries += count;

    if ( eedb_bank_info.nof_free_entries == _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_HALF_BANK ) 
    {
        eedb_bank_info.valid = FALSE;

        if ( !(eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE) )
        {
            eedb_bank_info.phase = BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC;
        }
        if ( !(eedb_bank_info.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_TYPE) )
        {
            eedb_bank_info.type = BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED;
            rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id,TRUE));
            BCMDNX_IF_ERR_EXIT(rv); 
        }
        second_half_id = ( bank_id % 2) ? bank_id - 1 : bank_id + 1 ; 
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info2, second_half_id);
        BCMDNX_IF_ERR_EXIT(rv); 

        /*If the second half of the EEDB bank is also empty - clear phase, extension, and dealloc extension bank*/
        if (!eedb_bank_info2.valid)
        {
            if ( !(eedb_bank_info2.force_flags & BCM_DPP_AM_LOCAL_OUT_LIF_BANK_FLAG_FORCE_PHASE) )
            {
                eedb_bank_info2.phase = BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC;
            }

            if (eedb_bank_info.is_extended)
            {
                eedb_bank_info.is_extended = FALSE;
                eedb_bank_info2.is_extended = FALSE;
                eedb_bank_info.extension_bank =  _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;
                eedb_bank_info2.extension_bank = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;

                rv = _bcm_dpp_am_local_outlif_extension_bank_dealloc(unit,bank_id/2 );
                BCMDNX_IF_ERR_EXIT(rv); 

                /*set extension in HW*/
                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,(bank_id/2), FALSE ,0 ));
                BCMDNX_IF_ERR_EXIT(rv); 
            }

            /* Set HW */
            rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id/2,BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC);
            BCMDNX_IF_ERR_EXIT(rv); 

            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info2, second_half_id);
            BCMDNX_IF_ERR_EXIT(rv); 
        }

    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
    BCMDNX_IF_ERR_EXIT(rv); 

   
exit:
    BCMDNX_FUNC_RETURN;

}


STATIC uint32 
_bcm_dpp_am_local_outlif_internal_alloc(int unit, uint8 bank_id, bcm_dpp_am_egress_encap_alloc_info_t *app_alloc_info, int flags, int *local_lif)
{
    int rv, elem;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
    BCMDNX_IF_ERR_EXIT(rv); 

    if (flags & BCM_DPP_AM_FLAG_ALLOC_WITH_ID) {
        elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(*local_lif);
    }

    if (_BCM_DPP_AM_EGRESS_LIF_APP_TYPE_HALF_ENTRY(app_alloc_info->pool_id, app_alloc_info->application_type)) 
    {
        rv = bcm_dpp_am_local_outlif_alloc(unit,bank_id,flags,&elem);
        BCMDNX_IF_ERR_EXIT(rv);
        eedb_bank_info.nof_free_entries--;
    }
    else
    {
        rv = bcm_dpp_am_local_outlif_alloc_align(unit, bank_id,flags,2,0,2,&elem);
        BCMDNX_IF_ERR_EXIT(rv);

        eedb_bank_info.nof_free_entries -= 2;
    }

    rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id);
    BCMDNX_IF_ERR_EXIT(rv); 

    *local_lif = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(bank_id,elem);

exit:
    BCMDNX_FUNC_RETURN;
}



STATIC uint32
_bcm_dpp_am_local_outlif_bank_alloc(int unit, int flags, bcm_dpp_am_egress_encap_alloc_info_t *app_alloc_info, int *bank_id , uint8 *bank_found  )
{
    int rv , bank_id_iter, second_half_id;
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info, eedb_bank_info2;
    SOC_PPD_EG_ENCAP_ACCESS_PHASE phase_access;   
    uint32 checked_bank_bmp = 0;
    uint8 lif_extension;
    uint8 ext_bank_found = FALSE;
    int ext_bank_id;

    uint8 type = ( flags & BCM_DPP_AM_OUT_LIF_FLAG_DIRECT ) ? BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_DIRECT : BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ;

    BCMDNX_INIT_FUNC_DEFS;

    rv = _bcm_dpp_am_egress_encap_alloc_mngr_pool_id_to_access_phase(unit, app_alloc_info, &phase_access);
    BCMDNX_IF_ERR_EXIT(rv);

    lif_extension = ( _BCM_DPP_AM_EGRESS_LIF_APP_TYPE_TO_EXTENSION(app_alloc_info->application_type) && (flags & BCM_DPP_AM_OUT_LIF_FLAG_WIDE ) );

    for (bank_id_iter = 0 ; bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS ; bank_id_iter++) 
    {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv); 

        if (!eedb_bank_info.valid) /*bank is not allocated yet - continue*/
        {
            continue;
        }
        if ( _BCM_DPP_AM_EGRESS_LOCAL_LIF_CHECK_EEDB_BANK(eedb_bank_info, phase_access, type, lif_extension) && eedb_bank_info.nof_free_entries > 0 ) 
        {
            *bank_found = TRUE;
            *bank_id = bank_id_iter;
            break;
        } else {
            checked_bank_bmp |= ( 1 << bank_id_iter);
            continue;
        }
    }
    /*try to allocate new bank*/
    for (bank_id_iter = 0 ; ( bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_EEDB_HALF_BANKS ) && (*bank_found == FALSE )  ; bank_id_iter++) 
    {
        if ( checked_bank_bmp & ( 1 << bank_id_iter) )
        {
            continue; /*no need to check this bank  - it can't be used*/
        }

        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv); 

        /*force validation - if bank attribures are forced and lif allocated attributes doesn't fit bank attributes, this bank cannot be used*/
        if (_BCM_DPP_AM_EGRESS_LOCAL_LIF_NEW_BANK_FORCE_VALIDATE(eedb_bank_info, phase_access, type, lif_extension))
        {
            continue;
        }

        second_half_id = ( bank_id_iter % 2) ? bank_id_iter - 1 : bank_id_iter + 1 ; 
        /*set also the phase of other half bank*/
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info2, second_half_id);
        BCMDNX_IF_ERR_EXIT(rv); 

        if  (  ( (_bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access] != eedb_bank_info.phase) && (eedb_bank_info.phase!= BCM_SWITCH_LIF_PROPERTY_EEDB_PHASE_DYNAMIC) ) || 
               ( eedb_bank_info2.valid && (lif_extension != eedb_bank_info.is_extended) ) ) 
        {
            continue;
        }

        if (lif_extension && !eedb_bank_info.is_extended) /*Need to allocate new extension bank*/
        {
                rv = _bcm_dpp_am_local_outlif_extension_bank_alloc(unit,(bank_id_iter/2), &ext_bank_id, &ext_bank_found );
                BCMDNX_IF_ERR_EXIT(rv); 

                if (ext_bank_found) 
                {
                    eedb_bank_info.extension_bank = ext_bank_id;
                    eedb_bank_info.is_extended = lif_extension;
                    eedb_bank_info2.extension_bank = ext_bank_id;
                    eedb_bank_info2.is_extended = lif_extension;
                }

                /*set extension in HW*/
                rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_extension_mapping_set,(unit,(bank_id_iter/2), TRUE ,(uint32)ext_bank_id ));
                BCMDNX_IF_ERR_EXIT(rv); 

        }

        eedb_bank_info.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];
        eedb_bank_info2.phase = _bcm_dpp_am_local_outlif_app_phase_access_to_bank_phase[phase_access];

        /* Set HW */
        rv = _bcm_dpp_am_local_outlif_bank_phase_set(unit,bank_id_iter/2,eedb_bank_info.phase);
        BCMDNX_IF_ERR_EXIT(rv); 

        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info2, second_half_id);
        BCMDNX_IF_ERR_EXIT(rv); 

        eedb_bank_info.type = type;
        eedb_bank_info.valid = TRUE;

        rv = MBCM_PP_DRIVER_CALL(unit,mbcm_pp_eg_encap_direct_bank_set,(unit,bank_id_iter,( type == BCM_SWITCH_LIF_PROPERTY_ID_RANGE_TYPE_MAPPED ) ? TRUE : FALSE));
        BCMDNX_IF_ERR_EXIT(rv); 

        rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv); 

        *bank_found = TRUE;
        *bank_id = bank_id_iter;
    }

exit:
    BCMDNX_FUNC_RETURN;
}

uint32
_bcm_dpp_am_local_outlif_is_alloc(int unit, int lif_index)
{
    int rv; 
    uint8 bank_id;
    int elem;

    BCMDNX_INIT_FUNC_DEFS;

    elem = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(lif_index);
    bank_id = _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(lif_index);

    rv = bcm_dpp_am_local_outlif_is_alloc(unit,bank_id,0,elem);
    BCM_RETURN_VAL_EXIT(rv);

exit:
    BCMDNX_FUNC_RETURN;

}


uint32
_bcm_dpp_am_local_outlif_extension_bank_alloc(int unit, int bank_id, int *ext_bank_id, uint8 *bank_found)
{
    int rv; 
    uint8 bank_id_iter;
    uint8 eedb_half_bank;
    int alloc_elem_id, start_elem;

    arad_pp_eg_encap_eedb_top_bank_info_t top_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    /*try to allocate new bank*/
    for (bank_id_iter = 0 ; ( bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS ) && (*bank_found == FALSE )  ; bank_id_iter++) 
    {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO, &top_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv);

        if (top_bank_info.extension_bank_owner == _BCM_DPP_AM_EEDB_BANK_UNASSIGNED)   /*This top bank can be used as an extension*/
        {
            *bank_found = TRUE;
            *ext_bank_id = bank_id_iter;
            top_bank_info.extension_bank_owner = bank_id;

            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO, &top_bank_info, bank_id_iter);
            BCMDNX_IF_ERR_EXIT(rv);

            eedb_half_bank = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_EEDB_BANK(bank_id_iter);
            start_elem = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_ELEMENT_OFFSET_IN_BANK(bank_id_iter);

            /* allocate the 1/4 bank in the regular bank pull-ids*/
            for ( alloc_elem_id = start_elem ; alloc_elem_id < start_elem + _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK /4  ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_outlif_alloc(unit,eedb_half_bank,BCM_DPP_AM_FLAG_ALLOC_WITH_ID,&alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }
    }

    if (*bank_found == FALSE)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_RESOURCE,(_BSL_BCM_MSG("Out of extenstion banks." )));
    }

exit:
    BCMDNX_FUNC_RETURN;
}


uint32
_bcm_dpp_am_local_outlif_extension_bank_dealloc(int unit, int bank_id)
{
    int rv; 
    uint8 bank_id_iter;
    uint8 eedb_half_bank;
    int alloc_elem_id, start_elem;
    uint8 bank_found = FALSE;
    arad_pp_eg_encap_eedb_top_bank_info_t top_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    /*try to allocate new bank*/
    for (bank_id_iter = 0 ; ( bank_id_iter < _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS ) && (bank_found == FALSE )  ; bank_id_iter++) 
    {
        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO, &top_bank_info, bank_id_iter);
        BCMDNX_IF_ERR_EXIT(rv);

        if (top_bank_info.extension_bank_owner == bank_id)   
        {
            bank_found = TRUE;
            top_bank_info.extension_bank_owner = _BCM_DPP_AM_EEDB_BANK_UNASSIGNED;

            rv = SOC_DPP_WB_ENGINE_SET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_TOP_BANK_INFO, &top_bank_info, bank_id_iter);
            BCMDNX_IF_ERR_EXIT(rv);

            eedb_half_bank = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_EEDB_BANK(bank_id_iter);
            start_elem = _BCM_DPP_AM_EGRESS_TOP_BANK_TO_ELEMENT_OFFSET_IN_BANK(bank_id_iter);

            /* de-allocate the 1/4 bank in the regular bank pull-ids*/
            for ( alloc_elem_id = start_elem ; alloc_elem_id < start_elem + _BCM_DPP_AM_EGRESS_LIF_NOF_ENTRIES_PER_BANK /4   ; alloc_elem_id ++  ) 
            {
                rv = bcm_dpp_am_local_outlif_dealloc(unit,eedb_half_bank,BCM_DPP_AM_FLAG_ALLOC_WITH_ID, 1, alloc_elem_id);
                BCMDNX_IF_ERR_EXIT(rv);

            }
        }
    }

    if (bank_found == FALSE)
    {
        BCMDNX_ERR_EXIT_MSG(BCM_E_PARAM,(_BSL_BCM_MSG("Given extension bank is illegal" )));
    }

exit:
    BCMDNX_FUNC_RETURN;

}

STATIC uint32
  _bcm_dpp_am_local_outlif_extension_lif_id_get(int unit, int base_lif,  int *ext_lif)
{
    int rv, ext_bank_id, ext_offset; 
    arad_pp_eg_encap_eedb_bank_info_t eedb_bank_info;

    BCMDNX_INIT_FUNC_DEFS;

    rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_EEDB_BANK_INFO, &eedb_bank_info, _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_ID_TO_BANK_ID(base_lif));
    BCMDNX_IF_ERR_EXIT(rv);

    if (eedb_bank_info.is_extended)
    {
        ext_bank_id = (ARAD_PP_EG_ENCAP_NOF_REGULAR_EEDB_BANKS * 2 + eedb_bank_info.extension_bank / _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_HALF_BANK );

        ext_offset =  _BCM_DPP_AM_EGRESS_LIF_LOCAL_LIF_OFFSET_IN_BANK(base_lif) / 4   + ( _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_TOP_BANK * ( eedb_bank_info.extension_bank % _BCM_DPP_AM_EGRESS_LIF_NOF_TOP_BANKS_PER_HALF_BANK )  );

        *ext_lif = _BCM_DPP_AM_EGRESS_LIF_ENTRY_TO_LOCAL_LIF(ext_bank_id, ext_offset) ;
    }
    else
    {
        *ext_lif = _BCM_DPP_AM_LOCAL_LIF_ID_UNASSIGNED;
    }
    
exit:
    BCMDNX_FUNC_RETURN;
}


uint32
  _bcm_dpp_am_local_outlif_bank_phase_set(int unit, int bank_id,  int phase)
{

    SOC_PPD_EG_ENCAP_RANGE_INFO encap_range_info = {0};  
    uint32 soc_sand_rv;

    BCMDNX_INIT_FUNC_DEFS;

    /* Set HW */
    soc_sand_rv = soc_ppd_eg_encap_range_info_get(unit, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv); 

    encap_range_info.bank_access_phase[bank_id] = _bcm_dpp_am_local_outlif_bank_phase_to_access_phase(phase);

    soc_sand_rv = soc_ppd_eg_encap_range_info_set(unit, &encap_range_info);
    BCM_SAND_IF_ERR_EXIT(soc_sand_rv);

exit:
    BCMDNX_FUNC_RETURN;

}

/* indicate whether an allocated lif is a wide entry (fetch information from SW DB) */
uint32
_bcm_dpp_local_lif_is_wide_entry(int unit, int local_lif, int is_ingress, int *is_wide_entry)
{
    int rv = BCM_E_NONE; 

    BCMDNX_INIT_FUNC_DEFS;

    /* The default case is that the entry is not wide */
    *is_wide_entry = FALSE;


    if (SOC_IS_ARADPLUS_AND_BELOW(unit)) {
        /* Wide entries are irrelevant in aradplus and below */
        BCM_EXIT;
    }

    if (is_ingress) {
        bcm_dpp_am_local_inlif_info_t inlif_info;

        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_INLIF_DB, &inlif_info, local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (DPP_IS_FLAG_SET(inlif_info.local_lif_flags, BCM_DPP_AM_IN_LIF_FLAG_WIDE)){
            *is_wide_entry = 1;
        }
    } else {
        bcm_dpp_am_local_out_lif_info_t outlif_info;

        rv = SOC_DPP_WB_ENGINE_GET_ARR(unit, SOC_DPP_WB_ENGINE_VAR_JER_LOCAL_OUTLIF_DB, &outlif_info, local_lif);
        BCMDNX_IF_ERR_EXIT(rv);

        if (DPP_IS_FLAG_SET(outlif_info.local_lif_flags, BCM_DPP_AM_OUT_LIF_FLAG_WIDE)){
            *is_wide_entry = 1;
        }
    }

exit:
    BCMDNX_FUNC_RETURN;

}


