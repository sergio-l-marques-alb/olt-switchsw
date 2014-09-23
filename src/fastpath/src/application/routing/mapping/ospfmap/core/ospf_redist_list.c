/*********************************************************************
*
* (C) Copyright Broadcom Corporation 2003-2007
*
**********************************************************************
* @filename  ospf_redist_list.c
*
* @purpose   This file contains operations on the list of routes OSPF is
*            currently redistributing. 
*
* @component OSPF Mapping Layer
*
* @comments  The redistribution list is a collection of routes maintained
*            by the OSPF mapping layer. There is a one-to-one correspondence
*            between the routes in this list and the routes OSPF is currently
*            redistributing. The list contains at most one entry for a given
*            IP address/mask. Keeping this list in the mapping layer enables
*            the mapping layer to respond more efficiently to best route 
*            changes and to configuration changes that affect redistribution.
*            When these changes occur, the mapping layer can tell whether the
*            change leads to actual changes in the routes to be redistributed.
*            The mapping layer only has to disturb the protocol engine when 
*            there is a change to a redistributed route.
*
* @comments  The redistribution list is initially implemented as an AVL tree.
*            The AVL tree is sized to hold the maximum number of routes in
*            the routing table, since theoretically, OSPF could redistribute
*            every route in the routing table. The key to this tree is an
*            IP address plus mask.
*
* @comments  Access to elements in the list is protected by a semaphore. 
*            Functions that return values from the list always return a copy
*            of the data in the list. 
*
* @create    7/8/2003
*
* @author    rrice
*
* @end
*
**********************************************************************/

#include "l7_ospfinclude.h"
#include "avl_api.h"

static L7_uint32 ospfRedistRoutesMax = 0; 
static avlTreeTables_t   *ospfRedistRouteTreeHeap;   /* space for tree nodes */
static ospfRedistRoute_t *ospfRedistRouteDataHeap;   /* space for data */
static avlTree_t         ospfRedistRouteTree;        

L7_RC_t ospfRedistListShow(void);


/*********************************************************************
*
* @purpose Create the redistribution list for OSPF. The list is
*          implemented as an AVL tree. 
*
* @param none 
*
* @returns L7_SUCCESS if tree successfully created.
* @returns L7_FAILURE otherwise
*
* @comments Memory is allocated here for the maximum possible number of
*           nodes in the tree. Space is allocated both for tree nodes 
*           and the data each node points to. This space is only freed
*           if we undo phase 1 initialization for OSPF.
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListCreate(void)
{
    L7_uint32 avlType = 0x10;      /* common constant for all our AVL trees */

    ospfRedistRoutesMax = platRtrRouteMaxEntriesGet();

    ospfRedistRouteTreeHeap = 
        osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, ospfRedistRoutesMax * sizeof(avlTreeTables_t));
    if (ospfRedistRouteTreeHeap == NULL) {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "Error: unable to allocate OSPF redistribution tree heap.\n");
        return L7_FAILURE;
    }
    memset(ospfRedistRouteTreeHeap, 0, 
           ospfRedistRoutesMax * sizeof(avlTreeTables_t));

    ospfRedistRouteDataHeap = 
        osapiMalloc(L7_OSPF_MAP_COMPONENT_ID, ospfRedistRoutesMax * sizeof(ospfRedistRoute_t));
    if (ospfRedistRouteDataHeap == NULL) {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, ospfRedistRouteTreeHeap);
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "Error: unable to allocate OSPF redistribution data heap.\n");
        return L7_FAILURE;
    }
    memset(ospfRedistRouteDataHeap, 0, 
           ospfRedistRoutesMax * sizeof(ospfRedistRoute_t));

    memset(&ospfRedistRouteTree, 0, sizeof(avlTree_t));
    
    /* key is IP address plus mask. */ 
    avlCreateAvlTree(&ospfRedistRouteTree, ospfRedistRouteTreeHeap,
                     ospfRedistRouteDataHeap, ospfRedistRoutesMax,
                     (L7_uint32)(sizeof(ospfRedistRoute_t)), avlType, 
                     sizeof(L7_uint32) * 2);
    (void)avlSetAvlTreeComparator(&ospfRedistRouteTree, avlCompareIPNetAddr);

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Destroy the redistribution list for OSPF. 
*
* @param none 
*
* @returns L7_SUCCESS 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListDestroy(void)
{
    if (ospfRedistRouteTreeHeap != L7_NULLPTR) {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, ospfRedistRouteTreeHeap);
        ospfRedistRouteTreeHeap = L7_NULLPTR;
    }

    if (ospfRedistRouteDataHeap != L7_NULLPTR) {
        osapiFree(L7_OSPF_MAP_COMPONENT_ID, ospfRedistRouteDataHeap);
        ospfRedistRouteDataHeap = L7_NULLPTR;
    }

    memset(&ospfRedistRouteTree, 0, sizeof(avlTree_t));

    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Add a route to the redistribution list for OSPF.
*
* @param none 
*
* @returns L7_SUCCESS if route successfully added.
* @returns L7_FAILURE if the list already contains a route to the destination
*                     or if the list is full.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListAdd(ospfRedistRoute_t *redistRoute)
{
    L7_RC_t rc = L7_FAILURE;
    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    /* make sure we haven't exceeded max number of entries. */
    if (avlTreeCount(&ospfRedistRouteTree) >= ospfRedistRoutesMax) {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "SOFTWARE ERROR: Attempt to add to a full OSPF redistribution list\n.");
    }

    redistRoute->used = L7_TRUE;
    if (avlInsertEntry(&ospfRedistRouteTree, redistRoute) != NULL) {
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "Error inserting route into OSPF redistribution list\n.");
    }
    else {
        rc = L7_SUCCESS;
    }

    osapiSemaGive(ospfRedistRouteTree.semId);

    return rc;
}

/*********************************************************************
*
* @purpose In the redistribution list, update the attributes of a route 
*          being redistributed by OSPF.
*
* @param @b{(input)} redistRoute - A route being redistributed.
*
* @returns L7_SUCCESS if route is successfully modified.
* @returns L7_FAILURE otherwise
*
* @comments This function should be called when we know OSPF has previously
*           redistributed a route to a given destination, but because of
*           a configuration change or a best route change, the attributes
*           of that route have changed. 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListModify(ospfRedistRoute_t *redistRoute)
{
    L7_BOOL rc = L7_FAILURE;
    /* First, we have to find the route with the same destination in the tree */
    ospfRedistRoute_t *routeInTree;

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);
    routeInTree = avlSearchLVL7(&ospfRedistRouteTree, redistRoute, AVL_EXACT);
    if (routeInTree) {
        routeInTree->metric = redistRoute->metric;
        routeInTree->metricType = redistRoute->metricType;
        routeInTree->tag = redistRoute->tag;
        routeInTree->forwardingAddr = redistRoute->forwardingAddr;
        routeInTree->origNexthop = redistRoute->origNexthop;
        routeInTree->used = L7_TRUE;
        rc = L7_SUCCESS;
    }
    else {
        /* Shouldn't get here. */
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "SOFTWARE ERROR: Failed to find route in ospfMapRedistRouteModify().\n");
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Delete a route from the redistribution list for OSPF.
*
* @param @b{(input)} destAddr - destination IP address
* @param @b{(input)} destMask - destination network mask
*
* @returns L7_SUCCESS if route successfully deleted.
* @returns L7_FAILURE if the list does not contain the route to be deleted.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListDelete(L7_uint32 destAddr, L7_uint32 destMask)
{
    ospfRedistRoute_t dummyRoute;
    L7_RC_t rc = L7_FAILURE;
    dummyRoute.destAddr = destAddr;
    dummyRoute.destMask = destMask;
    
    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    if (avlDeleteEntry(&ospfRedistRouteTree, &dummyRoute)) {
        rc = L7_SUCCESS;
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Purge all routes from the redistribution list for OSPF.
*
* @param none
*
* @returns L7_SUCCESS 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListPurge(void)
{
    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);
    avlPurgeAvlTree(&ospfRedistRouteTree, ospfRedistRoutesMax);
    osapiSemaGive(ospfRedistRouteTree.semId);
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Purge all unused routes from the redistribution list for OSPF.
*
* @param none
*
* @returns L7_SUCCESS 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListPurgeUnused(void)
{
    ospfRedistRoute_t *redistRoute;
    ospfRedistRoute_t *nextRoute;
    ospfRedistRoute_t dummyRoute;
    memset(&dummyRoute, 0, sizeof(ospfRedistRoute_t));

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    /* Get first entry in the tree. */
    redistRoute = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_EXACT);
    if (redistRoute == NULL) {
        /* Not redistributing the default route, so get next route. */
        redistRoute = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_NEXT);
    }
    while (redistRoute) {
        nextRoute = avlSearchLVL7(&ospfRedistRouteTree, redistRoute, AVL_NEXT);
        if (redistRoute->used == L7_FALSE) {
            avlDeleteEntry(&ospfRedistRouteTree, redistRoute);
        }
        redistRoute = nextRoute;
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return L7_SUCCESS;
}

/*********************************************************************
*
* @purpose Find a route to a given destination in the redistribution 
*          list for OSPF.
*
* @param @b{(input)} redistRoute - destAddr and destMask must be set to the
*                                  values to be searched for.
* @param @b{(output)} redistRoute - If the list contains a route to the given
*                                   destination, the attributes of the route 
*                                   being redistributed are copied to 
*                                   redistRoute.
*
* @returns L7_SUCCESS if a matching route was found.
* @returns L7_FAILURE otherwise.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListFind(ospfRedistRoute_t *redistRoute)
{
    L7_RC_t rc = L7_FAILURE;
    ospfRedistRoute_t *routeInTree;
    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);
    routeInTree = avlSearchLVL7(&ospfRedistRouteTree, redistRoute, AVL_EXACT);
    if (routeInTree) {
        redistRoute->metric = routeInTree->metric;
        redistRoute->metricType = routeInTree->metricType;
        redistRoute->tag = routeInTree->tag;
        redistRoute->forwardingAddr = routeInTree->forwardingAddr;
        redistRoute->origNexthop = routeInTree->origNexthop;
        redistRoute->used = routeInTree->used;
        rc = L7_SUCCESS;
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Mark a route in the redistribution list "in use."
*
* @param @b{(input)} destAddr - destination IP address
* @param @b{(input)} destMask - destination network mask
*
* @returns L7_SUCCESS if matching route found and marked.
* @returns L7_FAILURE otherwise.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListMarkUsed(L7_uint32 destAddr, L7_uint32 destMask)
{
    L7_RC_t rc = L7_FAILURE;
    ospfRedistRoute_t *routeInTree;
    ospfRedistRoute_t dummyRoute;
    dummyRoute.destAddr = destAddr;
    dummyRoute.destMask = destMask;

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);
    routeInTree = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_EXACT);
    if (routeInTree) {
        routeInTree->used = L7_TRUE;
        rc = L7_SUCCESS;
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Mark all routes in the list inactive.
*
* @param none 
*
* @returns L7_SUCCESS 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListMarkAllUnused(void)
{
    ospfRedistRoute_t *redistRoute;
    ospfRedistRoute_t dummyRoute;
    memset(&dummyRoute, 0, sizeof(ospfRedistRoute_t));

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    /* Get first entry in the tree. */
    redistRoute = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_EXACT);
    if (redistRoute == NULL) {
        /* Not redistributing the default route, so get next route. */
        redistRoute = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_NEXT);
    }
    while (redistRoute) {
        redistRoute->used = L7_FALSE;
        redistRoute = avlSearchLVL7(&ospfRedistRouteTree, redistRoute, AVL_NEXT);
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return L7_SUCCESS;
}


/*********************************************************************
*
* @purpose Determine whether the attributes of a route being redistributed
*          have changed.
*
* @param @b{(input)} redistRoute - A route that should now be redistributed.
*
* @returns L7_TRUE if one or more attributes of the route that should be 
*                  redistributed differ from those of the route currently
*                  being redistributed.
* @returns L7_FALSE if there is no change.
*
* @comments 
*
* @end
*
*********************************************************************/
L7_BOOL ospfMapRedistRouteChanged(ospfRedistRoute_t *redistRoute)
{
    L7_BOOL rc = L7_FALSE;
    /* First, we have to find the route with the same destination in the tree */
    ospfRedistRoute_t *routeInTree;

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);
    routeInTree = avlSearchLVL7(&ospfRedistRouteTree, redistRoute, AVL_EXACT);
    if (routeInTree) {
        if ((routeInTree->metric != redistRoute->metric) ||
            (routeInTree->metricType != redistRoute->metricType) ||
            (routeInTree->tag != redistRoute->tag) ||
            (routeInTree->forwardingAddr != redistRoute->forwardingAddr)) {
            rc = L7_TRUE;
        }
        else {
            rc = L7_FALSE;
        }
    }
    else {
        /* Shouldn't get here. */
        L7_LOGF(L7_LOG_SEVERITY_INFO, L7_OSPF_MAP_COMPONENT_ID,
                "SOFTWARE ERROR: Failed to find route in ospfMapRedistRouteChanged().\n");
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Get the first route in the redistribution list.
*
* @param @b{(output)} redistRoute - A copy of the first route in the list,
*                                   if return is L7_SUCCESS.
*
* @returns L7_SUCCESS if the first route is found
* @returns L7_NOT_EXIST if the tree is empty
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListGetFirst(ospfRedistRoute_t *redistRoute)
{    
    L7_RC_t rc = L7_NOT_EXIST;
    ospfRedistRoute_t *routeInTree;
    ospfRedistRoute_t dummyRoute;
    memset(&dummyRoute, 0, sizeof(ospfRedistRoute_t));

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    /* Get first entry in the tree. */
    routeInTree = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_EXACT);
    if (routeInTree == NULL) {
        /* Not redistributing the default route, so get next route. */
        routeInTree = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_NEXT);
    }
    if (routeInTree != NULL) {
        redistRoute->destAddr = routeInTree->destAddr;
        redistRoute->destMask = routeInTree->destMask;
        redistRoute->metric = routeInTree->metric;
        redistRoute->metricType = routeInTree->metricType;
        redistRoute->tag = routeInTree->tag;
        redistRoute->forwardingAddr = routeInTree->forwardingAddr;
        redistRoute->origNexthop = routeInTree->origNexthop;
        redistRoute->used = routeInTree->used;
        rc = L7_SUCCESS;
    }

    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Given a route in the redistribution list, get the next route.
*
* @param destAddr @b{(input)} - destination IP address indicating where 
*                               to begin search
* @param destMask @b{(input)} - destination network mask indicating where
*                               to begin search
* @param nextRoute @b{(output)} A copy of the next route in the 
*                               redistribution tree. 
*
* @returns L7_SUCCESS if a next route is found.
* @returns L7_NOT_EXIST if there are no more routes. 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfMapRedistListGetNext(L7_uint32 destAddr, 
                                 L7_uint32 destMask,
                                 ospfRedistRoute_t *redistRoute)
{
    L7_RC_t rc = L7_NOT_EXIST;
    ospfRedistRoute_t *routeInTree;
    ospfRedistRoute_t dummyRoute;
    dummyRoute.destAddr = destAddr;
    dummyRoute.destMask = destMask;

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    /* Not redistributing the default route, so get next route. */
    routeInTree = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_NEXT);

    if (routeInTree != NULL) {
        redistRoute->destAddr = routeInTree->destAddr;
        redistRoute->destMask = routeInTree->destMask;
        redistRoute->metric = routeInTree->metric;
        redistRoute->metricType = routeInTree->metricType;
        redistRoute->tag = routeInTree->tag;
        redistRoute->forwardingAddr = routeInTree->forwardingAddr;
        redistRoute->origNexthop = routeInTree->origNexthop;
        redistRoute->used = routeInTree->used;
        rc = L7_SUCCESS;
    }

    osapiSemaGive(ospfRedistRouteTree.semId);
    return rc;
}

/*********************************************************************
*
* @purpose Display all routes in the redist list.
*
* @param none
*
* @returns L7_SUCCESS 
*
* @comments 
*
* @end
*
*********************************************************************/
L7_RC_t ospfRedistListShow(void)
{
    ospfRedistRoute_t *redistRoute;
    ospfRedistRoute_t dummyRoute;
    memset(&dummyRoute, 0, sizeof(ospfRedistRoute_t));

    osapiSemaTake(ospfRedistRouteTree.semId, L7_WAIT_FOREVER);

    printf("\nPrefix             Metric  Met Type       Tag               FA   Used");

    /* Get first entry in the tree. */
    redistRoute = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_EXACT);
    if (redistRoute == NULL) {
        /* Not redistributing the default route, so get next route. */
        redistRoute = avlSearchLVL7(&ospfRedistRouteTree, &dummyRoute, AVL_NEXT);
    }
    while (redistRoute) {
        L7_uchar8 pfxStr[OSAPI_INET_NTOA_BUF_SIZE];
        L7_uchar8 faStr[OSAPI_INET_NTOA_BUF_SIZE];
        osapiInetNtoa(redistRoute->destAddr, pfxStr);
        osapiInetNtoa(redistRoute->forwardingAddr, faStr);
        printf("\n%15s/%2d %8u %9d %9u %16s %4s",
               pfxStr, rtoMaskLength(redistRoute->destMask), redistRoute->metric, 
               (L7_uint32) redistRoute->metricType, redistRoute->tag, faStr,
               (redistRoute->used ? "Y" : "N"));
        
        redistRoute = avlSearchLVL7(&ospfRedistRouteTree, redistRoute, AVL_NEXT);
    }
    osapiSemaGive(ospfRedistRouteTree.semId);
    return L7_SUCCESS;
}
