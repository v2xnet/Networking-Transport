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
 * @file itsnet_parser.h
 * itsnet conf code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#ifndef _ITSNET_PARSER_H_
#define _ITSNET_PARSER_H_ 1

#include "itsnet_header.h"
#include <stdint.h>
#include <stdbool.h>

int itsnet_cfg_parse(char* file);

void itsnet_cfg_clean();

char* Device_parse(void);

int ItsnetDataSize_parse(void);

mac_addr* BroadcastMac_parse(void);

int MaxNeighbor_parse(void);

int LocationTableEntry_parse(void);

char* EthPItsnet_parse(void);

int SendBeacon_parse(void);

int DebugLevel_parse(void);

int DetachFromTTY_parse(void);

itsnet_node_id* itsnet_aton(const char* asc);

mac_addr* itsnet_aton_ether(const char* asc);

itsnet_node_id* NodeId_parse(void);

bool Extended_parse(void);

int CovCravPort_parse(void);

bool ACK_parse(void);

bool OCB_parse(void);


#endif /* _ITSNET_PARSER_H_  */
