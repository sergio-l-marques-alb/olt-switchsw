/*
 * $Id: soc_sw_db.c,v 1.24 Broadcom SDK $
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
 */
#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_SWDB

#include <shared/bsl.h>

#include <soc/dcmn/error.h>

#include <soc/dpp/soc_sw_db.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/Petra/petra_sw_db.h>
#include <soc/dpp/Petra/petra_api_ssr.h>
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_TM/pb_api_ssr.h>
#endif
#ifdef BCM_PETRAB_SUPPORT
#include <soc/dpp/Petra/PB_PP/pb_pp_api_ssr.h>
#include <soc/dpp/Petra/PB_PP/pb_pp_sw_db.h>
#endif
#include <bcm_int/dpp/error.h>
#ifdef BCM_ARAD_SUPPORT
#include <soc/dpp/ARAD/arad_sw_db.h>
#ifdef BCM_WARM_BOOT_SUPPORT
#include <soc/dpp/ARAD/arad_wb_db.h>
#endif /* BCM_WARM_BOOT_SUPPORT */
#endif



#define SOC_SW_DB_PB_SW_DB_MAX_SIZE       (800000)
#define SOC_SW_DB_PB_PP_SW_DB_MAX_SIZE    (10000000)


#ifdef BCM_WARM_BOOT_SUPPORT

#ifdef BCM_PETRAB_SUPPORT

/*init - allocate scache if it is cold boot, close the old db and restore from scache if wormboot*/

int
soc_sw_db_init(int unit)
{
    int                     rv;
    soc_scache_handle_t     soc_petra_sw_db_wb_hdl;    
    uint32                  stable_size, scache_size, soc_sand_rv, scache_len = 0;    
    uint8                   *scache_ptr, *ptr;   
    
    SOC_SCACHE_HANDLE_SET(soc_petra_sw_db_wb_hdl, unit, SOC_MODULE_SW_DB, 0);

    if (SOC_WARM_BOOT(unit)) 
    {
      scache_len = 0;
      rv = soc_scache_ptr_get(unit, soc_petra_sw_db_wb_hdl, 
        &scache_ptr, &scache_len);
      if ((rv != SOC_E_NONE) || (scache_ptr == NULL) || (scache_len == 0)) {        
        return rv;
      }
      ptr = scache_ptr;

      /* close sw_db that was created on soc_petra init*/
      soc_sand_rv = soc_petra_sw_db_device_close(
        (unit)
        );
      SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
      soc_sand_rv = soc_pb_ssr_from_buff(
        (unit),
        ptr,
        scache_len
        );
      SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    } else 
    {      
      /* Is Level 2 recovery even available? */
      rv = soc_stable_size_get(unit, (int*)&stable_size);
      if (SOC_FAILURE(rv)) {
        return rv; 
      }      
      /* setup scache */
      if (stable_size > 0) {
        scache_size = SOC_SW_DB_PB_SW_DB_MAX_SIZE;        
        rv = soc_scache_alloc(unit, soc_petra_sw_db_wb_hdl, scache_size);
        if (rv != SOC_E_NONE) {          
          return rv;
        }
      }      
    }


    SOC_SCACHE_HANDLE_SET(soc_petra_sw_db_wb_hdl, unit, SOC_MODULE_SW_DB_PP, 0);

    if (SOC_WARM_BOOT(unit)) 
    {
      scache_len = 0;
      rv = soc_scache_ptr_get(unit, soc_petra_sw_db_wb_hdl, 
        &scache_ptr, &scache_len);
      if ((rv != SOC_E_NONE) || (scache_ptr == NULL) || (scache_len == 0)) {        
        return rv;
      }
      ptr = scache_ptr;

      /* close sw_db that was created on soc_petra init*/
      soc_sand_rv = soc_pb_pp_sw_db_device_close(
         (unit)
        );
      SOC_SAND_IF_ERR_RETURN(soc_sand_rv);
      soc_sand_rv = soc_pb_pp_ssr_from_buff(
        (unit),
        ptr,
        scache_len
        );
      SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    } else 
    {      
      /* Is Level 2 recovery even available? */
      rv = soc_stable_size_get(unit, (int*)&stable_size);
      if (SOC_FAILURE(rv)) {
        return rv; 
      }      
      /* setup scache */
      if (stable_size > 0) {
        scache_size = SOC_SW_DB_PB_PP_SW_DB_MAX_SIZE;        
        rv = soc_scache_alloc(unit, soc_petra_sw_db_wb_hdl, scache_size);
        if (rv != SOC_E_NONE) {          
          return rv;
        }
      }      
    }


    return SOC_E_NONE;
}


/*
 * Function:
 *      _soc_pb_sw_db_sync
 * Purpose:
 *      Record soc_petra_sw_db module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
STATIC int
_soc_pb_sw_db_sync(int unit)
{  
  soc_scache_handle_t   soc_petra_sw_db_wb_hdl;  
  uint8                 *scache_ptr = NULL, *ptr;  
  uint32                scache_len, soc_sand_rv;
  int                   rv = SOC_E_NONE;  

  /* build the relevant handle*/
  SOC_SCACHE_HANDLE_SET(soc_petra_sw_db_wb_hdl, unit, SOC_MODULE_SW_DB, 0);

  if (SOC_WARM_BOOT(unit)) {
    LOG_ERROR(BSL_LS_SOC_SWDB,
              (BSL_META_U(unit,
                          "Cannot write to SCACHE during WarmBoot\n")));
    return SOC_E_INTERNAL;
  }

  scache_len = 0;

  /* get the ptr to the scache buffer */
  rv = soc_scache_ptr_get(unit, soc_petra_sw_db_wb_hdl, 
        &scache_ptr, &scache_len);
  if ((rv != SOC_E_NONE) || (scache_ptr == NULL) || (scache_len == 0)) 
  {        
    return rv;
  }
  ptr = scache_ptr;

  /* use dune driver ssr mechanism to save the sw_db */
  soc_sand_rv = soc_pb_ssr_to_buff(
      (unit),
      ptr,
      scache_len
    );
  SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

  return SOC_E_NONE;  
}

/*
 * Function:
 *      _soc_pb_pp_sw_db_sync
 * Purpose:
 *      Record soc_pb_pp_sw_db module persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
_soc_pb_pp_sw_db_sync(int unit)
{  
  soc_scache_handle_t   soc_petra_sw_db_wb_hdl;  
  uint8                 *scache_ptr = NULL, *ptr;  
  uint32                scache_len, soc_sand_rv;
  int                   rv = SOC_E_NONE;  

  /* build the relevant handle*/
  SOC_SCACHE_HANDLE_SET(soc_petra_sw_db_wb_hdl, unit, SOC_MODULE_SW_DB_PP, 0);

  if (SOC_WARM_BOOT(unit)) {
    LOG_ERROR(BSL_LS_SOC_SWDB,
              (BSL_META_U(unit,
                          "Cannot write to SCACHE during WarmBoot\n")));
    return SOC_E_INTERNAL;
  }

  scache_len = 0;

  /* get the ptr to the scache buffer */
  rv = soc_scache_ptr_get(unit, soc_petra_sw_db_wb_hdl, 
        &scache_ptr, &scache_len);
  if ((rv != SOC_E_NONE) || (scache_ptr == NULL) || (scache_len == 0)) 
  {        
    return rv;
  }
  ptr = scache_ptr;

  /* use dune driver ssr mechanism to save the sw_db */
  soc_sand_rv = soc_pb_pp_ssr_to_buff(
      (unit),
      ptr,
      scache_len
    );
  SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

  return SOC_E_NONE;  
}
#endif /*BCM_PETRAB_SUPPORT*/

/*
 * Function:
 *      soc_sw_db_sync
 * Purpose:
 *      Record soc_sand sw_db modules persistent info for Level 2 Warm Boot
 * Parameters:
 *      unit - StrataSwitch unit number.
 * Returns:
 *      BCM_E_XXX
 */
int
 soc_sw_db_sync(int unit)
{
    int rv = SOC_E_NONE;  
#ifdef BCM_ARAD_SUPPORT
    uint32 soc_sand_rv;
#endif /*BCM_ARAD_SUPPORT*/
#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        rv = _soc_pb_sw_db_sync(unit);
        if (rv != SOC_E_NONE) {    
            return rv;
        }
    }
#endif /*BCM_PETRAB_SUPPORT*/


#ifdef BCM_ARAD_SUPPORT
    if (SOC_IS_ARAD(unit)) {
        soc_sand_rv = arad_wb_db_sync(unit);
        SOC_SAND_IF_ERR_RETURN(soc_sand_rv);

    }
#endif /*BCM_ARAD_SUPPORT*/


#ifdef BCM_PETRAB_SUPPORT
    if (SOC_IS_PETRAB(unit)) {
        rv = _soc_pb_pp_sw_db_sync(unit);  
        if (rv != SOC_E_NONE) {    
            return rv;
        }
    }
#endif /*BCM_PETRAB_SUPPORT*/

    rv = soc_dpp_wb_engine_sync(unit);
        if (rv != SOC_E_NONE) {    
            return rv;
        }

    return rv;
}


int
soc_dpp_scache_ptr_get(int unit, soc_scache_handle_t handle, soc_dpp_scache_oper_t oper,
                                 int flags, uint32 *size, uint8 **scache_ptr,
                                 uint16 version, uint16 *recovered_ver, int *already_exists)
{
    int        rc = SOC_E_NONE;  
    uint32     allocated_size;
    uint16     storage_version = version;
    int        alloc_size, incr_size;

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(scache_ptr);

    if (oper == socDppScacheRetreive) {
        SOCDNX_NULL_CHECK(size);
        SOCDNX_NULL_CHECK(recovered_ver);

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
           LOG_ERROR(BSL_LS_SOC_SWDB,
                     (BSL_META_U(unit,
                                 SOC_DPP_MSG("Failed in the function soc_scache_ptr_get.\n\r"))));
           return(rc);
        }
        (*size) = allocated_size;

        sal_memcpy(&version, *scache_ptr, sizeof(uint16));
        (*recovered_ver) = version;

        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }

        if (storage_version < *recovered_ver) {
            LOG_ERROR(BSL_LS_SOC_SWDB,
                      (BSL_META_U(unit,
                                  "Downgrade detected. Current version=%d.%d  found %d.%d\n"),
                                  SOC_SCACHE_VERSION_MAJOR(version),
                       SOC_SCACHE_VERSION_MINOR(version),
                       SOC_SCACHE_VERSION_MAJOR(storage_version),
                       SOC_SCACHE_VERSION_MINOR(storage_version)));

                /* Notify the application with an event                  */
                /* The application will then need to reconcile the       */
                /* version differences using the documented behavioral   */
                /* differences on per module (handle) basis              */
                SOC_IF_ERROR_RETURN
                    (soc_event_generate(unit,
                                        SOC_SWITCH_EVENT_WARM_BOOT_DOWNGRADE,
                                        handle, storage_version, version));


            if (flags & SOC_DPP_SCACHE_DOWNGRADE_INVALID) {
                LOG_ERROR(BSL_LS_SOC_SWDB,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("Internal error.\n\r"))));
                return(SOC_E_INTERNAL);
            }
        }
        else if (storage_version > *recovered_ver) {

            /* Individual BCM module implementations are version-aware. The       */
            /* default_ver is the latest version that the module is aware of.     */
            /* Each module should be able to understand versions <= default_ver.  */
            /* The actual recovered_ver is returned to the calling module during  */
            /* warm boot initialization. The individual module needs to parse its */
            /* scache based on the recovered_ver.                                 */

            LOG_VERBOSE(BSL_LS_SOC_SWDB,
                        (BSL_META_U(unit,
                                    "Upgrade scenario supported. Current version=%d.%d  found %d.%d\n"),
                                    SOC_SCACHE_VERSION_MAJOR(version),
                         SOC_SCACHE_VERSION_MINOR(version),
                         SOC_SCACHE_VERSION_MAJOR(storage_version),
                         SOC_SCACHE_VERSION_MINOR(storage_version)));
        }
    }
    else if (oper == socDppScacheCreate) {
        SOCDNX_NULL_CHECK(size);
        SOCDNX_NULL_CHECK(already_exists);

        SOC_SCACHE_ALIGN_SIZE(*size);
        
        alloc_size = (*size) + 2*SOC_WB_SCACHE_CONTROL_SIZE;

        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if ((rc != SOC_E_NONE) && (rc != SOC_E_NOT_FOUND) ) {
            LOG_ERROR(BSL_LS_SOC_SWDB,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("Failed in the function soc_scache_ptr_get.\n\r"))));
            return(rc);
        }

        if (rc == SOC_E_NONE) { /* already exists */
            (*already_exists) = TRUE;
            if (flags & SOC_DPP_SCACHE_EXISTS_ERROR) {
                LOG_ERROR(BSL_LS_SOC_SWDB,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("Failed : existance error.\n\r"))));
				return(SOC_E_PARAM);
            }

        }
        else { /* need to create */
            (*already_exists) = FALSE;
            rc = soc_scache_alloc(unit, handle, alloc_size);
            if (rc != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_SWDB,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("Failed in the function soc_scache_alloc.\n\r"))));
				return(rc);
            }

            rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
            if (rc != SOC_E_NONE) {
                LOG_ERROR(BSL_LS_SOC_SWDB,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("Failed in the function soc_scache_ptr_get.\n\r"))));
				return(rc);

            }
            if ((*scache_ptr) == NULL) {
                LOG_ERROR(BSL_LS_SOC_SWDB,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("Memory allocation failure.\n\r"))));
				return(SOC_E_MEMORY);                
            }
        }

        if (alloc_size != allocated_size) {
            LOG_ERROR(BSL_LS_SOC_SWDB,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("Internal error.\n\r"))));
			return(SOC_E_INTERNAL);
        }
        if ((*already_exists) == FALSE) {
            sal_memcpy(*scache_ptr, &version, sizeof(uint16));
        }
        else {
            sal_memcpy(&storage_version, *scache_ptr, sizeof(uint16));
            if (storage_version != version) {
                LOG_ERROR(BSL_LS_SOC_SWDB,
                          (BSL_META_U(unit,
                                      SOC_DPP_MSG("Internal error.\n\r"))));
				return(SOC_E_INTERNAL);

            }
        }

        if (recovered_ver != NULL) {
            (*recovered_ver) = version;
        }
    }
    else if (oper == socDppScacheRealloc) {
        SOCDNX_NULL_CHECK(size);

        /* get current size */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SWDB,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("Failed in the function soc_scache_ptr_get.\n\r"))));
            return(rc);
        }

        /* allocate new size */
        SOC_SCACHE_ALIGN_SIZE(*size);
        
        alloc_size = (*size) + 2*SOC_WB_SCACHE_CONTROL_SIZE;
        incr_size = alloc_size - allocated_size;

        rc = soc_scache_realloc(unit, handle, incr_size);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SWDB,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("Failed in the function soc_scache_realloc.\n\r"))));
			return(rc);
        }

        /* update version */
        rc = soc_scache_ptr_get(unit, handle, scache_ptr, &allocated_size);
        if (rc != SOC_E_NONE) {
            LOG_ERROR(BSL_LS_SOC_SWDB,
                      (BSL_META_U(unit,
                                  SOC_DPP_MSG("Failed in the function soc_scache_ptr_get.\n\r"))));
			return(rc);
        }
        sal_memcpy(&storage_version, *scache_ptr, sizeof(uint16));
        sal_memcpy(*scache_ptr, &version, sizeof(uint16));

        if (recovered_ver != NULL) {
            (*recovered_ver) = storage_version; 
        }
        if (already_exists != NULL) {
            (*already_exists) = TRUE;
        }
    }
    else if (oper == socDppScacheFreeCreate) {
        LOG_ERROR(BSL_LS_SOC_SWDB,
                  (BSL_META_U(unit,
                              SOC_DPP_MSG("Invalid parameter.\n\r"))));
		return SOC_E_PARAM;
		/* only re-alloc supported */
    }
    else {
        LOG_ERROR(BSL_LS_SOC_SWDB,
                  (BSL_META_U(unit,
                              SOC_DPP_MSG("Invalid parameter.\n\r"))));
		return SOC_E_PARAM;
		/* only re-alloc supported */
    }

    /* Advance over scache control info */
    (*scache_ptr) += SOC_WB_SCACHE_CONTROL_SIZE;

    
    (*size) = (allocated_size - 2*SOC_WB_SCACHE_CONTROL_SIZE); /* update size */

exit:
    SOCDNX_FUNC_RETURN;

}

#endif /*BCM_WARM_BOOT_SUPPORT*/

#undef _ERR_MSG_MODULE_NAME

/*need to add flag mechanism to sw db*/
