#include <soc/mcm/memregs.h>
/* $Id: qax_fabric.c,v 1.96 Broadcom SDK $
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
*/


#ifdef _ERR_MSG_MODULE_NAME
  #error "_ERR_MSG_MODULE_NAME redefined"
#endif

#define _ERR_MSG_MODULE_NAME BSL_SOC_FABRIC

/*************
 * INCLUDES  *
 *************/
#include <shared/bsl.h>
#include <shared/swstate/access/sw_state_access.h>
#include <soc/mem.h>
#include <soc/register.h>
/* { */
#include <soc/dcmn/error.h>
#include <soc/dcmn/dcmn_mem.h>
#include <soc/dpp/drv.h>
#include <soc/dpp/cosq.h>
#include <soc/dpp/dpp_wb_engine.h>
#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/QAX/qax_fabric.h>
#include <soc/dpp/QAX/qax_mgmt.h>

/*************
 * DEFINES   *
 *************/
/* { */


/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */


/*********************************************************************
* NAME:
*     soc_qax_fabric_force_set
* FUNCTION:
*     DIAG function. Force fabric interface for local / fabric or restore back to operational mode
* INPUT:
*       int   unit - Identifier of the device to access.
*       soc_dpp_fabric_force_t force - enum for requested force mode (local/fabric/restore)
* RETURNS:
*       OK or ERROR indication.
* REMARKS:
*       Relevant for Kalia only. Not supported in qax.
*       Used in mbcm dispatcher.
*********************************************************************/
soc_error_t
  soc_qax_fabric_force_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN soc_dpp_fabric_force_t        force
  )
{
    uint32  reg32_val;

    SOCDNX_INIT_FUNC_DEFS;

    if (force == socDppFabricForceFabric) {

        /*Force traffic routing to Fabric*/
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0x1); /*Forcing Fabric*/
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0x0); /*Disabling local forcing, to allow local->fabric immediate transition (without restoring first)*/
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

        /*Force fabric for control cells*/
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x1); /*Disables control cells local routing*/
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

    } else if (force == socDppFabricForceLocal) {

        /*Force traffic routing to local Egress*/
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0x1); /*Forcing Local*/
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0x0); /*Disabling Fabric forcing, to allow fabric->local immediate transition (without restoring first)*/
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

        /*Allow local route for control cells*/
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0); /*Allow control cells local routing*/
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

    } else if (force == socDppFabricForceRestore) {

        /* Restore default configurations by disabling local / fabric traffic routing forcing */
        SOCDNX_IF_ERR_EXIT(READ_ECI_GLOBAL_GENERAL_CFG_2r(unit, &reg32_val));
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_LOCALf, 0x0); /*Disabling Local forcing*/
        soc_reg_field_set(unit, ECI_GLOBAL_GENERAL_CFG_2r, &reg32_val, FORCE_FABRICf, 0x0); /*Disabling Fabric forcing*/
        SOCDNX_IF_ERR_EXIT(WRITE_ECI_GLOBAL_GENERAL_CFG_2r(unit, reg32_val));

        /*Allow local route for control cells*/
        SOCDNX_IF_ERR_EXIT(READ_FCT_FCT_ENABLER_REGISTERr(unit, &reg32_val));
        soc_reg_field_set(unit, FCT_FCT_ENABLER_REGISTERr, &reg32_val, DIS_LCLRTf, 0x0); /*Allow control cells local routing*/
        SOCDNX_IF_ERR_EXIT(WRITE_FCT_FCT_ENABLER_REGISTERr(unit, reg32_val));

    } else /*Error checking - force mode is not supported*/{

        cli_out("Option not supported\n");
        cli_out("Supported options are fabric, local, default\n");
        SOCDNX_IF_ERR_EXIT(_SHR_E_UNAVAIL);

    }


exit:
  SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*     soc_qax_fabric_multicast_set
* FUNCTION:
*     Setting destination for a specific multicast id in kalia
* INPUT:
*       int             unit            - Identifier of the device to access.
*       soc_multicast_t mc_id           - multicast id
*       uint32          destid_count    - number of destination for this mc_id
*       soc_module_t    *destid_array   - specific destination for replication for this specific mc_id
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
soc_qax_fabric_multicast_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  soc_multicast_t                mc_id,
    SOC_SAND_IN  uint32                         destid_count,
    SOC_SAND_IN  soc_module_t                   *destid_array
  )
{
    int index, offset, i;
    uint32 mc_rep = 0;
    soc_module_t local_dest;
    uint32 my_core0_fap_id;
    SHR_BITDCLNAME (table_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    index = mc_id / SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE;
    offset = (mc_id % SOC_JER_FABRIC_MESH_MC_FAP_GROUP_SIZE) * SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH;

    SOCDNX_IF_ERR_EXIT(qax_mgmt_system_fap_id_get(unit, &my_core0_fap_id));

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));
    for (i = 0; i < destid_count; ++i) {
        if (destid_array[i] == my_core0_fap_id){
            mc_rep |= SOC_JER_FABRIC_MESH_MC_REPLICATION_LOCAL_0_BIT;
        } else { /*dest_id is dest FAP*/
            local_dest = SOC_DPP_FABRIC_GROUP_MODID_UNSET(destid_array[i]);
            if (local_dest == 0) { /*In Kalia, only modid_set 0 is supported.*/
                mc_rep |= SOC_JER_FABRIC_MESH_MC_REPLICATION_DEST_0_BIT ;
            } else {
                SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("dest %d is invalid"),destid_array[i]));
            }

        }
    }
    SHR_BITCOPY_RANGE(table_data, offset, &mc_rep, 0, SOC_JER_FABRIC_MESH_MC_REPLICATION_LENGTH);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, index, table_data));

exit:
  SOCDNX_FUNC_RETURN;
}



/*********************************************************************
* NAME:
*     soc_qax_fabric_link_config_ovrd
* FUNCTION:
*     Overwriting qax default fabric configuration in case of kalia
* INPUT:
*       int   unit - Identifier of the device to access.
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_fabric_link_config_ovrd(
    int                unit
  )
{

    SOCDNX_INIT_FUNC_DEFS;

    if (SOC_IS_KALIA(unit)) {
        SOC_DPP_DEFS_SET(unit, nof_fabric_links, 16);
        SOC_DPP_DEFS_SET(unit, nof_instances_fmac, 4);
        SOC_DPP_DEFS_SET(unit, first_fabric_link_id, 0);
    } 

/*exit:*/
  SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*     soc_qax_fabric_cosq_control_backward_flow_control_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 03 2015
* FUNCTION:
*     Enable / disable backwards flow control on supported fifos
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          port -
*     gport number.
*  SOC_SAND_IN int                                   enable -
*     Whether to enable / disable the feature.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*
* REMARKS:
*     Used in mbcm dispatcher.
*     For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_set(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_IN int                                   enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MESHMC_FC_ENr(unit, &reg_val));
        soc_reg_field_set(unit, FDA_FDA_MESHMC_FC_ENr, &reg_val, MESHMC_FC_ENf, enable);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_MESHMC_FC_ENr(unit, reg_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("backwards flow control for cosq egress fabric ucast / mcast / tdm fifos are not supported in QAX\n")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_fabric_cosq_control_backward_flow_control_get(
      SOC_SAND_IN int                                   unit,
      SOC_SAND_IN soc_gport_t                           port,
      SOC_SAND_OUT int                                  *enable,
      SOC_SAND_IN soc_dpp_cosq_gport_egress_core_fifo_t fifo_type
  )
{
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_MESHMC_FC_ENr(unit, &reg_val));
        *enable = soc_reg_field_get(unit, FDA_FDA_MESHMC_FC_ENr, reg_val, MESHMC_FC_ENf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_tdm:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("backwards flow control for cosq egress fabric ucast / mcast / tdm fifos are not supported in QAX\n")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*     soc_qax_fabric_egress_core_cosq_gport_sched_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 03 2015
* FUNCTION:
*     Set WFQ weight on supported fifos.
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          pipe -
*     Which pipe to access.
*  SOC_SAND_IN int                                  weight -
*     Weight value to configure.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*
* REMARKS:
*     Used in mbcm dispatcher.
*     For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type)
{
    soc_reg_above_64_val_t above_64_reg_val;
    const soc_field_t fields[3] = {EGQ_WFQ_WEIGHT_FABFIF_0f, EGQ_WFQ_WEIGHT_FABFIF_1f, EGQ_WFQ_WEIGHT_FABFIF_2f};

    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_generic_pipe:
        if ((pipe < 0) || (pipe >= (sizeof (fields)/sizeof(soc_field_t)))){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Generic pipe must be between 0 and %d (including)\n"), (sizeof (fields)/sizeof(soc_field_t)-1)));
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, fields[pipe], weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        soc_reg_above_64_field32_set(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_MESHMCf, weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_fabric_egress_core_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int                                *weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t   fifo_type
  )
{
    soc_reg_above_64_val_t above_64_reg_val;
    const soc_field_t fields[3] = {EGQ_WFQ_WEIGHT_FABFIF_0f, EGQ_WFQ_WEIGHT_FABFIF_1f, EGQ_WFQ_WEIGHT_FABFIF_2f};

    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_NULL_CHECK(weight);
    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_generic_pipe:
        if ((pipe < 0) || (pipe >= (sizeof (fields)/sizeof(soc_field_t)))){
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("Generic pipe must be between 0 and %d (including)\n"), (sizeof (fields)/sizeof(soc_field_t)-1)));
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, fields[pipe]);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_WFQr(unit, 0, above_64_reg_val));
        *weight = soc_reg_above_64_field32_get(unit, FDA_FDA_EGQ_WFQr, above_64_reg_val, EGQ_N_WFQ_WEIGHT_MESHMCf);
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("invalid argument fifo_type\n")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


/*********************************************************************
* NAME:
*     soc_qax_cosq_gport_sched_set/get
* FUNCTION:
*     Configuration of weight for WFQs in fabric pipes:
*     all, ingress, egress.
* INPUT:
*  SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  int                                pipe -
*     Which pipe's weight to configure (0,1,2)
*  SOC_SAND_IN/SOC_SAND_OUT  int/int*              weight -
*     value to configure/retrieve pipe's weight
*  SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type -
*     type of fabric pipe to configure (all, ingress, egress)
*     Note: egress is not legal argument for QAX. "All" argument is actually identical to "ingress" argument
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_cosq_gport_sched_set(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_IN  int                                weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  )
{

    soc_reg_above_64_val_t above_64_reg_val;
    soc_field_t field;

    SOCDNX_INIT_FUNC_DEFS;

    if (fabric_pipe_type != soc_dpp_cosq_gport_fabric_pipe_egress) { /*ingress or all*/
        /* configure ingress part (FDT) */
        switch (pipe) {
        case 0:
            field = IPT_0_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_0_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_0_WFQ_CTX_2_WEIGHTf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d\n"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_0_WFQ_CONFIGr(unit, above_64_reg_val));
        soc_reg_above_64_field32_set(unit, FDT_IPT_0_WFQ_CONFIGr, above_64_reg_val, field, weight);
        SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_0_WFQ_CONFIGr(unit, above_64_reg_val));
    }else{
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric_pipe_egress is not legal argument for QAX\n")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_cosq_gport_sched_get(
    SOC_SAND_IN  int                                unit,
    SOC_SAND_IN  int                                pipe,
    SOC_SAND_OUT int*                               weight,
    SOC_SAND_IN  soc_dpp_cosq_gport_fabric_pipe_t   fabric_pipe_type
  )
{
    soc_reg_above_64_val_t reg_above_64_val;
    soc_field_t field;

    SOCDNX_INIT_FUNC_DEFS;

    if (weight == NULL) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid address %#x\n"), unit ,weight));
    }

    if (fabric_pipe_type != soc_dpp_cosq_gport_fabric_pipe_egress) { /*ingress or all*/
        switch (pipe) {
        case 0:
            field = IPT_0_WFQ_CTX_0_WEIGHTf;
            break;
        case 1:
            field = IPT_0_WFQ_CTX_1_WEIGHTf;
            break;
        case 2:
            field = IPT_0_WFQ_CTX_2_WEIGHTf;
            break;
        default:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unit %d, invalid pipe index %d\n"), unit ,pipe));
            break;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_0_WFQ_CONFIGr(unit, reg_above_64_val));
        *weight = soc_reg_above_64_field32_get(unit, FDT_IPT_0_WFQ_CONFIGr, reg_above_64_val, field);
    } else {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric_pipe_egress is not legal argument for QAX\n")));
    }

exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     soc_qax_fabric_cosq_gport_priority_drop_threshold_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 10 2015
* FUNCTION:
*     Set priority drop threshold on supported fifos.
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          gport -
*     gport number.
*  SOC_SAND_IN  soc_cosq_threshold_t                *threshold_val -
*     sturuct which contains the threshold value
*     to configure / retreive.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*     NOTE: Only soc_dpp_cosq_gport_egress_core_fifo_local_mcast fifo_type is supported in QAX!
*
* REMARKS:
*     Used in mbcm dispatcher.
*     For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_set(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_IN  soc_cosq_threshold_t                   *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    int i;
    soc_field_t field;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        if (threshold->dp == 0) {
            field = EGQ_MESHMCFIF_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_MESHMCFIF_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_MESHMCFIF_PRIO_2_DROP_THRf;
        }
        for (i = 0; i < SOC_DPP_DEFS_GET(unit, nof_cores); ++i) {
            SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, i, &reg_val));
            soc_reg_field_set(unit, FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr, &reg_val, field, threshold->value);
            SOCDNX_IF_ERR_EXIT(WRITE_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, i, reg_val));
        }
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("priority drop threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast / ucast are not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

soc_error_t
  soc_qax_fabric_cosq_gport_priority_drop_threshold_get(
    SOC_SAND_IN  int                                    unit,
    SOC_SAND_IN  soc_gport_t                            gport,
    SOC_SAND_INOUT  soc_cosq_threshold_t                *threshold,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{
    soc_field_t field;
    uint32 reg_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_mcast:
        if (threshold->dp == 0) {
            field = EGQ_MESHMCFIF_PRIO_0_DROP_THRf;
        } else if (threshold->dp == 1) {
            field = EGQ_MESHMCFIF_PRIO_1_DROP_THRf;
        } else {
            field = EGQ_MESHMCFIF_PRIO_2_DROP_THRf;
        }
        SOCDNX_IF_ERR_EXIT(READ_FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr(unit, 0, &reg_val));
        threshold->value = soc_reg_field_get(unit, FDA_FDA_EGQ_MESHMC_PRIO_DROP_THRr, reg_val, field);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast:
            SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("priority drop threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_mcast / ucast are not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }
exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     soc_qax_fabric_cosq_gport_rci_threshold_set / get
* TYPE:
*   PROC
* DATE:
*   Dec 10 2015
* FUNCTION:
*     Set / get rci threshold on supported fifos.
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN soc_gport_t                          gport -
*     gport number.
*  SOC_SAND_IN  int                                 threshold_val -
*     Threshold value to configure.
*  SOC_SAND_IN int                                  fifo_type -
*     Type of fifo to configure
*     Note: For QAX, only soc_dpp_cosq_gport_egress_core_fifo_local_ucast is supported.
*
* REMARKS:
*     Used in mbcm dispatcher.
*     For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_set(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_IN  int                    threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{

    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_ucast:
        SOCDNX_IF_ERR_EXIT(READ_TXQ_LOCAL_FIFO_CFGr(unit, reg_above_64_val));
        soc_reg_above_64_field32_set(unit, TXQ_LOCAL_FIFO_CFGr, reg_above_64_val, SRAM_DTQ_LOC_GEN_RCI_THf, threshold_val);
        SOCDNX_IF_ERR_EXIT(WRITE_TXQ_LOCAL_FIFO_CFGr(unit, reg_above_64_val));
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("rci threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast is not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
  soc_qax_fabric_cosq_gport_rci_threshold_get(
    SOC_SAND_IN  int                    unit,
    SOC_SAND_IN  soc_gport_t            gport,
    SOC_SAND_OUT  int                    *threshold_val,
    SOC_SAND_IN  soc_dpp_cosq_gport_egress_core_fifo_t  fifo_type
  )
{

    soc_reg_above_64_val_t reg_above_64_val;
    SOCDNX_INIT_FUNC_DEFS;

    switch (fifo_type) {
    case soc_dpp_cosq_gport_egress_core_fifo_local_ucast:
        SOCDNX_IF_ERR_EXIT(READ_TXQ_LOCAL_FIFO_CFGr(unit, reg_above_64_val));
        *threshold_val = soc_reg_above_64_field32_get(unit, TXQ_LOCAL_FIFO_CFGr, reg_above_64_val, SRAM_DTQ_LOC_GEN_RCI_THf);
        break;
    case soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("rci threshold for soc_dpp_cosq_gport_egress_core_fifo_fabric_ucast is not supported in QAX")));
        break;
    default:
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("unsupported fifo type")));
        break;
    }

exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     soc_qax_fabric_priority_bits_mapping_to_fdt_index_get
* TYPE:
*   PROC
* DATE:
*   Dec 13 2015
* FUNCTION:
*     configure cell attributes(is_hp, tc, dp, is_mc)
*     to an index in TXQ_PRIORITY_BITS_MAPPING_2_FDT table
* INPUT:
*  SOC_SAND_IN  int                                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                             tc -
*     traffic class
*  SOC_SAND_IN  uint32                             dp -
*     drop precedence
*  SOC_SAND_IN  uint32                              flags -
*     relevant flags for cell (is_mc, is_hp)
* OUTPUT:
*  SOC_SAND_OUT uint32                              *index
*     retrieved entry index to TXQ_PRIORITY_BITS_MAPPING_2_FDT
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   STATIC function
*   For Kalia only (not relevant for QAX)
*********************************************************************/

STATIC soc_error_t
soc_qax_fabric_priority_bits_mapping_to_fdt_index_get(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  uint32                     tc,
    SOC_SAND_IN  uint32                     dp,
    SOC_SAND_IN  uint32                     flags,
    SOC_SAND_OUT uint32                     *index
  )
{
    uint32 is_mc  = 0, is_hp = 0;
    SOCDNX_INIT_FUNC_DEFS;

    *index = 0;

    is_hp  = (flags & SOC_FABRIC_QUEUE_PRIORITY_HIGH_ONLY) ? 1 : 0 ;
    is_mc  = (flags & SOC_FABRIC_PRIORITY_MULTICAST)       ? 1 : 0 ;

    *index  |=  ((is_hp     << SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_OFFSET)  & SOC_QAX_FABRIC_PRIORITY_NDX_IS_HP_MASK  )|
                ((tc        << SOC_QAX_FABRIC_PRIORITY_NDX_TC_OFFSET)     & SOC_QAX_FABRIC_PRIORITY_NDX_TC_MASK     )|
                ((dp        << SOC_QAX_FABRIC_PRIORITY_NDX_DP_OFFSET)     & SOC_QAX_FABRIC_PRIORITY_NDX_DP_MASK     )|
                ((is_mc     << SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_OFFSET)  & SOC_QAX_FABRIC_PRIORITY_NDX_IS_MC_MASK  );

    SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     soc_qax_fabric_priority_set / set
* TYPE:
*   PROC
* DATE:
*   Dec 13 2015
* FUNCTION:
*     Set / Get fabric priority according to:
*     traffic_class, queue_type: hc/lc (flags), dp(color).
* INPUT:
*  SOC_SAND_IN  int                                unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                             tc -
*     traffic class
*  SOC_SAND_IN  uint32                             dp -
*     drop precedence
*  SOC_SAND_IN  uint32                             flags -
*     relevant flags for cell (is_mc, is_hp)
*  SOC_SAND_IN/OUT   int/int*                      fabric_priority -
*     fabric priority to set/ get in TXQ_PRIORITY_BITS_MAPPING_2_FDT
* RETURNS:
*   OK or ERROR indication.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/

soc_error_t
soc_qax_fabric_priority_set(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_IN  int                fabric_priority
  )
{
    uint32 index;
    uint32 tdm_min_prio;
    SOCDNX_INIT_FUNC_DEFS;

    tdm_min_prio = SOC_DPP_CONFIG(unit)->arad->init.fabric.fabric_tdm_priority_min;
    /*validate fabric_priority*/
    if ((fabric_priority < 0) || (fabric_priority >= tdm_min_prio)) {
        SOCDNX_EXIT_WITH_ERR(SOC_E_PARAM, (_BSL_SOC_MSG("fabric PKT priority is %d but must be between 0 and TDM min priority (that was set to to %d) \n"), fabric_priority, tdm_min_prio));
    }
    /*get index for TXQ_PRIORITY_BITS_MAPPING_2_FDT*/
    SOCDNX_IF_ERR_EXIT(soc_qax_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index));
    /*fill table with fabric priority in the index found*/
    SOCDNX_IF_ERR_EXIT(dcmn_fill_partial_table_with_entry(unit, TXQ_PRIORITY_BITS_MAPPING_2_FDTm,
                                                          0, 0, MEM_BLOCK_ALL, index, index, &fabric_priority));

exit:
  SOCDNX_FUNC_RETURN;
}


soc_error_t
soc_qax_fabric_priority_get(
    SOC_SAND_IN  int                unit,
    SOC_SAND_IN  uint32             tc,
    SOC_SAND_IN  uint32             dp,
    SOC_SAND_IN  uint32             flags,
    SOC_SAND_OUT  int                *fabric_priority
  )
{
    uint32 index, val = 0;
    SOCDNX_INIT_FUNC_DEFS;

    /*get index for TXQ_PRIORITY_BITS_MAPPING_2_FDT*/
    SOCDNX_IF_ERR_EXIT(soc_qax_fabric_priority_bits_mapping_to_fdt_index_get(unit, tc, dp, flags, &index));
    /*retrieve table entry in the index found*/
    SOCDNX_IF_ERR_EXIT(READ_TXQ_PRIORITY_BITS_MAPPING_2_FDTm(unit, MEM_BLOCK_ANY, index, &val));
    *fabric_priority = val;

exit:
  SOCDNX_FUNC_RETURN;
}

/*********************************************************************
* NAME:
*     qax_fabric_pcp_dest_mode_config_set
* TYPE:
*   PROC
* DATE:
*   Dec 16 2015
* FUNCTION:
*     Enables set / get operations on fabric-pcp (packet cell packing)
*     per destination device.
*     there are three supported pcp modes:
*       - 0- No Packing
*       - 1- Simple Packing
*       - 2- Continuous Packing
* INPUT:
*  SOC_SAND_IN  int                         unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                      flags-
*  SOC_SAND_IN  uint32                  	modid-
*     Id of destination device			
*  SOC_SAND_IN/OUT unit32/uint32*     		pcp_mode-
*     mode of pcp to set/get.
* REMARKS:
*   Used in mbcm dispatcher.
*   For Kalia only (not relevant for QAX)
*********************************************************************/
soc_error_t
  qax_fabric_pcp_dest_mode_config_set(
    SOC_SAND_IN int              unit,
    SOC_SAND_IN uint32           flags,
    SOC_SAND_IN uint32           modid,
    SOC_SAND_IN uint32           pcp_mode
  )
{
    SHR_BITDCLNAME (fdt_data, 137);
    SOCDNX_INIT_FUNC_DEFS;

    SOCDNX_IF_ERR_EXIT(READ_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ANY, modid, fdt_data));
    SHR_BITCOPY_RANGE(fdt_data, 6, &pcp_mode, 0, 2);
    SOCDNX_IF_ERR_EXIT(WRITE_FDT_IPT_MESH_MCm(unit, MEM_BLOCK_ALL, modid, fdt_data));


exit:
    SOCDNX_FUNC_RETURN;
}

#undef _ERR_MSG_MODULE_NAME

#include <soc/dpp/SAND/Utils/sand_footer.h>
