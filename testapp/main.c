#include <asm/types.h>
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>

#include "itsnet_types.h"

#define SOCK_PATH "/tmp/upper_layer_socket"
#define SOCK_ITSNET_PATH "/tmp/itsnet_socket"

/*static int sock_connected = 0;*/
static int sock;

void print_node_id(struct itsnet_node_id node_id)
{
    printf("%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x:%2.2x%2.2x ", (node_id.id[0] & 0xFF), (node_id.id[1] & 0xFF),
        (node_id.id[2] & 0xFF), (node_id.id[3] & 0xFF), (node_id.id[4] & 0xFF), (node_id.id[5] & 0xFF),
        (node_id.id[6] & 0xFF), (node_id.id[7] & 0xFF));
}

static int send_socket(message* m)
{
    int len;
    int length;
    struct sockaddr_un rem;

    if((sock = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
        return 1;
    }
    printf("Trying to connect...\n");

    rem.sun_family = AF_UNIX;
    strcpy(rem.sun_path, SOCK_ITSNET_PATH);
    len = strlen(rem.sun_path) + sizeof(rem.sun_family);
    if(connect(sock, (struct sockaddr*)&rem, len) == -1) {
        perror("connect");
        return 2;
    }

    printf("Connected.\n");

    length = sizeof(*m);
    printf("taille du message    %d\n", length);

    write(sock, &length, sizeof(length));
    write(sock, m, length);
    close(sock);
    return 0;
}

int itsnet_configure_node_id_request_send(struct message* m)
{
    int result;
    m->msg_hdr.sid = itsnet_management_sap;
    m->msg_hdr.opcode = itsnet_confirm;
    m->msg_hdr.aid = itsnet_configure_node_id;
    result = send_socket(m);
    return result;
}

int itsnet_configure_node_id_confirm_receive(struct message* m)
{

    int result;
    result = 1;
    printf("The Old NodeId is :");
    print_node_id(m->payload.itsnet_configure_node_id_conf.old_id);
    printf("The New NodeId Configured : ");
    print_node_id(m->payload.itsnet_configure_node_id_conf.new_id);
    return result;
}

int itsnet_get_node_id_request_send(struct message* m)
{
    int result;
    m->msg_hdr.aid = itsnet_get_node_id;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.sid = itsnet_management_sap;
    m->payload.itsnet_get_node_id_req.tmp = 1;
    result = send_socket(m);
    return result;
}

int itsnet_get_node_id_confirm_receive(struct message* m)
{
    printf("node_id");
    print_node_id(m->payload.itsnet_get_node_id_conf.node_id);

    /*int result;


        result=m->payload.itsnet_get_node_id_conf.node;

        return result;*/
}

int itsnet_get_neighbor_list_request_send(struct message* m)
{
    int result;
    m->msg_hdr.aid = itsnet_get_neighbor_list;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.sid = itsnet_management_sap;
    m->payload.itsnet_get_neighbor_list_req.tmp = 1;
    result = send_socket(m);
    return result;
}

int itsnet_get_neighbor_list_confirm_reveive(struct message* m)
{
    int result, size, i;
    result = 1;

    size = m->payload.itsnet_get_neighbor_list_conf.neighbor_list_size;

    printf("size %d \n", size);
    for(i = 0; i < size; i++) {

        printf("node id   :");
        print_node_id(m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[i].node_id);
        printf("\ntime_stamp    : %d \n", m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[i].time_stamp);
        printf("latitude      : %d \n", m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[i].latitude);
        printf("longitude     : %d \n", m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[i].longitude);
        printf("speed         : %d \n", m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[i].speed);
        printf("altitude      : %d \n", m->payload.itsnet_get_neighbor_list_conf.neighbor_list_pos[i].altitude);
    }
    return result;
}

int itsnet_event_indication_receive(struct message* m)
{
    switch(m->payload.itsnet_event_ind.event.type) {

    case itsnet_new_neighbor:
        printf("New Neighbor is added \n");
        printf("Node Identity :");
        print_node_id(m->payload.itsnet_event_ind.event.event.new_neighbor.node_id);
        printf("Node Latitude : %d \n", m->payload.itsnet_event_ind.event.event.new_neighbor.latitude);
        printf("Node Altitude : %d \n", m->payload.itsnet_event_ind.event.event.new_neighbor.altitude);
        printf("Node Speed : %d \n", m->payload.itsnet_event_ind.event.event.new_neighbor.speed);
        printf("Node Timestamp : %d \n", m->payload.itsnet_event_ind.event.event.new_neighbor.time_stamp);
        break;
    case itsnet_other_event:
        printf("Other Event \n");
        break;
    default:
        printf("unknown type\n");
        break;
    }

    return 0;
}

int itsnet_position_sensor_request_receive(struct message* m)
{
    /*TODO*/

    return 0;
}

itsnet_position_vector itsnet_get_position_vector_confirm_receive(struct message* m)
{

    struct itsnet_position_vector result;
    result.latitude = m->payload.itsnet_get_position_vector_conf.position_vector.latitude;
    result.heading = m->payload.itsnet_get_position_vector_conf.position_vector.heading;
    result.latitude = m->payload.itsnet_get_position_vector_conf.position_vector.latitude;
    result.longitude = m->payload.itsnet_get_position_vector_conf.position_vector.longitude;
    result.speed = m->payload.itsnet_get_position_vector_conf.position_vector.speed;
    result.time_stamp = m->payload.itsnet_get_position_vector_conf.position_vector.time_stamp;
    return (result);
}

int itsnet_unicast_indication_receive(struct message* m)
{
    printf("GeoUnicast Message is Received \n");
    printf("DATA %s: \n", m->payload.itsnet_unicast_ind.data);
    printf("from Node ID: ");
    print_node_id(m->payload.itsnet_unicast_ind.source_node_id);
}

int itsnet_geoanycast_indication_receive(struct message* m)
{

    printf("GeoAnycast Message is Received \n");
    printf("DATA %s: \n", m->payload.itsnet_geoanycast_ind.data);
    printf("from Node ID:");
    print_node_id(m->payload.itsnet_geoanycast_ind.source_node_id);
}

int itsnet_geobroadcast_indication_receive(struct message* m)
{
    printf("GeoBroadcast Message is Received \n");
    printf("DATA %s: \n", m->payload.itsnet_geobroadcast_ind.data);
    printf("from SOurce Node ID:");
    print_node_id(m->payload.itsnet_geobroadcast_ind.source_node_pos.node_id);
    printf("The FOrwarder ID is:");
    print_node_id(m->payload.itsnet_geobroadcast_ind.forwarder_node_pos.node_id);
}

static void socket_recv(struct message* m)
{
    struct itsnet_position_vector pos_vector;

    switch(m->msg_hdr.aid) {
    case itsnet_unicast:
        printf("itsnet_unicast_indication_receive\n");
        itsnet_unicast_indication_receive(m);
        break;
    case itsnet_geoanycast:
        printf("itsnet_geoanycast_indication_receive\n");
        itsnet_geoanycast_indication_receive(m);
        break;
    case itsnet_geobroadcast:
        printf("itsnet_geobroadcast_indication_receive\n");
        itsnet_geobroadcast_indication_receive(m);
        break;
    case itsnet_configure_node_id:
        printf("itsnet_configure_node_id_confirm_receive\n");
        itsnet_configure_node_id_confirm_receive(m);
        break;
    case itsnet_get_node_id:
        printf("itsnet_get_node_id_confirm_receive  \n");
        itsnet_get_node_id_confirm_receive(m);
        break;
    case itsnet_get_position_vector:
        pos_vector = itsnet_get_position_vector_confirm_receive(m);
        printf("itsnet_get_position_vector_confirm_receive \n");
        printf("Latitude: %d \n", pos_vector.latitude);
        printf("Longitude: %d \n", pos_vector.longitude);
        printf("Altitude: %d \n", pos_vector.altitude);
        printf("TimeStamp: %d \n", pos_vector.time_stamp);
        printf("Speed: %d \n", pos_vector.speed);
        break;
    case itsnet_events:
        printf("itsnet_event_indication_receive \n");
        itsnet_event_indication_receive(m);
        break;
    case itsnet_position_sensor:
        printf("itsnet_position_sensor_request_receive \n");
        itsnet_position_sensor_request_receive(m);
    case itsnet_get_neighbor_list:
        printf("itsnet_get_neighbor_list_confirm_reveive \n");
        itsnet_get_neighbor_list_confirm_reveive(m);
        break;
    default:
        printf("unknown type\n");
        break;
    }
    free(m);
}

int itsnet_get_position_vector_request_send(struct message* m)
{
    int result;
    m->msg_hdr.sid = itsnet_management_sap;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.aid = itsnet_get_position_vector;
    result = send_socket(m);
    return (result);
}

int itsnet_unicast_request_send(struct message* m)
{

    m->msg_hdr.sid = itsnet_transport_sap;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.aid = itsnet_unicast;
    strncpy(m->payload.itsnet_unicast_req.data, "test", 5);
    send_socket(m);
    return 0;
}

int itsnet_position_sensor_request_send(struct message* m)
{
    m->msg_hdr.sid = itsnet_management_sap;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.aid = itsnet_position_sensor;
    send_socket(m);
    return 0;
}

int itsnet_geoanycast_request_send(struct message* m)
{

    m->msg_hdr.sid = itsnet_transport_sap;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.aid = itsnet_geoanycast;
    strncpy(m->payload.itsnet_geobroadcast_req.data, "TEST", 5);
    send_socket(m);
    return 0;
}

int itsnet_geobroadcast_request_send(struct message* m)
{

    int i;
    m->msg_hdr.sid = itsnet_transport_sap;
    m->msg_hdr.opcode = itsnet_request;
    m->msg_hdr.aid = itsnet_geobroadcast;
    for(i = 0; i < 1482; i++)
        m->payload.itsnet_geobroadcast_req.data[i] = 'x';
    printf("fini\n");
    send_socket(m);
    return 0;
}

int itsnet_get_security_param_request_send(struct message* m)
{

    /*TODO*/
    send_socket(m);
    return 0;
}

int itsnet_configure_security_param_request_send(struct message* m)
{

    /*TODO*/
    send_socket(m);
    return 0;
}

int server(int client_socket)
{
    while(1) {
        int length;
        struct message* msg;
        if(read(client_socket, &length, sizeof(length)) == 0)
            return 0;
        msg = (struct message*)malloc(length);
        memset(msg, 0, sizeof(message));
        read(client_socket, msg, length);
        printf("%d     aid    \n", msg->msg_hdr.aid);
        printf("%d     sid    \n", msg->msg_hdr.sid);
        printf("%d     opcode    \n", msg->msg_hdr.opcode);
        printf("%d     length    \n", length);
        printf("read socket\n");
        socket_recv(msg);
    }
}

/**
 * listener
 * @param void
 */
static void* socket_listen(void* arg)
{
    int s, s2, t, len;
    struct sockaddr_un local, remote;
    int client_sent_quit_message;
    if((s = socket(AF_UNIX, SOCK_STREAM, 0)) == -1) {
        perror("socket");
    }

    local.sun_family = AF_UNIX;
    strcpy(local.sun_path, SOCK_PATH);
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    if(bind(s, (struct sockaddr*)&local, len) == -1) {
        perror("bind");
        exit(1);
    }

    if(listen(s, 5) == -1) {
        perror("listen");
        exit(1);
    }

    while(1) {
        int done, n;
        printf("Waiting for a connection...\n");
        t = sizeof(remote);

        if((s2 = accept(s, (struct sockaddr*)&remote, &t)) == -1) {
            perror("accept");
            printf("error\n");
        }
        printf("Connected.\n");
        server(s2);
        close(s2);
    }
    close(s);
    unlink(SOCK_PATH);
}

int main(void)
{

    char c;
    struct message* m;
    pthread_t application_listener;
    itsnet_geo_area geo_area;
    int latitude, longitude, radius;
    struct itsnet_node_id nodeid;

    /**  Socket listen  */
    pthread_create(&application_listener, NULL, socket_listen, NULL);

    /**  Socket Send    */
    m = (message*)malloc(sizeof(message));
    memset(m, 0, sizeof(message));
    while(1) {

        printf("donner un choix \n");

        /*Transport SAP Test*/
        printf("a: send a Geobroadcast \n");
        printf("b: send a Geounicast \n");
        printf("c: send a Geoanycast \n");

        /*Management SAP Test*/
        printf("d: set node identity \n");
        printf("e: get node identity \n");
        printf("f: get node position \n");
        printf("g: display neighbor table \n");

        printf("q: Exit \n");

        while((c = getchar()) != '\n' && c != EOF) {

            switch(c) {
            case 'a':
                printf("Sending a geobroadcast: \n");
                printf("Latitude: \n");
                scanf("%d", &latitude);
                printf("Longitude: \n");
                scanf("%d", &longitude);
                printf("Radius: \n");
                scanf("%d", &radius);
                m->payload.itsnet_geobroadcast_req.geo_area.latitude = latitude;
                m->payload.itsnet_geobroadcast_req.geo_area.longitude = longitude;
                m->payload.itsnet_geobroadcast_req.geo_area.geo_area_size = radius;
                itsnet_geobroadcast_request_send(m);
                break;
            case 'b':
                printf("envoi d'un geounicast  L'identité du noeud de destination: \n");
                scanf("%2x%2x:%2x%2x:%2x%2x:%2x%2x", (unsigned int*)&nodeid.id[0], (unsigned int*)&nodeid.id[1],
                    (unsigned int*)&nodeid.id[2], (unsigned int*)&nodeid.id[3], (unsigned int*)&nodeid.id[4],
                    (unsigned int*)&nodeid.id[5], (unsigned int*)&nodeid.id[6], (unsigned int*)&nodeid.id[7]);
                printf("Latitude:");
                scanf("%d", &latitude);
                printf("Longitude:");
                scanf("%d", &longitude);
                m->payload.itsnet_unicast_req.destination_latitude = latitude;
                m->payload.itsnet_unicast_req.destination_longitude = longitude;
                m->payload.itsnet_unicast_req.destination_node_id = nodeid;
                itsnet_unicast_request_send(m);
                break;
            case 'c':
                printf("envoi d'un geoanycast \n");
                printf("Latitude:");
                scanf("%d", &latitude);
                printf("Longitude:");
                scanf("%d", &longitude);
                printf("Radius:");
                scanf("%d", &radius);
                m->payload.itsnet_geoanycast_req.geo_area.latitude = latitude;
                m->payload.itsnet_geoanycast_req.geo_area.longitude = longitude;
                m->payload.itsnet_geoanycast_req.geo_area.geo_area_size = radius;
                itsnet_geoanycast_request_send(m);
                break;
            case 'd':
                printf("Configure new node Identity: \n");
                scanf("%2x%2x:%2x%2x:%2x%2x:%2x%2x", (unsigned int*)&nodeid.id[0], (unsigned int*)&nodeid.id[1],
                    (unsigned int*)&nodeid.id[2], (unsigned int*)&nodeid.id[3], (unsigned int*)&nodeid.id[4],
                    (unsigned int*)&nodeid.id[5], (unsigned int*)&nodeid.id[6], (unsigned int*)&nodeid.id[7]);
                m->payload.itsnet_configure_node_id_req.node_id = nodeid;
                itsnet_configure_node_id_request_send(m);
                break;
            case 'e':
                printf("display our node identity: \n");
                itsnet_get_node_id_request_send(m);
                break;
            case 'f':
                printf("display our node position: \n");
                itsnet_get_position_vector_request_send(m);
                break;
            case 'g':
                printf("display neighbor table: \n");
                itsnet_get_neighbor_list_request_send(m);
                break;
            case 'q':
                printf("exit \n");
                exit(0);
            default:
                printf("mauvais choix\n");
                break;
            }
        }
    }
    /*close(sock);*/
    return EXIT_SUCCESS;
}
