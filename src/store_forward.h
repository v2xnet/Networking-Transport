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
 * @file store_forward.h
 * store && forward code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _STORE_FORWARD_H_
#define _STORE_FORWARD_H_ 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "geo_anycast.h"
#include "geo_broadcast.h"
#include "geo_topo.h"
#include "geo_unicast.h"
#include "itsnet_header.h"
#include "store_forward.h"

#include "itsnet_header.h"
#include "itsnet_types.h"
#include "list.h"
#include "tqueue.h"
#include "util.h"

#define MAX_BUFFERED_ENTRIES 50
#define QUERY_LOCATION_FREQ 5
#define THRESHOLD_ANGLE 90
#define EXPIRE_TIME 180000

/**
 *list head
 */
extern struct list_head store_forward_packet_list;

/**
 *The structure describes the  store_forward packet
 */
struct itsnet_store_forward_packet {
    struct itsnet_packet* p;
    int retry_times;

    struct tq_elem tqe;      /** Timer queue entry */
    struct timespec expires; /** expire time for store_forward packet 	*/
    struct list_head list;   /** list head 			*/
};
typedef struct itsnet_store_forward_packet itsnet_store_forward_packet;

/*
itsnet_store_forward_packet* list_find_stored_packet(
    struct list_head * store_forward_packet_list,
    itsnet_time_stamp ts,
    itsnet_node_id id);
*/

int itsnet_store_forward_packet_list_add(struct list_head* store_forward_packet_list,
    itsnet_store_forward_packet* packet);

/*
void itsnet_store_forward_packet_list_print_node(
    const itsnet_store_forward_packet * packet);

void itsnet_store_forward_packet_list_print(
    const struct list_head * store_forward_packet_list);

void itsnet_store_forward_packet_list_free(
    struct list_head * store_forward_packet_list);

void itsnet_store_forward_packet_list_remove_packet(
    itsnet_store_forward_packet * packet);

void itsnet_store_forward_packet_list_update(
    struct list_head * store_forward_packet_list);

*/

void store_forward_packet_remove(struct tq_elem* tqe);

/**
 * itsnet_forward
 * @param p itsnet_packet
 */
// void itsnet_forward(struct itsnet_packet *p);

void itsnet_forward(struct tq_elem* tqe);

/**
 * store the given packet
 * @param p itsnet_packet
 */
void itsnet_store(struct itsnet_packet* p, int retry_time);

void trace_forwarded_packet(struct timeval tv);

/*static inline uint32_t timeval_to_ms(const struct timeval* tv)
{
    return ((uint32_t)(tv->tv_sec * 1000) + (tv->tv_usec / 1000));
}*/
#endif /* _STORE_FORWARD_H_*/
