/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_iscsi.h
*
* @purpose   This file contains all the routines for iSCSI
*
* @component hapi
*
* @comments
*
* @create    6/23/08
*
* @end
*
**********************************************************************/
#ifndef INCLUDE_BROAD_ISCSI_H
#define INCLUDE_BROAD_ISCSI_H

typedef enum
{
    ISCSI_DEBUG_NONE,
    ISCSI_DEBUG_LOW,
    ISCSI_DEBUG_MED,
    ISCSI_DEBUG_HIGH,
}
BROAD_ISCSI_DEBUG_LEVEL_t;

BROAD_ISCSI_DEBUG_LEVEL_t hapiBroadIscsiDebugLevel();

/*********************************************************************
*
* @purpose  Initialize iSCSI component of QOS package
*
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments  
*
* @end
*
*********************************************************************/
extern L7_RC_t hapiBroadQosIscsiInit (DAPI_t * dapiPtr);

/*********************************************************************
*
* @purpose  Initialize iSCSI component of QOS package
*
* @param   *dapi_g          @b{(input)} The driver object    
*
* @returns  L7_SUCCESS    Function executed normally
* @returns  L7_FAILURE    Function failed
*
* @comments  
*
* @end
*
*********************************************************************/
extern L7_RC_t hapiBroadQosIscsiPortInit (DAPI_PORT_t * dapiPortPtr);


/*********************************************************************
*
* @purpose  Returns whether the iSCSI component is actively monitoring
*           at least one target TCP port.
*
* @param    none 
*
* @returns  L7_TRUE       iSCSI configured to monitor a TCP port
* @returns  L7_FALSE      iSCSI not monitoring any TCP ports
*
* @comments  
*
* @end
*
*********************************************************************/
L7_BOOL hapiBroadQosIscsiActiveStatusGet(void);

#endif
