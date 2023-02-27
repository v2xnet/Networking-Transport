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
 * @file location_service.c
 * location service code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include <errno.h>
#include <unistd.h>

#include <asm/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dll_sap.h"
#include "geo_unicast.h"
#include "itsnet_parser.h"
#include "location_service.h"
#include "location_table.h"
#include "position_sensor.h"

void list_process_location_packet(struct list_head* location_packet_list,
    itsnet_node_id id,
    uint32_t latitude,
    uint32_t longitude)
{
    struct list_head* packet_list_entry;

    list_for_each(packet_list_entry, location_packet_list)
    {
        itsnet_location_packet* i;
        i = list_entry(packet_list_entry, struct itsnet_location_packet, list);
        if((memcmp(&id, &(i->p->payload.itsnet_ls.ls.ls_reply.dest_node_id), NODE_ID_LEN) == 0)) {
            printf("The Reply location service is destinated to this NODE");
            print_node_id(id);
            printf("Send a Unicast Packet for the Node who sent the request location service \n");

            i->p->payload.itsnet_unicast.dest_node_id = id;
            i->p->payload.itsnet_unicast.dest_latitude = latitude;
            i->p->payload.itsnet_unicast.dest_longitude = longitude;

            itsnet_geounicast_send(i->p);
        }
    }
}

int itsnet_location_packet_list_add(struct list_head* location_packet_list, itsnet_location_packet* packet)
{
    itsnet_location_packet* new_packet;
    struct timespec exp_in;
    tssetmsec(exp_in, 5000);

    if((new_packet = malloc(sizeof(struct itsnet_location_packet))) == NULL) {
        return -1;
    }
    memcpy(new_packet, packet, sizeof(struct itsnet_location_packet));
    new_packet->expires = exp_in;
    INIT_LIST_HEAD(&(new_packet->tqe.list));

    add_task_rel(&exp_in, &(new_packet->tqe), location_packet_remove);

    list_add_tail(&new_packet->list, location_packet_list);
    return 0;
}

void location_packet_remove(struct tq_elem* tqe)
{

    itsnet_location_packet* packet;
    packet = tq_data(tqe, itsnet_location_packet, tqe);
    list_del(&packet->list);
    free(packet);
}

/**
 * location request
 * @param itsnet_node_id node_id
 * @return int
 */
int itsnet_location_request(struct itsnet_packet* unicast_packet)
{

    int result;
    struct itsnet_packet* p;
    struct itsnet_position_vector pos;

    itsnet_location_packet* packet;

    pos = get_position_vector();

    uint8_t brodcast_mac[MAX_LLA_LEN];
    memcpy(brodcast_mac, BroadcastMac_parse(), MAX_LLA_LEN);

    p = (itsnet_packet*)malloc(sizeof(itsnet_packet));
    packet = (itsnet_location_packet*)malloc(sizeof(itsnet_location_packet));

    if(p == NULL) {
        printf("erreur allocation \n");
        result = -1;
    } else {

        memset(p, 0, sizeof(itsnet_packet));
        
        p->common_header.itsnet_header_type_subtype = HI_NIBBLE(itsnet_ls_id);
        p->common_header.itsnet_header_type_subtype |= LO_NIBBLE(LS_REQUEST);
        
        p->common_header.traffic_class = CLASS02;
        p->common_header.flags = OBU_SECURITY_ENABLED;
        p->common_header.payload_lenght = ITSNET_DATA_SIZE;
        p->common_header.max_hop_limit = 255;
        p->payload.itsnet_ls.forwarder_position_vector = pos;
        
        p->common_header.itsnet_next_header = HI_NIBBLE(ANY);
        p->common_header.itsnet_next_header |= LO_NIBBLE(0);

        p->payload.itsnet_ls.ls.ls_request.request_id = unicast_packet->payload.itsnet_unicast.dest_node_id;
        p->payload.itsnet_ls.ls.ls_request.source_position_vector = pos;
    }

    if(itsnet_packet_send(p, brodcast_mac) == -1) {
        printf("erreur envoie packet \n");
        result = -1;
    }

    printf("Adding The Packet to the Packet_list");

    packet->p = (itsnet_packet*)malloc(sizeof(itsnet_packet));

    memcpy(packet->p, unicast_packet, sizeof(struct itsnet_packet));

    itsnet_location_packet_list_add(&location_packet_list, packet);

    free(p);
    return 0;
}

/**
 * location reply
 * @param
 */
int itsnet_location_reply(struct itsnet_packet* p)
{

    int result = 0;
    struct itsnet_position_vector pos;
    pos = get_position_vector();
    uint8_t brodcast_mac[MAX_LLA_LEN];
    memcpy(brodcast_mac, BroadcastMac_parse(), MAX_LLA_LEN);

    struct itsnet_node_id node_id_req;
    struct itsnet_node* node;

    node_id_req = p->payload.itsnet_ls.ls.ls_request.request_id;

    node = list_find_node(&neighbor_list, &node_id_req);

    if(node != NULL) {
        p = (itsnet_packet*)malloc(sizeof(itsnet_packet));

        if(p == NULL) {
            printf("erreur allocation \n");
            result = -1;
            return result;
        }

        printf("NodeId destination is a neighbor \n");
        //			node_id=p->payload.itsnet_unicast.dest_node_id;

        memset(p, 0, sizeof(itsnet_packet));

        p->common_header.itsnet_header_type_subtype = HI_NIBBLE(itsnet_ls_id);
        p->common_header.itsnet_header_type_subtype |= LO_NIBBLE(LS_REQUEST);

        p->common_header.traffic_class = CLASS02;
        p->common_header.flags = OBU_SECURITY_ENABLED;
        p->common_header.payload_lenght = ITSNET_DATA_SIZE;
        p->common_header.max_hop_limit = 255;
        p->payload.itsnet_ls.forwarder_position_vector = pos;
        p->common_header.itsnet_next_header = HI_NIBBLE(ANY);
        p->common_header.itsnet_next_header = LO_NIBBLE(0);

        p->payload.itsnet_ls.ls.ls_reply.source_position_vector = pos;
        p->payload.itsnet_ls.ls.ls_reply.dest_node_id = node_id_req;
        p->payload.itsnet_ls.ls.ls_reply.dest_latitude = node->pos_vector.latitude;
        p->payload.itsnet_ls.ls.ls_reply.dest_longitude = node->pos_vector.longitude;

        if(itsnet_packet_send(p, brodcast_mac) == -1) {
            printf("erreur envoie packet \n");
            result = -1;
        }

        free(p);
       
    }
     return result;
}

void itsnet_location_handler(struct itsnet_packet* p)
{

    switch(LO_NIBBLE(p->common_header.itsnet_header_type_subtype)) {
    case ITSNET_LS_REQUEST:
        printf("value of packet : location sercive request \n");
        itsnet_location_reply(p);
        break;
    case ITSNET_LS_REPLY:
        printf("value of packet : location sercive reply \n");
        list_process_location_packet(&location_packet_list, p->payload.itsnet_ls.ls.ls_reply.dest_node_id,
            p->payload.itsnet_ls.ls.ls_reply.dest_latitude, p->payload.itsnet_ls.ls.ls_reply.dest_latitude);
        break;
    default:
        free(p);
        printf("Unknown packet type\n");
        break;
    }
}
