/*
 * $Id: cint_system_vswitch.c,v 1.13 Broadcom SDK $
 * $Copyright: Copyright 2015 Broadcom Corporation.
 * This program is the proprietary software of Broadcom Corporation
 * and/or its licensors, and may only be used, duplicated, modified
 * or distributed pursuant to the terms and conditions of a separate,
 * written license agreement executed between you and Broadcom
 * (an "Authorized License").  Except as set forth in an Authorized
 * License, Broadcom grants no license (express or implied), right
 * to use, or waiver of any kind with respect to the Software, and
 * Broadcom expressly reserves all rights in and to the Software
 * and all intellectual property rights therein.  IF YOU HAVE
 * NO AUTHORIZED LICENSE, THEN YOU HAVE NO RIGHT TO USE THIS SOFTWARE
 * IN ANY WAY, AND SHOULD IMMEDIATELY NOTIFY BROADCOM AND DISCONTINUE
 * ALL USE OF THE SOFTWARE.  
 *  
 * Except as expressly set forth in the Authorized License,
 *  
 * 1.     This program, including its structure, sequence and organization,
 * constitutes the valuable trade secrets of Broadcom, and you shall use
 * all reasonable efforts to protect the confidentiality thereof,
 * and to use this information only in connection with your use of
 * Broadcom integrated circuit products.
 *  
 * 2.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS
 * PROVIDED "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
 * REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY,
 * OR OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
 * DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
 * NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
 * ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
 * CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
 * OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
 * 
 * 3.     TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
 * BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL,
 * INCIDENTAL, SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER
 * ARISING OUT OF OR IN ANY WAY RELATING TO YOUR USE OF OR INABILITY
 * TO USE THE SOFTWARE EVEN IF BROADCOM HAS BEEN ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGES; OR (ii) ANY AMOUNT IN EXCESS OF
 * THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF OR USD 1.00,
 * WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY NOTWITHSTANDING
 * ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.$
 *
 * System resource encoding example
 * Following examples are provided to show the encoding of BCM APIs
 * over some of the system objects.
 */

/* from gport bits 21:0 are used for resource value rest of bits used for encoding */


int SYS_VSWITCH_VLAN_PORT_INGRESS_EGRESS_TYPES = (1<<11) | (1<<10);/* bits 11 and 10 in vlan/mpls_port_id are set*/
int EGRESS_ONLY = 2048; /* represents the state in which bit 11 in vlan/mpls_port_id is set, bit 10 is unset*/
int INGRESS_ONLY = 1024; /* represents the state in which bit 10 in vlan/mpls_port_id is set, bit 11 is unset*/
int FORWARDING_GROUP_TYPE = (1<<22); /* represents the state in which bit 22 in vlan/mpls_port_id is set */
int PUSH_PROFILE_TYPE = (1<<24); /* represents the state in which bit 24 in vlan/mpls_port_id is set */

int encoding_gport_id_resouce_bits = 0x3FFFFF;
/* encoding utils */
int encode_gport_id_to_resource_val(bcm_gport_t gport_id){
  return gport_id & encoding_gport_id_resouce_bits;
}

struct mpls_push_profile_info_t{
  int exp;
  int ttl;
  int cw;
  int qos;
  int flags;
  int valid;
};

mpls_push_profile_info_t mpls_push_profiles[7];

int fill_push_profile_info(bcm_mpls_egress_label_t *egress_label,
                           int is_cw, 
                           mpls_push_profile_info_t *profile_info)
{
  profile_info->exp   = egress_label->exp;
  profile_info->ttl   = egress_label->ttl;
  profile_info->cw    = is_cw;
  profile_info->qos   = egress_label->qos_map_id;
  profile_info->flags = egress_label->flags;
  profile_info->valid = 1;

  return BCM_E_NONE;
}

int match_push_profiles(mpls_push_profile_info_t *push_profile_info1,
                        mpls_push_profile_info_t *push_profile_info2)
{
  return (push_profile_info1->exp   == push_profile_info2->exp &&
        push_profile_info1->ttl   == push_profile_info2->ttl &&
        push_profile_info1->cw    == push_profile_info2->cw &&
        push_profile_info1->qos   == push_profile_info2->qos &&
        push_profile_info1->flags == push_profile_info2->flags &&
        push_profile_info1->valid == push_profile_info2->valid);
}

int allocate_push_profile(bcm_mpls_egress_label_t *egress_label,int is_cw)
{
  mpls_push_profile_info_t profile_info;
  int i, is_match;

  fill_push_profile_info(egress_label, is_cw, &profile_info);
  
  for (i = 0; i < 7; i++) {
    is_match = 0;
    if (match_push_profiles(&profile_info, &(mpls_push_profiles[i])))        
    {
      is_match = 1;
      break;
    }      
  }

  /* allocate in case of no match */
  if (!is_match) {
    for (i = 0; i < 7; i++) {
      if (!mpls_push_profiles[i].valid) {
        mpls_push_profiles[i] = profile_info;
        break;
      }
    }    
  }
}


int encode_mpls_port_to_push_profile_val(bcm_mpls_port_t *mpls_port){
  mpls_push_profile_info_t push_profile_info;
  int i, is_match;

  /* Find matched push_profile */
  fill_push_profile_info(&(mpls_port->egress_label), 
                         (mpls_port->flags & BCM_MPLS_PORT_CONTROL_WORD),
                         &push_profile_info);
  for (i = 0; i < 7; i++) {
    is_match = 0;
    if (match_push_profiles(&push_profile_info, &(mpls_push_profiles[i])))        
    {
      return i;
    }      
  }
  return -1;
}

/* 
 * MPLS EEI: 
 * _BCM_DPP_ENCAP_ID_VC_SET(phy_port->encap_id,mpls_port->egress_label.label,push_profile); 
 */
int _BCM_DPP_ENCAP_ID_PUSH_PROFILE_SHIFT         =  20;
int encode_mpls_port_to_eei_resrouce_val(bcm_mpls_port_t *mpls_port){
  int vc_label;
  int push_profile;
  int eei = 0;
  print *mpls_port;
  print mpls_push_profiles;
  push_profile = encode_mpls_port_to_push_profile_val(mpls_port);

  eei |= mpls_port->match_label & 0xFFFFF; /* 20 bits */
  eei |= (push_profile << _BCM_DPP_ENCAP_ID_PUSH_PROFILE_SHIFT);
  return eei;
}


/* 
 * 
 *  encap_id in forward info contain either
 *  - outLIF: pointer to egress EEDB, AC, PWE-pointer, etc.. 
 *  - EEI:    VC label, ISID, etc.. 
 *  - outlif_type. needed for remote encap for p2p
 *       as in this case need to set in HW what is the
 *       the peer gport type: ac2pwe, ac2ac, ac2isid
 *       and by pointer itself it cannot be identified
 *       so need to encode this information into encap-id
 *
 *  REMARK: defines at SDK\include\bcm_int\dpp\gport_mgmt.h
 */
 
int SYS_VSWITCH_GPORT_ENCAP_USAGE_GENERAL       =  0;
int SYS_VSWITCH_GPORT_ENCAP_USAGE_PWE           =  1;
int SYS_VSWITCH_GPORT_ENCAP_USAGE_ISID          =  2;
int SYS_VSWITCH_GPORT_ENCAP_USAGE_AC            =  3;
 
int encode_eei_gport_info_into_encap_id(int outlif_id, int outlif_type, int eei, int is_eei){
  int encap_id = 0;
  if (is_eei) {
    encap_id = (1 << 30) | eei;
  }
  else {
    encap_id = outlif_id;
  }

  encap_id |= outlif_type << 28;

  return encap_id;
}


/* 
 * The following utility functions replaces the call to bcm_l2_gport_frwrd_info_get,
 * by mapping the VLAN PORT gport info to frwrd-info directly
 * handling seperated into 3 cases according to gport-id and vlan_port usage.
 *  A) 1:1 porection: Forwarding = FEC.
 *  B) 1+1 proection: Forwarding = Multicast
 *  C) no protection: Forwarding = <Port, Outlif>
 */
int system_vlan_port_to_l2_gport_forward_info(
   int unit,
   bcm_vlan_port_t  *vlan_port,
   bcm_l2_gport_forward_info_t *frwrd_info)
{

    uint32 outlif=0;
    uint32 eei=0;
    uint8 is_eei=0;
    uint32 fec_id;
    uint32 mc_id;

    bcm_l2_gport_forward_info_t_init(frwrd_info);

    /* for vlan port forwarding informaton is always out_lif*/
    is_eei = 0;

    /* 
     * A) in case of 1:1 protection the forwarding information is FEC
     * the FEC id is passed in the vlan_port_id.
     */

    /* 1:1 protection identified if failover-id != 0*/
    if (vlan_port->failover_id != 0) {
        if (!(vlan_port->flags & BCM_VLAN_PORT_WITH_ID)) {
            printf("Error, WITH_ID flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        fec_id = encode_gport_id_to_resource_val(vlan_port->vlan_port_id);
        BCM_GPORT_FORWARD_PORT_SET(frwrd_info->phy_gport,fec_id);
        frwrd_info->encap_id = -1;
    }

    /* 
     * B) in case of 1+1 protection the forwarding information is Multicast (Bi-cast)
     * the Multicast id is passed in the vlan_port_id.
     */

    /* 1+1 protection identified if failover_mc_group is valid MC group*/
    else if (vlan_port->failover_mc_group != 0) {
        if (!(vlan_port->flags & BCM_VLAN_PORT_WITH_ID)) {
            printf("Error, WITH_ID flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        mc_id = encode_gport_id_to_resource_val(vlan_port->vlan_port_id);
        BCM_GPORT_MCAST_SET(frwrd_info->phy_gport,mc_id);        
        frwrd_info->encap_id = -1;
    }
    else{
        /* 
         * C) otherwise no protection the forwarding information is <port + outLIF>
         * the outLIF id is passed in the encap_id or on the vlan_port_id.
         */
        if (vlan_port->flags & BCM_VLAN_PORT_ENCAP_WITH_ID) {
            outlif = BCM_ENCAP_ID_GET(vlan_port->encap_id);
        }
        else if (vlan_port->flags & BCM_VLAN_PORT_WITH_ID) {
            outlif = encode_gport_id_to_resource_val(vlan_port->vlan_port_id);
        }
        else{
            printf("Need to supply WITH_ID or ECAN_WITH_ID flags\n"); 
            return BCM_E_PARAM;
        }
        frwrd_info->phy_gport = vlan_port->port;
        frwrd_info->encap_id = encode_eei_gport_info_into_encap_id(outlif,SYS_VSWITCH_GPORT_ENCAP_USAGE_AC,eei,is_eei);
    }

    

    return BCM_E_NONE;
}

/* 
 * The following utility functions replaces the call to bcm_l2_gport_frwrd_info_get,
 * by mapping the MPLS PORT gport info to frwrd-info directly
 * handling seperated into 3 cases according to gport-id and vlan_port usage.
 *  A) 1:1 PWE porection: Forwarding = FEC.
 *  B) 1+1 PWE proection: Forwarding = Multicast
 *  C) MPLS tunnel protection: <L3 FEC, EEI-VC>
 *  D) No protection: <Port, PWE-Outlif>  
 */
int system_mpls_port_to_l2_gport_forward_info(
   int unit,
   bcm_mpls_port_t  *mpls_port,
   bcm_l2_gport_forward_info_t *frwrd_info)
{

    uint32 outlif=0;
    uint32 eei=0;
    uint8 is_eei=0;
    uint32 fec_id;
    uint32 mc_id;

    bcm_l2_gport_forward_info_t_init(frwrd_info);

    /* 
     * A) in case of 1:1 protection the forwarding information is FEC
     * the FEC id is passed in the mpls_port_id.
     */

    /* 1:1 protection identified if failover-id != 0*/
    if (mpls_port->failover_id != 0) {
        if (!(mpls_port->flags & BCM_MPLS_PORT_WITH_ID)) {
            printf("Error, WITH_ID flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        fec_id = encode_gport_id_to_resource_val(mpls_port->mpls_port_id);
        BCM_GPORT_FORWARD_PORT_SET(frwrd_info->phy_gport,fec_id);
        frwrd_info->encap_id = -1;
    }

    /* 
     * B) in case of 1+1 protection the forwarding information is Multicast (Bi-cast)
     * the Multicast id is passed in the mpls_port_id.
     */

    /* 1+1 protection identified if failover_mc_group is valid MC group*/
    else if (mpls_port->failover_mc_group != 0) {
        if (!(mpls_port->flags & BCM_MPLS_PORT_WITH_ID)) {
            printf("Error, WITH_ID flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        mc_id = encode_gport_id_to_resource_val(mpls_port->mpls_port_id);
        BCM_GPORT_MCAST_SET(frwrd_info->phy_gport,mc_id);        
        frwrd_info->encap_id = -1;
    }
    /* C) MPLS protection , check if egress tunnel_if encoded is FEC */
    else if ((mpls_port->egress_tunnel_if & 0xff00000) == 0) {
        if (!(mpls_port->flags & BCM_MPLS_PORT_EGRESS_TUNNEL)) {
            printf("Error, EGRESS_TUNNEL flag has to be set\n"); 
            return BCM_E_PARAM;
        }
        /* L3 MPLS FEC */
        fec_id = encode_gport_id_to_resource_val(mpls_port->egress_tunnel_if);
        BCM_GPORT_FORWARD_PORT_SET(frwrd_info->phy_gport,fec_id);
        is_eei = 1;
        eei = encode_mpls_port_to_eei_resrouce_val(mpls_port);
        frwrd_info->encap_id =encode_eei_gport_info_into_encap_id(eei,SYS_VSWITCH_GPORT_ENCAP_USAGE_PWE,eei,is_eei);
    } else {
        /* 
         * D) otherwise no protection the forwarding information is <port + outLIF>
         * the outLIF id is passed in the encap_id or on the mpls_port_id.
         */
        if (mpls_port->flags & BCM_MPLS_PORT_ENCAP_WITH_ID) {
            outlif = BCM_ENCAP_ID_GET(mpls_port->encap_id);
        }
        else if (mpls_port->flags & BCM_MPLS_PORT_WITH_ID) {
            outlif = encode_gport_id_to_resource_val(mpls_port->mpls_port_id);
        }
        else{
            printf("Need to supply WITH_ID or ENCAP_WITH_ID flags\n"); 
            return BCM_E_PARAM;
        }
        frwrd_info->phy_gport = mpls_port->port;
        is_eei = 0;
        frwrd_info->encap_id = encode_eei_gport_info_into_encap_id(outlif,SYS_VSWITCH_GPORT_ENCAP_USAGE_PWE,eei,is_eei);
    }

    return BCM_E_NONE;
}


/*  
 * The following utility function shows and replaces the encoding of vlan_port_id in bcm_vlan_port_create 
 * gport is encoded according to 3 cases:
 *  A) 1+1  protection: Multicast-ID .
 *  B) 1:1 protection: FEC-Pointer
 *  C) No protection: LIF_ID
 */
int system_vlan_port_to_vlan_port_id(int unit, bcm_vlan_port_t *vp , bcm_gport_t *gport){

	int aux_gport = BCM_GPORT_VLAN_PORT;/* gport type*/
	int failover_id_bit = 0; /* marks bit 21 in case of 1+1 protection*/
	int object_type = 0;
	int type = 0;

	failover_id_bit = (vp->ingress_failover_id) ? 1 : 0;


	return system_aux_object_to_object_id(type, aux_gport, vp->failover_id, vp->failover_mc_group, gport, object_type, failover_id_bit, vp->flags);

}



/* 
 * The following utility function shows and replaces the encoding of mpls_port_id in bcm_mpls_port_add
 * gport is encoded according to 3 cases, similar to the above functin's specification
 */
int system_mpls_port_to_mpls_port_id(int unit, bcm_mpls_port_t *mp , bcm_gport_t *gport){

	int aux_gport = BCM_GPORT_MPLS_PORT;/* gport type*/
	int one_plus_one_protection_mask = 255;
	int failover_id_bit = 0; /* marks bit 21 in case of 1+1 protection*/
	int object_type = 1;
	int type = 0;

	one_plus_one_protection_mask <<= 24; /* 8 MSBs are set*/

	/* failover_id_bit is set iff bits 24-31 in mp->failover_mc_group represent a non-zero number*/
	failover_id_bit = (mp->failover_mc_group & one_plus_one_protection_mask) ? 1 : 0; 


	return system_aux_object_to_object_id(type, aux_gport, mp->failover_id, mp->failover_mc_group, gport, object_type, failover_id_bit, mp->flags);

}

/* 
 * The following utility function sets the egress only bit in the mpls/vpls_port_id gport
 */
int system_l2_port_id_egress_only_set(bcm_gport_t *gport){

	*gport |= EGRESS_ONLY;
}

/* 
 * The following utility function sets the ingress only bit in the mpls/vpls_port_id gport
 */
int system_l2_port_id_ingress_only_set(bcm_gport_t *gport){

	*gport |= INGRESS_ONLY;
}


/* 
 * The following utility function shows and replaces the encoding of l3_egid in bcm_l3_egress_create,
 * egress_intf is built according to passed FEC-Pointer value
 */
int system_fec_to_fec_id(int unit, bcm_if_t *egress_intf){
	
	int type = 1;
	int object_type = 2;

	return system_aux_object_to_object_id(type, 0, 0, 0, egress_intf, object_type,0, 0);

}



/* 
 * The following utility function replaces the call to bcm_mpls_tunnel_initiator_create,
 * tunnel_id is built according to passed OUT_LIF value
 */
int system_mpls_tunnel_to_tunnel_id(int unit, bcm_if_t *tunnel_id){
	
	int type = 2;
	int object_type = 2;

	return system_aux_object_to_object_id(type, 0, 0, 0, tunnel_id, object_type,0, 0);

}

int mpls_tunnel_id_to_mpls_tunnel_gport(int unit, bcm_if_t * tunnel_id){
	return system_aux_object_to_object_id(0, 0, 0, 0, tunnel_id, 0,0, 0);
}


/* 
 * The following utility function replaces the call to bcm_l3_egress_create,
 * encap_id is built according to passed OUT_LIF value
 */
int system_link_layer_to_link_layer_encap_id(int unit, bcm_if_t *encap_id){

	int type = 2;
	int object_type = 2;

	return system_aux_object_to_object_id(type, 0, 0, 0, encap_id, object_type,0, 0);
}



/* 
 * The following utility function replaces the call to create_l3_intf,
 * encap_id is built according to passed RIF-ID value
 */
int system_rif_to_rif_id(int unit, int *ingress_intf){

	int type = 0;
	int object_type = 2;

	return system_aux_object_to_object_id(type, 0, 0, 0, ingress_intf, object_type,0, 0);
}



/* This is an auxiliary function for encoded objects, each usage is according to object_type
 * object_type == 0: object is mpls_port_id 
 * object_type == 1: object is vlan_port_id 
 * object_type == 2: object is bcm_intf_t (FEC, OutLIF, RIF) 
 * type: represents either sub-type field (in case of mpls/vlan_port_id) or the 29-31 bits in an interface 
 */

int system_aux_object_to_object_id(int type, int aux_gport, bcm_failover_t failover_id, bcm_multicast_t failover_mc_group, int *id, int object_type, int failover_id_bit, int flags){

	int ingress_egress_mask = SYS_VSWITCH_VLAN_PORT_INGRESS_EGRESS_TYPES;

	if (object_type == 2) { /* object is L3 interface type. Here we add sub_type and id and return */
		type <<= 29;
		*id |= type;

		return BCM_E_NONE;

	}

	failover_id_bit <<= 21;

	aux_gport <<= 26;

	/* 1:1 protection case*/
	if (failover_id) {
		*id |= aux_gport;
	}
 
	/* 1+1 protection case*/
	else if (failover_mc_group) {
		type = 3;
		type <<= 22;
		aux_gport |= type;

		aux_gport |= failover_id_bit;
		*id |= aux_gport; 	
	}

	/* No protection case*/
	else{
		type = 2;
		type <<= 22;
		aux_gport |= type;

		if (!object_type) { /* vlan_port_id*/
			ingress_egress_mask &= flags;
			aux_gport |= type;

			if (ingress_egress_mask == EGRESS_ONLY) {
				aux_gport |= EGRESS_ONLY;
			}
			else if (ingress_egress_mask == INGRESS_ONLY) {
				aux_gport |= INGRESS_ONLY;
			}
        } 

		*id |= aux_gport;
	}

	return BCM_E_NONE;

}


/* 
 *  This is an auxiliary function translates fec to forwarding group id
 */

int system_aux_fec_to_forwarding_group(int fec, int * forwarding_group){

    fec = (fec | FORWARDING_GROUP_TYPE);
    BCM_GPORT_MPLS_PORT_ID_SET(*forwarding_group, fec);

}

/* This is an auxiliary function for encoded objects, each usage is according to object_type
 * object_type == 0: object is mpls_port_id 
 * object_type == 1: object is vlan_port_id 
 * object_type == 2: object is bcm_intf_t (FEC, OutLIF, RIF) 
 * type: represents either sub-type field (in case of mpls/vlan_port_id) or the 29-31 bits in an interface 
 */

int system_aux_push_profile_to_push_profile_id(int push_profile, int * push_profile_id){

    push_profile = (push_profile | PUSH_PROFILE_TYPE);
    BCM_GPORT_MPLS_PORT_ID_SET(*push_profile_id, push_profile);

}
