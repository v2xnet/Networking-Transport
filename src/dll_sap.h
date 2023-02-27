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
 * @file dll_sap.h
 * dll sap code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 * @author Lamis AMAMOU
 */

#ifndef _DLL_SAP_H_
#define _DLL_SAP_H_ 1
#include "ieee80211_radiotap.h"
#include "itsnet_header.h"
#include <stddef.h>
#include <sys/time.h>
#include <linux/kernel.h>

/*#define COVCRAV_TX_RADIOTAP_PRESENT (		\
        (1 << IEEE80211_RADIOTAP_RATE)		| \
        (1 << IEEE80211_RADIOTAP_DBM_TX_POWER)	| \
        (1 << IEEE80211_RADIOTAP_TX_FLAGS))
*/
#define COVCRAV_TX_RADIOTAP_PRESENT (		\
	(1 << IEEE80211_RADIOTAP_RATE)		| \
	0)
// (1 << IEEE80211_RADIOTAP_DBM_TX_POWER)	| 
//  (1 << IEEE80211_RADIOTAP_ANTENNA)       | 
// (1 << IEEE80211_RADIOTAP_FLAGS)     |
//(1 << IEEE80211_RADIOTAP_TX_FLAGS)	| 
struct covcrav_radiotap_header {
            struct ieee80211_radiotap_header radiotap_header;
            uint8_t rate;
            //uint8_t txpower;
            //uint8_t tx_flags;
            //uint8_t antenna;
        } __attribute__((packed));

        struct llc {
            uint8_t DSAP;
            uint8_t SSAP;
            uint8_t control;
        } __attribute__((packed));

        struct snap {
            uint16_t OID1;
            uint8_t OID2;
            uint16_t protocolID;
        } __attribute__((packed));

        struct ieee80211_hdr {
            uint16_t /*__le16*/ frame_control;
            uint16_t /*__le16*/ duration_id;
            uint8_t addr1[6];
            uint8_t addr2[6];
            uint8_t addr3[6];
            uint16_t /*__le16*/ seq_ctrl;
            // uint8_t addr4[6];
        } __attribute__((packed));

        static uint8_t wildcard_bssid[MAX_LLA_LEN] = { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff };

#define WLAN_FC_TYPE_DATA 2
#define WLAN_FC_SUBTYPE_DATA 0
        // static uint8_t geonet_llc[8] = { 0xaa, 0xaa, 0x03, 0x00, 0x00, 0x00, 0x89, 0x47 };
        /**
         * Radiotap is a protocol of sorts that is used to convey information about the
         * physical-layer part of wireless transmissions. When monitoring an interface
         * for packets, it will contain information such as what rate was used, what
         * channel it was sent on, etc. When injecting a packet, we can use it to tell
         * the 802.11 card how we want the frame to be transmitted.
         *
         * The format of the radiotap header is somewhat odd.
         * include/net/ieee80211_radiotap.h does an okay job of explaining it, but I'll
         * try to give a quick overview here.
         *
         * Keep in mind that all the fields here are little-endian, so you should
         * reverse the order of the bytes in your head when reading. Also, fields that
         * are set to 0 just mean that we let the card choose what values to use for
         * that option (for rate and channel for example, we'll let the card decide).
         */
         
    static const uint8_t u8aRadiotapHeader[] = {
        0x00, 0x00, // <-- radiotap version, pad
        0x0c, 0x00, // <- radiotap header length
        0x04, 0x80, 0x00, 0x00, // IEEE80211_RADIOTAP_RATE, IEEE80211_RADIOTAP_TX_FLAGS
        // 0x6c, // 54mbps
        0x22,
        0x0, // pad
        0x10 | 0x08, 0x00, // IEEE80211_RADIOTAP_F_FCS | IEEE80211_RADIOTAP_F_TX_NOACK
    };  

    static const uint8_t u8_radiotap_header_no_ack[] = {

            0x00, 0x00, // <-- radiotap version (ignore this)
            0x18, 0x00, // <-- number of bytes in our header (count the number of "0x"s)

              /**
               * The next field is a bitmap of which options we are including.
               * The full list of which field is which option is in ieee80211_radiotap.h,
               * but I've chosen to include:
               *   0x00 0x01: timestamp
               *   0x00 0x02: flags
               *   0x00 0x03: rate
               *   0x00 0x04: channel
               *   0x80 0x00: tx flags (seems silly to have this AND flags, but oh well)
               */
              0x0f, 0x80, 0x00, 0x00,

              0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // <-- timestamp

              /**
               * This is the first set of flags, and we've set the bit corresponding to
               * IEEE80211_RADIOTAP_F_FCS, meaning we want the card to add a FCS at the end
               * of our buffer for us.
               */
              0x10,

              0x6c, // <-- rate
              0x00, 0x00, 0x00, 0x00, // <-- channel

              /**
               * This is the second set of flags, specifically related to transmissions. The
               * bit we've set is IEEE80211_RADIOTAP_F_TX_NOACK, which means the card won't
               * wait for an ACK for this frame, and that it won't retry if it doesn't get
               * one.
               */
              0x08, 0x00
};
        static const uint8_t u8_radiotap_header_ack[] = {

            0x00, 0x00,             // <-- radiotap version + pad byte
            0x0b, 0x00,             // <- radiotap header length
            0x04, 0x0c, 0x00, 0x00, // <-- bitmap
            0x6c,                   // <-- rate (in 500kHz units)
            0x0c,                   //<-- tx power
            0x01                    //<-- antenna
        };

        /**
         * After an 802.11 MAC-layer header, a logical link control (LLC) header should
         * be placed to tell the receiver what kind of data will follow (see IEEE 802.2
         * for more information).
         *
         * For political reasons, IP wasn't allocated a global so-called SAP number,
         * which means that a simple LLC header is not enough to indicate that an IP
         * frame was sent. 802.2 does, however, allow EtherType types (the same kind of
         * type numbers used in, you guessed it, Ethernet) through the use of the
         * "Subnetwork Access Protocol", or SNAP. To use SNAP, the three bytes in the
         * LLC have to be set to the magical numbers 0xAA 0xAA 0x03. The next five bytes
         * are then interpreted as a SNAP header. To specify an EtherType, we need to
         * set the first three of them to 0. The last two bytes can then finally be set
         * to 0x0800, which is the IP EtherType.
         */

#define ETH_P_ITSNET 0x8947

        /* see linux/if_ether.h */
#define ETH_MAC_LEN ETH_ALEN            /* Octets in one ethernet addr   */
#define ETH_HEADER_LEN ETH_HLEN         /* Total octets in header.       */
#define ETH_MIN_FRAME_LEN ETH_ZLEN      /* Min. octets in frame sans FCS */
#define ETH_USER_DATA_LEN ETH_DATA_LEN  /* Max. octets in payload        */
#define ETH_MAX_FRAME_LEN ETH_FRAME_LEN /* Max. octets in frame sans FCS */

#define BUF_SIZE 1514

        /**
         * thread packet listener
         * @param *arg
         */
        static void* packet_listener(void* arg);

        /**
         * data link layer sap init
         * @param int
         */
        int dll_sap_init(void*);

        /**
         * dll_sap_cleanup
         * @param void
         */
        static void dll_sap_cleanup(void);

        /**
         * itsnet_packet_send
         * @param itsnet_packet packet
         */
        int itsnet_packet_send(struct itsnet_packet*, char*);

        /**
         * itsnet_packet_recv
         * @param itsnet_packet packet
         */
        static void itsnet_packet_recv(struct itsnet_packet* p);

        static void process_itsnet_packet(struct itsnet_packet* p);

        int open_sock_raw(void);

        int get_mac_addr(void);

        void trace_recv_packet(itsnet_packet* p, struct timeval t_recv);

        void packet_hexdump(const uint8_t* data, size_t size);

#endif /* _DLL_SAP_H_*/
