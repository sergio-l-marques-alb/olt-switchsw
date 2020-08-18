/** \file stif_apis.c
 * $Id$
 *  stif module gather all functionality that related to the
 *  statistic interface driver for DNX. stif_api.c
 *  holds all the STIF APIs implemantation for DNX It's matching
 *  h file is bcm/stat.h.
 * 
 */

/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */
#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_STAT

/*
 * Include files.
 * {
 */
#include <shared/utilex/utilex_bitstream.h>
#include <shared/shrextend/shrextend_error.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm/types.h>
#include <bcm/error.h>
#include <bcm/stat.h>
#include <bcm/port.h>
#include <bcm_int/dnx/port/port.h>
#include <bcm_int/dnx_dispatch.h>
#include <include/bcm_int/dnx/stat/stif/stif_mgmt.h>
#include <include/bcm_int/dnx/stk/stk_sys.h>
#include <soc/dnx/dbal/dbal.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_device.h>
#include <soc/dnx/dnx_data/auto_generated/dnx_data_stif.h>
#include <include/bcm_int/dnx/cmn/dnxcmn.h>
#include <include/bcm_int/dnx/algo/port/algo_port_mgmt.h>
#include <include/bcm_int/dnx/algo/port/algo_port_utils.h>

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

/**
* \brief  
*   verify bcm_stat_stif_source_t structure
* \param [in] unit       -  unit ID
* \param [in] flags     -  verify set or get API
* \param [in] source  -  stif source type and core
* \return 
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
dnx_stat_stif_source_verify(
    int unit,
    int flags,
    bcm_stat_stif_source_t source)
{
    SHR_FUNC_INIT_VARS(unit);
    /** check if stif is enabled */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not enabled, use soc property stat_if_enable to enable it.\n");
    }
    /** source verification */
    DNXCMN_CORE_VALIDATE(unit, source.core, TRUE);
    if (source.src_type < bcmStatStifSourceIngressEnqueue || source.src_type > bcmStatStifSourceEgressDequeue)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  stif source type (=%d)\n", source.src_type);
    }
    /** check the report mode */
    if ((dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_qsize ||
         dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_billing_ingress)
        && source.src_type == bcmStatStifSourceEgressDequeue)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  stif source type (=%d) - not supported for report mode QSIZE / INGRESS BILLING \n",
                     source.src_type);
    }
    if (dnx_data_stif.config.stif_report_mode_get(unit) != dnx_stat_stif_mgmt_report_mode_qsize
        && source.src_type == bcmStatStifSourceIngressScrubber)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  stif source type (=%d) - not supported for report mode BILLING/BILLING_INGRESS \n",
                     source.src_type);
    }
    if (dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_billing
        && source.src_type == bcmStatStifSourceIngressDequeue)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  stif source type (=%d) - not supported for report mode BILLING \n", source.src_type);
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   verify params for API bcm_stat_stif_source_mapping_set
* \param [in] unit    -  unit ID
* \param [in] flags   -  verify set or get API
* \param [in] source  -  stif source type and core 
* \param [in] port - logical port 
* \return 
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
dnx_stat_stif_source_mapping_source_mapping_set_verify(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t port)
{
    int port_type;
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_stat_stif_source_verify(unit, flags, source));
    /** Verify port - invalid port is also allowed meaning the source will be disconnected */
    if (port != BCM_PORT_INVALID)
    {
        /** get port type */
        SHR_IF_ERR_EXIT(dnx_algo_port_type_get(unit, port, &port_type));
        if (DNX_ALGO_PORT_TYPE_IS_NIF_ETH_STIF(unit, port_type) == FALSE)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  port (=%d) - not configured for stif \n", port);
        }
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   verify params for API bcm_stat_stif_source_mapping_get
* \param [in] unit    -  unit ID
* \param [in] flags   -  verify set or get API
* \param [in] source  -  stif source type and core 
* \param [in] port - logical port 
* \return 
*   \retval Non-zero (!= _SHR_E_NONE) in case of an error
*   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
dnx_stat_stif_source_mapping_source_mapping_get_verify(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t * port)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_IF_ERR_EXIT(dnx_stat_stif_source_verify(unit, flags, source));
    SHR_NULL_CHECK(port, _SHR_E_PARAM, "port");
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API map statistics source (statistics interface source) to logical
*  port. port=invalid means disconnect the source.
* \param [in] unit    -  unit ID
* \param [in] flags   -  flags
* \param [in] source  -  stif source type and core
* \param [in] port    -  logical port 
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_stif_source_mapping_set(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t port)
{
    int is_traffic_enabled, is_port_enabled, core_id;
    bcm_stat_stif_source_t source_local;
    SHR_FUNC_INIT_VARS(unit);
    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_stif_source_mapping_source_mapping_set_verify(unit, flags, source, port));
    /** Verify API is called before traffic is enabled */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &is_traffic_enabled));
    if (is_traffic_enabled == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "API bcm_stat_stif_source_mapping_set must be called before traffic is enabled \n");
    }

    SHR_IF_ERR_EXIT(bcm_dnx_port_enable_get(unit, port, &is_port_enabled));
    if (is_port_enabled)
    {
        /** Disable the port in order to clean SIF controller queue */
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, port, FALSE));
    }

    DNXCMN_CORES_ITER(unit, source.core, core_id)
    {
        source_local.core = core_id;
        source_local.src_type = source.src_type;
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_mapping_set(unit, flags, source_local, port));
    }

    if (is_port_enabled)
    {
        /** Enable the port  */
        SHR_IF_ERR_EXIT(bcm_dnx_port_enable_set(unit, port, TRUE));
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API get the mapping of statistics source to its logical port.
* port=invalid means that the source is disconnected.
* \param [in] unit    -  unit ID
* \param [in] flags   -  flags
* \param [in] source  -  stif source type and core
* \param [out] port    -  logical port 
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_stif_source_mapping_get(
    int unit,
    int flags,
    bcm_stat_stif_source_t source,
    bcm_port_t * port)
{
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(dnx_stat_stif_source_mapping_source_mapping_get_verify(unit, flags, source, port));
    /** for get API bcm_core_all is not supported and will be
     *  considered as core=0 */
    if (source.core == BCM_CORE_ALL)
    {
        source.core = 0;
    }
    SHR_IF_ERR_EXIT(dnx_stif_mgmt_source_mapping_get(unit, flags, source, port, NULL, NULL));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   verify record format - stif enabled, billing mode, flags
* \param [in] unit          -  unit ID
* \param [in] flags         -  BCM_STAT_INGRESS, BCM_STAT_EGRESS 
* \param [in] nof_elements - nof elements
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
bcm_dnx_stat_stif_record_format_verify(
    int unit,
    int flags,
    int nof_elements)
{

    SHR_FUNC_INIT_VARS(unit);
    /** check if stif is enabled */
    if (dnx_data_stif.config.feature_get(unit, dnx_data_stif_config_stif_enable) == FALSE)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not enabled, use soc property stat_if_enable to enable it.\n");
    }
    /** check if stif is in billing mode */
    if (dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_qsize)
    {
        SHR_ERR_EXIT(_SHR_E_DISABLED,
                     "Statistic Interface is not configured in mode BILLING, bcm_stat_stif_record_format_set_verify is for billing mode only. \n");
    }
    if (dnx_data_stif.config.stif_report_mode_get(unit) == dnx_stat_stif_mgmt_report_mode_billing_ingress
        && (flags == BCM_STAT_EGRESS))
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  flags (=%d) - supported flag for BILLING INGRESS mode is BCM_STAT_INGRESS only  \n",
                     flags);
    }
    /** flags verify */
    if (flags != BCM_STAT_INGRESS && flags != BCM_STAT_EGRESS)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM,
                     "invalid  flags (=%d) - supported is flag - BCM_STAT_INGRESS or BCM_STAT_EGRESS, not both together \n",
                     flags);
    }
    /** nof_elements verify */
    if (nof_elements < 0 || nof_elements > bcmStatStifRecordElementEgressMetaDataObj3)
    {
        SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  nof elements (=%d)\n", nof_elements);
    }
exit:
    SHR_FUNC_EXIT;
}
/**
* \brief  
*   verify record format element
* \param [in] unit          -  unit ID
* \param [in] flags         -  BCM_STAT_INGRESS, BCM_STAT_EGRESS 
* \param [in] nof_elements - nof elements 
* \param [in] record_format_elements    -  format element to be 
*        verified
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
bcm_dnx_stat_stif_record_format_element_verify(
    int unit,
    int flags,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements)
{
    int element;
    SHR_FUNC_INIT_VARS(unit);

    /** due to HW limitation, in 96 bits ingress enqueue record size - the record must contain at least one PP object. */
    /** therefore, we force the user to give bcmStatStifRecordElementObj0 as the first element */
    if ((flags == BCM_STAT_INGRESS)
        && (dnx_data_stif.config.stif_report_size_ingress_get(unit) == DBAL_ENUM_FVAL_STIF_BILLING_REPORT_SIZE_96_BITS))
    {
        if (record_format_elements[0].element_type != bcmStatStifRecordElementObj0)
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "For ingress enqueue record size=96 bits, bcmStatStifRecordElementObj0 must be the first element. \n");
        }
    }

    /**verify record_format_element */
    for (element = 0; element < nof_elements; element++)
    {
        /** elements order verification */
        /** PackeSize should always be provided */
        if ((element == nof_elements - 1)
            && (record_format_elements[element].element_type != bcmStatStifRecordElementPacketSize))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "invalid configuration - bcmStatStifRecordElementPacketSize must always be the last element in the array. \n");
        }
        if ((element != 0) && (record_format_elements[element].element_type == bcmStatStifRecordElementObj0))
        {
            SHR_ERR_EXIT(_SHR_E_PARAM,
                         "invalid configuration - if bcmStatStifRecordElementObj0 is set - it must be the first element in the array. \n");
        }
        /** elements order for EGRESS */
        if (flags == BCM_STAT_EGRESS)
        {
            /** Opcode (if exists) should be after the Metadata and
             *  before the Packetsize(Packetsize is the last element) */
            if ((element != nof_elements - 2)
                && (record_format_elements[element].element_type == bcmStatStifRecordElementOpCode))
            {
                SHR_ERR_EXIT(_SHR_E_PARAM,
                             "invalid configuration - if bcmStatStifRecordElementOpCode is set - it must be the last element in the array before the Packetsize. \n");
            }
        }
        /** object elements order */
        switch (record_format_elements[element].element_type)
        {
            case bcmStatStifRecordElementObj0:
            {
                if (record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj0
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj2
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj3)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid configuration - if bcmStatStifRecordElementObj0 is set - the next element cannot be Obj0,Obj2 or Obj3. \n");
                }
                break;
            }
            case bcmStatStifRecordElementObj1:
            {
                if (record_format_elements[element - 1].element_type != bcmStatStifRecordElementObj0
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj0
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj1
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj3)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid configuration - if bcmStatExternalRecordElementObj1 is set - previous element should be Obj0 and the next element cannot be Obj0 or Obj1 or Obj3. \n");
                }
                break;
            }
            case bcmStatStifRecordElementObj2:
            {
                if (record_format_elements[element - 1].element_type != bcmStatStifRecordElementObj1
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj0
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj1
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj2)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid configuration - if bcmStatExternalRecordElementObj2 is set - previous element should be Obj1 and the next element cannot be Obj0 or Obj1 or Obj2. \n");
                }
                break;
            }
            case bcmStatStifRecordElementObj3:
            {
                if (record_format_elements[element - 1].element_type != bcmStatStifRecordElementObj2
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj0
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj1
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj2
                    || record_format_elements[element + 1].element_type == bcmStatStifRecordElementObj3)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid configuration - if bcmStatExternalRecordElementObj3 is set - previous element should be Obj2 and the next element cannot be Obj0 or Obj1 or Obj2 or Obj3. \n");
                }
                break;
            }
            default:
            {
                break;
            }
        }
        /** mask value verification */
        switch (record_format_elements[element].element_type)
        {
            case bcmStatStifRecordElementObj0:
            case bcmStatStifRecordElementObj1:
            case bcmStatStifRecordElementObj2:
            case bcmStatStifRecordElementObj3:
            {
                if (flags & BCM_STAT_INGRESS)
                {
                    if (record_format_elements[element].mask != BCM_STAT_FULL_MASK)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "invalid  record format mask (=%d) for element type (=%d) - mask must be (=%d)\n",
                                     record_format_elements[element].mask, record_format_elements[element].element_type,
                                     BCM_STAT_FULL_MASK);
                    }
                }
                else
                {
                    /** for EGRESS stat objects can have sizes 16/18/20 - user
                     *  knows the width of each object has to mask it accordingly
                     *  in order to now internally how to shift the objects
                     *  depending on their size */
                    if (record_format_elements[element].mask != BCM_STAT_FULL_MASK &&
                        record_format_elements[element].mask != DNX_STIF_MGMT_EGRESS_STAT_OBJ_MASK_16_BITS &&
                        record_format_elements[element].mask != DNX_STIF_MGMT_EGRESS_STAT_OBJ_MASK_18_BITS)
                    {
                        SHR_ERR_EXIT(_SHR_E_PARAM,
                                     "invalid  record format mask (=%d) for element type (=%d) - mask can be (=%d; =%d; =%d)\n",
                                     record_format_elements[element].mask, record_format_elements[element].element_type,
                                     BCM_STAT_FULL_MASK, DNX_STIF_MGMT_EGRESS_STAT_OBJ_MASK_16_BITS,
                                     DNX_STIF_MGMT_EGRESS_STAT_OBJ_MASK_18_BITS);
                    }
                }
                break;
            }
            case bcmStatStifRecordElementPacketSize:
            {
                if (record_format_elements[element].mask != BCM_STAT_FULL_MASK)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid  record format mask (=%d) for element type (=%d) - mask must be (=%d)\n",
                                 record_format_elements[element].mask, record_format_elements[element].element_type,
                                 BCM_STAT_FULL_MASK);
                }
                break;
            }
            case bcmStatStifRecordElementOpCode:
            {
                if ((flags & BCM_STAT_INGRESS) && record_format_elements[element].mask != BCM_STAT_FULL_MASK)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid  record format mask (=%d) for element type (=%d) - mask must be (=%d)\n",
                                 record_format_elements[element].mask, record_format_elements[element].element_type,
                                 BCM_STAT_FULL_MASK);
                }
                if ((flags & BCM_STAT_EGRESS)
                    && (record_format_elements[element].mask !=
                        dnx_data_stif.report.billing_opsize_use_one_type_mask_get(unit)
                        && record_format_elements[element].mask !=
                        dnx_data_stif.report.billing_opsize_use_two_types_mask_get(unit)
                        && record_format_elements[element].mask !=
                        dnx_data_stif.report.billing_opsize_use_three_types_mask_get(unit)
                        && record_format_elements[element].mask !=
                        dnx_data_stif.report.billing_opsize_use_four_types_mask_get(unit)))
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid  record format mask (=%d) for element type (=%d) - allowed masks are %d, %d, %d, %d\n",
                                 record_format_elements[element].mask, record_format_elements[element].element_type,
                                 dnx_data_stif.report.billing_opsize_use_one_type_mask_get(unit),
                                 dnx_data_stif.report.billing_opsize_use_two_types_mask_get(unit),
                                 dnx_data_stif.report.billing_opsize_use_three_types_mask_get(unit),
                                 dnx_data_stif.report.billing_opsize_use_four_types_mask_get(unit));
                }
                break;
            }
            case bcmStatStifRecordElementIngressDispositionIsDrop:
            case bcmStatStifRecordElementIngressTmDropReason:
            case bcmStatStifRecordElementIngressTrafficClass:
            case bcmStatStifRecordElementIngressIncomingDropPrecedence:
            case bcmStatStifRecordElementIngressDropPrecedenceMeterResolved:
            case bcmStatStifRecordElementIngressDropPrecedenceMeter0Valid:
            case bcmStatStifRecordElementIngressDropPrecedenceMeter0Value:
            case bcmStatStifRecordElementIngressDropPrecedenceMeter1Valid:
            case bcmStatStifRecordElementIngressDropPrecedenceMeter1Value:
            case bcmStatStifRecordElementIngressDropPrecedenceMeter2Valid:
            case bcmStatStifRecordElementIngressDropPrecedenceMeter2Value:
            case bcmStatStifRecordElementIngressCore:
            case bcmStatStifRecordElementIngressPpMetaData:
            case bcmStatStifRecordElementIngressQueueNumber:
            {
                if (flags & BCM_STAT_EGRESS)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  record element type for Egress (=%d)\n",
                                 record_format_elements[element].element_type);
                }
                break;
            }
            case bcmStatStifRecordElementEgressMetaDataMultiCast:
            case bcmStatStifRecordElementEgressMetaDataPpDropReason:
            case bcmStatStifRecordElementEgressMetaDataEcnEligibleAndCni:
            case bcmStatStifRecordElementEgressMetaDataTrafficClass:
            case bcmStatStifRecordElementEgressMetaDataDropPrecedence:
            case bcmStatStifRecordElementEgressMetaDataObj0:
            case bcmStatStifRecordElementEgressMetaDataObj1:
            case bcmStatStifRecordElementEgressMetaDataObj2:
            case bcmStatStifRecordElementEgressMetaDataObj3:
            case bcmStatStifRecordElementEgressMetaDataCore:
            {
                if (flags & BCM_STAT_INGRESS)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  record element type for Ingress (=%d)\n",
                                 record_format_elements[element].element_type);
                }
                if (record_format_elements[element].mask != BCM_STAT_FULL_MASK)
                {
                    SHR_ERR_EXIT(_SHR_E_PARAM,
                                 "invalid  record format mask (=%d) for element type (=%d) - mask must be (=%d)\n",
                                 record_format_elements[element].mask, record_format_elements[element].element_type,
                                 BCM_STAT_FULL_MASK);
                }
                break;
            }
            default:
            {
                SHR_ERR_EXIT(_SHR_E_PARAM, "invalid  record format element type (=%d)\n",
                             record_format_elements[element].element_type);
            }
        }
    }
exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   verify params for API bcm_stat_stif_record_format_set
* \param [in] unit          -  unit ID
* \param [in] flags         -  BCM_STAT_INGRESS, BCM_STAT_EGRESS
* \param [in] nof_elements  -  nof of elements set in the format
* \param [in] record_format_elements    -  format elements 
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
bcm_dnx_stat_stif_record_format_set_verify(
    int unit,
    int flags,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements)
{
    SHR_FUNC_INIT_VARS(unit);
    /**verify stif billing mode and correct flags */
    SHR_IF_ERR_EXIT(bcm_dnx_stat_stif_record_format_verify(unit, flags, nof_elements));
    /**verify record_format_elements */
    SHR_IF_ERR_EXIT(bcm_dnx_stat_stif_record_format_element_verify(unit, flags, nof_elements, record_format_elements));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API builds the format of the billing records - ingress
*   /egress
* \param [in] unit          -  unit ID
* \param [in] flags         -  BCM_STAT_INGRESS, BCM_STAT_EGRESS
* \param [in] nof_elements  -  nof of elements set in the format
* \param [in] record_format_elements    -  format elements 
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_stif_record_format_set(
    int unit,
    int flags,
    int nof_elements,
    bcm_stat_stif_record_format_element_t * record_format_elements)
{
    int is_traffic_enabled;
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(bcm_dnx_stat_stif_record_format_set_verify
                          (unit, flags, nof_elements, record_format_elements));
    /** Verify API is called before traffic is enabled */
    SHR_IF_ERR_EXIT(dnx_stk_sys_traffic_enable_get(unit, &is_traffic_enabled));
    if (is_traffic_enabled == TRUE)
    {
        SHR_ERR_EXIT(_SHR_E_UNAVAIL,
                     "API bcm_stat_stif_source_mapping_set must be called before traffic is enabled \n");
    }
    if (flags == BCM_STAT_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_ingress_record_format_set(unit, nof_elements, record_format_elements));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_egress_record_format_set(unit, nof_elements, record_format_elements));
    }

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   verify params API bcm_stat_stif_record_format_get
* \param [in] unit          -  unit ID
* \param [in] flags         -  BCM_STAT_INGRESS, BCM_STAT_EGRESS
* \param [in] max_nof_elements  -  max nof of elements
* \param [out] elements_array   -  format elements 
* \param [out] nof_elements     -  nof elements in the array
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
static shr_error_e
bcm_dnx_stat_stif_record_format_get_verify(
    int unit,
    int flags,
    int max_nof_elements,
    bcm_stat_stif_record_format_element_t * elements_array,
    int *nof_elements)
{
    SHR_FUNC_INIT_VARS(unit);
    SHR_NULL_CHECK(elements_array, _SHR_E_PARAM, "elements_array");
    SHR_NULL_CHECK(nof_elements, _SHR_E_PARAM, "nof_elements");
    /**verify stif billing mode and correct flags */
    SHR_IF_ERR_EXIT(bcm_dnx_stat_stif_record_format_verify(unit, flags, max_nof_elements));

exit:
    SHR_FUNC_EXIT;
}

/**
* \brief  
*   API get the format of the billing records - ingress
*   /egress
* \param [in] unit          -  unit ID
* \param [in] flags         -  BCM_STAT_INGRESS, BCM_STAT_EGRESS
* \param [in] max_nof_elements  -  max nof of elements
* \param [out] elements_array   -  format elements 
* \param [out] nof_elements     -  nof elements in the array
* \return 
*   shr_error_e - Error Type 
* \remark 
*   * 
* \see
*   * None
*/
int
bcm_dnx_stat_stif_record_format_get(
    int unit,
    int flags,
    int max_nof_elements,
    bcm_stat_stif_record_format_element_t * elements_array,
    int *nof_elements)
{
    int elements_size_array[DNX_STIF_INGRESS_MAX_RECORD_ELEMENTS + DNX_STIF_RECORD_MAX_NOF_OBJECT_ELEMENTS + 1] = { 0 };
    SHR_FUNC_INIT_VARS(unit);

    /** Verify parameters */
    SHR_INVOKE_VERIFY_DNX(bcm_dnx_stat_stif_record_format_get_verify
                          (unit, flags, max_nof_elements, elements_array, nof_elements));
    if (flags == BCM_STAT_INGRESS)
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_ingress_record_format_get
                        (unit, max_nof_elements, elements_array, nof_elements, elements_size_array));
    }
    else
    {
        SHR_IF_ERR_EXIT(dnx_stif_mgmt_egress_record_format_get
                        (unit, max_nof_elements, elements_array, nof_elements, elements_size_array));
    }

exit:
    SHR_FUNC_EXIT;
}
