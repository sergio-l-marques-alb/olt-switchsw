/*
 *
 * (C) Copyright 2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2004 Red Hat, Inc.
 *
 * 2005 (c) MontaVista Software, Inc.
 * Vitaly Bordug <vbordug@ru.mvista.com>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/pci.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/irq.h>
#include <linux/interrupt.h>

#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/uaccess.h>
#include <asm/machdep.h>
#include <asm/pci-bridge.h>
#include <asm/immap_cpm2.h>
#include <asm/mpc8260.h>
#include <asm/cpm2.h>

#include "pq2ptin_pci.h"


#ifdef CONFIG_PQ2_PTIN
struct pci_controller hose_clemos;

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


//Celso Lemos - 19/02/2009
unsigned long SwapLong(unsigned long *val)
{
    *val = ((*val & 0xFF) << 24) | ((*val & 0xFF00) << 8) |
        ((*val & 0xFF0000UL) >> 8) | ((*val & 0xFF000000UL) >> 24);
}

//Celso Lemos - 19/02/2009
unsigned long ConstructConfigWord (unsigned long BusNum,unsigned long DevNum,e_CfgReg RegNum)
{
  unsigned long value;
  unsigned long FuncNum=0x0;
  value = (
          (BusNum <<16)    | 
          (DevNum <<11)    | 
          (FuncNum <<8)    | 
          (RegNum & (~3) ) |
          0x80000000
          );
  return value;

}

//Celso Lemos - 19/02/2009
void PCI_WriteLong (unsigned long address, unsigned long value) 
{
  	SwapLong(&value);
  	WRITE_LONG(*(unsigned long *) address, value);
}


//Celso Lemos - 19/02/2009
unsigned long PCI_ReadLong (unsigned long address)
{
  	unsigned long pci_value;
  	pci_value = GET_LONG(*(volatile unsigned long *) address);
  	SwapLong(&pci_value);	
  	return pci_value;
}


//Celso Lemos - 19/02/2009
void PCI_CfgWriteByte (unsigned long BusNum,unsigned long DevNum, e_CfgReg configuration_register_offset,unsigned short value)
{
  unsigned long configuration_data, configuration_address;
    
  configuration_address = ConstructConfigWord(BusNum,DevNum,configuration_register_offset);
  PCI_WriteLong ((unsigned long) &(cpm2_immr->im_pci.pci_cfg_addr), configuration_address);

  configuration_data = PCI_ReadLong((unsigned long) &(cpm2_immr->im_pci.pci_cfg_data));
  *(((unsigned char *)&configuration_data) + (3 - (configuration_register_offset & 3))) = value;
  PCI_WriteLong ((unsigned long) &(cpm2_immr->im_pci.pci_cfg_addr), configuration_address);
  PCI_WriteLong((unsigned long) &(cpm2_immr->im_pci.pci_cfg_data), configuration_data);
}


//Celso Lemos - 19/02/2009
void PCI_CfgWriteLong(unsigned long BusNum, unsigned long DevNum, e_CfgReg configuration_register_offset, unsigned long value)
{
     unsigned long Config_Addr;     

    /* 
    * get the value to be written to the cfga register
    */   
    Config_Addr = ConstructConfigWord(BusNum,DevNum,configuration_register_offset);
 
    /*
    * byte-swap the value and write the byte-swapped value to the register
    */
    SwapLong(&Config_Addr);

    /*
    * write to configuration address register
    */
    cpm2_immr->im_pci.pci_cfg_addr = Config_Addr; /*write*/    
    
    /*
    * write value to  cfgd
    */
    PCI_WriteLong((unsigned long)&cpm2_immr->im_pci.pci_cfg_data, value);
}


//Celso Lemos - 19/02/2009
void PCI_CfgWriteShort (unsigned long BusNum,unsigned long DevNum,	
			e_CfgReg configuration_register_offset,
			unsigned short value) 
{
 
  	unsigned long configuration_data, configuration_address;
    
	configuration_address = ConstructConfigWord(BusNum,DevNum,configuration_register_offset);
  	PCI_WriteLong ((unsigned long) &(cpm2_immr->im_pci.pci_cfg_addr), configuration_address);

  	configuration_data = PCI_ReadLong((unsigned long) &(cpm2_immr->im_pci.pci_cfg_data));
  	*(((unsigned short *)&configuration_data) + (1 - (configuration_register_offset & 1))) = value;

  	PCI_WriteLong ((unsigned long) &(cpm2_immr->im_pci.pci_cfg_addr), configuration_address);
  	PCI_WriteLong((unsigned long) &(cpm2_immr->im_pci.pci_cfg_data), configuration_data);
}


//Celso Lemos - 19/02/2009
unsigned long PCI_CfgReadLong(unsigned long BusNum, unsigned long DevNum, e_CfgReg RegNum)
{
     unsigned long Config_Addr;
     unsigned long temp;
    /* 
    * get the value to be written to the cfga register
    */   
    Config_Addr = ConstructConfigWord(BusNum,DevNum,RegNum);
 
    /*
    * byte-swap the value and write the byte-swapped value to the register
    */
    SwapLong(&Config_Addr);

    /*
    * write to configuration address register
    */
    cpm2_immr->im_pci.pci_cfg_addr = Config_Addr; /*write*/    
    
    /*
    * read cfgd
    */
    temp = GET_LONG(*(volatile unsigned long *)&cpm2_immr->im_pci.pci_cfg_data);
    
    /*
    * byte swap the value
    */
    SwapLong(&temp);
    return temp; 
}


//Celso Lemos - 19/02/2009
int pq2ptin_write_pci(struct pci_bus bus, int devfn, int pos, int len, u32 value)
{
  //printk("pq2_write_pci: pos=%d, len %d, value=%d\n");
  switch (len) {
    case 1: //write byte
      PCI_CfgWriteByte(0, devfn, pos, (u16)value);
      break;

    case 2: //write short
      PCI_CfgWriteShort(0, devfn, pos, (u16)value);
      break;

    case 4: //write long
      PCI_CfgWriteLong(0, devfn, pos, (u32)value);
      break;

    default:
      return(NULL);
  }
  return(0);
}


/*
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
      */

//Celso Lemos - 19/02/2009
int pq2ptin_read_pci(struct pci_bus bus, int devfn, int pos, int len, int *value)
{
  unsigned long aux_long;
  int aux;
  //printk("pq2_read_pci: pos=%d, len %d, value=%d\n");
  switch (len) {
    case 1: //Read byte      
      aux = (pos/4)*4; // base de leitura do long
      aux_long = PCI_CfgReadLong(0, devfn, aux);
      *value = (aux_long>>(pos-aux)*8) & 0xFF;
      //printk("pq2_read_pci(byte): devfn=%d, pos=0x%.2x, value=0x%.2x\n",devfn, pos, *value);
      break;

    case 2: //Read short
      aux = (pos/4)*4; // base de leitura do long
      aux_long = PCI_CfgReadLong(0, devfn, aux);
      *value = (aux_long>>(pos-aux)*8) & 0xFFFF;
      //printk("pq2_read_pci(short): devfn=%d, pos=0x%.2x, value=0x%.4x\n",devfn, pos, *value);
      break;

    case 4: //Read long
      *value = PCI_CfgReadLong(0, devfn, pos);
      //if (devfn==10) {
        //printk("pq2_read_pci (dword): devfn=%d, pos=0x%.2x, value=0x%.8x\n",devfn, pos, *value);
      //}
      break;

    default:
      *value = 0;
      return(-1);
  }
  return(0);
}


//Celso Lemos - 19/02/2009
void PCI_Init ()
{
  int i;
  unsigned long ConfigData;

  printk("\n");
  printk("+++++++++++++++++++++++++++++++++++++++++\n");
  printk("CX120G: Init PCI device\n\r");
  printk("+++++++++++++++++++++++++++++++++++++++++\n");

  //garante que o device pci sai de reset
  // ---------------------------------------------------------------------
  PCI_WriteLong((unsigned long)&cpm2_immr->im_pci.pci_gcr, 0);  
  PCI_WriteLong((unsigned long)&cpm2_immr->im_pci.pci_gcr, SOFT_PCI_RESET);  

  for (i=0;i<100;i++) {
    udelay(100);
  }

  //Vai configurar a latencia=0xF8 e a cache line size = 0x08 do device presente no bus PCI
  // ---------------------------------------------------------------------
  PCI_CfgWriteByte(0,10,e_CacheLine,0x00);
  PCI_CfgWriteByte(0,10,e_LatencyTimer,0x00);

  //Configura Base de enderecamento para o device PCI.
  //Como no processador o Bus pci est mapeado no addr=0x80000000 e a outbound window est mapeada tambm em 0x80000000, 
  // o addr que selecciona o device e' o 0x80000000
  // ---------------------------------------------------------------------
  PCI_CfgWriteLong(0,10,e_BaseAddrLow,0x80000000);

  //configura o device para responder a comandos de leitura de memoria atravs do bus pci
  // ---------------------------------------------------------------------
  PCI_CfgWriteLong(0,10,e_PciCommand,0x00000002);

  //configura o device para permitir acessos big endian
  // ---------------------------------------------------------------------
  //PCI_Configure_Access_big_endian();

  // ---------------------------------------------------------------------
  //Le os parametros dos registos de configuracao (Configuration mapping
  // ---------------------------------------------------------------------

  // Vendor Id e Device ID
  ConfigData = PCI_CfgReadLong(0,10,0x00);
  printk("+   - DEVICE ID           = 0x%.4x      +\n",(ConfigData>>16) & 0xFFFF);
  printk("+   - VENDOR ID           = 0x%.4x      +\n",ConfigData & 0xFFFF);
                       
  //Command / Status
  ConfigData = PCI_CfgReadLong(0,10,0x04);
  printk("+   - COMMAND/STATUS      = 0x%.8x  +\n",ConfigData);

  //Class Code / Revision ID
  ConfigData = PCI_CfgReadLong(0,10,0x08);
  printk("+   - CLASS CODE          = 0x%.6x    +\n",(ConfigData>>8) & 0xFFFFFF);
  printk("+   - REVISION ID         = 0x%.4x      +\n",ConfigData & 0xFF);


  //Self Teste / Header Type / Latency Timer / Cache Size
  ConfigData = PCI_CfgReadLong(0,10,0x0C);
  printk("+   - SELF TEST           = 0x%.2x        +\n",(ConfigData>>24) & 0xFF);
  printk("+   - HEADER TYPE         = 0x%.2x        +\n",(ConfigData>>16) & 0xFF);
  printk("+   - LATENCY TIMER       = 0x%.2x        +\n",(ConfigData>>8) & 0xFF);
  printk("+   - CACHE LINE SIZE     = 0x%.2x        +\n",ConfigData & 0xFF);


  //Base Address Low
  ConfigData = PCI_CfgReadLong(0,10,0x10);
  printk("+   - BASE ADRESS LOW     = 0x%.8x  +\n",ConfigData);

  //Base Address High
  ConfigData = PCI_CfgReadLong(0,10,0x14);
  printk("+   - BASE ADRESS HIGH    = 0x%.8x  +\n",ConfigData);

  //SubSystem ID / Subsystem vendor Id
  ConfigData = PCI_CfgReadLong(0,10,0x2C);
  printk("+   - SUBSYSTEM ID        = 0x%.4x      +\n",(ConfigData>>16) & 0xFFFF);
  printk("+   - SUBSYSTEM VENDOR ID = 0x%.4x      +\n",ConfigData & 0xFFFF);

  //Max Latency / Min Grant / interrupt pin / interrupt line
  ConfigData = PCI_CfgReadLong(0,10,0x3C);
  printk("+   - MAX LATENCY         = 0x%.2x        +\n",(ConfigData>>24) & 0xFF);
  printk("+   - MIN GRANT           = 0x%.2x        +\n",(ConfigData>>16) & 0xFF);
  printk("+   - INTERRUPT PIN       = 0x%.2x        +\n",(ConfigData>>8) & 0xFF);
  printk("+   - INTERRUPT LINE      = 0x%.2x        +\n",ConfigData & 0xFF);

  //trdy timeout / retry count
  ConfigData = PCI_CfgReadLong(0,10,0x40);
  printk("+   - TRDY TIMEOUT        = 0x%.2x        +\n",(ConfigData>>8) & 0xFF);
  printk("+   - RETRY COUNT         = 0x%.2x        +\n",ConfigData & 0xFF);

  printk("+++++++++++++++++++++++++++++++++++++++++\n");
}



//Celso Lemos - 19/02/2009
void PCI_Init_apos ()
{
  int i;
  unsigned long ConfigData;

  printk("\n");
  printk("+++++++++++++++++++++++++++++++++++++++++\n");
  printk("CX120G: Init PCI device apos config\n\r");
  printk("+++++++++++++++++++++++++++++++++++++++++\n");

  // Vendor Id e Device ID
  ConfigData = PCI_CfgReadLong(0,10,0x00);
  printk("+   - DEVICE ID           = 0x%.4x      +\n",(ConfigData>>16) & 0xFFFF);
  printk("+   - VENDOR ID           = 0x%.4x      +\n",ConfigData & 0xFFFF);
                       
  //Command / Status
  ConfigData = PCI_CfgReadLong(0,10,0x04);
  printk("+   - COMMAND/STATUS      = 0x%.8x  +\n",ConfigData);

  //Class Code / Revision ID
  ConfigData = PCI_CfgReadLong(0,10,0x08);
  printk("+   - CLASS CODE          = 0x%.6x    +\n",(ConfigData>>8) & 0xFFFFFF);
  printk("+   - REVISION ID         = 0x%.4x      +\n",ConfigData & 0xFF);


  //Self Teste / Header Type / Latency Timer / Cache Size
  ConfigData = PCI_CfgReadLong(0,10,0x0C);
  printk("+   - SELF TEST           = 0x%.2x        +\n",(ConfigData>>24) & 0xFF);
  printk("+   - HEADER TYPE         = 0x%.2x        +\n",(ConfigData>>16) & 0xFF);
  printk("+   - LATENCY TIMER       = 0x%.2x        +\n",(ConfigData>>8) & 0xFF);
  printk("+   - CACHE LINE SIZE     = 0x%.2x        +\n",ConfigData & 0xFF);


  //Base Address Low
  ConfigData = PCI_CfgReadLong(0,10,0x10);
  printk("+   - BASE ADRESS LOW     = 0x%.8x  +\n",ConfigData);

  //Base Address High
  ConfigData = PCI_CfgReadLong(0,10,0x14);
  printk("+   - BASE ADRESS HIGH    = 0x%.8x  +\n",ConfigData);

  //SubSystem ID / Subsystem vendor Id
  ConfigData = PCI_CfgReadLong(0,10,0x2C);
  printk("+   - SUBSYSTEM ID        = 0x%.4x      +\n",(ConfigData>>16) & 0xFFFF);
  printk("+   - SUBSYSTEM VENDOR ID = 0x%.4x      +\n",ConfigData & 0xFFFF);

  //Max Latency / Min Grant / interrupt pin / interrupt line
  ConfigData = PCI_CfgReadLong(0,10,0x3C);
  printk("+   - MAX LATENCY         = 0x%.2x        +\n",(ConfigData>>24) & 0xFF);
  printk("+   - MIN GRANT           = 0x%.2x        +\n",(ConfigData>>16) & 0xFF);
  printk("+   - INTERRUPT PIN       = 0x%.2x        +\n",(ConfigData>>8) & 0xFF);
  printk("+   - INTERRUPT LINE      = 0x%.2x        +\n",ConfigData & 0xFF);

  //trdy timeout / retry count
  ConfigData = PCI_CfgReadLong(0,10,0x40);
  printk("+   - TRDY TIMEOUT        = 0x%.2x        +\n",(ConfigData>>8) & 0xFF);
  printk("+   - RETRY COUNT         = 0x%.2x        +\n",ConfigData & 0xFF);

  printk("+++++++++++++++++++++++++++++++++++++++++\n");
}


static int pciauto_upper_iospc;
static int pciauto_upper_memspc;


void __init pq2ptin_pciauto_postscan_setup_bridge(struct pci_controller *hose,
		int current_bus,
		int pci_devfn,
		int sub_bus,
		int *iosave,
		int *memsave)
{
	int cmdstat;

    PCI_CfgWriteByte(0,10,e_CacheLine,0x00);
    PCI_CfgWriteByte(0,10,e_LatencyTimer,0x00);

    //Configura Base de enderecamento para o device PCI.
    //Como no processador o Bus pci est mapeado no addr=0x80000000 e a outbound window est mapeada tambm em 0x80000000, 
    // o addr que selecciona o device e' o 0x80000000
    // ---------------------------------------------------------------------
    PCI_CfgWriteLong(0,10,e_BaseAddrLow,0x80000000);

    //configura o device para responder a comandos de leitura de memoria atravs do bus pci
    // ---------------------------------------------------------------------
    PCI_CfgWriteLong(0,10,e_PciCommand,0x00000002);
}




void __init pq2ptin_pciauto_setup_bars(struct pci_controller *hose,
		int current_bus,
		int pci_devfn,
		int bar_limit)
{
	int bar_response, bar_size, bar_value;
	int bar, addr_mask;
	int * upper_limit;
	int found_mem64 = 0;

	printk("PCI Autoconfig: Found Bus %d, Device %d, Function %d\n",
		current_bus, PCI_SLOT(pci_devfn), PCI_FUNC(pci_devfn) );

	for (bar = PCI_BASE_ADDRESS_0; bar <= bar_limit; bar+=4) {
		/* Tickle the BAR and get the response */
        PCI_CfgWriteLong(current_bus,pci_devfn,bar,0xffffffff);

        bar_response = PCI_CfgReadLong(current_bus,pci_devfn,bar);

		/* If BAR is not implemented go to the next BAR */
		if (!bar_response)
			continue;

		/* Check the BAR type and set our address mask */
		if (bar_response & PCI_BASE_ADDRESS_SPACE) {
			addr_mask = PCI_BASE_ADDRESS_IO_MASK;
			upper_limit = &pciauto_upper_iospc;
			printk("PCI Autoconfig: BAR 0x%x, I/O, ", bar);
		} else {
			if ( (bar_response & PCI_BASE_ADDRESS_MEM_TYPE_MASK) ==
			PCI_BASE_ADDRESS_MEM_TYPE_64)
				found_mem64 = 1;

			addr_mask = PCI_BASE_ADDRESS_MEM_MASK;	
			upper_limit = &pciauto_upper_memspc;
			printk("PCI Autoconfig: BAR 0x%x, Mem ", bar);
		}

		/* Calculate requested size */
		bar_size = ~(bar_response & addr_mask) + 1;

		/* Allocate a base address */
		bar_value = (*upper_limit - bar_size) & ~(bar_size - 1);

		/* Write it out and update our limit */
        PCI_CfgWriteLong(current_bus,pci_devfn,bar,bar_value);
		*upper_limit = bar_value;

		/*
		 * If we are a 64-bit decoder then increment to the
		 * upper 32 bits of the bar and force it to locate
		 * in the lower 4GB of memory.
		 */
		if (found_mem64) {
			bar += 4;
            PCI_CfgWriteLong(current_bus,pci_devfn,bar,0x00000000);
			found_mem64 = 0;
		}

		printk("size=0x%x, address=0x%x\n",
			bar_size, bar_value);
	}
}



int __init pq2ptin_pciauto_bus_scan(struct pci_controller *hose, int current_bus)
{
	int sub_bus, pci_devfn, pci_class, cmdstat, found_multi = 0;
	unsigned short vid;
	unsigned char header_type;
    unsigned long aux;


    printk("pciauto_bus_scan: init, current_bus=%d\n",current_bus);
	/*
	 * Fetch our I/O and memory space upper boundaries used
	 * to allocated base addresses on this hose.
	 */
	if (current_bus == hose->first_busno) {
		pciauto_upper_iospc = hose->io_space.end + 1;
		pciauto_upper_memspc = hose->mem_space.end + 1;
	}

	sub_bus = current_bus;

    //pci_devfn = 10; //fixo

    for (pci_devfn = 0; pci_devfn < 0xff; pci_devfn++) {

      //printk("pciauto_bus_scan: pci_devfn=%d, current_bus=%d, pci_devfn=%d, PCI_FUNC(pci_devfn)=%d, found_multi=%d\n",
      //       pci_devfn,
      //       current_bus,
      //       pci_devfn,
      //       PCI_FUNC(pci_devfn),
      //       found_multi
      //       );

      if ( (current_bus == hose->first_busno) && (pci_devfn == 0) )
          continue;

      //if (PCI_FUNC(pci_devfn) && !found_multi)
      //    continue;
      
      aux = PCI_CfgReadLong(0,pci_devfn,e_CacheLine);
      //printk("pciauto_bus_scan: aux (e_CacheLine) =0x%.8x\n",aux);
  
      /* If config space read fails from this device, move on */
      header_type = (aux>>16) & 0xFF;
      if (header_type==0xFF) 
        continue;
        //return(sub_bus);
     
      if (!PCI_FUNC(pci_devfn))
      	found_multi = header_type & 0x80;
  
      aux = PCI_CfgReadLong(0,pci_devfn,e_VendorId);
      //printk("pciauto_bus_scan: aux (e_VendorId) =0x%.8x\n",aux);
     
      vid = aux & 0xFFFF;
  
      if (vid != 0xffff) {
  
          aux = PCI_CfgReadLong(0,pci_devfn,e_RevId);
          pci_class = aux;
  
          printk("pci_class = 0x%.8x\n",pci_class);
  
          if ( (pci_class >> 16) == PCI_CLASS_NETWORK_OTHER ) {
              int iosave, memsave;
  
              printk("PCI Autoconfig: Found P2P bridge, device %d\n", PCI_SLOT(pci_devfn));
              /* Allocate PCI I/O and/or memory space */            


              pq2ptin_pciauto_setup_bars(hose,
                      current_bus,
                      pci_devfn,
                      PCI_BASE_ADDRESS_1);
              
              sub_bus = sub_bus+1; //pq2_pciauto_bus_scan(hose, sub_bus+1);
              /*
              pq2_pciauto_postscan_setup_bridge(hose,
                      current_bus,
                      pci_devfn,
                      sub_bus,
                      &iosave,
                      &memsave);            
              */
          } 	
          else {
            printk("Classe desconhecida(0x%.4x)\n",pci_class);
          }
      }
    }
	return sub_bus;
}



#endif

/*
 * Interrupt routing
 */

static inline int
pq2ptin_pci_map_irq(struct pci_dev *dev, unsigned char idsel, unsigned char pin)
{
	static char pci_irq_table[][4] =
	//
	// 	PCI IDSEL/INTPIN->INTLINE
	//  	  A      B      C      D
	//
	{
		{ PIRQA, PIRQB, PIRQC, PIRQD },	// IDSEL 22 - PCI slot 0 
		{ PIRQD, PIRQA, PIRQB, PIRQC },	// IDSEL 23 - PCI slot 1 
		{ PIRQC, PIRQD, PIRQA, PIRQB },	// IDSEL 24 - PCI slot 2 
	};

	const long min_idsel = 22, max_idsel = 24, irqs_per_slot = 4;
	return PCI_IRQ_TABLE_LOOKUP;
}

/*
static void
pq2pci_mask_irq(unsigned int irq)
{
	int bit = irq - NR_CPM_INTS;

	*(volatile unsigned long *) PCI_INT_MASK_REG |= (1 << (31 - bit));
	return;
}

static void
pq2pci_unmask_irq(unsigned int irq)
{
	int bit = irq - NR_CPM_INTS;

	*(volatile unsigned long *) PCI_INT_MASK_REG &= ~(1 << (31 - bit));
	return;
}

static void
pq2pci_mask_and_ack(unsigned int irq)
{
	int bit = irq - NR_CPM_INTS;

	*(volatile unsigned long *) PCI_INT_MASK_REG |= (1 << (31 - bit));
	return;
}

static void
pq2pci_end_irq(unsigned int irq)
{
	int bit = irq - NR_CPM_INTS;

	*(volatile unsigned long *) PCI_INT_MASK_REG &= ~(1 << (31 - bit));
	return;
}

struct hw_interrupt_type pq2ptin_pci_ic = {
	"PQ2 PCI",
	NULL,
	NULL,
	pq2pci_unmask_irq,
	pq2pci_mask_irq,
	pq2pci_mask_and_ack,
	pq2pci_end_irq,
	0
};

static irqreturn_t
pq2pci_irq_demux(int irq, void *dev_id, struct pt_regs *regs)
{
	unsigned long stat, mask, pend;
	int bit;

	for(;;) {
		stat = *(volatile unsigned long *) PCI_INT_STAT_REG;
		mask = *(volatile unsigned long *) PCI_INT_MASK_REG;
		pend = stat & ~mask & 0xf0000000;
		if (!pend)
			break;
		for (bit = 0; pend != 0; ++bit, pend <<= 1) {
			if (pend & 0x80000000)
				__do_IRQ(NR_CPM_INTS + bit, regs);
		}
	}

	return IRQ_HANDLED;
}

static struct irqaction pq2pci_irqaction = {
	.handler = pq2pci_irq_demux,
	.flags 	 = SA_INTERRUPT,
	.mask	 = CPU_MASK_NONE,
	.name	 = "PQ2 PCI cascade",
};


void
pq2ptin_pci_init_irq(void)
{
	int irq;
	volatile cpm2_map_t *immap = cpm2_immr;

	for (irq = NR_CPM_INTS; irq < NR_CPM_INTS + 4; irq++)
		irq_desc[irq].handler = &pq2ptin_pci_ic;

	// make PCI IRQ level sensitive 
	immap->im_intctl.ic_siexr &=
		~(1 << (14 - (PCI_INT_TO_SIU - SIU_INT_IRQ6)));

	// install the demultiplexer for the PCI cascade interrupt
	setup_irq(PCI_INT_TO_SIU, &pq2pci_irqaction);
	return;
}
*/


static int
pq2pci_exclude_device(u_char bus, u_char devfn)
{
	return PCIBIOS_SUCCESSFUL;
}

/* PCI bus configuration registers.
 */
/**/
static void
pq2ptin_setup_pci(struct pci_controller *hose)
{
	__u32 val;
	volatile cpm2_map_t *immap = cpm2_immr;
	bd_t* binfo = (bd_t*) __res;
	u32 sccr = immap->im_clkrst.car_sccr;
	uint pci_div,freq,time;
		/* PCI int lowest prio */
	/* Each 4 bits is a device bus request	and the MS 4bits
	 is highest priority */
	/* Bus                4bit value
	   ---                ----------
	   CPM high      	0b0000
	   CPM middle           0b0001
	   CPM low       	0b0010
	   PCI reguest          0b0011
	   Reserved      	0b0100
	   Reserved      	0b0101
	   Internal Core     	0b0110
	   External Master 1 	0b0111
	   External Master 2 	0b1000
	   External Master 3 	0b1001
	   The rest are reserved
	 */
	immap->im_siu_conf.siu_82xx.sc_ppc_alrh = 0x61207893;
	/* park bus on core */
	immap->im_siu_conf.siu_82xx.sc_ppc_acr = PPC_ACR_BUS_PARK_CORE;
	/*
	 * Set up master windows that allow the CPU to access PCI space. These
	 * windows are set up using the two SIU PCIBR registers.
	 */

	immap->im_memctl.memc_pcimsk0 = M82xx_PCI_PRIM_WND_SIZE;
	immap->im_memctl.memc_pcibr0  = M82xx_PCI_PRIM_WND_BASE | PCIBR_ENABLE;

#ifdef M82xx_PCI_SEC_WND_SIZE
	immap->im_memctl.memc_pcimsk1 = M82xx_PCI_SEC_WND_SIZE;
	immap->im_memctl.memc_pcibr1  = M82xx_PCI_SEC_WND_BASE | PCIBR_ENABLE;
#endif

	/*
	 * Setting required to enable IRQ1-IRQ7 (SIUMCR [DPPC]),
	 * and local bus for PCI (SIUMCR [LBPC]).
	 */
    immap->im_siu_conf.siu_82xx.sc_siumcr =
        (immap->im_siu_conf.siu_82xx.sc_siumcr & 
         ~(SIUMCR_LBPC11 | SIUMCR_DPPC11))
        | SIUMCR_LBPC01;


	/* Enable PCI  */
	immap->im_pci.pci_gcr = cpu_to_le32(PCIGCR_PCI_BUS_EN);

	pci_div = ( (sccr & SCCR_PCI_MODCK) ? 2 : 1) *
			( ( (sccr & SCCR_PCIDF_MSK) >> SCCR_PCIDF_SHIFT) + 1);
	freq = (uint)((2*binfo->bi_cpmfreq)/(pci_div));
	time = (int)666666/freq;
	/* due to PCI Local Bus spec, some devices needs to wait such a long
	time after RST 	deassertion. More specifically, 0.508s for 66MHz & twice more for 33 */
	printk("%s: The PCI bus is %d Mhz.\nWaiting %s after deasserting RST...\n",__FILE__,freq,
	(time==1) ? "0.5 seconds":"1 second" );

	{
		int i;
		for(i=0;i<(500*time);i++)
			udelay(1000);
	}

    //Coloca o valor do registo interruptline = 24 //IRQ6
    PCI_CfgWriteByte(0,10,e_InterruptLine,24);

	/* setup ATU registers */
	immap->im_pci.pci_pocmr0 = cpu_to_le32(POCMR_ENABLE | POCMR_PCI_IO |
				((~(M82xx_PCI_IO_SIZE - 1U)) >> POTA_ADDR_SHIFT));
	immap->im_pci.pci_potar0 = cpu_to_le32(M82xx_PCI_LOWER_IO >> POTA_ADDR_SHIFT);
	immap->im_pci.pci_pobar0 = cpu_to_le32(M82xx_PCI_IO_BASE >> POTA_ADDR_SHIFT);

	/* Set-up non-prefetchable window */
	immap->im_pci.pci_pocmr1 = cpu_to_le32(POCMR_ENABLE | ((~(M82xx_PCI_MMIO_SIZE-1U)) >> POTA_ADDR_SHIFT));
	immap->im_pci.pci_potar1 = cpu_to_le32(M82xx_PCI_LOWER_MMIO >> POTA_ADDR_SHIFT);
	immap->im_pci.pci_pobar1 = cpu_to_le32((M82xx_PCI_LOWER_MMIO - M82xx_PCI_MMIO_OFFSET) >> POTA_ADDR_SHIFT);

	/* Set-up prefetchable window */
	immap->im_pci.pci_pocmr2 = cpu_to_le32(POCMR_ENABLE |POCMR_PREFETCH_EN |
		(~(M82xx_PCI_MEM_SIZE-1U) >> POTA_ADDR_SHIFT));
	immap->im_pci.pci_potar2 = cpu_to_le32(M82xx_PCI_LOWER_MEM >> POTA_ADDR_SHIFT);
	immap->im_pci.pci_pobar2 = cpu_to_le32((M82xx_PCI_LOWER_MEM - M82xx_PCI_MEM_OFFSET) >> POTA_ADDR_SHIFT);

 	/* Inbound transactions from PCI memory space */
	immap->im_pci.pci_picmr0 = cpu_to_le32(PICMR_ENABLE | PICMR_PREFETCH_EN |
					((~(M82xx_PCI_SLAVE_MEM_SIZE-1U)) >> PITA_ADDR_SHIFT));
	immap->im_pci.pci_pibar0 = cpu_to_le32(M82xx_PCI_SLAVE_MEM_BUS  >> PITA_ADDR_SHIFT);
	immap->im_pci.pci_pitar0 = cpu_to_le32(M82xx_PCI_SLAVE_MEM_LOCAL>> PITA_ADDR_SHIFT);

    immap->im_siu_conf.siu_82xx.sc_ppc_alrh = 0x30124567;                        //Consultado no uboot

	/* park bus on PCI */
	immap->im_siu_conf.siu_82xx.sc_ppc_acr = PPC_ACR_BUS_PARK_PCI;

	/* Enable bus mastering and inbound memory transactions */
    val=PCI_CfgReadLong(0,10,e_PciCommand);
    val &= 0xffff0000;

    val |= PCI_COMMAND_MEMORY|PCI_COMMAND_MASTER;
    PCI_CfgWriteLong(0,10,e_PciCommand,val);
}

/*

int pci_bus_read_config_byte (struct pci_bus *bus, unsigned int devfn, int where, u8 *val);
int pci_bus_read_config_word (struct pci_bus *bus, unsigned int devfn, int where, u16 *val);
int pci_bus_read_config_dword (struct pci_bus *bus, unsigned int devfn, int where, u32 *val);
int pci_bus_write_config_byte (struct pci_bus *bus, unsigned int devfn, int where, u8 val);
int pci_bus_write_config_word (struct pci_bus *bus, unsigned int devfn, int where, u16 val);
int pci_bus_write_config_dword (struct pci_bus *bus, unsigned int devfn, int where, u32 val);

PCI_OP_READ(byte, u8, 1)
PCI_OP_READ(word, u16, 2)
PCI_OP_READ(dword, u32, 4)
PCI_OP_WRITE(byte, u8, 1)
PCI_OP_WRITE(word, u16, 2)
PCI_OP_WRITE(dword, u32, 4)
*/

void __init pq2ptin_find_bridges(void)
{
	extern int pci_assign_all_busses;
	struct pci_controller * hose;
	int host_bridge;

	pci_assign_all_busses = 1;

//#ifdef CONFIG_PQ2_PTIN
//    hose = &hose_clemos;
//#else
	hose = pcibios_alloc_controller(); //clemos
//#endif
    
	if (!hose)
		return;

	ppc_md.pci_swizzle = common_swizzle;

	hose->first_busno = 0;
	hose->bus_offset = 0;
	hose->last_busno = 0xff;

#ifdef CONFIG_ADS8272
	hose->set_cfg_type = 1;
#endif

	setup_m8260_indirect_pci(hose,
				 (unsigned long)&cpm2_immr->im_pci.pci_cfg_addr,
				 (unsigned long)&cpm2_immr->im_pci.pci_cfg_data);

	/* Make sure it is a supported bridge */
    host_bridge=PCI_CfgReadLong(0,PCI_DEVFN(0,0),e_VendorId);
	switch (host_bridge) {
        case PCI_DEVICE_ID_MPC8265:
            printk("Host bridge: MPC8265 from MOTOROLA\n");
			break;
        case PCI_DEVICE_ID_MPC8272:
            printk("Host bridge: MPC8272 from MOTOROLA\n");
			break;

		default:
			printk("PCI (pq2ptin_find_bridges): Attempting to use unrecognized host bridge ID"
				" 0x%08x.\n", host_bridge);
			break;
	}

	pq2ptin_setup_pci(hose);

	hose->io_space.start =	M82xx_PCI_LOWER_IO;
	hose->io_space.end = M82xx_PCI_UPPER_IO;
	hose->mem_space.start = M82xx_PCI_LOWER_MEM;
	hose->mem_space.end = M82xx_PCI_UPPER_MMIO;
	hose->pci_mem_offset = M82xx_PCI_MEM_OFFSET;

	isa_io_base =
	(unsigned long) ioremap(M82xx_PCI_IO_BASE,
					M82xx_PCI_IO_SIZE);
	hose->io_base_virt = (void *) isa_io_base;


	/* setup resources */
	pci_init_resource(&hose->mem_resources[0],
			M82xx_PCI_LOWER_MEM,
			M82xx_PCI_UPPER_MEM,
			IORESOURCE_MEM|IORESOURCE_PREFETCH, "PCI prefetchable memory");

	pci_init_resource(&hose->mem_resources[1],
			M82xx_PCI_LOWER_MMIO,
			M82xx_PCI_UPPER_MMIO,
			IORESOURCE_MEM, "PCI memory");

	pci_init_resource(&hose->io_resource,
			M82xx_PCI_LOWER_IO,
			M82xx_PCI_UPPER_IO,
			IORESOURCE_IO | 1, "PCI I/O");


	ppc_md.pci_exclude_device = pq2pci_exclude_device;

    hose->last_busno = pq2ptin_pciauto_bus_scan(hose, hose->first_busno);
    
    hose->ops->write = pq2ptin_write_pci;
    hose->ops->read = pq2ptin_read_pci;

	ppc_md.pci_map_irq = pq2ptin_pci_map_irq;
	ppc_md.pcibios_fixup = NULL;
	ppc_md.pcibios_fixup_bus = NULL;

    PCI_Init_apos();
    return;
}
