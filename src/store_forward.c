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
 * @file store_forward.c
 * store forward code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include "store_forward.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <unistd.h>
#include <sys/time.h>
#include <asm/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "geo_anycast.h"
#include "geo_broadcast.h"
#include "geo_topo.h"
#include "geo_unicast.h"

#include "location_table.h"
#include "routing_manager.h"


/*
itsnet_store_forward_packet* list_find_stored_packet(
    struct list_head * store_forward_packet_list,
    itsnet_time_stamp ts,
    itsnet_node_id id)
{
        struct list_head* packet_list_entry;
        list_for_each(packet_list_entry, store_forward_packet_list) {
                itsnet_store_forward_packet* i;
                i = list_entry(packet_list_entry, struct itsnet_store_forward_packet, list);
                if ((memcmp( (const void*)(id), (const void*)(&(i->source_node_id)),NODE_ID_LEN) ==
0)&&(ts==i->time_stamp)) return i;
        }
        return NULL;

}
*/

int itsnet_store_forward_packet_list_add(struct list_head* store_forward_packet_list,
    itsnet_store_forward_packet* packet)
{
    /*	itsnet_store_forward_packet * new_packet;
        struct timespec exp_in;
        tssetmsec(exp_in,EXPIRE_TIME);


        if ( (new_packet = malloc(sizeof(*packet))) == NULL)
                {
                        return -1;
                }
        memcpy(new_packet, packet, sizeof(*packet));
        new_packet->expires=exp_in;
    */

    INIT_LIST_HEAD(&(packet->tqe.list));

    add_task_rel(&(packet->expires), &(packet->tqe), itsnet_forward);

    list_add_tail(&packet->list, store_forward_packet_list);

    printf("store packet succeed \n");
    return 0;
}

/*
void itsnet_store_forward_packet_list_print_node(
    const itsnet_store_forward_packet * packet)
{
        printf("source node id       : %d \n",packet->source_node_id);
        printf("time_stamp 	     : %d \n",packet->time_stamp);
}

void itsnet_store_forward_packet_list_print(
    const struct list_head * store_forward_packet_list)
{
        struct list_head* packet_list_entry;
        list_for_each(packet_list_entry, store_forward_packet_list) {
                itsnet_store_forward_packet * i;
                i = list_entry(packet_list_entry, struct itsnet_store_forward_packet, list);
                itsnet_store_forward_packet_list_print_node(i);

        }
}



*/

/*
void store_forward_packet_remove(
    struct tq_elem *tqe)
{

        itsnet_store_forward_packet * packet;
        packet=  tq_data(tqe,  itsnet_store_forward_packet, tqe);
        list_del(&packet->list);
        free(packet);

}
*/

void itsnet_forward(struct tq_elem* tqe)
{
    
    itsnet_store_forward_packet* packet;

    struct timespec exp_in;
    static struct timeval t_emit;

    tssetsec(exp_in, 5);

    struct timespec expire;

    clock_gettime(CLOCK_REALTIME, &expire);

    tsadd(expire, exp_in, expire);

    packet = tq_data(tqe, itsnet_store_forward_packet, tqe);

    if(itsnet_neighbor_list_is_empty(&neighbor_list) == 0) {

        if((packet->retry_times) < 5) {
            (packet->retry_times)++;
            add_task_rel(&exp_in, &(packet->tqe), itsnet_forward);
        } else {
            list_del(&packet->list);
            free(packet->p);
            free(packet);
        }

    }

    else

    {

        switch(HI_NIBBLE(packet->p->common_header.itsnet_header_type_subtype)) {
        case itsnet_unicast_id:
            // printf("value of packet : location sercive request \n");
            itsnet_geounicast_send(packet->p);
            break;
        case itsnet_geoanycast_id:
            // printf("value of packet : location sercive reply \n");
            itsnet_geoanycast_send(packet->p);
            break;
        case itsnet_geobroadcast_id:
            // printf("value of packet : location sercive request \n");
            gettimeofday(&t_emit,NULL);
            itsnet_geobroadcast_send(packet->p);
            trace_forwarded_packet(t_emit);
            break;
        case itsnet_tsb_id:
            // printf("value of packet : location sercive reply \n");
            break;

        default:
            list_del(&packet->list);
            free(packet->p);
            free(packet);
            printf("Unknown packet type\n");
            break;
        }

        list_del(&packet->list);
        free(packet);
    }

    // list_del(&packet->list);
    //				free(packet);
    /*todo*/
}

void itsnet_store(struct itsnet_packet* p, int retry_time)
{

    itsnet_store_forward_packet* new_packet;

    struct timespec exp_in;

    tssetsec(exp_in, 5);

    struct timespec expire;

    clock_gettime(CLOCK_REALTIME, &expire);

    tsadd(expire, exp_in, expire);

    new_packet = (itsnet_store_forward_packet*)malloc(sizeof(itsnet_store_forward_packet));
    new_packet->p = (itsnet_packet*)malloc(sizeof(itsnet_packet));

    memcpy(new_packet->p, p, sizeof(struct itsnet_packet));
    new_packet->retry_times = retry_time;
    new_packet->expires = exp_in;

    itsnet_store_forward_packet_list_add(&store_forward_packet_list, new_packet);

    printf("  \n\nstore and forward packet\n\n");

    /*todo*/
}

void trace_forwarded_packet(struct timeval t_emit)
{

    static FILE* f_emit;
   
    f_emit = fopen("/root/emit_fwd_dll.log", "a");
    
    fprintf(f_emit, "TIME %lld\n",timeval_to_us(&t_emit));
    fclose(f_emit);
}