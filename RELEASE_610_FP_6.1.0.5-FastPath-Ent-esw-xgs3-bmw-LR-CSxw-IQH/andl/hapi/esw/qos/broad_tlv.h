/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2002-2007
*
**********************************************************************
*
* Name: broad_tlv.h
*
* @purpose This file contains the prototypes for TLV functions
*
* Component: hapi
*
* Comments:
*
* Created by: grantc 7/24/02
*
*********************************************************************/
#ifndef INCLUDE_BROAD_TLV_H
#define INCLUDE_BROAD_TLV_H

/********************************************************************
* MACROS
********************************************************************/

#define GET_NEXT_TLV(pTLV)  ((L7_tlv_t *)(osapiNtohs(pTLV->length) + (L7_uchar8 *)pTLV + sizeof(pTLV->length) + sizeof(pTLV->type)))

#define GET_SUB_TLV(pTLV,x) ((L7_tlv_t *)((L7_uchar8 *)pTLV + sizeof(pTLV->length) + sizeof(pTLV->type) + x))

#define GET_VALUE_PTR(pTLV,x) ((L7_uchar8 *)pTLV + sizeof(pTLV->length) + sizeof(pTLV->type) + x)

#endif /* INCLUDE_BROAD_TLV_H */
