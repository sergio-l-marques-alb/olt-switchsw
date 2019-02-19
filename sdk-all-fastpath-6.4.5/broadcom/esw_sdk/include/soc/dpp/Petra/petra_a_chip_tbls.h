/* $Id: soc_petra_a_chip_tbls.h,v 1.6 Broadcom SDK $
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

#ifndef __SOC_PA_CHIP_TBLS_INCLUDED__
/* { */
#define __SOC_PA_CHIP_TBLS_INCLUDED__

/* { */

/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>

#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/SAND/SAND_FM/sand_indirect_access.h>
#include <soc/dpp/Petra/petra_chip_defines.h>

/* } */

/*************
 * DEFINES   *
 *************/
/* { */

/* $Id: soc_petra_a_chip_tbls.h,v 1.6 Broadcom SDK $
 * In the system red mechanism, we divide the queues to 16 ranges,
 * according to the thresholds. and each ranges receives a value of
 * 4 bit (0-15), this represents the queue in the system red test.
 */
#define SOC_PETRA_SYS_RED_NOF_Q_RNGS          16

#define SOC_PETRA_NOF_VSQ_GROUPS              4


#define SOC_PA_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL 4

#ifdef LINK_PB_LIBRARIES
#define SOC_PB_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL 8
#else
#define SOC_PB_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL SOC_PA_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL
#endif

#define SOC_PETRA_MULT_ING_REP_TBL_NOF_FLDS_PER_DEVICE \
  SOC_PETRA_CST_VALUE_DISTINCT(MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL, uint32)


#define SOC_PETRA_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL 8

#define SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE         8
#define SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS        2
#define SOC_PETRA_IHP_KEY_NOF_PROGS                   5
#define SOC_PETRA_IHP_KEY_PROG_NOF_INSTR              2
#define SOC_PETRA_IHP_KEY_PROG_INSTR_SIZE             13
#define SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS            4
#define SOC_PETRA_IHP_PRGR_COS_TBL_FLDS               16


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

  /* PgeMem:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* PgeMem: Packet Generation Engine (PGE) Memory.               */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pge_mem;

} __ATTRIBUTE_PACKED__ SOC_PETRA_OLP_PGE_MEM_TBL;


  /* Nif Ctxt Map: 512 entries. Each 64 entries represent 64        */
  /* incoming channels per ingress MAC lane (MAC lane 0 uses        */
  /* indexes 0-63, MAC lane 1 uses indexes 64-127, ...). SGMII       */
  /* interfaces use four predetermined addresses (0,16,32,48)       */
  /* per MAC lane. Non-SGMII interfaces can use any channel         */
  /* within the range. Each entry binds the incoming NIF            */
  /* port/channel to a FAP port. Write 7'd127 to an invalid          */
  /* NIF port/channel.                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* FapPort: Corresponding Incoming FAP port.                    */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fap_port;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_NIF_CTXT_MAP_TBL;


  /* Nif Port2ctxt Bit Map: Eight entries. Each entry               */
  /* represents, per MAC lane, a bit map of the associated          */
  /* Incoming FAP ports.0-3 - NIFA MACs.4-7 - NIFB MACs.            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* ContextsBitMapping: Bitmap of FAP ports associated with      */
   /* the MAC lane.                                                */
   /* range: 79:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD contexts_bit_mapping;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL;


  /* Rcy Ctxt Map: Egress Recycling channel to incoming FAP         */
  /* Port (Context) mapping. Write 7'd127 to an invalid              */
  /* Recycling channel.                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* FapPort: Corresponding Incoming FAP port.                    */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fap_port;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_RCY_CTXT_MAP_TBL;

typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* : CPU received packet counter. If bit 64 is set, counter     */
   /* overflowed.                                                  */
   /* range: 64:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

  /* : CPU received packet counter. If bit 64 is set, counter     */
  /* overflowed.                                                  */
  /* range: 64:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL;


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

  /* : CPU received packet counter. If bit 64 is set, counter     */
  /* overflowed.                                                  */
  /* range: 64:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

  /* : CPU received packet counter. If bit 64 is set, counter     */
  /* overflowed.                                                  */
  /* range: 64:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL;

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

  /* : CPU received packet counter. If bit 64 is set, counter     */
  /* overflowed.                                                  */
  /* range: 64:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL;

  /* Complete PC: Complete PC list. Interface is for debug          */
  /* only.                                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* PcbPointer: Pointer to complete PC                           */
   /* range: 8:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD pcb_pointer;

   /* Count: Number of DPs used by the PC                          */
   /* range: 12:9, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD count;

   /* ECC: ECC                                                     */
   /* range: 17:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ecc;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IDR_COMPLETE_PC_TBL;


  /* Is Ingress Replication DB: 8k double entries. For each         */
  /* Multicast ID, this register indicates whether ingress          */
  /* replication processing is required. If not, the Queue          */
  /* resolution is derived from the class field and the             */
  /* packet is queued to one of the Spatial Multicast queues        */
  /* 0 .. 3. Entry i in the database holds the indication of        */
  /* Multicast IDs 4i - 4i+3                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* IsIngressReplication[indx]: If set, the corresponding       */
   /* Multicast ID 4i+indx requires ingress replication                 */
   /* range: 0:0, access type: RW, default value: 0x0              */
   /* range: 1:1, access type: RW, default value: 0x0              */
   /* range: 2:2, access type: RW, default value: 0x0              */
   /* range: 3:3, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD is_ingress_replication[SOC_PETRA_MULT_ING_NOF_MC_IDS_PER_ENTRY_ING_REP_TBL];

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL;


  /* Multicast Replication Database: 32k Ingress Multicast          */
/* Replication Data Base. (Physically shared with egress          */
/* MLL table). Each multicast ID can result in up to 4k            */
/* different copies of the incoming packet at the ingress         */
/* FAP. Each copy is associated with a configurable               */
/* Queue/destination System port and an OutLif. Each entry        */
/* in this table can define 0-2 such copies, and points to        */
/* the next entry in the linked list. The OutLif can be            */
/* further used at the egress FOP as a key for Egress             */
/* Multicast lookup and can be used by egress Packet              */
/* Processor.                                                     */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

  /* BaseQueue1: Queue/System port of first copy. Value of        */
  /* 15'h3FFF indicates an invalid entry, for which no copy       */
  /* will be generated.                                           */
  /* range: 14:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD base_queue1;

  /* IsQueueNumber1: First Copy-If set, the next 15 bits          */
  /* represents a unicast queue; otherwise, system port ID.       */
  /* range: 15:15, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD is_queue_number1;

  /* BaseQueue2: Queue/System port of second copy. Value of       */
  /* 15'h3FFF indicates an invalid entry- for which no copy       */
  /* will be generated.                                           */
  /* range: 30:16, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD base_queue2;

  /* IsQueueNumber2: Second Copy-If set, the next 15 bits         */
  /* represents a unicast queue; otherwise, system port ID.       */
  /* range: 31:31, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD is_queue_number2;

  /* OutLif1: Logical Interface of the first copy.                */
  /* range: 47:32, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_lif1;

  /* OutLif2: Logical Interface of the second copy.               */
  /* range: 63:48, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_lif2;

  /* EgressData: Not used.                                        */
  /* range: 68:64, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD egress_data;

  /* LinkPtr: Pointer to the next entry. Value of 15'b0           */
  /* indicates the end of the list.                               */
  /* range: 83:69, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD link_ptr;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL;



/* Egress Replication Multicast DB: 32k Ingress Multicast        */
/* Replication Data Base. (Physically shared with ingress        */
/* MLL table). Each multicast ID can result in up to 4k           */
/* different copies of the incoming packet at the egress         */
/* FAP. Each copy is associated with a configurable              */
/* port and an OutLif. Each entry in this table can define 0-3   */
/* such copies, and points to the next entry in the linked list. */
/* The OutLif can be further used at the egress FOP as a key for */
/* Egress multicast lookup and can be used by egress Packet      */
/* Processor.                                                    */

typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

  /* OutLif3: Logical Interface of the third copy.               */
  /* range: 15:0, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_lif3;

  /* OutPort3: Third Copy - represents an outgoing FAP port       */
  /* range: 22:16, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_port3;

  /* OutLif2: Logical Interface of the third copy.                */
  /* range: 38:23, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_lif2;

  /* OutPort2: Third Copy - represents an outgoing FAP port       */
  /* range: 45:39, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_port2;

  /* OutLif1: Logical Interface of the third copy.                */
  /* range: 61:46, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_lif1;

  /* OutPort1: Third Copy - represents an outgoing FAP port       */
  /* range: 68:62, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD out_port1;

  /* LinkPtr: Pointer to the next entry. Value of 15'b0           */
  /* indicates the end of the list.                               */
  /* range: 83:69, access type: RW, default value: 0x0            */
  SOC_PETRA_TBL_FIELD link_ptr;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL;



  /* Mirror Table: 80 entry Mirror table. This memory maps          */
  /* the incoming port ID to a port mirroring configuration,        */
  /* indicating whether this port is inbound or outbound and        */
  /* the queue/System port. It also holds various other             */
  /* attributes of the mirrored copy.                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* MirrorDestination:                                           */
   /* range: 14:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD mirror_destination;

   /* IsMulticast: If set, MirrorDestination represents a           */
   /* multicast destination. otherwise, a system port ID.          */
   /* if destination represents a multicast ID, bits 13:0          */
   /* represent a multicast ID, bit 14 is TRUE and bit 15          */
   /* is FALSE.                                                    */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_multicast;

   /* IsQueueNumber: If set MirrorDestination represents           */
   /* unicast queue,otherwise system port ID                       */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_queue_number;

   /* IsOutboundMirror: Indicates, for each FAP port, whether      */
   /* it is outbound mirrored.                                     */
   /* range: 16:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_outbound_mirror;

   /* MirrorTrafficClass: if the TrafficClassOverWrite bit is      */
   /* set this value overwrites the original traffic class.        */
   /* range: 19:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mirror_traffic_class;

   /* TrafficClassOverWrite:                                       */
   /* range: 20:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD traffic_class_over_write;

   /* MirrorDropPrecedence: If the DropPrecedenceOverWrite bit     */
   /* is set this value overwrites the original drop               */
   /* precedence.                                                  */
   /* range: 22:21, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mirror_drop_precedence;

   /* DropPrecedenceOverWrite:                                     */
   /* range: 23:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drop_precedence_over_write;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_MIRROR_TABLE_TBL;


  /* Snoop Table: 16 entry Snoop Table. This memory maps the        */
  /* 4b snoop CMD to a snoop queue/System port, and holds           */
  /* various other attributes of the snooped copy.                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00060000 */

   /* SnoopDestination:                                            */
   /* range: 14:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD snoop_destination;

   /* IsMulticast: If set, SnoopDestination represents a           */
   /* multicast destination. otherwise, a system port ID.          */
   /* if destination represents a multicast ID, bits 13:0          */
   /* represent a multicast ID, bit 14 is TRUE and bit 15          */
   /* is FALSE.                                                    */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_multicast;

   /* IsQueueNumber: If set, SnoopDestination represents a         */
   /* unicast queue; otherwise, a system port ID.                  */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD is_queue_number;

   /* SnoopTrafficClass: If the TrafficClassOverWrite bit is       */
   /* set, this value overwrites the original traffic class.       */
   /* range: 19:17, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD snoop_traffic_class;

   /* TrafficClassOverWrite:                                       */
   /* range: 20:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD traffic_class_over_write;

   /* SnoopDropPrecedence: If the DropPrecedenceOverWrite bit      */
   /* is set, this value overwrites the original drop              */
   /* precedence.                                                  */
   /* range: 22:21, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD snoop_drop_precedence;

   /* DropPrecedenceOverWrite:                                     */
   /* range: 23:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drop_precedence_over_write;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_SNOOP_TABLE_TBL;


  /* Glag To Lag Range:                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000f0000 */

   /* GlagToLagRange: GLAG range memory table                      */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD glag_to_lag_range;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL;


  /* Smooth Division:                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* SmoothDivision: RLAG and GLAG offset memory table            */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD smooth_division;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_SMOOTH_DIVISION_TBL;


  /* Glag Mapping:                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

   /* GlagMapping: GLAG outlif mapping table                       */
   /* range: 16:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD glag_mapping;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_GLAG_MAPPING_TBL;


  /* Destination Table: This memory maps outlif to queue            */
  /* number with valid queue bit                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00300000 */

   /* QueueNumber: The queue number to which the outlif refers     */
   /* to.                                                          */
   /* range: 14:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD queue_number;

   /* QueueValid: If set the queue is valid.                       */
   /* range: 15:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD queue_valid;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_DESTINATION_TABLE_TBL;


  /* Glag Next Member: For GLAG resolution, this memory is          */
  /* read, and if it is in round robin load balanced mode           */
  /* (RrLbMode), the output received is the offset                  */
  /* corresponding to the GLAG. Note that in this case, the         */
  /* offset is incremented by one and written back to the           */
  /* memory.                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00400000 */

   /* offset: The glag member.                                     */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD offset;

   /* RrLbMode: round robin load balance mode.                     */
   /* range: 4:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD rr_lb_mode;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL;


  /* Rlag Next Member: For RLAG resolution, this memory is          */
  /* read, and if it is in round robin load balanced mode,          */
  /* the output received is the offset corresponding to the         */
  /* RLAG. Note that in this case, the offset is incremented        */
  /* by one and written back to the memory.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00500000 */

   /* TwoLsb: If it is an rlag4 entry, these two bits are the      */
   /* rlag member. Otherwise, if it is an rlag2 entry, the lsb     */
   /* is the next member and the msb is the round robin load       */
   /* balance mode.                                                */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD two_lsb;

   /* TwoMsb: If it is an rlag4 entry, only the lsb is valid       */
   /* and represents the round robin load balance mode.            */
   /* Otherwise if it is an rlag2 entry, the lsb is the next       */
   /* member and the msb is the round robin load balance mode.     */
   /* range: 3:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD two_msb;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL;


  /* Port Info: Port info table (one of three)                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000e0000 */

   /* PortType: May be one of the following 3'b000 - Reserved      */
   /* 3'b001 - Ethernet 3'b010 - RAW 3'b011 - TM 3'b100 -          */
   /* 3'b111 - Programmable                                        */
   /* range: 2:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD port_type;

   /* BytesToSkip: Number of bytes to skip from start of           */
   /* packet untill start of L2 header before parsing the          */
   /* packet. For example, this value may account for the          */
   /* sequence number and statistics tag, if they exist.           */
   /* range: 7:3, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD bytes_to_skip;

   /* CustomCommandSelect: Selects one of two sets of custom       */
   /* command for PTC (packet type classifier).                    */
   /* range: 8:8, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD custom_command_select;

   /* StatisticsProfileExtension: This is an extension to the      */
   /* statistics profile. It determines the number of nibbles      */
   /* (0 to 3) from the statistics resolved data that will be      */
   /* added to the lsb of the statistics tag.                      */
   /* range: 10:9, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD statistics_profile_extension;

   /* PortTypeExtension: For TM type ports: Specifies whether      */
   /* this port has an ITMH extension. For other ports this        */
   /* bit selects one of two possible PTC programs.                */
   /* range: 22:22, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD port_type_extension;

   /* StatisticsProfile: Selects one of 32 statistics              */
   /* profiles. The bits determine which fields will be added      */
   /* to the tag: bit 0 - If set, then the CID is added (valid     */
   /* only for Ethernet type ports) bit 1 - If set, then the       */
   /* FAP port is added bit 2 - If set, then the class is          */
   /* added bit 3 - If set, then the drop precedence is added      */
   /* bit 4 - If set, then the a UC/BC/MC indication is added      */
   /* (UC is 01; BC is 10; MC is 11) The data is added to the      */
   /* LSB of the statistics tag with the following order:          */
   /* (msb) [UC/BC/MC][DP][class][FAP-port][CID] (lsb)             */
   /* range: 27:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD statistics_profile;

   /* UseLagMember: If set and the resolved forwarding             */
   /* destination is a LAG, then it also specifies the LAG         */
   /* member to use.                                               */
   /* range: 28:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD use_lag_member;

   /* HasMirror: If set, then the port should be inbound           */
   /* mirrored.                                                    */
   /* range: 29:29, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD has_mirror;

   /* MirrorIsMc: If set, then when this port is mirrored, it      */
   /* will be mirrored to a multicast destination (and             */
   /* therefore multicast dbuffs are needed).                      */
   /* range: 30:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD mirror_is_mc;

   /* BytesToRemove: Number of fixed bytes to remove from          */
   /* start of header. For TM, Ethernet and raw ports this         */
   /* value accounts for the sequence number and statistics        */
   /* tag if exist.                                                */
   /* range: 35:31, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD bytes_to_remove;

   /* HeaderRemove: If set then the TM header is removed. For      */
   /* Ethernet and Raw ports, where the processed TM header is     */
   /* internally generated this value should be set to 0.          */
   /* range: 36:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD header_remove;

   /* AppendFtmh: If set then the FTMH is appended to the          */
   /* packet.                                                      */
   /* range: 37:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD append_ftmh;

   /* AppendProgHeader: If set then the programmable generated     */
   /* ITMH is appended to the packet (immediately after the        */
   /* FTMH). For Ethernet ports this value must be 0.              */
   /* range: 38:38, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD append_prog_header;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PORT_INFO_TBL;


  /* Port To System Port Id:                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00120000 */

   /* PortToSystemPortId: System port ID for each FAP port         */
   /* range: 12:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD port_to_system_port_id;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL;


  /* Static Header: Per-port static header for raw type             */
  /* ports. First byte of header is always at MSB (I.e. byte        */
  /* 79)For Ethernet ports: if bit 0 is set, then bits 79:48        */
  /* are an ingress shaping header for this port.                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00130000 */

   /* Shaping: If set then this port is ingress shaped             */
   /* (ignored for TM type ports)                                  */
   /* range: 0:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD shaping;

   /* ShapingItmh: The ingress shaping header. Valid only if       */
   /* Shaped is set.                                               */
   /* range: 32:1, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD shaping_itmh;

   /* Itmh: The ITMH for the packet. Valid only for raw type       */
   /* ports.                                                       */
   /* range: 80:33, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD itmh;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_STATIC_HEADER_TBL;


  /* System Port My Port Table:                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00140000 */

   /* SystemPortMyPortTable: Specifies for each system port if     */
   /* it belongs to the current device. Each row in memory         */
   /* corresponds to 4 ports (The 2 lsb of the system port         */
   /* select the column)                                           */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD system_port_my_port_table[SOC_PETRA_IHP_SYS_MY_PORT_TBL_FLDS];

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL;


  /* Ptc Commands1:                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00500000 */

   /* PtcCommands1: PTC command memory (first)                     */
   /* range: 39:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD ptc_commands;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PTC_COMMANDS1_TBL;


  /* Ptc Commands2:                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00510000 */

   /* PtcCommands2: PTC command memory (second)                    */
   /* range: 39:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD ptc_commands;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PTC_COMMANDS2_TBL;


  /* Ptc Key Program Lut: converts header stack index (I.e.         */
  /* ptc commands address) to key program parameters;Each           */
  /* line contains four sets of values;The input into the           */
  /* table is \{header_stack[10:5], header_stack[2:0]\}             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00520000 */

   /* PtcKeyProgramPtr: key program pointer                        */
   /* range: 5:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD ptc_key_program_ptr;

   /* PtcKeyProgramVar: key program variable                       */
   /* range: 13:6, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD ptc_key_program_var;

   /* PtcCosProfile: CoS profile                                   */
   /* range: 17:14, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ptc_cos_profile;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL;

  /* Key Program: Programmable port key program memory -           */
  /* instructions 0 and 1                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00530000 -  0x00570000*/

   /* Select:                                                      */
   /* range: 1:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD select[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* Length:                                                      */
   /* range: 4:2, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD length[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* NibbleShift:                                                 */
   /* range: 5:5, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD nibble_shift[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* ByteShift:                                                   */
   /* range: 10:6, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD byte_shift[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* OffsetSelect:                                                */
   /* range: 12:11, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD offset_select[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_KEY_PROGRAM_TBL;


  /* Key Program0: Programmable port key program memory -           */
  /* instructions 0 and 1                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00530000 */

   /* Select:                                                      */
   /* range: 1:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD select[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* Length:                                                      */
   /* range: 4:2, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD length[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* NibbleShift:                                                 */
   /* range: 5:5, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD nibble_shift[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* ByteShift:                                                   */
   /* range: 10:6, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD byte_shift[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

   /* OffsetSelect:                                                */
   /* range: 12:11, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD offset_select[SOC_PETRA_IHP_KEY_PROG_NOF_INSTR];

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_KEY_PROGRAM0_TBL;


  /* Key Program1:                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00540000 */

   /* KeyProgram1: Programmable port key program memory -          */
   /* instructions 2 and 3                                         */
   /* range: 25:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_program1;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_KEY_PROGRAM1_TBL;


  /* Key Program2: Programmable port key program memory -           */
  /* instructions 4 and 5                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00550000 */

   /* Instruction4:                                                */
   /* range: 12:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD instruction4;

   /* Instruction3:                                                */
   /* range: 25:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD instruction3;

   /* ExactMatchEnable:                                            */
   /* range: 26:26, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD exact_match_enable;

   /* ExactMatchMaskIndex:                                         */
   /* range: 28:27, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD exact_match_mask_index;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_KEY_PROGRAM2_TBL;


  /* Key Program3:                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00560000 */

   /* KeyProgram3: Programmable port key program memory -          */
   /* instructions 6 and 7                                         */
   /* range: 25:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD key_program3;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_KEY_PROGRAM3_TBL;


  /* Key Program4: Programmable port key program memory -           */
  /* instructions 8 and 9                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00570000 */

   /* Instruction9:                                                */
   /* range: 12:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD instruction9;

   /* Instruction10:                                               */
   /* range: 25:13, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD instruction10;

   /* TcamMatchEnable:                                             */
   /* range: 26:26, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tcam_match_enable;

   /* TcamMatchSelect: 1'b0 - 72bit 1'b1 - 144bit                  */
   /* range: 27:27, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tcam_match_select;

   /* TcamKeyAndValue: The 4-lsb of the tcam key are equal to      */
   /* key[3:0] & TcamKeyAndValue | TcamKeyOrValue                  */
   /* range: 31:28, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tcam_key_and_value;

   /* TcamKeyOrValue:                                              */
   /* range: 35:32, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD tcam_key_or_value;

   /* BytesToRemoveHdrSel: Select one of three header offsets      */
   /* (or 0) as base for headerremove                              */
   /* range: 37:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD bytes_to_remove_hdr_sel;

   /* BytesToRemoveHdrSize: Number of bytes to remove in           */
   /* addition to the selected header.                             */
   /* range: 42:38, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD bytes_to_remove_hdr_size;

   /* CosHdrVarMaskSelect: Selects one of two mask for the CoS     */
   /* header variable                                              */
   /* range: 44:43, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD cos_hdr_var_mask_select;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_KEY_PROGRAM4_TBL;


  /* Programmable Cos: Programmable port CoS lookup table.          */
  /* Input into table is the CoS-Hdr-Var(8b) variable. Each         */
  /* row holds 16 sets of CoS parameters for each value of          */
  /* CoS-Profile.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00580000 */

   /* DropPrecedence:                                              */
   /* range: 1:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD drop_precedence[SOC_PETRA_IHP_PRGR_COS_TBL_FLDS];

   /* TrafficClass:                                                */
   /* range: 4:2, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD traffic_class[SOC_PETRA_IHP_PRGR_COS_TBL_FLDS];

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL;


  /* Programmable Cos1:                                             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00620000 */

   /* ProgrammableCos1: The second programmable cos table.         */
   /* Each entry contains two values \{TrafficClass[2:0],          */
   /* DropPrecedence[1:0]\}. Entry into table is                   */
   /* CoS-Fwd-Var[2:0]. Note that CoS-Fwd-Var is never 3'b111.     */
   /* Each line in the table contains 16 entries (for all          */
   /* values of CoS-Profile).                                      */
   /* range: 79:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD programmable_cos1;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL;


  /* BDB Link List:                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* BDB Link List: BDB Link List BDB-ptr[15:0]                   */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD bdb_link_list;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_BDB_LINK_LIST_TBL;


  /* Packet Queue Descriptor (Dynamic ): Packet Queue               */
  /* Descriptor Dynamic fields                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* PqHeadPtr: Packet Q head pointer                             */
   /* (BbdPointer[15:0],BdbOffset[5:0]) The \{BDB,Offset\}         */
   /* where the next packet should be read from.                   */
   /* range: 21:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pq_head_ptr;

   /* QueNotEmpty: 0: Q is empty 1: Q has data                     */
   /* range: 22:22, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD que_not_empty;

   /* PqInstQueSize: Q Instantaneous Size - 16 byte resolution     */
   /* range: 46:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_inst_que_size;

   /* PqAvrgSzie: Q average size. Value= Avrg[6:0]<<Avrg[11:7]     */
   /* Used for WRED reject tests.                                  */
   /* range: 58:47, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_avrg_szie;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_DYNAMIC_TBL;


  /* Packet Queue Descriptor (Static): Packet Queue                 */
  /* Descriptor Static bits                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

   /* CreditClass: Selects one of 16 possible credit discount      */
   /* classes                                                      */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD credit_class;

   /* RateClass: Select sone of 64 possible rate classes. Used     */
   /* to select some Q parameters.                                 */
   /* range: 9:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD rate_class;

   /* ConnectionClass: Selects one of 32 possible connection       */
   /* classes (i.e. source or destination).                        */
   /* range: 14:10, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD connection_class;

   /* TrafficClass: Selects one of 8 possible traffic classes.     */
   /* range: 17:15, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD traffic_class;

   /* QueSignature: Signature key for the Q. Could be used as      */
   /* Forward-Action, reported to the IPT                          */
   /* range: 19:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD que_signature;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_STATIC_TBL;


  /* Packet Queue Tail Pointer: Packet Queue Tail pointerThis       */
  /* is the \{BDB,Offset\} whre the last packet Dbuff has           */
  /* been written.                                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00300000 */

   /* TailPtr: Tail pointer: \{ BDB[15:0],BDB-Ofset[5:0]\}         */
   /* range: 21:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD tail_ptr;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_PACKET_QUEUE_TAIL_POINTER_TBL;


  /* Packet Queue Red Weight table: Packet Queue Average            */
  /* WeightAccessed according to the Queue's Rate-Class[5:0].       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00400000 */

   /* PqWeight: Packet Queue Average Exponential Weight            */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD pq_weight;

   /* AvrgEn: If set, enables updating of the Average Qsize        */
   /* memory.                                                      */
   /* range: 6:6, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD avrg_en;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_PACKET_QUEUE_RED_WEIGHT_TABLE_TBL;


  /* Credit Discount table: Credit DiscountA value that is          */
  /* discounted from the packet size on packet Dequeue. (per        */
  /* Q's credit-class)                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00500000 */

   /* CrdtDiscVal: Credit Discount value (bytes)                   */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD crdt_disc_val;

   /* DiscntSign: If set then Discount is negative                 */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD discnt_sign;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_CREDIT_DISCOUNT_TABLE_TBL;


  /* Full User Count Memory: Full Multicast User CountNumber        */
  /* of duplications of the FullMulticast Dbuffs (UserCnt+1).       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00600000 */

   /* FlusCnt: Full Multicast User Count                           */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD flus_cnt;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_FULL_USER_COUNT_MEMORY_TBL;


  /* Mini-Multicast User Count Memory: Mini Multicast User          */
  /* CountNumber of duplications of the MiniMulticast Dbuffs        */
  /* (UserCnt+1).                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00700000 */

   /* MnUsCnt: Mini-Multicast User Count. Each line contains       */
   /* 32 concecutive User-count values.                            */
   /* range: 63:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD mn_us_cnt;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_MINI_MULTICAST_USER_COUNT_MEMORY_TBL;


  /* Packet Queue Red parameters table: Packet Queue Red            */
  /* parametersPer Q Rate Class and Drop-p.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00800000 */


   /* Tail Drop threshold. Maximum instantaneouse Qsize (16-bytes)  */
   /* Exponent of Maximum-Instantaneous-Queue-Size. Max-size =       */
   /* MaxSizeTh = Val[5:0]<<Val[10:6]                               */
   /* range: 5:0, access type: RW, default value: 0x0               */
   SOC_PETRA_TBL_FIELD pq_max_que_size_mnt;

   /* Mantissa of Maximum-Instantaneous-Queue-Size. Max-size =      */
   /* (mantissa + 1) << exponent.                                 */
   /* range: 10:6, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_max_que_size_exp;


   /* PqWredEn: WRED enable for the packet Q. If de-asserted       */
   /* the PacketQ RED test always give 'accept' answer.            */
   /* range: 11:11, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_wred_en;

   /* PqC2: RED parameter. C2=MaxProb*MinTh/(MaxTh-MinTh)          */
   /* range: 43:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_c2;

   /* PqC3: RED parameter. C3=-log2(1/MaxPcktSize)                 */
   /* range: 47:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_c3;

   /* PqC1: RED parameter. C1= log2(MaxProb/(MaxTh-MinTh))         */
   /* MaxProb is normalized to 2^32 value. MaxProb=                */
   /* (MaxPr(%)/100)*2^32                                          */
   /* range: 52:48, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_c1;

   /* PqAvrgMaxTh: PacketQ max threshold value (RED) Th =          */
   /* Val[6:0] << Val[11:7]                                        */
   /* range: 64:53, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_avrg_max_th;

   /* PqAvrgMinTh: PacketQ min threshold value (RED) Th =          */
   /* Val[6:0] << Val[11:7]                                        */
   /* range: 76:65, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_avrg_min_th;

   /* PqWredPcktSzIgnr: if set, then WRED logic ignores packet     */
   /* size when calculating acket drop probability. Otherwise,     */
   /* bigger packet size increase drop probability.                */
   /* range: 77:77, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD pq_wred_pckt_sz_ignr;
   /* AddmitLogic: Selects one of 4 templates for processing       */
   /* the 8 accept/reject signals generated by testing the         */
   /* packet against the various statistics.                       */
   /* range: 79:78, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD addmit_logic;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_PACKET_QUEUE_RED_PARAMETERS_TABLE_TBL;

  /* Packet Descriptor Fifos Memory: Tx PD-FIFO fifos memory.       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00900000 */

   /* TxPd: Packet Descriptor                                      */
   /* range: 29:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD tx_pd;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_PACKET_DESCRIPTOR_FIFOS_MEMORY_TBL;


  /* Tx Descriptor Fifos Memory: Tx Descriptor-FIFO fifos           */
  /* memory                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00a00000 */

   /* TxDscr: Tx Descriptor                                        */
   /* range: 84:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD tx_dscr;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_TX_DESCRIPTOR_FIFOS_MEMORY_TBL;


  /* VSQ Descriptor Rate Class - group A: Virtual Queue             */
  /* Descriptor Rate ClassVSQs : 0->3                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01100000 */

   /* VsqRcA: Rate class. Used to access parameter tables.         */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vsq_rc_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_A_TBL;


  /* VSQ Descriptor Rate Class - group B: Virtual Queue             */
  /* Descriptor Rate ClassVSQs : 4->35                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01200000 */

   /* VsqRcB: Rate class. Used to access parameter tables.         */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vsq_rc_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_B_TBL;


  /* VSQ Descriptor Rate Class - group C: Virtual Queue             */
  /* Descriptor Rate Class                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01300000 */

   /* VsqRcC: Rate class. Used to access parameter tables.         */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vsq_rc_c;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_C_TBL;


  /* VSQ Descriptor Rate Class - group D: Virtual Queue             */
  /* Descriptor Rate Class                                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01400000 */

   /* VsqRcD: Rate class. Used to access parameter tables.         */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD vsq_rc_d;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_DESCRIPTOR_RATE_CLASS_GROUP_D_TBL;


  /* VSQ Qsize memory - group A: VSQs 0->3, Instentaneouse          */
  /* size memory.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01500000 */

   /* VsqSizeWrds: VSQ size in 16-bytes resolution.                */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_size_wrds;

   /* VsqSizeBds: VSQ size counted according to consumed BDs       */
   /* (QDR entrees).                                               */
   /* range: 45:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vsq_size_bds;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_A_TBL;


  /* VSQ Qsize memory - group B: VSQs 4->35, Instentaneouse         */
  /* size memory.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01600000 */

   /* VsqSizeWrds: VSQ size in 16-bytes resolution.                */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_size_wrds;

   /* VsqSizeBds: VSQ size counted according to consumed BDs       */
   /* (QDR entrees).                                               */
   /* range: 45:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vsq_size_bds;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_B_TBL;


  /* VSQ Qsize memory - group C: VSQs 36->99, Instentaneouse        */
  /* size memory.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01700000 */

   /* VsqSizeWrds: VSQ size in 16-bytes resolution.                */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_size_wrds;

   /* VsqSizeBds: VSQ size counted according to consumed BDs       */
   /* (QDR entrees).                                               */
   /* range: 45:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vsq_size_bds;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_C_TBL;


  /* VSQ Qsize memory - group D: VSQs 100->355,                     */
  /* Instentaneouse size memory.                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01800000 */

   /* VsqSizeWrds: VSQ size in 16-bytes resolution.                */
   /* range: 23:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_size_wrds;

   /* VsqSizeBds: VSQ size counted according to consumed BDs       */
   /* (QDR entrees).                                               */
   /* range: 45:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vsq_size_bds;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_QSIZE_MEMORY_GROUP_D_TBL;


  /* VSQ Average Qsize memory - group A: VSQs : 0->3 ,Average       */
  /* Size in 16-bytes resolution                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01900000 */

   /* VsqAvrgSize: Average = Man[6:0]<<Exp[4:0]                    */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_avrg_size;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_A_TBL;


  /* VSQ Average Qsize memory - group B: VSQs : 4->35               */
  /* ,Average Size in 16-bytes resolution.                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01a00000 */

   /* VsqAvrgSize: Average = Man[6:0]<<Exp[4:0]                    */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_avrg_size;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_B_TBL;


  /* VSQ Average Qsize memory - group C: VSQs : 36->99              */
  /* ,Average Size in 16-bytes resolution.                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01b00000 */

   /* VsqAvrgSize: Average = Man[6:0]<<Exp[4:0]                    */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_avrg_size;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_C_TBL;


  /* VSQ Average Qsize memory - group D: VSQs : 100->355            */
  /* ,Average Size in 16-bytes resolution.                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x01c00000 */

   /* VsqAvrgSize: Average = Man[6:0]<<Exp[4:0]                    */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD vsq_avrg_size;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_AVERAGE_QSIZE_MEMORY_GROUP_D_TBL;


  /* VSQ Flow-Control Parameters table -                          */
  /* group A: Flow Control parameters (Per Rate-Class[3:0])VSQs : */
  /*  0->3                                                        */
  /* group B: Flow Control parameters (Per Rate-Class[3:0])VSQs : */
  /*  3->35                                                       */
  /* group C: Flow Control parameters (Per Rate-Class[3:0])VSQs : */
  /*  36->99                                                      */
  /* group D: Flow Control parameters (Per Rate-Class[3:0])VSQs : */
  /*  100->355                                                   */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;    /* 0x01d00000 */
                          /* 0x01e00000 */
                          /* 0x01f00000 */
                          /* 0x02000000 */
   /* WredEn: Enable WRED reject test                              */
   /* range: 0:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD wred_en;

   /* AvrgSizeEn: Enable update of the VSQ average size            */
   /* range: 1:1, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD avrg_size_en;

   /* RedWeightQ: Exponential queue size averaging weight          */
   /* range: 7:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD red_weight_q;

   /* SetThresholdWords: Threshold over which the flow control     */
   /* of the virtual queue is asserted. Value is compared to       */
   /* instantaneous size. FCSetTh[4:0]<< FCSetTh[9:5]              */
   /* range: 12:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD set_threshold_words_exp;
   /* range: 17:13, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD set_threshold_words_mnt;

   /* ClearThresholdWords: Threshold below which the flow          */
   /* control of the virtual queue is deasserted. Value is         */
   /* compared to instantaneous size.                              */
   /* range: 22:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD clear_threshold_words_exp;
   /* range: 27:23, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD clear_threshold_words_mnt;

   /* SetThresholdBd: Threshold over which the flow control of     */
   /* the virtual queue is asserted. Value is compared VSQ in      */
   /* consumed BDs size. FCSetTh[3:0]<< FCSetTh[8:4]               */
   /* range: 31:28, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD set_threshold_bd_exp;
   /* range: 36:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD set_threshold_bd_mnt;

   /* ClearThresholdBd: Threshold below which the flow control     */
   /* of the virtual queue is deasserted. Value is compared to     */
   /* VSQ in consumed BDs size.                                    */
   /* range: 40:37, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD clear_threshold_bd_exp;
   /* range: 45:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD clear_threshold_bd_mnt;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_FLOW_CONTROL_PARAMETERS_TABLE_GROUP_TBL;


  /* VSQ Queue Parameters table - group A: Virtual Q                */
  /* parameters (Per Rate-Class[3:0],Drop-p[1:0])Tail Drop          */
  /* thresholds &Red ParametersP-Drop(red) =                        */
  /* (c1*AvgSize-c2)*PcktSize*c3VSQs : 0->3                         */
  /* VSQ Queue Parameters table - group B: 4->35                    */
  /* VSQ Queue Parameters table - group C: 36->99                   */
  /* VSQ Queue Parameters table - group D: 100->355                 */

typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02100000 */
                          /* 0x02200000 */
                          /* 0x02300000 */
                          /* 0x02400000 */

   /* C2: Red Parameter. C2=MaxProb*MinTh/(MaxTh-MinTh)            */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD c2;

   /* C3: Red Parameter. C3=1/MaxPcktSize                          */
   /* range: 35:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD c3;

   /* C1: Red Parameter. C1= log2(MaxProb/(MaxTh-MinTh))           */
   /* range: 40:36, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD c1;

   /* MaxAvrgTh: Maximum average-size threshold (RED) MaxTh =      */
   /* Val[6:0]<<Val[11:7] Range: 0 -> 2^12                         */
   /* range: 52:41, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD max_avrg_th;

   /* MinAvrgTh: Maximum average-size threshold (RED) MinTh =      */
   /* Val[6:0]<<Val[11:7] Range: 0 -> 2^12                         */
   /* range: 64:53, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD min_avrg_th;

   /* VqWredPcktSzIgnr: if set, then WRED logic ignores packet     */
   /* size when calculating acket drop probability. Otherwise,     */
   /* bigger packet size increase drop probability.                */
   /* range: 65:65, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vq_wred_pckt_sz_ignr;
   /* VqMaxSzieBds: Tail drop queue size in BDs threshold.         */
   /* tTh[6:0]<< Th[10:7]                                          */
   /* range: 75:65, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vq_max_szie_bds_mnt;
   SOC_PETRA_TBL_FIELD vq_max_szie_bds_exp;

   /* VqMaxSizeWords: Tail drop queue size threshold (in           */
   /* 16-bytes units). tTh[6:0]<< Th[11:7]                         */
   /* range: 87:76, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD vq_max_size_words_mnt;
   SOC_PETRA_TBL_FIELD vq_max_size_words_exp;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_VSQ_QUEUE_PARAMETERS_TABLE_GROUP_TBL;


  /* System Red parameters table: System Red parameters:            */
  /* defines the reject test parameters (per Q rate-class and       */
  /* Drop-p).                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02500000 */

   /* AdmTh: Admit Threshold: Threshold below which packets        */
   /* are admitted                                                 */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD adm_th;

   /* ProbTh: Above AdmTh and below ProbTh drop with               */
   /* probability read from Ssytem-Red-Probability table           */
   /* according to DrpPobIndx1.                                    */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD prob_th;

   /* DrpTh: Drop threshold: above this threshold packet is        */
   /* always dropped. Above ProbTh and below DrpTh drop with       */
   /* probability read from Ssytem-Red-Probability table           */
   /* according to DrpPobIndx2.                                    */
   /* range: 11:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD drp_th;

   /* DrpProbIndx1: Drop probability index. Used when Qsize        */
   /* range is between AdmTh and ProbTh. (index to drop-p          */
   /* table)                                                       */
   /* range: 15:12, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drp_prob_indx1;

   /* DrpProbIndx2: Drop probabililty index. Used when Qsize       */
   /* range is between ProbTh and DrpTh. (index to drop-p          */
   /* table)                                                       */
   /* range: 19:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD drp_prob_indx2;

   /* SredEn: Sytem-RED enable. If set, then system red            */
   /* discard is enabled, otherwise, the system-red discard        */
   /* test is ignored.                                             */
   /* range: 20:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sys_red_en;
} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_SYSTEM_RED_PARAMETERS_TABLE_TBL;


  /* System Red drop probability values: System Red drop            */
  /* probability values.                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02600000 */

   /* DrpProb: Drop-probability , compared to a 16-bit LFSR.       */
   /* Note: The drop-probability will be (1-DrpProb)/(64K-1).      */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD drp_prob;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_SYSTEM_RED_DROP_PROBABILITY_VALUES_TBL;


  /* Source Qsize range thresholds (System Red): Source Q           */
  /* size range thresholds. Defines 16 ranges of the                 */
  /* instantaneous Qsize, that define the 4 bit System-Q-size       */
  /* value. Qsize below QszRngThN is defined with                    */
  /* source-Q-size N.                                               */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x02700000 */

   /* QszRngTh0-14: Qsize below this value translated to              */
   /* Source-Q-size 0. Value: QszRngTh[6:0]<<QszRndTh[11:7]        */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD qsz_rng_th[SOC_PETRA_SYS_RED_NOF_Q_RNGS];

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_SYSTEM_RED_TBL;



  /* QDR DATA.                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* DATA             */
   /* range: 31:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD data;

} __ATTRIBUTE_PACKED__ SOC_PETRA_QDR_MEM_TBL;

  /* Qdr Dll Mem:                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x20000000 */

   /* QdrDllMem: Master slave memory access through indirect       */
   /* range: 27:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD qdr_dll_mem;

} __ATTRIBUTE_PACKED__ SOC_PETRA_QDR_QDR_DLL_MEM_TBL;

  /* System Physical Port Lookup Table: Queue to system             */
  /* physical port- Every 4 queues are mapped to same system        */
  /* physical port.                                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* sys_phy_port: System physical port                           */
   /* range: 11:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sys_phy_port;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_SYSTEM_PHYSICAL_PORT_LOOKUP_TABLE_TBL;


  /* Destination Device And Port Lookup Table: System               */
  /* physical port to destination device/port table.                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00008000 */

   /* dest_port: Destination port                                  */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD dest_port;

   /* dest_dev: Destination device                                 */
   /* range: 18:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD dest_dev;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_DESTINATION_DEVICE_AND_PORT_LOOKUP_TABLE_TBL;


  /* Flow Id Lookup Table: Queue to egress base flow mapping-       */
  /* Every 4 queues are mapped to a single base flow and            */
  /* subflow mode. Details are explained under                       */
  /* InterdigitatedMode register.                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* base_flow: Base flow                                         */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD base_flow;

   /* subFlowMode: Sub flow mode                                   */
   /* range: 14:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sub_flow_mode;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_FLOW_ID_LOOKUP_TABLE_TBL;


  /* Queue Type Lookup Table:                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00018000 */

   /* QueueTypeLookupTable: Each queue is mapped to a queue        */
   /* type, to determine various thresholds and parameters         */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD queue_type_lookup_table;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_QUEUE_TYPE_LOOKUP_TABLE_TBL;


  /* Queue Priority Map Select:                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* QueuePriorityMapSelect: Every 64 contiguous queues are       */
   /* assigned one of four 64 bit bitmaps.                         */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD queue_priority_map_select;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_QUEUE_PRIORITY_MAP_SELECT_TBL;


  /* Queue Priority Maps Table:                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00028000 */

   /* QueuePriorityMapsTable: The bit position in the bitmap       */
   /* corresponding to the queue determines the priority.          */
   /* range: 63:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD queue_priority_maps_table;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_QUEUE_PRIORITY_MAPS_TABLE_TBL;


  /* Queue Size-Based Thresholds Table: The values in this          */
  /* table affect the credit request state (CRS) according to       */
  /* the difference between Q size and credit balance. Values       */
  /* for all thresholds are given in terms of mantissa and          */
  /* exponent, with a sign bit. The value of the threshold is       */
  /* (M x 2^E), where M=TH[6:4] ,E=TH[3:0] and TH[7] is the         */
  /* sign bit. All values are in 1B resolution                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* OffToSlowMsgTH: when (Qsize-CrBal) > OffToSlowMsgTH, and     */
   /* CRS==OFF, CRS may be changed to SLOW.                        */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD off_to_slow_msg_th;

   /* OffToNormMsgTH: When (Qsize-CrBal) > offToNormMsgTH, and     */
   /* CRS==OFF, CRS may be changed to NORM.                        */
   /* range: 15:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD off_to_norm_msg_th;

   /* SlowToNormMsgTH: When (Qsize-CrBal) > SlowToNormMsgTH,       */
   /* and CRS==SLOW, CRS may be changed to NORM.                   */
   /* range: 23:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD slow_to_norm_msg_th;

   /* NormToSlowMsgTH: When (Qsize-CrBal) < NormToSlowMsgTH,       */
   /* and CRS==NORM, CRS may be changed to SLOW.                   */
   /* range: 31:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD norm_to_slow_msg_th;

   /* FsmThMul: When queue size crosses N*(1<<FsmThMul+4) up,      */
   /* or (N*(1<<FsmThMul+4) - (1<<(FsmThMul+3)) down, a new        */
   /* FSM is generated (if CRS is not OFF). Value of 0             */
   /* disables this mechanism. Value should be configured          */
   /* between 1 to 8.                                              */
   /* range: 35:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fsm_th_mul;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_QUEUE_SIZE_BASED_THRESHOLDS_TABLE_TBL;


  /* Credit Balance Based Thresholds Table: The values in           */
  /* this table affect the credit request state (CRS)               */
  /* according to the credit balance (or the difference             */
  /* between it and the Q size). Values for all thresholds          */
  /* are given in terms of mantissa and exponent. The value         */
  /* of the threshold is (M x 2^E), where M=TH[7:4] and             */
  /* E=TH[3:0]. All values are in 1B resolution.                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00038000 */

   /* BackoffEnterQCrBalTh: When CrBal > BackoffEnterQCrBalTH      */
   /* , CRS is changed to OFF.                                     */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD backoff_enter_qcr_bal_th;

   /* BackoffExitQCrBalTh: When CrBal < BackoffExitBalTH CRS       */
   /* may be changed from OFF (according to other parameters).     */
   /* range: 15:8, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD backoff_exit_qcr_bal_th;

   /* BacklogEnterQCrBalTh: When (CrBal-Qsize) >                   */
   /* BacklogEnterQCrBalTH, CRS is changed to OFF.                 */
   /* range: 23:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD backlog_enter_qcr_bal_th;

   /* BacklogExitQCrBalTh: When (CrBal-Qsize) <                    */
   /* BacklogExitQCrBalTH CRS may be changed from OFF              */
   /* (according to other parameters).                             */
   /* range: 31:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD backlog_exit_qcr_bal_th;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_CREDIT_BALANCE_BASED_THRESHOLDS_TABLE_TBL;


  /* Empty Queue Credit Balance Table: The values in this           */
  /* table relate to the credit balance of an empty Q, and          */
  /* affect its latency and credit waste. NOTE: values given        */
  /* here are in 2's complement                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00040000 */

   /* EmptyQSatisfiedCrBal: When Qsize==0 and CrBal >=             */
   /* EmptyQSatisfiedCrBal, CRS is changed to OFF.                 */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD empty_qsatisfied_cr_bal;

   /* MaxEmptyQCrBal: Maximum value for queues credit balance      */
   /* when Qsize==0- A queue with CRS==OFF and Qsize==0 will       */
   /* continue to accumulate credits, if received, up to           */
   /* MaxEmptyQCrBal.                                              */
   /* range: 31:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD max_empty_qcr_bal;

   /* ExceedMaxEmptyQCrBal: when asserted, credit balance of       */
   /* an empty queue can exceed configured MaxEmptyQCrBal up       */
   /* to (CrValue-1), when a credit is received. This is used      */
   /* to prevent the IPS from deleting partial credits. NOTE:      */
   /* when this bit is set, the IPS will not delete excess         */
   /* credits when CRS changed to OFF.                             */
   /* range: 32:32, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD exceed_max_empty_qcr_bal;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_EMPTY_QUEUE_CREDIT_BALANCE_TABLE_TBL;


  /* Credit Watchdog Thresholds Table: The values in this           */
  /* tables gives the thresholds for credit watchdog, in case       */
  /* a long time passed since the last credit for an active         */
  /* Q. the Values are given in WD full cycle units. A value        */
  /* of 0 disables the relevant TH.                                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00048000 */

   /* WdStatusMsgGenPeriod: When WDLastCreditTime >=               */
   /* WDStatusMsgGenPeriod, the Q is entered to the FSMRQ for      */
   /* a FSM generation.                                            */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD wd_status_msg_gen_period;

   /* WdDeleteQTh: When WDLastCreditTime >= WDDeleteQTH, the Q     */
   /* is put in delete state.                                      */
   /* range: 7:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD wd_delete_qth;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_CREDIT_WATCHDOG_THRESHOLDS_TABLE_TBL;


  /* Queue Descriptor Table: This memory holds most of the Q        */
  /* descriptor variables. Caution: This table is for debug          */
  /* purposes and should be used with user discretion as it         */
  /* can effect the IPS operation. Special consideration            */
  /* should be given when writing to this table.                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00050000 */

   /* CrBal: The queue's credit balance, given in 2's              */
   /* complement integer in resolution of 1B.                      */
   /* range: 15:0, access type: RW, default value: 0xffff          */
   SOC_PETRA_TBL_FIELD cr_bal;

   /* CRS: Credit request state: 0: OFF. 1: SLOW. 2: NORM.         */
   /* range: 17:16, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD crs;

   /* OnePktDeq: Indicates that the queue is one packet            */
   /* dequeue mode (Qsize>CrBal).                                  */
   /* range: 18:18, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD one_pkt_deq;

   /* WdLastCrTime: Time (in full WD cycles) at which last         */
   /* credit was received.                                         */
   /* range: 23:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD wd_last_cr_time;

   /* In_DQCQ: Indicates that the queue is in DQCQ or in one       */
   /* of the IQM's contexts.                                       */
   /* range: 24:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD in_dqcq;

   /* WdDelete: Indicates if the queue has being deleted           */
   /* (credit source lost).                                        */
   /* range: 25:25, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD wd_delete;

   /* fsmrq_ctrl: Indicates the state of the queue to the          */
   /* FSMRQ: b00: no message needed, queue not in FSMRQ b01:       */
   /* queue in FSMRQ waiting for SLOW/NORM message. b10: queue     */
   /* needed to enter the FSMRQ, but the FSMRQ was not ready       */
   /* b11: queue is in FSMRQ but a message was triggered by        */
   /* credit arrival. Discard message next time.                   */
   /* range: 27:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fsmrq_ctrl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_QUEUE_DESCRIPTOR_TABLE_TBL;


  /* Queue Size Table: Q size variable of the Q descriptor.         */
  /* The value is given in mantissa and exponent format,            */
  /* where M=q_size[6:4] and E=q_size[3:0]. The value is (M x       */
  /* 2^E). A value of -1 (0xFF00 in the register) means that        */
  /* the Q size is over the maximum of the IPS. The queue           */
  /* size is given in 16B resolution. Caution: This table is         */
  /* for debug purposes and should be used with user                */
  /* discretion as it can effect the IPS operation. Special         */
  /* consideration should be given when writing to this             */
  /* table.                                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00058000 */

   /* Exponent: The exponent value of the queue size               */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD exponent;

   /* Mantissa: The mantissa value of the queue size               */
   /* range: 6:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD mantissa;

   /* qsize_4b: as given by the IQM for system red feature         */
   /* range: 10:7, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD qsize_4b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_QUEUE_SIZE_TABLE_TBL;


  /* System Red Max Queue Size Table: Maximum queue size for        */
  /* system RED feature- One entry per system physical port         */
  /* reads the value received from credits.                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00078000 */

   /* maxqsz: Maximum queue size as received from incoming         */
   /* credits.                                                     */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD maxqsz;

   /* maxqsz_age: 2b representing aging period for entry.          */
   /* Value decremented every SystemRedAgePeriod*16k clocks        */
   /* down to 0. When reaches 0, value is expired and              */
   /* overwritten by 0.                                            */
   /* range: 5:4, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD maxqsz_age;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_SYSTEM_RED_MAX_QUEUE_SIZE_TABLE_TBL;


/* FMS message memory and CAM indirect access.
Caution: This table is for debug purposes, and should be used with
 user discretion as it can effect the IPS operation.
Special consideration should be given when writing to this table.
*/

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00080000 */

  
  /* range: 15:0, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD flow_id;
  /* range: 26:16, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD dest_pipe;
  /* range: 28:27, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD flow_status;
  /* range: 36:29, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD dest_port;
  /* range: 40:37, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD queue_size_4b;
  /* range: 55:41, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD queue_number;
  /* range: 56:56, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD orig_fs;
  /* range: 57:57, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD disable_timer;
  /* range: 58:58, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD valid;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_FMS_MSG_MEM_TABLE_TBL;

/*
FMS timestamp FIFO memory.
*/

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00088000 */

  
  /* range: 5:0, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD time_stamp;

  
  /* range: 12:6, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD bin_idx;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_FMS_TIMESTAMP_TABLE_TBL;

/*enq_block qnum array.*/

typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00090000 */


  /* range: 14:0, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD qnum;


  /* range: 15:15, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD valid;

  /* range: 16:16, access type: RW, default value: 0x0              */
  SOC_PETRA_TBL_FIELD visited;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_ENQ_BLOCK_QNUM_TABLE_TBL;


  /* BDQ:                                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* BDQ: Data Buffer queue memory                                */
   /* range: 25:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD bdq;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_BDQ_TBL;


  /* PCQ:                                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00020000 */

   /* PCQ: Start of packet queue memory                            */
   /* range: 61:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD pcq;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_PCQ_TBL;


  /* Sop MMU:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00030000 */

   /* SopMMU: SOP MMU FIFO                                         */
   /* range: 61:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sop_mmu;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_SOP_MMU_TBL;


  /* Mop MMU:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00040000 */

   /* MopMMU: MOP MMU FIFO                                         */
   /* range: 29:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD mop_mmu;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_MOP_MMU_TBL;


  /* FDTCTL:                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00050000 */

   /* FDTCTL: FDT control FIFO                                     */
   /* range: 28:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD fdtctl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_FDTCTL_TBL;


  /* FDTDATA:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00060000 */

   /* FDTDATA: FDT DATA FIFO                                       */
   /* range: 511:0, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD fdtdata;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_FDTDATA_TBL;


  /* EGQCTL:                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00070000 */

   /* EGQCTL: EGQ control FIFO                                     */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD egqctl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_EGQCTL_TBL;


  /* EGQDATA:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00080000 */

   /* EGQDATA: EGQ DATA FIFO                                       */
   /* range: 511:0, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD egqdata;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_EGQDATA_TBL;


typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x000a0000 */

  SOC_PETRA_TBL_FIELD select_source_sum;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL;


  /* DLL_RAM:                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 */

   /* DLL_RAM: Master slave memory access through indirect         */
   /* range: 55:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD dll_ram;

} __ATTRIBUTE_PACKED__ SOC_PETRA_DPI_DLL_RAM_TBL;


  /* Unicast Distribution Memory for data cells:                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* Unicast Distribution Memory for data cells: Each address     */
   /* corresponds to two consecutive Destination-ID and            */
   /* contains a bitmap. Each bit corresponds to a physical        */
   /* link. 1 -: Destination is accessible through the link. 0     */
   /* -: Destination is not accessible through the link. Data      */
   /* cells destined for a destination exit the FAP only on        */
   /* valid links. Note, this table is updated automatically       */
   /* by a hardware process and normally should not be             */
   /* modified by the user. Note: This memory is dynamically       */
   /* updated and should not be changed by the user. User          */
   /* accesses should be done using only even addresses. The       */
   /* block divides the address by 2.                              */
   /* range: 71:0, access type: RW, default value: 72'hffffffffffffffffff */
   SOC_PETRA_TBL_FIELD unicast_distribution_memory_for_data_cells;

} __ATTRIBUTE_PACKED__ SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL;


  /* Unicast Distribution Memory for control cells:                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00001000 */

   /* Unicast Distribution Memory for control cells: Each          */
   /* address corresponds to two consecutive Destination-ID        */
   /* and contains a bitmap. Each bit corresponds to a             */
   /* physical link. 1 -: Destination is accessible through        */
   /* the link. 0 -: Destination is not accessible through the     */
   /* link. Control cells exit the FAP only on valid links.        */
   /* Note: this table is updated automatically by a hardware      */
   /* process and normally should not be modified by the user.     */
   /* Note: This memory is dynamically updated and should not      */
   /* be changed by the user. User accesses should be done         */
   /* using only even addresses. The block divides the address     */
   /* by 2.                                                        */
   /* range: 71:0, access type: RW, default value: 72'hffffffffffffffffff */
   SOC_PETRA_TBL_FIELD unicast_distribution_memory_for_control_cells;

} __ATTRIBUTE_PACKED__ SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL;

 /* Per Port Configuration Table(PPCT): Per Port                   */
  /* Configuration Table                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00160000 */

   /* MulticastLagLoadBalancingEnable: If set, then enables        */
   /* filtering of multicast to a LAG port. This means that        */
   /* multicast is load balanced in a LAG                          */
   /* range: 3:3, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD multicast_lag_load_balancing_enable;

   /* SysPortId: The system level port ID. This is used to         */
   /* compare with the Incoming-Sys-Port-ID when filtering         */
   /* range: 15:4, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD sys_port_id;

   /* GlagMemberPortId: System GLAG member port ID(significant     */
   /* only if Port-Is-GLAG-Member)                                 */
   /* range: 19:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD glag_member_port_id;

   /* GlagPortId: System level ID of the GLAG port                 */
   /* (significant only if Port-Is-GLAG-Member)                    */
   /* range: 27:20, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD glag_port_id;

   /* PortIsGlagMember: If set, then Indicates if the port is      */
   /* a LAG member                                                 */
   /* range: 28:28, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_is_glag_member;

   /* PortType: Raw, TM, CPU, Ethernet                             */
   /* range: 81:80, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_type;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PPCT_TBL;

 /* Egress Shaper Credit Configuration Memory          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00010000 (Nifa Ch1 Scm) + 0x00010000 * NIF_ID */

   /* PortCrToAdd: How many credits to add to port.                */
   /* range: 17:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD port_cr_to_add;

   /* OfpIndex: Outgoing Fap Port to add credits to.               */
   /* range: 24:18, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ofp_index;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_NIF_SCM_TBL;

  /* Egress Shaper Nifa And Nifb Non Channeleized Ports             */
  /* Credit Configuration (Nifab Nch Scm): NifA and NifB NCH        */
  /* Shaper Calendar Memory                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00090000 */

   /* PortCrToAdd: How many credits to add to port.                */
   /* range: 17:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD port_cr_to_add;

   /* OfpIndex: Outgoing Fap Port to add credits to.               */
   /* range: 24:18, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ofp_index;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL;


  /* Egress Shaper Recycling Ports Credit Configuration (Rcy        */
  /* Scm): Recycling Shaper Calendar Memory                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000a0000 */

   /* PortCrToAdd: How many credits to add to port.                */
   /* range: 17:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD port_cr_to_add;

   /* OfpIndex: Outgoing Fap Port to add credits to.               */
   /* range: 24:18, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ofp_index;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_RCY_SCM_TBL;


  /* Egress Shaper CPUPorts Credit Configuration(Cpu Scm):          */
  /* CPU Shaper Calendar Memory                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000b0000 */

   /* PortCrToAdd: How many credits to add to port.                */
   /* range: 17:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD port_cr_to_add;

   /* OfpIndex: Outgoing Fap Port to add credits to.               */
   /* range: 24:18, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD ofp_index;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_CPU_SCM_TBL;


  /* Egress Shaper Calendar Selector (CCM): Calendars select        */
  /* sequence configuration memory                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000c0000 */

   /* InterfaceSelect: Which interface to select.                  */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD interface_select;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_CCM_TBL;


  /* Per Port Max Credit Memory (PMC): Each entry holds the         */
  /* maximum credit balance that the port can accumulate,           */
  /* indicating the burst size of the OFP. Each entry               */
  /* associated with OFP.                                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000d0000 */

   /* PortMaxCredit: Maximum value that the credit balance of      */
   /* a port may have (in units of bytes).                         */
   /* range: 15:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD port_max_credit;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PMC_TBL;


  /* Egress Shaper Per Port Credit Balance Memory (CBM):            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000e0000 */

   /* EgressShaperPerPortCreditBalanceMemory (CBM): Credit         */
   /* Balance Memory                                               */
   /* range: 24:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD cbm;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_CBM_TBL;


  /* Free Buffesr Allocation Bitmap Memory (FBM): Represents        */
  /* the dynamic status of the EGQ 4K Buffers Memory. Each          */
  /* entry represents a bitmap of 32 buffers indicating, per        */
  /* buffer, whether it is free or occupied.                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x000f0000 */

   /* FreeBufferMemory: Each bit indicates whether the             */
   /* relevant buffer in the Data-Buffer-Memory (DBM) is           */
   /* occupied ('0') or free ('1')                                 */
   /* range: 31:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD free_buffer_memory;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_FBM_TBL;


  /* Free Descriptors Allocation Bitmap Memory (FDM):               */
  /* Represents the dynamic status of the EGQ 8K Descriptors.       */
  /* Each entry represents a bitmap of 64 packet descriptors        */
  /* indocating, per descriptor whether it is free or               */
  /* occupied.                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* FreeDescriptorMemory: Each bit indicates whether the         */
   /* relevant descriptor index is occupied ('0') or free          */
   /* ('1')                                                        */
   /* range: 63:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD free_descriptor_memory;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_FDM_TBL;


  /* Ofp Dequeue Wfq Configuration Memory (DWM): The table          */
  /* consist from eigthy entries, one entry per OFP. Defines        */
  /* the weights of the two classes connected to this               */
  /* scheduler. Enables WFQ and SP relation between the two         */
  /* classes. Weight of 0 for a class indicates that this           */
  /* class has SP over the other class. When both have equal        */
  /* weights it implies simple RR. Maximum weight difference        */
  /* is 255:1                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00110000 */

   /* McOrMcLowQueueWeight: The lower the weight the higher        */
   /* the bandwidth                                                */
   /* range: 7:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD mc_or_mc_low_queue_weight;

   /* UcOrUcLowQueueWeight: The lower the weight the higher        */
   /* the bandwidth                                                */
   /* range: 15:8, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD uc_or_uc_low_queue_weight;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_DWM_TBL;


  /* RRDM: RQP Segment reassembly descriptors (FSR)                 */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00120000 */

   /* CRCRemainder: Remainder of previous CRC calculation          */
   /* range: 15:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD crcremainder;

   /* ReasState: Context Reassembly current state                  */
   /* range: 17:16, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD reas_state;

   /* EOPFragNum: Total number of segments in the packet           */
   /* range: 26:18, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD eopfrag_num;

   /* NxtFragNumber: Expected fragment number of next packet       */
   /* segment                                                      */
   /* range: 35:27, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD nxt_frag_number;

   /* StoredSegSize: The number of words stored from previous      */
   /* segments of FAP20 cells reassembly                           */
   /* range: 37:36, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD stored_seg_size;

   /* Fix129: Indication for first segment of 129B packet size     */
   /* fix                                                          */
   /* range: 38:38, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD fix129;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_RRDM_TBL;


  /* RPDM: RQP Packet reassembly descriptors (PRP)                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00130000 */

   /* PacketStartBufferPointer: Pointer to start of packet         */
   /* buffer                                                       */
   /* range: 11:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD packet_start_buffer_pointer;

   /* PacketBufferWritePointer: Pointer to previos packet          */
   /* buffer                                                       */
   /* range: 24:12, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD packet_buffer_write_pointer;

   /* ReasState: Context Reassembly current state                  */
   /* range: 26:25, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD reas_state;

   /* PacketFragCnt: Conts the number of segments written to       */
   /* DBF                                                          */
   /* range: 34:27, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD packet_frag_cnt;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_RPDM_TBL;


  /* Port Configuration Table (PCT): Out-Port Configuration         */
  /* Table. Eighty entries table.                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00140000 */

   /* OutboundMirr: If set. then indicates to mirror the           */
   /* traffic back to the ingress (via the recycling port)         */
   /* range: 0:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD outbound_mirr;

   /* IsMirroring: Flag indicating that all traffic out of         */
   /* this port should be treated as if it is mirrored             */
   /* traffic. This means to bypass all egress editing of          */
   /* bridged or routed packets                                    */
   /* range: 1:1, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD is_mirroring;

   /* OtmhLifExtEna: 0: Never add a Out-LIF Extension 1: Add       */
   /* only if system multicast 2,3: Always add (must have FTMH     */
   /* extension)                                                   */
   /* range: 3:2, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD otmh_lif_ext_ena;

   /* OtmhDestExtEna: if set, then add an OTMH destination         */
   /* port extension                                               */
   /* range: 4:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD otmh_dest_ext_ena;

   /* OtmhSrcExtEna: If set, then add an OTMH source port          */
   /* extension                                                    */
   /* range: 5:5, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD otmh_src_ext_ena;

   /* PortType: Raw, TM, Ethernet, CPU                             */
   /* range: 49:48, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_type;

   /* PortChannelNumber: Mapping of the Outgoing FAP Port to a     */
   /* channel number on the Nif/CPU/Recycling ports                */
   /* range: 55:50, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD port_channel_number;

   /* DestinationPortExtensionID: Value stamped in                 */
   /* Destination-Port-Extension header if the packet              */
   /* originate from corresponding port                            */
   /* range: 68:56, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD destination_port_extension_id;

   /* HeaderCompensationType: If set, then header compensation     */
   /* type B register set is used. Otherwise header                */
   /* compensation type A register set is used.                    */
   /* range: 69:69, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD header_compensation_type;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PCT_TBL;


  /* Vlan Table Configuration Memory(Vlan Table): Vlan              */
  /* Membership Table. 4K entries. Required for egress              */
  /* multicast in bitmap mode.                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00150000 */

   /* VlanMembership: Bitmap of the OFPs. If bit N is set,         */
   /* indicates that OFP N is a member of the VLAN                 */
   /* range: 79:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD vlan_membership;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_VLAN_TABLE_TBL;


/* CfcFlowControl:                                                */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00180000 */

  /* : CFC flow control per OFP.                                  */
  /* range: 79:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD cfc_flow_control;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_CFC_FLOW_CONTROL_TBL;


/* NifaFlowControl:                                               */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00190000 */

  /* : NIFA flow control per NIF port.                            */
  /* range: 15:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD nifa_flow_control;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_NIFA_FLOW_CONTROL_TBL;


/* NifbFlowControl:                                               */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x001a0000 */

  /* : NIFB flow control per NIF port.                            */
  /* range: 15:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD nifb_flow_control;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_NIFB_FLOW_CONTROL_TBL;


/* CpuLastHeader:                                                 */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x001b0000 */

  /* : Last packet header arrived from CPU.                       */
  /* range: 63:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD cpu_last_header;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_CPU_LAST_HEADER_TBL;


/* IptLastHeader:                                                 */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x001c0000 */

  /* : Last packet header arrived from IPT.                       */
  /* range: 63:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD ipt_last_header;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_IPT_LAST_HEADER_TBL;


/* FdrLastHeader:                                                 */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x001d0000 */

  /* : Last packet header arrived from FDR.                       */
  /* range: 63:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD fdr_last_header;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_FDR_LAST_HEADER_TBL;


/* CpuPacketCounter:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x001e0000 */

  /* : CPU received packet counter. If bit 32 is set, counter     */
  /* overflowed.                                                  */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD cpu_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_CPU_PACKET_COUNTER_TBL;


/* IptPacketCounter:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x001f0000 */

  /* : IPT received packet counter. If bit 32 is set, counter     */
  /* overflowed.                                                  */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD ipt_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_IPT_PACKET_COUNTER_TBL;


/* FdrPacketCounter:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

  /* : FDR received packet counter. If bit 32 is set, counter     */
  /* overflowed.                                                  */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD fdr_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_FDR_PACKET_COUNTER_TBL;


/* RqpPacketCounter:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00210000 */

  /* : RQP2EHP packet counter. If bit 32 is set, counter          */
  /* overflowed.                                                  */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD rqp_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_RQP_PACKET_COUNTER_TBL;


/* RqpDiscardPacketCounter:                                       */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00220000 */

  /* : RQP2EHP discarded packet counter. If bit 32 is set,        */
  /* counter overflowed.                                          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD rqp_discard_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_RQP_DISCARD_PACKET_COUNTER_TBL;


/* EhpUnicastPacketCounter:                                       */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00230000 */

  /* : EHP2PQP Unicast packet counter. If bit 32 is set,          */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD ehp_unicast_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_EHP_UNICAST_PACKET_COUNTER_TBL;


/* EhpMulticastHighPacketCounter:                                 */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00240000 */

  /* : EHP2PQP Multicast High packet counter. If bit 32 is        */
  /* set, counter overflowed. If CheckBwToOfp is set counts       */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD ehp_multicast_high_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_EHP_MULTICAST_HIGH_PACKET_COUNTER_TBL;


/* EhpMulticastLowPacketCounter:                                  */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00250000 */

  /* : EHP2PQP Multicast Low packet counter. If bit 32 is         */
  /* set, counter overflowed. If CheckBwToOfp is set counts       */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD ehp_multicast_low_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_EHP_MULTICAST_LOW_PACKET_COUNTER_TBL;


/* EhpDiscardPacketCounter:                                       */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00260000 */

  /* : EHP2PQP discarded packet counter. If bit 32 is set,        */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD ehp_discard_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_EHP_DISCARD_PACKET_COUNTER_TBL;


/* PqpUnicastHighPacketCounter:                                   */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00270000 */

  /* : PQP2FQP Unicast High packet counter. If bit 32 is set,     */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_unicast_high_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_UNICAST_HIGH_PACKET_COUNTER_TBL;


/* PqpUnicastLowPacketCounter:                                    */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00280000 */

  /* : PQP2FQP Unicast Low packet counter. If bit 32 is set,      */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_unicast_low_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_UNICAST_LOW_PACKET_COUNTER_TBL;


/* PqpMulticastHighPacketCounter:                                 */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00290000 */

  /* : PQP2FQP Multicast High packet counter. If bit 32 is        */
  /* set, counter overflowed. If CheckBwToOfp is set counts       */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_multicast_high_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_PACKET_COUNTER_TBL;


/* PqpMulticastLowPacketCounter:                                  */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x002a0000 */

  /* : PQP2FQP Multicast Low packet counter. If bit 32 is         */
  /* set, counter overflowed. If CheckBwToOfp is set counts       */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_multicast_low_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_MULTICAST_LOW_PACKET_COUNTER_TBL;


/* PqpUnicastHighBytesCounter:                                    */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x002b0000 */

  /* : PQP2FQP Unicast High bytes counter. If bit 46 is set,      */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 46:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_unicast_high_bytes_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_UNICAST_HIGH_BYTES_COUNTER_TBL;


/* PqpUnicastLowBytesCounter:                                     */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x002c0000 */

  /* : PQP2FQP Unicast Low bytes counter. If bit 46 is set,       */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 46:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_unicast_low_bytes_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_UNICAST_LOW_BYTES_COUNTER_TBL;


/* PqpMulticastHighBytesCounter:                                  */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x002d0000 */

  /* : PQP2FQP Multicast High bytes counter. If bit 46 is         */
  /* set, counter overflowed. If CheckBwToOfp is set counts       */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 46:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_multicast_high_bytes_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_MULTICAST_HIGH_BYTES_COUNTER_TBL;


/* PqpMulticastLowBytesCounter:                                   */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x002e0000 */

  /* : PQP2FQP Multicast Low bytes counter. If bit 46 is set,     */
  /* counter overflowed. If CheckBwToOfp is set counts            */
  /* OfpToCheckBw packets, otherwise counts all packets.          */
  /* range: 46:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_multicast_low_bytes_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_MULTICAST_LOW_BYTES_COUNTER_TBL;


/* PqpDiscardUnicastPacketCounter:                                */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x002f0000 */

  /* : PQP discarded Unicast packet counter. If bit 32 is         */
  /* set, counter overflowed.                                     */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_discard_unicast_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_DISCARD_UNICAST_PACKET_COUNTER_TBL;


/* PqpDiscardMulticastPacketCounter:                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00300000 */

  /* : PQP discarded Multicast packet counter. If bit 32 is       */
  /* set, counter overflowed.                                     */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD pqp_discard_multicast_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_PQP_DISCARD_MULTICAST_PACKET_COUNTER_TBL;


/* FqpPacketCounter:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00310000 */

  /* : FQP2EPE packet counter. If bit 32 is set, counter          */
  /* overflowed. If CheckBwToOfp is set counts OfpToCheckBw       */
  /* packets, otherwise counts all packets.                       */
  /* range: 32:0, access type: RW, default value: 0x0             */
  SOC_PETRA_TBL_FIELD fqp_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_FQP_PACKET_COUNTER_TBL;



/* EPE2PNI packet counter:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00040000  */

  /* EPE2PNI packet counter. If bit 32 is set, counter */
  /* overflowed. If MaskCheckBwToPacketDescriptor     */
  /* is cleared counts accordingly, if all packet     */
  /* descriptors are masked counts all packets.       */
  SOC_PETRA_TBL_FIELD epe_packet_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL;

/* EpeBytesCounte:                                              */
typedef struct
{
  SOC_PETRA_TBL_ADDR addr;   /* 0x00050000 */

  /* EPE2PNI bytes counter. If bit 46 is set, counter  */
  /*   overflowed. If MaskCheckBwToPacketDescriptor  */
  /*   is cleared counts accordingly, if all packet  */
  /*   descriptors are masked counts all packets */
  SOC_PETRA_TBL_FIELD epe_bytes_counter;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL;


  /* Recycle to Out Going Fap Port Mapping: Recycle 2 OFP           */
  /* mapping. Used both for Recycle VSQ->SCH's HR FC , and for       */
  /* Recycle VSQ->EGQ's OFP FC.                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* EgqOfpNum: Define the out-going FAP port in the EGQ          */
   /* Entry validity is defined by registers RclLpHrMap and        */
   /* RclHpHrMap bitmaps.                                          */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD egq_ofp_num;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL;


  /* NIF A Class Based to OFP mapping: Maping the 32 NIFa's         */
  /* class-based FC to EGQ's OFP FC signals. Used to generate        */
  /* both EGQ-OFP FC, and SCH-OFP-HR FC. Each entry refers to       */
  /* two consecutive classes                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00100000 */

   /* EgqOfpNum0: Defines the out going FAP port in the EGQ        */
   /* The entry validity for SCH-OFP is defined by registers       */
   /* ClbLpOfpMap and ClbHpOfpMap (bitmaps). The entry             */
   /* validity for EGQ-OFP is defined by register ClbEgqOfpMap     */
   /* bitmap. This field is used for NIF's class-based index       */
   /* N.                                                           */
   /* range: 6:0, access type: RW, default value: 0x0              */
   /* EgqOfpNum1: Same as EgqOfpNum0, but for NIF's                */
   /* class-based index N+1.                                       */
   /* range: 13:7, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD egq_ofp_num[SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS];
} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL;


  /* NIF B Class Based to OFP mapping: Maping the 32 NIFb's         */
  /* class based FC to EGQ's OFP FC signals. Used both to            */
  /* generate EGQ-OFP FC, and SCH-OFP-HR FC. Each entry refers       */
  /* to two consecutive classes                                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00200000 */

   /* EgqOfpNum0: Defines the out going FAP port in the EGQ        */
   /* The entry validity for SCH-OFP is defined by registers       */
   /* ClbLpOfpMap and ClbHpOfpMap (bitmaps). The entry             */
   /* validity for EGQ-OFP is defined by register ClbEgqOfpMap     */
   /* bitmap. This field is used for NIF's class-based index       */
   /* N.                                                           */
   /* range: 6:0, access type: RW, default value: 0x0              */
   /* EgqOfpNum1: Same as EgqOfpNum0, but for NIF's                */
   /* class-based index N+1.                                       */
   /* range: 13:7, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD egq_ofp_num[SOC_PETRA_NOF_CFC_NIF_CLS2OFP_MAP_FLDS];
} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL;


  /* Out Of Band (A) Scheduler's based flow-control to OFP          */
  /* mapping: Maping of the 128 Scheduler-based FC received         */
  /* from the OOB interface to High and Low priority HR FCs         */
  /* to the SCH. Note: Under HGL mode the table cannot be            */
  /* accessed for writing !                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00400000 */

   /* OfpHr: Defines the out going FAP port HR number.             */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ofp_hr;

   /* LpOfpValid: If set to one, then the relevant NIF-class       */
   /* number defines a Low priority indication for OFP-HR.         */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lp_ofp_valid;

   /* HpOfpValid: If set to one, then the relevant NIF-class       */
   /* number defines a High priority indication for OFP-HR.        */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD hp_ofp_valid;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL;


  /* Out Of Band (B) Scheduler's based flow-control to OFP          */
  /* mapping: Maping of the 128 Scheduler-based FC received         */
  /* from the OOB interface to High and Low priority HR FCs         */
  /* to the SCH. Note: Under HGL mode the table cannot be            */
  /* accessed for writing !                                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00500000 */

   /* OfpHr: Defines the out going FAP port HR number.             */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD ofp_hr;

   /* LpOfpValid: If set to one, then the relevant NIF-class       */
   /* number defines a Low priority indication for OFP-HR.         */
   /* range: 7:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD lp_ofp_valid;

   /* HpOfpValid: If set to one, then the relevant NIF-class       */
   /* number defines a High priority indication for OFP-HR.        */
   /* range: 8:8, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD hp_ofp_valid;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL;


  /* Out Of Band Rx A calendar mapping: Mapping of the OOB-Rx       */
  /* received FC status frames to OFP-Based or Sch-based FC         */
  /* indications.(Used by OOB-RX0 interface).                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00600000 */

   /* FcIndex: Define the flow control index, which could be       */
   /* according tp bits 8:7: 0 - 79 indexes of the OFP-Based       */
   /* FCs(EGQ) 0 - 127 indexes of the SCH-Base FCs 0-15            */
   /* indexes of NIF's link-level (to stop NIF's TX on a           */
   /* specific link).                                              */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_index;

   /* FcDestSel: Defines the type of FC that FC-index refers       */
   /* to: 0: - The calendar channel carries one of the 128         */
   /* SCH-Based FCs (for hierarchical HRs). 1: - The calendar      */
   /* channel carries one of the 80 OFP-based FCs. 2: - The        */
   /* calenadar channel carries one of the 16 NIFa's               */
   /* link-level FCs. 3: - The calenadar channel carries one       */
   /* of the 16 NIFb's link-level FCs.                             */
   /* range: 8:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_dest_sel;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL;


  /* Out Of Band Rx B calendar mapping: Mapping of the OOB-Rx       */
  /* received FC status frames to OFP-Based or Sch-based FC         */
  /* indications.(Used by OOB-RX1 interface).                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00700000 */

   /* FcIndex: Define the flow control index, which could be       */
   /* according tp bits 8:7: 0 - 79 indexes of the OFP-Based       */
   /* FCs (EGQ) 0 - 127 indexes of the SCH-Base FCs 0-15           */
   /* indexes of NIF's link-level (to stop NIF's TX on a           */
   /* specific link).                                              */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_index;

   /* FcDestSel: Defines the type of FC that FC-index refers       */
   /* to: 0: - The calendar channel carries one of the 128         */
   /* SCH-Based FCs (for hierarchical HRs). 1: - The calendar      */
   /* channel carries one of the 80 OFP-based FCs. 2: - The        */
   /* calenadar channel carries one of the 16 NIFa's               */
   /* link-level FCs. 3: - The calenadar channel carries one       */
   /* of the 16 NIFb's link-level FCs.                             */
   /* range: 8:7, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_dest_sel;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL;


  /* Out Of Band Tx calendar mapping: Mapping of the OOB-TX         */
  /* calendar channels to the possible generated FC                 */
  /* indications.                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00800000 */

   /* FcIndex: The index to one of the generated FC options,       */
   /* that should be mapped to current calendar channel. The       */
   /* FC options are defined by the FcSourceSelect field. For      */
   /* the High/Low-p global source option, the decoding is as      */
   /* follows: 0:- FC is due to Free-BDB counter 1:- FC is due     */
   /* to Free-Unicast-Dbuff counter 2:- FC is due to               */
   /* Free-Full-Mc-Dbuff counter 3:- Not used                      */
   /* range: 8:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD fc_index;

   /* FcSourceSel: Defines the type of FC that the FC-index        */
   /* refers to: 0:- The index refers to the STE 356 VSQs. 1:      */
   /* - The index refers to the 32 Link-Level FC (from the         */
   /* NIF-RX). 2: - The index selects one of the High-priority     */
   /* Global resource FCs. 3:- The index selects one of the        */
   /* Low-priority Global resource FCs.                            */
   /* range: 10:9, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD fc_source_sel;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL;


  /* Scheduler Credit Generation Calendar (CAL): The CAL is a       */
  /* 20k-entry memory. Each of the eight NIFs, the CPU, and         */
  /* the recycle interfaces maintain two calendars. Passive         */
  /* and Activie. In total there are 20 calendars (10               */
  /* Interfaces x 2 Calendars). Each calendar has 1k entries.       */
  /* The length of each calendar is configured using the            */
  /* variables CALALen and CALBLen for each of the eight            */
  /* Channelized NIFs, the CPU, and the Recycle. Each               */
  /* calendar entry specifies one HR-Scheduler. An                  */
  /* HR-Scheduler with n occurrences in the calendar is             */
  /* awarded n CALxLen of its interface rate. At a single           */
  /* point in time, one calendar is designated as Active, and       */
  /* the other as Standby. The configuration bits CALSelA and       */
  /* CALSelB select which calendar is Active. This is               */
  /* required to support the dynamic assignment of bandwidth        */
  /* (e.g. GFP framer with LCAS). When the CPU accesses the         */
  /* Calendar, it should access the non-active calendar. The        */
  /* 20 Calendars are implemented as one memory in which a          */
  /* different addressing space is used for each calendar. The       */
  /* following addresses are assigned to the different              */
  /* calendars:NIF0 - CalanderA - Addresses: 0x40000000 -           */
  /* 400003ffNIF0 - CalanderB - Addresses: 0x40001000 -             */
  /* 400013ffNIF1 - CalanderA - Addresses: 0x40002000 -             */
  /* 400023ffNIF1 - CalanderB - Addresses: 0x40003000 -             */
  /* 400033ffNIF2 - CalanderA - Addresses: 0x40004000 -             */
  /* 400043ffNIF2 - CalanderB - Addresses: 0x40005000 -             */
  /* 400053ffNIF3 - CalanderA - Addresses: 0x40006000 -             */
  /* 400063ffNIF3 - CalanderB - Addresses: 0x40007000 -             */
  /* 400073ffNIF4 - CalanderA - Addresses: 0x40008000 -             */
  /* 400083ffNIF4 - CalanderB - Addresses: 0x40009000 -             */
  /* 400093ffNIF5 - CalanderA - Addresses: 0x4000a000 -             */
  /* 4000a3ffNIF5 - CalanderB - Addresses: 0x4000b000 -             */
  /* 4000b3ffNIF6 - CalanderA - Addresses: 0x4000c000 -             */
  /* 4000c3ffNIF6 - CalanderB - Addresses: 0x4000d000 -             */
  /* 4000d3ffNIF7 - CalanderA - Addresses: 0x4000e000 -             */
  /* 4000e3ffNIF7 - CalanderB - Addresses: 0x4000f000 -             */
  /* 4000f3ffCPU - CalanderA - Addresses: 0x40000800 -              */
  /* 40000bffCPU - CalanderB - Addresses: 0x40001800 -              */
  /* 40001bffRCY - CalanderA - Addresses: 0x40002800 -              */
  /* 40002bffRCY - CalanderB - Addresses: 0x40003800 -              */
  /* 40003bff                                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40000000 */

   /* HRSel: The HR scheduler to receive the credit when this      */
   /* entry is visited.                                            */
   /* range: 6:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD hrsel;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_CAL_TBL;


  /* Device Rate Memory (DRM): The DRM is a 296-entry memory.       */
  /* The selection of the current active rate is determined         */
  /* by the number of current active links (0 36) and the           */
  /* current status of fabric congestion, as reflected in the       */
  /* RCI bucket level (0 7). The address is a concatenation         */
  /* of NumActiveLinks and RCILevel.                                */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40010000 */

   /* DeviceRate: Determines, in one 64th of a clock period        */
   /* resolution, the rate at which credits are generated by       */
   /* the device. When DeviceRate = 0, no credits are              */
   /* generated. The DeviceRate Minimum value (other than) is      */
   /* 64                                                           */
   /* range: 17:0, access type: RW, default value: 0x4444          */
   SOC_PETRA_TBL_FIELD device_rate;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_DRM_TBL;


  /* Dual Shaper Memory (DSM): The DSM is a 512-entry memory.       */
  /* Each entry contains data for 16 dual bucket schedulers.        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40020000 */

   /* DualShaperEna: Indicates that shapers (16i+n )*2 and         */
   /* (16i+n)*2+1 share a dual bucket.                             */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD dual_shaper_ena;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_DSM_TBL;


  /* Flow Descriptor Memory Static (FDMS): The FDMS is a            */
  /* 56k-entry memory. This memory contains the                     */
  /* Flow-Attributes. There is one entry per sub-flow. Both         */
  /* Schedulers and flows share this memory context. The            */
  /* Flow-Attributes are set by the user. The Flow-Attributes       */
  /* depend on the type of scheduler this flow is attached          */
  /* to.                                                            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40030000 */

   /* Sch-Number: Identifieswhich scheduler the flow is            */
   /* attached to (i.e. getting credits from): 16128-16383         */
   /* selects one of 256 HR-Schedulers. 0-8191 selects one of      */
   /* 8192 CL-Schedulers. 8192-16127 selects one of 7936           */
   /* FQ-Schedulers.                                               */
   /* range: 13:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sch_number;

   /* COS: COS - Class of service. The COS options depend on       */
   /* the scheduler type as follows: HR: Defines the service       */
   /* class of the flow: 0xfc: EF1 0xfd: EF2 0xfe: EF3 0xff:       */
   /* BE For the following, see also the description of            */
   /* HRSelDual (in the row below). If the flow is on one of       */
   /* the WFQs on the HR scheduler: 0x0-0xfb: The weight is        */
   /* calculated as follows: Man = COS[4:0] in binary              */
   /* fraction. (Man == 0 is not a legal configuration) Exp =      */
   /* COS[7:5] The weight is Man/2^Exp For example: COS =          */
   /* 0x5a. Man = 0x1a, 11010 (1/(2^1)x1 + 1/(2^2)x1 +             */
   /* 1/(2^3)x0 + 1/(2^4)x1 + 1/(2^5)x0 = 0.8125) Exp = 0x2.       */
   /* Weight is 0.8125/4 = 0.203125 If the scheduler is in         */
   /* Enhanced Strict Mode and the flow belongs to EF4 - EF9:      */
   /* (This happens when HREnhancedStrict = 1 and DualMode = 1     */
   /* in the SHC, and HRSelDual = 0) 0x30:EF4 0x70:EF5             */
   /* 0xb0:EF6 0xf0:EF7 0xe4:EF8 0xe1:EF9 CL: There are two        */
   /* weighing schemes: The first scheme applies to strict         */
   /* priorities (WFQMode = 0 or 2, Discrete or weight             */
   /* applied) and to WFQ with Class Based Mode. WFQMode is        */
   /* defined in CL-Schedulers Type (SCT)). 0xfb: The flow is      */
   /* put on the extra SP added by enhance class. 0xfc: First      */
   /* Priority list 0xfd: Second Priority list 0xfe: Third         */
   /* Priority list 0xff: Fourth Priority list A flow is           */
   /* mapped to a list. According to the Scheduler Type memory     */
   /* (SCT), the list is mapped to a strict priority and/or        */
   /* weight. The second scheme (WFQMode = 1, Independent          */
   /* weights) is for independent flow weights in the range of     */
   /* 1 to 251. The flow is either mapped to a (same weight)       */
   /* strict priority or a unique weight, if specified. Four       */
   /* values are used to map the flow to a strict priority         */
   /* list that must be consistent with the Scheduler Type         */
   /* memory SCT. 0xfb: The flow is put on the SP added by EC      */
   /* 0xfc: First Strict Priority 0xfd: Second Priority list       */
   /* 0xfe: Third Priority list 0xff: Fourth Priority list         */
   /* Other values are interpreted as weights:0x0-0xfb: AF         */
   /* class with a weight is calculated as follows: Weight =       */
   /* COS[7:0] (in binary fraction) For example, COS = 0xda,       */
   /* Weight = 0xda, 11011010 ((1/(2^1)x1 + 1/(2^2)x1 +            */
   /* 1/(2^3)x0 + 1/(2^4)x1 + 1/(2^5)x1 + 1/(2^6)x0 +              */
   /* 1/(2^7)x1 + 1/(2^8)x0 = 0.8515625 FQ: The COS field is       */
   /* don't care                                                   */
   /* range: 21:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD cos;

   /* HRSelDual: This bit is significant only when the             */
   /* SchNumber is an HR scheduler, and this specific HR is in     */
   /* DualMode (as defined in the SHC) and the COS is 0x1 -        */
   /* 0xfb. In such cases: If HRSelDual = 0, and                   */
   /* HREnhancedStrict = 0, then the flow is attached the          */
   /* first part of the dual scheduler (A). If                     */
   /* HREnhancedStrict = 1, Then the flow can receive one of       */
   /* EF4-EF9 strict priorities. If HRSelDual = 1, and             */
   /* HREnhancedStrict = 0, then the flow is attached the          */
   /* second part of the dual scheduler (B). If                    */
   /* HREnhancedStrict = 1, then the flow is attached to the       */
   /* WFQ < EF9 on the HR scheduler. [See                          */
   /* HR-Scheduler-Configuration (SHC) for a description of        */
   /* the SHC.]                                                    */
   /* range: 22:22, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD hrsel_dual;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_FDMS_TBL;


  /* Shaper Descriptor Memory Static (SHDS): The SHDS is a          */
  /* 28k-entry memory. This memory contains the                     */
  /* Shaper-Flow-Attributes. Each entry contains the data for       */
  /* two flows. For each flow, this memory holds the                */
  /* Shaper-Flow-Attributes. The Flow-Attributes are set by         */
  /* the user and are never modified by the logic. The Shaper        */
  /* receives a credit every four clocks. The bandwidth given       */
  /* to a flow is the Rate (see calculation below) multiplied       */
  /* by the shaper bandwidth.                                       */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40040000 */

   /* PeakRateManEven: PeakRateMan is in binary fraction. The      */
   /* weight is PeakRateMan /2^ PeakRateExp (The following are     */
   /* not legal configurations: PeakRateMan == 0; PeakRateExp      */
   /* == 0xf; and PeakRateMan[0] == 0.) To calculate the           */
   /* shaper rate, the following calculation should be             */
   /* preformed:                                                   */
   /* (CreditSize[Bits]xWeight)/(5x4xClkPeriod[ns])=Rate in        */
   /* Gbps. For example: PeakRateMan = 0x34. PeakRateExp =         */
   /* 0x9. PeakRateMan = 0x34,110100 (1/(2^1)x1 + 1/(2^2)x1 +      */
   /* 1/(2^3)x0 + 1/(2^4)x1 + 1/(2^5)x0 + 1/(2^6)x0 = 0.8125)      */
   /* Weight is 0.8125/2^9 = 1.58691x10^-3 CreditSize of 256       */
   /* Byte (2048 Bit) ClkPeriod of 4ns (250 Mhz clk) The rate      */
   /* is: (2048x1.58691x10^-3)/(4x3x4)=0.067 Gbps = 67 Mbps        */
   /* range: 5:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD peak_rate_man_even;

   /* PeakRateExpEven:                                             */
   /* range: 9:6, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD peak_rate_exp_even;

   /* MaxBurstEven: CreditSize * MaxBurst = Maximum data burst     */
   /* size in bytes that the flow scheduler can extract.           */
   /* range: 18:10, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD max_burst_even;

   /* SlowRate2SelEven: If set, selects SlowRate2 on a             */
   /* per-sub-flow basis.                                          */
   /* range: 19:19, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD slow_rate2_sel_even;

   /* PeakRateManOdd: Same as above for the odd flow.              */
   /* range: 25:20, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD peak_rate_man_odd;

   /* PeakRateExpOdd: Same as above for the odd flow.              */
   /* range: 29:26, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD peak_rate_exp_odd;

   /* MaxBurstOdd: Same as above for the odd flow.                 */
   /* range: 38:30, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD max_burst_odd;

   /* SlowRate2SelOdd: Same as above for the odd flow.             */
   /* range: 39:39, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD slow_rate2_sel_odd;

   /* MaxBurstUpdateEven: Virtual bit. If set, then the when       */
   /* writing to the SHDS, MaxBurstEven is updated to the          */
   /* relevant entry in the TMC.                                   */
   /* range: 40:40, access type: W, default value: 0x0             */
   SOC_PETRA_TBL_FIELD max_burst_update_even;

   /* MaxBurstUpdateOdd: Virtual bit. If set, then the when        */
   /* writing to the SHDS, MaxBurstOdd is updated to the           */
   /* relevant entry in the TMC.                                   */
   /* range: 41:41, access type: W, default value: 0x0             */
   SOC_PETRA_TBL_FIELD max_burst_update_odd;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_SHDS_TBL;


  /* Scheduler Enable Memory (SEM): The SEM is a 2048-entry         */
  /* memory-each entry holds data for eight schedulers. The          */
  /* Scheduler-Enable-Memory has one bit per scheduler. If a        */
  /* bit is set, this indicates that the corresponding              */
  /* scheduler is in fact used as a scheduler; if not set           */
  /* this indicates that the scheduler is not used and the          */
  /* flow that feeds credits to the scheduler is free to be         */
  /* used for other purposes. There are a total of 16k               */
  /* schedulers:8192 Class schedulers (In entries 0-1023)7936       */
  /* FQ schedulers (In entries 1024-2015)256 HR schedulers          */
  /* (In entries 2016-2047)The memory holds eight bits per          */
  /* memory entry. Note: Where an aggregate x is comprised of        */
  /* two sub flows, both entries \{x[11:1],0\} and                  */
  /* \{x[11:1],1\} (odd and even adjacent to the aggregate)         */
  /* should be updated with the same data.                          */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40050000 */

   /* SchEnable: Each bit n corresponds to Scheduler 8i+n 0 -      */
   /* Scheduler 8i+n is a flow 1 - Scheduler 8i+n is a             */
   /* scheduler                                                    */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD sch_enable;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_SEM_TBL;


  /* Flow Sub-Flow (FSF): The FSF is a 2048-entry memory. The        */
  /* FSF states whether a flow is a regular or composite            */
  /* flow. One bit is allocated for every two flows. If the bit       */
  /* is not set, both odd and even flows are regular flows. If       */
  /* the Bit is set, then the Even flow is a composite flow         */
  /* and the odd flow is not valid. Each entry holds the data        */
  /* for 16 composite flows.                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40060000 */

   /* SFEnable: Each bit n corresponds to flows (16i+n)*2 and      */
   /* (16i+n)*2+1, with the following meaning: 0 - Flows           */
   /* (16i*+n)*2 and (16i*+n)*2+1 are regular flows. 1 - Flow      */
   /* (16i*+n)*2 is a composite flow and (16i*+n)*2+1 is not       */
   /* valid.                                                       */
   /* range: 15:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD sfenable;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_FSF_TBL;


  /* Flow Group Memory (FGM): The FGM is a 2048-entry memory.       */
  /* Each entry holds the group of eight schedulers,                */
  /* indicating the group (0-2) assigned to the scheduler.          */
  /* The flows belonging to a scheduler are attached to the         */
  /* group of the scheduler in the shaper according to this         */
  /* memory.                                                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40070000 */

   /* FlowGroup0: The group that sched 8i + n belongs to.              */
   /* range: 1:0, access type: RW, default value: undef            */
   SOC_PETRA_TBL_FIELD flow_group[SOC_PETRA_TBL_FGM_NOF_GROUPS_ONE_LINE];
} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_FGM_TBL;


  /* HR-Scheduler-Configuration (SHC): The SHC is a 256-entry       */
  /* memory. This memory describes an HR scheduler operating        */
  /* in single, dual, or Enhanced mode. It also holds the           */
  /* High/Low flow-control mode to be applied for the HR.           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40080000 */

   /* HRMode: HRMode describes the HR operation mode: HRMode       */
   /* 0x0: Single Mode HRMode 0x1: Dual WFQ mode HRMode 0x2:       */
   /* Reserved HRMode 0x3: Enhanced strict Mode Single Mode:       */
   /* The HR scheduler operates in normal mode. 3 SP > WFQ >       */
   /* BE. Dual WFQ mode: 3 SP > WFQ > WFQ > BE structure.          */
   /* Enhanced Strict mode: 9 SP > WFQ > BE structure.             */
   /* range: 1:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD hrmode;

   /* HRMaskType: HRMaskType selects which of HRMask0-3 is         */
   /* used for this particular HR. This is only relevant for       */
   /* entries 0-80.                                                */
   /* range: 3:2, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD hrmask_type;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_SHC_TBL;


  /* CL-Schedulers Configuration (SCC): The SCC is an               */
  /* 8k-entry memory. This memory allocates to each of the 8k       */
  /* class schedulers one of 256 types. (See memory                 */
  /* SCT). Entry i relates to class scheduler i.                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40090000 */

   /* CLSchType: Selects one of 256 CL-Schedulers-Types (in        */
   /* the SCT)                                                     */
   /* range: 7:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD clsch_type;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_SCC_TBL;


  /* CL-Schedulers Type (SCT): The SCT is a 256-entry memory.       */
  /* This memory defines for each class type exactly how the        */
  /* class scheduler will be configured. In the table below,        */
  /* n relates to set of values, while X relates to a range,        */
  /* as it is configured by the WFQ mode.                           */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x400a0000 */

   /* CLConfig: Encoding is an FLL bit map read from right to      */
   /* left. 0-part of WFQ 1-SP CLConfig -> Hierarchy               */
   /* description 0000 -> 1 level: WFQ with n=4 or X=254 0001      */
   /* ->2 levels: FQ > WFQ with n=3 or X=64 1000 ->2 levels:       */
   /* WFQ with n=3 or X=64 > FQ 0011 ->3 levels: FQ > FQ > WFQ     */
   /* with n=2 ( WFQMode must be set to 2 for this option.)        */
   /* 1111 ->4 levels: FQ > FQ > FQ > FQ (WFQMode must be set      */
   /* to 0 for this option.)                                       */
   /* range: 3:0, access type: RW, default value: 0x0              */
   SOC_PETRA_TBL_FIELD clconfig;

   /* AF0InvWeight: Weights for WFQ when operating in per-flow     */
   /* discrete weight or per-class weight                          */
   /* range: 13:4, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD af0_inv_weight;

   /* AF1InvWeight:                                                */
   /* range: 23:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD af1_inv_weight;

   /* AF2InvWeight:                                                */
   /* range: 33:24, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD af2_inv_weight;

   /* AF3InvWeight:                                                */
   /* range: 43:34, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD af3_inv_weight;

   /* WFQMode: 0 - Discrete weight per flow 1 - Independent        */
   /* weight per flow 2 - Weight applied per class                 */
   /* range: 45:44, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD wfqmode;

   /* EnhCLEn: This bit defines for each class type if it          */
   /* works in enhanced mode or regular mode. If it works in       */
   /* enhanced mode, then the FQ which has the same number as      */
   /* the class Is not available and its resources are used to     */
   /* enhance the class. This bit can only be set for class        */
   /* numbers is 0-7935.                                           */
   /* range: 46:46, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enh_clen;

   /* EnhCLSPHigh: This bit is only valid if the corresponding     */
   /* class is defined as an enhanced class in the ECM memory.     */
   /* If EnhCLSPHigh = 0, then the FQ complements the Class in     */
   /* low priority. If EnhCLSPHigh = 1, then the FQ                */
   /* complements the Class in high priority.                      */
   /* range: 47:47, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD enh_clsphigh;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_SCT_TBL;


  /* Flow to Queue Mapping (FQM): The FQM is a 16k-entry            */
  /* memory. Each entry relates Four flows to one Base Queue,       */
  /* Based on SubFlowMode. If SubFlowMode is set, then four         */
  /* queues are mapped to four even flows. Base_flow[15:2] is       */
  /* the address to the table. The mapping between the flow          */
  /* and the base queue is done Per four flows.                     */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x400b0000 */

   /* BaseQueueNum: Identifies the base queue number at the        */
   /* source pipe (FIP). See SubFLowMode for detialed              */
   /* explanation.                                                 */
   /* range: 12:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD base_queue_num;

   /* SubFlowMode: The flow maps to queue depending on             */
   /* SubFlowMode and InterDig. If InterDig = 0 and                */
   /* SubFlowMode = 0 then Q=\{BaseQueueNum, flow[1:0]\}. If       */
   /* InterDig = 0 and SubFlowMode = 1 then Q=\{BaseQueueNum,      */
   /* flow[2:1]\}. In this mode typically two adjacent entries     */
   /* should have the same BaseQueueNum and SubFlowMode in         */
   /* order to optimally the queue resources. If InterDig = 1      */
   /* and SubFlowMode = 0 then Q=\{BaseQueueNum, flow[2],          */
   /* flow[0]\}. In this mode typically two adjacent entries       */
   /* should have the same BaseQueueNum and SubFlowMode in         */
   /* order to optimally the queue resources. If InterDig = 1      */
   /* and SubFlowMode = 1 then Q=\{BaseQueueNum, flow[3:2]\}.      */
   /* In this mode typically four adjacent entries should have     */
   /* the same BaseQueueNum and SubFlowMode in order to            */
   /* optimally the queue resources.                               */
   /* range: 13:13, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD sub_flow_mode;

   /* FlowSlowEnable : A bit per each of the four flows in the     */
   /* entry. If the bit is set, then slow cos is valid for         */
   /* this flow. If it is not set, then a SLOW message is          */
   /* treated as NORMAL. For composite flows - both bits must      */
   /* be the same.                                                 */
   /* range: 17:14, access type: RW, default value: 0x0            */
   SOC_PETRA_TBL_FIELD flow_slow_enable;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_FQM_TBL;


  /* Flow to FIP Mapping (FFM): The FFM is an 8k-entry              */
  /* memory. Each entry maps a block of eight local flow-IDs        */
  /* to the source-FAP. (When reading each of these eight           */
  /* flows, the same source FAP id will be received. For            */
  /* writing the DeviceNumber associated with flow x, you           */
  /* need to access address \{x[13:3], 3'b0\}). Entries are         */
  /* meaningful only for mapping flows (And not for                 */
  /* aggregates).                                                   */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x400c0000 */

   /* DeviceNumber: Identifies the source device number to         */
   /* which the flow points.                                       */
   /* range: 10:0, access type: RW, default value: 0x0             */
   SOC_PETRA_TBL_FIELD device_number;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_FFM_TBL;


  /* Token Memory Controller (TMC): The TMC is a 56k-entry          */
  /* memory. The token memory holds the number of tokens            */
  /* currently available for a                                      */
  /* sub-flow/flow/scheduler. Updating the initial token count       */
  /* is done by updating the field MaxBurst and                     */
  /* MaxBurstUpdate in the SHDS. (Please consult API                */
  /* regarding update of initial values in this memory).            */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x40100000 */

   /* TokenCount: Number of tokens in the even bucket.             */
   /* range: 8:0, access type: RW, default value: 0x2              */
   SOC_PETRA_TBL_FIELD token_count;

   /* SlowStatus: The status of the Slow of the even flow.         */
   /* range: 9:9, access type: RW, default value: 0x1              */
   SOC_PETRA_TBL_FIELD slow_status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_TMC_TBL;


  /* Port Queue Size (PQS): The PQS is an 81-entry memory.          */
  /* Each entry holds the data relevant for system RED.             */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x401d0000 */

   /* MaxQSZ: Indicates the maximum qsize of the flow attached     */
   /* to the port.                                                 */
   /* range: 3:0, access type: R, default value: 0x0               */
   SOC_PETRA_TBL_FIELD max_qsz;

   /* FlowID: Indicates the flow that currently has the            */
   /* maximum queue size.                                          */
   /* range: 19:4, access type: R, default value: 0x0              */
   SOC_PETRA_TBL_FIELD flow_id;

   /* AgingBit: Indicates aging status of the Qsize value. If      */
   /* it is active, this means that the SMP updated the value      */
   /* since the last time that the aging reduced the port.         */
   /* range: 20:20, access type: R, default value: 0x0             */
   SOC_PETRA_TBL_FIELD aging_bit;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_PQS_TBL;


  /* Scheduler Init: Initialization of the scheduler is done        */
  /* in order to initialize all dynamic structures. The user        */
  /* should initialize the scheduler in the start up sequence       */
  /* by writing to the bit below. This is done by an indirect       */
  /* write to the address below, which will start a process         */
  /* that initializes the internal structures of the                */
  /* scheduler. The initialization process takes                    */
  /* approximately 56K core clocks to complete. To check if         */
  /* it is finished, read the register 0x040. Wait until the        */
  /* trigger is reset.                                              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x41000000 */

   /* SCHInit: Write 1 to this bit                                 */
   /* range: 0:0, access type: W, default value: 0x0               */
   SOC_PETRA_TBL_FIELD schinit;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_SCHEDULER_INIT_TBL;


  /* Force Status Message: This register enables the CPU to         */
  /* emulate a Flow Status Message. One application of this is       */
  /* for CBR/TDM traffic types. As a result, a steady stream        */
  /* of credit is guaranteed to the flow, eliminating the           */
  /* delay and jitter associated with the scheduling. The           */
  /* Write value contains the two fields of the status              */
  /* message. This message is not authenticated with the FFQ;       */
  /* hence there is no need for the other values. This              */
  /* address is Write only. To check if the operation has           */
  /* finished, read address 0x040.                                  */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x42000000 */

   /* MessageFlowId: The destination ID of the flow /scheduler     */
   /* of the message                                               */
   /* range: 15:0, access type: W, default value: 0x0              */
   SOC_PETRA_TBL_FIELD message_flow_id;

   /* MessageType : The message type 0: Fabric off 1: Fabric       */
   /* slow 2: Fabric fast                                          */
   /* range: 19:16, access type: W, default value: 0x0             */
   SOC_PETRA_TBL_FIELD message_type;

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_FORCE_STATUS_MESSAGE_TBL;


/* Block definition: OLP */
typedef struct
{
  SOC_PETRA_OLP_PGE_MEM_TBL pge_mem_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_OLP_TBLS;

/* Block definition: IRE */
typedef struct
{
  SOC_PETRA_IRE_NIF_CTXT_MAP_TBL nif_ctxt_map_tbl;

  SOC_PETRA_IRE_NIF_PORT2CTXT_BIT_MAP_TBL nif_port2ctxt_bit_map_tbl;

  SOC_PETRA_IRE_RCY_CTXT_MAP_TBL rcy_ctxt_map_tbl;

  SOC_PETRA_IRE_CPU_PACKET_COUNTER_TBL cpu_packet_counter_tbl;

  SOC_PETRA_IRE_OLP_PACKET_COUNTER_TBL olp_packet_counter_tbl;

  SOC_PETRA_IRE_NIFA_PACKET_COUNTER_TBL nifa_packet_counter_tbl;

  SOC_PETRA_IRE_NIFB_PACKET_COUNTER_TBL nifb_packet_counter_tbl;

  SOC_PETRA_IRE_RCY_PACKET_COUNTER_TBL rcy_packet_counter_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRE_TBLS;

/* Block definition: IDR */
typedef struct
{
  SOC_PETRA_IDR_COMPLETE_PC_TBL complete_pc_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IDR_TBLS;

/* Block definition: IRR */
typedef struct
{
  SOC_PETRA_IRR_IS_INGRESS_REPLICATION_DB_TBL is_ingress_replication_db_tbl;

  SOC_PETRA_IRR_INGRESS_REPLICATION_MULTICAST_DB_TBL ingress_replication_multicast_db_tbl;

  SOC_PETRA_IRR_EGRESS_REPLICATION_MULTICAST_DB_TBL egress_replication_multicast_db_tbl;

  SOC_PETRA_IRR_MIRROR_TABLE_TBL mirror_table_tbl;

  SOC_PETRA_IRR_SNOOP_TABLE_TBL snoop_table_tbl;

  SOC_PETRA_IRR_GLAG_TO_LAG_RANGE_TBL glag_to_lag_range_tbl;

  SOC_PETRA_IRR_SMOOTH_DIVISION_TBL smooth_division_tbl;

  SOC_PETRA_IRR_GLAG_MAPPING_TBL glag_mapping_tbl;

  SOC_PETRA_IRR_DESTINATION_TABLE_TBL destination_table_tbl;

  SOC_PETRA_IRR_GLAG_NEXT_MEMBER_TBL glag_next_member_tbl;

  SOC_PETRA_IRR_RLAG_NEXT_MEMBER_TBL rlag_next_member_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IRR_TBLS;

/* Block definition: IHP */
typedef struct
{
  SOC_PETRA_IHP_PORT_INFO_TBL port_info_tbl;

  SOC_PETRA_IHP_PORT_TO_SYSTEM_PORT_ID_TBL port_to_system_port_id_tbl;

  SOC_PETRA_IHP_STATIC_HEADER_TBL static_header_tbl;

  SOC_PETRA_IHP_SYSTEM_PORT_MY_PORT_TABLE_TBL system_port_my_port_table_tbl;

  SOC_PETRA_IHP_PTC_COMMANDS1_TBL ptc_commands1_tbl;

  SOC_PETRA_IHP_PTC_COMMANDS2_TBL ptc_commands2_tbl;

  SOC_PETRA_IHP_PTC_KEY_PROGRAM_LUT_TBL ptc_key_program_lut_tbl;

  SOC_PETRA_IHP_KEY_PROGRAM_TBL key_program_tbl;

  SOC_PETRA_IHP_KEY_PROGRAM0_TBL key_program0_tbl;

  SOC_PETRA_IHP_KEY_PROGRAM1_TBL key_program1_tbl;

  SOC_PETRA_IHP_KEY_PROGRAM2_TBL key_program2_tbl;

  SOC_PETRA_IHP_KEY_PROGRAM3_TBL key_program3_tbl;

  SOC_PETRA_IHP_KEY_PROGRAM4_TBL key_program4_tbl;

  SOC_PETRA_IHP_PROGRAMMABLE_COS_TBL programmable_cos_tbl;

  SOC_PETRA_IHP_PROGRAMMABLE_COS1_TBL programmable_cos1_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IHP_TBLS;

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

} __ATTRIBUTE_PACKED__ SOC_PETRA_IQM_TBLS;

/* Block definition: QDR */
typedef struct
{
  SOC_PETRA_QDR_MEM_TBL qdr_mem;

  SOC_PETRA_QDR_QDR_DLL_MEM_TBL qdr_dll_mem;

} __ATTRIBUTE_PACKED__ SOC_PETRA_QDR_TBLS;


/* Block definition: IPS */
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

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPS_TBLS;

/* Block definition: IPT */
typedef struct
{
  SOC_PETRA_IPT_BDQ_TBL bdq_tbl;

  SOC_PETRA_IPT_PCQ_TBL pcq_tbl;

  SOC_PETRA_IPT_SOP_MMU_TBL sop_mmu_tbl;

  SOC_PETRA_IPT_MOP_MMU_TBL mop_mmu_tbl;

  SOC_PETRA_IPT_FDTCTL_TBL fdtctl_tbl;

  SOC_PETRA_IPT_FDTDATA_TBL fdtdata_tbl;

  SOC_PETRA_IPT_EGQCTL_TBL egqctl_tbl;

  SOC_PETRA_IPT_EGQDATA_TBL egqdata_tbl;

  SOC_PETRA_IPT_SELECT_SOURCE_SUM_TBL select_source_sum_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_IPT_TBLS;

/* Block definition: DPI */
typedef struct
{
  SOC_PETRA_DPI_DLL_RAM_TBL dll_ram_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_DPI_TBLS;

/* Block definition: RTP */
typedef struct
{
  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_DATA_CELLS_TBL unicast_distribution_memory_for_data_cells_tbl;

  SOC_PETRA_RTP_UNICAST_DISTRIBUTION_MEMORY_FOR_CONTROL_CELLS_TBL unicast_distribution_memory_for_control_cells_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_RTP_TBLS;

/* Block definition: EGQ */
typedef struct
{
  SOC_PETRA_EGQ_NIF_SCM_TBL nif_scm_tbl;

  SOC_PETRA_EGQ_NIFAB_NCH_SCM_TBL nifab_nch_scm_tbl;

  SOC_PETRA_EGQ_RCY_SCM_TBL rcy_scm_tbl;

  SOC_PETRA_EGQ_CPU_SCM_TBL cpu_scm_tbl;

  SOC_PETRA_EGQ_CCM_TBL ccm_tbl;

  SOC_PETRA_EGQ_PMC_TBL pmc_tbl;

  SOC_PETRA_EGQ_CBM_TBL cbm_tbl;

  SOC_PETRA_EGQ_FBM_TBL fbm_tbl;

  SOC_PETRA_EGQ_FDM_TBL fdm_tbl;

  SOC_PETRA_EGQ_DWM_TBL dwm_tbl;

  SOC_PETRA_EGQ_RRDM_TBL rrdm_tbl;

  SOC_PETRA_EGQ_RPDM_TBL rpdm_tbl;

  SOC_PETRA_EGQ_PPCT_TBL ppct_tbl;

  SOC_PETRA_EGQ_PCT_TBL pct_tbl;

  SOC_PETRA_EGQ_VLAN_TABLE_TBL vlan_table_tbl;


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


} __ATTRIBUTE_PACKED__ SOC_PETRA_EGQ_TBLS;

/* Block definition: CFC */

typedef struct
{

  SOC_PETRA_EPNI_EPE_PACKET_COUNTER_TBL epe_packet_counter_tbl;

  SOC_PETRA_EPNI_EPE_BYTES_COUNTER_TBL epe_bytes_counter_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_EPNI_TBLS;


/* Block definition: CFC */
typedef struct
{
  SOC_PETRA_CFC_RECYCLE_TO_OUT_GOING_FAP_PORT_MAPPING_TBL recycle_to_out_going_fap_port_mapping_tbl;

  SOC_PETRA_CFC_NIF_A_CLASS_BASED_TO_OFP_MAPPING_TBL nif_a_class_based_to_ofp_mapping_tbl;

  SOC_PETRA_CFC_NIF_B_CLASS_BASED_TO_OFP_MAPPING_TBL nif_b_class_based_to_ofp_mapping_tbl;

  SOC_PETRA_CFC_A_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL a_schedulers_based_flow_control_to_ofp_mapping_tbl;

  SOC_PETRA_CFC_B_SCHEDULERS_BASED_FLOW_CONTROL_TO_OFP_MAPPING_TBL b_schedulers_based_flow_control_to_ofp_mapping_tbl;

  SOC_PETRA_CFC_OUT_OF_BAND_RX_A_CALENDAR_MAPPING_TBL out_of_band_rx_a_calendar_mapping_tbl;

  SOC_PETRA_CFC_OUT_OF_BAND_RX_B_CALENDAR_MAPPING_TBL out_of_band_rx_b_calendar_mapping_tbl;

  SOC_PETRA_CFC_OUT_OF_BAND_TX_CALENDAR_MAPPING_TBL out_of_band_tx_calendar_mapping_tbl;

} __ATTRIBUTE_PACKED__ SOC_PETRA_CFC_TBLS;

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

} __ATTRIBUTE_PACKED__ SOC_PETRA_SCH_TBLS;

  /* DRAM Address space: Data written to /read from the             */
  /* external DRAMAddress should be calculated in the               */
  /* following manner:\{row (X bits), column (X bits) , dram        */
  /* number (3 bits), bank number (3 bits)\}                        */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x00000000 */

   /* : Data to be written to / read from the external DRAM        */
   /* range: 511:0, access type: UNDEF, default value: 0x0         */
   SOC_PETRA_TBL_FIELD data;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL;


  /* IDF: IDF Indirect Mechanism                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x50000000 */

   /* : Data to be written to IDF                                  */
   /* range: 27:0, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD data;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_IDF_TBL;


  /* FDF: FDF Indirect Mechanism                                    */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x51000000 */

   /* : Data to be written to FDF                                  */
   /* range: 5:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD data;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_FDF_TBL;


  /* RdfaWaddrStatus: RDF WADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x52000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL;


  /* RdfbWaddrStatus: RDF WADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x53000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL;


  /* RdfcWaddrStatus: RDF WADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x54000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL;


  /* RdfdWaddrStatus: RDF WADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x55000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL;


  /* RdfeWaddrStatus: RDF WADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x56000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL;


  /* RdffWaddrStatus: RDF WADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x57000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL;


  /* RdfRaddr: RDF RADDR Indirect Mechanism                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x58000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RDF_RADDR_TBL;


  /* WafHalfaWaddr: WAF HALFA WADDR Indirect Mechanism              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x60000000 */

   /* : waddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAF_HALFA_WADDR_TBL;


  /* WafHalfbWaddr: WAF HALFB WADDR Indirect Mechanism              */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x61000000 */

   /* : waddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAF_HALFB_WADDR_TBL;


  /* WafaHalfaRaddrStatus: WAF HALFA RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x62000000 */

   /* : raddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_a;

   /* : status_half_a                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFA_HALFA_RADDR_STATUS_TBL;


  /* WafbHalfaRaddrStatus: WAF HALFA RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x63000000 */

   /* : raddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_a;

   /* : status_half_a                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFB_HALFA_RADDR_STATUS_TBL;


  /* WafcHalfaRaddrStatus: WAF HALFA RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x64000000 */

   /* : raddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_a;

   /* : status_half_a                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFC_HALFA_RADDR_STATUS_TBL;


  /* WafdHalfaRaddrStatus: WAF HALFA RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x65000000 */

   /* : raddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_a;

   /* : status_half_a                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFD_HALFA_RADDR_STATUS_TBL;


  /* WafeHalfaRaddrStatus: WAF HALFA RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x66000000 */

   /* : raddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_a;

   /* : status_half_a                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFE_HALFA_RADDR_STATUS_TBL;


  /* WaffHalfaRaddrStatus: WAF HALFA RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x67000000 */

   /* : raddr_half_a                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_a;

   /* : status_half_a                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_a;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFF_HALFA_RADDR_STATUS_TBL;


  /* WafaHalfbRaddrStatus: WAF HALFB RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x68000000 */

   /* : raddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_b;

   /* : status_half_b                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFA_HALFB_RADDR_STATUS_TBL;


  /* WafbHalfbRaddrStatus: WAF HALFB RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x69000000 */

   /* : raddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_b;

   /* : status_half_b                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFB_HALFB_RADDR_STATUS_TBL;


  /* WafcHalfbRaddrStatus: WAF HALFB RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x6a000000 */

   /* : raddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_b;

   /* : status_half_b                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFC_HALFB_RADDR_STATUS_TBL;


  /* WafdHalfbRaddrStatus: WAF HALFB RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x6b000000 */

   /* : raddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_b;

   /* : status_half_b                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFD_HALFB_RADDR_STATUS_TBL;


  /* WafeHalfbRaddrStatus: WAF HALFB RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x6c000000 */

   /* : raddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_b;

   /* : status_half_b                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFE_HALFB_RADDR_STATUS_TBL;


  /* WaffHalfbRaddrStatus: WAF HALFB RADDR + STATUS Indirect        */
  /* Mechanism                                                      */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x6d000000 */

   /* : raddr_half_b                                               */
   /* range: 3:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr_half_b;

   /* : status_half_b                                              */
   /* range: 8:4, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD status_half_b;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_WAFF_HALFB_RADDR_STATUS_TBL;


  /* RafWaddr: RAF WADDR Indirect Mechanism                         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x70000000 */

   /* : waddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD waddr;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAF_WADDR_TBL;


  /* RafaRaddrStatus: RAF RADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x71000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAFA_RADDR_STATUS_TBL;


  /* RafbRaddrStatus: RAF RADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x72000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAFB_RADDR_STATUS_TBL;


  /* RafcRaddrStatus: RAF RADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x73000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAFC_RADDR_STATUS_TBL;


  /* RafdRaddrStatus: RAF RADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x74000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAFD_RADDR_STATUS_TBL;


  /* RafeRaddrStatus: RAF RADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x75000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAFE_RADDR_STATUS_TBL;


  /* RaffRaddrStatus: RAF RADDR + STATUS Indirect Mechanism         */
typedef struct
{
   SOC_PETRA_TBL_ADDR addr;   /* 0x76000000 */

   /* : raddr                                                      */
   /* range: 4:0, access type: UNDEF, default value: 0x0           */
   SOC_PETRA_TBL_FIELD raddr;

   /* : status                                                     */
   /* range: 10:5, access type: UNDEF, default value: 0x0          */
   SOC_PETRA_TBL_FIELD status;

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_RAFF_RADDR_STATUS_TBL;


/* Block definition: MMU */
typedef struct
{
  SOC_PETRA_MMU_DRAM_ADDRESS_SPACE_TBL dram_address_space_tbl;

  SOC_PETRA_MMU_IDF_TBL idf_tbl;

  SOC_PETRA_MMU_FDF_TBL fdf_tbl;

  SOC_PETRA_MMU_RDFA_WADDR_STATUS_TBL rdfa_waddr_status_tbl;

  SOC_PETRA_MMU_RDFB_WADDR_STATUS_TBL rdfb_waddr_status_tbl;

  SOC_PETRA_MMU_RDFC_WADDR_STATUS_TBL rdfc_waddr_status_tbl;

  SOC_PETRA_MMU_RDFD_WADDR_STATUS_TBL rdfd_waddr_status_tbl;

  SOC_PETRA_MMU_RDFE_WADDR_STATUS_TBL rdfe_waddr_status_tbl;

  SOC_PETRA_MMU_RDFF_WADDR_STATUS_TBL rdff_waddr_status_tbl;

  SOC_PETRA_MMU_RDF_RADDR_TBL rdf_raddr_tbl;

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

} __ATTRIBUTE_PACKED__ SOC_PETRA_MMU_TBLS;

  /* Blocks definition } */

typedef struct
{
  SOC_PETRA_OLP_TBLS olp;
  SOC_PETRA_IRE_TBLS ire;
  SOC_PETRA_IDR_TBLS idr;
  SOC_PETRA_IRR_TBLS irr;
  SOC_PETRA_IHP_TBLS ihp;
  SOC_PETRA_IQM_TBLS iqm;
  SOC_PETRA_QDR_TBLS qdr;
  SOC_PETRA_IPS_TBLS ips;
  SOC_PETRA_IPT_TBLS ipt;
  SOC_PETRA_DPI_TBLS dpi;
  SOC_PETRA_RTP_TBLS rtp;
  SOC_PETRA_EGQ_TBLS egq;
  SOC_PETRA_EPNI_TBLS epni;
  SOC_PETRA_CFC_TBLS cfc;
  SOC_PETRA_SCH_TBLS sch;
  SOC_PETRA_MMU_TBLS mmu;
} __ATTRIBUTE_PACKED__ SOC_PA_TBLS;

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
* soc_pa_tbls_get
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Get a pointer to tables database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT  SOC_PA_TBLS  **soc_pa_tbls - pointer to soc_petra
*                           tables database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *soc_pa_tbls.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    soc_petra_is_tbl_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  soc_pa_tbls_get(
    SOC_SAND_OUT  SOC_PA_TBLS  **soc_pa_tbls
  );

/* Same functionality as soc_pa_tbls_get, but no error-checking */
SOC_PA_TBLS*
  soc_pa_tbls(void);

/*****************************************************
*NAME
*  soc_pa_tbls_init
*TYPE:
*  PROC
*DATE:
*  08/08/2007
*FUNCTION:
*  Dynamically allocates and initializes Soc_petra tables database.
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
  soc_pa_tbls_init(void);

/* } */


#include <soc/dpp/SAND/Utils/sand_footer.h>


/* } __SOC_PA_CHIP_TBLS_INCLUDED__*/
#endif
