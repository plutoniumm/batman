#define _RCMD 0x10000000
#define _RSFT 0x01000000
#define _ROPT 0x00100000
#define _RCTL 0x00010000
#define _LCMD 0x00001000
#define _LSFT 0x00000100
#define _LOPT 0x00000010
#define _LCTL 0x00000001

bool isSet(int flag, int flags) {
  return (flags & flag) == flag;
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
    case 51:  return "🔙";
    case 53:  return "[esc]";
    case 54:  return "[r⌘]";
    case 55:  return "[l⌘]";
    case 58:  return "[l⌥]";
    case 59:  return "[l⎈]";
    case 61:  return "[r⌥]";
    case 62:  return "[r⎈]";
    case 63:  return "[fn]";
    case 123: return "→";
    case 124: return "←";
    case 125: return "↓";
    case 126: return "↑";
  }
  return "[?]";
}