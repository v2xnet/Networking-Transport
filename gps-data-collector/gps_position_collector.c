#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "itsnet_types.h"
#include <gps.h>

#define PATH "./data"

uint32_t prev_time_stamp = 0;

static struct gps_data_t* GPS = NULL;

static struct itsnet_position_information ego_position_vector;

int convert_format_lat(const double lat);

int convert_format_long(const double lon);

int convert_time_stamp(const double ts);

int convert_format_spd(const double spd);

int convert_format_alt(const double alt);

static void gps_data_update()
{

    ego_position_vector.accuracy = 0;
    ego_position_vector.time_stamp = convert_time_stamp(GPS->fix.time);
    ego_position_vector.latitude = convert_format_lat(GPS->fix.latitude);
    ego_position_vector.longitude = convert_format_long(GPS->fix.longitude);
    ego_position_vector.speed = convert_format_spd(GPS->fix.speed);
    ego_position_vector.heading = 0;
    ego_position_vector.altitude = convert_format_alt(GPS->fix.altitude);
}

void save_data()
{

    FILE* f;
    gps_data_update();

    if((ego_position_vector.time_stamp != 0)) {
        f = fopen("./data3", "a");
        fprintf(f, "%ld %d %d %d %d\n", ego_position_vector.time_stamp, ego_position_vector.latitude,
            ego_position_vector.longitude, ego_position_vector.altitude, ego_position_vector.speed);
        /*
        prev_time_stamp=ego_position_vector.time_stamp;

        */
        fclose(f);
    }
}

int main(void)
{
    char server[16];
    char port[5];
    char* req = "pavdx\n";
    strcpy(server, "127.0.0.1\0");
    strcpy(port, "2947\0");
    GPS = (struct gps_data_t*)malloc(sizeof(struct gps_data_t));

    if(gps_open(server, port, GPS) < 0) {
        printf("\nerror opening gps");
        exit(errno);
    }

    while(1) {
        sleep(1);
        /*gps_query(GPS,"o\n");*/
        gps_stream(GPS, WATCH_ENABLE, NULL);
        /*gps_poll(GPS);*/
        gps_read(GPS, NULL, 0);
        save_data();
    }

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

int convert_format_long(const double lon)
{
    return (((int)(lon * 1000000)) << 4);
}

int convert_time_stamp(const double ts)
{
    return (((int)ts) % 65536);
}

int convert_format_spd(const double spd)
{
    return (int)(spd * 100);
}
