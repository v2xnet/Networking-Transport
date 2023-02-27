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
 * @file itsnet_types.h
 * itsnet types code.
 * @author Hichem BARGAOUI
 * @author Anouar CHEMEK
 */

#ifndef _ITSNET_TYPES_H_
#define _ITSNET_TYPES_H_ 1

#include <stdint.h>
#include "itsnet_header.h"

#define MAX_NEIGHBOR_LIST 100
#define MES_PAYLOAD_LEN 1500

/**
* type of sap
*/
enum itsnet_sap_id
{
	itsnet_management_sap = 1,	/** management sap 	 */
	itsnet_transport_sap,		/** transport layer sap  */
	itsnet_security_sap		/** security sap	 */
};

/**
* possible opcode in the message
*/
enum itsnet_opcode
{
	itsnet_request = 1,/** request */
	itsnet_indication,/** indication */
	itsnet_response,/** response */
	itsnet_confirm/** confirm */
};

/**
* identified the message action identifier
*/
enum itsnet_message_id
{

	/** management action */
	itsnet_configure_node_id = 1,	/** configure node_id */
	itsnet_get_node_id,		/** get node_id */
	itsnet_get_neighbor_list,	/** get_neighbor_list */
	itsnet_get_position_vector,	/** get_position_vector */
	itsnet_events,		/** event */
	itsnet_position_sensor,		/*** GPS-Position **/

	/** transport layer sap */

	itsnet_unicast,			/**  Geo-unicast  */
	itsnet_geoanycast,		/**  Geo-anycast */
	itsnet_geobroadcast,		/** Geo-broadcast */
	itsnet_geotopo,     /** Geo-topobroadcast */

	/** security sap */
	itsnet_get_security_param,	/** get_security_param */
	itsnet_configure_security_param	/** configure_security_param */

};

/**
 *common header for all itsnet messages
 */
struct message_header
{
	unsigned int sid;/** sap identifier */
	unsigned int opcode;/** operation code */
	unsigned int aid;/** action identifier */
};

typedef struct message_header message_header;

/**
 *The structure describes the position message
 */
struct itsnet_position_information
{

	uint8_t type;
	uint8_t accuracy;
	uint16_t reserved;
	uint32_t time_stamp;   /** UTC time in seconds, when the GPS data was calculated,NOT the time this message was generated */
	uint32_t latitude;  /** the latitude of the global position in 1/8 microdegree */
	uint32_t longitude;  /** the longitude of the global position in 1/8 microdegree*/
	uint16_t speed;  /** current speed in 0.01 meters per second*/
	uint16_t heading;  /** current curse in 0.005493247 degrees*/
	uint16_t altitude;    /** the altitude (meter over mean sea level)*/

};

typedef struct itsnet_position_information itsnet_position_information;

/**
 *The structure describes itsnet_configure_node_id_request
 */
struct itsnet_configure_node_id_request
{

	struct itsnet_node_id node_id;
};

typedef struct itsnet_configure_node_id_request itsnet_configure_node_id_request;

/**
 *The structure describes itsnet_configure_node_id_confirm
 */
struct itsnet_configure_node_id_confirm
{

	struct itsnet_node_id old_id;
	struct itsnet_node_id new_id;
	int result;

};

typedef struct itsnet_configure_node_id_confirm itsnet_configure_node_id_confirm;

/**
 *The structure describes itsnet_get_node_id_request
 */
struct itsnet_get_node_id_request
{

	int tmp;

};

typedef struct itsnet_get_node_id_request itsnet_get_node_id_request;

/**
 *The structure describes itsnet_get_node_id_confirm
 */
struct itsnet_get_node_id_confirm
{

	struct itsnet_node_id node_id;
	int result;

};

typedef struct itsnet_get_node_id_confirm itsnet_get_node_id_confirm;

/**
 *The structure describes itsnet_get_neighbor_list_request
 */
struct itsnet_get_neighbor_list_request
{

	int tmp;

};

typedef struct itsnet_get_neighbor_list_request itsnet_get_neighbor_list_request;

/**
 *The structure describes itsnet_get_neighbor_list_confirm
 */
struct itsnet_get_neighbor_list_confirm
{

	int neighbor_list_size;
	itsnet_position_vector neighbor_list_pos[MAX_NEIGHBOR_LIST];

};

typedef struct itsnet_get_neighbor_list_confirm itsnet_get_neighbor_list_confirm;

/**
 *The structure describes itsnet_get_position_vector_request
 */
struct itsnet_get_position_vector_request
{

	int tmp;

};

typedef struct itsnet_get_position_vector_request itsnet_get_position_vector_request;

/**
 *The structure describes itsnet_get_position_vector_confirm
 */
struct itsnet_get_position_vector_confirm
{

	itsnet_position_vector position_vector;
	int result;

};

typedef struct itsnet_get_position_vector_confirm itsnet_get_position_vector_confirm;

/**
 *The structure describes itsnet_event_indication
 */
struct itsnet_event_indication
{

	itsnet_event event;

};

typedef struct itsnet_event_indication itsnet_event_indication;


/**
 *The structure describes itsnet_position_sensor_request
 */
struct itsnet_position_sensor_request
{

	struct itsnet_position_information position_information;
};

typedef struct  itsnet_position_sensor_request   itsnet_position_sensor_request ;

/**
 *The structure describes itsnet_position_sensor_confirm
 */
struct itsnet_position_sensor_confirm
{

	int result;  /** optionnal */
};

typedef struct  itsnet_position_sensor_confirm   itsnet_position_sensor_confirm ;

/**
 *The structure describes itsnet_unicast_request
 */
struct itsnet_unicast_request
{

	struct itsnet_node_id destination_node_id;
	uint32_t destination_latitude;
	uint32_t destination_longitude;
	char data[MES_PAYLOAD_LEN];

};

typedef struct itsnet_unicast_request itsnet_unicast_request;

/**
 *The structure describes itsnet_unicast_confirm
 */
struct itsnet_unicast_confirm
{

	int result;

};

typedef struct itsnet_unicast_confirm itsnet_unicast_confirm;

/**
 *The structure describes itsnet_unicast_indication
 */

struct itsnet_unicast_indication
{

	struct itsnet_node_id source_node_id;
	char data[MES_PAYLOAD_LEN];
};

typedef struct itsnet_unicast_indication itsnet_unicast_indication;

/**
 *itsnet geo-area
 */
struct itsnet_geo_area
{

	int geo_area_type;
	itsnet_radius geo_area_size;
	uint32_t latitude;  /** the latitude of the global position in 1/8 microdegree */
	uint32_t longitude; /** the longitude of the global position in 1/8 microdegree*/
};

typedef struct itsnet_geo_area itsnet_geo_area;

/**
 *The structure describes itsnet_geoanycast_request
 */
struct itsnet_geoanycast_request
{

	itsnet_geo_area geo_area;
	char data[MES_PAYLOAD_LEN];
};

typedef struct itsnet_geoanycast_request itsnet_geoanycast_request;

/**
 *The structure describes itsnet_geoanycast_confirm
 */
struct itsnet_geoanycast_confirm
{

	int result;

};

typedef struct itsnet_geoanycast_confirm itsnet_geoanycast_confirm;

/**
 *The structure describes itsnet_anycast_indication
 */

struct itsnet_geoanycast_indication
{

	struct itsnet_node_id source_node_id;
	char data[MES_PAYLOAD_LEN];
};

typedef struct itsnet_geoanycast_indication itsnet_geoanycast_indication;

/**
 *The structure describes itsnet_geobroadcast_request
 */
struct itsnet_geobroadcast_request
{
	itsnet_geo_area geo_area;
	char data[MES_PAYLOAD_LEN];
};

typedef struct itsnet_geobroadcast_request itsnet_geobroadcast_request;

/**
 *The structure describes itsnet_geobroadcast_confirm
 */
struct itsnet_geobroadcast_confirm
{

	int result;
};

typedef struct itsnet_geobroadcast_confirm itsnet_geobroadcast_confirm;

/**
 *The structure describes itsnet_geobroadcast_indication
 */

struct itsnet_geobroadcast_indication
{

	itsnet_position_vector source_node_pos;
	itsnet_position_vector forwarder_node_pos;
	char data[MES_PAYLOAD_LEN];

};

typedef struct itsnet_geobroadcast_indication itsnet_geobroadcast_indication;


/**
 *The structure describes itsnet_geotopobroadcast_request
 */
struct itsnet_geotopo_request
{
	itsnet_hop_limit hop_limit;
	char data[MES_PAYLOAD_LEN];
};

typedef struct itsnet_geotopo_request itsnet_geotopo_request;

/**
 *The structure describes itsnet_geotopobroadcast_confirm
 */
struct itsnet_geotopo_confirm
{

	int result;
};

typedef struct itsnet_geotopo_confirm itsnet_geotopo_confirm;

/**
 *The structure describes itsnet_geotopobroadcast_indication
 */

struct itsnet_geotopo_indication
{

	itsnet_position_vector source_node_pos;
	itsnet_position_vector forwarder_node_pos;
	char data[MES_PAYLOAD_LEN];

};

typedef struct itsnet_geotopo_indication itsnet_geotopo_indication;


/**
 *The structure describes itsnet_get_security_param_request
 */
struct itsnet_get_security_param_request
{
	int tmp;
};

typedef struct itsnet_get_security_param_request itsnet_get_security_param_request;

/**
 *The structure describes itsnet_configure_security_param_request
 */
struct itsnet_configure_security_param_request
{

	int tmp;

};

typedef struct itsnet_configure_security_param_request itsnet_configure_security_param_request;

/**
 *The structure describes Message types
 */
struct message
{

	struct message_header msg_hdr;/** message header */

	union  payload_message   /**this is to reserve the maximum space used by messages*/
	{

		struct itsnet_configure_node_id_request itsnet_configure_node_id_req;
		struct itsnet_configure_node_id_confirm itsnet_configure_node_id_conf;
		struct itsnet_get_node_id_request itsnet_get_node_id_req;
		struct itsnet_get_node_id_confirm itsnet_get_node_id_conf;
		struct itsnet_get_neighbor_list_request itsnet_get_neighbor_list_req;
		struct itsnet_get_neighbor_list_confirm itsnet_get_neighbor_list_conf;
		struct itsnet_get_position_vector_request itsnet_get_position_vector_req;
		struct itsnet_get_position_vector_confirm itsnet_get_position_vector_conf;
		struct itsnet_event_indication itsnet_event_ind;
		struct itsnet_position_sensor_request itsnet_position_sensor_req;
		struct itsnet_position_sensor_confirm itsnet_position_sensor_conf;

		struct itsnet_unicast_request itsnet_unicast_req;
		struct itsnet_unicast_confirm itsnet_unicast_conf;
		struct itsnet_unicast_indication itsnet_unicast_ind;
		struct itsnet_geoanycast_request itsnet_geoanycast_req;
		struct itsnet_geoanycast_confirm itsnet_geoanycast_conf;
		struct itsnet_geoanycast_indication itsnet_geoanycast_ind;
		struct itsnet_geobroadcast_request itsnet_geobroadcast_req;
		struct itsnet_geobroadcast_confirm itsnet_geobroadcast_conf;
		struct itsnet_geobroadcast_indication itsnet_geobroadcast_ind;
		struct itsnet_geotopo_request itsnet_geotopo_req;
		struct itsnet_geotopo_confirm itsnet_geotopo_conf;
		struct itsnet_geotopo_indication itsnet_geotopo_ind;

		struct itsnet_get_security_param_request itsnet_get_security_param_req;
		struct itsnet_configure_security_param_request itsnet_configure_security_param_req;

	} payload;

};

typedef struct message message;

#endif	/* _ITSNET_TYPES_H_*/
