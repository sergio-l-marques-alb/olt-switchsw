/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_cli_protect.h
*
* @purpose
*
* @component XLIB
*
* @comments
*
* @create 04/17/2007
*
* @author Rama Sasthri, Kristipati
* @end
*
**********************************************************************/

/*! \file */

#ifndef XLIB_CLI_PROTECT_H
#define XLIB_CLI_PROTECT_H

#include <stdlib.h>
#include <string.h>

//#include "xlib_protect.h"
#include "xlib_private.h"
#include "xcli_lib.h"

#include "_xeobj_xtype.h"
#include "_xe_obj.h"

typedef void (*printCLIFunc_t) (void *wap);

void *xLibCli2WaInit (void *cliSrvCntx);
void xLibCli2AddKey (void *cntx, xLibId_t oid, xLibU16_t type);
void xLibCli2AddObj (void *cntx, xLibId_t oid, xLibId_t cpoid, xLibU16_t type, char *lbl, xLibU16_t flag);
void xLibCli2ErrorMap (void *cntx, xLibId_t oid, xLibU32_t code, xLibU32_t mCode);
void xLibCli2Get (void *cntx);
void xLibCli2RepeatGet (void *cntx);
void xLibCli2ObjUIEleValSet (void *cntx, xLibU32_t objID, xLibU32_t uiWebID, xLibU32_t uiCliID,
                             xLibU32_t eleID);
void xLibCli2SetError(void * wap, xLibId_t oid, xLibRC_t rc, xLibS8_t * name);
void xLibCli2SetError2 (void *cntx, xLibObj_t *objp, xLibS8_t * name);
xLibCS8_t *xLibCli2Set (void *cntx);
int xLibCli2Validate (void *cntx, xLibId_t oid, xLibU16_t type, const char *str, xcli_bool_t exe);
xLibCS8_t *xLibCli2FinishCommand (void *parserContext);
xLibCS8_t *xLibCli2GetFinish (void *cntx);
xLibCS8_t *xLibCli2SetFinish (void *cntx);
xLibCS8_t *xLibCli2RepeatGetFinish (void *cntx);
xLibCS8_t *xLibCliTextPrint (void *cntx, xLibS8_t *str);
void xLibCli2CustomPrint(void *cntx);
void xLibCli2DefaultPrint(void *cntx);
void xLibCli2SetCustomPrintFunc(void *cntx,printCLIFunc_t printFunc, xLibU16_t dispType);


#define XLIB_UTCLI_TREE_NODE_ADD(p,c,h,f,v,a,l,o) xcli_add_node_withopt(p, c, h, v, f, a, l, o)
#define XLIB_CLI_TREE_NODE_ADD(n,c,h,v,l,cond) xcli_add_node(n, c, h, v, NULL, NULL, l,cond)
#define XLIB_CLI_TREE_ACTN_SET(n,f,a) xcli_set_node_func(n,f,a)
#define XLIB_DISPLAY_TABULAR 1
#define XLIB_DISPLAY_NORMAL 0
#endif

