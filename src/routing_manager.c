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
 * @file routing_manager.c
 * routing manager code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <unistd.h>

#include <asm/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "location_table.h"
#include "routing_manager.h"

itsnet_processed_packet*
list_find_packet(struct list_head* processed_packet_list, itsnet_time_stamp ts, struct itsnet_node_id node_id)
{
    struct list_head* packet_list_entry;
    list_for_each(packet_list_entry, processed_packet_list)
    {
        itsnet_processed_packet* i;
        i = list_entry(packet_list_entry, struct itsnet_processed_packet, list);
        if((memcmp(&node_id, &(i->source_node_id), sizeof(itsnet_node_id)) == 0) && (ts == i->time_stamp)) {
         printf("\n already processed packet \n");
            return i;
        }
    }
    return NULL;
}

int itsnet_processed_packet_list_add(struct list_head* processed_packet_list,
    itsnet_time_stamp ts,
    struct itsnet_node_id node_id)
{

    itsnet_processed_packet* new_packet;
    struct timespec exp_in;
    tssetmsec(exp_in, 100); /** TODO */

    if((new_packet = malloc(sizeof(struct itsnet_processed_packet))) == NULL) {
        return -1;
    }
    //	memcpy(new_packet, packet, sizeof(*packet));
    new_packet->time_stamp = ts;
    new_packet->source_node_id = node_id;
    new_packet->expires = exp_in;
    INIT_LIST_HEAD(&(new_packet->tqe.list));

    /*printf("\n processed timestamp %d \n   node id", new_packet->time_stamp);
    print_node_id(new_packet->source_node_id);
    printf("\n\n\npacket added to processed list\n\n\n");*/
    add_task_rel(&exp_in, &(new_packet->tqe), processed_packet_remove);

    list_add_tail(&new_packet->list, processed_packet_list);
    return 0;
}

void itsnet_processed_packet_list_print_node(const itsnet_processed_packet* packet)
{
    printf("source node id  : ");
    print_node_id(packet->source_node_id);
    printf("time_stamp 	     : %d \n", packet->time_stamp);
}

void itsnet_processed_packet_list_print(const struct list_head* processed_packet_list)
{
    struct list_head* packet_list_entry;
    list_for_each(packet_list_entry, processed_packet_list)
    {
        itsnet_processed_packet* i;
        i = list_entry(packet_list_entry, struct itsnet_processed_packet, list);
        //itsnet_processed_packet_list_print_node(i);
    }
}

void processed_packet_remove(struct tq_elem* tqe)
{

    itsnet_processed_packet* packet;
    packet = tq_data(tqe, itsnet_processed_packet, tqe);
    list_del(&packet->list);
    free(packet);
}
