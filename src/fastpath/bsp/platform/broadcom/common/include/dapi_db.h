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

#include "ptin_globaldefs.h"  /* PTin added */

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


#if (PTIN_BOARD == PTIN_BOARD_CXO640G)

#define WC_MAX_NUMBER          18
#define WC_MAX_LANES            4
#define WC_MAX_GROUPS           4

#define WC_GROUP0_MAX_BW      164
#define WC_GROUP_MAX_BW       160

#define WC_SEGMENT_N_GROUPS     2
#define WC_SEGMENT_MAX_PORTS   32


#define WC_SLOT_MODE_NONE   0
#define WC_SLOT_MODE_2x10G  1
#define WC_SLOT_MODE_4x10G  2
#define WC_SLOT_MODE_1x40G  3
#define WC_SLOT_MODE_2x40G  4
#define WC_SLOT_MODE_3x40G  5
#define WC_SLOT_MODE_1x10G  6
#define WC_SLOT_MODE_3x10G  7
#define WC_SLOT_MODE_1x1G   8
#define WC_SLOT_MODE_2x1G   9
#define WC_SLOT_MODE_3x1G   10
#define WC_SLOT_MODE_4x1G   11
#define WC_SLOT_MODE_MAX    12

/*******************************************************************************
*
* @structures HAPI_WC_SLOT_MAP_t / HAPI_WC_PORT_MAP_t
*
* @purpose    The warpcores slot map information
*
* @end
*
*******************************************************************************/
typedef struct
{
  L7_ulong32 wcIndex;
  L7_ulong32 wcGroup;
  L7_uint8   invert_lanes;
  L7_uint8   invert_polarities;
  L7_long32  slotIdx;

} HAPI_WC_SLOT_MAP_t;

typedef struct
{
  L7_ulong32 portNum;
  L7_long32  slotNum;
  L7_ulong32 wcIdx;
  L7_ulong32 wcLane;
  L7_ulong32 wcSpeedG;

} HAPI_WC_PORT_MAP_t;
#endif

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

  #if (PTIN_BOARD == PTIN_BOARD_CXO640G)
  L7_uint32              *wcSlotMode;
  L7_ushort16             numOfWCSlotModeEntries;

  HAPI_WC_PORT_MAP_t     *wcPortMap;
  L7_ushort16             numOfWCPortMapEntries;
  #endif

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

/* PTin added */
#if (PTIN_BOARD == PTIN_BOARD_CXO640G)
/**
 * Build a WC map from the array of port modes
 * 
 * @param slot_mode : Slot modes
 * @param retMap    : Map to be returned
 * 
 * @return L7_RC_t : L7_SUCCESS - Valid map 
 *                   L7_NOT_SUPPORTED - Map not valid
 *                   L7_FAILURE - Error processing file
 */
L7_RC_t hpcConfigWCmap_build(L7_uint32 *slot_mode, HAPI_WC_PORT_MAP_t *retMap);
#endif

#endif  /* INCLUDE_DAPI_DB */
