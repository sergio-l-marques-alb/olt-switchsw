/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename  broad_debug.h
*
* @purpose   This files contains the debug function prototypes.
*
* @component hapi
*
* @comments
*
* @create    12/20/2001
*
* @author    mbaucom
*
* @end
*
**********************************************************************/

#include "l7_common.h"
#include "dapi.h"
#include "dapi_struct.h"

// PTin added
// Semaphore for shell structure
extern void *sem_shell_access;

typedef enum
{
  L7_BROAD_DEBUG_PBMP_PORT_ALL =  0,
  L7_BROAD_DEBUG_PBMP_FE_ALL   = -1,
  L7_BROAD_DEBUG_PBMP_GE_ALL   = -2,
  L7_BROAD_DEBUG_PBMP_XE_ALL   = -3,
  L7_BROAD_DEBUG_PBMP_HG_ALL   = -4,
  L7_BROAD_DEBUG_PBMP_HL_ALL   = -5, 
  L7_BROAD_DEBUG_PBMP_ST_ALL   = -6, 
  L7_BROAD_DEBUG_PBMP_GX_ALL   = -7,
  L7_BROAD_DEBUG_PBMP_XG_ALL   = -8,
  L7_BROAD_DEBUG_PBMP_E_ALL    = -9,
  L7_BROAD_DEBUG_PBMP_CMIC     = -10,
  L7_BROAD_DEBUG_PBMP_SPI      = -11
} L7_BROAD_DEBUG_PBMP_TYPE_t;


/*********************************************************************
*
* @purpose To do any debug setup required and get debug routines linked
*
* @param   none
*
* @returns void
*
* @notes   none
*
* @end
*
*********************************************************************/
void hapiBroadDebugInit(DAPI_t *dapi_g);

/*********************************************************************
*
* @purpose Prints out some general information for each unit, slot and port
*          in the current configuration
*
* @param   none
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugDDPrint();

/*********************************************************************
*
* @purpose Set the frame print flag
*
* @param   L7_ushort16 unitNum - the desired unit number
* @param   L7_ushort16 slotNum - the desired slot number
* @param   L7_ushort16 portNum - the desired port number
* @param   L7_BOOL flag    - indicator whether or not to display the frames
* @param   L7_uint32   time    - length in which it is desired to print frames
*
* @returns L7_RC_t result
*
* @notes   The time is not in any set units. Use experimentation to find a useful 
*          length.  If flag is set to true (or 1) then any frames sent to the 
*          specified usp will be displayed.  You can call the function again if
*          want it to stop before the time has expired but remember to set flag
*          to false (or 0).
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugFrameShow(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum, L7_BOOL flag, L7_uint32 time);

/*********************************************************************
*
* @purpose
*
* @param   L7_ushort16 i
* @param   L7_ushort16 p
*
* @returns L7_SUCCESS
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugIPAddressAdd(L7_ushort16 i,L7_ushort16 p);

/*********************************************************************
*
* @purpose Manually configure a port to be down
*
* @param   L7_ushort16 unitNum      - the desired unit number
* @param   L7_ushort16 slotNum      - the desired slot number
* @param   L7_ushort16 portNum      - the desired port number
* @param   L7_BOOL entireSystem - if true(or 1), all ports will be 
*                                     configured to be down
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugLinkDown(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum, L7_BOOL entireSystem);

/*********************************************************************
*
* @purpose Manually configures a port to be up
*
* @param   L7_ushort16 unitNum      - the desired unit number
* @param   L7_ushort16 slotNum      - the desired slot number
* @param   L7_ushort16 portNum      - the desired port number
* @param   L7_BOOL entireSystem - if set to true (or 1) then all
*                                     ports will be configured to be up
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugLinkUp(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum, L7_BOOL entireSystem);

/*********************************************************************
*
* @purpose Prints out the stats for the requested usp if it is up
*
* @param   L7_ushort16 unitNum - needs to be a valid unit number
* @param   L7_ushort16 slotNum - needs to be a valid slot number
* @param   L7_ushort16 portNum - needs to be a valid port number
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugStatsGet(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum);


/*********************************************************************
*
* @purpose Just a way to adjust the stats for a usp for some variety
*
* @param   L7_ushort16 unitNum - should be a valid unit number
* @param   L7_ushort16 slotNum - should be a valid slot number
* @param   L7_ushort16 portNum - should be a valid port number
*
* @returns L7_RC_t result
*
* @notes   none
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugStatsAlt(L7_ushort16 unitNum, L7_ushort16 slotNum, L7_ushort16 portNum);


/*********************************************************************
*
* @purpose Initialize some various stats for the ports
*
* @param   none
*
* @returns L7_RC_t result
*
* @notes   This is an easy way to vary the status of ports.  Some are 
*          configure to be up.  Different speeds are set also.
*
* @end
*
*********************************************************************/
L7_RC_t hapiBroadDebugStatsInit();

/*********************************************************************
* @purpose  Execute commands on a driver shell
*
* @param    
*
* @returns 
*
* @notes
*
* @end
*********************************************************************/

L7_RC_t hapiBroadDebugShell(void *data);


/*********************************************************************
* @purpose  Pass-thru function to invoke the Broadcom SDK diagnostic shell
*
* @param    void
*
* @returns  void
*
* @notes    This is here to provide the BSP with a way to start the
*           vendor SDK diag_shell() without making a direct reference.
*                                 
* @end
*********************************************************************/
void hapiBroadDebugDiagShell(void);


