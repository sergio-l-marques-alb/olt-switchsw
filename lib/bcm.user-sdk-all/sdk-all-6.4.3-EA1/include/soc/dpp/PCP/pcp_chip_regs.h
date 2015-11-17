/* $Id: pcp_chip_regs.h,v 1.5 Broadcom SDK $
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

#ifndef __SOC_PCP_CHIP_REGS_INCLUDED__
/* { */
#define __SOC_PCP_CHIP_REGS_INCLUDED__


/*************
 * INCLUDES  *
 *************/
/* { */
#include <soc/dpp/SAND/Utils/sand_header.h>
 
#include <soc/dpp/SAND/Utils/sand_framework.h>
#include <soc/dpp/PCP/pcp_chip_defines.h>
#include <soc/dpp/PCP/pcp_oam_api_general.h>
#include <soc/dpp/PCP/pcp_framework.h>


/* } */

/*************
 * DEFINES   *
 *************/
/* { */

#define  PCP_BLK_NOF_INSTANCES_ECI                1
#define  PCP_BLK_NOF_INSTANCES_CSI                1
#define  PCP_BLK_NOF_INSTANCES_ELK                1
#define  PCP_BLK_NOF_INSTANCES_OAM                1
#define  PCP_BLK_NOF_INSTANCES_STS                1
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)
#define  PCP_ELK_REGS_DSP_HEADER_REG_ARRAY_SIZE                                                     (2-1+1)

#define  PCP_ELK_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS                                              (6)
#define  PCP_ELK_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS                                                  (2)
#define  PCP_ELK_OLP_DSP_HEADER_REG_NOF_REGS                                                        (2)
#define  PCP_ELK_OLP_DSP_HEADER_REG_MULT_NOF_REGS                                                   (8)

#define  PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS                                                       (4)
#define  PCP_ELK_AGE_AGING_NOF_MODES                                                                (SOC_SAND_REG_SIZE_BITS / PCP_ELK_AGE_AGING_ENTRY_SIZE_IN_BITS)

#define  PCP_ELK_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS                                          (2)
#define  PCP_ELK_LOOKUP_NOF_MODES                                                                   (4)

#define  PCP_ELK_LOOKUP_MODE_ENTRY_SIZE_IN_BITS                                                     (5)
#define  PCP_ELK_LOOKUP_NOF_MODES                                                                   (4)

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

typedef struct
{
  uint32       base;
  uint16       step;
  uint16       size;
} __ATTRIBUTE_PACKED__ PCP_REG_ADDR;

typedef struct
{
  uint32       base;
  uint16       step;
  uint16       size;
  uint16       msb;
  uint16       lsb;
} __ATTRIBUTE_PACKED__ PCP_REG_FIELD;

/* Blocks definition { */
/* Block definition: ECI 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  PCP_REG_ADDR addr;

  /* Interrupt Register: Each bit in this register                  */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register.                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20000 */

     /* CSIInterrupt: Interrupt Indication asserted by CSI. This     */
     /* bit is set to 1'b1 upon an interrupt event.                  */
     /* range: 0:0, access type: RC, default value: 0x0              */
     PCP_REG_FIELD csiinterrupt;

     /* ELKInterrupt: Interrupt Indication asserted by ELK           */
     /* interrupt. This bit is set to 1'b1 upon an interrupt         */
     /* event.                                                       */
     /* range: 1:1, access type: RC, default value: 0x0              */
     PCP_REG_FIELD elkinterrupt;

     /* OAMPInterrupt: Interrupt Indication asserted by OAMP         */
     /* interrupt. This bit is set to 1'b1 upon an interrupt         */
     /* event.                                                       */
     /* range: 2:2, access type: RC, default value: 0x0              */
     PCP_REG_FIELD oampinterrupt;

     /* STSInterrupt: Interrupt Indication asserted by STS           */
     /* interrupt. This bit is set to 1'b1 upon an interrupt         */
     /* event.                                                       */
     /* range: 3:3, access type: RC, default value: 0x0              */
     PCP_REG_FIELD stsinterrupt;

     /* ECIInterrupt: Interrupt Indication asserted by ECI           */
     /* interrupt. Detailed interrupt cause should be read from      */
     /* the ECI Errors register. This bit is set to 1'b1 upon an     */
     /* interrupt event                                              */
     /* range: 4:4, access type: RC, default value: 0x0              */
     PCP_REG_FIELD eciinterrupt;

  } __ATTRIBUTE_PACKED__ interrupt_reg;


  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register.                             */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20010 */

     /* CSIInterruptMask: CSI Interrupt Mask.Setting this bit to     */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD csiinterrupt_mask;

     /* ELKInterruptMask: ELK Interrupt Mask.Setting this bit to     */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD elkinterrupt_mask;

     /* OAMPInterruptMask: OAMP Interrupt Mask.Setting this bit      */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD oampinterrupt_mask;

     /* STSInterruptMask: STS Interrupt Mask.Setting this bit to     */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 3:3, access type: RW, default value: 0x0              */
     PCP_REG_FIELD stsinterrupt_mask;

     /* ECIInterruptMask: ECI Interrupt Mask.Setting this bit to     */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 4:4, access type: RW, default value: 0x0              */
     PCP_REG_FIELD eciinterrupt_mask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20020 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_0;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20021 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_1;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20022 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 7:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_2;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20030 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_0;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20031 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_1;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20032 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 7:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_2;


  /* Indirect Command: Indirect Commands. Used to send              */
  /* indirect commands and receive their status                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20040 */

     /* IndirectCommandTrigger: Triggers indirect access as          */
     /* defined by the rest of the registers. The negation of        */
     /* this bit indicates that the operation has been               */
     /* completed.                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger;

     /* IndirectCommandTriggerOnData: If set, then writing to        */
     /* the LSBs of IndirectCommandWrData automatically set          */
     /* IndirectCommandTrigger.                                      */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger_on_data;

     /* IndirectCommandCount: Each write command is executed         */
     /* IndirectCommandCount number of times. The address is         */
     /* advanced by one every write command. If set to 0, one        */
     /* operation is performed.The CPU can read this field to        */
     /* find the number of writes left.                              */
     /* range: 15:2, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_count;

     /* IndirectCommandTimeout: Configures a timeout period for      */
     /* the indirect access command. Resolution is in 256            */
     /* clocks. If set to 0, then the command has no timeout.        */
     /* range: 30:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_timeout;

     /* IndirectCommandStatus: Status of last indirect access        */
     /* command. If set, then last command failed on timeout.        */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_status;

  } __ATTRIBUTE_PACKED__ indirect_command_reg;


  /* Indirect Command Address: Indirect access address.             */
  /* Defines which indirect object is accessed                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20041 */

     /* IndirectCommandAddr: Indirect access address. Defines        */
     /* which indirect object is accessed                            */
     /* range: 30:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_addr;

     /* IndirectCommandType: Type of command to perform: 0 -         */
     /* Write operation 1 - Read operation                           */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_type;

  } __ATTRIBUTE_PACKED__ indirect_command_address_reg;


  /* Debug subver:                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2004f */

     /* Debug_subver: FPGA internal debug sub version                */
     /* range: 15:0, access type: RO, default value: 0x1             */
     PCP_REG_FIELD debug_subver;

  } __ATTRIBUTE_PACKED__ debug_subver_reg;


  /* Rev Reg:                                                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20050 */

     /* Chip_Type: hardware chip type                                */
     /* range: 19:0, access type: RO, default value: 20'hdc150       */
     PCP_REG_FIELD chip_type;

     /* Dbg_Ver: hardware debug version                              */
     /* range: 27:20, access type: RO, default value: 0x4            */
     PCP_REG_FIELD dbg_ver;

     /* Chip_Ver: hardware chip version                              */
     /* range: 31:28, access type: RO, default value: 0x0            */
     PCP_REG_FIELD chip_ver;

  } __ATTRIBUTE_PACKED__ rev_reg;


  /* Scratch Register: Scratch Register                             */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20051 */

     /* ScratchRegister: Scratch register, for read/write tests      */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD scratch_register;

  } __ATTRIBUTE_PACKED__ scratch_reg;


  /* Cpu Acc Per Clk Reg:                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20052 */

     /* Cpu_Acc_Per_Clk: Number of system clocks between two CPU     */
     /* accesses, controls the allocated bandwidth to CPU            */
     /* range: 8:0, access type: RW, default value: 0x40             */
     PCP_REG_FIELD cpu_acc_per_clk;

  } __ATTRIBUTE_PACKED__ cpu_acc_per_clk_reg;


  /* eif phy cfg ctrl:                                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20053 */

     /* Phy_Write: Reconfigure Write Command, Serdes                 */
     /* configuration is triggered by rising edge on this bit        */
     /* and ends when busy bits are back to 0                        */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD phy_write;

     /* Phy_Busy: Reconfigure Busy                                   */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD phy_busy;

  } __ATTRIBUTE_PACKED__ eif_phy_cfg_ctrl_reg;


  /* eif phy cfg val:                                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20054 */

     /* Rx_Eqctrl: Equalization Control                              */
     /* range: 3:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rx_eqctrl;

     /* Rx_Eqdcgain: Equalizer DC Gain Control                       */
     /* range: 5:4, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rx_eqdcgain;

     /* Tx_Preemp: Pre tap Pre-emphasis Control                      */
     /* range: 10:6, access type: RW, default value: 0x0             */
     PCP_REG_FIELD tx_preemp;

     /* Tx_Vodctrl: Voltage Output Differential Control              */
     /* range: 13:11, access type: RW, default value: 0x0            */
     PCP_REG_FIELD tx_vodctrl;

  } __ATTRIBUTE_PACKED__ eif_phy_cfg_val_reg;


  /* Eif Mac Ctrl:                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20055 */

     /* Eif_Enable_Rx: TG1 Global after-reset enable for the         */
     /* receive part of the core. Can be 0 after reset and until     */
     /* the system is able to receive frames. Any line activity      */
     /* is ignored. Once enabled (1) it must not become              */
     /* de-asserted during operation again.                          */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD eif_enable_rx;

     /* Eif_Enable_Tx: EIF Global  after-reset enable for the        */
     /* transmit part of the core. Can be 0 after reset and          */
     /* until the system is able to transmit frames. Any line        */
     /* activity is ignored.  Once enabled (1) it must not           */
     /* become de-asserted during operation again                    */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD eif_enable_tx;

     /* Eif_Ing_Fc: elk Ingress Flow Control Enable                  */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD eif_ing_fc;

     /* Eif_Link_Ok: No link on elk interface                        */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD eif_link_ok;

  } __ATTRIBUTE_PACKED__ eif_mac_ctrl_reg;


  /* Eif Debug Reg:                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20056 */

     /* phy_rx_dig_reset: digital reset for rx PHY of EIF            */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD phy_rx_dig_reset;

     /* phy_tx_dig_reset: digital reset for tx PHY of EIF            */
     /* range: 1:1, access type: RW, default value: 0x1              */
     PCP_REG_FIELD phy_tx_dig_reset;

     /* phy_rx_anal_reset: analog reset for rx PHY of EIF            */
     /* range: 2:2, access type: RW, default value: 0x1              */
     PCP_REG_FIELD phy_rx_anal_reset;

     /* mac_tx_reset: reset for tx MAC of EIF                        */
     /* range: 3:3, access type: RW, default value: 0x1              */
     PCP_REG_FIELD mac_tx_reset;

     /* mac_rx_reset: reset for rx MAC of EIF                        */
     /* range: 4:4, access type: RW, default value: 0x1              */
     PCP_REG_FIELD mac_rx_reset;

     /* phy_freq_locked: eif serdes locked to incoming clock         */
     /* range: 5:5, access type: RO, default value: 0x0              */
     PCP_REG_FIELD phy_freq_locked;

     /* phy_pll_locked: eif serdes pll locked to ref clock           */
     /* range: 6:6, access type: RO, default value: 0x0              */
     PCP_REG_FIELD phy_pll_locked;

     /* mac_fifo_underflow: mac fifo underflow indication            */
     /* latched                                                      */
     /* range: 7:7, access type: RO, default value: 0x0              */
     PCP_REG_FIELD mac_fifo_underflow;

     /* mac_fifo_overflow: mac fifo overflow indication latched      */
     /* range: 8:8, access type: RO, default value: 0x0              */
     PCP_REG_FIELD mac_fifo_overflow;

  } __ATTRIBUTE_PACKED__ eif_debug_reg;


  /* Gen Cfg Reg:                                                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20057 */

     /* Rldram_576mb: A one indicate the use on 576mb RLDRAM         */
     /* with 20 active address bits and a zero indicate 288mb        */
     /* device with 19 active address bits                           */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD rldram_576mb;

     /* Qdr_36mb: A one indicate the use on 36mb QDR with 21         */
     /* active address bits and a zero indicate 18mb device with     */
     /* 20 active address bits                                       */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD qdr_36mb;

  } __ATTRIBUTE_PACKED__ gen_cfg_reg;


  /* Mem Init Reg:                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20058 */

     /* rld_init_done: 0 - following reset, when one indicate        */
     /* that RLDRAM HW init sequence is done (according to           */
     /* RLDRAM spec)                                                 */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rld_init_done;

     /* qdc_init_done: 0 - following reset, when one indicate        */
     /* that QDC sram training is done                               */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD qdc_init_done;

     /* mem_init_done: 0 - following reset, when set to one          */
     /* indicate to hardware that all memory init are done,          */
     /* which allow normal operation of main state machine (FSM)     */
     /* and start memory initialization. Setting back to 0 will      */
     /* not make a difference                                        */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD mem_init_done;

  } __ATTRIBUTE_PACKED__ mem_init_reg;


  /* General Interrupt Mask Register: Each bit in this              */
  /* register corresponds to an interrupt source in the             */
  /* Interrupt Register. The interrupt source is masked by          */
  /* writing 0 to the relevant bit in this register.                */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x20059 */

     /* FPGA_InterruptMask: FPGA Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD fpga_interrupt_mask;

     /* Soc_petra_InterruptMask: Soc_petra Interrupt Mask.Setting this       */
     /* bit to 1'b0 masks the interrupt.                             */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD soc_petra_interrupt_mask;

  } __ATTRIBUTE_PACKED__ general_interrupt_mask_reg;


  /* ECI Error Reg: ECI Errors Register                             */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2005a */

     /* PCIE_ERROR: PCIE Error indication from PCIe RX path This     */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD pcie_error;

     /* qdr_underflow: QDR fifo underflow This register is clear     */
     /* on read.                                                     */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD qdr_underflow;

     /* qdr_overflow: QDR fifo overflow This register is clear       */
     /* on read.                                                     */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD qdr_overflow;

     /* rld1_underflow: RLDRAM 1 fifo underflow This register is     */
     /* clear on read.                                               */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rld1_underflow;

     /* rld1_overflow: RLDRAM 1 fifo overflow This register is       */
     /* clear on read.                                               */
     /* range: 4:4, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rld1_overflow;

     /* rld2_underflow: RLDRAM 2 fifo underflow This register is     */
     /* clear on read.                                               */
     /* range: 5:5, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rld2_underflow;

     /* rld2_overflow: RLDRAM 2 fifo overflow This register is       */
     /* clear on read.                                               */
     /* range: 6:6, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rld2_overflow;

  } __ATTRIBUTE_PACKED__ eci_error_reg;


  /* debug fifo:                                                    */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2005b */

     /* debug_fifo: Head of debug fifo This register is clear on     */
     /* read.                                                        */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD debug_fifo;

  } __ATTRIBUTE_PACKED__ debug_fifo_reg;

} __ATTRIBUTE_PACKED__ PCP_ECI_REGS;
/* Block definition: CSI 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  PCP_REG_ADDR addr;

  /* Interrupt Register: Each bit in this register                  */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register.                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24000 */

     /* CglFifoFull: If this bit is set to 1'b1, it indicates        */
     /* that CGL Egress FIFO full event has happened                 */
     /* range: 0:0, access type: RC, default value: 0x0              */
     PCP_REG_FIELD cgl_fifo_full;

     /* CglCmdErr: If this bit is set to 1'b1, it indicates that     */
     /* CGL detected command error according to register             */
     /* CglCmdErr                                                    */
     /* range: 1:1, access type: RC, default value: 0x0              */
     PCP_REG_FIELD cgl_cmd_err;

     /* SmxTprLowErr: SMX TPR low indication.                        */
     /* range: 2:2, access type: RC, default value: 0x0              */
     PCP_REG_FIELD smx_tpr_low_err;

     /* SmxRprThreshErr: SMX RPR Threshhold indication.              */
     /* range: 3:3, access type: RC, default value: 0x0              */
     PCP_REG_FIELD smx_rpr_thresh_err;

     /* TpmFifoOvf: TPM FIFO overflow. Indicates that one of the     */
     /* fifo's overflow event has occurred.Violating FIFO            */
     /* indicated in TpmFifoErr register. Clear on read.             */
     /* range: 4:4, access type: RC, default value: 0x0              */
     PCP_REG_FIELD tpm_fifo_ovf;

     /* RpmFifoOvf: RPM FIFO overflow. Indicates that one of the     */
     /* fifo's overflow event has occurred. Violating FIFO           */
     /* indicated in RpmFifoErr register. Clear on read.             */
     /* range: 5:5, access type: RC, default value: 0x0              */
     PCP_REG_FIELD rpm_fifo_ovf;

     /* RpmPktErr: If this bit is set to 1'b1, it indicates that     */
     /* RPM detected packet error according to register              */
     /* RpmPktErr                                                    */
     /* range: 6:6, access type: RC, default value: 0x0              */
     PCP_REG_FIELD rpm_pkt_err;

     /* IngressEOB: Finished reading an Ingress buffer and           */
     /* returned descriptor ownership to CPU.                        */
     /* range: 12:12, access type: RC, default value: 0x0            */
     PCP_REG_FIELD ingress_eob;

     /* IngressEOP: Finished reading an Ingress buffer and           */
     /* returned descriptor ownership to CPU. Buffer was flagged     */
     /* as EOP.                                                      */
     /* range: 13:13, access type: RC, default value: 0x0            */
     PCP_REG_FIELD ingress_eop;

     /* DmaError: DMA Error identified and stored at the DmaErr      */
     /* register.                                                    */
     /* range: 14:14, access type: RC, default value: 0x0            */
     PCP_REG_FIELD dma_error;

     /* Class1EOB: Finished writing a buffer that was assigned       */
     /* for Class 1 and returned descriptor ownership to CPU.        */
     /* range: 16:16, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class1_eob;

     /* Class2EOB: Finished writing a buffer that was assigned       */
     /* for Class 2 and returned descriptor ownership to CPU.        */
     /* range: 17:17, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class2_eob;

     /* Class3EOB: Finished writing a buffer that was assigned       */
     /* for Class 3 and returned descriptor ownership to CPU.        */
     /* range: 18:18, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class3_eob;

     /* Class4EOB: Finished writing a buffer that was assigned       */
     /* for Class 4 and returned descriptor ownership to CPU.        */
     /* range: 19:19, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class4_eob;

     /* Class5EOB: Finished writing a buffer that was assigned       */
     /* for Class 5 and returned descriptor ownership to CPU.        */
     /* range: 20:20, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class5_eob;

     /* Class6EOB: Finished writing a buffer that was assigned       */
     /* for Class 6 and returned descriptor ownership to CPU.        */
     /* range: 21:21, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class6_eob;

     /* Class7EOB: Finished writing a buffer that was assigned       */
     /* for Class 7 and returned descriptor ownership to CPU.        */
     /* range: 22:22, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class7_eob;

     /* Class1EOP: Finished writing a buffer that was assigned       */
     /* for Class 1 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 24:24, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class1_eop;

     /* Class2EOP: Finished writing a buffer that was assigned       */
     /* for Class 2 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 25:25, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class2_eop;

     /* Class3EOP: Finished writing a buffer that was assigned       */
     /* for Class 3 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 26:26, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class3_eop;

     /* Class4EOP: Finished writing a buffer that was assigned       */
     /* for Class 4 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 27:27, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class4_eop;

     /* Class5EOP: Finished writing a buffer that was assigned       */
     /* for Class 5 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 28:28, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class5_eop;

     /* Class6EOP: Finished writing a buffer that was assigned       */
     /* for Class 6 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 29:29, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class6_eop;

     /* Class7EOP: Finished writing a buffer that was assigned       */
     /* for Class 7 and returned descriptor ownership to CPU.        */
     /* Buffer was flagged as EOP.                                   */
     /* range: 30:30, access type: RC, default value: 0x0            */
     PCP_REG_FIELD class7_eop;

  } __ATTRIBUTE_PACKED__ interrupt_reg;


  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register.                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24010 */

     /* CGLFifoFullMask: CGL Interrupt mask. Setting this bit to     */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD cglfifo_full_mask;

     /* CglCmdErrMask: CGL Interrupt mask. Setting this bit to       */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD cgl_cmd_err_mask;

     /* SMXTprLowErrMask: SMX TPR low indication mask. Setting       */
     /* this bit to 1'b0 masks the interrupt.                        */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD smxtpr_low_err_mask;

     /* SMXRprThreshErrMask: SMX RPR Threshhold indication mask.     */
     /* Setting this bit to 1'b0 masks the interrupt.                */
     /* range: 3:3, access type: RW, default value: 0x0              */
     PCP_REG_FIELD smxrpr_thresh_err_mask;

     /* TpmFifoOvfMask: TPM Interrupt mask. Setting this bit to      */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 4:4, access type: RW, default value: 0x0              */
     PCP_REG_FIELD tpm_fifo_ovf_mask;

     /* RpmFifoOvfMask: RPM Interrupt mask. Setting this bit to      */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 5:5, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rpm_fifo_ovf_mask;

     /* RpmPktErrMask: RPM Interrupt mask. Setting this bit to       */
     /* 1'b0 masks the interrupt.                                    */
     /* range: 6:6, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rpm_pkt_err_mask;

     /* IngressEOBMask: IngressEOB Interrupt Mask.Setting this       */
     /* bit to 1'b0 masks the interrupt.                             */
     /* range: 12:12, access type: RW, default value: 0x0            */
     PCP_REG_FIELD ingress_eobmask;

     /* IngressEOPMask: IngressEOP Interrupt Mask.Setting this       */
     /* bit to 1'b0 masks the interrupt.                             */
     /* range: 13:13, access type: RW, default value: 0x0            */
     PCP_REG_FIELD ingress_eopmask;

     /* DmaErrorMask: DmaError Interrupt mask. Setting this bit      */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 14:14, access type: RW, default value: 0x0            */
     PCP_REG_FIELD dma_error_mask;

     /* Class1EOBMask: Class1EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 16:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class1_eobmask;

     /* Class2EOBMask: Class2EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 17:17, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class2_eobmask;

     /* Class3EOBMask: Class3EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 18:18, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class3_eobmask;

     /* Class4EOBMask: Class4EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 19:19, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class4_eobmask;

     /* Class5EOBMask: Class5EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 20:20, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class5_eobmask;

     /* Class6EOBMask: Class6EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 21:21, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class6_eobmask;

     /* Class7EOBMask: Class7EOB Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 22:22, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class7_eobmask;

     /* Class1EOPMask: Class1EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 24:24, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class1_eopmask;

     /* Class2EOPMask: Class2EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 25:25, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class2_eopmask;

     /* Class3EOPMask: Class3EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 26:26, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class3_eopmask;

     /* Class4EOPMask: Class4EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 27:27, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class4_eopmask;

     /* Class5EOPMask: Class5EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 28:28, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class5_eopmask;

     /* Class6EOPMask: Class6EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 29:29, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class6_eopmask;

     /* Class7EOPMask: Class7EOP Interrupt Mask.Setting this bit     */
     /* to 1'b0 masks the interrupt.                                 */
     /* range: 30:30, access type: RW, default value: 0x0            */
     PCP_REG_FIELD class7_eopmask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;


  /* Csi Cmd: Csi commands                                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24080 */

     /* CsiRst: Reset Csi block. Set to 1'b1 to reset the block      */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD csi_rst;

     /* CsiInit: CSI Module Init.                                    */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD csi_init;

     /* CsiOe: if set to '1' CSI drives output streaming signals     */
     /* to Soc_petra, else high-z                                        */
     /* range: 2:2, access type: RW, default value: 0x1              */
     PCP_REG_FIELD csi_oe;

  } __ATTRIBUTE_PACKED__ csi_cmd_reg;


  /* PUC: statistics Power Up Config                                */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24090 */

     /* PUCData: Power Up Config data                                */
     /* range: 19:0, access type: RW, default value: 0xecc9          */
     PCP_REG_FIELD pucdata;

     /* PUCEnable: 0' - don't drive stat bus, '1' - drive stat       */
     /* bus                                                          */
     /* range: 20:20, access type: RW, default value: 0x1            */
     PCP_REG_FIELD pucenable;

  } __ATTRIBUTE_PACKED__ puc_reg;


  /* Cgl Cmd Err: CGL Command Error Register                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24095 */

     /* invalidate_completion: CGL Detected Command Error.           */
     /* Invalidate (disregard) register read completion. This        */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD invalidate_completion;

     /* read_signature_error: CGL Detected Command Error. Read       */
     /* signature error. This register is clear on read.             */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD read_signature_error;

     /* read_unsuccess_error: CGL Detected Command Error. Read       */
     /* unsuccess error. This register is clear on read.             */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD read_unsuccess_error;

     /* read_parity_error: CGL Detected Command Error. Read          */
     /* parity error This register is clear on read.                 */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD read_parity_error;

     /* write_signature_error: CGL Detected Command Error. Write     */
     /* signature error. This register is clear on read.             */
     /* range: 4:4, access type: RO, default value: 0x0              */
     PCP_REG_FIELD write_signature_error;

     /* write_unsuccess_error: CGL Detected Command Error. Write     */
     /* unsuccess error. This register is clear on read.             */
     /* range: 5:5, access type: RO, default value: 0x0              */
     PCP_REG_FIELD write_unsuccess_error;

     /* write_parity_error: CGL Detected Command Error. Write        */
     /* parity error. This register is clear on read.                */
     /* range: 6:6, access type: RO, default value: 0x0              */
     PCP_REG_FIELD write_parity_error;

     /* read_reply_invalid: CGL Detected Command Error. Read         */
     /* reply invalid. This register is clear on read.               */
     /* range: 7:7, access type: RO, default value: 0x0              */
     PCP_REG_FIELD read_reply_invalid;

     /* write_reply_invalid: CGL Detected Command Error. Write       */
     /* reply invalid. This register is clear on read.               */
     /* range: 8:8, access type: RO, default value: 0x0              */
     PCP_REG_FIELD write_reply_invalid;

     /* cmd_formation_error: CGL Detected Command Error. Command     */
     /* formation error. This register is clear on read.             */
     /* range: 9:9, access type: RO, default value: 0x0              */
     PCP_REG_FIELD cmd_formation_error;

     /* missing_start_of_cmd: CGL Detected Command Error.            */
     /* Missing start of cmd. This register is clear on read.        */
     /* range: 10:10, access type: RO, default value: 0x0            */
     PCP_REG_FIELD missing_start_of_cmd;

     /* read_reply_timeout: CGL Detected Command Error. Read         */
     /* reply timeout expired. This register is clear on read.       */
     /* range: 11:11, access type: RO, default value: 0x0            */
     PCP_REG_FIELD read_reply_timeout;

     /* write_reply_timeout: CGL Detected Command Error. Write       */
     /* reply timeout expired. This register is clear on read.       */
     /* range: 12:12, access type: RO, default value: 0x0            */
     PCP_REG_FIELD write_reply_timeout;

  } __ATTRIBUTE_PACKED__ cgl_cmd_err_reg;


  /* CGL Status: CGL block status indications                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24100 */

     /* CGLEgressFifoUsedDW: Number of data words currently in       */
     /* use in the CGL Egress FIFO                                   */
     /* range: 9:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD cglegress_fifo_used_dw;

     /* CGLEgressFifoEmpty: If this bit is set to 1'b1, it           */
     /* indicates that CGL Egress FIFO is empty                      */
     /* range: 16:16, access type: RO, default value: 0x1            */
     PCP_REG_FIELD cglegress_fifo_empty;

     /* CGLEgressFifoFull: If this bit is set to 1'b1, it            */
     /* indicates that CGL Egress FIFO is full                       */
     /* range: 20:20, access type: RO, default value: 0x0            */
     PCP_REG_FIELD cglegress_fifo_full;

     /* CglAf: CGL pending command pipe Almost Full indication       */
     /* range: 21:21, access type: RO, default value: 0x0            */
     PCP_REG_FIELD cgl_af;

  } __ATTRIBUTE_PACKED__ cgl_status_reg;


  /* Tpm Pkt CFG: Configuration register for packet transmit        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24101 */

     /* TpmPktFormat: If this bit is set to 1'b1, incomming          */
     /* packet header is Fabric format. Otherwise it is              */
     /* Incomming format.                                            */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD tpm_pkt_format;

  } __ATTRIBUTE_PACKED__ tpm_pkt_cfg_reg;


  /* Tpm Fifo Err: TPM FIFO Overflow information                    */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24102 */

     /* TpmDmaFifoFull: DMA Outgoing FIFO is Full. This register     */
     /* is clear on read.                                            */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD tpm_dma_fifo_full;

     /* TpmElkFifoFull: ELK Outgoing FIFO is Full. This register     */
     /* is clear on read.                                            */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD tpm_elk_fifo_full;

     /* TpmOamFifoFull: OAM Outgoing FIFO is Full. This register     */
     /* is clear on read.                                            */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD tpm_oam_fifo_full;

  } __ATTRIBUTE_PACKED__ tpm_fifo_err_reg;


  /* Rpm Stat Cfg: RPM Status Channel Configuration                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24103 */

     /* RpmStatCalLen: RPM Status Channel Calendar Length.           */
     /* Number of channel entries in the status channel              */
     /* calendar, range 1 to 7.                                      */
     /* range: 3:0, access type: RW, default value: 0x7              */
     PCP_REG_FIELD rpm_stat_cal_len;

     /* RpmStatCalM: RPM Status Channel Calendar M. Number of        */
     /* status channel repeatitions.                                 */
     /* range: 7:4, access type: RW, default value: 0x1              */
     PCP_REG_FIELD rpm_stat_cal_m;

     /* RpmStatEn: RPM status channel Enable. Set to 1'b1 to         */
     /* enable status channel for Out of Band Flow Control.          */
     /* range: 8:8, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rpm_stat_en;

  } __ATTRIBUTE_PACKED__ rpm_stat_cfg_reg;


  /* Rpm Pkt Pause Timeout: RPM Packet Pause Timeout                */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24104 */

     /* RpmPktPauseTimeout: RPM Packet pause timeout watchdog.       */
     /* Value is used to count streaming interface clock cycles      */
     /* when Soc_petra is pausing a packet burst that already been       */
     /* started.                                                     */
     /* range: 31:0, access type: RW, default value: 32'h80ffffff    */
     PCP_REG_FIELD rpm_pkt_pause_timeout;

  } __ATTRIBUTE_PACKED__ rpm_pkt_pause_timeout_reg;


  /* Rpm Pkt Err: RPM Packet Error Register                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24105 */

     /* RpmErrIgnoreData: RPM Detected Packet Error. Packet data     */
     /* should be ignored. This register is clear on read.           */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_err_ignore_data;

     /* RpmErrForceEop: RPM Detected Packet Error. RPM Forced        */
     /* end of packet signalling. This register is clear on          */
     /* read.                                                        */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_err_force_eop;

     /* RpmErrMissingSop: RPM Detected Packet Error. Missing         */
     /* Start of Burst. This register is clear on read.              */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_err_missing_sop;

     /* RpmErrUnexpectedSop: RPM Detected Packet Error.              */
     /* Unexpected Start of Burst. This register is clear on         */
     /* read.                                                        */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_err_unexpected_sop;

     /* RpmErrParityDuringPacket: RPM Detected Packet Error.         */
     /* Parity Error. This register is clear on read.                */
     /* range: 4:4, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_err_parity_during_packet;

     /* RpmErrEopTimeout: RPM Detected Packet Error. Packet          */
     /* pause period reached timeout. This register is clear on      */
     /* read.                                                        */
     /* range: 5:5, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_err_eop_timeout;

  } __ATTRIBUTE_PACKED__ rpm_pkt_err_reg;


  /* Rpm Map Class1: Class 1 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24108 */

     /* RpmMapClass1: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 1.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rpm_map_class1;

  } __ATTRIBUTE_PACKED__ rpm_map_class1_reg;


  /* Rpm Map Class2: Class 2 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24109 */

     /* RpmMapClass2: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 2.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD rpm_map_class2;

  } __ATTRIBUTE_PACKED__ rpm_map_class2_reg;


  /* Rpm Map Class3: Class 3 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2410a */

     /* RpmMapClass3: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 3.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x2              */
     PCP_REG_FIELD rpm_map_class3;

  } __ATTRIBUTE_PACKED__ rpm_map_class3_reg;


  /* Rpm Map Class4: Class 4 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2410b */

     /* RpmMapClass4: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 4.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x3              */
     PCP_REG_FIELD rpm_map_class4;

  } __ATTRIBUTE_PACKED__ rpm_map_class4_reg;


  /* Rpm Map Class5: Class 5 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2410c */

     /* RpmMapClass5: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 5.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x4              */
     PCP_REG_FIELD rpm_map_class5;

  } __ATTRIBUTE_PACKED__ rpm_map_class5_reg;


  /* Rpm Map Class6: Class 6 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2410d */

     /* RpmMapClass6: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 6.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x5              */
     PCP_REG_FIELD rpm_map_class6;

  } __ATTRIBUTE_PACKED__ rpm_map_class6_reg;


  /* Rpm Map Class7: Class 7 mapping to incoming streaming          */
  /* channel                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2410e */

     /* RpmMapClass7: This register selects a streaming channel      */
     /* number (0-63) to be mapped into DMA engine Class 7.          */
     /* Channel number must be unique and should not be selected     */
     /* by any other DMA Class or ELK or OAM engine.                 */
     /* range: 5:0, access type: RW, default value: 0x6              */
     PCP_REG_FIELD rpm_map_class7;

  } __ATTRIBUTE_PACKED__ rpm_map_class7_reg;


  /* Rpm Map Elk: ELK mapping to incoming streaming channel         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2410f */

     /* RpmMapElk: This register selects a streaming channel         */
     /* number (0-63) to be mapped into ELK engine. Channel          */
     /* number must be unique and should not be selected by any      */
     /* other DMA Class or OAM engine.                               */
     /* range: 5:0, access type: RW, default value: 0x8              */
     PCP_REG_FIELD rpm_map_elk;

  } __ATTRIBUTE_PACKED__ rpm_map_elk_reg;


  /* Rpm Map Oam: OAM mapping to incoming streaming channel         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24110 */

     /* RpmMapOam: This register selects a streaming channel         */
     /* number (0-63) to be mapped into OAM engine. Channel          */
     /* number must be unique and should not be selected by any      */
     /* other DMA Class or ELK engine.                               */
     /* range: 5:0, access type: RW, default value: 0x9              */
     PCP_REG_FIELD rpm_map_oam;

  } __ATTRIBUTE_PACKED__ rpm_map_oam_reg;


  /* Rpm Class1 Stat Slot: RPM Status channel mapping of            */
  /* Class 1 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24111 */

     /* RpmClass1StatSlot: Class 1 flow control status time slot     */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rpm_class1_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class1_stat_slot_reg;


  /* Rpm Class2 Stat Slot: RPM Status channel mapping of            */
  /* Class 2 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24112 */

     /* RpmClass2StatSlot: Class 2 flow control status time slot     */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD rpm_class2_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class2_stat_slot_reg;


  /* Rpm Class3 Stat Slot: RPM Status channel mapping of            */
  /* Class 3 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24113 */

     /* RpmClass3StatSlot: Class 3 flow control status time slot     */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x2              */
     PCP_REG_FIELD rpm_class3_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class3_stat_slot_reg;


  /* Rpm Class4 Stat Slot: RPM Status channel mapping of            */
  /* Class 4 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24114 */

     /* RpmClass4StatSlot: Class 4 flow control status time slot     */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x3              */
     PCP_REG_FIELD rpm_class4_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class4_stat_slot_reg;


  /* Rpm Class5 Stat Slot: RPM Status channel mapping of            */
  /* Class 5 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24115 */

     /* RpmClass5StatSlot: Class 5 flow control status time slot     */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x4              */
     PCP_REG_FIELD rpm_class5_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class5_stat_slot_reg;


  /* Rpm Class6 Stat Slot: RPM Status channel mapping of            */
  /* Class 6 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24116 */

     /* RpmClass6StatSlot: Class 6 flow control status time slot     */
     /* position in the status channel towards Soc_petra.. Use 4'hF      */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x5              */
     PCP_REG_FIELD rpm_class6_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class6_stat_slot_reg;


  /* Rpm Class7 Stat Slot: RPM Status channel mapping of            */
  /* Class 7 to status frame time slot                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24117 */

     /* RpmClass7StatSlot: Class 7 flow control status time slot     */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude this class from flow control frame.               */
     /* range: 3:0, access type: RW, default value: 0x6              */
     PCP_REG_FIELD rpm_class7_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_class7_stat_slot_reg;


  /* Rpm Elk Stat Slot: RPM Status channel mapping of Elk           */
  /* engine to status frame time slot                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24118 */

     /* RpmElkStatSlot: Elk flow control status time slot            */
     /* position in the status channel towards Soc_petra. Use 4'hF       */
     /* to exclude ELK from flow control frame.                      */
     /* range: 3:0, access type: RW, default value: 0xf              */
     PCP_REG_FIELD rpm_elk_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_elk_stat_slot_reg;


  /* Rpm Oam Stat Slot: RPM Status channel mapping of Oam           */
  /* engine to status frame time slot                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24119 */

     /* RpmOamStatSlot: Oam flow control status time slot            */
     /* position in the status channel to wards Soc_petra. Use 4'hF      */
     /* to exclude OAM from flow control frame.                      */
     /* range: 3:0, access type: RW, default value: 0xf              */
     PCP_REG_FIELD rpm_oam_stat_slot;

  } __ATTRIBUTE_PACKED__ rpm_oam_stat_slot_reg;


  /* Rpm Fifo Err: RPM FIFO Overflow information                    */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2411a */

     /* RpmDmaFifoFull1: DMA Class 1 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full1;

     /* RpmDmaFifoFull2: DMA Class 2 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full2;

     /* RpmDmaFifoFull3: DMA Class 3 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full3;

     /* RpmDmaFifoFull4: DMA Class 4 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full4;

     /* RpmDmaFifoFull5: DMA Class 5 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 4:4, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full5;

     /* RpmDmaFifoFull6: DMA Class 6 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 5:5, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full6;

     /* RpmDmaFifoFull7: DMA Class 7 Incoming FIFO is Full. This     */
     /* register is clear on read.                                   */
     /* range: 6:6, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_fifo_full7;

     /* RpmDmaSizeFifoFull1: DMA Class 1 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 8:8, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_size_fifo_full1;

     /* RpmDmaSizeFifoFull2: DMA Class 2 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 9:9, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rpm_dma_size_fifo_full2;

     /* RpmDmaSizeFifoFull3: DMA Class 3 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 10:10, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_size_fifo_full3;

     /* RpmDmaSizeFifoFull4: DMA Class 4 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 11:11, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_size_fifo_full4;

     /* RpmDmaSizeFifoFull5: DMA Class 5 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 12:12, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_size_fifo_full5;

     /* RpmDmaSizeFifoFull6: DMA Class 6 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 13:13, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_size_fifo_full6;

     /* RpmDmaSizeFifoFull7: DMA Class 7 Incoming Burst Size         */
     /* FIFO is Full. This register is clear on read.                */
     /* range: 14:14, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_size_fifo_full7;

     /* RpmElkFifoFull: ELK Incoming FIFO is Full. This register     */
     /* is clear on read.                                            */
     /* range: 16:16, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_elk_fifo_full;

     /* RpmElkFifoHazard: ELK Incoming FIFO reached a dangerous      */
     /* level This register is clear on read.                        */
     /* range: 17:17, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_elk_fifo_hazard;

     /* RpmOamFifoFull: OAM Incoming FIFO is Full. This register     */
     /* is clear on read.                                            */
     /* range: 20:20, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_oam_fifo_full;

     /* RpmOamFifoHazard: OAM Incoming FIFO reached a dangerous      */
     /* level This register is clear on read.                        */
     /* range: 21:21, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_oam_fifo_hazard;

     /* RpmDmaFifoHazard1: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 24:24, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard1;

     /* RpmDmaFifoHazard2: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 25:25, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard2;

     /* RpmDmaFifoHazard3: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 26:26, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard3;

     /* RpmDmaFifoHazard4: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 27:27, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard4;

     /* RpmDmaFifoHazard5: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 28:28, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard5;

     /* RpmDmaFifoHazard6: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 29:29, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard6;

     /* RpmDmaFifoHazard7: DMA Class 1 Incoming FIFO reached a       */
     /* dangerous level. This register is clear on read.             */
     /* range: 30:30, access type: RO, default value: 0x0            */
     PCP_REG_FIELD rpm_dma_fifo_hazard7;

  } __ATTRIBUTE_PACKED__ rpm_fifo_err_reg;


  /* Dma Err: DMA Engine Error History                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2411b */

     /* IdmDescReplyErr: Descriptor reply transaction error. An      */
     /* errornous descriptor reply recieved from PCIe sets this      */
     /* bit to 1'b1. This is caused by illgal number of              */
     /* transaction cycles or an abort. This register is clear       */
     /* on read.                                                     */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD idm_desc_reply_err;

     /* EdmDescReplyErr: Descriptor reply transaction error. An      */
     /* errornous descriptor reply recieved from PCIe edit sets      */
     /* this bit to 1'b1. This is caused by illgal number of         */
     /* transaction cycles or an abort. This register is clear       */
     /* on read.                                                     */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD edm_desc_reply_err;

     /* IssAbortError: ISS Abort error. One of the transactions      */
     /* from the CPU to the ISS resulted in an abortted              */
     /* transaction. This register is clear on read.                 */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD iss_abort_error;

     /* EssError: ESS error. The ESS has passed to the EDM an        */
     /* error indication for a failing buffer transaction. This      */
     /* register is clear on read.                                   */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_error;

     /* MultiCmpltByteCountErr: Multi Completion Byte Count          */
     /* Error. The DMA Arbitter found a Multicompletion Byte         */
     /* Count Error and aborted the completion. This register is     */
     /* clear on read.                                               */
     /* range: 4:4, access type: RO, default value: 0x0              */
     PCP_REG_FIELD multi_cmplt_byte_count_err;

  } __ATTRIBUTE_PACKED__ dma_err_reg;


  /* Idm Rst: Ingress Descriptor Manager (IDM) reset and            */
  /* enable                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2411c */

     /* IdmRst: Reset IDM block. Set to 1'b1 to reset the block      */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD idm_rst;

     /* IdmEnable: IDM block enable. Set to 1'b1 to start the        */
     /* IDM operation. All block registers must be configured        */
     /* before setting this bit to 1'b1. Setting this bit to         */
     /* 1'b0 will immediately stop the block operation.              */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD idm_enable;

  } __ATTRIBUTE_PACKED__ idm_rst_reg;


  /* Idm Trig: Trigger IDM state machine to fetch next              */
  /* descriptor                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2411d */

     /* IdmTrigPetra: Trigger to fetch the next descriptor           */
     /* assigned to Soc_petra. Set to 1'b1 to trigger descriptor         */
     /* fetching. This bit is self clear, it is set to 1'b0 when     */
     /* buffer/packet/descriptor list are complete, according to     */
     /* IdmMode register, and can not be cleared by user. This       */
     /* register is clear on read.                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD idm_trig_petra;

  } __ATTRIBUTE_PACKED__ idm_trig_reg;


  /* Idm Poll Timeout: Polling timeout configuration. Used to       */
  /* configure the elapsed time between consecutive polling         */
  /* attempts.                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24120 */

     /* IdmPollTimeout: Polling timeout configuration. Used to       */
     /* configure the elapsed time between consecutive polling       */
     /* attempts.                                                    */
     /* range: 31:0, access type: RW, default value: 0x2000          */
     PCP_REG_FIELD idm_poll_timeout;

  } __ATTRIBUTE_PACKED__ idm_poll_timeout_reg;


  /* Idm Desc Head: Address of head of Soc_petra descriptors            */
  /* chain list in the CPU                                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24121 */

     /* IdmDescHead: Address of head of Soc_petra descriptors chain      */
     /* list in the CPU.                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD idm_desc_head;

  } __ATTRIBUTE_PACKED__ idm_desc_head_reg_0;


  /* Idm Desc Head: Address of head of Soc_petra descriptors            */
  /* chain list in the CPU                                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24122 */

     /* IdmDescHead: Address of head of Soc_petra descriptors chain      */
     /* list in the CPU.                                             */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD idm_desc_head;

  } __ATTRIBUTE_PACKED__ idm_desc_head_reg_1;


  /* Idm Desc Curr: Address of Current descriptor in the CPU        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24123 */

     /* IdmDescCurr: Address of Current descriptor in the CPU        */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD idm_desc_curr;

  } __ATTRIBUTE_PACKED__ idm_desc_curr_reg_0;


  /* Idm Desc Curr: Address of Current descriptor in the CPU        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24124 */

     /* IdmDescCurr: Address of Current descriptor in the CPU        */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD idm_desc_curr;

  } __ATTRIBUTE_PACKED__ idm_desc_curr_reg_1;


  /* Idm Last Desc Header: Last Descriptor Header                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24125 */

     /* IdmLastDescHeader: The last descriptor header received       */
     /* by the IDM. Returns bits 159:128 of the descriptor.          */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD idm_last_desc_header;

  } __ATTRIBUTE_PACKED__ idm_last_desc_header_reg;


  /* Idm Desc Counter: Descriptors Counter                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24126 */

     /* IdmDescCounter: IDM descriptors counter. Counts the          */
     /* number of descriptors received by the IDM since last IDM     */
     /* reset. Clear on read. This register is clear on read.        */
     /* range: 30:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD idm_desc_counter;

     /* IdmDescCounterOvf: IDM Descriptors counter overflow.         */
     /* This register is clear on read.                              */
     /* range: 31:31, access type: RO, default value: 0x0            */
     PCP_REG_FIELD idm_desc_counter_ovf;

  } __ATTRIBUTE_PACKED__ idm_desc_counter_reg;


  /* Iss Rst: Ingress Schedule and Send (ISS) reset and             */
  /* enable                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24127 */

     /* IssRst: Reset ISS block. Set to 1'b1 to reset the block      */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD iss_rst;

     /* IssEnable: ISS block enable. Set to 1'b1 to start the        */
     /* ISS operation. All block registers must be configured        */
     /* before setting this bit to 1'b1. Setting this bit to         */
     /* 1'b0 will immediately stop the block operation.              */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD iss_enable;

  } __ATTRIBUTE_PACKED__ iss_rst_reg;


  /* Iss Last Burst Head: First 32b of the last arriving            */
  /* burst from CPU                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2412a */

     /* IssLastBurstHead: First 32b of the last arriving burst       */
     /* from CPU                                                     */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD iss_last_burst_head;

  } __ATTRIBUTE_PACKED__ iss_last_burst_head_reg;


  /* Iss Burst Counter: Bursts Counter                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2412b */

     /* IssBurstCounter: ISS bursts counter. Counts the number       */
     /* of bursts received by the ISS since last ISS reset.          */
     /* Clear on read. This register is clear on read.               */
     /* range: 30:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD iss_burst_counter;

     /* IssBurstCounterOvf: ISS bursts counter overflow. This        */
     /* register is clear on read.                                   */
     /* range: 31:31, access type: RO, default value: 0x0            */
     PCP_REG_FIELD iss_burst_counter_ovf;

  } __ATTRIBUTE_PACKED__ iss_burst_counter_reg;


  /* Edm Rst: Ingress Descriptor Manager (EDM) reset and            */
  /* enable                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2412c */

     /* EdmRst: Reset EDM block. Set to 1'b1 to reset the block      */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD edm_rst;

     /* EdmEnable: EDM block enable. Set to 1'b1 to start the        */
     /* EDM operation. All block registers must be configured        */
     /* before setting this bit to 1'b1. Setting this bit to         */
     /* 1'b0 will immediately stop the block operation.              */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD edm_enable;

  } __ATTRIBUTE_PACKED__ edm_rst_reg;


  /* Edm Desc Head Class1 Msb: Address of head of Class1            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24130 */

     /* EdmDescHeadClass1Msb: Address of head of Class1              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class1_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class1_msb_reg;


  /* Edm Desc Head Class1 Lsb: Address of head of Class1            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24131 */

     /* EdmDescHeadClass1Lsb: Address of head of Class1              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class1_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class1_lsb_reg;


  /* Edm Desc Head Class2 Msb: Address of head of Class2            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24132 */

     /* EdmDescHeadClass2Msb: Address of head of Class2              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class2_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class2_msb_reg;


  /* Edm Desc Head Class2 Lsb: Address of head of Class2            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24133 */

     /* EdmDescHeadClass2Lsb: Address of head of Class2              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class2_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class2_lsb_reg;


  /* Edm Desc Head Class3 Msb: Address of head of Class3            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24134 */

     /* EdmDescHeadClass3Msb: Address of head of Class3              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class3_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class3_msb_reg;


  /* Edm Desc Head Class3 Lsb: Address of head of Class3            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24135 */

     /* EdmDescHeadClass3Lsb: Address of head of Class3              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class3_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class3_lsb_reg;


  /* Edm Desc Head Class4 Msb: Address of head of Class4            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24136 */

     /* EdmDescHeadClass4Msb: Address of head of Class4              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class4_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class4_msb_reg;


  /* Edm Desc Head Class4 Lsb: Address of head of Class4            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24137 */

     /* EdmDescHeadClass4Lsb: Address of head of Class4              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class4_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class4_lsb_reg;


  /* Edm Desc Head Class5 Msb: Address of head of Class5            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24138 */

     /* EdmDescHeadClass5Msb: Address of head of Class5              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class5_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class5_msb_reg;


  /* Edm Desc Head Class5 Lsb: Address of head of Class5            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24139 */

     /* EdmDescHeadClass5Lsb: Address of head of Class5              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class5_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class5_lsb_reg;


  /* Edm Desc Head Class6 Msb: Address of head of Class6            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2413a */

     /* EdmDescHeadClass6Msb: Address of head of Class6              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class6_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class6_msb_reg;


  /* Edm Desc Head Class6 Lsb: Address of head of Class6            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2413b */

     /* EdmDescHeadClass6Lsb: Address of head of Class6              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class6_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class6_lsb_reg;


  /* Edm Desc Head Class7 Msb: Address of head of Class7            */
  /* descriptors chain list in the CPU, most significant 32b.       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2413c */

     /* EdmDescHeadClass7Msb: Address of head of Class7              */
     /* descriptors chain list in the CPU, most significant 32b.     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class7_msb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class7_msb_reg;


  /* Edm Desc Head Class7 Lsb: Address of head of Class7            */
  /* descriptors chain list in the CPU, least significant           */
  /* 32b.                                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2413d */

     /* EdmDescHeadClass7Lsb: Address of head of Class7              */
     /* descriptors chain list in the CPU, least significant         */
     /* 32b.                                                         */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD edm_desc_head_class7_lsb;

  } __ATTRIBUTE_PACKED__ edm_desc_head_class7_lsb_reg;


  /* Edm Last Desc Header: Last Descriptor Header                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2414c */

     /* EdmLastDescHeader: The last descriptor header received       */
     /* by the IDM. Returns bits 159:128 of the descriptor.          */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD edm_last_desc_header;

  } __ATTRIBUTE_PACKED__ edm_last_desc_header_reg;


  /* Edm Desc Counter: Descriptors Counter                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2414d */

     /* EdmDescCounter: EDM descriptors counter. Counts the          */
     /* number of descriptors received by the EDM since last EDM     */
     /* reset. Clear on read. This register is clear on read.        */
     /* range: 30:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD edm_desc_counter;

     /* EdmDescCounterOvf: EDM Descriptors counter overflow.         */
     /* This register is clear on read.                              */
     /* range: 31:31, access type: RO, default value: 0x0            */
     PCP_REG_FIELD edm_desc_counter_ovf;

  } __ATTRIBUTE_PACKED__ edm_desc_counter_reg;


  /* Ess Rst: Egress Schedule and Send (ESS) reset and enable       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2414e */

     /* EssRst: Reset ESS block. Set to 1'b1 to reset the block      */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_rst;

     /* EssEnable: ESS block enable. Set to 1'b1 to start the        */
     /* ESS operation. All block registers must be configured        */
     /* before setting this bit to 1'b1. Setting this bit to         */
     /* 1'b0 will immediately stop the block operation.              */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD ess_enable;

  } __ATTRIBUTE_PACKED__ ess_rst_reg;


  /* Ess Buff Size: PCI Express Packet Buffer Size                  */
  /* configuration                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2414f */

     /* EssBufferSize: Buffer size configuration. The buffer         */
     /* size assigned in the CPU memory for packet payload. This     */
     /* value is used only if UseBufferSize field is set to          */
     /* 1'b1. Otherwise the buffer size will be taken from the       */
     /* length field of the descriptor.                              */
     /* range: 29:16, access type: RW, default value: 0x400          */
     PCP_REG_FIELD ess_buffer_size;

     /* EssUseBufferSize: Use BufferSize field. Set this bit to      */
     /* 1'b1 to use the buffer size configured in BufferSize         */
     /* field. Set to 1'b0 to use the length field of the            */
     /* descriptor. If the descriptor is used for buffer size        */
     /* information then the CPU must update the length field of     */
     /* every descriptor for the correct buffer size before          */
     /* giving ownership to the FPGA.                                */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD ess_use_buffer_size;

  } __ATTRIBUTE_PACKED__ ess_buff_size_reg;


  /* Ess Weight Class1: Class 1 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24150 */

     /* EssWeightClass1: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class1;

  } __ATTRIBUTE_PACKED__ ess_weight_class1_reg;


  /* Ess Weight Class2: Class 2 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24151 */

     /* EssWeightClass2: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class2;

  } __ATTRIBUTE_PACKED__ ess_weight_class2_reg;


  /* Ess Weight Class3: Class 3 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24152 */

     /* EssWeightClass3: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class3;

  } __ATTRIBUTE_PACKED__ ess_weight_class3_reg;


  /* Ess Weight Class4: Class 4 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24153 */

     /* EssWeightClass4: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class4;

  } __ATTRIBUTE_PACKED__ ess_weight_class4_reg;


  /* Ess Weight Class5: Class 5 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24154 */

     /* EssWeightClass5: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class5;

  } __ATTRIBUTE_PACKED__ ess_weight_class5_reg;


  /* Ess Weight Class6: Class 6 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24155 */

     /* EssWeightClass6: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class6;

  } __ATTRIBUTE_PACKED__ ess_weight_class6_reg;


  /* Ess Weight Class7: Class 7 Weight for WRR                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24156 */

     /* EssWeightClass7: The weight used for the class by WRR        */
     /* scheduler. The weight range from 1 to 254. The higher        */
     /* the setting the lower the priority. The highest priority     */
     /* weight would be 1 and the lowest pririty weight would be     */
     /* 254. Setting this field to 255 sets strict priority for      */
     /* this class. For strict priority setting , the lower          */
     /* class indecies are served first. A setting of 0 would        */
     /* disable this class.                                          */
     /* range: 7:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD ess_weight_class7;

  } __ATTRIBUTE_PACKED__ ess_weight_class7_reg;


  /* Ess Pkt Pending: Pending Packets Status                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x24157 */

     /* EssPetraClass1PktPending: Soc_petra 1 Class 1 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class1_pkt_pending;

     /* EssPetraClass2PktPending: Soc_petra 1 Class 2 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class2_pkt_pending;

     /* EssPetraClass3PktPending: Soc_petra 1 Class 3 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class3_pkt_pending;

     /* EssPetraClass4PktPending: Soc_petra 1 Class 4 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class4_pkt_pending;

     /* EssPetraClass5PktPending: Soc_petra 1 Class 5 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 4:4, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class5_pkt_pending;

     /* EssPetraClass6PktPending: Soc_petra 1 Class 6 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 5:5, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class6_pkt_pending;

     /* EssPetraClass7PktPending: Soc_petra 1 Class 7 packet pending     */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* of this class are pending in the RPM.                        */
     /* range: 6:6, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_class7_pkt_pending;

     /* EssPetraOAMPPktPending: Soc_petra 1 OAMP packet pending          */
     /* indication. This bit is set to 1'b1 as long as packets       */
     /* intended to OAMP are pending in the RPM.                     */
     /* range: 7:7, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ess_petra_oamppkt_pending;

  } __ATTRIBUTE_PACKED__ ess_pkt_pending_reg;


  /* Ess Class Sleep Timer: ESS Scheduler Class Sleep Timer         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2415a */

     /* EssClassSleepPeriod: Ess Scheduler Class Sleep Timer         */
     /* Period. This timer will prevent a class from being           */
     /* selected by the scheduler if there is no valid buffer        */
     /* address available for it. The default is the lowest          */
     /* recommended value.                                           */
     /* range: 31:0, access type: RW, default value: 0x40            */
     PCP_REG_FIELD ess_class_sleep_period;

  } __ATTRIBUTE_PACKED__ ess_class_sleep_timer_reg;

} __ATTRIBUTE_PACKED__ PCP_CSI_REGS;
/* Block definition: ELK 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  PCP_REG_ADDR addr;

  /* Interrupt Register: This register contains the interrupt       */
  /* sources residing in this unit.                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28000 */

     /* ErrorDeleteUnknownKey: If set, a delete of a                 */
     /* non-existing entry was attempted.                            */
     /* range: 0:0, access type: RC, default value: 0x0              */
     PCP_REG_FIELD error_delete_unknown_key;

     /* ErrorReachedEntryLimit: If set, an insertion failed as a     */
     /* result of number of entries limit .                          */
     /* range: 1:1, access type: RC, default value: 0x0              */
     PCP_REG_FIELD error_reached_entry_limit;

     PCP_REG_FIELD error_cam_table_full;

     /* ErrorInsertedExisting: If set, then an existing entry        */
     /* was inserted. (Same Key)                                     */
     /* range: 3:3, access type: RC, default value: 0x0              */
     PCP_REG_FIELD error_inserted_existing;

     PCP_REG_FIELD error_learn_over_static;
     PCP_REG_FIELD error_refresh_over_static;
     PCP_REG_FIELD mact_reached_fid_limit;
     PCP_REG_FIELD mact_reached_fid_limit_static_allowed;

     /* DroppedLrfCommand: If set, the an internal learn request     */
     /* was dropped.                                                 */
     /* range: 8:8, access type: RC, default value: 0x0              */
     PCP_REG_FIELD dropped_lrf_command;

     /* EventReady: asserts when an event is ready for read          */
     /* range: 9:9, access type: RC, default value: 0x0              */
     PCP_REG_FIELD event_ready;

     /* EventFifoEventDrop: If set, an event to CPU was dropped      */
     /* range: 10:10, access type: RC, default value: 0x0            */
     PCP_REG_FIELD event_fifo_event_drop;

     /* EventFifoHighThresholdReached: If set, Event FIFO            */
     /* reached high threshold                                       */
     /* range: 11:11, access type: RC, default value: 0x0            */
     PCP_REG_FIELD event_fifo_high_threshold_reached;

     /* ReplyFifoReady: asserts when an event is ready for read      */
     /* range: 12:12, access type: RC, default value: 0x0            */
     PCP_REG_FIELD reply_fifo_ready;

     /* ReplyFifoReplyDrop: If set, a reply to a CPU request was     */
     /* dropped                                                      */
     /* range: 13:13, access type: RC, default value: 0x0            */
     PCP_REG_FIELD reply_fifo_reply_drop;

     /* DiagnosticCommandCompleted: If set, diagnostics command      */
     /* had completed                                                */
     /* range: 16:16, access type: RC, default value: 0x0            */
     PCP_REG_FIELD diagnostic_command_completed;

     /* UnexpSopErr: If set, indicates that an unexpected SOP        */
     /* (start of packet) has occurred on the Dune Special           */
     /* Packet Reception unit (DSPR).                                */
     /* range: 20:20, access type: RC, default value: 0x0            */
     PCP_REG_FIELD unexp_sop_err;

     /* UnexpEopErr: If set, indicates that an unexpected EOP        */
     /* (end of packet) has occurred on the Dune Special Packet      */
     /* Reception unit (DSPR).                                       */
     /* range: 21:21, access type: RC, default value: 0x0            */
     PCP_REG_FIELD unexp_eop_err;

     /* UnexpValidBytesErr: If set, indicates that an unexpected     */
     /* valid bytes has occurred on the Dune Special Packet          */
     /* Reception unit (DSPR).                                       */
     /* range: 22:22, access type: RC, default value: 0x0            */
     PCP_REG_FIELD unexp_valid_bytes_err;

     /* DspEthTypeErr:                                               */
     /* range: 23:23, access type: RC, default value: 0x0            */
     PCP_REG_FIELD dsp_eth_type_err;

  } __ATTRIBUTE_PACKED__ interrupt_reg;


  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28010 */

     /* ErrorDeleteUnknownKeyMask:                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD error_delete_unknown_key_mask;

     /* ErrorReachedEntryLimitMask:                                  */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD error_reached_entry_limit_mask;

     /* ErrorInsertedExistingMask:                                   */
     /* range: 3:3, access type: RW, default value: 0x0              */
     PCP_REG_FIELD error_inserted_existing_mask;

     /* DroppedLrfCommandMask:                                       */
     /* range: 8:8, access type: RW, default value: 0x0              */
     PCP_REG_FIELD dropped_lrf_command_mask;

     /* EventReadyMask:                                              */
     /* range: 9:9, access type: RW, default value: 0x0              */
     PCP_REG_FIELD event_ready_mask;

     /* EventFifoEventDropMask:                                      */
     /* range: 10:10, access type: RW, default value: 0x0            */
     PCP_REG_FIELD event_fifo_event_drop_mask;

     /* EventFifoHighThresholdReachedMask:                           */
     /* range: 11:11, access type: RW, default value: 0x0            */
     PCP_REG_FIELD event_fifo_high_threshold_reached_mask;

     /* ReplyFifoReplyDropMask:                                      */
     /* range: 12:12, access type: RW, default value: 0x0            */
     PCP_REG_FIELD reply_fifo_reply_drop_mask;

     /* DiagnosticCommandCompletedMask:                              */
     /* range: 15:15, access type: RW, default value: 0x0            */
     PCP_REG_FIELD diagnostic_command_completed_mask;

     /* UnexpSopErrMask: Writing 0 masks the corresponding           */
     /* interrupt source                                             */
     /* range: 19:19, access type: RW, default value: 0x0            */
     PCP_REG_FIELD unexp_sop_err_mask;

     /* UnexpEopErrMask: Writing 0 masks the corresponding           */
     /* interrupt source                                             */
     /* range: 20:20, access type: RW, default value: 0x0            */
     PCP_REG_FIELD unexp_eop_err_mask;

     /* UnexpValidBytesErrMask: Writing 0 masks the                  */
     /* corresponding interrupt source                               */
     /* range: 21:21, access type: RW, default value: 0x0            */
     PCP_REG_FIELD unexp_valid_bytes_err_mask;

     /* DspEthTypeErrMask:                                           */
     /* range: 22:22, access type: RW, default value: 0x0            */
     PCP_REG_FIELD dsp_eth_type_err_mask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28020 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_0;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28021 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_1;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28022 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_2;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28023 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_3;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28024 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_4;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28025 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_5;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28026 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 3:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_6;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28030 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_0;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28031 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_1;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28032 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_2;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28033 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_3;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28034 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_4;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28035 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_5;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28036 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 3:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_6;


  /* Indirect Command: Indirect Commands. Used to send              */
  /* indirect commands and receive their status                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28040 */

     /* IndirectCommandTrigger: Triggers indirect access as          */
     /* defined by the rest of the registers. The negation of        */
     /* this bit indicates that the operation has been               */
     /* completed.                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger;

     /* IndirectCommandTriggerOnData: If set, then writing to        */
     /* the LSBs of IndirectCommandWrData automatically set          */
     /* IndirectCommandTrigger.                                      */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger_on_data;

     /* IndirectCommandCount: Each write command is executed         */
     /* IndirectCommandCount number of times. The address is         */
     /* advanced by one every write command. If set to 0, one        */
     /* operation is performed.The CPU can read this field to        */
     /* find the number of writes left.                              */
     /* range: 15:2, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_count;

     /* IndirectCommandTimeout: Configures a timeout period for      */
     /* the indirect access command. Resolution is in 256            */
     /* clocks. If set to 0, then the command has no timeout.        */
     /* range: 30:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_timeout;

     /* IndirectCommandStatus: Status of last indirect access        */
     /* command. If set, then last command failed on timeout.        */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_status;

  } __ATTRIBUTE_PACKED__ indirect_command_reg;


  /* Indirect Command Address: Indirect access address.             */
  /* Defines which indirect object is accessed                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28041 */

     /* IndirectCommandAddr: Indirect access address. Defines        */
     /* which indirect object is accessed                            */
     /* range: 30:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_addr;

     /* IndirectCommandType: Type of command to perform: 0 -         */
     /* Write operation 1 - Read operation                           */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_type;

  } __ATTRIBUTE_PACKED__ indirect_command_address_reg;


  /* Gtimer Configuration:                                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28042 */

     /* GtimerCycle: The number of clocks the gtimer will count.     */
     /* range: 29:0, access type: RW, default value: 30'd250_000_000 */
     PCP_REG_FIELD gtimer_cycle;

     /* GtimerEnable: Whether to use all the counters in gtimer      */
     /* mode or read on clear                                        */
     /* range: 30:30, access type: RW, default value: 0x0            */
     PCP_REG_FIELD gtimer_enable;

     /* GtimerResetOnTrigger: If set, the gtimer will output a       */
     /* reset signal when the count begins.                          */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD gtimer_reset_on_trigger;

  } __ATTRIBUTE_PACKED__ gtimer_configuration_reg;


  /* Gtimer Trigger:                                                */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28043 */

     /* GtimerTrigger: When this register is written to, the         */
     /* gtimer starts counting. When this register is read, it       */
     /* gives the gtimer status (1= still counting, 0=count is       */
     /* over).                                                       */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD gtimer_trigger;

  } __ATTRIBUTE_PACKED__ gtimer_trigger_reg;


  /* Key Table Entry Limit: Limit on maximum number of              */
  /* entries in KEY table                                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28045 */

     /* KeyTableEntryLimit: Limit on maximum number of entries       */
     /* in KEY table. Allowed values - 0 to 1M (msb is reserved)     */
     /* range: 21:0, access type: RW, default value: 0x7fff          */
     PCP_REG_FIELD key_table_entry_limit;

  } __ATTRIBUTE_PACKED__ key_table_entry_limit_reg;


  /* Age Aging Resolution: Aging resolution                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28046 */

     /* AgeAgingResolution: 0: three values resolution, 1 six        */
     /* values resolution                                            */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD age_aging_resolution;

  } __ATTRIBUTE_PACKED__ age_aging_resolution_reg;


  /* Management Unit Failure: Describes the first management        */
  /* unit failure (since last time this register was read).         */
  /* The register is locked after a failure. It is unlocked         */
  /* when the first part of the register is read.                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28047 */

     /* MngmntUnitFailure: 65:0 - holds the Key that operation       */
     /* failed on. 66 - If set, then the there is a management       */
     /* unit failure. 69:67 - All other fields in this register      */
     /* are valid only when this bit is set. Specifies reason        */
     /* for EMC management failure: 7 - delete unknown key 6 -       */
     /* reached max entry limit 5 - cam table full 4 - inserted      */
     /* existing 3 - learn over static 2 - refresh over static 1     */
     /* - reserved 0 - reserved This register is clear on read.      */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD mngmnt_unit_failure;

  } __ATTRIBUTE_PACKED__ management_unit_failure_reg_0;


  /* Management Unit Failure: Describes the first management        */
  /* unit failure (since last time this register was read).         */
  /* The register is locked after a failure. It is unlocked         */
  /* when the first part of the register is read.                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28048 */

     /* MngmntUnitFailure: 65:0 - holds the Key that operation       */
     /* failed on. 66 - If set, then the there is a management       */
     /* unit failure. 69:67 - All other fields in this register      */
     /* are valid only when this bit is set. Specifies reason        */
     /* for EMC management failure: 7 - delete unknown key 6 -       */
     /* reached max entry limit 5 - cam table full 4 - inserted      */
     /* existing 3 - learn over static 2 - refresh over static 1     */
     /* - reserved 0 - reserved This register is clear on read.      */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD mngmnt_unit_failure;

  } __ATTRIBUTE_PACKED__ management_unit_failure_reg_1;


  /* Management Unit Failure: Describes the first management        */
  /* unit failure (since last time this register was read).         */
  /* The register is locked after a failure. It is unlocked         */
  /* when the first part of the register is read.                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28049 */

     /* MngmntUnitFailure: 65:0 - holds the Key that operation       */
     /* failed on. 66 - If set, then the there is a management       */
     /* unit failure. 69:67 - All other fields in this register      */
     /* are valid only when this bit is set. Specifies reason        */
     /* for EMC management failure: 7 - delete unknown key 6 -       */
     /* reached max entry limit 5 - cam table full 4 - inserted      */
     /* existing 3 - learn over static 2 - refresh over static 1     */
     /* - reserved 0 - reserved This register is clear on read.      */
     /* range: 10:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD mngmnt_unit_failure;

  } __ATTRIBUTE_PACKED__ management_unit_failure_reg_2;


  /* Diagnostics: Diagnostics register                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2804a */

     /* DiagnosticsLookup: When writing 1'b1 to this bit a           */
     /* lookup operation starts. When reading this bit as 1'b1 a     */
     /* lookup operation is ongoing.                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD diagnostics_lookup;

     /* DiagnosticsRead: When writing 1'b1 to this bit a read        */
     /* operation starts. When reading this bit as 1'b1 a read       */
     /* operation is ongoing.                                        */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD diagnostics_read;

     /* DiagnosticsReadAge: When writing 1'b1 to this bit a          */
     /* read&age operation starts. When reading this bit as 1'b1     */
     /* a read&age operation is ongoing.                             */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD diagnostics_read_age;

  } __ATTRIBUTE_PACKED__ diagnostics_reg;


  /* Diagnostics Index:                                             */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2804b */

     /* DiagnosticsIndex: Address to read from.                      */
     /* range: 21:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD diagnostics_index;

  } __ATTRIBUTE_PACKED__ diagnostics_index_reg;


  /* Diagnostics Key:                                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2804c */

     /* DiagnosticsKey: Key for lookup operation                     */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD diagnostics_key;

  } __ATTRIBUTE_PACKED__ diagnostics_key_reg[PCP_ELK_DIAGNOSTICS_KEY_REG_MULT_NOF_REGS];


  /* Diagnostics Key:                                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2804d */

     /* DiagnosticsKey: Key for lookup operation                     */
     /* range: 30:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD diagnostics_key;

  } __ATTRIBUTE_PACKED__ diagnostics_key_reg_1;


  /* Diagnostics Lookup Result: The result of the diagnostics       */
  /* lookup                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2804e */

     /* EntryFound: If set, then the entry looked up in the          */
     /* diagnostics command was found.                               */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD entry_found;

     /* EntryPayload: The payload of the entry looked up in the      */
     /* diagnostics command. The payload is defined as               */
     /* \{ASD(24), IsDynamic(1), destination(16)\}.                  */
     /* range: 31:1, access type: RO, default value: 0x0             */
     PCP_REG_FIELD entry_payload;

  } __ATTRIBUTE_PACKED__ diagnostics_lookup_result_reg_0;


  /* Diagnostics Lookup Result: The result of the diagnostics       */
  /* lookup                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2804f */

     /* EntryPayload: The payload of the entry looked up in the      */
     /* diagnostics command. The payload is defined as               */
     /* \{ASD(24), IsDynamic(1), destination(16)\}.                  */
     /* range: 9:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD entry_payload;

     /* EntryAgeStat: The Age status of the entry looked up in       */
     /* the diagnostics command.                                     */
     /* range: 13:10, access type: RO, default value: 0x0            */
     PCP_REG_FIELD entry_age_stat;

  } __ATTRIBUTE_PACKED__ diagnostics_lookup_result_reg_1;


  /* Diagnostics Read Result: The result of the diagnostics         */
  /* lookup                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28050 */

     /* EntryValid: if set, then the index that was read has a       */
     /* valid entry                                                  */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD entry_valid;

     /* EntryKey: The entry's Key (when reading)                     */
     /* range: 31:1, access type: RO, default value: 0x0             */
     PCP_REG_FIELD entry_key;

  } __ATTRIBUTE_PACKED__ diagnostics_read_result_reg_0;


  /* Diagnostics Read Result: The result of the diagnostics         */
  /* lookup                                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28051 */

     /* EntryKey: The entry's Key (when reading)                     */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD entry_key;

  } __ATTRIBUTE_PACKED__ diagnostics_read_result_reg_1;

  /* EMC Defrag Configuration Register                              */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28052 */

    PCP_REG_FIELD defrag_enable;
    PCP_REG_FIELD defrag_mode;
    PCP_REG_FIELD defrag_cam_threshold;
    PCP_REG_FIELD defrag_period;

  } __ATTRIBUTE_PACKED__ emc_defrag_configuration_register_reg;

  /* Entry Counter:                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28053 */

     /* EntryCounter: Number of entries in the EM-DB                 */
     /* range: 21:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD entry_counter;

  } __ATTRIBUTE_PACKED__ entry_counter_reg;


  /* Flu Machine Hit Counter:                                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28055 */

     /* FluMachineHitCounter: Number of entries touched              */
     /* (delete/transplant) or sent to CPU by the Flush machine.     */
     /* range: 16:0, access type: CNT, default value: 0x0            */
     PCP_REG_FIELD flu_machine_hit_counter;

  } __ATTRIBUTE_PACKED__ flu_machine_hit_counter_reg;


  /* Mact Managment Request Configuration: General properties       */
  /* of the MRQ (Management FIFO)                                   */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28056 */

     /* AllowStatic_2Exceed: if set, command insert/learn with       */
     /* static payload that fail will not fail on FID limits         */
     /* range: 8:8, access type: RW, default value: 0x0              */
     PCP_REG_FIELD allow_static_2_exceed;

     /* AcknowledgeOnlyFailure: if set, acknowledge messages         */
     /* will be sent to the CPU only on failed commands with         */
     /* valid stamp (not equal zero).                                */
     /* range: 12:12, access type: RW, default value: 0x0            */
     PCP_REG_FIELD acknowledge_only_failure;

     /* CheckFidLimit: Enables the VSI limit checker.                */
     /* range: 13:13, access type: RW, default value: 0x0            */
     PCP_REG_FIELD check_fid_limit;

     /* AcknowledgeFidCheckFailureAlways: If set and the VSI         */
     /* limit fails, a message to the CPU will be sent (for both     */
     /* internally and externally requests).                         */
     /* range: 14:14, access type: RW, default value: 0x0            */
     PCP_REG_FIELD acknowledge_fid_check_failure_always;

  } __ATTRIBUTE_PACKED__ mact_managment_request_configuration_reg;


  /* Mact Managment Request Configuration: General properties       */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28057 */

    PCP_REG_FIELD learn_filter_drop_duplicate;
    PCP_REG_FIELD learn_filter_entry_ttl;
    PCP_REG_FIELD learn_filter_res;
    PCP_REG_FIELD learn_filter_watermark;
    PCP_REG_FIELD learn_filter_drop_duplicate_counter;

  } __ATTRIBUTE_PACKED__ learn_filter_properties_reg;

  /* Lrn Lkp Gen Conf: Lookup modeleran lookup general configuration       */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28058 */

    PCP_REG_FIELD learn_lookup_elk_bits_63to58;
    PCP_REG_FIELD learn_lookup_elk_bits_127to122;
  } __ATTRIBUTE_PACKED__ lrn_lkp_gen_conf_reg;


  /* Lookup Lookup Mode: Learn, transplant and refresh              */
  /* control of SA lookup.                                          */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28059 */

     /* LookupAllowedEventsDynamic: 0: reserved 1: allow refresh     */
     /* 2: reserved 3: allow transplant/refresh                      */
     /* range: 1:0, access type: RW, default value: 0x3              */
     PCP_REG_FIELD lookup_allowed_events_dynamic[PCP_ELK_LOOKUP_NOF_MODES];

     /* LookupAllowedEventsStatic: 0: reserved 1: allow refresh      */
     /* 2: reserved 3: allow transplant/refresh                      */
     /* range: 3:2, access type: RW, default value: 0x3              */
     PCP_REG_FIELD lookup_allowed_events_static[PCP_ELK_LOOKUP_NOF_MODES];

     /* LookupAllowedEventsLearn: if set, learn event can be         */
     /* created by the learn-lookup                                  */
     /* range: 4:4, access type: RW, default value: 0x1              */
     PCP_REG_FIELD lookup_allowed_events_learn[PCP_ELK_LOOKUP_NOF_MODES];

  } __ATTRIBUTE_PACKED__ lookup_lookup_mode_reg;


  /* Cpu Request: Request for MRQ from CPU.                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2805a */

     /* MactReqKeyMac:                                               */
     /* range: 31:2, access type: UNDEF, default value: 0x0          */
     PCP_REG_FIELD mact_req_key_mac;

  } __ATTRIBUTE_PACKED__ cpu_request_reg_0;


  /* Cpu Request: Request for MRQ from CPU.                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2805b */

     /* MactReqKeyMac:                                               */
     /* range: 17:0, access type: UNDEF, default value: 0x0          */
     PCP_REG_FIELD mact_req_key_mac;

     /* MactReqKeyVsi:                                               */
     /* range: 31:18, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_key_vsi;

  } __ATTRIBUTE_PACKED__ cpu_request_reg_1;


  /* Cpu Request: Request for MRQ from CPU.                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2805c */

     /* MactReqKeyDb:                                                */
     /* range: 0:0, access type: UNDEF, default value: 0x0           */
     PCP_REG_FIELD mact_req_key_db;

     /* MactReqPartOfLag: Indicates that this Event was              */
     /* generated from a packet whose Destination port is a LAG.     */
     /* Used only for indication to the CPU.                         */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD mact_req_part_of_lag;

     /* MactReqCommand: The command type: (000) Delete (001)         */
     /* Insert (010) Refresh (011) Learn (101) ACK (110) Move        */
     /* (transplant) (111) reserved                                  */
     /* range: 4:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD mact_req_command;

     /* MactReqStamp: If different than zero then an ACK event       */
     /* will be generated upon completion                            */
     /* range: 12:5, access type: RW, default value: 0x0             */
     PCP_REG_FIELD mact_req_stamp;

     /* MactReqPayloadDestination:                                   */
     /* range: 28:13, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_payload_destination;

     /* MactReqPayloadEEI:                                           */
     /* range: 31:29, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_payload_eei;

  } __ATTRIBUTE_PACKED__ cpu_request_reg_2;


  /* Cpu Request: Request for MRQ from CPU.                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2805d */

     /* MactReqPayloadEEI:                                           */
     /* range: 20:0, access type: UNDEF, default value: 0x0          */
     PCP_REG_FIELD mact_req_payload_eei;

     /* MactReqPayloadIsDynamic:                                     */
     /* range: 21:21, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_payload_is_dynamic;

     /* MactReqAgePayload: Age status. Valid values are 3'h0 -       */
     /* 3'h6                                                         */
     /* range: 24:22, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_age_payload;

     /* MactReqSuccess: 0 = FAIL, 1 = SUCCESS                        */
     /* range: 25:25, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_success;

     /* MactReqReason: Reason for failure: (0) Reserved (1)          */
     /* Failed VSI limit check (2) Reserved (3) Management core      */
     /* failure                                                      */
     /* range: 27:26, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_reason;

     /* MactReqSelf:                                                 */
     /* range: 30:30, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_self;

     /* MactReqQualifier                                             */
     PCP_REG_FIELD mact_req_qualifier;
  } __ATTRIBUTE_PACKED__ cpu_request_reg_3;


  /* Cpu Request Trigger: CPU request triggering and status         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2805e */

     /* CpuRequestTrigger: When writing 1'b1 to this bit the         */
     /* CpuRequestRequest data will attemp to enter RequestFIFO.     */
     /* When reading this bit as 1'b1 a request is pending the       */
     /* RequestFIFO. When reading this bit as 1'b0 the previous      */
     /* request has successfully entered the RequestFIFO.            */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD cpu_request_trigger;

  } __ATTRIBUTE_PACKED__ cpu_request_trigger_reg;


  /* Age Machine Configuration: Aging configurations                */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2805f */

     /* AgeAgingEnable: if set, the aging is on (age-meta-cycle      */
     /* counts back)                                                 */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD age_aging_enable;

     /* AgeMachinePause: if set, the aging machine stops, aging      */
     /* will continue once the bit is cleared                        */
     /* range: 4:4, access type: RW, default value: 0x0              */
     PCP_REG_FIELD age_machine_pause;

     /* AgeMachineAccessShaper: The time (in clocks) between         */
     /* accesses of the age machine to the MACT.                     */
     /* range: 15:8, access type: RW, default value: 0x0             */
     PCP_REG_FIELD age_machine_access_shaper;

     /* AgeStamp: The stamp on the event created by the aging        */
     /* machine. This is used by the CPU in order to separate        */
     /* the age machine messages from other messages.                */
     /* range: 27:20, access type: RW, default value: 0xf0           */
     PCP_REG_FIELD age_stamp;

     /* AgeMachineDeleteAgedOut: If set, the aging machine           */
     /* deletes entries which are aged out.                          */
     /* range: 28:28, access type: RW, default value: 0x1            */
     PCP_REG_FIELD age_machine_delete_aged_out;

     /* AgeMachineNotifyAgedOut: If set, the aging machine will      */
     /* send a message to the CPU when an entry is aged out.         */
     /* range: 29:29, access type: RW, default value: 0x0            */
     PCP_REG_FIELD age_machine_notify_aged_out;

     /* AgeMachineDeleteAgedOutStatic: If set, the aging machine     */
     /* deletes entries which are aged out (for static entries).     */
     /* range: 30:30, access type: RW, default value: 0x0            */
     PCP_REG_FIELD age_machine_delete_aged_out_static;

     /* AgeMachineNotifyAgedOutStatic: If set, the aging machine     */
     /* will send a message to the CPU when an entry is aged out     */
     /* (for static entries).                                        */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD age_machine_notify_aged_out_static;

  } __ATTRIBUTE_PACKED__ age_machine_configuration_reg;


  /* Age Aging Mode: Aging configurations                           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28060 */

     /* AgingCfgPtr: msb of aginf configuration table address        */
     /* range: 1:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD aging_cfg_ptr[PCP_ELK_AGE_AGING_NOF_MODES];

     /* OwnSystemLagPort: if 1, lag ports are owned                  */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD own_system_lag_port[PCP_ELK_AGE_AGING_NOF_MODES];

     /* OwnSystemPhysPort: if 1, physical ports are owned            */
     /* range: 3:3, access type: RW, default value: 0x0              */
     PCP_REG_FIELD own_system_phys_port[PCP_ELK_AGE_AGING_NOF_MODES];

  } __ATTRIBUTE_PACKED__ age_aging_mode_reg;


  /* Age Machine Current Index:                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28061 */

     /* AgeMachineCurrentIndex: Writing, Non-zero value starts       */
     /* the age machine, the age machine scans the MACT from         */
     /* this value (counting down to zero). Reading, Returns the     */
     /* value of the current index the age machine is on.            */
     /* range: 21:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD age_machine_current_index;

  } __ATTRIBUTE_PACKED__ age_machine_current_index_reg;


  /* Age Machine Status:                                            */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28062 */

     /* AgeMachineActive: if set, the aging machine is not at        */
     /* idle state                                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD age_machine_active;

  } __ATTRIBUTE_PACKED__ age_machine_status_reg;


  /* Age Machine Meta Cycle:                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28063 */

     /* AgeMachineMetaCycle: The time (in 16clks resolution) of      */
     /* the aging meta cycle                                         */
     /* range: 31:0, access type: RW, default value: 0x100           */
     PCP_REG_FIELD age_machine_meta_cycle;

  } __ATTRIBUTE_PACKED__ age_machine_meta_cycle_reg;


  /* Age Machine Start Index:                                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28064 */

     /* AgeMachineStartIndex: The index to set when                  */
     /* aging-meta-cycle TO. should be set to 2M+32 (and not to      */
     /* 2M+32-1)                                                     */
     /* range: 21:0, access type: RW, default value: 22'h200020      */
     PCP_REG_FIELD age_machine_start_index;

  } __ATTRIBUTE_PACKED__ age_machine_start_index_reg;


  /* Age Machine End Index:                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28065 */

     /* AgeMachineEndIndex: The index at which the aging machine     */
     /* stops                                                        */
     /* range: 21:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD age_machine_end_index;

  } __ATTRIBUTE_PACKED__ age_machine_end_index_reg;


  /* Flu Machine Configuration: Flushing configurations             */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28067 */

     /* FluMachinePause: if set, the flush machine stops,            */
     /* flushing will continue once the bit is cleared               */
     /* range: 4:4, access type: RW, default value: 0x0              */
     PCP_REG_FIELD flu_machine_pause;

     /* FluMachineAccessShaper: The time (in clocks) between         */
     /* accesses of the flush machine to the MACT.                   */
     /* range: 15:8, access type: RW, default value: 0xf             */
     PCP_REG_FIELD flu_machine_access_shaper;

     /* FluStamp: The stamp on the event created by the flush        */
     /* machine. This is used by the CPU in order to separate        */
     /* the flush machine messages from other messages.              */
     /* range: 27:20, access type: RW, default value: 0xff           */
     PCP_REG_FIELD flu_stamp;

  } __ATTRIBUTE_PACKED__ flu_machine_configuration_reg;


  /* Flu Machine Current Index:                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28068 */

     /* FluMachineCurrentIndex: Writing, Non-zero value starts       */
     /* the flush machine, the flush machine scans the MACT from     */
     /* this value (counting down to zero). Reading, Returns the     */
     /* value of the current index the flush machine is on.          */
     /* range: 21:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD flu_machine_current_index;

  } __ATTRIBUTE_PACKED__ flu_machine_current_index_reg;


  /* Flu Machine Status:                                            */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28069 */

     /* FluMachineActive: if set, the flush machine is not at        */
     /* idle state                                                   */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD flu_machine_active;

  } __ATTRIBUTE_PACKED__ flu_machine_status_reg;


  /* Flu Machine End Index:                                         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2806a */

     /* FluMachineEndIndex: The index at which the flushing          */
     /* machine stops. FluMachineCurrentIndex should be larger       */
     /* then FluMachineEndIndex.                                     */
     /* range: 21:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD flu_machine_end_index;

  } __ATTRIBUTE_PACKED__ flu_machine_end_index_reg;


  /* Evf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2806b */

     /* EvfEvent: EVF MACT Event - MACT Request/Reply/Event          */
     /* format (see CpuRequestRequest for details) This register     */
     /* is clear on read.                                            */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD evf_event;

  } __ATTRIBUTE_PACKED__ evf_event_reg_0;


  /* Evf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2806c */

     /* EvfEvent: EVF MACT Event - MACT Request/Reply/Event          */
     /* format (see CpuRequestRequest for details) This register     */
     /* is clear on read.                                            */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD evf_event;

  } __ATTRIBUTE_PACKED__ evf_event_reg_1;


  /* Evf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2806d */

     /* EvfEvent: EVF MACT Event - MACT Request/Reply/Event          */
     /* format (see CpuRequestRequest for details) This register     */
     /* is clear on read.                                            */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD evf_event;

  } __ATTRIBUTE_PACKED__ evf_event_reg_2;


  /* Evf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2806e */

     /* EvfEvent: EVF MACT Event - MACT Request/Reply/Event          */
     /* format (see CpuRequestRequest for details) This register     */
     /* is clear on read.                                            */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD evf_event;

  } __ATTRIBUTE_PACKED__ evf_event_reg_3;

  /* Crf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2806f */

     /* MactReqKeyMac:                                               */
     /* range: 31:2, access type: UNDEF, default value: 0x0          */
     PCP_REG_FIELD mact_req_key_mac;

  } __ATTRIBUTE_PACKED__ crf_event_reg_0;


  /* Crf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28070 */

     /* MactReqKeyMac:                                               */
     /* range: 17:0, access type: UNDEF, default value: 0x0          */
     PCP_REG_FIELD mact_req_key_mac;

     /* MactReqKeyVsi:                                               */
     /* range: 31:18, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_key_vsi;

  } __ATTRIBUTE_PACKED__ crf_event_reg_1;


  /* Crf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28071 */

     /* MactReqKeyDb:                                                */
     /* range: 0:0, access type: UNDEF, default value: 0x0           */
     PCP_REG_FIELD mact_req_key_db;

     /* MactReqPartOfLag: Indicates that this Event was              */
     /* generated from a packet whose Destination port is a LAG.     */
     /* Used only for indication to the CPU.                         */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD mact_req_part_of_lag;

     /* MactReqCommand: The command type: (000) Delete (001)         */
     /* Insert (010) Refresh (011) Learn (101) ACK (110) Move        */
     /* (transplant) (111) reserved                                  */
     /* range: 4:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD mact_req_command;

     /* MactReqStamp: If different than zero then an ACK event       */
     /* will be generated upon completion                            */
     /* range: 12:5, access type: RW, default value: 0x0             */
     PCP_REG_FIELD mact_req_stamp;

     /* MactReqPayloadDestination:                                   */
     /* range: 28:13, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_payload_destination;

     /* MactReqPayloadEEI:                                           */
     /* range: 31:29, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_payload_eei;

  } __ATTRIBUTE_PACKED__ crf_event_reg_2;


  /* Crf Event:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28072 */

     /* MactReqPayloadEEI:                                           */
     /* range: 20:0, access type: UNDEF, default value: 0x0          */
     PCP_REG_FIELD mact_req_payload_eei;

     /* MactReqPayloadIsDynamic:                                     */
     /* range: 21:21, access type: UNDEF, default value: 0x0         */
     PCP_REG_FIELD mact_req_payload_is_dynamic;

     /* MactReqAgePayload: Age status. Valid values are 3'h0 -       */
     /* 3'h6                                                         */
     /* range: 24:22, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_age_payload;

     /* MactReqSuccess: 0 = FAIL, 1 = SUCCESS                        */
     /* range: 25:25, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_success;

     /* MactReqReason: Reason for failure: (0) Reserved (1)          */
     /* Failed VSI limit check (2) Reserved (3) Management core      */
     /* failure                                                      */
     /* range: 27:26, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_reason;

     /* MactReqSelf:                                                 */
     /* range: 30:30, access type: RW, default value: 0x0            */
     PCP_REG_FIELD mact_req_self;

     /* MactReqQualifier                                             */
     PCP_REG_FIELD mact_req_qualifier;

  } __ATTRIBUTE_PACKED__ crf_event_reg_3;


  /* Event Fifo High Threshold: Dynamic threshold for FIFO          */
  /* High report - when this value is exceeded Event-FIFO           */
  /* will not aacept events from aging machine/reply-FIFO           */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28073 */

     /* EventFifoHighThreshold: high threshhold                      */
     /* range: 6:0, access type: RW, default value: 0x64             */
     PCP_REG_FIELD event_fifo_high_threshold;

     /* EventReadByCPU: if 1, event is cleared by CPU read, else     */
     /* by OLP clear                                                 */
     /* range: 7:7, access type: RW, default value: 0x0              */
     PCP_REG_FIELD event_read_by_cpu;

     /* MapFidToFidType:                                             */
     /* range: 8:8, access type: RW, default value: 0x0              */
     PCP_REG_FIELD map_fid_to_fid_type;

  } __ATTRIBUTE_PACKED__ event_fifo_high_threshold_reg;


  /* Event Fifo Event Drop Counters: Event FIFO Counters            */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28074 */

     /* EventFifoRpfEventDropCounter: Number of events dropped       */
     /* from RPF (reply fifo). This register is clear on read.       */
     /* range: 7:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD event_fifo_rpf_event_drop_counter;

     /* EventFifoAgeEventDropCounter: Number of events dropped       */
     /* from AGE (aging machine). This register is clear on          */
     /* read.                                                        */
     /* range: 15:8, access type: RO, default value: 0x0             */
     PCP_REG_FIELD event_fifo_age_event_drop_counter;

     /* EventFifoLrfEventDropCounter: Number of events dropped       */
     /* from LRF (learn fifo). This register is clear on read.       */
     /* range: 23:16, access type: RO, default value: 0x0            */
     PCP_REG_FIELD event_fifo_lrf_event_drop_counter;

  } __ATTRIBUTE_PACKED__ event_fifo_event_drop_counters_reg;


  /* Event Fifo Status: The event fifo stores the messages          */
  /* generated by the MACT to the CPU.                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28075 */

     /* EventFifoWatermark: stores the highest entry count           */
     /* observed in the Event FIFO.                                  */
     /* range: 7:0, access type: WMK, default value: 0x0             */
     PCP_REG_FIELD event_fifo_watermark;

     /* EventFifoEntryCount: Reflects the current number of          */
     /* replys in the Event-FIFO. Notice that this does not          */
     /* include the event stored in the CPU block (indicated by      */
     /* EventReady interrupt).                                       */
     /* range: 15:8, access type: RO, default value: 0x0             */
     PCP_REG_FIELD event_fifo_entry_count;

  } __ATTRIBUTE_PACKED__ event_fifo_status_reg;


  /* Reply Fifo Entry Count:                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28076 */

     /* ReplyFifoEntryCount: Reflects the current number of          */
     /* replys in the reply-FIFO                                     */
     /* range: 7:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD reply_fifo_entry_count;

  } __ATTRIBUTE_PACKED__ reply_fifo_entry_count_reg;


  /* Reply Fifo Watermark: CPU Reply FIFO general                   */
  /* configuration                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28077 */

     /* ReplyFifoWatermark: stores the highest entry count           */
     /* observed in the CPU Reply FIFO                               */
     /* range: 7:0, access type: WMK, default value: 0x0             */
     PCP_REG_FIELD reply_fifo_watermark;

  } __ATTRIBUTE_PACKED__ reply_fifo_watermark_reg;


  /* Mrq Fifo Configuration: For debug purpose, configures          */
  /* the maximum size of MRQ FIFO.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28078 */

     /* RequestFifoMaxLevel: FIFO full threshold of MRQ              */
     /* range: 6:0, access type: RW, default value: 0x7f             */
     PCP_REG_FIELD request_fifo_max_level;

  } __ATTRIBUTE_PACKED__ mrq_fifo_configuration_reg;


  /* Managment In Counter: For debug purpose, counts the            */
  /* total number of commands entered the MRQ FIFO.                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28079 */

     /* MrqCounterIn: Events entered MRQ counter This register       */
     /* is clear on read.                                            */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD mrq_counter_in;

  } __ATTRIBUTE_PACKED__ managment_in_counter_reg;


  /* Olp General Configuration0: DSP Engines Configuration          */
  /* Register0                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2807a */

     /* DspEthType: Ethernet Type of packets received by Dune        */
     /* Special Packet Reception unit (DSPR).                        */
     /* range: 15:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD dsp_eth_type;

     /* DisDspEthType: Indicates that received DSPs have an          */
     /* Ethernet type field that should be checked                   */
     /* range: 16:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD dis_dsp_eth_type;

     /* BytesToSkip: Number of bytes to skip from the start of a     */
     /* DSP until the Ethernet type (if exists) or until the         */
     /* first DSP command (if Ethernet type doesn't exist).          */
     /* range: 25:20, access type: RW, default value: 0xc            */
     PCP_REG_FIELD bytes_to_skip;

     /* DaFwdType: The DA forwarding action type. This value is      */
     /* used for Soc_petra-A compatibility when generating a DSP         */
     /* command.                                                     */
     /* range: 30:28, access type: RW, default value: 0x1            */
     PCP_REG_FIELD da_fwd_type;

  } __ATTRIBUTE_PACKED__ olp_general_configuration0_reg;


  /* Olp General Configuration1: DSP Engines Configuration          */
  /* Register3                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2807b */

     /* SourceDevice: The device that caused generation of this      */
     /* message.                                                     */
     /* range: 10:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD source_device;

     /* dismem: disable olp memories                                 */
     /* range: 11:11, access type: RW, default value: 0x0            */
     PCP_REG_FIELD dismem;

  } __ATTRIBUTE_PACKED__ olp_general_configuration1_reg;


  /* Dsp Event Table:                                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2807e */

     /* DspEventTable: Determines destination of the event read.     */
     /* The vectory is accessed with the following fields of the     */
     /* event: \{DB-Profile(1), Part-of-Lag(1), Command(3),          */
     /* Self(1)\}. Each entry contains 3-bits indicating whether     */
     /* to push the event to FIFO1, FIFO2 or the loopback,           */
     /* respectively (i.e. lsb is the loopback indication).          */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD dsp_event_table;

  } __ATTRIBUTE_PACKED__ dsp_event_table_reg[PCP_ELK_OLP_DSP_EVENT_TABLE_REG_MULT_NOF_REGS];

  /* Dsp Engine Configuration[1..2]: DSP generation engines         */
  /* configuration                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28086 */

     /* MaxCmdDelay: Maximum command delay in 64 clocks              */
     /* resolution                                                   */
     /* range: 10:0, access type: RW, default value: 0x64            */
     PCP_REG_FIELD max_cmd_delay;

     /* DspGenerationEn: If set generation of DSP is enabled.        */
     /* range: 11:11, access type: RW, default value: 0x1            */
     PCP_REG_FIELD dsp_generation_en;

     /* MaxDspCmd: Maxium number of commands in a packet minus       */
     /* one.                                                         */
     /* range: 14:12, access type: RW, default value: 0x3            */
     PCP_REG_FIELD max_dsp_cmd;

     /* MinDsp: DSP minimum size in bytes: If packet is smaller,     */
     /* 0 padding is added.                                          */
     /* range: 22:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD min_dsp;

     /* DspHeaderSize: DSP Header size. Valid values are 0           */
     /* through 32.                                                  */
     /* range: 29:24, access type: RW, default value: 0x0            */
     PCP_REG_FIELD dsp_header_size;

     /* DspHeaderEn: Enable DSP header                               */
     /* range: 30:30, access type: RW, default value: 0x0            */
     PCP_REG_FIELD dsp_header_en;

  } __ATTRIBUTE_PACKED__ dsp_engine_configuration_reg[PCP_ELK_OLP_DSP_ENGINE_CONFIGURATION_REG_NOF_REGS];


  /* Dsp Header[1..2]:                                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28087 */

     /* DspHeader: DSP Header.                                       */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD dsp_header;

  } __ATTRIBUTE_PACKED__ dsp_header_reg[PCP_ELK_OLP_DSP_HEADER_REG_NOF_REGS][PCP_ELK_OLP_DSP_HEADER_REG_MULT_NOF_REGS];

  /* Lpm Lkp Conf: LPM lookup configurations                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x28121 */

     /* LpmReqBits63to60: the four msb of the lpm lookup request     */
     /* as received from XAUI                                        */
     /* range: 3:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD lpm_req_bits63to60;

     /* lpm_key_vrf_size: size of the VRF. Must be between 0 and     */
     /* 8                                                            */
     /* range: 7:4, access type: RW, default value: 0x0              */
     PCP_REG_FIELD lpm_key_vrf_size;

     /* rld_is_shared: set if rldram is used both for MACT and       */
     /* LPM                                                          */
     /* range: 8:8, access type: RW, default value: 0x1              */
     PCP_REG_FIELD rld_is_shared;

     /* use_large_rld: set if rldram with 1Mx72 banks is in use.     */
     /* Otherwise bank of 500Kx72b is assumed                        */
     /* range: 9:9, access type: RW, default value: 0x0              */
     PCP_REG_FIELD use_large_rld;

     /* lpm_sec_lkp_size: size of the key for second lkp, must       */
     /* be between 3 and 7                                           */
     /* range: 12:10, access type: RW, default value: 0x7            */
     PCP_REG_FIELD lpm_sec_lkp_size;

     /* lpm_thrd_lkp_size: size of the key for third lkp, must       */
     /* be between 3 and 7                                           */
     /* range: 15:13, access type: RW, default value: 0x5            */
     PCP_REG_FIELD lpm_thrd_lkp_size;

     PCP_REG_FIELD lpm_debug_mode;
     PCP_REG_FIELD lpm_debug_engine;

  } __ATTRIBUTE_PACKED__ lpm_lkp_conf_reg;

  /* lpm debug key:                                              */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28124*/

    /* DspHeader: lpm_debug_key.                                       */
    PCP_REG_FIELD debug_key_0;

  } __ATTRIBUTE_PACKED__ lpm_debug_key_0;

  /* lpm debug key 1:                                              */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28125*/

    /* DspHeader: lpm_debug_key.                                       */
    PCP_REG_FIELD debug_key_1;

  } __ATTRIBUTE_PACKED__ lpm_debug_key_1;

  /* lpm debug key 1:                                              */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28126*/

    /* DspHeader: lpm_debug_key.                                       */
    PCP_REG_FIELD debug_pld;

  } __ATTRIBUTE_PACKED__ lpm_debug_pld;


    /* lpm debug key 1:                                              */
  struct
  {
    PCP_REG_ADDR  addr; /* 0x28127*/

    /* DspHeader: lpm_debug_key.                                       */
    PCP_REG_FIELD debug_trigger;

  } __ATTRIBUTE_PACKED__ lpm_debug_trigger;



} __ATTRIBUTE_PACKED__ PCP_ELK_REGS;
/* Block definition: OAM 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  PCP_REG_ADDR addr;

  /* Interrupt Register: This register contains the interrupt       */
  /* sources residing in this unit.                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c000 */

     /* MsgFifoNotEmpty: 1'- Message fifo not empty - a message      */
     /* is wating for the CPU '0'- Message fifo is empty             */
     /* range: 0:0, access type: RC, default value: 0x0              */
     PCP_REG_FIELD msg_fifo_not_empty;

  } __ATTRIBUTE_PACKED__ interrupt_reg;


  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c010 */

     /* MsgFifoNotEmptyMask: 1'- enable the interrupt '0'- mask      */
     /* the interrupt                                                */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD msg_fifo_not_empty_mask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;


  /* Version:                                                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c012 */

     /* Version: Version of the OAMP                                 */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD version;

  } __ATTRIBUTE_PACKED__ version_reg;


  /* Scratchpad:                                                    */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c013 */

     /* Scratchpad:                                                  */
     /* range: 31:0, access type: RW, default value: 20'h00002       */
     PCP_REG_FIELD scratchpad;

  } __ATTRIBUTE_PACKED__ scratchpad_reg;


  /* Mode Register: Global control of the OAMP operation            */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c014 */

     /* SoftReset: 1' - Apply soft reset all OAMP machines; '0'      */
     /* - release OAMP from soft reset                               */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD soft_reset;

     /* CcmTxEnable: 1' - Enable CCM Tx; '0' - Disable CCM Tx        */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD ccm_tx_enable;

     /* CCM_scan_Enable: 1' - Enable CCM scan; '0' - Disable CCM     */
     /* scan;                                                        */
     /* range: 2:2, access type: RW, default value: 0x0              */
     PCP_REG_FIELD ccm_scan_enable;

     /* RX2CPU: '1' - OAMP Trap all incoming packets to CPU          */
     /* port; '0'- Normal operation                                  */
     /* range: 3:3, access type: RW, default value: 0x0              */
     PCP_REG_FIELD rx2_cpu;

  } __ATTRIBUTE_PACKED__ mode_reg;


  /* CPUPORT: CPU port, data used to build ITMH when packet         */
  /* is trapped to CPU                                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c015 */

     /* CpuSysPort: 13-bit the port to put in ITMH.FWD_DEST_INFO     */
     /* in order to route a packet to the CPU                        */
     /* range: 12:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD cpu_sys_port;

     /* CPU_port_TC: CPU port traffic class to put in                */
     /* ITMH.FWD_TRAFFIC_CLASS when packet is trapped to CPU         */
     /* range: 19:17, access type: RW, default value: 0x0            */
     PCP_REG_FIELD cpu_port_tc;

     /* CPU_port_DP: CPU port traffic class to put in                */
     /* ITMH.FWD_DP when packet is trapped to CPU                    */
     /* range: 23:22, access type: RW, default value: 0x0            */
     PCP_REG_FIELD cpu_port_dp;

  } __ATTRIBUTE_PACKED__ cpuport_reg;


  /* status1reg: General status                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c016 */

     /* TXC_BP: Sticky bit; '1' indicate OAMP received               */
     /* backpressure on the TXC interface                            */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD txc_bp;

     /* RXC_BP: Sticky bit; '1' indicate OAMP generated              */
     /* backpressure to the RXC interface                            */
     /* range: 1:1, access type: RO, default value: 0x0              */
     PCP_REG_FIELD rxc_bp;

     /* ITR_DROP: Sticky bit; '1' indicate OAMP discarded an         */
     /* interrupt because interrupt fifo was full                    */
     /* range: 2:2, access type: RO, default value: 0x0              */
     PCP_REG_FIELD itr_drop;

     /* OversizedRxPacket: Sticky bit; '1' indicate oversized        */
     /* (>2000 B) arrived through the RXC interface                  */
     /* range: 3:3, access type: RO, default value: 0x0              */
     PCP_REG_FIELD oversized_rx_packet;

     /* ITR_COUNT: Numbe of messages, resides in the eventss         */
     /* fifo, that wait for CPU to read                              */
     /* range: 11:4, access type: RO, default value: 0x0             */
     PCP_REG_FIELD itr_count;

  } __ATTRIBUTE_PACKED__ status1_reg;


  /* TX TPID:                                                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c017 */

     /* TX_TPID: TPDI to be inserted to outgoing packets             */
     /* range: 15:0, access type: RW, default value: 0x8100          */
     PCP_REG_FIELD tx_tpid;

  } __ATTRIBUTE_PACKED__ tx_tpid_reg;


  /* TIMERS: internal timers for debug                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c018 */

     /* count100: cound 100 in 10 ms units                           */
     /* range: 6:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD count100;

     /* count_seconds: time in seconds                               */
     /* range: 17:8, access type: RO, default value: 0x0             */
     PCP_REG_FIELD count_seconds;

  } __ATTRIBUTE_PACKED__ timers_reg;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c020 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_0;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c021 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_1;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c022 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_2;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c023 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_3;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c024 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_4;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c025 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_5;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c026 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_6;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c027 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_7;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c028 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_8;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c029 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_9;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c02a */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 11:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_10;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c030 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_0;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c031 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_1;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c032 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_2;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c033 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_3;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c034 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_4;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c035 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_5;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c036 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_6;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c037 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_7;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c038 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_8;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c039 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_9;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c03a */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 11:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_10;


  /* Indirect Command: Indirect Commands. Used to send              */
  /* indirect commands and receive their status                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c040 */

     /* IndirectCommandTrigger: Triggers indirect access as          */
     /* defined by the rest of the registers. The negation of        */
     /* this bit indicates that the operation has been               */
     /* completed.                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger;

     /* IndirectCommandTriggerOnData: If set, then writing to        */
     /* the LSBs of IndirectCommandWrData automatically set          */
     /* IndirectCommandTrigger.                                      */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger_on_data;

     /* IndirectCommandCount: Each write command is executed         */
     /* IndirectCommandCount number of times. The address is         */
     /* advanced by one every write command. If set to 0, one        */
     /* operation is performed.The CPU can read this field to        */
     /* find the number of writes left.                              */
     /* range: 15:2, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_count;

     /* IndirectCommandTimeout: Configures a timeout period for      */
     /* the indirect access command. Resolution is in 256            */
     /* clocks. If set to 0, then the command has no timeout.        */
     /* range: 30:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_timeout;

     /* IndirectCommandStatus: Status of last indirect access        */
     /* command. If set, then last command failed on timeout.        */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_status;

  } __ATTRIBUTE_PACKED__ indirect_command_reg;


  /* Indirect Command Address: Indirect access address.             */
  /* Defines which indirect object is accessed                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c041 */

     /* IndirectCommandAddr: Indirect access address. Defines        */
     /* which indirect object is accessed                            */
     /* range: 30:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_addr;

     /* IndirectCommandType: Type of command to perform: 0 -         */
     /* Write operation 1 - Read operation                           */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_type;

  } __ATTRIBUTE_PACKED__ indirect_command_address_reg;


  /* Indirect Command Data Increment:                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c042 */

     /* IndirectCommandDataIncrement:                                */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_data_increment;

  } __ATTRIBUTE_PACKED__ indirect_command_data_increment_reg_0;


  /* Indirect Command Data Increment:                               */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c043 */

     /* IndirectCommandDataIncrement:                                */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_data_increment;

  } __ATTRIBUTE_PACKED__ indirect_command_data_increment_reg_1;


  /* MAC SA BASE: MAC_SA[31:0] - used as MAC SA base.The OAMP       */
  /* concatenate to this value MEPDB.SA to form the MAC SA.         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c100 */

     /* MAC_SA_BASE:                                                 */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD mac_sa_base;

  } __ATTRIBUTE_PACKED__ mac_sa_base_reg_0;


  /* MAC SA BASE: MAC_SA[31:0] - used as MAC SA base.The OAMP       */
  /* concatenate to this value MEPDB.SA to form the MAC SA.         */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c101 */

     /* MAC_SA_BASE:                                                 */
     /* range: 8:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD mac_sa_base;

  } __ATTRIBUTE_PACKED__ mac_sa_base_reg_1;


  /* Interrupt Message:                                             */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c102 */

     /* InterruptMessage: Head of interrupt message fifo. When       */
     /* the fifo is empty, this register read gives 0xffffffff       */
     /* This register is clear on read.                              */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD interrupt_message;

  } __ATTRIBUTE_PACKED__ interrupt_message_reg;


  /* Rxc Counters: count OAMP incoming packets for debug            */
  /* purpose                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c103 */

     /* OamSopCount: Count incoming packets (sop) that are not       */
     /* marked to bypass                                             */
     /* range: 15:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD oam_sop_count;

  } __ATTRIBUTE_PACKED__ rxc_counters_reg;


  /* Txc Counters: count OAMP outgoing packets for debug            */
  /* purpose                                                        */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c104 */

     /* DownSopCount: Count outgoing packets (sop) directed down     */
     /* range: 15:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD down_sop_count;

     /* UpSopCount: Count outgoing packets (sop) directed up         */
     /* range: 31:16, access type: RO, default value: 0x0            */
     PCP_REG_FIELD up_sop_count;

  } __ATTRIBUTE_PACKED__ txc_counters_reg;


  /* Enable Interrupt Message Event: Disable interrupt              */
  /* message event                                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c105 */

     /* EnableCCM_TimeoutSetEvent: 1'- CCM timeout set event         */
     /* cause interrupt message '0'- CCM timeout set event does      */
     /* NOT cause interrupt message                                  */
     /* range: 0:0, access type: RW, default value: 0x1              */
     PCP_REG_FIELD enable_ccm_timeout_set_event;

  } __ATTRIBUTE_PACKED__ enable_interrupt_message_event_reg;


  /* Debug Reg0:                                                    */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c106 */

     /* DebugReg0:                                                   */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD debug_reg0;

  } __ATTRIBUTE_PACKED__ debug_reg0_reg;


  /* PTCH size:                                                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c107 */

     /* PTCH_size: size of OAMP header (constant)                    */
     /* range: 2:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD ptch_size;

  } __ATTRIBUTE_PACKED__ ptch_size_reg;


  /* CPU PTCH:                                                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x2c108 */

     /* CPU_PTCH: OAMH to add to cpu-trapped packet                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD cpu_ptch;

  } __ATTRIBUTE_PACKED__ cpu_ptch_reg;

} __ATTRIBUTE_PACKED__ PCP_OAM_REGS;
/* Block definition: STS 	 */
typedef struct
{
  uint32   nof_instances; /* 1 */
  PCP_REG_ADDR addr;

  /* Interrupt Register: This register contains the interrupt       */
  /* sources residing in this unit.                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30000 */

     /* OneErrorFixed: If set, one bit error in statistics           */
     /* record was fixed                                             */
     /* range: 0:0, access type: RC, default value: 0x0              */
     PCP_REG_FIELD one_error_fixed;

     /* TwoErrorsDetected: If set, two bits error in statistics      */
     /* record were detected but not fixed                           */
     /* range: 1:1, access type: RC, default value: 0x0              */
     PCP_REG_FIELD two_errors_detected;

     /* LossSync: If set, lost framing sync in statistics record     */
     /* was detected                                                 */
     /* range: 2:2, access type: RC, default value: 0x0              */
     PCP_REG_FIELD loss_sync;

  } __ATTRIBUTE_PACKED__ interrupt_reg;


  /* Interrupt Mask Register: Each bit in this register             */
  /* corresponds to an interrupt source in the Interrupt            */
  /* Register. The interrupt source is masked by writing 0 to       */
  /* the relevant bit in this register                              */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30010 */

     /* OneErrorFixedMask: Writing 0 masks the corresponding         */
     /* interrupt source                                             */
     /* range: 0:0, access type: RC, default value: 0x0              */
     PCP_REG_FIELD one_error_fixed_mask;

     /* TwoErrorsDetectedMask: Writing 0 masks the corresponding     */
     /* interrupt source                                             */
     /* range: 1:1, access type: RC, default value: 0x0              */
     PCP_REG_FIELD two_errors_detected_mask;

     /* LossSyncMask: Writing 0 masks the corresponding              */
     /* interrupt source                                             */
     /* range: 2:2, access type: RC, default value: 0x0              */
     PCP_REG_FIELD loss_sync_mask;

  } __ATTRIBUTE_PACKED__ interrupt_mask_reg;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30020 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_0;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30021 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 31:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_1;


  /* Indirect Command Wr Data: Indirect write data. Data that       */
  /* will be written to the addressed object when an indirect       */
  /* write operation is triggered.                                  */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30022 */

     /* IndirectCommandWrData: Indirect write data.                  */
     /* range: 15:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_wr_data;

  } __ATTRIBUTE_PACKED__ indirect_command_wr_data_reg_2;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30030 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_0;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30031 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 31:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_1;


  /* Indirect Command Rd Data: Data returned by an indirect         */
  /* read operation                                                 */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30032 */

     /* IndirectCommandRdData: Indirect read data                    */
     /* range: 15:0, access type: RO, default value: 0x0             */
     PCP_REG_FIELD indirect_command_rd_data;

  } __ATTRIBUTE_PACKED__ indirect_command_rd_data_reg_2;


  /* Indirect Command: Indirect Commands. Used to send              */
  /* indirect commands and receive their status                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30040 */

     /* IndirectCommandTrigger: Triggers indirect access as          */
     /* defined by the rest of the registers. The negation of        */
     /* this bit indicates that the operation has been               */
     /* completed.                                                   */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger;

     /* IndirectCommandTriggerOnData: If set, then writing to        */
     /* the LSBs of IndirectCommandWrData automatically set          */
     /* IndirectCommandTrigger.                                      */
     /* range: 1:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD indirect_command_trigger_on_data;

     /* IndirectCommandCount: Each write command is executed         */
     /* IndirectCommandCount number of times. The address is         */
     /* advanced by one every write command. If set to 0, one        */
     /* operation is performed.The CPU can read this field to        */
     /* find the number of writes left.                              */
     /* range: 15:2, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_count;

     /* IndirectCommandTimeout: Configures a timeout period for      */
     /* the indirect access command. Resolution is in 256            */
     /* clocks. If set to 0, then the command has no timeout.        */
     /* range: 30:16, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_timeout;

     /* IndirectCommandStatus: Status of last indirect access        */
     /* command. If set, then last command failed on timeout.        */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_status;

  } __ATTRIBUTE_PACKED__ indirect_command_reg;


  /* Indirect Command Address: Indirect access address.             */
  /* Defines which indirect object is accessed                      */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30041 */

     /* IndirectCommandAddr: Indirect access address. Defines        */
     /* which indirect object is accessed                            */
     /* range: 30:0, access type: RW, default value: 0x0             */
     PCP_REG_FIELD indirect_command_addr;

     /* IndirectCommandType: Type of command to perform: 0 -         */
     /* Write operation 1 - Read operation                           */
     /* range: 31:31, access type: RW, default value: 0x0            */
     PCP_REG_FIELD indirect_command_type;

  } __ATTRIBUTE_PACKED__ indirect_command_address_reg;


  /* Mode: statistics Mode configuration                            */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30100 */

     /* EccMode: 0' - no ECC, '1' - with ECC                         */
     /* range: 0:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD ecc_mode;

     /* StatMode: 00 - billing mode with inlif and outlif            */
     /* range: 2:1, access type: RW, default value: 0x0              */
     PCP_REG_FIELD stat_mode;

  } __ATTRIBUTE_PACKED__ mode_reg;

    /* AC offset: statistics record AC offset                     */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30101 */

     /* In_AC_offset: 3 msb of In AC in record to matc                      */
     /* range: 2:0, access type: RW, default value: 0x0              */
     PCP_REG_FIELD in_ac_offset;

     /* Out_AC_offset: 3 msb of Out AC in record to match           */
     /* range: 5:3, access type: RW, default value: 0x0              */
     PCP_REG_FIELD out_ac_offset;

  } __ATTRIBUTE_PACKED__ ac_offset_reg;

  /* STS Status: STS block status indications                       */
  struct
  {
     PCP_REG_ADDR  addr; /* 0x30102 */

     /* ReportSync: 1' indicates synchronization to statistics       */
     /* record frame                                                 */
     /* range: 0:0, access type: RO, default value: 0x0              */
     PCP_REG_FIELD report_sync;

  } __ATTRIBUTE_PACKED__ sts_status_reg;

} __ATTRIBUTE_PACKED__ PCP_STS_REGS;
  /* Blocks definition } */

  typedef struct
  {
    PCP_ECI_REGS eci;
    PCP_CSI_REGS csi;
    PCP_ELK_REGS elk;
    PCP_OAM_REGS oam;
    PCP_STS_REGS sts;
  } __ATTRIBUTE_PACKED__ PCP_REGS;
/* } */

/*************
 * GLOBALS   *
 *************/
/* { */

typedef enum
{

  PCP_REGS_GET = PCP_PROC_DESC_BASE_CHIP_REGS_FIRST,
  PCP_REGS_INIT,

  /*
   * Last element. Do no touch.
   */
  PCP_CHIP_REGS_PROCEDURE_DESC_LAST
} PCP_CHIP_REGS_PROCEDURE_DESC;

typedef enum
{
  PCP_REGS_NOT_INITIALIZED_ERR = PCP_ERR_DESC_BASE_CHIP_REGS_FIRST,
  PCP_LOW_LEVEL_ACCESS_ERR,
 
  /*
   * Last element. Do no touch.
   */
  PCP_CHIP_REGS_ERR_LAST
} PCP_CHIP_REGS_ERR;

/* } */

/*************
 * FUNCTIONS *
 *************/
/* { */

/*****************************************************
*NAME
* pcp_regs_get
*TYPE:
*  PROC
*FUNCTION:
*  Get a pointer to registers database.
*  The database is per chip-version.
*INPUT:
*  SOC_SAND_DIRECT:
*    SOC_SAND_OUT PCP_REGS  **pcp_regs - pointer to pcp
*                           registers database.
*  SOC_SAND_INDIRECT:
*    None.
*OUTPUT:
*  SOC_SAND_DIRECT:
*    error indication
*  SOC_SAND_INDIRECT:
*    *pcp_regs.
*REMARKS:
*    If the database is not initialized - error will be indicated.
*    pcp_is_reg_db_initialized should be called before
*    calling to this function.
*SEE ALSO:
*****************************************************/
uint32
  pcp_regs_get(
    SOC_SAND_OUT PCP_REGS  **pcp_regs
  );

PCP_REGS*
  pcp_regs(void);

/*****************************************************
*NAME
*  pcp_regs_init
*TYPE:
*  PROC
*FUNCTION:
*  Dynamically allocates and initializes Pcp registers database.
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
*   Calling upon already initialized registers will do nothing
*   User must make sure that this function is called after a semaphore was taken
*SEE ALSO:
*****************************************************/
uint32
  pcp_regs_init(void);

CONST SOC_PROCEDURE_DESC_ELEMENT*
  pcp_chip_regs_get_procs_ptr(void);

CONST SOC_ERROR_DESC_ELEMENT*
  pcp_chip_regs_get_errs_ptr(void);
/* } */

#include <soc/dpp/SAND/Utils/sand_footer.h>

/* } __SOC_PCP_CHIP_REGS_INCLUDED__*/
#endif

