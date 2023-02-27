#include <errno.h>

#include <asm/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#include <gps.h>

#include "itsnet_types.h"

#define SOCK_ITSNET_PATH "/tmp/itsnet_socket"
#define PI 3.14159265358979323846
#define RADIUS 6371 // Radius of the earth

static struct gps_data_t* GPS = NULL;

/*static int sock_connected = 0;*/

static int sock;

static struct itsnet_position_information ego_position_information;

int convert_format_lat(const double lat);

double inv_convert_format_lat(const int lat);

int convert_format_long(const double lon);

double inv_convert_format_long(const int lon);

uint32_t convert_time_stamp(const __suseconds_t ts); /* Time must be in milliseconds*/

int convert_format_spd(const double spd);

double inv_convert_format_spd(const int spd);

double _deg2rad(double deg);

int convert_format_alt(const double alt);

static void gps_data_update()
{
    __suseconds_t t_ms;
    t_ms=(GPS->fix.time.tv_sec*(uint32_t)1000)+(GPS->fix.time.tv_nsec/1000000);
    ego_position_information.accuracy = 0;
    ego_position_information.time_stamp = convert_time_stamp(t_ms);
    ego_position_information.latitude = convert_format_lat(GPS->fix.latitude);
    ego_position_information.longitude = convert_format_long(GPS->fix.longitude);
    ego_position_information.speed = convert_format_spd(GPS->fix.speed);
    ego_position_information.heading = 0;
    ego_position_information.altitude = convert_format_alt(GPS->fix.altitude);
}
/*static struct itsnet_position_information ego_position_information;*/

static int send_socket(message* m)
{
    int length,len;
    struct sockaddr_un client_sockaddr;
    
    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if(sock == -1) {
        perror("socket");
        return 1;
    }

    memset(&client_sockaddr, 0, sizeof(struct sockaddr_un));
    client_sockaddr.sun_family = AF_UNIX;
    strcpy(client_sockaddr.sun_path, SOCK_ITSNET_PATH);
    len = sizeof(client_sockaddr);

    length = sizeof(struct message);
    len= sizeof(struct sockaddr_un);
    sendto(sock, m,length, 0, (struct sockaddr*)&client_sockaddr,len);

    close(sock);
    return 0;
}

int main(void)
{

    struct message m;
    char server[16];
    char port[5];
    strcpy(server, "0.0.0.0\0");
    strcpy(port, "2947\0");
    GPS = (struct gps_data_t*)malloc(sizeof(struct gps_data_t));
    if(gps_open(server, port, GPS) < 0) {
        printf("\nerror opening gps");
        exit(errno);
    }
    memset(&m,0,sizeof(message));
    m.msg_hdr.aid = itsnet_position_sensor;
    m.msg_hdr.sid = itsnet_management_sap;
    m.msg_hdr.opcode = itsnet_request;
    m.msg_hdr.aid = itsnet_position_sensor;
    m.payload.itsnet_position_sensor_req.position_information = ego_position_information;
    while(1) {
        /*gps_query(GPS,"o\n");*/
        gps_stream(GPS, WATCH_ENABLE, NULL);
        /*gps_poll(GPS);*/
        gps_read(GPS, NULL, 0);
        gps_data_update();
        m.payload.itsnet_position_sensor_req.position_information = ego_position_information;
        send_socket(&m);
        /*sock_connected++; */
        sleep(1);
    }
    /* close(sock); */
    gps_close(GPS);

    return EXIT_SUCCESS;
}

int convert_format_lat(const double lat)
{
    return (((int)(lat * 1000000)) << 3);
}

int convert_format_alt(const double alt)
{
    return (((int)(alt * 1000000)) << 3);
}

double inv_convert_format_lat(const int lat)
{

    return ((double)(lat >> 3)) / 1000000;
}

int convert_format_long(const double lon)
{
    return (((int)(lon * 1000000)) << 3);
}

double inv_convert_format_long(const int lon)
{
    return ((double)(lon >> 3)) / 1000000;
}

uint32_t convert_time_stamp(const __suseconds_t ts)
{
    return (((int)ts) % 65536);
}

int convert_format_spd(const double spd)
{
    return (int)(spd * 100);
}

double inv_convert_format_spd(const int spd)
{
    return (double)(spd / 100);
}

double _deg2rad(double deg)
{
    return (deg * (PI / 180.0));
}
