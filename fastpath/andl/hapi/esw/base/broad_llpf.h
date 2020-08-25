/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2009
*
**********************************************************************
*
* @filename  broad_llpf.h
*
* @purpose   This file contains the hapi signatures for LLPF parameters
*
* @component hapi
*
* @comments
*
* @create    10/15/2009
*
* @author    Vijayanand K(kvijayan)
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_BROAD_LLPF_H
#define INCLUDE_BROAD_LLPF_H

#include "l7_common.h"
#include "llpf_exports.h"

/* Used to check if particular block type is set */
#ifdef L7_LLPF_PACKAGE
#define LLPF_PORT_BLOCK_PROTO_ISSET( __hapiPtr,  __blockType)  \
    ((( __hapiPtr->llpfBlockProtoEnable) & (1 <<__blockType)) ? L7_TRUE : L7_FALSE)
#else
#define LLPF_PORT_BLOCK_PROTO_ISSET( __hapiPtr,  __blockType)  L7_FALSE
#endif

#define ISDP_ETHERTYPE   0x2000
#define VTP_ETHERTYPE    0x2003
#define DTP_ETHERTYPE    0x2004
#define UDLD_ETHERTYPE   0x0111
#define PAGP_ETHERTYPE   0x0104
#define SSTP_ETHERTYPE   0x010B

/* TODO:max policies */
#define HAPI_BROAD_LLPF_MAX_POLICIES L7_LLPF_BLOCK_TYPE_LAST

#define BROAD_LLPF_DEBUG
#ifdef  BROAD_LLPF_DEBUG
#define HAPI_BROAD_LLPF_DEBUG(fmt, args...)         {                                                 \
                                                    if(broadLlpfDebug)                           \
                                                    { sysapiPrintf("\n%s [%d]: ", __FUNCTION__,__LINE__); \
                                                      sysapiPrintf(fmt, ##args);                          \
                                                    }                                               \
                                                  }
#else
#define HAPI_BROAD_LLPF_DEBUG(fmt, args...)
#endif

typedef  struct BROAD_POLICY_SNAP_HEADER_s
{
  L7_uint32 org_code;
  L7_ushort16 type;
}BROAD_POLICY_SNAP_HEADER_t;

/*********************************************************************
*
* @purpose Take semaphore to protect LLPF resources
*
* @param  None
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfSemTake();

/*********************************************************************
*
* @purpose Give semaphore to protect LLPF resources
*
* @param
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfSemGive();

/*********************************************************************
*
* @purpose Hooks in basic dapi cmds
*
* @param   DAPI_PORT_t *dapiPortPtr - used to hook in commands for a port
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfPortInit(DAPI_PORT_t *dapiPortPtr);
/*********************************************************************
 *
 * @purpose Install routine to install system port polices 
 *                                                   for LLPF component
 *
 * @param   DAPI_t           *dapi_g  @b{(input)} system information.
 *
 * @returns L7_RC_t     result
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadLlpfPolicyInstall(DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose Routine to remove LLPF polices 
 *
 * @param   DAPI_t           *dapi_g  @b{(input)} system information.
 *
 * @returns L7_RC_t     result
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadLlpfPolicyRemove(DAPI_t *dapi_g);

/*********************************************************************
 *
 * @purpose To Enable/Disable LLPF block mode on a port.
 *
 * @param   DAPI_USP_t *usp    - needs to be a valid usp
 * @param   DAPI_CMD_t  cmd    - DAPI_CMD_
 * @param   void       *data   - DAPI_INTF_MGMT_CMD_t.cmdData.
 * @param   DAPI_t     *dapi_g - the driver object
 *
 * @returns L7_RC_t result
 *
 * @notes
 *
 * @end
 *
 *********************************************************************/
L7_RC_t hapiBroadIntfLlpfBlockModeConfig(DAPI_USP_t *usp, DAPI_CMD_t cmd,
    void *data, DAPI_t *dapi_g);

/*********************************************************************
*     
* @purpose Notifies the Llpf component that a physical port has been
*          added to a LAG port
*     
* @param   *portUsp @b{(input)} system information for a physical port.
* @param   *lagUsp  @b{(input)} system information for a PORT CHANNEL.
* @param   *dapi_g  @b{(input)} system information.
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfLagAddNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Notifies the Llpf component that a physical port has been
*          removed from a LAG port
*
* @param  *portUsp @b{(input)} system information for a physical port.
* @param  *lagUsp  @b{(input)} system information for a PORT CHANNEL.
* @param  *dapi_g  @b{(input)} system information.
*
* @returns L7_RC_t     result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadLlpfLagDeleteNotify(DAPI_USP_t *portUsp, DAPI_USP_t *lagUsp, DAPI_t *dapi_g);



#endif /* End of BROAD_LLPF_H */

