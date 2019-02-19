/* $Id: pb_chip_tbls.h,v 1.7 Broadcom SDK $
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
/******************************************************************
*
* FILENAME:       soc_pb_chip_tbls.h
*
* MODULE PREFIX:  soc_pb_chip_tbls
*
* FILE DESCRIPTION:
*
* REMARKS:
* SW License Agreement: Dune Networks (c). CONFIDENTIAL PROPRIETARY INFORMATION.
* Any use of this Software is subject to Software License Agreement
* included in the Driver User Manual of this device.
* Any use of this Software constitutes an agreement to the terms
* of the above Software License Agreement.
******************************************************************/

#ifndef __SOC_PB_CHIP_TBLS_INCLUDED__
/* { */
#define __SOC_PB_CHIP_TBLS_INCLUDED__

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>

#include <soc/dpp/Petra/petra_chip_defines.h>
#include <soc/dpp/Petra/petra_a_chip_tbls.h>
#include <soc/dpp/Petra/PB_TM/pb_framework.h>

#include <soc/dpp/Petra/petra_api_general.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* block index and mask definitions { */
#define SOC_PB_OLP_MASK     (SOC_PB_OLP_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IRE_MASK     (SOC_PB_IRE_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IDR_MASK     (SOC_PB_IDR_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IRR_MASK     (SOC_PB_IRR_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IHP_MASK     (SOC_PB_IHP_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_QDR_MASK     (SOC_PB_QDR_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_MMU_MASK     (SOC_PB_MMU_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IQM_MASK     (SOC_PB_IQM_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IPS_MASK     (SOC_PB_IPS_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IPT_MASK     (SOC_PB_IPT_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_DPI_A_MASK   (SOC_PB_DPI_A_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_DPI_B_MASK   (SOC_PB_DPI_B_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_DPI_C_MASK   (SOC_PB_DPI_C_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_DPI_D_MASK   (SOC_PB_DPI_D_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_DPI_E_MASK   (SOC_PB_DPI_E_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_DPI_F_MASK   (SOC_PB_DPI_F_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_RTP_MASK     (SOC_PB_RTP_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_EGQ_MASK     (SOC_PB_EGQ_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_IHB_MASK     (SOC_PB_IHB_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_EPNI_MASK    (SOC_PB_EPNI_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_CFC_MASK     (SOC_PB_CFC_ID << SOC_SAND_MODULE_SHIFT)
#define SOC_PB_SCH_MASK     (SOC_PB_SCH_ID << SOC_SAND_MODULE_SHIFT)

#define SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS        (3)
#define SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_TBLS          (3)

#define SOC_PB_IQM_PRFSEL_NOF_TBLS                        (2)
#define SOC_PB_IQM_PRFCFG_NOF_TBLS                        (2)

#define SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS         (2)

#define SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS        (16)

#define SOC_PB_CHIP_REGS_NOF_CNT_PROCESSOR_IDS            (2)

#define SOC_PB_CNM_CP_SAMPLE_BASE_SIZE                    (8)

/* block index and mask definitions } */

/* } */

/*************
 *  MACROS   *
 *************/
/* { */

/* } */

/*************
 * TYPE DEFS *
 *************/
/* { */

/* Blocks definition { */


  /* Nif Ctxt Map: 1024 entries. Each 64 entries represent 64       */
  /* incoming channels per ingress MAC lane (MAC lane 0 uses        */
  /* indexes 0-63, MAC lane 1 uses indexes 64-127, ...). SGMII       */
  /* interfaces use four predetermined addresses (0, 16, 32         */
  /* and 48) per MAC lane. Non-SGMII interfaces can use any         */
  /* channel within the range. Each entry binds the incoming        */
  /* NIF port/channel to a FAP port. Write 7'd127 to an              */
  /* invalid NIF port/channel.                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00004000 */

   /* FapPort: Corresponding Incoming FAP port.                    */
   /* range: 6:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD fap_port;

   /* Parity:                                                      */
   /* range: 8:8, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRE_NIF_CTXT_MAP_TBL;

  /* Nif Port To Ctxt Bit Map: Sixteen entries. Each entry          */
  /* represents, per MAC lane, a bit map of the associated          */
  /* Incoming FAP ports.                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00005000 */

   /* ContextsBitMapping: Bitmap of FAP ports associated with      */
   /* the MAC lane.                                                */
   /* range: 79:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD contexts_bit_mapping;

   /* Parity:                                                      */
   /* range: 80:80, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL;

  /* Rcy Ctxt Map: Egress Recycling channel to incoming FAP         */
  /* Port (Context) mapping. Write 7'd127 to an invalid              */
  /* Recycling channel.                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00006000 */

   /* FapPort: Corresponding Incoming FAP port.                    */
   /* range: 6:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD fap_port;

   /* Parity:                                                      */
   /* range: 8:8, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRE_RCY_CTXT_MAP_TBL;

  /* Tdm Config: TDM configuration for each FAP port                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00007000 */

   /* Mode: Indicates the port's TDM editing functionality:        */
   /* 2'b00 - TDM packets are transferred unmodified 2'b01 -       */
   /* The FTMH header (in this table) is pre-pended to the         */
   /* packet 2'b10 - An optimized FTMH header(in this table)       */
   /* is pre-pended to the packet 2'b11 - PMC mode. The            */
   /* incoming packet has a PMC header.                            */
   /* range: 1:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD mode;

   /* CPU: If set then this FAP port is connected to the CPU.      */
   /* Size limitation on packet is not tested.                     */
   /* range: 4:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD cpu;

   /* Header: The FTMh to pre-pend to the packet, when needed.     */
   /* See mode above.                                              */
   /* range: 71:8, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD header;

   /* Parity:                                                      */
   /* range: 72:72, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRE_TDM_CONFIG_TBL;

  /* Context Mru: Maximum reception size for each reassembly        */
  /* context                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* OrgSize: Size limit before editing (on the wire)             */
   /* range: 13:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD org_size;

   /* Size: Size limit after editing (in DRAM)                     */
   /* range: 29:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD size;

   /* Parity:                                                      */
   /* range: 32:32, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_CONTEXT_MRU_TBL;


  /* Ethernet Meter Profiles: Stores 32 Ethernet meter              */
  /* profiles                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001a0000 */

   /* RateMantissa:                                                */
   /* range: 5:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD rate_mantissa;

   /* RateExp:                                                     */
   /* range: 15:8, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD rate_exp;

   /* BurstMantissa:                                               */
   /* range: 21:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD burst_mantissa;

   /* BurstExp:                                                    */
   /* range: 28:24, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD burst_exp;

   /* PacketMode:                                                  */
   /* range: 32:32, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD packet_mode;

   /* Parity:                                                      */
   /* range: 40:40, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL;


typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001a0000 */
    
   /* pcb pointer:                                                */
   /* range: 8:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD pcb_pointer;

   /* Ecc:                                                     */
   /* range: 15:12, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_PCB_LINK_TBL;



/* Ethernet Meter Status: Stores the Ethernet meters status        */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;     /* 0x001b0000 */

  /* range: 0:0, access type: UNDEF, default value: 0x0           */
  SOC_PETRA_TBL_FIELD enable;
  
  SOC_PETRA_TBL_FIELD time_stamp;
  SOC_PETRA_TBL_FIELD level;
  SOC_PETRA_TBL_FIELD reserved;
  SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_ETHERNET_METER_STATUS_TBL ;

  /* Ethernet Meter Config: Stores the Ethernet meters              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001c0000 */

   /* Profile: Dual Bucket Profile, defines: CIR, EIR, CBS,        */
   /* and EBS                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD profile;

   /* GlobalMeterPtr: Points to one of eight global meters         */
   /* range: 10:8, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD global_meter_ptr;

   /* Parity:                                                      */
   /* range: 12:12, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL;

  /* Global Meter Profiles: Stores 32 Ethernet meter profiles       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001d0000 */

   /* RateMantissa:                                                */
   /* range: 5:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD rate_mantissa;

   /* RateExp:                                                     */
   /* range: 15:8, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD rate_exp;

   /* BurstMantissa:                                               */
   /* range: 21:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD burst_mantissa;

   /* BurstExp:                                                    */
   /* range: 28:24, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD burst_exp;

   /* PacketMode:                                                  */
   /* range: 32:32, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD packet_mode;

   /* Enable:                                                      */
   /* range: 36:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD enable;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL;

  /* Global Meter Status: Stores the Ethernet meters                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001e0000 */

   /* TimeStamp:                                                   */
   /* range: 10:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD time_stamp;

   /* Level:                                                       */
   /* range: 40:12, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD level;

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_GLOBAL_METER_STATUS_TBL;


  /* MCDB-Egress-Format-B: This is Format-B of the multicast        */
  /* database used by the egress. This format supports one          */
  /* OTM-Port and one VLAN pointer.                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* Multicast-Bitmap-Ptr3: OTM-Port. A value of '-1' is          */
   /* interpreted as Null.                                         */
   /* range: 22:11, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD multicast_bitmap_ptr;

   /* CUD3: A configurable value that is used to tag each          */
   /* copy. This is typically used by egress processing logic      */
   /* to uniquely process each copy.                               */
   /* range: 38:23, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD out_lif2;

   /* OTM-Port2: OTM-Port. A value of '-1' is interpreted as       */
   /* Null.                                                        */
   /* range: 45:39, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD out_port;

   /* CUD2: A configurable value that is used to tag each          */
   /* copy. This is typically used by egress processing logic      */
   /* to uniquely process each copy.                               */
   /* range: 61:46, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD out_lif1;

   /* Format-Select: A value of '-1' is interpreted as Null        */
   /* and the table format is A. Values of 0-79 are                */
   /* interpreted as OTM-Port and the table format is A A          */
   /* value of -2 is interpreted as table format B. A value of     */
   /* -3 is interpreted as table format C. Values of 80-124        */
   /* are illegal.                                                 */
   /* range: 68:62, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD format_select;

   /* LinkPtr: Pointer to the next entry in the linked list A      */
   /* value of 0 means the end of the list                         */
   /* range: 86:72, access type: UNDEF, default value: 0x0         */

   SOC_PETRA_TBL_FIELD link_ptr;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL;

  /* MCDB-Egress-Format-C: This is Format-C of the multicast        */
  /* database used by the egress. This format supports two          */
  /* VLAN pointers.                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* Multicast-Bitmap-Ptr3: Pointer to a Multicast-Bitmap         */
   /* range: 17:6, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD multicast_bitmap_ptr2;

   /* CUD3: A configurable value that is used to tag each          */
   /* copy. This is typically used by egress processing logic      */
   /* to uniquely process each copy.                               */
   /* range: 33:18, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD out_lif2;

   /* Multicast-Bitmap-Ptr2: Pointer to a Multicast-Bitmap         */
   /* range: 45:34, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD multicast_bitmap_ptr1;

   /* CUD2: A configurable value that is used to tag each          */
   /* copy. This is typically used by egress processing logic      */
   /* to uniquely process each copy.                               */
   /* range: 61:46, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD out_lif1;

   /* Format-Select: A value of '-1' is interpreted as Null        */
   /* and the table format is A. Values of 0-79 are                */
   /* interpreted as OTM-Port and the table format is A A          */
   /* value of -2 is interpreted as table format B. A value of     */
   /* -3 is interpreted as table format C. Values of 80-124        */
   /* are illegal.                                                 */
   /* range: 68:62, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD format_select;

   /* LinkPtr: Pointer to the next entry in the linked list A      */
   /* value of 0 means the end of the list                         */
   /* range: 86:72, access type: UNDEF, default value: 0x0         */

   SOC_PETRA_TBL_FIELD link_ptr;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL;

  /* Snoop Mirror Table0: Snoop and Mirror table. This memory       */
  /* maps the incoming snoop command or mirror command to a         */
  /* replication. The first half is for snoop; the second           */
  /* half is for mirror. This table is split into two parts.         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* Destination: The destination for the snooped/mirrored        */
   /* packet. The destination must match the register              */
   /* SnoopIsIngMulticast/MirrorIsIngMulticast in the IDR.         */
   /* range: 15:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD destination;

   /* Tc: Traffic class for the copy. The traffic class is         */
   /* updated only if TcOw is set.                                 */
   /* range: 18:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tc;

   /* TcOw: If set then traffic class is overwritten               */
   /* range: 20:20, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tc_ow;

   /* Dp: Drop precedence for the copy. The drop precedence is     */
   /* updated only if DpOw is set.                                 */
   /* range: 25:24, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dp;

   /* DpOw: If set then drop precedence is overwritten.            */
   /* range: 28:28, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dp_ow;

   /* Parity:                                                      */
   /* range: 32:32, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL;

  /* Snoop Mirror Table1: Snoop and Mirror table. This memory       */
  /* maps the incoming snoop command or mirror command to a         */
  /* replication. First half is for snoop, second half is for       */
  /* mirror. This table is split into two parts. First line of        */
  /* this table must be set to all 0's.                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00040000 */

   /* MeterPtr0: Meter pointer to lower bank                       */
   /* range: 12:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD meter_ptr0;

   /* MeterPtr0Ow: If set then meter pointer to lower bank is      */
   /* overwritten.                                                 */
   /* range: 16:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD meter_ptr0_ow;

   /* MeterPtr1: Meter pointer to upper bank                       */
   /* range: 32:20, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD meter_ptr1;

   /* MeterPtr1Ow: If set then meter pointer to upper bank is      */
   /* overwritten.                                                 */
   /* range: 36:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD meter_ptr1_ow;

   /* CounterPtr0: Counter pointer to lower bank                   */
   /* range: 51:40, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD counter_ptr0;

   /* CounterPtr0Ow: If set then counter pointer to lower bank     */
   /* is overwritten.                                              */
   /* range: 52:52, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD counter_ptr0_ow;

   /* CounterPtr1: Counter pointer to upper bank                   */
   /* range: 67:56, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD counter_ptr1;

   /* CounterPtr1Ow: If set then counter pointer to upper bank     */
   /* is overwritten.                                              */
   /* range: 68:68, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD counter_ptr1_ow;

   /* DpCmd: DP command                                            */
   /* range: 73:72, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dp_cmd;

   /* DpCmdOw: If set then DP command is overwritten.              */
   /* range: 76:76, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dp_cmd_ow;

   /* Parity:                                                      */
   /* range: 80:80, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL;

  /* Free Pcb Memory: Free PCB Pointers FIFO Memory                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00060000 */

   /* Pcp: Pcp                                                     */
   /* range: 12:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD pcp;

   /* ECC: Ecc                                                     */
   /* range: 17:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_FREE_PCB_MEMORY_TBL;

  /* Pcb Link Table: PCB Link Table                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00070000 */

   /* Pcp: Pcp                                                     */
   /* range: 12:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD pcp;

   /* ECC: Ecc                                                     */
   /* range: 17:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_PCB_LINK_TABLE_TBL;

  /* Is Free Pcb Memory: Ingress Shaped free PCB Pointers           */
  /* FIFO Memory                                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00090000 */

   /* IsPcp: IsPcp                                                 */
   /* range: 9:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD is_pcp;

   /* ECC: Ecc                                                     */
   /* range: 13:10, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL;

  /* Is Pcb Link Table: Ingress Shaped PCB Link Table               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000a0000 */

   /* IsPcp: IsPcp                                                 */
   /* range: 9:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD is_pcp;

   /* ECC: Ecc                                                     */
   /* range: 13:10, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL;

  /* Rpf Memory: This memory holds free pointers into the           */
  /* resequencer memory.                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00110000 */

   /* Pointer:                                                     */
   /* range: 11:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD pointer;

   /* ECC:                                                         */
   /* range: 16:12, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_RPF_MEMORY_TBL;

  /* Mcr Memory: MCR Memory                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00180000 */

   /* Descriptor:                                                  */
   /* range: 72:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD descriptor;

   /* ECC:                                                         */
   /* range: 82:76, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_MCR_MEMORY_TBL;


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

  /* Descriptor:                                                */
  /* range: 84:0, access type: UNDEF, default value: 0x0        */
  SOC_PETRA_TBL_FIELD descriptor;

  /* NextPtr:                                                */
  /* range: 99:88, access type: UNDEF, default value: 0x0    */
  SOC_PETRA_TBL_FIELD next_ptr;

  /* Ecc:                                                     */
  /* range: 106:100, access type: UNDEF, default value: 0x0          */
  SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_RSQ_FIFO_TBL;


  /* Isf Memory: Ingress Shaped FIFO Memory                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001b0000 */

   /* Descriptor:                                                  */
   /* range: 51:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD descriptor;

   /* ECC:                                                         */
   /* range: 57:52, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_ISF_MEMORY_TBL;

  /* Destination Table: This memory maps a system port ID to        */
  /* a queue number and a traffic class profile                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x001c0000 */

   /* QueueNumber: The queue number to which the system port       */
   /* ID refers                                                    */
   /* range: 14:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD queue_number;

   /* QueueValid: If set the queue is valid.                       */
   /* range: 16:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD queue_valid;

   /* TcProfile: Traffic class profile. Selects one of two         */
   /* traffic class mappings                                       */
   /* range: 20:20, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tc_profile;

   /* Parity: If set the queue is valid.                           */
   /* range: 24:24, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_DESTINATION_TABLE_TBL;

  /* Lag To Lag Range: LAG range memory table. Specifies for         */
  /* each LAG its range (1 to 16)                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

   /* Range: The lag range may be from 1 to 16. A value of '0'     */
   /* indicates a lag of 1 member; a value of '15' indicates a     */
   /* lag of 16 members.                                           */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD range;

   /* Mode: If set, then round robin mode applies                  */
   /* range: 4:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD mode;

   /* Parity: If set the queue is valid.                           */
   /* range: 8:8, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL;

  /* Lag Mapping: LAG mapping table. Given the LAG and its           */
  /* member specifies the destination. Entry into table is           */
  /* \{LAG (8b), LAG-member (4b)\}.                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00220000 */

   /* Destination: The destination                                 */
   /* range: 15:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD destination;

   /* Parity: If set the queue is valid.                           */
   /* range: 16:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_LAG_MAPPING_TBL;

  /* Lag Next Member: For LAG resolution, this memory is            */
  /* read, and if it is in round-robin load-balanced mode,          */
  /* the output received is the offset corresponding to the         */
  /* LAG. Note that in this case, the offset is incremented         */
  /* by one and written back to the memory                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00230000 */

   /* Offset: The LAG member.                                      */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD offset;

   /* Parity: If set the queue is valid.                           */
   /* range: 4:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_LAG_NEXT_MEMBER_TBL;

  /* Smooth Division:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00240000 */

   /* Member: The LAG member.                                      */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD member;

   /* Parity: If set the queue is valid.                           */
   /* range: 4:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD parity;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_SMOOTH_DIVISION_TBL;

  /* Traffic Class Mapping:                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00250000 */

   /* TrafficClassMapping: The traffic class is mapped             */
   /* according to packet's source and destination. Table is       */
   /* accessed as follows: Before ingress shaping: line 0 -        */
   /* Destination is System-Port-ID with TC-Mapping-Profile        */
   /* equal to 0 line 2 - Destination is System-Port-ID with       */
   /* TC-Mapping-Profile equal to 1 line 4 - Destination is        */
   /* Flow line 6 - Destination is fabric or egress multicast      */
   /* After ingress shaping: line 1 - Destination is               */
   /* System-Port-ID with TC-Mapping-Profile equal to 0 line 3     */
   /* - Destination is System-Port-ID with TC-Mapping-Profile      */
   /* equal to 1 line 5 - Destination is Flow line 7 -             */
   /* Destination is fabric or egress multicast                    */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD traffic_class_mapping[SOC_PETRA_NOF_TRAFFIC_CLASSES];

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL;

  /* Port Mine Table Lag Port: If set, Lag Port has at least        */
  /* one member this Device                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01310000 */

   /* LagPortMine0:                                                */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lag_port_mine0;

   /* LagPortMine1:                                                */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lag_port_mine1;

   /* LagPortMine2:                                                */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lag_port_mine2;

   /* LagPortMine3:                                                */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lag_port_mine3;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL;

  /* Tm Port Pp Port Config: This table holds per TM-port           */
  /* parameters for resolving the incoming PP-Port.                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* PpPortOffset1: Offset in byte resolution of the lower        */
   /* word to input to the field extraction macro.                 */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pp_port_offset1;

   /* PpPortOffset2: Offset in byte resolution of the upper        */
   /* word to input to the field extraction macro.                 */
   /* range: 13:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pp_port_offset2;

   /* PpPortProfile: The profile of the field extraction           */
   /* macro.                                                       */
   /* range: 17:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pp_port_profile;

   /* PpPortUseOffsetDirectly: If set, the lower word to input     */
   /* to the field extraction macro is \{10'b0,                    */
   /* PpPortoffset1\}.                                             */
   /* range: 20:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pp_port_use_offset_directly;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL;

  /* Tm Port Sys Port Config: This table holds the per TM           */
  /* port parameters for resolving the source System-Port-ID.       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* SystemPortOffset1: Offset in byte resolution of the          */
   /* lower word to input to the field extraction macro.           */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD system_port_offset1;

   /* SystemPortOffset2: Offset in byte resolution of the          */
   /* upper word to input to the field extraction macro.           */
   /* range: 13:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD system_port_offset2;

   /* SystemPortValue: Source System-Port-ID that can be used      */
   /* in the field extraction macro (see                           */
   /* SystemPortValueToUse).                                       */
   /* range: 28:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD system_port_value;

   /* SystemPortProfile: The profile of the field extraction       */
   /* macro.                                                       */
   /* range: 33:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD system_port_profile;

   /* SystemPortValueToUse: Determines the input to the field      */
   /* extraction macro as follows: 0 - \{Header[Offset2],          */
   /* Header[Offset1]\} 1 - \{Header[offset2],                     */
   /* SystemPortValue\} 2 - \{Header[offset2],                     */
   /* PpPortSystemPortValue\} 3 - Reserved                         */
   /* range: 37:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD system_port_value_to_use;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00500000 */

/* IsemValid:                                                    */
/* range: 0:0, access type: RW, default value: 0x0               */
SOC_PETRA_TBL_FIELD isem_valid;

/* IsemVerifier:                                                 */
/* range: 20:1, access type: RW, default value: 0x0              */
SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_KEYT_H0_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00510000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 20:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_KEYT_H1_TBL;



typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00520000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 20:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_key;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_KEYT_AUX_TBL;

/* Isem Pldt H0 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00530000 */



  /* IsemValid:                                                    */
  /* range: 15:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_PLDT_H0_TBL;

/* Isem Pldt H1 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00540000 */



  /* IsemValid:                                                    */
  /* range: 15:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_PLDT_H1_TBL;

/* Isem Pldt Aux */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00550000 */



  /* IsemValid:                                                    */
  /* range: 15:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_PLDT_AUX_TBL;




/* Isem Management Memory A */


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00560000 */

  /* range: 33:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_key;

  /* IsemValid:                                                    */
  /* range: 49:34, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_MGMT_MEM_A_TBL;


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00570000 */


  /* range: 33:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_key;

  /* IsemValid:                                                    */
  /* range: 49:34, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_ISEM_MGMT_MEM_B_TBL;

/* Large Em Keyt H0 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00600000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 20:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_EM_KEYT_H0_TBL;

/* Large Em Keyt H1 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00610000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 20:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_EM_KEYT_H1_TBL;

/* Large Em Keyt H0 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00620000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 20:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_EM_KEYT_AUX_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00630000 */

  /* IsemValid:                                                    */
  /* range: 40:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD large_em_payload_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_EM_PLDT_H0;



  /* Pp Port Values: This table holds the per PP-Port values        */
  /* for port termination.                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00040000 */

   /* PpPortSystemPortValue: Per PP-port source System-Port-ID     */
   /* value.                                                       */
   /* range: 12:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pp_port_system_port_value;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_PP_PORT_VALUES_TBL;

  /* Pinfo Lbp:                                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00d00000 */

   /* CounterCompension: Meter and Counter compensation factor     */
   /* for the meters and counters proccessors Note:                */
   /* CounterCompension should should be used to compesate for     */
   /* DRAM CRC as well.                                            */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD counter_compension;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_PINFO_LBP_TBL;



  /* Congestion Point (CNM) descriptor - Static: Congestion         */
  /* point (CP) queue descriptor ( static )(per each CP Queue       */
  /* number out of 4K, CNM logic)                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02800000 */

   /* CpEnable: If set then the CP Queue is enabled to             */
   /* generate CNM. Else, disabled.                                */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cp_enable;

   /* CpClass: The CP profile/class , used as pointer to CP        */
   /* parameters table.                                            */
   /* range: 3:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cp_class;

   /* CpId: Congestion point index. A unique ID for each           */
   /* congestion point Q (used for CNM packet header               */
   /* construction)                                                */
   /* range: 15:4, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cp_id;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL;

  /* Congestion Point (CNM) descriptor - Dynamic: Congestion        */
  /* point (CP) queue descriptor ( Dynamic )(per each CP            */
  /* Queue number out of 4K, CNM logic)updated by the CNM           */
  /* logic.                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02900000 */

   /* CpEnqued1: The number of bytes enqued since last CNM         */
   /* packet generated. A CNM packet can be generated when         */
   /* CpEnqueued crosses the relevant SampBaseThreshold.           */
   /* (Reset whenever a CNM is sent, and decremented with each     */
   /* incoming packet) Resolution of 8 bytes.                      */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cp_enqued1;

   /* CpQsizeOld: If CNM sample mode on, then this fields is       */
   /* CpEnqued2[15:0], otherwsie bits 10:0 holds the               */
   /* congestion point Queue size on last event of CNM update      */
   /* (when CNM was generated, or Queue was eligible for CNM       */
   /* generation). Resolution of 256 bytes.                        */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cp_qsize_old;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL;

  /* Congestion Point (CNM) parameters table: Congestion            */
  /* point (CP) parameters table(per each CpClass , CNM logic       */
  /* )                                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02a00000 */

   /* CpQeq: Congestion point Queue equilibrium size. This is      */
   /* the reference Queue size for congestion decision.            */
   /* Resolution of 128 bytes.                                     */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cp_qeq;

   /* CpW: The signed weight of the Qdelta in the CNM feedback     */
   /* calculation. Cpw[4] : defines the sign of the weight.        */
   /* Cpw[4] = 0 : CpFb = Qoffset-Qdelta<<Cpw[3:0] Cpw[4] = 1      */
   /* : CpFb = Qoffset-Qdelta>>Cpw[3:0]                            */
   /* range: 16:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cp_w;

   /* CpFbMaxVal: The maximal negative value of CpFb. In units     */
   /* of 128 bytes. Note: this should be the absolute value of     */
   /* the reuired negative CpFb.                                   */
   /* range: 30:17  , access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD cp_fb_max_val;

   /* CpQuantDiv: The unsigned division factor to calculate        */
   /* the quantized feedback value sent on the CNM packet.         */
   /* range: 34:31  , access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD cp_quant_div;

   /* CpSampleBase0: The minimal number of packet's bytes that     */
   /* shold arrive to the Q (enqueued or discarded), between       */
   /* two consecutive CNM packets generated by the CP queue.       */
   /* The actual value is randomized in the range of               */
   /* 0.75->1.25 of this value. (64 bytes resolution) Selected     */
   /* when CpQntFb[5:3] = 0.                                       */
   /* range: 47:35  , access type: RW, default value: 0x0          */
   /* range: 60:48  , access type: RW, default value: 0x0          */
   /* range: 73:61  , access type: RW, default value: 0x0          */
   /* range: 86:74  , access type: RW, default value: 0x0          */
   /* range: 99:87  , access type: RW, default value: 0x0          */
   /* range: 112:100, access type: RW, default value: 0x0          */
   /* range: 125:113, access type: RW, default value: 0x0          */
   /* range: 138:126, access type: RW, default value: 0x0          */
    SOC_PETRA_TBL_FIELD cp_sample_base[SOC_PB_CNM_CP_SAMPLE_BASE_SIZE];

  

   /* CpFixedSampleBase: If set then CNM logic will not            */
   /* randomize the SampleBase value, otherwise, the               */
   /* SampleBase value is randomized as described above.           */
   /* range: 139:139, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD cp_fixed_sample_base;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL;

  /* Meter-processor result resolve table - Static: Meter           */
  /* Processor result mapping table (static)The table is            */
  /* accessed with the meter result to resolve the IQM-DP and       */
  /* EtmDe in case of metering. Index:                               */
  /* \{DP-Cmd[1:0],Iqm-DP[1:0], Meter-result[1:0]\}                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03000000 */

   /* IQM-DP: IQMDP: The drop precedence used by the IQM in        */
   /* reject and flow-control decisions.                           */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD iqm_dp;

   /* EtmDe: Egress TM discard eligible.                           */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD etm_de;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL;

  /* PRFSEL: For Both MeterProcc - Profile select memory (static)(per       */
  /* each meter index)                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* A - 0x03100000 */  /* B - 0x03200000 */

   /* Profile: One of 512 profiles for each leaky bucket Note:     */
   /* Upper 64 values of Profile may point to High-Rate LB.        */
   /* Meaning, if profile > 449, and HighRateEn asserted, then     */
   /* this profile is defined as HighRate.                         */
   /* range: 8:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD profile;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_PRFSEL_TBL;

  /* PRFCFG: For Both MeterProcc  -normal rate Leaky-Bucket settings         */
  /* (static)(per each meter-profile )                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* A - 0x03300000 */  /* B -  0x03400000 */

   /* ColorAware: Color Aware mode, If this flag is not set        */
   /* then the mode is Color Blind. Used when the service          */
   /* packets were already been policed/shaped                     */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD color_aware;

   /* CouplingFlag: Coupling Flag. If set, then excess credit      */
   /* from the Committed LB is passed to the Excess LB             */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD coupling_flag;

   /* FairnessMode: Fairness Mode. When set, a packet is           */
   /* granted without dependence with its size, and only later     */
   /* the LB level is updated.                                     */
   /* range: 2:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fairness_mode;

   /* CIR-Mantissa-64: Committed Information Rate Mantissa.        */
   /* The actual mantissa is the CIR-Mantissa-64 value + 64,       */
   /* e.g. if (CIR-Mantissa-64 == 3) ? CIR-Mantissa = 67 Value     */
   /* is in nibbles Note: * High meters - value is in bytes *      */
   /* Normal meters - value is in nibbles                          */
   /* range: 8:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD cir_mantissa_64;

   /* CIR-Reverse-Exponent: Committed Information Rate Reverse     */
   /* Exponent Note: * High meters - Allowed value range is 0      */
   /* to 3 * Normal meters - Allowed value range is 0 to 18        */
   /* range: 13:9, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cir_reverse_exponent;

   /* Reset-CIR: When set, reset the CIR. i.e., the Committed      */
   /* LB will never give credit                                    */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD reset_cir;

   /* CBS-Mantissa-64: Committed Burst Size Mantissa. The          */
   /* actual mantissa is the CBS-Mantissa-64 value + 64, e.g.      */
   /* if (CBS-Mantissa-64 == 1) ? CBS-Mantissa = 65 Allowed        */
   /* range is 0 to 13                                             */
   /* range: 20:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cbs_mantissa_64;

   /* CBS-Exponent: Committed Burst Size Exponent Note: * High     */
   /* meters - Allowed range is 0 to 15 * Normal meters -          */
   /* Allowed range is 0 to 13                                     */
   /* range: 24:21, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cbs_exponent;

   /* EIR-Mantissa-64: Excess Information Rate Mantissa. The       */
   /* actual mantissa is the EIR-Mantissa-64 value + 64 Note:      */
   /* * High meters - value is in bytes * Normal meters -          */
   /* value is in nibbles Value is in nibbles                      */
   /* range: 30:25, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD eir_mantissa_64;

   /* EIR-Reverse-Exponent: Excess Information Rate Reverse        */
   /* Exponent Note: * High meters - Allowed value range is 0      */
   /* to 3 * Normal meters - Allowed value range is 0 to 18        */
   /* range: 35:31, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD eir_reverse_exponent;

   /* Reset-EIR: When set, reset the EIR. I.e. The Excess LB       */
   /* will never give credit                                       */
   /* range: 36:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD reset_eir;

   /* EBS-Mantissa-64: Excess Burst Size Mantissa. The actual      */
   /* mantissa is the EBS-Mantissa-64 value + 64 Note: * High      */
   /* meters - Allowed range is 0 to 15 * Normal meters -          */
   /* Allowed range is 0 to 13                                     */
   /* range: 42:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ebs_mantissa_64;

   /* EBS-Exponent: Excess Burst Size Exponent                     */
   /* range: 46:43, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ebs_exponent;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_PRFCFG_TBL;

  /* Normal Dynamica: MeterProcA- Normal rate Leaky-Bucket          */
  /* Data base (Dynamic)This memory is used by the                  */
  /* Normal-rate leaky bucket logic to keep time stamp and          */
  /* credit balance.(per each meter profile )                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03700000 */

   /* TimeStamp: The "IR-normalized" value of                      */
   /* GlobalTimeCounter during the last access to this Meter       */
   /* instance                                                     */
   /* range: 19:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD time_stamp;

   /* CBL: Committed Bucket Level in nibble granularity.           */
   /* range: 40:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cbl;

   /* EBL: Excess Bucket Level in nibble granularity.              */
   /* range: 61:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ebl;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_NORMAL_DYNAMICA_TBL;

  /* Normal Dynamicb: MeterProcB- Normal rate Leaky-Bucket          */
  /* Data base (Dynamic)This memory is used by the                  */
  /* Normal-rate leaky bucket logic to keep time stamp and          */
  /* credit balance.(per each meter profile )                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03800000 */

   /* TimeStamp: The "IR-normalized" value of                      */
   /* GlobalTimeCounter during the last access to this Meter       */
   /* instance                                                     */
   /* range: 19:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD time_stamp;

   /* CBL: Committed Bucket Level in nibble granularity.           */
   /* range: 40:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cbl;

   /* EBL: Excess Bucket Level in nibble granularity.              */
   /* range: 61:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ebl;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_NORMAL_DYNAMICB_TBL;

  /* High Dynamica: MeterProcA- Highl rate Leaky-Bucket Data        */
  /* base (Dynamic)This memory is used by the Normal-rate           */
  /* leaky bucket logic to keep time stamp and credit               */
  /* balance.(per each high-rate meter profile )                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03900000 */

   /* Time-Stamp: The "IR-normalized" value of                     */
   /* GlobalTimeCounter during the last access to this Meter       */
   /* instance                                                     */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD time_stamp;

   /* CBL: Committed Bucket Level in byte granularity.             */
   /* range: 29:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cbl;

   /* EBL: Excess Bucket Level in byte granularity.                */
   /* range: 51:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ebl;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_HIGH_DYNAMICA_TBL;

  /* High Dynamicb: MeterProcB- Highl rate Leaky-Bucket Data        */
  /* base (Dynamic)This memory is used by the Normal-rate           */
  /* leaky bucket logic to keep time stamp and credit               */
  /* balance.(per each high-rate meter profile )                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03a00000 */

   /* Time-Stamp: The "IR-normalized" value of                     */
   /* GlobalTimeCounter during the last access to this Meter       */
   /* instance                                                     */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD time_stamp;

   /* CBL: Committed Bucket Level in byte granularity.             */
   /* range: 29:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cbl;

   /* EBL: Excess Bucket Level in byte granularity.                */
   /* range: 51:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD ebl;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_HIGH_DYNAMICB_TBL;

  /* Cnts Mem: CounterPorcessor  - counters memory                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;
                          /* 0x03b00000 */
                          /* 0x03c00000 */

   /* PacketsCounter: Packets counter, increments by 1 for         */
   /* each counter command.                                        */
   /* range: 24:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD packets_counter;

   /* OctetsCounter: Octets counter (bytes), increments by         */
   /* packet_size for each counter command.                        */
   /* range: 56:25, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD octets_counter;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_CNTS_MEM_TBL;

  /* Ovth Mema: CounterPorcessor A - over the threshold             */
  /* memory                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03d00000 */

   /* OvthCounterBits: Over the thresh indication: Each bit        */
   /* represents a counters memory entry. Indicates that at        */
   /* least one of the counters in the counters pair has           */
   /* crossed its configurable treshold (two configurable          */
   /* thresholds are available, one for packets and one for        */
   /* octets). Note: Each lines has 64 indications of 64           */
   /* consecutive counters pairs.                                  */
   /* range: 63:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD ovth_counter_bits;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_OVTH_MEMA_TBL;

  /* Ovth Memb: CounterPorcessor B - over the threshold             */
  /* memory                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x03e00000 */

   /* OvthCounterBits: Over the thresh indication: Each bit        */
   /* represents a counters memory entry. Indicates that at        */
   /* least one of the counters in the counters pair has           */
   /* crossed its configurable treshold (two configurable          */
   /* thresholds are available, one for packets and one for        */
   /* octets). Note: Each lines has 64 indications of 64           */
   /* consecutive counters pairs.                                  */
   /* range: 63:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD ovth_counter_bits;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_OVTH_MEMB_TBL;



  /* Sop MMU:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* SopMMU: SOP MMU FIFO                                         */
   /* range: 83:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sop_mmu;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_SOP_MMU_TBL;




  /* Egq Txq Wr Addr:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000b0000 */

   /* EgqTxqWrAddr: EGQ TX FIFO write pointer.                     */
   /* range: 4:0, access type: R, default value: 0x0               */
   SOC_PETRA_TBL_FIELD egq_txq_wr_addr;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_EGQ_TXQ_WR_ADDR_TBL;

  /* Egq Txq Rd Addr:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000c0000 */

   /* EgqTxqRdAddr: EGQ TX FIFO read pointer.                      */
   /* range: 4:0, access type: R, default value: 0x0               */
   SOC_PETRA_TBL_FIELD egq_txq_rd_addr;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL;

  /* Fdt Txq Wr Addr: FDT TX FIFO write pointer (DTQs)              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000d0000 */

   /* Dtq0WrAddr: Context0                                         */
   /* range: 8:0, access type: RO, default value: 0x0              */
   SOC_PETRA_TBL_FIELD dtq0_wr_addr;

   /* Dtq1WrAddr: Context1                                         */
   /* range: 17:9, access type: RO, default value: 0x0             */
   SOC_PETRA_TBL_FIELD dtq1_wr_addr;

   /* Dtq2WrAddr: Context2                                         */
   /* range: 26:18, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq2_wr_addr;

   /* Dtq3WrAddr: Context3                                         */
   /* range: 35:27, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq3_wr_addr;

   /* Dtq4WrAddr: Context4                                         */
   /* range: 44:36, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq4_wr_addr;

   /* Dtq5WrAddr: Context5                                         */
   /* range: 53:45, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq5_wr_addr;

   /* Dtq6WrAddr: Context6                                         */
   /* range: 62:54, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq6_wr_addr;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL;

  /* Fdt Txq Rd Addr: FDT TX FIFO read pointer (DTQs)               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000e0000 */

   /* Dtq0RdAddr: Context0                                         */
   /* range: 8:0, access type: RO, default value: 0x0              */
   SOC_PETRA_TBL_FIELD dtq0_rd_addr;

   /* Dtq1RdAddr: Context1                                         */
   /* range: 17:9, access type: RO, default value: 0x0             */
   SOC_PETRA_TBL_FIELD dtq1_rd_addr;

   /* Dtq2RdAddr: Context2                                         */
   /* range: 26:18, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq2_rd_addr;

   /* Dtq3RdAddr: Context3                                         */
   /* range: 35:27, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq3_rd_addr;

   /* Dtq4RdAddr: Context4                                         */
   /* range: 44:36, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq4_rd_addr;

   /* Dtq5RdAddr: Context5                                         */
   /* range: 53:45, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq5_rd_addr;

   /* Dtq6RdAddr: Context6                                         */
   /* range: 62:54, access type: RO, default value: 0x0            */
   SOC_PETRA_TBL_FIELD dtq6_rd_addr;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL;

  /* Gci Backoff Mask: GciBackofMask table. Configure mask           */
  /* bits for timer period randomization, each 12 bits define       */
  /* a configurable mask per each backoff timer (entry 0            */
  /* refers to timer1). Timer = \{LFSR[17:6] & Mask[11:0],           */
  /* LFSR[5:0]\}                                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000f0000 */

   /* Mask0: Mask bits for congestion level 0                      */
   /* range: 11:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD mask0;

   /* Mask1: Mask bits for congestion level 1                      */
   /* range: 23:12, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD mask1;

   /* Mask2: Mask bits for congestion level 2                      */
   /* range: 35:24, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD mask2;

   /* Mask3: Mask bits for congestion level 3                      */
   /* range: 47:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD mask3;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_GCI_BACKOFF_MASK_TBL;


  /* Ipt Contro L Fifo:                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* Ipt ControL Fifo: hold the ipt descriptor weith the rtp      */
   /* bitmap                                                       */
   /* range: 60:0, access type: RO, default value: undef           */
   SOC_PETRA_TBL_FIELD ipt_contro_l_fifo;

} __ATTRIBUTE_PACKED__ SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL;

/* MLF:                                             */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

  SOC_PETRA_TBL_FIELD field;

} __ATTRIBUTE_PACKED__ SOC_PB_RAW_TBL;


  /* QM: OFPs Queue FIFOs memory used for buffering the FQP         */
  /* descriptors.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00220000 */

   /* ReadPointer: Pointer to the packet buffer address in the     */
   /* DBF                                                          */
   /* range: 12:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD read_pointer;

   /* PacketSize256toEOP: Indicates the packet size in 32B         */
   /* resolution                                                   */
   /* range: 21:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD packet_size256to_eop;

   /* CounterDecreament: The counter decrement number              */
   /* range: 33:22, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD counter_decreament;

   /* CopyData: The data received from the PQP                     */
   /* range: 49:34, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD copy_data;

   /* LastSegSize: Indicates the last segment size of the          */
   /* packet                                                       */
   /* range: 51:50, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD last_seg_size;

   /* BeforeLastSegSize: Indicates the before last segment         */
   /* size and the packet size in case of Fix129                   */
   /* range: 54:52, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD before_last_seg_size;

   /* PqpQNum: One of four PQP queues of the descriptor origin     */
   /* range: 56:55, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD pqp_qnum;

   /* PqpOc768Qnum: Fat Pipe queue number of the descriptor in     */
   /* pqp                                                          */
   /* range: 58:57, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD pqp_oc768_qnum;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_QM_TBL;

  /* QSM:                                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00230000 */

   /* QSM: Queue Size memory                                       */
   /* range: 14:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD qsm;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_QSM_TBL;

  /* DCM:                                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00240000 */

   /* DCM: Descriptors count memory                                */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD dcm;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_DCM_TBL;

  /* Ofp Dequeue Wfq Configuration Memory (DWM_NEW): The table      */
  /* consist from eighty entries, one entry per OFP. Defines        */
  /* the weights of the two classes connected to this               */
  /* scheduler. Enables WFQ and SP relation between the two         */
  /* classes. Weight of 0 for a class indicates that this           */
  /* class has SP over the other class. When both have equal        */
  /* weights, this implies simple RR. Maximum weight                */
  /* difference is 255:1.                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00260000 */

   /* McHighQueueWeight: The lower the weight, the higher          */
   /* the bandwidth                                                */
   /* range: 7:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD mc_high_queue_weight;

   /* UcHighQueueWeight: The lower the weight the higher           */
   /* the bandwidth                                                */
   /* range: 15:8, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD uc_high_queue_weight;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_DWM_NEW_TBL;


  /* Port Configuration Table (PCT): Out-Port Configuration         */
  /* Table. Eighty-entries table. This table is accessed by         */
  /* FQP and ETPP.                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00290000 */

   /* ProgEditorValue: This value is stamped into                  */
   /* internal-fields in bits [63:48]                              */
   /* range: 15:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD prog_editor_value;

   /* OutPpPort: Outgoing packet processing port                   */
   /* range: 21:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD out_pp_port;

   /* OutboundMirr: If set then outbound mirroring is enabled      */
   /* per OFP                                                      */
   /* range: 22:22   , access type: UNDEF, default value: 0x0      */
   SOC_PETRA_TBL_FIELD outbound_mirr;

   /* DestPort: Destination system port                            */
   /* range: 35:23, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD dest_port;

   /* PortProfile: 2-bits profile per OFP, this two bits used      */
   /* in selecting editing program.                                */
   /* range: 39:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_profile;

   /* PortChNum: Port channel number. This field is used when      */
   /* mapping OFP to channelized interface.                        */
   /* range: 47:40, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_ch_num;

   /* PortType: Port-Type is decoded as follows: 0 - CPU 1 -       */
   /* RAW 2 - TM 3 - ETHERNET                                      */
   /* range: 49:48, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_type;

   /* CrAdjustType: If set then header adjust type B is            */
   /* used,otherwise type A is used.                               */
   /* range: 50:50, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cr_adjust_type;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PCT_TBL;


  /* Tc Dp Map Table:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x002d0000 */

   /* TcDpMap: Lookup table used to set the TC(3) and DP(2).       */
   /* Index to this table is 9 bits:                               */
   /* \{Port. TcDpMapProfile[1:0],egress-mc-copy,System-mc-copy,    */
   /* System-TC[2:0],System-DP[1:0]\} The output of this table     */
   /* is 5 bits decoded as \{DP[1:0], TC[2:0]\}                    */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD tc;
   /* range: 4:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD dp;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_TC_DP_MAP_TBL;


  /* Fqp Nif Port Mux: NIF ports Calendar used in EGQ               */
  /* scheduler.                                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004a0000 */

   /* FqpNifPortMux: Each entry value may be one of the            */
   /* following options: 1. NIF port (give BW to NIF port) 2.      */
   /* 125 (give BW to other interfaces only if they need it)       */
   /* 3. 126 (always give BW to other interfaces) 4. 127 (Not      */
   /* valid entry)                                                 */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fqp_nif_port_mux;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL;

  /* Key-Profile-Index (3)                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00650000 */

   /* KeyProfileMapIndex:                                          */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_profile_map_index;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL;

  /* TCAM Key Resolution Profile: The Key-Profile-Index             */
  /* selects one of eight key resolution profiles. The              */
  /* structure of a Key-Resolution-Profile is described             */
  /* below.                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00660000 */

   /* KeySelect: Determines which key is used: 3'b001 -            */
   /* Ethernet key 3'b010 - IPv4 key 3'b011 - MPLS key 3'b100      */
   /* - TM key 3'b101 -- key Other-cases - NO-LOOKUP          */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

   /* KeyAND_Value: The actual key to the TCAM is                  */
   /* (selected-key AND KeyAND_Value) OR KeyOR_Value. The          */
   /* AND/OR operations are applied to the msb of the key.         */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_and_value;

   /* KeyOR_Value:                                                 */
   /* range: 11:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_or_value;

   /* TCAM_Profile: Determines which banks in the TCAM are         */
   /* used for lookup                                              */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD tcam_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL;

  /* Parser Last Sys Record:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00680000 */

   /* ParserLastSysRecord: Last system headers record from         */
   /* ERPP parser                                                  */
   /* range: 226:0, access type: R, default value: 0x0             */
   SOC_PETRA_TBL_FIELD parser_last_sys_record;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL;

  /* Parser Last Nwk Record1:                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00690000 */

   /* ParserLastNwkRecord1: Last network headers record from       */
   /* ERPP parser                                                  */
   /* range: 255:0, access type: R, default value: 0x0             */
   SOC_PETRA_TBL_FIELD parser_last_nwk_record1;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL;

  /* Parser Last Nwk Record2:                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006a0000 */

   /* ParserLastNwkRecord2: Last network headers record from       */
   /* ERPP parser                                                  */
   /* range: 46:0, access type: R, default value: 0x0              */
   SOC_PETRA_TBL_FIELD parser_last_nwk_record2;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL;

  /* Erpp Debug: Erpp Debug Information                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006b0000 */

   /* ErppDebug: This register holds the ERPP block outputs        */
   /* selected by DebugSelect register.                            */
   /* range: 230:0, access type: R, default value: 0x0             */
   SOC_PETRA_TBL_FIELD erpp_debug;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_ERPP_DEBUG_TBL;

  /* Pinfo PMF: This table maps In-PP-Port to                       */
  /* PortPMF_Profile, a 3-bit variable that is used to define       */
  /* the program that the PMF executes.                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00300000 */

   /* PortPMF_Profile:                                             */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD port_pmf_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_PINFO_PMF_TBL;

  /* Packet Format Code Profile: This table maps                    */
  /* packet-format-code to packet-format-code-profile, a 3          */
  /* bit variable that is used to define the program that the       */
  /* PMF executes.                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00310000 */

   /* PacketFormatCodeProfile:                                     */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD packet_format_code_profile;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00520000 */
  
  

  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_valid;

  /* IsemVerifier:                                                 */
  /* range: 4:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD svem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_KEYT_H0_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00530000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_valid;

  /* IsemVerifier:                                                 */
  /* range: 4:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD svem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_KEYT_H1_TBL;



typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00540000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_valid;

  /* IsemVerifier:                                                 */
  /* range: 16:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD svem_key;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_KEYT_AUX_TBL;

  /* Isem Pldt H0 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00550000 */



  /* Payload:                                                    */
  /* range: 11:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_PLDT_H0_TBL;

/* Isem Pldt H1 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00560000 */



  /* IsemValid:                                                    */
  /* range: 11:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_PLDT_H1_TBL;

/* Isem Pldt Aux */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00570000 */



  /* IsemValid:                                                    */
  /* range: 11:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_PLDT_AUX_TBL;




/* Isem Management Memory A */


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00580000 */

  
  /* range: 15:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_key;

  /* IsemValid:                                                    */
  /* range: 27:16, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_MGMT_MEM_A_TBL;


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00590000 */

  /*IsemKey */
  /* range: 15:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_key;

  /* IsemValid:                                                    */
  /* range: 27:16, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD svem_payload_data;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_SVEM_MGMT_MEM_B_TBL;

  /* Src Dest Port For L3 ACL Key: Used to calculate L4Ops.         */
  /* The relevant bit of the L4Ops is set to '1'                    */
  /* if:(source_port >= source_port_min ) & (source_port <=         */
  /* source_port_max ) & (destination_port >=                       */
  /* destination_port_min ) & (destination_port <=                  */
  /* destination_port_max )                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00410000 */

   /* SourcePortMin:                                               */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD source_port_min;

   /* SourcePortMax:                                               */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD source_port_max;

   /* DestinationPortMin:                                          */
   /* range: 47:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination_port_min;

   /* DestinationPortMax:                                          */
   /* range: 63:48, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD destination_port_max;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL;

  /* Direct 1st Pass Key Profile Resolved Data: This table          */
  /* defines the address to DirectActionTable. Table Address:        */
  /* FirstPassKeyProfileIndex from Program-Vars-Table.              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00440000 */

   /* DirectKeySelect: This field defines the key to be chosen     */
   /* to access DirectActionTable: 0 - No Lookup 1 - KeyA[9:0]     */
   /* 2 - KeyA[19:10] 3 - KeyA[29:20] 4 - KeyA[39:30] 5 -          */
   /* KeyB[19:10] 6 - KeyB[29:20] 7 - KeyB[39:30]                  */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD direct_key_select;

   /* DirectDB_AND_Value: Specifies how to modify the 2 MSBs       */
   /* of the key. The modification is 'bitwise AND' on the MSB     */
   /* of the selected key. First the AND mask is applied, and      */
   /* then the OR mask                                             */
   /* range: 5:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD direct_db_and_value;

   /* DirectDB_OR_Value: specifies how to modify the 2 MSBs of     */
   /* the key. The modification is 'bitwise OR' on the MSB of      */
   /* the selected key. First the AND mask is applied, and         */
   /* then the OR mask                                             */
   /* range: 9:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD direct_db_or_value;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* Direct 2nd Pass Key Profile Resolved Data: This table          */
  /* defines the address to DirectActionTable. Table Address:        */
  /* If second-pass-action is valid, then the address is by         */
  /* SecondPassKeyProfileTable; else the address is by              */
  /* SecondPassKeyProfileIndex from Program-Vars-Table.             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00450000 */

   /* DirectKeySelect: See                                         */
   /* Direct_1stPassKeyProfileResolvedData.                        */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD direct_key_select;

   /* DirectDB_AND_Value: See                                      */
   /* Direct_1stPassKeyProfileResolvedData.                        */
   /* range: 5:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD direct_db_and_value;

   /* DirectDB_OR_Value: See                                       */
   /* Direct_1stPassKeyProfileResolvedData.                        */
   /* range: 9:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD direct_db_or_value;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM0 4b 1st Pass Key Profile Resolved Data: This table         */
  /* holds the Program and Key-Select for                           */
  /* Field-Extraction-Macros. Table Address:                         */
  /* FirstPassKeyProfileIndex from Program-Vars-Table.              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00470000 */

   /* Program: Choose one of 4 programs. Via this field the        */
   /* FEM accesses the table ProgramResolvedData                   */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect: Selects one of the following keys: 0 -            */
   /* TCAM_PD1 1 - TCAM_PD2 2 - TCAM_SEL3 3 - TCAM_SEL4 4 -        */
   /* Direct-Action 5 - Key-A[31:0] 6 - Key-A[47:16] 7 -           */
   /* Key-A[63:32] 8 - Key-A[79:48] 9 - Key-A[95:64] 10 -          */
   /* Key-B[31:0] 11 - Key-B[47:16] 12 - Key-B[63:32] 13 -         */
   /* Key-B[79:48] 14 - Key-B[95:64] 15 - NOP                      */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM0 4b 1st Pass Program Resolved Data: This table holds       */
  /* the 4bits to be taken from the key, which are used to          */
  /* address MapIndexTable. Table Address: According to the          */
  /* FEM-Program                                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00480000 */

   /* BitSelect: Pointer to the first MSB bit of 4 bits to be      */
   /* selected from the key. Minimum value is '3'.                 */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM0 4b 1st Pass Map Index Table: This table holds the         */
  /* FEM's Map-Data and Map-Index, which are used as an             */
  /* address to the FEM0_4b_MapTable. Table Address:                 */
  /* \{FEM-Program(2b), Key-Selected-Bits(4b)\}                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00490000 */

   /* MapIndex: Used to address the table MapTable.                */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData: Configurable data that can be used in the FEM's     */
   /* action.                                                      */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM0 4b 1st Pass Map Table: This table defines the             */
  /* action type and how to build the action. Table Address:         */
  /* MapIndex                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004a0000 */

   /* ActionType: 0. Destination 1. DP 2. TC 3. Forward 4.         */
   /* Snoop 5. Mirror 6. Outbound-Mirror-Disable 7.                */
   /* Exclude-Src 8. Ingress-Shaping 9. Meter 10. Counter 11.      */
   /* Statistics 12. Second-Pass-Data 13. Second-Pass-Program      */
   /* else - None                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0: Defines the relevant bit of the action      */
   /* as follows: If (FieldSelectMap[5] == 0) ActionBit =          */
   /* Key[FieldSelectMap[4:0]] else If (FieldSelectMap[5:2] ==     */
   /* 4'b1000) ActionBit = MapData[FieldSelectMap[1:0]] else       */
   /* ActionBit = FieldSelectMap[0]                                */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1: See above                                   */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2: See above                                   */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3: See above                                   */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM1 4b 1st Pass Key Profile Resolved Data: For details,       */
  /* see FEM0_4b_1stPassKeyProfileResolvedData.                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004b0000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM1 4b 1st Pass Program Resolved Data: For details,           */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004c0000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM1 4b 1st Pass Map Index Table: For details, see             */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004d0000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM1 4b 1st Pass Map Table: For details, see                   */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004e0000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM2 14b 1st Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x004f0000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM2 14b 1st Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00500000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM2 14b 1st Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00510000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM2 14b 1st Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00520000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* OffsetIndex: Select one of 4 offsets to be added to the      */
   /* action, the offsets are defined in OffsetTable               */
   /* range: 117:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM2 14b 1st Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00530000 */

   /* Offset:                                                      */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL;

  /* FEM3 14b 1st Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00540000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM3 14b 1st Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00550000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM3 14b 1st Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00560000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM3 14b 1st Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00570000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* OffsetIndex: Select one of 4 offsets to be added to the      */
   /* action, the offsets are defined in OffsetTable               */
   /* range: 117:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM3 14b 1st Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00580000 */

   /* Offset:                                                      */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL;

  /* FEM4 14b 1st Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00590000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM4 14b 1st Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x005a0000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM4 14b 1st Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x005b0000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM4 14b 1st Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x005c0000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* OffsetIndex: Select one of 4 offsets to be added to the      */
   /* action, the offsets are defined in OffsetTable               */
   /* range: 117:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM4 14b 1st Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x005d0000 */

   /* Offset:                                                      */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL;

  /* FEM5 17b 1st Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x005e0000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM5 17b 1st Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x005f0000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM5 17b 1st Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00600000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM5 17b 1st Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00610000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* FieldSelectMap14:                                            */
   /* range: 121:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map14;

   /* FieldSelectMap15:                                            */
   /* range: 129:124, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map15;

   /* FieldSelectMap16:                                            */
   /* range: 137:132, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map16;

   /* OffsetIndex:                                                 */
   /* range: 141:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM5 17b 1st Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00620000 */

   /* Offset:                                                      */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL;

  /* FEM6 17b 1st Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00630000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM6 17b 1st Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00640000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM6 17b 1st Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00650000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM6 17b 1st Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00660000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* FieldSelectMap14:                                            */
   /* range: 121:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map14;

   /* FieldSelectMap15:                                            */
   /* range: 129:124, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map15;

   /* FieldSelectMap16:                                            */
   /* range: 137:132, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map16;

   /* OffsetIndex:                                                 */
   /* range: 141:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM6 17b 1st Pass Offset Table: For details, see               */
  /* FEM4_14b_1stPassMapTable.                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00670000 */

   /* Offset:                                                      */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL;

  /* FEM7 17b 1st Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00680000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM7 17b 1st Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00690000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM7 17b 1st Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006a0000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM7 17b 1st Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006b0000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* FieldSelectMap14:                                            */
   /* range: 121:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map14;

   /* FieldSelectMap15:                                            */
   /* range: 129:124, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map15;

   /* FieldSelectMap16:                                            */
   /* range: 137:132, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map16;

   /* OffsetIndex:                                                 */
   /* range: 141:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL;

  /* FEM7 17b 1st Pass Offset Table: For details, see               */
  /* FEM4_14b_1stPassMapTable.                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006c0000 */

   /* Offset:                                                      */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL;

  /* FEM0 4b 2nd Pass Key Profile Resolved Data: This table         */
  /* holds the Program and Key-Select for                           */
  /* Field-Extraction-Macros. Table Address:                         */
  /* FirstPassKeyProfileIndex from Program-Vars-Table.              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006d0000 */

   /* Program: Choose one of 4 programs. Via this field the        */
   /* FEM accesses the table ProgramResolvedData                   */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect: Selects one of the following keys: 0 -            */
   /* TCAM_PD1 1 - TCAM_PD2 2 - TCAM_SEL3 3 - TCAM_SEL4 4 -        */
   /* Direct-Action 5 - Key-A[31:0] 6 - Key-A[47:16] 7 -           */
   /* Key-A[63:32] 8 - Key-A[79:48] 9 - Key-A[95:64] 10 -          */
   /* Key-B[31:0] 11 - Key-B[47:16] 12 - Key-B[63:32] 13 -         */
   /* Key-B[79:48] 14 - Key-B[95:64] 15 - NOP                      */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM0 4b 2nd Pass Program Resolved Data: This table holds       */
  /* the 4bits to be taken from the key, which are used to          */
  /* address MapIndexTable. Table Address: According to the          */
  /* FEM-Program                                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006e0000 */

   /* BitSelect: Pointer to the first MSB bit of 4 bits to be      */
   /* selected from the key. Minimum value is '3'.                 */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM0 4b 2nd Pass Map Index Table: This table holds the         */
  /* FEM's Map-Data and Map-Index, which are used as an             */
  /* address to the FEM0_4b_MapTable. Table Address:                 */
  /* \{FEM-Program(2b), Key-Selected-Bits(4b)\}                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x006f0000 */

   /* MapIndex: Used to address the table MapTable.                */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData: Configurable data that can be used in the FEM's     */
   /* action.                                                      */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM0 4b 2nd Pass Map Table: This table defines the             */
  /* action type and how to build the action. Table Address:         */
  /* MapIndex                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00700000 */

   /* ActionType: 0. Destination 1. DP 2. TC 3. Forward 4.         */
   /* Snoop 5. Mirror 6. Outbound-Mirror-Disable 7.                */
   /* Exclude-Src 8. Ingress-Shaping 9. Meter 10. Counter 11.      */
   /* Statistics 12. Second-Pass-Data 13. Second-Pass-Program      */
   /* else - None                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0: Defines the relevant bit of the action      */
   /* as follows: If (FieldSelectMap[5] == 0) ActionBit =          */
   /* Key[FieldSelectMap[4:0]] else If (FieldSelectMap[5:2] ==     */
   /* 4'b1000) ActionBit = MapData[FieldSelectMap[1:0]] else       */
   /* ActionBit = FieldSelectMap[0]                                */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1: See above                                   */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2: See above                                   */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3: See above                                   */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM1 4b 2nd Pass Key Profile Resolved Data: For details,       */
  /* see FEM0_4b_1stPassKeyProfileResolvedData.                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00710000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM1 4b 2nd Pass Program Resolved Data: For details,           */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00720000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM1 4b 2nd Pass Map Index Table: For details, see             */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00730000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM1 4b 2nd Pass Map Table: For details, see                   */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00740000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM2 14b 2nd Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00750000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM2 14b 2nd Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00760000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM2 14b 2nd Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00770000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM2 14b 2nd Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00780000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* OffsetIndex: Select one of 4 offsets to be added to the      */
   /* action, the offsets are defined in OffsetTable               */
   /* range: 117:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM2 14b 2nd Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00790000 */

   /* Offset:                                                      */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL;

  /* FEM3 14b 2nd Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007a0000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM3 14b 2nd Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007b0000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM3 14b 2nd Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007c0000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM3 14b 2nd Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007d0000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* OffsetIndex: Select one of 4 offsets to be added to the      */
   /* action, the offsets are defined in OffsetTable               */
   /* range: 117:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM3 14b 2nd Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007e0000 */

   /* Offset:                                                      */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL;

  /* FEM4 14b 2nd Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x007f0000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM4 14b 2nd Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00800000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM4 14b 2nd Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00810000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM4 14b 2nd Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00820000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* OffsetIndex: Select one of 4 offsets to be added to the      */
   /* action, the offsets are defined in OffsetTable               */
   /* range: 117:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM4 14b 2nd Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00830000 */

   /* Offset:                                                      */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL;

  /* FEM5 17b 2nd Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00840000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM5 17b 2nd Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00850000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM5 17b 2nd Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00860000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM5 17b 2nd Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00870000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* FieldSelectMap14:                                            */
   /* range: 121:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map14;

   /* FieldSelectMap15:                                            */
   /* range: 129:124, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map15;

   /* FieldSelectMap16:                                            */
   /* range: 137:132, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map16;

   /* OffsetIndex:                                                 */
   /* range: 141:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM5 17b 2nd Pass Offset Table: This table defines the         */
  /* offset to be added to the action. The address of this           */
  /* table is according to OffsetIndex                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00880000 */

   /* Offset:                                                      */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL;

  /* FEM6 17b 2nd Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00890000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM6 17b 2nd Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x008a0000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM6 17b 2nd Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x008b0000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM6 17b 2nd Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x008c0000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* FieldSelectMap14:                                            */
   /* range: 121:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map14;

   /* FieldSelectMap15:                                            */
   /* range: 129:124, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map15;

   /* FieldSelectMap16:                                            */
   /* range: 137:132, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map16;

   /* OffsetIndex:                                                 */
   /* range: 141:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM6 17b 2nd Pass Offset Table: For details, see               */
  /* FEM4_14b_1stPassMapTable.                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x008d0000 */

   /* Offset:                                                      */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL;

  /* FEM7 17b 2nd Pass Key Profile Resolved Data: For               */
  /* details, see FEM0_4b_1stPassKeyProfileResolvedData.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x008e0000 */

   /* Program:                                                     */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD program;

   /* KeySelect:                                                   */
   /* range: 5:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD key_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL;

  /* FEM7 17b 2nd Pass Program Resolved Data: For details,          */
  /* seeFEM0_4b_1stPassProgramResolvedData.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x008f0000 */

   /* BitSelect:                                                   */
   /* range: 4:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD bit_select;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL;

  /* FEM7 17b 2nd Pass Map Index Table: For details, see            */
  /* FEM0_4b_1stPassMapIndexTable.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00900000 */

   /* MapIndex:                                                    */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_index;

   /* MapData:                                                     */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD map_data;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL;

  /* FEM7 17b 2nd Pass Map Table: For details, see                  */
  /* FEM0_4b_1stPassMapTable.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00910000 */

   /* ActionType:                                                  */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD action_type;

   /* FieldSelectMap0:                                             */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD field_select_map0;

   /* FieldSelectMap1:                                             */
   /* range: 17:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map1;

   /* FieldSelectMap2:                                             */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map2;

   /* FieldSelectMap3:                                             */
   /* range: 33:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map3;

   /* FieldSelectMap4:                                             */
   /* range: 41:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map4;

   /* FieldSelectMap5:                                             */
   /* range: 49:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map5;

   /* FieldSelectMap6:                                             */
   /* range: 57:52, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map6;

   /* FieldSelectMap7:                                             */
   /* range: 65:60, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map7;

   /* FieldSelectMap8:                                             */
   /* range: 73:68, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map8;

   /* FieldSelectMap9:                                             */
   /* range: 81:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map9;

   /* FieldSelectMap10:                                            */
   /* range: 89:84, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map10;

   /* FieldSelectMap11:                                            */
   /* range: 97:92, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD field_select_map11;

   /* FieldSelectMap12:                                            */
   /* range: 105:100, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map12;

   /* FieldSelectMap13:                                            */
   /* range: 113:108, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map13;

   /* FieldSelectMap14:                                            */
   /* range: 121:116, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map14;

   /* FieldSelectMap15:                                            */
   /* range: 129:124, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map15;

   /* FieldSelectMap16:                                            */
   /* range: 137:132, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD field_select_map16;

   /* OffsetIndex:                                                 */
   /* range: 141:140, access type: RW, default value: 0x0          */
   SOC_PETRA_TBL_FIELD offset_index;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL;

  /* FEM7 17b 2nd Pass Offset Table: For details, see               */
  /* FEM4_14b_1stPassMapTable.                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00920000 */

   /* Offset:                                                      */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD offset;

} __ATTRIBUTE_PACKED__ SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL;

/* FEM7 17b 2nd Pass Map Index Table: For details, see            */


  
 
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 9:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_KEYT_H0_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00210000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 9:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_verifier;

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_KEYT_H1_TBL;



typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00220000 */



  /* IsemValid:                                                    */
  /* range: 0:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_valid;

  /* IsemVerifier:                                                 */
  /* range: 21:1, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD isem_key;

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_KEYT_AUX_TBL;

/* Isem Pldt H0 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00230000 */



  /* IsemValid:                                                    */
  /* range: 14:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_PLDT_H0_TBL;

/* Isem Pldt H1 */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00240000 */



  /* IsemValid:                                                    */
  /* range: 14:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_PLDT_H1_TBL;

/* Isem Pldt Aux */

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00250000 */



  /* IsemValid:                                                    */
  /* range: 14:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_PLDT_AUX_TBL;




/* Isem Management Memory A */


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00260000 */

  /* range: 20:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_key;

  /* IsemValid:                                                    */
  /* range: 35:21, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;


} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_MGMT_MEM_A_TBL;


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00270000 */


  /* range: 20:0, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_key;

  /* IsemValid:                                                    */
  /* range: 35:21, access type: RW, default value: 0x0               */
  SOC_PETRA_TBL_FIELD isem_payload_data;

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_ESEM_MGMT_MEM_B_TBL;


  /* Copy Engine0 Program: Holds two instructions per copy          */
  /* engine; each instruction copies to the key up to 32 bits       */
  /* from the packet header or from internal-fields                 */
  /* (PP-info).                                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000d0000 */

   /* Inst0_Valid: If set, then the instruction is valid and       */
   /* can updated the key; otherwise the instruction doesn't       */
   /* update the key.                                              */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst_valid[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS];

   /* Inst0_SourceSelect: 0 - source select is Packet Header 1     */
   /* - source select is Internal-Fields (PP-info)                 */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst_source_select[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS];

   /* Inst0_HeaderOffsetSelect: If (Source-Select ==               */
   /* Packet-Header), then this field selects one of 5 header      */
   /* offsets which affects StartNibble pointer: StartNibble =     */
   /* HeaderOffsets[HeaderOffsetSelect] + NibbleFieldOffset;       */
   /* else if (Source-Select == PP-info), then this field does     */
   /* NOT affect the Start Nibble and in this case StartNibble     */
   /* = NibbleFieldOffset.                                         */
   /* range: 4:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD inst_header_offset_select[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS];

   /* Inst0_NiblleFieldOffset: If (Source-Select ==                */
   /* Packet-Header), then this field is represented in 2's        */
   /* complement and: StartNibble =                                */
   /* HeaderOffsets[HeaderOffsetSelect] + NibbleFieldOffset        */
   /* else if (Source-Select == PP-info), then this field is       */
   /* always is a positive value and: StartNibble =                */
   /* NibbleFieldOffset                                            */
   /* range: 12:5, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD inst_niblle_field_offset[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS];

   /* Inst0_BitCount: Concatenate to the LSB of the key            */
   /* (BitCount+1) bits.                                           */
   /* range: 17:13, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst_bit_count[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS];

   /* Inst0_LFEM_Program: Selects one of four LFEM programs        */
   /* range: 19:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD inst_lfem_program[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_INSTS];

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL;


  /* Lfem0 Field Select Map:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* Lfem0FieldSelectMap: Each line contains 16 commands,         */
   /* where each command is 6 bits width. The decode of each       */
   /* command is as follows: if command[5] is set then output      */
   /* is command[4:0] else output is In-Key[command[4:0]]          */
   /* range: 95:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD lfem_field_select_map[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_FLDS];

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL;


  /* RCL2 OFP: Recycle VSQ To OFP mapping. Used both for             */
  /* Recycle VSQ->SCH's HR FC and for Recycle VSQ->EGQ's OFP        */
  /* FC.                                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* OfpNum: Define the outgoing FAP port                         */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ofp_num;

   /* SchHpValid: If set, this OFP is valid for SCH OFP            */
   /* High-Priority FC                                             */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD sch_hp_valid;

   /* SchLpValid: If set, this OFP is valid for SCH OFP            */
   /* Low-Priority FC                                              */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD sch_lp_valid;

   /* EgqHpValid: If set, this OFP is valid for EGQ OFP            */
   /* High-Priority FC                                             */
   /* range: 9:9, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD egq_hp_valid;

   /* EgqLpValid: If set, this OFP is valid for EGQ OFP            */
   /* Low-Priority FC                                              */
   /* range: 10:10, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD egq_lp_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_CFC_RCL2_OFP_TBL;

  /* NIFCLSB2 OFP: Maping the 128 NIF class-based FC to EGQ's       */
  /* OFP FC signals. Used to generate both EGQ-OFP FC and            */
  /* SCH-OFP-HR FC. Each entry refers to 4 consecutive              */
  /* classes                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* OfpNum0: Defines the outgoing FAP port in the EGQ. Four      */
   /* FC indications are handled in each clock. This is the        */
   /* 1st OFP                                                      */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ofp_num[4];

   /* SchOfpHpValid0: If set, the FC indication is valid for       */
   /* SCH FC of OFP0.                                              */
   /* range: 28:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sch_ofp_hp_valid[4];

   /* SchOfpLpValid0: If set, the FC indication is valid for       */
   /* SCH FC of OFP0.                                              */
   /* range: 32:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sch_ofp_lp_valid[4];

   /* EgqOfpHpValid0: If set, the FC indication is valid for       */
   /* EGQ FC of OFP0.                                              */
   /* range: 36:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD egq_ofp_hp_valid[4];

   /* EgqOfpLpValid0: If set, the FC indication is valid for       */
   /* EGQ FC of OFP0.                                              */
   /* range: 40:40, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD egq_ofp_lp_valid[4];

} __ATTRIBUTE_PACKED__ SOC_PB_CFC_NIFCLSB2_OFP_TBL;

  /* CALRXA: Mapping of the SPI4 OOB-Rx received FC status          */
  /* frames to OFP-based or Sch-based FC indications. Note:*         */
  /* Used by interface A in the PADS                                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* FcIndex: Define the flow control index which is defined      */
   /* by the FcDestSel field: 0 - 79 indexes of the OFP-Based      */
   /* FCs(EGQ) 0 - 127 indexes of the SCH-Base FCs 0-63            */
   /* indexes of NIF's link-level (to stop NIF's TX on a           */
   /* specific link).                                              */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_index;

   /* FcDestSel: Defines the type of FC that FcIndex refers        */
   /* to: 0: - The calendar channel carries one of the 128         */
   /* SCH-Based FCs (for hierarchical HRs). 1: - The calendar      */
   /* channel carries one of the 80 HP OFP-based FCs. 2: - The     */
   /* calendar channel carries one of the 80 LP OFP-based FCs.     */
   /* 3: - The calendar channel carries one of the 64 NIF's        */
   /* link-level FCs.                                              */
   /* range: 8:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_dest_sel;

} __ATTRIBUTE_PACKED__ SOC_PB_CFC_CALRX_TBL;

  /* Oob0 Sch Map: Mapping of the 128 Scheduler-based FC            */
  /* received from the OOB interface to high and/or low             */
  /* priority OFP-HRs.                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00040000 */

   /* OfpHr: Defines the outgoing FAP port HR number.              */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ofp_hr;

   /* LpOfpValid: If set to one, then the relevant NIF-class       */
   /* number defines a Low-priority indication for OFP-HR.         */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lp_ofp_valid;

   /* HpOfpValid: If set to one, then the relevant NIF-class       */
   /* number defines a High-priority indication for OFP-HR.        */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD hp_ofp_valid;

} __ATTRIBUTE_PACKED__ SOC_PB_CFC_OOB_SCH_MAP_TBL;

  /* ILKN CALTX: Mapping of the ILKN-TX calendar channels to       */
  /* the possible generated FC indications. Note:* Used for          */
  /* ILKN-Port 0 InBand Flow-Control TX only.                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000b0000 */

   /* FcIndex: The index to one of the generated FC                */
   /* indications that should be mapped to current calendar        */
   /* channel. The FC source is defined by the FcSourceSel         */
   /* field. For the High/Low-p global source option, the          */
   /* decoding is as follows: 0: FC is due to Free-BDB counter     */
   /* 1: FC is due to Free-Unicast-Dbuff counter 2: FC is due      */
   /* to Free-Full-Mc-Dbuff counter 3: Not used                    */
   /* range: 8:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_index;

   /* FcSourceSel: Defines the type of FC that the FcIndex         */
   /* refers to: 0: The index refers to one of the 356 STE         */
   /* VSQs. 1: The index refers to one of the 64 Link-Level FC     */
   /* (from the NIF-RX). 2: The index refers to one of the 3       */
   /* High-priority Global resource FCs. 3: The index refers       */
   /* to one of the 3 Low-priority Global resource FCs. 4: The     */
   /* index refers to one of the 64 CNM LL FCs 5: The index        */
   /* refers to one of the 128 CNM CLBS FCs 6: Undefined 7:        */
   /* Force FC indication                                          */
   /* range: 11:9, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD fc_source_sel;

} __ATTRIBUTE_PACKED__ SOC_PB_CFC_CALTX_TBL;


/* Block definition: OLP 	 */
typedef struct
{
  SOC_PETRA_OLP_PGE_MEM_TBL pge_mem_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_OLP_TBLS;

/* Block definition: IRE 	 */
typedef struct
{
  SOC_PB_IRE_NIF_CTXT_MAP_TBL nif_ctxt_map_tbl;

  SOC_PB_IRE_NIF_PORT_TO_CTXT_BIT_MAP_TBL nif_port_to_ctxt_bit_map_tbl;

  SOC_PB_IRE_RCY_CTXT_MAP_TBL rcy_ctxt_map_tbl;

  SOC_PB_IRE_TDM_CONFIG_TBL tdm_config_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_IRE_TBLS;

/* Block definition: IDR 	 */
typedef struct
{
  SOC_PB_IDR_CONTEXT_MRU_TBL context_mru_tbl;

  SOC_PETRA_IDR_COMPLETE_PC_TBL complete_pc_tbl;

  SOC_PB_IDR_PCB_LINK_TBL pcb_link_tbl;

  SOC_PB_IDR_ETHERNET_METER_PROFILES_TBL ethernet_meter_profiles_tbl;

  SOC_PB_IDR_ETHERNET_METER_STATUS_TBL ethernet_meter_status_tbl;

  SOC_PB_IDR_ETHERNET_METER_CONFIG_TBL ethernet_meter_config_tbl;

  SOC_PB_IDR_GLOBAL_METER_PROFILES_TBL global_meter_profiles_tbl;

  SOC_PB_IDR_GLOBAL_METER_STATUS_TBL global_meter_status_tbl;

  SOC_PB_RAW_TBL dbuff_pointer_cache[6];

} __ATTRIBUTE_PACKED__ SOC_PB_IDR_TBLS;

/* Block definition: IRR */
typedef struct
{
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL is_ingress_replication_db_tbl;
  
  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL ingress_replication_multicast_db_tbl;

  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL egress_replication_multicast_db_tbl;

  SOC_PB_IRR_MCDB_EGRESS_FORMAT_B_TBL mcdb_egress_format_b_tbl;

  SOC_PB_IRR_MCDB_EGRESS_FORMAT_C_TBL mcdb_egress_format_c_tbl;

  SOC_PB_IRR_SNOOP_MIRROR_TABLE0_TBL snoop_mirror_table0_tbl;

  SOC_PB_IRR_SNOOP_MIRROR_TABLE1_TBL snoop_mirror_table1_tbl;

  SOC_PB_IRR_FREE_PCB_MEMORY_TBL free_pcb_memory_tbl;

  SOC_PB_IRR_PCB_LINK_TABLE_TBL pcb_link_table_tbl;

  SOC_PB_IRR_IS_FREE_PCB_MEMORY_TBL is_free_pcb_memory_tbl;

  SOC_PB_IRR_IS_PCB_LINK_TABLE_TBL is_pcb_link_table_tbl;

  SOC_PB_IRR_RPF_MEMORY_TBL rpf_memory_tbl;

  SOC_PB_IRR_MCR_MEMORY_TBL mcr_memory_tbl;

  SOC_PB_IRR_ISF_MEMORY_TBL isf_memory_tbl;

  SOC_PB_IRR_DESTINATION_TABLE_TBL destination_table_tbl;

  SOC_PB_IRR_RSQ_FIFO_TBL rsq_fifo_tbl;

  SOC_PB_IRR_LAG_TO_LAG_RANGE_TBL lag_to_lag_range_tbl;

  SOC_PB_IRR_LAG_MAPPING_TBL lag_mapping_tbl;

  SOC_PB_IRR_LAG_NEXT_MEMBER_TBL lag_next_member_tbl;

  SOC_PB_IRR_SMOOTH_DIVISION_TBL smooth_division_tbl;

  SOC_PB_IRR_TRAFFIC_CLASS_MAPPING_TBL traffic_class_mapping_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_IRR_TBLS;

/* Block definition: IHP */
typedef struct
{
  SOC_PB_IHP_PORT_MINE_TABLE_LAG_PORT_TBL port_mine_table_lag_port_tbl;

  SOC_PB_IHP_TM_PORT_PP_PORT_CONFIG_TBL tm_port_pp_port_config_tbl;

  SOC_PB_IHP_TM_PORT_SYS_PORT_CONFIG_TBL tm_port_sys_port_config_tbl;

  SOC_PB_IHP_PP_PORT_VALUES_TBL pp_port_values_tbl;

  SOC_PB_IHP_ISEM_KEYT_H0_TBL isem_keyt_h0_tbl;

  SOC_PB_IHP_ISEM_KEYT_H1_TBL isem_keyt_h1_tbl;

  SOC_PB_IHP_ISEM_KEYT_AUX_TBL isem_keyt_aux_tbl;

  SOC_PB_IHP_ISEM_PLDT_H0_TBL isem_pldt_h0_tbl;

  SOC_PB_IHP_ISEM_PLDT_H1_TBL isem_pldt_h1_tbl;

  SOC_PB_IHP_ISEM_PLDT_AUX_TBL isem_pldt_aux_tbl;

  SOC_PB_IHP_ISEM_MGMT_MEM_A_TBL isem_mgmt_mem_a_tbl;

  SOC_PB_IHP_ISEM_MGMT_MEM_B_TBL isem_mgmt_mem_b_tbl;

  SOC_PB_IHP_EM_KEYT_H0_TBL isem_em_keyt_h0_tbl;

  SOC_PB_IHP_EM_KEYT_H1_TBL isem_em_keyt_h1_tbl;

  SOC_PB_IHP_EM_KEYT_AUX_TBL isem_em_keyt_aux_tbl;

  SOC_PB_IHP_EM_PLDT_H0 em_pldt_h0_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_IHP_TBLS;

/* Block definition: QDR 	 */
typedef struct
{
  SOC_PETRA_QDR_MEM_TBL qdr_mem;

  SOC_PETRA_QDR_QDR_DLL_MEM_TBL qdr_dll_mem;

} __ATTRIBUTE_PACKED__ SOC_PB_QDR_TBLS;

/* Block definition: MMU */
typedef struct
{
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL dram_address_space_tbl;

  SOC_PETRA_MMU_IDF_TBL idf_tbl;

  SOC_PETRA_MMU_FDF_TBL fdf_tbl;

  SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL waf_halfa_waddr_tbl;

  SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL waf_halfb_waddr_tbl;

  SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL wafa_halfa_raddr_status_tbl;

  SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL wafb_halfa_raddr_status_tbl;

  SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL wafc_halfa_raddr_status_tbl;

  SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL wafd_halfa_raddr_status_tbl;

  SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL wafe_halfa_raddr_status_tbl;

  SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL waff_halfa_raddr_status_tbl;

  SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL wafa_halfb_raddr_status_tbl;

  SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL wafb_halfb_raddr_status_tbl;

  SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL wafc_halfb_raddr_status_tbl;

  SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL wafd_halfb_raddr_status_tbl;

  SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL wafe_halfb_raddr_status_tbl;

  SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL waff_halfb_raddr_status_tbl;

  SOC_PETRA_MMU_RAF_WADDR_TBL raf_waddr_tbl;

  SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL rafa_raddr_status_tbl;

  SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL rafb_raddr_status_tbl;

  SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL rafc_raddr_status_tbl;

  SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL rafd_raddr_status_tbl;

  SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL rafe_raddr_status_tbl;

  SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL raff_raddr_status_tbl;

  SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL rdfa_waddr_status_tbl;

  SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL rdfb_waddr_status_tbl;

  SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL rdfc_waddr_status_tbl;

  SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL rdfd_waddr_status_tbl;

  SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL rdfe_waddr_status_tbl;

  SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL rdff_waddr_status_tbl;

  SOC_PETRA_MMU_RDF_RADDR_TBL rdf_raddr_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_MMU_TBLS;

/* Block definition: IQM */
typedef struct
{
  SOC_PETRA_IQM_BDB_LINK_LIST_TBL bdb_link_list_tbl;

  SOC_PETRA_IQM_DYNAMIC_TBL dynamic_tbl;

  SOC_PETRA_IQM_STATIC_TBL static_tbl;

  SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL packet_queue_tail_pointer_tbl;

  SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL packet_queue_red_weight_table_tbl;

  SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL credit_discount_table_tbl;

  SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL full_user_count_memory_tbl;

  SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL mini_multicast_user_count_memory_tbl;

  SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL packet_queue_red_parameters_table_tbl;

  SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL packet_descriptor_fifos_memory_tbl;

  SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL tx_descriptor_fifos_memory_tbl;

  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL vsq_descriptor_rate_class_group_a_tbl;

  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL vsq_descriptor_rate_class_group_b_tbl;

  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL vsq_descriptor_rate_class_group_c_tbl;

  SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL vsq_descriptor_rate_class_group_d_tbl;

  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL vsq_qsize_memory_group_a_tbl;

  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL vsq_qsize_memory_group_b_tbl;

  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL vsq_qsize_memory_group_c_tbl;

  SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL vsq_qsize_memory_group_d_tbl;

  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL vsq_average_qsize_memory_group_a_tbl;

  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL vsq_average_qsize_memory_group_b_tbl;

  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL vsq_average_qsize_memory_group_c_tbl;

  SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL vsq_average_qsize_memory_group_d_tbl;

  SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL vsq_flow_control_parameters_table_group_tbl[SOC_PETRA_NOF_VSQ_GROUPS];

  SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL vsq_queue_parameters_table_group_tbl[SOC_PETRA_NOF_VSQ_GROUPS];

  SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL system_red_parameters_table_tbl;

  SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL system_red_drop_probability_values_tbl;

  SOC_PETRA_IQM_SYSTEM_RED_TBL system_red_tbl;

  SOC_PB_IQM_CNM_DESCRIPTOR_STATIC_TBL cnm_descriptor_static_tbl;

  SOC_PB_IQM_CNM_DESCRIPTOR_DYNAMIC_TBL cnm_descriptor_dynamic_tbl;

  SOC_PB_IQM_CNM_PARAMETERS_TABLE_TBL cnm_parameters_table_tbl;

  SOC_PB_IQM_METER_PROCESSOR_RESULT_RESOLVE_TABLE_STATIC_TBL meter_processor_result_resolve_table_static_tbl;

  SOC_PB_IQM_PRFSEL_TBL prfsel_tbl[SOC_PB_IQM_PRFSEL_NOF_TBLS];

  SOC_PB_IQM_PRFCFG_TBL prfcfg_tbl[SOC_PB_IQM_PRFCFG_NOF_TBLS];

  SOC_PB_IQM_NORMAL_DYNAMICA_TBL normal_dynamica_tbl;

  SOC_PB_IQM_NORMAL_DYNAMICB_TBL normal_dynamicb_tbl;

  SOC_PB_IQM_HIGH_DYNAMICA_TBL high_dynamica_tbl;

  SOC_PB_IQM_HIGH_DYNAMICB_TBL high_dynamicb_tbl;

  SOC_PB_IQM_CNTS_MEM_TBL cnts_mem_tbl[SOC_PB_CHIP_REGS_NOF_CNT_PROCESSOR_IDS];
  
  SOC_PB_IQM_OVTH_MEMA_TBL ovth_mema_tbl;

  SOC_PB_IQM_OVTH_MEMB_TBL ovth_memb_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_IQM_TBLS;

/* Block definition: IPS 	 */
typedef struct
{
  SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL system_physical_port_lookup_table_tbl;

  SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL destination_device_and_port_lookup_table_tbl;

  SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL flow_id_lookup_table_tbl;

  SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL queue_type_lookup_table_tbl;

  SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL queue_priority_map_select_tbl;

  SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL queue_priority_maps_table_tbl;

  SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL queue_size_based_thresholds_table_tbl;

  SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL credit_balance_based_thresholds_table_tbl;

  SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL empty_queue_credit_balance_table_tbl;

  SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL credit_watchdog_thresholds_table_tbl;

  SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL queue_descriptor_table_tbl;

  SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL queue_size_table_tbl;

  SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL system_red_max_queue_size_table_tbl;

  SOC_PETRA_IPS_FMS_MSG_MEM_TABLE_TBL fms_msg_mem_table_tbl;

  SOC_PETRA_IPS_FMS_TIMESTAMP_TABLE_TBL fms_timestamp_table_tbl;

  SOC_PETRA_IPS_ENQ_BLOCK_QNUM_TABLE_TBL enq_block_qnum_table_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_IPS_TBLS;

/* Block definition: IPT 	 */
typedef struct
{
  SOC_PETRA_IPT_BDQ_TBL bdq_tbl;

  SOC_PETRA_IPT_PCQ_TBL pcq_tbl;

  SOC_PB_IPT_SOP_MMU_TBL sop_mmu_tbl;

  SOC_PETRA_IPT_MOP_MMU_TBL mop_mmu_tbl;

  SOC_PETRA_IPT_FDTCTL_TBL fdtctl_tbl;

  SOC_PETRA_IPT_FDTDATA_TBL fdtdata_tbl;

  SOC_PETRA_IPT_EGQCTL_TBL egqctl_tbl;

  SOC_PETRA_IPT_EGQDATA_TBL egqdata_tbl;

  SOC_PB_IPT_EGQ_TXQ_WR_ADDR_TBL egq_txq_wr_addr_tbl;

  SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL select_source_sum_tbl;

  SOC_PB_IPT_EGQ_TXQ_RD_ADDR_TBL egq_txq_rd_addr_tbl;

  SOC_PB_IPT_FDT_TXQ_WR_ADDR_TBL fdt_txq_wr_addr_tbl;

  SOC_PB_IPT_FDT_TXQ_RD_ADDR_TBL fdt_txq_rd_addr_tbl;

  SOC_PB_IPT_GCI_BACKOFF_MASK_TBL gci_backoff_mask_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_IPT_TBLS;

/* Block definition: DPI 	 */
typedef struct
{
  SOC_PETRA_DPI_DLL_RAM_TBL dll_ram_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_DPI_TBLS;

/* Block definition: FDT 	 */
typedef struct
{
  SOC_PB_FDT_IPT_CONTRO_L_FIFO_TBL ipt_contro_l_fifo_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_FDT_TBLS;

/* Block definition: NBI 	 */
typedef struct
{
  SOC_PB_RAW_TBL mlf_rx_tbl[16];

  SOC_PB_RAW_TBL mlf_tx_tbl[16];

  SOC_PB_RAW_TBL mlf_rx_eob;

  SOC_PB_RAW_TBL mlf_tx_eob;

} __ATTRIBUTE_PACKED__ SOC_PB_NBI_TBLS;

/* Block definition: RTP */
typedef struct
{
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL unicast_distribution_memory_for_data_cells_tbl;

  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL unicast_distribution_memory_for_control_cells_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_RTP_TBLS;

/* Block definition: EGQ */
typedef struct
{
  SOC_PETRA_EGQ_NIF_SCM_TBL nif_scm_tbl;
  SOC_PETRA_EGQ_RCY_SCM_TBL rcy_scm_tbl;

  SOC_PETRA_EGQ_CPU_SCM_TBL cpu_scm_tbl;

  SOC_PETRA_EGQ_CCM_TBL ccm_tbl;

  SOC_PETRA_EGQ_PMC_TBL pmc_tbl;

  SOC_PETRA_EGQ_CBM_TBL cbm_tbl;

  SOC_PETRA_EGQ_FBM_TBL fbm_tbl;

  SOC_PETRA_EGQ_FDM_TBL fdm_tbl;

  SOC_PB_EGQ_QM_TBL qm_tbl;

  SOC_PB_EGQ_QSM_TBL qsm_tbl;

  SOC_PB_EGQ_DCM_TBL dcm_tbl;

  SOC_PB_EGQ_DWM_NEW_TBL dwm_new_tbl;

  SOC_PETRA_EGQ_DWM_TBL dwm_tbl;

  SOC_PETRA_EGQ_RRDM_TBL rrdm_tbl;

  SOC_PETRA_EGQ_RPDM_TBL rpdm_tbl;

  SOC_PB_EGQ_PCT_TBL pct_tbl;

  SOC_PETRA_EGQ_VLAN_TABLE_TBL vlan_table_tbl;

  SOC_PB_EGQ_TC_DP_MAP_TBL tc_dp_map_tbl;

  SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL cfc_flow_control_tbl;

  SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL nifa_flow_control_tbl;

  SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL nifb_flow_control_tbl;

  SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL cpu_last_header_tbl;

  SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL ipt_last_header_tbl;

  SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL fdr_last_header_tbl;

  SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL cpu_packet_counter_tbl;

  SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL ipt_packet_counter_tbl;

  SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL fdr_packet_counter_tbl;

  SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL rqp_packet_counter_tbl;

  SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL rqp_discard_packet_counter_tbl;

  SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL ehp_unicast_packet_counter_tbl;

  SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL ehp_multicast_high_packet_counter_tbl;

  SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL ehp_multicast_low_packet_counter_tbl;

  SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL ehp_discard_packet_counter_tbl;

  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL pqp_unicast_high_packet_counter_tbl;

  SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL pqp_unicast_low_packet_counter_tbl;

  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL pqp_multicast_high_packet_counter_tbl;

  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL pqp_multicast_low_packet_counter_tbl;

  SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL pqp_unicast_high_bytes_counter_tbl;

  SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL pqp_unicast_low_bytes_counter_tbl;

  SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL pqp_multicast_high_bytes_counter_tbl;

  SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL pqp_multicast_low_bytes_counter_tbl;

  SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL pqp_discard_unicast_packet_counter_tbl;

  SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL pqp_discard_multicast_packet_counter_tbl;

  SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL fqp_packet_counter_tbl;

  SOC_PB_EGQ_FQP_NIF_PORT_MUX_TBL fqp_nif_port_mux_tbl;

  SOC_PB_EGQ_KEY_PROFILE_MAP_INDEX_TBL key_profile_map_index_tbl;

  SOC_PB_EGQ_TCAM_KEY_RESOLUTION_PROFILE_TBL tcam_key_resolution_profile_tbl;

  SOC_PB_EGQ_PARSER_LAST_SYS_RECORD_TBL parser_last_sys_record_tbl;

  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD1_TBL parser_last_nwk_record1_tbl;

  SOC_PB_EGQ_PARSER_LAST_NWK_RECORD2_TBL parser_last_nwk_record2_tbl;

  SOC_PB_EGQ_ERPP_DEBUG_TBL erpp_debug_tbl;

  SOC_PB_EGQ_SVEM_KEYT_H0_TBL svem_keyt_h0_tbl;

  SOC_PB_EGQ_SVEM_KEYT_H1_TBL svem_keyt_h1_tbl;

  SOC_PB_EGQ_SVEM_KEYT_AUX_TBL svem_keyt_aux_tbl;

  SOC_PB_EGQ_SVEM_PLDT_H0_TBL svem_pldt_h0_tbl;

  SOC_PB_EGQ_SVEM_PLDT_H1_TBL svem_pldt_h1_tbl;

  SOC_PB_EGQ_SVEM_PLDT_AUX_TBL svem_pldt_aux_tbl;

  SOC_PB_EGQ_SVEM_MGMT_MEM_A_TBL svem_mgmt_mem_a_tbl;

  SOC_PB_EGQ_SVEM_MGMT_MEM_B_TBL svem_mgmt_mem_b_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_EGQ_TBLS;

/* Block definition: IHB  */
typedef struct
{
  SOC_PB_IHB_PINFO_LBP_TBL pinfo_lbp_tbl;

  SOC_PB_IHB_PINFO_PMF_TBL pinfo_pmf_tbl;

  SOC_PB_IHB_PACKET_FORMAT_CODE_PROFILE_TBL packet_format_code_profile_tbl;

  SOC_PB_IHB_SRC_DEST_PORT_FOR_L3_ACL_KEY_TBL src_dest_port_for_l3_acl_key_tbl;

  SOC_PB_IHB_DIRECT_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL direct_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_DIRECT_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL direct_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM0_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem0_4b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM0_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem0_4b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_INDEX_TABLE_TBL fem0_4b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM0_4B_1ST_PASS_MAP_TABLE_TBL fem0_4b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM1_4B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem1_4b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM1_4B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem1_4b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_INDEX_TABLE_TBL fem1_4b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM1_4B_1ST_PASS_MAP_TABLE_TBL fem1_4b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM2_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem2_14b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM2_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem2_14b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_INDEX_TABLE_TBL fem2_14b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM2_14B_1ST_PASS_MAP_TABLE_TBL fem2_14b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM2_14B_1ST_PASS_OFFSET_TABLE_TBL fem2_14b_1st_pass_offset_table_tbl;

  SOC_PB_IHB_FEM3_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem3_14b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM3_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem3_14b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_INDEX_TABLE_TBL fem3_14b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM3_14B_1ST_PASS_MAP_TABLE_TBL fem3_14b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM3_14B_1ST_PASS_OFFSET_TABLE_TBL fem3_14b_1st_pass_offset_table_tbl;

  SOC_PB_IHB_FEM4_14B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem4_14b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM4_14B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem4_14b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_INDEX_TABLE_TBL fem4_14b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM4_14B_1ST_PASS_MAP_TABLE_TBL fem4_14b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM4_14B_1ST_PASS_OFFSET_TABLE_TBL fem4_14b_1st_pass_offset_table_tbl;

  SOC_PB_IHB_FEM5_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem5_17b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM5_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem5_17b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_INDEX_TABLE_TBL fem5_17b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM5_17B_1ST_PASS_MAP_TABLE_TBL fem5_17b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM5_17B_1ST_PASS_OFFSET_TABLE_TBL fem5_17b_1st_pass_offset_table_tbl;

  SOC_PB_IHB_FEM6_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem6_17b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM6_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem6_17b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_INDEX_TABLE_TBL fem6_17b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM6_17B_1ST_PASS_MAP_TABLE_TBL fem6_17b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM6_17B_1ST_PASS_OFFSET_TABLE_TBL fem6_17b_1st_pass_offset_table_tbl;

  SOC_PB_IHB_FEM7_17B_1ST_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem7_17b_1st_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM7_17B_1ST_PASS_PROGRAM_RESOLVED_DATA_TBL fem7_17b_1st_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_INDEX_TABLE_TBL fem7_17b_1st_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM7_17B_1ST_PASS_MAP_TABLE_TBL fem7_17b_1st_pass_map_table_tbl;

  SOC_PB_IHB_FEM7_17B_1ST_PASS_OFFSET_TABLE_TBL fem7_17b_1st_pass_offset_table_tbl;

  SOC_PB_IHB_FEM0_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem0_4b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM0_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem0_4b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_INDEX_TABLE_TBL fem0_4b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM0_4B_2ND_PASS_MAP_TABLE_TBL fem0_4b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM1_4B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem1_4b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM1_4B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem1_4b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_INDEX_TABLE_TBL fem1_4b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM1_4B_2ND_PASS_MAP_TABLE_TBL fem1_4b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM2_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem2_14b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM2_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem2_14b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_INDEX_TABLE_TBL fem2_14b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM2_14B_2ND_PASS_MAP_TABLE_TBL fem2_14b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM2_14B_2ND_PASS_OFFSET_TABLE_TBL fem2_14b_2nd_pass_offset_table_tbl;

  SOC_PB_IHB_FEM3_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem3_14b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM3_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem3_14b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_INDEX_TABLE_TBL fem3_14b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM3_14B_2ND_PASS_MAP_TABLE_TBL fem3_14b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM3_14B_2ND_PASS_OFFSET_TABLE_TBL fem3_14b_2nd_pass_offset_table_tbl;

  SOC_PB_IHB_FEM4_14B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem4_14b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM4_14B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem4_14b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_INDEX_TABLE_TBL fem4_14b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM4_14B_2ND_PASS_MAP_TABLE_TBL fem4_14b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM4_14B_2ND_PASS_OFFSET_TABLE_TBL fem4_14b_2nd_pass_offset_table_tbl;

  SOC_PB_IHB_FEM5_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem5_17b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM5_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem5_17b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_INDEX_TABLE_TBL fem5_17b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM5_17B_2ND_PASS_MAP_TABLE_TBL fem5_17b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM5_17B_2ND_PASS_OFFSET_TABLE_TBL fem5_17b_2nd_pass_offset_table_tbl;

  SOC_PB_IHB_FEM6_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem6_17b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM6_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem6_17b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_INDEX_TABLE_TBL fem6_17b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM6_17B_2ND_PASS_MAP_TABLE_TBL fem6_17b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM6_17B_2ND_PASS_OFFSET_TABLE_TBL fem6_17b_2nd_pass_offset_table_tbl;

  SOC_PB_IHB_FEM7_17B_2ND_PASS_KEY_PROFILE_RESOLVED_DATA_TBL fem7_17b_2nd_pass_key_profile_resolved_data_tbl;

  SOC_PB_IHB_FEM7_17B_2ND_PASS_PROGRAM_RESOLVED_DATA_TBL fem7_17b_2nd_pass_program_resolved_data_tbl;

  SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_INDEX_TABLE_TBL fem7_17b_2nd_pass_map_index_table_tbl;

  SOC_PB_IHB_FEM7_17B_2ND_PASS_MAP_TABLE_TBL fem7_17b_2nd_pass_map_table_tbl;

  SOC_PB_IHB_FEM7_17B_2ND_PASS_OFFSET_TABLE_TBL fem7_17b_2nd_pass_offset_table_tbl;

  SOC_PB_RAW_TBL elk_fifo_data[10];
  
} __ATTRIBUTE_PACKED__ SOC_PB_IHB_TBLS;

/* Block definition: EPNI 	 */
typedef struct
{
  SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL epe_packet_counter_tbl;

  SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL epe_bytes_counter_tbl;

  SOC_PB_EPNI_COPY_ENGINE_PROGRAM_TBL copy_engine_program_tbl[SOC_PB_EPNI_COPY_ENGINE_PROGRAM_NOF_TBLS];

  SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_TBL lfem_field_select_map_tbl[SOC_PB_EPNI_LFEM_FIELD_SELECT_MAP_NOF_TBLS];

  SOC_PB_EPNI_ESEM_KEYT_H0_TBL esem_keyt_h0_tbl;

  SOC_PB_EPNI_ESEM_KEYT_H1_TBL esem_keyt_h1_tbl;

  SOC_PB_EPNI_ESEM_KEYT_AUX_TBL esem_keyt_aux_tbl;

  SOC_PB_EPNI_ESEM_PLDT_H0_TBL esem_pldt_h0_tbl;

  SOC_PB_EPNI_ESEM_PLDT_H1_TBL esem_pldt_h1_tbl;

  SOC_PB_EPNI_ESEM_PLDT_AUX_TBL esem_pldt_aux_tbl;

  SOC_PB_EPNI_ESEM_MGMT_MEM_A_TBL esem_mgmt_mem_a_tbl;

  SOC_PB_EPNI_ESEM_MGMT_MEM_B_TBL esem_mgmt_mem_b_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_EPNI_TBLS;

/* Block definition: CFC */
typedef struct
{
  SOC_PB_CFC_RCL2_OFP_TBL rcl2_ofp_tbl;

  SOC_PB_CFC_NIFCLSB2_OFP_TBL nifclsb2_ofp_tbl;

  SOC_PB_CFC_CALRX_TBL calrxa_tbl;

  SOC_PB_CFC_CALRX_TBL calrxb_tbl;

  SOC_PB_CFC_OOB_SCH_MAP_TBL oob0_sch_map_tbl;

  SOC_PB_CFC_OOB_SCH_MAP_TBL oob1_sch_map_tbl;

  SOC_PB_CFC_CALTX_TBL caltx_tbl;

  SOC_PB_CFC_CALRX_TBL ilkn0_calrx_tbl;

  SOC_PB_CFC_CALRX_TBL ilkn1_calrx_tbl;

  SOC_PB_CFC_OOB_SCH_MAP_TBL ilkn0_sch_map_tbl;

  SOC_PB_CFC_OOB_SCH_MAP_TBL ilkn1_sch_map_tbl;

  SOC_PB_CFC_CALTX_TBL ilkn0_caltx_tbl;

  SOC_PB_CFC_CALTX_TBL ilkn1_caltx_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_CFC_TBLS;

/* Block definition: SCH */
typedef struct
{
  SOC_PETRA_SCH_CAL_TBL cal_tbl;

  SOC_PETRA_SCH_DRM_TBL drm_tbl;

  SOC_PETRA_SCH_DSM_TBL dsm_tbl;

  SOC_PETRA_SCH_FDMS_TBL fdms_tbl;

  SOC_PETRA_SCH_SHDS_TBL shds_tbl;

  SOC_PETRA_SCH_SEM_TBL sem_tbl;

  SOC_PETRA_SCH_FSF_TBL fsf_tbl;

  SOC_PETRA_SCH_FGM_TBL fgm_tbl;

  SOC_PETRA_SCH_SHC_TBL shc_tbl;

  SOC_PETRA_SCH_SCC_TBL scc_tbl;

  SOC_PETRA_SCH_SCT_TBL sct_tbl;

  SOC_PETRA_SCH_FQM_TBL fqm_tbl;

  SOC_PETRA_SCH_FFM_TBL ffm_tbl;

  SOC_PETRA_SCH_TMC_TBL soc_tmctbl;

  SOC_PETRA_SCH_PQS_TBL pqs_tbl;

  SOC_PETRA_SCH_SCHEDULER_INIT_TBL scheduler_init_tbl;

  SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL force_status_message_tbl;

} __ATTRIBUTE_PACKED__ SOC_PB_SCH_TBLS;

  /* Blocks definition } */

  typedef struct
{
  SOC_PB_OLP_TBLS olp;
  SOC_PB_IRE_TBLS ire;
  SOC_PB_IDR_TBLS idr;
  SOC_PB_IRR_TBLS irr;
  SOC_PB_IHP_TBLS ihp;
  SOC_PB_QDR_TBLS qdr;
  SOC_PB_MMU_TBLS mmu;
  SOC_PB_IQM_TBLS iqm;
  SOC_PB_IPS_TBLS ips;
  SOC_PB_IPT_TBLS ipt;
  SOC_PB_DPI_TBLS dpi;
  SOC_PB_FDT_TBLS fdt;
  SOC_PB_RTP_TBLS rtp;
  SOC_PB_EGQ_TBLS egq;
  SOC_PB_IHB_TBLS ihb;
  SOC_PB_EPNI_TBLS epni;
  SOC_PB_CFC_TBLS cfc;
  SOC_PB_SCH_TBLS sch;
  SOC_PB_NBI_TBLS nbi;
} __ATTRIBUTE_PACKED__ SOC_PB_TBLS;

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
/*****************************************************
*NAME
* soc_pb_tbls_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT SOC_PB_TBLS  **soc_pb_tbls - pointer to pb
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_pb_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_tbls_get(
    SOC_SAND_OUT SOC_PB_TBLS  **soc_pb_tbls
  );

/*****************************************************
*NAME
* soc_pb_tbls
*TYPE:
*  PROC
*FUNCTION:
*  Return a pointer to tables database.
*  The database is per chip-version. Without Error Checking
*INPUT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    *soc_pb_tbls.
*REMARKS:
*    No checking is performed whether the DB is initialized
*SEE ALSO:
*****************************************************/
SOC_PB_TBLS*
  soc_pb_tbls(void);

/*****************************************************
*NAME
*  soc_pb_tbls_init
*TYPE:
*  PROC
*FUNCTION:
*  Dynamically allocates and initializes Pb tables database.
*
*INPUT:
*  SOC_SAND_DIRECT:
*    None.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    None.
*REMARKS:
*   Calling upon already initialized tables will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  soc_pb_tbls_init(void);
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PB_CHIP_TBLS_INCLUDED__*/
#endif
