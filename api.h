/* xdo
 *
 * Compile:
 *   gcc `pkg-config --cflags --libs x11 xtst` api.c -o api
 *
 *   FWIW, pkg-config above outputs this:
 *   -D_THREAD_SAFE -I/usr/local/include  -L/usr/local/lib -lX11 -lXtst
 *
 * "do" things normally done from the keyboard and mouse
 *
 * Implemented:
 * type something_to_type
 * move xcoord ycoord [screennum]
 * click mouse_button
 * key key_sequence
 * sleep sleep_in_ms
 *
 * Commands a separated by semicolon and/or newlines.
 *
 * Example:
 *
 * Hello world using faked keyboard input
 * echo 'type hello world' | ./xdo
 *
 * Use this while firefox is focused to go to google:
 * echo 'key ctrl+l; sleep 1; type http://www.google.com/; key Return' | ./xdo
 *   - This does:
 *     1) Ctrl+L to focus the URL bar
 *     2) Sleep to let the event propogate
 *     3) Type http://www.google.com/
 *     4) Press return
 */

#include <sys/select.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>
#include <X11/extensions/XTest.h>
#include <math.h>

#define CLICK 1
#define MOUSEMOVE 2
#define MOUSEUP 3
#define MOUSEDOWN 4
#define RAD 3.1415/180

/* Commands */
void cmd_move(int,int,int);
void cmd_click(int);
void cmd_mouseup(int);
void cmd_mousedown(int);
void trace_spiral (void);
void cmd_sleep(char *args);

static Display *xdpy = NULL;
int screen = -1;

int cmd_init() {
  char *display_name = NULL;
  int dummy;
  
  if ( (display_name = getenv("DISPLAY")) == (void *)NULL) {
    fprintf(stderr, "Error: DISPLAY environment variable not set\n");
    exit(1);
  }

  if ((xdpy = XOpenDisplay(display_name)) == NULL) {
    fprintf(stderr, "Error: Can't open display: %s", display_name);
    exit(1);
  }

  if (XTestQueryExtension(xdpy, &dummy, &dummy, &dummy, &dummy) != True) {
    fprintf(stderr, "Error: No XTEST extension available on %s", display_name);
    return 1;
  }
}


/*
int main() {
  init();
  int button, command, mouse_x, mouse_y;
  printf("Command : ");
  scanf("%d", &command);
  if(command == 2)
  {
	trace_spiral();
  }

  switch(command)
  {
   case CLICK : printf("button : ");
		scanf("%d", &button);
		cmd_click(button);
	break;
   case MOUSEMOVE : printf("x y : ");
		scanf("%d %d",&mouse_x, &mouse_y);
		cmd_move(mouse_x,mouse_y,screen);
	break;
   case MOUSEUP : scanf("%d", &button);
	break;
   case MOUSEDOWN : scanf("%d", &button);
	break;
  }
}

*/

void cmd_sleep(char *args) {
  unsigned long sleep_ms;
  struct timeval t;

  sleep_ms = strtoul(args, NULL, 0);
  t.tv_sec = sleep_ms / 1000;
  t.tv_usec = (sleep_ms % 1000) * 1000; /* milliseconds -> microseconds */
  select(0, NULL, NULL, NULL, &t);
}

void cmd_move(int x, int y, int screen)
{
  XTestFakeMotionEvent(xdpy, screen, x, y, CurrentTime);
  XFlush(xdpy);
}

void cmd_click(int button)
{
  XTestFakeButtonEvent(xdpy, button, True, CurrentTime);
  XTestFakeButtonEvent(xdpy, button, False, CurrentTime);
  XFlush(xdpy);
}

void cmd_mousedown(int button)
{
  XTestFakeButtonEvent(xdpy, button, True, CurrentTime);
  XFlush(xdpy);
}

void cmd_mouseup(int button)
{
  XTestFakeButtonEvent(xdpy, button, False, CurrentTime);
  XFlush(xdpy);
}

void trace_spiral (void)
{
	int mouse_x;
	cmd_move(800,450,screen);
	cmd_mousedown(1);
	for(mouse_x=0; mouse_x < 70000 ; mouse_x++)
	{
	cmd_move(800,450,screen);
        cmd_move(mouse_x/100*(float)cos(mouse_x*RAD/5.0)+800.0, mouse_x/100*(float)sin(mouse_x*RAD/5.0)+450.0, screen);
        usleep(3000);
   	}
	cmd_mouseup(1);
}


