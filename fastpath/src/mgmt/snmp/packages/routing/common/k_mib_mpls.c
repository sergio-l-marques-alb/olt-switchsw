#include <k_private_base.h>

rsvpSessionEntry_t *
k_rsvpSessionEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 rsvpSessionNumber)
{
#ifdef NOT_YET
   static rsvpSessionEntry_t rsvpSessionEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpSessionEntryData.rsvpSessionNumber = ;
   rsvpSessionEntryData.rsvpSessionType = ;
   rsvpSessionEntryData.rsvpSessionDestAddr = ;
   rsvpSessionEntryData.rsvpSessionDestAddrLength = ;
   rsvpSessionEntryData.rsvpSessionProtocol = ;
   rsvpSessionEntryData.rsvpSessionPort = ;
   rsvpSessionEntryData.rsvpSessionSenders = ;
   rsvpSessionEntryData.rsvpSessionReceivers = ;
   rsvpSessionEntryData.rsvpSessionRequests = ;
   SET_ALL_VALID(rsvpSessionEntryData.valid);
   return(&rsvpSessionEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

rsvpSenderEntry_t *
k_rsvpSenderEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 rsvpSessionNumber,
                      SR_INT32 rsvpSenderNumber)
{
#ifdef NOT_YET
   static rsvpSenderEntry_t rsvpSenderEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpSenderEntryData.rsvpSenderNumber = ;
   rsvpSenderEntryData.rsvpSenderType = ;
   rsvpSenderEntryData.rsvpSenderDestAddr = ;
   rsvpSenderEntryData.rsvpSenderAddr = ;
   rsvpSenderEntryData.rsvpSenderDestAddrLength = ;
   rsvpSenderEntryData.rsvpSenderAddrLength = ;
   rsvpSenderEntryData.rsvpSenderProtocol = ;
   rsvpSenderEntryData.rsvpSenderDestPort = ;
   rsvpSenderEntryData.rsvpSenderPort = ;
   rsvpSenderEntryData.rsvpSenderFlowId = ;
   rsvpSenderEntryData.rsvpSenderHopAddr = ;
   rsvpSenderEntryData.rsvpSenderHopLih = ;
   rsvpSenderEntryData.rsvpSenderInterface = ;
   rsvpSenderEntryData.rsvpSenderTSpecRate = ;
   rsvpSenderEntryData.rsvpSenderTSpecPeakRate = ;
   rsvpSenderEntryData.rsvpSenderTSpecBurst = ;
   rsvpSenderEntryData.rsvpSenderTSpecMinTU = ;
   rsvpSenderEntryData.rsvpSenderTSpecMaxTU = ;
   rsvpSenderEntryData.rsvpSenderInterval = ;
   rsvpSenderEntryData.rsvpSenderRSVPHop = ;
   rsvpSenderEntryData.rsvpSenderLastChange = ;
   rsvpSenderEntryData.rsvpSenderPolicy = ;
   rsvpSenderEntryData.rsvpSenderAdspecBreak = ;
   rsvpSenderEntryData.rsvpSenderAdspecHopCount = ;
   rsvpSenderEntryData.rsvpSenderAdspecPathBw = ;
   rsvpSenderEntryData.rsvpSenderAdspecMinLatency = ;
   rsvpSenderEntryData.rsvpSenderAdspecMtu = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedSvc = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedBreak = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedCtot = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedDtot = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedCsum = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedDsum = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedHopCount = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedPathBw = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedMinLatency = ;
   rsvpSenderEntryData.rsvpSenderAdspecGuaranteedMtu = ;
   rsvpSenderEntryData.rsvpSenderAdspecCtrlLoadSvc = ;
   rsvpSenderEntryData.rsvpSenderAdspecCtrlLoadBreak = ;
   rsvpSenderEntryData.rsvpSenderAdspecCtrlLoadHopCount = ;
   rsvpSenderEntryData.rsvpSenderAdspecCtrlLoadPathBw = ;
   rsvpSenderEntryData.rsvpSenderAdspecCtrlLoadMinLatency = ;
   rsvpSenderEntryData.rsvpSenderAdspecCtrlLoadMtu = ;
   rsvpSenderEntryData.rsvpSenderStatus = ;
   rsvpSenderEntryData.rsvpSenderTTL = ;
   rsvpSenderEntryData.rsvpSessionNumber = ;
   SET_ALL_VALID(rsvpSenderEntryData.valid);
   return(&rsvpSenderEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpSenderEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_rsvpSenderEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpSenderEntry_set_defaults(doList_t *dp)
{
    rsvpSenderEntry_t *data = (rsvpSenderEntry_t *) (dp->data);

    if ((data->rsvpSenderDestAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpSenderAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpSenderDestPort = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpSenderPort = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpSenderHopAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpSenderPolicy = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_rsvpSenderEntry_set(rsvpSenderEntry_t *data,
                      ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_rsvpSenderEntry_UNDO
/* add #define SR_rsvpSenderEntry_UNDO in sitedefs.h to
 * include the undo routine for the rsvpSenderEntry family.
 */
int
rsvpSenderEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpSenderEntry_UNDO */

#endif /* SETS */

rsvpSenderOutInterfaceEntry_t *
k_rsvpSenderOutInterfaceEntry_get(int serialNum, ContextInfo *contextInfo,
                                  int nominator,
                                  int searchType,
                                  SR_INT32 rsvpSessionNumber,
                                  SR_INT32 rsvpSenderNumber,
                                  SR_INT32 ifIndex)
{
#ifdef NOT_YET
   static rsvpSenderOutInterfaceEntry_t rsvpSenderOutInterfaceEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpSenderOutInterfaceEntryData.rsvpSenderOutInterfaceStatus = ;
   rsvpSenderOutInterfaceEntryData.rsvpSessionNumber = ;
   rsvpSenderOutInterfaceEntryData.rsvpSenderNumber = ;
   rsvpSenderOutInterfaceEntryData.ifIndex = ;
   SET_ALL_VALID(rsvpSenderOutInterfaceEntryData.valid);
   return(&rsvpSenderOutInterfaceEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

rsvpResvEntry_t *
k_rsvpResvEntry_get(int serialNum, ContextInfo *contextInfo,
                    int nominator,
                    int searchType,
                    SR_INT32 rsvpSessionNumber,
                    SR_INT32 rsvpResvNumber)
{
#ifdef NOT_YET
   static rsvpResvEntry_t rsvpResvEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpResvEntryData.rsvpResvNumber = ;
   rsvpResvEntryData.rsvpResvType = ;
   rsvpResvEntryData.rsvpResvDestAddr = ;
   rsvpResvEntryData.rsvpResvSenderAddr = ;
   rsvpResvEntryData.rsvpResvDestAddrLength = ;
   rsvpResvEntryData.rsvpResvSenderAddrLength = ;
   rsvpResvEntryData.rsvpResvProtocol = ;
   rsvpResvEntryData.rsvpResvDestPort = ;
   rsvpResvEntryData.rsvpResvPort = ;
   rsvpResvEntryData.rsvpResvHopAddr = ;
   rsvpResvEntryData.rsvpResvHopLih = ;
   rsvpResvEntryData.rsvpResvInterface = ;
   rsvpResvEntryData.rsvpResvService = ;
   rsvpResvEntryData.rsvpResvTSpecRate = ;
   rsvpResvEntryData.rsvpResvTSpecPeakRate = ;
   rsvpResvEntryData.rsvpResvTSpecBurst = ;
   rsvpResvEntryData.rsvpResvTSpecMinTU = ;
   rsvpResvEntryData.rsvpResvTSpecMaxTU = ;
   rsvpResvEntryData.rsvpResvRSpecRate = ;
   rsvpResvEntryData.rsvpResvRSpecSlack = ;
   rsvpResvEntryData.rsvpResvInterval = ;
   rsvpResvEntryData.rsvpResvScope = ;
   rsvpResvEntryData.rsvpResvShared = ;
   rsvpResvEntryData.rsvpResvExplicit = ;
   rsvpResvEntryData.rsvpResvRSVPHop = ;
   rsvpResvEntryData.rsvpResvLastChange = ;
   rsvpResvEntryData.rsvpResvPolicy = ;
   rsvpResvEntryData.rsvpResvStatus = ;
   rsvpResvEntryData.rsvpResvTTL = ;
   rsvpResvEntryData.rsvpResvFlowId = ;
   rsvpResvEntryData.rsvpSessionNumber = ;
   SET_ALL_VALID(rsvpResvEntryData.valid);
   return(&rsvpResvEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpResvEntry_test(ObjectInfo *object, ObjectSyntax *value,
                     doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_rsvpResvEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                      doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpResvEntry_set_defaults(doList_t *dp)
{
    rsvpResvEntry_t *data = (rsvpResvEntry_t *) (dp->data);

    if ((data->rsvpResvDestAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvSenderAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvDestPort = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvPort = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvHopAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvScope = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvPolicy = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_rsvpResvEntry_set(rsvpResvEntry_t *data,
                    ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_rsvpResvEntry_UNDO
/* add #define SR_rsvpResvEntry_UNDO in sitedefs.h to
 * include the undo routine for the rsvpResvEntry family.
 */
int
rsvpResvEntry_undo(doList_t *doHead, doList_t *doCur,
                   ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpResvEntry_UNDO */

#endif /* SETS */

rsvpResvFwdEntry_t *
k_rsvpResvFwdEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 rsvpSessionNumber,
                       SR_INT32 rsvpResvFwdNumber)
{
#ifdef NOT_YET
   static rsvpResvFwdEntry_t rsvpResvFwdEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpResvFwdEntryData.rsvpResvFwdNumber = ;
   rsvpResvFwdEntryData.rsvpResvFwdType = ;
   rsvpResvFwdEntryData.rsvpResvFwdDestAddr = ;
   rsvpResvFwdEntryData.rsvpResvFwdSenderAddr = ;
   rsvpResvFwdEntryData.rsvpResvFwdDestAddrLength = ;
   rsvpResvFwdEntryData.rsvpResvFwdSenderAddrLength = ;
   rsvpResvFwdEntryData.rsvpResvFwdProtocol = ;
   rsvpResvFwdEntryData.rsvpResvFwdDestPort = ;
   rsvpResvFwdEntryData.rsvpResvFwdPort = ;
   rsvpResvFwdEntryData.rsvpResvFwdHopAddr = ;
   rsvpResvFwdEntryData.rsvpResvFwdHopLih = ;
   rsvpResvFwdEntryData.rsvpResvFwdInterface = ;
   rsvpResvFwdEntryData.rsvpResvFwdService = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecRate = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecPeakRate = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecBurst = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecMinTU = ;
   rsvpResvFwdEntryData.rsvpResvFwdTSpecMaxTU = ;
   rsvpResvFwdEntryData.rsvpResvFwdRSpecRate = ;
   rsvpResvFwdEntryData.rsvpResvFwdRSpecSlack = ;
   rsvpResvFwdEntryData.rsvpResvFwdInterval = ;
   rsvpResvFwdEntryData.rsvpResvFwdScope = ;
   rsvpResvFwdEntryData.rsvpResvFwdShared = ;
   rsvpResvFwdEntryData.rsvpResvFwdExplicit = ;
   rsvpResvFwdEntryData.rsvpResvFwdRSVPHop = ;
   rsvpResvFwdEntryData.rsvpResvFwdLastChange = ;
   rsvpResvFwdEntryData.rsvpResvFwdPolicy = ;
   rsvpResvFwdEntryData.rsvpResvFwdStatus = ;
   rsvpResvFwdEntryData.rsvpResvFwdTTL = ;
   rsvpResvFwdEntryData.rsvpResvFwdFlowId = ;
   rsvpResvFwdEntryData.rsvpSessionNumber = ;
   SET_ALL_VALID(rsvpResvFwdEntryData.valid);
   return(&rsvpResvFwdEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpResvFwdEntry_test(ObjectInfo *object, ObjectSyntax *value,
                        doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_rsvpResvFwdEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                         doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpResvFwdEntry_set_defaults(doList_t *dp)
{
    rsvpResvFwdEntry_t *data = (rsvpResvFwdEntry_t *) (dp->data);

    if ((data->rsvpResvFwdDestAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvFwdSenderAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvFwdDestPort = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvFwdPort = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvFwdHopAddr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvFwdScope = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->rsvpResvFwdPolicy = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_rsvpResvFwdEntry_set(rsvpResvFwdEntry_t *data,
                       ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_rsvpResvFwdEntry_UNDO
/* add #define SR_rsvpResvFwdEntry_UNDO in sitedefs.h to
 * include the undo routine for the rsvpResvFwdEntry family.
 */
int
rsvpResvFwdEntry_undo(doList_t *doHead, doList_t *doCur,
                      ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpResvFwdEntry_UNDO */

#endif /* SETS */

rsvpIfEntry_t *
k_rsvpIfEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 ifIndex)
{
#ifdef NOT_YET
   static rsvpIfEntry_t rsvpIfEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpIfEntryData.rsvpIfUdpNbrs = ;
   rsvpIfEntryData.rsvpIfIpNbrs = ;
   rsvpIfEntryData.rsvpIfNbrs = ;
   rsvpIfEntryData.rsvpIfRefreshBlockadeMultiple = ;
   rsvpIfEntryData.rsvpIfRefreshMultiple = ;
   rsvpIfEntryData.rsvpIfTTL = ;
   rsvpIfEntryData.rsvpIfRefreshInterval = ;
   rsvpIfEntryData.rsvpIfRouteDelay = ;
   rsvpIfEntryData.rsvpIfEnabled = ;
   rsvpIfEntryData.rsvpIfUdpRequired = ;
   rsvpIfEntryData.rsvpIfStatus = ;
   rsvpIfEntryData.ifIndex = ;
   SET_ALL_VALID(rsvpIfEntryData.valid);
   return(&rsvpIfEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpIfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_rsvpIfEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpIfEntry_set_defaults(doList_t *dp)
{
    rsvpIfEntry_t *data = (rsvpIfEntry_t *) (dp->data);

    data->rsvpIfRefreshBlockadeMultiple = 4;
    data->rsvpIfRefreshMultiple = 3;
    data->rsvpIfTTL = 0;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_rsvpIfEntry_set(rsvpIfEntry_t *data,
                  ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_rsvpIfEntry_UNDO
/* add #define SR_rsvpIfEntry_UNDO in sitedefs.h to
 * include the undo routine for the rsvpIfEntry family.
 */
int
rsvpIfEntry_undo(doList_t *doHead, doList_t *doCur,
                 ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpIfEntry_UNDO */

#endif /* SETS */

rsvpNbrEntry_t *
k_rsvpNbrEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 ifIndex,
                   OctetString * rsvpNbrAddress)
{
#ifdef NOT_YET
   static rsvpNbrEntry_t rsvpNbrEntryData;

   /*
    * put your code to retrieve the information here
    */

   rsvpNbrEntryData.rsvpNbrAddress = ;
   rsvpNbrEntryData.rsvpNbrProtocol = ;
   rsvpNbrEntryData.rsvpNbrStatus = ;
   rsvpNbrEntryData.ifIndex = ;
   SET_ALL_VALID(rsvpNbrEntryData.valid);
   return(&rsvpNbrEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpNbrEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_rsvpNbrEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpNbrEntry_set_defaults(doList_t *dp)
{
    rsvpNbrEntry_t *data = (rsvpNbrEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_rsvpNbrEntry_set(rsvpNbrEntry_t *data,
                   ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_rsvpNbrEntry_UNDO
/* add #define SR_rsvpNbrEntry_UNDO in sitedefs.h to
 * include the undo routine for the rsvpNbrEntry family.
 */
int
rsvpNbrEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpNbrEntry_UNDO */

#endif /* SETS */

rsvpGenObjects_t *
k_rsvpGenObjects_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
#ifdef NOT_YET
   static rsvpGenObjects_t rsvpGenObjectsData;

   /*
    * put your code to retrieve the information here
    */

   rsvpGenObjectsData.rsvpBadPackets = ;
   rsvpGenObjectsData.rsvpSenderNewIndex = ;
   rsvpGenObjectsData.rsvpResvNewIndex = ;
   rsvpGenObjectsData.rsvpResvFwdNewIndex = ;
   SET_ALL_VALID(rsvpGenObjectsData.valid);
   return(&rsvpGenObjectsData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_rsvpGenObjects_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_rsvpGenObjects_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_rsvpGenObjects_set(rsvpGenObjects_t *data,
                     ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_rsvpGenObjects_UNDO
/* add #define SR_rsvpGenObjects_UNDO in sitedefs.h to
 * include the undo routine for the rsvpGenObjects family.
 */
int
rsvpGenObjects_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_rsvpGenObjects_UNDO */

#endif /* SETS */

mplsLsrObjects_t *
k_mplsLsrObjects_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
#ifdef NOT_YET
   static mplsLsrObjects_t mplsLsrObjectsData;

   /*
    * put your code to retrieve the information here
    */

   mplsLsrObjectsData.mplsOutSegmentIndexNext = ;
   mplsLsrObjectsData.mplsXCIndexNext = ;
   mplsLsrObjectsData.mplsMaxLabelStackDepth = ;
   mplsLsrObjectsData.mplsLabelStackIndexNext = ;
   mplsLsrObjectsData.mplsTrafficParamIndexNext = ;
   mplsLsrObjectsData.mplsXCTrapEnable = ;
   SET_ALL_VALID(mplsLsrObjectsData.valid);
   return(&mplsLsrObjectsData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsLsrObjects_test(ObjectInfo *object, ObjectSyntax *value,
                      doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsLsrObjects_ready(ObjectInfo *object, ObjectSyntax *value, 
                       doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsLsrObjects_set(mplsLsrObjects_t *data,
                     ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsLsrObjects_UNDO
/* add #define SR_mplsLsrObjects_UNDO in sitedefs.h to
 * include the undo routine for the mplsLsrObjects family.
 */
int
mplsLsrObjects_undo(doList_t *doHead, doList_t *doCur,
                    ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsLsrObjects_UNDO */

#endif /* SETS */

mplsInterfaceConfEntry_t *
k_mplsInterfaceConfEntry_get(int serialNum, ContextInfo *contextInfo,
                             int nominator,
                             int searchType,
                             SR_INT32 mplsInterfaceConfIndex)
{
#ifdef NOT_YET
   static mplsInterfaceConfEntry_t mplsInterfaceConfEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsInterfaceConfEntryData.mplsInterfaceConfIndex = ;
   mplsInterfaceConfEntryData.mplsInterfaceLabelMinIn = ;
   mplsInterfaceConfEntryData.mplsInterfaceLabelMaxIn = ;
   mplsInterfaceConfEntryData.mplsInterfaceLabelMinOut = ;
   mplsInterfaceConfEntryData.mplsInterfaceLabelMaxOut = ;
   mplsInterfaceConfEntryData.mplsInterfaceTotalBandwidth = ;
   mplsInterfaceConfEntryData.mplsInterfaceAvailableBandwidth = ;
   mplsInterfaceConfEntryData.mplsInterfaceLabelParticipationType = ;
   mplsInterfaceConfEntryData.mplsInterfaceConfStorageType = ;
   mplsInterfaceConfEntryData.mplsInterfaceInLabelsUsed = ;
   mplsInterfaceConfEntryData.mplsInterfaceFailedLabelLookup = ;
   mplsInterfaceConfEntryData.mplsInterfaceOutLabelsUsed = ;
   mplsInterfaceConfEntryData.mplsInterfaceOutFragments = ;
   SET_ALL_VALID(mplsInterfaceConfEntryData.valid);
   return(&mplsInterfaceConfEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsInterfaceConfEntry_test(ObjectInfo *object, ObjectSyntax *value,
                              doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsInterfaceConfEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                               doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsInterfaceConfEntry_set_defaults(doList_t *dp)
{
    mplsInterfaceConfEntry_t *data = (mplsInterfaceConfEntry_t *) (dp->data);

    if ((data->mplsInterfaceLabelParticipationType = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsInterfaceConfEntry_set(mplsInterfaceConfEntry_t *data,
                             ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsInterfaceConfEntry_UNDO
/* add #define SR_mplsInterfaceConfEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsInterfaceConfEntry family.
 */
int
mplsInterfaceConfEntry_undo(doList_t *doHead, doList_t *doCur,
                            ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsInterfaceConfEntry_UNDO */

#endif /* SETS */

mplsInSegmentEntry_t *
k_mplsInSegmentEntry_get(int serialNum, ContextInfo *contextInfo,
                         int nominator,
                         int searchType,
                         SR_INT32 mplsInSegmentIfIndex,
                         SR_UINT32 mplsInSegmentLabel)
{
#ifdef NOT_YET
   static mplsInSegmentEntry_t mplsInSegmentEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsInSegmentEntryData.mplsInSegmentIfIndex = ;
   mplsInSegmentEntryData.mplsInSegmentLabel = ;
   mplsInSegmentEntryData.mplsInSegmentNPop = ;
   mplsInSegmentEntryData.mplsInSegmentAddrFamily = ;
   mplsInSegmentEntryData.mplsInSegmentXCIndex = ;
   mplsInSegmentEntryData.mplsInSegmentOwner = ;
   mplsInSegmentEntryData.mplsInSegmentTrafficParamPtr = ;
   mplsInSegmentEntryData.mplsInSegmentRowStatus = ;
   mplsInSegmentEntryData.mplsInSegmentStorageType = ;
   mplsInSegmentEntryData.mplsInSegmentOctets = ;
   mplsInSegmentEntryData.mplsInSegmentPackets = ;
   mplsInSegmentEntryData.mplsInSegmentErrors = ;
   mplsInSegmentEntryData.mplsInSegmentDiscards = ;
   mplsInSegmentEntryData.mplsInSegmentHCOctets = ;
   mplsInSegmentEntryData.mplsInSegmentPerfDiscontinuityTime = ;
   SET_ALL_VALID(mplsInSegmentEntryData.valid);
   return(&mplsInSegmentEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsInSegmentEntry_test(ObjectInfo *object, ObjectSyntax *value,
                          doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsInSegmentEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                           doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsInSegmentEntry_set_defaults(doList_t *dp)
{
    mplsInSegmentEntry_t *data = (mplsInSegmentEntry_t *) (dp->data);

    data->mplsInSegmentNPop = 1;
    data->mplsInSegmentAddrFamily = D_mplsInSegmentAddrFamily_other;
    data->mplsInSegmentXCIndex = 0;
    data->mplsInSegmentOwner = D_mplsInSegmentOwner_unknown;
    if ((data->mplsInSegmentTrafficParamPtr = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsInSegmentEntry_set(mplsInSegmentEntry_t *data,
                         ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsInSegmentEntry_UNDO
/* add #define SR_mplsInSegmentEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsInSegmentEntry family.
 */
int
mplsInSegmentEntry_undo(doList_t *doHead, doList_t *doCur,
                        ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsInSegmentEntry_UNDO */

#endif /* SETS */

mplsOutSegmentEntry_t *
k_mplsOutSegmentEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 mplsOutSegmentIndex)
{
#ifdef NOT_YET
   static mplsOutSegmentEntry_t mplsOutSegmentEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsOutSegmentEntryData.mplsOutSegmentIndex = ;
   mplsOutSegmentEntryData.mplsOutSegmentIfIndex = ;
   mplsOutSegmentEntryData.mplsOutSegmentPushTopLabel = ;
   mplsOutSegmentEntryData.mplsOutSegmentTopLabel = ;
   mplsOutSegmentEntryData.mplsOutSegmentNextHopIpAddrType = ;
   mplsOutSegmentEntryData.mplsOutSegmentNextHopIpv4Addr = ;
   mplsOutSegmentEntryData.mplsOutSegmentNextHopIpv6Addr = ;
   mplsOutSegmentEntryData.mplsOutSegmentXCIndex = ;
   mplsOutSegmentEntryData.mplsOutSegmentOwner = ;
   mplsOutSegmentEntryData.mplsOutSegmentTrafficParamPtr = ;
   mplsOutSegmentEntryData.mplsOutSegmentRowStatus = ;
   mplsOutSegmentEntryData.mplsOutSegmentStorageType = ;
   mplsOutSegmentEntryData.mplsOutSegmentOctets = ;
   mplsOutSegmentEntryData.mplsOutSegmentPackets = ;
   mplsOutSegmentEntryData.mplsOutSegmentErrors = ;
   mplsOutSegmentEntryData.mplsOutSegmentDiscards = ;
   mplsOutSegmentEntryData.mplsOutSegmentHCOctets = ;
   mplsOutSegmentEntryData.mplsOutSegmentPerfDiscontinuityTime = ;
   SET_ALL_VALID(mplsOutSegmentEntryData.valid);
   return(&mplsOutSegmentEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsOutSegmentEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsOutSegmentEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsOutSegmentEntry_set_defaults(doList_t *dp)
{
    mplsOutSegmentEntry_t *data = (mplsOutSegmentEntry_t *) (dp->data);

    data->mplsOutSegmentNextHopIpAddrType = D_mplsOutSegmentNextHopIpAddrType_unknown;
    if ((data->mplsOutSegmentNextHopIpv4Addr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsOutSegmentNextHopIpv6Addr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->mplsOutSegmentXCIndex = 0;
    data->mplsOutSegmentOwner = D_mplsOutSegmentOwner_unknown;
    if ((data->mplsOutSegmentTrafficParamPtr = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsOutSegmentEntry_set(mplsOutSegmentEntry_t *data,
                          ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsOutSegmentEntry_UNDO
/* add #define SR_mplsOutSegmentEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsOutSegmentEntry family.
 */
int
mplsOutSegmentEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsOutSegmentEntry_UNDO */

#endif /* SETS */

mplsXCEntry_t *
k_mplsXCEntry_get(int serialNum, ContextInfo *contextInfo,
                  int nominator,
                  int searchType,
                  SR_INT32 mplsXCIndex,
                  SR_INT32 mplsInSegmentIfIndex,
                  SR_UINT32 mplsInSegmentLabel,
                  SR_INT32 mplsOutSegmentIndex)
{
#ifdef NOT_YET
   static mplsXCEntry_t mplsXCEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsXCEntryData.mplsXCIndex = ;
   mplsXCEntryData.mplsXCLspId = ;
   mplsXCEntryData.mplsXCLabelStackIndex = ;
   mplsXCEntryData.mplsXCIsPersistent = ;
   mplsXCEntryData.mplsXCOwner = ;
   mplsXCEntryData.mplsXCRowStatus = ;
   mplsXCEntryData.mplsXCStorageType = ;
   mplsXCEntryData.mplsXCAdminStatus = ;
   mplsXCEntryData.mplsXCOperStatus = ;
   mplsXCEntryData.mplsInSegmentIfIndex = ;
   mplsXCEntryData.mplsInSegmentLabel = ;
   mplsXCEntryData.mplsOutSegmentIndex = ;
   SET_ALL_VALID(mplsXCEntryData.valid);
   return(&mplsXCEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsXCEntry_test(ObjectInfo *object, ObjectSyntax *value,
                   doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsXCEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                    doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsXCEntry_set_defaults(doList_t *dp)
{
    mplsXCEntry_t *data = (mplsXCEntry_t *) (dp->data);

    if ((data->mplsXCLspId = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->mplsXCIsPersistent = D_mplsXCIsPersistent_false;

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsXCEntry_set(mplsXCEntry_t *data,
                  ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsXCEntry_UNDO
/* add #define SR_mplsXCEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsXCEntry family.
 */
int
mplsXCEntry_undo(doList_t *doHead, doList_t *doCur,
                 ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsXCEntry_UNDO */

#endif /* SETS */

mplsLabelStackEntry_t *
k_mplsLabelStackEntry_get(int serialNum, ContextInfo *contextInfo,
                          int nominator,
                          int searchType,
                          SR_INT32 mplsLabelStackIndex,
                          SR_INT32 mplsLabelStackLabelIndex)
{
#ifdef NOT_YET
   static mplsLabelStackEntry_t mplsLabelStackEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsLabelStackEntryData.mplsLabelStackIndex = ;
   mplsLabelStackEntryData.mplsLabelStackLabelIndex = ;
   mplsLabelStackEntryData.mplsLabelStackLabel = ;
   mplsLabelStackEntryData.mplsLabelStackRowStatus = ;
   mplsLabelStackEntryData.mplsLabelStackStorageType = ;
   SET_ALL_VALID(mplsLabelStackEntryData.valid);
   return(&mplsLabelStackEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsLabelStackEntry_test(ObjectInfo *object, ObjectSyntax *value,
                           doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsLabelStackEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                            doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsLabelStackEntry_set_defaults(doList_t *dp)
{
    mplsLabelStackEntry_t *data = (mplsLabelStackEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsLabelStackEntry_set(mplsLabelStackEntry_t *data,
                          ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsLabelStackEntry_UNDO
/* add #define SR_mplsLabelStackEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsLabelStackEntry family.
 */
int
mplsLabelStackEntry_undo(doList_t *doHead, doList_t *doCur,
                         ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsLabelStackEntry_UNDO */

#endif /* SETS */

mplsTrafficParamEntry_t *
k_mplsTrafficParamEntry_get(int serialNum, ContextInfo *contextInfo,
                            int nominator,
                            int searchType,
                            SR_INT32 mplsTrafficParamIndex)
{
#ifdef NOT_YET
   static mplsTrafficParamEntry_t mplsTrafficParamEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsTrafficParamEntryData.mplsTrafficParamIndex = ;
   mplsTrafficParamEntryData.mplsTrafficParamMaxRate = ;
   mplsTrafficParamEntryData.mplsTrafficParamMeanRate = ;
   mplsTrafficParamEntryData.mplsTrafficParamMaxBurstSize = ;
   mplsTrafficParamEntryData.mplsTrafficParamRowStatus = ;
   mplsTrafficParamEntryData.mplsTrafficParamStorageType = ;
   SET_ALL_VALID(mplsTrafficParamEntryData.valid);
   return(&mplsTrafficParamEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsTrafficParamEntry_test(ObjectInfo *object, ObjectSyntax *value,
                             doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsTrafficParamEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                              doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsTrafficParamEntry_set_defaults(doList_t *dp)
{
    mplsTrafficParamEntry_t *data = (mplsTrafficParamEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsTrafficParamEntry_set(mplsTrafficParamEntry_t *data,
                            ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsTrafficParamEntry_UNDO
/* add #define SR_mplsTrafficParamEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsTrafficParamEntry family.
 */
int
mplsTrafficParamEntry_undo(doList_t *doHead, doList_t *doCur,
                           ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsTrafficParamEntry_UNDO */

#endif /* SETS */

mplsFTNObjects_t *
k_mplsFTNObjects_get(int serialNum, ContextInfo *contextInfo,
                     int nominator)
{
#ifdef NOT_YET
   static mplsFTNObjects_t mplsFTNObjectsData;

   /*
    * put your code to retrieve the information here
    */

   mplsFTNObjectsData.mplsFTNIndexNext = ;
   SET_ALL_VALID(mplsFTNObjectsData.valid);
   return(&mplsFTNObjectsData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

mplsFTNEntry_t *
k_mplsFTNEntry_get(int serialNum, ContextInfo *contextInfo,
                   int nominator,
                   int searchType,
                   SR_INT32 mplsFTNIndex)
{
#ifdef NOT_YET
   static mplsFTNEntry_t mplsFTNEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsFTNEntryData.mplsFTNIndex = ;
   mplsFTNEntryData.mplsFTNRowStatus = ;
   mplsFTNEntryData.mplsFTNDescr = ;
   mplsFTNEntryData.mplsFTNApplied = ;
   mplsFTNEntryData.mplsFTNMask = ;
   mplsFTNEntryData.mplsFTNAddrType = ;
   mplsFTNEntryData.mplsFTNSourceIpv4AddrMin = ;
   mplsFTNEntryData.mplsFTNSourceIpv6AddrMin = ;
   mplsFTNEntryData.mplsFTNSourceIpv4AddrMax = ;
   mplsFTNEntryData.mplsFTNSourceIpv6AddrMax = ;
   mplsFTNEntryData.mplsFTNDestIpv4AddrMin = ;
   mplsFTNEntryData.mplsFTNDestIpv6AddrMin = ;
   mplsFTNEntryData.mplsFTNDestIpv4AddrMax = ;
   mplsFTNEntryData.mplsFTNDestIpv6AddrMax = ;
   mplsFTNEntryData.mplsFTNSourcePortMin = ;
   mplsFTNEntryData.mplsFTNSourcePortMax = ;
   mplsFTNEntryData.mplsFTNDestPortMin = ;
   mplsFTNEntryData.mplsFTNDestPortMax = ;
   mplsFTNEntryData.mplsFTNProtocol = ;
   mplsFTNEntryData.mplsFTNActionType = ;
   mplsFTNEntryData.mplsFTNActionPointer = ;
   mplsFTNEntryData.mplsFTNExpBits = ;
   mplsFTNEntryData.mplsFTNStorageType = ;
   SET_ALL_VALID(mplsFTNEntryData.valid);
   return(&mplsFTNEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsFTNEntry_test(ObjectInfo *object, ObjectSyntax *value,
                    doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsFTNEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                     doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsFTNEntry_set_defaults(doList_t *dp)
{
    mplsFTNEntry_t *data = (mplsFTNEntry_t *) (dp->data);

    if ((data->mplsFTNDescr = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNMask = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    data->mplsFTNAddrType = D_mplsFTNAddrType_ipv4;
    if ((data->mplsFTNSourceIpv4AddrMin = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNSourceIpv6AddrMin = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNSourceIpv4AddrMax = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNSourceIpv6AddrMax = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNDestIpv4AddrMin = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNDestIpv6AddrMin = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNDestIpv4AddrMax = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNDestIpv6AddrMax = MakeOctetStringFromText("")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }
    if ((data->mplsFTNActionPointer = MakeOIDFromDot("0.0")) == 0) {
        return RESOURCE_UNAVAILABLE_ERROR;
    }

    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsFTNEntry_set(mplsFTNEntry_t *data,
                   ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsFTNEntry_UNDO
/* add #define SR_mplsFTNEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsFTNEntry family.
 */
int
mplsFTNEntry_undo(doList_t *doHead, doList_t *doCur,
                  ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsFTNEntry_UNDO */

#endif /* SETS */

mplsFTNMapEntry_t *
k_mplsFTNMapEntry_get(int serialNum, ContextInfo *contextInfo,
                      int nominator,
                      int searchType,
                      SR_INT32 mplsFTNMapIfIndex,
                      SR_INT32 mplsFTNMapPrevIndex,
                      SR_INT32 mplsFTNMapCurrIndex)
{
#ifdef NOT_YET
   static mplsFTNMapEntry_t mplsFTNMapEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsFTNMapEntryData.mplsFTNMapIfIndex = ;
   mplsFTNMapEntryData.mplsFTNMapPrevIndex = ;
   mplsFTNMapEntryData.mplsFTNMapCurrIndex = ;
   mplsFTNMapEntryData.mplsFTNMapRowStatus = ;
   mplsFTNMapEntryData.mplsFTNMapStorageType = ;
   SET_ALL_VALID(mplsFTNMapEntryData.valid);
   return(&mplsFTNMapEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

#ifdef SETS
int
k_mplsFTNMapEntry_test(ObjectInfo *object, ObjectSyntax *value,
                       doList_t *dp, ContextInfo *contextInfo)
{

    return NO_ERROR;
}

int
k_mplsFTNMapEntry_ready(ObjectInfo *object, ObjectSyntax *value, 
                        doList_t *doHead, doList_t *dp)
{

    dp->state = SR_ADD_MODIFY;
    return NO_ERROR;
}

int
k_mplsFTNMapEntry_set_defaults(doList_t *dp)
{
    mplsFTNMapEntry_t *data = (mplsFTNMapEntry_t *) (dp->data);


    SET_ALL_VALID(data->valid);
    return NO_ERROR;
}

int
k_mplsFTNMapEntry_set(mplsFTNMapEntry_t *data,
                      ContextInfo *contextInfo, int function)
{

   return COMMIT_FAILED_ERROR;
}

#ifdef SR_mplsFTNMapEntry_UNDO
/* add #define SR_mplsFTNMapEntry_UNDO in sitedefs.h to
 * include the undo routine for the mplsFTNMapEntry family.
 */
int
mplsFTNMapEntry_undo(doList_t *doHead, doList_t *doCur,
                     ContextInfo *contextInfo)
{
   return UNDO_FAILED_ERROR;
}
#endif /* SR_mplsFTNMapEntry_UNDO */

#endif /* SETS */

mplsFTNPerfEntry_t *
k_mplsFTNPerfEntry_get(int serialNum, ContextInfo *contextInfo,
                       int nominator,
                       int searchType,
                       SR_INT32 mplsFTNMapIfIndex,
                       SR_INT32 mplsFTNMapCurrIndex)
{
#ifdef NOT_YET
   static mplsFTNPerfEntry_t mplsFTNPerfEntryData;

   /*
    * put your code to retrieve the information here
    */

   mplsFTNPerfEntryData.mplsFTNMatchedPackets = ;
   mplsFTNPerfEntryData.mplsFTNMatchedOctets = ;
   mplsFTNPerfEntryData.mplsFTNMatchedHCPackets = ;
   mplsFTNPerfEntryData.mplsFTNMatchedHCOctets = ;
   mplsFTNPerfEntryData.mplsFTNMapIfIndex = ;
   mplsFTNPerfEntryData.mplsFTNMapCurrIndex = ;
   SET_ALL_VALID(mplsFTNPerfEntryData.valid);
   return(&mplsFTNPerfEntryData);
#else /* NOT_YET */
   return(NULL);
#endif /* NOT_YET */
}

