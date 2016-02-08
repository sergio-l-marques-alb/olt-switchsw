/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* Name: xlib_helper.h
*
* Purpose: Header for the Abstraction for XLIB from SNMP interface
*
* Created by: Madan Mohan Goud.K
*
* Component: SNMP
*
*********************************************************************/
#ifndef XLIBSNMP_H
#define XLIBSNMP_H

#include <stdio.h>
#include <sys/types.h>
#include <ctype.h>
#include <errno.h>

#if HAVE_STRING_H
#include <string.h>
#else
#include <strings.h>
#endif
#if HAVE_STDLIB_H
#include <stdlib.h>
#endif
#if HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_SYS_SOCKET_H
#include <sys/socket.h>
#endif
#if HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif

#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>
#include <net-snmp/agent/net-snmp-agent-includes.h>
#include <net-snmp/agent/snmp_vars.h>

#include "xlib_protect.h"

#define XLIB_SNMP_INDEX_MAX_LEN 128
#define XLIB_SNMP_BUF_LEN       1024
#define XLIB_SNMP_DUMMY_OBJ_ID 0xFFFF0001

/* This is a list which is used for handling indexes */
typedef struct
{
  xLibObjInfo_t xobj;           /* Keep this as first param */
  u_char value[XLIB_SNMP_INDEX_MAX_LEN];        /* keep this as second param */ /* TODO: make this as pointer */
  /* The above params are referenced by XLIB using xLibSnmpIndexInfo_t */
  u_char type;
  u_char oidlen;
  oid *oid;
  char oidname[MAX_OID_LEN + 2]; /* TODO: make this as pointer */
} xLibSnmpIndexList_t;

#define MIB2C_GENERATED_XIDS

#ifdef MIB2C_GENERATED_XIDS
#define XLIB_SNMP_INDEX(a,b,c,d,e) {{b,a},{0},c,0,NULL,d}
#else
#define XLIB_SNMP_INDEX(a,b,c,d,e) {{0,0},{0},c,0,NULL,d}
#endif

typedef struct
{
  xLibObjInfo_t xobj;           /* Keep this as first param */
  u_char type;
  int isRowStatusObject;
} xLibSnmpColumn_t;

#ifdef MIB2C_GENERATED_XIDS
#define XLIB_SNMP_COLUMN(a,b,c,d) {{b,a},c,d}
#else
#define XLIB_SNMP_COLUMN(a,b,c,d) {{0,0},c,d}
#endif

#ifdef MIB2C_GENERATED_XIDS
#define XLIB_SNMP_SCALAR_REGISTER(a,b,c,d,e,f,g) xLibSnmpHelperScalarRegister(a,b,c,d,e,f,g)
#else
#define XLIB_SNMP_SCALAR_REGISTER(a,b,c,d,e,f,g) xLibSnmpHelperScalarRegister(a,b,c,d,e,0,0)
#endif
int xLibSnmpHelperCommaOid2Name (char *s);
char *xLibSnmpRegInfoToOidStr (netsnmp_handler_registration * reginfo, unsigned int colnum);
int xLibSnmpGetVarSize (int type, void *value);
void xLibSnmpHelperDumpVarList (netsnmp_variable_list * context_ptr, char *file, int line,
                                char *msg);
void xLibSnmpHelperDumpIndexList (xLibSnmpIndexList_t * ilp, int count, char *file, int line,
                                  char *tbl);
netsnmp_variable_list *xLibSnmpGetFirstDataPoint (void **loopCntx, void **dataCntx,
                                                  netsnmp_variable_list * varList,
                                                  xLibSnmpIndexList_t * indexList,
                                                  int indexCount, char *tblName);
netsnmp_variable_list *xLibSnmpGetNextDataPoint (void **loopCntx, void **dataCntx,
                                                  netsnmp_variable_list * varList,
                                                  xLibSnmpIndexList_t * indexList,
                                                  int indexCount, char *tblName);
netsnmp_variable_list *xLibSnmpGetDataPoint (void **my_loop_context, void **my_data_context,
                                             netsnmp_variable_list * put_index_data,
                                             xLibSnmpIndexList_t * indexList,
                                             int numIndexes, char *tblName, int isFirst);
int xLibSnmpTableHandler (netsnmp_mib_handler * handler, netsnmp_handler_registration * reginfo,
                          netsnmp_agent_request_info * reqinfo, netsnmp_request_info * requests,
                          xLibSnmpIndexList_t * indexList, xLibSnmpColumn_t * columnList,
                          int num_indexes, char *tblName);
void xLibSnmpInitTable (oid * oid, int oidlen, Netsnmp_Node_Handler handler,
                        Netsnmp_First_Data_Point firstDataFunc,
                        Netsnmp_Next_Data_Point nextDataFunc,
                        Netsnmp_is_valid_column validColumnCheck, 
                        int min_column, int max_column, int canrw,
                        xLibSnmpIndexList_t * indexList, int indexCount,
                        xLibSnmpColumn_t * columnList, int colCount, char *tblName);

int xLibSnmpHelperScalar (netsnmp_mib_handler * handler, netsnmp_handler_registration * reginfo,
                          netsnmp_agent_request_info * reqinfo, netsnmp_request_info * requests);
void xLibSnmpHelperScalarRegister (char *name, u_char type, oid * oid, size_t len, u_char can,
                                   short, xLibId_t);

char *xLibSnmpHelperOid2Name (oid * name, int len);
int xLibSnmpHelperName2Oid (const char *name, oid *oidArr, int *len);
void xLibSnmpOidToStr (oid * name, int len, char *);

int xLibSnmpHelperEncode (netsnmp_variable_list * vars, u_char type, const char *value, size_t len);
int xLibSnmpHelperDecode (netsnmp_variable_list * vars, char *outBuf);

xLibRC_t xLibSnmpSetInstance (int keyCount, xLibId_t * keys, int *keyTypes, xLibS8_t ** keyVals,
                              xLibId_t id, xLibU16_t type, xLibS8_t * in);

int xLibIsValidColumn (xLibSnmpColumn_t * columnList, int colnum);
xLibRC_t xLibSnmpRowStatusGet(xLibId_t  oid,xLibU16_t type,xLibS8_t *val);


#ifdef XLIB_SNMP_DEBUG
#define MYTRACE_DEBUG(file,line,fmt,arg...) \
{\
  printf("%s:%d ::", file, line);\
  printf(fmt,##arg);\
  printf("\n");\
}
#else
#define MYTRACE_DEBUG(fmt,arg...)
#endif
#ifdef XLIB_SNMP_DEBUG
#define MYTRACE_DEBUG2(file,line,func,fmt,arg...) \
{\
  printf("%s:%d:%s ::", file, line, func);\
  printf(fmt,##arg);\
  printf("\n");\
}
#else
#define MYTRACE_DEBUG2(file,line,func,fmt,arg...)
#endif
#ifdef XLIB_SNMP_DEBUG
#define MYTRACE(fmt,arg...) MYTRACE_DEBUG2(__FILE__, __LINE__, __FUNCTION__, fmt, ##arg)
#else
#define MYTRACE(fmt,arg...)
#endif

typedef struct
{
  xLibId_t xid;
  u_short xtype;
  u_char asntype;
} xinfo_t;
#endif
