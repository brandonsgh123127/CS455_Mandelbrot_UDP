//
// Created by joel on 9/8/20.
//
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdlib.h>
#include <math.h>
#include "graphicslibrary.h"
#include<X11/Xlib.h>

#ifndef SIMPLEGRAPHICS_XWINDOWS_UTILITIES_H
#define SIMPLEGRAPHICS_XWINDOWS_UTILITIES_H

//old school defines
#define mouse_left_button 1
#define mouse_middle_button 2
#define mouse_right_button 3
#define mouse_scroll_forward 4
#define mouse_scroll_backward 5


typedef struct x_event {
    unsigned int mouse_x;
    unsigned int mouse_y;
    unsigned int button;
} x_event_t;



void init_x();
void close_x();
void redraw();
void display_image(rgb_image_t *rgbimage);
int process_event(x_event_t *event_data);
#endif //SIMPLEGRAPHICS_XWINDOWS_UTILITIES_H
