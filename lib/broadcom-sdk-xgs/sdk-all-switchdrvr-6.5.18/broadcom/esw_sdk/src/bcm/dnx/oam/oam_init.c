/** \file oam_init.c
 * $Id$
 *
 * OAM init procedures for DNX.
 *
 */
/*
 * $Copyright: (c) 2019 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifdef BSL_LOG_MODULE
#  error "BSL_LOG_MODULE redefined"
#endif
#define BSL_LOG_MODULE BSL_LS_BCMDNX_OAM

 /*
  * Include files.
  * {
  */
#include <soc/dnx/dbal/dbal.h>
#include <src/bcm/dnx/oam/oam_counter.h>
#include <src/bcm/dnx/oam/oam_internal.h>
#include <src/bcm/dnx/oam/oam_oamp.h>
#include <include/soc/dnx/swstate/auto_generated/access/oam_access.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_headers.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_l3.h>
#include <include/soc/dnx/dnx_data/auto_generated/dnx_data_oam.h>
#ifdef DNX_EMULATION_1_CORE
#include <soc/sand/sand_aux_access.h>
#endif

/*
 * }
 */

/*
 * DEFINEs
 * {
 */

/** Definitions used in initializing MP-TYPE tables */

/* 
 * Size of the elements of the table's key, in bits 
 * See tools/autocoder/dbal/dnx/fields/fields_oam_definition.xml 
 */
#define MIP_ABOVE_MDL_NUM_BITS 1
#define PACKET_IS_BFD_NUM_BITS 1
#define NOF_MEP_BELOW_MDL_NUM_BITS 3
#define MDL_MP_TYPE_NUM_BITS 2
#define NOF_MEP_ABOVE_MDL_NUM_BITS 3

/** Used to simplify the MEP active match/passive match logic */

/*
 * This LIF has an active MEP on the MDL specified in the
 * packet
 */
#define ACTIVE_INDEX 0

/*
 * This LIF has an passive MEP on the MDL specified in the
 * packet
 */
#define PASSIVE_INDEX 1

/*
 * Since "high" "middle" and "low" map to counter 1, 2 and 3, 
 * counter 3 will only be used if there are at least 3 MEPs 
 * in the LIF.  If there are only 2 MEPs, the matches will be 
 * mapped to "high" and "middle."  For a "low" match, the MEP 
 * should have no MEPs with a lower level and at least two with 
 * a higher level. 
 */
#define MIN_MEPS_ABOVE_FOR_MATCH_LOW 2

/*
 * Needed for the loop below: both the ingress and the
 * egress tables are initialized 
 */
#define INGRESS_AND_EGRESS 2

/** End of definitions used in initializing MP-TYPE tables */

/** Values for keys for MDB tables for OAMP */
#define MDB_MEP_DB_ID       3
#define MDB_RMEP_DB_ID      2
#define MDB_MEP_LOOKUP_ID   1
#define MDB_RMEP_LOOKUP_ID  0

/** To work correctly, UDH must be a single "0" */
#define DEFAULT_UDH_SIZE 1

/** Value for PPH base offset */
#define PPH_BASE_SIZE 12

#define PPH_BASE_SIZE_LEGACY 7

/** Time of Day Formats */
#define IS_1588  0
#define IS_NTP   1

/*
 * Entry of initialization table for oam opcode mapping
 */
typedef struct
{
    int oam_opcode;
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode;
} oam_opcode_mapping_table_table_entry_t;

/**
 * This struct is used to add entries to the 
 * egress_oam_process_map dbal table, which 
 * maps to the ETPPC_OAM_PROCESS_MAP register. 
 * Each entry has two key elements and one value 
 * element - the third key element is not in use. 
 */
typedef struct
{
    /** Type of MEP: down-MEP (0) or up-MEP (1)? */
    int key_mep_type;

    /** Subtype of MEP - see enum for possible values */
    dbal_enum_value_field_oam_sub_type_e key_subtype;

    /** Action code - see enum for possible values */
    dbal_enum_value_field_oam_process_action_e value_process_action;
} oam_process_map_table_entry_t;

/** Number of entries for default oam opcode map */
#define NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES 16

/** Init values */
/*
 * When injecting PTCH with opaque value 7, set the inject bit in the classifier
 * (key to OAM action tables).  The number 7 is a hardware constant.
 */
#define OAM_PACKET_INJECTED_BITMAP_INIT_VAL     0x80

/*
 * When injecting PPH with forwarding strength set(as value 1), set the inject bit in the egress classifier
 * (key to OAM action tables).
 */
#define OAM_EGRESS_PACKET_INJECTED_BITMAP_INIT_VAL     0x80

/**
 * The INGRESS_PP_PORT dbal table contains properties for 
 * ports.  Each entry in the table maps to a port and 
 * one of the cores The ENABLE_PP_INJECT field allows
 * injection for that particular port.  For the OAM 
 * feature, this field is set to 1 in all entries, so 
 * injection will be possible in all ports. 
 */
#define OAM_INJECT_PP_PORT_INIT_VAL                1

/** 
 *  Value for next protocol field - This value is a bitmap used
 *  by the hardware to choose Eth OAM processing based on the
 *  parser value of the header (14 for Eth OAM). The value is
 *  placed in the field "nextprotocol."\n dbal table:
 *  OAM_GENERAL_CONFIGURATION\n field:
 *  ETHERNET_NEXT_PROTOCOL_OAM\n physical register:
 *  IPPB_OAM_ETHERNET_CFG\n physical field:
 *  ETHERNET_NEXT_PROTOCOL_OAM
 */
#define OAM_ETHERNET_CFG_INIT_VAL                0x4000

/** 
 *  For invalid LIF headers, return OAM_LIF = 0\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: OAM_LIF_INVALID_HEADER\n
 *  physical register: IPPB_OAM_ETHERNET_CFG\n
 *  physical field: OAM_LIF_INVALID_HEADER
 */
#define OAM_LIF_INVALID_HEADER_INIT_VAL           0

/** 
 *  Enable for all cases by default\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: OAM_MPLS_TP_OR_BFD_ENABLE_MAP\n
 *  physical register: IPPB_OAM_IDENTIFICATION_ENABLE\n
 *  physical field: OAM_MPLS_TP_OR_BFD_ENABLE_MAP
 */
#define OAM_MPLS_TP_OR_BFD_ENABLE_MAP_INIT_VAL 0xFF

/** 
 *  Enable validity check for BFD and MPLS/PWE\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: OAM_BFD_VALIDITY_CHECK\n
 *  physical register: IPPB_OAM_IDENTIFICATION_ENABLE\n
 *  physical field: OAM_BFD_VALIDITY_CHECK
 */
#define OAM_BFD_VALIDITY_CHECK_INIT_VAL           1

/** 
 *  Disable one-hop BFD when TTL=255\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: OAM_NO_BFD_AUTHENTICATION\n
 *  physical register: IPPB_OAM_IDENTIFICATION_ENABLE\n
 *  physical field: OAM_NO_BFD_AUTHENTICATION
 */
#define OAM_NO_BFD_AUTHENTICATION_INIT_VAL        0

/** 
 *  If MPLS-TP, force opcode=0\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: OAM_MPLS_TP_FORCE_OPCODE\n
 *  physical register: IPPB_OAM_IDENTIFICATION_ENABLE\n
 *  physical field: OAM_MPLS_TP_FORCE_OPCODE
 */
#define OAM_MPLS_TP_FORCE_OPCODE_INIT_VAL         0

/** 
 *  If not OAM LIF, return this value (-1, like in Jericho 1)\n
 *  dbal table: OAM_GENERAL_CONFIGURATION\n
 *  field: OAM_INVALID_LIF\n
 *  physical register: ETPPC_CFG_INVALID_OAM_LIF\n
 *  physical field: CFG_INVALID_OAM_LIF
 */
#define OAM_INVALID_LIF_INIT_VAL             0xFFFF

/* 
 *  Definitions for initializing the egress OAM processing
 *  map.  The 192 bit register ETPPC_OAM_PROCESS_MAP is
 *  actually a 64 element array of 3 bit values, where each 3
 *  bit value is an action code (possible values 1-6, see below)
 *  and the 6 bit index contains 4 MSBs for the MEP sub type, 1
 *  bit for Up-MEP (1) or Down-MEP (0), and the LSB is for port
 *  profile (currently only port profile=0 is in use.)  The
 *  virtual dbal table EGRESS_OAM_PROCESS_MAP provides the
 *  above key-value mapping to the physical register.
 */

/** Number of entries we wish to write */
#define NUM_PROCESS_ENTRIES_TO_INIT 8

/* 
 *  Definitions for initializing the egress OAM processing
 *  map - end
 */

/** Number of OAM packet subtypes that require the addition of
 *  OAM-TS. */
#define NUM_OAM_TS_REQ_SUB_TYPES 6

/*
 * ID for OAM TCAM Identification APP DB 0
 */
#define OAM_TCAM_IDENTIFICATION_APPDB_0_ID  0

/*
 * ID for OAM TCAM Identification APP DB 1
 */
#define OAM_TCAM_IDENTIFICATION_APPDB_1_ID  1

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
  * Global and Static
  */

/*
 * Active MEP / Passive MEP tables 
 * When the LIF has a MEP at the MDL of the packet 
 * being processed, that MEP can either be active 
 * or passive.  As a result, a counter needs to be 
 * accessed.  "Match high", "Match middle" and 
 * "match low" dictate accessing counter 1, 2 or 3, 
 * respectively.  The action taken depends on whether 
 * the MEP is active or passive. 
 */

/** Used when the match high actions are chosen */
const uint8 match_high[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_HIGH,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_HIGH
};

/** Used when the match low actions are chosen */
const uint8 match_low[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_LOW,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_LOW
};

/** Used when the match middle actions are chosen */
const uint8 match_middle[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_MIDDLE,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_MIDDLE
};

/** Used when the match no_counter actions are chosen */
const uint8 match_no_counter[] = {
    DBAL_ENUM_FVAL_MP_TYPE_ACTIVE_MATCH_NO_COUNTER,
    DBAL_ENUM_FVAL_MP_TYPE_PASSIVE_MATCH_NO_COUNTER
};


/* *INDENT-OFF* */
/**
 *   Array that hold default oam opcode map configuration and used on init stage
 */
static const oam_opcode_mapping_table_table_entry_t oam_opcode_map[NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES] = {
        /** OAM Opcode       |  Internal OAM Opcode mapping                        */
        {DBAL_DEFINE_OAM_OPCODE_BFD_OR_OAM_DATA,  DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_BFD},
        {DBAL_DEFINE_OAM_OPCODE_CCM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM},
        {DBAL_DEFINE_OAM_OPCODE_LBR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBR},
        {DBAL_DEFINE_OAM_OPCODE_LBM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LBM},
        {DBAL_DEFINE_OAM_OPCODE_LTR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTR},
        {DBAL_DEFINE_OAM_OPCODE_LTM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LTM},
        {DBAL_DEFINE_OAM_OPCODE_AIS,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_AIS},
        {DBAL_DEFINE_OAM_OPCODE_LINEAR_APS,       DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LINEAR_APS},
        {DBAL_DEFINE_OAM_OPCODE_LMR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR},
        {DBAL_DEFINE_OAM_OPCODE_LMM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM},
        {DBAL_DEFINE_OAM_OPCODE_1DM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM},
        {DBAL_DEFINE_OAM_OPCODE_DMR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR},
        {DBAL_DEFINE_OAM_OPCODE_DMM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM},
        {DBAL_DEFINE_OAM_OPCODE_SLR,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR},
        {DBAL_DEFINE_OAM_OPCODE_SLM,              DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM}
};

/**
 * Array of entries to write to the egress_oam_process_map
 * dbal table.  This array will be used in a for loop to 
 * configure the virtual dbal table OAM_PROCESS_MAP, and through 
 * it, the register ETPPC_OAM_PROCESS_MAP 
 */
const oam_process_map_table_entry_t process_map_entries[NUM_PROCESS_ENTRIES_TO_INIT] = 
{
    { DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_STAMP_COUNTER                   },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_READ_COUNTER_AND_STAMP          },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_NTP               },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP                },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_UP_MEP,   DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_1588              },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_1588               },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_1ST,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_INGRESS_STAMP_NTP               },
    { DBAL_ENUM_FVAL_OAM_DIRECTION_DOWN_MEP, DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_2ND,
      DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_EGRESS_STAMP_NTP                }
};

/** OAM packet subtypes that require the addition of OAM-TS. */
const uint8 OAM_TS_REQ_SUB_TYPES[NUM_OAM_TS_REQ_SUB_TYPES] = {
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_1ST,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_TWAMP_REFLECTOR_2ND,
    DBAL_ENUM_FVAL_OAM_SUB_TYPE_RFC8321
};

/* *INDENT-ON* */
/*
 * }
 */

/**
 * This function return counter stamp offset(TxFCf) in OAM packets according oam opcode(internal and external)
 * Stamp offsets:
 * LMM, DMM, 1DM: 4
 * SLM, LMR: 12
 * SLR: 16
 * DMR: 20
 * CCM with piggy-back: 58
 * Other opcodes do not stamp - setting default offset to '0'.
 */
static int
dnx_oam_opcode_counter_offset(
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode)
{
    int offset;
    switch (internal_opcode)
    {
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMM:
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_1DM:
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMM:
        {
            offset = 4;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_LMR:
        {
            offset = 12;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLM:
        {
            offset = 12;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_SLR:
        {
            offset = 16;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_DMR:
        {
            offset = 20;
            break;
        }
        case DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_CCM:
        {
            offset = 58;
            break;
        }
        default:
        {
            offset = 0;
            break;
        }
    }

    return offset;
}
/*
 * See h file for description
 */
shr_error_e
dnx_oam_opcode_map_set(
    int unit,
    int opcode,
    dbal_enum_value_field_oam_internal_opcode_e internal_opcode)
{
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, &entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, opcode);

    /** Set the entry result */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE, internal_opcode);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_STAMP_OFFSET, INST_SINGLE,
                                 dnx_oam_opcode_counter_offset(opcode, internal_opcode));

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** re-using handle with egress table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP, entry_handle_id));

    /** setting key fields */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, opcode);

    /** Set the entry result */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE, internal_opcode);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}
/**
 * \brief
 *   Set default configuration for opcode map memories on:
 *             ingress(DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP table) &
 *             egress(DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP table)
 */
static shr_error_e
dnx_oam_opcode_map_init_set(
    int unit)
{
    uint32 entry_handle_id;
    uint32 index;
    int rv = _SHR_E_NONE;
    const oam_opcode_mapping_table_table_entry_t *tmp_oam_opcode_map;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Initialize all ingress oam opcode map entries with following values:
     *  internal opcode - other
     *  offset - '0'
     */

    /** Get table handle */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_ETHERNET_OAM_OPCODE_MAP, &entry_handle_id));

    /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Set the entry result */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_COUNTER_STAMP_OFFSET, INST_SINGLE, 0);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** re-using handle with egress table */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_EGRESS_ETHERNET_OAM_OPCODE_MAP, entry_handle_id));

    /** Initialize all ingress oam opcode map entries with following values:
     *  internal opcode - other
     */

    /** Set Range Key Fields */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_OPCODE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Set the entry result */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INTERNAL_OPCODE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_OAM_INTERNAL_OPCODE_ETH_OTHER);

    /** Commit table values */
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Loop for initializing oam map code mapping with default values */
    tmp_oam_opcode_map = oam_opcode_map;
    for (index = 0; index < NOF_OAM_OPCODE_DEFAULT_MAPPED_ENTRIES; index++, tmp_oam_opcode_map++)
    {
        rv = dnx_oam_opcode_map_set(unit, tmp_oam_opcode_map->oam_opcode, tmp_oam_opcode_map->internal_opcode);
        SHR_IF_ERR_EXIT(rv);
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function calculates the MP-type for a 
 *           received OAM packet with the characteristics
 *           specified in the parameters listed below.
 * 
 * \param [in] mip_above_mdl - 1 if the OAM LIF receiving the 
 *        packet has a MIP at an MD level higher than the one
 *        listed in the packet; 0 otherwise
 * \param [in] packet_is_bfd - 1 if the packet is a BFD packet; 
 *        0 otherwise
 * \param [in] nof_mep_below_mdl - Indicates how many MEPs in 
 *        the OAM LIF receiving the packet have a higher MDL
 *        than the one listed in the packet.  Possible values:
 *        0-7 (7 if the packet MDL is 0 and there are MEPs at
 *        each level 1-7)
 * \param [in] mdl_mp_type - What does the OAM LIF have at the 
 *        MDL specified in the packet?\n
 *        0: Nothing\n
 *        1: a MIP\n
 *        2: an active MEP\n
 *        3: a passive MEP 
 * \param [in] nof_mep_above_mdl - Indicates how many MEPs in 
 *        the OAM LIF receiving the packet have a lower MDL
 *        than the one listed in the packet.  Possible values:
 *        0-7 (7 if the packet MDL is 7 and there are MEPs at
 *        each level 0-6)
 *   
 * \return
 *   int 
 *   
 * \remark
 * This function performs the logical determination of the MP-type 
 * from the elements of the key:\n 
 * if the "packet_is_bfd" bit is on => MP_TYPE = BFD\n
 * otherwise, if the "mip_match" bit is on => MP_TYPE = 
 * MIP_MATCH\n 
 * otherwise, if this LIF has a MIP above the packet's MDL, but 
 * no MEPs above the MDL or at the MDL (no MEP match) =>\n 
 *      if there are no MEPs below the MDL (i.e. no MEPs for
 *      this LIF, just one MIP) => MP_TYPE = ABOVE_UPPER_MEP\n
 *      otherwise (one MIP above MDL, some MEPs below) =>
 *      MP_TYPE = BETWEEN_MIP_AND_MEP\n
 * otherwise, if there is no MEP at the packet's MDL =>\n 
 *      if there are no MEPs above the packet's MDL =>
 *      MP_TYPE = ABOVE_UPPER_MEP\n
 *      otherwise, if there are no MEPs below the packet's MDL =>
 *      MP_TYPE = BELOW_LOWER_MEP\n
 *      otherwise, if there are more MEPs above the MDL than below =>
 *      MP_TYPE = BETWEEN_MIDDLE_AND_LOW\n
 *      otherwise MP_TYPE = BETWEEN_HIGH_AND_MIDDLE\n
 * otherwise (i.e., if there is a MEP match) =>\n 
 *      if there are no MEPs below this packet's MDL and at least
 *      two above it => MP_TYPE = ACTIVE_MATCH_LOW /
 *      PASSIVE_MATCH_LOW, according to whether the matched MEP is
 *      active or passive.\n
 *      otherwise if there are no MEPs above this packet's MDL =>
 *      MP_TYPE = ACTIVE_MATCH_HIGH / PASSIVE_MATCH_HIGH\n
 *      otherwise MP_TYPE = ACTIVE_MATCH_MIDDLE / PASSIVE_MATCH_MIDDLE
 */

static int
dnx_oam_calculate_mp_type_from_key(
    int mip_above_mdl,
    int packet_is_bfd,
    int nof_mep_below_mdl,
    int mdl_mp_type,
    int nof_mep_above_mdl)
{
    /** Determine MP type according to the elements of the key  */
    uint8 active_or_passive;
    if (packet_is_bfd)
    {
        if ((mip_above_mdl) && (nof_mep_above_mdl == DNX_OAM_MAX_MDL) && (nof_mep_below_mdl == DNX_OAM_MAX_MDL))
        {
            /*
             * Special case for non oam packet (no oam_packet entry in mp_type table):
             * nof_below_mdl==7 , nof_above_mdl==7, bfd==1, mip_above_mdl==1 
             */
            return DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP;
        }
        /** BFD entry */
        return DBAL_ENUM_FVAL_MP_TYPE_BFD;
    }
    else
    {
        /** OAM entry */
        if (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_MIP)
        {
            /** MIP */
            return DBAL_ENUM_FVAL_MP_TYPE_MIP_MATCH;
        }
        else
        {
            /** Not a MIP   */
            if (mip_above_mdl && (nof_mep_above_mdl == 0) && (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP))
            {
                /** No matches for this MDL, only a MIP above, no MEPs above */
                if (nof_mep_below_mdl == 0)
                {
                    /** This LIF only has the MIP, no MEPs */
                    return DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP;
                }
                else
                {
                    /** There are MEPs below this MDL */
                    return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIP_AND_MEP;
                }
            }
            else
            {
                /** mip_above_mdl is irrelevant */
                if (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_NO_MEP)
                {
                    /** Nothing found in this MDL */
                    if (nof_mep_above_mdl == 0)
                    {
                        /** No MEPs above specified MDL in LIF  */
                        return DBAL_ENUM_FVAL_MP_TYPE_ABOVE_UPPER_MEP;
                    }
                    else
                    {
                        if (nof_mep_below_mdl == 0)
                        {
                            /** No MEPs below MOL, at least one MEP above */
                            return DBAL_ENUM_FVAL_MP_TYPE_BELOW_LOWER_MEP;
                        }
                        else
                        {

                            /** Some MEPs above and below */
                            if (nof_mep_above_mdl == 1)
                            {
                                /*
                                 * if only one mep above, the packet's level must be between high and middle 
                                 */
                                return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE;
                            }
                            else
                            {
                                if (nof_mep_below_mdl == 1)
                                {
                                    /*
                                     * if only one mep below, and more than 1 above, the packet's level must be between 
                                     * middle and low 
                                     */
                                    return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_MIDDLE_AND_LOW;
                                }
                                else
                                {
                                    /*
                                     * there are at least 2 meps below so they have counters assigned. Therefore
                                     * incoming packet is between the high and middle meps with counters 
                                     */
                                    return DBAL_ENUM_FVAL_MP_TYPE_BETWEEN_HIGH_AND_MIDDLE;
                                }
                            }

                        }
                    }
                }
                else
                {
                    /** MEP match: active or passive */
                    active_or_passive = (mdl_mp_type == DBAL_ENUM_FVAL_MDL_MP_TYPE_ACTIVE_MEP) ?
                        ACTIVE_INDEX : PASSIVE_INDEX;
                    if ((nof_mep_below_mdl == 0) && (nof_mep_above_mdl >= MIN_MEPS_ABOVE_FOR_MATCH_LOW))
                    {
                        /** At least two MEPs above MDL, none below */
                        return match_low[active_or_passive];
                    }
                    else
                    {
                        if (nof_mep_above_mdl == 0)
                        {
                            /** No MEPs above MDL */
                            return match_high[active_or_passive];
                        }
                        else
                        {
                            /*
                             * There is 1 MEP below, or, There are only two MEPs, and the packet is the lower level 
                             */
                            if ((nof_mep_below_mdl == 1) || ((nof_mep_below_mdl == 0) && (nof_mep_above_mdl == 1)))
                            {
                                return match_middle[active_or_passive];
                            }
                            else
                            {
                                /** More than 1 mep below and more than 2 MEPs => MATCH_NO_COUNTER*/
                                return match_no_counter[active_or_passive];
                            }
                        }
                    }
                }
            }
        }
    }
}

/**
 * \brief - This function initializes the ingress and egress 
 *          MP TYPE tables.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e
 *   
 * \remark - The target tables are IPPB_OAM_MP_TYPE_MAP (for\n\ 
 *         the ingress) and ETPPC_OAM_MP_TYPE_MAP (for the\n\
 *         egress.) The hardware uses the value extracted\n\
 *         from one of these tables to perform the\n\
 *         appropriate action on the packet currently being\n\
 *         processed.
 * \see
 *   * This function is part of the OAM initialization process,
 *     and is called from dnx_oam_init.
 */
static shr_error_e
dnx_oam_mp_type_table_init(
    int unit)
{
    int mip_above_mdl, packet_is_bfd, nof_mep_below_mdl;
    int table_index, mdl_mp_type, nof_mep_above_mdl, mp_type;
    uint32 entry_handle_id[2];
    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /** Write to both ingress and egress tables */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_MP_TYPE_MAP, &entry_handle_id[0]));
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_MP_TYPE_MAP, &entry_handle_id[1]));

    /** Loop over every entry in the table   */
    for (mip_above_mdl = 0; mip_above_mdl < SAL_BIT(MIP_ABOVE_MDL_NUM_BITS); mip_above_mdl++)
    {
        for (packet_is_bfd = 0; packet_is_bfd < SAL_BIT(PACKET_IS_BFD_NUM_BITS); packet_is_bfd++)
        {
            for (nof_mep_below_mdl = 0; nof_mep_below_mdl < SAL_BIT(NOF_MEP_BELOW_MDL_NUM_BITS); nof_mep_below_mdl++)
            {
                for (mdl_mp_type = 0; mdl_mp_type < SAL_BIT(MDL_MP_TYPE_NUM_BITS); mdl_mp_type++)
                {
                    for (nof_mep_above_mdl = 0; nof_mep_above_mdl < SAL_BIT(NOF_MEP_ABOVE_MDL_NUM_BITS);
                         nof_mep_above_mdl++)
                    {
                        /** Calculate all 5 key elements for this entry */
                        mp_type = dnx_oam_calculate_mp_type_from_key(mip_above_mdl, packet_is_bfd, nof_mep_below_mdl,
                                                                     mdl_mp_type, nof_mep_above_mdl);

                        /** Write the MP type determined above to the tables */
                        for (table_index = 0; table_index < INGRESS_AND_EGRESS; table_index++)
                        {
                            /** Enter key segments  */
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_MIP_ABOVE_MDL, mip_above_mdl);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_PACKET_IS_BFD, packet_is_bfd);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_NOF_MEP_BELOW_MDL, nof_mep_below_mdl);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_MDL_MP_TYPE, mdl_mp_type);
                            dbal_entry_key_field32_set(unit, entry_handle_id[table_index],
                                                       DBAL_FIELD_NOF_MEP_ABOVE_MDL, nof_mep_above_mdl);

                            /** Enter value */
                            dbal_entry_value_field32_set(unit, entry_handle_id[table_index],
                                                         DBAL_FIELD_MP_TYPE, INST_SINGLE, mp_type);

                            /** Access table */
                            SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id[table_index], DBAL_COMMIT));
                        }
                    }
                }
            }
        }
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes a few registers and 
 *        memories that are necessary for OAM functionality.
 *        These registers are mostly used for OAM
 *        classification, both in the ingress (for down-MEPs)
 *        and the egress (for up-MEPs).  See remark for more
 *        details.
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * Registers and memories initialized in this function:
 *  
 *   1. IPPB_PINFO_FLP_MEM - all entries, field
 *   ENABLE_PP_INJECT, through dbal table INGRESS_PINFO_FLP_MEM
 *   2. Eight registers grouped together in dbal virtual table
 *   OAM_GENERAL_CONFIGURATION\n
 *       a. IPPB_OAM_PACKET_INJECTED_BITMAP\n
 *       b. ETPPC_OAM_PACKET_INJECTED_BITMAP\n
 *       c. ETPPC_MAP_STRENGTH_TO_OAM_INJECTION\n
 *       d. IPPB_OAM_ETHERNET_CFG\n
 *       e. IPPB_OAM_IDENTIFICATION_ENABLE\n
 *       f. ETPPC_CFG_INVALID_OAM_LIF\n
 *       g. OAMP_TX_JR_2_AS_OAM\n
 *       h. IPPB_OAM_ID_RIF_THRESHOLD
 *   3. ETPPC_OAM_PROCESS_MAP, through dbal table
 *   EGRESS_OAM_PROCESS_MAP
 *   4. BUILD_OAM_TS_HEADER, through dbal table
 *   OAM_TS_HEADER_SEGMENTS
 *   5. IPPD_OAM_SUB_TYPE_MAP, through dbal table
 *   INGRESS_OAM_SUB_TYPE_TO_DATA_TYPE_MAP
 *   6. ETPPB_CFG_OAM_TYPE, through dbal table
 *   EGRESS_OAM_SUB_TYPE_TO_DATA_TYPE_MAP
 *   7. OAMP_MDB_APP_DB, through dbal table
 *   OAMP_MDB_APP_DB
 *   8. Two registers grouped together in dbal virtual table
 *   OAMP_UDH\n
 *       a. OAMP_UDH\n
 *       b. OAMP_HEADER_CONFIG
 *   9. PPH_BASE_SIZE, through dbal table
 *   SYSTEM_HEADER_GLOBAL_CFG
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_init_general_configuration(
    int unit)
{
    uint32 entry_handle_id;
    int index;
    uint8 oamp_tx_format;
    uint32 nof_rifs;
    int system_headers_mode;
    uint32 udh_size = 0;
    uint32 pph_base_size;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    system_headers_mode = dnx_data_headers.system_headers.system_headers_mode_get(unit);

    /**
     *    In table INGRESS_PP_PORT, set ENABLE_PP_INJECT as 1
     *    in all entries - so DBAL_RANGE_ALL is used for in the
     *    PP_PORT key field (signifying all ports) and in the
     *    CORE_ID field (signifying all cores.)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_PP_PORT, &entry_handle_id));
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PP_PORT, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_CORE_ID, DBAL_CORE_ALL);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PP_INJECT_ENABLE,
                                 INST_SINGLE, OAM_INJECT_PP_PORT_INIT_VAL);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     *  Initialize Values in scattered registers - mapped as one
     *  in dbal
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_GENERAL_CONFIGURATION, &entry_handle_id));
    /** 
     *  Initialize OAM_INJECT_BITMAP registers.  They allow
     *  OAM packet injection in general - as opposed to per port,
     *  per core
     */
    /** Ingress */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_PACKET_INJECTED_BITMAP,
                                 INST_SINGLE, OAM_PACKET_INJECTED_BITMAP_INIT_VAL);
    /** Egress */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_PACKET_INJECTED_BITMAP,
                                 INST_SINGLE, OAM_EGRESS_PACKET_INJECTED_BITMAP_INIT_VAL);

    /**PPH FWD strength to inject indication:currently set as 2 */
    /** Basically there are three mappings here: First the PPH/FTMH fields to fwd_action_strength (handled by dnx_rx_trap_etpp_strength_map_init),
     *   fwd_action_strenght to oam-injection through EGRESS_MAP_STRENGTH_TO_OAM_INJECTION and finally oam-injection for that strength to an enable bit.
     */
    
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_MAP_STRENGTH_TO_OAM_INJECTION,
                                 INST_SINGLE, OAM_EGRESS_PACKET_INJECTED_BITMAP_INIT_VAL);
    /** 
     *  Initialize ETHERNET_NEXT_PROTOCOL_OAM field.  This is used
     *  by the pipeline as a value for the field "nextprotocol" in
     *  ethernet OAM packets.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ETHERNET_NEXT_PROTOCOL_OAM,
                                 INST_SINGLE, OAM_ETHERNET_CFG_INIT_VAL);

    /** 
     *  Initialize OAM_LIF_INVALID_HEADER field.  If an OAM packet
     *  has an invalid header, the ingress classifier uses this
     *  value as the LIF for further processing.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LIF_INVALID_HEADER,
                                 INST_SINGLE, OAM_LIF_INVALID_HEADER_INIT_VAL);

    /** 
     *  Initialize OAM_MPLS_TP_OR_BFD_ENABLE_MAP field.  This field
     *  maps enable bits to service types.  For each service type (8
     *  total) a value of 1 enables identification of OAM or BFD
     *  packets, and 0 disables identification.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MPLS_TP_OR_BFD_ENABLE_MAP,
                                 INST_SINGLE, OAM_MPLS_TP_OR_BFD_ENABLE_MAP_INIT_VAL);

    /** 
     *  Initialize OAM_BFD_VALIDITY_CHECK field.  This field, if
     *  set, enables validity checks for MPLS-TP or BFD over PWE.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_BFD_VALIDITY_CHECK,
                                 INST_SINGLE, OAM_BFD_VALIDITY_CHECK_INIT_VAL);

    /** 
     *  Initialize OAM_NO_BFD_AUTHENTICATION field.  This field, if
     *  set, enables one-hop BFD when TTL is 255
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_NO_BFD_AUTHENTICATION,
                                 INST_SINGLE, OAM_NO_BFD_AUTHENTICATION_INIT_VAL);

    /** 
     *  Initialize OAM_MPLS_TP_FORCE_OPCODE field.  When an OAM
     *  packet is encapsulated with OAM-TP, this value is forced as
     *  the opcode that should be used.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MPLS_TP_FORCE_OPCODE,
                                 INST_SINGLE, OAM_MPLS_TP_FORCE_OPCODE_INIT_VAL);

    /** 
     *  Initialize OAM_INVALID_LIF field.  If an OAM packet
     *  has an invalid header, the egress classifier uses this
     *  value as the LIF for further processing.
     */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_INVALID_LIF, INST_SINGLE,
                                 OAM_INVALID_LIF_INIT_VAL);

    /**
     * Initialize configuration bit: TX_JR2_AS_OAM
     * type: OAM (0)
     */

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TX_JR2_AS_OAM, INST_SINGLE, 0);

    /** Initialize the "IPPB_OAM_ID_RIF_THRESHOLD" register */
    nof_rifs = dnx_data_l3.rif.nof_rifs_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RIF_ID_THRESHOLD, INST_SINGLE, nof_rifs - 1);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Initialize ETPPC_OAM_PROCESS_MAP tables */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_PROCESS_MAP, &entry_handle_id));

    /** First clear map */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_PORT_PROFILE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_MEP_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    /** Set everything to 0 */
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PROCESS_ACTION,
                                 INST_SINGLE, DBAL_ENUM_FVAL_OAM_PROCESS_ACTION_NONE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Configure appropriate actions */
    /**
     * Only the entries where port profile is 0 are currently in
     * use.  Theoretically, configuring both port profile 0 and port
     * profile 1 would allow the user to switch instantly between 
     * two complete sets of OAM process actions - but again, this 
     * has never been used in past products. 
     */
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_PORT_PROFILE, 0);
    for (index = 0; index < NUM_PROCESS_ENTRIES_TO_INIT; index++)
    {
    /** Add entries as defined by const arrays */
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_MEP_TYPE,
                                   process_map_entries[index].key_mep_type);
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                                   process_map_entries[index].key_subtype);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_PROCESS_ACTION,
                                     INST_SINGLE, process_map_entries[index].value_process_action);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /** 
     *  Initialize the "BUILD_OAM_TS_HEADER" register, through the
     *  DBAL virtual table OAM_TS_HEADER_PER_SUBTYPE
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_TS_HEADER_PER_SUBTYPE, &entry_handle_id));

    /** First clear map */
    dbal_entry_key_field32_range_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, DBAL_RANGE_ALL, DBAL_RANGE_ALL);

    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_OAM_TS_HEADER, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Now set for DM, LM and SLM */
    for (index = 0; index < NUM_OAM_TS_REQ_SUB_TYPES; index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, OAM_TS_REQ_SUB_TYPES[index]);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_BUILD_OAM_TS_HEADER, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

    /**
     *    Initialize mapping between OAM subtype and data type(on Ingress side) to be
     *    stamped on OAM Application Extension(oam_ts_data) header (0 - counter-value, 1 - TOD-1588,
     *    2 - NTP, 3 - none)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_INGRESS_OAM_SUB_TYPE_TO_DATA_TYPE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_1588);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_TOD_NTP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUBTYPE_KEY, DBAL_ENUM_FVAL_OAM_SUB_TYPE_RFC8321);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_INGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_INGRESS_OAM_DATA_TYPE_COUNTER_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /**
     *    Initialize mapping between OAM subtype and data type(on Egress side) to be
     *    stamped on OAM Application Extension(oam_ts_data) header
     *    ( 0 - TOD-1588,1 - NTP, 2 - counter-value, 3 - none)
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_EGRESS_OAM_SUB_TYPE_TO_DATA_TYPE_MAP, &entry_handle_id));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_1588);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_1588);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_DELAY_MEASUREMENT_NTP);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_TOD_NTP);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_EGRESS_OAM_DATA_TYPE, INST_SINGLE,
                                 DBAL_ENUM_FVAL_EGRESS_OAM_DATA_TYPE_COUNTER_VALUE);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Set keys for MDB tables that support OAMP functionality */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_MDB_APP_DB, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_DB_ID, INST_SINGLE, MDB_MEP_DB_ID);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_DB_ID, INST_SINGLE, MDB_RMEP_DB_ID);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_MEP_LOOKUP_ID, INST_SINGLE, MDB_MEP_LOOKUP_ID);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_RMEP_LOOKUP_ID, INST_SINGLE, MDB_RMEP_LOOKUP_ID);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Initialize UDH fields */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_UDH, &entry_handle_id));
    oamp_tx_format = dnx_data_headers.system_headers.system_headers_mode_get(unit);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_MODE, INST_SINGLE, oamp_tx_format);

    if (system_headers_mode == dnx_data_headers.system_headers.system_headers_mode_jericho_get(unit))
    {
        udh_size = BITS2BYTES(dnx_data_field.udh.field_class_id_size_0_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_1_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_2_get(unit));
        udh_size += BITS2BYTES(dnx_data_field.udh.field_class_id_size_3_get(unit));

        pph_base_size = PPH_BASE_SIZE_LEGACY;
    }
    else
    {
        udh_size = DEFAULT_UDH_SIZE;

        pph_base_size = PPH_BASE_SIZE;
    }
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_SIZE, INST_SINGLE, udh_size);
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_UDH_TX_DATA, INST_SINGLE, 0);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Initialize PPH Base size field */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_SYSTEM_HEADER_GLOBAL_CFG, &entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_PPH_BASE_SIZE, INST_SINGLE, pph_base_size);
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Enable OAM_LM_PER_SUB_TYPE_DISABLE field for LM - Enable counter read
     * Note: Dual ended CCM also uses OAM_SUB_TYPE_LOSS_MEASUREMENT
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_LM_READ_PER_SUB_TYPE_DISABLE, &entry_handle_id));
    dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_SUB_TYPE,
                               DBAL_ENUM_FVAL_OAM_SUB_TYPE_LOSS_MEASUREMENT);
    if (dnx_data_oam.general.oam_lm_read_per_sub_disable_field_exists_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_READ_INDEX_DISABLE, INST_SINGLE, 0);
    }
    if (dnx_data_oam.general.oam_lm_read_per_sub_enable_field_exists_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAM_LM_READ_INDEX_ENABLE, INST_SINGLE, 1);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /*
     * Set APPDB for oam tcam identification
     */
    SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TCAM_IDENTIFICATION_APPDB, entry_handle_id));
    dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IDENTIFICATION_APPDB_0, INST_SINGLE,
                                 OAM_TCAM_IDENTIFICATION_APPDB_0_ID);
    if (dnx_data_oam.general.oam_identification_appdb_1_field_exists_get(unit))
    {
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_IDENTIFICATION_APPDB_1, INST_SINGLE,
                                     OAM_TCAM_IDENTIFICATION_APPDB_1_ID);
    }
    SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes registers that are 
 *        necessary for OAM ToD . These registers are mostly
 *        used for OAM DM functionality.
 *        Both NTP and IEEE 1588 formats initialized. 
 * 
 * \param [in] unit - Number of hardware unit used.
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * Registers and memories initialized in this function:
 *  
 *   1. ECI_GP_CONTROL_6r
 *   2. ECI_GP_CONTROL_7r
 *   3. ECI_GP_CONTROL_8r
 * \see
 *   * None
 */
static shr_error_e
dnx_oam_init_tod(
    int unit)
{
    uint32 entry_handle_id;
    int tod_config_in_eci;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    /*
     * Init NTP
     */

    tod_config_in_eci = dnx_data_oam.general.oam_tod_config_in_eci_get(unit);
    if (tod_config_in_eci)
    {

        /*
         * 1588 Configurations 
         */
        SHR_IF_ERR_EXIT(dnx_oam_tod_set(unit, IS_1588, 0x1ffff0013576543));

        /*
         * NTP Configurations 
         */
        SHR_IF_ERR_EXIT(dnx_oam_tod_set(unit, IS_NTP, 0x1ffff0013576543));

        /*
         * General ToD Configuration
         */

    /** Reset for configuring */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, &entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_NSE_RESET, INST_SINGLE, 1);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, entry_handle_id));
    /** Set ECI and OAMP CMIC ToD mode to both NTP and 1588 */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_TOD_MODE, INST_SINGLE, 3);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OAMP_CMIC_TOD_MODE, INST_SINGLE, 3);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

    /** Set ToD */
        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_NSE_RESET, INST_SINGLE, 0);
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));

        SHR_IF_ERR_EXIT(DBAL_HANDLE_CLEAR(unit, DBAL_TABLE_OAM_TOD_GENERAL_CONFIGURATION, entry_handle_id));
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TODW_SELECT_IEEE_1588_TIMER, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_TODW_SELECT_NTP_TIMER, INST_SINGLE, 1);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_ECI_NSE_NCO_FREQ_CONTROL, INST_SINGLE,
                                     dnx_data_oam.general.oam_nse_nco_freq_control_get(unit));
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**Disable opcode statistics counting for per opcode for TX/RX. */
shr_error_e
dnx_oam_opcode_for_count_table_init(
    int unit)
{

    int index;
    uint32 entry_handle_id;

    SHR_FUNC_INIT_VARS(unit);
    DBAL_FUNC_INIT_VARS(unit);

    SHR_IF_ERR_EXIT(DBAL_HANDLE_ALLOC(unit, DBAL_TABLE_OAMP_OPCODE_FOR_COUNT, &entry_handle_id));

    for (index = 0; index < 6; index++)
    {
        dbal_entry_key_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_INDEX, index);
        /** setting value fields */
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT, INST_SINGLE, index);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_RX_ENABLE, INST_SINGLE, FALSE);
        dbal_entry_value_field32_set(unit, entry_handle_id, DBAL_FIELD_OPCODE_FOR_COUNT_TX_ENABLE, INST_SINGLE, FALSE);
        /** Preforming the action */
        SHR_IF_ERR_EXIT(dbal_entry_commit(unit, entry_handle_id, DBAL_COMMIT));
    }

exit:
    DBAL_FUNC_FREE_VARS;
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function initializes the OAM feature.  This 
 *        entails mostly initializing some hardware tables and
 *        registers using dbal abstractions.  This is needed for
 *        the Jericho 2 pipeline to execute its OAM related
 *        functionality correctly.
 * 
 * \param [in] unit - Identification Number of hardware unit 
 *        used.
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_oam_init(
    int unit)
{
    int rv = _SHR_E_NONE;

    SHR_FUNC_INIT_VARS(unit);

    /** init the SW DB of OAM EPs linked to each OAM group */
    rv = dnx_oam_sw_db_init(unit);
    SHR_IF_ERR_EXIT(rv);

    /** init key select ingress & egress tables */
    rv = dnx_oam_key_select_ingress_init(unit);
    SHR_IF_ERR_EXIT(rv);
#ifdef DNX_EMULATION_1_CORE
    if (soc_sand_is_emulation_system(unit))
    {
        SHR_EXIT_WITH_LOG(_SHR_E_NONE, "Not initializing OAM in emulation to save startup time%s%s%s", EMPTY, EMPTY,
                          EMPTY);
    }
#endif
    rv = dnx_oam_key_select_egress_init(unit);
    SHR_IF_ERR_EXIT(rv);

    /** set default configuration for oam opcode mapping */
    rv = dnx_oam_opcode_map_init_set(unit);
    SHR_IF_ERR_EXIT(rv);

    /** init counter mechanism */
    rv = dnx_oam_counters_init(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Initialize MP-TYPE table */
    SHR_IF_ERR_EXIT(dnx_oam_mp_type_table_init(unit));

    /** Initialize various OAM relevant settings */
    rv = dnx_oam_init_general_configuration(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Initialize ToD */
    rv = dnx_oam_init_tod(unit);
    SHR_IF_ERR_EXIT(rv);

    /** Initialize Opcode for count table */
    SHR_IF_ERR_EXIT(dnx_oam_opcode_for_count_table_init(unit));

    /** Initialize OAMP block */
    rv = dnx_oam_oamp_init(unit);
    SHR_IF_ERR_EXIT(rv);

exit:
    SHR_FUNC_EXIT;
}

/**
 * \brief - This function contains the deinitialization 
 *        procedure for the OAM feature.  Currently, it does not
 *        do anything.
 * 
 * \param [in] unit - Identification Number of hardware unit 
 *        used.
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_oam_deinit(
    int unit)
{
    SHR_FUNC_INIT_VARS(unit);

    /** deinit the SW DB of OAM EPs linked to each OAM group */
    SHR_IF_ERR_EXIT(dnx_oam_sw_db_deinit(unit));

    /** deinit OAMP setting done */
    SHR_IF_ERR_EXIT(dnx_oam_oamp_deinit(unit));

exit:
    SHR_FUNC_EXIT;
}
