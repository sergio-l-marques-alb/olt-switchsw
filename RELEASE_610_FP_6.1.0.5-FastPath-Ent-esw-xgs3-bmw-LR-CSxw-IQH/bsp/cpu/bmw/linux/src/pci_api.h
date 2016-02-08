//||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
//                                                                      //
//              ||||||\   ||||||||   ||||||   ||    ||                  //
//              ||   ||      ||        ||     ||||  ||                  //
//              |||||||      ||        ||     || || ||                  //
//              ||           ||        ||     ||  ||||                  //
//              ||           ||        ||     ||   |||                  //
//              ||           ||      ||||||   ||    ||                  //
//                                                                      //
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// FileName:                                                            //
//            qualidade_G806.h                                          //
//                                                                      //
//----------------------------------------------------------------------//
// Projecto:                                                            //
//            PR710_SDH_RA                                              //
//                                                                      //
//----------------------------------------------------------------------//
// Descrição: PCI API functions                                         //
//                                                                      //
//----------------------------------------------------------------------//
//                                                                      //
// Histórico:                                                           //
//            13 Novembro 2003 - primeira versão                        //
//                                                                      //
//----------------------------------------------------------------------//
// Autor: Celso Lemos                                                   //
//////////////////////////////////////////////////////////////////////////
#ifndef __PCI_API_H__
#define __PCI_API_H__

#include "globals.h"

extern void PCI_MaskNoResponse();
extern UINT32 PCI_ReadLong (UINT32 address);
extern void PCI_WriteLong (UINT32 address, UINT32 value); 
extern void PCI_CfgWriteLong(UINT32 BusNum, UINT32 DevNum, e_CfgReg RegNum, UINT32 value);
extern void PCI_CfgWriteShort (UINT32 BusNum,UINT32 DevNum,e_CfgReg configuration_register_offset,UINT16 value); 
extern void PCI_CfgWriteByte (UINT32 BusNum,UINT32 DevNum, e_CfgReg configuration_register_offset,UINT16 value);
extern UINT32 PCI_CfgReadLong(UINT32 BusNum, UINT32 DevNum, e_CfgReg RegNum);
extern UINT32 PCI_CalcWindowSize(UINT32);
extern void  PCI_InsertRelevantBitsInPciRegister(UINT32 register_address,  UINT32 new_val, UINT32 mask);
extern UINT32 ConstructConfigWord (UINT32 BusNum,UINT32 DevNum,e_CfgReg RegNum);

#endif //__PCI_API_H__
