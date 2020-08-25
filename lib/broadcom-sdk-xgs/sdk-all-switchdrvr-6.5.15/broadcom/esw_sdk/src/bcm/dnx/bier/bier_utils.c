/** \file bier_utils.c
 * $Id$
 *
 * General MULTICAST functionality for DNX.
 * Dedicated set of MULTICAST APIs are distributed between multicast_*.c files: \n
 *
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_MULTICAST

/*
 * Include files.
 * {
 */
#include <shared/shrextend/shrextend_debug.h>
#include <bcm/types.h>
#include <bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <bcm_int/dnx/algo/algo_gpm.h>
#include <bcm_int/dnx/multicast/multicast.h>
#include <bcm_int/dnx/legacy/gport_mgmt.h>
#include <bcm_int/dnx/trunk/trunk.h>

#include <soc/dnx/dnx_data/auto_generated/dnx_data_bier.h>
#include <soc/dnx/swstate/auto_generated/access/dnx_bier_db_access.h>

#include "bier_imp.h"
/*
 * }
 */

/*
 * MACROs
 * {
 */

/*
 * }
 */

/*
 * See .h
 */
shr_error_e
dnx_bier_convert_hw_to_gport(
    int unit,
    uint32 is_ingress,
    int core_id,
    dnx_bier_mcdb_dbal_field_t entry_val,
    bcm_gport_t * dest,
    bcm_if_t * cud)
{
    uint32 dest_val = 0;
    SOC_DNX_DEST_INFO dest_info;
    dbal_fields_e dbal_dest_type;
    bcm_port_t port;
    bcm_trunk_t trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    *cud = (entry_val.cud == DNX_MULTICAST_INVALID_CUD) ? BCM_IF_INVALID : entry_val.cud;
    if (is_ingress)
    {
        if (entry_val.dest == DNX_MULTICAST_ING_INVALID_DESTINATION)
        {
            *dest = BCM_GPORT_INVALID;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                            (unit, DBAL_FIELD_DESTINATION, &entry_val.dest, &dbal_dest_type, &dest_val));
            dest_info.dbal_type = dbal_dest_type;
            dest_info.id = dest_val;
            SHR_IF_ERR_EXIT(_bcm_dnx_gport_from_tm_dest_info(unit, dest, &dest_info));
        }
    }
    else
    {
        if (entry_val.dest == DNX_MULTICAST_EGR_INVALID_DESTINATION)
        {
            *dest = BCM_GPORT_INVALID;
        }
        else
        {
            SHR_IF_ERR_EXIT(dbal_fields_parent_field_decode
                            (unit, DBAL_FIELD_EGRESS_DESTINATION, &entry_val.dest, &dbal_dest_type, &dest_val));

            if (dbal_dest_type == DBAL_FIELD_DSP_AGG)
            {
               /** For egress trunks are represented by egress trunk profiles.
                * Get the corresponding trunk ID from the egress trunk profile decoded from HW */
                SHR_IF_ERR_EXIT(dnx_trunk_containing_egress_trunk_get(unit, dest_val, &trunk_id));
                dest_info.dbal_type = DBAL_FIELD_LAG_ID;
                dest_info.id = trunk_id;
                SHR_IF_ERR_EXIT(_bcm_dnx_gport_from_tm_dest_info(unit, dest, &dest_info));
            }
            else
            {
                SHR_IF_ERR_EXIT(dnx_algo_port_tm_to_logical_get(unit, core_id, entry_val.dest, &port));
                BCM_GPORT_LOCAL_SET(*dest, port);
            }
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/*
 * See .h
 */
shr_error_e
dnx_bier_convert_gport_to_hw(
    int unit,
    uint32 is_ingress,
    bcm_gport_t dest,
    bcm_if_t encap,
    int *core_id,
    dnx_bier_mcdb_dbal_field_t * entry_val,
    uint8 trunk_create)
{
    dnx_algo_gpm_gport_phy_info_t gport_info;
    int egress_trunk_id;
    SHR_FUNC_INIT_VARS(unit);

    entry_val->dest = DNX_MULTICAST_EGR_INVALID_DESTINATION;
    entry_val->cud = (encap == BCM_IF_INVALID) ? DNX_MULTICAST_INVALID_CUD : encap;

    if (entry_val->cud > DNX_MULTICAST_INVALID_CUD)
    {
        SHR_ERR_EXIT(_SHR_E_CONFIG, "invalid CUD (0x%x)", encap);
    }

    if (is_ingress)
    {
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_FLAGS_NONE, dest, &entry_val->dest));
        *core_id = 0;
    }
    else
    {
        /*
         * DNX_MULTICAST_IS_EGRESS 
         */
        if (BCM_GPORT_IS_TRUNK(dest))
        {
            /** In case of TRUNK should be created at both cores */
            *core_id = _SHR_CORE_ALL;

            /** When adding a TRUNK to an egress MC group, a trunk egress profile must be allocated. */
            if (trunk_create == TRUE)
            {
                /** Allocate egress profile for trunk */
                SHR_IF_ERR_EXIT(dnx_trunk_egress_create(unit, BCM_GPORT_TRUNK_GET(dest), &egress_trunk_id));
            }
        }
        else
        {
            /** Get core using local port */
            SHR_IF_ERR_EXIT(dnx_algo_gpm_gport_phy_info_get
                            (unit, dest, DNX_ALGO_GPM_GPORT_TO_PHY_OP_PP_PORT_IS_MANDATORY, &gport_info));

            *core_id = gport_info.internal_port_pp_info.core_id[0];
        }
        SHR_IF_ERR_EXIT(algo_gpm_encode_destination_field_from_gport
                        (unit, ALGO_GPM_ENCODE_DESTINATION_EGRESS_MULTICAST, dest, &entry_val->dest));

        /** When removing a replication which is TRUNK the egress profile must be removed. This is done after the destination is encoded.*/
        if (BCM_GPORT_IS_TRUNK(dest) && (trunk_create == FALSE))
        {
            /** Destroy the trunk egress profile. */
            SHR_IF_ERR_EXIT(dnx_trunk_egress_destroy(unit, BCM_GPORT_TRUNK_GET(dest)));
        }
    }

exit:
    SHR_FUNC_EXIT;
}
