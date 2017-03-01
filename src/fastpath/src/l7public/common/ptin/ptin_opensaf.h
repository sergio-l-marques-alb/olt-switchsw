/**
 * ptin_opensaf.h
 *  
 * Implements the opensaf interface module
 *
 * Created on: 2016/07/04 Author: Rui Fernandes(rui-f-fernandes@alticealbs.com) 
 * Notes: 
 *
 */
#ifndef _SAEVT_H
#define _SAEVT_H

#include "ptin_include.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <unistd.h>

#define MAC_SIZE_BYTES 6


typedef struct 
{ 

  L7_uint8 eventId;
  L7_uint8 memberIndex;
  L7_uint8 parentId;
  L7_uint8 onuId;

} __attribute__((packed)) ptin_opensaf_ngpon2_onustate;


/*********************************************************** *
 *  Init routine of opensaf event task
 *  
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
extern L7_RC_t ptin_opensaf_event_task_init();

/**
 * Read a event 
 *  
 * @param void data 
 * @param int len 
 *  
 * @return L7_RC_t L7_SUCCESS/L7_FAILURE
 */
L7_RC_t ptin_opensaf_read_event(void *data, int len, int id, char *chName, char *pubName);

#endif

