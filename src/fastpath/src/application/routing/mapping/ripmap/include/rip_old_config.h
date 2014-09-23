
/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename   rip_config.h
*
* @purpose    RIP Mapping layer structures and config data
*
* @component  RIP Mapping Layer
*
* @comments   none
*
* @create     05/21/2001
*
* @author     gpaussa
* @end
*
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#ifndef _INCLUDE_RIP_OLD_CONFIG_H_
#define _INCLUDE_RIP_OLD_CONFIG_H_

#include "l7_common.h"
#include "nimapi.h"


/****************************************
*
*  RIP Global Configuration Data                
*
*****************************************/

typedef struct ripCfgDataV2_s
{
  L7_uint32           ripAdminMode;             /* Enable RIP                 */
} ripCfgDataV2_t;     


/****************************************
*
*  RIP Circuit Configuration Data         
*
*****************************************/

typedef struct ripCfgCktV2_s
{
  L7_uint32           adminMode;                /* Enable RIP for interface   */
  L7_uint32           authType;                 /* L7_AUTH_TYPES_t            */
  L7_char8            authKey[L7_AUTH_MAX_KEY_RIP]; /* authentication key     */
  L7_uint32           authKeyLen;               /* original length of auth key*/
  L7_uint32           verSend;                  /* RIP version to send        */
  L7_uint32           verRecv;                  /* RIP version to receive     */
  L7_uint32           defaultMetric;            /* default route metric       */

} ripCfgCktV2_t;


/****************************************
*
*  RIP Configuration File                
*
*****************************************/

typedef struct L7_ripMapCfgV2_s 
{
  L7_fileHdr_t        cfgHdr;
  ripCfgDataV2_t      rtr;
  ripCfgCktV2_t       ckt[L7_MAX_SLOTS_PER_UNIT][L7_MAX_PORTS_PER_SLOT+1];
  L7_uint32           checkSum;

} L7_ripMapCfgV2_t;

#endif /* _INCLUDE_RIP_OLD_CONFIG_H_ */
