/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  dot1x_radius.h
*
* @purpose   dot1x RADIUS client interface header file
*
* @component dot1x
*
* @comments  none
*
* @create    04/08/2003
*
* @author    mfiorito
*
* @end
**********************************************************************/

#define DOT1X_MAC_ADDR_STR_LEN ((L7_MAC_ADDR_LEN * 2) + (L7_MAC_ADDR_LEN - 1))
#define DOT1X_SESSION_ID_LEN   DOT1X_USER_NAME_LEN * 2

typedef struct L7_radiusAttr_s
{
	L7_uchar8  type;
	L7_uchar8  length;	/* lengh of entire attribute including the type and length fields */
} L7_radiusAttr_t;

extern L7_RC_t dot1xRadiusResponseCallback(L7_uint32 status, L7_uint32 correlator,
                                             L7_uchar8 *attribues, L7_uint32 attributesLen);
extern L7_RC_t dot1xRadiusResponseProcess(L7_uint32 intIfNum, L7_uint32 status, L7_uchar8 *attributes,
                                          L7_uint32 attributesLen);
extern L7_RC_t dot1xRadiusAcceptProcess(L7_uint32 intIfNum, L7_uchar8 *radiusPayload, L7_uint32 payloadLen);
extern L7_RC_t dot1xRadiusChallengeProcess(L7_uint32 intIfNum, L7_uchar8 *radiusPayload, L7_uint32 payloadLen);
extern L7_RC_t dot1xRadiusAccessRequestSend(L7_uint32 intIfNum, L7_uchar8 *suppEapData);
extern L7_RC_t dot1xRadiusAccountingStart(L7_uint32 intIfNum);
extern L7_RC_t dot1xRadiusAccountingStop(L7_uint32 intIfNum, L7_uint32 termCause);
extern L7_RC_t dot1xRadiusSuppResponseProcess(L7_uint32 intIfNum, L7_netBufHandle bufHandle);
