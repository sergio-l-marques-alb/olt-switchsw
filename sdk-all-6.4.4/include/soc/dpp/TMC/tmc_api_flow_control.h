/*
 * $Id: tmc_api_flow_control.h,v 1.19 Broadcom SDK $
 * $Copyright: Copyright 2012 Broadcom Corporation.
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

#ifndef __SOC_TMC_API_FLOW_CONTROL_INCLUDED__
/* { */
#define __SOC_TMC_API_FLOW_CONTROL_INCLUDED__


/*************
 * INCLUDES  *
 *************/
/* { */

#include <soc/dpp/SAND/Utils/sand_header.h>
#include <soc/dpp/SAND/Management/sand_general_macros.h>

#include <soc/dpp/TMC/tmc_api_general.h>

/* } */
/*************
 * DEFINES   *
 *************/
/* { */

/*
 * Calendar Interface ID
 *   Arad: Interlaken(0-1) OOB(0-1)
 *   Jer:  Interlaken(0-6) OOB(0-1)
 *
 */
typedef uint32 SOC_TMC_FC_CAL_IF_ID;

#define SOC_TMC_FC_NOF_PP_INTF     2

#define SOC_TMC_FC_OOB_CAL_LEN_MAX 512

#define SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE 256

#define SOC_TMC_FC_HCFC_BITMAP_SIZE 512

#define SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN 8

#define  SOC_TMC_FC_OOB_CAL_REP_MIN 1
#define  SOC_TMC_FC_OOB_CAL_REP_MAX 15

#define  SOC_TMC_FC_HCFC_CHANNEL_NUM 5

/* 2^13 */
#define  SOC_TMC_FC_CAL_PAUSE_RESOLUTION_CLK_MAX    0x2000

typedef enum  
{
  SOC_TMC_FC_OOB_TX_SPEED_CORE_2 = 0,
    
  SOC_TMC_FC_OOB_TX_SPEED_CORE_4 = 1,
  
  SOC_TMC_FC_OOB_TX_SPEED_CORE_8 = 2,
  
  SOC_TMC_FC_NOF_OOB_TX_SPEED = 3
}SOC_TMC_FC_OOB_TX_SPEED;

typedef enum
{
    SOC_TMC_FC_EN = 0,

    SOC_TMC_FC_EGQ_TO_NIF_CNM_LLFC_EN = 1, 

    SOC_TMC_FC_EGQ_TO_NIF_CNM_PFC_EN = 2, 

    SOC_TMC_FC_EGQ_TO_SCH_DEVICE_EN = 3,

    SOC_TMC_FC_EGQ_TO_SCH_ERP_EN = 4, 
    
    SOC_TMC_FC_EGQ_TO_SCH_ERP_TC_EN = 5, 

    SOC_TMC_FC_EGQ_TO_SCH_IF_EN = 6,
    
    SOC_TMC_FC_EGQ_TO_SCH_PFC_EN = 7,
    
    SOC_TMC_FC_GLB_RSC_TO_EGQ_RCL_PFC_EN = 8, 

    SOC_TMC_FC_GLB_RSC_TO_HCFC_HP_CFG = 9, 

    SOC_TMC_FC_GLB_RSC_TO_HCFC_LP_CFG = 10, 

    SOC_TMC_FC_GLB_RSC_TO_NIF_LLFC_EN = 11, 
    
    SOC_TMC_FC_GLB_RSC_TO_NIF_PFC_EN = 12, 

    SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_HP_CFG = 13, 

    SOC_TMC_FC_GLB_RSC_TO_RCL_PFC_LP_CFG = 14, 

    SOC_TMC_FC_ILKN_RX_TO_EGQ_PFC_EN = 15, 

    SOC_TMC_FC_ILKN_RX_TO_EGQ_PORT_EN = 16, 
    
    SOC_TMC_FC_ILKN_RX_TO_GEN_PFC_EN = 17, 

    SOC_TMC_FC_ILKN_RX_TO_NIF_FAST_LLFC_EN = 18, 

    SOC_TMC_FC_ILKN_RX_TO_RET_REQ_EN = 19, 

    SOC_TMC_FC_LLFC_VSQ_TO_NIF_EN = 20, 
    
    SOC_TMC_FC_NIF_TO_GEN_PFC_EN = 21, 

    SOC_TMC_FC_PFC_VSQ_TO_NIF_EN = 22, 

    SOC_TMC_FC_SCH_OOB_RX_EN = 23, 

    SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PFC_EN = 24, 

    SOC_TMC_FC_SPI_OOB_RX_TO_EGQ_PORT_EN = 25, 

    SOC_TMC_FC_SPI_OOB_RX_TO_GEN_PFC_EN = 26, 

    SOC_TMC_FC_SPI_OOB_RX_TO_NIF_FAST_LLFC_EN = 27, 

    SOC_TMC_FC_SPI_OOB_RX_TO_RET_REQ_EN = 28, 

    SOC_TMC_FC_STAT_VSQ_TO_HCFC_EN = 29,

    SOC_TMC_FC_NOF_ENABLEs = 30
}SOC_TMC_FC_ENABLE;

typedef enum
{
  /*
   *  Out Of Band Flow Control Interface A. Used for Flow
   *  Control reception or generation.
   */
  SOC_TMC_FC_OOB_ID_A = 0,
  /*
   *  Out Of Band Flow Control Interface B. Used for Flow
   *  Control reception or generation.
   */
  SOC_TMC_FC_OOB_ID_B = 1,
  /*
   *  Number of types in SOC_TMC_FC_OOB_ID
   */
  SOC_TMC_FC_NOF_OOB_IDS = 2
}SOC_TMC_FC_OOB_ID;

typedef enum
{
  /*
   *  Calendar-based Flow Control Mode: SPI-like Out-Of-Band
   */
  SOC_TMC_FC_CAL_MODE_SPI_OOB = 0,
  /*
   *  Calendar-based Flow Control Mode: Interlaken Inband
   */
  SOC_TMC_FC_CAL_MODE_ILKN_INBND = 1,
  /*
   *  Calendar-based Flow Control Mode: Interlaken Out-Of-Band
   */
  SOC_TMC_FC_CAL_MODE_ILKN_OOB = 2,
  /*
   *  Number of types in SOC_TMC_FC_CAL_MODE
   */
  SOC_TMC_FC_NOF_CAL_MODES = 3
}SOC_TMC_FC_CAL_MODE;

typedef enum
{

  SOC_TMC_FC_CAL_TYPE_NONE = 0,
  SOC_TMC_FC_CAL_TYPE_SPI  = 1,
  SOC_TMC_FC_CAL_TYPE_ILKN = 2,
  SOC_TMC_FC_CAL_TYPE_HCFC = 3,
  SOC_TMC_FC_CAL_TYPE_COE  = 4,
  SOC_TMC_FC_CAL_TYPE_E2E  = 5,
  SOC_TMC_FC_NOF_TYPES
}SOC_TMC_FC_OOB_TYPE;

typedef enum
{
  SOC_TMC_FC_CAL_INB_TYPE_NONE = 0,
  SOC_TMC_FC_CAL_INB_TYPE_ILKN = 1,
  SOC_TMC_FC_CAL_INB_TYPE_COE  = 2,
  SOC_TMC_FC_CAL_INB_TYPE_E2E  = 3,
  SOC_TMC_FC_CAL_INB_NOF_TYPES
}SOC_TMC_FC_CAL_INB_TYPE;

typedef enum
{
  SOC_TMC_FC_COE_MODE_PAUSE  = 0,
  SOC_TMC_FC_COE_MODE_PFC = 1,
  SOC_TMC_FC_NOF_COE_MODES = 2
}SOC_TMC_FC_COE_MODE;

typedef enum
{
  SOC_TMC_FC_E2E_STATUS_SIZE_8B  = 0,
  SOC_TMC_FC_E2E_STATUS_SIZE_16B = 1,
  SOC_TMC_FC_E2E_STATUS_SIZE_24B = 2,
  SOC_TMC_FC_E2E_STATUS_SIZE_32B = 3,
  SOC_TMC_FC_E2E_STATUS_SIZE_48B = 4,
  SOC_TMC_FC_E2E_STATUS_SIZE_64B = 5,
  SOC_TMC_FC_NOF_E2E_STATUS_SIZE
}SOC_TMC_FC_E2E_STATUS_TYPE;

typedef enum
{
  /*
   *  Disable Ingress Flow Control generation via NIF, upon
   *  Global Resources High Priority FC indication.
   */
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_NONE = 0,
  /*
   *  Enable Ingress Flow Control generation via NIF, upon
   *  Global Resources High Priority FC indication, Link
   *  Level.
   */
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_LL = 1,
  /*
   *  Enable Ingress Flow Control generation via NIF, upon
   *  Global Resources High Priority FC indication, Class
   *  Based.
   */
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_CB = 2,

  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_PFC = 3,

  /* All the GLB HP options: LL+CB, LL+PFC */
  SOC_TMC_FC_INGR_GEN_GLB_HP_MODE_ALL = 4,
  /*
   *  Number of types in SOC_TMC_FC_INGR_GEN_GLB_HP_MODE
   */
  SOC_TMC_FC_NOF_INGR_GEN_GLB_HP_MODES
}SOC_TMC_FC_INGR_GEN_GLB_HP_MODE;

typedef enum
{
  /*
   *  CBFC Inheritance is disabled. Upon receiving
   *  (generating) FC of a certain class, only this class is
   *  handled
   */
  SOC_TMC_FC_INBND_PFC_INHERIT_DISABLED = 0,
  /*
   *  If TRUE, CBFC classes will affect lower priority
   *  classes, where the highest priority is class 0
   */
  SOC_TMC_FC_INBND_PFC_INHERIT_UP = 1,
  /*
   *  If TRUE, CBFC classes will affect lower priority
   *  classes, where the highest priority is class 7
   */
  SOC_TMC_FC_INBND_PFC_INHERIT_DOWN = 2,
  /*
   *  Number of types in SOC_TMC_FC_INBND_CB_INHERIT
   */
  SOC_TMC_FC_NOF_INBND_PFC_INHERITS = 3
}SOC_TMC_FC_INBND_PFC_INHERIT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Selects whether, and in which way, the CBFC affects
   *  lower priority classes
   */
  SOC_TMC_FC_INBND_PFC_INHERIT inherit;
  /*
   *  If bit 'b' is set, the appropriate HR in range 128-255
   *  will stop credit generation. For XAUI/SPAUI - 8 HRs can
   *  be set as a reaction point for 8 FC classes,
   *  accordingly. For GMII - 2 HRs (2 FC classes)The index of
   *  the HR to stop, for FC-Class K, is:For XAUI/SPAUI
   *  (NIF-ID = MAL-ID*4), 128 + 8*MAL-ID + K. For GMII, 128 +
   *  2 * NIF-ID + K
   */
  uint8 sch_hr_bitmap;

} SOC_TMC_FC_REC_INBND_PFC;

typedef enum
{
  /*
   *  Flow Control Reception Reaction Point on OFP level -
   *  Scheduler Port. Note 1: this option is not recommended
   *  to stop the OFP! Typically, the OFP is stopped in the
   *  EGQ, which then backpressures the scheduler according to
   *  the EGQ FC threshold configuration. Note 2: this option
   *  can be used to stop HR 0 - 79, High or Low priority FC,
   *  also if the matching OFP is not used, and the HR
   *  scheduler is used inside the scheduling hierarchy.
   */
  SOC_TMC_FC_REC_OFP_RP_SCH = 0x1,
  /*
   *  Flow Control Reception Reaction Point on OFP level -
   *  Egress Queues Manager (EGQ)
   */
  SOC_TMC_FC_REC_OFP_RP_EGQ = 0x2,
  /*
   *  Number of types in SOC_TMC_FC_REC_OFP_RP
   */
  SOC_TMC_FC_NOF_REC_OFP_RPS = 3
}SOC_TMC_FC_REC_OFP_RP;

typedef enum
{
  /*
   *  OFP FC priority - No FC
   */
  SOC_TMC_FC_OFP_PRIORITY_NONE = 0x0,
  /*
   *  OFP FC priority - Low
   */
  SOC_TMC_FC_OFP_PRIORITY_LP = 0x1,
  /*
   *  OFP FC priority - High. HP-FC triggers also LP-FC
   */
  SOC_TMC_FC_OFP_PRIORITY_HP = 0x3,
  /*
   *  Number of types in SOC_TMC_FC_OFP_PRIORITY
   */
  SOC_TMC_FC_NOF_OFP_PRIORITYS = 3
}SOC_TMC_FC_OFP_PRIORITY;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Flow Control Reception Reaction Point on OFP level -
   *  Scheduler/EGQ
   */
  SOC_TMC_FC_REC_OFP_RP react_point;
  /*
   *  OFP index. Range: 0 - 79.
   */
  uint32 ofp_ndx;
  /*
   *  FC Priority: high/low
   */
  SOC_TMC_FC_OFP_PRIORITY priority;

} SOC_TMC_FC_REC_OFP_MAP_INFO;

typedef enum
{
  /*
   *  Inband Flow Control disabled in the specified direction
   *  (generation/reception)
   */
  SOC_TMC_FC_INBND_MODE_DISABLED = 0,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_INBND_MODE_LL = 1,
  /*
   *  Class-based Flow Control. The number of Flow Control
   *  Classes depends on the NIF type: 2 classes for 1Gbps
   *  interface (SGMII), 8 classes for 10Gbps interfaces
   *  (XAUI/SPAUI)
   */
  SOC_TMC_FC_INBND_MODE_CB = 2,
  /*
   *  Number of types in SOC_TMC_FC_INBND_MODE - petra B
   */
  SOC_TMC_FC_NOF_INBND_MODES_PB = 3,
   /* ARAD ONLY */
   SOC_TMC_FC_INBND_MODE_PFC = SOC_TMC_FC_NOF_INBND_MODES_PB,
   
   SOC_TMC_FC_INBND_MODE_SAFC = 4,

   SOC_TMC_FC_INBND_MODE_DEVICE_DISABLED = 5,

   SOC_TMC_FC_NOF_INBND_MODES = 6
}SOC_TMC_FC_INBND_MODE;

typedef enum
{
  /*
   *  Inband Flow Control disabled in the specified direction
   *  (generation/reception)
   */
  SOC_TMC_FC_GEN_SRC_NONE = 0,
  /*
   *  FC Generation Source: VSQ LLFC
   */
  SOC_TMC_FC_GEN_SRC_VSQ_LLFC = 1,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GEN_SRC_VSQ_PFC = 2,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GEN_SRC_GLB_HIGH = 3,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GEN_SRC_GLB_LOW = 4,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GEN_SRC_VSQ_CAT2TC = 5,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GEN_SRC_NIF = 6,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GEN_SRC_CNM = 7,

  SOC_TMC_FC_NOF_GEN_SRC = 18
}SOC_TMC_FC_GEN_SRC;


typedef enum
{
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_BDB = 0,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_MINI_MC_DB = 1,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_MC_DB = 2,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_OCB_DB = 3,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_MIX_P0 = 4,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_MIX_P1 = 5,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_OCB_P0 = 6,
  /*
   *  Link-level Flow Control
   */
  SOC_TMC_FC_GLB_RES_TYPE_OCB_P1 = 7,  

  SOC_TMC_FC_NOF_GLB_RES_TYPE = 8
}SOC_TMC_FC_GLB_RES_TYPE;

typedef enum
{
  /*
   *  CBFC Inheritance is disabled. Upon receiving
   *  (generating) FC of a certain class, only this class is
   *  handled
   */
  SOC_TMC_FC_INBND_CB_INHERIT_DISABLED = 0,
  /*
   *  If TRUE, CBFC classes will affect lower priority
   *  classes, where the highest priority is class 0
   */
  SOC_TMC_FC_INBND_CB_INHERIT_UP = 1,
  /*
   *  If TRUE, CBFC classes will affect lower priority
   *  classes, where the highest priority is class 7
   */
  SOC_TMC_FC_INBND_CB_INHERIT_DOWN = 2,
  /*
   *  Number of types in SOC_TMC_FC_INBND_CB_INHERIT
   */
  SOC_TMC_FC_NOF_INBND_CB_INHERITS = 3
}SOC_TMC_FC_INBND_CB_INHERIT;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_FC_GEN_SRC gen_src;
  
  /*
   *  Selects whether, and in which way, the CBFC affects
   *  lower priority classes
   */
  SOC_TMC_FC_INBND_PFC_INHERIT inherit;

  /*
   *  If TRUE, PFC is generated based on VSQ PFC. 
   */
  uint8 vsq_pfc_enable;

  /*
   *  Jericho Only.
   */
  uint32 is_ocb_only;


  /*
   *   
   */
  uint32 glbl_rcs_pool;
  
  /*
   *  A bitmap that specifies the classes for CBFC generation.
   *  Controls FC generation upon Global Resources consumption
   *  Low-Level FC. 
   */
  uint32 glbl_rcs_low;

  /*
   *  A bitmap that specifies the classes for CBFC generation.
   *  Controls FC generation upon Global Resources consumption
   *  Low-Level FC. 
   */
  uint32 glbl_rcs_high;

  
  /*
   *  If TRUE, CBFC is generated based on CNM messages. The
   *  relevant NIF will generate FC, based on the 3-LSB of the
   *  CPID. These 3-LSB are expected to represent TC. Note 1: if
   *  port-to-interface mapping for the relevant NIF is
   *  modified, this configuration must be reset after the
   *  modification. Note 2: refer to CNM module APIs for CNM
   *  messages handling configuration
   */
  uint8 cnm_intercept_enable;

  uint32 cnm_pfc_channel;
  /*
   *  8-bits bitmap. If bit 'b' is set, the NIF will generate
   *  FC on class 'b' when lower internal NIF-FC threshold is
   *  crossed. Note: for GMII interface, only bits 0, 1 are
   *  relevant (since only two FC-classes are supported).
   *  Note: when higher internal NIF-FC threshold is crossed,
   *  FC is generated on all classes.
   */
  uint8 nif_cls_bitmap;

  uint8 cat2_tc;

  uint32 cat2_tc_bitmap;  

} SOC_TMC_FC_GEN_INBND_PFC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Enable/Disable the Calendar-based interface for FC
   *  generation (TX) or reception (RX). Can be enabled as
   *  SPI-OOB, ILKN-OOB or ILKN-Inbnd. OOB-TX enable is only
   *  valid for OOB interface B.
   */
  uint8 enable;
  /*
   *  Number of channels in calendar. SPI Range: 0 - 511. ILKN
   *  Range: 0 - 255.
   */
  uint32 cal_len;
  /*
   *  The number of calendar repetitions within a status
   *  frame. The actual Calendar is composed of adjacent
   *  sections of 'cal_len' length.'cal_len' * 'cal_reps' must
   *  not exceed the total calendar length (512/256). Range: 1
   *  - 15.
   */
  uint32 cal_reps;

} SOC_TMC_FC_CAL_IF_INFO;

typedef enum
{
  SOC_TMC_FC_GEN_CAL_SRC_STE = 0,
  /*
   *  Calendar-based Flow Control source - Index Range
   *  (NIF-ID): 0 - 63.
   */
  SOC_TMC_FC_GEN_CAL_SRC_NIF = 1,
  /*
   *  Calendar-based Flow Control source - Global Resource,
   *  High Priority FC. Index Range (Glbl-Rcs-HP-Id): 0 - 2.
   */
  SOC_TMC_FC_GEN_CAL_SRC_GLB_HP = 2,
  /*
   *  Calendar-based Flow Control source - Index Range
   *  (Glbl-Rcs-LP-Id): 0 - 2.
   */
  SOC_TMC_FC_GEN_CAL_SRC_GLB_LP = 3,
  /*
   *  Invalid/non-existing source. Serves as "empty calendar
   *  entry" indication.
   */
  SOC_TMC_FC_GEN_CAL_SRC_NONE = 4,
  /*
   * Number of types in SOC_PB_FC_GEN_CAL_SRC - Petra B
   */
  SOC_TMC_FC_NOF_GEN_CAL_SRCS_PB = 5,

  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_LLFC_VSQ = SOC_TMC_FC_NOF_GEN_CAL_SRCS_PB,

  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_PFC_VSQ,

  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_GLB_RCS,

  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_HCFC,
  
  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_LLFC,

  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_RETRANSMIT,

  /* Arad Only */
  SOC_TMC_FC_GEN_CAL_SRC_CONST,

  /* JERICHO Only */
  SOC_TMC_FC_GEN_CAL_SRC_STTSTCS_TAG,

  /*
   *  Number of types in SOC_TMC_FC_GEN_CAL_SRC
   */
  SOC_TMC_FC_NOF_GEN_CAL_SRCS
}SOC_TMC_FC_GEN_CAL_SRC;

typedef enum
{
  /*
   *  Global Resources Index- Buffer Descriptors
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_BDB = 0,
  /*
   *  Global Resources Index- Unicast Data Buffers
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_UNI = 1,
  /*
   *  Global Resources Index- Multicast Data Buffers
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_MUL = 2,
  /*
   *  Total number of Global Resources Indexes.
   */
  SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS_ARAD = 3,
  /*
   *  Global Resources Index- OCB Data Buffers
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB_DB = SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS_ARAD,
  /*
   *  Global Resources Index- Shared Buffers/BDs resource of Pool-0
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_MIX_P0 = 4,
  /*
   *  Global Resources Index- Shared Buffers/BDs resource of Pool-1
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_MIX_P1 = 5,
  /*
   *  Global Resources Index- Shared OCB-Buffers resource of Pool-0
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB_P0 = 6,
  /*
   *  Global Resources Index- Shared OCB-Buffers resource of Pool-1
   */
  SOC_TMC_FC_CAL_GLB_RCS_ID_OCB_P1 = 7,
  /*
   *  Total number of Global Resources Indexes.
   */
  SOC_TMC_FC_NOF_CAL_GLB_RCS_IDS = 8
}SOC_TMC_FC_CAL_GLB_RCS_ID;

typedef enum
{
  /*
   *  Calendar-based Flow Control destination (Reaction Point)
   *  - HR scheduling element. Index Range (HR-SE-ID): 128 -
   *  255.
   */
  SOC_TMC_FC_REC_CAL_DEST_HR = 0,
  /*
   *  Calendar-based Flow Control destination - Outgoing FAP
   *  Port, Egress (EGQ), Low Priority FC. Index Range
   *  (OFP-ID): 0 - 79.
   */
  SOC_TMC_FC_REC_CAL_DEST_OFP_EGQ_LP = 1,
  /*
   *  Calendar-based Flow Control destination - Outgoing FAP
   *  Port, Egress (EGQ), High Priority FC. Index Range
   *  (OFP-ID): 0 - 79.
   */
  SOC_TMC_FC_REC_CAL_DEST_OFP_EGQ_HP = 2,
  /*
   *  Calendar-based Flow Control destination - Outgoing FAP
   *  Port, HR scheduling element, Low Priority FC. Index
   *  Range (OFP-HR-ID): 0 - 79.
   */
  SOC_TMC_FC_REC_CAL_DEST_OFP_SCH_HR_LP = 3,
  /*
   *  Calendar-based Flow Control destination - Outgoing FAP
   *  Port, HR scheduling element, High FC. Index Range
   *  (OFP-HR-ID): 0 - 79.
   */
  SOC_TMC_FC_REC_CAL_DEST_OFP_SCH_HR_HP = 4,
  /*
   *  Calendar-based Flow Control destination - Network
   *  Interface. Index Range (NIF-ID): 0 - 63.
   */
  SOC_TMC_FC_REC_CAL_DEST_NIF = 5,
  /*
   *  Invalid/non-existing destination. Serves as "empty
   *  calendar entry" indication.
   */
  SOC_TMC_FC_REC_CAL_DEST_NONE = 6,
  /*
   *  Number of types in SOC_TMC_FC_REC_CAL_DEST - petra B
   */
  SOC_TMC_FC_NOF_REC_CAL_DESTS_PB = 7,

  /* Arad Only */
  SOC_TMC_FC_REC_CAL_DEST_PFC = SOC_TMC_FC_NOF_REC_CAL_DESTS_PB,

  /* Arad Only */
  SOC_TMC_FC_REC_CAL_DEST_NIF_LL,

  /* Arad Only */
  SOC_TMC_FC_REC_CAL_DEST_PORT_2_PRIORITY,

  /* Arad Only */
  SOC_TMC_FC_REC_CAL_DEST_PORT_8_PRIORITY,

  /* Arad Only */
  SOC_TMC_FC_REC_CAL_DEST_GENERIC_PFC,

  /* Arad Only */
  SOC_TMC_FC_REC_CAL_DEST_RETRANSMIT,

  SOC_TMC_FC_NOF_REC_CAL_DESTS
}SOC_TMC_FC_REC_CAL_DEST;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Calendar-based Flow Control source (trigger).
   */
  SOC_TMC_FC_GEN_CAL_SRC source;

  uint32 id;

} SOC_TMC_FC_GEN_CALENDAR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Calendar-based Flow Control destination (Reaction
   *  Point).
   */
  SOC_TMC_FC_REC_CAL_DEST destination;

  uint32 id;
  
} SOC_TMC_FC_REC_CALENDAR;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_FC_GEN_SRC gen_src;

  /*
   *  Jericho Only.
   */
  uint32 is_ocb_only;


  /*
   *   
   */
  uint32 glbl_rcs_pool;
  
  /*
   *  If TRUE, LLFC is generated based on CNM messages. Note
   *  1: if port-to-interface mapping for the relevant NIF is
   *  modified, this configuration must be reset after the
   *  modification. Note 2: refer to CNM module APIs for CNM
   *  messages handling configuration
   */
  uint8 cnm_enable;

  /*
   *  If TRUE, LLFC is generated based on global resource. 
   */
  uint8 glbl_rcs_enable;

  /*
   *  If TRUE, LLFC is generated based on VSQ LLFC. 
   */
  uint8 vsq_llfc_enable;

  /*
   *  If TRUE, LLFC is generated based on Almost full NIF indication. 
   */
  uint8 nif_enable;

} SOC_TMC_FC_GEN_INBND_LL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Selects whether, and in which way, the CBFC affects
   *  lower priority classes
   */
  SOC_TMC_FC_INBND_CB_INHERIT inherit;
  /*
   *  If bit 'b' is set, the appropriate HR in range 128-255
   *  will stop credit generation. For XAUI/SPAUI - 8 HRs can
   *  be set as a reaction point for 8 FC classes,
   *  accordingly. For GMII - 2 HRs (2 FC classes)The index of
   *  the HR to stop, for FC-Class K, is:For XAUI/SPAUI
   *  (NIF-ID = MAL-ID*4), 128 + 8*MAL-ID + K. For GMII, 128 +
   *  2 * NIF-ID + K
   */
  uint8 sch_hr_bitmap;

} SOC_TMC_FC_REC_INBND_CB;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Selects whether, and in which way, the CBFC affects
   *  lower priority classes
   */
  SOC_TMC_FC_INBND_CB_INHERIT inherit;
  /*
   *  A bitmap that specifies the classes for CBFC generation.
   *  Controls FC generation upon Global Resources consumption
   *  Low-Level FC. 
   */
  uint32 glbl_rcs_low;
  /*
   *  If TRUE, CBFC is generated based on CNM messages. The
   *  relevant NIF will generate FC, based on the 3-LSB of the
   *  CPID. These 3-LSB are expected to represent TC. Note 1: if
   *  port-to-interface mapping for the relevant NIF is
   *  modified, this configuration must be reset after the
   *  modification. Note 2: refer to CNM module APIs for CNM
   *  messages handling configuration
   */
  uint8 cnm_intercept_enable;
  /*
   *  8-bits bitmap. If bit 'b' is set, the NIF will generate
   *  FC on class 'b' when lower internal NIF-FC threshold is
   *  crossed. Note: for GMII interface, only bits 0, 1 are
   *  relevant (since only two FC-classes are supported).
   *  Note: when higher internal NIF-FC threshold is crossed,
   *  FC is generated on all classes.
   */
  uint8 nif_cls_bitmap;

} SOC_TMC_FC_GEN_INBND_CB;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Disabled/Link-Level/Class-Based (according to the NIF
   *  type)
   */
  SOC_TMC_FC_INBND_MODE mode;
  /*
   *  Only when CBFC mode is selected (ignored otherwise) -
   *  CBFC configuration
   *  Soc_petra only
   */
  SOC_TMC_FC_GEN_INBND_CB cb;
  /*
   *  Only when LLFC mode is selected (ignored otherwise) -
   *  LLFC configuration
   */
  SOC_TMC_FC_GEN_INBND_LL ll;

  /* Arad only */
  SOC_TMC_FC_GEN_INBND_PFC pfc;

  /* Jericho only */
  int core;

} SOC_TMC_FC_GEN_INBND_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  Disabled/Link-Level/Class-Based (according to the NIF
   *  type)
   */
  SOC_TMC_FC_INBND_MODE mode;
  /*
   *  Only when CBFC mode is selected (ignored otherwise) -
   *  CBFC configuration
   */
  SOC_TMC_FC_REC_INBND_CB cb;

  SOC_TMC_FC_REC_INBND_PFC pfc;

} SOC_TMC_FC_REC_INBND_INFO;

typedef struct 
{
  /* Jericho only */
  int core;
  
  /* 256 Bits - representing the 256 queue-pairs at the EGQ.
   * The CFC contains 16 generic bitmaps */
  uint32 bitmap[SOC_TMC_FC_PFC_GENERIC_BITMAP_SIZE / 32];
}SOC_TMC_FC_PFC_GENERIC_BITMAP;

typedef struct 
{
  /* 512 Bits - representing 512 HCFC FCs for Jericho, 256 HCFC FCs for Arad */
  uint32 bitmap[SOC_TMC_FC_HCFC_BITMAP_SIZE / 32];
}SOC_TMC_FC_HCFC_BITMAP;

typedef struct 
{
  /* Enable bitmap defines the set of enables */
  SHR_BITDCL bmp[_SHR_BITDCLSIZE(SOC_TMC_FC_NOF_ENABLEs)]; 
}SOC_TMC_FC_ENABLE_BITMAP;

typedef enum
{
  /*
   *  Mapping to EGQ PFC
   */
  SOC_TMC_FC_PFC_MAP_EGQ = 0,
  /*
   *  Mapping by using Generic PFC Bitmap
   */
  SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP = 1,
  /*
   *  Number of types in SOC_TMC_FC_PFC_MAP_MODE
   */
  SOC_TMC_FC_NOF_PFC_MAP_MODE = 2
}SOC_TMC_FC_PFC_MAP_MODE;

typedef struct 
{
  /* 
   * In case mode is SOC_TMC_FC_PFC_MAP_EGQ, index is dst PFC index;
   * In case mode is SOC_TMC_FC_PFC_MAP_GENERIC_BITMAP, index is generic PFC bitmaps index;
   */
  uint32 index;

  SOC_TMC_FC_PFC_MAP_MODE mode;

  uint32 valid;
  
}SOC_TMC_FC_PFC_MAP;

typedef enum
{
  /*
   *  ILKN Calendar cannot be used to indicate LLFC (RX/TX)
   */
  SOC_TMC_FC_ILKN_CAL_LLFC_NONE = 0,
  /*
   *  ILKN Calendar Channel 0 indicates LLFC (RX/TX)
   */
  SOC_TMC_FC_ILKN_CAL_LLFC_CH_0 = 1,
  /*
   *  ILKN Calendar Channels 16*n (i.e. 0, 16, 32, .., 240)
   *  indicate LLFC (RX/TX)
   */
  SOC_TMC_FC_ILKN_CAL_LLFC_CH_16N = 2,
  /*
   *  Number of types in ARAD_FC_ILKN_CAL_LLFC
   */
  SOC_TMC_FC_NOF_ILKN_CAL_LLFCS = 3
}SOC_TMC_FC_ILKN_CAL_LLFC;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  For RX (Flow Control Reception): if a certain bit in the
   *  multiple use bits is received with a value of XOFF, and
   *  its corresponding bit in the mask is set, this is
   *  interpreted as request for link level flow control on
   *  the ILKN. For TX (Flow Control Generation), this mask
   *  will be sent as LLFC indication when requested. The value
   *  '0' disables Inband-LLFC using multiple-use-bits.
   */
  uint8 multi_use_mask;
  /*
   *  Defines whether the ILKN-FC calendar can be used to
   *  receive/generate LLFC. If it can, defines the calendar
   *  channel/channels used for LLFC indication
   */
  SOC_TMC_FC_ILKN_CAL_LLFC cal_channel;

} SOC_TMC_FC_ILKN_LLFC_INFO;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR

  SOC_TMC_FC_GEN_CALENDAR entries[SOC_TMC_FC_ILKN_MUB_GEN_CAL_LEN];
} SOC_TMC_FC_ILKN_MUB_GEN_CAL;

typedef struct
{
  SOC_SAND_MAGIC_NUM_VAR
  /*
   *  This field consists of the thresholds for setting LLFC or 
   *  clearing LLFC.
   */
  SOC_TMC_THRESH_WITH_HYST_INFO llfc;
  /*
   *  This field consists of the thresholds for setting PFC or 
   *  clearing PFC.
   */
  SOC_TMC_THRESH_WITH_HYST_INFO pfc;

} SOC_TMC_FC_PORT_FIFO_TH;


typedef struct 
{
  /* enable watch dog for HCFC OOB RX0/RX1*/
  uint32 enable;

  /*
   * watch dog period
   * Message interface is considered healthy if once in a configured period of time, 
   * a message was received with no error and correct CRC
   * unit: usec.
   */
  uint32 period;

  /*
   * watchdog error status configuration
   * values:
   *   1 - CFC send XOFF when watch-dog error is reported
   *   0 - CFC send XON when watch-dog error is reported
   */
  uint32 error_status;

}SOC_TMC_FC_HCFC_WATCHDOG;

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
void SOC_TMC_FC_PFC_GENERIC_BITMAP_clear(SOC_SAND_OUT SOC_TMC_FC_PFC_GENERIC_BITMAP *generic_bm);

void SOC_TMC_FC_CAL_IF_INFO_clear(SOC_SAND_OUT SOC_TMC_FC_CAL_IF_INFO *cal_info);

void SOC_TMC_FC_GEN_CALENDAR_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_CALENDAR *cal_info);

void SOC_TMC_FC_GEN_INBND_INFO_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_INFO *info);

void SOC_TMC_FC_GEN_INBND_CB_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_CB *info);

void SOC_TMC_FC_GEN_INBND_LL_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_LL *info);

void SOC_TMC_FC_REC_INBND_CB_clear(SOC_SAND_OUT SOC_TMC_FC_REC_INBND_CB *info);

void SOC_TMC_FC_GEN_INBND_PFC_clear(SOC_SAND_OUT SOC_TMC_FC_GEN_INBND_PFC *info);

void SOC_TMC_FC_REC_CALENDAR_clear(SOC_SAND_OUT SOC_TMC_FC_REC_CALENDAR *info);

void
  SOC_TMC_FC_REC_INBND_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_REC_INBND_INFO *info
  );

void
  SOC_TMC_FC_ILKN_LLFC_INFO_clear(
    SOC_SAND_OUT SOC_TMC_FC_ILKN_LLFC_INFO *info
  );

#include <soc/dpp/SAND/Utils/sand_footer.h>

#endif



