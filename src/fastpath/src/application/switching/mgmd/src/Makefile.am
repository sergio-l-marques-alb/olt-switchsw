include $(top_srcdir)/build/export.var
ACLOCAL_AMFLAGS = -I m4

lib_LTLIBRARIES = libmgmd.la

libmgmd_la_SOURCES = ptin_mgmd_defs.h ptin_mgmd_inet_defs.h ptin_utils_inet_addr_api.h ptin_mgmd_features.h snooping_ptin_defs.h \
	avl.c avl.h avl_api.h avl_util.c                                            \
	ptin_mgmd_ctrl.c ptin_mgmd_ctrl.h                                           \
	ptin_mgmd_osapi.c ptin_mgmd_osapi.h                                         \
	ptin_mgmd_querytimer.c ptin_mgmd_querytimer.h                               \
	ptin_mgmd_statistics.c ptin_mgmd_statistics.h                               \
	ptin_mgmd_cnfgr.c ptin_mgmd_cnfgr.h                                         \
	ptin_mgmd_cfg_api.c ptin_mgmd_cfg_api.h                                     \
	ptin_mgmd_eventqueue.c ptin_mgmd_eventqueue.h                               \
	ptin_mgmd_proxytimer.c ptin_mgmd_proxytimer.h                               \
	ptin_mgmd_service_api.c ptin_mgmd_service_api.h                             \
	ptin_timer_api.c ptin_timer_api.h                                           \
	sll.c sll_api.h                                                             \
	ptin_mgmd_db.c ptin_mgmd_db.h                                               \
	ptin_mgmd_util.c ptin_mgmd_util.h                                           \
	logger.c logger.h                                                           \
	ptin_mgmd_cfg.c ptin_mgmd_cfg.h                                             \
	ptin_mgmd_grouptimer.c ptin_mgmd_grouptimer.h                               \
	ptin_mgmd_querier.c ptin_mgmd_querier.h                                     \
	ptin_mgmd_sourcetimer.c ptin_mgmd_sourcetimer.h                             \
	ptin_utils_inet_addr.c                                                      \
	snooping.c snooping.h                                                       \
	ptin_mgmd_api.c ptin_mgmd_api.h                                             \
	ptin_mgmd_specificquery.c ptin_mgmd_specificquery.h                         \
	ptin_mgmd_groupsourcespecifictimer.c ptin_mgmd_groupsourcespecifictimer.h   \
	ptin_mgmd_routercmtimer.c ptin_mgmd_routercmtimer.h                         \
	ptin_mgmd_proxycmtimer.c ptin_mgmd_proxycmtimer.h     							 \
	ptin_fifo_api.c ptin_fifo_api.h   														 \
	ptin_mgmd_whitelist.c ptin_mgmd_whitelist.h   

libmgmd_la_CFLAGS = $(CFLAGS) $(MGMD_CFLAGS) -I./
libmgmd_la_LDFLAGS = $(LDFLAGS) -g -avoid-version

sbin_PROGRAMS = emulator
emulator_SOURCES = emulator.c
emulator_CFLAGS = $(CFLAGS) $(MGMD_CFLAGS)
emulator_LDADD = $(LD_PATH) $(LIBS) $(LDADD) $(top_srcdir)/src/libmgmd.la $(MGMD_LIBS)

if ENABLE_PTIN_BINARY
sbin_PROGRAMS += mgmd
mgmd_SOURCES = ptin_mgmd_api.c
mgmd_CFLAGS = $(CFLAGS) $(MGMD_CFLAGS)
mgmd_LDADD = $(LD_PATH) $(LIBS) $(LDADD) $(top_srcdir)/src/libmgmd.la $(MGMD_LIBS)
endif

#libmgmd_la_LIBADD = 
pkginclude_HEADERS = ptin_mgmd_eventqueue.h ptin_mgmd_ctrl.h ptin_mgmd_defs.h ptin_mgmd_api.h

