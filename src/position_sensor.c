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
 * @file position_sensor.c
 * position sensor code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "itsnet_pseudonym.h"
#include "position_calculation.h"
#include "position_sensor.h"

static struct itsnet_position_vector ego_position_vector;

/**
 * get_position_vector
 * @param struct itsnet_position_information p
 * @return ego_position_vector
 */
itsnet_position_vector get_position_vector()
{
    return ego_position_vector;
}

/**
 * position_data_update
 * @param struct itsnet_position_information p
 * @return void
 */

void position_data_update(struct itsnet_position_information p)
{

    // memcpy(&(ego_position_vector.id),get_ego_node_id(),8);
    ego_position_vector.node_id = *(get_ego_node_id());

    /*ego_position_vector.accuracy.alt_ac = 0;
    ego_position_vector.accuracy.head_ac = 0;
    ego_position_vector.accuracy.pos_ac = 0;
    ego_position_vector.accuracy.speed_ac = 0;
    ego_position_vector.accuracy.time_ac = 0;*/
    ego_position_vector.time_stamp = p.time_stamp; // convert_time_stamp(GPS->fix.time);
    ego_position_vector.latitude = p.latitude;     // convert_format_lat(GPS->fix.latitude);
    ego_position_vector.longitude = p.longitude;   // convert_format_long(GPS->fix.longitude);
    ego_position_vector.speed = p.speed;           // convert_format_spd(GPS->fix.speed);
    ego_position_vector.heading = 0;
    /*ego_position_vector.altitude = p.altitude; */// convert_format_alt(GPS->fix.altitude);
}
