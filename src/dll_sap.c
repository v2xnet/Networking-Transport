/***************************************************************************
 *   ITSNET  Intelligent Transport System networking Stack                 *
 * 									   *
 ** Copyright(C)2010 ESPRIT											   *
 * 	        "École supérieure privée d'ingénierie et de technologie"       *
 *                                                                         *
 *   barghich@gmail.com                                                    *
 *   anouar.chemek@gmail.com                                               *
 *   @author Lamis AMAMOU					                               *
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
 * @file dll_sap.c
 * dll sap code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 * @author Lamis AMAMOU
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "beaconing.h"
#include "dll_sap.h"
#include "geo_anycast.h"
#include "geo_broadcast.h"
#include "geo_unicast.h"
#include "location_service.h"
#include "routing_manager.h"
#include <asm/types.h>
#include <errno.h>
#include <net/if.h>
#include <pthread.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <unistd.h>

#include "itsnet_parser.h"
#include "tqueue.h"
#include <arpa/inet.h>
#include <linux/if.h>
#include <linux/if_arp.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>

static int sock; /*Socketdescriptor*/

static struct sockaddr_ll socket_address; /* structure utilisée dans les sockets pour représenter une addresse de la couche liaison */ 
static unsigned char src_mac[ETH_MAC_LEN]; /*our MAC address*/

static pthread_mutex_t mutex;
static pthread_t itsnet_packet_listener;
static void process_itsnet_packet(struct itsnet_packet*);
char* device = NULL;
static bool OCB, ACK;

/**
 * @brief Get our Mac address
 *
 * This function permits to get device Mac address
 */
int get_mac_addr(void)
{

    struct ifreq ifr; /*la structure ifreq sert à passer des informations à la couche réseau du OS */
    int ifindex = 0; /*Ethernet Interface index*/
    int i;
    /*retrieve device interface index*/
    device = Device_parse();
    strncpy(ifr.ifr_name, device, IFNAMSIZ);
    if(ioctl(sock, SIOCGIFINDEX, &ifr) == -1) { /*communicate with the device driver*/ 
    
    /* SIOCGIFINDEX est une commande ioctl utilisé pour obtenir l'index d'une interface réseau donnée
    Elle est utilisée pour intéragir avec la couche réseau
    prend comme argument une structure ifreq qui contient le nom de l'interface réseau dont l'index est demandé */ 
    
        return -1;
    }
    ifindex = ifr.ifr_ifindex;

    /*Socketdescriptor*/

    /*retrieve corresponding MAC*/
    if(ioctl(sock, SIOCGIFHWADDR, &ifr) == -1) {

        return -1;
    }
    /*for(i = 0; i < 6; i++) {
        src_mac[i] = ifr.ifr_hwaddr.sa_data[i];
    }*/
    memcpy(src_mac, ifr.ifr_hwaddr.sa_data, ETH_MAC_LEN); /* copier le bloc mémoire ifr.ifr_hwaddr.sa_data (addresse mac de l'interface)  vers le bloc mémoire src_mac */ 

    return 0;
}

/**
 * @brief Try to open socket
 *
 * @returns 0 if succeed / < 0 if failed
 *
 * This function opens AF_PACKET socket
 */
int open_sock_raw(void)
{
    /*Try to open socket*/
    
    /*AF_PACKET est un type de socket qui permet de manipuler des paquets de données au niveau de la couche liaison de données */ 
    
    sock = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL)); /* le type ETH_P_ALL pour capturer tous les types de paquets Ethernet */ 
    
    if(sock != -1)
        return 0;

    return -1;
}

/**
 * @brief Prepare socket address
 *
 * This function permits to pf packet socket
 */
void create_raw_socket(void)
{
    device = Device_parse();
    /*prepare sockaddr_ll*/
    socket_address.sll_family = PF_PACKET; /*PF_PACKET est une socket de type packet */
    socket_address.sll_protocol = htons(ETH_P_ITSNET);
    socket_address.sll_ifindex = if_nametoindex(device);
    socket_address.sll_halen = 0;
    return;
}

static void* packet_listener(void* param)
{
    int i = 0;
    struct sockaddr_ll rcvaddr;
    uint8_t buffer[BUF_SIZE];
    int length;
    size_t size_buff, size_header;
    /*length of received packet*/
    uint8_t* etherhead; /*Pointer to Ethenet Header*/
    struct ethhdr* eh;  /*Another pointer to ethernet header*/
    uint8_t* payload;
    struct itsnet_packet* data;

    uint8_t fcchunk[2]; /* 802.11 header frame control */
    /* The paradiotaps of our packet */
    uint8_t* radiotap; /* radiotap */
    int sent;
    struct ieee80211_hdr* hdr;
    struct llc* geonet_llc;
    struct snap* geonet_snap;

    if(!OCB) {
        etherhead = buffer;
        eh = (struct ethhdr*)etherhead;
        payload = buffer + 14;
        data = (struct itsnet_packet*)payload;
        size_buff = sizeof(struct itsnet_packet);
    } else {
        size_header = 18 + sizeof(struct ieee80211_hdr) + sizeof(struct llc) + sizeof(struct snap);
        size_buff = size_header + sizeof(struct itsnet_packet) + 4 /*FCS*/;

        radiotap = (uint8_t*)buffer;
        hdr = (struct ieee80211_hdr*)(radiotap + 18);
        geonet_llc = (struct llc*)(hdr + 1);
        geonet_snap = (struct snap*)(geonet_llc + 1);

        payload = buffer + size_header;
        data = (struct itsnet_packet*)payload;
    }
    while(1) {
        /*Wait for incoming packet...*/

        length = recvfrom(sock, buffer, size_buff, 0, NULL, NULL);/*recvform is a system call used for receiving data from socket , recvfrom(socket desc, buffer, length, flags, source address, address length); blocks until data is received and returns the number of bytes received (-1 if error) */ 

        if(length == -1) {
            printf("Error while receiving\n");
        }
        /*See if we should answer (Ethertype == 0x0707 && destination address == our MAC)*/

        if(!OCB) {
            if(eh->h_proto == ntohs(ETH_P_ITSNET)) { /* h_proto is a field in sockaddr_ll used to specify the type of protocol being used in ethernet frame */ 
                if(memcmp((const void*)eh->h_source, (const void*)src_mac, ETH_MAC_LEN) != 0) {
                    process_itsnet_packet(data);
                    //printf("\nRECEiVED PACKET:%d",i);
                }
            }
        } else {
             //packet_hexdump((uint8_t*)data, BUF_SIZE);
            if(geonet_snap->protocolID == ntohs(ETH_P_ITSNET)) {
                if(memcmp((const void*)hdr->addr2, (const void*)src_mac, ETH_MAC_LEN) != 0) {
                    process_itsnet_packet(data);
                    //packet_hexdump((uint8_t*)data, BUF_SIZE);
                     printf("\nRECEiVED PACKET:%d",i);
                }
            }
        }
    }
}

int dll_sap_init(void* data)
{
    pthread_mutexattr_t mattrs;
    pthread_mutexattr_init(&mattrs);

    // printf("dll_sap_init...\n");
    OCB = OCB_parse();
    ACK = ACK_parse();
    if(open_sock_raw() < 0) // Open socket to receive its packet
        return -1;

    create_raw_socket();

    if(get_mac_addr() < 0) /*Try to get specified DEVICE Mac address*/
        return -1;
    // printf("start packet_listener\n");

    if(pthread_mutex_init(&mutex, &mattrs) || pthread_create(&itsnet_packet_listener, NULL, packet_listener, NULL)) {
        return -1;
    }
    // printf("dll_sap_init\n");
    return 0;
}

/**
 * @brief Send itsnet packet
 *
 * @param packet its packet to send
 * @param dest_mac destination mac address
 *
 * This function permits to send packet to a desrination mac address
 *
 */
int itsnet_packet_send(struct itsnet_packet* packet, char* dest_mac)
{
	
    uint8_t* buffer;
    uint8_t* etherhead;
    struct ethhdr* eh;
    size_t size_buff, size_header;
    uint8_t fcchunk[2]; /* 802.11 header frame control */
    /* The parts of our packet */
    // struct covcrav_radiotap_header* radiotap; /* radiotap */
    uint8_t* radiotap; /* radiotap */
    int sent;
    struct ieee80211_hdr* hdr;
    struct llc* geonet_llc;
    struct snap* geonet_snap;

    if(!OCB) {
        buffer = (uint8_t*)malloc(BUF_SIZE); /*Buffer for ethernet frame*/
        memset(buffer, 0, ETH_FRAME_LEN);
        etherhead = buffer;             /*Pointer to ethenet header*/
        eh = (struct ethhdr*)etherhead; /*Another pointer to ethernet header*/
        size_buff = sizeof(struct itsnet_packet);
    } else {

        if(ACK)
            // size_header = sizeof(struct covcrav_radiotap_header) + sizeof(struct ieee80211_hdr) + sizeof(struct llc)
            // +  sizeof(struct snap);
            size_header = sizeof(u8_radiotap_header_ack) + sizeof(struct ieee80211_hdr) + sizeof(struct llc) +
                sizeof(struct snap);
        else
            size_header = sizeof(u8_radiotap_header_no_ack) + sizeof(struct ieee80211_hdr) + sizeof(struct llc) +
                sizeof(struct snap);

        size_buff = size_header + sizeof(struct itsnet_packet) + 4 /*FCS*/;

        buffer = (uint8_t*)malloc(size_buff); /*Buffer for ethernet frame*/

        memset(buffer, 0, size_buff);

        // radiotap = (struct covcrav_radiotap_header*)buffer;
        radiotap = (uint8_t*)buffer;
        if(ACK)
            hdr = (struct ieee80211_hdr*)(radiotap + sizeof(u8_radiotap_header_ack));
        // hdr = (struct ieee80211_hdr*)(radiotap + sizeof(struct covcrav_radiotap_header));
        else
            hdr = (struct ieee80211_hdr*)(radiotap + sizeof(u8_radiotap_header_no_ack));

        geonet_llc = (struct llc*)(hdr + 1);

        geonet_snap = (struct snap*)(geonet_llc + 1);
    }

    pthread_mutex_lock(&mutex);

    socket_address.sll_addr[0] = dest_mac[0];
    socket_address.sll_addr[1] = dest_mac[1];
    socket_address.sll_addr[2] = dest_mac[2];
    socket_address.sll_addr[3] = dest_mac[3];
    socket_address.sll_addr[4] = dest_mac[4];
    socket_address.sll_addr[5] = dest_mac[5];
    socket_address.sll_addr[6] = 0x00;
    socket_address.sll_addr[7] = 0x00;

    if(OCB) {
        /*prepare buffer*/
        /* construct the radiotap header */
        /*
        radiotap->radiotap_header.it_version = 0x00;
        radiotap->radiotap_header.it_len = 0x00;
        radiotap->radiotap_header.it_len = __cpu_to_le16(sizeof(struct covcrav_radiotap_header));

        radiotap->radiotap_header.it_present = __cpu_to_le32(COVCRAV_TX_RADIOTAP_PRESENT);
        //radiotap->flags=IEEE80211_RADIOTAP_F_FCS;
        radiotap->rate = 0x6c;
        //radiotap->txpower=0x0c;
        //radiotap->antenna=0x01;
        //radiotap->tx_flags |= IEEE80211_RADIOTAP_F_TX_NOACK;*/
        if(ACK)
            memcpy(radiotap, u8_radiotap_header_ack, sizeof(u8_radiotap_header_ack));
        else
            memcpy(radiotap, u8_radiotap_header_no_ack, sizeof(u8_radiotap_header_no_ack));

        /* construct the 802.11 header */
        fcchunk[0] = ((WLAN_FC_TYPE_DATA << 2) | (WLAN_FC_SUBTYPE_DATA << 4));
        fcchunk[1] = 0x00; /* ad-hoc mode */
        memcpy(&hdr->frame_control, &fcchunk[0], 2 * sizeof(uint8_t));

        hdr->duration_id = 0xffff;
        memcpy(hdr->addr1, dest_mac, 6 * sizeof(uint8_t));
        memcpy(hdr->addr2, src_mac, 6 * sizeof(uint8_t));
        memcpy(hdr->addr3, wildcard_bssid, 6 * sizeof(uint8_t));
        hdr->seq_ctrl = 0x00;

        /* concstruct The LLC+SNAP header */

        geonet_llc->DSAP = 0xAA;
        geonet_llc->SSAP = 0xAA;
        geonet_llc->control = 0x03;

        geonet_snap->OID1 = 0x0000;
        geonet_snap->OID2 = 0x00;
        geonet_snap->protocolID = htons(ETH_P_ITSNET);

        /*fill it with its data....*/
        memcpy((void*)(buffer + size_header), packet, sizeof(struct itsnet_packet));
        // memcpy((void*)(buffer + 24), packet, sizeof(struct itsnet_packet));

        // packet_hexdump(buffer,size_buff);
    } else {
        memcpy((void*)buffer, (void*)dest_mac, ETH_ALEN);
        memcpy((void*)(buffer + ETH_ALEN), (void*)src_mac, ETH_ALEN);
        eh->h_proto = htons(ETH_P_ITSNET);
        /*fill it with its data....*/
        memcpy((void*)(buffer + 14), packet, ITSNET_DATA_SIZE);
    }

    /*send packet*/
    sent = sendto(sock, buffer, size_buff, 0, (struct sockaddr*)&socket_address, sizeof(socket_address));

    if(sent == -1) {
        return -1;
    }

    pthread_mutex_unlock(&mutex);

    free(buffer);
    return 0;
}

static void process_itsnet_packet(struct itsnet_packet* p)
{
    struct itsnet_packet* packet;
    static struct timeval t_recv;

    packet = (struct itsnet_packet*)malloc(sizeof(struct itsnet_packet));
    memcpy(packet, p, sizeof(struct itsnet_packet));
    // printf("\n%x",packet->common_header.itsnet_header_type_subtype >> 4);
    switch(packet->common_header.itsnet_header_type_subtype >> 4) {

    case itsnet_any_id:
        // printf("value of packet unspecified \n");
        break;
    case itsnet_beacon_id:
        // printf("value of packet Beacon\n");
        itsnet_beacon_handler(packet);
        break;
    case itsnet_unicast_id:
        // printf("value of packet Geo-unicast \n");
        itsnet_geounicast_handler(packet);
        break;
    case itsnet_geoanycast_id:
        // printf("value of packet Geo-anycast \n");
        itsnet_geoanycast_handler(packet);
        break;
    case itsnet_geobroadcast_id:
        // printf("value of packet Geo-broadcast \n");
        gettimeofday(&t_recv, NULL); /* added to trace received packet (recv_dll) and to no act with packets in the same
                                     received second as the same packet*/
        packet->payload.itsnet_geobroadcast.source_position_vector.time_stamp = timeval_to_ms(&t_recv); /* TODO*/
        itsnet_geobroadcast_handler(packet);
        trace_recv_packet(packet, t_recv);
        // printf("\nGEO_BROADCAST RECEIVED");
        break;
    case itsnet_tsb_id:
        // printf("value of packet Topologically-scoped broadcast\n");
        itsnet_geotopo_handler(packet);
        break;
    case itsnet_ls_id:
        // printf("value of packet Location service\n");
        itsnet_location_handler(packet);
        break;
    default:
        free(packet);
        printf("Unknown packet type\n");
        /*break;*/
    }
}

void trace_recv_packet(itsnet_packet* p, struct timeval t_recv)
{
    static FILE* f_recv;
    /*denm* BUFF;
    BUFF = malloc(sizeof(struct denm));*/
    f_recv = fopen("/root/recv_dll.log", "a");
    /*memcpy((void*)BUFF, (void*)p->payload.itsnet_geobroadcast.source_position_vector., sizeof(struct denm));*/
    fprintf(f_recv, "TIME %lld\n", timeval_to_us(&t_recv));
    fclose(f_recv);
    // free(BUFF);
}

void packet_hexdump(const uint8_t* data, size_t size)
{
    size_t i;

    printf("%02x:", data[0]);
    for(i = 1; i < size; i++) {
        printf("%02x:", data[i]);
        if((i & 0xf) == 0xf) {
            // Add a carrage return every 16 bytes
            printf("\n");
        }
    }
    printf("\n\n");
}
