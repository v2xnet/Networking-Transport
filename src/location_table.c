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
 * @file location_table.c
 * location table code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include "location_table.h"
#include "itsnet_pseudonym.h"
#include "position_calculation.h"
#include <stdio.h>
#include <stdlib.h>

/**
 * list_find_node
 * @param struct list_head * neighbor_list
 * @param itsnet_node_id id
 * @return itsnet_node*
 */
itsnet_node* list_find_node(struct list_head* neighbor_list, struct itsnet_node_id* node_id)
{
    struct list_head* node_list_entry;
    list_for_each(node_list_entry, neighbor_list)
    {
        itsnet_node* i;
        i = list_entry(node_list_entry, struct itsnet_node, list);
        if(memcmp(node_id, &(i->node_id), sizeof(itsnet_node_id)) == 0)
            return i;
    }
    return NULL;
}

/**
 * itsnet_neighbor_number
 * @param struct list_head * neighbor_list
 * @return int
 */
int itsnet_neighbor_number(struct list_head* neighbor_list)
{
    struct list_head* node_list_entry;
    int NodeNbre = 0;
    list_for_each(node_list_entry, neighbor_list)
    {
        itsnet_node* i;
        i = list_entry(node_list_entry, struct itsnet_node, list);

        NodeNbre += 1;
    }
    return NodeNbre;
}

int itsnet_neighbor_list_is_empty(struct list_head* neighbor_list)
{

    struct list_head* node_list_entry;
    int NodeNbre = 0;
    list_for_each(node_list_entry, neighbor_list)
    {
        itsnet_node* i;
        i = list_entry(node_list_entry, struct itsnet_node, list);

        NodeNbre += 1;
        if(NodeNbre >= 1)
            return 1;
    }
    return 0;
}

/**
 * add the node to the list
 * @param neighbor_list nodes list
 * @param node node to add
 * @return an integer
 */

int itsnet_neighbor_list_add(struct list_head* neighbor_list, itsnet_node* node)
{
    itsnet_node* new_node;
    struct timespec exp_in;

    tssetmsec(exp_in, 5000); /** TODO */

    if((new_node = malloc(sizeof(*node))) == NULL) {
        return -1;
    }
    memcpy(new_node, node, sizeof(*node));

    new_node->expires = exp_in;
    INIT_LIST_HEAD(&(new_node->tqe.list));

    if(memcmp(&(new_node->pos_vector.node_id), get_ego_node_id(), sizeof(itsnet_node_id)) != 0) {

        add_task_rel(&exp_in, &(new_node->tqe), itsnet_neighbor_list_remove_node);
    }

    list_add_tail(&new_node->list, neighbor_list);

    return 0;
}

/**
 * print the given node
 * @param itsnet_node node to print
 */

void itsnet_neighbor_list_print_node(const itsnet_node* node)
{
    printf("node id : ");
    print_node_id(node->node_id);
    printf("expires in : %lu.%09ld ", node->expires.tv_sec, node->expires.tv_nsec);
    printf("time_stamp : %d ", node->pos_vector.time_stamp);
    printf("latitude   : %d ", node->pos_vector.latitude);
    printf("longitude  : %d ", node->pos_vector.longitude);
    printf("speed      : %d ", node->pos_vector.speed);
}

/**
 * print the given list of nodes
 * @param neighbor_list nodes list to print
 */

void itsnet_neighbor_list_print(const struct list_head* neighbor_list)
{
    struct list_head* node_list_entry;
    list_for_each(node_list_entry, neighbor_list)
    {
        itsnet_node* i;
        i = list_entry(node_list_entry, struct itsnet_node, list);
        //itsnet_neighbor_list_print_node(i);
    }
}

/**
 * free the given list of nodes
 * @param neighbor_list nodes list to free
 */

void itsnet_neighbor_list_free(struct list_head* neighbor_list)
{
    struct list_head *node_list_entry, *i;
    list_for_each_safe(node_list_entry, i, neighbor_list)
    {
        itsnet_node* node;
        node = list_entry(node_list_entry, struct itsnet_node, list);
        // itsnet_neighbor_list_remove_node(node);
    }
}

/**
 * remove the given node from list
 * @param itsnet_node node to remove
 */
void itsnet_neighbor_list_remove_node(struct tq_elem* tqe)
{

    itsnet_node* node;
    node = tq_data(tqe, itsnet_node, tqe);
    list_del(&node->list);
    free(node);
}

/**
 * update the given node
 * @param itsnet_node node to remove
 */

void itsnet_neighbor_list_update_node(struct itsnet_node* node)
{

    struct timespec exp_in;
    // struct timespec expire;
    tssetmsec(exp_in, 5000);
    // itsnet_node * node;
    // node=  tq_data(tqe,  itsnet_node, tqe);
    //printf("update node \n");
    del_task(&node->tqe);
    //	INIT_LIST_HEAD(&(node->tqe.list));

    add_task_rel(&exp_in, &(node->tqe), itsnet_neighbor_list_remove_node);
}

itsnet_node_id itsnet_select_forwarder(struct list_head* neighbor_list,
    struct itsnet_position_vector pos,
    uint32_t lat_dest,
    uint32_t long_dest)
{

    double dist1;
    double distmp = 5000;
    struct list_head* node_list_entry;
    struct itsnet_node_id node_id;

    list_for_each(node_list_entry, neighbor_list)
    {
        itsnet_node* i;
        i = list_entry(node_list_entry, struct itsnet_node, list);

        dist1 = haversine_distance(i->pos_vector.latitude, i->pos_vector.longitude, lat_dest, long_dest);

        if(dist1 < distmp) {
            node_id = i->node_id;
            distmp = dist1;
        }
    }

    printf("forwarder id : ");
    print_node_id(node_id);
    return node_id;
}

void print_node_id(struct itsnet_node_id node_id)
{
    printf(":%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x \n", (node_id.id[0] & 0xFF), (node_id.id[1] & 0xFF),
        (node_id.id[2] & 0xFF), (node_id.id[3] & 0xFF), (node_id.id[4] & 0xFF), (node_id.id[5] & 0xFF),
        (node_id.id[6] & 0xFF), (node_id.id[7] & 0xFF));
}
