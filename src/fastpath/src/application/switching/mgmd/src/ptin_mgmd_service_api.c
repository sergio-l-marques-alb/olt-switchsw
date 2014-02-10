/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @filename  ptin_mgmd_service_api.c
*
* @purpose   The purpose of this file is to have the a central location for
* @purpose   all mgmdMap includes and definitions.
*
* @component mgmdMap Mapping Layer
*
* @comments  none
*
* @create    23/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
* @end
*
**********************************************************************/

#ifdef _COMPILE_AS_BINARY_ //All methods in this file should not be compiled if we are compiling as a lib

#include "ptin_mgmd_service_api.h"
#include "ptin_mgmd_logger.h"
#include "snooping_ptin_defs.h"

#include <string.h>

/**
 * Get port list associated to the given serviceID
 * 
 * @param serviceId[in]   : Service Identifier
 * @param portType[in]    : Port Type [Root - 0 ; Leaf - 1] 
 * @param portList[out]   : Bitmap of Ports with size MAX_INTERFACES
 * 
 * @return RC_t 
 *  
 * @notes: none 
 */
RC_t ptin_mgmd_port_getList(uint32 serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList)
{
  _UNUSED_(serviceId);
  _UNUSED_(portType);
  memset(portList, 0x00, PTIN_MGMD_PORT_MASK_INDICES * sizeof(unsigned char));
  return SUCCESS; 
}
 
/**
 * Get port type associated to the given serviceID
 * 
 * @param serviceId              : Service Identifier
 * @param portId                 : Port Identifier
 * @param portType               : Port Type [Root - 2 ; Leaf 1]
 *  
 * @return RC_t
 *  
 * @notes: none
 */
RC_t ptin_mgmd_port_getType(uint32 serviceId, uint32 portId, ptin_mgmd_port_type_t *portType)
{
  _UNUSED_(serviceId);
  _UNUSED_(portId);
  *portType = PTIN_MGMD_PORT_TYPE_LEAF;
  return SUCCESS;
}

/**
 * Get client list associated to the given portID/serviceId
 * 
 * @param serviceId  : Service Identifier
 * @param portId     : Port Identifier
 * @param clientList : Client bitmap (with size PTIN_MGMD_CLIENT_BITMAP_SIZE) 
 * 
 * @return RC_t 
 *  
 * @notes: none 
 */
RC_t ptin_mgmd_client_getList(uint32 serviceId, uint32 portId, uint8 *clientList)
{
  _UNUSED_(serviceId);
  _UNUSED_(portId);
  memset(clientList, 0x00, PTIN_MGMD_CLIENT_BITMAP_SIZE * sizeof(uint8));
  return SUCCESS; 
}
 
/**
 *  Open Port Id for a given Multicast IP Address and Source IP
 *  Address.
 * 
 *  @param serviceId            : Service Identifier
 *  @param groupAddr            : Multicast Group IP Address
 *  @param sourceAddr           : Unicast Source IP Address
 *  @param portId               : Port Identifier
 *  @param isStatic             : Static Group 
 *
 *  @return RC_t
 *  
 *  @notes: If the Source Address is equal to zero. Then it is
 *        considered to be any source
 */
RC_t ptin_mgmd_port_open(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr, BOOL isStatic)
{
  _UNUSED_(serviceId);
  _UNUSED_(portId);
  _UNUSED_(groupAddr);
  _UNUSED_(sourceAddr);
  _UNUSED_(isStatic);
  return SUCCESS;
}
 
/**
 *  Close Port Id for a given Multicast IP Address and Source IP
 *  Address.
 * 
 *  @param serviceId            : Service Identifier
 *  @param groupAddr            : Multicast Group IP Address
 *  @param sourceAddr           : Unicast Source IP Address
 *  @param portId               : Port Identifier
 *
 *  @return RC_t
 *  
 *  @notes: If the Source Address is equal to zero. Then it is
 *        considered to be any source
 */
RC_t ptin_mgmd_port_close(uint32 serviceId, uint32 portId, uint32 groupAddr, uint32 sourceAddr)
{
  _UNUSED_(serviceId);
  _UNUSED_(portId);
  _UNUSED_(groupAddr);
  _UNUSED_(sourceAddr);
  return SUCCESS;
}

/**
* @purpose Send IGMP/MLD packet
*
* @param   payload       : Packet payload with max size of [MAX_FRAME_SIZE]
* @param   payloadLength : Packet payload length
* @param   serviceId     : Service Identifier
* @param   portId        : Port Identifier
* @param   clientId      : Client Identifier 
* @param   family        : IP Address Family
*
* @return RC_t
*
* @notes none
*/
RC_t ptin_mgmd_tx_packet(uchar8 *payLoad, uint32 payloadLength, uint32 serviceId, uint32 portId, uint32 clientId, uchar8 family) 
{
  _UNUSED_(payLoad);
  _UNUSED_(payloadLength);
  _UNUSED_(serviceId);
  _UNUSED_(portId);
  _UNUSED_(clientId);
  _UNUSED_(family);
  return SUCCESS; 
}

#endif //_COMPILE_AS_BINARY_
