<img src="./assets/icon.svg" width="100" height="100" align="right">

### batman
This repository holds the code for a simple keylogger for macOS with hooks for function execution on keypresses. We'll soon use `batman.conf` to insert run functions to run on keypress.

## Usage
After cloning the repository, you can build the application with the following command:

```bash
$ make && make install
# installs to /usr/local/bin/batman
```

```bash
# keys.log is the file to log to
$ batman ./keys.log
```

### Optional Parameters
You can pass in two optional parameters to the program. The `clear` option will clear the logs at the default location. Any other argument passed in will be used as the path to the log file for that process. See below:

```bash
$ batman clear
Logfile cleared.
```

## Notes
- Terminal must be given Accessibility permissions in order to run the program Else it will error with `ERROR: Unable to create event tap.` (Security & Privacy > Accessibility > Terminal).