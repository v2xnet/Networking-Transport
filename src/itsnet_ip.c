/***************************************************************************
 *   ITSNET  Intelligent Transport System networking Stack                 *
 * 									   *
 *   Copyright(C)2010 ESPRIT						   *
 * 	    "École supérieure privée d'ingénierie et de technologie"       *
 *                                                                         *
 *   barghich@gmail.com                                                    *
 *   anouar.chemek@gmail.com                                               *
 *   thouraya.toukabri@inria.fr                                	 	   *
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
 * @file itsnet_ip.c
 * itsnet ip code.
 * @author Thouraya Toukabri
 * @author Tsukada Manabu
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */

#include <stdint.h>
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <asm/types.h>
#include <errno.h>
#include <fcntl.h>
#include <linux/if.h>
#include <linux/if_tun.h>
#include <netinet/if_ether.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/un.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

#include "./include/linux/ip_mp_alg.h"
#include <arpa/inet.h>
#include <linux/in_route.h>
#include <netinet/in.h>
#include <netinet/ip.h>

#include "./include/rt_names.h"
#include "./include/utils.h"

#include "geo_anycast.h"
#include "geo_broadcast.h"
#include "geo_unicast.h"
#include "itsnet_header.h"
#include "itsnet_ip.h"
#include "itsnet_parser.h"
#include "itsnet_types.h"
#include "position_sensor.h"

static pthread_mutex_t mutex;
static pthread_t itsnet_iplistener;

#define max(a, b) ((a) > (b) ? (a) : (b))
#define BUFF_SIZE 200

struct rtnl_handle rth = { .fd = -1 };
int preferred_family = AF_UNSPEC;
int resolve_hosts = 0;
int f1;
char dev1[7];
uint8_t dst_id[8];

static struct {
    int tb;
    int cloned;
    int flushed;
    char* flushb;
    int flushp;
    int flushe;
    int protocol, protocolmask;
    int scope, scopemask;
    int type, typemask;
    int tos, tosmask;
    int iif, iifmask;
    int oif, oifmask;
    int realm, realmmask;
    inet_prefix rprefsrc;
    inet_prefix rvia;
    inet_prefix rdst;
    inet_prefix mdst;
    inet_prefix rsrc;
    inet_prefix msrc;
} filter;

void iproute_reset_filter()
{
    memset(&filter, 0, sizeof(filter));
    filter.mdst.bitlen = -1;
    filter.msrc.bitlen = -1;
}

static inline int rtm_get_table(struct rtmsg* r, struct rtattr** tb)
{
    __u32 table = r->rtm_table;
    if(tb[RTA_TABLE])
        table = *(__u32*)RTA_DATA(tb[RTA_TABLE]);
    return table;
}

uint8_t convert(char c)
{
    switch(c) {

    case '0':
        return 0x00;
        break;
    case '1':
        return 0x01;
        break;
    case '2':
        return 0x02;
        break;
    case '3':
        return 0x03;
        break;
    case '4':
        return 0x04;
        break;
    case '5':
        return 0x05;
        break;
    case '6':
        return 0x06;
        break;
    case '7':
        return 0x07;
        break;
    case '8':
        return 0x08;
        break;
    case '9':
        return 0x09;
        break;
    case 'a':
        return 0x0a;
        break;
    case 'A':
        return 0x0a;
        break;
    case 'b':
        return 0x0b;
        break;
    case 'B':
        return 0x0b;
        break;
    case 'c':
        return 0x0c;
        break;
    case 'C':
        return 0x0c;
        break;
    case 'd':
        return 0x0d;
        break;
    case 'D':
        return 0x0d;
        break;
    case 'e':
        return 0x0e;
        break;
    case 'E':
        return 0x0e;
        break;
    case 'f':
        return 0x0f;
        break;
    case 'F':
        return 0x0f;
        break;
    default:
        return 1;
    }
}

int print_route(const struct sockaddr_nl* who, struct nlmsghdr* n, void* arg)
{
    FILE* fp = (FILE*)arg;
    struct rtmsg* r = NLMSG_DATA(n);
    int len = n->nlmsg_len;
    struct rtattr* tb[RTA_MAX + 1];
    char abuf[256];
    int host_len = -1;
    int i = 0, s = 0;
    __u32 table;
    char* c2c_id[4];
    char* tmpbuf = NULL;
    uint8_t dst[16];

    if(n->nlmsg_type != RTM_NEWROUTE && n->nlmsg_type != RTM_DELROUTE) {
        fprintf(stderr, "Not a route: %08x %08x %08x\n", n->nlmsg_len, n->nlmsg_type, n->nlmsg_flags);
        return 0;
    }
    if(filter.flushb && n->nlmsg_type != RTM_NEWROUTE)
        return 0;
    len -= NLMSG_LENGTH(sizeof(*r));
    if(len < 0) {
        fprintf(stderr, "BUG: wrong nlmsg len %d\n", len);
        return -1;
    }

    if(r->rtm_family == AF_INET6) {
        host_len = 128;
    } else if(r->rtm_family == AF_INET) {
        printf("IPv4!\n");
        return 0;
    } else if(r->rtm_family == AF_DECnet) {
        printf("DECnet!\n");
        return 0;
    } else if(r->rtm_family == AF_IPX) {
        printf("IPX!\n");
        return 0;
    }

    parse_rtattr(tb, RTA_MAX, RTM_RTA(r), len);
    table = rtm_get_table(r, tb);

    if(tb[RTA_GATEWAY] && filter.rvia.bitlen != host_len) {
        format_host(r->rtm_family, RTA_PAYLOAD(tb[RTA_GATEWAY]), RTA_DATA(tb[RTA_GATEWAY]), abuf, sizeof(abuf));
    }

    printf("IP next hop: %s \n ", abuf);

    printf("\n\n------- finding C2C NET ID from IPv6 address --------\n");

    for(i = 3; i >= 0; i--) {
        tmpbuf = strrchr(abuf, ':');
        if(tmpbuf != NULL) {

            c2c_id[i] = tmpbuf;
            *tmpbuf = '\0';
        }
    }

    printf("C2C NET ID: %s %s %s %s   \n", c2c_id[0] + 1, c2c_id[1] + 1, c2c_id[2] + 1, c2c_id[3] + 1, c2c_id[4] + 1);

    for(i = 0; i < 4; i++) {
        for(s = 0; s < strlen(c2c_id[i] + 1); s++) {
            dst[4 * i + s + 4 - strlen(c2c_id[i] + 1)] = convert(c2c_id[i][s + 1]);
            // printf(" c2c id %d %d= %c  \n",i,s,c2c_id[i][s+1]);
        }
    }

    for(i = 0; i < 8; i++)
        dst_id[i] = (dst[2 * i] << 4) | (dst[2 * i + 1] & 0x0f);

    printf("\n\n");
    fflush(fp);
    return 0;
}

int iproute_get(char* destination)
{
    struct {
        struct nlmsghdr n;
        struct rtmsg r;
        char buf[1024];
    } req;
    inet_prefix addr;

    printf("\n\n------- lookuping routing table ---------------------\n");

    memset(&req, 0, sizeof(req));

    iproute_reset_filter();

    req.n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg));
    req.n.nlmsg_flags = NLM_F_REQUEST;
    req.n.nlmsg_type = RTM_GETROUTE;
    req.r.rtm_family = preferred_family;
    req.r.rtm_table = 0;
    req.r.rtm_protocol = 0;
    req.r.rtm_scope = 0;
    req.r.rtm_type = 0;
    req.r.rtm_src_len = 0;
    req.r.rtm_dst_len = 0;
    req.r.rtm_tos = 0;

    get_prefix(&addr, destination, req.r.rtm_family);
    if(req.r.rtm_family == AF_UNSPEC)
        req.r.rtm_family = addr.family;
    if(addr.bytelen) {
        addattr_l(&req.n, sizeof(req), RTA_DST, &addr.data, addr.bytelen);
    }
    req.r.rtm_dst_len = addr.bitlen;

    if(req.r.rtm_dst_len == 0) {
        fprintf(stderr, "need at least destination address\n");
        exit(1);
    }

    ll_init_map(&rth);

    if(rtnl_talk(&rth, &req.n, 0, 0, &req.n, NULL, NULL) < 0) {
        printf("iproute_get: rtnl_talk failed!!\n");
        exit(2);
    }

    if(print_route(NULL, &req.n, (void*)stdout) < 0) {
        fprintf(stderr, "An error :-)\n");
        exit(1);
    }
}

int tun_alloc(char* dev)
{
    struct ifreq ifr;
    int fd, err;

    if((fd = open("/dev/net/tun", O_RDWR)) < 0)
        return -1;

    memset(&ifr, 0, sizeof(ifr));

    /* Flags: IFF_TUN   - TUN device (no Ethernet headers)
     *         IFF_TAP   - TAP device
     *         IFF_NO_PI - Do not provide packet information
     */
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    if(*dev)
        strncpy(ifr.ifr_name, dev, IFNAMSIZ);

    if((err = ioctl(fd, TUNSETIFF, (void*)&ifr)) < 0) {
        close(fd);
        return err;
    }
    strcpy(dev, ifr.ifr_name);

    return fd;
}

/**
 * iplistener
 * @param void
 * @return void
 */

static void* iplistener(void* arg)
{

    short buf[BUFF_SIZE];
    int l, fm;
    fd_set fds;
    int i = 0, m = 0, n = 0, j = 0, k = 0;
    uint16_t src[8], dst[8];
    char destination[37];

    printf("-------------------------------------------- waiting to send packet... ---------------------------\n\n "
           "Allocated devices: ");
    for(i = 0; i < 7; i++)
        printf("%c", dev1[i]);

    printf("\n\n");

    fm = f1 + 1;

    if(rtnl_open(&rth, 0) < 0)
        exit(1);

    while(1) {

        FD_ZERO(&fds);
        FD_SET(f1, &fds);

        select(fm, &fds, NULL, NULL, NULL);

        if(FD_ISSET(f1, &fds)) {

            l = read(f1, buf, sizeof(buf));

            printf("-------------------------------- packet on tun0------------------------------\n\n");

            printf("\t Got %i bytes from  %s   \n", l, dev1);

            printf("\n------------------------------- packet infomation ---------------------------\n\n");

            m = 0;
            for(i = 0; i < l; i++) {

                /* TCP/IP network byte order to host byte order (which is little-endian on Intel processors) */

                buf[i] = htons(buf[i]);

                /* Parse Source address */

                if(i > 10 && i < 19) {
                    src[m] = buf[i];
                    printf(" %x :", src[m]);
                    m++;
                }
            }

            printf("\n");
            n = 0;
            for(i = 0; i < l; i++) {
                /* Parse Destination address */

                if(i > 18 && i < 27) {
                    dst[n] = buf[i];
                    printf(" %x :", dst[n]);
                    n++;
                }
            }

            printf("\n");

            /* set destination mac@ to ff:ff:ff:ff:ff:ff */

            for(i = 0; i < 3; i++)

                buf[i] = 0xffff;
        }

        /* Dispatching packet to C2C layer */

        if((dst[0] >> 8) == 0xff)

        /*************************Send Geobroadcast************************/

        {

            printf("\n GeoBroadcast encapsulation...    \n");

            itsnet_ip_geobroadcast_tnl(*(&buf), l, 295127000, 81543616, 1000);

        } else {

            /*************************Send Geounicast************************/

            itsnet_node_id nodeID;

            /****Find IP next hop****/

            sprintf(*(&destination), "%x:%x:%x:%x:%x:%x:%x:%x", dst[0], dst[1], dst[2], dst[3], dst[4], dst[5], dst[6],
                dst[7]);
            iproute_get(destination);

            /****Fill destination node ID****/
            for(j = 0; j < 8; j++) {
                nodeID.id[j] = dst_id[j];
                printf(" %2x ", nodeID.id[j]);
            }

            printf("\n GEoUNicast encapsulation...   \n");

            itsnet_ip_geounicast_tnl(*(&buf), l, 295127000, 81543616, nodeID);

            /* print buffer on sending */

            printf("\n\n contenu du buffer\n");

            for(j = 0; j < l; j++)
                printf(" %2x :", buf[j] & 0xffff);
        }
    }

    rtnl_close(&rth);
}

/**
 * itsnet_iplistener_init
 * @param void
 * @return int
 */
int itsnet_iplistener_init(void)
{

    strcpy(dev1, "tun0");

    if((f1 = tun_alloc(dev1)) < 0) {
        printf("Cannot allocate TAP device for sending \n");
        exit(1);
    }

    pthread_mutexattr_t mattrs;
    pthread_mutexattr_init(&mattrs);
    pthread_mutexattr_settype(&mattrs, PTHREAD_MUTEX_TIMED_NP);

    if(pthread_mutex_init(&mutex, &mattrs) || pthread_create(&itsnet_iplistener, NULL, iplistener, NULL))
        return -1;

    return 0;
}

/**
 * itsnet_ip_geobroadcast_tnl
 * @param short * buf
 * @param int l
 * @param uint32_t latitude
 * @param uint32_t longitude
 * @param double geo_area_size
 * @return int
 */
int itsnet_ip_geobroadcast_tnl(short* buf, int l, uint32_t latitude, uint32_t longitude, double geo_area_size)
{

    int result, i;
    struct itsnet_packet* p;
    struct itsnet_position_vector pos;

    pos = get_position_vector();
    p = (itsnet_packet*)malloc(sizeof(itsnet_packet));

    if(p == NULL) {
        printf("erreur allocation \n");
        result = -1;
    } else {

        memset(p, 0, sizeof(itsnet_packet));
        p->common_header.protocol_info.itsnet_header_type = itsnet_geobroadcast_id;
        p->common_header.traffic_class = CLASS01;
        p->common_header.protocol_info.itsnet_header_subtype = ITSNET_GEOBCAST_CIRCLE;
        p->common_header.flags = OBU_SECURITY_ENABLED;
        p->common_header.payload_lenght = ITSNET_DATA_SIZE;
        p->common_header.hop_limit = 255;
        p->common_header.forwarder_position_vector = pos;
        p->common_header.protocol_info.itsnet_next_header = ip;

        p->payload.itsnet_geobroadcast.source_position_vector = pos;
        p->payload.itsnet_geobroadcast.dest_latitude = latitude;
        p->payload.itsnet_geobroadcast.dest_longitude = longitude;
        p->payload.itsnet_geobroadcast.geo_area_size = geo_area_size;
    }

    memcpy(p->payload.itsnet_geobroadcast.payload, buf, l);

    result = itsnet_geobroadcast_send(p);
    // free(p);
    return result;
}

/**
 * itsnet_ip_geounicast_tnl
 * @param short * buf
 * @param int l
 * @param uint32_t latitude
 * @param uint32_t longitude
 * @param itsnet_node_id node_id
 * @return int
 */
int itsnet_ip_geounicast_tnl(short* buf, int l, uint32_t latitude, uint32_t longitude, itsnet_node_id node_id)
{

    int result;
    struct itsnet_packet* p;
    struct itsnet_position_vector pos;

    pos = get_position_vector();
    p = (itsnet_packet*)malloc(sizeof(itsnet_packet));

    if(p == NULL) {
        printf("erreur allocation \n");
        result = -1;
    } else {

        memset(p, 0, sizeof(itsnet_packet));
        p->common_header.protocol_info.itsnet_header_type = itsnet_unicast_id;
        p->common_header.traffic_class = CLASS01;
        p->common_header.protocol_info.itsnet_header_subtype = 0;
        p->common_header.flags = OBU_SECURITY_ENABLED;
        p->common_header.payload_lenght = ITSNET_DATA_SIZE;
        p->common_header.hop_limit = 255;
        p->common_header.forwarder_position_vector = pos;
        p->common_header.protocol_info.itsnet_next_header = ip;

        p->payload.itsnet_unicast.source_position_vector = pos;
        p->payload.itsnet_unicast.dest_latitude = latitude;
        p->payload.itsnet_unicast.dest_longitude = longitude;
        p->payload.itsnet_unicast.dest_node_id = node_id;
    }

    memcpy(p->payload.itsnet_unicast.payload, buf, l);
    result = itsnet_geounicast_send(p);
    //	free(p);
    return result;
}

/**
 * itsnet_ip_geoanycast_tnl
 * @param short * buf
 * @param int l
 * @param uint32_t latitude
 * @param uint32_t longitude
 * @param double geo_area_size
 * @return int
 */
int itsnet_ip_geoanycast_tnl(short* buf, int l, uint32_t latitude, uint32_t longitude, double geo_area_size)
{
    int result;
    struct itsnet_packet* p;
    struct itsnet_position_vector pos;

    pos = get_position_vector();
    p = (itsnet_packet*)malloc(sizeof(itsnet_packet));

    if(p == NULL) {
        printf("erreur allocation \n");
        result = -1;
    } else {

        memset(p, 0, sizeof(itsnet_packet));
        p->common_header.protocol_info.itsnet_header_type = itsnet_geoanycast_id;
        p->common_header.traffic_class = CLASS01;
        p->common_header.protocol_info.itsnet_header_subtype = ITSNET_GEOANYCAST_CIRCLE;
        p->common_header.flags = OBU_SECURITY_ENABLED;
        p->common_header.payload_lenght = ITSNET_DATA_SIZE;
        p->common_header.hop_limit = 255;
        p->common_header.forwarder_position_vector = pos;
        p->common_header.protocol_info.itsnet_next_header = ip;

        p->payload.itsnet_geoanycast.source_position_vector = pos;
        p->payload.itsnet_geoanycast.dest_latitude = latitude;
        p->payload.itsnet_geoanycast.dest_longitude = longitude;
        p->payload.itsnet_geoanycast.geo_area_size = geo_area_size;
    }

    memcpy(p->payload.itsnet_geoanycast.payload, buf, l);
    result = itsnet_geoanycast_send(p);
    //	free(p);
    return result;
}

/**
 * itsnet_ip_receive
 * @param p itsnet_packet
 * @return int
 */

void itsnet_ip_receive(struct itsnet_packet* p)
{

    int result, fm;

    int i = 0, l = 0, n = 0;

    short buf[BUFF_SIZE];

    fd_set fds;

    l = p->common_header.payload_lenght;

    printf("\n-------------------------------- waiting to receive packet... ---------------------------\n Allocated "
           "devices: ");

    for(i = 0; i < 7; i++)
        printf("%c", dev1[i]);

    printf("\n\n");

    fm = f1 + 1;

    FD_ZERO(&fds);
    FD_SET(f1, &fds);

    select(fm, NULL, &fds, NULL, NULL);

    if(FD_ISSET(f1, &fds)) {

        switch(p->common_header.protocol_info.itsnet_header_type) {
        case itsnet_geobroadcast_id:

            printf(
                "\n -------------------------- Received MULTICAST IP PACKET IN C2C PACKET ----------------------- \n");
            for(i = 0; i < l; i++) {
                printf(" %2x : ", p->payload.itsnet_geobroadcast.payload[i] & 0xffff);
                buf[i] = ntohs(p->payload.itsnet_geobroadcast.payload[i]);
            }

            pthread_mutex_lock(&mutex);

            n = write(f1, buf, sizeof(buf));

            pthread_mutex_unlock(&mutex);

            printf("\n\t Got %i bytes written on %s   \n", n, dev1);

            break;

        case itsnet_unicast_id:

            printf("\n -------------------------- Received UNICAST IP PACKET IN C2C PACKET ----------------------- \n");

            for(i = 0; i < l; i++) {
                printf("%2x : ", p->payload.itsnet_unicast.payload[i] & 0xffff);
                buf[i] = ntohs(p->payload.itsnet_unicast.payload[i]);
            }

            pthread_mutex_lock(&mutex);

            n = write(f1, buf, sizeof(buf));

            pthread_mutex_unlock(&mutex);

            printf("\n\t Got %i bytes written on %s  \n", n, dev1);

            break;

        case itsnet_geoanycast_id:

            printf("\n -------------------------- Received ANYCAST IP PACKET IN C2C PACKET ----------------------- \n");

            for(i = 0; i < l; i++) {
                printf("%2x : ", p->payload.itsnet_geoanycast.payload[i] & 0xffff);
                buf[i] = ntohs(p->payload.itsnet_geoanycast.payload[i]);
            }

            pthread_mutex_lock(&mutex);

            n = write(f1, buf, BUFF_SIZE);

            pthread_mutex_unlock(&mutex);

            printf("\n\t Got %i bytes written on %s   \n", n, dev1);

            break;

        default: {
            printf("unknown packet\n");
        }
        }
    }
}
