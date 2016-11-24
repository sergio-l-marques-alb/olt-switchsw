/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename dapi_db.h
*
* @purpose dapi component's internal data types and functions
*
* @component dapi
*
* @comments none
*
* @create 12/04/2003
*
* @author smanders
* @end
*
**********************************************************************/

/*************************************************************

*************************************************************/
#ifndef INCLUDE_DAPI_DB
#define INCLUDE_DAPI_DB

#include "l7_common.h"
#include "dapi.h"
#include "sysapi_hpc.h"

/*******************************************************************************
*
* @structures HAPI_CARD_SLOT_MAP_t
*
* @purpose    The slot map information
*
* @end
*
*******************************************************************************/
typedef struct
{
  L7_ulong32 slotNum;
  L7_ulong32 portNum;
  L7_int32   bcm_cpuunit;
  L7_int32   bcm_port;

} HAPI_CARD_SLOT_MAP_t ;

/*******************************************************************************
*
* @structures HAPI_CARD_PORT_MAP_t
*
* @purpose    Info for the ports
*
* @end
*
*******************************************************************************/
typedef struct
{
  L7_ulong32 portNum;

  /* tempstack Should be removed */
  /* This info will be provided from Card Manager Structure */

} HAPI_CARD_PORT_MAP_t ;

/*******************************************************************************
*
* @structures DAPI_CARD_ENTRY_t
*
* @purpose    The main information database for cards.  It is populated during
*             the cardDbInit
*
* @end
*
*******************************************************************************/

/* function pointer to initializers to allocate and initialize storage for the dapiCardInfo entries *
typedef L7_RC_t (*DAPI_CARD_INIT_t)(SYSAPI_HPC_CARD_DESCRIPTOR_t *cardDesc);
*/

typedef L7_RC_t (*HAPI_BASE_CARD_INIT_t)(L7_ulong32 cardTypeID, void *handle);
typedef L7_RC_t (*HAPI_CARD_FUNC_t)(DAPI_USP_t *usp, DAPI_CMD_t cmd, void *data, void *handle);

typedef struct
{
  /*
   * hapi init routines
   */
  HAPI_CARD_FUNC_t        hapiCardInsert;
  HAPI_CARD_FUNC_t        hapiCardRemove;

  HAPI_CARD_SLOT_MAP_t   *slotMap;
  L7_ushort16             numOfSlotMapEntries;

  HAPI_CARD_PORT_MAP_t   *portMap;
  L7_ushort16             numOfPortMapEntries;

  /*
   * This is the number of Physical Ports plus the CPU Port
   */
  L7_ushort16             numOfNps;
  void                   *npInfo;
  L7_ushort16             npInfoSlot;

} DAPI_CARD_ENTRY_t;

/*******************************************************************************
*
* @structures DAPI_BASE_CARD_ENTRY_t
*
* @purpose    The main information database for the base card
*
* @end
*
*******************************************************************************/
typedef struct
{
  /*
   * hapi init routines
   */
  HAPI_BASE_CARD_INIT_t   hapiBaseCardInsert;

} DAPI_BASE_CARD_ENTRY_t;

L7_RC_t dapiBroadCardInfoInit(SYSAPI_HPC_CARD_DESCRIPTOR_t *cardDesc);

#endif  /* INCLUDE_DAPI_DB */
