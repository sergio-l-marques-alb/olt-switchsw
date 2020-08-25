/** \file oam_oamp.h
 * OAMP support - contains internal functions and definitions
 * for support of the OAM/BFD accelerator oam functionality
 */
/*
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef OAM_OAMP_H_INCLUDED
/*
 * {
 */
#define OAM_OAMP_H_INCLUDED

#ifndef BCM_DNX_SUPPORT
#error "This file is for use by DNX (JR2) family only!"
#endif

#include "include/bcm/oam.h"
#include <bcm/types.h>
#include <soc/dnx/dbal/dbal.h>
#include <bcm_int/dnx/algo/oam/algo_oam.h>
#include <soc/dnx/dbal/dbal_structures.h>
#include <src/bcm/dnx/oam/oam_internal.h>

/** 
 *  Convert from hardware entry index (15 bits) to OAM-ID (17
 *  bits)
 */
#define MEP_DB_ENTRY_TO_OAM_ID(x) (x<<2)

/** 
 *  Convert from OAM-ID (17 bits) to hardware entry index (15
 *  bits)
 */
#define OAM_ID_TO_MEP_DB_ENTRY(x) (x>>2)

/** Interval in microseconds between two OAMP MEP DB scans */
#define OAMP_SCAN_INTERVAL_uS 1670

/** Flags relevant for accelerated OAM CCM endpoints   */

/** Update flag - always necessary because of allocation  */
#define DNX_OAMP_OAM_CCM_MEP_UPDATE             SAL_BIT(0)

/**
 *  When set, this endpoint will be written to the OAMP MEP DB
 *  in offloaded mode: two MDB entries will also be added
 */
#define DNX_OAMP_OAM_CCM_MEP_LM_DM_OFFLOADED    SAL_BIT(1)

/** Indicates 1/4 entry mode   */
#define DNX_OAMP_OAM_CCM_MEP_Q_ENTRY            SAL_BIT(2)

/** 0 = down-MEP; 1 = up-MEP   */
#define DNX_OAMP_OAM_CCM_MEP_IS_UPMEP           SAL_BIT(3)

/** MAID that is copied from extra data header */
#define DNX_OAMP_OAM_CCM_MEP_48B_MAID           SAL_BIT(4)

/** Read only: remote detect indicator produced by scanner   */
#define DNX_OAMP_OAM_CCM_MEP_RDI_FROM_SCANNER   SAL_BIT(5)

/**
 *  Read only: remote detect indicator read for received
 *  packet
 */
#define DNX_OAMP_OAM_CCM_MEP_RDI_FROM_PACKET    SAL_BIT(6)

/** Enable port status TLV injection   */
#define DNX_OAMP_OAM_CCM_MEP_PORT_TLV_EN        SAL_BIT(7)

/** Value of port status TLV   */
#define DNX_OAMP_OAM_CCM_MEP_PORT_TLV_VAL       SAL_BIT(8)

/** 1 = multicast; 0 = unicast   */
#define DNX_OAMP_OAM_CCM_MEP_MC_UC_SEL          SAL_BIT(9)

/** Do not send packets for this MEP ID.   */
#define DNX_OAMP_OAM_CCM_MEP_TXM_DSBL_TX        SAL_BIT(10)

/** Use FEC ID or global out-LIF?   */
#define DNX_OAMP_OAM_CCM_MEP_DESTINATION_IS_FEC SAL_BIT(11)

/** Use for Egress Down-MEP Injection   */
#define DNX_OAMP_OAM_CCM_MEP_DOWN_MEP_EGRESS_INJECTION   SAL_BIT(12)

/**
 *  Bit in the "FEC-ID or Global out-LIF" field that determines
 *  whether the field is FEC-ID (1) or global out-LIF (0)
 */
#define FIELD_IS_FEC_ID_FLAG 22

/**
 *  Number of bytes of extra data in a single EXTRA_DATA_HDR
 *  or EXTRA_DATA_PLD entry
 */
#define DATA_SEG_LEN 20

/**
 *  Number of bytes needed for a two-way delay field
 *  (LAST_DELAY, MAX_DELAY, etc.)
 */
#define TWO_WAY_DELAY_SIZE 6

/**
 *  Number of bytes needed for a one-way delay field
 *  (LAST_DELAY_DMM, MAX_DELAY_DMR, etc.)
 */
#define ONE_WAY_DELAY_SIZE 5

/** In order to avoid pointing to a valid entry, this value can be used in fields that point to other MEP DB entries  */
#define INVALID_ENTRY 0xFFFF

/** OAM group create indication   */
#define DNX_OAMP_OAM_GROUP_CREATE SAL_BIT(0)

/** OAM group destroy indication   */
#define DNX_OAMP_OAM_GROUP_DESTROY 0

/** OAM group MD format short/ICC   */
#define DNX_OAMP_OAM_GROUP_MD_FORMAT SAL_BIT(0)

/** OAM group MA NAME format short   */
#define DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_SHORT 3

/** OAM group MA NAME format ICC   */
#define DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_ICC 32

/** MAID offset in OAM packet (in bits)*/
#define DNX_OAMP_OAM_GROUP_MAID_OFFSET 80

/** OAM group MA NAME ICC data length in bytes */
#define DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH 6

/** OAM group MA NAME ICC data offset in MAID name */
#define DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET 3

/** OAM LONG group MA NAME length in MAID name */
#define DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_LENGTH 2

/** OAM LONG group MA NAME offset in MAID name */
#define DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_OFFSET (DNX_OAMP_OAM_GROUP_ICC_DATA_OFFSET + DNX_OAMP_OAM_GROUP_ICC_DATA_LENGTH)

/** OAM group MA NAME UMC data length in bytes */
#define DNX_OAMP_OAM_GROUP_UMC_DATA_LENGTH 5

/** OAM group MA NAME UMC data offset in MAID name */
#define DNX_OAMP_OAM_GROUP_UMC_DATA_OFFSET (DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_OFFSET + DNX_OAMP_OAM_GROUP_LONG_MA_NAME_DATA_LENGTH)

/** In the "local port" field, offset of core ID bit */
#define PORT_CORE_BIT_INDEX 8

#define DNX_OAM_MEP_DB_NOF_BITS_IN_HEADER_ENTRY  160
#define DNX_OAM_MEP_DB_NOF_BITS_IN_PAYLOAD_ENTRY 160

#define DNX_OAM_OAMP_NOF_MAID_BITS (BCM_OAM_GROUP_NAME_LENGTH * NOF_BITS_IN_BYTE)

/** ITMH-Ext size(3) for OAMP-PE and 1 additional byte to be aligned with 4 */
#define DNX_OAM_OAMP_ITMH_EXT_SIZE       4
#define DNX_OAM_OAMP_ITMH_EXT_SIZE_BITS (DNX_OAM_OAMP_ITMH_EXT_SIZE * NOF_BITS_IN_BYTE)

/** PPH size(12) for OAMP-PE*/
#define DNX_OAM_OAMP_PPH_SIZE      12
#define DNX_OAM_OAMP_PPH_SIZE_BITS (DNX_OAM_OAMP_PPH_SIZE * NOF_BITS_IN_BYTE)

/** Generally maximum extra data size could be 80 bytes, but currently we don't need */
#define DNX_OAM_OAMP_EXTRA_DATA_MAX_SIZE (BCM_OAM_GROUP_NAME_LENGTH + DNX_OAM_OAMP_ITMH_EXT_SIZE + DNX_OAM_OAMP_PPH_SIZE)

#define DNX_OAM_MDB_EXTRA_DATA_HDR_SEG_LEN 63
#define DNX_OAM_MDB_EXTRA_DATA_PLD_SEG_LEN 109

/**
 * PP port: the data needed for up-MEPs
 * Is a port index and a core ID
 */
typedef struct
{
    uint8 pp_port;
    int8 port_core;
} upmep_port_t;

/**
 * This union covers all possibilities:
 * - For up-MEP, the above structure
 * - Otherwise, just a profile index
 */
typedef union
{
    upmep_port_t ccm_eth_up_mep_port;
    uint16 port_profile;
} u_port_access;

/** Structure for accelerated OAM CCM endpoints   */
typedef struct
{
    /**
     *  MEP type.  Values for OAM:
     *  Ethernet OAM:
     *  DBAL_ENUM_FVAL_OAMP_MEP_TYPE_ETH_OAM = 0
     *  Y1731 OAM over MPLS-TP:
     *  DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_MPLSTP = 1
     *  Y1731 OAM over PWE:
     *  DBAL_ENUM_FVAL_OAMP_MEP_TYPE_Y1731_PWE = 2
     */
    dbal_enum_value_field_oamp_mep_type_e mep_type;

    /** See flag mask definitions named DNX_OAMP_OAM_CCM_MEP_XXX  */
    uint32 flags;

    /** Only one of these values can be used.  See flag above   */
    union
    {
        /** FEC ID   */
        uint32 fec_id;

        /** Global out-LIF   */
        uint32 glob_out_lif;
    } fec_id_or_glob_out_lif;

    /** MPLS/PWE label (for MPLS/PWE only)   */
    uint32 label;

    /** Counter pointer for TxFCf stamping by SLM   */
    uint32 counter_ptr;

    /** VSI used for Egress Down-MEP injection */
    uint32 vsi;
    /**
     *  Endpoint identification number used in transmitted CCM
     *  packets
     */
    uint16 mep_id;

    /** MPLS-TP/PWE only: system port   */
    uint16 dest_sys_port_agr;

    /**
     *  Pointer to extra data header entry (48b MAID params) - self
     *  contained entries only.  For offloaded entries, extra data
     *  entries are automatically created in the MDB.
     */
    uint16 extra_data_header;

    /**
     *  Pointer to LM/DM entry for self-contained endpoints or
     *  Part 2 entry for offloaded endpoints.
     */
    uint16 flex_lm_dm_entry;

    /**
     *  2 byte MAID - relevant only if 48b MAID flag is clear
     */
    uint16 maid;

    /** VID of the inner VLAN   */
    uint16 inner_vid;

    /** VID of the outer VLAN   */
    uint16 outer_vid;

    /** 15 least significant bits of the source MAC address   */
    uint16 sa_gen_lsb;

    /**
     *  Interval between sending CCM packets for this endpoint.
     */
    dbal_enum_value_field_ccm_interval_e ccm_interval;

    /**
     *  choice between two possible values for the 33 most
     *  significant bits of the source MAC address
     */
    uint8 sa_gen_msb_profile;

    /** ICC pointer for ICC based MAID   */
    uint8 icc_index;

    /** MD Level   */
    uint8 mdl;

    /** Number VLAN tags (0, 1 or 2)   */
    uint8 nof_vlan_tags;

    /** Pointer to TPID value of inner VLAN   */
    uint8 inner_tpid_index;

    /** PCP DEI of the inner VLAN   */
    uint8 inner_pcp_dei;

    /** Pointer to TPID value of outer VLAN   */
    uint8 outer_tpid_index;

    /** PCP DEI of the outer VLAN   */
    uint8 outer_pcp_dei;

    /**
     *  Code for interface status TLV;
     *  0 - no interface status TLV
     */
    uint8 interface_status_tlv_code;

    /** Packet priority - mapped ITMH TC/DP   */
    uint8 itmh_tc_dp_profile;

    /**
     *  Points to an entry that contains more endpoint
     *  parameters, mostly LM/DM
     */
    uint8 mep_profile;

    /**
     *  Points to an entry that contains parameters that dictate
     *  programmable editor modification of packets generated by
     *  this endpoint
     */
    uint8 mep_pe_profile;

    /** PP port - mapping according to MEP type */
    u_port_access unified_port_access;

    /**
     *  MPLS/PWE only: points to a profile that contains TTL and
     *  QoS values
     */
    uint8 push_profile;

    /** Counter for this endpoint is accessed by this interface   */
    uint8 counter_interface;

    /** Number of the core performing the counter processing */
    uint8 crps_core_select;

} dnx_oam_oamp_ccm_endpoint_t;

/** Flags used in LM APIs   */

/** Update flag - always necessary because of allocation  */
#define DNX_OAMP_OAM_LM_MEP_UPDATE                      SAL_BIT(0)

/** 
 *  For self-contained mode: use a second entry (the same offset
 *  in the next entry bank) for LM_STAT
 *  For LM/DM offloaded mode: add the optional second MEP DB
 *  entry
 */
#define DNX_OAMP_OAM_LM_MEP_ADD_STAT_ENTRY              SAL_BIT(1)

/** Measure the next received SLR  */
#define DNX_OAMP_OAM_LM_MEP_MEASURE_NEXT_RECEIVED_SLR   SAL_BIT(2)

/** For MDB entries, an access key is necessary   */

/**
 *  Access key for the first of two mandatory MDB entries for
 *  LM/DM offloaded endpoints
 */
#define MDB_PART_1_ACCESS 0
/**
 *  Access key for the second of two mandatory MDB entries for
 *  LM/DM offloaded endpoints
 */
#define MDB_PART_2_ACCESS 1

/** Access key for the mandatory MDB entry for 1/4 endpoints  */
#define MDB_Q_ENTRY_ACCESS 0

/** Access keys for the MDB entries for 48B MAID */
#define MDB_EXTRA_DATA_HDR_ACCESS  2
#define MDB_EXTRA_DATA_PLD1_ACCESS 3
#define MDB_EXTRA_DATA_PLD2_ACCESS 4
#define MDB_EXTRA_DATA_PLD3_ACCESS 5
#define MDB_EXTRA_DATA_PLD4_ACCESS 6
#define MDB_EXTRA_DATA_PLD5_ACCESS 7

/** maximum size of MEP_DB or MDB extra data header + extra data payload entries */
#define OAM_OAMP_EXTRA_DATA_MAX_SIZE 80

/** MEP PE PROFILE and TCAM constants */
#define DNX_OAM_OAMP_OPCODE_CCM_CCM_LM SAL_BIT(0)

#define DNX_OAM_OAMP_GACH_SELECT   0
#define DNX_OAM_OAMP_OPCODE_SELECT 1

/** LM_DM find and add flags */
#define LM_DM_FIND_AND_ADD 0
#define LM_DM_FIND_ONLY 1

typedef struct
{
    /** See flag mask definitions named OAM_LM_MEP_XXX  */
    uint32 flags;

    /** Local LM count for transmitted packets - read only */
    uint32 lm_my_tx;

    /** Local LM count for received packets - read only */
    uint32 lm_my_rx;

    /** Remote LM count for transmitted packets - read only */
    uint32 lm_peer_tx;

    /** Remote LM count for received packets - read only */
    uint32 lm_peer_rx;

    /** Accumulated far endpoint packet losses - read only */
    uint32 acc_lm_far;

    /** Accumulated near endpoint packet losses - read only */
    uint32 acc_lm_near;

    /**
     *  Entry to use for LM_DB entry for self-contained endpoints or
     *  second "dynamic" MEP DB entry for LM/DM offloaded endpoints.
     */
    uint32 flex_lm_entry;

    /** Last loss measurement value from far endpoint - read only */
    uint16 last_lm_far;

    /**
     *  Last loss measurement value from near endpoint - read
     *  only
     */
    uint16 last_lm_near;

    /**
     *  Highest number packet losses from a single far endpoint -
     *  read only
     */
    uint16 max_lm_far;

    /**
     *  Highest number packet losses from a single near endpoint -
     *  read only
     */
    uint16 max_lm_near;

    /**
     *  Profile - select the least 3 significant bytes of the MAC
     *  destination address in LMM injection
     */
    uint8 lmm_dmm_da_profile;

    /*
     * LM DB entry ID - Entry ID of the LM_DB entry in case of self
     * contained endpoints. If it is non zero, indicates LM is enabled
     * as well on the endpoint.  - Read only
     */
    uint32 lm_db_entry_idx;
    /*
     * LM STAT entry ID - Entry ID of the LM_STAT entry in case of self
     * contained endpoints. If it is non zero, indicates LM STAT is enabled
     * as well on the endpoint.  - Read only
     */
    uint32 lm_stat_entry_idx;

    /*
     * OAMP Mep profile 
     */
    uint8 mep_profile;
} dnx_oam_oamp_endpoint_lm_params_t;

/** Flags used in DM APIs   */

/** Add a one-way DM statistics entry   */
#define DNX_OAMP_OAM_DM_MEP_MEASURE_ONE_WAY  SAL_BIT(0)

/** Add a two-way DM statistics entry   */
#define DNX_OAMP_OAM_DM_MEP_MEASURE_TWO_WAY  SAL_BIT(1)

typedef struct
{
    /** See flag mask definitions named DNX_OAM_DM_MEP_XXX  */
    uint32 flags;

    /**
     *  Entry to use for DM_STAT_ONE_WAY entry or DM_STAT_TWO_WAY_ENTRY.
     *  Relevant on for self contained mode where LM has not been
     *  added, or for offloaded entries.
     */
    uint32 flex_dm_entry;

    /** Last value of two-way delay measurement - read only */
    uint8 last_delay[TWO_WAY_DELAY_SIZE];

    /** Highest two-way delay measurement - read only */
    uint8 max_delay[TWO_WAY_DELAY_SIZE];

    /** Lowest two-way delay measurement - read only */
    uint8 min_delay[TWO_WAY_DELAY_SIZE];

    /** Highest one-way delay measurement (DMM) - read only */
    uint8 max_delay_dmm[ONE_WAY_DELAY_SIZE];

    /** Highest one-way delay measurement (DMR) - read only */
    uint8 max_delay_dmr[ONE_WAY_DELAY_SIZE];

    /** Lowest one-way delay measurement (DMM) - read only */
    uint8 min_delay_dmm[ONE_WAY_DELAY_SIZE];

    /** Lowest one-way delay measurement (DMR) - read only */
    uint8 min_delay_dmr[ONE_WAY_DELAY_SIZE];

    /** Last value of one-way delay measurement (DMM) - read only */
    uint8 last_delay_dmm[ONE_WAY_DELAY_SIZE];

    /** Last value of one-way delay measurement (DMR) - read only */
    uint8 last_delay_dmr[ONE_WAY_DELAY_SIZE];

    /**
     *  Profile - select the least 3 significant bytes of the MAC
     *  destination address in LMM/DMM injection
     */
    uint8 lmm_dmm_da_profile;

    /*
     * DM STAT entry ID - Entry ID of the DM_STAT entry in case of self
     * contained endpoints. If it is non zero, indicates DM is enabled
     * as well on the endpoint.  - Read only
     */
    uint32 dm_stat_entry_idx;

    /*
     * OAMP Mep profile 
     */
    uint8 mep_profile;
} dnx_oam_oamp_endpoint_dm_params_t;

typedef struct
{
    /** Interface status*/
    uint8 interface_status;

    /** Port status*/
    uint8 port_status;

} dnx_oam_oamp_oam_state_t;

typedef struct
{
    /** BFD state*/
    uint8 state;

    /** Diag profile */
    uint8 diag_profile;

    /** Detect multiplier*/
    uint16 detect_multiplier;

    /** Flags profile*/
    uint8 flags_profile;

} dnx_oam_oamp_bfd_state_t;

typedef struct
{
    /** oam id*/
    uint16 oam_id;

    /** Interval used for Loss of continuity in microseconds*/
    uint32 period;

    /** Threshold for loss of continuity mechanism */
    uint8 loc_clear_threshold;

    /** punt profile */
    uint8 punt_profile;

    /** punt good packets profile */
    uint8 punt_good_profile;

    /** rmep status for oam and bfd*/
    union
    {
        dnx_oam_oamp_oam_state_t eth_state;

        dnx_oam_oamp_bfd_state_t bfd_state;
    } rmep_state;

    /** Indication that last CCM message had the RDI bit(read only)*/
    uint8 rdi_received;

    /**Indication Loss of continuity happen(read only) */
    uint8 loc;

} dnx_oam_oamp_rmep_db_entry_t;

/**
 *  This enumerator lists the 1 bit fields in the OAMP MEP DB
 *  table - to be used for masking when reading from/writing
 *  to 32-bit "flags" variables.  Note that when both the
 *  OAMP_LM_DM_OFFLOADED flag and the OAMP_MEP_Q_ENTRY flag
 *  are set, the entry is a type 2 LM/DM offloaded entry.
 */
typedef enum
{
    OAMP_MEP_UPDATE,
    OAMP_MEP_LM_DM_OFFLOADED,
    OAMP_MEP_Q_ENTRY,
    OAMP_MEP_IS_UPMEP,
    OAMP_MEP_RDI_FROM_SCANNER,
    OAMP_MEP_RDI_FROM_PACKET,
    OAMP_MEP_PORT_TLV_EN,
    OAMP_MEP_PORT_TLV_VAL,
    /**
     * This flag is always set to multicast,
     * because unicast is not supported.
     */
    OAMP_MEP_MC_UC_SEL,
    OAMP_MEP_BFD_TUNNEL_MPLS_ENABLE,
    OAMP_MEP_MICRO_BFD,
    OAMP_MEP_BFD_ECHO,
    OAMP_MEP_EGRESS_INJECTION,
    OAMP_MEP_ACH,
    OAMP_MEP_GAL,
    OAMP_MEP_ROUTER_ALERT,
    OAMP_MEP_LAST_ENTRY,
    OAMP_MEP_LM_CNTRS_VALID,
    OAMP_MEP_MEASURE_NEXT_RECEIVED_SLR,
    OAMP_MEP_LM_ENABLE,
    OAMP_MEP_DM_ENABLE,
    OAMP_MEP_LM_STAT_ENABLE,
    OAMP_MEP_DM_STAT_ENABLE,
    OAMP_MEP_RESET_LM_STATISTICS,
    OAMP_MEP_RESET_DM_STATISTICS,
    OAMP_MEP_COPY_LM_STATISTICS,
    OAMP_MEP_COPY_DM_STATISTICS
} dnx_oam_oamp_mep_db_flags_e;

/**
 *  This structure is used to pass arguments to the OAMP MEP DB
 *  tables
 */
typedef struct
{
    union
    {
        struct
        {
            uint32 bfd_your_disc;
            uint32 bfd_ipv4_dip;
            uint8 bfd_tx_rate;
            uint8 bfd_src_ip_profile;
            uint8 bfd_detect_mult;
            uint8 bfd_min_tx_interval_profile;
            uint8 bfd_min_rx_interval_profile;
            uint8 bfd_diag_profile;
            uint8 bfd_flags_profile;
            uint8 bfd_ip_ttl_tos_profile;
            uint8 bfd_sta;
            uint8 bfd_ach_sel;
        } bfd_only;
        struct
        {
            uint16 oam_maid;
            uint16 oam_inner_vid;
            uint16 oam_outer_vid;
            uint16 oam_mep_id;
            uint16 oam_sa_mac_lsb;
            uint8 oam_sa_mac_msb_profile;
            uint8 oam_ccm_interval;
            uint8 oam_mdl;
            uint8 oam_inner_tpid_index;
            uint8 oam_inner_dei_pcp;
            uint8 oam_outer_tpid_index;
            uint8 oam_outer_dei_pcp;
            uint8 oam_icc_index;
            uint8 oam_nof_vlan_tags;
            uint8 oam_prt_qualifier_profile;
            uint8 oam_interface_status_tlv_code;
            uint8 oam_lmm_da_profile;
        } oam_only;
        struct
        {
            uint16 crc_val1;
            uint16 crc_val2;
            uint16 opcode_bmp;
            uint8 data_segment[DATA_SEG_LEN];
            uint8 extra_data_len;
        } extra_data;
        struct
        {
            uint32 lm_my_tx;
            uint32 lm_my_rx;
            uint32 lm_peer_tx;
            uint32 lm_peer_rx;
            uint32 acc_lm_far;
            uint32 acc_lm_near;
            uint16 last_lm_far;
            uint16 last_lm_near;
            uint16 max_lm_far;
            uint16 max_lm_near;
            uint8 max_delay[TWO_WAY_DELAY_SIZE];
            uint8 min_delay[TWO_WAY_DELAY_SIZE];
            uint8 last_delay[TWO_WAY_DELAY_SIZE];
            uint8 last_delay_dmm[ONE_WAY_DELAY_SIZE];
            uint8 last_delay_dmr[ONE_WAY_DELAY_SIZE];
            uint8 max_delay_dmm[ONE_WAY_DELAY_SIZE];
            uint8 max_delay_dmr[ONE_WAY_DELAY_SIZE];
            uint8 min_delay_dmm[ONE_WAY_DELAY_SIZE];
            uint8 min_delay_dmr[ONE_WAY_DELAY_SIZE];
        } lm_dm;
    } exclusive;
    struct
    {
        uint32 label;
        uint8 push_profile;
    } mpls_pwe;
    uint32 flags;
    uint32 fec_id_or_glob_out_lif;
    uint32 counter_ptr;
    uint16 dest_sys_port_agr;
    uint32 extra_data_ptr;
    uint16 flex_lm_dm_ptr;
    dbal_enum_value_field_oamp_mep_type_e mep_type;
    uint8 itmh_tc_dp_profile;
    uint8 ip_ttl_tos_probile;
    uint8 mep_profile;
    uint8 mep_pe_profile;
    u_port_access unified_port_access;
    uint8 ip_subnet_len;
    uint8 counter_interface;
    uint8 crps_core_select;
} dnx_oam_oamp_mep_db_args_t;

typedef enum
{
    DNX_OAMP_OAM_MA_NAME_TYPE_ICC = 0,
    DNX_OAMP_OAM_MA_NAME_TYPE_SHORT,
    DNX_OAMP_OAM_MA_NAME_TYPE_NOF
} dnx_oam_ma_name_type_e;

/* 
 *Get the group name (string) and returns a dnx_oam_ma_name_type_e enum.
 * Macro assumes group is one of the two supported formats:
 * Short: {1,3,2,{two byte name},0,0,...,0}
 * ICC based: {1,32,13,{13 byte name},0,0,...,0}
 */
#define BCM_DNX_OAM_GROUP_NAME_TO_NAME_TYPE(name) \
            ( (name[1]==DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_ICC) ?\
            DNX_OAMP_OAM_MA_NAME_TYPE_ICC : \
             (name[1]==DNX_OAMP_OAM_GROUP_MA_NAME_FORMAT_SHORT)?\
            DNX_OAMP_OAM_MA_NAME_TYPE_SHORT : DNX_OAMP_OAM_MA_NAME_TYPE_NOF)

/* oam_group_umc_data_t  used to store OAM group UMC data*/
typedef uint8 oam_group_umc_data_t[5];

typedef struct
{
    /*
     * Rate of sampling packets to CPU. values ranges from 0 to 7 representing
     * the number of packets (with events) required to be received before 
     * sampling a packet to CPU. 
     */
    uint8 punt_rate;
    /*
     * '0' disables punt of packet in case of state/RDI change or validity check fail;
     * '1' enables the punt according to Punt Rate
     */
    uint8 punt_enable;
    /*
     * '00' Do not update state and RDI according to received packet; generate event. 
     * '01' Update state and RDI according to received packet; don't generate event. 
     * '10' Update state and RDI only if event FIFO is not full; generate event.
     * '11' Update state and RDI  regardless of event FIFO; generate event.
     */
    dbal_enum_value_field_profile_rx_state_update_en_e rx_state_update_en;
    /*
     * '00' Do not update rmep_db state on scan; generate event. 
     * '01' Update rmep_db state on scan; don't generate event.
     * '10' Update rmep_db state only if event FIFO is not full; generate event.
     * '11' Update rmep_db state regardless of event FIFO; generate event.
     */
    dbal_enum_value_field_profile_scan_state_update_en_e profile_scan_state_update_en;

    /*
     * Enable the scanner machine to update MEP DB RDI indication in case LoC (timeout) event detected 
     */
    uint8 mep_rdi_update_loc_en;

    /*
     * Enable the scanner machine to update MEP DB RDI indication in case LoC clear (time-in) event detected 
     */
    uint8 mep_rdi_update_loc_clear_en;

    /*
     * Enables the RX machine to update the MEP DB RDI indication by copy the RDI indication from the received valid
     * CCM packet 
     */
    uint8 mep_rdi_update_rx_en;
} dnx_oam_oamp_punt_event_profile_t;

typedef enum
{
    OAMP_MEP_TX_OPCODE_CCM_BFD = 0,
    OAMP_MEP_TX_OPCODE_LMM = 1,
    OAMP_MEP_TX_OPCODE_DMM = 2,
    OAMP_MEP_TX_OPCODE_OP0 = 3,
    OAMP_MEP_TX_OPCODE_OP1 = 4,
    OAMP_MEP_TX_OPCODE_MAX_SUPPORTED = 5
} dnx_oam_oamp_tx_opcode;

/**
 * \brief - This function initializes OAMP tables related to CPU.
 *
 * \param [in] unit - Number of hardware unit used.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * Registers and memories initialized in this function:
 *
 *   The following registers are initialized together in a dbal
 *       a. OAMP_INIT_CPU_PORT_CFG table(OAMP_CPU_PORT)\n
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_init_cpu_port_configuration(
    int unit);

/**
 * \brief - This function creates or modifies the dbal entries
 *        needed for an accelerated OAM CCM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.  For full
 *        entry this must be a multiple or 4.
 * \param [in] entry_values - Structure containing all the
 *        endpoint parameters
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_ccm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_ccm_endpoint_t * entry_values);

/**
 * \brief - This function reads the data relevant to a single
 *        accelerated OAM CCM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.  For full
 *        entry this must be a multiple or 4.
 * \param [in] is_down_mep_egress_injection - indication if endpoint is down mep egress injection
 * \param [out] entry_values - Structure into which all the
 *        endpoint parameters are written.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_ccm_endpoint_get(
    int unit,
    uint16 oam_id,
    uint8 is_down_mep_egress_injection,
    dnx_oam_oamp_ccm_endpoint_t * entry_values);

/**
 * \brief - This function deletes the entries used for
 *        accelerating an OAM CCM endpoint from dbal (clearing
 *        them from the hardware tables as well)
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.  For full
 *        entry this must be a multiple or 4.
 * \param [in] extra_data_length - extra data length in bits
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_ccm_endpoint_clear(
    int unit,
    uint16 oam_id,
    uint8 extra_data_length);

/**
 * \brief - This function adds, modifies or enables entries
 *        needed to allow an accelerated endpoint to collect LM
 *        statistics
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [in] entry_values - Only the flags are relevant here.
 *        The rest are read-only
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_lm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values);

/**
 * \brief - This function reads the LM statistics for an
 *        accelerated OAM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [out] entry_values - a structure into which the
 *        statistics will be written
 * \param [out] lm_exists - whether lm is enabled or not.
 * \param [out] lm_stat_exists - whether lm stat entry is found or not.
 *                               This makes sense only for self contained endpoints.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_lm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_lm_params_t * entry_values,
    uint8 *lm_exists,
    uint8 *lm_stat_exists);

/**
 * \brief - This function is relevant for self-contained OAM
 *        endpoints only.  The LM entries for this endpoint will
 *        be deleted from dbal and freed.  If there are any DM
 *        entries succeeding them, they will also be deleted.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint that will have
 *        its LM/DM entries deleted.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_lm_endpoint_clear(
    int unit,
    uint16 oam_id);

/**
 * \brief - This function adds, modifies or enables entries
 *        needed to allow an accelerated endpoint to collect DM
 *        statistics
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [in] entry_values - Only the flags are relevant here.
 *        The rest are read-only
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_dm_endpoint_set(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values);

/**
 * \brief - This function reads the DM statistics for an
 *        accelerated OAM endpoint
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 *        Must be a multiple or 4.
 * \param [out] entry_values - a structure into which the
 *        statistics will be written
 * \param [out] is_found - Whether dm entry exists or not.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_dm_endpoint_get(
    int unit,
    uint16 oam_id,
    dnx_oam_oamp_endpoint_dm_params_t * entry_values,
    uint8 *is_found);

/**
 * \brief - This function deletes the OAMP DM entry for an
 *        accelerated OAM endpoint.  This can apply to both
 *        self-contained mode and offloaded mode.  The entries
 *        are deleted from DBAL and freed in the hardware.  For
 *        self-contained mode, the "last entry" bit will be set
 *        for the relevant LM entry.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAMP endpoint entry that
 *        will have its DM entries deleted.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_dm_endpoint_clear(
    int unit,
    uint16 oam_id);

/**
 * \brief - This function add entry to RMEP_INDEX_DB.
 *          This DB map OAM-ID and MEP-ID to Remote MEP index(RMEP_INDEX).
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 * \param [in] mep_id - Index of Maintenance End Point
 * \param [in] rmep_id - Remote MEP DB(RMEP_INDEX) id.
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_EXIST - entry exist
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_rmep_index_db_add(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 rmep_id);

/**
 * \brief - This function return the Index of Remote MEP according OAM-ID and MEP-ID.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 * \param [in] mep_id - Index of Maintenance End Point
 * \param [out] rmep_id - Remote MEP DB(RMEP_INDEX) id.
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_NOT_FOUND - entry not exist
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_rmep_index_db_get(
    int unit,
    uint16 oam_id,
    uint16 mep_id,
    uint32 *rmep_id);

/**
 * \brief - This function remove the entry from RMEP_INDEX_DB table.
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] oam_id - Index of OAM endpoint.
 * \param [in] mep_id - Index of Maintenance End Point
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_NOT_FOUND - entry not exist
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_rmep_index_db_remove(
    int unit,
    uint16 oam_id,
    uint16 mep_id);

/**
 * \brief - This function add Remote MEP to OAMP_RMEP_DB table.
 *          In case the entry is offloaded function add static part of Remote MEP to OAMP_RMEP_STATIC_DATA_DB table
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] rmep_id - Remote MEP ID
 * \param [in] is_oam - Indication if RMEP is oam('1) of BFD('0')
 * \param [in] is_update - Indication if rmep is new or update
 * \param [in] rmep_entry - Remote MEP parameters
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_NOT_FOUND - entry not exist(in case is_update is '1')
 *   \retval _SHR_E_EXIST - entry already exist(in case is_update is '0'

 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_rmep_add(
    int unit,
    uint32 rmep_id,
    uint32 is_oam,
    uint32 is_update,
    dnx_oam_oamp_rmep_db_entry_t * rmep_entry);

/**
 * \brief - This function return Remote MEP parameters according Remote MEP ID.
 *          Parameters are taken from OAMP_RMEP_DB table and in case of offloaded entry from  OAMP_RMEP_STATIC_DATA_DB table
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] rmep_id - Remote MEP ID
 * \param [in] is_oam - Indication if RMEP is oam('1) of BFD('0')
 * \param [out] rmep_entry - Remote MEP parameters
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_NOT_FOUND
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_rmep_get(
    int unit,
    uint32 rmep_id,
    uint32 is_oam,
    dnx_oam_oamp_rmep_db_entry_t * rmep_entry);

/**
 * \brief - This function remove Remote MEP entry from OAMP_RMEP_DB.
 *          In case the entry is offloaded remove static part of Remote MEP from OAMP_RMEP_STATIC_DATA_DB table
 *
 * \param [in] unit - Number of hardware unit used
 * \param [in] rmep_id - Remote MEP ID
 * \param [in] is_oam - Indication if RMEP is oam('1) of BFD('0')
 *
 * \return
 *   shr_error_e
 *   \retval _SHR_E_NONE - no error
 *   \retval _SHR_E_NOT_FOUND - entry not exist
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_rmep_remove(
    int unit,
    uint32 rmep_id,
    uint32 is_oam);

/**
 * \brief - This function gets OAMP punt profile from HW.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - Punt profile index.
 * \param [out] punt_profile_data - Punt profile data read from HW
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   This function get the memory under PUNT_PROFILE_TBL DBAL table.
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_punt_profile_tbl_get(
    int unit,
    int profile_id,
    dnx_oam_oamp_punt_event_profile_t * punt_profile_data);

/**
 * \brief - This function writes OAMP punt profile into HW.
 * 
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile_id - Punt profile index.
 * \param [in] punt_profile_data - Punt profile data to write to HW
 *   
 * \return
 *   shr_error_e
 *   
 * \remark
 *   This function set the memory under PUNT_PROFILE_TBL DBAL table.
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_punt_profile_tbl_set(
    int unit,
    int profile_id,
    const dnx_oam_oamp_punt_event_profile_t * punt_profile_data);

/**
 * \brief -
 * Initialize OAMP module.
 * This function calls initialazation functions for OAMP registers/memories
 *
 * Called from oam init
 *
 * \param [in] unit - Number of hardware unit used
 *
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_init(
    int unit);

/**
 * \brief -
 * De-initialize OAMP module.
 *
 * \param [in] unit - Number of hardware unit used
 *
 * \return
 *   \retval Non-zero (!= _SHR_E_NONE) in case of an error
 *   \retval Zero (= _SHR_E_NONE) in case of NO ERROR
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_deinit(
    int unit);

/**
 * \brief - This function adds or modifies a single entry in the
 *        OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - ID of entry to add or modify.
 *        For full entry, this must be a multiple of 4.
 * \param [in] entry_values - Structure containing the fields to
 *        write to the target entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mep_db_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function returns the arguments of a single
 *        entry in the OAMP MEP DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - ID of entry to read.  For full entry,
 *        this must be a multiple of 4, that is OAM_ID.
 * \param [in] entry_values - Pointer to a struct to which the
 *        entry's arguments will be written.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mep_db_get(
    int unit,
    const uint32 entry_id,
    dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function deletes a single entry in the OAMP MEP
 *        DB table
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] entry_id - entry_id - ID of entry to delete.  For
 *        full entry, this must be a multiple of 4.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mep_db_delete(
    int unit,
    const uint32 entry_id);

/**
 * \brief - This function adds or modifies a single entry in the
 *        MDB OAMP MEP DB static data table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] entry_values - struct containing the values to be
 *        written to the entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   While some
 *   endpoints require multiple MDB entries, this function
 *   only adds one, so it must be called repeatedly.
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mdb_add(
    int unit,
    const uint32 oamp_entry_id,
    const dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function reads a single entry in the
 *        MDB OAMP MEP DB static data table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] access_index - this parameter specifies which MDB
 *        entry should be retrieved (up to 6 supported)
 * \param [out] entry_values - struct containing the values to
 *        be written to the entry.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   While some endpoints require multiple MDB entries, this
 *   function reads adds one, so it must be called repeatedly.
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mdb_get(
    int unit,
    const uint32 oamp_entry_id,
    const uint32 access_index,
    dnx_oam_oamp_mep_db_args_t * entry_values);

/**
 * \brief - This function deletes a single entry in the MDB OAMP
 *        MEP DB static data table.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oamp_entry_id - Physical location of this endpoint's
 *        entry in the OAMP MEP DB table, in quarter entries
 * \param [in] access_index - this parameter specifies which MDB
 *        entry should be deleted (up to 6 supported)
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mdb_delete(
    int unit,
    const uint32 oamp_entry_id,
    const uint32 access_index);

/**
 * \brief - This function adds OAMP MEP profile entry that is used in
 *          OAM PDU TX from OAMP
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile - MEP profile index
 * \param [in] mep_profile_info - Pointer to MEP profile information structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mep_profile_set(
    int unit,
    uint8 mep_profile,
    const dnx_oam_mep_profile_t * mep_profile_info);

/**
 * \brief - This function gets OAMP MEP profile entry that is used in
 *          OAM PDU TX from OAMP
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_profile - MEP profile index
 * \param [out] mep_profile_info - Pointer to MEP profile information structure
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mep_profile_get(
    int unit,
    uint8 mep_profile,
    dnx_oam_mep_profile_t * mep_profile_info);

/**
 * \brief - This function add priority(tc/dp) profile that is used in ITMH header
 *
 * \param [in] unit - Number of hardware unit used.
* \param [in] profile - Priority profile(maximum 8 profiles)
 * \param [in] tc - traffic class
 * \param [in] dp - drop precedence
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_itmh_priority_profile_set(
    int unit,
    uint8 profile,
    uint8 tc,
    uint8 dp);

/**
 * \brief - This function provide priority(tc/dp) according profile.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - Priority profile(maximum 8 profiles)
 * \param [out] tc - traffic class
 * \param [out] dp - drop precedence
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_itmh_priority_profile_get(
    int unit,
    uint8 profile,
    uint8 *tc,
    uint8 *dp);

/**
 * \brief - This function add mpls/pwe push profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MPLS/PWE push profile(maximum 16 profiles
 * \param [in] ttl - time-to-live bits
 * \param [in] exp - experimental bits
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mpls_pwe_exp_ttl_profile_set(
    int unit,
    uint8 profile,
    uint8 ttl,
    uint8 exp);

/**
 * \brief - This function provide exp and ttl bits according mpls/pwe push profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MPLS/PWE push profile(maximum 16 profiles
 * \param [out] ttl - time-to-live bits
 * \param [out] exp - experimental bits
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mpls_pwe_exp_ttl_profile_get(
    int unit,
    uint8 profile,
    uint8 *exp,
    uint8 *ttl);

/**
 * \brief - This function add TPID profile used by MEP_DB
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - TPID Profile/Index(maximum 4 profiles)
 * \param [in] tpid - tpid
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_tpid_profile_set(
    int unit,
    uint8 profile,
    uint16 tpid);

/**
 * \brief - This function provide tpid according tpid profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - tpid profile profile(maximum 4 profiles)
 * \param [out] tpid - tpid
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_tpid_profile_get(
    int unit,
    uint8 profile,
    uint16 *tpid);

/**
 * \brief - This function map local port to system port
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] pp_port_profile - key to table entry
 * \param [in] system_port - system_port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_local_port_to_system_port_set(
    int unit,
    uint16 pp_port_profile,
    uint32 system_port);
/**
 * \brief - This function provide mapping of local port to system port
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] pp_port_profile - key to table entry
 * \param [out] system_port - system_port
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_local_port_to_system_port_get(
    int unit,
    uint16 pp_port_profile,
    uint32 *system_port);

/**
 * \brief - This function set Source MAC profile(33 MSB bits)
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - mac profile(maximum 2 profiles)
 * \param [in] sa_mac_addr_msb - Source mac address(only 33 msb bits are relevant
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_sa_mac_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t sa_mac_addr_msb);

/**
 * \brief - This function provide Source MAC address(33 MSB bits) according profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - mac profile(maximum 2 profiles)
 * \param [out] sa_mac_addr_msb - Source mac address(only 33 msb bits are relevant
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_sa_mac_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t sa_mac_addr_msb);

/**
 * \brief - Function set/update/remove profiles that were changed during endpoint create/modify/destroy
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] mep_hw_profiles_write_data - parameters of profiles
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * oam_oamp_profile_hw_data_t
 */
shr_error_e dnx_oam_oamp_profile_hw_set(
    int unit,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data);

/**
 * \brief - map bcm endpoint type to oamp mep db type
 *
 * \param [in] unit - Relevant unit.
 * \param [in] bcm_mep_type - MEP BCM Type
 * \param [out] mep_db_mep_type - MEP type used in MEP_DB
 *
 * \retval Error indication.
 *
 */
shr_error_e dnx_oam_oamp_endpoint_bcm_mep_type_to_mep_db_mep_type(
    int unit,
    const bcm_oam_endpoint_type_t bcm_mep_type,
    dbal_enum_value_field_oamp_mep_type_e * mep_db_mep_type);

/**
* \brief
*  Get an Local OAM accelerated Endpoint information.
* \param [in] unit  -
*  Relevant unit.
* \param [out] endpoint_info -
*  Pointer to the structure, in which to write the information
*  from the Get procedure.
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*/
shr_error_e dnx_oam_local_acc_endpoint_get(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info);

/**
* \brief
*  Function release endpoint profiles(sw) related to mep_db
* \param [in] unit  - Relevant unit.
* \param [in] mep_db_entry - mep_db entry according endpoint_info
* \param [out] mep_hw_profiles_write_data - return profiles that should be removed(data cleared) in hw
* \retval
*   shr_error - Error indication
* \remark
*   * None
* \see
*   * \ref bcm_dnx_oam_endpoint_create
*   * \ref bcm_dnx_oam_endpoint_destroy
*   * \ref bcm_dnx_oam_endpoint_destroy_all
*/
shr_error_e dnx_oam_oamp_ccm_endpoint_mep_db_profiles_free(
    int unit,
    const dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data);

/**
 * \brief - Function fill mep_db struct. Assumes ID is allocated
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the requested MEP
 *                                 return mep_id if with id flag not set.
 * \param [in] group_info - structure containing MEG parameters
 * \param [out] mep_db_entry - return filled mep_db entry according endpoint_info
 * \param [out] mep_hw_profiles_write_data - return new profiles that should be created in hw
 * \param [out] mep_hw_profiles_delete_data - when modifying, return existing  profiles that
 *        should be deleted from hw
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * dnx_oam_local_mep_create
 */
shr_error_e dnx_oam_oamp_ccm_endpoint_mep_db_create(
    int unit,
    const bcm_oam_endpoint_info_t * endpoint_info,
    const bcm_oam_group_info_t * group_info,
    dnx_oam_oamp_ccm_endpoint_t * mep_db_entry,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_write_data,
    oam_oamp_profile_hw_data_t * mep_hw_profiles_delete_data);

/**

 * \brief - Allocate and fill mep id
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_info - info structure of the 
 *                                 requested MEP return mep_id
 *                                 if with id flag not set.
 * \param [in] group_name - group_name
 *  
 *
 * \return
 *   shr_error_e
 *
 * \see
 *   * None
 */
shr_error_e dnx_oam_local_accelerated_endpoint_fill_id(
    int unit,
    bcm_oam_endpoint_info_t * endpoint_info,
    const uint8 *group_name);

/**
 * \brief - This function configures value at LMM MAC DA OUI(MSB) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA MSB Profile/Index(maximum 8 profiles)
 * \param [in] msb - MSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mac_da_msb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t msb);
/**
 * \brief - This function gets the value configured at LMM MAC DA OUI(MSB) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA MSB Profile/Index(maximum 8 profiles)
 * \param [out] msb - MSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mac_da_msb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t msb);
/**
 * \brief - This function configures value at LMM MAC DA NIC(lsb) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA lsb Profile/Index(maximum 8 profiles)
 * \param [in] lsb - LSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mac_da_lsb_profile_set(
    int unit,
    uint8 profile,
    bcm_mac_t lsb);
/**
 * \brief - This function gets the value configured at LMM MAC DA NIC(lsb) profile
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] profile - MAC DA lsb Profile/Index(maximum 8 profiles)
 * \param [out] lsb - LSB of MAC DA
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_mac_da_lsb_profile_get(
    int unit,
    uint8 profile,
    bcm_mac_t lsb);
/**
 * \brief - This function gets the OAMP RMEP DB entry lifetime and lifetime_unit 
 *          value from RMEP structure CCM period paramter
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] period_us - RMEP CCM period value in micro-second
 * \param [out] lifetime - lifetime vlaue in RMEP DB entry
 * \param [out] lifetime_units - lifetime_unit vlaue in RMEP DB entry
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_oam_oamp_period_to_lifetime(
    int unit,
    uint32 period_us,
    uint32 *lifetime,
    uint32 *lifetime_units);

/**
 * \brief - This function checks that the specified OAMP_MEP_DB
 *          short entry either has sub-index 0, or that the short
 *          entry with sub-index 0 has the requested entry type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - number of entry being added.
 * \param [in] req_mep_type - type for comparison.
 * \param [out] sub_index - Calculated sub-index
 * \param [out] is_leader_alloc - TRUE if entry with sub-index
 *        0 is allocated, FALSE otherwise.
 * \param [out] is_leader_type_match - TRUE if entry type is a
 *        match, FALSE otherwise
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_short_entry_type_check_mep_db(
    int unit,
    uint16 oam_id,
    dbal_enum_value_field_oamp_mep_type_e req_mep_type,
    int *sub_index,
    uint8 *is_leader_alloc,
    uint8 *is_leader_type_match);

/**
 * \brief - This function checks that a short entry being allocated
 *          either has sub-index 0, or that the short entry with
 *          sub-index 0 has the same MEP type.
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] oam_id - number of entry being added.
 * \param [in] endpoint_info - OAM entry parameters for
 *             intended entry.
 *
 * \return
 *   void
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_short_entry_type_verify(
    int unit,
    uint16 oam_id,
    const bcm_oam_endpoint_info_t * endpoint_info);

/**
 * \brief - This function sets the OAMP MEP DB entries
 *          for a MEG with a 48B MAID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] group_info - structure containing the MEG params
 * \param [in] calculated_crc - 16-bit CRC used for verifying
 *        incoming CCM packets for endpoints in this MEG
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_48b_maid_set(
    int unit,
    bcm_oam_group_info_t * group_info,
    uint16 calculated_crc);

/**
 * \brief - This function clears the OAMP MEP DB entries
 *          for a MEG with a 48B MAID
 *
 * \param [in] unit - Number of hardware unit used.
 * \param [in] header_entry_index - index of the
 *        EXTRA_DATA_HEADER type entry
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_48b_maid_clear(
    int unit,
    uint32 header_entry_index);

/**
 * \brief
 * This function calculates the phase counts for given opcode in the opcode parameter
 * and updates it in the mep_profile_data. It also changes the other opcodes' phase counts
 * if necessary.
 * \param [in] unit - Number of hardware unit used.
 * \param [in] endpoint_id - id of the endpoint. Useful only if opcode = CCM/BFD
 * \param [in] is_short_entry_type - If it is a quarter entry. Useful only for opcode = CCM/BFD
 * \param [in] ccm_rate - rate of CCM/BFD opcode,since it is not available in mep_profile_data
 * \param [in] ccm_rx_without_tx - Indicates if it is a RX-only MEP.
 * \param [in] opcode   - The opcode for which rate is newly configured.
 * \param [in,out] mep_profile_data - Contains the rates for opcodes. Will be filled with
 *                                    appropriate phase counts.
 *
 * \return
 *   shr_error_e
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oam_oamp_scan_count_calc(
    int unit,
    bcm_oam_endpoint_t endpoint_id,
    uint8 is_short_entry_type,
    dbal_enum_value_field_ccm_interval_e ccm_rate,
    uint8 ccm_rx_without_tx,
    dnx_oam_oamp_tx_opcode opcode,
    dnx_oam_mep_profile_t * mep_profile_data);

/**
 * \brief
 * This function calculate period ccm period value according requested interval.
 * \param [in] ccm_interval - ENUM value for the ccm period.
 * \param [in,out] ccm_period - CCM period in miliseconds.
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
void dnx_oam_oamp_ccm_interval_to_ccm_period(
    dbal_enum_value_field_ccm_interval_e ccm_interval,
    int *ccm_period);

/**
 * \brief
 * This function adds extra data header entry and extra data payload entries to the oamp mep db
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id - index of OAM endpoint
 * \param [in] is_offloaded - indicates if this is an extention to an offloaded entry
 * \param [in] is_q_entry - indicates if this is an extention to 1/4 entry
 * \param [in] calculated_crc - crc to be put in the extra data header entry
 * \param [in] opcode - the opcode for which extra data is created
 * \param [in] nof_data_bits - number of data bits to add to the extra data header and extra data payload entries
 * \param [in] data - pointer the data to be copied to the extra data header and extra data payload entries
 * \param [in] is_update - indication if mep is new or updated
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */

shr_error_e dnx_oamp_mdb_extra_data_set(
    int unit,
    uint16 oam_id,
    uint8 is_offloaded,
    uint8 is_q_entry,
    uint16 calculated_crc,
    int opcode,
    uint32 nof_data_bits,
    uint8 *data,
    uint8 is_update);

/**
 * \brief
 * This function gets oam_id of lm with priority session
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id - index of OAM endpoint
 * \param [in] priority - priority of the session
 * \param [out] session_oam_id - oam_id of the lm session with 
 *        the given priority
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_oamp_lm_with_priority_session_map_get(
    int unit,
    int oam_id,
    uint8 priority,
    uint32 *session_oam_id);

/**
 * \brief
 * This function clears lm session entry for a given oam_id and 
 * priority
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id - index of OAM endpoint
 * \param [in] priority - priority of the session
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e
dnx_oamp_lm_with_priority_session_map_clear(
    int unit,
    int oam_id,
    uint8 priority);

/*
 * This function get extra data header entry from the oamp mdb
 * \param [in] unit - number of hardware unit used
 * \param [in] oam_id- index of OAM endpoint
 * \param [in] extra_data_length - length of extra data in bits
 * \param [out] data - pointer the data to get the extra data header and extra data payload entries
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mdb_extra_data_get(
    int unit,
    uint16 oam_id,
    uint32 extra_data_length,
    uint8 *data
);


/**
 * \brief
 * This function adds extra data header entry and extra data payload entries to the oamp mep db
 * \param [in] unit - number of hardware unit used
 * \param [in] index- index of OAM endpoint
 * \param [in] nof_data_bits - number of data bits to add to the extra data header and extra data payload entries
 * \param [in] data - pointer the data to be copied to the extra data header and extra data payload entries
 * \param [in] opcode - the opcode for which extra data is created
 * \param [in] calculated_crc - crc to be put in the extra data header entry
 * \param [in] is_update - indication if mep is new or updated
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_extra_data_set(
    int unit,
    int index,
    uint32 nof_data_bits,
    uint8 *data,
    int opcode,
    uint16 calculated_crc,
    uint8 is_update);

/**
 * \brief
 * This function get extra data header entry from the oamp mep db
 * \param [in] unit - number of hardware unit used
 * \param [in] index- index of OAM endpoint
 * \param [out] data - pointer the data to get the extra data header and extra data payload entries
 *
 * \return
 *   * None
 *
 * \remark
 *   * None
 * \see
 *   * None
 */
shr_error_e dnx_oamp_mep_db_extra_data_get(
    int unit,
    int index,
    uint8 *data);

/*
 * }
 */
#endif /* OAM_OAMP_H_INCLUDED */
