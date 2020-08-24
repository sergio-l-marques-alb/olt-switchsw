/* $Id: arad_pp_lag.h,v 1.8 Broadcom SDK $
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/

#ifndef __ARAD_PP_LAG_INCLUDED__
/* { */
#define __ARAD_PP_LAG_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/Management/sand_error_code.h>

#include <soc/dpp/ARAD/ARAD_PP/arad_pp_framework.h>

#include <soc/dpp/PPC/ppc_api_lag.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */
#define ARAD_PP_FLEX_POST_HASH_KEYS_INPUT_SIZE                47
#define ARAD_PP_FLEX_ECMP_KEY_INPUT_MAX_32BIT_BUFF_SIZE        2
#define SOC_SUPPORTS_ECMP_HASH_COMBINE(_unit) (SOC_IS_JERICHO_B0(_unit) || SOC_IS_QMX_B0(_unit) || SOC_IS_QAX_B0(_unit) || SOC_IS_JERICHO_PLUS_ONLY(_unit) || SOC_IS_QUX_A0(_unit))
/* } */
/*************
 * MACROS    *
 *************/
/* { */

/* } */
/*************
 * TYPE DEFS *
 *************/
/* { */

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_LAG_SET = ARAD_PP_PROC_DESC_BASE_LAG_FIRST,
  ARAD_PP_LAG_SET_PRINT,
  ARAD_PP_LAG_SET_UNSAFE,
  ARAD_PP_LAG_SET_VERIFY,
  ARAD_PP_LAG_GET,
  ARAD_PP_LAG_GET_PRINT,
  ARAD_PP_LAG_GET_VERIFY,
  ARAD_PP_LAG_GET_UNSAFE,
  SOC_PPC_LAG_MEMBER_ADD,
  SOC_PPC_LAG_MEMBER_ADD_PRINT,
  SOC_PPC_LAG_MEMBER_ADD_UNSAFE,
  SOC_PPC_LAG_MEMBER_ADD_VERIFY,
  SOC_PPC_LAG_MEMBER_REMOVE,
  SOC_PPC_LAG_MEMBER_REMOVE_PRINT,
  SOC_PPC_LAG_MEMBER_REMOVE_UNSAFE,
  SOC_PPC_LAG_MEMBER_REMOVE_VERIFY,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_PRINT,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_UNSAFE,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_SET_VERIFY,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_PRINT,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_VERIFY,
  ARAD_PP_LAG_HASHING_GLOBAL_INFO_GET_UNSAFE,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET_PRINT,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET_UNSAFE,
  ARAD_PP_LAG_HASHING_PORT_INFO_SET_VERIFY,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET_PRINT,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET_VERIFY,
  ARAD_PP_LAG_HASHING_PORT_INFO_GET_UNSAFE,
  ARAD_PP_LAG_HASHING_MASK_SET,
  ARAD_PP_LAG_HASHING_MASK_SET_PRINT,
  ARAD_PP_LAG_HASHING_MASK_SET_UNSAFE,
  ARAD_PP_LAG_HASHING_MASK_SET_VERIFY,
  ARAD_PP_LAG_HASHING_MASK_GET,
  ARAD_PP_LAG_HASHING_MASK_GET_PRINT,
  ARAD_PP_LAG_HASHING_MASK_GET_VERIFY,
  ARAD_PP_LAG_HASHING_MASK_GET_UNSAFE,
  ARAD_PP_LAG_GET_PROCS_PTR,
  ARAD_PP_LAG_GET_ERRS_PTR,
  /*
   * } Auto generated. Do not edit previous section.
   */

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_LAG_PROCEDURE_DESC_LAST
} ARAD_PP_LAG_PROCEDURE_DESC;

typedef enum
{
  /*
   * Auto generated. Do not edit following section {
   */
  ARAD_PP_LAG_LAG_NDX_OUT_OF_RANGE_ERR = ARAD_PP_ERR_DESC_BASE_LAG_FIRST,
  ARAD_PP_LAG_SUCCESS_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_SYS_PORT_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_PORT_NDX_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_MASKS_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_SEED_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_HASH_FUNC_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_KEY_SHIFT_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_NOF_HEADERS_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_FIRST_HEADER_TO_PARSE_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_MASK_OUT_OF_RANGE_ERR,
  SOC_PPC_LAG_MEMBER_ID_OUT_OF_RANGE_ERR,
  ARAD_PP_LAG_NOF_ENTRIES_OUT_OF_RANGE_ERR,
  SOC_PPC_LAG_LB_TYPE_OUT_OF_RANGE_ERR,
  /*
   * } Auto generated. Do not edit previous section.
   */
   ARAD_PP_LAG_ASYMETRIC_ERR,
   ARAD_PP_LAG_DOUPLICATE_MEMBER_ERR,

  /*
   * Last element. Do no touch.
   */
  ARAD_PP_LAG_ERR_LAST
} ARAD_PP_LAG_ERR;
/*
 * Enumeration of the possible LB hierarchies
 */
typedef enum
{
  ARAD_PP_HIERARCHY_1 = 0,
  ARAD_PP_HIERARCHY_2,
  ARAD_PP_HIERARCHY_NOF
} ARAD_PP_HIERARCHY;
/*
 * Enumeration of the PFC profile indexes
 */
typedef enum
{
  ARAD_PP_LB_VECTOR_1 = 0,
  ARAD_PP_LB_VECTOR_2,
  ARAD_PP_LB_VECTOR_3,
  ARAD_PP_LB_VECTOR_4,
  ARAD_PP_LB_VECTOR_5,
  ARAD_PP_LB_VECTOR_NOF
} ARAD_PP_LB_VECTOR;
/*
 * Enumeration of the hashing types
 */
typedef enum
{
  ARAD_PP_DIAG_ECMP = 0,
  ARAD_PP_DIAG_LAG
} ARAD_PP_DIAG_TYPE;


typedef struct
{
  /*
  * indicates whether LB key is symmetric
  */
  uint8 lb_key_is_symtrc;

  /*
   * modid of current device
   */
  uint32 my_modid;

  SOC_PPC_HASH_MASKS masks;

} ARAD_PP_LAG;

/*
 *  This structure contains the diagnostic information of the FLB keys
 */
typedef struct
{
    uint32 key_0_shift;
    uint32 key_1_shift;
    uint32 key_0_input[ARAD_PP_FLEX_ECMP_KEY_INPUT_MAX_32BIT_BUFF_SIZE];
    uint32 key_1_input[ARAD_PP_FLEX_ECMP_KEY_INPUT_MAX_32BIT_BUFF_SIZE];
    uint32 flow_label_dst[3];
}ARAD_PP_LB_PRINT_STATEFUL_SIGNALS;

/*
 *  This structure contains the diagnostic information of the CLB configuration of ECMP/ECMP 2 hierarchy/LAG of a port
 */
typedef struct
{
    uint32 polynomial;
    uint32 key_shift;
    uint32 seed;
    uint32 use_port_id;
    uint32 header_count;
    uint32 starting_header;
}ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION;
/*
 * This structure contains the diagnostic information of the configured LB
 */
typedef struct
{
    uint32 plc;
    uint32 plc_profile;
    uint32 plc_bit;
    uint32 pfc_stage1;
    uint32 pfc_stage2;
    uint32 pfc_profile_index;
    uint32 port_profile;
    uint32 fwrd_index;
    uint32 bos_search;
    uint32 bos_include;
    uint32 eli_search;
    uint32 LB_vectors[ARAD_PP_LB_VECTOR_NOF];
    ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION lag;
    ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION ecmp;
    /* For devices that have two hierarchy support for configured LB (seed and port_id are not exists in the second hierarchy) */
    ARAD_PP_LB_PRINT_HASH_TYPE_CLB_OPTION ecmp_2nd;
}ARAD_PP_LB_PRINT_CONFIGURED_SIGNALS;

/*
 * This structure contains the ECMP and LAG diagnostic information
 */
typedef struct
{
    int    ecmp_fec;
    uint8  ecmp_is_valid;
    uint32 offset;
    uint32 ecmp_fec_pointer;
    uint32 start_pointer;
    uint32 is_protected;
    uint32 group_size;
    /*
     * The LB key relevant only for the second hierarchy.
     */
    uint32 ecmp_lb_key;
}ARAD_PP_LB_PRINT_HIERARCHY_ECMP_SIGNALS;

/*
 * This structure contains all the load balancing diagnostic information for both the LAG and the ECMP for a single core
 */
typedef struct
{
    uint32                                  in_port;
    uint32                                  is_stateful;
    uint32                                  lag_valid;
    uint32                                  is_specualte;
    ARAD_PP_LB_PRINT_CONFIGURED_SIGNALS     configured;
    ARAD_PP_LB_PRINT_STATEFUL_SIGNALS       stateful;
    ARAD_PP_LB_PRINT_HIERARCHY_ECMP_SIGNALS hier_sig[ARAD_PP_HIERARCHY_NOF];
}ARAD_PP_LB_PRINT_SIGNALS_PER_CORE;

/*
 * This structure contains all the load balancing diagnostic information for both the LAG and the ECMP for all the cores
 */
typedef struct
{
    int                                     first_core;
    int                                     last_core;
    uint32                                  is_combine_flb_clb;
    ARAD_PP_DIAG_TYPE                       diag_type;
    ARAD_PP_LB_PRINT_SIGNALS_PER_CORE       core[SOC_MAX_NUM_IHB_BLKS];
}ARAD_PP_LB_PRINT_SIGNALS;
/* } */
/*************
 * GLOBALS   *
 *************/
/* { */

/* } */
/*************
 * FUNCTIONS *
 *************/
/* { */

uint32
  arad_pp_lag_init_unsafe(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Configure a LAG. A LAG is defined by a group of System
 *   Physical Ports that compose it. This configuration
 *   affects 1. LAG resolution: when the destination of
 *   packet is LAG 2. Learning: when packet source port
 *   belongs to LAG, then the LAG is learnt.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lag_ndx -
 *     LAG ID. Range: 0 - 255.
 *   SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info -
 *     Lag members. Maximal number of out-going LAG members is
 *     16. The number of incoming LAG members is not limited,
 *     and it can be the number of Local FAP ports in each
 *     device.
 * REMARKS:
 *   1. Local to system port mapping must be configured
 *   before using this API (Incoming and Outgoing) - for LAG
 *   pruning. 2. LAG configuration must be consistent
 *   system-wide, for incoming and outgoing ports. 3. The
 *   member index inside the LAG (0-255) is defined by the
 *   index of the appropriate port in the members array. 4.
 *   Setting LAG with a group of system ports, will first
 *   clean-up any previous configuration of the LAG. For
 *   example, setting LAG 1 with system members 1,2,3,4 and
 *   then setting the same LAG with members 3,4,5,6 will
 *   clean up the effect of the previous configuration and
 *   set up the new configuration.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info
  );

uint32
  arad_pp_lag_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info
  );

uint32
  arad_pp_lag_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx
  );

/*********************************************************************
*     Gets the configuration set by the "arad_pp_lag_set_unsafe"
 *     API.
 *     Refer to "arad_pp_lag_set_unsafe" API for details.
*********************************************************************/
uint32
  arad_pp_lag_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_INFO                            *lag_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_member_add_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Add a system port as a member in LAG.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lag_ndx -
 *     LAG ID. Range: 0 - 255.
 *   SOC_SAND_IN  SOC_PPC_LAG_MEMBER                          *member -
 *     System port to be added as a member, and the
 *     member-index.
 *   SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success -
 *     Whether the operation succeeds (upon add). Add operation
 *     may fail if there is no place in the SA Auth DB.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_member_add_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                          *member,
    SOC_SAND_OUT SOC_SAND_SUCCESS_FAILURE                      *success
  );

uint32
  arad_pp_lag_member_add_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                          *member
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_member_remove_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Remove a system port from a LAG.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  lag_ndx -
 *     LAG ID. Range: 0 - 255.
 *   SOC_SAND_IN  uint32                                  sys_port -
 *     System port to be removed as a member.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_member_remove_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                     *member
  );

uint32
  arad_pp_lag_member_remove_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  lag_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER                     *member
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_global_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the LAG hashing global attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info -
 *     LAG Hashing global settings
 * REMARKS:
 *   The hashing function result is 8 bit value. The TM use
 *   the value to choose the LAG port member, to which the
 *   packet is sent
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );

uint32
  arad_pp_lag_hashing_global_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );

uint32
  arad_pp_lag_hashing_global_info_get_verify(
    SOC_SAND_IN  int                                 unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lag_hashing_global_info_set_unsafe" API.
 *     Refer to "arad_pp_lag_hashing_global_info_set_unsafe" API
 *     for details.
*********************************************************************/
uint32
  arad_pp_lag_hashing_global_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT SOC_PPC_LAG_HASH_GLOBAL_INFO                *glbl_hash_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_port_info_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the LAG hashing per-lag attributes
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                  port_ndx -
 *     In-PP Port ID
 *   SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info -
 *     LAG Hashing per-ingress port settings
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_hashing_port_info_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  port_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

uint32
  arad_pp_lag_hashing_port_info_set_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_ndx,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

uint32
  arad_pp_lag_hashing_port_info_get_verify(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  uint32                                  port_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lag_hashing_port_info_set_unsafe" API.
 *     Refer to "arad_pp_lag_hashing_port_info_set_unsafe" API
 *     for details.
*********************************************************************/

uint32
  arad_pp_lag_hashing_port_info_get_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  int                                 core_id,
    SOC_SAND_IN  uint32                                  port_ndx,
    SOC_SAND_OUT SOC_PPC_LAG_HASH_PORT_INFO                  *lag_hash_info
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_port_lb_profile_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set LB-Profile Per port
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  uint32                                 port_ndx -
 *     In-PP Port ID
 *   SOC_SAND_IN  uint32                                 lb_profile -
 *     LAG Hashing per-ingress port settings
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/

int
  arad_pp_lag_hashing_port_lb_profile_set(
    SOC_SAND_IN  int                            unit,
    SOC_SAND_IN  int                            core,
    SOC_SAND_IN  uint32                         pp_port,
    SOC_SAND_IN  uint32                         lb_profile
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_mask_set_unsafe
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Sets the key used by hash functions for LAG/ECMP load
 *   balancing.
 * INPUT:
 *   SOC_SAND_IN  int                  unit -
 *     Identifier of the device to access.
 *   SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO       *mask_info -
 *     how to build the key used as input by hash functions for
 *     LAG/ECMP load balancing
 * REMARKS:
 *   This setting is mutual to the ECMP hashing function
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_set_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_hashing_mask_set_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_hashing_mask_get_verify(
    SOC_SAND_IN  int                  unit
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "arad_pp_lag_hashing_mask_set_unsafe" API.
 *     Refer to "arad_pp_lag_hashing_mask_set_unsafe" API for
 *     details.
*********************************************************************/
uint32
  arad_pp_lag_hashing_mask_get_unsafe(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_OUT SOC_PPC_HASH_MASK_INFO       *mask_info
  );

uint32
  arad_pp_lag_lb_key_range_set_unsafe(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO                            *lag_info);


uint32
  arad_pp_lag_hash_func_to_hw_val(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint8         hash_func_id,
    SOC_SAND_OUT  uint32        *hw_val
  );

uint32
  arad_pp_lag_hash_func_from_hw_val(
    SOC_SAND_IN  int       unit,
    SOC_SAND_IN  uint32        hw_val,
    SOC_SAND_OUT  uint8        *hash_func_id
  );

/*********************************************************************
* NAME:
 *   arad_pp_lag_hashing_ecmp_hash_slb_combine
 * FUNCTION:
 *    Allows to globally use the configured LB result as a seed for
 *    the Stateful LB (the ECMP group must be Stateful)
 * INPUT:
 *   int                  unit -
 *     Identifier of the device to access.
 *   int                  combine_slb -
 *     A flag indication whether to take the configured LB result as a seed
 *     for the SLB or not.
 * REMARKS:
 *    This bit is only for Stateful LB ECMP.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
arad_pp_lag_hashing_ecmp_hash_slb_combine_set(
   int            unit,
   int            combine_slb
);

uint32
arad_pp_lag_hashing_ecmp_hash_slb_combine_get(
   int            unit,
   int            *combine_slb
);

/*********************************************************************
* NAME:
 *   arad_pp_lag_get_procs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of procedures of the
 *   arad_pp_api_lag module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_PROCEDURE_DESC_ELEMENT*
  arad_pp_lag_get_procs_ptr(void);

/*********************************************************************
* NAME:
 *   arad_pp_lag_get_errs_ptr
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the pointer to the list of errors of the
 *   arad_pp_api_lag module.
 * INPUT:
 * REMARKS:
 *
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
CONST SOC_ERROR_DESC_ELEMENT*
  arad_pp_lag_get_errs_ptr(void);

uint32
  SOC_PPC_LAG_HASH_GLOBAL_INFO_verify(
    SOC_SAND_IN  int                        unit,
    SOC_SAND_IN  SOC_PPC_LAG_HASH_GLOBAL_INFO *info
  );

uint32
  SOC_PPC_LAG_HASH_PORT_INFO_verify(
    SOC_SAND_IN  SOC_PPC_LAG_HASH_PORT_INFO *info
  );

uint32
  SOC_PPC_HASH_MASK_INFO_verify(
    SOC_SAND_IN  SOC_PPC_HASH_MASK_INFO *info
  );

uint32
  SOC_PPC_LAG_MEMBER_verify(
    SOC_SAND_IN  SOC_PPC_LAG_MEMBER *info
  );

uint32
  SOC_PPC_LAG_INFO_verify(
    SOC_SAND_IN  int                  unit,
    SOC_SAND_IN  SOC_PPC_LAG_INFO *info
  );

uint32
  arad_pp_lag_init_polynomial_for_tm_mode(
    SOC_SAND_IN int unit
  );

soc_error_t
  soc_jer_pp_lag_get_load_balancing_diagnostic_signal(
    SOC_SAND_IN    int  unit,
    SOC_SAND_INOUT ARAD_PP_LB_PRINT_SIGNALS* lb_signals
  );

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __ARAD_PP_LAG_INCLUDED__*/
#endif


