/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
*
* @filename     ws_dev_loc.c
*
* @purpose      Wireless device location functionality
*
* @component    wireless
*
* @comments     none
*
* @create       08/03/2009
*
* @author       syed moin ahmed 
*
* @end
*
**********************************************************************/

#ifndef INCLUDE_USMDB_WS_DEV_LOC_API_H
#define INCLUDE_USMDB_WS_DEV_LOC_API_H

/*
LVL7 Base Includes
*/

/*******************************************************************************
* @purpose  Get the dev-loc tunable paramter - Acceptable sigma.
*
* @param  value          @b{(output)}  acceptable sigma value.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    None.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneAcceptableSigmaGet(L7_int32 *value);

/*******************************************************************************
* @purpose  Set the dev-loc tunable paramter - Acceptable sigma.
*
* @param  value          @b{(input)}  acceptable sigma value.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    None.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneAcceptableSigmaSet(L7_int32 value);

/*******************************************************************************
* @purpose  Get the dev-loc tunable paramter - radius fudge limit
*
* @param  value          @b{(output)}  radius fudge limit in meters.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    Useful in deriving circle intersection between ap pairs.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneRadiusFudgeLmtGet(L7_int32 *value);

/*******************************************************************************
* @purpose  Set the dev-loc tunable paramter - radius fudge limit
*
* @param  value          @b{(input)}  radius fudge limit in meters.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    Useful in deriving circle intersection between ap pairs.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneRadiusFudgeLmtSet(L7_int32 value);


/*******************************************************************************
* @purpose  Get the dev-loc tunable paramter - hgih signal threshold
*
* @param  value   @b{(output)}  high signal threshold value in dbm.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    Useful in deriving special case solution.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneHighSigThresholdGet(L7_uint8 band, L7_int32 *value);

/*******************************************************************************
* @purpose  Set the dev-loc tunable paramter - hgih signal threshold
*
* @param  value   @b{(input)}  high signal threshold value in dbm.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    Useful in deriving special case solution.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneHighSigThresholdSet(L7_uint8 band, L7_int32 value);

/*******************************************************************************
* @purpose  Get the dev-loc tunable paramter - default triangulation APs
*
* @param  value   @b{(output)}  number of default triangulation APs.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    No. of AP signal responses considered in location triangulation
*           after arranging the responses in descending order of signal strength.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneNumTriangApsGet(L7_int32 *value);

/*******************************************************************************
* @purpose  Set the dev-loc tunable paramter - default triangulation APs
*
* @param  value   @b{(input)}  number of default triangulation APs.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    No. of AP signal responses considered in location triangulation
*           after arranging the responses in descending order of signal strength.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneNumTriangApsSet(L7_int32 value);

/*******************************************************************************
* @purpose  Set the dev-loc tunable paramters to default values.
*
* @param    none.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocTuneParamsDefaultSet(void);


/*******************************************************************************
* @purpose  Get the indexed sig-dist map.
*
* @param  map   @b{(input)}  frequency band (2.4 GHz/ 5.0 GHz).
* @param  index @b{(input)}  map index.
* @param  map   @b{(output)}  signal-dist maps.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapGet(L7_uint8 band, 
                                     L7_int32 index, 
                                     signalToDistanceMap_t *map);

/*******************************************************************************
* @purpose  Set the reference signal strength for indexed sig-dist map.
*
* @param  map   @b{(input)}  frequency band (2.4 GHz/ 5.0 GHz).
* @param  mapIndex @b{(input)}  map index.
* @param  value    @b{(input)}  signal strength vlaue in dbm.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapRefSignalSet(L7_uint8 band, 
                                              L7_int32 mapIndex, 
                                              L7_int32 value);

/*******************************************************************************
* @purpose  Set the reference 'distance' for indexed sig-dist map.
*
* @param  map   @b{(input)}  frequency band (2.4 GHz/ 5.0 GHz).
* @param  mapIndex @b{(input)}  map index.
* @param  value    @b{(input)}  distance vlaue in meters.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapRefDistSet(L7_uint8 band, 
                                            L7_int32 mapIndex, 
                                            L7_int32 value);

/*******************************************************************************
* @purpose  Set the reference 'signal degradation' for indexed sig-dist map.
*
* @param  map   @b{(input)}  frequency band (2.4 GHz/ 5.0 GHz).
* @param  mapIndex @b{(input)}  map index.
* @param  value    @b{(input)}  signal degradation vlaue in dbm.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapRefSigDegradSet(L7_uint8 band, 
                                                 L7_int32 mapIndex, 
                                                 float value);

/*******************************************************************************
* @purpose  Set the reference 'obstacle distance' for indexed sig-dist map.
*
* @param  map   @b{(input)}  frequency band (2.4 GHz/ 5.0 GHz).
* @param  mapIndex @b{(input)}  map index.
* @param  value    @b{(input)}  obstacle distance vlaue in meters.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapRefObstDistSet(L7_uint8 band, 
                                                L7_int32 mapIndex, 
                                                L7_int32 value);

/*******************************************************************************
* @purpose  Populate the signal to distance mapping tables.
*
* @param  band    @b{(input)}  frequency band (2.4GHz/5.0GHz).
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapsCreate(L7_uint8 band);

/*******************************************************************************
* @purpose  Populate the indexed signal to distance mapping table.
*
* @param  band     @b{(input)}  frequency band. (2.4 GHz/ 5.0 Ghz.)
* @param  index    @b{(input)}  map index.
*
* @returns  L7_SUCCESS.
* @returns  L7_FAILURE.
*
* @notes    none.
*
* @end
*******************************************************************************/
L7_RC_t usmDbWsDevLocSigToDistMapCreate(L7_uint8 band, L7_int32 index);

#endif /* INCLUDE_USMDB_WS_DEV_LOC_API_H */
