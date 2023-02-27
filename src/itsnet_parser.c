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
 * @file itsnet_parser.c
 * itsnet conf code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include <confuse.h>
#include <ctype.h>
#include <net/ethernet.h>
#include <stdio.h>
#include <stdlib.h>
#include "itsnet_parser.h"

static cfg_opt_t opts[] = {
    CFG_STR("Device", NULL, CFGF_NONE),
    CFG_INT("ItsnetDataSize", 1500, CFGF_NONE),
    CFG_STR("BroadcastMac", "ff:ff:ff:ff:ff:ff", CFGF_NONE),
    CFG_INT("MaxNeighbor", 10, CFGF_NONE),
    CFG_INT("LocationTableEntry", 5, CFGF_NONE),
    CFG_STR("EthPItsnet", "0x707", CFGF_NONE),
    CFG_BOOL("SendBeacon", cfg_false, CFGF_NONE),
    CFG_INT("DebugLevel", 3, CFGF_NONE),
    CFG_BOOL("DetachFromTTY", cfg_false, CFGF_NONE),
    CFG_STR("NodeId","aa:aa:bb:bb:cc:cc:00:11", CFGF_NONE),
    CFG_BOOL("Extended", cfg_false, CFGF_NONE),
    CFG_INT("CovCravPort", 7777, CFGF_NONE),
    CFG_BOOL("OCB",cfg_false, CFGF_NONE),
    CFG_BOOL("ACK",cfg_false, CFGF_NONE),
    CFG_END(),
};

static cfg_t* cfg;

/**
 * itsnet_cfg_parse
 * @param file
 * @return int
 */
int itsnet_cfg_parse(char* file)
{
    int ret = 0;
    cfg = cfg_init(opts, CFGF_NONE);

    ret = cfg_parse(cfg, file);
    if(ret == CFG_FILE_ERROR) {
        fprintf(stderr, "Cannot find configuration file %s\n", file);
        exit(1);
    } else if(ret == CFG_PARSE_ERROR) {
        fprintf(stderr, "Parse error in configuration file %s\n", file);
        return -2;
    }
    return 0;
}

/**
 * itsnet_aton
 * @param char*
 * @return itsnet_node_id
 */
itsnet_node_id* itsnet_aton(const char* asc)
{
    size_t cnt;

    struct itsnet_node_id* node_id = (struct itsnet_node_id*)malloc(sizeof(struct itsnet_node_id));

    for(cnt = 0; cnt < 8; ++cnt) {
        unsigned int number;
        char ch;

        ch = _tolower(*asc++);
        if((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
            return NULL;
        number = isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

        ch = _tolower(*asc);
        if((cnt < 7 && ch != ':') || (cnt == 7 && ch != '\0' && !isspace(ch))) {
            ++asc;
            if((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
                return NULL;
            number <<= 4;
            number += isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

            ch = *asc;
            if(cnt < 7 && ch != ':')
                return NULL;
        }

        /* Store result.  */
        node_id->id[cnt] = (unsigned char)number;

        /* Skip ':'.  */
        ++asc;
    }
    return node_id;
}

/**
 * itsnet_aton
 * @param char*
 * @return itsnet_node_id
 */
mac_addr* itsnet_aton_ether(const char* asc)
{
    size_t cnt;

    struct mac_addr* addr = (struct mac_addr*)malloc(sizeof(struct mac_addr));

    for(cnt = 0; cnt < 6; ++cnt) {
        unsigned int number;
        char ch;

        ch = _tolower(*asc++);
        if((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
            return NULL;
        number = isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

        ch = _tolower(*asc);
        if((cnt < 5 && ch != ':') || (cnt == 5 && ch != '\0' && !isspace(ch))) {
            ++asc;
            if((ch < '0' || ch > '9') && (ch < 'a' || ch > 'f'))
                return NULL;
            number <<= 4;
            number += isdigit(ch) ? (ch - '0') : (ch - 'a' + 10);

            ch = *asc;
            if(cnt < 5 && ch != ':')
                return NULL;
        }

        /* Store result.  */
        addr->address[cnt] = (unsigned char)number;

        /* Skip ':'.  */
        ++asc;
    }
    return addr;
}

/**
 * itsnet_cfg_clean
 */
void itsnet_cfg_clean()
{
    cfg_free(cfg);
}

/* Config file accessors */

/**
 * Device_parse
 * @param void
 * @return char*
 */
char* Device_parse(void)
{
    return ((char*)cfg_getstr(cfg, "Device"));
}


/**
 * OCB_parse
 * @param void
 * @return char *
 */
bool OCB_parse(void)
{
    return (cfg_getbool(cfg, "OCB"));
}


/**
 * ACK_parse
 * @param void
 * @return char *
 */
bool ACK_parse(void)
{
    return (cfg_getbool(cfg, "ACK"));
}


/**
 * ItsnetDataSize_Parse
 * @param void
 * @return int
 */
int ItsnetDataSize_parse(void)
{
    return (cfg_getint(cfg, "ItsnetDataSize"));
}

/**
 * BroadcastMac_Parse
 * @param void
 * @return address
 */
mac_addr* BroadcastMac_parse(void)
{
    struct mac_addr* ea = NULL;
    ea = itsnet_aton_ether(cfg_getstr(cfg, "BroadcastMac"));
    return ea;
}

/**
 * NodeId_parse
 * @param void
 * @return itsnet_node_id
 */
itsnet_node_id* NodeId_parse(void)
{
    struct itsnet_node_id* ea;
    ea = itsnet_aton(cfg_getstr(cfg, "NodeId"));
    return ea;
}

/**
 * MaxNeighbor_parse
 * @param void
 * @return int
 */
int MaxNeighbor_parse(void)
{
    return (cfg_getint(cfg, "MaxNeighbor"));
}

/**
 * LocationTableEntry_parse
 * @param void
 * @return int
 */
int LocationTableEntry_parse(void)
{
    return (cfg_getint(cfg, "LocationTableEntry"));
}

/**
 * EthPItsnet_parse
 * @param void
 * @return char*
 */
char* EthPItsnet_parse(void)
{
    return ((char*)cfg_getstr(cfg, "EthPItsnet"));
}

/**
 * SendBeacon_parse
 * @param void
 * @return int
 */
int SendBeacon_parse(void)
{
    return (cfg_getbool(cfg, "SendBeacon"));
}

/**
 * DebugLevel_parse
 * @param void
 * @return int
 */
int DebugLevel_parse(void)
{
    return (cfg_getint(cfg, "DebugLevel"));
}

/**
 * DetachFromTTY_parse
 * @param void
 * @return int
 */
int DetachFromTTY_parse(void)
{

    return (cfg_getbool(cfg, "DetachFromTTY"));
}

/**
 * Extended_parse
 * @param void
 * @return char *
 */
bool Extended_parse(void)
{
    return (cfg_getbool(cfg, "Extended"));
}

/**
 * CovCravPort_parse
 * @param void
 * @return int
 */
int CovCravPort_parse(void)
{
    return (cfg_getint(cfg, "CovCravPort"));
}