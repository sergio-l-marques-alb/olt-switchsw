/*
  * $Copyright: (c) 2008 Broadcom Corp.
 * All Rights Reserved.$
 *
 * ESW failover API
 */


#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm_int/esw/mbcm.h>
#include <bcm_int/esw_dispatch.h>
#if defined(BCM_TRIUMPH2_SUPPORT)
#include <bcm_int/esw/field.h>
#include <bcm_int/esw/triumph2.h>
#endif /* BCM_TRIUMPH2_SUPPORT */


/*
 * Function:
 *		bcm_esw_failover_init
 * Purpose:
 *		Init  failover module
 * Parameters:
 *		IN :  unit
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_esw_failover_init(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
        return bcm_tr2_failover_init(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}


 /* Function:
 *      bcm_failover_cleanup
 * Purpose:
 *      Detach the failover module, clear all HW states
 * Parameters:
 *      unit     - Device Number
 * Returns:
 *      BCM_E_XXXX
 */

int
bcm_esw_failover_cleanup(int unit)
{
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
        return bcm_tr2_failover_cleanup(unit);
    }
#endif
    return BCM_E_UNAVAIL;
}


/*
 * Function:
 *		bcm_esw_failover_create
 * Purpose:
 *		Create a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  flags
 *           OUT :  failover_id
 * Returns:
 *		BCM_E_XXX
 */

int 
bcm_esw_failover_create(int unit, uint32 flags, bcm_failover_t *failover_id)
{
    int rv = BCM_E_UNAVAIL;	
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
         if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
              rv = bcm_tr2_failover_create(unit, flags, failover_id);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_destroy
 * Purpose:
 *		Destroy a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 * Returns:
 *		BCM_E_XXX
 */

int 
bcm_esw_failover_destroy(int unit, bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
         if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
              rv = bcm_tr2_failover_destroy(unit, failover_id);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
   }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_set
 * Purpose:
 *		Set a failover object to enable or disable (note that failover object
 *            0 is reserved
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 *           IN :  value
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_esw_failover_set(int unit, bcm_failover_t failover_id, int value)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
      if (soc_feature(unit, soc_feature_failover)) {
             if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
                   rv = bcm_tr2_failover_set(unit, failover_id, value);
                   bcm_tr2_failover_unlock (unit);
             }
             return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_get
 * Purpose:
 *		Get the enable status of a failover object
 * Parameters:
 *		IN :  unit
 *           IN :  failover_id
 *           OUT :  value
 * Returns:
 *		BCM_E_XXX
 */


int 
bcm_esw_failover_get(int unit, bcm_failover_t failover_id, int *value)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
      if (soc_feature(unit, soc_feature_failover)) {
         if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
              rv = bcm_tr2_failover_get(unit, failover_id, value);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_prot_nhi_create
 * Purpose:
 *		Create  the  entry for  PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  Primary Next Hop Index
 *           IN :  Failover Group Index
 * Returns:
 *		BCM_E_XXX
 */


int
bcm_esw_failover_prot_nhi_create (int unit, int nh_index)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
      if (soc_feature(unit, soc_feature_failover)) {
         if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
              rv = bcm_tr2_failover_prot_nhi_create(unit, nh_index);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
    return rv;
}


/*
 * Function:
 *		bcm_esw_failover_prot_nhi_set
 * Purpose:
 *		Set the parameters for PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  Primary Next Hop Index
 *           IN :  Protection Next Hop Index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_esw_failover_prot_nhi_set(int unit, int nh_index, uint32 prot_nh_index, bcm_failover_t failover_id)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
         if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
              rv = bcm_tr2_failover_prot_nhi_set(unit, nh_index, prot_nh_index, failover_id);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
    return rv;
}

/*
 * Function:
 *		bcm_esw_failover_prot_nhi_get
 * Purpose:
 *		Get the parameters for PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  primary Next Hop Index
 *           OUT :  Failover Group Index
 *           OUT : Protection Next Hop Index
 * Returns:
 *		BCM_E_XXX
 */


int
bcm_esw_failover_prot_nhi_get(int unit, int nh_index, bcm_failover_t  *failover_id, int  *prot_nh_index)
{
    int rv = BCM_E_UNAVAIL;
#ifdef BCM_TRIUMPH2_SUPPORT
    if (soc_feature(unit, soc_feature_failover)) {
         if ( bcm_tr2_failover_lock (unit) == BCM_E_NONE ) {
              rv = bcm_tr2_failover_prot_nhi_get(unit, nh_index, failover_id, prot_nh_index);
              bcm_tr2_failover_unlock (unit);
         }
         return rv;
    }
#endif
    return rv;
}

