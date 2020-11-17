
//https://www.geeksforgeeks.org/udp-server-client-implementation-c/
// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>

#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <netinet/in.h>

#include <pthread.h>
#include "UDP_Routines.h"

#define PORT     8080

int gsockfd;
double mandelbrot_scale = 1.0;
double mandelbrot_imaginary_center = 0.0;
double mandelbrot_real_center = 0.0;
void completeFlag(int *arg[]){
    for(int i = 0; i < 3; i++){
        if(arg[i]!=1)
            arg[i]=0;
    }
}
// Driver code
int main() {

    static int count=0;

    pthread_t tid1,tid2,tid3;
    char threadIDs[3];
    threadIDs[0]=&tid1;
    threadIDs[1]=&tid2;
    threadIDs[2]=&tid3;
    int threadComplete[3];
    threadComplete[0]=0;
    threadComplete[1]=0;
    threadComplete[2]=0;
    gsockfd=open_inet_udp_socket(PORT);
    //open_uxds_udp_socket();
    while (1) {
        //  await request
        struct rqst_udp_pkt *rqst = make_rqst();
        rqst->number = count;
        await_request(rqst);
        for(int i = 0; i < 3; i++) {
            if(threadComplete[i]!=1) {
                threadComplete[i] = 1;
                pthread_create(&threadIDs[1], NULL, send_response, (void *) rqst);
                pthread_cleanup_push(completeFlag, threadComplete) ;
                count++;
                pthread_cleanup_pop(1);
                break;
            }
            else
                continue;
        }
    }
    return 0;
}