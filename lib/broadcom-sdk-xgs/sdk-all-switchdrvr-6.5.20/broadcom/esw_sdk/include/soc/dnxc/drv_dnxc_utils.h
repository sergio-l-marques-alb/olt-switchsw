/**
 * \file soc/dnxc/drv_dnxc_utils.h
 *
 * This file contains definitions and prototypes used for 'hiding'
 * drv.h
 *
 */

/*
 * $Copyright: (c) 2020 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
 */

#ifndef DRV_DNX_UTILS_H_INCLUDED
/*
 * {
 */
#define DRV_DNX_UTILS_H_INCLUDED
#include <bcm/types.h>
#include <bcm/switch.h>

#include <soc/memory.h>
#include <soc/feature.h>
#include <soc/register.h>

typedef void (
    *SOC_TIME_CALLOUT_TYPE) (
    int);

/**
 * \brief
 *   Enumeration used for the selection of a specific 'device' to identify by
 *   'SOC_IS_*'.
 * \remark
 *   * Note that 'SOC_IS_*' is implemented using procedure soc_is().
 * \see
 *   * SOC_IS_JERICHO
 *   * SOC_IS_JERICHO_PLUS
 *   * SOC_IS_JERICHO_AND_BELOW
 */
typedef enum
{
    FIRST_SOC_IS_DEVICE_E = 0,
    /*
     *  See SOC_IS_JERICHO()
     */
    JERICHO_DEVICE = FIRST_SOC_IS_DEVICE_E,
    /*
     *  See SOC_IS_JERICHO_AND_BELOW()
     */
    DNX_JERICHO_AND_BELOW,
    /*
     *  See SOC_IS_QMX()
     */
    QMX_DEVICE,
    /*
     *  See SOC_IS_JERICHO_B0_AND_ABOVE()
     */
    JERICHO_B0_AND_ABOVE_DEVICE,
    /*
     *  See SOC_IS_JERICHO_PLUS_A0()
     */
    DNX_JERICHO_PLUS_A0,
    /*
     *  See SOC_IS_J2C()
     */
    J2C_DEVICE,
    /*
     *  See SOC_IS_J2C_TYPE()
     */
    J2C_TYPE,
    /*
     *  See SOC_IS_Q2A()
     */
    Q2A_DEVICE,
    /*
     *  See SOC_IS_Q2A_B0()
     */
    Q2A_B0_DEVICE,
    /*
     *  See SOC_IS_Q2A_B1()
     */
    Q2A_B1_DEVICE,
    /*
     *  See SOC_IS_Q2A_B()
     */
    Q2A_B_DEVICE,
    /*
     *  See SOC_IS_J2P()
     */
    J2P_DEVICE,
    /*
     *  See SOC_IS_JERICHO2_ONLY()
     */
    JERICHO2_ONLY_DEVICE,
    /*
     *  See SOC_IS_JERICHO2_A0()
     */
    JERICHO2_A0_DEVICE,
    /*
     *  See SOC_IS_JERICHO2_B1_ONLY()
     */
    JERICHO2_B1_ONLY,
    /*
     *  See SOC_IS_DNX()
     */
    DNX_DEVICE,
    /*
     *  See SOC_IS_DNX2()
     */
    DNX2_DEVICE,
    /*
     *  See SOC_IS_DNXF()
     */
    DNXF_DEVICE,
    /*
     *  See SOC_IS_DFE()
     */
    DFE_DEVICE,
    /*
     *  See SOC_IS_ARDON()
     */
    ARDON_DEVICE,
    /*
     *  See SOC_IS_ARAD()
     */
    ARAD_DEVICE,
    /*
     *  See SOC_IS_DPP()
     */
    DPP_DEVICE,
    /*
     *  See SOC_IS_LTSW()
     */
    DNX_LTSW,
    /*
     *  See SOC_IS_ARADPLUS_AND_BELOW()
     */
    DNX_ARADPLUS_AND_BELOW,
    /*
     *  See SOC_IS_JERICHO_PLUS_ONLY()
     */
    DNX_JERICHO_PLUS_ONLY,
    /*
     *  See SOC_IS_QUX()
     */
    DNX_QUX,
    /*
     *  See SOC_IS_QAX()
     */
    DNX_QAX,
    /*
     *  See SOC_IS_RAMON()
     */
    DNX_RAMON,
    /*
     *  See SOC_IS_INIT()
     */
    DNX_INIT,
    NUM_SOC_IS_DEVICE_E
} soc_is_device_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific element on 'soc_control_t' structure
 *   within the global structure 'soc_control'.
 * \remark
 *   * Note that 'soc_control_t' is used when invoking the macro SOC_CONTROL().
 * \see
 *   * soc_control_t
 */
typedef enum
{
    /*
     * Start at '0'
     */
    FIRST_SOC_CONTROL_E = 0,
    /**
     *  Identifier of element 'time_call_ref_count' whose type is 'int'.
     */
    TIME_CALL_REF_COUNT = FIRST_SOC_CONTROL_E,
    /**
     * Warm boot cache is dirty. uint32.
     */
    SCACHE_DIRTY,
    SWITCH_EVENT_NOMINAL_STORM,
    /*
     * Interrupts array. Pointer to soc_interrupt_t (intr.h)
     */
    INTERRUPTS_INFO,
    /**
     *  Identifier of element 'soc_time_callout' whose type is: void (*soc_time_callout)(int)
     */
    SOC_TIME_CALLOUT,
    /**
     *  Identifier of element 'soc_flags' whose type is 'uint32'.
     */
    SOC_FLAGS,
    /**
     *  Identifier of element 'mem_scan_pid' whose type is 'sal_thread_t'.
     */
    MEM_SCAN_PID,
    /**
     *  Identifier of element 'mem_scan_interval' whose type is 'sal_usecs_t'.
     */
    MEM_SCAN_INTERVAL,
    /**
     * Update interval in usec
     * (Zero when thread not running)
     */
    COUNTER_INTERVAL,
    /**
     *  Identifier of element 'controlled_counters' whose type is 'soc_controlled_counter_t *'.
     */
    CONTROLLED_COUNTERS,
    /**
     *  Identifier of element 'counter_pbmp' whose type is 'VOL pbmp_t'.
     */
    COUNTER_PBMP,
    /**
     * Packets received by CPU
     */
    DMA_RPKT,
    /**
     * Bytes received by CPU
     */
    DMA_RBYT,
    /**
     * Packets transmitted by CPU
     */
    DMA_TPKT,
    /**
     * Bytes transmitted by CPU
     */
    DMA_TBYT,
    /*
     * Last
     */
    NUM_SOC_CONTROL_E
} soc_control_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific 'ptype' structure within
 *   'soc_info_t'.
 * \remark
 *   * Note that 'soc_info_t' is used when invoking the macro SOC_INFO().
 * \see
 *   * soc_info_t
 */
typedef enum
{
    /*
     * Start at '0'
     */
    FIRST_SOC_PTYPE_E = 0,
    FE_PTYPE = FIRST_SOC_PTYPE_E,
    GE_PTYPE,
    /**
     * 100g port
     */
    CE_PTYPE,
    /**
     * 200g and 400g port
     */
    CDE_PTYPE,
    /**
     * 10g PON port
     */
    TPON_PTYPE,
    /**
     * PON port. #005
     */
    PON_PTYPE,
    /**
     * LLID of epon
     */
    LLID_PTYPE,
    XE_PTYPE,
    /**
     * 50g port
     */
    LE_PTYPE,
    /**
     * DNX 200g port. #009
     */
    CC_PTYPE,
    HG_PTYPE,
    /**
     * Interlaken
     */
    IL_PTYPE,
    /**
     * Stub Control Header
     * #12
     */
    SCH_PTYPE,
    /**
     * Higig subport
     */
    HG_SUBPORT_PTYPE,
    /**
     * Higig Lite
     */
    HL_PTYPE,
    /**
     * hg | ge (Higig Lite). #015
     */
    ST_PTYPE,
    /**
     * gx (Unicore)
     */
    GX_PTYPE,
    /**
     * xl (Warpcore)
     * #17
     */
    XL_PTYPE,
    /**
     * xlb0 (Apache PM 4x10)
     */
    XLB0_PTYPE,
    /**
     * QSGMII ports
     */
    QSGMII_PTYPE,
    /**
     * MXQport (KA). #020
     */
    MXQ_PTYPE,
    /**
     * xg (Hyperlite + Octal Serdes)
     */
    XG_PTYPE,
    /**
     * xg (Unicore + Octal Serdes)
     * #22
     */
    XQ_PTYPE,
    /**
     * xport cores based port
     */
    XT_PTYPE,
    /**
     * clp-lite cores based port
     */
    XW_PTYPE,
    /**
     * cport and xport cores based port. #025
     */
    CL_PTYPE,
    /**
     * clport-gen2 based port
     */
    CLG2_PTYPE,
    /**
     * cxxport - PM 12x10
     * #27
     */
    CXX_PTYPE,
    /**
     * CPRI ports
     */
    CPRI_PTYPE,
    /**
     * RSVD4 ports
     */
    RSVD4_PTYPE,
    /**
     * cport cores based port. #030
     */
    C_PTYPE,
    /**
     * loopback port
     */
    LBPORT_PTYPE,
    /**
     * Re-direction port (EP Redir V2)
     * #30
     */
    RDBPORT_PTYPE,
    /**
     * Flow Analytics Engine Port
     */
    FAEPORT_PTYPE,
    /**
     * macsec port
     */
    MACSECPORT_PTYPE,
    /**
     * Auxilliary port. #035
     */
    AXP_PTYPE,
    /**
     * Hyperlite ports
     */
    HYPLITE_PTYPE,
    /**
     * Internal to the MMU only
     * #35
     */
    MMU_PTYPE,
    /**
     * spi interface port
     */
    SPI_PTYPE,
    /**
     * spi subport
     */
    SPI_SUBPORT_PTYPE,
    /**
     * fabric control. #040
     */
    SCI_PTYPE,
    /**
     * fabric data
     */
    SFI_PTYPE,
    /**
     * requeue port
     * 40
     */
    REQ_PTYPE,
    /**
     * TDM port
     */
    TDM_PTYPE,
    /**
     * RCY port
     */
    RCY_PTYPE,
    /**
     * CD Port cores based port. #045
     */
    CD_PTYPE,
    /**
     * fe|ge|xe|hg|spi|fl
     */
    PORT_PTYPE,
    /**
     * fe|ge|xe
     * #45
     */
    ETHER_PTYPE,
    /**
     * fe|ge|xe|hg|cmic|fl
     */
    ALL_PTYPE,
    /**
     * Packet Processing Port
     */
    PP_PTYPE,
    /**
     * Link PHY Port. #050
     */
    LP_PTYPE,
    /**
     * SubTag Port
     */
    SUBTAG_PTYPE,
    /**
     * Internal processor Port
     * #50
     */
    INTP_PTYPE,
    /**
     * Link bonding group port. #053
     */
    LBGPORT_PTYPE,
    /**
     * FlexE busA client. #054
     */
    FLEXE_CLIENT_PTYPE,
    /**
     * FlexE MAC client. #055
     */
    FLEXE_MAC_PTYPE,
    /**
     * FlexE SAR client. #056
     */
    FLEXE_SAR_PTYPE,
    /**
     * FlexE PHY. #057
     */
    FLEXE_PHY_PTYPE,
    /*
     * Last
     */
    NUM_SOC_PTYPE_E
} soc_ptype_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific 'pbmp' structure within
 *   'soc_info_t' or structures derived from it (like 'custom_reg_access').
 * \remark
 *   * Note that 'soc_info_t' is used when invoking the macro SOC_INFO().
 * \see
 *   * soc_info_t
 *   * soc_custom_reg_access_t
 */
typedef enum
{
    FIRST_SOC_PBMP_E = 0,
    /**
     * bitmap of ports where roe compression is enabled
     */
    ROE_COMPRESSION = FIRST_SOC_PBMP_E,
    /**
     * LMD port bitmap
     */
    LMD_PBM,
    /**
     * spi bitmap
     */
    SPI_BITMAP,
    /**
     * 2.5 Gpbs Comboserdes ports
     */
    S_PBM,
    /**
     * GMII port bitmap
     */
    GMII_PBM,
    /**
     * HiGig2 encap port bitmap
     */
    HG2_PBM,
    /**
     * Internal loopback ports
     */
    LB_PBM,
    /**
     * Ports internal to the MMU
     */
    MMU_PBM,
    /**
     * Extended queuing ports
     */
    EQ_PBM,
    /**
     * Management ports
     */
    MANAGEMENT_PBM,
    /**
     * Oversubscription ports
     */
    OVERSUB_PBM,
    /**
     * Pors with buffer in exteral DRAM
     */
    PBM_EXT_MEM,
    /**
     * Ports in X-pipe
     */
    XPIPE_PBM,
      /**
     * Ports in Y-pipe
     */
    YPIPE_PBM,
    /**
     * Ports in each pipe
     */
    PIPE_PBM,
    /**
     * TDM port bitmap
     */
    TDM_PBM,
    /**
     * recycling port bitmap
     */
    RCY_PBM,
    /**
     * physical port bitmap
     */
    PHYSICAL_PBM,
    /**
     * LinkPHY enabled port bitmap
     */
    LINKPHY_PBM,
     /**
     * 3rd Vlan SubTag enabled port bitmap
     */
    SUBTAG_PBM,
    /**
     * port bitmap on which 3rd Vlan SubTag is allowed at runtime
     */
    SUBTAG_ALLOWED_PBM,
    /**
     * Port bitmap on which LinkPHY enable/disable is allowed at runtime
     */
    LINKPHY_ALLOWED_PBM,
    /**
     * p_port bitmap reserved for LinkPHY
     */
    LINKPHY_PP_PORT_PBM,
    /**
     * Port bitmap reserved for LinkPHY, which are also enabled
     */
    ENABLED_LINKPHY_PP_PORT_PBM,
    /**
     * pp_port bitmap reserved for SubTag
     */
    SUBTAG_PP_PORT_PBM,
    /**
     * Port bitmap reserved for SubTag, which are also enabled
     */
    ENABLED_SUBTAG_PP_PORT_PBM,
    /**
     * p_port_bitmap to be used for general purpose
     */
    GENERAL_PP_PORT_PBM,
    /**
     * Port_bitmap reserved for general purpose, which are also enabled
     */
    ENABLED_GENERAL_PP_PORT_PBM,
    /**
     * See soc_info_t->soc_custom_reg_access_t->custom_port_pbmp
     */
    CUSTOM_PORT_PBMP,
    /**
     * Mask of ILKN ports.
     */
    PBMP_IL_ALL_BITMAP,
    /**
     * Mask of XLAUI ports.
     */
    PBMP_XL_ALL_BITMAP,
    /**
     * Mask of 100gig ports.
     */
    PBMP_CE_ALL_BITMAP,
    /**
     * Mask of 10gig ports.
     */
    PBMP_XE_ALL_BITMAP,
    /**
     * Mask of Higig ports.
     */
    PBMP_HG_ALL_BITMAP,
    /**
     * Mask of SFI ports.
     */
    PBMP_SFI_ALL_BITMAP,
    /**
     * Mask of all front panel ports.
     */
    PBMP_PORT_ALL_BITMAP,
    /**
     * Mask of CPU ports.
     */
    PBMP_CMIC_BITMAP,
    /**
     * Mask of RECYCLE ports.
     */
    PBMP_RCY_ALL_BITMAP,
    /**
     * Mask of eth ports.
     */
    PBMP_E_ALL_BITMAP,
    /**
     * fe|ge|xe|hg|cmic|fl
     */
    PBMP_ALL_BITMAP,
    /**
     * 200g and 400g port
     */
    PBMP_CDE_BITMAP,
    /**
     * 50g port
     */
    PBMP_LE_BITMAP,
    /**
     * DNX 200g port
     */
    PBMP_CC_BITMAP,
    /**
     * FlexE busA client
     */
    PBMP_FLEXE_CLIENT_BITMAP,
    /**
     * FlexE MAC client
     */
    PBMP_FLEXE_MAC_BITMAP,
    /**
     * FlexE SAR client
     */
    PBMP_FLEXE_SAR_BITMAP,
    /**
     * FlexE PHY client
     */
    PBMP_FLEXE_PHY_BITMAP,

    BLOCK_BITMAP,
    /*
     * Last
     */
    NUM_SOC_PBMP_E
} soc_pbmp_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific 'uint8' variable within
 *   'soc_info_t'.
 * \remark
 *   * Note that 'soc_info_t' is used when invoking the macro SOC_INFO().
 * \see
 *   * soc_info_t
 */
typedef enum
{
    FIRST_UINT8_IN_SOC_INFO_E = 0,
    /**
     * Array of SOC_MAX_NUM_PORTS elements (uint8s).
     */
    PORT_NAME_ALTER_VALID = FIRST_UINT8_IN_SOC_INFO_E,
    /**
     * Array of SOC_MAX_NUM_BLKS elements (int8s).
     * Indicates whether block is used.
     */
    BLOCK_VALID,
    NUM_UINT8_IN_SOC_INFO_E
} uint8_in_soc_info_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific two dimensional 'char'
 *   variable array within 'soc_info_t'.
 * \remark
 *   * Note that 'soc_info_t' is used when invoking the macro SOC_INFO().
 * \see
 *   * soc_info_t
 */
typedef enum
{
    FIRST_CHAR2D_IN_SOC_INFO_E = 0,
    /**
     * Array of [SOC_MAX_NUM_PORTS][SOC_DRV_PORT_NAME_MAX] elements (chars).
     */
    PORT_NAME = FIRST_CHAR2D_IN_SOC_INFO_E,
    PORT_NAME_ALTER,
    /**
     * Array of [SOC_MAX_NUM_BLKS][16] elements (chars).
     */
    BLOCK_NAME,
    NUM_CHAR2D_IN_SOC_INFO_E
} char2d_in_soc_info_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific 'int' variable within
 *   'soc_block_info_t'.
 * \remark
 *   * Note that 'soc_block_info_t' is used when invoking the macro SOC_BLOCK_INFO().
 * \see
 *   * soc_block_info_t
 */
typedef enum
{
    FIRST_SOC_BLOCK_INFO_E = 0,
    /**
     * Block type: SOC_BLK_*
     */
    TYPE = FIRST_SOC_BLOCK_INFO_E,
    /**
     * Block instance number of block type.
     */
    NUMBER,
    /**
     * Pic number for schan commands.
     */
    SCHAN,
    /**
     * Pic number for cmic r/w commands.
     */
    CMIC,
    NUM_SOC_BLOCK_INFO_E
} soc_block_info_e;

/**
 * \brief
 *   Enumeration used for the selection of a specific 'int' variable within
 *   'soc_info_t'.
 * \remark
 *   * Note that 'soc_info_t' is used when invoking the macro SOC_INFO().
 * \see
 *   * soc_info_t
 */
typedef enum
{
    FIRST_INT_IN_SOC_INFO_E = 0,
    /**
     * Array of SOC_MAX_NUM_BLKS elements (ints).
     */
    BLOCK_PORT_INT = FIRST_INT_IN_SOC_INFO_E,
    /**
     * Array of SOC_MAX_NUM_BLKS elements (ints).
     */
    PORT_TYPE_INT,
    /**
     * Logic to phy
     * Array of SOC_MAX_NUM_BLKS elements (ints).
     */
    PORT_L2P_MAPPING_INT,
    /**
     * Number of lanes per port.
     * Array of SOC_MAX_NUM_BLKS elements (ints).
     */
    PORT_NUM_LANES_INT,
    /**
     * Phy to logic
     * Array of SOC_MAX_NUM_BLKS elements (ints).
     */
    PORT_P2L_MAPPING_INT,
    /**
     * First fabric port
     */
    FABRIC_LOGICAL_PORT_BASE_INT,
    /**
     * Number of time sync interfaces
     */
    NUM_TIME_INTERFACE_INT,
    /**
     * number of uCs
     */
    NUM_UCS_INT,
    HBC_BLOCK_INT,
    FSRD_BLOCK_INT,
    FMAC_BLOCK_INT,
    BRDC_HBC_BLOCK_INT,
    /** add for Q2A gddr6 controller */
    DCC_BLOCK_INT,
    /** add flexe blocks */
    FASIC_BLOCK_INT,
    FLEXEWP_BLOCK_INT,
    FSAR_BLOCK_INT,
    FSCL_BLOCK_INT,
    HAS_BLOCK_INT,
    NUM_INT_IN_SOC_INFO_E
} int_in_soc_info_e;

/**
 * \brief
 *   Procedure used to hide MACRO soc_mem_force_read_through_set()
 */
void dnx_drv_soc_mem_force_read_through_set(
    int unit,
    int enable);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_MEM_FORCE_READ_THROUGH()
 */
uint8 dnx_drv_soc_mem_force_read_through(
    int unit);
/**
 * \brief
 *   Procedure used to hide procedure soc_dev_name()
 */
const char *dnx_drv_soc_dev_name(
    int unit);
/**
 * \brief
 *   Procedure used to hide expression SOC_CONTROL(unit)->interrupts_info->interrupt_db_info
 */
void *dnx_drv_soc_interrupt_db_ptr(
    int unit);
/**
 * \brief
 *   Procedure used to hide MACRO MAX_NUM_OF_CORES
 */
int dnx_drv_max_num_of_cores(
    void);
/**
 * \brief
 *   Procedure used to hide PROCEDURE SOC_BLOCK_IN_LIST()
 */
int dnx_drv_soc_block_in_list(
    int unit,
    int *blk,
    int type);
/**
 * \brief
 *   Procedure used to hide PROCEDURE soc_is_valid_block_instance()
 */
shr_error_e dnx_drv_soc_is_valid_block_instance(
    int unit,
    soc_block_types_t block_types,
    int block_instance,
    int *is_valid);
/**
 * \brief
 *   Procedure used to hide PROCEDURE SOC_BLOCK_IS_TYPE()
 */
int dnx_drv_soc_block_is_type(
    int unit,
    int blk_idx,
    int *list);
/**
 * \brief
 *   Procedure used to hide PROCEDURE soc_event_register()
 */
shr_error_e dnx_drv_soc_event_register(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata);
/**
 * \brief
 *   Procedure used to hide MACRO soc_reg_block_in_list()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_block_in_list(
    int unit,
    int reg,
    int port_type);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_NUMELS()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_numels(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_FIRST_ARRAY_INDEX()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_first_array_index(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_ARRAY()
 */
int dnx_drv_soc_reg_array(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_ARRAY()
 */
int dnx_drv_soc_reg_array2(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_RST_VAL_GET()
 */
shr_error_e dnx_drv_soc_reg_rst_val_get(
    int unit,
    int reg,
    uint64 *rval64_p);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_RST_MSK_GET()
 */
shr_error_e dnx_drv_soc_reg_rst_msk_get(
    int unit,
    int reg,
    uint64 *rmsk64_p);
/**
 * \brief
 *   Procedure used to get pointer to 'reg_info' array element
 *   in SOC_DRIVER(unit).
 *   Return a pointer to structure of type 'soc_reg_info_t'.
 */
void *dnx_drv_reg_info_ptr_get(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide PROCEDURE soc_event_unregister()
 */
shr_error_e dnx_drv_soc_event_unregister(
    int unit,
    bcm_switch_event_cb_t cb,
    void *userdata);
/**
 * \brief
 *   Procedure used to hide PROCEDURE soc_dnxc_avs_value_get()
 */
shr_error_e dnx_drv_soc_dnxc_avs_value_get(
    int unit,
    uint32 *rov);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_CONTROL_UNLOCK()
 */
void dnx_drv_soc_control_unlock(
    int unit);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_CONTROL_LOCK()
 */
void dnx_drv_soc_control_lock(
    int unit);
/**
 * \brief
 *   Procedure used to hide MACRO IS_E_PORT()
 */
int dnx_drv_is_e_port(
    int unit,
    bcm_port_t port);
/**
 * \brief
 *   Procedure used to hide MACRO IS_HG_PORT()
 */
int dnx_drv_is_hg_port(
    int unit,
    bcm_port_t port);
/**
 * \brief
 *   Procedure used to hide MACRO IS_SPI_SUBPORT_PORT()
 */
int dnx_drv_is_spi_subport_port(
    int unit,
    bcm_port_t port);
/**
 * \brief
 *   Procedure used to hide MACRO _SOC_CONTROLLED_COUNTER_USE()
 */
int dnx_drv_soc_controlled_counter_use(
    int unit,
    bcm_port_t port);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_DETACH()
 */
void dnx_drv_soc_detach(
    int unit,
    int do_detach);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_PROPERTY_NAME_MAX
 */
int dnx_drv_soc_property_name_max(
    void);
/**
 * \brief
 *   Procedure used to hide MACRO IHP_BLOCK()
 */
int dnx_drv_ihp_block(
    int unit,
    int instance);
/**
 * \brief
 *   Procedure used to hide MACRO ILE_BLOCK()
 */
int dnx_drv_ile_block(
    int unit,
    int instance);
/**
 * \brief
 *   Procedure used to hide procedure soc_block_name_lookup_ext()
 */
char *dnx_drv_soc_block_name_lookup_ext(
    soc_block_t has_block,
    int unit);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_IS_WRITE_ONLY()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_is_write_only(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_IS_ABOVE_32()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_is_above_32(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_IS_ABOVE_64()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_is_above_64(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO soc_reg_is_array()
 */
int dnx_drv_soc_reg_is_array(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_IS_64()
 *   Note that register is handled as if it were of type 'int'
 *   (and not soc_reg_t) to avoid inclusion of a large H file
 *   (allenum.h)
 */
int dnx_drv_soc_reg_is_64(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_ABOVE_64_INFO()
 *   But it returns a pointer to 'soc_reg_above_64_info_t'
 *   rather than the structure itself.
 */
void *dnx_drv_soc_reg_above_64_info_p(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO _SOC_REG_ABOVE_64_IS_ZERO(fldval)
 */
int dnx_drv_soc_reg_above_64_is_zero(
    soc_reg_above_64_val_t fldval);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_REG_ABOVE_64_INFO(unit, reg).size
 */
int dnx_drv_soc_reg_words(
    int unit,
    soc_reg_t reg);

/**
 * \brief
 *   Procedure used to hide MACRO IS_SFI_PORT()
 */
int dnx_drv_is_sfi_port(
    int unit,
    bcm_port_t port);
/**
 * \brief
 *   Procedure used to hide MACRO IS_IL_PORT()
 */
int dnx_drv_is_il_port(
    int unit,
    bcm_port_t port);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_WARM_BOOT()
 */
int dnx_drv_soc_warm_boot(
    int unit);
/**
 * \brief
 *   Procedure used to hide MACRO CLPORT_BLOCK()
 */
int dnx_drv_clport_block(
    int unit,
    int instance);
/**
 * \brief
 *   Procedure used to hide MACRO CDPORT_BLOCK()
 */
int dnx_drv_cdport_block(
    int unit,
    int instance);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_CORE_DEFAULT
 */
int dnx_drv_soc_core_default(
    void);
/**
 * \brief
 *   Procedure used to get address of SOC_INFO element:
 *     Structure named 'custom_reg_access' of type 'soc_custom_reg_access_t *'.
 */
soc_custom_reg_access_t *dnx_drv_soc_info_custom_reg_access_get(
    int unit);
/*
 * \brief -
 *   Procedure implemented to replace direct access to 'int' elements on the global
 *   'soc_control[unit]->info' (structure of type 'soc_info_t')
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] int_id -
 *   int_in_soc_info_e. Identifier of the element to access (on soc_info_t).
 * \param [in,out] int_p_p -
 *   Pointer to int pointer.
 *   As input:
 *     Caller provides a pointer to a pointer to int.
 *   As output
 *     This procedure loads pointed memory (which is a pointer to int) by the address
 *     of the required element.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * SOC_INFO(unit)
 *   * soc_control_t
 *   * soc_info_t
 */
shr_error_e soc_info_int_address_get(
    int unit,
    int_in_soc_info_e int_id,
    int **int_p_p);
/*
 * \brief -
 *   Procedure implemented to  get no. of elements on 'int' array on the global
 *   'soc_control[unit]->info' (structure of type 'soc_info_t')
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] int_id -
 *   int_in_soc_info_e. Identifier of the element to access (on soc_info_t).
 * \param [out] count_p -
 *   Pointer to int pointer.
 *   As input:
 *     Caller provides a pointer to location to store no. of elements.
 *   As output
 *     This procedure loads pointed memory (which is int) by the number
 *     of elements on indicated element. If indicated element is not an array,
 *     a value of '1' is returned.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * SOC_INFO(unit)
 *   * soc_control_t
 *   * soc_info_t
 */
shr_error_e soc_info_int_count_get(
    int unit,
    int_in_soc_info_e int_id,
    int *count_p);
/*
 * \brief -
 *   Procedure implemented to replace direct access to 'pbmp_t' elements on the global
 *   'soc_control[unit]->info' (structure of type 'soc_info_t' and its derivatives)
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] int_id -
 *   int_in_soc_info_e. Identifier of the element to access (on soc_info_t).
 * \param [in,out] pbmp_p_p -
 *   Pointer to pbmp_t pointer.
 *   As input:
 *     Caller provides a pointer to a pointer to pbmp_t.
 *   As output
 *     This procedure loads pointed memory (which is a pointer to pbmp_t) by the address
 *     of the required element.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * SOC_INFO(unit)
 *   * soc_control_t
 *   * soc_info_t
 */
shr_error_e soc_info_pbmp_address_get(
    int unit,
    soc_pbmp_e pbmp,
    pbmp_t ** pbmp_p_p);

/**
 * \brief -
 *   Procedure used to replace the MACRO SOC_PORT_MIN
 */
shr_error_e dnx_drv_soc_port_min(
    int unit,
    soc_ptype_e ptype,
    int *port_min);
/**
 * \brief -
 *   Procedure used to replace the MACRO SOC_PORT_MAX
 */
shr_error_e dnx_drv_soc_port_max(
    int unit,
    soc_ptype_e ptype,
    int *port_max);

/*
 * \brief
 *   Procedure to replace 'DNX_ALGO_PORT_SOC_INFO_PORT_ADD' macro:
 *     macro used to add port to soc info port bitmaps
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] ptype -
 *   soc_ptype_e. See 'ptype' on SOC_INFO list of port types
 * \param [in] logical_port -
 *   bcm_port_t. Logical port to add to bitmap.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * 'DNX_ALGO_PORT_SOC_INFO_PORT_ADD' macro has been removed and this
 *     procedure comes in its place
 * \see
 *   * SOC_INFO(unit)
 */
shr_error_e dnx_algo_port_soc_info_port_add(
    int unit,
    soc_ptype_e ptype,
    bcm_port_t logical_port);
/*
 * \brief
 *   Procedure to replace 'DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE' macro:
 *     macro used to remove port from soc info port bitmaps
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] ptype -
 *   soc_ptype_e. See 'ptype' on SOC_INFO list of port types
 * \param [in] logical_port -
 *   bcm_port_t. Logical port to remove from bitmap.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * 'DNX_ALGO_PORT_SOC_INFO_PORT_REMOVE' macro has been removed and this
 *     procedure comes in its place
 * \see
 *   * SOC_INFO(unit)
 */
shr_error_e dnx_algo_port_soc_info_port_remove(
    int unit,
    soc_ptype_e ptype,
    bcm_port_t logical_port);
/*
 * \brief
 *   Procedure to replace access via macro:
 *     SOC_PORT_BLOCK(unit, port)
 * \return
 *   The value as returned by SOC_PORT_BLOCK(unit, port).
 * \remark
 *   * All IO parameters are as per specified MACRO. See there for description
 * \see
 *   * SOC_PORT_BLOCK
 */
soc_block_t dnx_drv_soc_port_block(
    int unit,
    uint32 phy_port);
/*
 * \brief
 *   Procedure to replace access via macro:
 *     SOC_BLOCK_PORT(unit, block)
 * \return
 *   The value as returned by SOC_BLOCK_PORT(unit, block).
 * \remark
 *   * All IO parameters are as per specified MACRO. See there for description
 * \see
 *   * SOC_BLOCK_PORT
 */
int dnx_drv_soc_block_port(
    int unit,
    int block);
/*
 * \brief
 *   Procedure to replace access via macro:
 *     SOC_PORT_IDX_INFO(unit, port, idx).blk
 * \return
 *   The value as returned by SOC_PORT_IDX_INFO(unit, port, idx).blk.
 * \remark
 *   * All IO parameters are as per specified MACRO. See there for description
 * \see
 *   * SOC_PORT_IDX_INFO
 */
int dnx_drv_soc_port_idx_info_blk(
    int unit,
    int port,
    int idx);
/**
 * \brief
 *   Procedure used to hide access to the various elements on SOC_BLOCK_INFO(unit, port_block)
 *   where 'port_block' is the number of the block (identifier) and 'soc_block_info' is
 *   the identifier of the element.
 *   The return value is a pointer to a pointer to the selected element on a struct of
 *   type 'soc_block_info_t'.
 *   See dnx_drv_soc_block_info_block_address_get().
 */
shr_error_e dnx_drv_soc_block_info_port_block_address_get(
    int unit,
    soc_block_info_e soc_block_info,
    int port_block,
    int **soc_block_info_p_p);
/**
 * \brief
 *   Procedure used to hide access to SOC_BLOCK_INFO(unit, port_block)
 *   where 'port_block' is the number of the block (identifier) and the
 *   return value is a pointer to a pointer to a struct of type 'soc_block_info_t'.
 *   See soc_block_info_port_block_address_get().
 */
shr_error_e dnx_drv_soc_block_info_block_address_get(
    int unit,
    int port_block,
    void **soc_block_info_p_p);
/**
 * \brief
 *   Procedure used to hide access to two dimensional array of type char/uint8
 *   on SOC_INFO(unit).
 *   The memory pointed by 'sizeof_second_dimension' is loaded, by this procedure,
 *   by the dimension of the second index of the two dimensonal array.
 *   If 'sizeof_second_dimension' is NULL (as input) then this procedure
 *   ignores it.
 */
shr_error_e soc_info_char_2dimensions_address_size_get(
    int unit,
    char2d_in_soc_info_e char_2d_id,
    int first_index,
    char **char_p_p,
    int *sizeof_second_dimension);
/**
 * \brief
 *   Procedure used to hide access to variables of type char/uint8
 *   on SOC_INFO(unit).
 */
shr_error_e soc_info_uint8_address_get(
    int unit,
    uint8_in_soc_info_e uint8_id,
    uint8 **uint8_p_p);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_get()
 */
uint32 dnx_drv_soc_property_get(
    int unit,
    const char *name,
    uint32 defl);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_suffix_num_get()
 */
uint32 dnx_drv_soc_property_suffix_num_get(
    int unit,
    int num,
    const char *name,
    const char *suffix,
    uint32 defl);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_suffix_num_str_get()
 */
char *dnx_drv_soc_property_suffix_num_str_get(
    int unit,
    int num,
    const char *name,
    const char *suffix);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_get_str()
 */
char *dnx_drv_soc_property_get_str(
    int unit,
    const char *name);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_port_get()
 */
uint32 dnx_drv_soc_property_port_get(
    int unit,
    soc_port_t port,
    const char *name,
    uint32 defl);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_suffix_num_get_only_suffix()
 */
uint32 dnx_drv_soc_property_suffix_num_get_only_suffix(
    int unit,
    int num,
    const char *name,
    const char *suffix,
    uint32 defl);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_suffix_num_only_suffix_str_get()
 */
char *dnx_drv_soc_property_suffix_num_only_suffix_str_get(
    int unit,
    int num,
    const char *name,
    const char *suffix);
/**
 * \brief
 *   Procedure used to hide procedure soc_property_port_get_str()
 */
char *dnx_drv_soc_property_port_get_str(
    int unit,
    soc_port_t port,
    const char *name);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_IS_VALID()
 */
int dnx_drv_soc_reg_is_valid(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_IS_COUNTER()
 */
int dnx_drv_soc_reg_is_counter(
    int unit,
    int reg);
/*
 * \brief -
 *   Procedure implemented to replace the macro TIME_INTERFACE_ID_MAX
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \return
 *   The value as returned by TIME_INTERFACE_ID_MAX ( NUM_TIME_INTERFACE(unit) - 1).
 * \remark
 *   * None
 * \see
 *   * TIME_INTERFACE_ID_MAX
 *   * NUM_TIME_INTERFACE
 */
int time_interface_id_max(
    int unit);

/*
 * \brief -
 *   Procedure implemented to replace direct access to global variables.
 *   This procedure replaces the various SOC_IS_* macros.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] soc_device -
 *   soc_is_device_e. Identifier of the device to probe. If device is
 *   as specified, return 'TRUE'
 * \return
 *   If TRUE then current device is as specified.
 *   Otherwise, it is some other device.
 * \remark
 *   * If value specified in 'soc_device' is unknown then FALSE is
 *     returned.
 * \see
 *   * SOC_IS_JERICHO
 *   * SOC_INFO(unit).chip_type
 */
int soc_is(
    int unit,
    soc_is_device_e soc_device);

/*
 * \brief -
 *   Procedure implemented to access elements which are directly on the global
 *   structure 'soc_control' (See SOC_CONTROL(unit). Since there are many types
 *   of elements on that structure, this procedure returns a 'void *' and it is
 *   up to the caller to set the correct type.
 * \param [in] unit -
 *   HW Identifier of the device to access.
 * \param [in] soc_control_element -
 *   soc_control_e. Identifier of the specific element to get address of (on 'soc_control_t',
 *   within  soc_control[DEVICE ID].
 * \param [out] soc_control_element_p_p -
 *   void **. This procedure loads pointed memory by a pointer to specified element
 *   (within  soc_control[DEVICE ID]). The returned type is 'void *' and it is up to the caller
 *   to apply the correct type.
 * \return
 *   If zero (_SHR_E_NONE), then no error was encountered.
 *   Otherwise, see shr_error_e
 * \remark
 *   * None
 * \see
 *   * soc_info_t
 */
shr_error_e soc_control_element_address_get(
    int unit,
    soc_control_e soc_control_element,
    void **soc_control_element_p_p);
/**
 * \brief
 *   Procedure used to hide MACRO CMIC_SCHAN_WORDS()
 */
int dnx_drv_cmic_schan_words(
    int unit);
/**
 * \brief
 *   Procedure used to hide procedure soc_feature()
 */
int dnx_drv_soc_feature(
    int unit,
    soc_feature_t soc_feature);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_FEATURE_GET()
 */
uint32 dnx_drv_soc_feature_get(
    int unit,
    soc_feature_t soc_feature);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_FEATURE_CLEAR()
 */
void dnx_drv_soc_feature_clear(
    int unit,
    soc_feature_t soc_feature);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_FEATURE_SET()
 */
void dnx_drv_soc_feature_set(
    int unit,
    soc_feature_t soc_feature);
/**
 * \brief
 *   Procedure used to hide soc_intr_enable()
 */
uint32 dnx_drv_soc_intr_enable(
    int unit,
    uint32 mask);

/**
 * \brief
 *   Procedure used to hide soc_intr_disable()
 */
uint32 dnx_drv_soc_intr_disable(
    int unit,
    uint32 mask);

/**
 * \brief
 *   Procedure used to hide the macro SOC_PORT_VALID(unit,port)
 */
int dnx_drv_soc_port_valid(
    int unit,
    bcm_gport_t gport);

/**
 * \brief
 *   Procedure used to hide the macro SOC_MEM_BLOCK_MIN(unit,mem)
 */
int dnx_drv_soc_mem_block_min(
    int unit,
    soc_mem_t mem);

/**
 * \brief
 *   Procedure used to hide the macro SOC_MEM_BLOCK_MAX(unit,mem)
 */
int dnx_drv_soc_mem_block_max(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_BLOCK_ANY(unit, mem)
 */
int dnx_drv_soc_mem_block_any(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_BLOCK_VALID(unit, mem)
 */
int dnx_drv_soc_mem_block_valid(
    int unit,
    soc_mem_t mem,
    int blk);
/**
 * \brief
 *   Procedure used to hide the MACRO MEM_UNLOCK(unit, mem)
 */
void dnx_drv_mem_unlock(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO MEM_LOCK(unit, mem)
 */
void dnx_drv_mem_lock(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the global &SOC_MEM_INFO(unit, mem)
 */
soc_mem_info_t *dnx_drv_soc_mem_info_ptr(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the global SOC_MEM_INFO(unit, mem).blocks
 */
uint32 dnx_drv_soc_mem_info_blocks(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the global SOC_MEM_INFO(unit, mem).blocks_hi
 */
uint32 dnx_drv_soc_mem_info_blocks_hi(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_index_min(unit,mem)
 */
int dnx_drv_soc_mem_index_min(
    int unit,
    soc_mem_t mem);

/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_index_max(unit,mem)
 */
int dnx_drv_soc_mem_index_max(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO COPYNO_ALL
 */
int dnx_drv_copyno_all(
    void);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_IS_ARRAY(unit,mem)
 */
int dnx_drv_soc_mem_is_array(
    int unit,
    soc_mem_t mem);

/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_NUMELS(unit,mem)
 */
int dnx_drv_soc_mem_numels(
    int unit,
    soc_mem_t mem);

/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_FIRST_ARRAY_INDEX(unit,mem)
 */
int dnx_drv_soc_mem_first_array_index(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_ABOVE_64_RST_VAL_GET(unit,reg,rval)
 */
void dnx_drv_soc_reg_above_64_rst_val_get(
    int unit,
    int reg,
    soc_reg_above_64_val_t rval);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_REG_ABOVE_64_RST_MSK_GET(unit,reg,rmsk)
 */
void dnx_drv_soc_reg_above_64_rst_msk_get(
    int unit,
    int reg,
    soc_reg_above_64_val_t rmsk);
/**
 * \brief
 *   Procedure used to hide MACRO SOC_UNIT_NAME(unit)
 */
char *dnx_drv_soc_unit_name(
    int unit);
/**
 * \brief
 *   Procedure used to hide procedure soc_attached(unit)
 */
int dnx_drv_soc_attached(
    int unit);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_ndev_idx2dev(dev_idx)
 */
int dnx_drv_soc_ndev_idx2dev(
    int dev_idx);
/**
 * \brief
 *   Procedure used to get value of the global 'ndev'
 */
int dnx_drv_soc_ndev_get(
    void);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_entry_words(unit,mem)
 */
int dnx_drv_soc_mem_entry_words(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_writeonly(unit,mem)
 */
int dnx_drv_soc_mem_is_writeonly(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_readonly(unit,mem)
 */
int dnx_drv_soc_mem_is_readonly(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_debug(unit,mem)
 */
int dnx_drv_soc_mem_is_debug(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_sorted(unit,mem)
 */
int dnx_drv_soc_mem_is_sorted(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_hashed(unit,mem)
 */
int dnx_drv_soc_mem_is_hashed(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_cam(unit,mem)
 */
int dnx_drv_soc_mem_is_cam(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_cbp(unit,mem)
 */
int dnx_drv_soc_mem_is_cbp(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_bistepic(unit,mem)
 */
int dnx_drv_soc_mem_is_bistepic(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_bistffp(unit,mem)
 */
int dnx_drv_soc_mem_is_bistffp(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_bistcbp(unit,mem)
 */
int dnx_drv_soc_mem_is_bistcbp(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_cachable(unit,mem)
 */
int dnx_drv_soc_mem_is_cachable(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_index_count(unit,mem)
 */
int dnx_drv_soc_mem_index_count(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_signal(unit,mem)
 */
int dnx_drv_soc_mem_is_signal(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_IS_VALID(unit,mem)
 */
int dnx_drv_soc_mem_is_valid(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_is_valid(unit,mem)
 *   (Small letters)
 */
int dnx_drv_a_soc_mem_is_valid(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_UNIT_VALID(unit)
 */
int dnx_drv_soc_unit_valid(
    int unit);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_PCI_VENDOR(unit)
 */
uint16 dnx_drv_soc_pci_vendor(
    int unit);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_PCI_DEVICE(unit)
 */
uint16 dnx_drv_soc_pci_device(
    int unit);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_PCI_REVISION(unit)
 */
uint8 dnx_drv_soc_pci_revision(
    int unit);
/**
 * \brief
 *   Procedure used to hide the MACRO soc_mem_flags(unit,mem)
 */
int dnx_drv_soc_mem_flags(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_F_ALL_MODULES_INITED
 */
int dnx_drv_soc_f_all_modules_inited(
    void);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_F_INITED
 */
int dnx_drv_soc_f_inited(
    void);
/**
 * \brief
 *   Procedure used to hide the MACRO MEM_BLOCK_ALL
 */
int dnx_drv_mem_block_all(
    void);
/**
 * \brief
 *   Procedure used to hide the MACRO MEM_BLOCK_ANY
 */
int dnx_drv_mem_block_any(
    void);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_ARRAY_INFOP(unit,mem)
 */
soc_mem_array_info_t *dnx_drv_soc_mem_array_infop(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_REG_ARRAY_INFOP(unit,reg)
 */
soc_reg_array_info_t *dnx_drv_soc_reg_array_info(
    int unit,
    int reg);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_WORDS(unit,mem)
 */
int dnx_drv_soc_mem_words(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_BYTES(unit,mem)
 */
int dnx_drv_soc_mem_bytes(
    int unit,
    soc_mem_t mem);
/**
 * \brief
 *   Procedure used to hide the MACRO SOC_MEM_TABLE_BYTES(unit,mem)
 */
int dnx_drv_soc_mem_table_bytes(
    int unit,
    soc_mem_t mem);
/*
 * The following procedures are for DNX only (and not for DNXF and, hence, not for dnxc)
 */
#ifdef BCM_DNX_SUPPORT
/* { */
/**
 * \brief
 *   Procedure used to hide procedure soc_dnx_hard_reset()
 */
shr_error_e dnx_drv_soc_dnx_hard_reset(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_rcpu_init(unit)
 */
shr_error_e dnx_drv_soc_dnx_rcpu_init(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_info_config(unit)
 */
int dnx_drv_soc_dnx_info_config(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_info_config_deinit(unit)
 */
int dnx_drv_soc_dnx_info_config_deinit(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_schan_config_and_init(unit)
 *   (Function that is intilizing the SCHAN)
 */
shr_error_e dnx_drv_soc_dnx_schan_config_and_init(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_endian_config(unit)
 *   (Configuration of the endianness)
 */
shr_error_e dnx_drv_soc_dnx_endian_config(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_polling_init(unit)
 *   (Polling initialization)
 */
shr_error_e dnx_drv_soc_dnx_polling_init(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_ring_config(unit)
 *   (Rings s-bus and broadcast blocks configuration)
 */
shr_error_e dnx_drv_soc_dnx_ring_config(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_init_reset_cmic_regs(unit)
 */
int dnx_drv_soc_dnx_init_reset_cmic_regs(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_iproc_config(unit)
 *   (Iproc/PAXB configuration not configured earlier)
 */
shr_error_e dnx_drv_soc_dnx_iproc_config(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_soft_reset(unit)
 *   (Configuration of the Soft reset)
 */
shr_error_e dnx_drv_soc_dnx_soft_reset(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_dma_init(unit)
 *   (DMA init)
 */
shr_error_e dnx_drv_soc_dnx_dma_init(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_dma_deinit(unit)
 *   (DMA deinit)
 */
shr_error_e dnx_drv_soc_dnx_dma_deinit(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_mutexes_init(unit)
 *   (allocates needed Mutexes for device)
 */
shr_error_e dnx_drv_soc_dnx_mutexes_init(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_mutexes_deinit(unit)
 *   (free Devices Mutexes)
 */
shr_error_e dnx_drv_soc_dnx_mutexes_deinit(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_sbus_polled_interrupts(unit)
 *   (Use polling for s-bus access)
 */
shr_error_e dnx_drv_soc_dnx_sbus_polled_interrupts(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_perform_bist_tests(unit)
 *   (Init of the BIST tests. Needs to be run after WB, because of a global variable init inside sand_init_cpu2tap().)
 */
shr_error_e dnx_drv_soc_dnx_perform_bist_tests(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_unmask_mem_writes(unit)
 *   (Access of memory write masks to no masks)
 */
shr_error_e dnx_drv_soc_dnx_unmask_mem_writes(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_drv_sbus_broadcast_config(unit)
 */
int dnx_drv_soc_dnx_drv_sbus_broadcast_config(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_mark_not_inited(unit)
 *   (mark device as not inited)
 */
shr_error_e dnx_drv_soc_dnx_mark_not_inited(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_soft_init(unit)
 *   (Making the Soft Init)
 */
shr_error_e dnx_drv_soc_dnx_soft_init(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_dnx_soft_init(unit)
 *   (Making the Soft Init)
 */
shr_error_e dnx_drv_soc_dnx_soft_init_no_fabric(
    int unit);
/**
 * \brief
 *   Procedure used to hide the PROCEDURE soc_read_jr2_hbm_temp(unit)
 *   (Read temperature from specified HBM into *out_temp)
 */
int dnx_drv_soc_dnx_read_hbm_temp(
    const int unit,
    const unsigned hbm_number,
    uint32 *out_temp);
/**
 * \brief
 *   Procedure used to disable blocks which might no PLL input
 *   those block will be enabled once enable PLL input
 */
shr_error_e dnx_drv_soc_block_enable_set(
    int unit);

/* } */
#endif
/*
 * }
 */
#endif /* */
