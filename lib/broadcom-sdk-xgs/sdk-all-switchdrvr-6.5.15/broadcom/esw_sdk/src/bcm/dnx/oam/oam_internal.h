/** \file oam_internal.h
 * $Id$
 *
 * General OAM - contains internal functions and definitions for
 * OAM feature support
 *
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef INTERNAL_OAM_H_INCLUDED
/*
 * {
 */
#define INTERNAL_OAM_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include "include/bcm/oam.h"
#include <bcm_int/dnx/algo/algo_gpm.h>
/*
 * }
 */

/** MAC size in bytes */
#define DNX_OAM_MAC_SIZE 6

/**
 * defines of prefix_ids for ease of use
 */
#define PREFIX_HIGH DBAL_ENUM_FVAL_OAM_KEY_PREFIX_HIGH
#define PREFIX_MID DBAL_ENUM_FVAL_OAM_KEY_PREFIX_MID
#define PREFIX_LOW DBAL_ENUM_FVAL_OAM_KEY_PREFIX_LOW
#define NOF_OAM_PREFIXES 3

/** 
 *  Counter interface to be used for MEPs without counter
 */
#define INVALID_COUNTER_IF DBAL_NOF_ENUM_OAM_LM_COUNTER_READ_IDX_SEL_VALUES

/*
 * Flags for the dnx_oam_acc_mep_lookup and
 *  dnx_oam_acc_mep_store functions
 */

/** Access the ingress tables, as opposed to egress tables */
#define DNX_OAM_CLASSIFIER_INGRESS 0x1
/** Access the egress tables, as opposed to ingress tables */
#define DNX_OAM_CLASSIFIER_EGRESS 0x0
/** Access both sides */
#define DNX_OAM_CLASSIFIER_BOTH_SIDES 0x2
/** Update flag */
#define DNX_OAM_CLASSIFIER_UPDATE 0x4
/** BFD flag */
#define DNX_OAM_CLASSIFIER_BFD 0x8

/** 
 *  For action table access, add or read entry in theb LIF
 *  action table (OAMA)
 */
#define DNX_OAM_CLASSIFIER_LIF_ACTION 0x2
/** 
 *  For action table access, add or read entry in the
 *  accelerated MEP action table (OAMB)
 */
#define DNX_OAM_CLASSIFIER_ACC_MEP_ACTION 0x4

/** General OAM constants   */
#define DNX_OAM_MAX_MDL 7

/** Maximum counter index */
#define DNX_MAX_COUNTER_RANGE 0x000fffff

/** For Jericho 2, Ingress Trap Code is 9 bits */
#define DNX_OAM_INGRESS_TRAP_CODE_BITS 9
#define DNX_OAM_MAX_INGRESS_TRAP_CODE SAL_UPTO_BIT(DNX_OAM_INGRESS_TRAP_CODE_BITS)

/** For Jericho 2, Egress Trap Code is 3 bits */
#define DNX_OAM_EGRESS_ACTION_PROFILE_BITS 3
#define DNX_OAM_MAX_EGRESS_ACTION_PROFILE SAL_UPTO_BIT(DNX_OAM_EGRESS_ACTION_PROFILE_BITS)

/** For Jericho 2, Forwarding Strength is 4 bits */
#define DNX_OAM_FORWARDING_STRENGTH_BITS 4
#define DNX_OAM_MAX_FORWARDING_STRENGTH SAL_UPTO_BIT(DNX_OAM_FORWARDING_STRENGTH_BITS)

/** For Jericho 2, Snoop Strength is 3 bits */
#define DNX_OAM_SNOOP_STRENGTH_BITS 3
#define DNX_OAM_MAX_SNOOP_STRENGTH SAL_UPTO_BIT(DNX_OAM_SNOOP_STRENGTH_BITS)

#define MAX_BOOL_VALUE 1

#define DNX_OAM_INVALID_LIF DNX_ALGO_GPM_LIF_INVALID

/** Jericho2 OAM profile defines and macros */
#define DNX_OAM_PROFILE_IDX_SHIFT   0
#define DNX_OAM_PROFILE_IDX_MASK    0xf
#define DNX_OAM_PROFILE_TYPE_SHIFT  24
#define DNX_OAM_PROFILE_TYPE_MASK   0x3
#define DNX_OAM_PROFILE_VALID_SHIFT 31
#define DNX_OAM_PROFILE_VALID_MASK  0x1

/*
 * Returns the actual profile.
 */
#define DNX_OAM_PROFILE_IDX_GET(profile_id) \
            (((profile_id) >> DNX_OAM_PROFILE_IDX_SHIFT) & DNX_OAM_PROFILE_IDX_MASK)

/*
 * Returns one of the bcm_oam_profile_type_t enums.
 */
#define DNX_OAM_PROFILE_TYPE_GET(profile_id) \
            (((profile_id) >> DNX_OAM_PROFILE_TYPE_SHIFT) & DNX_OAM_PROFILE_TYPE_MASK)

/*
 *  Check the "valid" bit.
 */
#define DNX_OAM_PROFILE_VALID_GET(profile_id) \
            (((profile_id) >> DNX_OAM_PROFILE_VALID_SHIFT) & DNX_OAM_PROFILE_VALID_MASK)

/*
 * Given the actual profile and the profile type (an enum from bcm_oam_profile_type_t), 
 * return an encoded profile, including the valid bit.
 */
#define DNX_OAM_PROFILE_SET(profile_idx, profile_type) \
            (((profile_idx & DNX_OAM_PROFILE_IDX_MASK) << DNX_OAM_PROFILE_IDX_SHIFT) | \
            ((profile_type & DNX_OAM_PROFILE_TYPE_MASK) << DNX_OAM_PROFILE_TYPE_SHIFT) | \
            (1 << DNX_OAM_PROFILE_VALID_SHIFT))

/* Is endpoint_info struct represents an accelerated endpoint */
#define DNX_OAM_DISSECT_IS_ENDPOINT_ACCELERATED(p_endpoint_info) \
                          (((p_endpoint_info)->opcode_flags & BCM_OAM_OPCODE_CCM_IN_HW) == BCM_OAM_OPCODE_CCM_IN_HW)

/* Is endpoint_info struct represents an up-MEP */
#define DNX_OAM_DISSECT_IS_ENDPOINT_UPMEP(p_endpoint_info) \
                          (((p_endpoint_info)->flags & BCM_OAM_ENDPOINT_UP_FACING) == BCM_OAM_ENDPOINT_UP_FACING)

/* Is endpoint_info struct represents a MIP */
#define DNX_OAM_DISSECT_IS_ENDPOINT_MIP(p_endpoint_info) \
                          (((p_endpoint_info)->flags & BCM_OAM_ENDPOINT_INTERMEDIATE) == BCM_OAM_ENDPOINT_INTERMEDIATE)

/* Is endpoint_info struct represents an up-MEP */
#define DNX_OAM_DISSECT_IS_ENDPOINT_PCP_BASED(p_endpoint_info) \
                          (((p_endpoint_info)->lm_flags & BCM_OAM_LM_PCP) == BCM_OAM_LM_PCP)

/* Is endpoint_info struct enables TX statistics */
#define DNX_OAM_DISSECT_IS_ENDPOINT_ENABLED_TX_STATISTICS(p_endpoint_info) \
                          (((p_endpoint_info)->flags2 & BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS) == BCM_OAM_ENDPOINT_FLAGS2_TX_STATISTICS)
/* Is endpoint_info struct enables RX statistics */
#define DNX_OAM_DISSECT_IS_ENDPOINT_ENABLED_RX_STATISTICS(p_endpoint_info) \
                          (((p_endpoint_info)->flags2 & BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS) == BCM_OAM_ENDPOINT_FLAGS2_RX_STATISTICS)
/* Is endpoint_info struct enables per opcode statistics */
#define DNX_OAM_DISSECT_IS_ENDPOINT_ENABLED_PER_OPCODE_STATISTICS(p_endpoint_info) \
                          (((p_endpoint_info)->flags2 & BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS) == BCM_OAM_ENDPOINT_FLAGS2_PER_OPCODE_STATISTICS)
                          /* Is endpoint_info struct enables per opcode statistics */
#define DNX_OAM_DISSECT_IS_RX_DIRECTION(direction) \
                          ((direction & bcmOamControlOampStatsRxOpcode) == bcmOamControlOampStatsRxOpcode)
/* Is endpoint_info struct represents an down mep egress injection */
#define DNX_OAM_DISSECT_IS_DOWN_MEP_EGRESS_INJECTION(p_endpoint_info) \
                          (((p_endpoint_info)->flags2 & BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN) == BCM_OAM_ENDPOINT_FLAGS2_EGRESS_INJECTION_DOWN)


/** OAM group MD format of non accelerated endpoints */
#define DNX_OAM_GROUP_NON_ACCELERATED_MD_FORMAT 0

/** Constants for MEP_ID */
#define DNX_OAM_REMOTE_MEP_INDEX_BIT 25
#define DNX_OAM_UNACCELERATED_MEP_UPMEP_INDEX_BIT 27
#define DNX_OAM_UNACCELERATED_MEP_MDL_INDEX_BIT 28
#define DNX_OAM_DOWN_MEP_LEVEL_0_NON_ACC_BIT 22

/* Remote MEP index calcuation */
#define DNX_OAM_REMOTE_MEP_INDEX_FROM_INTERNAL(rmep_id_internal, endpoint)  \
    (endpoint = rmep_id_internal + (1 << DNX_OAM_REMOTE_MEP_INDEX_BIT))

/* Calculate internal remote MEP index from external endpoint ID */
#define DNX_OAM_REMOTE_MEP_INDEX_TO_INTERNAL(rmep_id_internal, endpoint)  \
    (rmep_id_internal = endpoint & 0xFFFFFF)

/* Is the endpoint a REMOTE endpoint ? */
#define DNX_OAM_MEP_INDEX_IS_REMOTE(endpoint)  \
    (((endpoint >> DNX_OAM_REMOTE_MEP_INDEX_BIT) & 1) == 1)

/* Packet type definitions */
#define DNX_OAM_PACKET_IS_OAM 1
#define DNX_OAM_PACKET_IS_DATA 0

/* Data counting definitions */
#define DNX_OAM_ENABLE_COUNTING 1
#define DNX_OAM_DISABLE_COUNTING 0

/* Extracting the MSB(24b) and LSB(24b) parts of input 
 * MAC DA(bcm_mac_t) into output bcm_mac_t  structures. 
 */
#define GET_LSB_FROM_MAC_ADDR(mac_addr, out_mac_addr) \
   do\
   {\
        out_mac_addr[0] = 0;\
        out_mac_addr[1] = 0;\
        out_mac_addr[2] = 0;\
        out_mac_addr[3] = mac_addr[3];\
        out_mac_addr[4] = mac_addr[4];\
        out_mac_addr[5] = mac_addr[5];\
   } while(0)

#define GET_MSB_FROM_MAC_ADDR(mac_addr, out_mac_addr) \
   do\
   {\
        out_mac_addr[5] = 0;\
        out_mac_addr[4] = 0;\
        out_mac_addr[3] = 0;\
        out_mac_addr[2] = mac_addr[2];\
        out_mac_addr[1] = mac_addr[1];\
        out_mac_addr[0] = mac_addr[0];\
   } while(0)

#define GET_MAC_FROM_MSB_LSB(msb_mac_addr, lsb_mac_addr, out_mac_addr) \
   do\
   {\
        out_mac_addr[5] = lsb_mac_addr[5];\
        out_mac_addr[4] = lsb_mac_addr[4];\
        out_mac_addr[3] = lsb_mac_addr[3];\
        out_mac_addr[2] = msb_mac_addr[2];\
        out_mac_addr[1] = msb_mac_addr[1];\
        out_mac_addr[0] = msb_mac_addr[0];\
   } while(0)

/**
 * Offsets are calcualted in the following way:
 * ETH: DA, SA addresss (12B) + num-tags (taken from mep db) * 4B
 *        + Ethertype (2B) + offset in OAM PDU (4B).
 *
 * For piggy backed CCMs an extra 54 bytes are added
 *          4B (sequence #) + 2B (MEP-ID) + 48B (MEG-ID)
 * 
 * MPLS: GAL + GACH + MPLS/PWE label (4B each) + offset in OAM PDU (4B).
 * 
 * PWE: GACH, PWE label, offset in OAM PDU (4 byte each)
 * 
 * For SLM offset increases by 8 bytes 
 *    (Source MEP ID (2B) + Responder MEP ID (2B) + TEST ID (4B))
 *  
 */
#define DNX_OAM_SET_LMM_OFFSET(mep_type, num_vlan_tags, is_piggy_back, is_slm, value) \
    do {\
        if (mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM) {\
            value = (num_vlan_tags *4) + 18; \
        } else if (mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP) {\
            value = 16;\
        } else if (mep_type == DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE) {\
            value = 12;\
        }\
        if (is_piggy_back) {\
            value += 54;\
        } else if (is_slm) {\
            value += 8;\
        }\
    }while (0)

/* For SLR the stamping is done 4 bytes after the SLM stamping,
 * For LMR 8 bytes after LMM stamping.
 */
#define DNX_OAM_SET_LMR_OFFSET(mep_type, num_vlan_tags, is_piggy_back, is_slm, value) \
    do {\
        DNX_OAM_SET_LMM_OFFSET(mep_type, num_vlan_tags, is_piggy_back, is_slm, value);\
        value += 4;\
        if (!is_slm) {\
            value +=4;\
        }\
    }while (0)

/* DMM offset is same as LMM offset */
#define DNX_OAM_SET_DMM_OFFSET(mep_type, num_vlan_tags, value) \
    DNX_OAM_SET_LMM_OFFSET(mep_type, num_vlan_tags, 0, 0, value)

/* DMR stamping is 16 bytes after DMM stamping offset */
#define DNX_OAM_SET_DMR_OFFSET(mep_type, num_vlan_tags, value) \
    do {\
        DNX_OAM_SET_DMM_OFFSET(mep_type, num_vlan_tags, value);\
        value +=16;\
     } while (0)

/* Callbacks are not supported by warmboot. Need to re-register after warmboot */
bcm_oam_event_cb _g_oam_event_cb[BCM_MAX_NUM_UNITS][bcmOAMEventCount];

/*User Data*/
void *_g_oam_event_ud[BCM_MAX_NUM_UNITS][bcmOAMEventCount];

/*
 * Key of the physical oam action tables. 
 * To be used by dnx_oam_action_set, dnx_oam_action_get
 */
typedef struct
{
    /**  is_inject - Signals that processed packet is injected packet */
    uint32 is_inject;
    /**  oam_internal_opcode - internal 4 bit OAM opcode chooses one of the opcodes that are actually used */
    dbal_enum_value_field_oam_internal_opcode_e oam_internal_opcode;
    /**   is_my_mac - Signals that MAC equals MEP's MAC */
    uint32 is_my_mac;
    /** 
     *  mp_type - Type of the packet in relation to existing MEPs
     *  on most inner LIF.  Not relevant for egress accelerated
     *  MEP action table (EGRESS_OAM_ACC_MEP_ACTION_DB)
     */
    dbal_enum_value_field_mp_type_e mp_type;
    /** mp_profile - Represent a set of actions to be taken for packet with OAM lif */
    uint32 mp_profile;
    /** da_is_mc - Signals MultiCast packet */
    uint32 da_is_mc;
} oam_action_key_t;

/*
 * Entry of the physical oam action tables. 
 * To be used by dnx_oam_action_set, dnx_oam_action_get
 */
typedef struct
{
   /** 
    *  To make it clear that there are ingress only fields and
    *  egress only fields, we place them in a union
    */
    union
    {
        struct
        {
           /** 
            *  ingress_trap_code - Represents mate traps for ingress
            *  predefined traps - for ingress tables only
            */
            uint32 ingress_trap_code;
            /** 
             *  Signals that an UP-MEP as opposed to DOWN-MEP was hit.
             *  Relevant only for ingress tables.
             */
            uint32 is_up_mep;
            /** Meter Disable - releavnt for ingress tables only */
            uint32 meter_disable;
        } ingress_only_fields;
        struct
        {
            /** 
             * egress_action_profile - Represents egress
             * predefined actions for egress tables only 
             */
            uint32 egress_action_profile;
        } egress_only_fields;
    } table_specific_fields;

    /** 
     *  forwarding_strength - Represents forwarding strength for
     *  mate trap; relevant for all tables
     */
    uint32 forwarding_strength;
    /** 
     *  snoop_strength - Represents Snoop Strength for mate trap;
     *  relevant for all tables
     */
    uint32 snoop_strength;
    /** 
     *  oam_sub_type - OAM sub-type that chooses
     *  encapsulation actions for the packet
     *  Relevant for all tables
     */
    dbal_enum_value_field_oam_sub_type_e oam_sub_type;
    /** 
     *  oama_counter_disable_access_0/1/2 - A single bit that
     *  disables the counter from the first / second / third
     *  OAM_LIF_DB lookup. Relevant only for LIF action tables
     *  (OAMA)
     */
    uint32 oama_counter_disable_access_0;
    uint32 oama_counter_disable_access_1;
    uint32 oama_counter_disable_access_2;
    /** 
     *  oamb_counter_disable - A single bit that disables the
     *  counter for accelerated MEPs.  Relevant only for accelerated
     *  MEP action tables (OAMB)
     */
    uint32 oamb_counter_disable;
} oam_action_content_t;

/**
 * Structures used to access (read or write) from dbal tables 
 * INGRESS_OAM_LIF_DB and EGRESS_OAM_LIF_DB
 */

/** 
 *  List of key fields used in dbal tables INGRESS_OAM_LIF_DB
 *  amd EGRESS_OAM_LIF_DB.  This struct is used (by pointer) as
 *  input in both dnx_oam_lif_add and dnx_oam_lif_get functions.
 */
typedef struct
{
    /**
     * ID of core to which accessed table belongs.  Current possible values: 
     * 0, 1, or _SHR_CORE_ALL (to access both tables.)
     */
    int core_id;

    /**
     * Flags used for accessing table.  Currently on one flag in use: 
     * DNX_OAM_CLASSIFIER_INGRESS - if on access ingress table(s), 
     * if off - egress
     */
    uint32 flags;

    /**
     * Allows one LIF to have multiple entries in the table - up to 
     * 3, corresponding to the 3 counters and 3 table accesses the 
     * hardware supports.  Possible values: 0, 1, 2 
     */
    uint32 oam_key_prefix;

    /**
     * ID of LIF, selected from 3 possible LIFs in the key-select stage. 
     * For ingress, the number of bits allotted for this field 
     * DNX_OAM_INGRESS_LIF_KEY_BASE_BITS.  For egress, the  number of 
     * bits is DNX_OAM_EGRESS_LIF_KEY_BASE_BITS.
     */
    uint32 oam_lif_id;
} dnx_oam_lif_entry_key_t;

/** 
 *  List of value fields used in dbal tables INGRESS_OAM_LIF_DB
 *  amd EGRESS_OAM_LIF_DB.  This struct is used (by pointer) as
 *  input for the dnx_oam_lif_add function, and as output for
 *  the dnx_oam_lif_get function.
 */
typedef struct
{
    /**
     * An array 8 2-bit elements corresponding to the 8 
     * possible MDLs.  mp_map[0] corresponds to MDL 0, 
     * mp_map[1] corresponds to MDL 1, etc.  The type 
     * of an element in the array is automatically generated 
     * enum. 
     */
    dbal_enum_value_field_mdl_mp_type_e mdl_mp_type[DNX_OAM_MAX_MDL + 1];

    /**
     * Part of the key used when accessing the LIF-Action OAMA
     * table, to determine further processing actions (trapping, 
     * snooping.) 
     */
    uint32 mp_profile;

    /*
     * ID of the counter to access
     */
    uint32 counter_base;

    /**
     * Number of counter access for this LIF.  This number 
     * indicates the hardware device that is supposed to 
     * access the counter.  Possible values: 0, 1, 2.  The 
     * user has to make sure that for any OAM packet received, 
     * of the maximum 3 entries read, no two entries will have 
     * the same counter_base value (hardware limitation.) 
     */
    uint8 counter_interface;
} dnx_oam_lif_entry_value_t;

/** 
 *  List of value fields that reflect the oam_lif information in
 *  sw_state
 */
typedef struct
{
    /** mp_profile assigned to lif */
    uint32 mp_profile;
    /** Array of endpoint's id arranged by their level */
    uint32 endpoint_id[DNX_OAM_MAX_MDL + 1];
} dnx_oam_lif_info_t;

/** 
 *  List of value fields that reflect the endpoint information
 *  in sw_state
 */
typedef struct
{
   /** Convergence of all flags of endpoint (FLAGS, FLAGS2,
    *  OPCODE_FLAGS, LM_FLAGS) */
    uint32 sw_state_flags;
    /** Endpoint type (ETH / MPLS / PWE) */
    bcm_oam_endpoint_type_t type;
    /** Endpoint group ID */
    uint32 group;
    /** Endpoint MDL */
    uint8 level;
    /** Endpoint gport */
    uint32 gport;
    /** Endpoint counter pointer   */
    uint32 lm_counter_idx;
    /** Endpoint counter interface   */
    uint32 lm_counter_if;
    /** Endpoint out Gport for MPLS MEPs   */
    uint32 mpls_out_gport;
} dnx_oam_endpoint_info_t;

/** 
 *  List of value fields that reflect the REMOTE endpoint information
 *  in sw_state DBAL for remote endpoints
 */
typedef struct
{
    uint16 rmep_id;
    uint16 oam_id;
} dnx_oam_remote_endpoint_info_t;

/**
 * \brief 
 * MEP direction, resolved from MEP info stored in sw_state
 */
typedef enum mep_direction_e
{
    INVALID_MEP_DIRECTION = -1,
    NO_MEP = 0,
    MEP_DIRECTION_IS_DOWN = 1,
    MEP_DIRECTION_IS_UP = 2,
    /*
     * Both upmep and downmep is defined with the same level
     */
    MEP_DIRECTION_IS_BOTH_DIRECTIONS = 3,
    /*
     *  MIP is actually two way MEP
     */
    MEP_IS_MIP = 4,
    NUM_MEP_DIRECTION_E
} mep_direction_t;

/** 
 * Lifs assigned to ports 
 */
typedef struct oam_lif_resolution_s
{
   /** in_lif to be used by INGRESS OAM (global_in_lif) */
    int global_in_lif;
   /** in_lif to be used by INGRESS LIF tables (local_in_lif) */
    int local_in_lif;
   /** out_lif to be used by EGRESS OAM and Egress LIF tables
    *  (local__out_lif) */
    int local_out_lif;
    /**
     * The table relevant to the inlif DBAL_TABLE_XXX
     */
    dbal_tables_e inlif_dbal_table_id;
    /**
     * The result type relevant to the inlif DBAL_RESULT_TYPE_XXX
     */
    uint32 inlif_dbal_result_type;
    /**
     * The table relevant to the outlif DBAL_TABLE_XXX
     */
    dbal_tables_e outlif_dbal_table_id;
    /**
     * The result type relevant to the outlif DBAL_RESULT_TYPE_XXX
     */
    uint32 outlif_dbal_result_type;
} oam_lif_resolution_t;

/** 
 *  Configuration of both lifs of port/s,
 *  List of MEPs configured on a lif (both directions). This
 *  struct maps the MEPs on a lif by their level
 */
typedef struct oam_lif_configured_lif_s
{
   /** mp_profile assigned to the lifs for ingress and egress */
    int mp_profile[DNX_OAM_CLASSIFIER_BOTH_SIDES];
   /** direction of the mep. Each level represents both sides */
    mep_direction_t mep_dir[DNX_OAM_MAX_MDL + 1];
   /** counter_base assigned to a mep, must be the same for both
    *  lifs */
    uint32 counter_base[DNX_OAM_MAX_MDL + 1];
   /** counter interface to be used by the mep, must be the same
    *  for both lifs */
    uint8 counter_if[DNX_OAM_MAX_MDL + 1];
    /** level of the modified MEP */
    uint8 new_level;
   /** mdl_mp_types of the mep for ingress and egress */
    dbal_enum_value_field_mdl_mp_type_e mdl_mp_type[DNX_OAM_CLASSIFIER_BOTH_SIDES][DNX_OAM_MAX_MDL + 1];
} oam_lif_configured_lif_t;

/** 
 *  lif-db status as resolved from analysis on configured_lif
 */
typedef struct oam_lif_db_state_s
{
   /** Number of prefixes in lif-db that should be occupied */
    uint8 occupied_prefixes[DNX_OAM_CLASSIFIER_BOTH_SIDES];
   /** flag for prefix is modified */
    uint8 prefix_is_modified[DNX_OAM_CLASSIFIER_BOTH_SIDES][NOF_OAM_PREFIXES];
   /** counter bases in 3 prefixes of lif-db in hw */
    uint32 counter_base_in_prefix[DNX_OAM_CLASSIFIER_BOTH_SIDES][NOF_OAM_PREFIXES];
    /** counter interfaces in 3 prefixes of lif-db in hw */
    uint32 counter_if_in_prefix[DNX_OAM_CLASSIFIER_BOTH_SIDES][NOF_OAM_PREFIXES];
} oam_lif_db_state_t;

/** 
 *  Results of analysis performed on both lif's configuration
 */
typedef struct oam_lif_analysis_results_s
{
   /** number of levels on lif with meps */
    uint8 nof_levels_with_mep;
    /** number of levels on lif with lm-meps */
    uint8 nof_levels_with_lm_mep;
   /** number of levels on lif with mips */
    uint8 nof_levels_with_mip;
   /** levels of the meps on the lif in ascending order */
    uint8 meps_by_level[DNX_OAM_MAX_MDL + 1];
    /** levels of the lm_meps on the lif in ascending order */
    uint8 lm_meps_by_level[DNX_OAM_MAX_MDL];
   /** whether or not the level has a counter assigned */
    uint8 level_is_lm[DNX_OAM_MAX_MDL + 1];
    /** lif_db state as resolved from the analysis */
    oam_lif_db_state_t lif_db;
} oam_lif_analysis_results_t;

/* Callback function type for RMEP linked list traversal */
typedef shr_error_e(
    *dnx_oam_sw_db_lmep_to_rmep_traverse_cb_f) (
    int unit,
    bcm_oam_endpoint_t local_endpoint,
    bcm_oam_endpoint_t remote_endpoint);

/**
 * \brief
 *   Get OAM group entry info in SW table accoridng to group_id
 *   input. the function will set the OAM group info in
 *   group_info according to information in OAM  group SW table.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group- 
 *  ID of the OAM group to get
 * \param [in] group_info- Pointer to
 *   the structure in which we set information about the
 *   group_info retrieved from the SW table.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_group_info_get(
    int unit,
    bcm_oam_group_t group,
    bcm_oam_group_info_t * group_info);

/**
 * \brief - This function reads an entry from the OAM LIF 
 *        database.  These are four tables (ingress and egress,
 *        per each of two cores) that associate the LIF with an
 *        OAM counter, and contain other OAM LIF relevant
 *        information: the existence of OAM MEPs and MIPs on the
 *        LIF and their MDLs, and part of the key for reading
 *        data from another database: LIF-Action OAMA.
 * 
 * \param [in] unit - Identifying number of hardware unit used.
 *  
 * \param [in] oam_lif_entry_key - see definition of struct for
 *          description of fields
 *   
 * \param [out] oam_lif_entry_value - see definition of 
 *          struct for description of fields
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * dnx_oam_lif_add - related procedure
 */
shr_error_e dnx_oam_lif_get(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    dnx_oam_lif_entry_value_t * oam_lif_entry_value);

/**
 * \brief - This function modifies single mdl_mp_type value in a 
 *        given entry in oam_lif_db
 *  
 * \param [in] unit - Identifying number of hardware unit used.
 *
 * \param [in] level - level of the mdl-mp_type to be modified
 *  
 * \param [in] oam_lif_entry_key - see definition of struct for
 *          description of fields
 *   
 * \param [in] oam_lif_entry_value - see definition of 
 *          struct for description of fields
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 */
shr_error_e dnx_oam_lif_modify_mdl_mp_type(
    int unit,
    int level,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    const dnx_oam_lif_entry_value_t * oam_lif_entry_value);

/**
 * \brief - This function writes an entry to the OAM LIF 
 *        database.  These are four tables (ingress and egress,
 *        per each of two cores) that associate the LIF with an
 *        OAM counter, and contain other OAM LIF relevant
 *        information: the existence of OAM MEPs and MIPs on the
 *        LIF and their MDLs, and part of the key for reading
 *        data from another database: LIF-Action OAMA.
 * 
 * \param [in] unit - Identifying number of hardware unit used.
 *
 * \param [in] dbal_oper_flags - Operation to be done. May be 
 *                     either DBAL_COMMIT for adding a new entry
 *                     or, DBAL_COMMIT_UPDATE_ENTERY for
 *                     updating an existing entry
 *  
 * \param [in] oam_lif_entry_key - see definition of struct for
 *          description of fields
 *   
 * \param [in] oam_lif_entry_value - see definition of 
 *          struct for description of fields
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * dnx_oam_lif_get - related procedure
 */
shr_error_e dnx_oam_lif_add(
    int unit,
    dbal_entry_action_flags_e dbal_oper_flags,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key,
    const dnx_oam_lif_entry_value_t * oam_lif_entry_value);

/**
 * \brief - This function deletes an entry from the OAM LIF 
 *        database.  Any entry added with the same key and the
 *        API dnx_oam_lif_add will be deleted here.  See that
 *        API for more details.
 * 
 * \param [in] unit - Identifying number of hardware unit used.
 * \param [in] oam_lif_entry_key - see definition of struct for
 *          description of fields
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_lif_delete(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_entry_key);

/**
 * \brief - Update mp-profile in OAM-LIF-DB entry.Update 
 *          is done per LIF, direction and prefix
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_lif_db_key - The lif to update the profile
 *        for, the specific prefix entry to update
 *        the profile for and the direction.LIF, prefix, 
 *        direction (ing/eg) for which the profile is updated.
 * \param [in] mp_profile - the new profile to be set.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   Following memories are related:
 *   DBAL_TABLE_INGRESS_OAM_LIF_DB
 *   DBAL_TABLE_EGRESS_OAM_LIF_DB
 *
 * \see
 *   bcm_dnx_oam_lif_profile_set
 */
shr_error_e dnx_oam_lif_modify_mp_profile(
    int unit,
    const dnx_oam_lif_entry_key_t * oam_lif_db_key,
    int mp_profile);

/**
 * \brief - This function reads the specified entry from a 
 *        my_cfm_mac table, and returns the first matching
 *        my_cfm_mac address it finds: it is composed of the
 *        fixed MSB of the MAC, and the first LSB it finds that
 *        is marked as "valid."  The data is read from the dbal
 *        tables INGRESS_MY_CFM_MAC_TABLE and
 *        EGRESS_MY_CFM_MAC_TABLE, which map to the physical
 *        tables IPPB_OAM_MY_CFM_MAC and ETPPC_MY_CFM_MAC_TABLE,
 *        respectively.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 * \param [in] core_id - ID of core to which accessed table 
 *        belongs.  Current possible values: 0, 1, or
 *        _SHR_CORE_ALL (to access both tables.)
 * \param [in] port_id - number of configured port
 * \param [out] my_cfm_mac - Resulting MAC address
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_my_cfm_mac_get(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac);

/**
 * \brief - This function adds a MAC address to the my_cfm_mac 
 *        setting of a port.  If the port has none, any adress
 *        is accepted.  If the port has one or more, the MSB of
 *        the MAC must match, and the LSB must not be already
 *        marked as valid.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 * \param [in] core_id - ID of core to which accessed table 
 *        belongs.  Current possible values: 0, 1, or
 *        _SHR_CORE_ALL (to access both tables.)
 * \param [in] port_id - number of configured port
 * \param [in] my_cfm_mac - MAC address to add.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_my_cfm_mac_add(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac);

/**
 * \brief - This function deletes a MAC address from a port's 
 *        my_mac_cfm list.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 * \param [in] core_id - ID of core to which accessed table 
 *        belongs.  Current possible values: 0, 1, or
 *        _SHR_CORE_ALL (to access both tables.)
 * \param [in] port_id - number of configured port
 * \param [in] my_cfm_mac - MAC address to delete.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_my_cfm_mac_delete(
    int unit,
    uint32 flags,
    int core_id,
    uint32 port_id,
    bcm_mac_t my_cfm_mac);

/**
 * \brief - This function looks up the accelerated MEP in the
 *        MEP DB using the previously looked-up LIF, and the
 *        MDL.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Number of hardware unit used.
 *   \param [in] core_id - Core ID which is used to access table
 *          INGRESS_OAM_ACC_MEP_DB or EGRESS_OAM_ACC_MEP_DB
 *   \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 *   \param [in] oam_lif - LIF looked up in the step before this
 *          one.  At the ingress, oam_lif is the global_in_lif.
 *          At the egress, oam_lif is the local_out_lif.
 *   \param [in] mdl - Level of MEP to look up (DNX_OAM_MDL_BITS
 *          bits)
 *   \param [out] mep_id - pointer to which the resulting ID
 *          of MEP field in the entry will be written
 *          (DNX_OAM_MEP_ID_BITS bits)
 *   \param [out] acc_mp_profile - pointer to which the
 *          resulting Accelerated MEP profile field in the entry
 *          will be written
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   Writes to table INGRESS_OAM_ACC_MEP_DB for ingress, and
 *   to EGRESS_OAM_ACC_MEP_DB for egress
 * \see
 *   * None
 */
shr_error_e dnx_oam_acc_mep_get(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl,
    uint32 *mep_id,
    uint32 *acc_mp_profile);

/**
 * \brief - This function writes an accelerated MEP to the MEP
 *          DB.
 *
 * \par DIRECT_INPUT:
 *   \param [in] unit - Number of hardware unit used.
 *   \param [in] core_id - Core ID which is used to access table
 *          INGRESS_OAM_ACC_MEP_DB or EGRESS_OAM_ACC_MEP_DB
 *   \param [in] flags - currently only two in use,
 *          DNX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
            DNX_OAM_CLASSIFIER_BFD: BFD entry add (flag),
            or OAM entry add(no flag).
 *   \param [in] oam_lif - LIF looked up in the step before this
 *          one.  At the ingress, oam_lif is the global_in_lif.
 *          At the egress, oam_lif is the local_out_lif.
 *   \param [in] mdl - Level of MEP to look up (DNX_OAM_MDL_BITS
 *          bits)
 *   \param [in] mep_id - ID of MEP field in entry
 *          (DNX_OAM_MEP_ID_BITS bits)
 *   \param [in] acc_mp_profile - Accelerated MEP profile field
 *          in entry (DNX_OAM_ACC_MP_PROFILE_BITS bits)
 *
 * \par INDIRECT INPUT:
 *   * None
 * \par DIRECT OUTPUT:
 *   shr_error_e
 * \par INDIRECT OUTPUT
 *   * None
 * \remark
 *   Reads from table INGRESS_OAM_ACC_MEP_DB for ingress, and
 *   from EGRESS_OAM_ACC_MEP_DB for egress
 * \see
 *   * None
 */
shr_error_e dnx_oam_acc_mep_add(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl,
    int mep_id,
    int acc_mp_profile);

/**
 * \brief - This function deletes a MEP from the accelerated MEP 
 *        DB.  The entry added with these parameters using the
 *        API dnx_oam_acc_mep_add will be deleted here.
 * 
 *   \param [in] unit - Number of hardware unit used.
 *   \param [in] core_id - Core ID which is used to access table
 *          INGRESS_OAM_ACC_MEP_DB or EGRESS_OAM_ACC_MEP_DB
 *   \param [in] flags - currently only one in use,
 *          DMX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *          (flag), or egress DB lookup (no flag)
 *   \param [in] oam_lif - LIF looked up in the step before this
 *          one.  At the ingress, oam_lif is the global_in_lif.
 *          At the egress, oam_lif is the local_out_lif.
 *   \param [in] mdl - Level of MEP to look up (DNX_OAM_MDL_BITS
 *          bits)
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_acc_mep_delete(
    int unit,
    int core_id,
    uint32 flags,
    int oam_lif,
    int mdl);

/**
 * \brief - This function writes a LIF action to OAMA, an ACC
 *        MEP action to OAMB, or both.  See remark for more
 *        details.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently  in use:\n
 *        DNX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *        (flag), or egress DB lookup (no flag)\n
 *        DNX_OAM_CLASSIFIER_LIF_ACTION: Add an entry
 *        to the LIF action table (OAMA)\n
 *        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION: Add an entry to an
 *        the accelerated MEP action table (OAMB)
 * \param [in] action_key - Key of the information to be 
 *        written to OAMA and/or OAMB table.  See structure
 *        definition for fields and relevance to the different
 *        tables.
 * \param [in] action_content - Value of the information to be 
 *        written to OAMA and/or OAMB table.  See structure
 *        definition for fields and relevance to the different
 *        tables.
 *
 * \return shr_error_e - Error indication 
 * \remark This function will access one or two hardware tables,
 *         using one or two dbal tables:
 * For ingress:\n
 * OAMA tables:\n
 *   dbal -  INGRESS_OAM_LIF_ACTION_DB\n
 *   hardware - IPPB_OAMA\n
 * OAMB tables:\n
 *   dbal - INGRESS_OAM_ACC_MEP_ACTION_DB\n
 *   hardware - IPPB_OAMB\n
 * For egress:\n
 * OAMA tables:\n
 *   dbal -  EGRESS_OAM_LIF_ACTION_DB\n
 *   hardware - ETPPC_OAMA\n
 * OAMB tables:\n
 *   dbal - EGRESS_OAM_ACC_MEP_ACTION_DB\n
 *   hardware - ETPPC_OAMB
 */
shr_error_e dnx_oam_action_set(
    int unit,
    uint32 flags,
    const oam_action_key_t * action_key,
    const oam_action_content_t * action_content);

/**
 * \brief - This function reads a LIF action entry from OAMA, an
 *        ACC MEP action entry from OAMB, or both.  See remark
 *        for more details.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently  in use:\n
 *        DNX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *        (flag), or egress DB lookup (no flag)\n
 *        DNX_OAM_CLASSIFIER_LIF_ACTION: Get values from the
 *        entry in the LIF action table (OAMA)\n
 *        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION: Get values from the
 *        entry in the accelerated MEP action table (OAMB)
 * \param [in] action_key - Key of the information to be read
 *        from OAMA and/or OAMB table.  See structure
 *        definition for fields and relevance to the different
 *        tables.
 * \param [out] action_content - The values read from OAMA
 *        and/or OAMB table should be written here.  See structure
 *        definition for fields and relevance to the different
 *        tables.
 *
 * \return shr_error_e - Error indication
 * \remark This function will access one or two hardware tables,
 *         using one or two dbal tables:
 * For ingress:\n
 * OAMA tables:\n
 *   dbal -  INGRESS_OAM_LIF_ACTION_DB\n
 *   hardware - IPPB_OAMA\n
 * OAMB tables:\n
 *   dbal - INGRESS_OAM_ACC_MEP_ACTION_DB\n
 *   hardware - IPPB_OAMB\n
 * For egress:\n
 * OAMA tables:\n
 *   dbal -  EGRESS_OAM_LIF_ACTION_DB\n
 *   hardware - ETPPC_OAMA\n
 * OAMB tables:\n
 *   dbal - EGRESS_OAM_ACC_MEP_ACTION_DB\n
 *   hardware - ETPPC_OAMB
 */
shr_error_e dnx_oam_action_get(
    int unit,
    uint32 flags,
    const oam_action_key_t * action_key,
    oam_action_content_t * action_content);

/**
 * \brief - This function deletes a LIF action entry from the 
 *        OAMA table or an ACC MEP action entry from OAMB.  The
 *        same entry added using this key with the API
 *        dnx_oam_action_set will be deleted here.  See that API
 *        for more details.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] flags - currently  in use:\n
 *        DNX_OAM_CLASSIFIER_INGRESS: ingress DB lookup
 *        (flag), or egress DB lookup (no flag)\n
 *        DNX_OAM_CLASSIFIER_LIF_ACTION: Get values from the
 *        entry in the LIF action table (OAMA)\n
 *        DNX_OAM_CLASSIFIER_ACC_MEP_ACTION: Get values from the
 *        entry in the accelerated MEP action table (OAMB)
 * \param [in] action_key - Key of the information to be read
 *        from OAMA and/or OAMB table.  See structure
 *        definition for fields and relevance to the different
 *        tables.
 *   
 * \return
 *   shr_error_e 
 *   
 * \remark
 *   * For the flags, exactly one of the two flags
 *     DNX_OAM_CLASSIFIER_LIF_ACTION and
 *     DNX_OAM_CLASSIFIER_ACC_MEP_ACTION must be on.
 * \see
 *   * None
 */

shr_error_e dnx_oam_action_clear(
    int unit,
    uint32 flags,
    const oam_action_key_t * action_key);

/**
 * \brief -
 *   Initialize oam key selection Ingress table.
 *   Should be called once at init stage.
 * \author
 *   Aviv Eliyahu (avive)
 * \param [in] unit -
 *   Unit number
 * \return
 *   shr_error_e
 * \remark
 *   Fills IPPB_OAM_KEY_SEL_MEM memory
 * \see
 *   * None
 */
shr_error_e dnx_oam_key_select_ingress_init(
    int unit);

/**
 * \brief -
 *   Initialize oam key selection Egress table.
 *   Should be called once at init stage.
 * \author
 *   Aviv Eliyahu (avive)
 * \param [in] unit -
 *   Unit number
 * \return
 *   shr_error_e
 * \remark
 *   Fills ETPPC_OAM_KEY_TABLE memory
 * \see
 *   * None
 */
shr_error_e dnx_oam_key_select_egress_init(
    int unit);

/**
 * \brief -
 *   Get ingress key select results for a given key
 * \author
 *   Alfi Gavish (alfig)
 * \param [in] unit - Unit number 
 * \param [in] nof_valid_lm_lifs - number of lifs on packet that
 *        are valid for lm
 * \param [in] in_lif_valid_for_lm - is first layer of the 
 *        packetvalid for lm ?
 * \param [in] your_disc_valid - is your discriminator valid
 * \param [in] *key_pre_1 - pointer to key prefix 1(result of 
 *        key select) - prefix for the access number x to
 *        oam-lif-db
 * \param [in] key_base_1 - pointer to key base 1(result of key 
 *        select) - lif for the access number x to oam-lif-db
 * \param [in] *key_pre_2 - pointer to key prefix 2(result of 
 *        key select) - prefix for the access number x to
 *        oam-lif-db
 * \param [in] key_base_2 - pointer to key base 2(result of key 
 *        select) - lif for the access number x to oam-lif-db
 * \param [in] *key_pre_3 - pointer to key prefix 3(result of 
 *        key select) - prefix for the access number x to
 *        oam-lif-db
 * \param [in] key_base_3 - pointer to key base 3(result of key 
 *        select) - lif for the access number x to oam-lif-db
 * \return
 *   shr_error_e
 * \remark
 *   To be used by diagnostics
 * \see
 *   * None
 */
shr_error_e dnx_oam_key_select_ingress_get(
    int unit,
    int nof_valid_lm_lifs,
    int in_lif_valid_for_lm,
    int your_disc_valid,
    uint32 *key_pre_1,
    uint32 *key_pre_2,
    uint32 *key_pre_3,
    uint32 *key_base_1,
    uint32 *key_base_2,
    uint32 *key_base_3);

/**
 * \brief -
 *   Get ingress key select results for a given key
 * \author
 *   Alfi Gavish (alfig)
 * \param [in] unit - Unit number 
 * \param [in] nof_valid_lm_lifs - number of lifs on packet that
 *        are valid for lm
 * \param [in] counter_ptr_valid - is first layer of the 
 *        packetvalid for lm ?
 * \param [in] packet_is_oam - Set for oam packet
 * \param [in] oam_inject - Set for injected packet
 * \param [in] *key_pre_1 - pointer to key prefix 1(result of 
 *        key select) - prefix for the access number x to
 *        oam-lif-db
 * \param [in] key_base_1 - pointer to key base 1(result of key 
 *        select) - lif for the access number x to oam-lif-db
 * \param [in] *key_pre_2 - pointer to key prefix 2(result of 
 *        key select) - prefix for the access number x to
 *        oam-lif-db
 * \param [in] key_base_2 - pointer to key base 2(result of key 
 *        select) - lif for the access number x to oam-lif-db
 * \param [in] *key_pre_3 - pointer to key prefix 3(result of 
 *        key select) - prefix for the access number x to
 *        oam-lif-db
 * \param [in] key_base_3 - pointer to key base 3(result of key 
 *        select) - lif for the access number x to oam-lif-db
 * \return
 *   shr_error_e
 * \remark
 *   To be used by diagnostics
 * \see
 *   * None
 */
shr_error_e dnx_oam_key_select_egress_get(
    int unit,
    int nof_valid_lm_lifs,
    int counter_ptr_valid,
    int oam_inject,
    int packet_is_oam,
    uint32 *key_pre_1,
    uint32 *key_pre_2,
    uint32 *key_pre_3,
    uint32 *key_base_1,
    uint32 *key_base_2,
    uint32 *key_base_3);

/**
 * \brief -
 * Get oam_counter_disable_map entry
 * 
 * The HW table (IPPB_OAM_COUNTER_DISABLE_MAP) enables each counter 
 * according to nof_valid_lm_lifs, oamb_dis_cnt, in_lif_valid_for_lm, mp_type. 
 *  
 * This function gets an entry from IPPB_OAM_COUNTER_DISABLE_MAP
 * table.
 *  
 * 
 * \param [in] unit -     Relevant unit.
 * \param [in] table -    table name ( may be ingress or egress table )
 * \param [in] nof_valid_lm_lifs -     Number of lifs with valid lm meps (used as key)
 *             nof_valid_lm_lifs = oam_lif_0_valid + oam_lif_1_valid + oam_lif_2_valid
 * \param [in] oamb_dis_cnt -          OAMB dis_cnt bit from acc action table (used as key)
 * \param [in] in_lif_valid_for_lm -   Indication for lm mep on most inner lif (used as key)
 *             in_lif_valid_for_lm = in_lif[0]==OAM_LIF_0
 * \param [in] mp_type -               mp_type (used as key)
 * \param [in] *dis_cnt_1 -            pointer for the received 
 *         value to be written into
 * \param [in] *dis_cnt_2 -            pointer for the received 
 *         value to be written into
 * \param [in] *dis_cnt_3 -            pointer for the received 
 *         value to be written into
 *  
 * \retval Error indication according to shr_error_e enum
 * \remark Entry of the HW table will be set
 *  
 */
shr_error_e dnx_oam_counter_disable_map_entry_get(
    int unit,
    int table,
    int nof_valid_lm_lifs,
    int oamb_dis_cnt,
    int in_lif_valid_for_lm,
    dbal_enum_value_field_mp_type_e mp_type,
    uint32 *dis_cnt_1,
    uint32 *dis_cnt_2,
    uint32 *dis_cnt_3);

/**
 * \brief - 
 * Set the PCP value for oam priority map profile and tc values/
 *  
 * \param [in] unit         -   Relevant unit.
 * \param [in] oam_priority_map - 2 bit value that 
 *                              defines the priority map.
 *                              Defined by
 *                              EGRESS_OAM_PRIORITY_MAP_PROFILE
 *                              table.
 * \param [in] oam_tc - 3 bit traffic class value
 * \param [in] oam_pcp - 3 bit PCP (oam priority) value
 *  
 * \return error indication.
 *  
 * \remark 
 *  
 * Counter Resolution: 
 *  
 *  Lm_counter_index[counter_if] = counter_base[counter_if] +
 *                                 counter_pointer[counter_if]<<shift_value[counter_if] +
 *                                 use_priority[counter_if]*priority[counter_if] *  
 */
shr_error_e dnx_oam_priority_map_set(
    int unit,
    uint32 oam_priority_map,
    uint32 oam_tc,
    uint32 oam_pcp);

/**
 * \brief - 
 * Get mp_type value from mp_type table
 *  
 * \param [in] unit         -   Relevant unit. 
 * \param [in] is_ingress  - 1 for ingress table, 0 for egress 
 *        table
 * \param [in] nof_mep_above_mdl - How many meps in lif are 
 *        have higher level than incoming oam packet
 * \param [in] nof_mep_below_mdl - How many meps in lif are 
 *        have lower level than incoming oam packet
 * \param [in] mdl_mp_type - mp_type of the incoming packet
 * \param [in] packet_is_bfd  - 1 for bfd packet, 0 otherwise
 * \param [in] mip_above_mdl  - 1 if there is mip on lif, with 
 *        higher level than incoming packet
 * \param [in] *mp_type  - pointer to mp_type (table's value for 
 *        the given key)
 *  
 * \return error indication.
 *  
 */
shr_error_e dnx_oam_mp_type_get(
    int unit,
    uint32 is_ingress,
    uint32 nof_mep_above_mdl,
    uint32 mdl_mp_type,
    uint32 nof_mep_below_mdl,
    uint32 packet_is_bfd,
    uint32 mip_above_mdl,
    uint32 *mp_type);

/**
* \brief
*  Get OAM opcode map.
* \param [in] unit  -
*    Relevant unit.
* \param [in] is_ingress  -
*    Set for ingress side.
* \param [in] opcode  -
*    Opcode to read.
* \param [in] *internal_opcode -
*    Pointer to internal_opcode. The value will be updated by
*    the internal opcode which related to the opcode
* \return Error indication
*/
shr_error_e dnx_oam_opcode_map_get(
    int unit,
    uint32 is_ingress,
    uint8 opcode,
    dbal_enum_value_field_oam_internal_opcode_e * internal_opcode);

/**
 * \brief - This function looks up an oam-lif from the OAM SW 
 *        state LIF table, and reads the data to the struct
 *        referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_lif - oam_lif ID of the lif to retrieve
 *                       global-lif for ingress,
 *                       local-lif for egress.
 * \param [in] is_ingress - side of entry to get.
 * \param [out] lif_info - pointer to struct to return the 
 *             lif information
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_oam_lif_info_get(
    int unit,
    uint32 oam_lif,
    uint8 is_ingress,
    dnx_oam_lif_info_t * lif_info);

/**
 * \brief - This function sets oam-lif info to the OAM SW state 
 *        LIF table
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] dbal_oper_flag - defines the operation type: may 
 *        be DBAL_COMMIT for adding a new entry, or
 *        DBAL_COMMIT_UPDATE_ENTERY for update
 * \param [in] oam_lif - oam_lif ID of the lif to set 
 *                       global-lif for ingress,
 *                       local-lif for egress.
 * \param [in] is_ingress - side of entry to be set.
 * \param [in] lif_info - lif information to be set into 
 *             sw_state 
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_oam_lif_info_set(
    int unit,
    dbal_entry_action_flags_e dbal_oper_flag,
    uint32 oam_lif,
    uint8 is_ingress,
    dnx_oam_lif_info_t * lif_info);

/**
 * \brief - This function looks up an endpoint-info from the OAM
 *        SW state ENDPOINT_INFO table, and reads the data to
 *        the struct referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - id of the endpoint to retrieve
 * \param [out] *sw_endpoint_info - Pointer to temporary 
 *        structure to which data will be written for subsequent
 *        use.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_endpoint_info_get(
    int unit,
    uint32 endpoint_id,
    dnx_oam_endpoint_info_t * sw_endpoint_info);

/**
 * \brief - This function sets an endpoint-info into the OAM SW
 *        state LIF table,
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - id of the endpoint to retrieve
 * \param [in] *sw_endpoint_info - Pointer to temporary 
 *        structure to which data will be written for subsequent
 *        use.
 * \param [in] is_update - TRUE for update, FALSE for new entry.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_endpoint_info_set(
    int unit,
    uint32 endpoint_id,
    dnx_oam_endpoint_info_t * sw_endpoint_info,
    uint8 is_update);

/**
 * \brief - This function deletes an endpoint-info from the OAM 
 *        SW state LIF table,
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - id of the endpoint to retrieve
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_endpoint_info_remove_from_sw_state(
    int unit,
    uint32 endpoint_id);

/**
 * \brief
 *   verify the group is allocated/exsits . The function will
 *   return Zero if no error was detected, Negative if error was
 *   detected.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group -  ID of the OAM group to verify
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_group_verify_is_allocated(
    int unit,
    bcm_oam_group_t group);

/**
 * \brief - 
 *  Get information about the configured lifs.
 *  Information gathered:
 *   - mp_profile[ingress]
 *   - mp_profile[egress]
 *   - counter_base[level] (same for both ingress and egress)
 *   - counter_if[level] (same for both ingress and egress)
 *   - mep_dir[level] : may be up, down, both, or mip
 * 
 * \param [in] unit -     Relevant unit.
 * \param [in] lif_resolution - Structure of lif resolution
 * \param [out] configured_lif - Structure of configured lif 
 *        parameters
 *  
 * \retval Error indication. 
 *  
 */
shr_error_e dnx_oam_lif_configured_lif_get(
    int unit,
    const oam_lif_resolution_t * lif_resolution,
    oam_lif_configured_lif_t * configured_lif);

/**
 * \brief - 
 * Analyze the configured_lif structure to prepare it for 
 * hlm_by_mdl verification and oam_lif_db (hw) management. 
 *  
 *  Following information is resolved for later use in endpoint_create:
 *    - configured_lif including the new MEP:
 *      - mep_dir[level] (new endpoint added)
 *      - counter base[level] (new endpoint added)
 *      - meps_by_level[index] - array of the mep-levels on lif, in ascending order
 *      - meps_level_is_lm[index] - is the mep lm-mep ?
 *      - lm_mep_levels[index] - array of the lm_mep-levels on
 *        lif, in ascending order
 *      - nof_meps
 * 
 * \param [in] unit -     Relevant unit.
 * \param [in] configured_lif - Structure of configured lif
 *        parameters including parameters of the meps allocated
 *        on that lif.
 * \param [out] lif_analysis - results of analysis performed on 
 *        the lifs
 *  
 * \retval Error indication. 
 *  
 */
shr_error_e dnx_oam_analyze_configured_lif(
    int unit,
    const oam_lif_configured_lif_t * configured_lif,
    oam_lif_analysis_results_t * lif_analysis);

/**
 * \brief - Clear dnx_oam_lif_info_t structure
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [out] lif_info - pointer to struct to be cleared
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_lif_info_t_init(
    int unit,
    dnx_oam_lif_info_t * lif_info);

/**
 * \brief
 *  The function initializes the sw db structure for oam group
 *  , which has an array of linked lists. The size of the 
 *  array is also allocated to be the size of number of OAM
 *  groups in device. The index of the array is the group id and
 *  the linked list is the eps related to the group
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_ma_to_mep_create(
    int unit);

/**
 * \brief
 *  The function destroyes the sw db structure and frees the
 *  array
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_ma_to_mep_destroy(
    int unit);

/**
 * \brief
 *   The function adds the OAM EP to relavent OAM group link
 *   list
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_id- 
 *  ID of the OAM group linked list
 * \param [in] ep_id - 
 *  Id of EP to add to linked list
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_ma_to_mep_insert_mep(
    int unit,
    bcm_oam_group_t group_id,
    bcm_oam_endpoint_t ep_id);

/**
 * \brief
 *   The function removes the OAM EP to relavent OAM group sw
 *   linked list
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] group_id- 
 *  ID of the OAM group linked list
 * \param [in] ep_id - 
 *  Id of EP to remove from linked list
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_ma_to_mep_delete_mep(
    int unit,
    bcm_oam_group_t group_id,
    bcm_oam_endpoint_t ep_id);

/**
 * \brief
 *  The function initializes sw db structures for oam
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_init(
    int unit);

/**
 * \brief
 *  The function de-initializes sw db structures for oam
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_deinit(
    int unit);

/*
 * \brief
 * The function sets the Channel value defined at a specified index
 * for BFD PWE
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] index -
 *   Index of the BFD PWE DBAL table
 * \param [in] value -
 *   Channel value to be programmed at that index.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
 */
shr_error_e dnx_oam_bfd_pwe_channel_value_set(
    int unit,
    uint32 index,
    uint32 value);
/*
 * \brief
 * The function sets the Channel value defined at a specified index
 * for Y1731 MPLS TP
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] value -
 *   channel value to be programmed.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
 */
shr_error_e dnx_oam_y1731_mpls_tp_channel_value_set(
    int unit,
    uint32 value);
/*
 * \brief
 * The function sets the Channel value defined at a specified index
 * for Y1731 PWE
 * \param [in] unit -
 *   Relevant unit.
 * \param [in] value -
 *   channel value to be programmed.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
 */
shr_error_e dnx_oam_y1731_pwe_channel_value_set(
    int unit,
    uint32 value);
/*
 * }
 */

/**
 * \brief
 *  The function initializes the sw db structure for oam endpoint
 *  , which has an array of rmep linked lists. The size of the 
 *  array is also allocated to be the size of number of OAM
 *  endpoints in device. The index of the array is the endpoint id and
 *  the linked list is the rmeps for the given endpoint
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_lmep_to_rmep_create(
    int unit);

/**
 * \brief
 *  The function destroyes the OAM endpoint sw db structure and frees the
 *  array
 * \param [in] unit  - 
 *   Relevant unit.
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_lmep_to_rmep_destroy(
    int unit);

/**
 * \brief
 *   The function adds the OAM RMEP to relavent OAM endpoint link
 *   list
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] endpoint_id - 
 *  ID of the OAM endpoint
 * \param [in] rmep_id - 
 *  Id of RMEP to add to linked list
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_lmep_to_rmep_insert_rmep(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_oam_endpoint_t rmep_id);

/**
 * \brief
 *   The function removes the OAM RMEPEP from relavent OAM endpoint sw
 *   linked list
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] endpoint_id - 
 *  ID of the OAM endpoint
 * \param [in] rmep_id - 
 *  Id of RMEP to remove from linked list
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_lmep_to_rmep_delete_rmep(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    bcm_oam_endpoint_t rmep_id);

/**
 * \brief
 *   The function traverses the RMEP linked list and calls the callback function
 *   for each RMEP in the linked list.
 * \param [in] unit  - 
 *   Relevant unit.
 * \param [in] endpoint_id- 
 *  ID of the OAM endpoint
 * \param [in] cb - 
 *  Callback function called for each RMEP in the list.
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see 
 *  * None
*/
shr_error_e dnx_oam_sw_db_lmep_to_rmep_traverse_list(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    dnx_oam_sw_db_lmep_to_rmep_traverse_cb_f cb);

/**
 * \brief
 *   The function empties the OAM RMEP LIST of given LMEP
 * \param [in] unit  -
 *   Relevant unit.
 * \param [in] endpoint_id-
 *  ID of the OAM endpoint
 * \return
 *   \retval Zero if no error was detected
 *   \retval Negative if error was detected.
 * \remark
 *   * None
 * \see
 *  * None
*/
shr_error_e dnx_oam_sw_db_lmep_to_rmep_empty_rmep_list(
    int unit,
    bcm_oam_endpoint_t endpoint_id);

/**
 * \brief - This function looks up an Remote endpoint-info from the OAM
 *        SW state REMOTE_ENDPOINT_INFO table, and reads the data to
 *        the struct referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - Remote MEP HW idx.
 * \param [out] sw_endpoint_info - Pointer to temporary
 *        structure to which data will be written for subsequent
 *        use.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_remote_endpoint_info_get(
    int unit,
    uint32 rmep_idx,
    dnx_oam_remote_endpoint_info_t * sw_endpoint_info);

/**
 * \brief - This function sets an Remote endpoint-info into the OAM
 *        SW state REMOTE_ENDPOINT_INFO table, based on the data in 
 *        the struct referenced by the user provided pointer.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - Remote MEP HW idx.
 * \param [in] sw_endpoint_info - Pointer to temporary
 *        structure from which data will be written for subsequent
 *        use.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_remote_endpoint_info_set(
    int unit,
    uint32 rmep_idx,
    const dnx_oam_remote_endpoint_info_t * sw_endpoint_info);

/**
 * \brief - This function clear an Remote endpoint-info from the OAM
 *        SW state REMOTE_ENDPOINT_INFO table
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] rmep_idx - Remote MEP HW idx.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_state_remote_endpoint_info_delete(
    int unit,
    uint32 rmep_idx);

/**
 * \brief - Main function for Remote MEP create. 
 *          This function calls to sub-functions that
 *          verifies create request and adds the new MEP
 *          to sw and hw
 *  
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested
 *        MEP
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_remote_endpoint_create(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info);

/**
* \brief
*  Get a Remote OAM Endpoint information.
* \param [in] unit  -
*  Relevant unit.
* \param [in] rmep_idx_internal  -
*  Internal remote MEP index
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_oam_remote_endpoint_get(
    int unit,
    int rmep_idx_internal,
    bcm_oam_endpoint_info_t * endpoint_info);

/**
* \brief
*  Destroy a REMOTE OAM Endpoint.
*  
* \param [in] unit  - Relevant unit. 
* \param [in] rmep_idx - Id of the remote endpoint to destroy. 
* \param [in] endpoint_info - Info structure of the endpoint.
* \remark
*   * None
* \see
*   * None
*/
shr_error_e dnx_oam_remote_endpoint_destroy(
    int unit,
    int rmep_idx,
    bcm_oam_endpoint_info_t * endpoint_info);

/**
 * \brief - This function sets an profile_is_slm indication in 
 *        sw_state,
 *  
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mp_profile - profile to be set as lm or slm
 * \param [in] is_ingress  - 1 for ingress table, 0 for egress
 *        table
 * \param [in] is_acc - 1 for accelerated profile, 0 for
 *        non-accelerated
 * \param [in] profile_is_slm - Value to be set: May be 
 *            DNX_OAM_LOSS_IS_LM or DNX_OAM_LOSS_IS_SLM
 * 
 * \return
 *   shr_error_e
 *  
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_db_lm_mode_set(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 profile_is_slm);

/**
 * \brief - This function looks up a profile_is_slm indication 
 *        in sw_state,
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mp_profile - profile of which lm/slm indication 
 *                          is requested
 * \param [in] is_ingress  - 1 for ingress table, 0 for egress
 *        table
 * \param [in] is_acc - 1 for accelerated profile, 0 for
 *        non-accelerated
 * \param [out] profile_is_slm - Value: Will be 
 *            DNX_OAM_LOSS_IS_LM or DNX_OAM_LOSS_IS_SLM
 * 
 * \return
 *    shr_error_e 
 * 
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_db_lm_mode_get(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 *profile_is_slm);

/**
 * \brief - This function sets an profile_is_ntp indication in
 *        sw_state,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mp_profile - profile to be set as DM 1588 or NTP
 * \param [in] is_ingress  - 1 for ingress table, 0 for egress
 *        table
 * \param [in] is_acc - 1 for accelerated profile, 0 for
 *        non-accelerated
 * \param [in] profile_is_ntp - Value to be set: May be
 *            DNX_OAM_LOSS_IS_LM or DNX_OAM_LOSS_IS_SLM
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_db_dm_mode_set(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 profile_is_ntp);

/**
 * \brief - This function looks up a profile_is_ntp indication
 *        in sw_state,
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mp_profile - profile of which DM 1588/NTP indication
 *                          is requested
 * \param [in] is_ingress  - 1 for ingress table, 0 for egress
 *        table
 * \param [in] is_acc - 1 for accelerated profile, 0 for
 *        non-accelerated
 * \param [out] profile_is_ntp - Value: Will be
 *            DNX_OAM_LOSS_IS_LM or DNX_OAM_LOSS_IS_SLM
 *
 * \return
 *    shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_db_dm_mode_get(
    int unit,
    uint32 mp_profile,
    uint8 is_ingress,
    uint8 is_acc,
    uint8 *profile_is_ntp);

/**
 * \brief - This function sets an all profiles to indicate LM 
 *          in profile_is_slm table
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_sw_db_lif_mp_profile_init(
    int unit);

/**
 * \brief - This function configures enabling/disabling 
 *          of data-packet counting 
 *  
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - profile to be set
 * \param [in] is_ingress  - 1 for ingress table, 0 for egress 
 *        table
 * \param [in] enable - 1 to enable data packet counting, 
 *                      0 to disable data packet counting
 * 
 * \return
 *   shr_error_e 
 *   
 * \see
 *   * None
 */
shr_error_e dnx_oam_enable_data_counting(
    int unit,
    int profile_id,
    uint8 is_ingress,
    int enable);

/**
* \brief
*  Utility function to destroy all REMOTE OAM Endpoints
*  associated with a particular LMEP
*
* \param [in] unit  - Relevant unit.
* \param [in] local_id - Id of the local endpoint
*                  whose RMEPs need to be destroyed.
* \return
*   shr_error_e
* \see
*   dnx_oam_endpoint_destroy
*/
shr_error_e dnx_oam_lmep_remote_endpoints_destroy(
    int unit,
    bcm_oam_endpoint_t local_id);

/**
* \brief
*  Destroy all OAM endpoints in OAM group.
* \par DIRECT INPUT:
*   \param [in] unit  -
*   Relevant unit.
*   \param [in] group -
*   ID of the OAM group in which to destroy all endpoints.
* \par INDIRECT INPUT:
*   * none
* \par DIRECT OUTPUT:
*   shr_error_unavail - Return unavailable.
* \par INDIRECT OUTPUT:
*   * None
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_get
*/
shr_error_e bcm_dnx_oam_endpoint_destroy_all(
    int unit,
    bcm_oam_group_t group);

/**
 * \brief - This is the call back function used in sal_dpc call.
 * 
 * \param [in] unit_ptr - pointer to hardware unit used.
 *   
 * \return
 *   void
 */

void dnx_oam_oamp_dma_event_handler(
    void *unit_ptr);


/**
 * \brief - Find a short entry to use for an OAM endpoint
 *          that will work correctly in light of the JR2 A0
 *          hardware bug, where the type used for a short
 *          entry endpoint is always the type in sub-index 0:
 *          either find an entry where sub-index 0 has the
 *          same type, or provide a sub-index 0 entry.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in,out] endpoint_info - info structure of the requested
 *        MEP
 * \param [in] group_name - MAID for this endpoint
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e
dnx_oam_endpoint_workaround_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name);

#endif /* INTERNAL_OAM_INCLUDED */
