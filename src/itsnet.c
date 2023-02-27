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
 * @file itsnet.c
 * itsnet code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <asm/types.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "beaconing.h"
#include "dll_sap.h"
#include "itsnet.h"
#include "itsnet_listener.h"
#include "itsnet_parser.h"
#include "position_sensor.h"
#include "routing_manager.h"
#include "tqueue.h"
//#include "itsnet_ip.h"
#include "itsnet_pseudonym.h"
#include "location_table.h"
#include "position_sensor.h"

LIST_HEAD(neighbor_list);

/**
 * clean memory
 * @return an integer
 */

int itsnet_cleanup()
{
}

/**
 * main function
 * @param argc
 * @param argv
 */
int itsnet_main(int argc, char** argv)
{

    if(taskqueue_init() < 0) {
        printf("taskqueue_init failed\n");
        exit(1);
    }

    if(dll_sap_init(NULL) < 0) {
        printf("dll_sap__init failed\n");
        exit(1);
    }

    if(itsnet_listener_init() < 0) {

        printf("position_sensor_init failed\n");
        exit(1);
    }

    /*	if (itsnet_iplistener_init() < 0)
                {
                        printf("itsnet_iplistener_init failed\n");

                        exit(1);
                }
    */

    itsnet_init_conf();

    return 0;
}
/**
 *
 */

void itsnet_init_conf()
{
    set_ego_node_id(NodeId_parse());

    itsnet_beacon_send();
}
