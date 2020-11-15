#include <stdio.h>
#include <unistd.h>
#include <argp.h>

#include "graphicslibrary.h"
#include "mandelbrot.h"
double mandelbrot_scale = 0.0;
double mandelbrot_imaginary_center = 0.0;
double mandelbrot_real_center = 0.0;


double re= 0.0;
double im= 0.1;
double scale= 1.0;

static int parse_opt (int key, char *arg, struct argp_state *state) {
    switch (key)
    {     case 'r':
        {
            unsigned int i;

            if (arg == NULL)
                re = 0.1;
            else
                re = atof (arg);

            break;
        }
        case 'i':
        {
            unsigned int i;

            if (arg == NULL)
                im = 0.1;
            else
                im = atof (arg);

            break;
        }
        case 's':
        {
            unsigned int i;

            if (arg == NULL)
                scale = 1.0;
            else
                scale = atof (arg);

            break;
        }
    }
    return 0;
}

int main(int argc, char **argv) {
    // parse_input_args
    struct argp_option options[] =     {
            { "real", 'r', "NUM", OPTION_ARG_OPTIONAL,
              "real center"},
            { "imag", 'i', "NUM", OPTION_ARG_OPTIONAL,
                    "imaginary center"},
            { "scale", 's', "NUM", OPTION_ARG_OPTIONAL,
                    "scale"},
              { 0 }
    };
    struct argp argp = { options, parse_opt, 0, 0 };

    argp_parse (&argp, argc, argv, 0, 0, 0);

    double complex center = re+im*1.0i;

    rgb_image_t * image=calculate_mandelbrot(center,scale,512,100);


    if (isatty(fileno(stdout))) {
        write_rgb_file("Mandy.ppm", image);
    }else {
        write_rgb_pipe(image);
    }
    return 0;
}
