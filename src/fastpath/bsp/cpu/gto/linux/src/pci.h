/**************************************************************** 
 *
 * File:  	pci.h
 *
 * 
 * Description: This file contains all the necessary definitions
 * Mainly pci related definitions and other general macros.
 *
 ****************************************************************/
#ifndef PCI_H
#define PCI_H

#include "globals.h"

/*
* selecting test mode
*/
#define DMA_DIRECT    1 /*1=direct         ;  0=chaining*/
#define MESG_REGISTER 1 /*1=message register; 0=doorbell*/

/*
* macro Defintions
*/
#define	READ_BYTE(arg, data) data = arg
#define	READ_SHORT(arg, data) data = arg
#define	READ_LONG(arg, data) data = arg
#define	GET_BYTE(arg) arg
#define	GET_SHORT(arg) arg
#define	GET_LONG(arg) arg
#define	WRITE_BYTE(arg, data) arg = data
#define	WRITE_SHORT(arg, data) arg = data
#define	WRITE_LONG(arg, data) arg = data

/*
* definitions related to dma
*/
#define NUM_OF_DMA_BLOCKS 0x4
#define DMA_BLOCK_SIZE 0x800
#define BASE_OF_DMA_SOURCE_BLOCKS 0xc00000
#define BASE_OF_DMA_DEST_BLOCKS   0x80000000

/* 
* dma mode register bits
*/
#define DIR_MODE_SNOOP_ENABLE   0x00100000
#define INTERRUPT_STEER         0x00080000
#define DEST_HOLD_ENABLE        0x00002000
#define SOURCE_HOLD_ENABLE      0x00001000
#define END_OF_TRANSFER_INT     0x00000080
#define TRANSFER_ERROR_MODE            0x00000008
#define CHANNEL_DIRECT_TRANSFER_MODE   0x00000004 
#define CHANNEL_CONTINUES              0x00000002
#define CHANNEL_START                  0x00000001

/* 
* dma chaining definitions 
*/
#define EOTD 0x00000001


/* dma status register bits */
#define TRANSFER_ERROR          0x00000080
#define CHANNEL_BUSY            0x00000004
#define EOSI                    0x00000002
#define END_OF_CHAIN_DIR_INT    0x00000001

/* 
* pci comparison mask register bits 
*/
#define ENABLE_TRANSLATION      0x80000000
#define DISABLE_SNOOPING_IO     0x40000000
#define PREFETCHABLE            0x20000000
#define GPLA_PREFETCHABLE       0x00000008

/* 
* omimr and oisr related
*/
#define OUTBOUND_POST_QUEUE             0x00000020
#define OUTBOUND_DOORBELL               0x00000008
#define OUTBOUND_MAILBOX_1              0x00000002
#define OUTBOUND_MAILBOX_0              0x00000001

/* 
* imimr and imisr related
*/
#define OUTBOUND_FREE_OVERFLOW          0x00000100
#define INBOUND_POST_OVERFLOW           0x00000080
#define INBOUND_POST_QUEUE              0x00000020
#define MACHINE_CHECK_DOORBELL          0x00000010
#define INBOUND_DOORBELL                0x00000008
#define INBOUND_MAILBOX_1               0x00000002
#define INBOUND_MAILBOX_0               0x00000001

/* 
* doorbell register (inbound and outbound): bit definition 
*/
#define IN_DOORBELL_BIT_31_MACHINE_CHECK 0x80000000
#define IN_DOORBELL_BIT_30               0x40000000
#define IN_DOORBELL_BIT_29               0x20000000
#define IN_OUT_DOORBELL_BIT_28           0x10000000
#define IN_OUT_DOORBELL_BIT_27           0x08000000

/*
* definitions pertaining to the windows
* in the configuration space
*/
#define FULL_MASK               0xffffffff
#define NUM_OF_WINDOWS_IN_PQII  3
#define ACTUAL_BASE       0xfffff000

/*
* command register (in config space)
* related definitions
*/
#define SERR                    0x0100
#define PERR                    0x0040
#define PCI_MASTER              0x0004
#define RESPOND_TO_MEMORY_SPACE 0x0002

/* 
* general purpose control register 
*/
#define  INT2PCI  0x00004000
#define  MCP2PCI  0x00002000

/* 
* general control register 
*/
#define SOFT_PCI_RESET          0x00000001

/*
* bits for emr and esr 
*/
#define I2O_DBMC                0x00001000
#define NMI                     0x00000800
#define IRA                     0x00000400
#define I2O_IPQO                  0x00000200
#define I2O_OFQO                  0x00000100
#define PCI_PERR_WR               0x00000080
#define PCI_PERR_RD               0x00000040
#define PCI_SERR                  0x00000020
#define PCI_TAR_ABT               0x00000010
#define PCI_NO_RSP                0x00000008
#define PCI_DATA_PAR_RD           0x00000004
#define PCI_DATA_PAR_WR           0x00000002
#define PCI_ADDR_PAR              0x00000001

/*
* pci interrupt controller related 
* defintions (for host only)
*/
#define POSSIBLE_PCI_INTERRUPTS 0xFFF00000
#define PCI_INT 0x4730000       /* pci interrupt controller location(/CS8) */

/*
* for future use and not used by this program
* definitions for SCCR
*/
#define SCCR_NEW_DEF //testing the SCCR bit description of the PCI functional spec
#define SCCR_PCI_MODE   0x00000100 //bit 23=PCI_MODE
#define SCCR_PCI_MODCK  0x00000080 //bit 24=PCI_MODCK
#define SCCR_PCI_PCIDF  0x00000078//bit 25,26,27,28=PCIDF
#define SCCR_PCI_CLPD   0x00000004//bit 29=CLPD
#define SCCR_PCI_DFBRG  0x00000003//bit 30,31=DFBRG

/* 
* this is the structure of a DMA chain buffer in the memory 
*/
typedef struct {
  UINT32            source_address; 
  UINT32            reserved0;
  UINT32            destination_address;
  UINT32            reserved1; 
  UINT32            next_descriptor_address;
  UINT32            reserved2;
  UINT32            size;
  UINT32            reserved3;
} __attribute__((packed)) t_DmaChainBuffer;


/* 
* offsets in the configuration space
*/
typedef enum{
    e_VendorId = 0x00,  
	e_DevId = 0x02, 
	e_PciCommand = 0x04, 
	e_PciStatus = 0x06, 
	e_RevId = 0x08, 
	e_ProgIntf = 0x09, 
	e_SubclassCode = 0xa, 
	e_BaseClass = 0x0b, 
	e_CacheLine = 0x0c, 
	e_LatencyTimer = 0x0d, 
	e_HeaderType = 0x0e, 
	e_BISTControl = 0x0f, 
	e_BaseAddrLow = 0x10, 
    e_BaseAddrHigh = 0x14, 
	e_SubsystemVendorId = 0x2c, 
	e_SubsystemId = 0x2e, 
	e_InterruptLine = 0x3c, 
	e_InterruptPin = 0x3d, 
	e_MinGnt = 0x3e, 
	e_MaxLat = 0x3f, 
    e_TrdyTimeout = 0x40, 
    e_Retry = 0x41,    
	} e_CfgReg;


/*
* function prototypes related to intialization
* and DMA
*/
extern void PCI_Init (void);

extern void PCI_Teste ();

#endif

