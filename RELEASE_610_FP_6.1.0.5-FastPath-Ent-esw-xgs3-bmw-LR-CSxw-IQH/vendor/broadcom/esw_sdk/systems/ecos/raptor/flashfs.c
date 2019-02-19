//==========================================================================
//
//      flashfs.c
//
//      flash rom file system
//
//==========================================================================
#include <cyg/infra/cyg_type.h>
#include <pkgconf/io_fileio.h>
#include <cyg/fileio/fileio.h>

#define CYGNUM_FS_ROM_BASE_ADDRESS 0xbc800000

//==========================================================================

MTAB_ENTRY( romfs_mte1,
            "/",
            "romfs",
            "",
            (CYG_ADDRWORD)CYGNUM_FS_ROM_BASE_ADDRESS); /* put romfs in here to test */


            
