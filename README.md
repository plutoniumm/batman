<img src="./assets/icon.svg" width="100" height="100" align="right">

### batman
This repository holds the code for a simple keylogger for macOS with hooks for function execution on keypresses. We'll soon have a `.brc` config file to insert custom functions to run on keypresses.

## Usage
After cloning the repository, you can build the application with the following command:

```bash
$ make && make install
# installs to /usr/local/bin/batman
```

Logs to `/var/log/keystroke.log` (change this in `keylogger.h`).

```bash
$ batman
Logging to: /var/log/keystroke.log
```

If only modifier keys are logging (e.g. in macOS ≥ 10.10), run with root access.

If you'd like the application to run in the background on startup, run the `startup` make target:

```bash
$ sudo make startup
```

To run the application now (note: you will need to run the `sudo make startup` command first):

```bash
$ sudo make load
```

To quit the application now (note: you will need to run the `sudo make startup` command first)::

```bash
$ sudo make unload
```

## Uninstallation

You can completely remove the application from your system (including the startup daemon) by running the following command (logs will not be deleted):

```bash
$ sudo make uninstall
```

### Optional Parameters
You can pass in two optional parameters to the program. The `clear` option will clear the logs at the default location. Any other argument passed in will be used as the path to the log file for that process. See below:

```bash
$ batman clear
Logfile cleared.

$ batman ~/logfile.txt
Logging to: /Users/Casey/logfile.txt
```

## Notes
- Terminal must be given Accessibility permissions in order to run the program Else it will error with `ERROR: Unable to create event tap.` (Security & Privacy > Accessibility > Terminal).