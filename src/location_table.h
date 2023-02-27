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
 * @file location_table.h
 * location table code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _LOCATION_TABLE_H_
#define _LOCATION_TABLE_H_ 1

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <time.h>

#include "itsnet_header.h"
#include "list.h"
#include "tqueue.h"
#include "util.h"

#define MAX_NEIGHBOR 100
#define LOCATION_TABLE_ENTRY_EXP 5

/**
 *list head
 */
extern struct list_head neighbor_list;

/**
 *The structure describes the  neighbor
 */
struct itsnet_node {
    struct itsnet_node_id node_id;            /** node identity		*/
    struct tq_elem tqe;                       /** Timer queue entry */
    struct timespec expires;                  /** expire time for message 	*/
    struct itsnet_position_vector pos_vector; /** position vector		*/
    struct list_head list;                    /** list head 			*/
};
typedef struct itsnet_node itsnet_node;

itsnet_node* list_find_node(struct list_head* neighbor_list, struct itsnet_node_id* node_id);

int itsnet_neighbor_number(struct list_head* neighbor_list);

int itsnet_neighbor_list_add(struct list_head* neighbor_list, itsnet_node* node);

void itsnet_neighbor_list_print_node(const itsnet_node* node);

void itsnet_neighbor_list_print(const struct list_head* neighbor_list);

void itsnet_neighbor_list_free(struct list_head* neighbor_list);

void itsnet_neighbor_list_remove_node(struct tq_elem* tqe);

void itsnet_neighbor_list_update_node(struct itsnet_node* node);

itsnet_node_id itsnet_select_forwarder(struct list_head* neighbor_list,
    struct itsnet_position_vector pos,
    uint32_t lat_dest,
    uint32_t long_dest);

int itsnet_neighbor_list_is_empty(struct list_head* neighbor_list);

void print_node_id(struct itsnet_node_id node_id);

#endif /* _LOCATION_TABLE_H_*/
