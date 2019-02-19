/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2001-2007
*
**********************************************************************
* @filename  ospf_lsdb_api.c
*
* @purpose   Ospf Link State Database Api functions
*
* @component Ospf Mapping Layer
*
* @comments  This file includes getters & setters for ospf variables, 
*             which are specific to the link state database table
*
* @create    07/10/2001
*
* @author    anayar
*
* @end
*             
**********************************************************************/

/*************************************************************
                    
*************************************************************/



#include "l7_ospfinclude.h"

extern L7_ospfMapCfg_t    *pOspfMapCfgData;
                    
/*
***********************************************************************
*                     API FUNCTIONS  -  LSDB CONFIG
***********************************************************************
*/

/*----------------------------------------------------------------------*/
/* OSPF Link State Database                                             */
/* The Link State Database contains the Link State Advertisements from  */
/* throughout the areas that the device is attached to.                 */
/*----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get the process's Link State Database entry.
*
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
*                   
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single link-state Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapLsdbEntryGet ( L7_uint32 AreaId, L7_uint32 Type,
                              L7_uint32 Lsid, L7_uint32 RouterId,
                              L7_ospfLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenLsdbEntryGet(AreaId, Type, Lsid, RouterId, p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Link State Database entry.
*
* @param    *AreaId     type
* @param    *Type       entry
* @param    *Lsid       Link State id
* @param    *RouterId   Database entry
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single link-state Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapLsdbEntryNext ( L7_uint32 *AreaId, L7_uint32 *Type,
                               L7_uint32 *Lsid, L7_uint32 *RouterId,
                               L7_ospfLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenLsdbEntryNext(AreaId, Type, Lsid, RouterId, p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA type for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA type
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The type of link state advertisment.  Each link state
*             type has a separate advertisement format.
*
*             Link State Advertisement Types:
*               routerLink  (1)
*               networkLink (2)
*               summaryLink (3)
*               asSummaryLink (4)
*               asExternalLink (5)
*               multicastLink (6)
*               nssaExternalLink (7) "
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSATypeGet(L7_uint32 areaId, L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSATypeGet(areaId, val);
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA LS Id for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA LS Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The Link State ID is an LS Type Specific field
*             containing either a Router ID or an IP Address; it
*             identifies the piece of the routing domain that is
*             being described by the advertisement."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSALSIdGet(L7_uint32 areaId, L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSALSIdGet(areaId, val);
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Router Id for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Router Id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The 32 bit number that uniquely identifies the 
*             originating router in the Autonomous System."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSARouterIdGet(L7_uint32 areaId, L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSARouterIdGet(areaId, val);
}

/*********************************************************************
* @purpose  Get the sequence number.
*
* @param    AreaId      type
* @param    Type        entry
* @param    Lsid        Link State id
* @param    RouterId    Database entry
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."   
*
* @end
*********************************************************************/
L7_RC_t ospfMapLsdbSequenceGet ( L7_uint32 AreaId, L7_int32 Type,
                                 L7_uint32 Lsid, L7_uint32 RouterId,
                                 L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenLsdbSequenceGet(AreaId, Type, Lsid, RouterId, val);

  return L7_FAILURE;
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Sequence for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Sequence
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."   
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSASequenceGet(L7_uint32 areaId, L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSASequenceGet(areaId, val);
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Age for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Age
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32-bit integer.
*             It is used to detect old and duplicate link state
*             advertisments.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number
*             the most recent the advertisement."   
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSAAgeGet(L7_uint32 areaId, L7_int32 Type, 
                                    L7_uint32 Lsid, L7_uint32 RouterId, 
                                    L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSAAgeGet(areaId, Type, Lsid, RouterId, val);
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Options for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Options
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSAOptionsGet(L7_uint32 areaId, L7_int32 Type, 
                                        L7_uint32 Lsid, L7_uint32 RouterId,
                                        L7_uchar8 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSAOptionsGet(areaId, Type, Lsid, RouterId, val);
}

/*********************************************************************
* 
* @purpose  Get the flags field of an OSPF router LSA
*          
* @param    areaId    area ID where router LSA is flooded
* @param    routerId  router ID of originating router
* @param    rtrLsaFlags   flags field in router LSA
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    none
*        
* @end
*********************************************************************/
L7_RC_t ospfMapRouterLsaFlagsGet(L7_uint32 areaId, L7_uint32 routerId, 
                                 L7_uchar8 *rtrLsaFlags)
{
    if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

    return ospfMapExtenRouterLsaFlagsGet(areaId, routerId, rtrLsaFlags);
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Checksum for the specified Area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uint32 LSA Checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This field is the checksum of the complete content of
*             the advertisement, excepting the age field.  The age
*             field is excepted so that an advertisement's age can
*             be incremented without updating the checksum.  The
*             checksum used is the same that is used for ISO
*             connectionless datagrams; it is commonly referred to
*             as the Fletcher checksum."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSACksumGet(L7_uint32 areaId, L7_int32 Type, 
                                      L7_uint32 Lsid, L7_uint32 RouterId, 
                                      L7_uint32 *val)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSACksumGet(areaId, Type, Lsid, RouterId, val);
}

/*********************************************************************
* 
* @purpose  Get the Ospf lsa-database LSA Advertisement packet for the
*           specified area
*          
* @param    areaId    L7_uint32 area ID
* @param    *val      L7_uchar8 LSA Advertise packet
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The entire Link State Advertisement, including its
*             header."
*        
* @end
*********************************************************************/
L7_uint32 ospfMapAreaLSADbLSAAdvertiseGet(L7_uint32 areaId, L7_int32 Type, 
                                          L7_uint32 Lsid, L7_uint32 RouterId,
                                          L7_char8 **buf, L7_uint32 *buf_len)
{
  if(ospfMapOspfInitialized() != L7_TRUE)
      return L7_FAILURE;

  return ospfMapExtenAreaLSADbLSAAdvertiseGet(areaId, Type, Lsid, RouterId, buf, buf_len);
}

/*
***********************************************************************
*                     API FUNCTIONS  -  EXT LSDB CONFIG
***********************************************************************
*/

/*----------------------------------------------------------------------*/
/* OSPF Link State Database, External                                   */
/* The Link State Database contains the Link State Advertisements from  */
/* throughout the areas that the device is attached to.                 */
/* This table is identical to the OSPF LSDB Table in format, but        */
/* contains only External Link State Advertisements. The purpose is     */
/* to allow external LSAs to be displayed once for the router rather    */
/* than once in each non-stub area.                                     */
/*----------------------------------------------------------------------*/

/*********************************************************************
* @purpose  Get a single Link State entry.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single Link State Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbEntryGet ( L7_uint32 Type, L7_uint32 Lsid,
                                 L7_uint32 RouterId,
                                 L7_ospfLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbEntryGet(Type,Lsid,RouterId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the next Link State entry.
*
* @param    *Type       link state type
* @param    *Lsid       link state id
* @param    *RouterId   router id
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " A single Link State Advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbEntryNext ( L7_uint32 *Type, L7_uint32 *Lsid,
                                  L7_uint32 *RouterId,
                                  L7_ospfLsdbEntry_t *p_Lsa)
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbEntryNext(Type,Lsid,RouterId,p_Lsa);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the sequence number field.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        sequence number
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The sequence number field is a signed 32 bit integer.
*             It is used to detect old and duplicate link state
*             advertisements.  The space of sequence numbers is
*             linearly ordered.  The larger the sequence number the
*             more recent the advertisement."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbSequenceGet ( L7_int32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbSequenceGet(Type,Lsid,RouterId,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the age of the link state advertisement in seconds.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        age in seconds
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This field is the age of the link state advertisement
*             in seconds."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbAgeGet ( L7_int32 Type, L7_uint32 Lsid,
                               L7_uint32 RouterId, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbAgeGet(Type,Lsid,RouterId,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the checksum of the complete contents of the advertisement, 
*           excepting the age field.  
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        checksum
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " This field is the checksum of the complete content of
*             the advertisement, excepting the age field.  The age
*             field is excepted so that an advertisement's age can
*             be incremented without updating the checksum.  The
*             checksum used is the same that is used for ISO
*             connectionless datagrams; it is commonly referred to
*             as the Fletcher checksum."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbChecksumGet ( L7_int32 Type, L7_uint32 Lsid,
                                    L7_uint32 RouterId, L7_int32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbChecksumGet(Type,Lsid,RouterId,val);

  return L7_FAILURE;
}

/*********************************************************************
* @purpose  Get the entire Link State Advertisement, including its header.
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *buf        link state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The entire Link State Advertisement, including its
*             header."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbAdvertisementGet ( L7_int32 Type, L7_uint32 Lsid,
                                         L7_uint32 RouterId, L7_char8 **buf,
										 L7_uint32 *len )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbAdvertisementGet(Type,Lsid,RouterId,buf,len);

  return L7_FAILURE;
}


/*********************************************************************
* @purpose  Get the length of the Link State Advertisement
*
* @param    Type        link state type
* @param    Lsid        link state id
* @param    RouterId    router id
* @param    *val        length of link state
*
* @returns  L7_SUCCESS  if success
* @returns  L7_FAILURE  if failure
*
* @notes    " The entire Link State Advertisement, including its
*             header."
*
* @end
*********************************************************************/
L7_RC_t ospfMapExtLsdbAdvertisementLengthGet ( L7_int32 Type, L7_uint32 Lsid,
                                         L7_uint32 RouterId, L7_uint32 *val )
{
  if (ospfMapOspfInitialized() == L7_TRUE) 
    return ospfMapExtenExtLsdbAdvertisementLengthGet(Type,Lsid,RouterId,val);

  return L7_FAILURE;
}

