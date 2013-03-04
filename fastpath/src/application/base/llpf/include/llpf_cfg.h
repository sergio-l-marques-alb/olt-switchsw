/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename llpf_cfg.h
*
* @purpose Contains prototypes and Data Structures
*          to support the Link Local Protocol Filtering application
*
* @component LLPF
*
* @comments
*
* @create 10/05/2009
*
* @author vijayanand K
* @end
*
**********************************************************************/
#ifndef INCLUDE_LLPF_CFG_H
#define INCLUDE_LLPF_CFG_H


#include "nimapi.h"
#include "comm_mask.h"
#include "l7_common.h"
#include "llpf_api.h"

typedef struct
{
  L7_fileHdr_t   cfgHdr;

  /* ISDP Config Info for each port*/
  L7_INTF_MASK_t blockIsdp;  
  
  /* VTP Config Info for each port*/
  L7_INTF_MASK_t blockVtp; 
  
  /* DTP Config Info for each port*/
  L7_INTF_MASK_t blockDtp; 
  
  /* UDLD Config Info for each port*/
  L7_INTF_MASK_t blockUdld; 
  
  /* PAGP Config Info for each port*/
  L7_INTF_MASK_t blockPagp; 
  
  /* SSTP Config Info for each port*/
  L7_INTF_MASK_t blockSstp; 
  
  /* Config Info for all blocking protocols for each port*/
  L7_INTF_MASK_t blockAll; 
    
  /* check sum of config file NOTE: needs to be last entry */
  L7_uint32      checkSum;    

} llpfCfgData_t;



#define LLPF_CFG_FILENAME      "llpf.cfg"
#define LLPF_CFG_VER_1        0x1
#define LLPF_CFG_VER_CURRENT  LLPF_CFG_VER_1

#endif  /* end of INCLUDE_CFG_H */
