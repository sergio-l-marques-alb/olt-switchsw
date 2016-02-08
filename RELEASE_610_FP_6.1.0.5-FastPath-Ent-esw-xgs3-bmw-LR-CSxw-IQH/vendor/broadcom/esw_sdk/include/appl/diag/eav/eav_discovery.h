#ifndef EAV_DISCOVERY_H

enum LLDP_TLV_HDR_TYPE {
  TLV_CHASSIS_HDR_TYPE = 1,
  TLV_PORT_HDR_TYPE    = 2,
  TLV_TTL_HDR_TYPE     = 3,
  TLV_SYSCAP_HDR_TYPE  = 7,
  TLV_ORGSP_HDR_TYPE   = 127,
  TLV_END_HDR_TYPE     = 0,
  TLV_SRP_RES_REQ_TYPE = 9,
  TLV_SRP_HOP_STATUS_TYPE = 10
};


enum LLDP_TLV_HDR_LENGTH {
  TLV_CHASSIS_HDR_LENGTH = 8,
  TLV_PORT_HDR_LENGTH    = 8,
  TLV_TTL_HDR_LENGTH     = 2,
  TLV_SYSCAP_HDR_LENGTH  = 4,
  TLV_ORGSP_HDR_LENGTH   = 8,
  TLV_END_HDR_LENGTH     = 0,
  TLV_SRP_RES_REQ_LENGTH = 4,
  TLV_SRP_HOP_STATUS_LENGTH = 8
};


#define LLDP_DEST_MAC(a) { a[0] = 1; a[1] = 0x80; a[2] = 0xC2;\
                           a[3] = 0; a[4] = 0;    a[5] = 0x0E; }

#define LLDP_ETHERTYPE  0x88CC

/** generic tlv hdr */
typedef struct lldp_tlv_hdr_s {
#ifdef LE_HOST
  uint16 length:7, tlv_type:9;
#else
  uint16 tlv_type:7, length:9;
#endif
  /*
  uint8      tlv_type; 
  uint8        length;
  */
} __attribute__ ((__packed__)) lldp_tlv_hdr_t;

/** chasis/port info */
typedef struct lldp_info_s {
  lldp_tlv_hdr_t  hdr;
  uint8       subtype;
  uint8 ascii_name[7];
}  __attribute__ ((__packed__)) lldp_info_t;

/** ttl info */
typedef struct lldp_ttl_s {
  lldp_tlv_hdr_t  hdr;
  uint16      ttl_sec;
}  __attribute__ ((__packed__)) lldp_ttl_t;

/** syscapabilities */
typedef struct lldp_syscap_s {
  lldp_tlv_hdr_t hdr;
#ifdef LE_HOST
    uint32        dc0:2, /** dc : dont care */
	      bridge0:1,
		  dc1:4,
		 nic0:1,
     en_capabilities0:8,
		  dc2:2,    
	      bridge1:1,
		  dc3:4,
		 nic1:1,
     en_capabilities1:8;
#else
    uint32 en_capabilities1:8,
		 nic1:1,
		  dc3:4,
	      bridge1:1,
		  dc2:2,    
     en_capabilities0:8,
		 nic0:1,
		  dc1:4,
	      bridge0:1,
		  dc0:2; /** dc : dont care */
#endif    
}  __attribute__ ((__packed__)) lldp_syscap_t;

/** organization specific info */
typedef struct lldp_org_specific_s {
  lldp_tlv_hdr_t hdr;
  uint8        oui[3];
  uint8       subtype;
  uint8       prot_id;
  uint8 ascii_name[3];
}  __attribute__ ((__packed__)) lldp_org_specific_t;

/** typical lldp header for using in eav */
typedef struct eav_lldp_typical_s {
  /** ether header */
  struct eth_hdr_s {
    uint8       dmac[6];
    uint8       smac[6];  
    uint16    ethertype;
  }  __attribute__ ((__packed__)) ethhdr;

  lldp_info_t            lldp_chss;
  lldp_info_t            lldp_port;
  lldp_ttl_t                   ttl;
  lldp_syscap_t             syscap;
  lldp_org_specific_t org_specific;

  lldp_tlv_hdr_t               end;

} __attribute__ ((__packed__)) eav_lldp_typical_t;

int eav_lldp_typical_init( eav_lldp_typical_t *pkt );

#endif
