/**************************************************************** 
 *
 * File:  	pci.c
 *
 * 
 * Description: This file contains all the necessary routines to 
 * initialize the PCI bridge of MB and AI, and finally do the DMA
 * transfer from MB to AI
 *
 ****************************************************************/
#include "globals.h"
#include "immap_cpm2.h"
#include <string.h>
#include <stdlib.h>        
#include <stdio.h>        
#include "pci.h"        
#include "pci_api.h"        
#include "addrmap.h"        
//#include "BCM5680x.h"        

#define PCI_BASE_ADDR 0xAFFF0000 //0x80000000

/*
* pointers to memory map of host and agent
*/
extern cpm2_map_t *PPC8280RegPtr;//ponteiro para os registos do processador


void PCI_Configure_Access_big_endian(void)
{
  volatile UINT32 *pci_addr;
  TAddrMap AddrMap2;

  pci_addr = (volatile UINT32 *) AddrAlloc(&AddrMap2, (unsigned int) (PCI_BASE_ADDR), (long) 0x10000);

  pci_addr[0x174/4] = 0x05050505;  

  AddrFree(&AddrMap2);
}




UINT32 regsIngress[4] = {
  0x180C01,
  0x01180600,
  0x01180603,
  0x01180203
};

UINT32 regsEgress[4] = {
  0x480600,
  0x480601,
  0x480600,
  0x480601
};


/*
void PCI_teste_clemos(UINT8 OPCODE, UINT8 DPORT, UINT8 LENGTH, UINT32 addr)
{
  //int j;
  volatile UINT32 *pci_addr,i;
  TAddrMap AddrMap2;
  UINT32 SPORT; //block, Port, offset;
  UINT32 aux,aux1; //block, Port, offset;

  //OPCODE = 0xB;
  //DPORT  = 0x1; 
  SPORT  = DPORT; 
  //LENGTH = 4;

  pci_addr[0x400/4] = 1;
  pci_addr[0x580/4] = 0xffffffff;
  pci_addr[0x10C/4] = 0x43;  
  aux = pci_addr[0x10C/4];
  printf("val[0x10C] = 0x%.8lx\n\r",pci_addr[0x10C/4]);
  pci_addr[0x10C/4] = aux | 0x80;


  printf("val[0x10C] = 0x%.8lx\n\r",pci_addr[0x10C/4]);
  pci_addr[0x10C/4] = aux;  
  printf("val[0x10C] = 0x%.8lx\n\r",pci_addr[0x10C/4]);

  usleep(10);

  aux = ((OPCODE & 0x3F) << 26) | 
                 ((DPORT & 0x3F)  << 20) | 
                 ((SPORT & 0x3F)  << 14) | 
                 ((LENGTH & 0x3F)  << 7);

  printf("aux[0] = 0x%.2lx\n\r",aux); 

  pci_addr[0x00] = aux;  
  pci_addr[0x01] = addr; 
  //pci_addr[0x01] = 0xF180600; 

  pci_addr[0x50/4] = 0x80;  

  for (i=0;i<10;i++) {
    aux1 = pci_addr[0x50/4];
    if ((aux1 & 2)!=0) {
      break;
    }
    printf("ctrl(%ld) = 0x%.8lx\n\r", i, pci_addr[0x50/4]);    
  }
  printf("ctrl(%ld) = 0x%.8lx\n\r", i, pci_addr[0x50/4]);    
  printf("Header: 0x%.8lx, val1 =0x%.8lx val2 =0x%.8lx\n\r",pci_addr[0],pci_addr[1],pci_addr[2]);
  pci_addr[0x50/4] = 0x01;  
  //usleep(1);
}

*/


/*
void PCI_teste_clemos64(UINT8 OPCODE, UINT8 DPORT, UINT8 LENGTH, UINT32 addr)
{
  //int j;
  volatile UINT32 *pci_addr;
  TAddrMap AddrMap2;

  pci_addr = (volatile UINT32 *) AddrAlloc(&AddrMap2, (unsigned int) (PCI_BASE_ADDR), (long) 0x10000);

  printf("Read = 0x%.16llx\n\r",BCM5680x_Read_Indirect_Reg64((T_BCM5680x_CMIC *)pci_addr, ((DPORT & 0x3F)  << 20), addr));

  AddrFree(&AddrMap2);
}
*/




void PCI_teste_clemos_wr(UINT8 OPCODE, UINT8 DPORT, UINT8 LENGTH, UINT32 addr, UINT32 val)
{
  //int j;
  volatile UINT32 *pci_addr,i;
  TAddrMap AddrMap2;
  UINT32 SPORT; //block, Port, offset;
  UINT32 aux,aux1; //block, Port, offset;


  pci_addr = (volatile UINT32 *) AddrAlloc(&AddrMap2, (unsigned int) (PCI_BASE_ADDR), (long) 0x10000);


  //OPCODE = 0xB;
  //DPORT  = 0xC; 
  SPORT  = DPORT; 
  //LENGTH = 4;

  //pci_addr[0x400/4] = 1;
  //pci_addr[0x580/4] = 0xffffffff;
  pci_addr[0x10C/4] = 0x43;  
  aux = pci_addr[0x10C/4];
  printf("val[0x10C] = 0x%.8lx\n\r",pci_addr[0x10C/4]);
  pci_addr[0x10C/4] = aux | 0x80;


  printf("val[0x10C] = 0x%.8lx\n\r",pci_addr[0x10C/4]);
  pci_addr[0x10C/4] = aux;  
  printf("val[0x10C] = 0x%.8lx\n\r",pci_addr[0x10C/4]);

  usleep(10);

  aux = ((OPCODE & 0x3F) << 26) | 
                 ((DPORT & 0x3F)  << 20) | 
                 ((SPORT & 0x3F)  << 14) | 
                 ((LENGTH & 0x3F)  << 7);

  printf("aux[0] = 0x%.2lx\n\r",aux); 

  pci_addr[0x00] = aux;  
  pci_addr[0x01] = addr; 
  pci_addr[0x02] = val; 
  //pci_addr[0x01] = 0xF180600; 

  pci_addr[0x50/4] = 0x80;  

  for (i=0;i<10;i++) {
    aux1 = pci_addr[0x50/4];
    if ((aux1 & 2)!=0) {
      break;
    }
    printf("ctrl(%ld) = 0x%.8lx\n\r", i, pci_addr[0x50/4]);    
  }
  printf("ctrl(%ld) = 0x%.8lx\n\r", i, pci_addr[0x50/4]);    
  printf("Header: 0x%.8lx, val =0x%.8lx\n\r",pci_addr[0],pci_addr[1]);
  pci_addr[0x50/4] = 0x01;  
  //usleep(1);
}







/********************************************************
 * routine:     PCI_Init
 *
 * description:
 * this routine calls all other subroutines in order to 
 * do the PCI DMA data movement. It initializes the PCI bridge
 * of MB and AI, sets up all the inbound/outbound windows 
 * on MB and AI, then depending on the definition selected by
 * the user performs PCI DMA either in direct mode or in 
 * chaining mode.
 *
 * arguments:
 *      none
 *      
 * return code:
 *      none
 *
 ********************************************************/
void PCI_Teste ()
{
  UINT32 ConfigData;

  printf("\n\r\n\r");
  printf("+++++++++++++++++++++++++++++++++++++++++\n\r");
  printf("OLT_8CH: Init PCI device\n\r");
  printf("+++++++++++++++++++++++++++++++++++++++++\n\r");

/*  //garante que o device pci sai de reset
  // ---------------------------------------------------------------------
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_gcr, 0);  
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_gcr, SOFT_PCI_RESET);  

  sleep(1);

  //Vai configurar a latencia=0xF8 e a cache line size = 0x08 do device presente no bus PCI
  // ---------------------------------------------------------------------
//  PCI_CfgWriteByte(0,10,e_CacheLine,0x08);
//  PCI_CfgWriteByte(0,10,e_LatencyTimer,0xC0);

  PCI_CfgWriteByte(0,10,e_InterruptLine,0x06);

  //Configura Base de enderecamento para o device PCI.
  //Como no processador o Bus pci est mapeado no addr=0x80000000 e a outbound window est mapeada tambm em 0x80000000, 
  // o addr que selecciona o device e' o 0x80000000
  // ---------------------------------------------------------------------
  PCI_CfgWriteLong(0,10,e_BaseAddrLow,PCI_BASE_ADDR);

  //configura o device para responder a comandos de leitura de memoria atravs do bus pci e indica o chip como sendo bus master
  // ---------------------------------------------------------------------
  PCI_CfgWriteLong(0,10,e_PciCommand,0x00000006);

  //configura o device para permitir acessos big endian
  // ---------------------------------------------------------------------
  //PCI_Configure_Access_big_endian();
*/
  // ---------------------------------------------------------------------
  //Le os parametros dos registos de configuracao (Configuration mapping
  // ---------------------------------------------------------------------

  // Vendor Id e Device ID
  ConfigData = PCI_CfgReadLong(0,10,0x00);
  printf("+   - DEVICE ID           = 0x%.4x      +\n\r",(unsigned int) ((ConfigData>>16) & 0xFFFF));
  printf("+   - VENDOR ID           = 0x%.4x      +\n\r",(unsigned int) (ConfigData & 0xFFFF));
                       
  //Command / Status
  ConfigData = PCI_CfgReadLong(0,10,0x04);
  printf("+   - COMMAND/STATUS      = 0x%.8x  +\n\r",(unsigned int) ConfigData);

  //Class Code / Revision ID
  ConfigData = PCI_CfgReadLong(0,10,0x08);
  printf("+   - CLASS CODE          = 0x%.6x    +\n\r",(unsigned int) ((ConfigData>>8) & 0xFFFFFF));
  printf("+   - REVISION ID         = 0x%.4x      +\n\r",(unsigned int) (ConfigData & 0xFF));


  //Self Teste / Header Type / Latency Timer / Cache Size
  ConfigData = PCI_CfgReadLong(0,10,0x0C);
  printf("+   - SELF TEST           = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>24) & 0xFF));
  printf("+   - HEADER TYPE         = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>16) & 0xFF));
  printf("+   - LATENCY TIMER       = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>8) & 0xFF));
  printf("+   - CACHE LINE SIZE     = 0x%.2x        +\n\r",(unsigned int) (ConfigData & 0xFF));


  //Base Address Low
  ConfigData = PCI_CfgReadLong(0,10,0x10);
  printf("+   - BASE ADRESS LOW     = 0x%.8x  +\n\r",(unsigned int) ConfigData);

  //Base Address High
  ConfigData = PCI_CfgReadLong(0,10,0x14);
  printf("+   - BASE ADRESS HIGH    = 0x%.8x  +\n\r",(unsigned int) ConfigData);

  //SubSystem ID / Subsystem vendor Id
  ConfigData = PCI_CfgReadLong(0,10,0x2C);
  printf("+   - SUBSYSTEM ID        = 0x%.4x      +\n\r",(unsigned int) ((ConfigData>>16) & 0xFFFF));
  printf("+   - SUBSYSTEM VENDOR ID = 0x%.4x      +\n\r",(unsigned int) (ConfigData & 0xFFFF));

  //Max Latency / Min Grant / interrupt pin / interrupt line
  ConfigData = PCI_CfgReadLong(0,10,0x3C);
  printf("+   - MAX LATENCY         = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>24) & 0xFF));
  printf("+   - MIN GRANT           = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>16) & 0xFF));
  printf("+   - INTERRUPT PIN       = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>8) & 0xFF));
  printf("+   - INTERRUPT LINE      = 0x%.2x        +\n\r",(unsigned int) (ConfigData & 0xFF));

  //trdy timeout / retry count
  ConfigData = PCI_CfgReadLong(0,10,0x40);
  printf("+   - TRDY TIMEOUT        = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>8) & 0xFF));
  printf("+   - RETRY COUNT         = 0x%.2x        +\n\r",(unsigned int) (ConfigData & 0xFF));

  printf("+++++++++++++++++++++++++++++++++++++++++\n\r");
}





void 
PCI_Init ()
{
  UINT32 ConfigData;

  printf( "+++++++++++++++++++++++++++++++++++++++++\n\r"
          "+ OLT_8CH: Init PCI device              +\n\r"
          "+++++++++++++++++++++++++++++++++++++++++\n\r"
        );

  //garante que o device pci sai de reset
  // ---------------------------------------------------------------------
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_gcr, 0);  
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_gcr, SOFT_PCI_RESET);  

  sleep(1);

//  //Vai configurar a latencia=0xF8 e a cache line size = 0x08 do device presente no bus PCI
//  // ---------------------------------------------------------------------
  PCI_CfgWriteByte(0,10,e_CacheLine,0x08);
  PCI_CfgWriteByte(0,10,e_LatencyTimer,0xF8);

  PCI_CfgWriteByte(0,10,e_InterruptLine,0x06);

  //Configura Base de enderecamento para o device PCI.
  //Como no processador o Bus pci est mapeado no addr=0x80000000 e a outbound window est mapeada tambm em 0x80000000,
  // o addr que selecciona o device e' o 0x80000000
  // ---------------------------------------------------------------------
  PCI_CfgWriteLong(0,10,e_BaseAddrLow,PCI_BASE_ADDR);

  //configura o device para responder a comandos de leitura de memoria atravs do bus pci e indica o chip como sendo bus master
  // ---------------------------------------------------------------------
  PCI_CfgWriteLong(0,10,e_PciCommand,0x00000006);

  //configura o device para permitir acessos big endian
  // ---------------------------------------------------------------------
  //PCI_Configure_Access_big_endian();

  // ---------------------------------------------------------------------
  //Le os parametros dos registos de configuracao (Configuration mapping
  // ---------------------------------------------------------------------

  // Vendor Id e Device ID
  ConfigData = PCI_CfgReadLong(0,10,0x00);
  printf("+   - DEVICE ID           = 0x%.4x      +\n\r",(unsigned int) ((ConfigData>>16) & 0xFFFF));
  printf("+   - VENDOR ID           = 0x%.4x      +\n\r",(unsigned int) (ConfigData & 0xFFFF));

  //Command / Status
  ConfigData = PCI_CfgReadLong(0,10,0x04);
  printf("+   - COMMAND/STATUS      = 0x%.8x  +\n\r",(unsigned int) ConfigData);

  //Class Code / Revision ID
  ConfigData = PCI_CfgReadLong(0,10,0x08);
  printf("+   - CLASS CODE          = 0x%.6x    +\n\r",(unsigned int) ((ConfigData>>8) & 0xFFFFFF));
  printf("+   - REVISION ID         = 0x%.4x      +\n\r",(unsigned int) (ConfigData & 0xFF));


  //Self Teste / Header Type / Latency Timer / Cache Size
  ConfigData = PCI_CfgReadLong(0,10,0x0C);
  printf("+   - SELF TEST           = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>24) & 0xFF));
  printf("+   - HEADER TYPE         = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>16) & 0xFF));
  printf("+   - LATENCY TIMER       = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>8) & 0xFF));
  printf("+   - CACHE LINE SIZE     = 0x%.2x        +\n\r",(unsigned int) (ConfigData & 0xFF));


  //Base Address Low
  ConfigData = PCI_CfgReadLong(0,10,0x10);
  printf("+   - BASE ADRESS LOW     = 0x%.8x  +\n\r",(unsigned int) ConfigData);

  if ( ConfigData != 0xafff0000 ) {
    printf("PCI faillure! Reset the board.\n\r");
    exit(0);
  }

  //Base Address High
  ConfigData = PCI_CfgReadLong(0,10,0x14);
  printf("+   - BASE ADRESS HIGH    = 0x%.8x  +\n\r",(unsigned int) ConfigData);

  //SubSystem ID / Subsystem vendor Id
  ConfigData = PCI_CfgReadLong(0,10,0x2C);
  printf("+   - SUBSYSTEM ID        = 0x%.4x      +\n\r",(unsigned int) ((ConfigData>>16) & 0xFFFF));
  printf("+   - SUBSYSTEM VENDOR ID = 0x%.4x      +\n\r",(unsigned int) (ConfigData & 0xFFFF));

  //Max Latency / Min Grant / interrupt pin / interrupt line
  ConfigData = PCI_CfgReadLong(0,10,0x3C);
  printf("+   - MAX LATENCY         = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>24) & 0xFF));
  printf("+   - MIN GRANT           = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>16) & 0xFF));
  printf("+   - INTERRUPT PIN       = 0x%.2x        +\n\r",(unsigned int) ((ConfigData>>8) & 0xFF));
  printf("+   - INTERRUPT LINE      = 0x%.2x        +\n\r",(unsigned int) (ConfigData & 0xFF));

  //trdy timeout / retry count
  ConfigData = PCI_CfgReadLong(0,10,0x40);
  printf("+   - TRDY TIMEOUT        = 0x%.2x        +\n\r",(unsigned int) (unsigned int) ((ConfigData>>8) & 0xFF));
  printf("+   - RETRY COUNT         = 0x%.2x        +\n\r",(unsigned int) (unsigned int) (ConfigData & 0xFF));

  printf("+++++++++++++++++++++++++++++++++++++++++\n\r");
}

