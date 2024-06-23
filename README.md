<img src="./assets/icon.svg" width="100" height="100" align="right">

**batman** \
\
This repository holds the code for a simple keylogger for macOS with hooks for function execution on keypresses. Use `batman.conf` to insert run functions to run on keypress.

Ex:
```conf
# this will echo hello when both cmds & g are pressed
rcmd+lcmd+g=/bin/echo,hello
```

## Usage
After cloning the repository, you can build the application with the following command:

```bash
$ make all && ./batman ./keys.log
```

## Notes
- Terminal must be given Accessibility permissions in order to run the program Else it will error with `ERROR: Unable to create event tap.` (Security & Privacy > Accessibility > Terminal).