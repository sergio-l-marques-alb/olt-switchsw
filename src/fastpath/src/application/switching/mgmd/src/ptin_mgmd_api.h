/*********************************************************************
*
* (C) Copyright PT Inovação S.A. 2013-2013
*
**********************************************************************
*
* @create    21/10/2013
*
* @author    Daniel Filipe Figueira
* @author    Marcio Daniel Melo
*
**********************************************************************/
#ifndef _PTIN_MGMD_API_H
#define _PTIN_MGMD_API_H

#include "ptin_mgmd_eventqueue.h"

#include <pthread.h>

//Log output streams
#define MGMD_LOG_STDERR 1
#define MGMD_LOG_STDOUT 2
#define MGMD_LOG_FILE   3

//Log contexts
#define PTIN_MGMD_LOG  1
#define PTIN_TIMER_LOG 2
#define PTIN_FIFO_LOG  3

//Log severity
#define MGMD_LOG_FATAL    2
#define MGMD_LOG_CRITICAL 3
#define MGMD_LOG_ERROR    4
#define MGMD_LOG_WARNING  5
#define MGMD_LOG_NOTICE   6
#define MGMD_LOG_INFO     7
#define MGMD_LOG_DEBUG    8
#define MGMD_LOG_TRACE    9


typedef struct
{
  unsigned int (*igmp_admin_set) (unsigned char admin);
  unsigned int (*mld_admin_set)  (unsigned char admin);

  unsigned int (*cos_set)        (unsigned char cos);
                         
  unsigned int (*portList_get)   (unsigned int serviceId, ptin_mgmd_port_type_t portType, PTIN_MGMD_PORT_MASK_t *portList);
  unsigned int (*portType_get)   (unsigned int serviceId, unsigned int portId, ptin_mgmd_port_type_t *portType);
                         
  unsigned int (*clientList_get) (unsigned int serviceId, unsigned int portId, PTIN_MGMD_CLIENT_MASK_t *clientList);
                         
  unsigned int (*port_open)      (unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr, unsigned char isStatic);
  unsigned int (*port_close)     (unsigned int serviceId, unsigned int portId, unsigned int groupAddr, unsigned int sourceAddr);
                         
  unsigned int (*tx_packet)      (unsigned char *framePayload, unsigned int payloadLength, unsigned int serviceId, unsigned int portId, unsigned int clientId, unsigned char family);
} ptin_mgmd_externalapi_t;


/**
 * Used to initialize MGMD
 * 
 * @param thread_id[out]  : MGMD thread ID
 * @param externalApi[in] : Struct with API callbacks
 * @param logOutput[in]   : Output stream [MGMD_LOG_STDERR; MGMD_LOG_STDOUT; MGMD_LOG_FILE]
 * @param logFile[in]     : System path plus file name for the log file
 *  
 * @return RC_t 
 *  
 * @note 'logFile' defaults to /var/log/mgmd.log if passed as PTIN_NULLPTR.
 * @note 'logFile' is ignored if 'logOutput' is not LOG_FILE
 */
RC_t ptin_mgmd_init(pthread_t *thread_id, ptin_mgmd_externalapi_t* externalApi, uint8 logOutput, char8* logFile);

/**
 * Used to uninitialize MGMD
 * 
 * @param thread_id[out] : MGMD thread ID
 * 
 * @return RC_t 
 */
RC_t ptin_mgmd_deinit(pthread_t thread_id);

/**
 * Used to initialize MGMD
 * 
 * @param context[in]  : Log context
 * @param severity[in] : Log severity level
 *  
 * @return RC_t 
 */
RC_t ptin_mgmd_logseverity_set(uint8 context, uint8 severity);

/**
 * Used to set MGMD log level
 * 
 * @param logOutput[in]: Output stream [MGMD_LOG_STDERR; MGMD_LOG_STDOUT; MGMD_LOG_FILE]
 * @param logFile[in]  : System path plus file name for the log file
 *  
 * @return none 
 *  
 * @note 'logFile' defaults to /var/log/mgmd.log if passed as PTIN_NULLPTR.
 * @note 'logFile' is ignored if 'logOutput' is not LOG_FILE 
 */
void ptin_mgmd_logredirect(uint8 logOutput, char8* logFile);

#endif //_PTIN_MGMD_API_H
