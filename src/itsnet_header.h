/***************************************************************************
 *                              CarGeo6 Project                            *
 *         C2CNet layer implementation conforming with GeoNet D2.2         *
 *                                                                         *
 *                      Copyright(C)2010 ESPRIT-INRIA                      *
 *                                                                         *
 *                                                                         *
 *   Authors:                                                              *
 *   Hichem BARGAOUI <barghich@gmail.com>                                  *
 *   Anouar CHEMEK <anouar.chemek@gmail.com>                               *
 *                                                                         *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

/**
 * @file itsnet_header.h
 * itsnet header code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#ifndef ITSNET_HEADER_H
#define ITSNET_HEADER_H 1

#include <stdint.h>
#include "itsnet_constant.h"



typedef uint16_t itsnet_payload_lenght; /** total lenght of the datagram in octet (data + header) */
typedef uint8_t itsnet_hop_limit;       /** time to leave, max number of hops */
typedef uint32_t itsnet_time_stamp;     /** time stamp at which the position was acquired*/

/*typedef char itsnet_node_id[8];*/ /** identifier of the ITS station (not yet defined)*/

typedef uint32_t itsnet_latitude;     /** latitude of the ITS station */
typedef uint32_t itsnet_longitude;    /** longitude of the ITS station*/
typedef uint16_t itsnet_speed;        /** speed of the ITS station*/
typedef uint16_t itsnet_heading;      /** heading of the ITS station*/
typedef uint16_t itsnet_altitude;     /** altitude of the ITS station*/
typedef uint8_t itsnet_txpower;       /** Transmission power level with which the packet was sent, in 1⁄2 of dBm*/
typedef uint8_t itsnet_flags;         /** Flags reserved to distinguish vehicle and RSU */
typedef uint8_t itsnet_traffic_class; /** Traffic class parameter*/

/** The transport porotocol, i.e. the usage of port numbers are still under discussion.*/
typedef uint16_t itsnet_source_port;      /** Port number of the sending application. It is zero if not used */
typedef uint16_t itsnet_destination_port; /**Port number of the destination application. It is zero if not used.*/
typedef uint16_t itsnet_radius;
typedef uint8_t itsnet_channel;

/**
 *The structure describes itsnet identity
 */

struct itsnet_node_id {
    uint8_t id[NODE_ID_LEN];
};

typedef struct itsnet_node_id itsnet_node_id;

/**
 *itsnet position vector accuracy
 */

struct itsnet_accuracy {
    uint8_t time_ac : 4;  /**time accuracy */
    uint8_t pos_ac : 4;   /**position accuracy*/
    uint8_t speed_ac : 3; /**speed accuracy */
    uint8_t head_ac : 3;  /**heading accuracy */
    uint8_t alt_ac : 2;   /**altitude accuracy */
};
typedef struct itsnet_accuracy itsnet_accuracy;

/**
 * @class itsnet_position_vector
 * @author anouar chemek
 * @date 27/03/21
 * @file itsnet_header.h
 * @brief itsnet short position vector
 */
struct itsnet_short_position_vector{
    itsnet_node_id node_id;
    itsnet_time_stamp time_stamp;
    itsnet_latitude latitude;   /** the latitude of the global position in 1/8 microdegree */
    itsnet_longitude longitude; /** the longitude of the global position in 1/8 microdegree*/
};
typedef struct itsnet_short_position_vector itsnet_short_position_vector;

/**
 *itsnet position vector
 */
struct itsnet_position_vector {
    itsnet_node_id node_id;
    itsnet_time_stamp
        time_stamp; /** UTC time in seconds, when the GPS data was calculated,NOT the time this message was generated */
    itsnet_latitude latitude;   /** the latitude of the global position in 1/8 microdegree */
    itsnet_longitude longitude; /** the longitude of the global position in 1/8 microdegree*/
    itsnet_speed speed;         /** current speed in 0.01 meters per second*/
    itsnet_heading heading;     /** current curse in 0.005493247 degrees*/
};
typedef struct itsnet_position_vector itsnet_position_vector;


/**
 * @class itsnet_basic_header
 * @author anouar chemek
 * @date 25/03/21
 * @file itsnet_header.h
 * @brief The Basic Header shall be present in every GeoNetworking packet and consists of the fields as depicted in
 * EN 302 636-4-1 - V1.3.1
 */
struct itsnet_basic_header {
    uint8_t itsnet_version_next_header;     /** 4bits Protocol Version 4bits next header */
    uint8_t reserved;               /** Set to 0 */
    uint8_t itsnet_lifetime; /**the maximum tolerable time a packet may be buffered until it reaches its destination */
    uint8_t itsnet_rhl;      /**[hops] */
};
typedef struct itsnet_basic_header itsnet_basic_header;




/**
 * @class itsnet_common_header
 * @author anouar chemek
 * @date 25/03/21
 * @file itsnet_header.h
 * @brief The Common Header shall be present in every GeoNetworking packet and consists of the fields as depicted in 
 *  EN 302 636-4-1 - V1.3.1
 */
struct itsnet_common_header {
    uint8_t itsnet_next_header;
    uint8_t itsnet_header_type_subtype;
    itsnet_traffic_class traffic_class;
    itsnet_flags flags;
    itsnet_payload_lenght payload_lenght;
    itsnet_hop_limit max_hop_limit;
    uint8_t reserved;
};

typedef struct itsnet_common_header itsnet_common_header;


/**
 * value of header type
 */
enum itsnet_header_type {

    itsnet_any_id = 0,      /** value of packet unspecified */
    itsnet_beacon_id,       /** value of packet Beacon */
    itsnet_unicast_id,      /** value of packet Geo-unicast  */
    itsnet_geoanycast_id,   /** value of packet Geo-anycast */
    itsnet_geobroadcast_id, /** value of packet Geo-broadcast */
    itsnet_tsb_id,          /** value of packet Topologically-scoped broadcast */
    itsnet_ls_id            /** value of packet Location service */
};

/**
 *The structure describes itsnet_any packet
 */
struct itsnet_any_t {
    itsnet_common_header header;

    /** unspecified */
};

typedef struct itsnet_any_t itsnet_any_t;

/**
 *The structure describes beacon packet
 */
struct itsnet_beacon_t {
    itsnet_position_vector source_position_vector;
};

typedef struct itsnet_beacon_t itsnet_beacon_t;

/**
 *The structure describes itsnet_unicast packet
 */
struct itsnet_unicast_t {
    itsnet_position_vector source_position_vector; /** Source node position vector */
    itsnet_node_id dest_node_id;
    itsnet_latitude dest_latitude;
    itsnet_longitude dest_longitude;
    short payload[ITSNET_DATA_SIZE]; /** data temp must be fixed*/
    itsnet_position_vector forwarder_position_vector; /** The Position Vector of the last forwarder (node from which the packet comes)*/

};

typedef struct itsnet_unicast_t itsnet_unicast_t;

/**
 *The structure describes itsnet_geoanycast packet
 */
struct itsnet_geoanycast_t {
    itsnet_position_vector source_position_vector; /** Source node position vector */
    itsnet_radius geo_area_size;                   /** radius/height,latitude and longitude (geo-area destination)   */
    itsnet_latitude dest_latitude;
    itsnet_longitude dest_longitude;
    short payload[ITSNET_DATA_SIZE]; /** data temp must be fixed*/
    itsnet_position_vector forwarder_position_vector; /** The Position Vector of the last forwarder (node from which the packet comes)*/

};

typedef struct itsnet_geoanycast_t itsnet_geoanycast_t;

/**
 *The structure describes itsnet_geobroadcast packet
 */
struct itsnet_geobroadcast_t {
    itsnet_position_vector source_position_vector; /** Source node position vector */
    itsnet_radius geo_area_size;                   /** radius/height,latitude and longitude (geo-area destination)   */
    itsnet_latitude dest_latitude;
    itsnet_longitude dest_longitude;
    short payload[ITSNET_DATA_SIZE]; /** data temp must be fixed*/
    itsnet_position_vector forwarder_position_vector; /** The Position Vector of the last forwarder (node from which the packet comes)*/

};
typedef struct itsnet_geobroadcast_t itsnet_geobroadcast_t;

/**
 *The structure describes itsnet_tsb packet
 */
struct itsnet_tsb_t {
    itsnet_position_vector source_position_vector; /** Source node position vector */
    short payload[ITSNET_DATA_SIZE];               /** data temp must be fixed*/
    itsnet_position_vector forwarder_position_vector; /** The Position Vector of the last forwarder (node from which the packet comes)*/

};

typedef struct itsnet_tsb_t itsnet_tsb_t;

/**
 *The structure describes the Location service request subtypes
 */

struct itsnet_ls_request {

    itsnet_position_vector source_position_vector; /** Source node position vector */
    itsnet_node_id request_id;                     /** Request ID */
};

typedef struct itsnet_ls_request itsnet_ls_request;

/**
 *The structure describes the Location service reply subtypes
 */
struct itsnet_ls_reply {

    itsnet_position_vector source_position_vector; /** Source node position vector */
    itsnet_node_id dest_node_id;
    itsnet_latitude dest_latitude;
    itsnet_longitude dest_longitude;

};

typedef struct itsnet_ls_reply itsnet_ls_reply;

/**
 *The structure describes the Location service packet
 */
struct itsnet_ls_t {
    union ls_t {
        struct itsnet_ls_request ls_request;
        struct itsnet_ls_reply ls_reply;
    } ls;
    itsnet_position_vector forwarder_position_vector; /** The Position Vector of the last forwarder (node from which the packet comes)*/

};

typedef struct itsnet_ls_t itsnet_ls_t;

/**
 *The structure describes its packet
 */
struct itsnet_packet {
    struct itsnet_basic_header basic_header;
    struct itsnet_common_header common_header; /** packet header */
    union payload_packet                       /**this is to reserve the maximum space used by packets*/
    {
        struct itsnet_any_t itsnet_any;                   /** unspecified */
        struct itsnet_beacon_t itsnet_beacon;             /** Beacon */
        struct itsnet_unicast_t itsnet_unicast;           /** Geo-unicast  */
        struct itsnet_geoanycast_t itsnet_geoanycast;     /** Geo-anycast */
        struct itsnet_geobroadcast_t itsnet_geobroadcast; /** Geo-broadcast */
        struct itsnet_tsb_t itsnet_tsb;                   /** Topologically-scoped broadcast */
        struct itsnet_ls_t itsnet_ls;                     /** Location service */
    } payload;
    
};

typedef struct itsnet_packet itsnet_packet;

/**
 *The structure describes adress
 */

struct mac_addr {
    uint8_t address[MAX_LLA_LEN];
};

typedef struct mac_addr mac_addr;

/**
 *The structure describes Itsnet events
 */

struct itsnet_event {
    int type;
    union event_t {
        itsnet_position_vector new_neighbor;
        int other;
    } event;
};
typedef struct itsnet_event itsnet_event;

#endif /* ITSNET_HEADER_H */
