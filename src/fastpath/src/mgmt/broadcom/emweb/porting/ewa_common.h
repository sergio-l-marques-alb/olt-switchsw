/*********************************************************************
 *
 * (C) Copyright Broadcom Corporation 2001-2007
 *
 **********************************************************************
 *
 * @filename  ewa_common.h
 *
 * @purpose   Provide common emweb porting functions
 *
 * @component emweb
 *
 * @comments  
 *
 * @create    06/07/2009
 *
 * @author    Rama Sasthri, Kristipati
 *
 * @end
 *
 **********************************************************************/

#ifndef EWA_COMMON_H
#define EWA_COMMON_H

#include "log.h"

void ewaSocketBufferSet (int sock, int recv, int send, char *which);
void ewaSocketReUseAddrSet (int sock, int val, char *which);
void ewaSocketLingerSet (int sock, int onoff, int val, char *which);
void ewaSocketKeepAliveSet (int sock, int val, char *which);
void ewaSocketSend (int sock, char *dataPtr, int bytes_to_be_sent);
L7_RC_t ewaNetClientInfoGet(EwsContext context, L7_inet_addr_t *pRemote, L7_uint32 *pPort);

#define EWA_LOGERR(_fmt,_args...) L7_LOGF(L7_LOG_SEVERITY_ERROR, L7_CLI_WEB_COMPONENT_ID,_fmt, ##_args)
#define EWA_LOGWARN(_fmt,_args...) L7_LOGF(L7_LOG_SEVERITY_WARNING, L7_CLI_WEB_COMPONENT_ID,_fmt, ##_args)
#define EWA_LOGDBG(_fmt,_args...) L7_LOGF(L7_LOG_SEVERITY_DEBUG, L7_CLI_WEB_COMPONENT_ID,_fmt, ##_args)

#endif
