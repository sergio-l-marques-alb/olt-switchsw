
/*******************************************************************************
 *
 * (C) Copyright Broadcom Corporation 2000-2008
 *
 ********************************************************************************
 *
 * @filename fpobj_baseARPInfo.c
 *
 * @purpose  
 *
 * @component object handlers
 *
 * @comments  Refer to base-object.xml
 *
 * @create  21 April 2008, Monday
 *
 * @notes   This file is auto generated and should be used as starting point to
 *          develop the object handlers
 *
 * @author  Rama Sasthri, Kristipati
 * @end
 *
 ********************************************************************************/


#include "fpobj_util.h"
#include "_xe_baseARPInfo_obj.h"
#include "usmdb_sim_api.h"


/*******************************************************************************
 * @function fpObjGet_baseARPInfo_ARPIndex
 *
 * @purpose Get 'ARPIndex'
 *@description  [ARPIndex] ARP index.    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_baseARPInfo_ARPIndex (void *wap, void *bufp)
{
    xLibU32_t objARPIndexValue;
    xLibU32_t nextObjARPIndexValue;
    xLibIpV4_t nextObjIpAddressValue;
    xLibStr6_t nextObjMacAddressValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    FPOBJ_TRACE_ENTER (bufp);

    memset (&objARPIndexValue, 0, sizeof (objARPIndexValue));
    memset (&nextObjARPIndexValue, 0, sizeof (nextObjARPIndexValue));
    memset (&nextObjIpAddressValue, 0, sizeof (nextObjIpAddressValue));
    memset (nextObjMacAddressValue, 0, sizeof (nextObjMacAddressValue));

    /* retrieve key: ARPIndex */
    owa.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_ARPIndex,
            (xLibU8_t *) & objARPIndexValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
        owa.l7rc =  usmDbArpInfoGetNext(L7_UNIT_CURRENT, 
                (L7_long32 *) &nextObjARPIndexValue, 
                (L7_ulong32 *)&nextObjIpAddressValue, 
                nextObjMacAddressValue);
    }
    else
    {
        FPOBJ_TRACE_CURRENT_KEY (bufp, &objARPIndexValue, owa.len);
        nextObjARPIndexValue = objARPIndexValue;

        do
        {
            owa.l7rc =  usmDbArpInfoGetNext(L7_UNIT_CURRENT, 
                    (L7_long32 *) &nextObjARPIndexValue, 
                    (L7_ulong32 *)&nextObjIpAddressValue, 
                    nextObjMacAddressValue);
        }
        while ((nextObjARPIndexValue == objARPIndexValue) 
                && (owa.l7rc == L7_SUCCESS));
    }	
    if (owa.l7rc != L7_SUCCESS)
    {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }
    FPOBJ_TRACE_NEW_KEY (bufp, &nextObjARPIndexValue, owa.len);
    /* return the object value: ARPIndex */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjARPIndexValue,
            sizeof (objARPIndexValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_baseARPInfo_IpAddress
 *
 * @purpose Get 'IpAddress'
 *@description  [IpAddress] Ip Address.    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_baseARPInfo_IpAddress (void *wap, void *bufp)
{
    xLibU32_t objARPIndexValue;
    xLibIpV4_t objIpAddressValue;
    xLibU32_t nextObjARPIndexValue;
    xLibIpV4_t nextObjIpAddressValue;
    xLibStr6_t nextObjMacAddressValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
    fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));

    FPOBJ_TRACE_ENTER (bufp);

    memset (&objARPIndexValue, 0, sizeof (objARPIndexValue));
    memset (&objIpAddressValue, 0, sizeof (objIpAddressValue));
    memset (&nextObjARPIndexValue, 0, sizeof (nextObjARPIndexValue));
    memset (&nextObjIpAddressValue, 0, sizeof (nextObjIpAddressValue));
    memset (nextObjMacAddressValue, 0, sizeof (nextObjMacAddressValue));

    /* retrieve key: ARPIndex */
    kwa1.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_ARPIndex,
            (xLibU8_t *) & objARPIndexValue, &kwa1.len);
    if (kwa1.rc != XLIBRC_SUCCESS)
    {
        kwa1.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa1);
        return kwa1.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objARPIndexValue, kwa1.len);

    /* retrieve key: IpAddress */
    owa.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_IpAddress,
            (xLibU8_t *) & objIpAddressValue, &owa.len);
    if (owa.rc != XLIBRC_SUCCESS)
    {
        /*Get first entry from ARP cache table for given ARPIndex*/
        FPOBJ_TRACE_CURRENT_KEY (bufp, NULL, 0);
        do
        {
            owa.l7rc =  usmDbArpInfoGetNext(L7_UNIT_CURRENT, 
                    (L7_long32 *) &nextObjARPIndexValue, 
                    (L7_ulong32 *)&nextObjIpAddressValue, 
                    nextObjMacAddressValue);
        }
        while ((objARPIndexValue != nextObjARPIndexValue) 
                && (owa.l7rc == L7_SUCCESS));
        if (owa.l7rc != L7_SUCCESS)
        {
            owa.rc = XLIBRC_ENDOF_TABLE;
            FPOBJ_TRACE_EXIT (bufp, owa);
            return owa.rc;
        }
    }
    else
    {
        FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, owa.len);
        /* Get next ARP cache entry,
         * Check if the ARPIndex matches with that obtained from the 
         * filter database */
        nextObjARPIndexValue = objARPIndexValue;
        nextObjIpAddressValue = objIpAddressValue;

        owa.l7rc =  usmDbArpInfoGetNext(L7_UNIT_CURRENT, 
                (L7_long32 *) &nextObjARPIndexValue, 
                (L7_ulong32 *)&nextObjIpAddressValue, 
                nextObjMacAddressValue);
        if ((objARPIndexValue != nextObjARPIndexValue) || 
                (owa.l7rc != L7_SUCCESS))
        {
            owa.rc = XLIBRC_ENDOF_TABLE;
            FPOBJ_TRACE_EXIT (bufp, owa);
            return owa.rc;
        }
    }
    /* return the object value: IpAddress */
    FPOBJ_TRACE_NEW_KEY (bufp, &nextObjIpAddressValue, owa.len);
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & nextObjIpAddressValue, 
            sizeof (objIpAddressValue));

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_baseARPInfo_MacAddress
 *
 * @purpose Get 'MacAddress'
 *@description  [MacAddress] Mac Address.    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_baseARPInfo_MacAddress (void *wap, void *bufp)
{
    xLibU32_t objARPIndexValue;
    xLibIpV4_t objIpAddressValue;
    xLibU32_t nextObjARPIndexValue;
    xLibIpV4_t nextObjIpAddressValue;
    xLibStr6_t nextObjMacAddressValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibStr6_t));
    fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));

    FPOBJ_TRACE_ENTER (bufp);

    memset (&objARPIndexValue, 0, sizeof (objARPIndexValue));
    memset (&objIpAddressValue, 0, sizeof (objIpAddressValue));
    memset (&nextObjARPIndexValue, 0, sizeof (nextObjARPIndexValue));
    memset (&nextObjIpAddressValue, 0, sizeof (nextObjIpAddressValue));
    memset (nextObjMacAddressValue, 0, sizeof (nextObjMacAddressValue));

    /* retrieve key: ARPIndex */
    kwa1.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_ARPIndex,
            (xLibU8_t *) & objARPIndexValue, &kwa1.len);
    if (kwa1.rc != XLIBRC_SUCCESS)
    {
        kwa1.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa1);
        return kwa1.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objARPIndexValue, kwa1.len);

    /* retrieve key: IpAddress */
    kwa2.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_IpAddress,
            (xLibU8_t *) & objIpAddressValue, &kwa2.len);
    if (kwa2.rc != XLIBRC_SUCCESS)
    {
        kwa2.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa2);
        return kwa2.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, kwa2.len);

   /*1. Keep getting ARP cache entry from first entry onwards, one by one,
     * until its IP address matches with that obtained from the filter 
     * database OR the end of ARP cache table is reached.*/
    do
    {
        owa.l7rc =  usmDbArpInfoGetNext(L7_UNIT_CURRENT, 
                (L7_long32 *) &nextObjARPIndexValue, 
                (L7_ulong32 *)&nextObjIpAddressValue, 
                nextObjMacAddressValue);
    }
    while ((objIpAddressValue != nextObjIpAddressValue) 
            && (owa.l7rc == L7_SUCCESS));
    /*2. If the ARPIndex obtained in step 1 matches with that 
     * obtained from filter database then
     * return the MACAddress of this entry */
    if ((owa.l7rc != L7_SUCCESS) || (objARPIndexValue != nextObjARPIndexValue))
    {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    /* return the object value: MacAddress */
    FPOBJ_TRACE_NEW_KEY (bufp, nextObjMacAddressValue, owa.len);
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) nextObjMacAddressValue, 
            sizeof (nextObjMacAddressValue));

    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}

/*******************************************************************************
 * @function fpObjGet_baseARPInfo_IntIfNum
 *
 * @purpose Get 'IntIfNum'
 *@description  [IntIfNum] interface number.    
 * @notes       
 *
 * @return
 *******************************************************************************/
xLibRC_t fpObjGet_baseARPInfo_IntIfNum (void *wap, void *bufp)
{
    xLibU32_t objARPIndexValue;
    xLibIpV4_t objIpAddressValue;
    xLibU32_t nextObjARPIndexValue;
    xLibIpV4_t nextObjIpAddressValue;
    xLibStr6_t nextObjMacAddressValue;
    fpObjWa_t owa = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    fpObjWa_t kwa1 = FPOBJ_INIT_WA (sizeof (xLibU32_t));
    fpObjWa_t kwa2 = FPOBJ_INIT_WA (sizeof (xLibIpV4_t));
    xLibU32_t objIntIfNumValue;

    FPOBJ_TRACE_ENTER (bufp);

    memset (&objARPIndexValue, 0, sizeof (objARPIndexValue));
    memset (&objIpAddressValue, 0, sizeof (objIpAddressValue));
    memset (&nextObjARPIndexValue, 0, sizeof (nextObjARPIndexValue));
    memset (&nextObjIpAddressValue, 0, sizeof (nextObjIpAddressValue));
    memset (nextObjMacAddressValue, 0, sizeof (nextObjMacAddressValue));

    /* retrieve key: ARPIndex */
    kwa1.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_ARPIndex,
            (xLibU8_t *) & objARPIndexValue, &kwa1.len);
    if (kwa1.rc != XLIBRC_SUCCESS)
    {
        kwa1.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa1);
        return kwa1.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objARPIndexValue, kwa1.len);

    /* retrieve key: IpAddress */
    kwa2.rc = xLibFilterGet (wap, XOBJ_baseARPInfo_IpAddress,
            (xLibU8_t *) & objIpAddressValue, &kwa2.len);
    if (kwa2.rc != XLIBRC_SUCCESS)
    {
        kwa2.rc = XLIBRC_FILTER_MISSING;
        FPOBJ_TRACE_EXIT (bufp, kwa2);
        return kwa2.rc;
    }

    FPOBJ_TRACE_CURRENT_KEY (bufp, &objIpAddressValue, kwa2.len);

    /*1. Keep getting ARP cache entry from first entry onwards, one by one,
     * until its IP address matches with that obtained from the filter 
     * database OR the end of ARP cache table is reached.*/
    do
    {
        owa.l7rc =  usmDbArpInfoGetNext(L7_UNIT_CURRENT, 
                (L7_long32 *) &nextObjARPIndexValue, 
                (L7_ulong32 *)&nextObjIpAddressValue, 
                nextObjMacAddressValue);
    }
    while ((objIpAddressValue != nextObjIpAddressValue) 
            && (owa.l7rc == L7_SUCCESS));
    /*2. If the ARPIndex obtained in step 1 matches with that 
     * obtained from filter database then use the MACAddress of this entry to
     * get the IntIfNum */
    if ((owa.l7rc != L7_SUCCESS) || (objARPIndexValue != nextObjARPIndexValue))
    {
        owa.rc = XLIBRC_ENDOF_TABLE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;
    }

    owa.l7rc =  usmDbArpEntryGet(L7_UNIT_CURRENT, nextObjMacAddressValue,
                &objIntIfNumValue);  
    if (owa.l7rc != L7_SUCCESS)
    {
        /*owa.rcdd = XLIBRC_FAILURE;
        FPOBJ_TRACE_EXIT (bufp, owa);
        return owa.rc;*/
        /* Return IntfNum = 0 to handle case where MACAddress is learnt
           * on service port */
        objIntIfNumValue = 0;
    }

    /* return the object value: IntIfNum */
    owa.rc = xLibBufDataSet (bufp, (xLibU8_t *) & objIntIfNumValue,
            sizeof (objIntIfNumValue));
    FPOBJ_TRACE_EXIT (bufp, owa);
    return owa.rc;
}





