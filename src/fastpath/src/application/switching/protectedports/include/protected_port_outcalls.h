/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2006-2007
*
**********************************************************************
* @filename  protected_port_outcalls.h
*
* @purpose   protected port outcalls file
*
* @component protectedPort 
*
* @comments
*
* @create    6/6/2005
*
* @author    ryadagiri 
*
* @end
*
**********************************************************************/
#ifndef PROTECTED_PORT_OUTCALLS_H
#define PROTECTED_PORT_OUTCALLS_H

/* Begin Function Prototypes */


L7_BOOL protectedPortIntfTypeIsValid(L7_uint32 sysIntfType);
L7_RC_t protectedPortIntfValid(L7_uint32 intIfNum);

/* End Function Prototypes */
#endif /* PROTECTED_PORT_OUTCALLS_H */
