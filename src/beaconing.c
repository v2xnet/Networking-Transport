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
 * @file beaconing.c
 * beaconing code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "beaconing.h"
#include "dll_sap.h"
#include "itsnet_event_service.h"
#include "itsnet_parser.h"
#include "itsnet_security.h"
#include "location_table.h"
#include "position_sensor.h"
#include "tqueue.h"

#include "itsnet_pseudonym.h"

static struct tq_elem tqe;

/**
 * itsnet_beacon_send
 * @param itsnet_beacon_send packet
 */

void itsnet_beacon_send()
{
    struct itsnet_packet* p;
    uint8_t brodcast_mac[MAX_LLA_LEN];
    struct timespec exp_in;
    tssetmsec(exp_in, TIME_BEACON_INTERVAL);
    //tssetmsec(exp_in, itsGnBeaconServiceRetransmitTimer);
    struct itsnet_position_vector pos;
    pos = get_position_vector();
    
    brodcast_mac[0]=0xff;
    brodcast_mac[1]=0xff;
    brodcast_mac[2]=0xff;
    brodcast_mac[3]=0xff;
    brodcast_mac[4]=0xff;
    brodcast_mac[5]=0xff;
    //memcpy(brodcast_mac, "0xff:0xff:0xff:0xff:0xff:0xff", MAX_LLA_LEN);
    p = (itsnet_packet*)malloc(sizeof(itsnet_packet));
    memset(p, 0, sizeof(itsnet_packet));
    if(p == NULL) {
        printf("erreur allocation \n");
    } else  {
        
        p->basic_header.itsnet_version_next_header = HI_NIBBLE(1);
        p->basic_header.itsnet_version_next_header |= LO_NIBBLE(common_header);
        p->basic_header.itsnet_lifetime=lt_base_10s;
        p->basic_header.itsnet_rhl=1;
        

        p->common_header.itsnet_next_header =  HI_NIBBLE(ANY); /** 4bits common header next header */
        p->common_header.itsnet_next_header |=  LO_NIBBLE(0); /** 4bits reserved must be set to zero */
        p->common_header.itsnet_header_type_subtype =  HI_NIBBLE(itsnet_beacon_id);
        p->common_header.itsnet_header_type_subtype |= LO_NIBBLE(UNSPECIFIED);
            
        p->common_header.traffic_class = CLASS03;
        p->common_header.flags = 0;
        p->common_header.flags  =  Mobile << 7;
        p->common_header.payload_lenght = 0;
        p->common_header.max_hop_limit = 1;
        p->common_header.reserved=0;
        
        p->payload.itsnet_beacon.source_position_vector.node_id = *(get_ego_node_id());
        p->payload.itsnet_beacon.source_position_vector = pos;
        

        itsnet_packet_send(p, brodcast_mac);

        free(p);
    }

    itsnet_neighbor_list_print(&neighbor_list);
    INIT_LIST_HEAD(&(tqe.list));
    add_task_rel(&exp_in, &tqe, itsnet_beacon_send);
}

/**
 * handle the received packet
 * @param itsnet_packet packet
 * @brief When a node receives a beacon, it performs the following processing steps:
 *If security check fails, the packet will be dropped.
 *If the beacon comes from a node that is not included in the location table, a new entry will
 *be created for this node, and location information from the common C2C NET header will
 *be written into the entry.
 *If the beacon comes from a node that is already included in the location table, the location
 *information will be updated with information from the common C2C NET header.
 */
void itsnet_beacon_handler(struct itsnet_packet* p)
{
    itsnet_node node;
    itsnet_node* n;
    itsnet_event e;
    struct timespec exp_in;
    tssetsec(exp_in, LOCATION_TABLE_ENTRY_EXP);
    struct timespec expire;

    clock_gettime(CLOCK_REALTIME, &expire);
    tsadd(expire, exp_in, expire);

    if(check_security(p) == 1) {

        node.node_id = p->payload.itsnet_beacon.source_position_vector.node_id;
        node.expires = expire;

        if(list_find_node(&neighbor_list, &(p->payload.itsnet_beacon.source_position_vector.node_id)) == NULL) {

            e.type = itsnet_new_neighbor;
            e.event.new_neighbor = p->payload.itsnet_beacon.source_position_vector;
            itsnet_event_send(e);

            node.pos_vector = p->payload.itsnet_beacon.source_position_vector;
            itsnet_neighbor_list_add(&neighbor_list, &node);

        } else {

            n = list_find_node(&neighbor_list, &(p->payload.itsnet_beacon.source_position_vector.node_id));
            n->pos_vector = p->payload.itsnet_beacon.source_position_vector;
            n->expires = expire;
            itsnet_neighbor_list_update_node(n);
        }
    }
}
