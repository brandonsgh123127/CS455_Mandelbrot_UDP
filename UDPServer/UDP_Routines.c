//
// Created by joel on 10/27/20.
//
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <pthread.h>
#include <unistd.h>
#include <sys/un.h>

#include "UDP_Routines.h"
#include "graphicslibrary.h"
#include "mandelbrot.h"

extern int gsockfd;

void *send_response(void  *rqst){
    char *hello = (char *)malloc(40);

    // ToDo
    // The request will have the region of interest
    // calc mandy of the region get the RGB and send it back.

  //  send the rgb_image_t data in the packet

    int image_number,n_real,n_imaginary,r_start,i_start;
    double real_offset,real_end,imaginary_offset,imaginary_end;
    sscanf(((struct rqst_udp_pkt *)rqst)->rqst_data,"%d %d %d %lf,%lf,%d,%lf,%lf,%d",
            &image_number,&r_start,&i_start,&real_offset,&real_end,&n_real,&imaginary_offset,&imaginary_end,&n_imaginary);
    printf("\n Block--> %lf,%lf,%d,%lf,%lf,%d",real_offset,real_end,n_real,imaginary_offset,imaginary_end,n_imaginary);


    // need to calculate the subregion and send back the image data
    double complex min = real_offset + imaginary_offset * 1i;
    double complex max = real_end + imaginary_end * 1i;

    //CALCULATE PORTION OF IMAGE
    rgb_image_t *image= calculate_mandelbrot2(min,max,n_real,n_imaginary,512);


    // Note: In our multi-threaded server we need unique buffers for each thread so need to malloc

    const int header_size = 6;
    char *pkt_data = malloc(n_real*n_imaginary*3+header_size*sizeof(int));
    ((int *)pkt_data)[0]= image_number;
    ((int *)pkt_data)[1]= r_start;
    ((int *)pkt_data)[2]= i_start;
    ((int *)pkt_data)[3]= n_real;
    ((int *)pkt_data)[4]= n_imaginary;
    ((int *)pkt_data)[5]= 0;
    memcpy(pkt_data+header_size*sizeof(int),image->image_data,n_real*n_imaginary*3);


    sendto(gsockfd, pkt_data,n_real*n_imaginary*3+header_size*sizeof(int) ,
           MSG_CONFIRM, (const struct sockaddr *) ((struct rqst_udp_pkt *)rqst)->uxds_cliaddr,
           ((struct rqst_udp_pkt *)rqst)->uxds_len);

    free(pkt_data);
    free_rgb_image(image);

}

void await_request(struct rqst_udp_pkt * rqst)
{

    int n;
    n = recvfrom(gsockfd, (char *)rqst->rqst_data, MAXLINE,
                 MSG_WAITALL, ( struct sockaddr *) rqst->uxds_cliaddr,
                 &(rqst->uxds_len));
    rqst->rqst_data[n] = '\0';

}

struct rqst_udp_pkt * make_rqst()
{
    struct rqst_udp_pkt *rqst = (struct rqst_udp_pkt *) malloc(sizeof(struct rqst_udp_pkt));
    rqst -> inet_cliaddr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    rqst -> uxds_cliaddr = (struct sockaddr_un *) malloc(sizeof(struct sockaddr_un));
    rqst -> inet_svraddr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    rqst -> uxds_svraddr = (struct sockaddr_un *) malloc(sizeof(struct sockaddr_un));
    rqst->uxds_len = sizeof(struct sockaddr_un);
    rqst->inet_len = sizeof(struct sockaddr_un);
    rqst -> rqst_data = (char *) malloc(MAXLINE);
    return rqst;
}

int open_inet_udp_socket(unsigned short port)
{
    struct sockaddr_in servaddr, *cliaddr;
    static int count=0;
    int sockfd;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));

    // Filling server information
    servaddr.sin_family = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address
    if (bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }


    return sockfd;
}

int open_uxds_udp_socket()
{
    struct sockaddr_un servaddr;
    struct sockaddr_un cliaddr;
    static int count=0;
    int sockfd;
    // Creating socket file descriptor
    if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));
    memset(&cliaddr, 0, sizeof(cliaddr));
    servaddr.sun_family = AF_UNIX;
    // Copy 'tmp/UDSDGSRV' to sun path
    strcpy(servaddr.sun_path, "/tmp/UDSDGSRV");
    // Filling server information


    // Bind the socket with the server address
    unlink("/tmp/UDSDGSRV");
    //binds address to socket descriptor
    if (bind(sockfd, (const struct sockaddr *) &servaddr,
             sizeof(servaddr)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    return sockfd;
}