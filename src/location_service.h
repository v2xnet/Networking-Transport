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
 * @file location_service.h
 * location service code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _LOCATION_SERVICE_H_
#define _LOCATION_SERVICE_H_ 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "itsnet_header.h"
#include "tqueue.h"

#define LOCATION_PACKET_LIST_SIZE 50
#define LOCATION_REQUEST_TIME 5
#define LOCATION_RETRY_TIMES 5

/**
 *list head
 */
extern struct list_head location_packet_list;

/**
 *The structure describes the  location packet
 */
struct itsnet_location_packet {

    struct itsnet_packet* p;
    int retry_times;
    struct tq_elem tqe;      /** Timer queue entry */
    struct timespec expires; /** expire time for location packet 	*/
    struct list_head list;   /** list head 			*/
};
typedef struct itsnet_location_packet itsnet_location_packet;

void list_process_location_packet(struct list_head* location_packet_list,
    itsnet_node_id id,
    uint32_t latitude,
    uint32_t longitude);

int itsnet_location_packet_list_add(struct list_head* location_packet_list, itsnet_location_packet* packet);

void itsnet_location_packet_list_free(struct list_head* location_packet_list);

void itsnet_location_packet_list_remove_packet(itsnet_location_packet* packet);

void location_packet_remove(struct tq_elem* tqe);

int itsnet_location_request(struct itsnet_packet* unicast_packet);

int itsnet_location_reply(struct itsnet_packet* p);

void itsnet_location_handler(struct itsnet_packet* p);

#endif /* _LOCATION_SERVICE_H_*/
