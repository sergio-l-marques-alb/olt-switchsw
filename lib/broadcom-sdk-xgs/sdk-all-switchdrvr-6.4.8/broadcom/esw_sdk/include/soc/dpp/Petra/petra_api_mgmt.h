/* $Id$
 * $Copyright: Copyright 2015 Broadcom Corporation.
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

#ifndef __SOC_PETRA_API_MGMT_INCLUDED__
/* { */
#define __SOC_PETRA_API_MGMT_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/dpp_config_defs.h>
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>
#include <soc/dpp/SAND/SAND_FM/sand_chip_defines.h>
#include <soc/dpp/Petra/petra_api_general.h>
#include <soc/dpp/TMC/tmc_api_mgmt.h>
#include <soc/dpp/Petra/petra_api_serdes.h>
#include <soc/dpp/Petra/petra_api_fabric.h>
#include <soc/dpp/Petra/petra_api_nif.h>
#include <soc/dpp/Petra/petra_api_ingress_traffic_mgmt.h>
#include <soc/dpp/Petra/petra_api_ports.h>
#include <soc/dpp/Petra/petra_api_egr_queuing.h>
#include <soc/dpp/Petra/petra_api_end2end_scheduler.h>
#include <soc/dpp/Petra/petra_api_ingress_header_parsing.h>
#include <soc/dpp/Petra/petra_api_dram.h>
#include <soc/dpp/Petra/petra_api_stat_if.h>
/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/* $Id$
 *  Minimal credit size, in Byte resolution
 */
#define SOC_PETRA_CREDIT_SIZE_BYTES_MIN 1

/*
 *  Maximal credit size, in Byte resolution
 */
#define SOC_PETRA_CREDIT_SIZE_BYTES_MAX ((8 * 1024) - 1)

#define  SOC_PETRA_MAX_NOF_REVISIONS SOC_PETRA_REVISION_NOF_IDS

/*     Maximal length of DRAM configuration sequence.          */
#define  SOC_PETRA_HW_DRAM_CONF_SIZE_MAX 100

/*     Out of total of 15 SerDes quartets, two (one per
*     internal NIF Group consisting of 4 MAL-s) may be
*     assigned to either Network or Fabric interfaces.        */

#define SOC_PETRA_MGMT_VER_REG_BASE       0x0
#define SOC_PETRA_MGMT_CHIP_TYPE_FLD_LSB   4
#define SOC_PETRA_MGMT_CHIP_TYPE_FLD_MSB   23
#define SOC_PETRA_MGMT_DBG_VER_FLD_LSB     24
#define SOC_PETRA_MGMT_DBG_VER_FLD_MSB     27
#define SOC_PETRA_MGMT_CHIP_VER_FLD_LSB    28
#define SOC_PETRA_MGMT_CHIP_VER_FLD_MSB    31

/*
 *  This value disables limitation based on external (original)
 *  packet size
 */
#define SOC_PETRA_MGMT_PCKT_SIZE_EXTERN_NO_LIMIT 0

#define SOC_PETRA_HW_DRAM_CONF_MODE_MIN   (SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER)
#define SOC_PETRA_HW_DRAM_CONF_MODE_MAX   (SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS)

#define SOC_PETRA_MGMT_FDR_TRFC_DISABLE         0x0
#define SOC_PETRA_MGMT_FDR_TRFC_ENABLE_VAR_CELL 0x80fd5fa1
#define SOC_PETRA_MGMT_FDR_TRFC_ENABLE_FIX_CELL 0x80fd5fa1
/*
  * Indicator to the Software that the device was initialized, and traffic was enabled
  * Upon warm-start initialization, the software may read this register, and see
  * if the traffic was enabled.
  * If it was, the SW may utilize the SSR capabilities, to initialize the software,
  * without affecting the device
  * Used by soc_petra_mgmt_enable_traffic_set()
  */
#define SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_1 SOC_PETRA_MGMT_FDR_TRFC_ENABLE_VAR_CELL
#define SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_2 SOC_PETRA_MGMT_FDR_TRFC_ENABLE_FIX_CELL
/* Values of SOC_PETRA_MGMT_FDR_TRFC_ENABLE were changed. These were the
   previous values. Those also indicate that device was initialized (ssr) */
#define SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_3 0x80033fa1
#define SOC_PETRA_MGMT_DEVICE_WAS_INITIALIZED_4 0x80034fa1

#define SOC_PETRA_MGMT_PCKT_RNG_HW_OFFSET 1
#define SOC_PETRA_MGMT_PCKT_RNG_NIF_CRC_BYTES 4
#define SOC_PETRA_MGMT_PCKT_RNG_DRAM_CRC_BYTES 2
#define SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL \
  (SOC_PETRA_MGMT_PCKT_RNG_HW_OFFSET)
#define SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL \
  (SOC_PETRA_MGMT_PCKT_RNG_HW_OFFSET + SOC_PETRA_MGMT_PCKT_RNG_NIF_CRC_BYTES - SOC_PETRA_MGMT_PCKT_RNG_DRAM_CRC_BYTES)
#define SOC_PETRA_MGMT_PCKT_MAX_SIZE_INTERNAL_MAX                     ((0x3FFF) + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_INTERNAL)
#define SOC_PETRA_MGMT_PCKT_MAX_SIZE_EXTERNAL_MAX                     ((0x3FFF) + SOC_PETRA_MGMT_PCKT_RNG_CORRECTION_EXTERNAL)
/*************
 * MACROS    *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */
#define SOC_PETRA_MGMT_CORE_CLK_RATE_250MHZ                    SOC_TMC_MGMT_CORE_CLK_RATE_250MHZ
#define SOC_PETRA_MGMT_CORE_CLK_RATE_300MHZ                    SOC_TMC_MGMT_CORE_CLK_RATE_300MHZ
typedef SOC_TMC_MGMT_CORE_CLK_RATE                             SOC_PETRA_MGMT_CORE_CLK_RATE;


#define SOC_PETRA_MGMT_FABRIC_HDR_TYPE_PETRA                   SOC_TMC_MGMT_FABRIC_HDR_TYPE_PETRA
#define SOC_PETRA_MGMT_FABRIC_HDR_TYPE_FAP20                   SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP20
#define SOC_PETRA_MGMT_FABRIC_HDR_TYPE_FAP10M                  SOC_TMC_MGMT_FABRIC_HDR_TYPE_FAP10M
typedef SOC_TMC_MGMT_FABRIC_HDR_TYPE                           SOC_PETRA_MGMT_FABRIC_HDR_TYPE;

#define SOC_PETRA_MGMT_TDM_MODE_PACKET                         SOC_TMC_MGMT_TDM_MODE_PACKET
#define SOC_PETRA_MGMT_TDM_MODE_TDM_OPT                        SOC_TMC_MGMT_TDM_MODE_TDM_OPT
#define SOC_PETRA_MGMT_TDM_MODE_TDM_STA                        SOC_TMC_MGMT_TDM_MODE_TDM_STA
typedef SOC_TMC_MGMT_TDM_MODE                                  SOC_PETRA_MGMT_TDM_MODE;

#define SOC_PETRA_MGMT_NOF_TDM_MODES                           SOC_TMC_MGMT_NOF_TDM_MODES

typedef SOC_TMC_MGMT_PCKT_SIZE                                 SOC_PETRA_MGMT_PCKT_SIZE;

#define SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_EXTERN               SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_EXTERN     
#define SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE_INTERN               SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE_INTERN     
#define SOC_PETRA_MGMT_NOF_PCKT_SIZE_CONF_MODES                 SOC_TMC_MGMT_NOF_PCKT_SIZE_CONF_MODES       
typedef SOC_TMC_MGMT_PCKT_SIZE_CONF_MODE                       	SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE;

typedef enum
{
  /*
   *  Soc_petra device revision: 1
   */
  SOC_PETRA_REVISION_ID_1=0,
  /*
   *  Total number of Soc_petra revisions
   */
  SOC_PETRA_REVISION_NOF_IDS=1
}SOC_PETRA_REVISION_ID;

typedef enum
{
  /*
   *  Device type: DN-88210-DV - 1 x OC768c
   */
  SOC_PETRA_REVISION_TYPE_88210=0,
  /*
   *  Device type: DN-88220-DV - 2 x OC192 or 2 x 10GE
   */
  SOC_PETRA_REVISION_TYPE_88220=1,
  /*
   *  Device type: DN-88230-DV- 8 x 10GE or 4 x OC192
   */
  SOC_PETRA_REVISION_TYPE_88230=2,
  /*
   *  Device type: DN-88330-DV- 8 x 10GE or 4 x OC192 + PP
   */
  SOC_PETRA_REVISION_TYPE_88330=3,
  /*
   *  Total number of Soc_petra sub-types
   */
  SOC_PETRA_REVISION_NOF_TYPES=4
}SOC_PETRA_REVISION_SUB_TYPE_ID;

typedef enum
{
  /*
   *  Configure dram according to a buffer of registers to write
   *  and their values (<addr, val>)
   */
  SOC_PETRA_HW_DRAM_CONF_MODE_BUFFER=0,
  /*
   *  Configure dram according to logical parameters provided in
   *  the dram's vendor data sheet
   */
  SOC_PETRA_HW_DRAM_CONF_MODE_PARAMS=1,
  /*
   *  Total number of DRAM configuration modes
   */
  SOC_PETRA_HW_NOF_DRAM_CONF_MODES
}SOC_PETRA_HW_DRAM_CONF_MODE;

typedef enum
{
  /*
   *  te QDR data is parity protected (4 bits parity, 32 bits
   *  of data).
   */
  SOC_PETRA_HW_QDR_PROTECT_TYPE_PARITY=0,
  /*
   *  te QDR data is ECC protected (6 bits for ECC, 30 bits of
   *  data).
   */
  SOC_PETRA_HW_QDR_PROTECT_TYPE_ECC=1,
  /*
   *  Total number of QDR protection types
   */
  SOC_PETRA_HW_NOF_QDR_PROTECT_TYPES=2
}SOC_PETRA_HW_QDR_PROTECT_TYPE;

typedef enum
{
  /*
   *  QDR SRAM memory size, 18 Mbits.
   */
  SOC_PETRA_HW_QDR_SIZE_MBIT_18=0,
  /*
   *  QDR SRAM memory size, 36 Mbits.
   */
  SOC_PETRA_HW_QDR_SIZE_MBIT_36=1,
  /*
   *  QDR SRAM memory size, 72 Mbits.
   */
  SOC_PETRA_HW_QDR_SIZE_MBIT_72=2,
  /*
   *  QDR SRAM memory size, 144 Mbits.
   */
  SOC_PETRA_HW_QDR_SIZE_MBIT_144=3,
  /*
   *  Total number of QDR SRAM memory sizes.
   */
  SOC_PETRA_HW_NOF_QDR_SIZE_MBITS=4
}SOC_PETRA_HW_QDR_SIZE_MBIT;


typedef enum
{
  /*
   *  SerDes reference clock - 125.00 Mhz
   */
  SOC_PETRA_MGMT_SRD_REF_CLK_125_00=0,
  /*
   *  SerDes reference clock - 156.25 Mhz
   */
  SOC_PETRA_MGMT_SRD_REF_CLK_156_25=1,
  /*
   *  SerDes reference clock - 200.00 Mhz
   */
  SOC_PETRA_MGMT_SRD_REF_CLK_200_00=2,
  /*
   *  SerDes reference clock - 312.50 Mhz
   */
  SOC_PETRA_MGMT_SRD_REF_CLK_312_50=3,
  /*
   *  SerDes reference clock - 218.75 Mhz
   *  Valid for NIF (or Combo towards the NIF)  SerDes only.
   */
  SOC_PETRA_MGMT_SRD_REF_CLK_218_75=4,
  /*
   *  SerDes reference clock - 212.50 Mhz
   *  Valid for NIF (or Combo towards the NIF)  SerDes only.
   */
  SOC_PETRA_MGMT_SRD_REF_CLK_212_50=5,
  /*
   *  Total number of valid SerDes reference clock values.
   */
  SOC_PETRA_MGMT_NOF_SRD_REF_CLKS=6
}SOC_PETRA_MGMT_SRD_REF_CLK;

#define SOC_PETRA_MGMT_IS_SRD_REF_CLK_ENUM(ref_clk_val) \
  SOC_SAND_NUM2BOOL(SOC_SAND_IS_VAL_IN_RANGE(ref_clk_val, SOC_PETRA_MGMT_SRD_REF_CLK_125_00, SOC_PETRA_MGMT_NOF_SRD_REF_CLKS))

#define SOC_PETRA_MGMT_SRD_REF_CLK_BY_NUM_KBPS_MIN 10000
#define SOC_PETRA_MGMT_SRD_REF_CLK_BY_NUM_KBPS_MAX 810000


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The device revision ID. According to the device revision
   *  information read from the device
   */
  SOC_PETRA_REVISION_ID device_rev_id;
  /*
   *  The device sub-type. Available options: DN-88210-DV,
   *  DN-88220-DV, DN-88230-DV
   */
  SOC_PETRA_REVISION_SUB_TYPE_ID device_sub_type;
  /*
   *  Enable bug fixes added in 'device_rev_id'
   */
  uint8 enable_bug_fixes;
  /*
   *  Enable features added in 'device_rev_id'
   */
  uint8 enable_features;
}SOC_PETRA_REVISION_CTRL_REV_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The valid entries in the revision_info structure. Range:
   *  0 - 1.
   */
  uint32 nof_valid_entries;
  /*
   *  Per revision information
   */
  SOC_PETRA_REVISION_CTRL_REV_INFO revision_info[SOC_PETRA_MAX_NOF_REVISIONS];
}SOC_PETRA_REVISION_CTRL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
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

}SOC_PETRA_HW_PLL_PARAMS;

typedef struct
{
  /*
   *  Number of valid entries in dram_conf.
   */
  uint32 buff_len;

  SOC_PETRA_REG_INFO  buff_seq[SOC_PETRA_HW_DRAM_CONF_SIZE_MAX];

}SOC_PETRA_HW_DRAM_CONF_BUFFER;

typedef struct
{
  uint32  dram_freq;

  SOC_PETRA_DRAM_INFO params;

}SOC_PETRA_HW_DRAM_CONF_PARAMS;

typedef union
{
  /*
   *  DRAM configuration sequence. The sequence is a buffer of
   *  address-value pairs. This defines the dram configuration
   *  (DRC)
   */
   SOC_PETRA_HW_DRAM_CONF_BUFFER buffer_mode;

  /*
   *  DRAM auto configuration. The driver automatically configures DRAM
   *  operation modes. This defines the dram configuration (DRC)
   */
   SOC_PETRA_HW_DRAM_CONF_PARAMS params_mode;

}SOC_PETRA_HW_DRAM_CONF;


typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable DRAM configuration
   */
  uint8 enable;
  /*
   *  Per DRAM interface, defines if exists and needs to be
   *  configured. Note: The following number of DRAM
   *  interfaces can be configured: 2, 3, 4, 6
   */
  uint8 is_valid[SOC_DPP_DEFS_MAX(HW_DRAM_INTERFACES_MAX)];
  /*
   *  DRAM type.
   */
  SOC_PETRA_DRAM_TYPE dram_type;
  /*
   *  DRAM Pll configuration as derived from the DRAM
   *  reference clock.
   */
  SOC_PETRA_HW_PLL_PARAMS pll_conf;
  /*
   *  Number of Banks.
   */
  SOC_PETRA_DRAM_NUM_BANKS nof_banks;
  /*
   *  Number of DRAM columns. Range:
   *  256/512/1024/2048/4096/8192
   */
  SOC_PETRA_DRAM_NUM_COLUMNS nof_columns;
  /*
   *  Summarized DRAM size of all DRAM interfaces. Units:
   *  Mbytes.
   */
  uint32 dram_size_total_mbyte;
  /*
   *  The size of a single data buffer in the DRAM
   */
  SOC_PETRA_ITM_DBUFF_SIZE_BYTES dbuff_size;
  /*
   *  T - Configure DRAM operation modes automatically
   *  F - Configure DRAM operation modes manually
   */
  SOC_PETRA_HW_DRAM_CONF_MODE conf_mode;

  /*
   *  Dram configuration mode
   */
  SOC_PETRA_HW_DRAM_CONF dram_conf;

}SOC_PETRA_HW_ADJ_DDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Statistics interface configuration
   */
  SOC_PETRA_STAT_IF_INFO if_conf;
  /*
   *  Statistics interface report configuration
   */
  SOC_PETRA_STAT_IF_REPORT_INFO rep_conf;

}SOC_PETRA_HW_ADJ_STAT_IF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable QDR configuration.
   */
  uint8 enable;
  /*
   *  Select Parity or ECC protection type.
   */
  SOC_PETRA_HW_QDR_PROTECT_TYPE protection_type;
  /*
   *  If TRUE, the 250Mhz Core clock is used as QDR reference
   *  clock. Otherwise (lower frequency) - QDR clock is used.
   *  In the later case, pll configuration must be set.
   */
  uint8 is_core_clock_freq;
  /*
   *  QDR Pll configuration as derived from the QDR reference
   *  clock. Note: this field is only relevant if
   *  is_core_clock_freq is FALSE - ignored otherwise.
   *  CAUTION: it is a misconfiguration to use QDR clock,
      configured to the Core clock frequency (250Mhz) or above!
   */
  SOC_PETRA_HW_PLL_PARAMS pll_conf;
  /*
   *  Total QDR SRAM memory size Units: Mbits.
   */
  SOC_PETRA_HW_QDR_SIZE_MBIT qdr_size_mbit;
}SOC_PETRA_HW_ADJ_QDR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable SerDes configuration
   */
  uint8 enable;
  /*
   *  SerDes configuration
   */
  SOC_PETRA_SRD_ALL_INFO conf;
}SOC_PETRA_HW_ADJ_SERDES;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable fabric configuration
   */
  uint8 enable;
  /*
   *  The way the device is connected to fabric.
   */
  SOC_PETRA_FABRIC_CONNECT_MODE connect_mode;
  /*
   *  FTMH Header configuration: always allow, never allow,
   *  allow only when the packet is multicast.
   */
  SOC_PETRA_PORTS_FTMH_EXT_OUTLIF ftmh_extension;
}SOC_PETRA_HW_ADJ_FABRIC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  MAC lane index. Range: 0 - 7.
   */
  uint32 mal_ndx;
  /*
   *  disable basic per-mal configuration - NIF type,
   *  topology-related.
   */
  SOC_PETRA_NIF_INFO conf;
}SOC_PETRA_HW_ADJ_MAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  MAC lane index. Range: 0 - 7.
   */
  uint32 mal_ndx;
  /*
   *  SPAUI extensions configuration
   */
  SOC_PETRA_NIF_SPAUI_INFO conf;
}SOC_PETRA_HW_ADJ_SPAUI;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Network Interface index, SGMII. Range: 0 - 31.
   */
  SOC_PETRA_INTERFACE_ID nif_ndx;
  /*
   *  SGMII extensions configuration
   */
  SOC_PETRA_NIF_SGMII_INFO conf;
}SOC_PETRA_HW_ADJ_SGMII;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable Fat Pipe configuration
   */
  uint8 enable;
  /*
   *  Fat Pipe configuration
   */
  SOC_PETRA_NIF_FAT_PIPE_INFO conf;
}SOC_PETRA_HW_ADJ_FAT_PIPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of entries: per-MAC lane basic configuration.
   *  Range: 0 - 8.
   */
  uint32 mal_nof_entries;
  /*
   *  MAL basic configuration, including interface type and
   *  topology (serdes)-related.
   */
  SOC_PETRA_HW_ADJ_MAL mal[SOC_PETRA_NOF_MAC_LANES];
  /*
   *  Number of entries: SPAUI extensions configuration.
   *  Range: 0 - 8.
   */
  uint32 spaui_nof_entries;
  /*
   *  SPAUI extensions configuration. If enabled, configured in
   *  both RX and TX directions.
   */
  SOC_PETRA_HW_ADJ_SPAUI spaui[SOC_PETRA_NOF_MAC_LANES];
  /*
   *  Number of entries: SGMII configuration. Range: 0 - 32.
   */
  uint32 sgmii_nof_entries;
  /*
   *  SGMII configuration. If enabled, configured in both RX
   *  and TX directions.
   */
  SOC_PETRA_HW_ADJ_SGMII sgmii[SOC_PETRA_IF_NOF_NIFS];
  /*
   *  Enable/disable Fat pipe configuration.
   */
  uint8 fat_pipe_enable;
  /*
   *  Fat pipe configuration.
   */
  SOC_PETRA_HW_ADJ_FAT_PIPE fat_pipe;

}SOC_PETRA_HW_ADJ_NIF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable Clock frequency configuration
   */
  uint8 enable;
  /*
   *  Number of ticks the SOC_PETRA device clock ticks per second
   *  (about a tick every 4.00 nano-seconds). Default value:
   *  250. Units: MHz. For Soc_petra-A, Range: 150 - 250. For
   *  Soc_petra-B, Range: 150 - 300.
   */
  uint32 frequency;

} SOC_PETRA_HW_ADJ_CORE_FREQ;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  DRAM configuration. DRAM is used by the Soc_petra to store
   *  packets at the ingress.
   */
  SOC_PETRA_HW_ADJ_DDR dram;
  /*
   *  Statistics interface configuration.
   */
  SOC_PETRA_HW_ADJ_STAT_IF stat_if;
  /*
   *  QDR configuration. QDR is used by the Soc_petra to store
   *  packet buffer descriptors and per-packet information at
   *  the ingress.
   */
  SOC_PETRA_HW_ADJ_QDR qdr;
  /*
   *  SerDes physical parameters configuration.
   */
  SOC_PETRA_HW_ADJ_SERDES serdes;
  /*
   *  Fabric configuration.
   */
  SOC_PETRA_HW_ADJ_FABRIC fabric;
  /*
   *  Network Interfaces configuration.
   */
  SOC_PETRA_HW_ADJ_NIF nif;
  /*
   *  Core clock frequency
   *  configuration.
   */
  SOC_PETRA_HW_ADJ_CORE_FREQ core_freq;

}SOC_PETRA_HW_ADJUSTMENTS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Statistics Tag field configuration - position and size
   *  inside the packet
   */
  SOC_PETRA_IHP_PCKT_STAG_HDR_DATA fld_conf;
  /*
   *  Statistics Tag encoding: VSQ, Drop Precedence etc.
   */
  SOC_PETRA_ITM_STAG_INFO encoding;
}SOC_PETRA_INIT_STAG_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/disable credit worth configuration.
   */
  uint8 credit_worth_enable;
  /*
   *  Bytes-worth of a single credit. Range: 256 - 8K bytes.
   */
  uint32 credit_worth;
  /*
   *  Enable/disable Statistics Tag configuration
   */
  uint8 stag_enable;
  /*
   *  Statistics Tag configuration - in-packet position and
   *  size and inner encoding.
   */
  SOC_PETRA_INIT_STAG_INFO stag;
  /* 
   *  Enable/disable shadowing the device memories. 
   */
  uint8 mem_shadow_enable;
}SOC_PETRA_INIT_BASIC_CONF;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  FAP Port index to configure. Range: 0 - 79
   */
  uint32 port_ndx;
  /*
   *  RAW/TM/ETH/Programmable header parsing type in the incoming direction.
   */
  SOC_PETRA_PORT_HEADER_TYPE header_type_in;
  /*
   *  header type in the outgoing direction.
   */
  SOC_PETRA_PORT_HEADER_TYPE header_type_out;
}SOC_PETRA_INIT_PORT_HDR_TYPE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  FAP Port index to configure. Range: 0 - 79
   */
  uint32 port_ndx;
  /*
   *  FAP Port to interface mapping configuration - map the
   *  specified FAP Port to interface and (for channelized
   *  interfaces) channel.
   */
  SOC_PETRA_PORT2IF_MAPPING_INFO conf;
}SOC_PETRA_INIT_PORT_TO_IF_MAP;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  FAP Port index to configure. Range: 0 - 79
   */
  uint32 port_ndx;
  /*
   *  Pert port packet header configuration.
   *  Allows skipping/removing a given amount of bytes
   *  before the Layer-2 header (e.g. ITMH).
   */
  SOC_PETRA_IHP_PORT_INFO conf;
}SOC_PETRA_INIT_PCKT_HDR_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Number of entries to configure - FAP ports packet header
   *  type. Range: 0 - 80.
   */
  uint32 hdr_type_nof_entries;
  /*
   *  FAP ports packet header type - RAW/TM/Eth/Programmable
   *  configuration.
   */
  SOC_PETRA_INIT_PORT_HDR_TYPE hdr_type[SOC_PETRA_NOF_FAP_PORTS];
  /*
   *  Number of entries to configure - FAP ports to NIF
   *  mapping. Range: 0 - 80.
   */
  uint32 if_map_nof_entries;
  /*
   *  FAP ports to Interface mapping configuration.
   */
  SOC_PETRA_INIT_PORT_TO_IF_MAP if_map[SOC_PETRA_NOF_FAP_PORTS];
  /*
   *  Number of entries to configure - packet header per-port
   *  configuration. Range: 0 - 80.
   */
  uint32 packet_hdr_info_nof_entries;
  /*
   *  Packet header per-port configuration.
   */
  SOC_PETRA_INIT_PCKT_HDR_INFO packet_hdr_info[SOC_PETRA_NOF_FAP_PORTS];
}SOC_PETRA_INIT_PORTS;

/*
 *	Reference clock configuration.
 *  Two configuration modes are supported:
 *  1. By enumerator. In this case, each reference clock
 *     value must be set to a value as defined in the SOC_PETRA_MGMT_SRD_REF_CLK enumerator.
 *     For example, to set the a 156.25 MHz ref-clock, use the value: SOC_PETRA_MGMT_SRD_REF_CLK_156_25.
 *     The SerDes rate configuration (m/n/divisor) is then chosen accordingly.
 *  2. By number. In this case, the exact ref-clock in kilohertz is specified.
 *     For example, to set the a 156. MHz ref-clock, use the value: 156250.
 *     The SerDes rate configuration then uses m = n = 0, and the appropriate divisor.
 */
typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  The reference clock that feeds the NIF CMU-s.
   */
  uint32 nif_ref_clk;
  /*
   *  The reference clock that feeds the Fabric CMU-s.
   */
  uint32 fabric_ref_clk;
  /*
   *  The reference clock that feeds the Combo CMU-s. The
   *  combo CMU-s can be dedicated to either NIF or fabric -
   *  refer to the 'shared_quartet' configuration in the
   *  operation mode.
   */
  uint32 combo_ref_clk;
} SOC_PETRA_MGMT_SRD_REF_CLKS;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Soc_petra device sub-type
   */
  SOC_PETRA_REVISION_SUB_TYPE_ID device_sub_type;
  /*
   *  Enable Packet Processing features. Valid only for Soc_petra
   *  revisions that support Packet Processing.
   */
  uint8 pp_enable;
  /*
   *  Enable Stacking Port. Valid only for Soc_petra
   *  revisions that support Packet Processing.
   */
  uint8 stacking_enable;
  /*
   *  If TRUE, fap20 devices exist in the system. This imposes
   *  certain limitations on Soc_petra behavior (e.g. fabric
   *  cells must be fixed size, fap20-compatible fabric header must be used etc.).
   */
  uint8 is_fap20_in_system;
  /*
   *  If TRUE, fap21 devices exist in the system. This imposes
   *  certain limitations on Soc_petra behavior (e.g. fabric
   *  cells must be fixed size etc.).
   */
  uint8 is_fap21_in_system;
  /*
   *  If TRUE, soc_petra revision A1 or A0 devices exist in the
   *  system.
   */
  uint8 is_a1_or_below_in_system;
  /*
   *  If TRUE, the system uses SOC_SAND_FE200 fabric.
   */
  uint8 is_fe200_fabric;
  /*
   *  Fixed/variable size cells, enable/disable packet
   *  segmentation.
   */
  SOC_PETRA_FABRIC_CELL_FORMAT fabric_cell_format;
  /*
   *  Out of total of 15 SerDes quartets, two (one per
   *  internal NIF Group consisting of 4 MAL-s) may be
   *  assigned to either Network or Fabric interfaces.
   */
  uint8 is_combo_nif[SOC_PETRA_COMBO_NOF_QRTTS];
  /*
   *  SerDes Reference Clocks. Three ref-clocks are
   *  configured: 1. Towards NIF 2. Towards Fabric MAC3.
   *  Shared (Combo, either to NIF or MAC) - either to NIF or
   *  MAC value, according to the shared_quartet
   *  configuration.
   */
  SOC_PETRA_MGMT_SRD_REF_CLKS ref_clocks_conf;
  /*
   *  Enable/disable statistics tag.
   */
  uint8 stag_enable;
  /*
   *  This configuration affects the maximal number of Egress
   *  MC groups that can be opened. Can only be enabled (TRUE)
   *  when working with SOC_SAND_FE600 (not SOC_SAND_FE200 or mesh).
   *  If TRUE, up to 16K Egress MC groups can be opened. Otherwise - up to 8K.
   *  The configuration must be consistant with the SOC_SAND_FE600 device configuration.
   *  Note! If enabled, the FAP-IDs range in the system is limited to 0 - 511.
   */
  uint8 egr_mc_16k_groups_enable;
  /*
   *  TDM traffic mode and FTMH format configuration.
   */
  SOC_PETRA_MGMT_TDM_MODE tdm_mode;

} SOC_PETRA_MGMT_OPERATION_MODE;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  If TRUE, the appropriate MAC lane will be taken out of
   *  reset. If FALSE - no configuration is applied.
   */
  uint8 mal[SOC_PETRA_NOF_MAC_LANES];
} SOC_PETRA_INIT_OOR;

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
*     soc_petra_register_device
* TYPE:
*   PROC
* FUNCTION:
*     This procedure registers a new device to be taken care
*     of by this device driver. Physical device must be
*     accessible by CPU when this call is made..
* INPUT:
*  SOC_SAND_IN  uint32                  *base_address -
*     Base address of direct access memory assigned for
*     device's registers. This parameter needs to be specified
*     even if physical access to device is not by direct
*     access memory since all logic, within driver, up to
*     actual physical access, assumes 'virtual' direct access
*     memory. Memory block assigned by this pointer must not
*     overlap other memory blocks in user's system and
*     certainly not memory blocks assigned to other SOC_PETRA
*     devices using this procedure.
*  SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr -
*     BSP-function for device reset. Refer to
*     'SOC_SAND_RESET_DEVICE_FUNC_PTR' definition.
*  SOC_SAND_OUT uint32                 *unit_ptr -
*     This procedure loads pointed memory with identifier of
*     newly added device. This identifier is to be used by the
*     caller for further accesses to this device..
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_register_device(
             uint32                  *base_address,
    SOC_SAND_IN  SOC_SAND_RESET_DEVICE_FUNC_PTR reset_device_ptr,
    SOC_SAND_OUT int                 *unit_ptr
  );

/*********************************************************************
* NAME:
*     soc_petra_unregister_device
* TYPE:
*   PROC
* FUNCTION:
*     Undo soc_petra_register_device()
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     The device ID to be unregistered.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_unregister_device(
    SOC_SAND_IN  int                 unit
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_operation_mode_set
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode -
*     Soc_petra device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_operation_mode_get
* TYPE:
*   PROC
* FUNCTION:
*     Set soc_petra device operation mode. This defines
*     configurations such as support for certain header types
*     etc.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *op_mode -
*     Soc_petra device operation mode.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_operation_mode_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *op_mode
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_credit_worth_set
* TYPE:
*   PROC
* FUNCTION:
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                  credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_credit_worth_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                  credit_worth
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_credit_worth_get
* TYPE:
*   PROC
* FUNCTION:
*     Bytes-worth of a single credit. It should be configured
*     the same in all the FAPs in the systems, and should be
*     set before programming the scheduler.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint32                  *credit_worth -
*     Credit worth value. Range: 1 - 8K-1. Unit: bytes.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
int
  soc_petra_mgmt_credit_worth_get_dispatch(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32             *credit_worth
  );

uint32
  soc_petra_mgmt_credit_worth_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                  *credit_worth
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_init_sequence_phase1
* TYPE:
*   PROC
* FUNCTION:
*     Initialize the device, including:1. Prevent all the
*     control cells. 2. Initialize the device tables and
*     registers to default values. 3. Initialize
*     board-specific hardware interfaces according to
*     configurable information, as passed in 'hw_adjust'. 4.
*     Perform basic device initialization. The configuration
*     can be enabled/disabled as passed in 'enable_info'.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_INOUT  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust -
*     Contains user-defined initialization information for
*     hardware interfaces.
*  SOC_SAND_IN  SOC_PETRA_INIT_BASIC_CONF     *basic_conf -
*     Basic configuration that must be configured for all
*     systems - credit worth, dram buffers configuration etc.
*  SOC_SAND_IN  SOC_PETRA_INIT_PORTS          *fap_ports -
*     local FAP ports configuration - header parsing type,
*     mapping to NIF etc. scheduler etc.
*  SOC_SAND_IN  uint8                 silent -
*     If TRUE, progress printing will be suppressed.
* REMARKS:
*     1. For all configurations that can be done per-direction
*     (e.g. NIF - rx/tx, FAP - incoming/outgoing) - the
*     configuration is performed for both directions if
*     enabled. It may be overridden before phase2 if needed.
*     2. For all input structures, NULL pointer may be passed.
*     If input structure is passed as NULL, the appropriate
*     configuration will not be performed.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_init_sequence_phase1(
    SOC_SAND_IN     int                 unit,
    SOC_SAND_INOUT  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN     SOC_PETRA_INIT_BASIC_CONF     *basic_conf,
    SOC_SAND_IN     SOC_PETRA_INIT_PORTS          *fap_ports,
    SOC_SAND_IN     uint8                 silent
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_init_sequence_phase2
* TYPE:
*   PROC
* FUNCTION:
*     Out-of-reset sequence. Enable/Disable the device from
*     receiving and transmitting control cells.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_INIT_OOR            *oor_info -
*     Out Of Reset configuration. Some blocks need to be set
*     out of reset before traffic can be enabled.
*  SOC_SAND_IN  uint8                 silent -
*     TRUE - Print progress messages. FALSE - Do not print
*     progress messages.
* REMARKS:
*     1. After phase 2 initialization, traffic can be enabled.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_init_sequence_phase2(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_INIT_OOR            *oor_info,
    SOC_SAND_IN  uint8                 silent
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_system_fap_id_set
* TYPE:
*   PROC
* FUNCTION:
*     Set the fabric system ID of the device. Must be unique
*     in the system.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint32                 sys_fap_id -
*     The system ID of the device (Unique in the system).
 *     Range: 0 - 2047.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint32                 sys_fap_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_system_fap_id_get
* TYPE:
*   PROC
* FUNCTION:
*     Set the fabric system ID of the device. Must be unique
*     in the system.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint32                 *sys_fap_id -
*     The system ID of the device (Unique in the system).
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_system_fap_id_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint32                 *sys_fap_id
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_hw_interfaces_set
* TYPE:
*   PROC
* FUNCTION:
*     Initialize a sub-set of the HW interfaces of the device.
*     The function might be called more than once, each time
*     with different fields, indicated to be written to the
*     device
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust -
*     BSP related information, and a valid bit for each field.
*  SOC_SAND_IN  uint8                 silent -
*     If TRUE, all printing will be suppressed.
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_hw_interfaces_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_HW_ADJUSTMENTS      *hw_adjust,
    SOC_SAND_IN  uint8                 silent
  );

uint32
  soc_petra_mgmt_all_ctrl_cells_enable_get(
    SOC_SAND_IN  int  unit,
    SOC_SAND_OUT uint8  *enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_all_ctrl_cells_enable_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting control cells.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     SOC_SAND_IN uint8 enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_all_ctrl_cells_enable_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_enable_traffic_set
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  uint8                 enable -
*     SOC_SAND_IN uint8 enable
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  uint8                 enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_enable_traffic_get
* TYPE:
*   PROC
* DATE:
*   Apr 21 2008
* FUNCTION:
*     Enable / Disable the device from receiving and
*     transmitting traffic.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_OUT uint8                 *enable -
*     Enable/disable indication
* REMARKS:
*     None.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_enable_traffic_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_OUT uint8                 *enable
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_pckt_size_range_set
* TYPE:
*   PROC
* FUNCTION:
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
*     External mode filters the packets according to there
*     original size. Internal mode filters the packets
*     according to their size inside the device, after ingress
*     editing.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range -
*     the minimal and maximal packet size boundaries.
* REMARKS:
 *   1. The allowed range depends on the Fabric Cell mode.
 *   For Fixed-Size cells, the range is 33 - 16K-128. For
 *   Variable Size cells, the range is 64 - 16K-128. 2. For
 *   external size limitation (before ingress editing),
 *   setting minimal or maximal size to '0' disables
 *   filtering according to the external (original) packet
 *   size. 3. For external size limitation, the API assumes
 *   Network CRC of 4 bytes. If 3-Bytes CRC is used (optional
 *   for a SPAUI interface), add '1' to the min/max
 *   boundaries of the external packet size. 4. In Soc_petra-B, a
 *   configuration of the maximum packet size can be done per
 *   port via the soc_pb_mgmt_max_pckt_size_set() API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_set(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  );

/*********************************************************************
* NAME:
*     soc_petra_mgmt_pckt_size_range_get
* TYPE:
*   PROC
* FUNCTION:
*     Set the boundaries (minimal/maximal allowed size) for
*     the expected packets. The limitation can be performed
*     based on the packet size before or after the ingress
*     editing (external and internal configuration mode,
*     accordingly). Packets outside the specified range are
*     dropped.
* INPUT:
*  SOC_SAND_IN  int                 unit -
*     Identifier of the device to access.
*  SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx -
*     External mode filters the packets according to there
*     original size. Internal mode filters the packets
*     according to their size inside the device, after ingress
*     editing.
*  SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range -
*     the minimal and maximal packet size boundaries.
* REMARKS:
 *   1. The allowed range depends on the Fabric Cell mode.
 *   For Fixed-Size cells, the range is 33 - 16K-128. For
 *   Variable Size cells, the range is 64 - 16K-128. 2. For
 *   external size limitation (before ingress editing),
 *   setting minimal or maximal size to '0' disables
 *   filtering according to the external (original) packet
 *   size. 3. For external size limitation, the API assumes
 *   Network CRC of 4 bytes. If 3-Bytes CRC is used (optional
 *   for a SPAUI interface), add '1' to the min/max
 *   boundaries of the external packet size. 4. In Soc_petra-B, a
 *   configuration of the maximum packet size can be done per
 *   port via the soc_pb_mgmt_max_pckt_size_set() API.
* RETURNS:
*     OK or ERROR indication.
*********************************************************************/

uint32
  soc_petra_mgmt_pckt_size_range_get(
    SOC_SAND_IN  int                 unit,
    SOC_SAND_IN  SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE conf_mode_ndx,
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE      *size_range
  );

/*********************************************************************
* NAME:
 *   soc_petra_mgmt_core_frequency_get
 * TYPE:
 *   PROC
 * FUNCTION:
 *   Get the core clock frequency of the device.
 * INPUT:
 *   SOC_SAND_IN  int                                 unit -
 *     Identifier of the device to access.
 *   SOC_SAND_OUT uint32                                  *clk_freq -
 *     Device core clock frequency. Units: MHz. For Soc_petra-A,
 *     Range: 150 - 250. For Soc_petra-B, Range: 150 - 300.
 * REMARKS:
 *   None.
 * RETURNS:
 *   OK or ERROR indication.
*********************************************************************/
uint32
  soc_petra_mgmt_core_frequency_get(
    SOC_SAND_IN  int                                 unit,
    SOC_SAND_OUT uint32                                 *clk_freq
  );

uint32
  soc_petra_mgmt_init_sequence_fixes_apply(
    SOC_SAND_IN int unit
  );
void
  soc_petra_PETRA_REVISION_CTRL_REV_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_REVISION_CTRL_REV_INFO *info
  );

void
  soc_petra_PETRA_REVISION_CTRL_clear(
    SOC_SAND_OUT SOC_PETRA_REVISION_CTRL *info
  );

void
  soc_petra_PETRA_HW_PLL_PARAMS_clear(
    SOC_SAND_OUT SOC_PETRA_HW_PLL_PARAMS *info
  );

void
  soc_petra_PETRA_HW_ADJ_DDR_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_DDR *info
  );

void
  soc_petra_PETRA_HW_ADJ_STAT_IF_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_STAT_IF *info
  );

void
  soc_petra_PETRA_HW_ADJ_QDR_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_QDR *info
  );

void
  soc_petra_PETRA_HW_ADJ_SERDES_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_SERDES *info
  );

void
  soc_petra_PETRA_HW_ADJ_FABRIC_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_FABRIC *info
  );

void
  soc_petra_PETRA_HW_ADJ_MAL_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_MAL *info
  );

void
  soc_petra_PETRA_HW_ADJ_SPAUI_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_SPAUI *info
  );

void
  soc_petra_PETRA_HW_ADJ_SGMII_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_SGMII *info
  );

void
  soc_petra_PETRA_HW_ADJ_FAT_PIPE_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_FAT_PIPE *info
  );

void
  soc_petra_PETRA_HW_ADJ_NIF_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_NIF *info
  );

void
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJ_CORE_FREQ *info
  );

void
  soc_petra_PETRA_HW_ADJUSTMENTS_clear(
    SOC_SAND_OUT SOC_PETRA_HW_ADJUSTMENTS *info
  );

void
  soc_petra_PETRA_INIT_STAG_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_STAG_INFO *info
  );

void
  soc_petra_PETRA_INIT_BASIC_CONF_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_BASIC_CONF *info
  );

void
  soc_petra_PETRA_INIT_PORT_HDR_TYPE_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PORT_HDR_TYPE *info
  );

void
  soc_petra_PETRA_INIT_PORT_TO_IF_MAP_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PORT_TO_IF_MAP *info
  );

void
  soc_petra_PETRA_INIT_PCKT_HDR_INFO_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PCKT_HDR_INFO *info
  );

void
  soc_petra_PETRA_INIT_PORTS_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_PORTS *info
  );

void
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_clear(
    SOC_SAND_OUT SOC_PETRA_MGMT_SRD_REF_CLKS *info
  );

void
  soc_petra_PETRA_MGMT_PCKT_SIZE_clear(
    SOC_SAND_OUT SOC_PETRA_MGMT_PCKT_SIZE *info
  );

void
  soc_petra_PETRA_MGMT_OPERATION_MODE_clear(
    SOC_SAND_OUT SOC_PETRA_MGMT_OPERATION_MODE *info
  );

void
  soc_petra_PETRA_INIT_OOR_clear(
    SOC_SAND_OUT SOC_PETRA_INIT_OOR *info
  );

#if SOC_PETRA_DEBUG_IS_LVL1

const char*
  soc_petra_PETRA_REVISION_ID_to_string(
    SOC_SAND_IN SOC_PETRA_REVISION_ID enum_val
  );

const char*
  soc_petra_PETRA_REVISION_SUB_TYPE_ID_to_string(
    SOC_SAND_IN SOC_PETRA_REVISION_SUB_TYPE_ID enum_val
  );

const char*
  soc_petra_PETRA_HW_QDR_PROTECT_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_HW_QDR_PROTECT_TYPE enum_val
  );

const char*
  soc_petra_PETRA_HW_QDR_SIZE_MBIT_to_string(
    SOC_SAND_IN SOC_PETRA_HW_QDR_SIZE_MBIT enum_val
  );

const char*
  soc_petra_PETRA_MGMT_FABRIC_HDR_TYPE_to_string(
    SOC_SAND_IN SOC_PETRA_MGMT_FABRIC_HDR_TYPE enum_val
  );

const char*
  soc_petra_PETRA_MGMT_SRD_REF_CLK_to_string(
    SOC_SAND_IN SOC_PETRA_MGMT_SRD_REF_CLK enum_val
  );

const char*
  soc_petra_PETRA_MGMT_PCKT_SIZE_CONF_MODE_to_string(
    SOC_SAND_IN SOC_PETRA_MGMT_PCKT_SIZE_CONF_MODE enum_val
  );

const char*
  soc_petra_PETRA_MGMT_TDM_MODE_to_string(
    SOC_SAND_IN  SOC_PETRA_MGMT_TDM_MODE enum_val
  );

void
  soc_petra_PETRA_REVISION_CTRL_REV_INFO_print(
    SOC_SAND_IN SOC_PETRA_REVISION_CTRL_REV_INFO *info
  );

void
  soc_petra_PETRA_REVISION_CTRL_print(
    SOC_SAND_IN SOC_PETRA_REVISION_CTRL *info
  );

void
  soc_petra_PETRA_HW_PLL_PARAMS_print(
    SOC_SAND_IN SOC_PETRA_HW_PLL_PARAMS *info
  );

void
  soc_petra_PETRA_HW_ADJ_DDR_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_DDR *info
  );

void
  soc_petra_PETRA_HW_ADJ_STAT_IF_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_STAT_IF *info
  );

void
  soc_petra_PETRA_HW_ADJ_QDR_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_QDR *info
  );

void
  soc_petra_PETRA_HW_ADJ_SERDES_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SERDES *info
  );

void
  soc_petra_PETRA_HW_ADJ_FABRIC_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_FABRIC *info
  );

void
  soc_petra_PETRA_HW_ADJ_MAL_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_MAL *info
  );

void
  soc_petra_PETRA_HW_ADJ_SPAUI_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SPAUI *info
  );

void
  soc_petra_PETRA_HW_ADJ_SGMII_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_SGMII *info
  );

void
  soc_petra_PETRA_HW_ADJ_FAT_PIPE_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_FAT_PIPE *info
  );

void
  soc_petra_PETRA_HW_ADJ_NIF_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJ_NIF *info
  );

void
  soc_petra_PETRA_HW_ADJ_CORE_FREQ_print(
    SOC_SAND_IN  SOC_PETRA_HW_ADJ_CORE_FREQ *info
  );

void
  soc_petra_PETRA_HW_ADJUSTMENTS_print(
    SOC_SAND_IN SOC_PETRA_HW_ADJUSTMENTS *info
  );

void
  soc_petra_PETRA_INIT_STAG_INFO_print(
    SOC_SAND_IN SOC_PETRA_INIT_STAG_INFO *info
  );

void
  soc_petra_PETRA_INIT_BASIC_CONF_print(
    SOC_SAND_IN SOC_PETRA_INIT_BASIC_CONF *info
  );

void
  soc_petra_PETRA_INIT_PORT_HDR_TYPE_print(
    SOC_SAND_IN SOC_PETRA_INIT_PORT_HDR_TYPE *info
  );

void
  soc_petra_PETRA_INIT_PORT_TO_IF_MAP_print(
    SOC_SAND_IN SOC_PETRA_INIT_PORT_TO_IF_MAP *info
  );

void
  soc_petra_PETRA_INIT_PCKT_HDR_INFO_print(
    SOC_SAND_IN SOC_PETRA_INIT_PCKT_HDR_INFO *info
  );

void
  soc_petra_PETRA_INIT_PORTS_print(
    SOC_SAND_IN SOC_PETRA_INIT_PORTS *info
  );

void
  soc_petra_PETRA_MGMT_SRD_REF_CLKS_print(
    SOC_SAND_IN SOC_PETRA_MGMT_SRD_REF_CLKS *info
  );

void
  soc_petra_PETRA_MGMT_PCKT_SIZE_print(
    SOC_SAND_IN SOC_PETRA_MGMT_PCKT_SIZE *info
  );

void
  soc_petra_PETRA_MGMT_OPERATION_MODE_print(
    SOC_SAND_IN SOC_PETRA_MGMT_OPERATION_MODE *info
  );

void
  soc_petra_PETRA_INIT_OOR_print(
    SOC_SAND_IN SOC_PETRA_INIT_OOR *info
  );

#endif /* SOC_PETRA_DEBUG_IS_LVL1 */

/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PETRA_API_MGMT_INCLUDED__*/
#endif
