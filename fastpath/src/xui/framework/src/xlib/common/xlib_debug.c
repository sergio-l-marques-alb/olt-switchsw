/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_debug.c
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

#include <stdio.h>
#include <string.h>

#include "xlib_private.h"

extern int xLibTraceLevel;

int xLibDebugDumpObjects (void)
{
  return 0;
}

int xLibDebugEnable(int dbg)
{
  xLibTraceLevel = dbg;
  return xLibTraceLevel;
}

int xLibDebugDumpSizes (void)
{
  printf("\r\n");
  printf("\r\n xLibWa_t -------- %d", sizeof(xLibWa_t));
  printf("\r\n xLibObj_t -------- %d", sizeof(xLibObj_t));
  printf("\r\n xLibKey_t -------- %d", sizeof(xLibKey_t));
  printf("\r\n xLibFilter_t -------- %d", sizeof(xLibFilter_t));
  printf("\r\n");
  return 0;
}
