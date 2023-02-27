bin_PROGRAMS = itsnet
itsnet_SOURCES = beaconing.c dll_sap.c geo_anycast.c geo_broadcast.c geo_topo.c \
	geo_unicast.c itsnet.c itsnet_event_service.c itsnet_listener.c \
	itsnet_parser.c itsnet_pseudonym.c itsnet_security.c location_service.c \
	location_table.c main.c position_calculation.c position_sensor.c routing_manager.c \
	store_forward.c tqueue.c

# set the include path found by configure
INCLUDES= $(all_includes)

# the library search path.
itsnet_LDFLAGS = $(all_libraries) 
noinst_HEADERS = beaconing.h dll_sap.h geo_anycast.h geo_broadcast.h geo_topo.h \
	geo_unicast.h itsnet.h itsnet_header.h itsnet_parser.h itsnet_security.h list.h \
	location_service.h location_table.h position_calculation.h position_sensor.h \
	routing_manager.h store_forward.h tqueue.h util.h

_SOURCES = debug.h itsnet_conf.h itsnet_event_service.h \
	itsnet_listener.h itsnet_pseudonym.h
_SOURCES = itsnet_types.h
itsnet_LDADD = -lm -lconfuse
