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
 * @file itsnet_constant.h
 * geo broadcast code.
 * @author hichem BARGAOUI
 * @author anouar Chemek
 */
 
 
 #define ITSNET_DATA_SIZE 200

#define ITSNET_LS_REQUEST 0
#define ITSNET_LS_REPLY 1
#define ITSNET_GEOBCAST_CIRCLE 0
#define ITSNET_GEOBCAST_RECT 1
#define ITSNET_GEOANYCAST_CIRCLE 0
#define ITSNET_GEOANYCAST_RECT 1
#define ITSNET_BEACON 0
#define TRANSPORT_NEXT_HEADER 200
#define NODE_ID_LEN 8
#define MAX_LLA_LEN 6
#define TIME_BEACON_INTERVAL 1000
#define TIMEOUT_NODE 5000
#define UNSPECIFIED 0

#define itsGnProtocolVersion 1
#define itsGnLocalGnAddr 1
#define itsGnMinUpdateFrequencyEPV 1000 /** Minimum update frequency of EPV in [1/ms] */
#define itsGnPaiInterval 80 /** Distance related to the confidence interval for latitude and longitude [m].*/
#define itsGnMaxSduSize 1398 /** Maximum size of GN-SDU [octets] 1 500 - GN_MAX (88) - GNSEC_MAX (0) */
#define itsGnMaxGeoNetworkingHeaderSize 88  /** GN_MAX: Maximum size of GeoNetworking header [octets] Without security defined in clause 9.8.2.*/
#define itsGnLifetimeLocTE 20 /** Lifetime of location table entry [s] */
#define itsGnLocationServiceMaxRetrans 10 /** Maximum number of retransmissions of LS Request packets*/
#define itsGnLocationServiceRetransmitTimer 1000 /** Duration of Location service retransmit timer [ms]*/
#define itsGnLocationServicePacketBufferSize 1024 /** Size of Location service packet buffer [Octets]*/
#define itsGnBeaconServiceRetransmitTimer 3000 /** Duration of Beacon service retransmit timer [ms]*/
#define itsGnBeaconServiceMaxJitter  itsGnBeaconServiceRetransmitTimer/4  /** Maximum beacon jitter [ms] */
#define itsGnDefaultHopLimit 10 /** Default hop limit indicating the maximum number of hops a packet travels*/
#define itsGnDPLLength 8 /** Length of Duplicate Packet List (DPL) per source (clause A.2)*/
#define itsGnMaxPacketLifetime 600 /** Upper limit of the maximum lifetime [s] */
#define itsGnDefaultPacketLifetime 60 /** Default packet lifetime [s] */
#define itsGnMaxPacketDataRate 100 /** Maximum packet data rate for a GeoAdhoc router [Ko/s].*/
#define itsGnMaxPacketDataRateEmaBeta 90 /** Weight factor for the Exponential Moving Average of the packet data rate PDR (clause B.2) in percent */
#define itsGnMaxGeoAreaSize 10 /** Maximum size of the geographical area for a GBC and GAC packet [km 2 ]. */
#define itsGnMinPacketRepetitionInterval 100 /** Lower limit of the packet repetition interval [ms]*/
#define itsGnNonAreaForwardingAlgorithm  GREEDY /** Default forwarding algorithm outside target area*/
#define itsGnAreaForwardingAlgorithm CBF  /** Default forwarding algorithm inside target area */
#define itsGnCbfMinTime 1 /** Minimum duration a GN packet shall be buffered in the CBF packet buffer [ms]*/
#define itsGnCbfMaxTime 100 /** Maximum duration a GN packet shall be buffered in the CBF packet buffer [ms]*/
#define itsGnDefaultMaxCommunicationRange 1000 /** Default theoretical maximum communication range [m]*/
#define itsGnBroadcastCBFDefSectorAngle 30 /** Default threshold angle for advanced GeoBroadcast algorithm in clause F.4 [degrees] */
#define itsGnUcForwardingPacketBufferSize 256 /** Size of UC forwarding packet buffer [Ko]*/
#define itsGnBcForwardingPacketBufferSize 1024 /** Size of BC forwarding packet buffer [Ko] */
#define itsGnCbfPacketBufferSize 256 /** itsGnCbfPacketBufferSize */
#define itsGnDefaultTrafficClass 0x00

#define HI_NIBBLE(b) ((b & 0xF) << 4)
#define LO_NIBBLE(b) (b & 0xF)


enum itsGnSecurity{ /** Indicates whether GN security is enabled */
    DISABLED=0,
    ENABLED
};

enum itsGnSnDecapResultHandling{ /** Indicates the handling of the SN-DECAP result code (service primitive SN-ENCAP.confirm parameter report).*/
    STRICT=0,
    NON_STRICT
};

enum itsGnLocalAddrConfMethod{
    AUTO=0,
    MANAGED,
    ANONYMOUS
};

enum itsGnIsMobile {
  Stationary=0,
  Mobile
};


enum itsGnIfType{ /** Indicates type of interface */
    Unspecified=0,
    ITS_G5 
};

/** 
 * Value of Next Header (Basic Header)
 **/
enum itsnet_basic_next_header{
    ANY_ = 0,
    common_header,
    secured_packet
};

/*
 *  Value of Next Heaser ( Common Header )
 * */
enum itsnet_common_next_header{
    ANY = 0,
    BTP_A,
    BTP_B,
    IPV6    
};


enum itsnet_lifetime_base{
  lt_base_50ms=0,
  lt_base_1s,
  lt_base_10s,
  lt_base_100s
};

/**
 * value of header sub types
 */
enum itsnet_geobroadcast_sub_type{
    GEOBROADCAST_CIRCLE=0,
    GEOBROADCAST_RECT,
    GEOBROADCAST_ELIP
};

enum itsnet_geoanycast_sub_type{
    GEOANYCAST_CIRCLE=0,
    GEOANYCAST_RECT,
    GEOANYCAST_ELIP
};

enum itsnet_TSB_sub_type{
    SINGLE_HOP=0,
    MULTI_HOP
};

enum itsnet_LS_sub_type{
    LS_REQUEST=0,
    LS_REPLY
};

/**
 * value of event types
 */
enum event_type {
    itsnet_new_neighbor = 1, /** new neighbor is added */
    itsnet_other_event
};

/**
 * value of next_header field in the common header
 */
enum next_header { security = 1, transport = 2, ip = 3, other = 0 };

/**
 * value of flag field in the common header
 */
enum header_flag {
    OBU_SECURITY_DISABLED = 0,
    RSU_SECURITY_DISABLED = 1,
    OBU_SECURITY_ENABLED = 2,
    RSU_SECURITY_ENABLED = 3
};

/**
 * value of Traffic Class field in the common header
 */
enum traffic_class { CLASS00 = 0, CLASS01 = 1, CLASS02 = 2, CLASS03 = 3 };
