#ifndef EAV_PTP_H
#define EAV_PTP_H

#define EAV_TIMESYNC_ETHERTYPE 0x88F7

typedef struct ptp_timestamp_s {
  uint8 seconds[6];
  uint8 nanoseconds[4];  /** not on word boundary, no direct access */
  uint16 timescaleoffset;
} __attribute__ ((__packed__)) ptp_timestamp_t;

typedef struct ptp_portidentity_s {
  uint8 clockIdentity[8];
  uint16 portNumber;
} __attribute__ ((__packed__)) ptp_portidentity_t;

typedef struct ptp_header_s {
#ifdef LE_HOST
  uint8 messageType:4;
  uint8 transportSpecific:4;

  uint8 versionPTP:4;
  uint8 reserved0:4;
  
  uint16 messageLength;
  uint8 domainNumber;
  uint8 reserved1;
  uint16 flags;

  uint8 correctionField[8];
  uint8 reserved2[4];
  ptp_portidentity_t sourcePortIdentity;

  uint16 sequenceID;
  uint8 control;
  uint8 logMeanMessageInterval;
#else
  uint8 transportSpecific:4;
  uint8 messageType:4;
  uint8 reserved0:4;
  uint8 versionPTP:4;
  
  uint16 messageLength;
  uint8 domainNumber;
  uint8 reserved1;
  uint16 flags;

  uint8 correctionField[8];
  uint8 reserved2[4];
  ptp_portidentity_t sourcePortIdentity;

  uint16 sequenceID;
  uint8 control;
  uint8 logMeanMessageInterval;
#endif
} __attribute__ ((__packed__)) ptp_header_t;

typedef struct ptp_sync_payload_s {
  ptp_header_t header;
  ptp_timestamp_t timestamp;  
}  __attribute__ ((__packed__)) ptp_sync_payload_t;

#define PTP_SYNC_EXTEND_MAGIC_WORD 0xBEAADFFB

typedef struct ptp_sync_extend_payload_s {
  ptp_header_t header;
  ptp_timestamp_t timestamp;
  uint32 magicword;
  uint32 nhops;
  struct {
    uint64 ingresstime;
    uint64 egresstime;
    uint32 pdelay;
  } info[8];
}  __attribute__ ((__packed__)) ptp_sync_extend_payload_t;


typedef struct ptp_pdelayresp_s {
  ptp_timestamp_t timestamp;
  ptp_portidentity_t portid;
} __attribute__ ((__packed__)) ptp_pdelayresp_t;


/** pdelay request */
typedef struct ptp_pdelayreq_payload_s {
  ptp_header_t header;
  uint8 resv0[12];
} __attribute__ ((__packed__)) ptp_pdelayreq_payload_t;


/** pdelay respond */
typedef struct ptp_pdelayresp_payload_s {
  ptp_header_t header;
  ptp_pdelayresp_t resp;
  uint8 resv0[2];
} __attribute__ ((__packed__)) ptp_pdelayresp_payload_t;


/** pdelay respond follow up */
typedef struct ptp_pdelayrespfollowup_payload_s {
  ptp_header_t header;
  ptp_pdelayresp_t resp;
  uint8 resv0[2];
} __attribute__ ((__packed__)) ptp_pdelayrespfollowup_payload_t;

#endif
