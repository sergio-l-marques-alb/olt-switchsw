
/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2007
 *
 **********************************************************************
 *
 * @filename nvstoreapi.h
 *
 * @purpose Global Save and Restore Manager API
 *
 * @component nvStore
 *
 * @comments none
 *
 * @create 08/03/2000
 *
 * @author bmutz
 * @end
 *
 **********************************************************************/

#ifndef INCLUDE_NVSTORE_API
#define INCLUDE_NVSTORE_API

#include "file_exports.h"

typedef struct
{
  L7_COMPONENT_IDS_t registrar_ID;
#if 0
  L7_uint32 registrar_ID; 
#endif
  L7_RC_t (*notifySave)(void);        /* save routine */
  L7_BOOL (*hasDataChanged)(void);    /* has data changed routine */
  L7_RC_t (*notifyConfigDump)(void);  /* dump config file routine */
  L7_RC_t (*notifyDebugDump)(void);   /* dump debug routine */
  void (*resetDataChanged)(void); /* Reset the data cahnged flag */

} nvStoreFunctionList_t;

/****************************************
 *
 *  NVSTORE Functions                   
 *
 *****************************************/

L7_RC_t nvStoreRegister(nvStoreFunctionList_t notifyFunctionList);
L7_RC_t nvStoreTxtCfgRegister(void (*notifySave)( L7_char8 *filename ));

/*********************************************************************
 * @purpose  Deregister a component from nvStore
 *
 * @param    componentId     one of L7_COMPONENT_IDS_t
 *
 * @returns  L7_RC_t  L7_SUCCESS or L7_FALIURE
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_RC_t nvStoreDeregister( L7_COMPONENT_IDS_t  componentId);
L7_RC_t nvStoreSave(L7_SAVE_CONFIG_TYPE_t generateConfig);
L7_RC_t nvStoreComponentSave(L7_COMPONENT_IDS_t componentId);
L7_RC_t nvStoreComponentSaveAll (void);
L7_BOOL nvStoreHasDataChanged(void);
void nvStoreResetDataChanged(void);
L7_BOOL nvStoreHasDataChangedSinceLastRunCfg(void);
void nvStoreResetCompDataChanged(L7_COMPONENT_IDS_t componentId);
L7_RC_t nvStoreConfigDump(void);
L7_RC_t nvStoreDebugDump(void);
L7_uint32 nvStoreCrc32( L7_uchar8 * blk_adr, L7_uint32 blk_len );
L7_RC_t nvStoreGetFile(L7_char8 *fileName, L7_char8 *buffer, L7_uint32 bufferSize, L7_uint32 *checkSum,
    L7_uint32 version, void (*defaultBuild)(L7_uint32));

/*********************************************************************
* @purpose  Erase startup-config file
*
* @returns  L7_SUCCESS
* @returns  L7_FAILURE
*
* @notes    none
*
* @end
*********************************************************************/
L7_RC_t nvStoreEraseStartupConfig(void);

/*********************************************************************
 * @purpose  Set starting value for the cumulative CRC computation.
 *
 * @returns  crc32Value
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrcInit (void);

/*********************************************************************
 * @purpose  Update cumulative CRC.
 *
 * @param    old_crc  Previous CRC value.
 * @param    octet    New data byte.
 *
 * @returns  New CRC value.
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrcUpdate (L7_uint32 old_crc, L7_uchar8 octet);


/*********************************************************************
 * @purpose  Finish cumulative CRC computation.
 *
 * @param    old_crc  Previous CRC value.
 *
 * @returns  Final CRC for the data block.
 *
 * @notes 
 *                                 
 * @end
 *********************************************************************/
L7_uint32 nvStoreCrcFinish (L7_uint32 old_crc);

/*********************************************************************
* @purpose  Check if a component's user config data has changed
*
* @param    componentId  @((input)) id of component to check
*
* @returns  L7_BOOL  L7_TRUE or L7_FALSE
*
* @notes
*
* @end
*********************************************************************/
L7_BOOL nvStoreHasComponentDataChanged( L7_uint32 componentId );


#endif
