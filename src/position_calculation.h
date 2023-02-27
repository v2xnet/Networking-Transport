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
 * @file position_calculation.h
 * position calculation code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _POSITION_CALCULATION_H_
#define _POSITION_CALCULATION_H_ 1

#include "itsnet_header.h"

int convert_format_lat(const double lat);

double inv_convert_format_lat(const int lat);

int convert_format_long(const double lon);

double inv_convert_format_long(const int lon);

int convert_time_stamp(const __time_t ts);

int convert_format_spd(const double spd);

double inv_convert_format_spd(const int spd);

double _deg2rad(double deg);

double haversine_distance(uint32_t lat11, uint32_t lon11, uint32_t lat22, uint32_t lon22);

int isrelevant(uint32_t lat1, uint32_t lon1, uint32_t lat2, uint32_t lon2, uint16_t radius);

#endif /* _POSITION_CALCULATION_H_ */
