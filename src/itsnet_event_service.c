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
 * @file itsnet_event_service.c
 * itsnet event service code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include "itsnet_event_service.h"
#include <stdlib.h>

/**
 * itsnet_event_send
 * @param itsnet_event
 */

void itsnet_event_send(itsnet_event e)
{
    struct message* m;
    m = (message*)malloc(sizeof(message));
    m->msg_hdr.sid = itsnet_management_sap;
    m->msg_hdr.opcode = itsnet_indication;
    m->msg_hdr.aid = itsnet_events;
    m->payload.itsnet_event_ind.event = e;
    itsnet_event_indication_send(m);
}
