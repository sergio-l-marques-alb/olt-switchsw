int trunk_get(int tid)
{
    bcm_trunk_info_t trunk_info;
    int i, members_max = 10;
    bcm_trunk_member_t member_array[10];
    bcm_gport_t gport_trunk;
    int rv=BCM_E_NONE;

    rv = bcm_trunk_get(0, tid, &trunk_info, members_max, member_array, &members_max);
    if (rv != BCM_E_NONE)
    {
	printf("Error: Error getting information for tid=%d (rv=%d: %s)\r\n", tid, rv, bcm_errmsg(rv));
	return rv;
    }

    BCM_GPORT_TRUNK_SET(&gport_trunk, tid);

    printf("TID=%d, Gport=0x%x\r\n", tid, gport_trunk);
    printf("PSC=%d\r\n", trunk_info.psc);
    printf("Number of members: %d\r\n", members_max);
    printf("Members: { ");
    for (i = 0; i < members_max; i++)
    {
	printf("0x%x ", member_array[i].gport);
    }
    printf("}\r\n");

    return 0;
}



int trunk_create(int tid, int psc)
{
    bcm_trunk_member_t member_array;
    bcm_trunk_info_t trunk_info;
    bcm_gport_t gport_trunk;
    int rv= BCM_E_NONE;

    sal_memset(&member_array, 0, sizeof(member_array));
    sal_memset(&trunk_info, 0, sizeof(trunk_info));

   /*
    * Creates #tid trunk (no memory allocation and no members inside)
    */
    rv =  bcm_trunk_create(0, ((tid != 0) ? BCM_TRUNK_FLAG_WITH_ID : 0), &tid);
    if(rv != BCM_E_NONE && rv != BCM_E_EXISTS)
    {
	printf("Error: bcm_trunk_create(), rv=%d %s\n", rv,bcm_errmsg(rv));
	return rv;
    }
    printf("trunk creat: tid=%d\n", tid);

    trunk_info.psc= psc;

    /*
     * Adds members (in member_array) to trunk and activates trunk mechanism
     */
    rv = bcm_trunk_set(0, tid, &trunk_info, 0, &member_array);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_set(), rv=%d %s\n", rv,bcm_errmsg(rv));
        return rv;
    }

    /* Translate the trunk id to a gport id to be used for the ehternet port. */
    BCM_GPORT_TRUNK_SET(&gport_trunk, tid);

    printf("gport_trunk=0x%x\r\n", gport_trunk);
	
    return gport_trunk;
}

int trunk_delete(bcm_trunk_t tid)
{
    int rv;
    rv = bcm_trunk_destroy(unit, tid);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_destroy(), rv=%d %s\n", rv,bcm_errmsg(rv));
        return rv;
    }

    return rv;
}

/*
* Example how to delete port to an existing trunk tid
*/

int trunk_member_delete(bcm_trunk_t tid, int local_port)
{
    int rv;
    bcm_trunk_member_t member_array;

    BCM_GPORT_SYSTEM_PORT_ID_SET(member_array.gport, local_port);
    /*
     *  bcm_trunk_member_add adds member_array to an existing tid.
     *  - if multiple instances of a same port is in trunk, just one instance of it will be deleted
     */
    rv = bcm_trunk_member_delete(0, tid, &member_array);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_member_delete(), rv=%d\n", rv);
        return rv;
    }

    printf("Port 0x%x removed from TID %d\r\n", member_array.gport, tid);

    return rv;
}

int trunk_member_add(bcm_trunk_t tid, int local_port)
{
    int rv;
    bcm_trunk_member_t member_array;

    BCM_GPORT_SYSTEM_PORT_ID_SET(member_array.gport, local_port);
    /*
     *  bcm_trunk_member_add adds member_array to an existing tid.
     *  - multiple instances of a same port is allowed (to change ballance load)
     *  - same port cannot be member of different tid's
     */
    rv =  bcm_trunk_member_add(0, tid, &member_array);
    if(rv != BCM_E_NONE)
    {
        printf("Error: bcm_trunk_member_add(), rv=%d\n", rv);
        return rv;
    }

    printf("Port 0x%x added to TID %d\r\n", member_array.gport, tid);

    return rv;
}

