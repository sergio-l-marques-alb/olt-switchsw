/* $Id: jer2_arad_mgmt.h,v 1.20 Broadcom SDK $
 * $Copyright: (c) 2018 Broadcom.
 * Broadcom Proprietary and Confidential. All rights reserved.$
*/


#ifndef __JER2_ARAD_MGMT_INCLUDED__
/* { */
#define __JER2_ARAD_MGMT_INCLUDED__

#ifndef BCM_DNX_SUPPORT 
#error "This file is for use by DNX (Jer2) family only!"
#endif

/*************
 * INCLUDES  *
 *************/
/* { */


#include <soc/dnx/legacy/SAND/Utils/sand_framework.h>
#include <soc/dnx/legacy/SAND/Management/sand_general_macros.h>
#include <soc/dnx/legacy/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dnx/legacy/ARAD/arad_general.h>
#include <soc/dnx/legacy/TMC/tmc_api_mgmt.h>
#include <soc/dnx/legacy/ARAD/arad_fabric.h>
#include <soc/dnx/legacy/ARAD/arad_ingress_traffic_mgmt.h>
#include <bcm_int/dnx/cosq/egress/egr_queuing.h>
#include <soc/dnx/legacy/ARAD/arad_flow_control.h>

#include <soc/dnxc/legacy/dnxc_defs.h>
#include <soc/dnxc/legacy/fabric.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

#define  JER2_ARAD_MAX_NOF_REVISIONS JER2_ARAD_REVISION_NOF_IDS

/*     Out of total of 15 SerDes quartets, two (one per
*     internal NIF Group consisting of 4 MAL-s) may be
*     assigned to either Network or Fabric interfaces.        */
#define  JER2_ARAD_MGMT_NOF_SHARED_SERDES_QUARTETS 2

#define JER2_ARAD_MGMT_VER_REG_BASE       0
#define JER2_ARAD_MGMT_CHIP_TYPE_FLD_LSB   4
#define JER2_ARAD_MGMT_CHIP_TYPE_FLD_MSB   23
#define JER2_ARAD_MGMT_DBG_VER_FLD_LSB     24
#define JER2_ARAD_MGMT_DBG_VER_FLD_MSB     27
#define JER2_ARAD_MGMT_CHIP_VER_FLD_LSB    28
#define JER2_ARAD_MGMT_CHIP_VER_FLD_MSB    31

/*
 *  This value disables limitation based on external (original)
 *  packet size
 */
#define JER2_ARAD_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT 0

#define JER2_ARAD_MGMT_FDR_TRFC_DISABLE         0x0
#define JER2_ARAD_MGMT_FDR_TRFC_ENABLE_VAR_CELL_LSB 0x1FB007E8
#define JER2_ARAD_MGMT_FDR_TRFC_ENABLE_VAR_CELL_MSB 0x10

/* OCB range: -1 to disable the range verification */
#define JER2_ARAD_MGMT_OCB_MC_RANGE_DISABLE 0xFFFFFFFF

/*
 *  Packet Size adjustments
 */
/* The offset decremented by the HW before checking packet range */
#define JER2_ARAD_MGMT_PCKT_RNG_HW_OFFSET 1

/* The CRC size assumed by the packet range API (external size).    */
/* Note: for 3-byte CRC (SPAUI), add one to the requested external  */
/* packet size range                                                */
#define JER2_ARAD_MGMT_PCKT_RNG_NIF_CRC_BYTES 4

/* The value to decrement from the requested when */
/* limiting the internal packet size              */
#define JER2_ARAD_MGMT_PCKT_RNG_CORRECTION_INTERNAL \
  (JER2_ARAD_MGMT_PCKT_RNG_HW_OFFSET)


/* The value to decrement from the requested when */
/* limiting the external packet size              */
#define JER2_ARAD_MGMT_PCKT_RNG_CORRECTION_EXTERNAL \
  (JER2_ARAD_MGMT_PCKT_RNG_NIF_CRC_BYTES - JER2_ARAD_MGMT_PCKT_RNG_HW_OFFSET  - JER2_ARAD_MGMT_PCKT_RNG_DRAM_CRC_BYTES)
#define JER2_ARAD_MGMT_PCKT_MAX_SIZE_EXTERNAL_MAX        ((0x3FFF)+JER2_ARAD_MGMT_PCKT_RNG_CORRECTION_EXTERNAL)
#define JER2_ARAD_MGMT_PCKT_MAX_SIZE_INTERNAL_MAX        ((0x3FFF)+JER2_ARAD_MGMT_PCKT_RNG_CORRECTION_INTERNAL)

/*
 *  Packet size limitations
 */
/* Minimal packet size, variable size cells */
#define JER2_ARAD_MGMT_PCKT_SIZE_BYTES_VSC_MIN      64
/* Maximal packet size, variable size cells */
#define JER2_ARAD_MGMT_PCKT_SIZE_BYTES_VSC_MAX      (16*1024 - 128)
/* Minimal packet size, fixed size cells */
#define JER2_ARAD_MGMT_PCKT_SIZE_BYTES_FSC_MIN      33
/* Maximal packet size, fixed size cells */
#define JER2_ARAD_MGMT_PCKT_SIZE_BYTES_FSC_MAX      (16*1024 - 128)

/* Maximal packet size of the packet on the line (before ingress editing) */
#define JER2_ARAD_MGMT_PCKT_SIZE_BYTES_EXTERN_MAX      (JER2_ARAD_MGMT_PCKT_SIZE_BYTES_FSC_MAX    \
                                                   + (JER2_ARAD_MGMT_PCKT_RNG_NIF_CRC_BYTES  \
                                                   - JER2_ARAD_MGMT_PCKT_RNG_DRAM_CRC_BYTES))

#define JER2_ARAD_MGMT_IDR_NUM_OF_CONTEXT            (192)

#define JER2_ARAD_MGMT_MAX_BUFFERS_PER_PACKET        (32)

/*
 * jer2_arad_mgmt_all_ctrl_cells_enable_set flags
 */
#define JER2_ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_NONE           0
#define JER2_ARAD_MGMT_ALL_CTRL_CELLS_FLAGS_SOFT_RESET     0x1

#define JER2_ARAD_PLUS_CREDIT_VALUE_MODES_PER_WORD (sizeof(uint32) * 8)
#define JER2_ARAD_PLUS_CREDIT_VALUE_MODE_WORDS (SOC_DNX_NOF_FAPS_IN_SYSTEM / JER2_ARAD_PLUS_CREDIT_VALUE_MODES_PER_WORD)

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


#define JER2_ARAD_MGMT_FABRIC_HDR_TYPE_JER2_ARAD                SOC_DNX_MGMT_FABRIC_HDR_TYPE_JER2_ARAD
#define JER2_ARAD_MGMT_FABRIC_HDR_TYPE_FAP20                    SOC_DNX_MGMT_FABRIC_HDR_TYPE_FAP20
#define JER2_ARAD_MGMT_FABRIC_HDR_TYPE_FAP10M                   SOC_DNX_MGMT_FABRIC_HDR_TYPE_FAP10M
#define JER2_ARAD_MGMT_NOF_FABRIC_HDR_TYPES                     SOC_DNX_MGMT_NOF_FABRIC_HDR_TYPES
typedef SOC_DNX_MGMT_FABRIC_HDR_TYPE                           JER2_ARAD_MGMT_FABRIC_HDR_TYPE;

#define JER2_ARAD_MGMT_TDM_MODE_PACKET                          SOC_DNX_MGMT_TDM_MODE_PACKET
#define JER2_ARAD_MGMT_TDM_MODE_TDM_OPT                         SOC_DNX_MGMT_TDM_MODE_TDM_OPT
#define JER2_ARAD_MGMT_TDM_MODE_TDM_STA                         SOC_DNX_MGMT_TDM_MODE_TDM_STA
typedef SOC_DNX_MGMT_TDM_MODE                                  JER2_ARAD_MGMT_TDM_MODE;

#define JER2_ARAD_MGMT_NOF_TDM_MODES                            SOC_DNX_MGMT_NOF_TDM_MODES

typedef SOC_DNX_MGMT_PCKT_SIZE                                 JER2_ARAD_MGMT_PCKT_SIZE;

#define JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE_EXTERN               SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE_EXTERN     
#define JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE_INTERN               SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE_INTERN     
#define JER2_ARAD_MGMT_NOF_PCKT_SIZE_CONF_MODES                 SOC_DNX_MGMT_NOF_PCKT_SIZE_CONF_MODES       
typedef SOC_DNX_MGMT_PCKT_SIZE_CONF_MODE                       JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE;

typedef SOC_DNX_MGMT_OCB_VOQ_INFO                              JER2_ARAD_MGMT_OCB_VOQ_INFO;

#ifdef FIXME_DNX_LEGACY /** SOC_DNX_CONFIG not supported */
#endif

typedef enum
{
  /*
   *  Arad device revision: 1
   */
  JER2_ARAD_REVISION_ID_1=0,
  /*
   *  Total number of Arad revisions
   */
  JER2_ARAD_REVISION_NOF_IDS=1
}JER2_ARAD_REVISION_ID;

typedef enum
{
  /*
   * Full Multicast 4K replecations with 64K DBuff mode
   */
  JER2_ARAD_INIT_FMC_4K_REP_64K_DBUFF_MODE = 0,
  /*
   * Full Multicast 64 replecations with 128K DBuff mode
   */
  JER2_ARAD_INIT_FMC_64_REP_128K_DBUFF_MODE = 1,
  /*
   * number of modes
   */
  JER2_ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_NOF_MODES = 2
}JER2_ARAD_INIT_NBR_FULL_MULTICAST_DBUFF_MODES;

#define JER2_ARAD_INIT_PDM_MODE_SIMPLE SOC_DNX_INIT_PDM_MODE_SIMPLE
#define JER2_ARAD_INIT_PDM_MODE_REDUCED SOC_DNX_INIT_PDM_MODE_REDUCED
#define JER2_ARAD_INIT_PDM_NOF_MODES SOC_DNX_INIT_PDM_NOF_MODES
typedef SOC_DNX_INIT_PDM_MODE JER2_ARAD_INIT_PDM_MODE;


typedef struct
{
  
  /*
   *  Pll-M
   */
  uint32 m;
  /*
   *  Pll-N
   */
  uint32 n;
  /*
   *  Pll-P
   */
  uint32 p;

}JER2_ARAD_HW_PLL_PARAMS;


#define JER2_ARAD_INIT_SERDES_REF_CLOCK_125 soc_dnxc_init_serdes_ref_clock_125
#define JER2_ARAD_INIT_SERDES_REF_CLOCK_156_25 soc_dnxc_init_serdes_ref_clock_156_25
#define JER2_ARAD_INIT_SREDES_NOF_REF_CLOCKS soc_dnxc_init_serdes_nof_ref_clocks
#define JER2_ARAD_INIT_SERDES_REF_CLOCK soc_dnxc_init_serdes_ref_clock_t
#define JER2_ARAD_INIT_SERDES_REF_CLOCK_DISABLE soc_dnxc_init_serdes_ref_clock_disable

typedef enum{
  /*
   * 25 Mhz
   */
  JER2_ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_25_MHZ = 0,
  /*
   * 125 Mhz
   */
  JER2_ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY_125_MHZ = 1,
  /*
   *  Number of frequencies in JER2_ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY
   */
  JER2_ARAD_INIT_SYNTHESIZER_NOF_CLOCK_FREQUENCIES = 2
}JER2_ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY;

typedef enum
{
  /* For short range connection - 1.5V : Arad */
  JER2_ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V=0,
  /* For long range connection - 3.3V : Arad */
  JER2_ARAD_MGMT_EXT_VOL_MOD_3p3V,
  /* Short range connection- 1.5V HSTL internal reference VDDO/2 : Arad */
  JER2_ARAD_MGMT_EXT_VOL_MOD_HSTL_1p5V_VDDO,
  /* For short range connection - 1.8V : Jericho/Qmx */
  JER2_ARAD_MGMT_EXT_VOL_MOD_HSTL_1p8V,

  JER2_ARAD_MGMT_EXT_VOL_NOF_MODES
} JER2_ARAD_MGMT_EXT_VOLT_MOD;

typedef struct
{
  
  /*
   * nif clock frequency
   */
  JER2_ARAD_INIT_SERDES_REF_CLOCK nif_clk_freq;
  /*
   * fabric clock frequency
   */
  JER2_ARAD_INIT_SERDES_REF_CLOCK fabric_clk_freq;
  /*
   * external synthesizer clock frequency
   */
  JER2_ARAD_INIT_SYNTHESIZER_CLOCK_FREQUENCY synthesizer_clock_freq;
  /* 
   * IEEE1588 DPLL mode, 0 - eci ts pll clk disabled, 1 - configure eci ts pll clk
   */
  uint32 ts_clk_mode;
  /* 
   * broadsync clock enable, 0 - disable, 1 - enable
   */
  uint32 bs_clk_mode;
  /* 
   * Initial phase values for the IEEE1588 DPLL, lower 32 bits
   */
  uint32 ts_pll_phase_initial_lo;
  /* 
   * Initial phase values for the IEEE1588 DPLL, upper 32 bits
   */
  uint32 ts_pll_phase_initial_hi;

}JER2_ARAD_INIT_PLL; 

typedef struct
{
  
  /*
   *  Enable/disable Clock frequency configuration
   */
  uint8 enable;

  /*
   *  System Reference clock. Units: kHz. 
   */
  uint32 system_ref_clock;

} JER2_ARAD_INIT_CORE_FREQ;

typedef struct
{
  
  /*
   *  PP Port index to configure. Range: 0 - 63.
   */
  uint32 profile_ndx;
  /*
   *  PP Port configuration.
   */
  SOC_DNX_PORT_PP_PORT_INFO conf;

} JER2_ARAD_INIT_PP_PORT;

typedef struct
{
  
  /*
   *  FAP Port index to configure. Range: 0 - 79.
   */
  uint32 port_ndx;
  /*
   *  PP Port for this TM Port.
   */
  uint32 pp_port;

} JER2_ARAD_INIT_PP_PORT_MAP;

typedef struct
{
  
  /*
   *  FAP Port index to configure. Range: 0 - 79.
   */
  uint32 port_ndx;
  /*
   *  Egress queue priority profile index. Range: 0 - 3.
   */
  uint32 conf;
  /* 
   *  Egress port priority (number of q-pairs). Values: 1,2,8
   */
  dnx_egr_port_priority_mode_e priority_mode;
  /* 
   *  Egress base-q pair number. Range: 0-256
   */
  uint32 base_q_pair;
  /* 
   *  Egress port shaper mode
   */
  dnx_egr_port_shaper_mode_e shaper_mode;

} JER2_ARAD_INIT_EGR_Q_PROFILE_MAP;

typedef struct
{
  
  /*
   *  TM Port index to configure. Range: 0 - 255.
   */
  uint32 port_ndx;
  /* 
   *  multicast_offset
   */
  uint32 multicast_offset;

} JER2_ARAD_MULTICAST_ID_OFFSET_MAP;

typedef struct
{
  
  /*
   *  FAP Port index to configure. Range: 0 - 79
   */
  uint32 port_ndx;
  /*
   *  RAW/TM/ETH/Programmable header parsing type in the incoming direction.
   */
  SOC_DNX_PORT_HEADER_TYPE header_type_in;
  /*
   *  header type in the outgoing direction.
   */
  SOC_DNX_PORT_HEADER_TYPE header_type_out;
  /*
   *  If True, then Packets coming from this TM Port have a
   *  first header to strip before any processing. For
   *  example, in the Fat Pipe processing a Sequence Number
   *  header (2 Bytes) must be stripped.
   *  For injected packets, the PTCH Header must be
   *  removed (4 Bytes).
   *  Units: Bytes. Range: 0 - 63.
   *  In Petra-B, it was per PP-Port
   */
  uint32 first_header_size;

}JER2_ARAD_INIT_PORT_HDR_TYPE;

typedef struct
{
  
  /*
   *  FAP Port index to configure. Range: 0 - 255
   */
  uint32 port_ndx;

}JER2_ARAD_INIT_PORT_TO_IF_MAP;

typedef struct
{
  
  /* 
   * Lag Mode - Arad only
   */
  SOC_DNX_PORT_LAG_MODE lag_mode;
  /*
   *  stacking is enabled in the system.
   *    0 - not a stacking system.
   *    1-  stacking system.
   */
  uint8 is_stacking_system;
 /* 
  * use trunk as ingress MC destination. 
  *   0 - don't use trunk, use instead ports in trunk in ingress MC
  *   1 - use trunk as ingress mc destination
  */
  uint8 use_trunk_as_ingress_mc_dest;

  /*
   *  DSP-stacking is enabled in the system.
   *    0 - not enabled.
   *    1-  enabled.
   */
  uint8 add_dsp_extension_enable;
  /*
   *   PPH learn extension is Disabled in the system.
   *    0 - PPH learn extension is not disabled.
   *    1-  never add the PPH learn extension (unless explictly required in FP action).
   */
  uint8 pph_learn_extension_disable;
  /* 
   * use msb of lb-key in stack trunk resolutions
   */
  uint8   stack_resolve_using_msb;
  /* 
   * use msb of lb-key in smooth-division trunk resolutions
   */
  uint8   smooth_division_resolve_using_msb;
  /*
   *  System RED is enabled in the system.
   *    0 - Disabled.
   *    1 - Enabled.
   */
  uint8 is_system_red;
  /*
   *  The TM Domin of current device.
   */
  uint8 tm_domain;
  /*
   * Enable OAMP port
   */
  uint8 oamp_port_enable;
  /*
   *  Device swap mode for incoming packets
   */
  JER2_ARAD_SWAP_INFO    swap_info;

} JER2_ARAD_INIT_PORTS;


typedef enum
{
  /*
   * 80% unicast, 20 % multicat
   */
  JER2_ARAD_OCB_REPARTITION_MODE_80_PRESENTS_UNICAST = 0,
  /*
   * 100 % unicast
   */
  JER2_ARAD_OCB_REPARTITION_MODE_ALL_UNICAST = 1,
  /*
   * nof modes of JER2_ARAD_OCB_REPARTION_MODE
   */
  JER2_ARAD_OCB_NOF_REPARTITION_MODES = 2
}JER2_ARAD_OCB_REPARTITION_MODE;

typedef enum
{
  /*
   * Work in regular priority mode
   */
  JER2_ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_OFF,
  /*
   * Work in strict priority mode
   */
  JER2_ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE_ON,

  JER2_ARAD_MGMT_ILKN_TDM_SP_NOF_MODES
}JER2_ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE;


typedef struct
{
  

  /* OOB TX CLK Speed */
  JER2_ARAD_FC_OOB_TX_SPEED oob_tx_speed[SOC_DNX_FC_NOF_OOB_IDS];
  
  /* OOB Type - SPI / ILKN / HCFC / NONE */
  SOC_DNX_FC_OOB_TYPE fc_oob_type[SOC_DNX_FC_NOF_OOB_IDS];

  /* Bitwise mapping */
  /* 0x1 - RX */
  /* 0x2 - TX */
  /* 0x3 - Both */
  uint32 fc_directions[SOC_DNX_FC_NOF_OOB_IDS];

  /* OOB Calendar length per interface per direction [2][2] */
  uint32 fc_oob_calender_length[SOC_DNX_FC_NOF_OOB_IDS][SOC_DNX_CONNECTION_DIRECTION_BOTH];

  /* OOB Calendar reps per interface per direction [2][2] */
  uint32 fc_oob_calender_rep_count[SOC_DNX_FC_NOF_OOB_IDS][SOC_DNX_CONNECTION_DIRECTION_BOTH];

  /* Enable CL SCHEDULER Flow Control (Ardon - priority over port) */
  int cl_sch_enable;

}JER2_ARAD_INIT_FC;

typedef struct
{
  

  /* RCPU slave port */
  soc_pbmp_t slave_port_pbmp;

} JER2_ARAD_INIT_RCPU;

typedef struct
{
  
  /*
   *  Enable Packet Processing features. Valid only for Arad
   *  revisions that support Packet Processing.
   */
  uint8 pp_enable;
  /* 
   *  Enable Petra-B in the system.
   */
  uint8 is_petrab_in_system;
  
  /*
   *  Fabric configuration.
   */
  JER2_ARAD_INIT_FABRIC fabric;
  /*
   *  Core clock frequency
   *  configuration.
   */
  JER2_ARAD_INIT_CORE_FREQ core_freq;
  /* 
   * Ports configuration
   */
  JER2_ARAD_INIT_PORTS ports;
  /*
   *  TDM traffic mode and FTMH format configuration.
   */
  JER2_ARAD_MGMT_TDM_MODE tdm_mode;
  /*
   *  ILKN TDM strict priority mode
   */
  JER2_ARAD_MGMT_ILKN_TDM_DEDICATED_QUEUING_MODE ilkn_tdm_dedicated_queuing;
  /*
   *  TDM egress priority mode
   */
  uint32 tdm_egress_priority;
  /*
   *  TDM egress drop precedence
   */
  uint32 tdm_egress_dp;
  /*
   *  pll
   */
  JER2_ARAD_INIT_PLL pll;

  /* Flow Control */
  JER2_ARAD_INIT_FC fc;
  /* 
   * Egress Shared CGM mode 
   */ 
  dnx_egr_queuing_partition_scheme_e eg_cgm_scheme;
  /* 
   * RCPU 
   */ 
  JER2_ARAD_INIT_RCPU rcpu;
  /*
   * OOB External voltage mode
   */
  JER2_ARAD_MGMT_EXT_VOLT_MOD ex_vol_mod;
  /* 
   * enable nif recovery check
   */
  uint32 nif_recovery_enable; 
  /* 
   * max num of iterations to allow
   * on nif recovery check (if enabled)
   */
  uint32 nif_recovery_iter;

  /*
   * if set, mirror/snooped packets DSP-Ext field  
   * will be stamped with the SYS dsp. 
   * FTMH extension must be enabled. 
   */
  uint8 mirror_stamp_sys_dsp_ext;

  /* 
   * enable allocating rcy port (termination context) for each mirrored channel in a channelized interface, otherwise only 1 termination context (tm port) 
   * is allocated for the whole interface rather than seperate termination context for each channel 
   */
  uint32 rcy_channelized_shared_context_enable; 
}JER2_ARAD_MGMT_INIT;

typedef struct
{
  
  /*
   * minimum
   */
  uint32 min;
  /*
   * maximum
   */
  uint32 max;
}JER2_ARAD_MGMT_OCB_MC_RANGE;

typedef struct
{
  /* is reserved port */
  int is_reserved;

  /* core id of reserved port */
  int core;

  /* tm_port of reserved port */
  uint32 tm_port;
}JER2_ARAD_MGMT_RESERVED_PORT_INFO;


typedef struct soc_dnx_config_jer2_arad_plus_s {
    uint16 nof_remote_faps_with_remote_credit_value; /* number of remote faps for which the reomte credit value  was assigned */
    uint32 per_module_credit_value[JER2_ARAD_PLUS_CREDIT_VALUE_MODE_WORDS];
} soc_dnx_config_jer2_arad_plus_t;

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

/*********************************************************************
* NAME:
*     jer2_arad_register_device
* TYPE:
*   PROC
* FUNCTION:
*     This procedure registers a new device to be taken care
*     of by this device driver. Physical device must be
*     accessible by CPU when this call is made..
* INPUT:
*  DNX_SAND_IN  uint32                  *base_address -
*     Base address of direct access memory assigned for
*     device's registers. This parameter needs to be specified
*     even if physical access to device is not by direct
*     access memory since all logic, within driver, up to
*     actual physical access, assumes 'virtual' direct access
*     memory. Memory block assigned by this pointer must not
*     overlap other memory blocks in user's system and
*     certainly not memory blocks assigned to other JER2_ARAD
*     devices using this procedure.
*  DNX_SAND_IN  DNX_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr -
*     BSP-function for device reset. Refer to
*     'DNX_SAND_RESET_DEVICE_FUNC_PTR' definition.
*  DNX_SAND_OUT uint32                 *unit_ptr -
*     This procedure loads pointed memory with identifier of
*     newly added device. This identifier is to be used by the
*     caller for further accesses to this device..
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_register_device(
             uint32                  *base_address,
    DNX_SAND_IN  DNX_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    DNX_SAND_INOUT int                 *unit_ptr
  );

/*********************************************************************
* NAME:
*     jer2_arad_unregister_device
* TYPE:
*   PROC
* FUNCTION:
*     Undo jer2_arad_register_device()
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     The device ID to be unregistered.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_unregister_device(
    DNX_SAND_IN  int                 unit
  );
int
  jer2_arad_mgmt_tm_domain_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 tm_domain
  );

int
  jer2_arad_mgmt_tm_domain_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32                 tm_domain
  );

int
  jer2_arad_mgmt_tm_domain_get(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_OUT uint32                 *tm_domain
  );

/*********************************************************************
*     Enable / Disable the forcing of (bypass) TDM cells to fabric
*********************************************************************/
int
  jer2_arad_force_tdm_bypass_traffic_to_fabric_set(
    DNX_SAND_IN  int     unit,
    DNX_SAND_IN  int     enable
  );
/*********************************************************************
*     Check if forcing of (bypass) TDM cells to fabric
*********************************************************************/
int
  jer2_arad_force_tdm_bypass_traffic_to_fabric_get(
    DNX_SAND_IN  int     unit,
    DNX_SAND_OUT int     *enable
  );

/*********************************************************************
* NAME:
*     jer2_arad_mgmt_enable_traffic_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint8                 enable -
*     DNX_SAND_IN uint32 enable_indication
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_mgmt_enable_traffic_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     jer2_arad_mgmt_enable_traffic_verify
* TYPE:
*   PROC
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint8                 enable -
*     DNX_SAND_IN uint8 enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_mgmt_enable_traffic_verify(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     jer2_arad_mgmt_enable_traffic_get
* TYPE:
*   PROC
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_OUT uint8  *enable -
*     DNX_SAND_OUT uint8  enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  jer2_arad_mgmt_enable_traffic_get(
    DNX_SAND_IN  int  unit,
    DNX_SAND_OUT uint8  *enable
  );

/*********************************************************************
* NAME:
 *   jer2_arad_mgmt_max_pckt_size_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the maximal allowed packet size. The limitation can
 *   be performed based on the packet size before or after
 *   the ingress editing (external and internal configuration
 *   mode, accordingly). Packets above the specified value
 *   are dropped.
 * INPUT:
 *   DNX_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  uint32                      port_ndx -
 *     Incoming port index. Range: 0 - 79.
 *   DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
 *     External mode filters the packets according to there
 *     original size. Internal mode filters the packets
 *     according to their size inside the device, after ingress
 *     editing.
 *   DNX_SAND_IN  uint32                       *max_size -
 *     Maximal allowed packet size per incoming port. Packets
 *     above this value will be dropped. Units: bytes.
 * REMARKS:
 *   1. This API gives a better resolution (i.e., per
 *   incoming port) than jer2_arad_mgmt_pckt_size_range_set. 2.
 *   If both APIs are used to configure the maximal packet
 *   size, the value configured is set by the API called at
 *   last.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_mgmt_max_pckt_size_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      port_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    DNX_SAND_IN  uint32                       max_size
  );

int
  jer2_arad_mgmt_max_pckt_size_set_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      port_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    DNX_SAND_IN  uint32                       max_size
  );

int
  jer2_arad_mgmt_max_pckt_size_get_verify(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      port_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx
  );

/*********************************************************************
*     Gets the configuration set by the
 *     "jer2_arad_mgmt_max_pckt_size_set" API.
 *     Refer to "jer2_arad_mgmt_max_pckt_size_set" API for
 *     details.
*********************************************************************/
int
  jer2_arad_mgmt_max_pckt_size_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      port_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    DNX_SAND_OUT uint32                       *max_size
  );
int
  jer2_arad_mgmt_pckt_size_range_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE          *size_range
  );

int
  jer2_arad_mgmt_pckt_size_range_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  JER2_ARAD_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    DNX_SAND_OUT JER2_ARAD_MGMT_PCKT_SIZE          *size_range
  );

/*********************************************************************
 * Set the MTU (maximal allowed packet size) for any packet,
 * according to the buffer size.
 *********************************************************************/
int
  jer2_arad_mgmt_set_mru_by_dbuff_size(
    DNX_SAND_IN  int     unit
  );

uint8
  jer2_arad_mgmt_is_pp_enabled(
    DNX_SAND_IN int unit
  );

/*********************************************************************
* NAME:
 *   jer2_arad_mgmt_ocb_mc_range_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ocb muliticast range.
 * INPUT:
 *   DNX_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *   DNX_SAND_IN  uint32                      range_ndx -
 *     Incoming range index. Range: 0 - 1.
 *   DNX_SAND_IN  JER2_ARAD_MGMT_OCB_MC_RANGE *range -
 *     Structure with minimum and maximum.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_mgmt_ocb_mc_range_set(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      range_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_OCB_MC_RANGE         *range
  );

int
  jer2_arad_mgmt_ocb_mc_range_get(
    DNX_SAND_IN  int                      unit,
    DNX_SAND_IN  uint32                      range_ndx,
    DNX_SAND_OUT JER2_ARAD_MGMT_OCB_MC_RANGE         *range
  );

/*********************************************************************
* NAME:
 *   jer2_arad_mgmt_ocb_voq_eligible_set
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Set the ocb queue parameters.
 * INPUT:
 *   DNX_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *  DNX_SAND_IN  uint32                    q_category_ndx -
 *     Queue category. Range: 0 - 3.
 *  DNX_SAND_IN  uint32                    rate_class_ndx -
 *     Queue rate class index. Range: 0 - 63
 *  DNX_SAND_IN  uint32                    tc_ndx - 
 *     Traffic class. Range: 0 - 7.
 *  DNX_SAND_IN  JER2_ARAD_MGMT_OCB_VOQ_INFO       *info - 
 *     Structure with the required data:
 *      - enable/diasable the ocb.
 *      - 2 word thresholds
 *      - 2 buffers thresholds
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_mgmt_ocb_voq_eligible_set(
    DNX_SAND_IN  int                    unit,
    DNX_SAND_IN  uint32                    q_category_ndx,
    DNX_SAND_IN  uint32                    rate_class_ndx,
    DNX_SAND_IN  uint32                    tc_ndx,
    DNX_SAND_IN  JER2_ARAD_MGMT_OCB_VOQ_INFO       *info,
    DNX_SAND_OUT JER2_ARAD_MGMT_OCB_VOQ_INFO    *exact_info
  );

int
soc_jer2_arad_cache_table_update_all(
    DNX_SAND_IN int unit
  );

/*********************************************************************
* NAME:
 *   jer2_arad_mgmt_ocb_voq_eligible_dynamic_set
 * FUNCTION:
 *   Set the  IDR_MEM_1F0000 Stores1 bit per queue number (128k queues) which indicates if the queue can be used.
 * INPUT:
 *   DNX_SAND_IN  int                   unit -
 *     Identifier of the device to access.
 *  DNX_SAND_IN  uint32                    qid -
 *     the qid range 0-128K
 *  DNX_SAND_IN  uint32                    enable -
 *     enable q FALSE or TRUE
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/

int
  jer2_arad_mgmt_ocb_voq_eligible_dynamic_set(
    DNX_SAND_IN  int                    unit,
    DNX_SAND_IN  uint32                    qid,
    DNX_SAND_IN  uint32                    enable
  );

/*********************************************************************
*     Set the Soc_JER2_ARAD B0 revision specific features.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int soc_jer2_arad_mgmt_rev_b0_set(
    DNX_SAND_IN  int       unit);

/*********************************************************************
*     Set the Soc_JER2_ARAD PLUS revision specific features.
*     Details: in the H file. (search for prototype)
*********************************************************************/
int soc_jer2_arad_mgmt_rev_jer2_arad_plus_set(
    DNX_SAND_IN  int       unit);
    

int 
  jer2_arad_mgmt_nof_block_instances(
    int unit, 
    soc_block_types_t block_types, 
    int *nof_block_instances
  );

int ardon_mgmt_drv_pvt_monitor_enable(int unit);

int 
   jer2_arad_mgmt_temp_pvt_get(
     int unit,
     int temperature_max,
     soc_switch_temperature_monitor_t *temperature_array,
     int *temperature_count
   ) ;

/*********************************************************************
*     Get the AVS - Adjustable Voltage Scaling value of the Arad
*********************************************************************/
int jer2_arad_mgmt_avs_value_get(
        int       unit,
        uint32*      avs_val);
/* } */
int
  jer2_arad_mgmt_sw_ver_set(
    DNX_SAND_IN  int                      unit);


    
#ifdef FIXME_DNX_LEGACY
/********************************************************** 
* Set OCB VOQ INFO defaults 
**********************************************************/ 
int jer2_arad_mgmt_ocb_voq_info_defaults_set(
    DNX_SAND_IN     int                         unit,
    DNX_SAND_OUT    JER2_ARAD_MGMT_OCB_VOQ_INFO      *ocb_info
    );
#endif 

/***********************************************************************************
* NAME:
*     jer2_arad_calc_assigned_rebounded_credit_conf
* TYPE:
*   PROC
* FUNCTION:
*   Helper utility for *_mgmt_credit_worth_set():
*   Calculate value for SCH_ASSIGNED_CREDIT_CONFIGURATION, SCH_REBOUNDED_CREDIT_CONFIGURATION,
*   given input 'credit_worth'
*   Output goes into *fld_val to be loaded into registers specified above, into 
*   ASSIGNED_CREDIT_WORTH, REBOUNDED_CREDIT_WORTH fields, correspondingly
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32              credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
*  DNX_SAND_OUT uint32              *fld_val -
*     Value to load into fields specified in 'FUNCTION' above.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
***********************************************************************************/
int
  jer2_arad_calc_assigned_rebounded_credit_conf(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32              credit_worth,
    DNX_SAND_OUT uint32              *fld_val
  ) ;
/*********************************************************************
* NAME:
*     jer2_arad_mgmt_credit_worth_set
* TYPE:
*   PROC
* FUNCTION:
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
* INPUT:
*  DNX_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  DNX_SAND_IN  uint32                  credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  jer2_arad_mgmt_credit_worth_set(
    DNX_SAND_IN  int                 unit,
    DNX_SAND_IN  uint32              credit_worth
  );

/*
 * Arad+ only: set local and remote (1 and 2) credit worth values
 */
int
  jer2_arad_plus_mgmt_credit_worth_remote_set(
    DNX_SAND_IN  int    unit,
	DNX_SAND_IN  uint32    credit_worth_remote
  );

/*
 * Arad+ only: get local and remote (1 and 2) credit worth values
 */
int
  jer2_arad_plus_mgmt_credit_worth_remote_get(
    DNX_SAND_IN  int    unit,
	DNX_SAND_OUT uint32    *credit_worth_remote
  );

int
  jer2_arad_plus_mgmt_change_all_faps_credit_worth(
    DNX_SAND_IN  int    unit,
    DNX_SAND_OUT uint8     credit_value_to_use
  );


int
  jer2_arad_mgmt_ocb_voq_eligible_get(
    DNX_SAND_IN  int                    unit,
    DNX_SAND_IN  uint32                    q_category_ndx,
    DNX_SAND_IN  uint32                    rate_class_ndx,
    DNX_SAND_IN  uint32                    tc_ndx,
    DNX_SAND_OUT JER2_ARAD_MGMT_OCB_VOQ_INFO       *info
  );

void
  jer2_arad_JER2_ARAD_HW_PLL_PARAMS_clear(
    DNX_SAND_OUT JER2_ARAD_HW_PLL_PARAMS *info
  );

void
  JER2_ARAD_INIT_PLL_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PLL *info
  );

void
  jer2_arad_JER2_ARAD_INIT_FABRIC_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_FABRIC *info
  );

void
  jer2_arad_JER2_ARAD_INIT_CORE_FREQ_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_CORE_FREQ *info
  );

void
  jer2_arad_JER2_ARAD_MGMT_INIT_clear(
    DNX_SAND_OUT JER2_ARAD_MGMT_INIT *info
  );

void
  jer2_arad_JER2_ARAD_MGMT_OCB_MC_RANGE_clear(
    DNX_SAND_OUT JER2_ARAD_MGMT_OCB_MC_RANGE *info
  );

void
  jer2_arad_JER2_ARAD_MGMT_OCB_VOQ_INFO_clear(
    DNX_SAND_OUT JER2_ARAD_MGMT_OCB_VOQ_INFO *info
  );

void
  jer2_arad_JER2_ARAD_MGMT_PCKT_SIZE_clear(
    DNX_SAND_OUT JER2_ARAD_MGMT_PCKT_SIZE *info
  );


void
  jer2_arad_JER2_ARAD_INIT_PORT_HDR_TYPE_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PORT_HDR_TYPE *info
  );

void
  jer2_arad_JER2_ARAD_INIT_PORT_TO_IF_MAP_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PORT_TO_IF_MAP *info
  );

void
  JER2_ARAD_INIT_PP_PORT_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PP_PORT *info
  );

void
  JER2_ARAD_INIT_PP_PORT_MAP_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PP_PORT_MAP *info
  );

void
  JER2_ARAD_INIT_EGR_Q_PROFILE_MAP_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_EGR_Q_PROFILE_MAP *info
  );
  
void
  JER2_ARAD_INIT_PORTS_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PORTS *info
  );

void
  jer2_arad_JER2_ARAD_INIT_PORTS_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_PORTS *info
  );

void
  jer2_arad_JER2_ARAD_INIT_FC_clear(
    DNX_SAND_OUT JER2_ARAD_INIT_FC *info
  );

void
  jer2_arad_JER2_ARAD_HW_PLL_PARAMS_print(
    DNX_SAND_IN JER2_ARAD_HW_PLL_PARAMS *info
  );

void
  jer2_arad_JER2_ARAD_INIT_FABRIC_print(
    DNX_SAND_IN JER2_ARAD_INIT_FABRIC *info
  );

void
  jer2_arad_JER2_ARAD_INIT_CORE_FREQ_print(
    DNX_SAND_IN  JER2_ARAD_INIT_CORE_FREQ *info
  );

void
  jer2_arad_JER2_ARAD_MGMT_INIT_print(
    DNX_SAND_IN JER2_ARAD_MGMT_INIT *info
  );

void
  jer2_arad_JER2_ARAD_INIT_PORT_HDR_TYPE_print(
    DNX_SAND_IN JER2_ARAD_INIT_PORT_HDR_TYPE *info
  );

void
  jer2_arad_JER2_ARAD_INIT_PORT_TO_IF_MAP_print(
    DNX_SAND_IN JER2_ARAD_INIT_PORT_TO_IF_MAP *info
  );

void
  JER2_ARAD_INIT_PP_PORT_print(
    DNX_SAND_IN  JER2_ARAD_INIT_PP_PORT *info
  );

void
  JER2_ARAD_INIT_PP_PORT_MAP_print(
    DNX_SAND_IN  JER2_ARAD_INIT_PP_PORT_MAP *info
  );

void
  JER2_ARAD_INIT_EGR_Q_PROFILE_MAP_print(
    DNX_SAND_IN  JER2_ARAD_INIT_EGR_Q_PROFILE_MAP *info
  );

void
  JER2_ARAD_INIT_PORTS_print(
    DNX_SAND_IN  JER2_ARAD_INIT_PORTS *info
  );



/* } __JER2_ARAD_MGMT_INCLUDED__*/

#endif
