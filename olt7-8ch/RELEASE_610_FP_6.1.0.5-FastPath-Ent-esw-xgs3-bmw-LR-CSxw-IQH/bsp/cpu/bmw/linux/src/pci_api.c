/**************************************************************** 
 *
 * File:  pci.c	
 *
 * 
 * Description: PCI API routines.
 *		
 * 
 ****************************************************************/

#include "immap_cpm2.h"
#include <string.h>
#include <stdlib.h> 
#include "pci.h"               
#include "pci_api.h"


extern cpm2_map_t *PPC8280RegPtr;//ponteiro para os registos do processador

/*
* PCI write routines pertaining to memory and config space
*/
//void PCI_WriteLong (UINT32 address, UINT32 value); 
//void PCI_CfgWriteLong(UINT32 BusNum, UINT32 DevNum, e_CfgReg RegNum, UINT32 value);
//void PCI_CfgWriteShort (UINT32 BusNum,UINT32 DevNum, e_CfgReg configuration_register_offset,UINT16 value); 
//void PCI_CfgWriteByte (UINT32 BusNum,UINT32 DevNum, e_CfgReg configuration_register_offset,UINT16 value);

/*
* PCI read routines pertaining to memory and config space
*/
UINT32 PCI_ReadLong (UINT32 address);
UINT32 PCI_CfgReadLong(UINT32 BusNum, UINT32 DevNum, e_CfgReg RegNum);

/*
* Utility routines 
*/
void SwapLong(unsigned long *val);
UINT32 PCI_CalcWindowSize(UINT32);
void PCI_InsertRelevantBitsInPciRegister(UINT32 register_address,  UINT32 new_val, UINT32 mask);
UINT32 PCI_InsertRelevantBits( UINT32 old, UINT32 new_val, UINT32 mask);
void PCI_MaskNoResponse();
void PCI_UnmaskNoResponse();

/****************************************************************
 *  Routine:        PCI_MaskNoResponse
 *
 *  Description:    Masks the no response error.This prevents from 
 *  generating a Machine Check when the PCI bridge acts as a master of 
 *  PCI bus, initiates a transaction and finally gets a master abort
 *  because none of the devices on the PCI claims that transaction. 
 * Clears  PCI_NO_RSP bit of the Error Mask Register.
 *
 *  Arguments:  
 *    part          pointer to PCI internal map
 *
 *  Return value:   None. 
 *	
 *
 ******************************************************************/
void PCI_MaskNoResponse() 
{
  UINT32 error_mask;
  error_mask = ((I2O_DBMC|NMI|IRA|I2O_IPQO|I2O_OFQO|PCI_PERR_WR|               
               PCI_PERR_RD|PCI_SERR|PCI_TAR_ABT|                
               PCI_DATA_PAR_RD|PCI_DATA_PAR_WR|PCI_ADDR_PAR)&(~PCI_NO_RSP));  
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_emr, error_mask);
}


/****************************************************************
 *  Routine:        PCI_UnmaskNoResponse
 *
 *  Description:    Unmasks the no response error.Because of unmasking 
 *  a Machine Check is generated when the PCI bridge acts as a master of 
 *  PCI bus, initiates a transaction and finally gets a master abort
 *  because none of the devices on the PCI claims that transaction. 
 *  Sets  PCI_NO_RSP bit of the Error Mask Register.
 *
 *  Arguments:  
 *    part          pointer to PCI internal map
 *
 *  Return value:   None. 
 *	
 *
 ******************************************************************/
void PCI_UnmaskNoResponse()
{
 UINT32 error_mask, error_status;
 error_mask = ((I2O_DBMC|NMI|IRA|I2O_IPQO|I2O_OFQO|PCI_PERR_WR|               
               PCI_PERR_RD|PCI_SERR|PCI_TAR_ABT|                
               PCI_DATA_PAR_RD|PCI_DATA_PAR_WR|PCI_ADDR_PAR|PCI_NO_RSP));  
  error_status = PCI_ReadLong((UINT32)&PPC8280RegPtr->im_pci.pci_esr);
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_esr, PCI_NO_RSP);
  PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_emr, error_mask);
}




/****************************************************************
 *  Routine:   
 *      PCI_CfgWriteLong.                   
 *
 *  Description: 
 *      Swaps a long to little endian and writes it to the PCI 
 *      configuration space 
 *
 *  Arguments:
 *	  BusNum: single PCI bus therefore the value is 0.This arguement
 *            is provided in case there are multiple pci buses in 
 *            the system 
 *    DevNum:each slot on the MB has its own device number
 *    configuration_register_offset: Configuration register address.
 *    pPCIMap:pointer to internal PCI map
 *    value:Value to  write.
 *
 *  Return value: None.
 *	
 *****************************************************************/
void PCI_CfgWriteLong(UINT32 BusNum, UINT32 DevNum, e_CfgReg configuration_register_offset, UINT32 value)
{
     UINT32 Config_Addr;     

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
    PPC8280RegPtr->im_pci.pci_cfg_addr = Config_Addr; /*write*/    
    
    /*
    * write value to  cfgd
    */
    PCI_WriteLong((UINT32)&PPC8280RegPtr->im_pci.pci_cfg_data, value);
}


/****************************************************************
 *  Routine:   
 *      PCI_CfgWriteLong.                   
 *
 *  Description: 
 *  Reads a long from the PCI configuration space.
 *
 *  Arguments:
 *	  BusNum: single PCI bus therefore the value is 0.This arguement
 *            is provided in case there are multiple pci buses in 
 *            the system 
 *    DevNum:each slot on the MB has its own device number
 *    RegNum: Configuration register address.
 *    pPCIMap:pointer to internal PCI map
 *
 *  Return value: long value from the configuration register
 *	
 *****************************************************************/
UINT32 PCI_CfgReadLong(UINT32 BusNum, UINT32 DevNum, e_CfgReg RegNum)
{
     UINT32 Config_Addr;
     UINT32 temp;
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
    PPC8280RegPtr->im_pci.pci_cfg_addr = Config_Addr; /*write*/    
    
    /*
    * read cfgd
    */
    temp = GET_LONG(*(volatile UINT32 *)&PPC8280RegPtr->im_pci.pci_cfg_data);
    
    /*
    * byte swap the value
    */
    SwapLong(&temp);
    return temp; 
}


/****************************************************************
 *  Routine:   
 *      ConstructConfigWord                   
 *
 *  Description: 
 *  construct appropriate value for the configuration address reg
 *
 *  Arguments:
 *	  BusNum: single PCI bus therefore the value is 0.This arguement
 *            is provided in case there are multiple pci buses in 
 *            the system 
 *    DevNum:each slot on the MB has its own device number
 *    RegNum: Configuration register address.
 *
 *  Return value: a long value which can be assigned to config addr reg
 *	
 *****************************************************************/
UINT32 ConstructConfigWord (UINT32 BusNum,UINT32 DevNum,e_CfgReg RegNum)
{
  UINT32 value;
  UINT32 FuncNum=0x0;
  value = (
          (BusNum <<16)    | 
          (DevNum <<11)    | 
          (FuncNum <<8)    | 
          (RegNum & (~3) ) |
          0x80000000
          );
  return value;
  
}



/****************************************************************
 *  Routine:  PCI_WriteLong.      
 *
 *  Description: Swapes endian and writes longs endian.
 *	
 *
 *  Arguments:
 *	address   Address to be written. 
 *  value     Value to be read or written to the address.
 *
 *  Return value: 
 *      None.
 *	
 *
 ****************************************************************/
void PCI_WriteLong (UINT32 address, UINT32 value) 
{
  	SwapLong(&value);
  	WRITE_LONG(*(UINT32 *) address, value);
}

/****************************************************************
 *  Routine:  PCI_ReadLong.      
 *
 *  Description: Reads longs and changes their endian.
 *	
 *
 *  Arguments:
 *  	address to be read. 
 *
 *
 *  Return value: 
 *      Value in the address swaped.
 *	
 *
 ****************************************************************/
UINT32 PCI_ReadLong (UINT32 address)
{
  	UINT32 pci_value;
  	pci_value = GET_LONG(*(volatile UINT32 *) address);
  	SwapLong(&pci_value);	
  	return pci_value;
}


/****************************************************************
 *  Routine:   SwapLong  
 *
 *  Description:
 *      this routine swap a long from big endian to little endian
 *
 *  Arguments:
 *  long *val - the address of the long to swap
 *
 *  Return value:
 *  void
 *
 ****************************************************************/
void SwapLong(unsigned long *val)
{
    *val = ((*val & 0xFF) << 24) | ((*val & 0xFF00) << 8) |
        ((*val & 0xFF0000UL) >> 8) | ((*val & 0xFF000000UL) >> 24);
}

/****************************************************************
 *  Routine:  PCI_CalcWindowSize      
 *
 *  Description: Calculates a memory window size from its mask.
 *	
 *
 *  Arguments: 
 *    cmr     The mask of a memory window..
 *	
 *
 *  Return value: Size.
 *	
 *
 ****************************************************************/
UINT32 PCI_CalcWindowSize(UINT32 cmr)
{
  return 1+ (~(cmr & ACTUAL_BASE));
}


/****************************************************************
 *  Routine:   

 *      PCI_CfgWriteShort.                   
 *
 *  Description: 
 *	Swaps a short to little endian and writes it to the PCI configuration space.
 *
 *  Arguments:
 *	  BusNum                                 This value is 0 since single PCI bus;defined
 *                                           mainly for future is if there are multiple PCI
 *                                           bus.
 *    DevNum                                 Each slot in the MB will have a fixed Dev # 
 *    pci_slot                               The bus/device/function coordinates of the slot.
 *    configuration_register_offset          Configuration register address.
 *    value                                  Value to write.
 *
 *  Return value: None.
 *	
 *
 ****************************************************************/
void PCI_CfgWriteShort (UINT32 BusNum,UINT32 DevNum,	
			e_CfgReg configuration_register_offset,
			UINT16 value) 
{
 
  	UINT32 configuration_data, configuration_address;
    
	configuration_address = ConstructConfigWord(BusNum,DevNum,configuration_register_offset);
  	PCI_WriteLong ((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_addr), configuration_address);

  	configuration_data = PCI_ReadLong((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_data));
  	*(((UINT16 *)&configuration_data) + (1 - (configuration_register_offset & 1))) = value;

  	PCI_WriteLong ((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_addr), configuration_address);
  	PCI_WriteLong((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_data), configuration_data);
}

/****************************************************************
 *  Routine:   
 *     PCI_CfgWriteByte.                   
 *
 *  Description: 
 *	Writes  a byte to the PCI configuration space. 
 *
 *  Arguments:
 *	  BusNum                              This value is 0 since single PCI bus;defined
 *                                        mainly for future is if there are multiple PCI
 *                                        bus.
 *    DevNum                              Each slot in the MB will have a fixed Dev # 
 
 *    configuration_register_offset       Configuration register address.
 *    value.                              Value to write.
 *
 *  Return value: None.
 *	
 *
 ****************************************************************/
void PCI_CfgWriteByte (UINT32 BusNum,UINT32 DevNum, e_CfgReg configuration_register_offset,UINT16 value)
{
  UINT32 configuration_data, configuration_address;
    
  configuration_address = ConstructConfigWord(BusNum,DevNum,configuration_register_offset);
  PCI_WriteLong ((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_addr), configuration_address);

  configuration_data = PCI_ReadLong((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_data));
  *(((UINT8 *)&configuration_data) + (3 - (configuration_register_offset & 3))) = value;
  PCI_WriteLong ((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_addr), configuration_address);
  PCI_WriteLong((UINT32) &(PPC8280RegPtr->im_pci.pci_cfg_data), configuration_data);
}

/****************************************************************
 *  Routine:   PCI_InsertRelevantBitsInPciRegister     
 *
 *  Description: Uses PCI_InsertRelevantBits to modify a PCI register.
 *
 *  Arguments:  
 *    register_address  Address of a PCI register.
 *    new_val           Value to put in the register.
 *    mask              Which registers not to touch.
 *
 *  Return value:       None
 *	
 *
 ******************************************************************/
 void  PCI_InsertRelevantBitsInPciRegister(UINT32 register_address,  UINT32 new_val, UINT32 mask)
{
  UINT32 old_val;
  old_val = PCI_ReadLong(register_address);
  new_val = PCI_InsertRelevantBits(old_val, new_val, mask);
  PCI_WriteLong(register_address, new_val);
}

/****************************************************************
 *  Routine:        PCI_InsertRelevantBits
 *
 *  Description:    Inserts the bits of new_val that are not within
 *                  mask, into old.
 *
 *  Arguments:  
 *    old           Old value.
 *    new_val       New value.
 *    mask          Which bits to leave as is.
 *
 *  Return value:   Result of the computation.
 *	
 *
 ******************************************************************/
UINT32 PCI_InsertRelevantBits( UINT32 old, UINT32 new_val, UINT32 mask)
{ 
  return ((old & mask) | (new_val & (~mask)));
}
