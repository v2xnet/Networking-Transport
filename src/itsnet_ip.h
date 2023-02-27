/***************************************************************************
 *   ITSNET  Intelligent Transport System networking Stack                 *
 * 									   *
 ** Copyright(C)2010 ESPRIT											   *
 * 	        "Ãcole supÃ©rieure privÃ©e d'ingÃ©nierie et de technologie"       *
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
 * @file itsnet_ip.h
 * ip tunnel code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 * @author Tsukada Manabu
 * @author Thouraya
 */

#ifndef _ITSNET_IP_H_
#define _ITSNET_IP_H_ 1

int itsnet_ip_geobroadcast_tnl(short* buf, int l, uint32_t latitude, uint32_t longitude, double geo_area_size);

int itsnet_ip_geounicast_tnl(short* buf, int l, uint32_t latitude, uint32_t longitude, itsnet_node_id node_id);

int itsnet_ip_geoanycast_tnl(short* buf, int l, uint32_t latitude, uint32_t longitude, double geo_area_size);

void itsnet_ip_receive(struct itsnet_packet* p);

#endif /* _ITSNET_IP_H_*/
