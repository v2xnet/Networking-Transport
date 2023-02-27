/***************************************************************************
 *   ITSNET  Intelligent Transport System networking Stack                 *
 * 									   *
 ** Copyright(C)2010 ESPRIT											   *
 * 	        "École supérieure privée d'ingénierie et de technologie"       *
 *                                                                         *
 *   barghich@gmail.com                                                    *
 *   anouar.chemek@gmail.com                                               *
 *  							                                           *
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
 * @file itsnet_listener.h
 * itsnet listener code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _ITSNET_LISTENER_H_
#define _ITSNET_LISTENER_H_ 1

#include "itsnet_types.h"
#include <sys/time.h>
static int send_socket(message* m);
int itsnet_configure_node_id_confirm_send();
int itsnet_get_node_id_confirm_send();
int itsnet_configure_node_id_request_receive(struct message* m);
int itsnet_get_node_id_request_receive(struct message* m);
int itsnet_get_neighbor_list_request_receive(struct message* m);
int itsnet_get_position_vector_confirm_send();
int itsnet_get_position_vector_request_receive(struct message* m);
int itsnet_position_sensor_confirm_send();
int itsnet_position_sensor_request_receive(struct message* m);
int itsnet_event_indication_send(struct message* m);
int itsnet_geounicast_request_receive(struct message* m);
int itsnet_geoanycast_request_receive(struct message* m);
int itsnet_geobroadcast_request_receive(struct message* m);
int itsnet_unicast_confirm_send(int res);
int itsnet_geoanycast_confirm_send(int res);
int itsnet_geobroadcast_confirm_send(int res);
int itsnet_unicast_indication_send(struct itsnet_packet* p);
int itsnet_geoanycast_indication_send(struct itsnet_packet* p);
int itsnet_geobroadcast_indication_send(struct itsnet_packet* p);

int itsnet_get_security_param_request_receive(struct message* m);
int itsnet_configure_security_param_request_receive(struct message* m);
static void socket_recv(struct message* m);
int itsnet_listener_init(void);
int server(int client_socket);
static void* listener(void* arg);
void itsnet_listener_cleanup(void);
void trace_emit_packet(struct timeval t_emit);

#endif /* _ITSNET_LISTENER_H_*/
