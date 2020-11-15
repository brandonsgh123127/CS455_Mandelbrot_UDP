//
// Created by joel on 10/27/20.
//

#ifndef UDPSERVER_UDP_ROUTINES_H
#define UDPSERVER_UDP_ROUTINES_H


#define MAXLINE  1024
struct rqst_udp_pkt{
    int number;
    struct sockaddr_un * uxds_cliaddr;
    struct sockaddr_un * uxds_svraddr;
    struct sockaddr_in * inet_cliaddr;
    struct sockaddr_in * inet_svraddr;
    int uxds_len;
    int inet_len;
    char *rqst_data;
};


void *send_response(void  * rqst);
void await_request(struct rqst_udp_pkt * rqst);
struct rqst_udp_pkt * make_rqst();
int open_inet_udp_socket(unsigned short port);
int open_uxds_udp_socket();
#endif //UDPSERVER_UDP_ROUTINES_H
