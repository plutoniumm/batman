#include "batman.h"
#define BATCH_SZ 200

struct hashmap *hm = NULL;
struct hashmap *stats = NULL;

char buffer[BATCH_SZ];

int buffer_index = 0;
int hashmap_size = 0;
int stats_size = 0;

void flush(int code) {
  if (buffer_index == 0) return;
  fwrite(buffer, 1, buffer_index, logfile);
  fflush(logfile);

  buffer_index = 0;
  memset(buffer, 0, BATCH_SZ);

  if (code != 0) {
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

  printf("Logging to: %s\n", logfileLocation);
  fflush(stdout);
  CFRunLoopRun();

  return 0;
}

int meta_flags = 0;
// _ = proxy, __ = refcon
CGEventRef CGEventCallback(CGEventTapProxy _, CGEventType type, CGEventRef event, void *__) {
  if (type != kCGEventKeyDown && type != kCGEventFlagsChanged) {
    return event;
  };

  CGKeyCode keyCode =
      (CGKeyCode)CGEventGetIntegerValueField(event, kCGKeyboardEventKeycode);

  bool caps = false;
  bool down = false;

  if (type == kCGEventFlagsChanged) {
    switch (keyCode) {
      case 54:  // r-cmd
      case 55:  // l-cmd
        if (keyCode == 54) meta_flags ^= _RCMD;
        if (keyCode == 55) meta_flags ^= _LCMD;
        break;
      case 56:  // l-shift
      case 60:  // r-shift
        if (keyCode == 60) meta_flags ^= _RSFT;
        if (keyCode == 56) meta_flags ^= _LSFT;
        break;
      case 58:  // l-opt
      case 61:  // r-opt
        if (keyCode == 58) meta_flags ^= _LOPT;
        if (keyCode == 61) meta_flags ^= _ROPT;
        break;
      case 59:  // l-ctrl
      case 62:  // r-ctrl
        if (keyCode == 59) meta_flags ^= _LCTL;
        if (keyCode == 62) meta_flags ^= _RCTL;
        break;
      case 57:  // caps
        caps = true;
        break;
    }
  } else if (type == kCGEventKeyDown) {
    down = true;
  }

  char key_str[128];
  key_str[0] = '\0';
  const char *key = convertKeyCode(keyCode, isShift(meta_flags), caps);

  if(!(keyCode >= 54 && keyCode <= 62)){
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

    int match = -1;
    for (int i = 0; i < hashmap_size; i++) {
      if (strcmp(hm[i].key, key_str) == 0) {
        match = i;
        printf("Match: %s->%s\n", key_str, hm[i].value);
        exec(hm[i].value);
        break;
      };
    };

    // this will prevent writing shift, ctrl, cmd, option
    for (int i = 0; i < strlen(key); i++) {
      buffer[buffer_index] = key[i];
      buffer_index++;
    };
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
