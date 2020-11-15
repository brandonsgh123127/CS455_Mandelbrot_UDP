#include <stdio.h>
#include <unistd.h>
#include "graphicslibrary.h"
#include "xwindows_utilities.h"

 extern double mandelbrot_real_center = -0.5;
 extern double mandelbrot_imaginary_center=0.0;
 extern double mandelbrot_scale = 1.0;
 const double mandelbrot_radius = 1.5;

struct rgb_image img;
int main () {
    rgb_image_t * image = &img;
    x_event_t mouse_event;
    // Initialize xWindows
    image = read_ppm_rgb_mandy();
    init_x();
    int done = 0;
    /* look for events while not done */
    while(!done) {
        // call xwindows function for click checking...
        done = process_event(&mouse_event);
        // map_window_to_complex calculate a new center
        mandelbrot_scale += (mouse_event.button==mouse_scroll_forward)?1.0:0.0;
        mandelbrot_scale -= (mouse_event.button==mouse_scroll_backward)?1.0:0.0;
        mandelbrot_scale = (mandelbrot_scale<1.0)?1.0:mandelbrot_scale;
        // Calculate fraction/offset vals.
        double real_fraction = mouse_event.mouse_x/512.0-0.5;
        double imaginary_fraction = mouse_event.mouse_y/512.0-0.5;
        double real_offset = real_fraction * mandelbrot_radius/mandelbrot_scale;
        double imaginary_offset = imaginary_fraction * mandelbrot_radius/mandelbrot_scale;

        //Append offsets to extern variables
        mandelbrot_real_center += real_offset;
        mandelbrot_imaginary_center +=imaginary_offset;

        // Reset to original scale by right click
        mandelbrot_scale=(mouse_event.button==mouse_right_button)?2.0:mandelbrot_scale;
        mandelbrot_real_center=(mouse_event.button==mouse_right_button)?-0.50:mandelbrot_real_center;
        mandelbrot_real_center= mandelbrot_real_center < -2? -2:mandelbrot_real_center;
        mandelbrot_real_center= mandelbrot_real_center > 1? 1:mandelbrot_real_center;
        mandelbrot_imaginary_center=(mouse_event.button==mouse_right_button)?-1.00:mandelbrot_imaginary_center;
        mandelbrot_imaginary_center= mandelbrot_imaginary_center < -1.5? -1.5:mandelbrot_imaginary_center;
        mandelbrot_imaginary_center= mandelbrot_imaginary_center > 1.5? 1.5:mandelbrot_imaginary_center;

        printf("Real: %f, Imaginary: %f, Scale: %f\n",mandelbrot_real_center,mandelbrot_imaginary_center,mandelbrot_scale);
        // here we will popen mandelbrot to get a new image
        /*REFORMATTING FOR USE WITH CLIENT AND HOST SERVER*/
        // image = read_ppm_rgb_mandy();

        (mouse_event.button==mouse_left_button)?image=gen_ppm_rgb_client():(mouse_event.button==mouse_scroll_forward)?image=gen_ppm_rgb_client():(mouse_event.button==mouse_scroll_backward)?image=gen_ppm_rgb_client():perror("Unable to open client!");

        display_image(image);
        //write_rgb_file("Mandy.ppm", image);
	}
    free_rgb_image(image);
    close_x();
}





