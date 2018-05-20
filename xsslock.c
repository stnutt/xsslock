#include <X11/extensions/scrnsaver.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static int error_handler(Display *display, XErrorEvent *error)
{
    return 0;
}

// TODO add ability to enable/set screensaver timeout
int main(int argc, char *argv[])
{
    Display *display;
    display = XOpenDisplay(NULL);
    if(!display)
    {
        return EXIT_FAILURE;
    }

    XSetErrorHandler(error_handler);

    int event_base;
    int error_base;

    if(!XScreenSaverQueryExtension(display, &event_base, &error_base))
    {
        XCloseDisplay(display);
        return EXIT_FAILURE;
    }

    if(argc == 1)
    {
        XActivateScreenSaver(display);
    }
    else
    {
        XScreenSaverSelectInput(display, RootWindow(display, 0), ScreenSaverNotifyMask);
        while(1)
        {
            XEvent event;
            XNextEvent(display, &event);
            if(event.type == event_base)
            {
                XScreenSaverNotifyEvent *notify_event;
                notify_event = (XScreenSaverNotifyEvent *) &event;
                if(notify_event->state == ScreenSaverOn)
                {
                    if(fork() == 0)
                    {
                        execvp(argv[1], &argv[1]);
                        _exit(EXIT_FAILURE);
                    }
                    wait(NULL);
                    while(XCheckTypedEvent(display, event_base, &event))
                    {
                    }
                }
            }
        }
    }

    XCloseDisplay(display);
    return EXIT_SUCCESS;
}
