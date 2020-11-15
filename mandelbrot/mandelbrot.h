//
// Created by joel on 9/10/20.
//
#include <complex.h>
#ifndef MANDELBROT_MANDELBROT_H
#define MANDELBROT_MANDELBROT_H
rgb_image_t * calculate_mandelbrot2(double complex min,
                                    double complex max,
                                    unsigned int steps_real,
                                    unsigned int steps_imaginary,
                                    unsigned int max_iteration);
rgb_image_t * calculate_mandelbrot(double complex center,double scale,unsigned int n,unsigned int max_iteration);
unsigned int iterations(double complex p0,unsigned int max_iteration);

#endif //MANDELBROT_MANDELBROT_H
