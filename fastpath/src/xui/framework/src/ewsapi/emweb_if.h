/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename emweb_if.h
*
* @purpose
*
* @component EMWEB
*
* @comments
*
* @create 04/17/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

#ifndef EMWEB_IF_H
#define EMWEB_IF_H

#include "ewnet.h"

void *emwebAppWaGet(void *context, int size);
void emwebAppWaSet(void *context, void *wap);
void emwebFileHandlerSet(EwsContext context, void *handler);
void emwebFormSubmitSet (void *context, unsigned int type);
unsigned int emwebFormSubmitGet (void *context);

char *emwebFileNameGet(void *arg);

void emwebFormValueSet (char **var, unsigned char *status, char *value, int isDyn);

char *emwebContextBufferGet (void *context, unsigned short *bufSize);
void *emwebGetFilters (void *context);
void emwebOptionSet (void *optp, char *bufChoice, void *valp, int sel);

boolean emwebServeAfterSubmitGet(void *context);
void emwebServeAfterSubmitSet(void *context, boolean val);

char *emwebContextSendReply(void *context, char *file);
char *emwebErrorBufGet (void *context, unsigned short *len);
char *emwebLevelCheckBuffer(void *context);

void emwebDownloadUrlSet (EwsContext context, char *url);

char *emwebContextGetValue(void *cntx);

int emwebUserAccessGet(EwsContext context);

unsigned long ewaGetHostByname (char *host);
int ewaStdInConfig (void);
char *emwebStrdup (char *srcString);
void emwebFree (void *str);
int emwebSockServer (int ipaddr, int port, int v6, int retry, int max);
int ewaRecordLocalAddress (EwaNetHandle connection, ew_sockaddr_union_t * peerp);

char *emwebFilterDumpGet(void *context, boolean isWrite);
void emwebFilterDumpSet(void *context, char *datap, boolean isWrite);
void emwebFilterDumpAppend(void *context, char *datap, boolean isWrite);

char *emwebRepeatInitDetails(void *context, short index, int count, int rows, int max, boolean more);
char *emwebRepeatIsFirst(void *context, short index);
char *emwebRepeatIsLast(void *context, short index);
char *emwebRepeatIsMore(void *context, short index);
int emwebGetReqIntValue(void *context, char **buf16);
int emwebContextRemoteIpAddressGet(void* cntx,L7_inet_addr_t* remote);
int emwebContextListenPortGet(void* cntx,int* listenPort);
char* emwebFilePathHandler(EwsContext context,char* url);
int emwebGetRepeatCount (void *context, short index);

#ifdef XUI_DEBUG
#define EMWEB_TRACE_LINE() sysapiPrintf("%s:%d %s\n", __FILE__, __LINE__, __FUNCTION__)
#else
#define EMWEB_TRACE_LINE()
#endif

#define EMWEB_ASSERT(x) \
if (!(x))\
{\
  sysapiPrintf("XLIB_ASSERT: %s:%d\n", __FILE__, __LINE__);\
  exit(0);\
}

typedef struct EwaNetHandle_s emwebWa_t;

extern void *emwebWorkAreaGetByConnection(void *context);
extern void *emwebWorkAreaGetByHandle(void *handle);
char *emwebIsRepeatNotEmpty(void *context, short index);

#endif
