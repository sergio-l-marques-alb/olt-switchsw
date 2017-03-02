/**
 * ptin_opensaf_checkpoint.h
 *  
 * Implements the opensaf interface module
 *
 * Created on: 2016/07/04 Author: Rui Fernandes(rui-f-fernandes@alticealbs.com) 
 * Notes: 
 *
 */

#include "ptin_include.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

typedef enum {
    SWITCHDRVR_ONU       = 0x01,
    ONU_STATE,
    NGPON2GROUPS
} ptin_checkpoints;

/*********************************************************** */

/**
 * Initializes a event 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 *
 */

//extern L7_RC_t ptin_igmp_proxy_init(void);
#define PTIN_MAX_OPENSAF_CHECKPOINTS 10
typedef unsigned int SectionIndex;

/**
 * Initialize a opensaf checkpoint handler
 * 
 * @param name of the handler, number of section and length of 
 *             each section
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
extern L7_RC_t ptin_opensaf_init();

/**
 * Lock a checkpoint 
 * 
 * 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
void ptin_opensaf_CheckpointLock(int id);

/**
 * Unlock a checkpoint 
 * 
 * 
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
void ptin_opensaf_CheckpointUnlock(int id);

/**
 * Check if the checkpoint is open/initialized
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_BOOL ptin_opensaf_isOpen(int id);

/**
 * Gets IGMP Proxy configuration
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_getMaxSections(int id);

/**
 * Gets the max section of  a checkpoint
 * 
 * @param igmpProxy Structure with config parameters
 * 
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE 
 *  
 */
L7_RC_t ptin_opensaf_getMaxSectionSize(int id);

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
L7_RC_t ptin_opensaf_write_checkpoint(void *data, int len, SectionIndex idx, int offset, int id, int add);

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
L7_RC_t ptin_opensaf_read_checkpoint(void *data, int len, SectionIndex idx, int offset,int id);


/** Delete a section from a checkpoint
 *
 * @param idx SectionIndex - identifier of section to be deleted
 * @return int
 *
 */
L7_RC_t ptin_opensaf_checkpoint_deleteSection(SectionIndex idx,int id);

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
void ptin_checkpoint_runValidation(int step, const char *caller,int id);


/** Print all sections contents to stdout
 *
 * @return void
 *
 */    
L7_RC_t ptin_checkpoint_dumpCheckpoint(int id);


/** Call this function to guarantee that an active replica exists.
 *  If an active replica already exists nothing is done,
 *  otherwise local node will be set to active replica.
 *
 * @return int
 *
 */
  
  
/** Get section content 
 *
 * @return void
 *
 */   
L7_RC_t ptin_checkpoint_getSection(int id, int section, void *data, int* size);  

/** Print a section content 
 *
 * @return void 
 */   
L7_RC_t ptin_checkpoint_findDatainSection(int id, int section, void* data, int size, int* position);

/** Find the offset of a free element
 *
 * @return void 
 */   
L7_RC_t ptin_opensaf_find_free_element(int* offset, int section, int id);

L7_RC_t ptin_checkpoint_guaranteeActiveReplica(int id);

L7_RC_t ptin_opensaf_checkpoint_teste(int sectionsNum, int sectionLen, int id);

L7_RC_t ptin_opensaf_checkpoint_teste2(int id);


