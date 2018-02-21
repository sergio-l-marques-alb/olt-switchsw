//To Create L3 Interface
int create_l3_intf(int unit, int flags, int vlan, bcm_mac_t my_mac_addr, int *intf)
{
   int rc;
   bcm_l3_intf_t l3if;
   bcm_l3_intf_t_init(l3if);
   print my_mac_addr;
   //MAC of the L3 Interface
   //If InRif-> This MAC MUST be the sames of DMAC of incomming Packet!
   //If OutRif-> This MAC will be the SMAC of outgoing Packet!!
   sal_memcpy(l3if.l3a_mac_addr, my_mac_addr, 6);
   //This field set 12LSB Bits of the MAC Adress! The global bits are set using bcm_l2_station_add
   l3if.l3a_vid = vlan; //The VLAN/VSI of the InterFace
   l3if.l3a_inner_vlan = BCM_VLAN_NONE;
   l3if.l3a_ttl = 31;
   l3if.l3a_mtu = 1524;
   //l3if.l3a_intf_id = *intf;
   l3if.l3a_flags = flags;
   //Creates a RIF
   rc = bcm_l3_intf_create(unit, l3if);
   *intf = l3if.l3a_intf_id;

   return rc;
}

//To Create L3 Interface
int delete_l3_intf(int unit, int flags, int vlan /*-VSI-*/, bcm_mac_t my_mac_addr, int intf)
{
   int rc;
   bcm_l3_intf_t l3if;
   bcm_l3_intf_t_init(l3if);
   print my_mac_addr;
   //MAC of the L3 Interface
   //If InRif-> This MAC MUST be the sames of DMAC of incomming Packet!
   //If OutRif-> This MAC will be the SMAC of outgoing Packet!!
   sal_memcpy(l3if.l3a_mac_addr, my_mac_addr, 6);
   //This field set 12LSB Bits of the MAC Adress! The global bits are set using bcm_l2_station_add
   l3if.l3a_vid = vlan; //The VLAN/VSI of the InterFace
   l3if.l3a_inner_vlan = BCM_VLAN_NONE;
   l3if.l3a_ttl = 31;
   l3if.l3a_mtu = 1524;
   l3if.l3a_intf_id = intf;
   l3if.l3a_flags = flags;
   //Creates a RIF
   rc = bcm_l3_intf_delete(unit, l3if);

   return rc;
}

int create_l3_ingress(int unit, uint32 flags, int l3_intf)
{
   int rc;
   bcm_l3_ingress_t l3_ing_intf;                          

   bcm_l3_ingress_t_init(&l3_ing_intf);
   l3_ing_intf.flags = flags | BCM_L3_INGRESS_WITH_ID;
   
   rc = bcm_l3_ingress_create(unit, l3_ing_intf, &l3_intf);
   
   return rc;
}

int create_l3_egress(int unit, uint32 flags, int port, int vlan /*RIF-VSI*/, int l3_intf, bcm_mac_t nh_mac_addr, int *l3egid)
{
   //Create a NEXT HOP Entry _> FEC DB; EEP, EEDB PP321PPUniv

   int rc;
   /*Couples DestPort, NxtHope and L3 Interface. (L3 Interface can be VSI or Tunnel Interface)*/
   bcm_l3_egress_t l3eg;
   bcm_l3_egress_t_init(l3eg);

   l3eg.intf = l3_intf;//L3 Interface ID (contains VSI-RIF or Tunnel-RIF).
   //If Tunnel-RIF: EncapID points Outgoing MPLS Tunnel
   //If VSI-RIF : then VLAN=VSI-RIF, and EncapID points to outgoing LL including VSI
   sal_memcpy(l3eg.mac_addr, nh_mac_addr, 6);//MAC next Hope DMAC
   l3eg.vlan   = vlan; //Next Hope VLAN.   ==0 is taken from OutRif. !0, the allocated LL entry will include <DA+VLAN> entries
   l3eg.port   = port; //Outgoing egress Port. Port that packet will be sent
   // l3eg.encap_id = *encap_id; /*encap_id is the pointer that connects ForwardEquivalemtClass to EgressEncapsulationPointer*/
   //Returned Pointer to Egress Encapsulation LL LinkLayer Information//ARP POINTER
   //Creates a FEC (if_id)
   //Creates a Tunell API MPLS/IP where object is encap_id
   rc = bcm_l3_egress_create(unit, flags, &l3eg, l3egid);
   //                                       ||       ||-->FEC_ID
   print *l3egid;//FEC ID

   return rc;
}

