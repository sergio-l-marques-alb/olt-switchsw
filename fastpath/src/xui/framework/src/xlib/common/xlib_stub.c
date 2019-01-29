/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2000-2007
*
**********************************************************************
*
* @filename xlib_stub.c
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
#include <stdarg.h>
#include <string.h>

#include "xlib_private.h"
#include "_xeobj_xtype.h"

#define MAX_INSTANCE_COUNT 5

typedef struct entry_s
{
  struct entry_s *next;
  xLibId_t oid;
  xLibU16_t type;
  int nextCount;
  char value[MAX_INSTANCE_COUNT][256];
  char key[MAX_INSTANCE_COUNT][1024];
} entry_t;

static entry_t *head, *tail;

static entry_t *get_stub_entry (xLibWa_t * wap, xLibId_t oid)
{
  entry_t *ep;
  xLibU16_t i, type = 0;

  for (ep = head; ep; ep = ep->next)
  {
    if (ep->oid == oid)
      return ep;
  }

  for (i = 0; i < wap->objCount; i++)
  {
    if (wap->objs[i].oid == oid)
    {
      type = wap->objs[i].type;
      break;
    }
  }

  ep = (entry_t *) xLibMalloc(sizeof(*ep));
  ep->oid = oid;
  ep->type = type;
  if (head == NULL)
  {
    head = ep;
  }
  else
  {
    tail->next = ep;
  }
  tail = ep;

  for (i = 0; i < MAX_INSTANCE_COUNT; i++)
  {
    strcpy (ep->value[i], "Dummy");
  }
  return ep;
}

static void xLibStubFill (entry_t * ep, void *var)
{
  if (ep->type == XLIB_DTYPE_string)
  {
    strcpy (var, "string");
  }
  else if (ep->type == XLIB_DTYPE_int || ep->type == XLIB_DTYPE_uint)
  {
    int localint = 5;
    memcpy (var, &localint, 4);
  }
}

int xLibStubFunction (char *which, xLibWa_t * wap, xLibBuf_t * bufp, ...)
{
  int i;
  va_list ap;
  void *keys[10], *var;
  entry_t *ep = get_stub_entry (wap, bufp->oid);

  va_start (ap, bufp);
  for (i = 0; i < wap->keyCount; i++)
  {
    keys[i] = (void *) va_arg (ap, int);
  }
  var = (void *) va_arg (ap, int);
  va_end (ap);

  if (!strcmp (which, "get") || !strcmp (which, "first"))
  {
    xLibStubFill (ep, var);
    ep->nextCount = 0;
  }
  else if (!strcmp (which, "next"))
  {
    if (ep->nextCount >= MAX_INSTANCE_COUNT)
    {
      return 1;
    }
    xLibStubFill (ep, var);
    ep->nextCount++;
  }

  ep = NULL;
  return 0;
}
