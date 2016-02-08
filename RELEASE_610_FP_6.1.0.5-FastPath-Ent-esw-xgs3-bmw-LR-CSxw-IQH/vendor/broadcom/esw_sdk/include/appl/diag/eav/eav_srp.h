#include <bcm/types.h>

#ifndef EAV_SRP
#define EAV_SRP

#define SRP_RESERVATION_DEST_MAC(a) { (a)[0] = 1; (a)[1] = 0x80; (a)[2] = 0xC2;\
                                      (a)[3] = 0; (a)[4] = 0;    (a)[5] = 0x22; }

#define SRP_REGISTRATION_DEST_MAC(a) { (a)[0] = 1; (a)[1] = 0x80; (a)[2] = 0xC2;\
                                      (a)[3] = 0; (a)[4] = 0;    (a)[5] = 0x20; }


#define SRP_RESERVATION_ETHERTYPE   0x88b6
#define SRP_REGISTRATION_ETHERTYPE  0x88f6

#define EAV_MAX_NUMBER_OF_PORTS   5

typedef struct srp_registration_s {
  uint8 protocolVersion;
  uint8 attributeType;
#ifdef LE_HOST
  uint16 numberOfValue :12,
	 leaveAllEvent :4;
#else
  uint16 leaveAllEvent :4,
	 numberOfValue :12;
#endif
  uint8 streamID[6];
  uint8 attributeEvent;
  uint8 endMark;
} __attribute__ ((__packed__)) srp_registration_t;

typedef struct srp_rsv_header_s {
  
  uint8 protocolVersion;
  uint8 length;
  uint8 streamID[6];
 
} __attribute__ ((__packed__)) srp_rsv_header_t;

typedef struct srp_resource_requirement_s {
  
#ifdef LE_HOST
  uint16 maxPacketCount :10,
	      reserved1 :3,
	   trafficClass :3;
  uint16  maxPacketSize :11,
	      reserved2 :5;
#else
  uint16   trafficClass :3,
	      reserved1 :3,
	 maxPacketCount :10;
  uint16      reserved2 :5,
	  maxPacketSize :11;				  
#endif    
  uint16  reserved3;
} __attribute__ ((__packed__)) srp_resource_requirement_t;

typedef struct srp_reservation_status_s {
  uint8 reservationResult;
  uint8 reservationDelay[3];
} __attribute__ ((__packed__)) srp_reservation_status_t;

typedef  struct srp_reservation_hop_status_s {
  uint8 hopReservationStatus :2,
	       hopTechnology :6;
  uint8 hopEgressMACAddress[6];
  uint8 hopResourceStatus;
} __attribute__ ((__packed__)) srp_reservation_hop_status_t; 


typedef struct srp_reservation_payload_s {

  srp_rsv_header_t rsv;
  srp_resource_requirement_t rr;
  srp_reservation_status_t rstat;
  uint8 reservationHopCount;
  srp_reservation_hop_status_t hops; /** array of hops, just place one as the holder */

} __attribute__ ((__packed__)) srp_reservation_payload_t;


typedef struct srp_registration_test_packet_s {
  uint8 dmac[6];
  uint8 smac[6];
  uint16 etype;
  
  srp_registration_t reg;
  uint8 padding[38];
} __attribute__ ((__packed__)) srp_registration_test_packet_t;

typedef struct srp_registration_testv_packet_s {
  uint8 dmac[6];
  uint8 smac[6];
  uint8 vlantag[4];
  uint16 etype;

  srp_registration_t reg;
  uint8 padding[38];
} __attribute__ ((__packed__)) srp_registration_testv_packet_t;

typedef struct srp_reservation_test_packet_s {
  uint8 dmac[6];
  uint8 smac[6];
  uint16 etype;
  
  srp_rsv_header_t rsv;
  srp_resource_requirement_t rr;
  srp_reservation_status_t rstat;
  uint8 reservationHopCount;
  srp_reservation_hop_status_t hops[2];
  uint8 padding[15];
} __attribute__ ((__packed__)) srp_reservation_test_packet_t;



typedef struct srp_reservation_testv_packet_s {
  uint8 dmac[6];
  uint8 smac[6];

  uint8 vlantag[4];
  uint16 etype;
  
  srp_rsv_header_t rsv;
  srp_resource_requirement_t rr;
  srp_reservation_status_t rstat;
  uint8 reservationHopCount;
  srp_reservation_hop_status_t hops[2];
  uint8 padding[15];
} __attribute__ ((__packed__)) srp_reservation_testv_packet_t;


/** attribute event type */
enum {
  SRP_ATTRIBUTE_EVENT_NEW = 0,
  SRP_ATTRIBUTE_EVENT_JOININ = 1 * 36,
  SRP_ATTRIBUTE_EVENT_IN = 2 * 36,
  SRP_ATTRIBUTE_EVENT_JOINMT = 3 * 36,
  SRP_ATTRIBUTE_EVENT_MT = 4 * 36,
  SRP_ATTRIBUTE_EVENT_LV = 5 * 36
};

#endif
