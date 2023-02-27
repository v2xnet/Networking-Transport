#include <errno.h>

#include <asm/types.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>

#define PATH_MAX 20

#define PI 3.14159265358979323846

#include "itsnet_types.h"

#define SOCK_ITSNET_PATH "/tmp/itsnet_socket"

/*#define SUN_LEN(ptr) ((size_t) (((struct sockaddr_un *) 0)->sun_path)        \
              + strlen ((ptr)->sun_path))*/

/*static int sock_connected = 0;*/
static int sock;
/*
char line[LINE_MAX];
*/

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
/*
static int itsnet_position_sensor_request_send(struct message *m)
{
        m->msg_hdr.sid=itsnet_management_sap;
        m->msg_hdr.opcode=itsnet_request;
        m->msg_hdr.aid=itsnet_position_sensor;
        send_socket(m);
        return 0;

}

*/



int main(int argc, char** argv)
{

    FILE* fp;
    struct message m;
    uint32_t t_ms;
   
    memset(&m,0, sizeof(message));
    m.msg_hdr.aid = itsnet_position_sensor;
    m.msg_hdr.sid = itsnet_management_sap;
    m.msg_hdr.opcode = itsnet_request;

    if(argc == 2) {
        fp = fopen(*(argv + 1), "r");

        while(!feof(fp)) {

            fscanf(fp, "%u", &m.payload.itsnet_position_sensor_req.position_information.time_stamp);
            fscanf(fp, "%d", &m.payload.itsnet_position_sensor_req.position_information.latitude);
            fscanf(fp, "%d", &m.payload.itsnet_position_sensor_req.position_information.longitude);
            fscanf(fp, "%hd", &m.payload.itsnet_position_sensor_req.position_information.altitude);
            fscanf(fp, "%hd", &m.payload.itsnet_position_sensor_req.position_information.speed);
            sleep(1);
            send_socket(&m);
            /*sock_connected++;*/
        }
        /*close(sock);*/
        fclose(fp);

        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
