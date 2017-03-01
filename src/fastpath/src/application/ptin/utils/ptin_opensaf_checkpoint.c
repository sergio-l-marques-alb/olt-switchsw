/**
 * ptin_opensaf.c 
 *  
 * Implements the opensaf interface module
 *
 * Created on: 2016/07/04 Author: Rui Fernandes(rui-f-fernandes@alticealbs.com) 
 * Notes: 
 *
 */
#ifndef _PTIN_OPENSAF_CHECKPOINT_H
#define _PTIN_OPENSAF_CHECKPOINT_H


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include "ptin_opensaf_checkpoint.h"
#include "ptin_utils.h"

#if (PTIN_BOARD != PTIN_BOARD_TG16G && PTIN_BOARD != PTIN_BOARD_IS_STANDALONE )
#include <saCkpt.h>

typedef struct 
{
  SaCkptHandleT ckptHandle;
  SaCkptCheckpointHandleT checkpointHandle;
  SaNameT ckptName;  
  SaCkptCheckpointCreationAttributesT ckptCreateAttr;
  SaCkptCheckpointOpenFlagsT ckptOpenFlags;
  L7_BOOL  initialized;
  L7_BOOL  ignoreSectionNotExistDebug;
  L7_int32 errorCount;
  L7_char8 ckptNameStr[32];
  pthread_mutex_t ckptMux; //to be used to guaranty atomicity in macro functions
  L7_uint64 bmp_section[255];

} ptin_opensaf_checkpoint_t; 

ptin_opensaf_checkpoint_t ptin_checkpoint[PTIN_MAX_OPENSAF_CHECKPOINTS];

#if (PTIN_BOARD != PTIN_BOARD_OLT1T0)

static L7_RC_t ptin_opensaf_checkpoint_init(const char *name, int sectionsNum, int sectionLen, int id);
static L7_RC_t ptin_opensaf_readOnly_checkpoint_init(const char *name, int id);
static L7_RC_t ptin_opensaf_checkpoint_deinit(int id);
static L7_RC_t ptin_opensaf_checkCheckpointInitialization(int id);

#endif

/********************************Start PTin Opensaf Checkpoint*********************************************************/
/**
 * Initialize a opensaf checkpoint handler
 * 
 * @param name of the handler, number of section and length of 
 *             each section
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_RC_t ptin_opensaf_init()
{
  memset(&ptin_checkpoint, 0, sizeof(ptin_checkpoint));

  char *name ="MAC";

  if(ptin_opensaf_checkpoint_init(name, 256*32 , 60, 1) != 0)
  {
    PT_LOG_NOTICE(LOG_CTX_OPENSAF," Some error occour in the initialition of opensaf checkpoint ");
  }

  return L7_SUCCESS;
}

/**
 * Initialize a opensaf checkpoint handler
 * 
 * @param name of the handler, number of section and length of 
 *             each section
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
static L7_RC_t ptin_opensaf_checkpoint_init(const char *name, int sectionsNum, int sectionLen, int id)
{

   L7_char8         namedn[128];
   L7_uint32        namelen;
   SaVersionT	    version;
   L7_uint32		saRet,i;
   SaTimeT timeout = 10000000000ll;


   if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
   {
     PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %d", id);
     return L7_FAILURE;
   }

   if (ptin_checkpoint[id].initialized == L7_TRUE)
   {
     PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d is already in use %d", id);
     return L7_FAILURE;
   }

   SaCkptCheckpointCreationAttributesT *pCkptCreateAttr;

   ptin_checkpoint[id].errorCount = 0;
   pthread_mutex_init(&ptin_checkpoint[id].ckptMux, NULL);

   snprintf(ptin_checkpoint[id].ckptNameStr, sizeof(ptin_checkpoint[id].ckptNameStr), "%s", name);
   namelen = snprintf(namedn, sizeof(namedn), "safCkpt=%s,safApp=safCkptService", name);

   bzero(&ptin_checkpoint[id].ckptName, sizeof(ptin_checkpoint[id].ckptName));
   memcpy(ptin_checkpoint[id].ckptName.value, namedn, namelen);
   ptin_checkpoint[id].ckptName.length = namelen;

    ptin_checkpoint[id].ckptCreateAttr.creationFlags =
//#ifdef NONCOLLOCATED
                                    SA_CKPT_WR_ALL_REPLICAS;
//#else
                                   //SA_CKPT_WR_ACTIVE_REPLICA|SA_CKPT_CHECKPOINT_COLLOCATED;
//#endif
    ptin_checkpoint[id].ckptCreateAttr.checkpointSize = sectionsNum * sectionLen;
    ptin_checkpoint[id].ckptCreateAttr.retentionDuration = 2592000000000000ll; //2592000 seconds
    ptin_checkpoint[id].ckptCreateAttr.maxSections = sectionsNum;
    ptin_checkpoint[id].ckptCreateAttr.maxSectionSize = sectionLen;
    ptin_checkpoint[id].ckptCreateAttr.maxSectionIdSize = sizeof(SectionIndex);

    ptin_checkpoint[id].ckptOpenFlags = SA_CKPT_CHECKPOINT_CREATE|SA_CKPT_CHECKPOINT_READ|SA_CKPT_CHECKPOINT_WRITE;

    //ptin_checkpoint[id].ckptOpenFlags = SA_CKPT_CHECKPOINT_READ;

   version.releaseCode = 'B';
   version.majorVersion = 2;
   version.minorVersion = 3;

   if ((ptin_checkpoint[id].ckptOpenFlags & SA_CKPT_CHECKPOINT_CREATE) != 0)
   {
     pCkptCreateAttr = &ptin_checkpoint[id].ckptCreateAttr;
   }
   else
   {
     pCkptCreateAttr = NULL;
   }

   saRet = saCkptInitialize(&ptin_checkpoint[id].ckptHandle, NULL, &version);

   if (saRet != SA_AIS_OK)
   {
     PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d initializing opensaf for checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
     return L7_FAILURE;
   }

   for (i=0; i<2; ++i)
   {
     saRet = saCkptCheckpointOpen(  ptin_checkpoint[id].ckptHandle,
                                      &ptin_checkpoint[id].ckptName,
                                      pCkptCreateAttr,
                                      ptin_checkpoint[id].ckptOpenFlags,
                                      timeout,
                                      &ptin_checkpoint[id].checkpointHandle);

     if (saRet == SA_AIS_ERR_EXIST && i == 0)
     {
       PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint %s already exists with different attributes. Delete it...", ptin_checkpoint[id].ckptNameStr);

       saRet = saCkptCheckpointUnlink(ptin_checkpoint[id].ckptHandle, &ptin_checkpoint[id].ckptName);

       if (saRet != SA_AIS_OK)
       {
         PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d deleting checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
       }
     }
     else
     {
        break;
     }
   }
   if (saRet != SA_AIS_OK)
   {
     PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d opening checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
     ptin_opensaf_checkpoint_deinit(id);

     return L7_FAILURE;
   }
    

   PT_LOG_NOTICE(LOG_CTX_OPENSAF, "Opensaf initialized and Checkpoint opened: %s", ptin_checkpoint[id].ckptNameStr);
   ptin_checkpoint[id].initialized = 1;

   if (!pCkptCreateAttr)
   {
     //Opened in read only. Lets read checkpoint attributes.

     SaCkptCheckpointDescriptorT checkpointStatus;
     saRet = saCkptCheckpointStatusGet(ptin_checkpoint[id].checkpointHandle, &checkpointStatus);

     if (saRet == SA_AIS_OK)    
     {
       memcpy(&ptin_checkpoint[id].ckptCreateAttr, &checkpointStatus.checkpointCreationAttributes, sizeof(ptin_checkpoint[id].ckptCreateAttr));
     }
     else
     {
       PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d getting status of checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
     }
   }

  return L7_SUCCESS;
}


/**
 * Initialize a opensaf read-only checkpoint handler
 * 
 * @param name of the handler
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
static L7_RC_t ptin_opensaf_readOnly_checkpoint_init(const char *name, int id)
{
  L7_char8 namedn[128];
  L7_uint32 namelen,i;


  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s\n", id);
    return L7_FAILURE;
  }

  if (ptin_checkpoint[id].initialized == L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d is already in use %s", id);
    return L7_FAILURE;
  }

  ptin_checkpoint[id].errorCount = 0;

  pthread_mutex_init(&ptin_checkpoint[id].ckptMux, NULL);

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }

  if (ptin_checkpoint[id].initialized == L7_TRUE)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d is already in use %s", id);
    return L7_FAILURE;
  }

  snprintf(ptin_checkpoint[id].ckptNameStr, sizeof(&ptin_checkpoint[id].ckptMux), "%s", name);
  namelen = snprintf(namedn, sizeof(namedn), "safCkpt=%s,safApp=safCkptService", name);

  bzero(&ptin_checkpoint[id].ckptName, sizeof(ptin_checkpoint[id].ckptName));
  memcpy(ptin_checkpoint[id].ckptName.value, namedn, namelen);
  ptin_checkpoint[id].ckptName.length = namelen;

  ptin_checkpoint[id].ckptOpenFlags = SA_CKPT_CHECKPOINT_READ;

  //this will not be used, we only want to read the checkpoint
  ptin_checkpoint[id].ckptCreateAttr.maxSections = 0xFF;
  ptin_checkpoint[id].ckptCreateAttr.maxSectionSize = 0xFF;

  SaVersionT	version;
  int		saRet;
  SaTimeT timeout = 10000000000ll;
  SaCkptCheckpointCreationAttributesT *pCkptCreateAttr;

  version.releaseCode = 'B';
  version.majorVersion = 2;
  version.minorVersion = 3;

  if ((ptin_checkpoint[id].ckptOpenFlags & SA_CKPT_CHECKPOINT_CREATE) != 0)
  {
    pCkptCreateAttr = &ptin_checkpoint[id].ckptCreateAttr;
  }
  else
  {
    pCkptCreateAttr = NULL;
  }

  saRet = saCkptInitialize(&ptin_checkpoint[id].ckptHandle, NULL, &version);
  if (saRet != SA_AIS_OK)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d initializing opensaf for checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
    return L7_FAILURE;
  }

  for (i=0; i<2; ++i)
  {
    saRet = saCkptCheckpointOpen( ptin_checkpoint[id].ckptHandle,
                                  &ptin_checkpoint[id].ckptName,
                                  pCkptCreateAttr,
                                  ptin_checkpoint[id].ckptOpenFlags,
                                  timeout,
                                  &ptin_checkpoint[id].checkpointHandle);

    if (saRet == SA_AIS_ERR_EXIST && i == 0)
    {
      PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint %s already exists with different attributes. Delete it...", ptin_checkpoint[id].ckptNameStr);
      saRet = saCkptCheckpointUnlink(ptin_checkpoint[id].ckptHandle, &ptin_checkpoint[id].ckptName);

      if (saRet != SA_AIS_OK)
      {
        PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d deleting checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
      }
    }
    else
    {
      break;
    }
  }
  if (saRet != SA_AIS_OK)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d opening checkpoint %s\n", saRet, ptin_checkpoint[id].ckptNameStr);
    ptin_opensaf_checkpoint_deinit(id);

    return L7_FAILURE;
  }
  
  PT_LOG_ERR(LOG_CTX_OPENSAF, "Opensaf initialized and Checkpoint opened: %s", ptin_checkpoint[id].ckptNameStr);
  ptin_checkpoint[id].initialized = 1;

  if (!pCkptCreateAttr)
  {
    //Opened in read only. Lets read checkpoint attributes.

    SaCkptCheckpointDescriptorT checkpointStatus;
    saRet = saCkptCheckpointStatusGet(ptin_checkpoint[id].checkpointHandle, &checkpointStatus);
    if (saRet == SA_AIS_OK)
    {
        memcpy(&ptin_checkpoint[id].ckptCreateAttr, &checkpointStatus.checkpointCreationAttributes, sizeof(ptin_checkpoint[id].ckptCreateAttr));
    }
    else
    {
        PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d getting status of checkpoint %s", saRet, ptin_checkpoint[id].ckptNameStr);
    }
  }

  return L7_SUCCESS;
}

#if (PTIN_BOARD != PTIN_BOARD_OLT1T0)
/**
 * Deinitialize a opensaf checkpoint handler
 * 
 * @param name of the handler
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
static L7_RC_t ptin_opensaf_checkpoint_deinit(int id)
{
  int saRet1, saRet2;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }

  saRet1 = saCkptCheckpointClose(ptin_checkpoint[id].checkpointHandle);
  saRet2 = saCkptFinalize(ptin_checkpoint[id].ckptHandle);

  if (ptin_checkpoint[id].initialized == 0)
  {
    //already not opened
  }
  else
  {
    if (saRet1 == SA_AIS_OK && saRet2 == SA_AIS_OK)
    {
      PT_LOG_INFO(LOG_CTX_OPENSAF, "Checkpoint closed and Opensaf finalized: %s", ptin_checkpoint[id].ckptNameStr);
      ptin_checkpoint[id].initialized = 0;
    }
    else
    {
      PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d,%d closing checkpoint %s", saRet1, saRet2, ptin_checkpoint[id].ckptNameStr);

      if ((saRet1 == SA_AIS_ERR_LIBRARY || saRet2 == SA_AIS_ERR_LIBRARY) ||
            (saRet1 == SA_AIS_ERR_BAD_HANDLE || saRet2 == SA_AIS_ERR_BAD_HANDLE))
      {
        PT_LOG_ERR(LOG_CTX_OPENSAF, "Opensaf corrupted? Better reinitialize.");
        ptin_checkpoint[id].initialized = 0;
      }
    }
  }
  return L7_SUCCESS;
}
#endif
/**
 * Check if a checkpoint is initialize
 * 
 * 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
static L7_RC_t ptin_opensaf_checkCheckpointInitialization(int id)
{
  int saRet = 0;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }

  if (ptin_checkpoint[id].initialized == 0)
  {
  	PT_LOG_INFO(LOG_CTX_OPENSAF, "Checkpoint %s is not initialized (error count: %d)", ptin_checkpoint[id].ckptNameStr, ptin_checkpoint[id].errorCount);

  	if (ptin_checkpoint[id].errorCount >= 100)
  	{
  	   char com[128];
      
  	   PT_LOG_WARN(LOG_CTX_OPENSAF, "RESTART OPENSAF\n");
  	   snprintf(com, sizeof(com), "touch /usr/local/ptin/tmp/opensaf_restart");
  	   system(com);
      
  	   ptin_checkpoint[id].errorCount = 0;
  	}
  	else
    {
      ptin_checkpoint[id].errorCount++;
    }

  	saRet = ptin_opensaf_readOnly_checkpoint_init(ptin_checkpoint[id].ckptNameStr,id);
  }
  else
  {
  	ptin_checkpoint[id].errorCount = 0;
  }

  return saRet;
}

/**
 * Lock a checkpoint 
 * 
 * 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
void ptin_opensaf_CheckpointLock(int id)
{

  pthread_mutex_lock(&ptin_checkpoint[id].ckptMux);

}

/**
 * Unlock a checkpoint 
 * 
 * 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
void ptin_opensaf_CheckpointUnlock(int id)
{
  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
  }

  pthread_mutex_unlock(&ptin_checkpoint[id].ckptMux);

}

/**
 * Check if the checkpoint is open/initialized
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_BOOL ptin_opensaf_isOpen(int id)
{
  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }

  return ptin_checkpoint[id].initialized;
}

/**
 * Gets the max section of  a checkpoint
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_RC_t ptin_opensaf_getMaxSections(int id)
{
  return ptin_checkpoint[id].ckptCreateAttr.maxSections;
}

/**
 * Gets the max section size of a checkpoint
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_RC_t ptin_opensaf_getMaxSectionSize(int id)
{

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }

  return ptin_checkpoint[id].ckptCreateAttr.maxSectionSize;
}

/** 
 * Write data in a checkpoint 
 *  
 * @param data void* - data to be stored in the checkpoint
 * @param len int - length of data
 * @param idx SectionIndex - identifier of section where data will be stored 
 * @param offset int (optional) - offset in the section that marks the start where data will be write 
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 *
 */
L7_RC_t ptin_opensaf_write_checkpoint(void *data, int len, SectionIndex idx, int offset, int id, int add)
{
  L7_RC_t saRet,i;
  SaCkptIOVectorElementT writeVector;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }
    
  if (ptin_checkpoint[id].ckptCreateAttr.maxSections > 1)
  {
    writeVector.sectionId.id = (unsigned char *)&idx;
    writeVector.sectionId.idLen = sizeof(idx);
  }
  else
  {
    writeVector.sectionId = (SaCkptSectionIdT)SA_CKPT_DEFAULT_SECTION_ID;
  }

  writeVector.dataBuffer = data;
  writeVector.dataSize = len;
  writeVector.dataOffset = offset;
  writeVector.readSize = 0;

  //Try write to section. If section does not exist, create it and write to section again.
  for (i=0; i<2; ++i)
  {
    ptin_opensaf_checkCheckpointInitialization(id);
    
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Going to write in checkpoint %s:%u", ptin_checkpoint[id].ckptNameStr, idx);
    
    saRet = saCkptCheckpointWrite(	ptin_checkpoint[id].checkpointHandle,
                                    &writeVector,
                                    1,
                                    NULL);


    if (saRet == SA_AIS_OK)
    {
      if( add == 1 )
      {
        L7_uint64 bmp_aux = 0; 
        bmp_aux = 1 << (offset / len);
        ptin_checkpoint[id].bmp_section[idx] |= bmp_aux;
        PT_LOG_TRACE(LOG_CTX_OPENSAF, "Written %d ", ptin_checkpoint[id].bmp_section[idx]);
      }
      else
      {
        L7_uint64 bmp = -1;
        bmp = 0 << (offset / len);
        ptin_checkpoint[id].bmp_section[idx] &= bmp;
        PT_LOG_TRACE(LOG_CTX_OPENSAF, "Written %d ", ptin_checkpoint[id].bmp_section[idx]);
      }

      PT_LOG_TRACE(LOG_CTX_OPENSAF, "Written %d bytes at offset %d to checkpoint %s:%u", len, offset, ptin_checkpoint[id].ckptNameStr, idx);
      return L7_SUCCESS;
    }
    
    if (saRet == SA_AIS_ERR_NOT_EXIST && i == 0) //enter here only in first iteration
    {
      PT_LOG_DEBUG(LOG_CTX_OPENSAF, "Section not exists yet: %s:%u", ptin_checkpoint[id].ckptNameStr, idx);

      SaCkptSectionCreationAttributesT sectionCreationAttributes;
        
      sectionCreationAttributes.sectionId = &writeVector.sectionId;
      sectionCreationAttributes.expirationTime = SA_TIME_END;

      saRet = saCkptSectionCreate(	ptin_checkpoint[id].checkpointHandle,
                                        &sectionCreationAttributes,
                                        NULL,
                                        0);
      if (saRet == SA_AIS_ERR_EXIST)
      {
        PT_LOG_ERR(LOG_CTX_OPENSAF, "INCOHERENCE - Section not created because already exists: %s:%u", ptin_checkpoint[id].ckptNameStr, idx);
      }
      else
      {
        if (saRet != SA_AIS_OK)
        {
           PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d creating section %s:%u", saRet, ptin_checkpoint[id].ckptNameStr, idx);
           ptin_opensaf_checkpoint_deinit(id);
           break;
        }
      }
    }
    else
    {
      if (saRet != SA_AIS_OK)
      {
          PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d writing to checkpoint %s:%u", saRet, ptin_checkpoint[id].ckptNameStr, idx);
          //section was created, so at this point no error is acceptable, better reinit opensaf
          ptin_opensaf_checkpoint_deinit(id);
      }
    }
  }
  
  return L7_FAILURE;
}

/** Read data from a checkpoint
 *
 * @param data void* - buffer to store the data read from checkpoint 
 * @param len int - length of data 
 * @param idx SectionIndex - identifier of section from where data will be read 
 * @param offset int (optional) - offset in the section that marks the start where data will be read
 *  
 * @return int 
 *
 */
L7_RC_t ptin_opensaf_read_checkpoint(void *data, int len, SectionIndex idx, int offset, int id)
{
  int saRet;
  SaCkptIOVectorElementT readVector;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid ", id);
    return L7_FAILURE;
  }

  ptin_opensaf_checkCheckpointInitialization(id);
  bzero( data, len);
  if (ptin_checkpoint[id].ckptCreateAttr.maxSections > 1)
  {
    readVector.sectionId.id = (unsigned char *)&idx;
    readVector.sectionId.idLen = sizeof(idx);
  }
  else
  {
    readVector.sectionId = (SaCkptSectionIdT)SA_CKPT_DEFAULT_SECTION_ID;
  }

  readVector.dataBuffer = data;
  readVector.dataSize = len;
  readVector.dataOffset = offset;

  saRet = saCkptCheckpointRead(	ptin_checkpoint[id].checkpointHandle,
  								&readVector,
  								1,
  								NULL);

  if (saRet != SA_AIS_OK)
  {
    if (!ptin_checkpoint[id].ignoreSectionNotExistDebug)
    {
  	  PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d reading from checkpoint %s:%u", saRet, ptin_checkpoint[id].ckptNameStr, idx);
    }
  	//the only acceptable error is the section does not exist, otherwise better reinit opensaf
  	if (saRet != SA_AIS_ERR_NOT_EXIST)
    {
      ptin_opensaf_checkpoint_deinit(id);
    }

  	return L7_FAILURE;
  }

  PT_LOG_TRACE(LOG_CTX_OPENSAF, "Read %d bytes at offset %d from checkpoint %s:%u", len, offset, ptin_checkpoint[id].ckptNameStr, idx);

  return L7_SUCCESS;
}

/** Delete a section from a checkpoint
 *
 * @param idx SectionIndex - identifier of section to be deleted
 * @return int
 *
 */
L7_RC_t ptin_opensaf_checkpoint_deleteSection(SectionIndex idx, int id)
{
  L7_RC_t saRet;
  SaCkptSectionIdT sectionId;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s", id);
    return L7_FAILURE;
  }

  ptin_opensaf_checkCheckpointInitialization(id);

  sectionId.id = (unsigned char *)&idx;
  sectionId.idLen = sizeof(idx);

  saRet = saCkptSectionDelete(ptin_checkpoint[id].checkpointHandle, &sectionId);

  if (saRet == SA_AIS_ERR_NOT_EXIST)
  {
    PT_LOG_DEBUG(LOG_CTX_OPENSAF, "Section not deleted because does not exist");
  }
  else
  {
    if (saRet != SA_AIS_OK)
  	{
      PT_LOG_ERR(LOG_CTX_OPENSAF, "Error %d deleting section %s:%u", saRet, ptin_checkpoint[id].ckptNameStr, idx);
      //the only acceptable error is the section does not exist, otherwise better reinit opensaf
  	  if (saRet != SA_AIS_ERR_NOT_EXIST)
      {
        ptin_opensaf_checkpoint_deinit(id);
      }
  	return L7_FAILURE;
  	}
  	PT_LOG_DEBUG(LOG_CTX_OPENSAF, "Deleted section from checkpoint %s:%u", ptin_checkpoint[id].ckptNameStr, idx);
  }

  memset(ptin_checkpoint[id].bmp_section,0 ,sizeof(ptin_checkpoint[id].bmp_section));

  return L7_SUCCESS;
}

/** Check if the checkpoint is operational (write and read in a
 *  section)
 *  
 *
 * @param data void* - data to be stored in the checkpoint
 * @param len int - length of data
 * @param idx SectionIndex - identifier of section where data will be stored 
 * @param offset int (optional) - offset in the section that marks the start where data will be write
 *  
 * @return int 
 *
 */
void ptin_checkpoint_runValidation(int step, const char *caller, int id)
{
  PT_LOG_INFO(LOG_CTX_OPENSAF, "***** Caller %s: Running validation of checkpoint %s *****\n", caller ? caller : "-", ptin_checkpoint[id].ckptNameStr);

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid %s\n", id);
  }

  if (step == 1)
  {
    char buffer[1] = { 0 };

    PT_LOG_INFO(LOG_CTX_OPENSAF, "*write*\n");
    ptin_opensaf_write_checkpoint( buffer, 1, 255, 0, id, 1);
    PT_LOG_INFO(LOG_CTX_OPENSAF, "*read*\n");
    ptin_opensaf_read_checkpoint(buffer, 1, 255, 0, id);
    PT_LOG_INFO(LOG_CTX_OPENSAF, "*delete*\n");
    ptin_opensaf_checkpoint_deleteSection(255, id);
  }
}

/** Print all sections contents to stdout
 *
 * @return void
 *
 */   
L7_RC_t ptin_checkpoint_dumpCheckpoint(int id)
{
  int saRet;
  SaCkptSectionIterationHandleT   iteratorHandle;
  SaCkptSectionDescriptorT        sectionDescriptor;
  SectionIndex                    idx;
  int                             bytesToPrint, j;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);
  }

  saRet = saCkptSectionIterationInitialize(ptin_checkpoint[id].checkpointHandle, SA_CKPT_SECTIONS_ANY, 0, &iteratorHandle);

  if (saRet != SA_AIS_OK)
  {
     return L7_SUCCESS;
  }

  while (1)
  {
    saRet = saCkptSectionIterationNext(iteratorHandle, &sectionDescriptor);
    if (saRet != SA_AIS_OK)
    {
      break;
    }
    
    if ( sectionDescriptor.sectionId.id )
    {
      memcpy( &idx, sectionDescriptor.sectionId.id, sizeof(idx) );
    }
    else
    {
      idx = 0;
    }
    bytesToPrint = sectionDescriptor.sectionSize;
    unsigned char buffer[bytesToPrint];
        
    if (ptin_opensaf_read_checkpoint(buffer, bytesToPrint, idx, 0, id) == 0)
    {
      printf("Dumping %d bytes of section 0x%08X:\n", bytesToPrint, idx);
      printf(" %s \n", buffer);
      for (j=0; j<bytesToPrint; ++j)
      {
        printf("0x%c ", buffer[j]);      //PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  
      }
      printf("\n");                      //PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  
    } 
  }
  
  saCkptSectionIterationFinalize(iteratorHandle);

  return L7_SUCCESS;
}

/** Get section content 
 *
 * @return void
 *
 */   
L7_RC_t ptin_checkpoint_getSection(int id, int section, void *data, int* size)
{
  int saRet;
  SaCkptSectionIterationHandleT   iteratorHandle;
  SaCkptSectionDescriptorT        sectionDescriptor;
  SectionIndex                    idx;
  int                             bytesToPrint, j;

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);
  }
  saRet = saCkptSectionIterationInitialize(ptin_checkpoint[id].checkpointHandle, SA_CKPT_SECTIONS_ANY, 0, &iteratorHandle);

  if (saRet != SA_AIS_OK)
  {
     return L7_SUCCESS;
  }

  while (1)
  {
    saRet = saCkptSectionIterationNext(iteratorHandle, &sectionDescriptor);

    if (saRet != SA_AIS_OK)
    {
      break;
    }

    if( *sectionDescriptor.sectionId.id  != section )
    {
      continue;
    }
    
    if ( sectionDescriptor.sectionId.id )
    {
      memcpy( &idx, sectionDescriptor.sectionId.id, sizeof(idx) );
    }
    else
    {
      idx = 0;
    }

    bytesToPrint = sectionDescriptor.sectionSize;
    unsigned char buffer[bytesToPrint];
        
    if (ptin_opensaf_read_checkpoint(buffer, bytesToPrint, idx, 0, id) == 0)
    {  
      for (j=0; j<bytesToPrint; ++j)
      {
        PT_LOG_TRACE(LOG_CTX_OPENSAF,"0x%c ", buffer[j]); //PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  
      }
      *size = bytesToPrint;
      memcpy( data, buffer, sizeof(buffer) );  
    }
  } 

  saCkptSectionIterationFinalize(iteratorHandle);

  return L7_SUCCESS;
}


/** Print a section content 
 *
 * @return void
 *
 */   
L7_RC_t ptin_checkpoint_dumpSection(int id, int section)
{
  int saRet;
  SaCkptSectionIterationHandleT   iteratorHandle;
  SaCkptSectionDescriptorT        sectionDescriptor;
  SectionIndex                    idx;
  int                             bytesToPrint, j;

 PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);
  }
  saRet = saCkptSectionIterationInitialize(ptin_checkpoint[id].checkpointHandle, SA_CKPT_SECTIONS_ANY, 0, &iteratorHandle);

  if (saRet != SA_AIS_OK)
  {
     return L7_SUCCESS;
  }

  while (1)
  {
    saRet = saCkptSectionIterationNext(iteratorHandle, &sectionDescriptor);

    if (saRet != SA_AIS_OK)
    {
      break;
    }

    if( *sectionDescriptor.sectionId.id  != section )
    {
      continue;
    }
    
    if ( sectionDescriptor.sectionId.id )
    {
      memcpy( &idx, sectionDescriptor.sectionId.id, sizeof(idx) );
    }
    else
    {
      idx = 0;
    }

    bytesToPrint = sectionDescriptor.sectionSize;
    unsigned char buffer[bytesToPrint];
        
    if (ptin_opensaf_read_checkpoint(buffer, bytesToPrint, idx, 0, id) == 0)
    {
      PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  
      printf("Dumping %d bytes of section 0x%08X:\n", bytesToPrint, idx);
      printf(" %s \n", buffer);
      for (j=0; j<bytesToPrint; ++j)
      {
        printf("0x%c ", buffer[j]);      //PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  
      }
      printf("\n");                      //PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid \n", id);  
    }
  }

  saCkptSectionIterationFinalize(iteratorHandle);

  return L7_SUCCESS;
}

/** Print a section content 
 *
 * @return void 
 */   
L7_RC_t ptin_checkpoint_findDatainSection(int id, int section, void* data, int size, int* position)
{
  int saRet;
  SaCkptSectionIterationHandleT   iteratorHandle;
  SaCkptSectionDescriptorT        sectionDescriptor;
  SectionIndex                    idx;
  int                             bytesToPrint,offset = 0;//, j;
  unsigned char aux_data[6]= "";

  if (id>PTIN_MAX_OPENSAF_CHECKPOINTS)
  {
    PT_LOG_ERR(LOG_CTX_OPENSAF, "Checkpoint id %d not valid ", id);
  }
  saRet = saCkptSectionIterationInitialize(ptin_checkpoint[id].checkpointHandle, SA_CKPT_SECTIONS_ANY, 0, &iteratorHandle);

  if (saRet != SA_AIS_OK)
  {
     return L7_FAILURE;
  }

  memcpy( &aux_data,(unsigned char *)data,sizeof(aux_data));

  saRet = saCkptSectionIterationNext(iteratorHandle, &sectionDescriptor);

  if (saRet != SA_AIS_OK)
  {
    return L7_FAILURE;
  }

  //if( *sectionDescriptor.sectionId.id  != section )
  //{
    //continue;
  //}
  
  if ( sectionDescriptor.sectionId.id )
  {
    memcpy( &idx, sectionDescriptor.sectionId.id, sizeof(idx) );
  }
  else
  {
    idx = 0;
  }
  bytesToPrint = sectionDescriptor.sectionSize;

  unsigned char buffer[size];
  unsigned char aux_buffer[size];
  
  while(bytesToPrint>offset)
  {

    if (ptin_opensaf_read_checkpoint(buffer, size, idx, offset, id) == 0)
    {         
       memcpy( &aux_buffer, &buffer, sizeof(aux_buffer));
       
       PT_LOG_TRACE(LOG_CTX_OPENSAF,"Search Opensaf : %c %c %c %c %c %c ", aux_buffer[0], aux_buffer[1], aux_buffer[2], 
                                                                            aux_buffer[3], aux_buffer[4], aux_buffer[5]);
            
       PT_LOG_TRACE(LOG_CTX_OPENSAF,"Search Data : %c %c %c %c %c %c ",    aux_data[0], aux_data[1], aux_data[2], 
                                                                            aux_data[3], aux_data[4], aux_data[5]);
    
       if( (aux_buffer[0] != aux_data[0]) || (aux_buffer[1] != aux_data[1]) || (aux_buffer[2] != aux_data[2]) || 
           (aux_buffer[3] != aux_data[3]) || (aux_buffer[4] != aux_data[4]) || (aux_buffer[5] != aux_data[5]) )
       { 

         offset = offset + sizeof(aux_buffer);
         PT_LOG_TRACE(LOG_CTX_OPENSAF, "Offset : %d ", id); 
                 
         continue;
       }

       PT_LOG_TRACE(LOG_CTX_OPENSAF, "Data found");
       saCkptSectionIterationFinalize(iteratorHandle);
       *position = offset; //index 

       return L7_SUCCESS;
    }    
  }

  PT_LOG_ERR(LOG_CTX_OPENSAF, "Data not found " );
  saCkptSectionIterationFinalize(iteratorHandle);

  return L7_FAILURE;
}

/** Find the offset of a free element
 *
 * @return void 
 */   
L7_RC_t ptin_opensaf_find_free_element(int* offset, int section, int id)
{

  L7_uint64 aux=1, i=0; 

  if(ptin_checkpoint[id].bmp_section[section] == 0)
  {
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Offset %d \n", *offset);
    offset = 0;
    return L7_SUCCESS;
  }

  if(ptin_checkpoint[id].bmp_section[section] == -1)
  {
    *offset = -1;
    PT_LOG_TRACE(LOG_CTX_OPENSAF, "Offset %d \n", *offset);
    return L7_FAILURE;
  }


  while(aux != 0)
  {
    aux = 0;
    aux = ptin_checkpoint[id].bmp_section[section] & (1 << i);

    PT_LOG_TRACE(LOG_CTX_OPENSAF, "ptin_checkpoint[id].bmp_section[section] %x \n", ptin_checkpoint[id].bmp_section[section]);
    i++;
  }

  *offset = (i-1) * 6;
  PT_LOG_TRACE(LOG_CTX_OPENSAF, "Offset %d \n", *offset);
  return L7_SUCCESS;
}


/********************************End PTin Opensaf Checkpoint*********************************************************/


/**
 * Initialize a opensaf checkpoint handler
 * 
 * @param name of the handler, number of section and length of 
 *             each section
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_RC_t ptin_opensaf_checkpoint_teste(int sectionsNum, int sectionLen, int id)
{
  char *name ="teste";
  L7_RC_t rc;
 
  rc = ptin_opensaf_checkpoint_init(name,sectionsNum,sectionLen,id);

  return rc;
}


/**
 * Initialize a opensaf checkpoint handler
 * 
 * @param name of the handler, number of section and length of 
 *             each section
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_RC_t ptin_opensaf_checkpoint_teste2(int id)
{
  char *name ="teste";
  L7_RC_t rc;
 
  rc = ptin_opensaf_readOnly_checkpoint_init(name, 1);

  return rc;
}


L7_RC_t ptin_opensaf_write_checkpoint_teste(void)
{

  char *name ="olaola";
  L7_RC_t rc;  

  rc = ptin_opensaf_write_checkpoint(name, 6, 1, 0, 1, 1);

  return rc;
}

L7_RC_t ptin_checkpoint_findDatainSection_teste()
{

  char teste[6] ="olaola";
  L7_uint32 position;
  
  return ptin_checkpoint_findDatainSection(1, 1, &teste, sizeof(teste), &position);
}

#endif

#endif


