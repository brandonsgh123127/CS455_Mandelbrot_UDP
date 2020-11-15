//
// Created by joel on 9/8/20.
//

#include "xwindows_utilities.h"
#include "graphicslibrary.h"
Display *dis;
int screen;
Window win;
GC gc;
const int window_size_x = 512;
const int window_size_y = 512;


void init_x() {
/* get the colors black and white (see section for details) */
//unsigned long black,white;

    unsigned long black,white;

    dis = XOpenDisplay((char *)0);
    screen = DefaultScreen(dis );
    black = BlackPixel(dis ,screen ),
            white = WhitePixel(dis, screen);
    win = XCreateSimpleWindow(dis ,
                              DefaultRootWindow(dis ),0,0,
                              window_size_x, window_size_y,
                              5,black, white);
    // The next two lines notifies us when the user clicks on the x in the corner of the window
    Atom delWindow = XInternAtom(dis,"WM_DELETE_WINDOW",0);
    XSetWMProtocols(dis,win,&delWindow,1);

    XSetStandardProperties(dis,win,"Mandelbrot Set","Mandelbrot",None,NULL,0,NULL);
    XSelectInput(dis, win, ExposureMask|ButtonPressMask|KeyPressMask);

    gc=XCreateGC(dis, win, 0,0);
    XSetBackground(dis,gc,white);
    XSetForeground(dis,gc,black);
    XClearWindow(dis, win);
    XMapRaised(dis, win);
};

void display_image(rgb_image_t *rgbimage){


    char *data = malloc(window_size_x * window_size_y * 4);
    XImage *image = NULL;
    memset(data, 50, window_size_x * window_size_y * 4);

    double dx = (1.0*rgbimage->image_size_x)/(1.0*window_size_x);
    double dy = (1.0*rgbimage->image_size_y)/(1.0*window_size_y);
    double xx =0.0;
    double yy = 0.0;
    int loc = 0;
    for (int r=0;r<window_size_y;r++) {
        yy=0.0;
        for (int c = 0; c < window_size_x; c++) {

            int loc2 = (rgbimage->image_size_y*floor(xx)+floor(yy))*3;
            data[loc] = rgbimage->image_data[loc2];
            data[loc + 1] = rgbimage->image_data[loc2+1];
            data[loc + 2] = rgbimage->image_data[loc2+2];
            loc = loc+4;
            yy+=dy;
        }
        xx += dx;
    }

    image = XCreateImage(
            dis,
            CopyFromParent,
            DefaultDepth(
                    dis,
                    DefaultScreen(dis)),
            ZPixmap,
            0,
            data,
            window_size_x,
            window_size_y,
            32,
            window_size_x * 4);

    XPutImage(
            dis,
            win,
            gc,
            image,
            0, 0,  // from which offset to start drawing
            0, 0,  // position on the screen
            window_size_x, window_size_y); // width and height of what you want to draw
}

int process_event(x_event_t *event_data){
    XEvent event;		/* the XEvent declaration !!! */
    KeySym key;		/* a dealie-bob to handle KeyPress Events */
    char text[255];		/* a char buffer for KeyPress Events */
    event_data->mouse_x=127;
    event_data->mouse_y=127;
    event_data->button=0;
    XNextEvent(dis, &event);

    if (event.type==Expose && event.xexpose.count==0) {
        /* the window was exposed redraw it! */
        redraw();
    }
    if (event.type==KeyPress&&
        XLookupString(&event.xkey,text,255,&key,0)==1) {
        /* use the XLookupString routine to convert the invent
           KeyPress data into regular text.  Weird but necessary...
        */
        printf("You pressed the %c key!\n",text[0]);
        if (text[0]=='q') {
            return 1;
        }

    }
    if(event.type ==ClientMessage)
        return 1;
    if (event.type==ButtonPress) {
        /* tell where the mouse Button was Pressed */
        event_data->mouse_x=event.xbutton.x;
        event_data->mouse_y=event.xbutton.y;
        event_data->button=event.xbutton.button;
    }
    return 0;
}

void close_x() {
    XFreeGC(dis, gc);
    XDestroyWindow(dis,win);
    XCloseDisplay(dis);

};
void redraw() {
    XClearWindow(dis, win);
};