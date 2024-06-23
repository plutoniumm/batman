#include "batman.h"
#define BATCH_SZ 200

CGEventFlags lastFlags = 0;

struct hashmap *hm = NULL;
struct hashmap *stats = NULL;

char buffer[BATCH_SZ];

int buffer_index = 0;
int hashmap_size = 0;
int stats_size = 0;

void flush(int code) {
  if (buffer_index == 0) return;
  // write to file
  fwrite(buffer, 1, buffer_index, logfile);
  fflush(logfile);

  buffer_index = 0;
  memset(buffer, 0, BATCH_SZ);

  if (code != 0) {
    fprintf(logfile, "Keylogging has ended.\n");
    fclose(logfile);
    exit(0);
  };
};

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

int count = 0;
CGEventRef CGEventCallback(
  CGEventTapProxy proxy,
  CGEventType type,
  CGEventRef event,
  void *refcon
) {
  if (type != kCGEventKeyDown && type != kCGEventFlagsChanged) {
    return event;
  };
  printf("\n\nEvent type: %d\n", type);

  CGEventFlags flags = CGEventGetFlags(event);
  CGKeyCode keyCode =
      (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  bool caps = false;
  bool down = false;
  bool meta = keyCode >= 54 && keyCode <= 62;;
  int pressed = 0;
  int meta_flags = 0;

  if(keyCode == 7){
    exit(0);
  };

  if (type == kCGEventFlagsChanged) {
    switch (keyCode) {
      case 54:  // [right-cmd]
      case 55:  // [left-cmd]
        down = (flags & kCGEventFlagMaskCommand) &&
               !(lastFlags & kCGEventFlagMaskCommand);
        if (keyCode == 54) meta_flags ^= _RCMD;
        if (keyCode == 55) meta_flags ^= _LCMD;
        break;
      case 56:  // [left-shift]
      case 60:  // [right-shift]
        down = (flags & kCGEventFlagMaskShift) &&
               !(lastFlags & kCGEventFlagMaskShift);
        if (keyCode == 60) meta_flags ^= _RSFT;
        if (keyCode == 56) meta_flags ^= _LSFT;
        break;
      case 58:  // [left-option]
      case 61:  // [right-option]
        down = (flags & kCGEventFlagMaskAlternate) &&
               !(lastFlags & kCGEventFlagMaskAlternate);
        if (keyCode == 58) meta_flags ^= _LOPT;
        if (keyCode == 61) meta_flags ^= _ROPT;
        break;
      case 59:  // [left-ctrl]
      case 62:  // [right-ctrl]
        down = (flags & kCGEventFlagMaskControl) &&
               !(lastFlags & kCGEventFlagMaskControl);
        if (keyCode == 59) meta_flags ^= _LCTL;
        if (keyCode == 62) meta_flags ^= _RCTL;
        break;
      case 57:  // [caps]
        down = (flags & kCGEventFlagMaskAlphaShift) &&
               !(lastFlags & kCGEventFlagMaskAlphaShift);
      default:
        break;
    }
    printf("Metaflags: %d\n", meta_flags);
  } else if (type == kCGEventKeyDown) {
    down = true;
  }

  count++;
  printf("Count: %d, Metaflags: %d\n", count, meta_flags);
  printf("Keycode: %d, Flags: %llu, LastFlags: %llu\n", keyCode, flags, lastFlags);
  lastFlags = flags;

  char key_str[128];
  key_str[0] = '\0';
  const char *key = convertKeyCode(keyCode, isShift(flags), caps);

  if(!meta){
    if (isSet(_RCMD, meta_flags)) strcat(key_str, "rcmd+");
    if (isSet(_LCMD, meta_flags)) strcat(key_str, "lcmd+");
    if (isSet(_RSFT, meta_flags)) strcat(key_str, "rshift+");
    if (isSet(_LSFT, meta_flags)) strcat(key_str, "lshift+");
    if (isSet(_ROPT, meta_flags)) strcat(key_str, "ropt+");
    if (isSet(_LOPT, meta_flags)) strcat(key_str, "lopt+");
    if (isSet(_RCTL, meta_flags)) strcat(key_str, "rctrl+");
    if (isSet(_LCTL, meta_flags)) strcat(key_str, "lctrl+");
    if (caps) strcat(key_str, "caps+");

    strcat(key_str, key);
    printf("Key combination: %s\n", key_str);
  };
  /*
    - This line exists because special chars are triggered twice
    - this records it only on keydown and not keyup
    - any combinations/hooks will have to be registered BEFORE this line
  */
  if (!down) return event;
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
    flush(0);
    update_hashmap(&hm, &hashmap_size);
  };
  return event;
}
