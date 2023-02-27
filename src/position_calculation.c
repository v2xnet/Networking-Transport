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
 * @file position_calculation.c
 * position calculation code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include "position_calculation.h"
#include <stdio.h>
#include <math.h>

#define PI 3.14159265358979323846
#define RADIUS 6371 // Radius of the earth

/**
 * convert latitude to standard
 * @param const double lat
 * @return int latitude
 */
int convert_format_lat(const double lat)
{
    return (((int)(lat * 1000000)) << 3);
}

/**
 * convert altitude to standard
 * @param const double alt
 * @return int altitude
 */
int convert_format_alt(const double alt)
{
    return (((int)(alt * 1000000)) << 3);
}

/**
 * inv_convert latitude
 * @param const int lat
 * @return double latitude
 */
double inv_convert_format_lat(const int lat)
{

    return ((double)(lat >> 3)) / 1000000;
}

/**
 * convert longitude to standard
 * @param const double lon
 * @return int longitude
 */
int convert_format_long(const double lon)
{
    return (((int)(lon * 1000000)) << 3);
}

/**
 * inv_convert longitude
 * @param const int lon
 * @return double longitude
 */
double inv_convert_format_long(const int lon)
{
    return ((double)(lon >> 3)) / 1000000;
}

/**
 * convert time stamp to standard
 * @param const double ts
 * @return int time stamp
 */
int convert_time_stamp(const __time_t ts)
{
    return (((int)ts) % 65536);
}

/**
 * convert speed to standard
 * @param const double spd
 * @return int speed
 */
int convert_format_spd(const double spd)
{
    return (int)(spd * 100);
}

/**
 * inv_convert speed to standard
 * @param const double spd
 * @return double speed
 */
double inv_convert_format_spd(const int spd)
{
    return (double)(spd / 100);
}

/**
 * convert deg to rad
 * @param double deg
 * @return double rad
 */
double _deg2rad(double deg)
{
    return (deg * (PI / 180.0));
}

/**
 * calculate haversine distance
 * @param double lat1
 * @param double lon1
 * @param double lat2
 * @param double lon2
 * @return double result
 */
double haversine_distance(uint32_t lat11, uint32_t lon11, uint32_t lat22, uint32_t lon22)
{
    double lat1, lon1, lat2, lon2;

    lat1 = inv_convert_format_lat(lat11);
    lon1 = inv_convert_format_long(lon11);
    lat2 = inv_convert_format_lat(lat22);
    lon2 = inv_convert_format_long(lon22);

    lon1 = _deg2rad(lon1);
    lat1 = _deg2rad(lat1);
    lon2 = _deg2rad(lon2);
    lat2 = _deg2rad(lat2);

    double dlon = lon2 - lon1;
    double dlat = lat2 - lat1;
    double a = pow(sin(dlat / 2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon / 2), 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return RADIUS * c;
}

/**
 * is_relevant
 * @param uint32_t lat1
 * @param uint32_t lon1
 * @param uint32_t lat2
 * @param uint32_t lon2
 * @param uint16_t radius
 * @return integer
 */
int isrelevant(uint32_t lat1, uint32_t lon1, uint32_t lat2, uint32_t lon2, uint16_t radius)
{

    double distance = haversine_distance(lat1, lon1, lat2, lon2);
    /*printf("\nLAT1:%d\tLON1:%d\tLAT2:%d\tLONG2:%d\n",lat1,lon1,lat2,lon2);
    printf("\nDISTANCE IN KM IS %f \tRADIUS iS:%d\n",distance,radius);*/
    if((distance * 1000) <= radius)
        return 1;
    else
        return 0;
}
