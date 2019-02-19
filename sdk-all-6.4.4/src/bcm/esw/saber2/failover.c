/*
 * $Id: failover.c $
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
 * Saber2 failover API
 */

#include <shared/bsl.h>

#include <soc/defs.h>
#include <sal/core/libc.h>

#if defined(BCM_SABER2_SUPPORT) &&  defined(INCLUDE_L3)

#include <soc/drv.h>
#include <soc/mem.h>
#include <soc/util.h>
#include <soc/debug.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/failover.h>
#include <bcm_int/esw_dispatch.h>
#include <bcm_int/esw/katana2.h>
#include <bcm_int/esw/katana.h>
#include <bcm_int/common/multicast.h>
#include <bcm_int/esw/failover.h>
#include <bcm_int/esw/triumph2.h>

typedef struct _bcm_failover_keeper_s {
    int primary_nhi;
    int secondary_nhi;
} _bcm_failover_keeper_t;
STATIC _bcm_failover_keeper_t *failover_keeper;

int
bcm_sb2_failover_init(int unit)
{
    int index;
    if (failover_keeper == NULL) {
        failover_keeper =
            sal_alloc((sizeof(_bcm_failover_keeper_t) * 
                        soc_mem_index_count(unit,MMU_PROT_GROUP_TABLEm )), 
                    "failover_keeper");
        for (index = 0; index < soc_mem_index_count(unit,MMU_PROT_GROUP_TABLEm) ;
                index++) { 
            failover_keeper[index].primary_nhi = -1;
            failover_keeper[index].secondary_nhi = -1;
        }

    }
    return BCM_E_NONE;
}

int
bcm_sb2_failover_cleanup(int unit)
{
    if (failover_keeper) {
        sal_free(failover_keeper);
        failover_keeper = NULL;
     }
 
    return BCM_E_NONE;

}
/*
 * Function:
 *		bcm_sb2_failover_prot_nhi_set
 * Purpose:
 *		Set the parameters for PROT_NHI
 * Parameters:
 *		IN :  unit
 *           IN :  Primary Next Hop Index
 *           IN :  Protection Next Hop Index
 *           IN :  Failover Group Index
 * Returns:
 *		BCM_E_XXX
 */

int
bcm_sb2_failover_prot_nhi_set(int unit, uint32 flags, int nh_index, uint32 prot_nh_index, 
                                           bcm_multicast_t  mc_group, bcm_failover_t failover_id)
{
    mmu_initial_nhop_tbl_entry_t   prot_nhi_entry;
    int rv;
    soc_field_t field[4] = {
        PROT_GRP_IDX0f,
        PROT_GRP_IDX1f,
        PROT_GRP_IDX2f,
        PROT_GRP_IDX3f};
    _BCM_GET_FAILOVER_ID(failover_id);
    if (failover_keeper[failover_id].primary_nhi != -1) {
        return BCM_E_PARAM;
    }
    failover_keeper[failover_id].primary_nhi = nh_index;
    failover_keeper[failover_id].secondary_nhi = prot_nh_index;

    BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm, 
                             MEM_BLOCK_ANY, prot_nh_index / 4, &prot_nhi_entry));

    soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm, 
                                       &prot_nhi_entry, field[prot_nh_index % 4],
                                       (uint32) failover_id);

    rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                       MEM_BLOCK_ALL, prot_nh_index / 4, &prot_nhi_entry);

    return rv;

}


/*
 * Function: bcm_sb2_failover_prot_nhi_get
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
bcm_sb2_failover_prot_nhi_get(int unit, int nh_index, 
            bcm_failover_t  *failover_id, int  *prot_nh_index, bcm_multicast_t  *mc_group)
{
    mmu_initial_nhop_tbl_entry_t   prot_nhi_entry;
    soc_field_t field[4] = {
        PROT_GRP_IDX0f,
        PROT_GRP_IDX1f,
        PROT_GRP_IDX2f,
        PROT_GRP_IDX3f};


    BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm, 
                             MEM_BLOCK_ANY, (*prot_nh_index / 4), &prot_nhi_entry));

    *failover_id =   soc_mem_field32_get(unit, MMU_INITIAL_NHOP_TBLm,
                              &prot_nhi_entry, field[(*prot_nh_index % 4)]);

    *prot_nh_index = failover_keeper[*failover_id].secondary_nhi;

    return BCM_E_NONE;

}


/*
 * Function:
 *        bcm_sb2_failover_status_set
 * Purpose:
 *        Set the parameters for a failover object
 * Parameters:
 *        IN :  unit
 *           IN :  failover_id
 *           IN :  value
 * Returns:
 *        BCM_E_XXX
 */

int
bcm_sb2_failover_status_set(int unit,
                                     bcm_failover_element_t *failover,
                                     int value)
{
    int rv = BCM_E_UNAVAIL;
    mmu_initial_nhop_tbl_entry_t  mmu_prot_nhi_entry;
    soc_field_t field[4] =
    {
        PROT_DROP_STATUS0f,
        PROT_DROP_STATUS1f,
        PROT_DROP_STATUS2f,
        PROT_DROP_STATUS3f
    };

    if ((value < 0) || (value > 1)) {
       return BCM_E_PARAM;
    }
    _BCM_GET_FAILOVER_ID(failover->failover_id);
    if (failover->failover_id != BCM_FAILOVER_INVALID) {
         /* Group protection for Port and Tunnel: Egress and Ingress */
         BCM_IF_ERROR_RETURN(
              bcm_tr2_failover_mmu_id_validate ( unit, failover->failover_id ));

         if (value) {
             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[failover->failover_id].secondary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[failover->failover_id].secondary_nhi % 4] , 0);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[failover->failover_id].secondary_nhi / 4 ,
                     &mmu_prot_nhi_entry);

             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[failover->failover_id].primary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[failover->failover_id].primary_nhi % 4] , 1);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[failover->failover_id].primary_nhi / 4 ,
                     &mmu_prot_nhi_entry);
         } else {
             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[failover->failover_id].primary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[failover->failover_id].primary_nhi % 4] , 0);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[failover->failover_id].primary_nhi / 4 ,
                     &mmu_prot_nhi_entry);

             BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                         MEM_BLOCK_ANY, 
                         failover_keeper[failover->failover_id].secondary_nhi / 4,
                         &mmu_prot_nhi_entry));

             soc_mem_field32_set(unit, MMU_INITIAL_NHOP_TBLm,
                     &mmu_prot_nhi_entry,
                     field[failover_keeper[failover->failover_id].secondary_nhi % 4] , 1);

             rv = soc_mem_write(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ALL, failover_keeper[failover->failover_id].secondary_nhi / 4 ,
                     &mmu_prot_nhi_entry);

         }
    } else {
       return BCM_E_PARAM;
    } 
    return rv;
}


/*
 * Function:
 *        bcm_sb2_failover_status_get
 * Purpose:
 *        Get the parameters for a failover object
 * Parameters:
 *        IN :  unit
 *           IN :  failover_id
 *           OUT :  value
 * Returns:
 *        BCM_E_XXX
 */

int
bcm_sb2_failover_status_get(int unit,
                                     bcm_failover_element_t *failover,
                                     int  *value)
{
    mmu_initial_nhop_tbl_entry_t   prot_nhi_entry;
    soc_field_t field[4] =
    {
        PROT_DROP_STATUS0f,
        PROT_DROP_STATUS1f,
        PROT_DROP_STATUS2f,
        PROT_DROP_STATUS3f
    };

    _BCM_GET_FAILOVER_ID(failover->failover_id);
    if (failover->failover_id != BCM_FAILOVER_INVALID) {

         BCM_IF_ERROR_RETURN(
            bcm_tr2_failover_mmu_id_validate ( unit, failover->failover_id ));

         BCM_IF_ERROR_RETURN (soc_mem_read(unit, MMU_INITIAL_NHOP_TBLm,
                     MEM_BLOCK_ANY, 
                     failover_keeper[failover->failover_id].secondary_nhi / 4,
                     &prot_nhi_entry));

         *value =   soc_mem_field32_get(unit, MMU_INITIAL_NHOP_TBLm,
                 &prot_nhi_entry, 
                 field[failover_keeper[failover->failover_id].secondary_nhi % 4]);
         *value = *value ? 0 : 1 ;
    } else {
        return BCM_E_PARAM;
    }
    return BCM_E_NONE;
}

#endif /* defined(BCM_SABER2_SUPPORT) &&  defined(INCLUDE_L3) */
