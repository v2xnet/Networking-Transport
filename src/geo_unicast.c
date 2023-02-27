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
 * @file geo_unicast.c
 * geo unicast code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include "geo_unicast.h"
#include "dll_sap.h"
#include "itsnet_listener.h"
#include "itsnet_pseudonym.h"
#include "itsnet_security.h"
#include "position_sensor.h"
#include <stdio.h>
#include <string.h>

#include "itsnet_parser.h"
#include "location_service.h"
#include "location_table.h"
#include "store_forward.h"

#include "routing_manager.h"
//#include "itsnet_ip.h"

LIST_HEAD(location_packet_list);
LIST_HEAD(store_forward_packet_list);

// struct list_head* node_list_entry;

/**
 * itsnet_geounicast_send
 * @param p itsnet_packet
 */
int itsnet_geounicast_send(struct itsnet_packet* p)
{

    int result = 0;

    uint8_t brodcast_mac[MAX_LLA_LEN];
    memcpy((uint8_t*)brodcast_mac, (uint8_t*)BroadcastMac_parse()->address, MAX_LLA_LEN);

    itsnet_neighbor_list_print(&neighbor_list);

    itsnet_processed_packet_list_add(&processed_packet_list,
        p->payload.itsnet_unicast.source_position_vector.time_stamp,
        p->payload.itsnet_unicast.source_position_vector.node_id);

    itsnet_processed_packet_list_print(&processed_packet_list);

    if(list_find_node(&neighbor_list, &(p->payload.itsnet_unicast.dest_node_id)) != NULL) {

        printf("Node destination is a neighbor \n");

        if(itsnet_packet_send(p, brodcast_mac) == -1) {
            printf("erreur envoie packet \n");
            result = -1;
        }

        free(p);
        return result;
        /*else, if the position of D is already provided by the upper layer or known from the
        location management table, then Dest_GeoLocation = geographical location of D
        */
    } else if(p->payload.itsnet_unicast.dest_latitude == 0 && p->payload.itsnet_unicast.dest_longitude == 0) {

        printf("Starting Location Service psocess \n");
        itsnet_location_request(p); /*todo*/
        return 0;

    } else {
        printf("The destination geo area is provided by the upper layer \n");
    }

    printf("The destination Latitude & longitude is done \n");
    printf("destination latitude %d\n destination longitude %d\n\n", p->payload.itsnet_unicast.dest_latitude,
        p->payload.itsnet_unicast.dest_longitude);
    printf("starting forwarding process \n");

    if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0) {
        printf("there are neighbours available around : forwarding \n");

        p->payload.itsnet_unicast.forwarder_position_vector.node_id =
            itsnet_select_forwarder(&neighbor_list, p->payload.itsnet_unicast.source_position_vector,
                p->payload.itsnet_unicast.dest_latitude, p->payload.itsnet_unicast.dest_longitude);
        if(itsnet_packet_send(p, brodcast_mac) == -1) {
            printf("erreur envoie packet \n");
            result = -1;
        }
        free(p);
        return result;

    } else {
        /*put P in the store and forward buffer*/
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
int itsnet_geounicast_handler(struct itsnet_packet* p)
{

    uint8_t brodcast_mac[MAX_LLA_LEN];
    struct itsnet_position_vector pos;
    int result = 0;
    itsnet_node node;
    itsnet_node* n;
    itsnet_event e;
    struct timespec exp_in;
    tssetsec(exp_in, LOCATION_TABLE_ENTRY_EXP);
    struct timespec expire;

    clock_gettime(CLOCK_REALTIME, &expire);
    tsadd(expire, exp_in, expire);

    pos = get_position_vector();
    memcpy((uint8_t*)brodcast_mac, (uint8_t*)BroadcastMac_parse(), MAX_LLA_LEN);

    if(check_security(p) == 1) {

        printf("security check succed \n");

        if(list_find_packet(&processed_packet_list, p->payload.itsnet_unicast.source_position_vector.time_stamp,
               p->payload.itsnet_unicast.source_position_vector.node_id) != NULL) {

            free(p);
            return 0;
        }

        printf("packed not already processed  \n");

        itsnet_processed_packet_list_add(&processed_packet_list,
            p->payload.itsnet_unicast.source_position_vector.time_stamp,
            p->payload.itsnet_unicast.source_position_vector.node_id);

        /* update/add S & F*/

        /*TODO Source & Forwarder*/

        /*if(!memcmp(get_ego_node_id(),&(p->payload.itsnet_unicast.dest_node_id),sizeof(itsnet_node_id)))

                {*/
        /* Select the destination of received packet : IP or Transport*/
        printf("The packet is destinated to me: nodeId: \n");
        print_node_id(p->payload.itsnet_unicast.dest_node_id);
        printf("The next header is : %d\n", HI_NIBBLE(p->common_header.itsnet_next_header));
        if(HI_NIBBLE(p->common_header.itsnet_next_header) == BTP_B) {
            /*Send packet to upper layer */
            printf("reception by upper layer result \n");
            result = itsnet_unicast_indication_send(p);
        }
        if(HI_NIBBLE(p->common_header.itsnet_next_header) == IPV6) {
            printf("IP PACKET IN C2C PACKET is RECEIVED \n");
            free(p);
            //	itsnet_ip_receive(p);
        }
        return (result);

        //}

        printf("******The packet is not destinated to me \n");

        if(list_find_node(&neighbor_list, &(p->payload.itsnet_unicast.dest_node_id)) != NULL) {

            /*Todo forwarding & update last forwarder J in common header*/

            p->payload.itsnet_unicast.forwarder_position_vector = pos;

            if(itsnet_packet_send(p, brodcast_mac) == -1) {
                printf("erreur envoie packet \n");
                result = -1;
            }
            free(p);
            return result;
        }
        if(itsnet_neighbor_list_is_empty(&neighbor_list) != 0) {

            if(memcmp(
                   &(p->payload.itsnet_unicast.forwarder_position_vector.node_id), get_ego_node_id(), sizeof(itsnet_node_id))) {
                print_node_id(*(get_ego_node_id()));
                printf("I'm not the selected forwarder,the forwarder is : \n");
                print_node_id(p->payload.itsnet_unicast.forwarder_position_vector.node_id);
                free(p);
                return 0;
            }

            printf("I'm the selected forwarder \n");
            p->payload.itsnet_unicast.forwarder_position_vector = pos;
            p->payload.itsnet_unicast.forwarder_position_vector.node_id = itsnet_select_forwarder(
                &neighbor_list, pos, p->payload.itsnet_unicast.dest_latitude, p->payload.itsnet_unicast.dest_longitude);
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
    } else {

        free(p);
        return 0;
    }
}
