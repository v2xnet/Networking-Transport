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
 * @file routing_manager.h
 * routing manager code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _ROUTING_MANAGER_H_
#define _ROUTING_MANAGER_H_ 1

#include "geo_anycast.h"
#include "geo_broadcast.h"
#include "geo_topo.h"
#include "geo_unicast.h"
#include "store_forward.h"

#include "itsnet_header.h"
#include "list.h"
#include "tqueue.h"
#include "util.h"

/**
 *list head
 */
extern struct list_head processed_packet_list;

/**
 *The structure describes the  processed packet
 */
struct itsnet_processed_packet {
    struct itsnet_node_id source_node_id; /** node identity		*/
    itsnet_time_stamp time_stamp;         /** time_stamp		*/
    struct tq_elem tqe;                   /** Timer queue entry */
    struct timespec expires;              /** expire time for processed packet 	*/
    struct list_head list;                /** list head 			*/
};
typedef struct itsnet_processed_packet itsnet_processed_packet;

itsnet_processed_packet*
list_find_packet(struct list_head* processed_packet_list, itsnet_time_stamp ts, struct itsnet_node_id node_id);

int itsnet_processed_packet_list_add(struct list_head* processed_packet_list,
    itsnet_time_stamp ts,
    struct itsnet_node_id node_id);

void itsnet_processed_packet_list_print_node(const itsnet_processed_packet* packet);

void itsnet_processed_packet_list_print(const struct list_head* processed_packet_list);

void itsnet_processed_packet_list_free(struct list_head* processed_packet_list);

void itsnet_processed_packet_list_remove_packet(itsnet_processed_packet* packet);

void itsnet_processed_packet_list_update(struct list_head* processed_packet_list);

void processed_packet_remove(struct tq_elem* tqe);

#endif /* _ROUTING_MANAGER_H_*/
