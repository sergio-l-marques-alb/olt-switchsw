#include <shared/bsl.h>

#include <soc/mcm/memregs.h>

/*
 * $Id: jer2_arad_tbl_access.c,v 1.186 Broadcom SDK $
 *
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 *
 */


#ifdef BSL_LOG_MODULE
  #error "BSL_LOG_MODULE redefined"
#endif

#define BSL_LOG_MODULE BSL_LS_SOCDNX_MEM

/*************
 * INCLUDES  *
 *************/
/* { */
#include <shared/shrextend/shrextend_debug.h>
#include <soc/dnxc/legacy/error.h>

#include <soc/mem.h>
#include <soc/error.h>

#include <soc/dnx/legacy/ARAD/arad_tbl_access.h>
#include <soc/dnx/legacy/ARAD/arad_chip_tbls.h>
#include <soc/dnx/legacy/ARAD/arad_reg_access.h>
#include <soc/dnx/legacy/ARAD/arad_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/SAND/Utils/sand_os_interface.h>
#include <soc/dnx/legacy/drv.h> /* for assert() */
#include <soc/dnx/legacy/mbcm.h> 
#include <soc/sand/sand_mem.h> 
#include <soc/dnxc/legacy/utils.h>
#include <soc/hwstate/hw_log.h>
#include <shared/swstate/access/sw_state_access.h>


#include <soc/dnx/dnx_data/auto_generated/dnx_data_egr_queuing.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define JER2_ARAD_WRED_NOF_DROP_PRECEDENCE   (JER2_ARAD_NOF_DROP_PRECEDENCE)


#define JER2_ARAD_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE 1


#define JER2_ARAD_IHB_TCAM_HIT_INDICATION_NOF_TCAM_ENTRIES_PER_LINE  (8)
#define JER2_ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_BIT(line)    (line % JER2_ARAD_IHB_TCAM_HIT_INDICATION_NOF_TCAM_ENTRIES_PER_LINE)
#define JER2_ARAD_IHB_TCAM_HIT_INDICATION_TCAM_ENTRY_TO_LINE(line)   (line / JER2_ARAD_IHB_TCAM_HIT_INDICATION_NOF_TCAM_ENTRIES_PER_LINE)

#define JER2_ARAD_IRE_NIF_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define JER2_ARAD_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL_ENTRY_SIZE                                   7
#define JER2_ARAD_IRE_RCY_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define JER2_ARAD_IRE_CPU_CTXT_MAP_TBL_ENTRY_SIZE                                               1
#define JER2_ARAD_IRE_TDM_CONFIG_TBL_ENTRY_SIZE                                                 5
#define JER2_ARAD_IDR_CONTEXT_MRU_TBL_ENTRY_SIZE                                                2
#define JER2_ARAD_IRR_SNOOP_MIRROR_TABLE0_TBL_ENTRY_SIZE                                        2
#define JER2_ARAD_IRR_SNOOP_MIRROR_TABLE1_TBL_ENTRY_SIZE                                        4
#define JER2_ARAD_IRR_DESTINATION_TABLE_TBL_ENTRY_SIZE                                          1
#define JER2_ARAD_IRR_LAG_TO_LAG_RANGE_TBL_ENTRY_SIZE                                           1
#define JER2_ARAD_IRR_LAG_MAPPING_TBL_ENTRY_SIZE                                                1
#define JER2_ARAD_IRR_SMOOTH_DIVISION_TBL_ENTRY_SIZE                                            1
#define JER2_ARAD_IRR_TRAFFIC_CLASS_MAPPING_TBL_ENTRY_SIZE                                      1
#define JER2_ARAD_IRR_STACK_TRUNK_RESOLVE_TBL_ENTRY_SIZE                                        1
#define JER2_ARAD_IRR_STACK_FEC_RESOLVE_TBL_ENTRY_SIZE                                          1
#define JER2_ARAD_IHP_TM_PORT_SYS_PORT_CONFIG_TBL_ENTRY_SIZE                                    2
#define JER2_ARAD_QDR_QDR_DLL_MEM_TBL_ENTRY_SIZE                                                1
#define JER2_ARAD_IQM_STATIC_TBL_ENTRY_SIZE                                                     1
#define JER2_ARAD_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL_ENTRY_SIZE                              1
#define JER2_ARAD_IQM_CREDIT_DISCOUNT_TABLE_TBL_ENTRY_SIZE                                      1
#define JER2_ARAD_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                          4
#define JER2_ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL_ENTRY_SIZE                          1
#define JER2_ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL_ENTRY_SIZE                          1
#define JER2_ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL_ENTRY_SIZE                          1
#define JER2_ARAD_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL_ENTRY_SIZE                          1
#define JER2_ARAD_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_A_TBL_ENTRY_SIZE                         3
#define JER2_ARAD_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL_ENTRY_SIZE                                1
#define JER2_ARAD_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL_ENTRY_SIZE                         1
#define JER2_ARAD_IQM_SYSTEM_RED_TBL_ENTRY_SIZE                                                 6
#define JER2_ARAD_IQM_CNM_PARAMETERS_TABLE_TBL_ENTRY_SIZE                                       5
#define JER2_ARAD_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                          2
#define JER2_ARAD_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL_ENTRY_SIZE                   2
#define JER2_ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_ENTRY_SIZE                                       1
#define JER2_ARAD_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL_ENTRY_SIZE                                    1
#define JER2_ARAD_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL_ENTRY_SIZE                                  1
#define JER2_ARAD_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL_ENTRY_SIZE                                  2
#define JER2_ARAD_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                          2
#define JER2_ARAD_IPS_SLOW_FACTOR_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                               6
#define JER2_ARAD_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                      2
#define JER2_ARAD_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL_ENTRY_SIZE                           2
#define JER2_ARAD_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL_ENTRY_SIZE                           1
#define JER2_ARAD_EGQ_CCM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_EGQ_PMC_TBL_ENTRY_SIZE                                                        3
#define JER2_ARAD_EGQ_DWM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_EGQ_FQP_NIF_PORT_MUX_TBL_ENTRY_SIZE                                           1
#define JER2_ARAD_EGQ_PQP_NIF_PORT_MUX_TBL_ENTRY_SIZE                                           1
#define JER2_ARAD_EGQ_ERPP_PER_PORT_TBL_ENTRY_SIZE                                              5
#define JER2_ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE                                                  2
#define JER2_ARAD_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                             1
#define JER2_ARAD_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                           1
#define JER2_ARAD_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL_ENTRY_SIZE                                 ((139/32) + 1) /* 19b FEM case*/
#define JER2_ARAD_IHB_PINFO_LBP_TBL_ENTRY_SIZE                                                  2
#define JER2_ARAD_IHB_HEADER_PROFILE_TBL_ENTRY_SIZE                                             1
#define JER2_ARAD_CFC_CALRX_TBL_ENTRY_SIZE                                                      1
#define JER2_ARAD_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                                1
#define JER2_ARAD_CFC_CALTX_TBL_ENTRY_SIZE                                                      1
#define JER2_ARAD_CFC_CALRX_TBL_ENTRY_SIZE                                                1
#define JER2_ARAD_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                              1
#define JER2_ARAD_CFC_OOB_SCH_MAP_TBL_ENTRY_SIZE                                              1
#define JER2_ARAD_CFC_CALTX_TBL_ENTRY_SIZE                                                1
#define JER2_ARAD_SCH_DRM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_DSM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_FDMS_TBL_ENTRY_SIZE                                                       2
#define JER2_ARAD_SCH_SHDS_TBL_ENTRY_SIZE                                                       2
#define JER2_ARAD_SCH_SEM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_FSF_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_FGM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_SHC_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_SCC_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_SCT_TBL_ENTRY_SIZE                                                        2
#define JER2_ARAD_SCH_FQM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_FFM_TBL_ENTRY_SIZE                                                        1
#define JER2_ARAD_SCH_SCHEDULER_INIT_TBL_ENTRY_SIZE                                             1
#define JER2_ARAD_SCH_FORCE_STATUS_MESSAGE_TBL_ENTRY_SIZE                                       1


#define JER2_ARAD_PP_IHP_MACT_AGING_CONFIGURATION_TABLE_TBL_ENTRY_SIZE                              ( 1)
#define JER2_ARAD_PP_IHP_TM_PORT_PARSER_PROGRAM_POINTER_CONFIG_TBL_ENTRY_SIZE                                   ( 1)
#define JER2_ARAD_PP_IHP_VIRTUAL_PORT_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                           ( 1)
#define JER2_ARAD_PP_IHP_VIRTUAL_PORT_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                            ( 1)
#define JER2_ARAD_PP_IHP_VIRTUAL_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                           ( 4)
#define JER2_ARAD_PP_IHP_SRC_SYSTEM_PORT_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                        ( 4)
#define JER2_ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_BIT_SELECT_TABLE_TBL_ENTRY_SIZE                 ( 1)
#define JER2_ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_MAP_INDEX_TABLE_TBL_ENTRY_SIZE                  ( 1)
#define JER2_ARAD_PP_IHP_PARSER_PROGRAM_POINTER_FEM_FIELD_SELECT_MAP_TBL_ENTRY_SIZE                 ( 1)
#define JER2_ARAD_PP_IHP_PARSER_PROGRAM_TBL_ENTRY_SIZE                                              ( 1)
#define JER2_ARAD_PP_IHP_VLAN_PORT_MEMBERSHIP_TBL_ENTRY_SIZE                                        ( 5)
#define JER2_ARAD_PP_IHP_SEM_RESULT_TABLE_TBL_ENTRY_SIZE                                            ( 2)
#define JER2_ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE                                    ( 5)
#define JER2_ARAD_PP_IHP_VTT1ST_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE                                    ( 6)
#define JER2_ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION0_TBL_ENTRY_SIZE                                    ( 5)
#define JER2_ARAD_PP_IHP_VTT2ND_KEY_CONSTRUCTION1_TBL_ENTRY_SIZE                                    ( 6)
#define JER2_ARAD_PP_IHP_TC_DP_MAP_TABLE_TBL_ENTRY_SIZE                                                ( 2)
#define JER2_ARAD_PP_IHP_TERMINATION_PROFILE_TABLE_TBL_ENTRY_SIZE                                   ( 1)
#define JER2_ARAD_PP_IHP_ACTION_PROFILE_MPLS_VALUE_TBL_ENTRY_SIZE                                   ( 1)
#define JER2_ARAD_PP_IHB_FEC_ENTRY_ACCESSED_TBL_ENTRY_SIZE                                          ( 1)
#define JER2_ARAD_PP_IHB_PATH_SELECT_TBL_ENTRY_SIZE                                                 ( 1)
#define JER2_ARAD_PP_IHB_TCAM_ACTION_TBL_ENTRY_SIZE                                                 ( 1)
#define JER2_ARAD_PP_EGQ_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE                               ( 1)
#define JER2_ARAD_PP_EGQ_VSI_MEMBERSHIP_TBL_ENTRY_SIZE                                              ( 9)
#define JER2_ARAD_PP_EPNI_TX_TAG_TABLE_TBL_ENTRY_SIZE                                               ( 9)
#define JER2_ARAD_PP_EPNI_PCP_DEI_MAP_TBL_ENTRY_SIZE                                                ( 2)
#define JER2_ARAD_PP_EPNI_DP_MAP_TBL_ENTRY_SIZE                                                     ( 1)
#define JER2_ARAD_PP_EPNI_EGRESS_EDIT_CMD_TBL_ENTRY_SIZE                                            ( 2)
#define JER2_ARAD_PP_EPNI_INGRESS_VLAN_EDIT_COMMAND_MAP_TBL_ENTRY_SIZE                              ( 1)
#define JER2_ARAD_PP_EPNI_ISID_TABLE_TBL_ENTRY_SIZE                                                    ( 1)
#define JER2_ARAD_PP_IHP_VLAN_EDIT_PCP_DEI_MAP_TBL_ENTRY_SIZE                                       ( 1)
#define JER2_ARAD_PP_IHP_INGRESS_VLAN_EDIT_COMMAND_TABLE_TBL_ENTRY_SIZE                             ( 1) 
#define JER2_ARAD_PP_IHB_FLP_PROCESS_TBL_ENTRY_SIZE 3
#define JER2_ARAD_PP_IHB_LPM_TBL_ENTRY_SIZE 1
#define JER2_ARAD_PP_IHB_PMF_PASS_2_KEY_UPDATE_TBL_ENTRY_SIZE 1

#define JER2_ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_EN_2_BIT     4
#define JER2_ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_LSB  5
#define JER2_ARAD_IQM_CNM_DS_TBL_DATA_SAMPLING_PROFILE_MSB  7

#define JER2_ARAD_IQM_CRDTDIS_DATA_NOF_BITS 0x7


#define JER2_ARAD_PP_VTT_PD_BITMAP_NOF_FIELDS  4
#define JER2_ARAD_PP_FLP_PD_BITMAP_NOF_FIELDS  7

#define JER2_ARAD_PP_IHP_LIF_TABLE_ENTRY_SIZE 4

#undef IRR_SNOOP_MIRROR_TABLE_0m
#define IRR_SNOOP_MIRROR_TABLE_0m \
    (SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_DEST_TABLEm : irr_snoop_mirror_table_0_backup)
#undef IRR_SNOOP_MIRROR_TABLE_1m
#define IRR_SNOOP_MIRROR_TABLE_1m \
    (SOC_IS_JERICHO(unit) ? IRR_SNOOP_MIRROR_TM_TABLEm : irr_snoop_mirror_table_1_backup)

#define JER2_ARAD_PP_LAG_BY_CHIP(_unit, _name)   \
        ((SOC_IS_QAX(unit)) ?  TAR_##_name##m : IRR_##_name##m)

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

#define SOC_DNX_MEM_FIELD32_GET(field_get,table_name,field_name,exit_num) \
{ \
    res=_SHR_E_NONE;\
  if(_SHR_E_NONE == res) { \
      field_get = soc_mem_field32_get( \
                      unit, \
                      table_name, \
                      data, \
                      field_name); \
   } \
   SHR_IF_ERR_EXIT(res); \
}

#define SOC_DNX_MEM_FIELD32_SET(field_set,table_name,field_name,exit_num) \
{ \
          res=_SHR_E_NONE;\
    if(_SHR_E_NONE == res) { \
        soc_mem_field32_set( \
          unit, \
          table_name, \
          data, \
          field_name, \
          field_set ); \
        } \
    SHR_IF_ERR_EXIT(res); \
}


/* for odd entry jump to odd table*/

#define JER2_ARAD_PP_TBL_FEC_ARRAY_INDEX(_entry_index) ((_entry_index)%2)
#define JER2_ARAD_PP_TBL_FEC_ENTRY_INDEX(_entry_index) ((_entry_index)/2)

#define JER2_ARAD_PP_SUPER_FEC_ENTRY_INDEX(fec_ndx) ((fec_ndx)/2)

/* Layout is: 8 banks, each has (FEC_NUM / 8 * 2) lines with 2 fields - data0 and data1. */
#define JER2_JERICHO_PP_SUPER_FEC_SZ                                 (2)
#ifdef FIXME_DNX_LEGACY /** SOC_DNX_DEFS not supported */
#define JER2_JERICHO_PP_SUPER_FEC_BANK_SIZE(unit)                    (SOC_DNX_DEFS_GET(unit, nof_fecs) / (SOC_DNX_DEFS_GET(unit, nof_fec_banks) * JER2_JERICHO_PP_SUPER_FEC_SZ))
#define JER2_JERICHO_PP_SUPER_FEC_ARRAY_INDEX(unit, _entry_index)    (((_entry_index) / JER2_JERICHO_PP_SUPER_FEC_SZ) / JER2_JERICHO_PP_SUPER_FEC_BANK_SIZE(unit))
#define JER2_JERICHO_PP_SUPER_FEC_ENTRY_INDEX(unit, _entry_index)    (((_entry_index) / JER2_JERICHO_PP_SUPER_FEC_SZ) % JER2_JERICHO_PP_SUPER_FEC_BANK_SIZE(unit))
#endif
#define JER2_JERICHO_PP_FEC_ENTRY_OFFSET(unit, _entry_index)         ((_entry_index) % JER2_JERICHO_PP_SUPER_FEC_SZ)

/* Simple TCAM print */
#define JER2_ARAD_PP_TBL_ACC_TCAM_DEBUG_PRINT 0
/*#define JER2_ARAD_PP_TBL_ACC_TCAM_DEBUG_PRINT 1*/

/* Low-Level TCAM print for all the transaction */
#define JER2_ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT 0
/*#define JER2_ARAD_PP_TBL_ACC_TCAM_LOW_LEVEL_DEBUG_PRINT 1*/

#define JER2_ARAD_PP_TBL_ACC_TCAM_DEBUG_ACTION_PRINT 0
/*#define JER2_ARAD_PP_TBL_ACC_TCAM_DEBUG_ACTION_PRINT 1*/

/* Conversation from entry offset to lif bank id (0-3 in jer2_arad, 0-7 in Jericho).
   In Arad it's simply the MSBs, in Jericho it's 2 MSBs + 1 LSB */

#define JER2_ARAD_MBMP_SET_DYNAMIC(_mem)      _SHR_PBMP_PORT_ADD(jer2_arad_mem_is_dynamic_bmp[_mem/32], _mem%32)
#define JER2_ARAD_MBMP_IS_DYNAMIC(_mem)       _SHR_PBMP_MEMBER(jer2_arad_mem_is_dynamic_bmp[_mem/32], _mem%32)

/* Conversion FLP instruction valid bitmap from JER2_ARAD to Jericho
 * - during Jericho porting, use instruction 0-2 similarly, 
 * and instructions 3-5 as Jericho instructions 4-6. */
#define FLP_INSTR_VALID_BMP_JER2_ARAD_TO_JER2_JER(inst_valid_bitmap)  \
  (((inst_valid_bitmap & 0x38) << 1) | ((inst_valid_bitmap) & 7))


/* Default mask for TCAM entry field */
#define JER2_ARAD_TBL_ACCESS_DEFAULT_TCAM_ENTRY_FIELD_MASK(_unit, _mem, _field) \
           ((1 << soc_mem_field_length((_unit), (_mem), (_field)))-1)

/* Default PRGE Program Selection TCAM entry field mask */
#define JER2_ARAD_EGR_PRGE_PROG_SEL_DEFAULT_ENTRY_FIELD_MASK(_unit, _field) \
           (JER2_ARAD_TBL_ACCESS_DEFAULT_TCAM_ENTRY_FIELD_MASK((_unit), EPNI_PRGE_PROGRAM_SELECTION_CAMm, (_field)))
/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

/* } */


/*
 * Read indirect table vsq_flow_control_parameters_table_group_a_tbl from block IQM,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */


void
  jer2_arad_iqm_mantissa_exponent_get(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   uint32              reg_val,
    DNX_SAND_IN   uint32              mantissa_size,
    DNX_SAND_OUT  uint32              *mantissa,
    DNX_SAND_OUT  uint32              *exponent
  )
{
    *mantissa = DNX_SAND_GET_BITS_RANGE(reg_val, mantissa_size - 1, 0);
    *exponent = reg_val >> mantissa_size;
}


void
  jer2_arad_iqm_mantissa_exponent_set(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   uint32              mantissa,
    DNX_SAND_IN   uint32              exponent,
    DNX_SAND_IN   uint32              mantissa_size,
    DNX_SAND_OUT  uint32              *reg_val
  )
{
    *reg_val = mantissa + (exponent << mantissa_size);
}




/*
 * Write indirect table flow_id_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_ips_flow_id_lookup_table_tbl_region_set(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_IN   uint32              region_size,
    DNX_SAND_IN   JER2_ARAD_IPS_FLOW_ID_LOOKUP_TABLE_TBL_DATA* IPS_flow_id_lookup_table_tbl_data
  )
{
  uint32 err;
  uint32 *data = NULL, *cur_data;
  uint32 flags=0;
  int index_max, index_min;
  int i; 
  SHR_FUNC_INIT_VARS(unit);

  index_min=entry_offset;
  index_max=entry_offset+region_size-1;
  data = (uint32 *)soc_cm_salloc(unit, region_size*sizeof(uint32), "IPS_QFMm"); /* allocate DMA memory buffer */
  if (data == NULL) {
      SHR_ERR_EXIT(_SHR_E_UNAVAIL, "_SHR_E_INTERNAL");
  }

  err = dnx_sand_os_memset(data, 0x0, region_size*sizeof(uint32));
  SHR_IF_ERR_EXIT(err);

  for (i=0, cur_data=data; i<region_size; i++, cur_data++) {
      *cur_data = 0;
      soc_mem_field32_set(unit, IPS_QFMm, cur_data, BASE_FLOWf, IPS_flow_id_lookup_table_tbl_data->base_flow);
      soc_mem_field32_set(unit, IPS_QFMm, cur_data, SUB_FLOW_MODEf, IPS_flow_id_lookup_table_tbl_data->sub_flow_mode);
  }

  err = soc_mem_array_write_range(unit, flags, IPS_QFMm, 0, MEM_BLOCK_ANY, index_min , index_max, data);
  SHR_IF_ERR_EXIT(err);


exit:
    if (data != NULL) {
        soc_cm_sfree(unit, data);
    }
  SHR_FUNC_EXIT;
}

/*
 * Write indirect table queue_type_lookup_table_tbl from block IPS,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */



soc_mem_t
  jer2_arad_egq_scm_chan_arb_id2scm_id(
    DNX_SAND_IN  int unit,
    DNX_SAND_IN  uint32 chan_arb_id
  )
{  
  soc_mem_t
    egq_scm_name;
  /*
   *  Go to the correct table
   */
  switch (chan_arb_id)
  {
  case DNX_OFP_RATES_EGQ_CHAN_ARB_00:
    egq_scm_name = EGQ_CH_0_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_01:
    egq_scm_name = EGQ_CH_1_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_02:
    egq_scm_name = EGQ_CH_2_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_03:
    egq_scm_name = EGQ_CH_3_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_04:
    egq_scm_name = EGQ_CH_4_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_05:
    egq_scm_name = EGQ_CH_5_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_06:
    egq_scm_name = EGQ_CH_6_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_07:
    egq_scm_name = EGQ_CH_7_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_CPU:
    egq_scm_name = EGQ_CH_8_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_RCY:
    egq_scm_name = EGQ_CH_9_SCMm;
    break;
  case DNX_OFP_RATES_EGQ_CHAN_ARB_NON_CHAN:
    egq_scm_name = EGQ_NONCH_SCMm;
    break;
  default:
    egq_scm_name = EGQ_CH_0_SCMm;
  }

  return egq_scm_name;
}

/*
 * Read indirect table scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  jer2_arad_egq_scm_tbl_get(
    DNX_SAND_IN   int                       unit,
    DNX_SAND_IN   int                       core,
    DNX_SAND_IN   dnx_ofp_rates_cal_info_t   *cal_info,
    DNX_SAND_IN   uint32                    entry_offset,
    DNX_SAND_OUT  JER2_ARAD_EGQ_SCM_TBL_DATA*    EGQ_nif_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_NIF_SCM_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_scm_name;
  soc_field_t
    egq_cr_field_name,
    egq_index_field_name;
 
  SHR_FUNC_INIT_VARS(unit);

  SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);

  err = dnx_sand_os_memset(
          EGQ_nif_scm_tbl_data,
          0x0,
          sizeof(JER2_ARAD_EGQ_SCM_TBL_DATA)
        );
  SHR_IF_ERR_EXIT(err);

  switch (cal_info->cal_type)
  {
  case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
    if (cal_info->chan_arb_id > (dnx_data_egr_queuing.params.nof_calendars_get(unit) - 1))
    {
      SHR_ERR_EXIT(_SHR_E_UNAVAIL, "JER2_ARAD_OFP_RATES_CHAN_ARB_INVALID_ERR");
    }
    egq_scm_name = EPS_OTM_HP_CRDT_TABLEm;
    egq_cr_field_name = OTM_CRDT_TO_ADDf;
    egq_index_field_name = OTM_INDEXf;
    break;
  case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    egq_scm_name = EPS_QP_CREDIT_TABLEm;
    egq_cr_field_name = QPAIR_CRDT_TO_ADDf;
    egq_index_field_name = QPAIR_INDEXf;
    break;
  case DNX_OFP_RATES_EGQ_CAL_TCG:
    egq_scm_name = EPS_TCG_CREDIT_TABLEm;
    egq_cr_field_name = TCG_CRDT_TO_ADDf;
    egq_index_field_name = TCG_INDEXf;
    break;
  default:
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "JER2_ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR");
  }
  
  JER2_ARAD_DEVICE_CHECK(unit, exit);
  err = soc_mem_read(
          unit,
          egq_scm_name,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

err=_SHR_E_NONE;
    if(_SHR_E_NONE == err) {
          EGQ_nif_scm_tbl_data->port_cr_to_add = soc_mem_field32_get(
                  unit,
                  egq_scm_name,
                  data,
                  egq_cr_field_name    );
        }
  SHR_IF_ERR_EXIT(err);

    if(_SHR_E_NONE == err) {
          EGQ_nif_scm_tbl_data->ofp_index = soc_mem_field32_get(
                  unit,
                  egq_scm_name,
                  data,
                egq_index_field_name );
  }
  SHR_IF_ERR_EXIT(err);

exit:
  SHR_FUNC_EXIT;
}

/*
 * Write indirect table scm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_scm_tbl_set(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   int             core,
    DNX_SAND_IN   dnx_ofp_rates_cal_info_t *cal_info,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_IN   JER2_ARAD_EGQ_SCM_TBL_DATA* EGQ_nif_scm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_NIF_SCM_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_scm_name;
  soc_field_t
    egq_cr_field_name,
    egq_index_field_name;
 
  SHR_FUNC_INIT_VARS(unit);

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);

  switch (cal_info->cal_type)
  {
  case DNX_OFP_RATES_EGQ_CAL_CHAN_ARB:
    if (cal_info->chan_arb_id > (dnx_data_egr_queuing.params.nof_calendars_get(unit) - 1))
    {
      SHR_ERR_EXIT(_SHR_E_UNAVAIL, "JER2_ARAD_OFP_RATES_CHAN_ARB_INVALID_ERR");
    }
     /*
      *  Go to the correct table, per chan arb id
      */
    egq_scm_name = EPS_OTM_HP_CRDT_TABLEm;
    egq_cr_field_name = OTM_CRDT_TO_ADDf;
    egq_index_field_name = OTM_INDEXf;
    break;
  case DNX_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    egq_scm_name = EPS_QP_CREDIT_TABLEm;
    egq_cr_field_name = QPAIR_CRDT_TO_ADDf;
    egq_index_field_name = QPAIR_INDEXf;
    break;
  case DNX_OFP_RATES_EGQ_CAL_TCG:
    egq_scm_name = EPS_TCG_CREDIT_TABLEm;
    egq_cr_field_name = TCG_CRDT_TO_ADDf;
    egq_index_field_name = TCG_INDEXf;
    break;
  default:
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "JER2_ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR");
  }  

  err=_SHR_E_NONE;
  JER2_ARAD_DEVICE_CHECK(unit, exit);
  if(_SHR_E_NONE == err) {
    soc_mem_field32_set(
          unit,
          egq_scm_name,
          data,
          egq_cr_field_name,
          EGQ_nif_scm_tbl_data->port_cr_to_add);
  }
  SHR_IF_ERR_EXIT(err);

  if(_SHR_E_NONE == err) {
    soc_mem_field32_set(
          unit,
          egq_scm_name,
          data,
          egq_index_field_name,
          EGQ_nif_scm_tbl_data->ofp_index );
  }
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_write(
          unit,
          egq_scm_name,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

  if(egq_scm_name == EPS_OTM_HP_CRDT_TABLEm) {
      /*
       * low priority need to have double size of credit
       */
      soc_mem_field32_set(
          unit,
          egq_scm_name,
          data,
          egq_cr_field_name,
          EGQ_nif_scm_tbl_data->port_cr_to_add * 2);
      err = soc_mem_write(
              unit,
              EPS_OTM_LP_CRDT_TABLEm,
              EPS_BLOCK(unit, core),
              entry_offset,
              data
            );
      SHR_IF_ERR_EXIT(err);
  }

exit:
  SHR_FUNC_EXIT;
}

/*
 * Read indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_ccm_tbl_get(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_OUT  dnx_egq_ccm_tbl_data_t* EGQ_ccm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_CCM_TBL_ENTRY_SIZE];
 
  SHR_FUNC_INIT_VARS(unit);

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);

  err = dnx_sand_os_memset(
          EGQ_ccm_tbl_data,
          0x0,
          sizeof(*EGQ_ccm_tbl_data)
        );
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_read(
          unit,
          EPS_CAL_CAL_INDX_MUXm,
          MEM_BLOCK_ANY,
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

err=_SHR_E_NONE;
    if(_SHR_E_NONE == err) {
          EGQ_ccm_tbl_data->interface_select   = soc_mem_field32_get(
              unit,
              EPS_CAL_CAL_INDX_MUXm,
              data,
              CAL_INDXf
          );
  }
  SHR_IF_ERR_EXIT(err);

exit:
  SHR_FUNC_EXIT;
}

/*
 * Write indirect table ccm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_ccm_tbl_set(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   int             core,
    DNX_SAND_IN   uint32          entry_offset,
    DNX_SAND_IN   dnx_egq_ccm_tbl_data_t* EGQ_ccm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_CCM_TBL_ENTRY_SIZE];
 
  SHR_FUNC_INIT_VARS(unit);

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);

  err=_SHR_E_NONE;
  if(_SHR_E_NONE == err) {
            soc_mem_field32_set(
          unit,
          EPS_CAL_CAL_INDX_MUXm,
          data,
          CAL_INDXf,
          EGQ_ccm_tbl_data->interface_select
      );
  }
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_write(
          unit,
          EPS_CAL_CAL_INDX_MUXm,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

exit:
  SHR_FUNC_EXIT;
}

#if (0)
/* { */
static
  int
    jer2_arad_egq_pmc_names_get(
      DNX_SAND_IN   int             unit,
      DNX_SAND_IN   dnx_ofp_rates_cal_info_t *cal_info,
      DNX_SAND_OUT  soc_mem_t             *egq_pmc_name,
      DNX_SAND_OUT  soc_field_t           *egq_cr_field_name
    )
{

  SHR_FUNC_INIT_VARS(unit);

  SHR_NULL_CHECK(cal_info, _SHR_E_PARAM, "cal_info");
  SHR_NULL_CHECK(egq_pmc_name, _SHR_E_PARAM, "egq_pmc_name");
  SHR_NULL_CHECK(egq_cr_field_name, _SHR_E_PARAM, "egq_cr_field_name");

  switch (cal_info->cal_type)
  {
  case JER2_ARAD_OFP_RATES_EGQ_CAL_CHAN_ARB:
    *egq_pmc_name = EPS_OTM_HP_CRDT_TABLEm;
    *egq_cr_field_name = OTM_MAX_BURSTf;
    break;
  case JER2_ARAD_OFP_RATES_EGQ_CAL_PORT_PRIORITY:
    *egq_pmc_name = EPS_QP_CREDIT_TABLEm;
    *egq_cr_field_name = QPAIR_MAX_BURSTf;
    break;
  case JER2_ARAD_OFP_RATES_EGQ_CAL_TCG:
    *egq_pmc_name = EPS_TCG_CREDIT_TABLEm;
    *egq_cr_field_name = TCG_MAX_BURSTf;
    break;
  default:
    SHR_ERR_EXIT(_SHR_E_UNAVAIL, "JER2_ARAD_OFP_RATES_EGQ_CAL_INVALID_ERR");
  }

exit:
  SHR_FUNC_EXIT;
}

/*
 * Read indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_pmc_tbl_get(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   int             core,
    DNX_SAND_IN   dnx_ofp_rates_cal_info_t *cal_info,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_OUT  JER2_ARAD_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_PMC_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_pmc_name;
  soc_field_t
    egq_max_credit_field_name;
 
  SHR_FUNC_INIT_VARS(unit);

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);

  err = dnx_sand_os_memset(
          EGQ_pmc_tbl_data,
          0x0,
          sizeof(JER2_ARAD_EGQ_PMC_TBL_DATA)
        );
  SHR_IF_ERR_EXIT(err);

  err = jer2_arad_egq_pmc_names_get(
          unit,
          cal_info,
          &egq_pmc_name,
          &egq_max_credit_field_name
        );
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_read(
          unit,
          egq_pmc_name,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

err=_SHR_E_NONE;
    if(_SHR_E_NONE == err) {
          EGQ_pmc_tbl_data->port_max_credit   = soc_mem_field32_get(
                  unit,
                  egq_pmc_name,
                  data,
                  egq_max_credit_field_name            );
        }
  SHR_IF_ERR_EXIT(err);

exit:
  SHR_FUNC_EXIT;
}

/*
 * Write indirect table pmc_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_pmc_tbl_set(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   int             core,
    DNX_SAND_IN   dnx_ofp_rates_cal_info_t *cal_info,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_IN   JER2_ARAD_EGQ_PMC_TBL_DATA* EGQ_pmc_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_PMC_TBL_ENTRY_SIZE];
  soc_mem_t
    egq_pmc_name;
  soc_field_t
    egq_max_credit_field_name;
 
  SHR_FUNC_INIT_VARS(unit);

  err = jer2_arad_egq_pmc_names_get(
          unit,
          cal_info,
          &egq_pmc_name,
          &egq_max_credit_field_name
        );
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_read(
          unit,
          egq_pmc_name,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

  soc_mem_field32_set(
      unit,
      egq_pmc_name,
      data,
      egq_max_credit_field_name,
      EGQ_pmc_tbl_data->port_max_credit
  );
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_write(
          unit,
          egq_pmc_name,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

  if(egq_pmc_name == EPS_OTM_HP_CRDT_TABLEm) {
      err = soc_mem_read(
              unit,
              EPS_OTM_LP_CRDT_TABLEm,
              EPS_BLOCK(unit, core),
              entry_offset,
              data
            );
      SHR_IF_ERR_EXIT(err);

      soc_mem_field32_set(
          unit,
          EPS_OTM_LP_CRDT_TABLEm,
          data,
          egq_max_credit_field_name,
          EGQ_pmc_tbl_data->port_max_credit * 2
      );
      SHR_IF_ERR_EXIT(err);

      err = soc_mem_write(
              unit,
              EPS_OTM_LP_CRDT_TABLEm,
              EPS_BLOCK(unit, core),
              entry_offset,
              data
            );
      SHR_IF_ERR_EXIT(err);
  }

exit:
  SHR_FUNC_EXIT;
}
/* } */
#endif

/*
 * Read indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_dwm_tbl_get(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   int             core,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_OUT  dnx_egq_dwm_tbl_data_t * EGQ_dwm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_DWM_TBL_ENTRY_SIZE];
 
  SHR_FUNC_INIT_VARS(unit);

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);

  err = dnx_sand_os_memset(
          EGQ_dwm_tbl_data,
          0x0,
          sizeof(*EGQ_dwm_tbl_data)
        );
  SHR_IF_ERR_EXIT(err);

  err = soc_mem_read(
          unit,
          EPS_DWMm,
		  EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

  EGQ_dwm_tbl_data->mc_or_mc_low_queue_weight = soc_mem_field32_get(
                  unit,
				  EPS_DWMm,
                  data,
                  MC_OR_MC_LOW_QUEUE_WEIGHTf);
  SHR_IF_ERR_EXIT(err);


  EGQ_dwm_tbl_data->uc_or_uc_low_queue_weight   = soc_mem_field32_get(
          unit,
		  EPS_DWMm,
          data,
          UC_OR_UC_LOW_QUEUE_WEIGHTf);
  SHR_IF_ERR_EXIT(err);

exit:
  SHR_FUNC_EXIT;
}

/*
 * Write indirect table dwm_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_dwm_tbl_set(
    DNX_SAND_IN   int             unit,
    DNX_SAND_IN   int             core,
    DNX_SAND_IN   uint32              entry_offset,
    DNX_SAND_IN   dnx_egq_dwm_tbl_data_t* EGQ_dwm_tbl_data
  )
{
  uint32
    err;
  uint32
    data[JER2_ARAD_EGQ_DWM_TBL_ENTRY_SIZE];
 
  SHR_FUNC_INIT_VARS(unit);

  err = dnx_sand_os_memset(
          &(data[0]),
          0x0,
          sizeof(data)
        );
  SHR_IF_ERR_EXIT(err);


  soc_mem_field32_set(
          unit,
          EPS_DWMm,
          data,
          MC_OR_MC_LOW_QUEUE_WEIGHTf,
          EGQ_dwm_tbl_data->mc_or_mc_low_queue_weight );



  soc_mem_field32_set(
          unit,
          EPS_DWMm,
          data,
          UC_OR_UC_LOW_QUEUE_WEIGHTf,
          EGQ_dwm_tbl_data->uc_or_uc_low_queue_weight );

  err = soc_mem_write(
          unit,
          EPS_DWMm,
          EPS_BLOCK(unit, core),
          entry_offset,
          data
        );
  SHR_IF_ERR_EXIT(err);

exit:
  SHR_FUNC_EXIT;
}

/*
 * Read indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_ppct_tbl_get(
    DNX_SAND_IN   int                       unit,
    DNX_SAND_IN   int                       core_id,
    DNX_SAND_IN   uint32                    internal_port,
    DNX_SAND_OUT  JER2_ARAD_EGQ_PPCT_TBL_DATA*   EGQ_ppct_tbl_data
  )
{
    uint32
        rv;
    uint32
        data[JER2_ARAD_EGQ_ERPP_PER_PORT_TBL_ENTRY_SIZE];
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(EGQ_ppct_tbl_data, 0x0, sizeof(JER2_ARAD_EGQ_PPCT_TBL_DATA));

    rv = READ_ERPP_PER_PORT_TABLEm(unit, ERPP_BLOCK(unit, core_id), internal_port, data);
    EGQ_ppct_tbl_data->cos_map_profile = soc_mem_field32_get(unit, ERPP_PER_PORT_TABLEm, data, TC_MAP_PROFILEf);
    SHR_IF_ERR_EXIT(rv);


exit:
    SHR_FUNC_EXIT;
}

/*
 * Write indirect table ppct_tbl from block EGQ,
 * doesn't take semaphore!
 * Must only be called from a function taking the device semaphore
 */

int
  jer2_arad_egq_ppct_tbl_set(
    DNX_SAND_IN   int                       unit,
    DNX_SAND_IN   int                       core_id,
    DNX_SAND_IN   uint32                    internal_port,
    DNX_SAND_IN   JER2_ARAD_EGQ_PPCT_TBL_DATA*   EGQ_ppct_tbl_data
  )
{
    uint32 data[JER2_ARAD_EGQ_ERPP_PER_PORT_TBL_ENTRY_SIZE];
    int rv;
 
    SHR_FUNC_INIT_VARS(unit);

    sal_memset(data, 0, sizeof(uint32) * JER2_ARAD_EGQ_ERPP_PER_PORT_TBL_ENTRY_SIZE);

    soc_mem_field32_set(unit, ERPP_PER_PORT_TABLEm, data, TC_MAP_PROFILEf, EGQ_ppct_tbl_data->cos_map_profile);
    rv = WRITE_ERPP_PER_PORT_TABLEm(unit, ERPP_BLOCK(unit, core_id), internal_port, data);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}




/*
 * Read indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
static
  void
    jer2_arad_egq_tc_dp_map_table_entry_translate(
      DNX_SAND_IN  int             unit,
      DNX_SAND_IN  JER2_ARAD_EGQ_TC_DP_MAP_TBL_ENTRY       *entry,
      DNX_SAND_OUT uint32                         *entry_offset
    )
{
  uint32
    offset = 0;

  offset |= DNX_SAND_SET_BITS_RANGE(entry->dp, 1, 0);
  offset |= DNX_SAND_SET_BITS_RANGE(entry->tc, 4, 2);  
  offset |= DNX_SAND_SET_BITS_RANGE(entry->is_egr_mc, 5, 5);
  offset |= DNX_SAND_SET_BITS_RANGE(entry->map_profile, 9, 6);

  *entry_offset = offset;
}

void
  JER2_ARAD_EGQ_TC_DP_MAP_TBL_ENTRY_clear(
    DNX_SAND_OUT JER2_ARAD_EGQ_TC_DP_MAP_TBL_ENTRY       *info
  )
{
  SHR_FUNC_INIT_VARS(NO_UNIT);
  SHR_NULL_CHECK(info, _SHR_E_PARAM, "info");

  dnx_sand_os_memset(info, 0x0, sizeof(JER2_ARAD_EGQ_TC_DP_MAP_TBL_ENTRY));
  info->tc = 0;
  info->dp = 0;  
  info->is_egr_mc = 0;
  info->map_profile = 0;

exit:
  SHR_VOID_FUNC_EXIT;
}

int
  jer2_arad_egq_tc_dp_map_tbl_get(
    DNX_SAND_IN  int                            unit,
    DNX_SAND_IN  int                            core_id,
    DNX_SAND_IN  JER2_ARAD_EGQ_TC_DP_MAP_TBL_ENTRY   *entry,
    DNX_SAND_OUT JER2_ARAD_EGQ_TC_DP_MAP_TBL_DATA    *tbl_data
  )
{
    uint32 entry_offset = 0;
    uint32 data = 0;
    SHR_FUNC_INIT_VARS(unit);
 
    sal_memset(tbl_data, 0, sizeof(JER2_ARAD_EGQ_TC_DP_MAP_TBL_DATA));

    jer2_arad_egq_tc_dp_map_table_entry_translate(unit, entry, &entry_offset);

    SHR_IF_ERR_EXIT(soc_mem_read(unit, PQP_TC_DP_MAPm, PQP_BLOCK(unit, core_id), entry_offset, &data));

    tbl_data->tc = soc_mem_field32_get(unit, PQP_TC_DP_MAPm, &data, EGRESS_TCf);
    tbl_data->dp = soc_mem_field32_get(unit, PQP_TC_DP_MAPm, &data, CGM_MC_DPf);

exit:
    SHR_FUNC_EXIT;
}

/*
 * Write indirect table tc_dp_map_table_tbl from block EGQ,
 * doesn't take semaphore.
 * Must only be called from a function taking the device semaphore
 */
int
  jer2_arad_egq_tc_dp_map_tbl_set(
    DNX_SAND_IN  int                            unit,
    DNX_SAND_IN  int                            core_id,
    DNX_SAND_IN  JER2_ARAD_EGQ_TC_DP_MAP_TBL_ENTRY   *entry,
    DNX_SAND_IN  JER2_ARAD_EGQ_TC_DP_MAP_TBL_DATA    *tbl_data
  )
{
    uint32 entry_offset = 0;
    uint32 data = 0;

    SHR_FUNC_INIT_VARS(unit);

    jer2_arad_egq_tc_dp_map_table_entry_translate(unit, entry, &entry_offset);

    soc_mem_field32_set(unit, PQP_TC_DP_MAPm, &data, EGRESS_TCf, tbl_data->tc);
    soc_mem_field32_set(unit, PQP_TC_DP_MAPm, &data, CGM_MC_DPf, tbl_data->dp);

    SHR_IF_ERR_EXIT(soc_mem_write(unit, PQP_TC_DP_MAPm, PQP_BLOCK(unit, core_id), entry_offset, &data));

exit:
    SHR_FUNC_EXIT;
}


static int soc_mem_is_in_soc_property(int unit, soc_mem_t mem, int en)
{
    /*Check whether given 'mem' is defined in the soc_property configuration.*/
    char mem_name[SOC_PROPERTY_NAME_MAX];
    char *mptr;

    if (en == TRUE)    /* mem_cache_enable_specific_ */
    {
        sal_strncpy(mem_name, spn_MEM_CACHE_ENABLE, SOC_PROPERTY_NAME_MAX);
        mptr = &mem_name[sal_strlen(mem_name)];
        sal_strncpy(mptr, "_specific_", (SOC_PROPERTY_NAME_MAX - sal_strlen(mem_name)));
    }
    else            /* mem_nocache_ */
        sal_strncpy(mem_name, "mem_nocache_",SOC_PROPERTY_NAME_MAX);

    mptr = &mem_name[sal_strlen(mem_name)];
    sal_strncpy(mptr, SOC_MEM_NAME(unit, mem), (SOC_PROPERTY_NAME_MAX - sal_strlen(mem_name)));


    if (soc_property_get(unit, mem_name, 0))
    {
        LOG_ERROR(BSL_LS_APPL_TESTS, (BSL_META("unit %d memory %d (%s), soc_prop %s\n"), unit, mem, SOC_MEM_NAME(unit, mem), mem_name));
        return TRUE;
    }
    return FALSE;
}

int jer2_arad_tbl_mem_cache_enable_specific_tbl(int unit, soc_mem_t mem, void* en)
{
  int rc        = _SHR_E_NONE;
    int cache_enable = *(int *)en;

    SOC_MEM_ALIAS_TO_ORIG(unit,mem);
    if (soc_mem_is_in_soc_property(unit, mem, cache_enable))
    {
        if(!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
        {
            LOG_ERROR(BSL_LS_SOC_MEM, (BSL_META("unit %d cache %s failed for %d (%s) rv %d\n"), unit, cache_enable ?"enable":"disable", mem, SOC_MEM_NAME(unit, mem), rc));
            return _SHR_E_UNAVAIL;
        }
        rc = jer2_arad_tbl_mem_cache_mem_set(unit, mem, en);
    }

    return rc;
}

int jer2_arad_tbl_mem_cache_mem_set(int unit, soc_mem_t mem, void* en)
{
    int rc         = _SHR_E_NONE;
    int enable     = *(int *)en;
    int dis        = 0;

  SOC_MEM_ALIAS_TO_ORIG(unit,mem);
    if(!SOC_MEM_IS_VALID(unit, mem) || !soc_mem_is_cachable(unit, mem))
    {
        return _SHR_E_NONE;
    }

    /* Skip the "mem_nocache_" memories in soc_property */
    if (enable == TRUE)
    {
        if (soc_mem_is_in_soc_property(unit, mem, dis))
            return _SHR_E_NONE;
    }

    /* Turn on cache memory for all tables */
    if (soc_mem_is_valid(unit, mem) &&
        ((SOC_MEM_INFO(unit, mem).blocks | SOC_MEM_INFO(unit, mem).blocks_hi) != 0))
    {
       rc = soc_mem_cache_set(unit, mem, COPYNO_ALL, enable);
    }
    else
    {
        rc = _SHR_E_NONE;
    }

    return rc;
}





