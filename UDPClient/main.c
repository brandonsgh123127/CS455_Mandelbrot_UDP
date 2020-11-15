// Client side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <complex.h>
#include <argp.h>
#include "graphicslibrary.h"

#define PORT	 8080
#define MAXLINE 1024

struct rqst_udp_pkt{
    int number;
    struct sockaddr_un * uxds_cliaddr;
    struct sockaddr_un * uxds_svraddr;
    struct sockaddr_in * inet_cliaddr;
    struct sockaddr_in * inet_svraddr;
    int len;
    char *rqst_data;
};

struct rqst_udp_pkt * make_rqst()
{
    struct rqst_udp_pkt *rqst = (struct rqst_udp_pkt *) malloc(sizeof(struct rqst_udp_pkt));
    rqst -> inet_cliaddr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    rqst -> uxds_cliaddr = (struct sockaddr_un *) malloc(sizeof(struct sockaddr_un));
    rqst -> inet_svraddr = (struct sockaddr_in *) malloc(sizeof(struct sockaddr_in));
    rqst -> uxds_svraddr = (struct sockaddr_un *) malloc(sizeof(struct sockaddr_un));
    rqst -> rqst_data = (char *) malloc(MAXLINE);
    return rqst;
}

int open_inet_server_socket(int port, struct rqst_udp_pkt * rqst_pkt)
{
    int sockfd;
    struct sockaddr_in	 servaddr;
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(rqst_pkt->inet_svraddr, 0, sizeof(struct sockaddr_in));

    // Filling server information
    rqst_pkt->inet_svraddr->sin_family = AF_INET;
    rqst_pkt->inet_svraddr->sin_port = htons(port);
    rqst_pkt->inet_svraddr->sin_addr.s_addr = htonl(INADDR_ANY);
    if (connect(sockfd, (struct sockaddr*) (* rqst_pkt).inet_svraddr, sizeof(struct sockaddr_in)) < 0){
        perror("Cannot connect to server!");
        exit(-1);
    }

    return sockfd;

}

int open_uxds_server_socket(char * uxds_udp_server_path, struct rqst_udp_pkt * rqst_pkt)
{
    int socket_fd;


    if((socket_fd = socket(AF_UNIX, SOCK_DGRAM, 0)) < 0)
    {

        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset((* rqst_pkt).uxds_cliaddr, 0, sizeof(struct sockaddr_un));
    (* (* rqst_pkt).uxds_cliaddr).sun_family = AF_UNIX;
    strcpy((* (* rqst_pkt).uxds_cliaddr).sun_path, "/tmp/UDSDGCLNT");  // This might be best to pass in

    printf("%s\n",(* (* rqst_pkt).uxds_cliaddr).sun_path);
    unlink((* (* rqst_pkt).uxds_cliaddr).sun_path);
    if(bind(socket_fd, (const struct sockaddr *) (* rqst_pkt).uxds_cliaddr,
            sizeof(struct sockaddr_un)) < 0)
    {
        perror("client: bind");
        return 1;
    }

    memset(rqst_pkt->uxds_svraddr, 0, sizeof(struct sockaddr_un));
    rqst_pkt->uxds_svraddr->sun_family = AF_UNIX;
    strcpy(rqst_pkt->uxds_svraddr->sun_path, uxds_udp_server_path);



    return socket_fd;
}

void send_requests(int sockfd,const struct sockaddr * svraddr,
                   int image_number,
                   complex double center,double scale,int real_segs,int imaginary_segs,int n,unsigned char * buffer[])
{
    const double extent = 3.0;
    int n_real = n/real_segs;           // n needs to be multiple of real_segs
    int n_imaginary = n/imaginary_segs; // n needs to be multiple of imaginary_segs
    const double delta_real = extent/(1.0*real_segs*scale);
    const double delta_imaginary = extent/(1.0*imaginary_segs*scale);
    const double start_real = creal(center)-extent/(2.*scale);
    const double start_imaginary = cimag(center)-extent/(2.*scale);

    // we  need to send start_real,start_imaginary,end_real,end_imaginary,n_real,n_imaginary)
    int r_counter,r_start;
    int i_counter,i_start;

    double real_offset,real_end,imaginary_offset,imaginary_end;
    for (r_counter=0,r_start=0;r_counter<real_segs; r_counter++,r_start+=n_real) {
        for (i_counter = 0, i_start = 0; i_counter < imaginary_segs; i_counter++, i_start += n_imaginary) {
            // make a packet
            real_offset = start_real + r_counter * delta_real;
            real_end = real_offset + delta_real;

            imaginary_offset = start_imaginary + i_counter * delta_imaginary;
            imaginary_end = imaginary_offset + delta_imaginary;
            printf("\n %lf,%lf,%lf,%d,%lf,%lf,%d",
                   real_offset, real_end, delta_real,n_real,
                   imaginary_offset, imaginary_end,n_imaginary);
            sprintf(buffer, "%d %d %d %lf,%lf,%d,%lf,%lf,%d", image_number, r_start, i_start, real_offset, real_end,
                    n_real, imaginary_offset, imaginary_end, n_imaginary);
            int len = sizeof(struct sockaddr_un);
            sendto(sockfd, (const char *) buffer, strlen(buffer),
                   MSG_CONFIRM, (const struct sockaddr *) svraddr,
                   len);


        }
    }
}
rgb_image_t * await_responses(int sockfd,struct rqst_udp_pkt * rqst_pkt,int expected_number,int image_size) {
    // expected image number
    // dump all packets which are not expected
    // we know how many packets for a completed image wait for those
    // if time_out request resend... this should not happen on a single machine

    int image_number, n;
    int len = sizeof(struct sockaddr_un);
    unsigned char buffer[MAXLINE*16];
    rgb_image_t *image;
    image = malloc(sizeof(rgb_image_t));
    image->image_size_x=image_size;
    image->image_size_y=image_size;
    image->image_data = malloc(image->image_size_x * image->image_size_y * 3);
    while (expected_number > 0) {
        n = recvfrom(sockfd, (char *) buffer, MAXLINE*16,
                     MSG_WAITALL, (struct sockaddr *) rqst_pkt->uxds_svraddr,
                     &len);
        printf("\n Data-->");
        printf("%d,%d,%d,%d %d %d:\n",
               ((int *) buffer)[0], ((int *) buffer)[1],
               ((int *) buffer)[2], ((int *) buffer)[3],
               ((int *) buffer)[4], ((int *) buffer)[5]
        );
        int image_number, r_start, i_start, n_real, n_imaginary;
        image_number = ((int *) buffer)[0];
        r_start = ((int *) buffer)[1];
        i_start = ((int *) buffer)[2];
        n_real = ((int *) buffer)[3];
        n_imaginary = ((int *) buffer)[4];
        int pkt_counter = 6 * sizeof(int); // first byte after header
        for (int r = 0; r < n_real; r++) {
            //printf("R: %d\nn_real: %d\n",r,n_real);
            for (int i = 0; i < n_imaginary; i++) {
               // printf("I: %d\nn_imag: %d\n",i,n_imaginary);
                int pixel_location = (i_start + r) * image_size * 3 + (r_start + i) * 3;
                printf("%d,",pixel_location);
                image->image_data[pixel_location + 0] = buffer[pkt_counter++];
                image->image_data[pixel_location + 1] = buffer[pkt_counter++];
                image->image_data[pixel_location + 2] = buffer[pkt_counter++];
            }
            printf("\nexpected:%d\n",expected_number);
        }
    expected_number--;
    }
    return image;
    // this could return the populated rgb image..
}

/*
 *
 *
 * ARGP AND MAIN BELOW THIS SECTION...
 *
 *
 *
 */



/* Used by main to communicate with parse_opt. */
static struct arguments
{
    char *arg1;                   /* arg1 */
    char **strings;               /* [string…] */
    double center;
    double scale,real,imaginary;      /* vals of args*/
};

static error_t
parse_opt (int key, char *arg, struct argp_state *state)
{

    /* Get the input argument from argp_parse, which we
       know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;
    switch (key)
    {
        // Memory allocated to copy over arg without the '=' sign.
        char centDest[5],scaleDest[5],realDest[5],imagDest[5];
        case 'c':
            memcpy( centDest, &arg[1], 5 );
            arguments->center = atof(centDest);
            break;
        case 's':
            memcpy( scaleDest, &arg[1], 5 );
            arguments->scale = atof(scaleDest);
            break;
        case 'r':
            memcpy( realDest, &arg[1], 5 );
            arguments->center = atof(realDest);
            break;
        case 'i':
            memcpy( imagDest, &arg[1], 5 );
            arguments->scale = atof(imagDest);
            break;
        case ARGP_KEY_NO_ARGS:
            break;
        case ARGP_KEY_ARG:
            arguments->arg1 = arg;
            arguments->strings = &state->argv[state->next];
            state->next = state->argc;
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

double mandelbrot_scale = 1.0;
double mandelbrot_imaginary_center = 0.0;
double mandelbrot_real_center = 0.0;
struct argp_option options[] =     {
        { "scale", 's', "NUM", OPTION_ARG_OPTIONAL,
          "scale"},
        { "center", 'c', "NUM", OPTION_ARG_OPTIONAL,
          "center"},
        { 0 }
};
struct argp argp = { options, parse_opt, 0, 0 };

// Driver code
int main(int argc, char **argv) {
    int sockfd;
    unsigned char buffer[MAXLINE*24];
    struct sockaddr_in	 servaddr;
    struct arguments arguments;

    arguments.scale=1;
    arguments.center=0.2;
    argp_parse(&argp, argc, argv, 0, 0, &arguments);
    //printf("\n%f\n",arguments.center);

    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Filling server information
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;


    struct rqst_udp_pkt * rqst_pkt= make_rqst();

    sockfd=open_uxds_server_socket("/tmp/UDSDGSRV",rqst_pkt);
    //TODO - open server socket given request packet, send request  -CHECK
    sockfd=open_inet_server_socket(PORT,rqst_pkt);
    // TODO - IMPLEMENT SEND REQUEST

    int n;
    socklen_t len;

    printf("%s\n",rqst_pkt->uxds_svraddr->sun_path);


    complex double center = -0.5+0*I;
    int image_size = 512;
    //TODO- WHENEVER SWITCHING UDP TYPE, CHANGE FROM INET to UXDS
    send_requests(sockfd,(struct sockaddr *) rqst_pkt->inet_svraddr,2,arguments.center,arguments.scale,16,16,image_size,buffer);
    rgb_image_t  *image = await_responses(sockfd, rqst_pkt,256,image_size);
    close(sockfd);
    if (isatty(fileno(stdout))) {
        write_rgb_file("test.ppm",image);
    }else {
        write_rgb_pipe(image);
    }
    free_rgb_image(image);
    return 0;
}
