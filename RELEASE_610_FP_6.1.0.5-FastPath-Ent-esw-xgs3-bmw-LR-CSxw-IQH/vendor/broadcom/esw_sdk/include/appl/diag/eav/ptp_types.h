/**
 * PTP protocol header file :::::
 *
 *
 * ethertype : 0x88F7
 * dst_mac   : {0,0x10,0x18,0xAA,0xBB,0xCC};
 * src_mac   : {0,0x10,0x18,0x53,0x95,0x00};
 */
#ifndef PTP_TYPES_H
#define PTP_TYPES_H
                        
/* ptp constants */
#define PTP_VERSION							2
#define PTP_DEFAULT_SUBDOMAIN				0
#define PTP_UUID_LENGTH                     6
#define PTP_CODE_STRING_LENGTH              4
#define PTP_SUBDOMAIN_NAME_LENGTH           16
#define PTP_MAX_MANAGEMENT_PAYLOAD_SIZE     90
#define PTP_TYPE_ETHERNET					1

/* ptp flags */
#define FLAG_SECURE					1<<7
#define FLAG_PTP_SYNC_BURST 		1<<6
#define FLAG_PTP_SYNC_BURST_REQ 	1<<6
#define FLAG_TIME_SCALE_ACCURATE 	1<<5
#define FLAG_UTC_REASONABLE 		1<<4
#define FLAG_TIME_APPPROXIMATE 		1<<3
#define FLAG_STANDBY_MASTER 		1<<2
#define FLAG_PTP_LI_59				1<<1
#define FLAG_PTP_LI_61 				1<<0

/* PTP Message Id's */
enum {
	PTP_SYNC = 0,
	PTP_DELAY_REQ = 1,                          /** implementing */
	PTP_PDELAY_REQ = 2,                         /** in current implementation */
	PTP_PDELAY_RESP = 3,                        /** in current implementation */
	PTP_FOLLOW_UP = 8,                          /** implementing */
	PTP_DELAY_RESP = 9,   
	PTP_PDELAY_RESP_FOLLOW_UP = 10,             /** in current implementation */
	PTP_ANNOUNCE = 11,
	PTP_TRANSPORT = 12,
	PTP_MANAGEMENT_MSG = 13,
	PTP_PDELAY_UPDATE = 14                      /** new message id to announce delay from master to slave */
};

/* PTP Control message types */
enum {
	PTP_CONTROL_SYNC = 0,
	PTP_CONTROL_DELAY_REQ,
	PTP_CONTROL_FOLLOWUP,
	PTP_CONTROL_DELAY_RESP,
	PTP_CONTROL_MANAGMENT
};

#if 0
typedef struct {
  /** software maintain this roll over, but it is not in seconds, but the upper 32 bits of nano second */
  uint32_t seconds;    

  /** direct hardware register time base, 32 bits, count in nano second.
      it is increment by a 25MHZ clock with 40 tick at a time.
      in adjustment, we can tweak how many ticks to increment at each clock for how many times.
      we need to work out an algorithm to use this capability.
    */
  int32_t nanoseconds;  
} TimeRepresentation;

/* Message header */
typedef struct {
  uint8_t      transportSpecific:4;
  uint8_t	       messageID:4;   /************** message type */
  uint8_t              reserved0:4;
  uint8_t  	      versionPTP:4;

  uint16_t 	     messageLength;
  uint8_t             domainNumber;
  uint8_t                reserved1;

  uint16_t                   flags;
  
#if 0
  uint8_t correctionField[8];
  uint8_t reserved2[4];
#endif

#if 0
  char 		reserved[3];
  uint8_t		subdomain;
  char		flags[3];
#endif

  uint8_t               __gap;
  uint8_t		logSyncPeriod;

  int32_t		correctionNsHigh;
  int16_t		correctionNsLow;
  uint16_t	correctionSubNs;
  char		sourceUuid[PTP_UUID_LENGTH];
  uint16_t	sourcePortId;
  uint8_t		sourceCommunicationTechnology;
  uint8_t		reserved2;
  uint16_t	sequenceID;          /************** requester's sequence id **/
  uint8_t		control;
  uint8_t		reserved3;  
} MsgHeader;

/* Sync message */
typedef struct {
	MsgHeader	header;
  	uint16_t  	epochNumber;
  	TimeRepresentation  originTimestamp;
  	int16_t  	currentUTCOffset;
} MsgSync;

/* Follow_Up message */
typedef struct { /*{ 0x01,0x00,0x7f,0xff,0xff,0xff }*/
	MsgHeader	header;
  	uint16_t  	epochNumber;
  	TimeRepresentation  preciseOriginTimestamp;
  	int16_t  	currentUTCOffset;
  	uint16_t  	associatedSequenceId;
} MsgFollowUp;

/* Announce message */
typedef struct {
	MsgHeader	header;
	uint16_t	epochNumber;
	TimeRepresentation originTimestamp;
	int16_t		currentUTCOffset;
	uint16_t	announceFlags;
	uint16_t	localStepsRemoved;
	char		grandmasterUuid[PTP_UUID_LENGTH];
  	uint16_t	grandmasterPortId;
  	uint8_t		grandmasterCommunicationTechnology;
  	uint8_t		reserved;
  	uint8_t		grandmasterStratum;
  	uint8_t		grandmasterIdentifier;
  	int16_t		grandmasterVariance;
  	uint16_t	grandmasterSequenceId;
  	char		masterUuid[PTP_UUID_LENGTH];
  	uint16_t	masterPortId;
  	uint8_t		masterCommunicationTechnology;
  	uint8_t		reserved1;
  	uint8_t		localClockStratum;
  	uint8_t		localClockIdentifier;
  	int16_t		localClockVariance;
  	int32_t		estimatedMasterClockPhaseChangeRate;
	int16_t		estimatedMasterVariance;
	int8_t		logAnnouncePeriod;
} MsgAnnounce;
	

/**********************************************************************/
/************************************************* Pdelay_Req message */
/*
 implementation in ::::
    timesync_receive_pdelay_req :: handle pdelay request
    ptp_build_MsgPdelayReq      :: utility for constructing the packet
    timesync_send_pdelayReq     :: send pdelay request from master port
 */
/**********************************************************************/
typedef struct{
	MsgHeader	header;
	char		reserved[28];
} MsgPdelayReq;



/**********************************************************************/
/************************************************ Pdelay_Resp message */
/*
 implementation in ::::
   timesync_receive_pdelay_resp :: handle pdelay respond and 
                                 and update the request state.
   ptp_build_MsgPdelayResp      :: utility for constructing the packet
 */
/**********************************************************************/

typedef struct{
  MsgHeader	header;   /** header with uniq seqid */

  uint16_t	requestingSequenceId; /** REQUESTER's seqid */

  /******* t2, the req arrived time in nano second field */
  TimeRepresentation	originTimestamp; 

  TimeRepresentation	requestReceiptTimestamp;
  char		requestingPortUuid[PTP_UUID_LENGTH];
  uint16_t	requestingPortPortId;
  uint8_t		requestingPortCommunicationTechnology;
  uint8_t		reserved;
} MsgPdelayResp;



/**********************************************************************/
/*********************************************** Pdelay_Resp_Followup */
/*
 implementation in ::::
   timesync_receive_pdelay_resp_follow :: handle pdelay respond follow and 
                                 and update the request state.
   ptp_build_MsgPdelayRespFollow     :: utility for constructing the packet
 */
/**********************************************************************/

typedef struct{
  MsgHeader	header;  /** header with uniq seqid */

  /******** this SEQID is the ORIGINAL requester's seqId */
  uint16_t  	associatedSequenceId;

  /******** t3, timestamp when resp packet sent *******/ 
  TimeRepresentation  preciseOriginTimestamp;
} MsgPdelayRespFollow;

#endif

#endif
