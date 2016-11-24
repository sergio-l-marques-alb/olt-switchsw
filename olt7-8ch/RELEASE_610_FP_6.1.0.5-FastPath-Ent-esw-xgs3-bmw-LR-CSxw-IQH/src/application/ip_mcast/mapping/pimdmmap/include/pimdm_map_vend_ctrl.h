/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
*
* @filename    pimdm_map_vend_ctrl.h
*
* @purpose     PIMDM vendor-specific internal definitions
*
* @component   PIMDM Mapping Layer
*
* @comments    none
*
* @create      02/04/2002
*
* @author      gkiran
*
* @end
*
**********************************************************************/
#ifndef _PIMDM_VEND_CTRL_H_
#define _PIMDM_VEND_CTRL_H_

/*******************************************************************************
**                    Data Structure Definitions                              **
*******************************************************************************/

/* Hello Interval */
typedef struct pimdmMapHelloIntrvl_s
{
  L7_uint32 rtrIfNum;
  L7_uint32 intrvl;
}pimdmMapHelloIntrvl_t;

/* Triggered Hello Delay */
typedef struct pimdmMapTrigHelloDelay_s
{
  L7_uint32 rtrIfNum;
  L7_uint32 triggeredHelloDelay;
}pimdmMapTrigHelloDelay_t;

/* Hello Holdtime */
typedef struct pimdmMapHelloHoldTime_s
{
  L7_uint32 rtrIfNum;
  L7_uint32 helloHoldTime;
}pimdmMapHelloHoldTime_t;

/* Propagation Delay */
typedef struct pimdmMapPropagationDelay_s
{
  L7_uint32 rtrIfNum;
  L7_uint32 propagationDelay;
}pimdmMapPropagationDelay_t;

/* Override Interval */
typedef struct pimdmMapOverrideIntrvl_s
{
  L7_uint32 rtrIfNum;
  L7_uint32 overrideIntvl;
}pimdmMapOverrideIntrvl_t;

/* Interface Admin Mode */
typedef struct pimdmMapIntfMode_s
{
  L7_uint32      rtrIfNum;
  L7_uint32      intIfNum;
  L7_uint32      mode;
  L7_uint32      helloInterval;
  L7_uint32      triggeredHelloDelay;
  L7_uint32      helloHoldTime;
  L7_uint32      propagationDelay;
  L7_uint32      overrideIntvl;
  L7_inet_addr_t intfAddr;
  L7_inet_addr_t intfMask;
}pimdmMapIntfMode_t;

/* Global Admin Mode */
typedef struct pimdmMapAdminMode_s
{
  L7_uint32 mode;
}pimdmMapAdminMode_t;

/* Mapping Layer to Vendor Code Interfacing Structure */
typedef struct pimdmMapEvent_s
{
  union
  {
    pimdmMapIntfMode_t         intfMode;         /* Interface Admin Mode */
    pimdmMapAdminMode_t        adminMode;        /* Global Admin Mode */
    pimdmMapHelloIntrvl_t      helloIntrvl;      /* Hello Interval */
    pimdmMapTrigHelloDelay_t   trigHelloDelay;   /* Triggered Hello Delay */
    pimdmMapHelloHoldTime_t    helloHoldTime;    /* Hello Hold Time */
    pimdmMapPropagationDelay_t propagationDelay; /* Propagation Delay */
    pimdmMapOverrideIntrvl_t   overrideIntrvl;   /* Override Interval */
  }msg;
} pimdmMapEvent_t;

/*********************************************************************
* @purpose  To get the vendor(protocol) CB handle based on family Type
*
* @param    familyType    @b{(input)} Address Family type.
*
* @returns  cbHandle return cbHandle, else L7_NULLPTR
*
* @comments none
*
* @end
*********************************************************************/
MCAST_CB_HNDL_t pimdmMapProtocolCtrlBlockGet(L7_uchar8 familyType);

#endif /* _PIMDM_VEND_CTRL_H_ */
