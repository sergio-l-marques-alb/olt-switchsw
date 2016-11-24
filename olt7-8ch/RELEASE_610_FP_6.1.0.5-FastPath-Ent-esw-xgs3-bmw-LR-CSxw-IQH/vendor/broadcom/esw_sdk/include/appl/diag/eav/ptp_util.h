#include <appl/diag/eav/eav_ptp.h>
#include <appl/diag/eav/ptp_types.h>

#ifndef PTP_UTIL_H
#define PTP_UTIL_H

int ptp_build_MsgSync( );

int ptp_build_MsgFollowUp( );

int ptp_build_MsgPdelayReq( int unit, int seqid, uint8 *pbuf, int size, int *ppktSize );

int ptp_build_MsgPdelayResp( int unit, ptp_pdelayreq_payload_t *ppdreq, ptp_timestamp_t *ptimestamp, uint8 *pbuf, int size, int *ppktSize );

int ptp_build_MsgPdelayRespFollowup( int unit, ptp_pdelayreq_payload_t *ppdreq, ptp_timestamp_t *ptimestamp, uint8 *pbuf, int size, int *ppktSize );



#endif
