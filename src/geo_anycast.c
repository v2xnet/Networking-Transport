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
 * @file geo_anycast.c
 * geo anycast code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include "geo_anycast.h"
#include "dll_sap.h"
#include "itsnet_ip.h"
#include "itsnet_listener.h"
#include "itsnet_parser.h"
#include "itsnet_pseudonym.h"
#include "itsnet_security.h"
#include "location_service.h"
#include "location_table.h"
#include "position_calculation.h"
#include "position_sensor.h"
#include "routing_manager.h"
#include "store_forward.h"
#include <stdio.h>

LIST_HEAD(processed_packet_list);

// struct list_head* node_list_entry;

/**
 * itsnet_geoanycast_send
 * @param p itsnet_packet
 */

int itsnet_geoanycast_send(struct itsnet_packet* p)
{

    int result = 0;
    uint8_t brodcast_mac[MAX_LLA_LEN];
    memcpy((uint8_t*)brodcast_mac, (uint8_t*)BroadcastMac_parse()->address, MAX_LLA_LEN);

    itsnet_processed_packet_list_add(&processed_packet_list,
        p->payload.itsnet_geoanycast.source_position_vector.time_stamp,
        p->payload.itsnet_geoanycast.source_position_vector.node_id);

    itsnet_processed_packet_list_print(&processed_packet_list);

    if(isrelevant(p->payload.itsnet_geoanycast.source_position_vector.latitude,
           p->payload.itsnet_geoanycast.source_position_vector.longitude, p->payload.itsnet_geoanycast.dest_latitude,
           p->payload.itsnet_geoanycast.dest_longitude, p->payload.itsnet_geoanycast.geo_area_size)) {
        printf("belong to geo area  \n");
        free(p);
        return 0;
    } else if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0) {

        printf("The Packet should be forwarded to the destination \n");
        p->payload.itsnet_geoanycast.forwarder_position_vector.node_id=
            itsnet_select_forwarder(&neighbor_list, p->payload.itsnet_geoanycast.forwarder_position_vector,
                p->payload.itsnet_geoanycast.dest_latitude, p->payload.itsnet_geoanycast.dest_longitude);
        if(itsnet_packet_send(p, brodcast_mac) == -1) {
            printf("erreur envoie packet \n");
            result = -1;
        }
        free(p);
        return result;

    } else {
        printf("no neighbor put in store and forward \n");
        itsnet_store(p, 0);
    }

    free(p);
    return result;
}

/**
 * handle the given packet
 * @param p itsnet_packet
 */
int itsnet_geoanycast_handler(struct itsnet_packet* p)
{

    uint8_t brodcast_mac[MAX_LLA_LEN];
    struct itsnet_position_vector pos;
    int result;
    itsnet_node node;
    itsnet_node* n;
    itsnet_event e;

    pos = get_position_vector();
    memcpy((uint8_t*)brodcast_mac, (uint8_t*)BroadcastMac_parse()->address, MAX_LLA_LEN);
    printf("NodeId of the forwarder \n");
    print_node_id(p->payload.itsnet_geoanycast.forwarder_position_vector.node_id);

    if(check_security(p) == 1) {
        printf("security check succed \n");

        if(list_find_packet(&processed_packet_list, p->payload.itsnet_geoanycast.source_position_vector.time_stamp,
               p->payload.itsnet_geoanycast.source_position_vector.node_id) != NULL) {
            printf("packed already processed drop it \n");
            free(p);
            return 0;
        }

        printf("packed not already processed  \n");

        itsnet_processed_packet_list_add(&processed_packet_list,
            p->payload.itsnet_geoanycast.source_position_vector.time_stamp,
            p->payload.itsnet_geoanycast.source_position_vector.node_id);

        /* update/add S & F*/

        /*TODO Source & Forwarder*/

        if(!isrelevant(pos.latitude, pos.longitude, p->payload.itsnet_geoanycast.dest_latitude,
               p->payload.itsnet_geoanycast.dest_longitude, p->payload.itsnet_geoanycast.geo_area_size) &&
            isrelevant(p->payload.itsnet_geoanycast.forwarder_position_vector.latitude,
                p->payload.itsnet_geoanycast.forwarder_position_vector.longitude, p->payload.itsnet_geoanycast.dest_latitude,
                p->payload.itsnet_geoanycast.dest_longitude, p->payload.itsnet_geoanycast.geo_area_size)) {

            printf("J does not belong to the geo-area & forwarder is belong to the geo-area\n");
            free(p);
            return 0;
        }

        printf("J  belong to the geo-area \n");

        if(isrelevant(pos.latitude, pos.longitude, p->payload.itsnet_geoanycast.dest_latitude,
               p->payload.itsnet_geoanycast.dest_longitude, p->payload.itsnet_geoanycast.geo_area_size)) {
            /* Select the destination of received packet : IP or Transport*/
            printf("The packet is destinated to this geo-area \n");
            printf("JUST SEND PACKET TO UPPER LAYER or TO THE IP HANDLER");
            printf("The next header is : %d\n", p->common_header.itsnet_next_header);
            if(HI_NIBBLE(p->common_header.itsnet_next_header) == BTP_B) {
                /*Send packet to upper layer */
                result = itsnet_geoanycast_indication_send(p);
            }

            if(HI_NIBBLE(p->common_header.itsnet_next_header) == IPV6) {
                /*Send packet to IP packet handler */
                free(p);
                printf("IP PACKET IN C2C PACKET is RECEIVED \n");
                // itsnet_ip_receive(p);
            }

            free(p);
            return result;
        }

        printf("Start forwarding Parts \n");

        if(!isrelevant(p->payload.itsnet_geoanycast.forwarder_position_vector.latitude,
               p->payload.itsnet_geoanycast.forwarder_position_vector.longitude, p->payload.itsnet_geoanycast.dest_latitude,
               p->payload.itsnet_geoanycast.dest_longitude, p->payload.itsnet_geoanycast.geo_area_size))

        {

            if(memcmp(&(p->payload.itsnet_geoanycast.forwarder_position_vector.node_id), get_ego_node_id(), NODE_ID_LEN)) {
                print_node_id(p->payload.itsnet_geoanycast.forwarder_position_vector.node_id);
                printf("I'm not the selected forwarder \n");
                return 0;
            }
            printf("The packet should  be forwarded towards D\n");

            if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0) {
                p->payload.itsnet_geoanycast.forwarder_position_vector = pos;
                p->payload.itsnet_geoanycast.forwarder_position_vector.node_id = itsnet_select_forwarder(&neighbor_list, pos,
                    p->payload.itsnet_geoanycast.dest_latitude, p->payload.itsnet_geoanycast.dest_longitude);
                if(itsnet_packet_send(p, brodcast_mac) == -1) {
                    printf("erreur envoie packet \n");
                    result = -1;
                }

                free(p);
                return result;

            } else {
                /*put P in the store and forward buffer*/
                printf("put P in the store and forward buffer\n");
                itsnet_store(p, 0);
            }
        }
    } else {
        free(p);
    }
    
    return 0;
}
